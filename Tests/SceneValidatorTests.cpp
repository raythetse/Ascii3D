// Tests for SceneValidator against Tests/fixtures/*.scn and the real scenes in CustomScenes/.
// Run from Test Explorer in Visual Studio, or with vstest.console.exe on Ascii3DTests.dll.
#include "CppUnitTest.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
#include "TestPaths.h"
#include "../SceneValidator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace fs = std::filesystem;
using SceneValidator::Severity;

namespace
{
	constexpr Severity E{ Severity::Error };
	constexpr Severity W{ Severity::Warning };

	// (line, severity, rule); message wording is not checked
	using Summary = std::tuple<size_t, Severity, int>;

	// Each fixture must produce exactly these findings
	const std::map<std::string, std::vector<Summary>> EXPECTED{
		{ "valid.scn", {} },
		{ "rule01_line1_wrong.scn", { { 1, E, 1 } } },
		{ "rule01_line1_params.scn", { { 1, W, 1 } } },
		{ "rule02_lexical.scn", {
			{ 2, E, 2 },				// trailing space
			{ 3, E, 2 },				// blank line
			{ 4, E, 2 },				// tab between tokens
			{ 7, E, 2 },				// comma
			{ 7, E, 5 }, { 7, E, 6 },	// ...which also breaks the color value
		} },
		{ "rule03_keywords.scn", { { 2, E, 3 }, { 6, E, 3 } } },
		{ "rule04_end.scn", { { 6, E, 4 } } },
		{ "rule05_params.scn", {
			{ 5, E, 5 }, { 5, W, 9 },	// misspelled "position", so position is also missing
			{ 6, E, 5 },				// luminance repeated
			{ 7, E, 5 },				// position has 2 values
		} },
		{ "rule06_values.scn", { { 2, E, 6 }, { 3, E, 6 }, { 8, E, 6 } } },
		{ "rule07_ranges.scn", { { 2, E, 7 }, { 3, E, 7 }, { 3, E, 7 }, { 6, E, 7 } } },
		{ "rule08_comb_params.scn", { { 5, W, 8 }, { 8, W, 8 } } },
		{ "rule09_generator.scn", { { 2, W, 9 }, { 3, W, 9 } } },
		{ "rule10_combination.scn", { { 2, E, 10 }, { 4, E, 10 } } },
		{ "rule11_depth.scn", { { 5, W, 11 }, { 11, W, 11 } } },
		{ "rule12_needs_shape.scn", { { 2, E, 12 }, { 7, E, 12 } } },
		{ "rule13_light_order.scn", { { 6, E, 13 } } },
		{ "rule13_no_light.scn", { { 4, E, 13 } } },
		{ "rule14_anim_position.scn", { { 2, E, 14 }, { 6, E, 14 } } },
		{ "rule15_anim_property.scn", {
			{ 3, E, 15 },	// plain COMBAdd cannot be animated
			{ 5, E, 15 },	// 3D animation on 1D radius
			{ 6, E, 15 },	// misspelled propName
			{ 7, E, 15 },	// no propName
			{ 10, E, 15 },	// repetitions animated
			{ 12, E, 15 },	// 1D animation on vec3 translation
		} },
		{ "rule16_duplicate_anim.scn", { { 5, E, 16 } } },
	};

	// Malformed on purpose (it demonstrates the warnings screen)
	constexpr std::string_view DEMO_SCENE{ "Loader_Warnings_Demo.scn" };

	std::string readFile(const fs::path& path)
	{
		std::ifstream in{ path, std::ios::binary };
		std::stringstream text;
		text << in.rdbuf();
		return text.str();
	}

	std::vector<Summary> summary(const std::vector<SceneValidator::Finding>& findings)
	{
		std::vector<Summary> out;
		for (const auto& f : findings) out.emplace_back(f.line, f.severity, f.rule);
		std::ranges::sort(out);
		return out;
	}

	std::string describe(const std::string& name, const std::vector<SceneValidator::Finding>& findings)
	{
		std::string out;
		for (const auto& f : findings) out += "\n  " + f.format(name);
		return out;
	}

	std::string toCrlf(std::string text)
	{
		std::string lf;
		for (size_t i{}; i < text.size(); ++i) {
			if (text[i] == '\r' && i + 1 < text.size() && text[i + 1] == '\n') continue;
			lf += text[i];
		}
		std::string crlf;
		for (char c : lf) {
			if (c == '\n') crlf += '\r';
			crlf += c;
		}
		return crlf;
	}

	std::vector<fs::path> realScenes()
	{
		std::vector<fs::path> scenes;
		for (const auto& entry : fs::directory_iterator(TestPaths::repoRoot() / "CustomScenes")) {
			if (entry.path().extension() == ".scn" && entry.path().filename() != DEMO_SCENE) scenes.push_back(entry.path());
		}
		std::ranges::sort(scenes);
		return scenes;
	}

	// Collects failures so one run reports every bad fixture, not just the first
	void failIfAny(const std::string& failures)
	{
		if (!failures.empty()) Assert::Fail(TestPaths::wide(failures).c_str());
	}
}

TEST_CLASS(SceneValidatorTests)
{
public:
	TEST_METHOD(FixturesGiveExpectedFindings)
	{
		std::string failures;
		for (const auto& [name, expected] : EXPECTED) {
			auto findings = SceneValidator::validate(readFile(TestPaths::fixtures() / name));
			auto wanted = expected;
			std::ranges::sort(wanted);
			if (summary(findings) != wanted) failures += "\n" + name + " gave:" + describe(name, findings);
		}
		failIfAny(failures);
	}

	TEST_METHOD(EveryFixtureIsListed)
	{
		std::set<std::string> onDisk;
		for (const auto& entry : fs::directory_iterator(TestPaths::fixtures())) {
			if (entry.path().extension() == ".scn") onDisk.insert(entry.path().filename().string());
		}
		std::set<std::string> listed;
		for (const auto& [name, expected] : EXPECTED) listed.insert(name);
		Assert::IsTrue(onDisk == listed, L"Tests/fixtures/*.scn and EXPECTED list different files");
	}

	TEST_METHOD(EveryRuleHasAFixture)
	{
		std::set<int> covered;
		for (const auto& [name, expected] : EXPECTED) {
			for (const auto& [line, severity, rule] : expected) covered.insert(rule);
		}
		for (int rule{ 1 }; rule <= 16; ++rule) {
			Assert::IsTrue(covered.contains(rule), (L"no fixture covers rule " + std::to_wstring(rule)).c_str());
		}
	}

	TEST_METHOD(CrlfGivesSameResult)
	{
		std::string failures;
		for (const auto& [name, expected] : EXPECTED) {
			const std::string text{ readFile(TestPaths::fixtures() / name) };
			if (summary(SceneValidator::validate(text)) != summary(SceneValidator::validate(toCrlf(text)))) {
				failures += "\n" + name;
			}
		}
		failIfAny(failures);
	}

	TEST_METHOD(RealScenesHaveNoErrors)
	{
		const auto scenes = realScenes();
		Assert::IsFalse(scenes.empty(), L"no scenes found in CustomScenes/");
		std::string failures;
		for (const auto& path : scenes) {
			const std::string name{ path.filename().string() };
			std::vector<SceneValidator::Finding> errors;
			for (auto& f : SceneValidator::validate(readFile(path))) {
				if (f.severity == Severity::Error) errors.push_back(std::move(f));
			}
			if (!errors.empty()) failures += describe(name, errors);
		}
		failIfAny(failures);
	}
};

#include "SceneValidator.h"
#include <algorithm>
#include <array>
#include <charconv>
#include <functional>
#include <initializer_list>
#include <map>
#include <optional>
#include <set>
#include <utility>

namespace SceneValidator
{
namespace
{
// ---------------------------------------------------------------------------
// Tables (compare these with sections 5-9 of the spec)
// ---------------------------------------------------------------------------

enum class ValueType { Float, Int, Bool, Vec2, Vec3, Name };

constexpr size_t valueCount(ValueType type)
{
	switch (type) {
	case ValueType::Vec2: return 2;
	case ValueType::Vec3: return 3;
	default:              return 1;
	}
}

struct Param
{
	std::string_view name;
	ValueType type;
	std::optional<std::string_view> dflt;	// nullopt only for propName
};

struct Animatable
{
	std::string_view name;
	int dim;
};

enum class Kind { Comb, Shape, Op, Anim, StepBack, Light, Root };

// Parameter and animation lists keep the spec's order, which the messages list in.
// nullopt params: not checked (OPCubeRoller). nullopt animatable: no table to check
// against (OPCubeRoll*, and keywords that are never an animation target).
struct KeywordInfo
{
	Kind kind;
	std::optional<std::vector<Param>> params;
	std::optional<std::vector<Animatable>> animatable;
	int dim{};	// animations only
};

using ParamList = std::vector<Param>;
using AnimList = std::vector<Animatable>;

template <typename T>
std::vector<T> concat(std::initializer_list<std::vector<T>> parts)
{
	std::vector<T> out;
	for (const auto& part : parts) out.insert(out.end(), part.begin(), part.end());
	return out;
}

constexpr std::string_view STEP_BACK{ "stepBack" };
constexpr std::string_view LIGHT{ "LightSource" };
constexpr std::string_view END{ "end" };
constexpr std::string_view COMMENT{ "//" };

// std::less<> lets a std::string-keyed map be searched with a string_view. The map is
// sorted, which also gives near-miss suggestions a stable order.
using KeywordTable = std::map<std::string, KeywordInfo, std::less<>>;

KeywordTable buildKeywords()
{
	using enum ValueType;
	KeywordTable table;

	// Section 5. Plain combinations accept no animations.
	const ParamList smoothParams{ { "smoothing", Float, "0.5" }, { "colorBlending", Float, "1.0" },
		{ "colorShifting", Float, "0.0" } };
	const AnimList smoothAnims{ { "smoothing", 1 }, { "colorBlending", 1 }, { "colorShifting", 1 } };
	table["COMBAdd"] = { Kind::Comb, ParamList{ { "onTime", Float, "1" }, { "offTime", Float, "0" },
		{ "phase", Float, "0" }, { "overwriteColor", Bool, "0" } }, AnimList{} };
	table["COMBSubtract"] = { Kind::Comb, ParamList{}, AnimList{} };
	table["COMBIntersect"] = { Kind::Comb, ParamList{}, AnimList{} };
	table["COMBExclude"] = { Kind::Comb, ParamList{ { "colorMode", Int, "0" } }, AnimList{} };
	for (const char* name : { "COMBAddSmooth", "COMBSubtractSmooth", "COMBIntersectSmooth", "COMBExcludeSmooth" }) {
		table[name] = { Kind::Comb, smoothParams, smoothAnims };
	}

	// Section 6. Every shape also takes color (vec3), and color can be animated (3D).
	const ParamList color{ { "color", Vec3, "1 1 1" } };
	const ParamList axis{ { "axis", Int, "1" } };
	const ParamList twoD{ { "operation", Int, "0" }, { "magnitude", Float, "1" },
		{ "revolveRotations", Float, "0" }, { "revolveRotateOffset", Vec2, "0 0" } };
	const AnimList twoDAnims{ { "magnitude", 1 }, { "revolveRotations", 1 }, { "revolveRotateOffset", 2 } };
	auto shape = [&](const char* name, std::initializer_list<ParamList> params, std::initializer_list<AnimList> anims) {
		table[name] = { Kind::Shape, concat(params), concat(anims) };
	};
	shape("SDFObject", { color }, { { { "color", 3 } } });
	shape("SDFPlane", { color, { { "normal", Vec3, "0 1 0" }, { "offset", Float, "0" } } },
		{ { { "color", 3 }, { "normal", 3 }, { "offset", 1 } } });
	shape("SDFBox", { color, { { "dimensions", Vec3, "1 1 1" } } }, { { { "color", 3 }, { "dimensions", 3 } } });
	shape("SDFBoxFrame", { color, { { "dimensions", Vec3, "1 1 1" }, { "edgeThickness", Float, "0.1" } } },
		{ { { "color", 3 }, { "dimensions", 3 }, { "edgeThickness", 1 } } });
	shape("SDFSphere", { color, { { "radius", Float, "0.5" } } }, { { { "color", 3 }, { "radius", 1 } } });
	shape("SDFEllipsoid", { color, { { "radii", Vec3, "1 0.5 0.25" } } }, { { { "color", 3 }, { "radii", 3 } } });
	shape("SDFCrystal", { color, { { "edgeLength", Float, "1" } } }, { { { "color", 3 }, { "edgeLength", 1 } } });
	shape("SDFCone", { color, { { "height", Float, "1" }, { "sweep", Float, "0.5" } } },
		{ { { "color", 3 }, { "height", 1 }, { "sweep", 1 } } });
	shape("SDFSolidAngle", { color, { { "radius", Float, "0.5" }, { "sweep", Float, "0.25" } } },
		{ { { "color", 3 }, { "radius", 1 }, { "sweep", 1 } } });
	// Axis shapes (axis cannot be animated)
	shape("SDFTorus", { color, axis, { { "radii", Vec2, "1 0.25" } } }, { { { "color", 3 }, { "radii", 2 } } });
	shape("SDFTorusSegment", { color, axis, { { "radii", Vec2, "1 0.25" }, { "sweep", Float, "0.25" } } },
		{ { { "color", 3 }, { "radii", 2 }, { "sweep", 1 } } });
	shape("SDFCapsule", { color, axis, { { "radius", Float, "0.5" }, { "bodyLength", Float, "1" } } },
		{ { { "color", 3 }, { "radius", 1 }, { "bodyLength", 1 } } });
	shape("SDFCylinder", { color, axis, { { "radius", Float, "0.5" }, { "sideLength", Float, "1" } } },
		{ { { "color", 3 }, { "radius", 1 }, { "sideLength", 1 } } });
	shape("SDFTaperedCapsule", { color, axis, { { "radii", Vec2, "0.5 0.25" }, { "bodyLength", Float, "1" } } },
		{ { { "color", 3 }, { "radii", 2 }, { "bodyLength", 1 } } });
	shape("SDFTaperedCylinder", { color, axis, { { "radii", Vec2, "0.5 0.25" }, { "sideLength", Float, "1" } } },
		{ { { "color", 3 }, { "radii", 2 }, { "sideLength", 1 } } });
	// 2D shapes (also take the twoD params)
	const AnimList colorAnim{ { "color", 3 } };
	shape("SDFRhombus", { color, twoD, { { "dimensions", Vec2, "1 0.5" } } }, { colorAnim, twoDAnims, { { "dimensions", 2 } } });
	shape("SDFTriangle", { color, twoD, { { "point1", Vec2, "-0.5 -0.5" }, { "point2", Vec2, "0 0.5" }, { "point3", Vec2, "0.5 0" } } },
		{ colorAnim, twoDAnims, { { "point1", 2 }, { "point2", 2 }, { "point3", 2 } } });
	shape("SDFEquilateralTriangle", { color, twoD, { { "base", Float, "1" } } }, { colorAnim, twoDAnims, { { "base", 1 } } });
	shape("SDFIsoscelesTriangle", { color, twoD, { { "dimensions", Vec2, "1 0.5" } } }, { colorAnim, twoDAnims, { { "dimensions", 2 } } });
	shape("SDFPieSlice", { color, twoD, { { "sweep", Float, "0.25" }, { "radius", Float, "0.5" } } },
		{ colorAnim, twoDAnims, { { "sweep", 1 }, { "radius", 1 } } });
	for (const char* name : { "SDFPentagon", "SDFHexagon", "SDFOctagon" }) {
		shape(name, { color, twoD, { { "radius", Float, "0.5" } } }, { colorAnim, twoDAnims, { { "radius", 1 } } });
	}
	shape("SDFStar", { color, twoD, { { "radius", Float, "0.5" }, { "numPoints", Int, "5" }, { "ratio", Float, "0.5" } } },
		{ colorAnim, twoDAnims, { { "radius", 1 }, { "ratio", 1 }, { "numPoints", 1 } } });

	// Section 7
	auto op = [&](const char* name, std::optional<ParamList> params, std::optional<AnimList> anims) {
		table[name] = { Kind::Op, std::move(params), std::move(anims) };
	};
	op("OPTranslate", ParamList{ { "translation", Vec3, "0 1 0" } }, AnimList{ { "translation", 3 } });
	op("OPRotate", ParamList{ { "axis", Vec3, "0 1 0" }, { "rotations", Float, "0.5" } }, AnimList{ { "rotations", 1 }, { "axis", 3 } });
	op("OPElongate", ParamList{ { "axis", Int, "1" }, { "length", Float, "1" }, { "offset", Float, "0" } },
		AnimList{ { "length", 1 }, { "offset", 1 } });
	op("OPRepeat", ParamList{ { "spacing", Vec3, "1 1 1" }, { "limits", Vec3, "1 1 1" } }, AnimList{ { "spacing", 3 }, { "limits", 3 } });
	op("OPRepeatCircle", ParamList{ { "axis", Int, "1" }, { "repetitions", Int, "10" } }, AnimList{});
	op("OPMirror", ParamList{ { "mirrorX", Bool, "1" }, { "mirrorY", Bool, "0" }, { "mirrorZ", Bool, "0" } }, AnimList{});
	op("OPRoundOff", ParamList{ { "magnitude", Float, "0.01" } }, AnimList{ { "magnitude", 1 } });
	op("OPOnion", ParamList{ { "magnitude", Float, "0.01" } }, AnimList{ { "magnitude", 1 } });
	op("OPInsideOut", ParamList{}, AnimList{});
	// Not for the generator (rule 9). OPCubeRoller's "axes N then N vec3s" has a variable
	// length, so its parameters are not checked.
	op("OPCubeRollHeight", ParamList{ { "cubeHeight", Float, "1" }, { "period", Float, "1" }, { "phase", Float, "0" } }, std::nullopt);
	op("OPCubeRoller", std::nullopt, std::nullopt);

	// Section 8. Numeric params are vec2/vec3 in the 2D/3D variants.
	const std::array<std::pair<std::string_view, std::vector<std::pair<std::string_view, std::string_view>>>, 3> animations{ {
		{ "ANContinuous", { { "amplitude", "1" }, { "phase", "0" } } },
		{ "ANSmoothCycle", { { "period", "1" }, { "amplitude", "1" }, { "phase", "0" } } },
		{ "ANLinearCycle", { { "forwardTime", "1" }, { "delayTime1", "1" }, { "backwardTime", "1" },
			{ "delayTime2", "1" }, { "amplitude", "1" }, { "phase", "0" } } },
	} };
	// Defaults like "1 1 1" need storage that outlives this function
	static std::set<std::string, std::less<>> repeatedDefaults;
	for (const auto& [base, params] : animations) {
		for (int dim{ 1 }; dim <= 3; ++dim) {
			ParamList list{ { "propName", Name, std::nullopt } };
			for (const auto& [name, dflt] : params) {
				std::string repeated{ dflt };
				for (int i{ 1 }; i < dim; ++i) repeated += " " + std::string{ dflt };
				std::string_view stored{ *repeatedDefaults.insert(std::move(repeated)).first };
				list.push_back({ name, dim == 1 ? Float : dim == 2 ? Vec2 : Vec3, stored });
			}
			table[std::string{ base } + std::to_string(dim) + "D"] = { Kind::Anim, std::move(list), std::nullopt, dim };
		}
	}

	// Section 9
	table[std::string{ LIGHT }] = { Kind::Light, ParamList{ { "position", Vec3, "5 5 -3" },
		{ "luminance", Float, "100" }, { "ambientPower", Float, "0.2" }, { "color", Vec3, "1 1 1" } }, std::nullopt };
	table[std::string{ STEP_BACK }] = { Kind::StepBack, ParamList{}, std::nullopt };
	return table;
}

// A function-local static is built on first use. A namespace-scope table built from
// other translation units' statics could be read before they exist (the same static
// initialization order problem SDFPlane::DFLT_NORMAL had with Vec3::Up).
const KeywordTable& keywords()
{
	static const KeywordTable table{ buildKeywords() };
	return table;
}

const KeywordInfo* lookup(std::string_view keyword)
{
	auto it = keywords().find(keyword);
	return it == keywords().end() ? nullptr : &it->second;
}

const std::set<std::string_view> GENERATOR_EXCLUDED{ "OPCubeRollHeight", "OPCubeRoller" };
const std::set<std::string_view> NEEDS_EXISTING_SHAPE{ "COMBSubtract", "COMBIntersect", "COMBExclude",
	"COMBSubtractSmooth", "COMBIntersectSmooth", "COMBExcludeSmooth" };
const std::set<std::string_view> NO_PARAM_COMBINATIONS{ "COMBSubtract", "COMBIntersect" };

// Rule 9 skips these: the spec's own examples leave them out
const std::map<std::string_view, std::set<std::string_view>> EXPLICIT_EXEMPT{
	{ "COMBAdd", { "onTime", "offTime", "phase", "overwriteColor" } },
	{ "SDFObject", { "color" } },
};

struct IntRange { long long low; std::optional<long long> high; };
const std::map<std::string_view, IntRange> INT_RANGES{
	{ "axis", { 0, 2 } }, { "operation", { 0, 2 } }, { "colorMode", { 0, 3 } },
	{ "repetitions", { 1, std::nullopt } }, { "numPoints", { 1, std::nullopt } },
};

using KeywordProp = std::pair<std::string_view, std::string_view>;
const std::set<KeywordProp> NONZERO_VECTORS{ { "SDFPlane", "normal" }, { "OPRotate", "axis" } };

// Rule 15: properties that are accepted but should not be animated
struct Hazard { Severity severity; std::string_view message; };
const std::map<KeywordProp, Hazard> ANIMATION_HAZARDS{
	{ { "SDFPlane", "normal" }, { Severity::Warning, "an animated plane normal is not re-normalized; avoid it" } },
	{ { "OPRotate", "axis" }, { Severity::Warning, "an animated rotation axis is not re-normalized; avoid it" } },
	{ { "OPRotate", "rotMat" }, { Severity::Error, "rotMat is internal and cannot be animated; the loader drops the animation" } },
	{ { "OPRepeatCircle", "repetitions" }, { Severity::Error,
		"do not animate repetitions: the loader accepts it, but fractional counts are untested, "
		"so the format does not allow it" } },
};

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

constexpr bool isSpace(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f'; }
constexpr bool isDigit(char c) { return c >= '0' && c <= '9'; }

std::vector<std::string_view> splitWords(std::string_view s)
{
	std::vector<std::string_view> words;
	size_t i{};
	while (i < s.size()) {
		while (i < s.size() && isSpace(s[i])) ++i;
		size_t start{ i };
		while (i < s.size() && !isSpace(s[i])) ++i;
		if (i > start) words.push_back(s.substr(start, i - start));
	}
	return words;
}

std::string_view rstrip(std::string_view s)
{
	while (!s.empty() && isSpace(s.back())) s.remove_suffix(1);
	return s;
}

// [+-]?(\d+\.?\d*|\.\d+)([eE][+-]?\d+)?
bool isNumber(std::string_view s)
{
	size_t i{};
	auto digits = [&] { size_t start{ i }; while (i < s.size() && isDigit(s[i])) ++i; return i - start; };
	if (i < s.size() && (s[i] == '+' || s[i] == '-')) ++i;
	if (digits() > 0) {
		if (i < s.size() && s[i] == '.') { ++i; digits(); }
	}
	else {
		if (i >= s.size() || s[i] != '.') return false;
		++i;
		if (digits() == 0) return false;
	}
	if (i < s.size() && (s[i] == 'e' || s[i] == 'E')) {
		++i;
		if (i < s.size() && (s[i] == '+' || s[i] == '-')) ++i;
		if (digits() == 0) return false;
	}
	return i == s.size();
}

// [+-]?\d+
bool isInt(std::string_view s)
{
	if (!s.empty() && (s[0] == '+' || s[0] == '-')) s.remove_prefix(1);
	return !s.empty() && std::ranges::all_of(s, isDigit);
}

// std::from_chars rejects a leading '+', which the number patterns allow
std::string_view withoutPlus(std::string_view s) { return !s.empty() && s[0] == '+' ? s.substr(1) : s; }

std::string lower(std::string_view s)
{
	std::string out{ s };
	for (char& c : out) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
	return out;
}

size_t editDistance(std::string_view a, std::string_view b)
{
	std::vector<size_t> previous(b.size() + 1);
	for (size_t j{}; j <= b.size(); ++j) previous[j] = j;
	for (size_t i{ 1 }; i <= a.size(); ++i) {
		std::vector<size_t> current{ i };
		for (size_t j{ 1 }; j <= b.size(); ++j) {
			current.push_back(std::min({ previous[j] + 1, current[j - 1] + 1,
				previous[j - 1] + (a[i - 1] != b[j - 1] ? 1u : 0u) }));
		}
		previous = std::move(current);
	}
	return previous.back();
}

// Closest candidate if it differs only by case or by at most 2 edits
std::optional<std::string_view> nearMiss(std::string_view word, std::vector<std::string_view> candidates)
{
	std::ranges::sort(candidates);
	const std::string wordLower{ lower(word) };
	for (auto candidate : candidates) {
		if (lower(candidate) == wordLower) return candidate;
	}
	if (candidates.empty()) return std::nullopt;
	auto best = std::ranges::min_element(candidates, {}, [&](std::string_view c) { return editDistance(word, c); });
	if (editDistance(word, *best) <= 2) return *best;
	return std::nullopt;
}

template <typename Range, typename Proj = std::identity>
std::string join(const Range& items, std::string_view separator, Proj proj = {})
{
	std::string out;
	bool first{ true };
	for (const auto& item : items) {
		if (!first) out += separator;
		out += std::string_view{ std::invoke(proj, item) };
		first = false;
	}
	return out;
}

std::vector<std::string_view> paramNames(const ParamList& params)
{
	std::vector<std::string_view> names;
	for (const auto& p : params) names.push_back(p.name);
	return names;
}

const Param* findParam(const ParamList& params, std::string_view name)
{
	auto it = std::ranges::find(params, name, &Param::name);
	return it == params.end() ? nullptr : &*it;
}

const Animatable* findAnimatable(const AnimList& anims, std::string_view name)
{
	auto it = std::ranges::find(anims, name, &Animatable::name);
	return it == anims.end() ? nullptr : &*it;
}

bool isComment(std::string_view token) { return token.starts_with(COMMENT); }

// ---------------------------------------------------------------------------
// Validation
// ---------------------------------------------------------------------------

struct Entry
{
	size_t line;
	std::string_view keyword;
	std::vector<std::string_view> args;
};

using Target = std::pair<size_t, std::string_view>;	// (line, keyword)

class Validator
{
public:
	explicit Validator(std::string_view text)
	{
		// Accept LF or CRLF line endings; a final newline does not add a line.
		size_t start{};
		while (true) {
			size_t end{ text.find('\n', start) };
			std::string_view line{ text.substr(start, end == std::string_view::npos ? std::string_view::npos : end - start) };
			while (!line.empty() && line.back() == '\r') line.remove_suffix(1);
			m_lines.emplace_back(line);
			if (end == std::string_view::npos) break;
			start = end + 1;
		}
		if (m_lines.size() > 1 && m_lines.back().empty()) m_lines.pop_back();
	}

	std::vector<Finding> run()
	{
		checkLineOne();
		std::vector<Entry> entries;	// keyword lines from line 2 on
		std::optional<size_t> endLine;
		for (size_t number{ 1 }; number <= m_lines.size(); ++number) {
			std::string_view raw{ m_lines[number - 1] };
			checkLexical(number, raw);
			auto tokens = splitWords(raw);
			if (number == 1 || tokens.empty() || isComment(tokens[0])) continue;
			std::string_view keyword{ tokens[0] };
			if (endLine) {
				add(number, Severity::Error, 4, "'end' on line " + std::to_string(*endLine) + " must be the last line");
			}
			if (keyword == END) {
				endLine = number;
				if (tokens.size() > 1) add(number, Severity::Error, 4, "'end' must be alone on its line");
				continue;
			}
			if (!lookup(keyword)) {
				reportUnknownKeyword(number, keyword);
				continue;
			}
			std::vector<std::string_view> args(tokens.begin() + 1, tokens.end());
			checkParams(number, keyword, args);
			entries.push_back({ number, keyword, std::move(args) });
		}
		checkStructure(entries);
		std::ranges::stable_sort(m_findings, {}, [](const Finding& f) { return std::pair{ f.line, f.rule }; });
		return std::move(m_findings);
	}

private:
	std::vector<std::string> m_lines;
	std::vector<Finding> m_findings;

	void add(size_t line, Severity severity, int rule, std::string message)
	{
		m_findings.push_back({ line, severity, rule, std::move(message) });
	}

	// Rule 1 -----------------------------------------------------------------

	void checkLineOne()
	{
		std::string_view first{ m_lines.empty() ? std::string_view{} : std::string_view{ m_lines[0] } };
		if (first == "SDFObject") return;
		auto tokens = splitWords(first);
		if (!tokens.empty() && tokens[0] == "SDFObject") {
			add(1, Severity::Warning, 1, "line 1 should be exactly 'SDFObject'; the loader never reads line 1, "
				"so these parameters do nothing");
		}
		else {
			add(1, Severity::Error, 1, "line 1 must be exactly 'SDFObject'; the loader never reads line 1, "
				"so whatever is written here is lost");
		}
	}

	// Rules 2-3 --------------------------------------------------------------

	void checkLexical(size_t number, std::string_view raw)
	{
		auto words = splitWords(raw);
		if (words.empty()) {
			add(number, Severity::Error, 2, "blank or whitespace-only line");
			return;
		}
		if (rstrip(raw).size() != raw.size()) add(number, Severity::Error, 2, "trailing whitespace");
		if (raw.substr(std::min(raw.find_first_not_of('\t'), raw.size())).find('\t') != std::string_view::npos) {
			add(number, Severity::Error, 2, "tab used other than as leading indentation");
		}
		if (raw.find(',') != std::string_view::npos && !isComment(words[0])) {
			add(number, Severity::Error, 2, "commas are not part of the format; separate values with spaces "
				"(the loader drops the rest of the line at a comma)");
		}
	}

	void reportUnknownKeyword(size_t number, std::string_view keyword)
	{
		std::vector<std::string_view> candidates{ END };
		for (const auto& [name, info] : keywords()) candidates.push_back(name);
		std::string word{ keyword };
		if (auto guess = nearMiss(keyword, candidates)) {
			add(number, Severity::Error, 3, "unknown keyword '" + word + "' (did you mean '" + std::string{ *guess }
				+ "'?); the loader skips this line");
		}
		else {
			add(number, Severity::Error, 3, "unknown keyword '" + word
				+ "'; the loader skips this line (use '// text' for comments)");
		}
	}

	// Rules 5-9 --------------------------------------------------------------

	void checkParams(size_t number, std::string_view keyword, const std::vector<std::string_view>& args)
	{
		const std::string kw{ keyword };
		if (GENERATOR_EXCLUDED.contains(keyword)) {
			add(number, Severity::Warning, 9, kw + " is undocumented and not allowed in generated scenes");
		}
		const KeywordInfo& info{ *lookup(keyword) };
		if (!info.params) return;
		const ParamList& table{ *info.params };
		if (NO_PARAM_COMBINATIONS.contains(keyword) && !args.empty()) {
			add(number, Severity::Warning, 8, kw + " takes no parameters; '" + join(args, " ") + "' is ignored");
			return;
		}

		// name -> values; try_emplace keeps the first values given for a repeated name
		std::map<std::string_view, std::vector<std::string_view>> seen;
		size_t i{};
		while (i < args.size()) {
			std::string_view name{ args[i] };
			++i;
			const Param* param{ findParam(table, name) };
			if (!param) {
				const std::string nm{ name };
				const auto guess = nearMiss(name, paramNames(table));
				const std::string hint{ guess ? " (did you mean '" + std::string{ *guess } + "'?)" : "" };
				const ParamList& addParams{ *lookup("COMBAdd")->params };
				if (keyword == "COMBExclude" && findParam(addParams, name)) {
					add(number, Severity::Warning, 8, "COMBExclude only takes colorMode; '" + nm + "' is ignored");
				}
				else if (!table.empty()) {
					add(number, Severity::Error, 5, "'" + nm + "' is not a parameter of " + kw + hint
						+ "; valid: " + join(table, ", ", &Param::name));
				}
				else {
					add(number, Severity::Error, 5, kw + " takes no parameters, got '" + nm + "'");
				}
				// Skip the values that belong to the unknown name.
				while (i < args.size() && isNumber(args[i])) ++i;
				continue;
			}

			const std::string nm{ name };
			if (seen.contains(name)) {
				add(number, Severity::Error, 5, "'" + nm + "' is repeated (first given earlier on this line)");
			}
			if (param->type == ValueType::Name) {
				if (i >= args.size()) {
					add(number, Severity::Error, 5, "'" + nm + "' needs a value");
					seen.try_emplace(name);
				}
				else {
					seen.try_emplace(name, std::vector{ args[i] });
					++i;
				}
				continue;
			}

			// Take numbers up to the next parameter name. A non-number is taken only while
			// values are still missing, so a misspelled name that follows a complete value
			// is reported as a name.
			const size_t expected{ valueCount(param->type) };
			std::vector<std::string_view> values;
			while (i < args.size() && !findParam(table, args[i]) && (isNumber(args[i]) || values.size() < expected)) {
				values.push_back(args[i]);
				++i;
			}
			seen.try_emplace(name, values);
			checkValues(number, keyword, *param, values);
		}

		checkExplicit(number, keyword, info, seen);
	}

	void checkValues(size_t number, std::string_view keyword, const Param& param, const std::vector<std::string_view>& values)
	{
		const std::string nm{ param.name };
		const size_t expected{ valueCount(param.type) };
		if (values.size() != expected) {
			add(number, Severity::Error, 5, "'" + nm + "' needs exactly " + std::to_string(expected)
				+ (expected == 1 ? " value" : " values") + ", got " + std::to_string(values.size()));
		}
		auto bad = std::ranges::find_if_not(values, isNumber);
		if (bad != values.end()) {
			add(number, Severity::Error, 6, "'" + nm + "' value '" + std::string{ *bad }
				+ "' is not a number (the loader drops the rest of the line here)");
			return;
		}
		if (values.size() != expected) return;
		if (param.type == ValueType::Int) {
			if (!isInt(values[0])) {
				add(number, Severity::Error, 6, "'" + nm + "' must be a whole number, got '" + std::string{ values[0] } + "'");
				return;
			}
			if (auto range = INT_RANGES.find(param.name); range != INT_RANGES.end()) {
				const auto [low, high] = range->second;
				std::string_view digits{ withoutPlus(values[0]) };
				long long value{};
				const auto result = std::from_chars(digits.data(), digits.data() + digits.size(), value);
				// A number too large for long long is out of every range; print it as written
				const bool overflow{ result.ec == std::errc::result_out_of_range };
				if (overflow || value < low || (high && value > *high)) {
					const std::string allowed{ high ? std::to_string(low) + "-" + std::to_string(*high) : ">= " + std::to_string(low) };
					add(number, Severity::Error, 7, "'" + nm + "' must be " + allowed + ", got "
						+ (overflow ? std::string{ digits } : std::to_string(value)));
				}
			}
		}
		else if (param.type == ValueType::Bool) {
			if (values[0] != "0" && values[0] != "1") {
				add(number, Severity::Error, 6, "'" + nm + "' must be exactly 0 or 1, got '" + std::string{ values[0] } + "'");
			}
		}
		if (NONZERO_VECTORS.contains({ keyword, param.name })) {
			const bool allZero = std::ranges::all_of(values, [](std::string_view v) {
				std::string_view s{ withoutPlus(v) };
				double d{};
				std::from_chars(s.data(), s.data() + s.size(), d);
				return d == 0.0;
			});
			if (allZero) add(number, Severity::Error, 7, std::string{ keyword } + " '" + nm + "' must not be 0 0 0");
		}
	}

	void checkExplicit(size_t number, std::string_view keyword, const KeywordInfo& info,
		const std::map<std::string_view, std::vector<std::string_view>>& seen)
	{
		const auto exempt = EXPLICIT_EXEMPT.find(keyword);
		std::vector<const Param*> missing;
		for (const Param& p : *info.params) {
			if (seen.contains(p.name)) continue;
			if (exempt != EXPLICIT_EXEMPT.end() && exempt->second.contains(p.name)) continue;
			if (info.kind == Kind::Anim && p.name == "propName") continue;	// rule 15 reports it
			missing.push_back(&p);
		}
		if (missing.empty()) return;
		std::vector<std::string> defaults;
		for (const Param* p : missing) {
			if (p->dflt) defaults.push_back(std::string{ p->name } + " " + std::string{ *p->dflt });
		}
		add(number, Severity::Warning, 9, "write every parameter explicitly; missing: "
			+ join(missing, ", ", [](const Param* p) { return p->name; })
			+ " (defaults used: " + join(defaults, ", ") + ")");
	}

	// Rules 10-16 ------------------------------------------------------------

	struct Level { std::string_view keyword; size_t line; bool hasShape; };

	void checkStructure(const std::vector<Entry>& entries)
	{
		std::vector<Level> levels{ { "SDFObject", 1, false } };	// the root is an empty group
		std::optional<Target> pendingComb;	// a combination still waiting for its shape
		Kind previousKind{ Kind::Root };	// last meaningful line
		size_t previousLine{ 1 };
		std::optional<Target> animTarget;	// what the last animation bound to
		std::map<std::pair<size_t, std::string_view>, size_t> animated;	// (target line, propName) -> line
		std::optional<size_t> firstLight;
		bool stackEndChecked{};

		auto closeStack = [&](size_t line) {
			if (stackEndChecked) return;
			stackEndChecked = true;
			if (pendingComb) {
				reportLonelyComb(*pendingComb);
				pendingComb.reset();
			}
			const size_t openLevels{ levels.size() - 1 };
			if (openLevels) {
				add(line, Severity::Warning, 11, std::to_string(openLevels) + " level(s) still open at the end of the "
					"stack lines (innermost opened on line " + std::to_string(levels.back().line)
					+ "); the loader closes them implicitly");
			}
		};

		for (const auto& [number, keyword, args] : entries) {
			const KeywordInfo& info{ *lookup(keyword) };
			const Kind kind{ info.kind };
			const std::string kw{ keyword };

			// Rule 13: lights come after every stack line.
			if (kind == Kind::Light) {
				if (!firstLight) {
					firstLight = number;
					closeStack(number);
				}
			}
			else if (firstLight) {
				add(number, Severity::Error, 13, "stack line after the first LightSource (line "
					+ std::to_string(*firstLight) + "); put all LightSource lines at the end");
			}

			// Rule 10: a combination must be followed by exactly one shape.
			if (pendingComb && (kind == Kind::Comb || kind == Kind::Op || kind == Kind::StepBack || kind == Kind::Light)) {
				reportLonelyComb(*pendingComb);
				pendingComb.reset();
			}

			switch (kind) {
			case Kind::Comb:
				if (NEEDS_EXISTING_SHAPE.contains(keyword) && !levels.back().hasShape) {
					add(number, Severity::Error, 12, kw + " needs an earlier shape in the same level to act on, "
						"but the level opened on line " + std::to_string(levels.back().line) + " is still empty");
				}
				pendingComb = Target{ number, keyword };
				animTarget = Target{ number, keyword };
				break;
			case Kind::Shape:
				if (!pendingComb) {
					add(number, Severity::Error, 10, kw + " has no combination line (COMB*) directly before it; "
						"the loader skips this shape and everything under it, up to its closing stepBack");
				}
				pendingComb.reset();
				levels.back().hasShape = true;
				levels.push_back({ keyword, number, keyword != "SDFObject" });	// a real shape is something to act on
				animTarget = Target{ number, keyword };
				break;
			case Kind::Op:
				animTarget = Target{ number, keyword };
				break;
			case Kind::StepBack:
				if (levels.size() == 1) {
					add(number, Severity::Warning, 11, "stepBack at root depth does nothing (the loader ignores it); "
						"check the level count");
				}
				else {
					levels.pop_back();
				}
				animTarget.reset();
				break;
			case Kind::Light:
				animTarget.reset();
				break;
			case Kind::Anim: {
				auto target = checkAnimationPosition(number, keyword, previousKind, previousLine, animTarget);
				if (target) checkAnimationProperty(number, keyword, info, args, *target, animated);
				animTarget = target;	// an invalid position stays invalid for the next AN*
				break;
			}
			case Kind::Root:
				break;
			}

			previousKind = kind;
			previousLine = number;
		}

		closeStack(entries.empty() ? m_lines.size() : entries.back().line);
		if (!firstLight) add(m_lines.empty() ? 1 : m_lines.size(), Severity::Error, 13, "the scene needs at least one LightSource");
	}

	void reportLonelyComb(const Target& comb)
	{
		add(comb.first, Severity::Error, 10, std::string{ comb.second }
			+ " is not followed by a shape; the loader drops the combination");
	}

	std::optional<Target> checkAnimationPosition(size_t number, std::string_view keyword, Kind previousKind,
		size_t previousLine, const std::optional<Target>& animTarget)
	{
		if (previousKind == Kind::Shape || previousKind == Kind::Op || previousKind == Kind::Comb) return animTarget;
		if (previousKind == Kind::Anim) return animTarget;	// same target as the animation before it
		std::string where;
		if (previousKind == Kind::Root) where = "line 1 (the root, which the loader never reads)";
		else if (previousKind == Kind::StepBack) where = "a stepBack (line " + std::to_string(previousLine) + ")";
		else where = "a LightSource (line " + std::to_string(previousLine) + ")";
		add(number, Severity::Error, 14, std::string{ keyword } + " must follow a shape, OP*, COMB* or AN* line, not "
			+ where + "; the loader drops the animation");
		return std::nullopt;
	}

	void checkAnimationProperty(size_t number, std::string_view keyword, const KeywordInfo& info,
		const std::vector<std::string_view>& args, const Target& target,
		std::map<std::pair<size_t, std::string_view>, size_t>& animated)
	{
		const auto [targetLine, targetKeyword] = target;
		const std::string tk{ targetKeyword };
		const std::string where{ tk + " (line " + std::to_string(targetLine) + ")" };

		// The first value after the first propName
		std::string_view prop;
		for (size_t i{}; i + 1 < args.size(); ++i) {
			if (args[i] == "propName") { prop = args[i + 1]; break; }
		}
		if (prop.empty()) {
			add(number, Severity::Error, 15, std::string{ keyword } + " has no propName; the loader drops the animation");
			return;
		}
		const std::string pn{ prop };
		const auto hazard = ANIMATION_HAZARDS.find({ targetKeyword, prop });
		const auto& animatable{ lookup(targetKeyword)->animatable };
		if (!animatable) return;	// target not in the tables (OPCubeRoll*); nothing to check against
		if (hazard != ANIMATION_HAZARDS.end() && hazard->second.severity == Severity::Error) {
			add(number, Severity::Error, 15, where + ": " + std::string{ hazard->second.message });
			return;
		}
		if (animatable->empty()) {
			add(number, Severity::Error, 15, where + " cannot be animated; the loader drops the animation");
			return;
		}
		const Animatable* property{ findAnimatable(*animatable, prop) };
		if (!property) {
			std::vector<std::string_view> names;
			for (const auto& a : *animatable) names.push_back(a.name);
			const auto guess = nearMiss(prop, names);
			const std::string hint{ guess ? " (did you mean '" + std::string{ *guess } + "'?)" : "" };
			add(number, Severity::Error, 15, "'" + pn + "' is not animatable on " + where + hint
				+ "; valid: " + join(*animatable, ", ", &Animatable::name));
			return;
		}
		if (info.dim != property->dim) {
			const std::string needed{ std::to_string(property->dim) };
			add(number, Severity::Error, 15, "'" + pn + "' on " + tk + " is " + needed + "D but " + std::string{ keyword }
				+ " is " + std::to_string(info.dim) + "D; the loader drops the animation. Use the " + needed + "D variant");
		}
		if (hazard != ANIMATION_HAZARDS.end()) {
			add(number, hazard->second.severity, 15, where + ": " + std::string{ hazard->second.message });
		}

		// Rule 16
		if (auto [it, inserted] = animated.try_emplace({ targetLine, prop }, number); !inserted) {
			add(number, Severity::Error, 16, "'" + pn + "' on " + where + " is already animated on line "
				+ std::to_string(it->second) + "; only the later animation takes effect");
		}
	}
};
}	// namespace

std::string Finding::format(std::string_view path) const
{
	return std::string{ path } + ":" + std::to_string(line) + ": " + (severity == Severity::Error ? "error" : "warning")
		+ ": [rule " + std::to_string(rule) + "] " + message;
}

std::vector<Finding> validate(std::string_view text)
{
	return Validator{ text }.run();
}

bool affectsLoading(const Finding& finding)
{
	return finding.severity == Severity::Error && finding.rule != 2 && finding.rule != 4;
}
}	// namespace SceneValidator

// Loads the scenes in CustomScenes/ through SDFScene, the same path the app's menu uses.
#include "CppUnitTest.h"
#include <filesystem>
#include <string>
#include "TestPaths.h"
#include "../SDFScene.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace fs = std::filesystem;

namespace
{
	// SDFScene reads ".\CustomScenes\<name>", so run each test from the repository root
	struct RepoWorkingDirectory
	{
		fs::path previous{ fs::current_path() };
		RepoWorkingDirectory() { fs::current_path(TestPaths::repoRoot()); }
		~RepoWorkingDirectory() { fs::current_path(previous); }
	};
}

TEST_CLASS(SceneLoadTests)
{
public:
	TEST_METHOD(RealScenesLoadWithoutProblems)
	{
		RepoWorkingDirectory cwd;
		std::string failures;
		size_t count{};
		for (const auto& entry : fs::directory_iterator("CustomScenes")) {
			const std::string name{ entry.path().filename().string() };
			if (entry.path().extension() != ".scn" || name == "Loader_Warnings_Demo.scn") continue;
			++count;
			SDFScene scene{ name };
			Assert::IsNotNull(scene.baseObject(), TestPaths::wide(name).c_str());
			Assert::IsFalse(scene.lights().empty(), TestPaths::wide(name + " has no lights").c_str());
			for (const auto& warning : scene.loadWarnings()) failures += "\n" + name + ": " + warning;
		}
		Assert::IsTrue(count > 0, L"no scenes found in CustomScenes/");
		if (!failures.empty()) Assert::Fail(TestPaths::wide(failures).c_str());
	}

	TEST_METHOD(DemoSceneReportsItsProblems)
	{
		RepoWorkingDirectory cwd;
		SDFScene scene{ "Loader_Warnings_Demo.scn" };
		// 13 validator errors from its deliberate mistakes
		Assert::IsTrue(scene.loadWarnings().size() == 13,
			(L"expected 13 problems, got " + std::to_wstring(scene.loadWarnings().size())).c_str());
		Assert::IsNotNull(scene.baseObject());
	}
};

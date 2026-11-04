#pragma once
#include <filesystem>
#include <string>
#include <string_view>

namespace TestPaths
{
	// The repository root, found from this header's own path (__FILE__ is a full path:
	// the project compiles with /FC), so the tests don't depend on the working directory.
	inline std::filesystem::path repoRoot()
	{
		return std::filesystem::path{ __FILE__ }.parent_path().parent_path();
	}

	inline std::filesystem::path fixtures() { return repoRoot() / "Tests" / "fixtures"; }

	// Assert messages are wide strings; scene files and findings are ASCII
	inline std::wstring wide(std::string_view s) { return { s.begin(), s.end() }; }
}

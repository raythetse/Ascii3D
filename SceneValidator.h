#pragma once
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

// Checks .scn text against the strict format in ClaudeNotes/scene_format_spec_v0.4.md
// (section 11, rules 1-16). Tests: Tests/SceneValidatorTests.cpp, with one fixture per
// rule in Tests/fixtures/. From the command line: Ascii3D.exe --validate FILE...
namespace SceneValidator
{
	enum class Severity { Error, Warning };

	struct Finding
	{
		size_t line;		// 1-based
		Severity severity;
		int rule;			// rule number in spec section 11
		std::string message;

		// "path:line: error|warning: [rule N] message", as printed by Ascii3D.exe --validate
		std::string format(std::string_view path) const;
	};

	// Every finding, sorted by line, then rule
	std::vector<Finding> validate(std::string_view text);

	// True for findings where the loader loads something other than what the line says:
	// errors, except the purely lexical rules 2 (whitespace, commas) and 4 (end placement).
	// These are the findings the app shows before rendering; warnings are style and
	// generator rules.
	bool affectsLoading(const Finding& finding);
}

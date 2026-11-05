// Tests for Vec2 and Vec3, starting with the math bugs found in review (handoff 2026-11-02):
// division assigned the divisor, floor() returned the fractional part, and Vec2::cross
// returned a Vec2. They are fixed; these tests keep them fixed.
#include "CppUnitTest.h"
#include <format>
#include <string>
#include <type_traits>
#include "../Vec2.h"
#include "../Vec3.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	std::wstring show(const Vec2& v) { return std::format(L"({}, {})", v.x, v.y); }
	std::wstring show(const Vec3& v) { return std::format(L"({}, {}, {})", v.x, v.y, v.z); }

	template <typename V>
	void assertEqual(const V& expected, const V& actual)
	{
		Assert::IsTrue(expected == actual, (L"expected " + show(expected) + L", got " + show(actual)).c_str());
	}
}

// Compile-time checks: these document the types, and fail the build if they regress
static_assert(std::is_same_v<decltype(Vec2{}.cross(Vec2{})), double>, "Vec2::cross returns a scalar");
static_assert(!std::is_convertible_v<Vec3, bool>, "Vec3::operator bool is explicit");
static_assert(std::is_trivially_copyable_v<Vec3> && std::is_trivially_copyable_v<Vec2>, "rule of zero");

TEST_CLASS(Vec3Tests)
{
public:
	TEST_METHOD(DivideAssignByVectorDividesEachComponent)
	{
		Vec3 v{ 6.0, 8.0, 10.0 };
		v /= Vec3{ 2.0, 4.0, 5.0 };
		assertEqual(Vec3{ 3.0, 2.0, 2.0 }, v);
	}

	TEST_METHOD(DivideAssignByScalarMatchesDivide)
	{
		Vec3 v{ 6.0, 8.0, 10.0 };
		v /= 2.0;
		assertEqual(Vec3{ 3.0, 4.0, 5.0 }, v);
		assertEqual(Vec3{ 6.0, 8.0, 10.0 } / 2.0, v);
	}

	TEST_METHOD(DivideByZeroGivesZero)
	{
		// The project's convention: a zero divisor gives 0 instead of inf/NaN, the same
		// for /, /=, scalar and per component
		assertEqual(Vec3::Zero, Vec3{ 1.0, 2.0, 3.0 } / 0.0);
		Vec3 v{ 1.0, 2.0, 3.0 };
		v /= 0.0;
		assertEqual(Vec3::Zero, v);
		assertEqual(Vec3{ 1.0, 0.0, 3.0 }, Vec3{ 2.0, 5.0, 6.0 } / Vec3{ 2.0, 0.0, 2.0 });
	}

	TEST_METHOD(FloorRoundsDown)
	{
		// It used to return the fractional part (GLSL fract)
		assertEqual(Vec3{ 1.0, -2.0, 2.0 }, Vec3{ 1.5, -1.5, 2.0 }.floor());
	}

	TEST_METHOD(CrossFollowsRightHandRule)
	{
		assertEqual(Vec3{ 0.0, 0.0, 1.0 }, Vec3{ 1.0, 0.0, 0.0 }.cross(Vec3{ 0.0, 1.0, 0.0 }));
		assertEqual(Vec3{ 0.0, 0.0, -1.0 }, Vec3{ 0.0, 1.0, 0.0 }.cross(Vec3{ 1.0, 0.0, 0.0 }));
	}

	TEST_METHOD(QuantizeRoundsToSteps)
	{
		assertEqual(Vec3{ 0.5, 1.0, -0.5 }, Vec3{ 0.4, 1.1, -0.6 }.quantize(2));
		assertEqual(Vec3{ 0.4, 1.1, -0.6 }, Vec3{ 0.4, 1.1, -0.6 }.quantize(0));	// 0: unchanged
	}

	TEST_METHOD(AssignFromScalarSetsEveryComponent)
	{
		// Uses the implicit Vec3(double) constructor; there is no separate operator=(double)
		Vec3 v{ 1.0, 2.0, 3.0 };
		v = 2.0;
		assertEqual(Vec3{ 2.0 }, v);
	}

	TEST_METHOD(BoolIsFalseOnlyForZero)
	{
		Assert::IsFalse(static_cast<bool>(Vec3::Zero));
		Assert::IsTrue(static_cast<bool>(Vec3{ 0.0, 0.0, 0.1 }));
	}

	TEST_METHOD(StaticConstantsAreInitialized)
	{
		// Defined in Vec3.cpp; checked at run time, after static initialization has finished
		assertEqual(Vec3{ 0.0, 1.0, 0.0 }, Vec3::Up);
		assertEqual(Vec3{ 1.0, 1.0, 1.0 }, Vec3::White);
	}
};

TEST_CLASS(Vec2Tests)
{
public:
	TEST_METHOD(DivideAssignByVectorDividesEachComponent)
	{
		// y used to be assigned the divisor
		Vec2 v{ 6.0, 8.0 };
		v /= Vec2{ 2.0, 4.0 };
		assertEqual(Vec2{ 3.0, 2.0 }, v);
	}

	TEST_METHOD(DivideAssignByScalarMatchesDivide)
	{
		Vec2 v{ 6.0, 8.0 };
		v /= 2.0;
		assertEqual(Vec2{ 3.0, 4.0 }, v);
		assertEqual(Vec2{ 6.0, 8.0 } / 2.0, v);
	}

	TEST_METHOD(DivideByZeroGivesZero)
	{
		assertEqual(Vec2::Zero, Vec2{ 1.0, 2.0 } / 0.0);
		Vec2 v{ 1.0, 2.0 };
		v /= 0.0;
		assertEqual(Vec2::Zero, v);
	}

	TEST_METHOD(CrossIsSignedArea)
	{
		Assert::AreEqual(1.0, Vec2{ 1.0, 0.0 }.cross(Vec2{ 0.0, 1.0 }));
		Assert::AreEqual(-1.0, Vec2{ 0.0, 1.0 }.cross(Vec2{ 1.0, 0.0 }));
		Assert::AreEqual(6.0, Vec2{ 2.0, 0.0 }.cross(Vec2{ 1.0, 3.0 }));
	}

	TEST_METHOD(EqualityComparesBothComponents)
	{
		Assert::IsTrue(Vec2{ 1.0, 2.0 } == Vec2{ 1.0, 2.0 });
		Assert::IsFalse(Vec2{ 1.0, 2.0 } == Vec2{ 1.0, 3.0 });
		Assert::IsFalse(Vec2{ 1.0, 2.0 } != Vec2{ 1.0, 2.0 });	// != is generated from ==
		Assert::IsTrue(Vec2{ 2.0 } == 2.0);
	}
};

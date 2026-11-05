// Tests for the animation value formulas (spec section 8). GameTimer can't be set to an
// arbitrary time, but reset() puts game time at exactly 0, so every test reads the value
// at t = 0. That is enough to pin down what phase and amplitude mean.
#include "CppUnitTest.h"
#include <format>
#include <string>
#include "../Animation.h"
#include "../GameTimer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	constexpr double tolerance = 1e-12;

	// The property's value at t = 0, starting from init
	template <typename Anim, AnimType T>
	T valueAtStart(Anim animation, T init)
	{
		GameTimer::reset();
		T property = init;
		Assert::IsTrue(animation.bindTo(property), L"animation should bind to its own type");
		animation.update();
		return property;
	}

	void assertNear(const Vec3& expected, const Vec3& actual)
	{
		auto message = std::format(L"expected ({}, {}, {}), got ({}, {}, {})",
			expected.x, expected.y, expected.z, actual.x, actual.y, actual.z);
		Assert::IsTrue((expected - actual).length() < tolerance, message.c_str());
	}
}

TEST_CLASS(AnimationTests)
{
public:
	TEST_METHOD(SmoothCyclePhaseIsInSeconds)
	{
		// period 2 s, phase 0.5 s = a quarter cycle ahead: sin(pi/2) = 1, so init + amplitude.
		// When phase was a fraction of the period, this was half a cycle: sin(pi) = 0, so init.
		double value = valueAtStart(ANSmoothCycle1D{ "radius", 2.0, 0.5, 0.5 }, 1.0);
		Assert::AreEqual(1.5, value, tolerance);
	}

	TEST_METHOD(SmoothCycleStartsAtInitWithNoPhase)
	{
		Assert::AreEqual(1.0, valueAtStart(ANSmoothCycle1D{ "radius", 2.0, 0.5, 0.0 }, 1.0), tolerance);
	}

	TEST_METHOD(SmoothCycle3DPhaseIsPerComponentSeconds)
	{
		// Each component is a quarter cycle ahead (phase = period / 4), except z, which has no phase
		ANSmoothCycle3D animation{ "translation", Vec3{ 2.0, 4.0, 1.0 }, Vec3{ 1.0 }, Vec3{ 0.5, 1.0, 0.0 } };
		assertNear(Vec3{ 1.0, 1.0, 0.0 }, valueAtStart(animation, Vec3{}));
	}

	TEST_METHOD(LinearCycleIsCenteredOnInit)
	{
		// Cycle: forward 1 s, hold 1 s, backward 1 s, hold 1 s. The value runs from
		// init - amplitude (progress 0) to init + amplitude (progress 1).
		auto at = [](double phase) {
			return valueAtStart(ANLinearCycle1D{ "radius", 0.25, 1.0, 1.0, 1.0, 1.0, phase }, 1.0);
		};
		Assert::AreEqual(0.75, at(0.0), tolerance, L"starts at init - amplitude");
		Assert::AreEqual(1.0, at(0.5), tolerance, L"halfway through forward");
		Assert::AreEqual(1.25, at(1.5), tolerance, L"holding at init + amplitude");
		Assert::AreEqual(1.0, at(2.5), tolerance, L"halfway through backward");
		Assert::AreEqual(0.75, at(3.5), tolerance, L"holding at init - amplitude");
	}

	TEST_METHOD(ContinuousPhaseIsInSeconds)
	{
		// init + (t + phase) * amplitude = 1 + 0.5 * 2
		Assert::AreEqual(2.0, valueAtStart(ANContinuous1D{ "radius", 2.0, 0.5 }, 1.0), tolerance);
	}
};

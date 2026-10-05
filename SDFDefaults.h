#pragma once
#include <vector>
#include "Vec2.h"
#include "Vec3.h"

namespace DEF
{
	// Combo defaults
	inline static double	COMBSmooth{ 0.5 };
	inline static double	COMBColBlend{ 1.0 };
	inline static double	COMBColShift{ 0.0 };

	// Distortion defaults
	inline static double	DISTMagnitude{ 1.0 };

	// Domain defaults
	inline static Vec3		DOMSpacing{ 1.0 };
	inline static Vec3		DOMLimits{ 1.0 };

	// Resurface defaults
	inline static double	RESURFRoundMagnitude{ 0.01 };
	inline static double	RESURFOnionMagnitude{ 0.01 };

	// Transform defaults
	inline static Vec3		TRANSVector{ 0.0, 1.0, 0.0 };
	inline static double	TRANSAmplitude{ 1.0 };
	inline static double	TRANSPeriod{ 1.0 };
	inline static double	TRANSPhase{ 0.0 };
	inline static double	TRANSTransTimes{ 1.0 };
	inline static double	TRANSEndDelays{ 0.0 };
	inline static double	TRANSCubeHeight{ 1.0 };
	inline static double	TRANSRotations{ 0.5 };
	inline static double	TRANSRotsPerSec{ 0.25 };
	inline static double	TRANSRotateTime{ 1.0 };
	inline static double	TRANSDelayTime{ 1.0 };
	inline static double	TRANSQuarterTurnTime{ 1.0 };
	inline static double	TRANSElongateLength{ 1.0 };
	inline static double	TRANSElongateOffset{ 0.0 };
	inline static std::vector<Vec3>	TRANSCubeRotAxes{ Vec3::Forward, Vec3::Right, Vec3::Backward, Vec3::Left };

	// Prop Mod defaults
	inline static double	PMPeriod{ 1.0 };
	inline static double	PMAmplitude{ 1.0 };
	inline static double	PMPhase{ 0.0 };

	// LightSource defaults
	inline static Vec3		LSPosition{ 3.0, 4.0, -4.0 };
	inline static double	LSLuminance{ 1.0 };
	inline static double	LSAmbientPower{ 0.15 };
	inline static Color		LSColor{ Color::White };
};


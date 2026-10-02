#pragma once
#include <vector>
#include "Vec2.h"
#include "Vec3.h"

namespace DEF
{
	// SDFObject defaults
	inline static Color		OBJColor{ 1.0 };
	inline static double	OBJPlanePos{ 0.0 };
	inline static Vec3		OBJBoxDimensions{ 1.0 };
	inline static double	OBJFrameThickness{ 0.1 };
	inline static double	OBJRadius{ 0.5 };
	inline static Vec2		OBJRadii{ 1.0, 0.25 };
	inline static Vec3		OBJRadiiV3{ 1.0, 0.5, 0.25 };
	inline static double	OBJLength{ 1.0 };
	inline static double	OBJRatio{ 0.125 };
	inline static Vec3		OBJNormal{ 0.0, 1.0, 0.0 };
	inline static double	OBJOffset{ 0.0 };
	inline static double	OBJSweep{ 0.25 };
	inline static Vec2		OBJOP2DDimensions{ 1.0, 0.5 };
	inline static double	OBJOP2DMagnitude{ 2.0 };
	inline static size_t	OBJOP2DNumPoints{ 5 };
	inline static Vec2		OBJOP2DTPoint1{ -1.0, 0.0 };
	inline static Vec2		OBJOP2DTPoint2{ 0.0, 1.0 };
	inline static Vec2		OBJOP2DTPoint3{ 1.0, 0.0 };

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
	inline static Vec3		TRANSVector{ Vec3::Up };
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


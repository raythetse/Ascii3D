#pragma once
#include <sstream>
#include <string>
#include <vector>
#include "LightSource.h"
#include "SDFObjectInc.h"

class GLSLMarcher
{
public:
	static const std::string s_shaderLayout;
	static const std::string s_shaderFunctions;
	static const std::string s_shaderMainFunction;
	static const std::string s_shaderMainEnd;
	static void buildSampleFunction(std::stringstream& ss, const std::unique_ptr<SDFObject>& baseObject);
	static void buildLightPasses(std::stringstream& ss, const std::vector<std::unique_ptr<LightSource>>& lightSources);
};


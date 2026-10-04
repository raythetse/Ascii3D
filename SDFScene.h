#pragma once
#include <memory>
#include <ranges>
#include <string>
#include <vector>
#include "SDFObject.h"
#include "LightSource.h"

class SDFBuilder;

class SDFScene
{
private:
	static const std::string m_shaderLayout;
	static const std::string m_shaderFunctions;
	static const std::string m_shaderMainFunction;
	static const std::string m_shaderMainEnd;
public:
	enum LoadOption {
		Test,
		Exclusion,
		WalkingCubes,
		Elongate,
	};

private:
	std::unique_ptr<SDFObject> m_baseObject{};
	std::vector<std::unique_ptr<LightSource>> m_lights{};
	void test();
	void excludeDemo();
	void cubesDemo();
	void elongateDemo();
	void torusDemo();
	void buildBart();
	void buildSampleFunction(std::stringstream& ss) const;
	void buildLightPasses(std::stringstream& ss) const;
public:
	SDFScene() = default;
	SDFScene(LoadOption loadOption);
	SDFScene(std::string fileName);
	SDFScene(SDFBuilder& obj);
	SDFScene(SDFBuilder&& obj);
	void add(SDFBuilder& obj);
	void add(SDFBuilder&& obj);
	void update();
	SDFObject* baseObject() { return m_baseObject.get(); }
	std::vector<std::unique_ptr<LightSource>>& lights() { return m_lights; }
	bool serialize(std::vector<std::string>& strVec) const;
	std::string buildComputeShader() const;
};


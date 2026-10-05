#pragma once
#include <memory>
#include <ranges>
#include <string>
#include <vector>
#include "SDFBuilder.h"
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
private:
	std::unique_ptr<SDFObject> m_baseObject{};
	std::vector<std::unique_ptr<LightSource>> m_lights{};
	void buildSampleFunction(std::stringstream& ss) const;
	void buildLightPasses(std::stringstream& ss) const;
protected:
	virtual void buildScene() {};
	void add(SDFBuilder& obj);
	void add(SDFBuilder&& obj);
public:
	SDFScene() = default;
	SDFScene(std::string fileName);
	void update();
	SDFObject* baseObject() { return m_baseObject.get(); }
	std::vector<std::unique_ptr<LightSource>>& lights() { return m_lights; }
	bool serialize(std::vector<std::string>& strVec) const;
	std::string buildComputeShader() const;
};


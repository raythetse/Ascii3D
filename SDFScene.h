#pragma once
#include <memory>
#include <ranges>
#include <string>
#include <vector>
#include "SDFBuilder.h"
#include "SDFObjectInc.h"
#include "LightSource.h"

class SDFBuilder;

// A scene contains a root object for an object tree and the lights
class SDFScene
{
private:
	std::unique_ptr<SDFObject> m_baseObject{};
	std::vector<std::unique_ptr<LightSource>> m_lights{};
	std::vector<std::string> m_loadWarnings{};
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
	const std::vector<std::string>& loadWarnings() const { return m_loadWarnings; }
	bool serialize(std::vector<std::string>& strVec) const;
	std::string buildComputeShader() const;
};


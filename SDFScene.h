#pragma once
#include <memory>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>
#include "SDFBuilder.h"
#include "SDFObjectInc.h"
#include "LightSource.h"

class SDFBuilder;

class SDFScene
{
private:
	std::unique_ptr<std::unordered_map <std::string, std::vector<std::string>>> m_branchMap;
	std::unique_ptr<SDFObject> m_baseObject{};
	std::vector<std::unique_ptr<LightSource>> m_lights{};
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


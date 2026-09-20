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
	static const std::string m_shaderMainBegin;
	static const std::string m_shaderMainEnd;
public:
	enum LoadOption {
		Test,
		Cubes,
		Elongate,
		Frame,
		Oxygen,
		PropMod,
		Torus,
	};

private:
	std::unique_ptr<SDFObject> m_baseObject{};
	std::vector<std::unique_ptr<LightSource>> m_lights{};
	void test();
	void frameDemo();
	void oxygenDemo();
	void cubesDemo();
	void propModDemo();
	void elongateDemo();
	void torusDemo();
	void propModTests();
public:
	SDFScene() = default;
	SDFScene(LoadOption loadOption);
	SDFScene(std::string fileName);
	SDFScene(SDFBuilder& obj);
	SDFScene(SDFBuilder&& obj);
	void add(SDFBuilder& obj);
	void add(SDFBuilder&& obj);
	void update();
	SDFObject* object() { return m_baseObject.get(); }
	std::vector<std::unique_ptr<LightSource>>& lights() { return m_lights; }
	bool serialize(std::vector<std::string>& strVec) const;
	std::string getSceneShader() const;
};


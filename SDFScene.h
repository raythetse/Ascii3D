#pragma once
#include <memory>
#include <ranges>
#include <vector>
#include "SDFObject.h"
#include "LightSource.h"

class SDFBuilder;

class SDFScene
{
public:
	enum LoadOption {
		Test,
		Cubes,
		Elongate,
		Frame,
		Oxygen,
		PropMod,
	};

private:
	std::vector<std::unique_ptr<SDFObject>> m_objects{};
	std::vector<std::unique_ptr<LightSource>> m_lights{};
	void test();
	void frameDemo();
	void oxygenDemo();
	void cubesDemo();
	void propModDemo();
	void elongateDemo();
	void propModTests();
public:
	SDFScene() = default;
	SDFScene(LoadOption loadOption);
	SDFScene(SDFBuilder& obj);
	SDFScene(SDFBuilder&& obj);
	void add(SDFBuilder& obj);
	void add(SDFBuilder&& obj);
	void update();
	std::vector<std::unique_ptr<SDFObject>>& objects() { return m_objects; }
	std::vector<std::unique_ptr<LightSource>>& lights() { return m_lights; }
	bool serialize(std::vector<std::string>& strVec) const;
};


#pragma once
#include <memory>
#include <ranges>
#include <vector>
#include "SDFBuilder.h"
#include "SDFObject.h"
#include "LightSource.h"

class SDFScene
{
public:
	enum LoadOption {
		Cubes,
		Elongate,
		Frame,
		Oxygen,
		PropMod,
	};

private:
	std::vector<std::unique_ptr<SDFObject>> m_objects{};
	std::vector<std::unique_ptr<LightSource>> m_lights{};
	void add(SDFBuilder& obj);
	void add(SDFBuilder&& obj);
	void frameDemo();
	void oxygenDemo();
	void cubesDemo();
	void propModDemo();
	void elongateDemo();
	void propModTests();
public:
	SDFScene() = default;
	SDFScene(LoadOption loadOption);
	SDFScene(std::string_view fileName);
	void update();
	std::vector<std::unique_ptr<SDFObject>>& objects() { return m_objects; }
	std::vector<std::unique_ptr<LightSource>>& lights() { return m_lights; }
};


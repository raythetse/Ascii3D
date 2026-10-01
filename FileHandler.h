#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace FileHandler
{
	inline void saveShader(std::string& str, std::string fileName) {
		std::ofstream ofs{ fileName };
		ofs << str;
		ofs.close();
	}

	inline void saveScene(std::vector<std::string>& strVec, std::string fileName)
	{
		std::ofstream ofs{ fileName };
		for (auto& str : strVec) {
			ofs << str << '\n';
		}
		ofs.close();
	}

	inline void readSDFScene(std::vector<std::string>& strVec, std::string fileName) 
	{
		namespace fs = std::filesystem;
		fs::path path{ ".\\CustomScenes\\" + fileName };

		std::ifstream ifs{ path };
		std::string line{};
		while (std::getline(ifs, line)) {
			strVec.push_back(line);
		}
	}

	inline void enumerateCustomScenes(std::vector<std::string>& files)
	{
		namespace fs = std::filesystem;
		fs::path path{ ".\\CustomScenes" };

		if (!fs::exists(path)) {
			fs::create_directory(path);
			return;
		}

		for (const auto& file : fs::directory_iterator(path)) {
			if (file.is_regular_file()) {
				if (file.path().extension() == ".scn") {
					files.push_back(file.path().filename().string());
				}
			}
		}
	}
};


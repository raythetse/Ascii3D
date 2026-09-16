#pragma once
#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace FileHandler
{
	inline void saveSDFScene(std::vector<std::string>& strVec, std::string fileName)
	{
		std::ofstream ofs{ fileName };
		for (auto& str : strVec) {
			ofs << str << '\n';
		}
		ofs.close();
	}

	inline void readSDFScene(std::vector<std::string>& strVec, std::string fileName) 
	{
		std::ifstream ifs{ fileName };
		std::string line{};
		while (std::getline(ifs, line)) {
			strVec.push_back(line);
		}
	}
};


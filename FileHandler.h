#pragma once
#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace FileHandler
{
	inline void saveToFile(std::string& str, std::string fileName) {
		std::ofstream ofs{ fileName };
		ofs << str;
		ofs.close();
	}

	inline void saveToFile(std::vector<std::string>& strVec, std::string fileName)
	{
		std::ofstream ofs{ fileName };
		for (auto& str : strVec) {
			ofs << str << '\n';
		}
		ofs.close();
	}

	inline void saveToFile(std::stringstream& ss, std::string fileName) {
		std::ofstream ofs{ fileName };
		ofs << ss.str();
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


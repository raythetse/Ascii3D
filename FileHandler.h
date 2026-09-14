#pragma once
#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace FileHandler
{
	void saveSDFScene(std::vector<std::string>& strVec, std::string fileName)
	{
		std::ofstream ofs{ fileName };
		for (auto& str : strVec) {
			ofs << str << '\n';
		}
		ofs.close();
	}
	std::unique_ptr<std::vector<std::string>> readSDFScene(std::string fileName);
};


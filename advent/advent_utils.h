#pragma once

#include <fstream>
#include <sstream>
#include <cassert>
#include <string>
#include <string_view>
#include <iostream>
#include <filesystem>

namespace advent
{
	inline std::ifstream open_input(const std::string& filename)
	{
		auto result = std::ifstream{ filename };
		assert(result.is_open());
#ifndef NDEBUG
		if (std::filesystem::file_size(filename) <= 0)
		{
			std::cerr << "\nWARNING! File '" << filename << "' is empty.";
		}
#endif
		return result;
	}

	inline std::ifstream open_puzzle_input(int day)
	{
		std::ostringstream name;
		name << "advent" << day << "/advent" << day << ".txt";
		return open_input(name.str());
	}

	inline std::ifstream open_testcase_input(int day, char id)
	{
		std::ostringstream name;
		name << "advent" << day << "/testcase_" << id << ".txt";
		return open_input(name.str());
	}
}
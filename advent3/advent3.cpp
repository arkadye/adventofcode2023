#include "advent3.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY3DBG 1
#ifdef NDEBUG
#define DAY3DBG 0
#else
#define DAY3DBG ENABLE_DAY3DBG
#endif

#if DAY3DBG
	#include <iostream>
#endif

namespace
{
#if DAY3DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "coords.h"
#include "small_vector.h"
#include "sorted_vector.h"
#include "int_range.h"
#include "istream_line_iterator.h"
#include "transform_if.h"

#include <map>
#include <algorithm>
#include <ranges>

namespace
{
	using Coords = utils::coords;
	using Symbol = char;

	auto is_symbol(char c) noexcept
	{
		return !std::isdigit(c) && c != '.';
	}

	using SymbolMap = utils::flat_map<Coords, Symbol>;
	struct NumberLocation
	{
		int value = 0;
		utils::small_vector<Coords, 4> locations;
	};

	class Map
	{
		SymbolMap symbols;
		std::vector<NumberLocation> numbers;

		void add_symbol(const Coords& loc, Symbol s)
		{
			AdventCheck(is_symbol(s));
			AdventCheck(!symbols.contains_key(loc));
			symbols.insert_or_assign(loc, s);
		}

		void add_digit(const Coords& loc, char d)
		{
			AdventCheck(isdigit(d));
			const int as_value = d - '0';
			if (!numbers.empty())
			{
				NumberLocation& last_number = numbers.back(); // We may be altering this.
				const Coords& last_location = last_number.locations.back();
				if (last_location == loc + Coords::left()) // Update current number
				{
					last_number.value *= 10;
					last_number.value += (as_value);
					last_number.locations.push_back(loc);
					return;
				}
			}

			numbers.emplace_back(NumberLocation{ as_value , {loc} });
		}

		void add_char(const Coords& loc, char c)
		{
			if (isdigit(c)) add_digit(loc, c);
			else if (is_symbol(c)) add_symbol(loc, c);
		}

		void add_char(int row, int col, char c)
		{
			add_char(Coords{ col,row }, c);
		}

		void add_line(int row, std::string_view line)
		{
			for (int column : utils::int_range(static_cast<int>(line.size())))
			{
				const char c = line[column];
				add_char(row, column, c);
			}
		}

		bool number_has_neighbour(const NumberLocation& nl) const
		{
			AdventCheck(!nl.locations.empty());
			utils::small_vector<Coords, 16> locations_to_check;
			const std::size_t num_locations_to_check = 2 * nl.locations.size() + 6;
			if (num_locations_to_check > 16)
			{
				log << "\nWARNING: Number at " << nl.locations.front()
					<< " (" << nl.value << ") overruns locations array.";
			}
			locations_to_check.reserve(num_locations_to_check);
			locations_to_check.push_back(nl.locations.front() + Coords::left() + Coords::up());
			locations_to_check.push_back(nl.locations.front() + Coords::left() + Coords::down());
			locations_to_check.push_back(nl.locations.front() + Coords::left());
			locations_to_check.push_back(nl.locations.back() + Coords::right() + Coords::up());
			locations_to_check.push_back(nl.locations.back() + Coords::right() + Coords::down());
			locations_to_check.push_back(nl.locations.back() + Coords::right());

			for (const Coords& loc : nl.locations)
			{
				locations_to_check.push_back(loc + Coords::up());
				locations_to_check.push_back(loc + Coords::down());
			}

			auto has_symbol = [this](const Coords& c)
				{
					return symbols.contains_key(c);
				};

			const bool result = stdr::any_of(locations_to_check, has_symbol);
			return result;
		}

		utils::small_vector<int, 8> get_neighbouring_numbers(const Coords& location) const
		{
			const auto neighbours = location.neighbours_plus_diag();
			auto transform_fn = [&neighbours](const NumberLocation& nl)
				{
					auto find_fn = [&nl](const Coords& loc)
						{
							return stdr::find(nl.locations, loc) != end(nl.locations);
						};
					auto find_result = stdr::find_if(neighbours, find_fn);
					return (find_result != end(neighbours) ? nl.value : 0);
				};

			auto keep_fn = [](int val) { return val > 0; };

			utils::small_vector<int, 8> result;

			utils::ranges::transform_if_post(numbers, std::back_inserter(result), transform_fn, keep_fn);
			return result;
		}

		int get_gear_power(const Coords& gear_location) const
		{
			AdventCheck(symbols.at(gear_location) == '*');
			const auto neighbours = get_neighbouring_numbers(gear_location);
			return (neighbours.size() == 2u ? neighbours[0] * neighbours[1] : 0);
		}

	public:

		explicit Map(std::istream& input)
		{
			numbers.reserve(1200);
			symbols.reserve(800);
			int row = 0;
			for (std::string_view line : utils::istream_line_range{ input })
			{
				add_line(row++, line);
			}
		}

		int solve_p1() const
		{
			auto transform_fn = [this](const NumberLocation& nl)
				{
					return number_has_neighbour(nl) ? nl.value : 0;
				};
			return std::transform_reduce(begin(numbers), end(numbers), 0, std::plus<int>{}, transform_fn);
		}

		int64_t solve_p2() const
		{
			auto transform_fn = [this](const std::pair<const Coords, Symbol>& symbol)
				{
					return symbol.second == '*' ? get_gear_power(symbol.first) : 0;
				};
			return std::transform_reduce(begin(symbols), end(symbols), int64_t{ 0 }, std::plus<int64_t>{}, transform_fn);
		}
	};



	int solve_p1(std::istream& input)
	{
		const Map map{ input };
		return map.solve_p1();
	}
}

namespace
{
	int64_t solve_p2(std::istream& input)
	{
		const Map map{ input };
		return map.solve_p2();
	}
}

namespace
{
	std::istringstream get_testcase_input()
	{
		return std::istringstream{
		R"(467..114..
...*......
..35..633.
......#...
617*......
.....+.58.
..592.....
......755.
...$.*....
.664.598..)"
		};
	}
}

ResultType testase_three_p1()
{
	auto iss = get_testcase_input();
	return solve_p1(iss);
}

ResultType testase_three_p2()
{
	auto iss = get_testcase_input();
	return solve_p2(iss);
}

ResultType advent_three_p1()
{
	auto input = advent::open_puzzle_input(3);
	return solve_p1(input);
}

ResultType advent_three_p2()
{
	auto input = advent::open_puzzle_input(3);
	return solve_p2(input);
}

#undef DAY3DBG
#undef ENABLE_DAY3DBG
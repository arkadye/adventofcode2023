#include "advent21.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY21DBG 1
#ifdef NDEBUG
#define DAY21DBG 0
#else
#define DAY21DBG ENABLE_DAY21DBG
#endif

#if DAY21DBG
	#include <iostream>
#endif

namespace
{
#if DAY21DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "coords.h"
#include "int_range.h"
#include "sorted_vector.h"
#include "istream_line_iterator.h"

namespace
{
	using Coords = utils::coords;

	using PointCloud = utils::sorted_vector<Coords>;

	struct ParseResult
	{
		PointCloud garden_plots;
		Coords start_point;
	};

	constexpr const char WALL = '#';
	constexpr const char PLOT = '.';
	constexpr const char START = 'S';

	ParseResult parse_input(std::istream& input)
	{
		ParseResult result;
		result.garden_plots.reserve(131 * 131);
		Coords current{ 0,0 };
		for (std::string_view line : utils::istream_line_range{ input })
		{
			for (char c : line)
			{
				switch (c)
				{
				case WALL:
					break;
				case START:
					result.start_point = current;
					//[[fallthrough]]
				case PLOT:
					result.garden_plots.push_back(current);
					break;
				default:
					AdventUnreachable();
					break;
				}
				++current.x;
			}
			current.x = 0;
			++current.y;
		}
		return result;
	}

	PointCloud get_next_steps(const PointCloud& current_places, const PointCloud& plots)
	{
		PointCloud result;
		result.reserve(4 * current_places.size());
		for (const Coords& c : current_places)
		{
			AdventCheck(plots.contains(c));
			stdr::copy_if(c.neighbours(), std::back_inserter(result), [&plots](const Coords& n) {return plots.contains(n); });
		}
		result.unique();
		return result;
	}

	std::size_t solve_p1(std::istream& input, int num_steps)
	{
		const ParseResult parse_result = parse_input(input);
		PointCloud possible_plots{ parse_result.start_point };
		for (auto i : utils::int_range{ num_steps })
		{
			PointCloud next = get_next_steps(possible_plots, parse_result.garden_plots);
			possible_plots = std::move(next);
		}
		return possible_plots.size();
	}
}

namespace
{
	int solve_p2(std::istream& input)
	{
		return 0;
	}
}

ResultType testcase_twentyone_p1(std::istream& input)
{
	return solve_p1(input, 6);
}

ResultType advent_twentyone_p1()
{
	auto input = advent::open_puzzle_input(21);
	return solve_p1(input, 64);
}

ResultType advent_twentyone_p2()
{
	auto input = advent::open_puzzle_input(21);
	return solve_p2(input);
}

#undef DAY21DBG
#undef ENABLE_DAY21DBG
#include "advent18.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY18DBG 1
#ifdef NDEBUG
#define DAY18DBG 0
#else
#define DAY18DBG ENABLE_DAY18DBG
#endif

#if DAY18DBG
	#include <iostream>
#endif

namespace
{
#if DAY18DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "coords.h"
#include "sorted_vector.h"
#include "to_value.h"
#include "coords_iterators.h"
#include "parse_utils.h"
#include "istream_line_iterator.h"
#include "range_contains.h"

#include <algorithm>

namespace
{
	using Coords = utils::coords;
	using Dir = utils::direction;

	using PointCloud = utils::sorted_vector<Coords>;

	Dir to_direction(char c)
	{
		switch(c)
		{
		case 'U': return Dir::up;
		case 'D': return Dir::down;
		case 'L': return Dir::left;
		case 'R': return Dir::right;
		default: break;
		}
		AdventUnreachable();
	}

	Dir to_direction(std::string_view sv)
	{
		AdventCheck(sv.size() == 1);
		return to_direction(sv.front());
	}

	void add_points(PointCloud& result, utils::coords_iterators::elem_range<int> elems)
	{
		stdr::copy(elems, std::back_inserter(result));
	}

	Coords add_points(PointCloud& result, const Coords& from, Dir dir, int distance)
	{
		const Coords& to = from + distance * Coords::dir(dir);
		add_points(result, utils::coords_iterators::elem_range<int>::get_range(from,to));
		return to;
	}

	Coords add_points(PointCloud& result, const Coords& from,  std::string_view line)
	{
		auto [dir_str , dist_str] = utils::get_string_elements(line, 0, 1);
		const Dir dir = to_direction(dir_str);
		const int distance = utils::to_value<int>(dist_str);
		return add_points(result, from, dir, distance);
	}

	struct ParseResult
	{
		PointCloud points;
		Coords bottom_left;
		Coords top_right;
	};

	ParseResult parse_input(std::istream& input)
	{
		ParseResult result;
		Coords current_point{0,0};
		for(std::string_view line : utils::istream_line_range{input})
		{
			current_point = add_points(result.points, current_point, line);
			result.bottom_left.x = std::min(result.bottom_left.x, current_point.x);
			result.bottom_left.y = std::min(result.bottom_left.y, current_point.y);
			result.top_right.x = std::max(result.top_right.x, current_point.x);
			result.top_right.y = std::max(result.top_right.y, current_point.y);
		}
		return result;
	}

	PointCloud flood_fill(const Coords& bottom_left, const Coords& top_right, const PointCloud& blockers)
	{
		AdventCheck(bottom_left.x < top_right.x);
		AdventCheck(bottom_left.y < top_right.y);

		const int max_points = (top_right.y - bottom_left.y + 1) * (top_right.x - bottom_left.x + 1);
		auto is_on_grid = [&bottom_left,&top_right](const Coords& c)
		{
			return utils::range_contains_inc(c.x, bottom_left.x,top_right.x)
				&& utils::range_contains_inc(c.y, bottom_left.y, top_right.y);
		};

		PointCloud result;
		std::vector<Coords> points_to_check{ bottom_left,top_right,Coords{bottom_left.x,top_right.y}, Coords{top_right.x, bottom_left.y} };
		result.reserve(max_points);
		points_to_check.reserve(max_points);
		while(!points_to_check.empty())
		{
			const Coords point = points_to_check.back();
			points_to_check.pop_back();

			if(!is_on_grid(point)) continue;
			if(blockers.contains(point)) continue;
			const auto [insert_it , insert_success] = result.insert_unique(point);
			AdventCheck(static_cast<int64_t>(result.size()) <= max_points);
			if(!insert_success) continue;
			stdr::copy(point.neighbours(), std::back_inserter(points_to_check));
		}
		return result;
	}

	int solve_p1(std::istream& input)
	{
		const ParseResult parse_result = parse_input(input);
		const Coords bottom_left_border = parse_result.bottom_left - Coords{1,1};
		const Coords top_right_border = parse_result.top_right + Coords{1,1};
		const PointCloud outside = flood_fill(bottom_left_border, top_right_border, parse_result.points);
		const int max_points = (top_right_border.x - bottom_left_border.x + 1) * (top_right_border.y - bottom_left_border.y + 1);
		AdventCheck(static_cast<int64_t>(outside.size()) <= max_points);
		return max_points - outside.size();
	}
}

namespace
{
	int solve_p2(std::istream& input)
	{
		return 0;
	}
}

ResultType testcase_eighteen_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType testcase_eighteen_p2(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_eighteen_p1()
{
	auto input = advent::open_puzzle_input(18);
	return solve_p1(input);
}

ResultType advent_eighteen_p2()
{
	auto input = advent::open_puzzle_input(18);
	return solve_p2(input);
}

#undef DAY18DBG
#undef ENABLE_DAY18DBG
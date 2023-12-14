#include "advent13.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY13DBG 1
#ifdef NDEBUG
#define DAY13DBG 0
#else
#define DAY13DBG ENABLE_DAY13DBG
#endif

#if DAY13DBG
	#include <iostream>
#endif

namespace
{
#if DAY13DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "grid.h"

namespace
{
	enum class Tile : char
	{
		ash = '.',
		rock = '#'
	};

	Tile to_tile(char c)
	{
		AdventCheck(c == '.' || c == '#');
		return static_cast<Tile>(c);
	}

	using Grid = utils::grid<Tile>;

	Grid parse_grid(std::istream& input)
	{
		return utils::grid_helpers::build(input,to_tile);
	}

	int solve_p1(std::istream& input)
	{
		return 0;
	}
}

namespace
{
	int solve_p2(std::istream& input)
	{
		return 0;
	}
}

ResultType testcase_thirteen_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType testcase_thirteen_p2(std::istream& input)
{
	return solve_p1(input);
}

ResultType advent_thirteen_p1()
{
	auto input = advent::open_puzzle_input(13);
	return solve_p1(input);
}

ResultType advent_thirteen_p2()
{
	auto input = advent::open_puzzle_input(13);
	return solve_p2(input);
}

#undef DAY13DBG
#undef ENABLE_DAY13DBG
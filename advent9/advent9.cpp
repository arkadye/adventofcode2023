#include "advent9.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY9DBG 1
#ifdef NDEBUG
#define DAY9DBG 0
#else
#define DAY9DBG ENABLE_DAY9DBG
#endif

#if DAY9DBG
	#include <iostream>
#endif

namespace
{
#if DAY9DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "istream_line_iterator.h"
#include "string_line_iterator.h"
#include "to_value.h"
#include <vector>
#include <functional>
#include <algorithm>
#include <numeric>

namespace
{
	enum class Direction : char
	{
		foward, backward
	};

	template <Direction dir>
	int64_t get_next_value(const std::vector<int64_t>& vals)
	{
		AdventCheck(!vals.empty());
		if (std::ranges::all_of(vals, [](int64_t v) { return v == 0; }))
		{
			return 0;
		}

		std::vector<int64_t> differences;
		differences.reserve(vals.size() - 1);

		std::transform(begin(vals) + 1, end(vals), begin(vals), std::back_inserter(differences), std::minus<int64_t>{});

		const int64_t next_diff = get_next_value<dir>(differences);
		if constexpr (dir == Direction::foward)
		{
			return vals.back() + next_diff;
		}
		if constexpr (dir == Direction::backward)
		{
			return vals.front() - next_diff;
		}
		AdventUnreachable();
		return 0;
	}

	template <Direction dir>
	int64_t get_next_value(std::string_view line)
	{
		std::vector<int64_t> vals;
		std::ranges::transform(utils::string_line_range{ line , ' '}, std::back_inserter(vals), utils::to_value<int64_t>);
		return get_next_value<dir>(vals);
	}

	template <Direction dir>
	int64_t solve_generic(std::istream& input)
	{
		using ILI = utils::istream_line_iterator;
		return std::transform_reduce(ILI{ input }, ILI{}, int64_t{ 0 }, std::plus<int64_t>{},
			[](std::string_view sv) { return get_next_value<dir>(sv); });
	}

	int64_t solve_p1(std::istream& input)
	{
		return solve_generic<Direction::foward>(input);
	}

	int64_t solve_p2(std::istream& input)
	{
		return solve_generic<Direction::backward>(input);
	}
}

ResultType testcase_nine_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType testcase_nine_p2(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_nine_p1()
{
	auto input = advent::open_puzzle_input(9);
	return solve_p1(input);
}

ResultType advent_nine_p2()
{
	auto input = advent::open_puzzle_input(9);
	return solve_p2(input);
}

#undef DAY9DBG
#undef ENABLE_DAY9DBG
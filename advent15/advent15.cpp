#include "advent15.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY15DBG 1
#ifdef NDEBUG
#define DAY15DBG 0
#else
#define DAY15DBG ENABLE_DAY15DBG
#endif

#if DAY15DBG
	#include <iostream>
#endif

namespace
{
#if DAY15DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "istream_line_iterator.h"
#include <algorithm>
#include <numeric>

namespace
{
	constexpr std::size_t MULTIPLIER = 17;
	using Hash = std::uint8_t;

	Hash add_char_to_hash(Hash existing, char c)
	{
		AdventCheck(c != '\n');
		const Hash new_hash = MULTIPLIER * (existing + c);
		return new_hash;
	}

	Hash hash_string(std::string_view input)
	{
		return std::accumulate(begin(input), end(input), Hash{ 0 }, add_char_to_hash);
	}

	std::size_t solve_p1(std::istream& input)
	{
		using ILI = utils::istream_line_iterator;
		return std::transform_reduce(ILI{ input,',' }, ILI{}, std::size_t{ 0 }, std::plus<std::size_t>{}, hash_string);

	}
}

namespace
{
	int solve_p2(std::istream& input)
	{
		return 0;
	}
}

ResultType testcase_fifteen_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType advent_fifteen_p1()
{
	auto input = advent::open_puzzle_input(15);
	return solve_p1(input);
}

ResultType advent_fifteen_p2()
{
	auto input = advent::open_puzzle_input(15);
	return solve_p2(input);
}

#undef DAY15DBG
#undef ENABLE_DAY15DBG
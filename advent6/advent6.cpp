#include "advent6.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY6DBG 1
#ifdef NDEBUG
#define DAY6DBG 0
#else
#define DAY6DBG ENABLE_DAY6DBG
#endif

#if DAY6DBG
	#include <iostream>
#endif

namespace
{
#if DAY6DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "small_vector.h"
#include "parse_utils.h"
#include "to_value.h"
#include "string_line_iterator.h"
#include "isqrt.h"

#include <algorithm>
#include <cmath>

namespace
{
	struct RaceDetails
	{
		int64_t time = 0;
		int64_t distance = 0;
	};

	using RaceList = utils::small_vector<RaceDetails,4>;

	RaceList parse_input_p1(std::istream& input)
	{
		RaceList result;
		std::string current_line;
		std::getline(input,current_line);
		std::string_view times = utils::remove_specific_prefix(current_line, "Time:");
		auto make_time = [](std::string_view time_str)
		{
			RaceDetails result;
			result.time = utils::to_value<int64_t>(time_str);
			return result;
		};
		using SLR = utils::string_line_range;
		std::ranges::transform(SLR{times,' '},std::back_inserter(result),make_time);
		std::getline(input,current_line);
		std::string_view distances = utils::remove_specific_prefix(current_line, "Distance:");
		auto add_distance = [](std::string_view distance_str, RaceDetails& detail)
		{
			detail.distance = utils::to_value<int64_t>(distance_str);
			return detail;
		};

		using SLI = utils::string_line_iterator;
		std::ranges::transform(SLR{distances,' '}, result, begin(result), add_distance);
		return result;
	}

	RaceDetails parse_input_p2(std::istream& input)
	{
		auto add_digit = [](int64_t total, char c)
		{
			return std::isdigit(c) ? (10 * total + c - '0') : total;
		};
		auto parse_line = [&add_digit](std::string_view line)
		{
			return std::accumulate(begin(line),end(line),int64_t{0},add_digit);
		};

		RaceDetails result;

		std::string current_line;
		std::getline(input,current_line);
		std::string_view time_str = utils::remove_specific_prefix(current_line,"Time:");
		result.time = parse_line(time_str);

		std::getline(input,current_line);
		std::string_view distance_str = utils::remove_specific_prefix(current_line, "Distance:");
		result.distance = parse_line(distance_str);
		return result;
	}

	/*
	SOLUTION NOTES
	For a time available of T and a press length of P the total distance moved D is:
		D = (T-P) * P.
	This can be rewritten as a quadratic equation in P in the form:
		aP^2 + bP + c = 0 with a=-1, b=T, C=-D

	Using the general form to solve this:
		x = (-b +/- sqrt(b^2-4ac)) / 2a

	Substitute in:
		P = (T +/- sqrt(T^2 - 4D)) / 2

	Call sqrt(T^2 - 4D) = K for ease.

	So we have two points where the press meets the correct distance. The winning presses are integers between these points.
		Pa = (T + K) / 2
		Pb = (T - K) / 2

	To get these we take floor(Pa) and floor(Pb), and then floor(Pb) - floor(Pa) is the result.
	*/

	int64_t get_num_winning_presses(const RaceDetails& rd)
	{
		const int64_t t = rd.time;
		const int64_t d = rd.distance;
		const int64_t determinant = t * t - 4 * d;
		const bool is_square = utils::is_square(determinant);
		const double range = std::sqrt(static_cast<double>(determinant));
		const double winning_range_bottom = (static_cast<double>(t) - range) / 2.0;
		const double winning_range_top = (static_cast<double>(t) + range) / 2.0;
		const int64_t result = static_cast<int64_t>(winning_range_top) - static_cast<int64_t>(winning_range_bottom) - (is_square ? 1 : 0);
		return result;
	}

	int solve_p1(const RaceList& rl)
	{
		return std::transform_reduce(begin(rl),end(rl),int64_t{1}, std::multiplies<int64_t>{}, get_num_winning_presses);
	}

	int solve_p1(std::istream& input)
	{
		const RaceList rl = parse_input_p1(input);
		return solve_p1(rl);
	}
}

namespace
{
	int64_t solve_p2(std::istream& input)
	{
		const RaceDetails rd = parse_input_p2(input);
		return get_num_winning_presses(rd);
	}
}

ResultType testcase_six_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType testcase_six_p2(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_six_p1()
{
	auto input = advent::open_puzzle_input(6);
	return solve_p1(input);
}

ResultType advent_six_p2()
{
	auto input = advent::open_puzzle_input(6);
	return solve_p2(input);
}

#undef DAY6DBG
#undef ENABLE_DAY6DBG
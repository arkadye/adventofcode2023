#include "advent2.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY2DBG 1
#ifdef NDEBUG
#define DAY2DBG 0
#else
#define DAY2DBG ENABLE_DAY2DBG
#endif

#if DAY2DBG
	#include <iostream>
#endif

namespace
{
#if DAY2DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "string_line_iterator.h"
#include "parse_utils.h"
#include "to_value.h"
#include "istream_line_iterator.h"
#include "int_range.h"

#include <array>
#include <cstdint>
#include <algorithm>
#include <ranges>
#include <numeric>

namespace
{
	using BallCount = uint_fast8_t;
	enum Color : uint8_t
	{
		red, green, blue, NUM
	};

	Color to_color(std::string_view in)
	{
		AdventCheck(!in.empty());
		switch (in.front())
		{
		case 'r':
			AdventCheck(in == "red");
			return Color::red;
		case 'g':
			AdventCheck(in == "green");
			return Color::green;
		case 'b':
			AdventCheck(in == "blue");
			return Color::blue;
		default:
			break;
		}
		AdventUnreachable();
		return Color::NUM;
	}

	class BallSet
	{
		std::array<BallCount, static_cast<int>(Color::NUM)> data;
	public:
		constexpr BallSet() : data{ 0,0,0 } {}
		constexpr BallSet(Color a, BallCount count_a, Color b, BallCount count_b, Color c, BallCount count_c) noexcept
		{
			(*this)[a] = count_a;
			(*this)[b] = count_b;
			(*this)[c] = count_c;
		}
		constexpr BallCount& operator[](Color idx) noexcept { return data[static_cast<int>(idx)]; }
		constexpr const BallCount& operator[](Color idx) const noexcept { return data[static_cast<int>(idx)]; }
		constexpr int get_power() const
		{
			return std::reduce(begin(data), end(data), 1, std::multiplies<int>{});
		}
		void set_minima(const BallSet& other)
		{
			for (auto i : utils::int_range{ data.size() })
			{
				data[i] = std::max<BallCount>(data[i],other.data[i]);
			}
		}
	};

	constexpr BallSet ball_set_p1{ Color::red, 12, Color::green, 13, Color::blue, 14 };

	bool check_individual_color(const BallSet& valid_set, std::string_view description)
	{
		const auto [num_str, color_str] = utils::split_string_at_first(description, ' ');
		const auto num_balls = utils::to_value<BallCount>(num_str);
		const Color color = to_color(color_str);
		return num_balls <= valid_set[color];
	}

	bool check_draw(const BallSet& valid_set, std::string_view draw_description)
	{
		using Range = utils::string_line_range;
		auto check_fn = [&valid_set](std::string_view description)
		{
			return check_individual_color(valid_set, description);
		};
		return std::ranges::all_of(Range{draw_description, ','}, check_fn);
	}

	bool check_game(const BallSet& valid_set, std::string_view all_draws)
	{
		using Range = utils::string_line_range;
		auto check_fn = [&valid_set](std::string_view draw_description)
		{
			return check_draw(valid_set, draw_description);
		};

		return std::ranges::all_of(Range{ all_draws,';' }, check_fn);
	}

	int get_game_val(const BallSet& valid_set, std::string_view line)
	{
		auto [game_id, draw_report] = utils::split_string_at_first(line, ':');
		const bool is_valid = check_game(valid_set, draw_report);
		if (!is_valid) return 0;
		std::string_view game_id_num = utils::remove_specific_prefix(game_id, "Game ");
		return utils::to_value<int>(game_id_num);
	}

	int solve_p1_generic(const BallSet& valid_set, std::istream& input)
	{
		using ILI = utils::istream_line_iterator;
		auto trans_fn = [&valid_set](std::string_view line)
			{
				return get_game_val(valid_set, line);
			};
		return std::transform_reduce(ILI{ input }, ILI{}, 0,  std::plus<int>{}, trans_fn);
	}

	int solve_p1(std::istream& input)
	{
		return solve_p1_generic(ball_set_p1, input);
	}
}

namespace
{
	BallSet get_from_draw(std::string_view draw)
	{
		auto combine_fn = [](BallSet balls, std::string_view desc)
			{
				auto [num_str, color_str] = utils::split_string_at_first(desc, ' ');
				const auto num = utils::to_value<BallCount>(num_str);
				const Color color = to_color(color_str);
				balls[color] = num;
				return balls;
			};

		using SLI = utils::string_line_iterator;
		return std::accumulate(SLI{ draw ,','}, SLI{}, BallSet{}, combine_fn);
	}

	BallSet get_from_game(std::string_view game)
	{
		auto combine_fn = [](BallSet balls, std::string_view draw)
			{
				const BallSet next_draw = get_from_draw(draw);
				balls.set_minima(next_draw);
				return balls;
			};
		using SLI = utils::string_line_iterator;
		return std::accumulate(SLI{ game , ';'}, SLI{}, BallSet{}, combine_fn);
	}

	int get_line_power(std::string_view line)
	{
		auto [id, game] = utils::split_string_at_first(line, ':');
		const BallSet set = get_from_game(game);
		return set.get_power();
	}

	int64_t solve_p2(std::istream& input)
	{
		using ILI = utils::istream_line_iterator;
		return std::transform_reduce(ILI{ input }, ILI{}, int{ 0 }, std::plus<int>{}, get_line_power);
	}
}

ResultType testcase_two_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType testcase_two_p2(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_two_p1()
{
	auto input = advent::open_puzzle_input(2);
	return solve_p1(input);
}

ResultType advent_two_p2()
{
	auto input = advent::open_puzzle_input(2);
	return solve_p2(input);
}

#undef DAY2DBG
#undef ENABLE_DAY2DBG
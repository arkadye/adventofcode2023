#include "advent4.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY4DBG 1
#ifdef NDEBUG
#define DAY4DBG 0
#else
#define DAY4DBG ENABLE_DAY4DBG
#endif

#if DAY4DBG
	#include <iostream>
#endif

namespace
{
#if DAY4DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "parse_utils.h"
#include "sorted_vector.h"
#include "trim_string.h"
#include "to_value.h"
#include "istream_line_iterator.h"
#include "int_range.h"

#include <algorithm>

namespace
{
	using Number = uint8_t;
	using NumberSet = utils::sorted_vector<Number,std::less<Number>,25>;

	NumberSet get_numbers(std::string_view numbers)
	{
		NumberSet result;
		while(!numbers.empty())
		{
			if(std::isspace(numbers.front()))
			{
				numbers.remove_prefix(1);
				continue;
			}
			auto [this_num , rest] = utils::split_string_at_first(numbers,' ');
			const Number num = utils::to_value<Number>(this_num);
			result.push_back(num);
			numbers = rest;
		}
		return result;
	}

	int get_score(int num_matches)
	{
		switch(num_matches)
		{
		case 0:
			return 0;
		case 1:
			return 1;
		default:
			break;
		}
		return 2 * get_score(num_matches -1);
	}

	int get_num_matching_numbers(std::string_view numbers)
	{
		auto [winning_numbers_str, my_numbers_str] = utils::split_string_at_first(numbers, " | ");
		const NumberSet winning_numbers = get_numbers(winning_numbers_str);
		const NumberSet my_number = get_numbers(my_numbers_str);

		NumberSet matching_numbers;
		stdr::set_intersection(winning_numbers, my_number, std::back_inserter(matching_numbers));
		return static_cast<int>(matching_numbers.size());
	}

	int score_card_p1(std::string_view card)
	{
		auto [prefix , numbers] = utils::split_string_at_first(card, ": ");
		const int num_matches = get_num_matching_numbers(numbers);
		return get_score(num_matches);
	}

	int solve_p1(std::istream& input)
	{
		using ILI = utils::istream_line_iterator;
		return std::transform_reduce(ILI{input},ILI{},0,std::plus<int>{},score_card_p1);
	}
}

namespace
{
	using CardAmounts = std::vector<int>;
	int get_amount(const CardAmounts& amounts, int id)
	{
		AdventCheck(id > 0);
		const std::size_t idx = static_cast<std::size_t>(id) - 1;
		return idx < amounts.size() ? amounts[idx] : 0;
	}

	CardAmounts add_cards(CardAmounts amounts, int id, int num)
	{
		AdventCheck(id > 0);
		const std::size_t idx = static_cast<std::size_t>(id) - 1;
		if(idx >= amounts.size())
		{
			amounts.resize(idx+1,num);
		}
		else
		{
			amounts[idx] += num;
		}
		return amounts;
	}

	int get_card_id(std::string_view card_prefix)
	{
		auto [dummy,idx_str] = utils::split_string_at_first(card_prefix, ' ');
		idx_str = utils::trim_string(idx_str);
		return utils::to_value<int>(idx_str);
	}

	CardAmounts score_card_p2(CardAmounts current_amounts, std::string_view card)
	{
		auto [prefix,numbers] = utils::split_string_at_first(card,": ");
		const int card_idx = get_card_id(prefix);
		current_amounts = add_cards(std::move(current_amounts),card_idx,1);

		const int multiplier = get_amount(current_amounts, card_idx);
		const int num_matches = get_num_matching_numbers(numbers);

		for(int i : utils::int_range(num_matches))
		{
			const int new_idx = card_idx + i + 1;
			current_amounts = add_cards(std::move(current_amounts), new_idx, multiplier);
		}
		return current_amounts;
	}

	int solve_p2(std::istream& input)
	{
		using ILI = utils::istream_line_iterator;
		CardAmounts amounts;
		amounts.reserve(220);

		amounts = std::accumulate(ILI{input},ILI{},std::move(amounts),score_card_p2);
		return std::reduce(begin(amounts),end(amounts),0,std::plus<int>{});
	}
}

ResultType testcase_four_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType testcase_four_p2(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_four_p1()
{
	auto input = advent::open_puzzle_input(4);
	return solve_p1(input);
}

ResultType advent_four_p2()
{
	auto input = advent::open_puzzle_input(4);
	return solve_p2(input);
}

#undef DAY4DBG
#undef ENABLE_DAY4DBG
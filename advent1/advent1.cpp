#include "advent1.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY1DBG 1
#ifdef NDEBUG
#define DAY1DBG 0
#else
#define DAY1DBG ENABLE_DAY1DBG
#endif

#if DAY1DBG
	#include <iostream>

#endif

namespace
{
#if DAY1DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "istream_line_iterator.h"
#include "int_range.h"

#include <numeric>
#include <array>
#include <algorithm>
#include <ranges>
#include <set>
#include "range_contains.h"

namespace
{
	using SubstrToDigitMap = std::array<std::set<std::string_view>,10>;
	const SubstrToDigitMap SUBSTR_TO_DIGIT_P1 = {
		std::set<std::string_view>{"0"},
		std::set<std::string_view>{"1"},
		std::set<std::string_view>{"2"},
		std::set<std::string_view>{"3"},
		std::set<std::string_view>{"4"},
		std::set<std::string_view>{"5"},
		std::set<std::string_view>{"6"},
		std::set<std::string_view>{"7"},
		std::set<std::string_view>{"8"},
		std::set<std::string_view>{"9"}
	};

	const SubstrToDigitMap SUBSTR_TO_DIGIT_P2 = {
		std::set<std::string_view>{"0"},
		std::set<std::string_view>{"1" , "one"},
		std::set<std::string_view>{"2" , "two"},
		std::set<std::string_view>{"3" , "three"},
		std::set<std::string_view>{"4" , "four"},
		std::set<std::string_view>{"5" , "five"},
		std::set<std::string_view>{"6" , "six"},
		std::set<std::string_view>{"7" , "seven"},
		std::set<std::string_view>{"8" , "eight"},
		std::set<std::string_view>{"9" , "nine"}
	};

	bool is_in_range(std::size_t digit)
	{
		return utils::range_contains_inc(digit, std::size_t{0}, std::size_t{9});
	}

	bool has_substr_at_index(std::string_view data, std::string_view substr, std::size_t data_index)
	{
		if(data.size() <= data_index) return false;
		const std::string_view data_view = data.substr(data_index,substr.size());
		return data_view == substr;
	}

	bool has_substr_at_index(std::string_view data, const std::set<std::string_view>& substr_candidates, std::size_t data_index)
	{
		return stdr::any_of(substr_candidates, [data,data_index](std::string_view substr){return has_substr_at_index(data,substr,data_index); });
	}

	std::size_t get_digit_at_location(std::string_view data, const SubstrToDigitMap& substr_map, std::size_t data_index)
	{
		auto find_pred = [data, data_index](const std::set<std::string_view>& substr_candidates)
		{
			return has_substr_at_index(data, substr_candidates, data_index);
		};

		const auto find_result = stdr::find_if(substr_map, find_pred);
		
		if(find_result == end(substr_map)) return std::numeric_limits<std::size_t>::max();

		const auto offset = std::distance(begin(substr_map), find_result);
		AdventCheck(is_in_range(offset));
		return static_cast<std::size_t>(offset);
	}

	int64_t get_first_of(std::string_view data, const SubstrToDigitMap& substr_map, const auto& index_range)
	{
		for(std::size_t idx : index_range)
		{
			const std::size_t digit = get_digit_at_location(data, substr_map, idx);
			if(is_in_range(digit))
			{
				return static_cast<int64_t>(digit);
			}
		}

		// None found so return 0
		return 0;
	}

	int64_t get_val_generic(std::string_view data, const SubstrToDigitMap& substr_map)
	{
		if(data.empty()) return 0;
		const utils::int_range<std::size_t> index_range{0, data.size()};
		const int64_t left_digit = get_first_of(data,substr_map,index_range);
		const int64_t right_digit = get_first_of(data,substr_map,stdr::views::reverse(index_range));
		const int64_t result = 10 * left_digit + right_digit;
		log << "\n" << data << " --> " << result;
		return result;
	}

	int64_t get_val_p1(std::string_view data)
	{
		return get_val_generic(data, SUBSTR_TO_DIGIT_P1);
	}

	int64_t solve_p1(std::istream& input)
	{
		using ILI = utils::istream_line_iterator;
		return std::transform_reduce(ILI{input},ILI{},int64_t{0},std::plus<int64_t>{},get_val_p1);
	}
}

namespace
{
	int64_t get_val_p2(std::string_view data)
	{
		return get_val_generic(data, SUBSTR_TO_DIGIT_P2);
	}

	int64_t solve_p2(std::istream& input)
	{
		using ILI = utils::istream_line_iterator;
		return std::transform_reduce(ILI{ input }, ILI{}, int64_t{ 0 }, std::plus<int64_t>{}, get_val_p2);
	}
}

ResultType testcase_one_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType testcase_one_p2(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_one_p1()
{
	auto input = advent::open_puzzle_input(1);
	return solve_p1(input);
}

ResultType advent_one_p2()
{
	auto input = advent::open_puzzle_input(1);
	return solve_p2(input);
}

#undef DAY1DBG
#undef ENABLE_DAY1DBG
#include "advent12.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY12DBG 1
#ifdef NDEBUG
#define DAY12DBG 0
#else
#define DAY12DBG ENABLE_DAY12DBG
#endif

#if DAY12DBG
	#include <iostream>
#endif

namespace
{
#if DAY12DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "small_vector.h"
#include "string_line_iterator.h"
#include "istream_line_iterator.h"
#include "to_value.h"
#include "int_range.h"
#include "parse_utils.h"

#include <string_view>
#include <algorithm>
#include <map>

namespace
{
	using NumType = uint8_t;
	enum class SpringState : char
	{
		operational = '.',
		damaged = '#',
		unknown = '?'
	};

	SpringState to_spring_state(char c)
	{
		AdventCheck(std::string_view{"#.?"}.find(c) != std::string_view::npos);
		return static_cast<SpringState>(c);
	}

	using SpringSet = utils::small_vector<SpringState,256>;
	using NumSet = utils::small_vector<NumType,128>;

	struct LineDescriptor
	{
		SpringSet springs;
		NumSet nums;
	};

	template <typename Container>
	Container unfold(Container input, int fold)
	{
		const std::size_t num_elems = input.size();
		for(auto i : utils::int_range{fold-1})
		{
			std::copy(begin(input),begin(input) + num_elems, std::back_inserter(input));
		}
		return input;
	}

	SpringSet parse_springs(std::string_view springs, std::size_t fold)
	{
		SpringSet result;
		const auto initial_capacity = result.capacity();
		result.reserve(fold * springs.size());
		bool previous_was_working = true; // Set this to true to ignore leading working springs.
		for(std::size_t i : utils::int_range{fold * springs.size()})
		{
			const std::size_t idx = i % springs.size();
			const char c = springs[idx];
			if(i != 0u && idx == 0u)
			{
				result.push_back(SpringState::unknown);
				previous_was_working = false;
			}
			const SpringState spring = to_spring_state(c);
			switch(spring)
			{
			case SpringState::damaged:
			case SpringState::unknown:
				result.push_back(spring);
				previous_was_working = false;
				break;
			case SpringState::operational:
				if(!previous_was_working)
				{
					result.push_back(spring);
				}
				previous_was_working = true;
				break;
			}
		}

		if(result.size() > initial_capacity)
		{
			log << "\nWarning: string_set had to expand to hold " << result.size() << " values.";
		}
		return result;
	}

	NumSet parse_nums(std::string_view nums, std::size_t fold)
	{
		NumSet result;
		const auto initial_capacity = result.capacity();
		result.reserve(fold * nums.size());
		for(std::size_t i : utils::int_range{fold})
		{
			stdr::transform(utils::string_line_range{nums,','},  std::back_inserter(result),utils::to_value<int8_t>);
		}
		if (result.size() > initial_capacity)
		{
			log << "\nWarning: num_set had to expand to hold " << result.size() << " values.";
		}
		return result;
	}

	LineDescriptor parse_line(std::string_view line, std::size_t fold)
	{
		const auto [springs,nums] = utils::split_string_at_first(line,' ');
		return LineDescriptor{ parse_springs(springs, fold) , parse_nums(nums, fold) };
	}

	struct MemoEntry
	{
		std::size_t spring_idx;
		std::size_t num_idx;
		NumType range_in_progress;
		auto operator<=>(const MemoEntry&) const noexcept = default;
	};

	using Memo = std::map<MemoEntry,std::size_t>;

	std::size_t count_possible_arrangements(Memo& memo, NumType partial_damaged_range_in_progress, SpringState next_spring, std::size_t spring_idx, const SpringSet& springs, std::size_t num_idx, const NumSet& nums)
	{
		MemoEntry memo_code{spring_idx,num_idx,partial_damaged_range_in_progress};
		const bool is_memoing = (next_spring == SpringState::unknown);
		if(is_memoing)
		{
			const auto memo_entry = memo.find(memo_code);
			if(memo_entry != end(memo))
			{
				return memo_entry->second;
			}
		}

		auto add_to_memo = [&memo,is_memoing,&memo_code](std::size_t val)
		{
			if(is_memoing)
			{
				memo[memo_code] = val;
			}
		};

		if(spring_idx == springs.size())
		{
			AdventCheck(next_spring == SpringState::unknown);
			if(num_idx == nums.size())
			{
				const std::size_t result = partial_damaged_range_in_progress == 0 ? 1 : 0;
				add_to_memo(result);
				return result;
			}
			const NumType next_num = nums[num_idx];
			if (next_num != partial_damaged_range_in_progress)
			{
				add_to_memo(0);
				return 0;
			}
			const std::size_t result = count_possible_arrangements(memo, 0, SpringState::unknown, spring_idx, springs, num_idx + 1, nums);
			add_to_memo(result);
			return result;
		}

		const NumType expected_num = (num_idx != nums.size()) ? nums[num_idx]: 0;
		if(partial_damaged_range_in_progress > expected_num)
		{
			add_to_memo(0);
			return 0;
		}

		const SpringState spring = springs[spring_idx];

		if(next_spring == SpringState::damaged)
		{
			AdventCheck(spring != SpringState::operational);
			const std::size_t result = count_possible_arrangements(memo, partial_damaged_range_in_progress + 1, SpringState::unknown, spring_idx + 1, springs, num_idx, nums);
			add_to_memo(result);
			return result;
		}
		if(next_spring == SpringState::operational)
		{
			AdventCheck(spring != SpringState::damaged);
			if(partial_damaged_range_in_progress == 0)
			{
				const std::size_t result = count_possible_arrangements(memo, partial_damaged_range_in_progress, SpringState::unknown, spring_idx + 1, springs, num_idx, nums);
				add_to_memo(result);
				return result;
			}
			if(partial_damaged_range_in_progress != expected_num)
			{
				add_to_memo(0);
				return 0;
			}
			const std::size_t result = count_possible_arrangements(memo, 0, SpringState::unknown, spring_idx + 1, springs, num_idx + 1, nums);
			add_to_memo(result);
			return result;
		}
		if(next_spring == SpringState::unknown)
		{
			if(spring != SpringState::unknown)
			{
				const std::size_t result = count_possible_arrangements(memo, partial_damaged_range_in_progress, spring, spring_idx, springs, num_idx, nums);
				add_to_memo(result);
				return result;
			}

			const std::size_t operational_result = count_possible_arrangements(memo, partial_damaged_range_in_progress, SpringState::operational, spring_idx, springs, num_idx , nums);
			const std::size_t damaged_result = count_possible_arrangements(memo, partial_damaged_range_in_progress, SpringState::damaged, spring_idx, springs, num_idx , nums);
			const std::size_t result = operational_result + damaged_result;
			add_to_memo(result);
			return result;
		}
		AdventUnreachable();
		return 0;
	}

	std::size_t count_possible_arrangements(const SpringSet& springs, const NumSet& nums)
	{
		Memo memo;
		return count_possible_arrangements(memo, 0, SpringState::unknown, 0u, springs, 0u, nums);
	}

	std::size_t count_possible_arrangements(const LineDescriptor& desc)
	{
		return count_possible_arrangements(desc.springs, desc.nums);
	}

	std::size_t count_possible_arrangements(std::string_view line, std::size_t fold)
	{
		const LineDescriptor ld = parse_line(line, fold);
		return count_possible_arrangements(ld);
	}

	std::size_t solve_generic(std::istream& input, std::size_t fold)
	{
		using ILI = utils::istream_line_iterator;
		auto transform_fn = [fold](std::string_view sv)
		{
			return count_possible_arrangements(sv,fold);
		};
		const std::size_t result = std::transform_reduce(ILI{input},ILI{},std::size_t{0},std::plus<std::size_t>{},transform_fn);
		return result;
	}

	std::size_t solve_p1(std::istream& input)
	{
		return solve_generic(input, 1u);
	}

	std::size_t solve_p2(std::istream& input)
	{
		return solve_generic(input,5u);
	}
}

ResultType testcase_twelve_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType testcase_twelve_p2(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_twelve_p1()
{
	auto input = advent::open_puzzle_input(12);
	return solve_p1(input);
}

ResultType advent_twelve_p2()
{
	auto input = advent::open_puzzle_input(12);
	return solve_p2(input);
}

#undef DAY12DBG
#undef ENABLE_DAY12DBG
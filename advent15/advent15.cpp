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
#include "small_vector.h"
#include "to_value.h"
#include "parse_utils.h"
#include "range_contains.h"
#include "int_range.h"

#include <algorithm>
#include <numeric>
#include <variant>
#include <array>

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
	using FocalLength = int8_t;

	struct InstructionBase
	{
		std::string label;
		Hash box_num = 0;
	};

	struct RemoveInstruction : InstructionBase {};

	struct ReplaceInstruction : InstructionBase
	{
		FocalLength focal_length = 0;
	};

	using Instruction = std::variant<RemoveInstruction,ReplaceInstruction>;

	Instruction parse_instruction(std::string_view sv)
	{
		std::string label;
		Hash label_hash = 0;
		while(!sv.empty())
		{
			const char c = sv.front();
			sv.remove_prefix(1);

			if(std::isalpha(c))
			{
				label.push_back(c);
				label_hash = add_char_to_hash(label_hash, c);
			}
			else if(c== '-')
			{
				RemoveInstruction ri;
				ri.label = std::move(label);
				ri.box_num = label_hash;
				return ri;
			}
			else if(c=='=')
			{
				ReplaceInstruction ri;
				ri.label = std::move(label);
				ri.box_num = label_hash;
				ri.focal_length = utils::to_value<FocalLength>(sv);
				AdventCheck(utils::range_contains_inc(ri.focal_length,1,9));
				return ri;
			}
			else
			{
				AdventUnreachable();
			}
		}
		AdventUnreachable();
		return RemoveInstruction{};
	}

	struct Lens
	{
		std::string label;
		FocalLength focal_length;
	};

	constexpr std::size_t ASSUMED_MAX_BOX_CONTENT = 6;
	constexpr std::size_t NUM_BOXES = 256;
	using Box = utils::small_vector<Lens,ASSUMED_MAX_BOX_CONTENT>;

	using State = std::array<Box,NUM_BOXES>;

	void update_state(State& state, const Instruction& instruction)
	{
		struct StateUpdater
		{
			State& s;
			Box& get_box(const InstructionBase& i)
			{
				return s[i.box_num];
			}
			auto get_lens_finder(const InstructionBase& i) const
			{
				return [&label=i.label](const Lens& lens) { return lens.label == label; };
			}
			void operator()(const RemoveInstruction& remove_instruction)
			{
				Box& box = get_box(remove_instruction);
				const auto remove_result = std::remove_if(begin(box),end(box), get_lens_finder(remove_instruction));
				box.erase(remove_result,end(box));
			}
			void operator()(const ReplaceInstruction& replace_instruction)
			{
				Box& box = get_box(replace_instruction);
				const auto lens_it = stdr::find_if(box, get_lens_finder(replace_instruction));
				if(lens_it != end(box))
				{
					lens_it->focal_length = replace_instruction.focal_length;
				}
				else
				{
					box.push_back(Lens{replace_instruction.label,replace_instruction.focal_length});
					if(box.size() > ASSUMED_MAX_BOX_CONTENT)
					{
						log << "\nWARNING: Heap allocation as box grows to size " << box.size();
					}
				}
			}
		};

		std::visit(StateUpdater{state}, instruction);
	}

	constexpr utils::int_range<std::size_t> get_ascending_range()
	{
		return utils::int_range{std::size_t{1}, std::numeric_limits<std::size_t>::max()};
	}

	std::size_t combine_with_ascending(const auto& container)
	{
		const auto range = utils::int_range{std::size_t{1}, std::numeric_limits<std::size_t>::max()};
		auto transform_fn = [](const auto& left, std::size_t right)
		{
			const std::size_t s = score(left);
			return s * right;
		};
		return std::transform_reduce(begin(container),end(container),begin(range),std::size_t{0},std::plus<std::size_t>{}, transform_fn);
	}

	std::size_t score(const Lens& lens)
	{
		return lens.focal_length;
	}

	std::size_t score(const Box& box)
	{
		return combine_with_ascending(box);
	}

	std::size_t score(const State& state)
	{
		return combine_with_ascending(state);
	}

	std::size_t solve_p2(std::istream& input)
	{
		State state;
		for(std::string_view instruction_str : utils::istream_line_range(input , ','))
		{
			const Instruction instruction = parse_instruction(instruction_str);
			update_state(state, instruction);
		}
		return score(state);
	}
}

ResultType testcase_fifteen_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType testcase_fifteen_p2(std::istream& input)
{
	return solve_p2(input);
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
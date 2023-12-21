#include "advent19.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY19DBG 1
#ifdef NDEBUG
#define DAY19DBG 0
#else
#define DAY19DBG ENABLE_DAY19DBG
#endif

#if DAY19DBG
	#include <iostream>
#endif

namespace
{
#if DAY19DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include <array>
#include <map>

#include "parse_utils.h"
#include "string_line_iterator.h"
#include "istream_block_iterator.h"
#include "to_value.h"
#include "small_vector.h"

namespace
{
	using TraitType = int;
	enum class PartTrait
	{
		x , m , a, s, NumTraits
	};

	constexpr std::size_t to_idx(PartTrait t) noexcept
	{
		const std::size_t result = static_cast<std::size_t>(t);
		return result;
	}

	struct Part
	{
	private:
		std::array<TraitType,to_idx(PartTrait::NumTraits)> traits;
	public:
		Part() : traits{} {}
		TraitType& operator[](PartTrait t) noexcept
		{
			const std::size_t idx = static_cast<std::size_t>(t);
			AdventCheck(idx < traits.size());
			return traits[idx];
		}
		TraitType operator[](PartTrait t) const noexcept
		{
			const std::size_t idx = static_cast<std::size_t>(t);
			AdventCheck(idx < traits.size());
			return traits[idx];
		}
	};

	Part parse_part(std::string_view line)
	{
		line = utils::remove_specific_prefix(line, '{');
		line = utils::remove_specific_suffix(line, '}');
		Part result;
		for(std::string_view trait_string : utils::string_line_range{line,','})
		{
			const auto [trait_id,trait_val] = utils::split_string_at_first(trait_string, '=');
			const TraitType val = utils::to_value<TraitType>(trait_val);
			const PartTrait trait = [trait_id]()
			{
				AdventCheck(trait_id.length() == 1u);
				const char id = trait_id.front();
				switch(id)
				{
				case 'x': return PartTrait::x;
				case 'm': return PartTrait::m;
				case 'a': return PartTrait::a;
				case 's': return PartTrait::s;
				default:
					AdventUnreachable();
					break;
				}
				return PartTrait::NumTraits;
			}();
			result[trait] = val;
		}
		return result;
	}

	enum class Operation
	{
		less_than,
		more_than
	};

	using LocationName = std::string;

	struct InstructionBase
	{
		PartTrait trait;
		Operation op;
		TraitType value;
	};

	struct UncompiledInstruction : InstructionBase
	{
		LocationName true_target;
		LocationName false_target;
	};

	using UncompiledProgram = std::map<LocationName,UncompiledInstruction>;

	struct PreprocessFragmentResult
	{
		std::variant<UncompiledProgram,LocationName> result;
		std::string_view remaining_line;
	};

	PreprocessFragmentResult preprocess_fragment(LocationName context, std::string_view fragment)
	{
		PreprocessFragmentResult result;
		const std::size_t find_result = fragment.find_first_of(":,");
		if(find_result >= fragment.size()) // We found a target name.
		{
			result.result = LocationName{fragment};
			return result;
		}

		result.remaining_line = fragment.substr(find_result+1);

		if(fragment[find_result] == ',') // Still a name
		{
			result.result = LocationName{fragment.substr(0, find_result)};
			return result;
		}
		AdventCheck(fragment[find_result] == ':');
		fragment = fragment.substr(0u, find_result);
		if()
	}

	struct Instruction : InstructionBase
	{
		std::size_t true_target;
		std::size_t false_target;
	};

	 preprocess_fragment(const LocationName& location, )

	std::map<LocationName,UncompiledInstruction> preprocess_line(std::string_view line)
	{
		
	}

	std::map<LocationName,UncompiledInstruction> preprocess_instructions(std::string_view all_instructions)
	{
		
	};

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

ResultType testcase_nineteen_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType testcase_nineteen_p2(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_nineteen_p1()
{
	auto input = advent::open_puzzle_input(19);
	return solve_p1(input);
}

ResultType advent_nineteen_p2()
{
	auto input = advent::open_puzzle_input(19);
	return solve_p2(input);
}

#undef DAY19DBG
#undef ENABLE_DAY19DBG
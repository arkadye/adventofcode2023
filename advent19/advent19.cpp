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
#include "istream_line_iterator.h"
#include "to_value.h"
#include "small_vector.h"
#include "combine_maps.h"
#include "int_range.h"
#include "enums.h"

namespace
{
	using TraitType = uint64_t;
	enum class PartTrait
	{
		x , m , a, s, NumTraits
	};

	constexpr const char* ACCEPT = "A";
	constexpr const char* REJECT = "R";
	constexpr const char* ENTRY = "in";

	constexpr PartTrait to_trait(char id)
	{
		switch (id)
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
	}

	struct Part
	{
	private:
		std::array<TraitType,utils::to_idx(PartTrait::NumTraits)> traits;
	public:
		Part(TraitType x, TraitType m, TraitType a, TraitType s) : traits{ x,m,a,s } {}
		Part(TraitType val) : Part{ val,val,val,val } {}
		Part() : Part{ 0 } {}
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
		TraitType get_total_rating() const noexcept
		{
			return std::reduce(begin(traits), end(traits), TraitType{ 0 }, std::plus<TraitType>{});
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
			AdventCheck(trait_id.length() == 1u);;
			const PartTrait trait = to_trait(trait_id.front());
			result[trait] = val;
		}
		return result;
	}

	enum class Operation
	{
		less_than,
		more_than
	};

	Operation to_operation(char c)
	{
		switch (c)
		{
		case '<':
			return Operation::less_than;
		case '>':
			return Operation::more_than;
		default:
			break;
		}
		AdventUnreachable();
		return Operation::less_than;
	}

	using LocationName = std::string;

	struct InstructionBase
	{
		PartTrait trait;
		Operation op;
		TraitType operand;
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
		auto [condition, targets] = utils::split_string_at_point(fragment, find_result);
		UncompiledProgram program;
		UncompiledInstruction instruction;
		instruction.trait = to_trait(condition.front());
		condition.remove_prefix(1);
		instruction.op = to_operation(condition.front());
		condition.remove_prefix(1);
		instruction.operand = utils::to_value<TraitType>(condition);

		for (bool path : {true, false})
		{
			LocationName subcontext = [&context, path]()
				{
					LocationName result = context;
					result.push_back(path ? '+' : '-');
					return result;
				}();

			PreprocessFragmentResult subresult = preprocess_fragment(subcontext, targets);
			targets = subresult.remaining_line;

			LocationName& target = path ? instruction.true_target : instruction.false_target;

			if (std::holds_alternative<LocationName>(subresult.result))
			{
				target = std::move(std::get<LocationName>(subresult.result));
			}
			else if (std::holds_alternative<UncompiledProgram>(subresult.result))
			{
				target = std::move(subcontext);
				program = std::move(std::get<UncompiledProgram>(subresult.result));
			}
			else
			{
				AdventUnreachable();
			}
		}
		result.remaining_line = targets;
		AdventCheck(!program.contains(context));
		program[context] = std::move(instruction);
		result.result = std::move(program);
		return result;
	}

	UncompiledProgram preprocess_line(std::string_view line)
	{
		line = utils::remove_specific_suffix(line, '}');
		const auto [context, instruction] = utils::split_string_at_first(line, '{');
		const PreprocessFragmentResult result = preprocess_fragment(LocationName{ context }, instruction);
		AdventCheck(std::holds_alternative<UncompiledProgram>(result.result));
		AdventCheck(result.remaining_line.empty());
		return std::get<UncompiledProgram>(result.result);
	}

	UncompiledProgram preprocess_instructions(std::istream& input)
	{
		UncompiledProgram result;
		while (true)
		{
			AdventCheck(!input.eof());
			std::string line;
			std::getline(input, line);
			if (line.empty())
			{
				return result;
			}

			auto combine_fn = [](const UncompiledInstruction& l, const UncompiledInstruction& r)
				{
					AdventUnreachable();
					return l;
				};
			UncompiledProgram line_result = preprocess_line(line);
			result = utils::combine_maps(std::move(result), std::move(line_result), combine_fn);
		}
		return result;
	};

	struct Instruction : InstructionBase
	{
		std::size_t true_target;
		std::size_t false_target;
	};

	struct Program
	{
		std::vector<Instruction> instructions;
		std::size_t in_code;
		std::size_t accept_code;
		std::size_t reject_code;
	};

	Program compile(const UncompiledProgram& uncompiled)
	{
		const std::map<LocationName, std::size_t> name_to_idx = [&uncompiled]()
			{
				std::map<LocationName, std::size_t> result;
				std::size_t idx = 0;
				for (const auto& [name, instruction] : uncompiled)
				{
					result[name] = idx++;
				}
				result[ACCEPT] = idx++;
				result[REJECT] = idx;
				return result;
			}();

		Program result;
		AdventCheck(name_to_idx.contains(ACCEPT));
		AdventCheck(name_to_idx.contains(REJECT));
		AdventCheck(name_to_idx.contains(ENTRY));
		result.accept_code = name_to_idx.at(ACCEPT);
		result.reject_code = name_to_idx.at(REJECT);
		result.in_code = name_to_idx.at(ENTRY);
		result.instructions.reserve(uncompiled.size());

		auto compile_instruction = [&name_to_idx](const std::pair<LocationName,UncompiledInstruction>& elem)
			{
				const UncompiledInstruction& ui = elem.second;
				Instruction result;
				result.trait = ui.trait;
				result.op = ui.op;
				result.operand = ui.operand;
				result.true_target = name_to_idx.at(ui.true_target);
				result.false_target = name_to_idx.at(ui.false_target);
				return result;
			};
		stdr::transform(uncompiled, std::back_inserter(result.instructions), compile_instruction);
		return result;
	}

	Program get_program(std::istream& input)
	{
		const UncompiledProgram up = preprocess_instructions(input);
		return compile(up);
	}

	std::size_t execute_instruction(const Part& part, const Instruction& i)
	{
		const TraitType part_val = part[i.trait];
		const bool passes = [part_val, test_val = i.operand, op = i.op]()
			{
				switch (op)
				{
				case Operation::less_than:
					return part_val < test_val;
				case Operation::more_than:
					return part_val > test_val;
				default:
					break;
				}
				AdventUnreachable();
				return false;
			}();
		return passes ? i.true_target : i.false_target;
	}

	bool test_part(const Program& program, const Part& part)
	{
		std::size_t current_location = program.in_code;
		while (true)
		{
			if (current_location == program.accept_code)
			{
				return true;
			}
			if (current_location == program.reject_code)
			{
				return false;
			}

			const Instruction& instruction = program.instructions[current_location];
			current_location = execute_instruction(part, instruction);
		}
	}

	TraitType test_part(const Program& program, std::string_view part_line)
	{
		const Part part = parse_part(part_line);
		const bool accepted = test_part(program, part);
		return accepted ? part.get_total_rating() : 0;
	}

	TraitType solve_p1(std::istream& input)
	{
		using ILI = utils::istream_line_iterator;
		const Program program = get_program(input);
		return std::transform_reduce(ILI{ input }, ILI{}, TraitType{ 0 },
			std::plus<TraitType>{}, [&program](std::string_view line) {return test_part(program, line); });
	}
}

namespace
{
	constexpr TraitType min_value = 1u;
	constexpr TraitType max_value = 4000u;
	uint64_t get_number_accepted(const Program& program, std::size_t ip, const Part& lower, const Part& upper)
	{
		if (ip == program.reject_code)
		{
			return 0;
		}
		if (ip == program.accept_code)
		{
			auto transform_fn = [&lower, &upper](std::underlying_type_t<PartTrait> idx)
				{
					const PartTrait trait = static_cast<PartTrait>(idx);
					return upper[trait] - lower[trait];
				};
			const auto range = utils::int_range{ utils::to_idx(PartTrait::NumTraits) };
			return std::transform_reduce(begin(range), end(range), uint64_t{ 1 },
				std::multiplies<uint64_t>{}, transform_fn);
		}

		for (auto i : utils::int_range{ utils::to_idx(PartTrait::NumTraits) })
		{
			const PartTrait trait = static_cast<PartTrait>(i);
			if (lower[trait] >= upper[trait]) return 0; // No valid value for this trait.
		}

		const Instruction& i = program.instructions[ip];
		Part top_of_low = upper;
		Part bottom_of_high = lower;
		switch (i.op)
		{
		case Operation::less_than:
			top_of_low[i.trait] = bottom_of_high[i.trait] = i.operand;
			return get_number_accepted(program, i.true_target, lower, top_of_low)
				+ get_number_accepted(program, i.false_target, bottom_of_high, upper);
		case Operation::more_than:
			top_of_low[i.trait] = bottom_of_high[i.trait] = i.operand + 1;
			return get_number_accepted(program, i.true_target, bottom_of_high, upper)
				+ get_number_accepted(program, i.false_target, lower, top_of_low);
		default:
			AdventUnreachable();
			break;
		}
		return 0;
	}

	uint64_t get_number_accepted(const Program& program)
	{
		const Part lower{ min_value };
		const Part upper{ max_value + 1 };
		return get_number_accepted(program, program.in_code, lower, upper);
	}

	uint64_t solve_p2(std::istream& input)
	{
		const Program program = get_program(input);
		return get_number_accepted(program);
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
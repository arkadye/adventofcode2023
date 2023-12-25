#include "advent20.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY20DBG 1
#ifdef NDEBUG
#define DAY20DBG 0
#else
#define DAY20DBG ENABLE_DAY20DBG
#endif

#if DAY20DBG
	#include <iostream>
#endif

namespace
{
#if DAY20DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "small_vector.h"
#include "parse_utils.h"
#include "enums.h"
#include "istream_line_iterator.h"
#include "string_line_iterator.h"
#include "sorted_vector.h"
#include "int_range.h"

#include <array>
#include <string>
#include <map>
#include <deque>

namespace
{
	enum class ModuleType : char
	{
		flip_flop,
		conjunction,
		broadcast,
		output,
		NumModulesTypes
	};

	using ModuleName = std::string;
	using OutputNameList = utils::small_vector<ModuleName, 8>;

	struct ModuleDescriptor
	{
		ModuleName name;
		OutputNameList outputs;
		ModuleType type;
	};

	ModuleDescriptor parse_module(std::string_view line)
	{
		auto [name, broadcast_list] = utils::split_string_at_first(line, " -> ");
		AdventCheck(!name.empty());
		const ModuleType type = [c = name.front()]()
		{
			switch (c)
			{
			case '%': return ModuleType::flip_flop;
			case '&': return ModuleType::conjunction;
			default: return ModuleType::broadcast;
			}
			AdventUnreachable();
			return ModuleType::NumModulesTypes;
		}();
		if (type != ModuleType::broadcast)
		{
			name.remove_prefix(1);
		}

		ModuleDescriptor result;
		result.name = ModuleName{ name };
		result.type = type;
		stdr::transform(utils::string_line_range{ broadcast_list, ", " }, std::back_inserter(result.outputs),
			[](std::string_view sv) {return ModuleName{ sv }; });
		return result;
	}

	using UncompiledModules = std::vector<ModuleDescriptor>;

	UncompiledModules preprocess_input(std::istream& input)
	{
		UncompiledModules result;
		{
			ModuleDescriptor output;
			output.name = "rx";
			output.type = ModuleType::output;
			result.push_back(std::move(output));
		}
		stdr::transform(utils::istream_line_range{ input }, std::back_inserter(result), parse_module);
		return result;
	}

	enum class Signal :char
	{
		nothing,
		low,
		high
	};

	using ModuleId = std::size_t;
	constexpr const ModuleId NO_MODULE = std::numeric_limits<ModuleId>::max();

	struct Instruction
	{
		ModuleId source;
		ModuleId destination;
		Signal signal;
	};
	using Instructions = std::deque<Instruction>;

	struct ModuleBase
	{
		ModuleId id;
		utils::small_vector<ModuleId, 8> output_modules;
		virtual void add_input(ModuleId id) {}
		virtual Signal get_output_pulse(const Instruction& input) = 0;
		Instructions process_pulse(const Instruction& input)
		{
			AdventCheck(input.destination == id);
			AdventCheck(input.signal != Signal::nothing);
			Instructions result;
			const Signal output = get_output_pulse(input);
			if (output == Signal::nothing) return result;
			stdr::transform(output_modules, std::back_inserter(result),
				[output,source_id=id](ModuleId dest_id)
				{
					Instruction result;
					result.source = source_id;
					result.destination = dest_id;
					result.signal = output;
					return result;
				});
			return result;
		}
		virtual bool is_complete() const { return false;  }
		virtual void reset() {}
	};

	struct FlipFlopModule : ModuleBase
	{
		bool is_on= false;
		Signal get_output_pulse(const Instruction& input) override
		{
			switch (input.signal)
			{
			case Signal::high:
				return Signal::nothing;
			case Signal::low:
				is_on = !is_on;
				return is_on ? Signal::high : Signal::low;
			default:
				AdventUnreachable();
				break;
			}
			return Signal::nothing;
		}
	};

	struct BroadcastModule : ModuleBase
	{
		Signal get_output_pulse(const Instruction& input) override { return input.signal; }
	};

	struct ConjunctionModule : ModuleBase
	{
		utils::flat_map<ModuleId, Signal> input_modules;
		void add_input(ModuleId id) override { input_modules.push_back(std::pair{ id,Signal::low }); }
		Signal get_output_pulse(const Instruction& input) override
		{
			const auto input_record = input_modules.find_by_key(input.source);
			AdventCheck(input_record != end(input_modules));
			input_record->second = input.signal;
			const bool all_high = stdr::all_of(input_modules, [](Signal s) {return s == Signal::high; }, &decltype(input_modules)::value_type::second);
			return all_high ? Signal::low : Signal::high;
		}
	};

	struct OutputModule : ModuleBase
	{
		enum class State
		{
			ready,
			active,
			invalididated
		};
		State state = State::ready;
		Signal get_output_pulse(const Instruction& input) override
		{
			if (input.signal == Signal::high)
			{
				state = State::invalididated;
			}
			else if (input.signal == Signal::low)
			{
				switch (state)
				{
				case State::ready:
					state = State::active;
					break;
				case State::active:
					state = State::invalididated;
					break;
				default:
					break;
				}
			}
			return Signal::nothing;
		}
		bool is_complete() const override { return state == State::active; }
		void reset() override { state = State::ready; }
	};

	using Module = std::unique_ptr<ModuleBase>;

	using ModuleList = std::vector<Module>;
	using ModuleNames = utils::flat_map<ModuleName, ModuleId>;

	ModuleId name_to_id(const ModuleNames& names, const ModuleName& name)
	{
		const auto result = names.find_by_key(name);
		return result != end(names) ? result->second : NO_MODULE;
	}

	template <typename UnderlyingType> requires std::is_base_of_v<ModuleBase,UnderlyingType>
	Module compile_module(const ModuleDescriptor& md, const ModuleNames& names)
	{
		Module mod = std::make_unique<UnderlyingType>();
		mod->id = name_to_id(names, md.name);
		stdr::transform(md.outputs, std::back_inserter(mod->output_modules), [&names](const ModuleName& mn) {return name_to_id(names, mn); });
		return mod;
	}

	struct CompileResult
	{
		ModuleList modules;
		ModuleId broadcaster_id = 0;
		ModuleId output_id = 0;
	};

	CompileResult compile_modules(const UncompiledModules& uncompiled)
	{
		// Create a list of module name to ids
		ModuleNames name_map;
		stdr::transform(utils::int_range{ uncompiled.size() }, std::back_inserter(name_map),
			[&uncompiled](std::size_t idx)
			{
				const ModuleDescriptor& mod = uncompiled[idx];
				return std::pair{ mod.name, idx };
			});

		CompileResult result;
		result.broadcaster_id = name_to_id(name_map, ModuleName{ "broadcaster" });
		result.output_id = name_to_id(name_map, ModuleName{ "rx" });

		// Switch each module to use ids instead of names
		auto compile_impl = [&name_map](const ModuleDescriptor& md)
		{
			switch (md.type)
			{
			case ModuleType::flip_flop:
				return compile_module<FlipFlopModule>(md, name_map);
			case ModuleType::conjunction:
				return compile_module<ConjunctionModule>(md, name_map);
			case ModuleType::broadcast:
				return compile_module<BroadcastModule>(md, name_map);
			case ModuleType::output:
				return compile_module<OutputModule>(md, name_map);
			default:
				AdventUnreachable();
				break;
			}
			return Module{};
		};
		stdr::transform(uncompiled, std::back_inserter(result.modules), compile_impl);

		// Tie outputs to conjunction inputs
		for (ModuleId id : utils::int_range{ result.modules.size() })
		{
			AdventCheck(result.modules[id]->id == id);
			for (ModuleId output : result.modules[id]->output_modules)
			{
				if (output == NO_MODULE) continue;
				AdventCheck(output < result.modules.size());
				result.modules[output]->add_input(id);
			}
		}

		return result;
	}

	CompileResult get_modules(std::istream& input)
	{
		UncompiledModules preprocessed = preprocess_input(input);
		return compile_modules(preprocessed);
	}

	// Returns a pair of low and high pulses.
	std::pair<std::size_t, std::size_t> press_button(ModuleList& modules, ModuleId starting_node, Signal starting_signal)
	{
		std::size_t low_signals = 0;
		std::size_t high_signals = 0;
		auto count_signal = [&low_signals, &high_signals](Signal s)
		{
			switch (s)
			{
			case Signal::low:
				++low_signals;
				break;
			case Signal::high:
				++high_signals;
				break;
			default:
				break;
			}
		};
		count_signal(starting_signal);
		Instructions instructions;
		{
			Instruction initial;
			initial.source = std::numeric_limits<ModuleId>::max();
			initial.signal = starting_signal;
			initial.destination = starting_node;
			instructions.push_back(initial);
		}

		while (!instructions.empty())
		{
			const Instruction i = instructions.front();
			instructions.pop_front();
			if (i.destination == NO_MODULE) continue;
			Module& target_module = modules[i.destination];
			Instructions next_instructions = target_module->process_pulse(i);

			for (Instruction& n : next_instructions)
			{
				count_signal(n.signal);
				instructions.push_back(std::move(n));
			}
		}
		return std::pair{ low_signals,high_signals };
	}

	std::size_t solve_p1(std::istream& input)
	{
		auto [modules, broadcast_id, dummy] = get_modules(input);
		std::size_t low = 0;
		std::size_t high = 0;
		for (auto i : utils::int_range{ 1000 })
		{
			const auto [l,h] = press_button(modules, broadcast_id, Signal::low);
			low += l;
			high += h;
		}
		return low * high;
	}
}

namespace
{
	std::size_t solve_p2(std::istream& input)
	{
		auto [modules, broadcast_id, output_id] = get_modules(input);
		AdventCheck(dynamic_cast<OutputModule*>(modules[output_id].get()) != nullptr);
		std::size_t result = 0;
		while (!modules[output_id]->is_complete())
		{
			modules[output_id]->reset();
			press_button(modules, broadcast_id, Signal::low);
			++result;
		}
		return result;
	}
}

ResultType testcase_twenty_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType advent_twenty_p1()
{
	auto input = advent::open_puzzle_input(20);
	return solve_p1(input);
}

ResultType advent_twenty_p2()
{
	auto input = advent::open_puzzle_input(20);
	return solve_p2(input);
}

#undef DAY20DBG
#undef ENABLE_DAY20DBG
#include "advent8.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY8DBG 1
#ifdef NDEBUG
#define DAY8DBG 0
#else
#define DAY8DBG ENABLE_DAY8DBG
#endif

#if DAY8DBG
	#include <iostream>
#endif

namespace
{
#if DAY8DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include <array>
#include <algorithm>
#include <vector>

#include "range_contains.h"
#include "sorted_vector.h"
#include "parse_utils.h"
#include "istream_line_iterator.h"

namespace
{
	enum class Direction : char
	{
		left = 'L',
		right = 'R'
	};

	Direction to_direction(char c)
	{
		AdventCheck(std::string_view{"LR"}.find(c) < std::string_view::npos);
		return static_cast<Direction>(c);
	}

	class NodeId
	{
#if DAY8DBG
		std::array<char,3> m_data{'\0','\0','\0'};
		bool is_start_node = false;
		bool is_end_node = false;
#else
		static constexpr std::size_t start_flag_offset = 31;
		static constexpr std::size_t end_flag_offset = 30;
		uint32_t m_data{0};
#endif
	public:
		NodeId(std::string_view name, std::string_view start_node_suffix, std::string_view end_node_suffix)
		{
			constexpr std::string_view valid_chars{"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
			
			AdventCheck(name.size() == 3);
			const bool start = name.ends_with(start_node_suffix);
			const bool finish = name.ends_with(end_node_suffix);
#if DAY8DBG
			std::ranges::copy(name,begin(m_data));
			is_start_node = start;
			is_end_node = finish;
#else
			auto acc_fn = [valid_chars](uint32_t running_val , char c)
			{
				constexpr uint32_t max_val = 26;
				const uint32_t incr = valid_chars.find(c);
				AdventCheck(incr < valid_chars.size());
				const uint32_t new_val = max_val * running_val + incr;
				return new_val;
			};
			m_data = std::accumulate(begin(name),end(name),uint32_t{0},acc_fn);
			AdventCheck((m_data & (start_flag_offset | end_flag_offset)) == uint32_t{0});
			m_data = m_data | ((start ? 1 : 0) << start_flag_offset);
			m_data = m_data | ((finish ? 1 : 0) << end_flag_offset);
#endif
		}
		auto operator<=>(const NodeId& other) const noexcept = default;
		bool is_path_start() const
		{
#if DAY8DBG
			return is_start_node;
#else
			return (start_flag_offset & m_data) != uint32_t{0};
#endif
		}
		bool is_path_end() const
		{
#if DAY8DBG
			return is_end_node;
#else
			return (end_flag_offset & m_data) != uint32_t{ 0 };
#endif
		}
	};

	class Junction
	{
		NodeId left_path;
		NodeId right_path;
	public:
		Junction(NodeId left, NodeId right) : left_path{left} , right_path{right}{}
		NodeId get_next(Direction d) const
		{
			switch(d)
			{
			case Direction::left:
				return left_path;
			case Direction::right:
				return right_path;
			default:
				break;
			}
			AdventUnreachable();
			return NodeId{"","",""};
		}
	};

	using DirectionList = std::vector<Direction>;
	DirectionList parse_directions(std::string_view input)
	{
		DirectionList result;
		result.reserve(input.size());
		std::ranges::transform(input, std::back_inserter(result), to_direction);
		return result;
	}

	std::pair<NodeId,Junction> parse_node(std::string_view input, std::string_view node_start_suffix, std::string_view node_end_suffix)
	{
		auto make_node_id = [node_start_suffix,node_end_suffix](std::string_view id_str)
		{
			return NodeId{id_str,node_start_suffix,node_end_suffix};
		};
		auto [id_str , directions] = utils::split_string_at_first(input," = ");
		directions = utils::remove_specific_prefix(directions,'(');
		directions = utils::remove_specific_suffix(directions,')');
		auto [left_str,right_str] = utils::split_string_at_first(directions,", ");
		return std::pair{ make_node_id(id_str) , Junction{ make_node_id(left_str), make_node_id(right_str) } };
	}

	using NodeMap = utils::flat_map<NodeId,Junction>;

	NodeMap parse_nodes(std::istream& input)
	{
		NodeMap result;
		result.reserve(750);

		using ILI = utils::istream_line_iterator;
		//std::transform(ILI{input}, ILI{},std::back_inserter(result),parse_node); // TODO make istream_line_range work with std::ranges.

		auto node_exists = [&result](NodeId id)
		{
			return result.contains_key(id);
		};

		auto is_junction_valid = [&node_exists](const Junction& j)
		{
			return node_exists(j.get_next(Direction::left)) && node_exists(j.get_next(Direction::right));
		};

		auto is_valid =[&is_junction_valid](const std::pair<NodeId,Junction>& node)
		{
			return is_junction_valid(node.second);
		};

		AdventCheck(std::ranges::all_of(result, is_valid));

		return result;
	}

	struct Description
	{
		DirectionList direction_list;
		NodeMap node_map;
		utils::small_vector<NodeId,8> start_nodes;
		utils::small_vector<NodeId,8> end_nodes;
	};

	Description parse_input(std::istream& input, std::string_view start_node_suffix, std::string_view end_node_suffix)
	{
		Description result;
		{
			std::string line;
			std::getline(input,line);
			result.direction_list = parse_directions(line);

			// Eat the blank line
			std::getline(input,line);
			AdventCheck(line.empty());
		}

		result.node_map = parse_nodes(input);
		return result;
	}

	int count_steps(const Description& desc)
	{
		int result = 0;
		/*NodeId location = start;
		while(location != end)
		{
			AdventCheckMsg(result < std::numeric_limits<int>::max(),"Looped too many times. Overflowing!");
			const int step_idx = result % desc.direction_list.size();
			const Junction& junction = desc.node_map.at(location);
			location = junction.get_next(desc.direction_list[step_idx]);
			++result;
		}*/
		return result;
	}

	int solve_p1(std::istream& input)
	{
		const Description desc = parse_input(input, "AAA", "ZZZ");
		return count_steps(desc);
	}
}

namespace
{
	int solve_p2(std::istream& input)
	{
		return 0;
	}
}

ResultType testcase_eight_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType testcase_eight_p2(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_eight_p1()
{
	auto input = advent::open_puzzle_input(8);
	return solve_p1(input);
}

ResultType advent_eight_p2()
{
	auto input = advent::open_puzzle_input(8);
	return solve_p2(input);
}

#undef DAY8DBG
#undef ENABLE_DAY8DBG
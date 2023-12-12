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
#include "transform_if.h"
#include "int_range.h"

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
		std::array<char,3> m_data{'\0','\0','\0'};
		bool is_start_node = false;
		bool is_end_node = false;
	public:
		NodeId() {}
		NodeId(std::string_view name, std::string_view start_node_suffix, std::string_view end_node_suffix)
		{
			constexpr std::string_view valid_chars{"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
			
			AdventCheck(name.size() == 3);
			const bool start = name.ends_with(start_node_suffix);
			const bool finish = name.ends_with(end_node_suffix);

			stdr::copy(name,begin(m_data));
			is_start_node = start;
			is_end_node = finish;
		}
		auto operator<=>(const NodeId& other) const noexcept = default;
		bool is_path_start() const
		{
			return is_start_node;
		}
		bool is_path_end() const
		{
			return is_end_node;
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
			return NodeId{};
		}
	};

	using DirectionList = std::vector<Direction>;
	DirectionList parse_directions(std::string_view input)
	{
		DirectionList result;
		result.reserve(input.size());
		stdr::transform(input, std::back_inserter(result), to_direction);
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

	NodeMap parse_nodes(std::istream& input, std::string_view node_start_suffix, std::string_view node_end_suffix)
	{
		NodeMap result;
		result.reserve(750);

		auto parse_wrapper = [node_start_suffix, node_end_suffix](std::string_view line)
			{
				return parse_node(line, node_start_suffix, node_end_suffix);
			};

		stdr::transform(utils::istream_line_range{input},std::back_inserter(result),parse_wrapper);

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

		AdventCheck(stdr::all_of(result, is_valid));

		return result;
	}

	struct Description
	{
		DirectionList direction_list;
		NodeMap node_map;
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

		result.node_map = parse_nodes(input, start_node_suffix, end_node_suffix);
		return result;
	}

	struct PathDescription
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
		int64_t cycle_start = 0;
		int64_t cycle_end = 0;
		std::vector<int64_t> end_points;
	};

	PathDescription get_path_description(const Description& desc, NodeId start, const auto& end_points_range)
	{
		auto is_end_point = [&end_points_range](NodeId node)
			{
				return stdr::find(end_points_range, node) != end(end_points_range);
			};

		struct LocationData
		{
			std::size_t path_idx{ 0 };
			NodeId node_id{};
			auto operator<=>(const LocationData&) const noexcept = default;
		};

		utils::flat_map<LocationData, int64_t> visited_locations;

		PathDescription result;
		NodeId current_node = start;
		int64_t current_step = 0;
		while (true)
		{
			const std::size_t path_idx = static_cast<std::size_t>(current_step % desc.direction_list.size());
			const LocationData current_location{ path_idx,current_node };
			const auto find_result = visited_locations.find_by_key(current_location);
			if (find_result != end(visited_locations))
			{
				result.cycle_start = find_result->second;
				result.cycle_end = current_step;
				return result;
			}

			visited_locations.insert_keep_sorted(std::pair{ current_location,current_step });
			if (is_end_point(current_node))
			{
				result.end_points.push_back(current_step);
			}

			const Direction dir = desc.direction_list[path_idx];
			const Junction& junc = desc.node_map.at(current_node);
			current_node = junc.get_next(dir);
			++current_step;
		}
		AdventUnreachable();
		return result;
	}

	int64_t count_steps(const Description& desc)
	{
		constexpr std::size_t max_starting_point_size = 8;
		using SmallNodeList = utils::small_vector<NodeId, max_starting_point_size>;
		SmallNodeList start_nodes;
		SmallNodeList end_nodes;
		
		auto jn_to_node = [](const std::pair<NodeId, Junction>& n) { return n.first; };
		auto keep_start = [](NodeId n) { return n.is_path_start(); };
		auto keep_end = [](NodeId n) { return n.is_path_end(); };
		utils::ranges::transform_if_post(desc.node_map, std::back_inserter(start_nodes), jn_to_node, keep_start);
		utils::ranges::transform_if_post(desc.node_map, std::back_inserter(end_nodes), jn_to_node, keep_end);

		auto node_to_path = [&desc,&end_nodes](NodeId start)
			{
				return get_path_description(desc, start, end_nodes);
			};

		utils::small_vector<PathDescription, max_starting_point_size> paths;
		stdr::transform(start_nodes, std::back_inserter(paths), node_to_path);

		for (const PathDescription& path : paths)
		{
			log << "\nEnd points: [";
			for (auto i : utils::int_range{ path.end_points.size() })
			{
				if (i != 0u) log << ',';
				log << path.end_points[i];
			}
			log << "] Cycle: " << path.cycle_start << " -- > " << path.cycle_end;
		}

		if (paths.size() == std::size_t{ 1 })
		{
			const PathDescription& pd = paths[0];
			AdventCheck(!pd.end_points.empty());
			return pd.end_points[0];
		}

		// I think I can just take the LCM of the cycle lengths but I'm pretty sure this doesn't work in the general case.
		return std::transform_reduce(begin(paths), end(paths), std::size_t{ 1 },
			std::lcm<std::size_t, std::size_t>, [](const PathDescription& pd) {return pd.cycle_end - pd.cycle_start; });
	}

	int64_t solve_p1(std::istream& input)
	{
		const Description desc = parse_input(input, "AAA", "ZZZ");
		return count_steps(desc);
	}
}

namespace
{
	int64_t solve_p2(std::istream& input)
	{
		const Description desc = parse_input(input, "A", "Z");
		return count_steps(desc);
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
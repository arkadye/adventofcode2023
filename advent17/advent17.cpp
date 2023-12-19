#include "advent17.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY17DBG 1
#ifdef NDEBUG
#define DAY17DBG 0
#else
#define DAY17DBG ENABLE_DAY17DBG
#endif

#if DAY17DBG
	#include <iostream>
#endif

namespace
{
#if DAY17DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "grid.h"
#include "range_contains.h"
#include "enums.h"
#include "swap_remove.h"
#include "small_vector.h"
#include "int_range.h"

#include <algorithm>
#include <set>

namespace
{
	using Tile = uint8_t;
	using Grid = utils::grid<Tile>;

	Grid parse_grid(std::istream& input)
	{
		auto parse_char = [](char c)
		{
			AdventCheck(utils::range_contains_inc(c, '1','9'));
			return static_cast<Tile>(c - '0');
		};
		return utils::grid_helpers::build(input, parse_char);
	}

	struct PathResult
	{
		std::size_t path_cost = 0;
	};

	PathResult get_path(const Grid& grid, const utils::coords& start_point, const utils::coords& end_point, int min_straight_line_path, int max_straight_line_path)
	{
		AdventCheck(grid.is_on_grid(start_point));
		AdventCheck(grid.is_on_grid(end_point));
		AdventCheck(min_straight_line_path <= max_straight_line_path);

		enum EntryType
		{
			horizontal,
			vertical,
			start_pos
		};

		struct NodeBase
		{
			utils::coords location;
			EntryType entry_type;
			auto operator<=>(const NodeBase&) const noexcept = default;
		};

		struct Node : NodeBase
		{
			std::size_t path_cost;
			std::size_t plus_heuristic;
		};

		auto sort_by_comparer = [](const Node& left, const Node& right, const auto& transform_fn)
		{
			return transform_fn(left) <=> transform_fn(right);
		};

		auto sort_by_heuristic = [&sort_by_comparer](const Node& left, const Node& right)
		{
			return sort_by_comparer(left,right,[](const Node& n){return std::pair{n.plus_heuristic,n.path_cost}; }) == std::weak_ordering::less;
		};

		auto sort_by_location = [&sort_by_comparer](const Node& left, const Node& right)
		{
			return sort_by_comparer(left, right, [](const Node& n) {return std::pair{ n.location,n.entry_type }; }) == std::weak_ordering::equivalent;
		};

		auto make_node = [&end_point](const utils::coords& point, EntryType entry, std::size_t current_cost)
		{
			Node result;
			result.location = point;
			result.entry_type = entry;
			result.path_cost = current_cost;
			result.plus_heuristic = current_cost + static_cast<std::size_t>(point.manhatten_distance(end_point));
			return result;
		};

		auto make_result = [](const Node& node)
		{
			PathResult result;
			result.path_cost = node.path_cost;
			return result;
		};

		utils::small_vector<Node,1> nodes_to_check;
		nodes_to_check.reserve(2 * grid.size());

		std::set<NodeBase> checked_nodes;
		//checked_nodes.reserve(2 * grid.size());

		nodes_to_check.push_back( make_node(start_point,EntryType::start_pos, 0u) );

		while(!nodes_to_check.empty())
		{
			const auto min_node_it = std::min_element(begin(nodes_to_check), end(nodes_to_check), sort_by_heuristic); // TODO switch to ranges
			const Node node = *min_node_it;
			utils::swap_remove(nodes_to_check,min_node_it);

			AdventCheck(grid.is_on_grid(node.location));

			if(node.location == end_point)
			{
				return make_result(node);
			}

			const auto find_previous_result = checked_nodes.find(node);
			if(find_previous_result != end(checked_nodes))
			{
				// We already checked this one.
				continue;
			}

			checked_nodes.insert(find_previous_result, node);

			const utils::small_vector<utils::direction,4> next_directions = [entry = node.entry_type]()
			{
				utils::small_vector<utils::direction,4> result;
				switch(entry)
				{
				case start_pos:
					result.assign({utils::direction::up,utils::direction::right,utils::direction::down,utils::direction::left});
					break;
				case EntryType::horizontal:
					result.assign({utils::direction::up, utils::direction::down});
					break;
				case EntryType::vertical:
					result.assign({utils::direction::left, utils::direction::right});
					break;
				default:
					AdventUnreachable();
					break;
				}
				return result;
			}();

			for(utils::direction dir : next_directions)
			{
				const EntryType et = utils::is_vertical(dir) ? EntryType::vertical : EntryType::horizontal;
				std::size_t running_cost = node.path_cost;

				for(int offset : utils::int_range{1,max_straight_line_path+1})
				{
					const utils::coords target_coords = node.location + offset * utils::coords::dir(dir);
					if(!grid.is_on_grid(target_coords)) break;

					running_cost += grid.at(target_coords);
					if(offset >= min_straight_line_path)
					{
						nodes_to_check.push_back(make_node(target_coords,et,running_cost));
					}
				}
			}
		}
		AdventUnreachable();
		return PathResult{};
	}

	std::size_t solve_generic(std::istream& input, int min_path, int max_path)
	{
		const Grid grid = parse_grid(input);
		const PathResult path_result = get_path(grid, grid.top_left(), grid.bottom_right(), min_path, max_path);
		return path_result.path_cost;
	}

	std::size_t solve_p1(std::istream& input)
	{
		return solve_generic(input,1,3);
	}

	int solve_p2(std::istream& input)
	{
		return solve_generic(input,4,10);
	}
}

ResultType testcase_seventeen_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType testcase_seventeen_p2(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_seventeen_p1()
{
	auto input = advent::open_puzzle_input(17);
	return solve_p1(input);
}

ResultType advent_seventeen_p2()
{
	auto input = advent::open_puzzle_input(17);
	return solve_p2(input);
}

#undef DAY17DBG
#undef ENABLE_DAY17DBG
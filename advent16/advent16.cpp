#include "advent16.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY16DBG 1
#ifdef NDEBUG
#define DAY16DBG 0
#else
#define DAY16DBG ENABLE_DAY16DBG
#endif

#if DAY16DBG
	#include <iostream>
#endif

namespace
{
#if DAY16DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "grid.h"
#include "coords.h"
#include "small_vector.h"
#include "coords_iterators.h"
#include "comparisons.h"

namespace
{
	enum class TileType : uint8_t
	{
		space,
		upward_mirror,
		downward_mirror,
		vertical_splitter,
		horizontal_splitter
	};

	TileType to_tile(char c)
	{
		const std::array<std::pair<char, TileType>,5> map{
			std::pair{'.' , TileType::space},
			std::pair{'/' , TileType::upward_mirror},
			std::pair{'\\', TileType::downward_mirror},
			std::pair{'-', TileType::horizontal_splitter},
			std::pair{'|', TileType::vertical_splitter}
		};
		const auto find_result = stdr::find(map, c, &std::pair<char, TileType>::first);
		AdventCheck(find_result != end(map));
		return find_result->second;
	}

	char tiletype_to_char(TileType tt)
	{
		switch (tt)
		{
		case TileType::space:
			return '.';
		case TileType::downward_mirror:
			return '\\';
		case TileType::upward_mirror:
			return '/';
		case TileType::horizontal_splitter:
			return '-';
		case TileType::vertical_splitter:
			return '|';
		default:
			break;
		}
		AdventUnreachable();
		return '!';
	}

	struct Tile
	{
		TileType type : 3;
		uint8_t entered_from_north : 1;
		uint8_t entered_from_east : 1;
		uint8_t entered_from_south : 1;
		uint8_t entered_from_west : 1;
		Tile(TileType t = TileType::space)
			: type{ t }
			, entered_from_north{ 0 }
			, entered_from_east{ 0 }
			, entered_from_south{ 0 }
			, entered_from_west{ 0 }
		{}
		bool energised() const { return entered_from_north || entered_from_east || entered_from_south || entered_from_west; }
	};

	char tile_to_char(Tile t)
	{
		return tiletype_to_char(t.type);
	}

	Tile make_tile_from_type(TileType type)
	{
		Tile result;
		result.entered_from_north = 0;
		result.entered_from_east = 0;
		result.entered_from_south = 0;
		result.entered_from_west = 0;
		result.type = type;
		return result;
	}

	Tile make_tile_from_char(char c)
	{
		const TileType type = to_tile(c);
		return make_tile_from_type(type);
	}

	using Grid = utils::grid<Tile>;

	Grid parse_grid(std::istream& input)
	{
		return utils::grid_helpers::build(input, make_tile_from_char);
	}

	using Direction = utils::direction;
	using DirList = utils::small_vector<Direction, 2>;
	DirList get_output_directions(utils::direction in_dir, TileType type)
	{
		auto reflect_downward = [in_dir]()
			{
				const bool h = utils::is_horizontal(in_dir);
				const utils::turn_dir td = h ? utils::turn_dir::clockwise : utils::turn_dir::anticlockwise;
				const utils::direction r = utils::rotate(in_dir, td);
				return r;
			};
		switch(type)
		{
		case TileType::space:
			return DirList{ in_dir };
		case TileType::downward_mirror:
			return DirList{ reflect_downward() };
		case TileType::upward_mirror:
			return DirList{ utils::turn_around(reflect_downward()) };
		case TileType::horizontal_splitter:
			return utils::is_vertical(in_dir) ? DirList{ Direction::left,Direction::right } : DirList{ in_dir };
		case TileType::vertical_splitter:
			return utils::is_horizontal(in_dir) ? DirList{ Direction::up,Direction::down } : DirList{ in_dir };
		}
		AdventUnreachable();
		return DirList{};
	}

	DirList process_beam_entry(Tile& tile, Direction dir)
	{
		switch (dir)
		{
		case Direction::up:
			if (tile.entered_from_south) return DirList{};
			tile.entered_from_south = 1;
			break;
		case Direction::right:
			if (tile.entered_from_west) return DirList{};
			tile.entered_from_west = 1;
			break;
		case Direction::down:
			if (tile.entered_from_north) return DirList{};
			tile.entered_from_north = 1;
			break;
		case Direction::left:
			if (tile.entered_from_east) return DirList{};
			tile.entered_from_east = 1;
			break;
		}
		return get_output_directions(dir, tile.type);
	}

	Grid fire_beam(Grid grid, utils::coords target, Direction direction)
	{
		struct Node
		{
			utils::coords target;
			Direction dir;
		};
		std::vector<Node> nodes_to_search{ Node{target,direction} };
		while (!nodes_to_search.empty())
		{
			const Node node = nodes_to_search.back();
			nodes_to_search.pop_back();
			if (!grid.is_on_grid(node.target)) continue;
			Tile& tile = grid.at(node.target);
			const DirList next_dirs = process_beam_entry(tile, node.dir);
			for (Direction d : next_dirs)
			{
				const utils::coords next_target = node.target + utils::coords::dir(d);
				nodes_to_search.emplace_back(next_target, d);
			}
		}
		return grid;
	}

	std::size_t num_energised_tiles(const Grid& grid)
	{
		return stdr::count(utils::grid_helpers::elem_view{ grid }, true, &Tile::energised);
	}

	std::size_t solve_p1(std::istream& input)
	{
		Grid grid = parse_grid(input);
		grid = fire_beam(std::move(grid), grid.top_left(), Direction::right);
		const std::size_t result = num_energised_tiles(grid);
		return result;
	}
}

namespace
{
	std::size_t get_range_max(const auto& range, const auto& transform)
	{
		return std::transform_reduce(begin(range), end(range), std::size_t{ 0 }, utils::Larger<std::size_t>{}, transform);
	}

	std::size_t get_max_energization(const Grid& grid, Direction dir)
	{
		const auto coords_range = [&grid,dir]()
			{
				using namespace utils::coords_iterators;
				const utils::coords limits = grid.get_max_point();
				switch (dir)
				{
				case Direction::up:
					log << "\nDir up";
					return elem_range<int>::get_row(grid.bottom_left(), limits.x);
				case Direction::right:
					log << "\nDir right";
					return elem_range<int>::get_column(grid.bottom_left(), limits.y);
				case Direction::down:
					log << "\nDir down";
					return elem_range<int>::get_row(grid.top_left(), limits.x);
				case Direction::left:
					log << "\nDir left";
					return elem_range<int>::get_column(grid.bottom_right(), limits.y);
				default:
					break;
				}
				AdventUnreachable();
				return elem_range{ utils::coords{0,0},utils::coords{0,0} };
			}();

		auto try_entry_point = [&grid, dir](const utils::coords& c)
			{
				const Grid g = fire_beam(grid, c, dir);
				return num_energised_tiles(g);
			};
		
		log << ". Got range with " << coords_range.size() << " values.";
		const std::size_t result = get_range_max(coords_range, try_entry_point);
		return result;
	}

	std::size_t solve_p2(std::istream& input)
	{
		const Grid grid = parse_grid(input);
		std::array<Direction, 4> all_dirs{ Direction::up,Direction::right,Direction::down,Direction::left };
		const std::size_t result = get_range_max(all_dirs,
			[&grid](Direction dir) { return get_max_energization(grid, dir); });
		return result;
	}
}

ResultType testcase_sixteen_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType testcase_sixteen_p2(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_sixteen_p1()
{
	auto input = advent::open_puzzle_input(16);
	return solve_p1(input);
}

ResultType advent_sixteen_p2()
{
	auto input = advent::open_puzzle_input(16);
	return solve_p2(input);
}

#undef DAY16DBG
#undef ENABLE_DAY16DBG
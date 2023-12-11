#include "advent10.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY10DBG 1
#ifdef NDEBUG
#define DAY10DBG 0
#else
#define DAY10DBG ENABLE_DAY10DBG
#endif

#if DAY10DBG
	#include <iostream>
#endif

namespace
{
#if DAY10DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "grid.h"
#include "coords.h"
#include "small_vector.h"
#include "sorted_vector.h"
#include "range_contains.h"
#include "int_range.h"

#include <array>
#include <vector>
#include <variant>

namespace
{
	using Coords = utils::coords;
	using Dir = utils::direction;

	class Pipe
	{
		std::array<Dir, 2> dirs;
	public:
		constexpr Pipe(Dir connection_a, Dir connection_b) : dirs{ connection_a,connection_b } {}
		Dir traverse(Dir in_dir) const noexcept
		{
			const Dir reversed_in_dir = utils::turn_around(in_dir);
			if (reversed_in_dir == dirs[0]) return dirs[1];
			if (reversed_in_dir == dirs[1]) return dirs[0];
			AdventUnreachable();
			return in_dir;
		}
		bool can_accept_input_from(Dir in_dir) const noexcept
		{
			const Dir reversed_in_dir = utils::turn_around(in_dir);
			return std::ranges::find(dirs, reversed_in_dir) != end(dirs);
		}
		bool get_path_supersampled(const Coords& subcoordinate) const
		{
			AdventCheck(utils::range_contains_exc(subcoordinate.x, 0, 3));
			AdventCheck(utils::range_contains_exc(subcoordinate.y, 0, 3));
			utils::small_vector<Coords, 3> path_coords;
			path_coords.emplace_back(1, 1);
			std::ranges::transform(dirs, std::back_inserter(path_coords), [](Dir d) {return Coords{ 1,1 } + Coords::dir(d); });
			const auto find_result = std::ranges::find(path_coords, subcoordinate);
			return (find_result != end(path_coords));

		}
		bool operator==(const Pipe&) const noexcept = default;
	};

	struct StartPosition
	{
		bool operator==(const StartPosition&) const noexcept { return true; }
		bool get_path_supersampled(const Coords& subcoordinate) const
		{
			AdventCheck(utils::range_contains_exc(subcoordinate.x, 0, 3));
			AdventCheck(utils::range_contains_exc(subcoordinate.y, 0, 3));
			constexpr std::array<Coords, 5> paths{ Coords{0,1},Coords{1,0}, Coords{1,1},Coords{2,1}, Coords{1,2} };
			const auto find_result = std::ranges::find(paths, subcoordinate);
			return find_result != end(paths);
		}
	};
	struct Ground
	{
		bool operator==(const Ground&) const noexcept { return true; }
		bool get_path_supersampled(const Coords& subcoordinate) const
		{
			AdventCheck(utils::range_contains_exc(subcoordinate.x, 0, 3));
			AdventCheck(utils::range_contains_exc(subcoordinate.y, 0, 3));
			return false;
		}
	};

	using Tile = std::variant<Pipe, StartPosition, Ground>;

	Tile to_tile(char c)
	{
		switch (c)
		{
		case '.':
			return Ground{};
		case 'S':
			return StartPosition{};
		case '-':
			return Pipe{ Dir::left, Dir::right };
		case '|':
			return Pipe{ Dir::up, Dir::down };
		case 'L':
			return Pipe{ Dir::down, Dir::right };
		case 'F':
			return Pipe{ Dir::up, Dir::right };
		case 'J':
			return Pipe{ Dir::down, Dir::left };
		case '7':
			return Pipe{ Dir::up, Dir::left };
		default:
			break;
		}
		AdventUnreachable();
		return Ground{};
	}

	using Grid = utils::grid<Tile>;

	Grid parse_input(std::istream& input)
	{
		return utils::grid_helpers::build(input, to_tile);
	}

	Coords get_starting_point(const Grid& g)
	{
		const std::optional<Coords> find_result = g.get_coordinates(StartPosition{});
		AdventCheck(find_result.has_value());
		return *find_result;
	}

	Dir get_next_dir(Dir in_dir, Tile tile)
	{
		AdventCheck(std::holds_alternative<Pipe>(tile));
		const Pipe& pipe = std::get<Pipe>(tile);
		return pipe.traverse(in_dir);
	}

	void get_path(utils::sorted_vector<Coords>& partial_path, const Grid& grid, Coords location, Dir direction)
	{
		while (true)
		{
			partial_path.push_back(location);
			location = location + Coords::dir(direction);
			if (!grid.is_on_grid(location))
			{
				partial_path.clear();
				return;
			}
			const Tile& next_tile = grid.at(location);
			if (std::holds_alternative<StartPosition>(next_tile))
			{
				return;
			}
			if (std::holds_alternative<Ground>(next_tile))
			{
				partial_path.clear();
				return;
			}
			AdventCheck(std::holds_alternative<Pipe>(next_tile));
			const Pipe& pipe = std::get<Pipe>(next_tile);
			if (!pipe.can_accept_input_from(direction))
			{
				partial_path.clear();
				return;
			}
			direction = pipe.traverse(direction);
		}
		AdventUnreachable();
		return;
	}

	utils::sorted_vector<Coords> get_path(const Grid& grid, Coords location, Dir direction)
	{
		utils::sorted_vector<Coords> result;
		get_path(result, grid, location, direction);
		return result;
	}

	utils::sorted_vector<Coords> get_path(const Grid& grid)
	{
		const Coords start = get_starting_point(grid);
		for (Dir dir : {Dir::up, Dir::right, Dir::down, Dir::left})
		{
			const utils::sorted_vector<Coords> path = get_path(grid, start, dir);
			if (!path.empty())
			{
				return path;
			}
		}
		return {};
	}

	utils::sorted_vector<Coords> get_path(std::istream& input)
	{
		const Grid grid = parse_input(input);
		return get_path(grid);
	}

	std::size_t solve_p1(std::istream& input)
	{
		const utils::sorted_vector<Coords> path = get_path(input);
		AdventCheck(!path.empty());
		const std::size_t path_length = path.size();
		const bool is_odd = (path_length % 2) != 0;
		const std::size_t half_length = (path_length / 2) + (is_odd ? 1 : 0);
		return half_length;
	}
};

namespace
{
	struct GetPathSupersampled
	{
		Coords subcoordinate;
		template <typename T>
		bool impl(const T& item) const
		{
			return item.get_path_supersampled(subcoordinate);
		}

		bool operator()(const Pipe& p) const { return impl(p); }
		bool operator()(const StartPosition& sp) const { return impl(sp); }
		bool operator()(const Ground& g) const { return impl(g); }
	};

	bool get_path_supersampled(const Tile& tile, const Coords& coord)
	{
		return std::visit(GetPathSupersampled{ coord }, tile);
	}

	enum class SearchState
	{
		outside_unexpanded, outside_expanded, inside, path
	};

	int solve_p2(std::istream& input)
	{
		const Grid grid = parse_input(input);
		const Coords max_coords = grid.get_max_point();

		utils::grid<SearchState> big_grid;
		const Coords big_max_coords = 3 * max_coords;
		big_grid.resize(big_max_coords, SearchState::inside);

		for (int y : utils::int_range{ big_max_coords.y })
		{
			const int small_y = y / 3;
			const int sub_y = y % 3;
			const bool is_vertical_edge = (y == 0 || y == big_max_coords.y - 1);
			for (int x : utils::int_range{ big_max_coords.x })
			{
				AdventCheck(big_grid.is_on_grid(Coords{ x,y }));
				const int small_x = x / 3;
				const int sub_x = x % 3;
				const bool is_horizontal_edge = (x == 0 || x == big_max_coords.x - 1);
				const bool is_edge = is_vertical_edge || is_horizontal_edge;

				const Tile& tile = grid.at(Coords{ small_x,small_y });
				const bool is_pipe = get_path_supersampled(tile, Coords{ sub_x,sub_y });

				SearchState& sample = big_grid.at(Coords{ x,y });
				if (is_pipe) sample = SearchState::path;
				else if (is_edge) sample = SearchState::outside_unexpanded;
				else AdventCheck(sample == SearchState::inside);
			}
		}

		while (true)
		{
			bool has_unexpanded = false;
			for (int y : utils::int_range{ big_max_coords.y })
			{
				for (int x : utils::int_range{ big_max_coords.x })
				{
					const Coords loc{ x,y };
					SearchState& sample = big_grid.at(loc);
					if (sample != SearchState::outside_unexpanded) continue;

					has_unexpanded = true;
					sample = SearchState::outside_expanded;
					for (const Coords& neighbour : loc.neighbours())
					{
						if (!big_grid.is_on_grid(neighbour)) continue;

						SearchState& neighbour_sample = big_grid.at(neighbour);

						if (neighbour_sample != SearchState::inside) continue;

						neighbour_sample = SearchState::outside_unexpanded;
					}
				}
			}

			if (!has_unexpanded) break;
		}

		int inside_bits = 0;
		for (int y : utils::int_range{ max_coords.y })
		{
			for (int x : utils::int_range{ max_coords.x })
			{
				const Coords coords{ x,y };
				const Coords big_coords_base = 3 * coords;
				bool is_inside = true;
				for (int sy : utils::int_range{ 3 })
				{
					for (int sx : utils::int_range{ 3 })
					{
						const Coords big_coords = big_coords_base + Coords{ sx,sy };
						const SearchState sample = big_grid.at(big_coords);
						AdventCheck(sample != SearchState::outside_unexpanded);
						if (sample == SearchState::outside_expanded)
						{
							is_inside = false;
							break;
						}
					}
					if (!is_inside) break;
				}
				if (is_inside)
				{
					++inside_bits;
				}
			}
		}
		return inside_bits;
	}
}

ResultType testcase_ten_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType testcase_ten_p2(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_ten_p1()
{
	auto input = advent::open_puzzle_input(10);
	return solve_p1(input);
}

ResultType advent_ten_p2()
{
	auto input = advent::open_puzzle_input(10);
	return solve_p2(input);
}

#undef DAY10DBG
#undef ENABLE_DAY10DBG
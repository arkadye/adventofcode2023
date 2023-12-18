#include "advent14.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY14DBG 1
#ifdef NDEBUG
#define DAY14DBG 0
#else
#define DAY14DBG ENABLE_DAY14DBG
#endif

#if DAY14DBG
	#include <iostream>
#endif

namespace
{
#if DAY14DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "small_vector.h"
#include "grid.h"
#include "coords.h"

#include <algorithm>
#include <array>
#include <map>

namespace
{
	enum class Tile : uint8_t
	{
		round,
		block,
		space
	};

	char to_char(Tile tile) noexcept
	{
		switch (tile)
		{
		case Tile::round:
			return 'O';
		case Tile::block:
			return '#';
		case Tile::space:
			return '.';
		default:
			AdventUnreachable();
			break;
		}
		return '!';
	}

	std::ostream& operator<<(std::ostream& oss, Tile tile)
	{
		oss << to_char(tile);
		return oss;
	}

	Tile to_tile(char c) noexcept
	{
		switch (c)
		{
		case '.':
			return Tile::space;
		case 'O':
			return Tile::round;
		case '#':
			return Tile::block;
		default:
			break;
		}
		AdventUnreachable();
	}

	using Coords = utils::coords;
	using Direction = utils::direction;
	using Grid = utils::grid<Tile>;
	using RowView = utils::grid_helpers::elem_view<const Grid>;


	Grid parse_grid(std::istream& input)
	{
		return utils::grid_helpers::build(input, to_tile);
	}

	Grid tilt_grid(Grid grid, Direction tilt_dir)
	{
		const Direction uphill = utils::turn_around(tilt_dir);
		const bool is_vertical = utils::is_vertical(tilt_dir);

		const Coords max_point = grid.get_max_point();
		const auto major_extent = is_vertical ? max_point.y : max_point.x;
		const auto minor_extent = is_vertical ? max_point.x : max_point.y;

		const bool is_ascending = utils::is_ascending(uphill);

		const auto major_range = is_ascending
			? utils::int_range{ major_extent }
			: utils::int_range{ major_extent }.reverse();

		constexpr int ASSUMED_MINOR_EXTENT = 128;
		if (minor_extent > ASSUMED_MINOR_EXTENT)
		{
			log << "\nWarning! Minor extent (" << minor_extent << ") larger than assumed minor extent (" << ASSUMED_MINOR_EXTENT << ")";
		}
		utils::small_vector<int, ASSUMED_MINOR_EXTENT> roll_landing_point(minor_extent, major_range.front());

		auto place_blocker = [&roll_landing_point, is_vertical, uphill](const Coords& coord)
			{
				const Coords next_free_space = coord + Coords::dir(uphill);
				const int major_coord = is_vertical ? next_free_space.y : next_free_space.x;
				const int minor_coord = is_vertical ? next_free_space.x : next_free_space.y;
				roll_landing_point[minor_coord] = major_coord;
			};

		auto get_roll_location = [&roll_landing_point, is_vertical, uphill](Coords coord)
			{
				int& major_coord = is_vertical ? coord.y : coord.x;
				const int minor_coord = is_vertical ? coord.x : coord.y;
				major_coord = roll_landing_point[minor_coord];
				return coord;
			};

		for (auto major_idx : major_range)
		{
			for (auto minor_idx : utils::int_range{ minor_extent })
			{
				const int x = is_vertical ? minor_idx : major_idx;
				const int y = is_vertical ? major_idx : minor_idx;
				const Coords loc{ x,y };
				const Tile tile = grid.at(loc);

				switch (tile)
				{
				case Tile::round:
					{
						const Coords roll_to = get_roll_location(loc);
						place_blocker(roll_to);
						grid.at(loc) = Tile::space;
						grid.at(roll_to) = Tile::round;
					}
					break;
				case Tile::block:
					place_blocker(loc);
					break;
				case Tile::space:
					// Do nothing.
					break;
				default:
					AdventUnreachable();
					break;
				}
			}
		}
		return grid;
	}

	std::size_t score_row(const RowView& row)
	{
		std::size_t num_rounds = stdr::count(row, Tile::round);
		return num_rounds;
	}

	std::size_t score_grid(const Grid& grid)
	{
		auto transform = [](const RowView& row, std::size_t mul)
			{
				const std::size_t score = score_row(row);
				const std::size_t result = mul * score;
				return result;
			};

		auto row_range = utils::grid_helpers::row_view{ grid };
		auto mul_range = utils::int_range{ grid.get_max_point().y , -1, -1 };

		const std::size_t result = std::transform_reduce(begin(row_range), end(row_range), begin(mul_range),
			std::size_t{0},	std::plus<std::size_t>{}, transform);
		return result;
	}

	std::size_t solve_p1(std::istream& input)
	{
		Grid grid = parse_grid(input);
		log << "\nInput grid:\n" << grid;
		grid = tilt_grid(std::move(grid),Direction::down);
		log << "\n\nOutput grid:\n" << grid;
		const std::size_t result = score_grid(grid);
		log << "\nScore = " << result << '\n';
		return result;
	}
}

namespace
{
	using CycleNumber = uint64_t;
	using GridHash = uint64_t;

	struct GridRecord
	{
		Grid grid;
		CycleNumber cycle;
	};

	GridHash hash_grid(const Grid& grid)
	{
		auto grid_view = utils::grid_helpers::elem_view{ grid };
		auto acc_fn = [](GridHash hash, Tile tile)
			{
				return 3 * hash + static_cast<GridHash>(tile);
			};
		return std::accumulate(begin(grid_view), end(grid_view), GridHash{ 0 }, acc_fn);
	}

	Grid cycle_grid(Grid grid)
	{
		constexpr std::array<Direction, 4> directions{ Direction::down,Direction::left,Direction::up,Direction::right };
		for (Direction d : directions)
		{
			grid = tilt_grid(std::move(grid), d);
		}
		return grid;
	}

	Grid cycle_grid	(Grid grid, CycleNumber num_cycles)
	{
		std::multimap<GridHash, GridRecord> previous_steps;
		uint64_t num_hash_collisions = 0;
		for (auto i : utils::int_range{ num_cycles })
		{
			grid = cycle_grid(std::move(grid));
			const GridHash hash = hash_grid(grid);
			log << "\nAfter cycle " << i  << ":" << grid
				<< "\nHash=" << hash << " Score=" << score_grid(grid) << "\n";
			const auto [matching_hash_begin, matching_hash_end] = previous_steps.equal_range(hash);
			for (auto it = matching_hash_begin; it != matching_hash_end; ++it)
			{
				const auto& [hash, record] = *it;
				if (record.grid != grid)
				{
					++num_hash_collisions;
					continue;
				}
				const CycleNumber loop_start = record.cycle;
				const CycleNumber loop_length = i - loop_start;
				const CycleNumber cycles_remaining = num_cycles - i - 1;
				const CycleNumber finish_point_in_loop = cycles_remaining % loop_length;
				const CycleNumber result_cycle_number = loop_start + finish_point_in_loop;
				log << "\nEnding found at cycle " << result_cycle_number
					<< " which is in a " << loop_length
					<< " cycle loops starting at " << loop_start
					<< "\nNum hash collisions: " << num_hash_collisions;
				if (result_cycle_number == i) return grid;
				auto range_view = [](const std::pair<GridHash, GridRecord>& elem)
					{
						return elem.second.cycle;
					};
				const auto result_entry = stdr::find(previous_steps, result_cycle_number, range_view);
				AdventCheck(result_entry != end(previous_steps));
				return result_entry->second.grid;
			}
			previous_steps.insert(matching_hash_begin, std::pair{ hash, GridRecord{grid, i} });
		}
		return grid;
	}

	std::size_t solve_p2(std::istream& input)
	{
		Grid grid = parse_grid(input);
		log << "\nStarting grid:" << grid << '\n';
		grid = cycle_grid(std::move(grid), 1000000000);
		return score_grid(grid);
	}
}

ResultType testcase_fourteen_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType testcase_fourteen_p2(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_fourteen_p1()
{
	auto input = advent::open_puzzle_input(14);
	return solve_p1(input);
}

ResultType advent_fourteen_p2()
{
	auto input = advent::open_puzzle_input(14);
	return solve_p2(input);
}

#undef DAY14DBG
#undef ENABLE_DAY14DBG
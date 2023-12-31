#include "advent23.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY23DBG 1
#ifdef NDEBUG
#define DAY23DBG 0
#else
#define DAY23DBG ENABLE_DAY23DBG
#endif

#if DAY23DBG
	#include <iostream>
#endif

namespace
{
#if DAY23DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "grid.h"
#include "enums.h"
#include "comparisons.h"

#include <set>
#include <map>

namespace
{
	enum class Tile : char
	{
		path = '.',
		forest = '#',
		slope_north = '^',
		slope_east = '>',
		slope_south = 'v',
		slope_west = '<'
	};

	template <AdventDay day>
	Tile to_tile(char c)
	{
		AdventCheck(std::string_view{ ".#<>^v" }.find(c) != std::string_view::npos);
		if constexpr (day == AdventDay::one)
		{
			return static_cast<Tile>(c);
		}
		if constexpr (day == AdventDay::two)
		{
			return c == '#' ? Tile::forest : Tile::path;
		}
		AdventUnreachable();
		return Tile::forest;
	}

	using CoordType = int16_t;
	using Grid = utils::grid<Tile>;
	using Coords = utils::basic_coords<CoordType>;

	void check_idx(std::integral auto idx)
	{
		AdventCheck(utils::range_contains_exc(idx, 0, std::numeric_limits<CoordType>::max()));
	}

	template <std::integral T>
	void check_coords(const utils::basic_coords<T>& c)
	{
		check_idx(c.x);
		check_idx(c.y);
	}

	template <AdventDay day>
	Grid parse_grid(std::istream& input)
	{
		Grid result = utils::grid_helpers::build(input, to_tile<day>);
		check_coords(result.get_max_point());
		return result;
	}

	Coords get_only_path_on_row(const Grid& grid, int row_idx)
	{
		check_idx(row_idx);
		auto row_view = utils::grid_helpers::get_row_elem_view(grid, row_idx);
		AdventCheck(stdr::count(row_view, Tile::path) == 1);
		auto find_result = stdr::find(row_view, Tile::path);
		AdventCheck(find_result != end(row_view));
		auto col_idx = std::distance(begin(row_view), find_result);
		check_idx(col_idx);
		return Coords{ static_cast<CoordType>(col_idx), static_cast<CoordType>(row_idx) };
	}

	Coords get_start_position(const Grid& grid)
	{
		return get_only_path_on_row(grid, grid.get_max_point().y-1);
	}

	Coords get_finish_location(const Grid& grid)
	{
		return get_only_path_on_row(grid, 0);
	}

	using Path = std::vector<bool>;
	using MemoState = std::pair<Path, Coords>;
	using Memo = std::map<MemoState, int64_t>;

	constexpr int64_t NO_PATH = -1;

	int64_t get_remaining_path_length(const Grid& grid, Memo& memo, Path& partial_path, Coords position, Coords end_pos)
	{
		if (position == end_pos) return 0u;

		// Check we're inbounds
		if (!grid.is_on_grid(position)) return NO_PATH;

		// Check we're on a path
		const Tile tile = grid.at(position);
		if (tile == Tile::forest) return NO_PATH;

		const std::size_t path_idx = position.x + grid.get_max_point().x * position.y;

		// Check for retraced steps
		if (partial_path[path_idx]) return NO_PATH;

		// Check the memo.
		MemoState memostate{ partial_path,position };
		const auto memo_check = memo.find(memostate);
		if (memo_check != end(memo)) return memo_check->second;

		using Dir = utils::direction;
		// Get permitted directions
		const utils::small_vector<Dir, 4> allowed_dirs = [tile]()
		{
			utils::small_vector<Dir, 4> result;
			switch (tile)
			{
			case Tile::slope_east:
				result.push_back(Dir::right);
				break;
			case Tile::slope_north:
				result.push_back(Dir::up);
				break;
			case Tile::slope_south:
				result.push_back(Dir::down);
				break;
			case Tile::slope_west:
				result.push_back(Dir::left);
				break;
			case Tile::path:
				result.assign({ Dir::up,Dir::left,Dir::right,Dir::down });
				break;
			}
			return result;
		}();

		partial_path[path_idx] = true;

		auto check_next_step = [&grid,&memo,&partial_path,&position,&end_pos](Dir d)
		{
			const Coords next_position = position + Coords::dir(d);
			return get_remaining_path_length(grid, memo, partial_path, next_position, end_pos);
		};

		const int64_t sub_path = std::transform_reduce(begin(allowed_dirs), end(allowed_dirs), int64_t{ 0 },
			utils::Larger<int64_t>{}, check_next_step);
		const int64_t result = sub_path + 1;
		partial_path[path_idx] = false;

		memo.insert(std::pair{ memostate,result });
		return result;
	}

	std::size_t get_path_length(const Grid& grid)
	{
		const Coords start_pos = get_start_position(grid);
		const Coords finish_pos = get_finish_location(grid);
		Path path;
		Memo memo;
		path.resize(grid.get_max_point().x * grid.get_max_point().y);
		return get_remaining_path_length(grid, memo, path, start_pos, finish_pos);
	}

	template <AdventDay day>
	std::size_t solve_generic(std::istream& input)
	{
		const Grid grid = parse_grid<day>(input);
		return get_path_length(grid);
	}

	std::size_t solve_p1(std::istream& input)
	{
		return solve_generic<AdventDay::one>(input);
	}

	std::size_t solve_p2(std::istream& input)
	{
		return solve_generic<AdventDay::two>(input);
	}
}

ResultType testcase_twentythree_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType testcase_twentythree_p2(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_twentythree_p1()
{
	auto input = advent::open_puzzle_input(23);
	return solve_p1(input);
}

ResultType advent_twentythree_p2()
{
	auto input = advent::open_puzzle_input(23);
	return solve_p2(input);
}

#undef DAY23DBG
#undef ENABLE_DAY23DBG
#include "advent13.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY13DBG 1
#ifdef NDEBUG
#define DAY13DBG 0
#else
#define DAY13DBG ENABLE_DAY13DBG
#endif

#if DAY13DBG
	#include <iostream>
#endif

namespace
{
#if DAY13DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "grid.h"
#include "istream_block_iterator.h"

namespace
{
	enum class Tile : char
	{
		ash = '.',
		rock = '#'
	};

	Tile to_tile(char c)
	{
		AdventCheck(c == '.' || c == '#');
		return static_cast<Tile>(c);
	}

	using Grid = utils::grid<Tile>;

	Grid parse_grid(std::string_view input)
	{
		std::istringstream iss{std::string{input}};
		return utils::grid_helpers::build(iss,to_tile);
	}

	bool do_lines_match(const auto& view, std::size_t first_idx, std::size_t second_idx)
	{
		const auto first = view[first_idx];
		const auto second = view[second_idx];
		return first == second;
	}

	std::optional<std::size_t> get_forward_line_of_symmetry(const auto& view)
	{
		const auto left_row = view.front();
		for(std::size_t idx : utils::int_range{std::size_t{1},view.size().reverse()})
		{
			if(do_lines_match(0,idx))
			{
				auto verify_offset = [&view,idx](std::size_t offset)
				{
					AdventCheck(offset < idx/2);
					const auto low_offset = offset;
					const auto high_offset = idx - offset;
					return do_lines_match(view,low_offset,high_offset);
				};
				if(std::ranges::all_of(utils::int_range{1u,idx/2},verify_offset))
				{
					return idx / 2;
				}
			}
		}
		return std::nullopt;
	}

	std::optional<std::size_t> get_line_of_symmetry(const auto& view)
	{
		const auto forward_check = get_forward_line_of_symmetry(view);
		if(forward_check.has_value())
		{
			return forward_check;
		}
		const auto backward_check = get_forward_line_of_symmetry(view.reverse());
		return backward_check;
	}

	std::optional<std::size_t> get_horizontal_line_of_symmetry(const Grid& grid)
	{
		const auto rows = utils::grid_helpers::row_view{grid};
		return get_line_of_symmetry(rows);
	}

	std::optional<std::size_t> get_vertical_line_of_symmetry(const Grid& grid)
	{
		const auto columns = utils::grid_helpers::column_view{ grid };
		return get_line_of_symmetry(columns);
	}

	std::size_t get_score(const Grid& grid, std::size_t horizontal_mul , std::size_t vertical_mul)
	{
		const auto row_result = get_horizontal_line_of_symmetry(grid);
		if(row_result.has_value())
		{
			return horizontal_mul * row_result.value();
		}
		const auto column_result = get_vertical_line_of_symmetry(grid);
		AdventCheck(column_result.has_value());
		return vertical_mul * column_result.value();
	}

	std::size_t solve_p1(std::istream& input)
	{
		auto transform_fn = [](std::string_view block)
		{
			const Grid grid = parse_grid(block);
			return get_score(grid,100,1);
		};

		using IBI = utils::istream_line_iterator;
		return std::transform_reduce(IBI{input},IBI{},std::size_t{0},std::plus<std::size_t>{},transform_fn);
	}
}

namespace
{
	int solve_p2(std::istream& input)
	{
		return 0;
	}
}

ResultType testcase_thirteen_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType testcase_thirteen_p2(std::istream& input)
{
	return solve_p1(input);
}

ResultType advent_thirteen_p1()
{
	auto input = advent::open_puzzle_input(13);
	return solve_p1(input);
}

ResultType advent_thirteen_p2()
{
	auto input = advent::open_puzzle_input(13);
	return solve_p2(input);
}

#undef DAY13DBG
#undef ENABLE_DAY13DBG
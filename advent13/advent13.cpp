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
	constexpr std::size_t NO_REFLECTION = std::numeric_limits<std::size_t>::max();
	constexpr std::size_t HORIZONTAL_MUL = 100;
	constexpr std::size_t VERTICAL_MUL = 1;

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

	std::ostream& operator<<(std::ostream& oss, Tile t)
	{
		oss << static_cast<char>(t);
		return oss;
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
		const bool matches = (first == second);
		//log << "\nCmp [" << first_idx << "]'" << first << "' == [" << second_idx << "]'" << second << "' = " << (matches ? "matches" : "no match");
		return first == second;
	}

	bool has_reflection_at(const auto& view, std::size_t first_idx)
	{
		std::ptrdiff_t low = first_idx - 1;
		std::ptrdiff_t high = first_idx + 2;
		while (low >= 0u && high < std::ssize(view))
		{
			if (!do_lines_match(view, low, high))
			{
				return false;
			}
			--low;
			++high;
		}
		return true;
	}

	std::size_t get_line_of_symmetry(const auto& view, std::size_t ignore_reflection)
	{
		if (view.empty()) return NO_REFLECTION;
		for (auto first_idx : utils::int_range{ view.size() -1 })
		{
			const auto second_idx = first_idx + 1;
			if (second_idx == ignore_reflection) continue;
			if (do_lines_match(view, first_idx, second_idx) && has_reflection_at(view, first_idx))
			{
				return second_idx;
			}
		}
		return NO_REFLECTION;
	}

	std::size_t get_horizontal_line_of_symmetry(const Grid& grid, std::size_t ignore_reflection)
	{
		const auto rows = utils::grid_helpers::row_view{grid};
		return get_line_of_symmetry(rows, ignore_reflection / HORIZONTAL_MUL);
	}

	std::size_t get_vertical_line_of_symmetry(const Grid& grid, std::size_t ignore_reflection)
	{
		const auto columns = utils::grid_helpers::column_view{ grid };
		return get_line_of_symmetry(columns, ignore_reflection / VERTICAL_MUL);
	}

	std::size_t get_score(const Grid& grid, std::size_t ignore_reflection)
	{
		const auto row_result = get_horizontal_line_of_symmetry(grid, ignore_reflection);
		if(row_result != NO_REFLECTION)
		{
			return HORIZONTAL_MUL * row_result;
		}
		const auto column_result = get_vertical_line_of_symmetry(grid, ignore_reflection);
		if (column_result != NO_REFLECTION)
		{
			return VERTICAL_MUL * column_result;
		}
		return NO_REFLECTION;
	}

	std::size_t get_score_p1(const Grid& grid)
	{
		return get_score(grid, NO_REFLECTION);
	}

	std::size_t solve_p1(std::istream& input)
	{
		auto transform_fn = [](std::string_view block)
		{
			const Grid grid = parse_grid(block);
			const auto score = get_score_p1(grid);
			log << "\nBlock:\n" << block << "\nScore=" << score << "\n";
			AdventCheck(score != NO_REFLECTION);
			return score;
		};

		using IBI = utils::istream_block_iterator;
		return std::transform_reduce(IBI{input},IBI{},std::size_t{0},std::plus<std::size_t>{},transform_fn);
	}
}

namespace
{
	Tile invert(Tile t)
	{
		switch (t)
		{
		case Tile::ash:
			return Tile::rock;
		case Tile::rock:
			return Tile::ash;
		}
		AdventUnreachable();
		return Tile::ash;
	}
	std::size_t get_score_p2(Grid grid)
	{
		const std::size_t reflection_to_ignore = get_score(grid, NO_REFLECTION);
		for (const utils::coords& c : utils::coords_iterators::elem_range{grid.get_max_point()})
		{
			Tile& elem = grid.at(c);
			elem = invert(elem);
			const std::size_t result = get_score(grid, reflection_to_ignore);
			if (result != NO_REFLECTION)
			{
				log << "\nFlipped tile at " << c;
				return result;
			}
			elem = invert(elem);
		}
		return NO_REFLECTION;
	}

	std::size_t solve_p2(std::istream& input)
	{
		auto transform_fn = [](std::string_view block)
			{
				Grid grid = parse_grid(block);
				log << "\nSolving grid:\n" << block;
				const auto score = get_score_p2(std::move(grid));
				log << "\nBlock:\n" << block << "\nScore=" << score << "\n";
				AdventCheck(score != NO_REFLECTION);
				return score;
			};

		using IBI = utils::istream_block_iterator;
		return std::transform_reduce(IBI{ input }, IBI{}, std::size_t{ 0 }, std::plus<std::size_t>{}, transform_fn);
	}
}

ResultType testcase_thirteen_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType testcase_thirteen_p2(std::istream& input)
{
	return solve_p2(input);
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
#include "advent11.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY11DBG 1
#ifdef NDEBUG
#define DAY11DBG 0
#else
#define DAY11DBG ENABLE_DAY11DBG
#endif

#if DAY11DBG
	#include <iostream>
#endif

namespace
{
#if DAY11DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "coords.h"
#include "istream_line_iterator.h"
#include "small_vector.h"
#include "int_range.h"
#include "sorted_vector.h"

#include <algorithm>

namespace
{
	using CoordType = std::size_t;
	using Galaxy = utils::basic_coords<CoordType>;
	using GalaxyContainer = utils::sorted_vector<Galaxy,std::less<Galaxy>,460>;

	constexpr char GALAXY = '#';

	GalaxyContainer parse_map(std::istream& input, CoordType scale_factor)
	{
		using CoordSet = utils::sorted_vector<CoordType,std::less<CoordType>,140>;

		// Get the initial map.
		GalaxyContainer result;
		CoordType max_x = 0;
		CoordType max_y = 0;
		CoordSet x_vals_with_galaxy;
		CoordSet y_vals_with_galaxy;
		for(std::string_view line : utils::istream_line_range{input})
		{
			max_x = std::max(line.size(),max_x);
			for(std::size_t x : utils::int_range{line.size()})
			{
				const char c = line[x];
				if(c == GALAXY)
				{
					const Galaxy loc{x , max_y};
					result.push_back(loc);
					if(!x_vals_with_galaxy.contains(x)) x_vals_with_galaxy.push_back(x);
					if(!y_vals_with_galaxy.contains(max_y)) y_vals_with_galaxy.push_back(max_y);
				}
			}
			++max_y;
		}
		
		CoordSet empty_x_coords;
		CoordSet empty_y_coords;
		std::ranges::set_difference(utils::int_range{max_x+1}, x_vals_with_galaxy, std::back_inserter(empty_x_coords));
		std::ranges::set_difference(utils::int_range{max_y+1}, y_vals_with_galaxy, std::back_inserter(empty_y_coords));

		auto transform_fn = [&empty_x_coords,&empty_y_coords,scale_factor](const Galaxy& c)
		{
			const std::size_t x_offset = std::ranges::count_if(empty_x_coords, [x = c.x](CoordType coord){ return coord < x; });
			const std::size_t y_offset = std::ranges::count_if(empty_y_coords, [y = c.y](CoordType coord){ return coord < y; });
			return c + ((scale_factor-1) * Galaxy{x_offset,y_offset});
		};

		for(auto x : empty_x_coords)
		{
			log << "\nNo galaxy on x=" << x;
		}
		for (auto y : empty_y_coords)
		{
			log << "\nNo galaxy on y=" << y;
		}
		for(const auto galaxy : result)
		{
			log << "\nGalaxy at [" << galaxy << ']';
		}
		log << "\n... Converting with scale factor " << scale_factor << " ...";
		std::ranges::transform(result,begin(result),transform_fn);
		for (const auto galaxy : result)
		{
			log << "\nGalaxy at [" << galaxy << ']';
		}

		return result;
	}

	CoordType get_distance_between(const Galaxy& a, const Galaxy& b)
	{
		const CoordType result = a.manhatten_distance(b);
		//log << "\nDistance from [" << a << "] to [" << b << "] is " << result;
		return result;
	}

	CoordType get_sum_of_distance(const GalaxyContainer& galaxies)
	{
		CoordType result = 0;
		std::size_t num_pairs = 0;
		for(auto it_a = begin(galaxies);it_a != end(galaxies);++it_a)
		{
			const Galaxy& galaxy_a = *it_a;
			for(auto it_b = it_a+1;it_b!=end(galaxies);++it_b)
			{
				const Galaxy& galaxy_b = *it_b;
				result += get_distance_between(galaxy_a,galaxy_b);
				++num_pairs;
			}
		}
		log << "\nChecked " << num_pairs << " pairs";
		return result;
	}

	CoordType solve_generic(std::istream& input,std::size_t scale_factor)
	{
		const GalaxyContainer galaxies = parse_map(input, scale_factor);
		return get_sum_of_distance(galaxies);
	}

	CoordType solve_p1(std::istream& input)
	{
		return solve_generic(input,2);
	}

	CoordType solve_p2(std::istream& input)
	{
		return solve_generic(input, 1000000);
	}

	std::istringstream get_testcase_input()
	{
		return std::istringstream{
R"(...#......
.......#..
#.........
..........
......#...
.#........
.........#
..........
.......#..
#...#.....)" };
	}
}

ResultType eleven_internal::testcase(std::size_t factor)
{
	auto input = get_testcase_input();
	return solve_generic(input, factor);
}

ResultType advent_eleven_p1()
{
	auto input = advent::open_puzzle_input(11);
	return solve_p1(input);
}

ResultType advent_eleven_p2()
{
	auto input = advent::open_puzzle_input(11);
	return solve_p2(input);
}

#undef DAY11DBG
#undef ENABLE_DAY11DBG
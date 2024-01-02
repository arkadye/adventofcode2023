#include "advent24.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY24DBG 1
#ifdef NDEBUG
#define DAY24DBG 0
#else
#define DAY24DBG ENABLE_DAY24DBG
#endif

#if DAY24DBG
	#include <iostream>
#endif

namespace
{
#if DAY24DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "coords3d.h"
#include "parse_utils.h"
#include "to_value.h"
#include "istream_line_iterator.h"
#include "range_contains.h"

#include <vector>

namespace
{
	using iCoords = utils::coords3d<int64_t>;
	using fCoords = utils::coords3d<double>;

	double dbl(auto i)
	{
		return static_cast<double>(i);
	}

	struct Particle
	{
		iCoords position;
		iCoords velocity;
	};

	std::ostream& operator<<(std::ostream& oss, const Particle& p)
	{
		oss << "P=[" << p.position << "] V=[" << p.velocity << ']';
		return oss;
	}

	iCoords parse_coords(std::string_view sv, int64_t offset)
	{
		auto [xs, ys, zs] = utils::get_string_elements(sv, ',', 0, 1, 2);
		auto val = [offset](std::string_view sv)
		{
			return utils::to_value<int64_t>(sv) - offset;
		};

		return iCoords{ val(xs), val(ys), val(zs) };
	}

	Particle parse_particle(std::string_view line, int64_t offset)
	{
		auto [pos_str, vel_str] = utils::split_string_at_first(line, '@');
		const  Particle result{ parse_coords(pos_str, offset), parse_coords(vel_str, 0) };
		AdventCheck(result.velocity != iCoords{ 0 });
		return result;
	}

	std::vector<Particle> parse_particle_list(std::istream& input, int64_t offset)
	{
		std::vector<Particle> result;
		result.reserve(300);
		auto transform_fn = [offset](std::string_view sv)
		{
			return parse_particle(sv, offset);
		};
		stdr::transform(utils::istream_line_range{ input }, std::back_inserter(result), transform_fn);
		return result;
	}

	struct IntersectionResult
	{
		fCoords intersection_point;
		double time_a = 0.0;
		double time_b = 0.0;
	};

	// Put intersection time in z axis.
	std::optional<IntersectionResult> get_2d_intersection(Particle a, Particle b)
	{
		a.position.z = a.velocity.z = b.position.z = b.velocity.z = 0;
		// y = mx + c
		// m = vy / vx
		// c = y(init) - mx(init)

		// Need a special case for vx = 0

		// For two lines (y = m0 * x + c0; y = m1 * x + c1) solve such that x & y match.
		// m0 * x + c0 = m1 * x + c1
		// m0 * x - m1 * x + c0 - c1 = 0
		// x * (m0 - m1) + (c0 - c1) = 0
		// x = (c1 - c0) / (m0 - m1)

		// If m0 == m1 there is no solution

		AdventCheckMsg(a.velocity.x != 0 && b.velocity.x != 0, "Time to write that special case for vx=0");

		auto get_divisor_for_m = [](const Particle& p) { return std::gcd(p.velocity.x, p.velocity.y); };
		const int64_t div_a = get_divisor_for_m(a);
		const int64_t div_b = get_divisor_for_m(b);

		if (a.velocity / div_a == b.velocity / div_b)
		{
			// Lines are parallel
			return std::nullopt;
		}

		auto get_m = [](const Particle& p) { return dbl(p.velocity.y) / dbl(p.velocity.x); };
		auto get_c = [](const Particle& p, double m) { return dbl(p.position.y) - m * dbl(p.position.x); };

		const double m_a = get_m(a);
		const double m_b = get_m(b);
		AdventCheck(std::abs(m_a - m_b) > 0.0000000001);
		const double c_a = get_c(a, m_a);
		const double c_b = get_c(b, m_b);
		
		auto get_y = [](double m, double x, double c) { return m * x + c; };

		const double x_cross = (c_b - c_a) / (m_a - m_b);
		const double y_cross = get_y(m_a, x_cross, c_a);
		auto roughly_even = [](double a, double b)
		{
			return std::abs(a - b) < 0.00001;
		};
		AdventCheck(roughly_even(y_cross, get_y(m_b, x_cross, c_b)));

		// Set the time.
		auto calculate_t = [](double target, int64_t init, int64_t v)
		{
			const double delta = target - dbl(init);
			return delta / dbl(v);
		};

		const double ta = calculate_t(x_cross, a.position.x, a.velocity.x);
		const double tb = calculate_t(x_cross, b.position.x, b.velocity.x);
		AdventCheck(roughly_even(ta, calculate_t(y_cross, a.position.y, a.velocity.y)));
		AdventCheck(roughly_even(tb, calculate_t(y_cross, b.position.y, b.velocity.y)));

		IntersectionResult result;
		result.intersection_point = fCoords{ x_cross, y_cross, 0.0 };
		result.time_a = ta;
		result.time_b = tb;
		return result;
	}

	bool intersects_within_range(const Particle& a, const Particle& b, double max_range)
	{
		const auto result = get_2d_intersection(a, b);
		log << '\n' << a << " and " << b;
		if (result.has_value())
		{
			const auto& full_val = result.value();
			if (full_val.time_a >= 0.0 && full_val.time_b >= 0.0f)
			{
				const auto& val = full_val.intersection_point;
				log << " intersect at " << val << '.';
				return utils::range_contains_inc(val.x, 0.0, max_range) && utils::range_contains_exc(val.y, 0.0, max_range);
			}
			log << " intersects in the past.";
			return false;
		}
		log << " do not intersect.";
		return false;
	}

	int count_intersections(const std::vector<Particle>& particles, int64_t max_range)
	{
		int result = 0;
		for (auto outer = begin(particles); outer != end(particles); ++outer)
		{
			auto pred = [&a = *outer, drange = dbl(max_range)](const Particle& b)
				{
					return intersects_within_range(a, b, drange);
				};

			const auto additional = std::count_if(outer + 1, end(particles), pred);
			result += static_cast<int>(additional);
		}
		return result;
	}

	int solve_p1(std::istream& input, int64_t min_range, int64_t max_range)
	{
		AdventCheck(min_range < max_range);
		const std::vector<Particle> particles = parse_particle_list(input, min_range);
		return count_intersections(particles, max_range - min_range);
	}
}

namespace
{
	int solve_p2(std::istream& input)
	{
		return 0;
	}
}

ResultType testcase_twentyfour_p1(std::istream& input)
{
	return solve_p1(input, 7, 27);
}

ResultType advent_twentyfour_p1()
{
	auto input = advent::open_puzzle_input(24);
	return solve_p1(input, 200000000000000, 400000000000000);
}

ResultType advent_twentyfour_p2()
{
	auto input = advent::open_puzzle_input(24);
	return solve_p2(input);
}

#undef DAY24DBG
#undef ENABLE_DAY24DBG
#pragma once

#include <compare>
#include <numeric>
#include <cmath>
#include <array>
#include <iostream>

#include "../advent/advent_assert.h"
#include "split_string.h"
#include "to_value.h"
#include "int_range.h"

namespace utils
{

	enum class direction : char
	{
		up = 0,
		right,
		down,
		left
	};

	inline constexpr bool is_horizontal(direction dir)
	{
		switch (dir)
		{
		case direction::right:
		case direction::left:
			return true;
		case direction::up:
		case direction::down:
			return false;
		}
		AdventUnreachable();
		return false;
	}

	inline constexpr bool is_vertical(direction dir)
	{
		return !is_horizontal(dir);
	}

	enum class turn_dir : char
	{
		clockwise,
		anticlockwise
	};

	inline direction rotate(direction dir, turn_dir td)
	{
		int increment = 0;
		switch (td)
		{
		case turn_dir::clockwise:
			increment = 1;
			break;
		case turn_dir::anticlockwise:
			increment = -1;
			break;
		}
		int dir_i = static_cast<int>(dir);
		dir_i += increment;
		constexpr int NUM_DIRECTIONS = 4;
		dir_i += NUM_DIRECTIONS;
		dir_i %= NUM_DIRECTIONS;
		return static_cast<direction>(dir_i);
	}

	inline direction turn_around(direction dir)
	{
		switch (dir)
		{
		case direction::up:
			return direction::down;
		case direction::right:
			return direction::left;
		case direction::down:
			return direction::up;
		case direction::left:
			return direction::right;
		}
		AdventUnreachable();
		return dir;
	}

	template <typename T>
	struct basic_coords
	{
		T x, y;
		auto operator<=>(const basic_coords&) const noexcept = default;

		auto direction_to(const basic_coords& other) const noexcept;
		T size_squared() const noexcept { return x * x + y * y; }
		double angle() const noexcept;
		auto reduce() const noexcept;
		auto manhatten_distance() const noexcept
		{
			if constexpr (std::is_unsigned_v<T>)
			{
				return x + y;
			}
			if constexpr (std::is_signed_v<T>)
			{
				return std::abs(x) + std::abs(y);
			}
		}
		auto manhatten_distance(const basic_coords& other) const noexcept;
		constexpr basic_coords(T x_, T y_) : x{ x_ }, y{ y_ }{}
		constexpr explicit basic_coords(T init) : basic_coords{ init,init } {}
		constexpr basic_coords() : basic_coords{ 0 } {}

		basic_coords& operator=(const basic_coords&) noexcept = default;

		template <typename T2>
		basic_coords& operator+=(const basic_coords<T2>& other) noexcept
		{
			x += other.x;
			y += other.y;
			return *this;
		}

		template <typename T2> 
		basic_coords& operator-=(const basic_coords<T2>& other) noexcept
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}

		template <typename RHSTYPE>  requires std::is_arithmetic_v<RHSTYPE>
		basic_coords& operator*=(RHSTYPE other) noexcept
		{
			x *= other;
			y *= other;
			return *this;
		}

		template <typename RHSTYPE> requires std::is_arithmetic_v<RHSTYPE>
		basic_coords& operator/=(RHSTYPE other) noexcept
		{
			AdventCheck(other != static_cast<RHSTYPE>(0));
			x /= other;
			y /= other;
			return *this;
		}

		T operator[](std::size_t idx) const noexcept
		{
			AdventCheck(idx < 2u);
			switch(idx)
			{
			case 0: return x;
			case 1: return y;
			default:
				AdventUnreachable();
				break;
			}
			return static_cast<T>(0);
		}

		T& operator[](std::size_t idx) noexcept
		{
			AdventCheck(idx < 2u);
			switch (idx)
			{
			case 0: return x;
			case 1: return y;
			default:
				AdventUnreachable();
				break;
			}
			return x;
		}

		static basic_coords up() noexcept { return basic_coords{ 0,1 }; }
		static basic_coords down() noexcept { return basic_coords{ 0,-1 }; }
		static basic_coords left() noexcept { return basic_coords{ -1,0 }; }
		static basic_coords right() noexcept { return basic_coords{ 1,0 }; }
		static basic_coords dir(direction dir) noexcept
		{
			switch (dir)
			{
			case direction::up:
				return up();
			case direction::down:
				return down();
			case direction::left:
				return left();
			case direction::right:
				return right();
			default:
				AdventCheck(false);
				return basic_coords{};
			}
		}

		// Return cardinal directions, starting from North and moving clockwise.
		auto neighbours() const
		{
			return std::array<basic_coords, 4>
			{
				*this + up(),
				*this + down(),
				*this + right(),
				*this + left()
			};
		}

		// Returns all eight compass points starting from North and moving clockwise.
		auto neighbours_plus_diag() const
		{
			return std::array<basic_coords, 8>
			{
				*this + up(),
				*this + up() + right(),
				*this + right(),
				*this + right() + down(),
				*this + down(),
				*this + down() + left(),
				*this + left(),
				*this + left() + up()
			};
		}

		static basic_coords from_chars(std::string_view input)
		{
			basic_coords result;
			auto [x, y] = utils::split_string_at_first(input, ',');
			x = utils::trim_string(x);
			y = utils::trim_string(y);
			result.x = utils::to_value<T>(x);
			result.y = utils::to_value<T>(y);
			return result;
		}
	};

	/*template <std::integral T>
	inline auto coords_area_elem_range(basic_coords<T> start, basic_coords<T> finish)
	{
		const auto [min_x, max_x] = std::minmax(start.x, finish.x);
		const auto [min_y, max_y] = std::minmax(start.y, finish.y);
		const std::size_t width = static_cast<std::size_t>(max_x - min_x);
		const std::size_t height = static_cast<std::size_t>(max_y - min_y);
		const std::size_t size = height * width;
		auto deref = [start, finish, width, size](std::size_t idx)
			{
				AdventCheck(idx < size);
				const bool goes_right = start.x <= finish.x;
				const bool goes_up = start.y <= finish.y;
				const T x_offset = static_cast<T>(idx % width);
				const T y_offset = static_cast<T>(idx / width);
				const T x = goes_right ? start.x + x_offset : start.x - x_offset;
				const T y = goes_up ? start.y + y_offset : start.y - y_offset;
				return basic_coords<T>{x, y};
			};
		std::invoke_result_t<decltype(deref), std::size_t> x = basic_coords<T>{};
		return int_range_adaptor{ std::move(deref),size };
	}

	template <std::integral T>
	using coords_area_elem_range_t = decltype(coords_area_elem_range(basic_coords<T>{}, basic_coords<T>{}));

	template <std::integral T>
	inline coords_area_elem_range_t<T> coords_area_elem_range(basic_coords<T> finish)
	{
		return coords_area_elem_range<T>(basic_coords<T>{}, finish);
	}

	template <std::integral T>
	inline coords_area_elem_range_t<T> coords_row_range(basic_coords<T> start, basic_coords<T> finish)
	{
		AdventCheck(start.y == finish.y);
		++finish.y;
		return coords_area_elem_range<T>(start, finish);
	}

	template <std::integral T>
	inline coords_area_elem_range_t<T> coords_row_range(basic_coords<T> start, T x_finish)
	{
		return coords_row_range<T>(start, basic_coords<T>{x_finish, start.y});
	}

	template <std::integral T>
	inline coords_area_elem_range_t<T> coords_row_range(T x_start, basic_coords<T> finish)
	{
		return coords_row_range<T>(basic_coords<T>{x_start, finish.y}, finish);
	}

	template <std::integral T>
	inline coords_area_elem_range_t<T> coords_row_range(T x_start, T x_finish, T y_const)
	{
		return coords_row_range<T>(basic_coords<T>{x_start, y_const}, basic_coords<T>{x_finish, y_const});
	}

	template <std::integral T>
	inline auto coords_area_rows_range(basic_coords<T> start, basic_coords<T> finish)
	{
		const auto [min_y, max_y] = std::minmax(start.y, finish.y);
		const auto size = static_cast<std::size_t>(max_y - min_y);
		auto deref_fn = [start, finish, size](std::size_t idx)
			{
				AdventCheck(idx < size);
				const bool going_up = start.y <= finish.y;
				const T y = going_up ? start.y + idx : start.y - idx;
				return coords_row_range(start.x, finish.x, y);
			};
		return int_range_adaptor{ deref_fn, size };
	}

	template <std::integral T>
	using coords_area_rows_range_t = decltype(coords_area_rows_range<T>(basic_coords<T>{}, basic_coords<T>{}));

	template <std::integral T>
	inline coords_area_rows_range_t<T> coords_area_rows_range(basic_coords<T> finish)
	{
		return coords_area_rows_range(basic_coords<T>{}, finish);
	}

	template <std::integral T>
	inline coords_area_elem_range_t<T> coords_column_range(basic_coords<T> start, basic_coords<T> finish)
	{
		AdventCheck(start.x == finish.x);
		++finish.x;
		return coords_area_elem_range<T>(start, finish);
	}

	template <std::integral T>
	inline coords_area_elem_range_t<T> coords_column_range(basic_coords<T> start, T y_finish)
	{
		return coords_column_range<T>(start, basic_coords<T>{start.x,y_finish});
	}

	template <std::integral T>
	inline coords_area_elem_range_t<T> coords_column_range(T y_start, basic_coords<T> finish)
	{
		return coords_column_range<T>(basic_coords<T>{finish.x, y_start}, finish);
	}

	template <std::integral T>
	inline coords_area_elem_range_t<T> coords_column_range(T y_start, T y_finish, T x_const)
	{
		return coords_column_range<T>(basic_coords<T>{x_const, y_start}, basic_coords<T>{x_const, y_finish});
	}

	template <std::integral T>
	inline auto coords_area_columns_range(basic_coords<T> start, basic_coords<T> finish)
	{
		const auto [min_x, max_x] = std::minmax(start.x, finish.x);
		const auto size = static_cast<std::size_t>(max_x - min_x);
		auto deref_fn = [start, finish, size](std::size_t idx)
			{
				AdventCheck(idx < size);
				const bool going_right = start.x <= finish.x;
				const T x = going_right ? start.x + idx : start.x - idx;
				return coords_column_range(start.y, finish.y, x);
			};
		return int_range_adaptor{ deref_fn, size };
	}

	template <std::integral T>
	using coords_area_columns_range_t = decltype(coords_area_columns_range<T>(basic_coords<T>{}, basic_coords<T>{}));

	template <std::integral T>
	inline coords_area_columns_range_t<T> coords_area_columns_range(basic_coords<T> finish)
	{
		return coords_area_columns_range(basic_coords<T>{}, finish);
	}*/

	using coords = basic_coords<int32_t>;
	using coords64 = basic_coords<int64_t>;

	inline double pi()
	{
		return 4.0 * atan(1);
	}

	template <typename T>
	inline double basic_coords<T>::angle() const noexcept
	{
		double angle = std::atan2(x, -y);
		if (angle < 0.0) angle += (2.0 * pi());
		return angle;
	}


	template <typename T1, typename T2>
	inline auto operator-(const basic_coords<T1>& l, const basic_coords<T2>& r) noexcept
	{
		basic_coords result = l;
		result -= r;
		return result;
	}

	template <typename T1, typename T2>
	inline auto operator+(const basic_coords<T1>& l, const basic_coords<T2>& r) noexcept
	{
		basic_coords result = l;
		result += r;
		return result;
	}

	template <typename T, typename OtherType> requires std::is_arithmetic_v<OtherType>
	inline auto operator/(const basic_coords<T>& l, OtherType r) noexcept
	{
		basic_coords result = l;
		result /= r;
		return result;
	}

	template <typename T, typename OtherType> requires std::is_arithmetic_v<OtherType>
	inline auto operator*(const basic_coords<T>& l, OtherType r) noexcept
	{
		basic_coords result = l;
		result *= r;
		return result;
	}

	template <typename T, typename OtherType> requires std::is_arithmetic_v<OtherType>
	inline auto operator*(OtherType l, const basic_coords<T>& r) noexcept
	{
		return r * l;
	}

	template <typename T>
	inline auto basic_coords<T>::reduce() const noexcept
	{
		return size_squared() != 0 ? *this / std::gcd(x, y) : *this;
	}

	template <typename T>
	inline auto basic_coords<T>::direction_to(const basic_coords& other) const noexcept
	{
		const basic_coords direction = other - *this;
		if (direction.size_squared() == 0) return direction;
		return direction.reduce();
	}

	template <typename T>
	inline auto basic_coords<T>::manhatten_distance(const basic_coords& other) const noexcept
	{
		const T x_diff = (x < other.x ? other.x - x : x - other.x);
		const T y_diff = (y < other.y ? other.y - y : y - other.y);
		const basic_coords t{x_diff,y_diff};
		return t.manhatten_distance();
	}

	template <typename T>
	inline auto manhatten_distance(const basic_coords<T>& a, const basic_coords<T>& b)
	{
		return a.manhatten_distance(b);
	}

	template <typename T>
	inline std::ostream& operator<<(std::ostream& out, const basic_coords<T>& c)
	{
		out << c.x << " , " << c.y;
		return out;
	}

	inline std::ostream& operator<<(std::ostream& out, direction dir)
	{
		switch (dir)
		{
		case direction::up:
			out << "up";
			break;
		case direction::right:
			out << "right";
			break;
		case direction::down:
			out << "down";
			break;
		case direction::left:
			out << "left";
			break;
		default:
			AdventUnreachable();
			break;
		}
		return out;
	}

	template <typename T>
	inline std::istream& operator>>(std::istream& in, basic_coords<T>& c)
	{
		char mid = '\0';
		in >> c.x >> mid >> c.y;
		AdventCheck(mid == ',');
		return in;
	}
}
#pragma once

#include <compare>
#include <numeric>
#include <cmath>
#include <array>
#include <iostream>

#include "../advent/advent_assert.h"
#include "split_string.h"
#include "to_value.h"
#include "index_iterator.h"

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

	// Range of coords with a constant y
	template <std::integral INT>
	class coords_row_range
	{
		INT x_start;
		INT x_finish;
		INT y_const;
	public:
		// Typedefs
		using value_type = basic_coords<INT>;
		using reference_type = basic_coords<INT>;
		using pointer_type = basic_coords<INT>*;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		constexpr coords_row_range() noexcept : coords_column_range{0,0,0}{}
		constexpr coords_row_range(INT start_x, INT finish_x, INT const_y) noexcept : x_start{ start_x }, x_finish{ finish_x }, y_const{ const_y }
		{}

		constexpr coords_row_range(const basic_coords<INT>& start, INT finish) noexcept : coords_row_range{ start.x,finish,start.y } {}
		constexpr coords_row_range(const basic_coords<INT>& start, const basic_coords<INT>& finish) noexcept : coords_row_range{ start,finish.x, start.y }
		{
			AdventCheck(start.y == finish.y);
		}
		constexpr coords_row_range(INT start, const basic_coords<INT>& finish) noexcept : coords_row_range{ start,finish.x,finish.y } {}
		explicit constexpr coords_row_range(const basic_coords<INT>& finish) noexcept : coords_row_range{ INT{0},finish } {}

		constexpr coords_row_range& operator=(const coords_row_range&) = default;

		constexpr value_type operator[](std::size_t idx) const noexcept
		{
			AdventCheck(idx < size());
			const bool going_forward = x_start <= x_finish;
			const INT x_out = (going_forward ? x_start + idx : x_start - idx);
			return value_type{ x_out, y_const };
		}

		constexpr size_type size() const noexcept
		{
			const auto [x_min, x_max] = std::minmax(x_start, x_finish);
			const auto result = x_max - x_min;
			return static_cast<size_type>(result);
		}

		constexpr value_type front() const noexcept { return (*this)[0]; }
		constexpr value_type back() const noexcept { return (*this)[size() - 1]; }
		constexpr coords_row_range<INT> reverse() const noexcept
		{
			const bool going_forward = x_start <= x_finish;
			const INT new_start = going_forward ? x_finish - 1 : x_finish + 1;
			const INT new_finish = going_forward ? x_start - 1 : x_start + 1;
			return coords_row_range<INT>{new_start, new_finish, y_const};
		}

		INDEX_ITERATOR_MEMBER_BOILERPLATE(coords_row_range<INT>);
	};

	// Range of coords with a constant x
	template <std::integral INT>
	class coords_column_range
	{
		INT y_start;
		INT y_finish;
		INT x_const;
	public:
		// Typedefs
		using value_type = basic_coords<INT>;
		using reference_type = basic_coords<INT>;
		using pointer_type = basic_coords<INT>*;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		constexpr coords_column_range() noexcept : coords_column_range{0,0,0}{}
		constexpr coords_column_range(INT start_y, INT finish_y, INT const_x) noexcept : y_start{ start_y }, y_finish{ finish_y }, x_const{ const_x }
		{}

		constexpr coords_column_range(const basic_coords<INT>& start, INT finish) noexcept : coords_column_range{ start.y,finish,start.x } {}
		constexpr coords_column_range(const basic_coords<INT>& start, const basic_coords<INT>& finish) noexcept : coords_column_range{ start,finish.y, start.x }
		{
			AdventCheck(start.x == finish.x);
		}
		constexpr coords_column_range(INT start, const basic_coords<INT>& finish) noexcept : coords_column_range{ start,finish.y,finish.x } {}
		explicit constexpr coords_column_range(const basic_coords<INT>& finish) noexcept : coords_column_range{ INT{0},finish } {}

		constexpr coords_column_range& operator=(const coords_column_range&) = default;

		constexpr value_type operator[](std::size_t idy) const noexcept
		{
			AdventCheck(idx < size());
			const bool going_forward = y_start <= y_finish;
			const INT y_out = (going_forward ? y_start + idy : y_start - idy);
			return value_type{ y_out, x_const };
		}

		constexpr size_type size() const noexcept
		{
			const auto [y_min, y_max] = std::minmax(y_start, y_finish);
			const auto result = y_max - y_min;
			return static_cast<size_type>(result);
		}

		constexpr value_type front() const noexcept { return (*this)[0]; }
		constexpr value_type back() const noexcept { return (*this)[size() - 1]; }
		constexpr coords_column_range<INT> reverse() const noexcept
		{
			const bool going_forward = y_start <= y_finish;
			const INT new_start = going_forward ? y_finish - 1 : y_finish + 1;
			const INT new_finish = going_forward ? y_start - 1 : y_start + 1;
			return coords_column_range<INT>{new_start, new_finish, x_const};
		}

		INDEX_ITERATOR_MEMBER_BOILERPLATE(coords_column_range<INT>);
	};

	// Range of coords which cover an area.
	template <std::integral INT>
	class coords_area_elem_range
	{
		basic_coords<INT> start;
		basic_coords<INT> finish;
	public:
		// Typedefs
		using value_type = basic_coords<INT>;
		using reference_type = basic_coords<INT>;
		using pointer_type = basic_coords<INT>*;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		constexpr coords_area_elem_range() noexcept = default;
		constexpr coords_area_elem_range(const basic_coords<INT>& first, const basic_coords<INT>& last) : start{ first }, finish{ last } {}
		constexpr explicit coords_area_elem_range(const basic_coords<INT>& last) : coords_area_elem_range{ basic_coords<INT>{},last } {}

		constexpr coords_area_elem_range& operator=(const coords_area_elem_range&) = default;

		constexpr value_type operator[](std::size_t idx) const noexcept
		{
			AdventCheck(idx < size());
			const bool going_right = start.x <= finish.x;
			const bool going_up = start.y <= finish.y;
			const INT y_offset = static_cast<INT>(idx / width());
			const INT x_offset = static_cast<INT>(idx % width());
			const INT x_result = going_right ? start.x + x_offset : start.x - x_offset;
			const INT y_result = going_up ? start.y + y_offset : start.y - y_offset;
			return value_type{ x_result,y_result };
		}

		constexpr size_type width() const noexcept
		{
			const auto [x_min, x_max] = std::minmax(start.x, finish.x);
			const auto result = x_max - x_min;
			return static_cast<size_type>(result);
		}

		constexpr size_type height() const noexcept
		{
			const auto [y_min, y_max] = std::minmax(start.y, finish.y);
			const auto result = y_max - y_min;
			return static_cast<size_type>(result);
		}

		constexpr size_type size() const noexcept
		{
			return width() * height();
		}

		constexpr value_type front() const noexcept { return (*this)[0]; }
		constexpr value_type back() const noexcept { return (*this)[size() - 1]; }
		constexpr coords_area_range<INT> reverse() const noexcept
		{
			const bool going_right = start.x <= finish.x;
			const bool going_up = start.y <= finish.y;
			auto transform = [going_right, going_up](const basic_coords<INT>& c)
				{
					if constexpr (std::is_unsigned_v<INT>)
					{
						AdventCheck(c.x != 0);
						AdventCheck(c.y != 0);
					}
					const auto new_x = going_right ? c.x - 1 : c.x + 1;
					const auto new_y = going_up ? c.y - 1 : c.y + 1;
					return basic_coords<INT>{new_x, new_y};
				};
			return coords_area_elem_range<INT>{transform(finish), transform(start)};
		}

		INDEX_ITERATOR_MEMBER_BOILERPLATE(coords_area_elem_range<INT>);
	};

	// Range of row ranges which cover an area.
	template <std::integral INT>
	class coords_area_row_range
	{
		basic_coords<INT> start;
		basic_coords<INT> finish;
	public:
		// Typedefs
		using value_type = coords_row_range<INT>;
		using reference_type = coords_row_range<INT>;
		using pointer_type = coords_row_range<INT>*;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		constexpr coords_area_row_range() noexcept = default;
		constexpr coords_area_row_range(const basic_coords<INT>& first, const basic_coords<INT>& last) noexcept : start{ first }, finish{ last } {}
		constexpr explicit coords_area_row_range(const basic_coords<INT>& last) noexcept : coords_area_row_range{ basic_coords<INT>{},last } {}

		constexpr value_type operator[](std::size_t idx) const noexcept
		{
			AdventCheck(idx < size());
			const bool going_up = start.y <= finish.y;
			const INT y = going_up ? start.y + idx : start.y - idx;
			return value_type{start.x,finish.x,y};
		}

		constexpr size_type size() const noexcept
		{
			const auto [min_y , max_y] = std::minmax(start.y, finish.y);
			return static_cast<size_type>(max_y - min_y);
		}

		constexpr value_type front() const noexcept { return (*this)[0]; }
		constexpr value_type back() const noexcept { return (*this)[size() - 1]; }
		constexpr coords_area_row_range<INT> reverse() const noexcept
		{
			const bool going_right = start.x <= finish.x;
			const bool going_up = start.y <= finish.y;
			auto transform = [going_right, going_up](const basic_coords<INT>& c)
			{
				if constexpr (std::is_unsigned_v<INT>)
				{
					AdventCheck(c.x != 0);
					AdventCheck(c.y != 0);
				}
				const auto new_x = going_right ? c.x - 1 : c.x + 1;
				const auto new_y = going_up ? c.y - 1 : c.y + 1;
				return basic_coords<INT>{new_x, new_y};
			};
			return coords_area_row_range<INT>{transform(finish), transform(start)};
		}

		INDEX_ITERATOR_MEMBER_BOILERPLATE(coords_area_row_range<INT>);
	};

	// Range of row ranges which cover an area.
	template <std::integral INT>
	class coords_area_column_range
	{
		basic_coords<INT> start;
		basic_coords<INT> finish;
	public:
		// Typedefs
		using value_type = coords_row_range<INT>;
		using reference_type = coords_row_range<INT>;
		using pointer_type = coords_row_range<INT>*;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		constexpr coords_area_column_range() noexcept = default;
		constexpr coords_area_column_range(const basic_coords<INT>& first, const basic_coords<INT>& last) noexcept : start{ first }, finish{ last } {}
		constexpr explicit coords_area_column_range(const basic_coords<INT>& last) noexcept : coords_area_column_range{ basic_coords<INT>{},last } {}
		constexpr coords_area_column_range(const coords_area_column_range&) noexcept = default;

		constexpr value_type operator[](std::size_t idx) const noexcept
		{
			AdventCheck(idx < size());
			const bool going_up = start.x <= finish.x;
			const INT x = going_up ? start.x + idx : start.x - idx;
			return value_type{ start.y,finish.y,x };
		}

		constexpr size_type size() const noexcept
		{
			const auto [min_x, max_x] = std::minmax(start.x, finish.x);
			return static_cast<size_type>(max_x - min_x);
		}

		constexpr value_type front() const noexcept { return (*this)[0]; }
		constexpr value_type back() const noexcept { return (*this)[size() - 1]; }
		constexpr coords_area_row_range<INT> reverse() const noexcept
		{
			const bool going_right = start.x <= finish.x;
			const bool going_up = start.y <= finish.y;
			auto transform = [going_right, going_up](const basic_coords<INT>& c)
			{
				if constexpr (std::is_unsigned_v<INT>)
				{
					AdventCheck(c.x != 0);
					AdventCheck(c.y != 0);
				}
				const auto new_x = going_right ? c.x - 1 : c.x + 1;
				const auto new_y = going_up ? c.y - 1 : c.y + 1;
				return basic_coords<INT>{new_x, new_y};
			};
			return coords_area_row_range<INT>{transform(finish), transform(start)};
		}

		INDEX_ITERATOR_MEMBER_BOILERPLATE(coords_area_column_range<INT>);
	};

	using coords = basic_coords<int64_t>;

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
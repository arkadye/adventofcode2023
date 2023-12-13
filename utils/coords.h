#pragma once

#include <compare>
#include <numeric>
#include <cmath>
#include <array>
#include <iostream>

#include "../advent/advent_assert.h"
#include "split_string.h"
#include "to_value.h"

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

/*		class base_iterator : public std::random_access_iterator_tag
		{
		protected:
			basic_coords<T> current_location;
			constexpr auto cmp(const base_iterator& other) const noexcept
			{
				return current_location <=> other.current_location;
			}
		public:
			constexpr explicit base_iterator(basic_coords init) noexcept : current_location{init}{}
			constexpr base_iterator(const base_iterator&) noexcept = default;
			constexpr base_iterator& operator=(const base_iterator&) noexcept = default;
			constexpr auto operator<=>(const base_iterator&) const noexcept = default;
			constexpr bool operator*() const noexcept { return current_location; }
		};

		class range_base
		{
		protected:
			basic_coords<T> start, finish;
		public:
			range_base(const basic_coords<T>& first, const basic_coords<T>& last) : start{first}, finish{last}{}
		};

		class row_iterator : public base_iterator
		{
			void check_other(const row_iterator& other) const
			{
				AdventCheck(current_location.y == other.y);
			}
		public:
			using difference_type = decltype(T{} - T{});
			using value_type = basic_coords<T>;
			using pointer = basic_coords<T>*;
			using reference = basic_coords<T>&;
			using iterator_category = std::random_access_iterator_tag;
			template <std::integral RHSTYPE>
			constexpr row_iterator& operator+(RHSTYPE rhs) noexcept { current_location.x += rhs; return *this; }
			constexpr row_iterator& operator++() noexcept { return (*this) += 1; }
			constexpr row_iterator operator++(int) noexcept { auto result = *this; ++(*this); return result; }
			template <std::integral RHSTYPE>
			constexpr row_iterator& operator-(RHSTYPE rhs) noexcept { current_location.x -= rhs; return *this; }
			constexpr row_iterator& operator--() noexcept { return (*this) -= 1; }
			constexpr row_iterator operator--(int) noexcept { auto result = *this; --(*this); return result; }
			constexpr difference_type operator-(const row_iterator& other) const { check_other(other); return other.x - current_location.x; }
			constexpr auto operator<=>(const row_iterator& other) const
			{
				check_other(other);
				return cmp(other);
			}
			constexpr auto operator==(const row_iterator& other) const { return (*this <=> other) == 0; }
		};

		class row_range : public range_base
		{
		public:
			row_range(const basic_coords<T>& first, const basic_coords<T>& last) : range_base{first,last}
			{
				AdventCheck(first.y == last.y);
			}
			row_iterator begin() const noexcept { return row_iterator{start}; }
			row_iterator end() const noexcept { return row_iterator{finish}; }
		};

		class column_iterator : public base_iterator
		{
			void check_other(const column_iterator& other) const
			{
				AdventCheck(current_location.x == other.x);
			}
		public:
			using difference_type = decltype(T{} - T{});
			using value_type = basic_coords<T>;
			using pointer = basic_coords<T>*;
			using reference = basic_coords<T>&;
			using iterator_category = std::random_access_iterator_tag;
			template <std::integral RHSTYPE>
			constexpr column_iterator& operator+(RHSTYPE rhs) noexcept { current_location.y += rhs; return *this; }
			constexpr column_iterator& operator++() noexcept { return (*this) += 1; }
			constexpr column_iterator operator++(int) noexcept { auto result = *this; ++(*this); return result; }
			template <std::integral RHSTYPE>
			constexpr column_iterator& operator-(RHSTYPE rhs) noexcept { current_location.y -= rhs; return *this; }
			constexpr column_iterator& operator--() noexcept { return (*this) -= 1; }
			constexpr column_iterator operator--(int) noexcept { auto result = *this; --(*this); return result; }
			constexpr difference_type operator-(const column_iterator& other) const { check_other(other); return other.y - current_location.y; }
			constexpr auto operator<=>(const column_iterator& other) const
			{
				check_other(other);
				return cmp(other);
			}
			constexpr bool operator==(const column_iterator& other) const { return (*this <=> other) == 0; }
		};

		class column_range : public range_base
		{
		public:
			column_range(const basic_coords<T>& first, const basic_coords<T>& last) : range_base{ first,last }
			{
				AdventCheck(first.x == last.x);
			}
			column_iterator begin() const noexcept { return column_iterator{ start }; }
			column_iterator end() const noexcept { return column_iterator{ finish }; }
		};

		class area_iterator : public base_iterator
		{
			T x_min, x_max;
			void check_other(const area_iterator& other) const
			{
				AdventCheck(x_min == other.x_min);
				AdventCheck(x_max == other.x_max);
			}
		public:
			using difference_type = decltype(T{} - T{});
			using value_type = basic_coords<T>;
			using pointer = basic_coords<T>*;
			using reference = basic_coords<T>&;
			using iterator_category = std::random_access_iterator_tag;
			area_iterator(const basic_coords<T>& coords, T min, T max) : area_iterator{coords}{ x_min = min; x_max = max; }
			template <std::integral RHSTYPE>
			constexpr area_iterator& operator+(RHSTYPE rhs) noexcept { current_location.y += rhs; return *this; }
			constexpr area_iterator& operator++() noexcept { return (*this) += 1; }
			constexpr area_iterator operator++(int) noexcept { auto result = *this; ++(*this); return result; }
			template <std::integral RHSTYPE>
			constexpr area_iterator& operator-(RHSTYPE rhs) noexcept { current_location.y -= rhs; return *this; }
			constexpr area_iterator& operator--() noexcept { return (*this) -= 1; }
			constexpr area_iterator operator--(int) noexcept { auto result = *this; --(*this); return result; }
			constexpr difference_type operator-(const area_iterator& other) const { check_other(other); return other.y - current_location.y; }
			constexpr auto operator<=>(const area_iterator& other) const
			{
				check_other(other);
				return cmp(other);
			}
			constexpr bool operator==(const area_iterator& other) const { return (*this <=> other) == 0; }
		};

		class area_range : public range_base
		{
		public:
			area_range(const basic_coords<T>& first, const basic_coords<T>& last) : range_base{ first,last } {}
			area_iterator begin() const noexcept { return area_iterator{ start , start.x, finish.x }; }
			area_iterator end() const noexcept { return area_iterator{ finish , start.x, finish.x }; }
		};
*/
	};

	using coords = basic_coords<int>;

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
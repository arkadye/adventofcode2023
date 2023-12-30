#include "advent22.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY22DBG 1
#ifdef NDEBUG
#define DAY22DBG 0
#else
#define DAY22DBG ENABLE_DAY22DBG
#endif

#if DAY22DBG
	#include <iostream>
#endif

namespace
{
#if DAY22DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include "parse_utils.h"
#include "to_value.h"
#include "range_contains.h"
#include "sorted_vector.h"
#include "istream_line_iterator.h"
#include "int_range.h"

#include <tuple>

namespace
{
	using CoordType = int;

	struct Coords
	{
		CoordType x = 0;
		CoordType y = 0;
		CoordType z = 0;
		Coords& operator+=(const Coords& c) noexcept
		{
			x += c.x;
			y += c.y;
			z += c.z;
			return *this;
		}
		constexpr auto operator<=>(const Coords& other) const noexcept
		{
			auto to_tuple = [](const Coords& c)
				{
					return std::tuple{ c.z,c.x,c.y };
				};
			return to_tuple(*this) <=> to_tuple(other);
		}
		constexpr bool operator==(const Coords&) const noexcept = default;
	};

	Coords parse_coords(std::string_view sv)
	{
		auto to_val = [](std::string_view arg)
		{
			return utils::to_value<CoordType>(arg);
		};
		auto [x, y, z] = utils::get_string_elements(sv, ',', 0, 1, 2);
		return Coords{ to_val(x), to_val(y), to_val(z) };
	}

	Coords operator+(const Coords& l, const Coords& r) noexcept
	{
		Coords result = l;
		result += r;
		return result;
	}

	struct Block
	{
		Coords lower_bound;
		Coords upper_bound;
		constexpr auto operator<=>(const Block&) const noexcept = default;
		bool is_inside(const Coords& c) const noexcept
		{
			using utils::range_contains_exc;
			return range_contains_exc(c.x, lower_bound.x, upper_bound.x)
				&& range_contains_exc(c.y, lower_bound.y, upper_bound.y)
				&& range_contains_exc(c.z, lower_bound.z, upper_bound.z);
		}
	};

	Block parse_block(std::string_view line)
	{
		auto [start, finish] = utils::split_string_at_first(line, '~');
		const Coords lower = parse_coords(start);
		const Coords upper = parse_coords(finish) + Coords{ 1,1,1 };
		AdventCheck(lower.x < upper.x);
		AdventCheck(lower.y < upper.y);
		AdventCheck(lower.z < upper.z);
		return Block{ lower,upper };
	}

	// This will be sorted from z-bottom to z-top
	using BlockList = utils::sorted_vector<Block>;

	BlockList parse_block_list(std::istream& input)
	{
		BlockList result;
		stdr::transform(utils::istream_line_range{ input }, std::back_inserter(result), parse_block);
		return result;
	}

	bool is_supporting_block(const Block& lower, const Block& upper)
	{
		if (lower.upper_bound.z != upper.lower_bound.z) return false;
		
		if (lower.upper_bound.x <= upper.lower_bound.x) return false;
		if (lower.upper_bound.y <= upper.lower_bound.y) return false;
		
		if (upper.upper_bound.x <= lower.lower_bound.x) return false;
		if (upper.upper_bound.y <= lower.lower_bound.y) return false;
		
		return true;
	}

	bool is_block_supported(const Block& block, const BlockList& block_list, const std::optional<Block>& block_to_ignore)
	{
		if (block.lower_bound.z == 0) return true;
		const auto search_range_top = stdr::lower_bound(block_list, block.lower_bound.z, std::less<CoordType>{}, [](const Block& b) {return b.lower_bound.z; });
		stdr::subrange search_range{ begin(block_list), search_range_top };

		const auto blocks_to_search = stdr::views::reverse(search_range);
		
		const auto find_result = stdr::find_if(blocks_to_search, [&upper = block, &block_to_ignore](const Block& lower)
			{
				if (block_to_ignore.has_value())
				{
					if (lower == block_to_ignore.value()) return false;
				}
				return is_supporting_block(lower, upper);
			});
		return find_result != blocks_to_search.end();
	}

	bool is_block_supported(const Block& block, const BlockList& block_list)
	{
		return is_block_supported(block, block_list, std::nullopt);
	}

	auto get_lowest_unsupported_block(const BlockList& block_list, const std::optional<Block>& block_to_ignore) -> decltype(begin(block_list))
	{
		return stdr::find_if(block_list, [&block_list, &block_to_ignore](const Block& block)
			{
				return !is_block_supported(block, block_list, block_to_ignore);
			});
	}

	auto get_lowest_unsupported_block(const BlockList& block_list) -> decltype(begin(block_list))
	{
		return get_lowest_unsupported_block(block_list, std::nullopt);
	}

	std::pair<BlockList, bool> drop_lowest_unsupported_block(BlockList blocks)
	{
		const auto lowest_unsupported_block = get_lowest_unsupported_block(blocks);
		if (lowest_unsupported_block == end(blocks))
		{
			return std::pair{ std::move(blocks), false };
		}

		Block block_to_drop = *lowest_unsupported_block;
		blocks.erase(lowest_unsupported_block);

		while (true)
		{
			--block_to_drop.lower_bound.z;
			--block_to_drop.upper_bound.z;
			if (is_block_supported(block_to_drop, blocks))
			{
				blocks.insert_keep_sorted(block_to_drop);
				return std::pair{ std::move(blocks), true };
			}
		}

		AdventUnreachable();
		return std::pair{ std::move(blocks),false };
	}

	std::pair<BlockList,int> drop_blocks(BlockList input)
	{
		int num_dropped_blocks = 0;
		while (true)
		{
			auto [blocks, dropped_a_block] = drop_lowest_unsupported_block(std::move(input));
			if (dropped_a_block)
			{
				input = std::move(blocks);
				++num_dropped_blocks;
			}
			else
			{
				return std::pair{ std::move(blocks), num_dropped_blocks };
			}
		}
		AdventUnreachable();
		return std::pair{ std::move(input),0 };
	}

	BlockList parse_dropped_blocks(std::istream& input)
	{
		BlockList block_list = parse_block_list(input);
		auto [dropped_blocks, who_cares_about_this_variable_i_sure_dont] = drop_blocks(std::move(block_list));
		return dropped_blocks;
	}

	bool has_unsupported_blocks(const BlockList& block_list, const std::optional<Block>& block_to_ignore)
	{
		const auto lowest_unsupported_block = get_lowest_unsupported_block(block_list, block_to_ignore);
		return lowest_unsupported_block != end(block_list);
	}

	bool can_remove_block(const BlockList& block_list, const Block& block_to_remove)
	{
		return !has_unsupported_blocks(block_list, block_to_remove);
	}

	int64_t solve_p1(std::istream& input)
	{
		const BlockList blocks = parse_dropped_blocks(input);
		auto pred = [&blocks](const Block& block)
			{
				return can_remove_block(blocks, block);
			};
		return stdr::count_if(blocks, pred);
	}
}

namespace
{
	int get_num_blocks_to_drop(BlockList block_list)
	{
		const auto [drop_this_variable_you_dont_need_it, num_blocks] = drop_blocks(std::move(block_list));
		return num_blocks;
	}

	int disintegrate_block_and_count_drops(BlockList block_list, std::size_t position)
	{
		block_list.erase(begin(block_list) + position);
		const auto [dummy_block_list, num_blocks] = drop_blocks(std::move(block_list));
		return num_blocks;
	}

	int solve_p2(std::istream& input)
	{
		const BlockList block_list = parse_dropped_blocks(input);
		const utils::int_range idx_range{ block_list.size() };
		auto get_drops = [&block_list](std::size_t idx)
			{
				return disintegrate_block_and_count_drops(block_list, idx);
			};
		const int result = std::transform_reduce(begin(idx_range), end(idx_range), int{ 0 }, std::plus<int>{}, get_drops);
		return result;
	}
}

ResultType testcase_twentytwo_p1(std::istream& input)
{
	return solve_p1(input);
}

ResultType testcase_twentytwo_p2(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_twentytwo_p1()
{
	auto input = advent::open_puzzle_input(22);
	return solve_p1(input);
}

ResultType advent_twentytwo_p2()
{
	auto input = advent::open_puzzle_input(22);
	return solve_p2(input);
}

#undef DAY22DBG
#undef ENABLE_DAY22DBG
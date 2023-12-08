#include "advent7.h"
#include "../advent/advent_utils.h"

#define ENABLE_DAY7DBG 1
#ifdef NDEBUG
#define DAY7DBG 0
#else
#define DAY7DBG ENABLE_DAY7DBG
#endif

#if DAY7DBG
	#include <iostream>
#endif

namespace
{
#if DAY7DBG
	std::ostream & log = std::cout;
#else
	struct {	template <typename T> auto& operator<<(const T&) const noexcept { return *this; } } log;
#endif
}

#include <array>
#include <algorithm>
#include <numeric>

#include "enums.h"
#include "parse_utils.h"
#include "sorted_vector.h"
#include "to_value.h"
#include "istream_line_iterator.h"
#include "int_range.h"
#include "comparisons.h"

namespace
{
	enum class HandType : uint8_t
	{
		high_card,
		one_pair,
		two_pair,
		three_of_a_kind,
		full_house,
		four_of_a_kind,
		five_of_a_kind,
		NUM_VALUES
	};

	constexpr auto operator<=>(HandType left, HandType right) noexcept
	{
		return utils::enum_order(left, right);
	}

	std::string to_string(HandType hand_type)
	{
#define CASE(ht) case ht: return #ht
		switch(hand_type)
		{
		CASE(HandType::high_card);
		CASE(HandType::one_pair);
		CASE(HandType::two_pair);
		CASE(HandType::three_of_a_kind);
		CASE(HandType::full_house);
		CASE(HandType::four_of_a_kind);
		CASE(HandType::five_of_a_kind);
		default:
			break;
		}
#undef CASE
		return "ERRORHANDTYPE";
	}

	enum class Card : uint8_t
	{
		joker,
		two,
		three,
		four,
		five,
		six,
		seven,
		eight,
		nine,
		ten,
		jack,
		queen,
		king,
		ace,
		NUM_VALUES
	};

	using Hand = std::array<Card,5>;

	template <AdventDay DAY>
	Card to_card(char c)
	{
		auto impl = [c](std::string_view cards)
		{
			AdventCheck(cards.size() == utils::to_idx(Card::NUM_VALUES));
			const std::size_t idx = cards.find(c);
			AdventCheck(idx < cards.size());
			return static_cast<Card>(idx);
		};

		if constexpr (DAY == AdventDay::one)
		{
			return impl("x23456789TJQKA");
		}
		if constexpr (DAY == AdventDay::two)
		{
			return impl("J23456789TxQKA");
		}
		AdventUnreachable();
		return Card::NUM_VALUES;
	}

	template <AdventDay DAY>
	Hand to_hand(std::string_view input)
	{
		Hand result;
		std::ranges::transform(input, std::begin(result), to_card<DAY>);
		return result;
	}

	HandType get_hand_type(const Hand& hand)
	{
		const auto joker_location = std::ranges::find(hand,Card::joker);
		if(joker_location != end(hand))
		{
			const auto pos = std::distance(begin(hand),joker_location);
			auto transform_fn = [&hand, pos](std::size_t card_idx)
			{
				AdventCheck(card_idx < utils::to_idx(Card::NUM_VALUES));
				const Card card = static_cast<Card>(card_idx);
				AdventCheck(card != Card::joker);
				Hand hand_copy = hand;
				hand_copy[pos] = card;
				return get_hand_type(hand_copy);
			};
			constexpr auto card_range = utils::int_range{utils::to_idx(Card::two),utils::to_idx(Card::NUM_VALUES)};
			const HandType result = std::transform_reduce(begin(card_range),end(card_range),HandType::high_card,[](HandType l, HandType r){return std::max(l,r);},transform_fn);
			return result;
		}
		using ValueMap = std::array<int8_t,utils::to_idx(Card::NUM_VALUES)>;
		auto add_card = [](ValueMap map , Card card)
		{
			++map[utils::to_idx(card)];
			return map;
		};
		
		const ValueMap value_count = std::accumulate(begin(hand),end(hand),ValueMap{},add_card);
		const auto max_count = std::ranges::max(value_count);
		switch(max_count)
		{
		case 5:
			return HandType::five_of_a_kind;
		case 4:
			return HandType::four_of_a_kind;
		case 1:
			return HandType::high_card;
		case 3:
			return std::ranges::find(value_count,2) != end(value_count) ? HandType::full_house : HandType::three_of_a_kind;
		case 2:
			return std::ranges::count(value_count,2) == 2 ? HandType::two_pair : HandType::one_pair;
		default:
			break;
		}
		AdventUnreachable();
		return static_cast<HandType>(std::numeric_limits<std::underlying_type_t<HandType>>::max());
	}

	uint32_t to_value(HandType type)
	{
		return utils::to_idx(type);
	}

	uint32_t to_value(Card card)
	{
		return utils::to_idx(card);
	}

	using HandHash = uint32_t;

	HandHash get_hand_hash(const Hand& hand)
	{
		const HandType type = get_hand_type(hand);
		auto add_card = [](uint32_t partial_hash,Card card)
		{
			return partial_hash * utils::to_idx(Card::NUM_VALUES) + utils::to_idx(card);
		};
		const uint32_t type_val = to_value(type);
		const uint32_t result = std::accumulate(begin(hand),end(hand),type_val,add_card);
		return result;
	}

	using BetType = int;
	using HandsMap = utils::flat_map<HandHash,BetType>;

	template <AdventDay DAY>
	HandsMap parse_input(std::istream& input)
	{
		auto parse_line = [](std::string_view line)
		{
			const auto [hand_str,bet_str] = utils::split_string_at_first(line,' ');
			const Hand hand = to_hand<DAY>(hand_str);
			const int bet = utils::to_value<BetType>(bet_str);
			const HandHash hand_hash = get_hand_hash(hand);
			return std::pair{hand_hash,bet};
		};

		using ILI = utils::istream_line_iterator;
		HandsMap result;
		result.reserve(1000);
		std::transform(ILI{input}, ILI{},std::back_inserter(result),parse_line); // TODO move to a std::ranges version when I'm using a compiler with good errors.
		return result;
	}

	template <AdventDay DAY>
	int solve_generic(std::istream& input)
	{
		const HandsMap hm = parse_input<DAY>(input);
		auto acc_fn = [&hm](std::size_t idx)
		{
			const int multiplier = static_cast<int>(idx) + 1;
			return multiplier * hm[idx].second;
		};
		const auto idx_range = utils::int_range{ hm.size() };
		const auto result = std::transform_reduce(begin(idx_range), end(idx_range), 0, std::plus<int>{}, acc_fn);
		return result;
	}

	template <AdventDay DAY>
	std::string solve_testcase(std::istream& input)
	{
		std::string in_line;
		std::getline(input, in_line);
		const std::string_view hand_str = utils::get_string_element(in_line, 0);
		const Hand hand = to_hand<DAY>(hand_str);
		const HandType type = get_hand_type(hand);
		return to_string(type);
	}

	int solve_p1(std::istream& input)
	{
		return solve_generic<AdventDay::one>(input);
	}

	int solve_p2(std::istream& input)
	{
		return solve_generic<AdventDay::two>(input);
	}
}

ResultType testcase_seven_p1_a(std::istream& input)
{
	return solve_testcase<AdventDay::one>(input);
}

ResultType testcase_seven_p2_a(std::istream& input)
{
	return solve_testcase<AdventDay::two>(input);
}

ResultType testcase_seven_p1_b(std::istream& input)
{
	return solve_p1(input);
}

ResultType testcase_seven_p2_b(std::istream& input)
{
	return solve_p2(input);
}

ResultType advent_seven_p1()
{
	auto input = advent::open_puzzle_input(7);
	return solve_p1(input);
}

ResultType advent_seven_p2()
{
	auto input = advent::open_puzzle_input(7);
	return solve_p2(input);
}

#undef DAY7DBG
#undef ENABLE_DAY7DBG
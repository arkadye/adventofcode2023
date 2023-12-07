#pragma once

#include <sstream>
#include <string_view>

namespace advent
{
	namespace combine_inputs_internal
	{
		inline std::string impl(std::ostringstream& collector)
		{
			return collector.str();
		}

		template <typename...StringViews>
		inline std::string impl(std::ostringstream& collector, std::string_view next_string, StringViews...remaining)
		{
			collector << '\n' << next_string;
			return impl(collector,remaining...);
		}
	}

	template <typename...StringViews>
	inline std::string combine_inputs(std::string_view first_string, StringViews...remaining)
	{
		std::ostringstream collector;
		collector << first_string;
		return combine_inputs_internal::impl(collector, remaining...);
	}
}

#define TEST_ONE_A "1abc2"
#define TEST_ONE_B "pqr3stu8vwx"
#define TEST_ONE_C "a1b2c3d4e5f"
#define TEST_ONE_D "treb7uchet"
static const auto TEST_FILE_A = advent::combine_inputs(TEST_ONE_A, TEST_ONE_B, TEST_ONE_C, TEST_ONE_D);
#define TEST_ONE_F "two1nine"
#define TEST_ONE_G "eightwothree"
#define TEST_ONE_H "abcone2threexyz"
#define TEST_ONE_I "xtwone3four"
#define TEST_ONE_J "4nineeightseven2"
#define TEST_ONE_K "zoneight234"
#define TEST_ONE_L "7pqrstsixteen"
static const auto TEST_FILE_B = advent::combine_inputs(TEST_ONE_F, TEST_ONE_G, TEST_ONE_H, TEST_ONE_I, TEST_ONE_J, TEST_ONE_K, TEST_ONE_L);

constexpr const char* TEST_TWO_A = "Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green";
constexpr const char* TEST_TWO_B = "Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue";
constexpr const char* TEST_TWO_C = "Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red";
constexpr const char* TEST_TWO_D = "Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red";
constexpr const char* TEST_TWO_E = "Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green";
static const auto TEST_TWO_F = advent::combine_inputs(TEST_TWO_A, TEST_TWO_B, TEST_TWO_C, TEST_TWO_D, TEST_TWO_E);

constexpr const char* TEST_FOUR_A = "Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53";
constexpr const char* TEST_FOUR_B = "Card 2 : 13 32 20 16 61 | 61 30 68 82 17 32 24 19";
constexpr const char* TEST_FOUR_C = "Card 3 : 1 21 53 59 44 | 69 82 63 72 16 21 14  1";
constexpr const char* TEST_FOUR_D = "Card 4 : 41 92 73 84 69 | 59 84 76 51 58  5 54 83";
constexpr const char* TEST_FOUR_E = "Card 5 : 87 83 26 28 32 | 88 30 70 12 93 22 82 36";
constexpr const char* TEST_FOUR_F = "Card 6 : 31 18 13 56 72 | 74 77 10 23 35 67 36 11";
static const auto TEST_FOUR_G = advent::combine_inputs(TEST_FOUR_A, TEST_FOUR_B, TEST_FOUR_C, TEST_FOUR_D, TEST_FOUR_E, TEST_FOUR_E);

constexpr const char* TEST_FIVE_A =
R"(seeds: 79 14 55 13

seed-to-soil map:
50 98 2
52 50 48

soil-to-fertilizer map:
0 15 37
37 52 2
39 0 15

fertilizer-to-water map:
49 53 8
0 11 42
42 0 7
57 7 4

water-to-light map:
88 18 7
18 25 70

light-to-temperature map:
45 77 23
81 45 19
68 64 13

temperature-to-humidity map:
0 69 1
1 0 69

humidity-to-location map:
60 56 37
56 93 4)";

constexpr const char* TEST_SIX_A = "Time:      7\nDistance:  9";
constexpr const char* TEST_SIX_B = "Time:      15\nDistance:  40";
constexpr const char* TEST_SIX_C = "Time:      30\nDistance:  200";
constexpr const char* TEST_SIX_D = "Time:      7  15   30\nDistance:  9  40  200";
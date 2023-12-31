#pragma once

#include <sstream>
#include <string_view>

namespace advent
{
	namespace combine_inputs_internal
	{
		inline std::string impl(std::string_view joiner, std::ostringstream& collector)
		{
			return collector.str();
		}

		template <typename...StringViews>
		inline std::string impl(std::string_view joiner, std::ostringstream& collector, std::string_view next_string, StringViews...remaining)
		{
			collector << joiner << next_string;
			return impl(joiner, collector,remaining...);
		}
	}

	template <std::size_t NUM_NEWLINES, typename...StringViews>
	inline std::string combine_inputs(std::string_view first_string, StringViews...remaining)
	{
		std::ostringstream collector;
		std::string joiner(NUM_NEWLINES, '\n');
		collector << first_string;
		return combine_inputs_internal::impl(joiner, collector, remaining...);
	}
}

#define TEST_ONE_A "1abc2"
#define TEST_ONE_B "pqr3stu8vwx"
#define TEST_ONE_C "a1b2c3d4e5f"
#define TEST_ONE_D "treb7uchet"
static const auto TEST_ONE_FILE_A = advent::combine_inputs<1>(TEST_ONE_A, TEST_ONE_B, TEST_ONE_C, TEST_ONE_D);
#define TEST_ONE_F "two1nine"
#define TEST_ONE_G "eightwothree"
#define TEST_ONE_H "abcone2threexyz"
#define TEST_ONE_I "xtwone3four"
#define TEST_ONE_J "4nineeightseven2"
#define TEST_ONE_K "zoneight234"
#define TEST_ONE_L "7pqrstsixteen"
static const auto TEST_ONE_FILE_B = advent::combine_inputs<1>(TEST_ONE_F, TEST_ONE_G, TEST_ONE_H, TEST_ONE_I, TEST_ONE_J, TEST_ONE_K, TEST_ONE_L);

constexpr const char* TEST_TWO_A = "Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green";
constexpr const char* TEST_TWO_B = "Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue";
constexpr const char* TEST_TWO_C = "Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red";
constexpr const char* TEST_TWO_D = "Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red";
constexpr const char* TEST_TWO_E = "Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green";
static const auto TEST_TWO_F = advent::combine_inputs<1>(TEST_TWO_A, TEST_TWO_B, TEST_TWO_C, TEST_TWO_D, TEST_TWO_E);

constexpr const char* TEST_FOUR_A = "Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53";
constexpr const char* TEST_FOUR_B = "Card 2 : 13 32 20 16 61 | 61 30 68 82 17 32 24 19";
constexpr const char* TEST_FOUR_C = "Card 3 : 1 21 53 59 44 | 69 82 63 72 16 21 14  1";
constexpr const char* TEST_FOUR_D = "Card 4 : 41 92 73 84 69 | 59 84 76 51 58  5 54 83";
constexpr const char* TEST_FOUR_E = "Card 5 : 87 83 26 28 32 | 88 30 70 12 93 22 82 36";
constexpr const char* TEST_FOUR_F = "Card 6 : 31 18 13 56 72 | 74 77 10 23 35 67 36 11";
static const auto TEST_FOUR_G = advent::combine_inputs<1>(TEST_FOUR_A, TEST_FOUR_B, TEST_FOUR_C, TEST_FOUR_D, TEST_FOUR_E, TEST_FOUR_E);

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

#define TEST_SIX_A "Time: 7\nDistance:  9"
#define TEST_SIX_B "Time: 15\nDistance:  40"
#define TEST_SIX_C "Time: 30\nDistance:  200"
#define TEST_SIX_D "Time: 7  15   30\nDistance:  9  40  200"

#define TEST_SEVEN_A "32T3K 765"
#define TEST_SEVEN_B "T55J5 684"
#define TEST_SEVEN_C "KK677 28"
#define TEST_SEVEN_D "KTJJT 220"
#define TEST_SEVEN_E "QQQJA 483"
static const auto TEST_SEVEN_FILE_A = advent::combine_inputs<1>(TEST_SEVEN_A, TEST_SEVEN_B, TEST_SEVEN_C, TEST_SEVEN_D, TEST_SEVEN_E);

constexpr const char* TEST_EIGHT_A = 
R"(RL

AAA = (BBB, CCC)
BBB = (DDD, EEE)
CCC = (ZZZ, GGG)
DDD = (DDD, DDD)
EEE = (EEE, EEE)
GGG = (GGG, GGG)
ZZZ = (ZZZ, ZZZ))";

constexpr const char* TEST_EIGHT_B =
R"(LLR

AAA = (BBB, BBB)
BBB = (AAA, ZZZ)
ZZZ = (ZZZ, ZZZ))";

constexpr const char* TEST_EIGHT_C =
R"(LR

11A = (11B, XXX)
11B = (XXX, 11Z)
11Z = (11B, XXX)
22A = (22B, XXX)
22B = (22C, 22C)
22C = (22Z, 22Z)
22Z = (22B, 22B)
XXX = (XXX, XXX))";

#define TEST_NINE_A "0 3 6 9 12 15"
#define TEST_NINE_B "1 3 6 10 15 21"
#define TEST_NINE_C "10 13 16 21 30 45"
static const auto TEST_NINE_D = advent::combine_inputs<1>(TEST_NINE_A, TEST_NINE_B, TEST_NINE_C);

constexpr const char* TEST_TEN_A =
R"(.....
.S-7.
.|.|.
.L-J.
.....)";

constexpr const char* TEST_TEN_B =
R"(-L|F7
7S-7|
L|7||
-L-J|
L|-JF)";

constexpr const char* TEST_TEN_C =
R"(..F7.
.FJ|.
SJ.L7
|F--J
LJ...)";

constexpr const char* TEST_TEN_D =
R"(7-F7-
.FJ|7
SJLL7
|F--J
LJ.LJ)";

constexpr const char* TEST_TEN_E =
R"(...........
.S-------7.
.|F-----7|.
.||.....||.
.||.....||.
.|L-7.F-J|.
.|..|.|..|.
.L--J.L--J.
...........)";

constexpr const char* TEST_TEN_F =
R"(.F----7F7F7F7F-7....
.|F--7||||||||FJ....
.||.FJ||||||||L7....
FJL7L7LJLJ||LJ.L-7..
L--J.L7...LJS7F-7L7.
....F-J..F7FJ|L7L7L7
....L7.F7||L7|.L7L7|
.....|FJLJ|FJ|F7|.LJ
....FJL-7.||.||||...
....L---J.LJ.LJLJ...)";

constexpr const char* TEST_TEN_G =
R"(FF7FSF7F7F7F7F7F---7
L|LJ||||||||||||F--J
FL-7LJLJ||||||LJL-77
F--JF--7||LJLJ7F7FJ-
L---JF-JLJ.||-FJLJJ7
|F|F-JF---7F7-L7L|7|
|FFJF7L7F-JF7|JL---7
7-L-JL7||F7|L7F-7F7|
L.L7LFJ|||||FJL7||LJ
L7JLJL-JLJLJL--JLJ.L)";

constexpr const char* TEST_TEN_H =
R"(F-S
|.|
L-J)";

#define TEST_TWELVE_A "???.### 1,1,3"
#define TEST_TWELVE_B ".??..??...?##. 1,1,3"
#define TEST_TWELVE_C "?#?#?#?#?#?#?#? 1,3,1,6"
#define TEST_TWELVE_D "????.#...#... 4,1,1"
#define TEST_TWELVE_E "????.######..#####. 1,6,5"
#define TEST_TWELVE_F "?###???????? 3,2,1"

static const auto TEST_TWELVE_G = advent::combine_inputs<1>(TEST_TWELVE_A, TEST_TWELVE_B, TEST_TWELVE_C, TEST_TWELVE_D, TEST_TWELVE_E, TEST_TWELVE_F);

constexpr const char* TEST_THIRTEEN_A =
R"(#.##..##.
..#.##.#.
##......#
##......#
..#.##.#.
..##..##.
#.#.##.#.)";

constexpr const char* TEST_THIRTEEN_B =
R"(#...##..#
#....#..#
..##..###
#####.##.
#####.##.
..##..###
#....#..#)";

static const auto TEST_THIRTEEN_C = advent::combine_inputs<2>(TEST_THIRTEEN_A, TEST_THIRTEEN_B);

constexpr const char* TEST_FOURTEEN_A =
R"(O....#....
O.OO#....#
.....##...
OO.#O....O
.O.....O#.
O.#..O.#.#
..O..#O..O
.......O..
#....###..
#OO..#....)";

constexpr const char* TEST_FIFTEEN_A = "rn=1,cm-,qp=3,cm=2,qp-,pc=4,ot=9,ab=5,pc-,pc=6,ot=7";

constexpr const char* TEST_SIXTEEN_A =
R"(.|...\....
|.-.\.....
.....|-...
........|.
..........
.........\
..../.\\..
.-.-/..|..
.|....-|.\
..//.|....)";

constexpr const char* TEST_SEVENTEEN_A =
R"(2413432311323
3215453535623
3255245654254
3446585845452
4546657867536
1438598798454
4457876987766
3637877979653
4654967986887
4564679986453
1224686865563
2546548887735
4322674655533)";

constexpr const char* TEST_SEVENTEEN_B =
R"(111111111111
999999999991
999999999991
999999999991
999999999991)";

constexpr const char* TEST_EIGHTEEN_A =
R"(R 6 (#70c710)
D 5 (#0dc571)
L 2 (#5713f0)
D 2 (#d2c081)
R 2 (#59c680)
D 2 (#411b91)
L 5 (#8ceee2)
U 2 (#caa173)
L 1 (#1b58a2)
U 2 (#caa171)
R 2 (#7807d2)
U 3 (#a77fa3)
L 2 (#015232)
U 2 (#7a21e3))";

constexpr const char* TEST_NINETEEN_A =
R"(px{a<2006:qkq,m>2090:A,rfg}
pv{a>1716:R,A}
lnx{m>1548:A,A}
rfg{s<537:gd,x>2440:R,A}
qs{s>3448:A,lnx}
qkq{x<1416:A,crn}
crn{x>2662:A,R}
in{s<1351:px,qqz}
qqz{s>2770:qs,m<1801:hdj,R}
gd{a>3333:R,R}
hdj{m>838:A,pv}

{x=787,m=2655,a=1222,s=2876}
{x=1679,m=44,a=2067,s=496}
{x=2036,m=264,a=79,s=2244}
{x=2461,m=1339,a=466,s=291}
{x=2127,m=1623,a=2188,s=1013})";

constexpr const char* TEST_TWENTY_A =
R"(broadcaster -> a, b, c
%a -> b
%b -> c
%c -> inv
&inv -> a)";

constexpr const char* TEST_TWENTY_B =
R"(broadcaster -> a
%a -> inv, con
&inv -> b
%b -> con
&con -> output)";

constexpr const char* TEST_TWENTYONE_A =
R"(...........
.....###.#.
.###.##..#.
..#.#...#..
....#.#....
.##..S####.
.##..#...#.
.......##..
.##.#.####.
.##..##.##.
...........)";

constexpr const char* TEST_TWENTYTWO_A =
R"(1,0,1~1,2,1
0,0,2~2,0,2
0,2,3~2,2,3
0,0,4~0,2,4
2,0,5~2,2,5
0,1,6~2,1,6
1,1,8~1,1,9)";

constexpr const char* TEST_TWENTYTHREE_A =
R"(#.#####################
#.......#########...###
#######.#########.#.###
###.....#.>.>.###.#.###
###v#####.#v#.###.#.###
###.>...#.#.#.....#...#
###v###.#.#.#########.#
###...#.#.#.......#...#
#####.#.#.#######.#.###
#.....#.#.#.......#...#
#.#####.#.#.#########v#
#.#...#...#...###...>.#
#.#.#v#######v###.###v#
#...#.>.#...>.>.#.###.#
#####v#.#.###v#.#.###.#
#.....#...#...#.#.#...#
#.#########.###.#.#.###
#...###...#...#...#.###
###.###.#.###v#####v###
#...#...#.#.>.>.#.>.###
#.###.###.#.###.#.#v###
#.....###...###...#...#
#####################.#)";
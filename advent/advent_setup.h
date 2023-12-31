#pragma once

#include "advent_testcase_setup.h"
#include "advent_test_inputs.h"
#include "advent_headers.h"
#include "advent_solutions.h"

static const verification_test tests[] =
{
	TESTCASE_WITH_ARG(testcase_one_p1, TEST_ONE_A, 12),
	TESTCASE_WITH_ARG(testcase_one_p1, TEST_ONE_B, 38),
	TESTCASE_WITH_ARG(testcase_one_p1, TEST_ONE_C, 15),
	TESTCASE_WITH_ARG(testcase_one_p1, TEST_ONE_D, 77),
	TESTCASE_WITH_ARG(testcase_one_p1, TEST_ONE_FILE_A, 142),
	TESTCASE_WITH_ARG(testcase_one_p2, TEST_ONE_F, 29),
	TESTCASE_WITH_ARG(testcase_one_p2, TEST_ONE_G, 83),
	TESTCASE_WITH_ARG(testcase_one_p2, TEST_ONE_H, 13),
	TESTCASE_WITH_ARG(testcase_one_p2, TEST_ONE_I, 24),
	TESTCASE_WITH_ARG(testcase_one_p2, TEST_ONE_J, 42),
	TESTCASE_WITH_ARG(testcase_one_p2, TEST_ONE_K, 14),
	TESTCASE_WITH_ARG(testcase_one_p2, TEST_ONE_L, 76),
	TESTCASE_WITH_ARG(testcase_one_p2, TEST_ONE_FILE_B, 281),
	DAY(one,DAY_01_1_SOLUTION,DAY_01_2_SOLUTION),
	TESTCASE_WITH_ARG(testcase_two_p1, TEST_TWO_A, 1),
	TESTCASE_WITH_ARG(testcase_two_p1, TEST_TWO_B, 2),
	TESTCASE_WITH_ARG(testcase_two_p1, TEST_TWO_C, 0),
	TESTCASE_WITH_ARG(testcase_two_p1, TEST_TWO_D, 0),
	TESTCASE_WITH_ARG(testcase_two_p1, TEST_TWO_E, 5),
	TESTCASE_WITH_ARG(testcase_two_p1, TEST_TWO_F, 8),
	TESTCASE_WITH_ARG(testcase_two_p2, TEST_TWO_A, 48),
	TESTCASE_WITH_ARG(testcase_two_p2, TEST_TWO_B, 12),
	TESTCASE_WITH_ARG(testcase_two_p2, TEST_TWO_C, 1560),
	TESTCASE_WITH_ARG(testcase_two_p2, TEST_TWO_D, 630),
	TESTCASE_WITH_ARG(testcase_two_p2, TEST_TWO_E, 36),
	TESTCASE_WITH_ARG(testcase_two_p2, TEST_TWO_F, 2286),
	DAY(two,DAY_02_1_SOLUTION,DAY_02_2_SOLUTION),
	TESTCASE(testase_three_p1,4361),
	TESTCASE(testase_three_p2,467835),
	DAY(three,DAY_03_1_SOLUTION,DAY_03_2_SOLUTION),
	TESTCASE_WITH_ARG(testcase_four_p1, TEST_FOUR_A,8),
	TESTCASE_WITH_ARG(testcase_four_p1, TEST_FOUR_B,2),
	TESTCASE_WITH_ARG(testcase_four_p1, TEST_FOUR_C,2),
	TESTCASE_WITH_ARG(testcase_four_p1, TEST_FOUR_D,1),
	TESTCASE_WITH_ARG(testcase_four_p1, TEST_FOUR_E,0),
	TESTCASE_WITH_ARG(testcase_four_p1, TEST_FOUR_F,0),
	TESTCASE_WITH_ARG(testcase_four_p1, TEST_FOUR_G,13),
	TESTCASE_WITH_ARG(testcase_four_p2, TEST_FOUR_G,30),
	DAY(four,DAY_04_1_SOLUTION,DAY_04_2_SOLUTION),
	TESTCASE_WITH_ARG(testcase_five_p1_a<79>, TEST_FIVE_A,82),
	TESTCASE_WITH_ARG(testcase_five_p1_a<14>, TEST_FIVE_A,43),
	TESTCASE_WITH_ARG(testcase_five_p1_a<55>, TEST_FIVE_A,86),
	TESTCASE_WITH_ARG(testcase_five_p1_a<13>, TEST_FIVE_A,35),
	TESTCASE_WITH_ARG(testcase_five_p1_b, TEST_FIVE_A,35),
	TESTCASE_WITH_ARG(testcase_five_p2_b, TEST_FIVE_A,46),
	DAY(five,DAY_05_1_SOLUTION,DAY_05_2_SOLUTION),
	TESTCASE_WITH_ARG(testcase_six_p1, TEST_SIX_A, 4),
	TESTCASE_WITH_ARG(testcase_six_p1, TEST_SIX_B, 8),
	TESTCASE_WITH_ARG(testcase_six_p1, TEST_SIX_C, 9),
	TESTCASE_WITH_ARG(testcase_six_p1, TEST_SIX_D, 288),
	TESTCASE_WITH_ARG(testcase_six_p2, TEST_SIX_D, 71503),
	DAY(six,DAY_06_1_SOLUTION,DAY_06_2_SOLUTION),
#define DAY_SEVEN_TESTCASE_A(Day , Input , Result) TESTCASE_WITH_ARG(testcase_seven_p ## Day ## _a , Input, #Result)
	DAY_SEVEN_TESTCASE_A(1, "AAAAA",HandType::five_of_a_kind),
	DAY_SEVEN_TESTCASE_A(1, "33332",HandType::four_of_a_kind),
	DAY_SEVEN_TESTCASE_A(1, "2AAAA",HandType::four_of_a_kind),
	DAY_SEVEN_TESTCASE_A(1, "23332",HandType::full_house),
	DAY_SEVEN_TESTCASE_A(1, "77888",HandType::full_house),
	DAY_SEVEN_TESTCASE_A(1, "77788",HandType::full_house),
	DAY_SEVEN_TESTCASE_A(1, "TTT98",HandType::three_of_a_kind),
	DAY_SEVEN_TESTCASE_A(1, "23432",HandType::two_pair),
	DAY_SEVEN_TESTCASE_A(1, "A23A4",HandType::one_pair),
	DAY_SEVEN_TESTCASE_A(1, "23456",HandType::high_card),
	DAY_SEVEN_TESTCASE_A(1, TEST_SEVEN_A,HandType::one_pair),
	DAY_SEVEN_TESTCASE_A(1, TEST_SEVEN_B,HandType::three_of_a_kind),
	DAY_SEVEN_TESTCASE_A(1, TEST_SEVEN_C,HandType::two_pair),
	DAY_SEVEN_TESTCASE_A(1, TEST_SEVEN_D,HandType::two_pair),
	DAY_SEVEN_TESTCASE_A(1, TEST_SEVEN_E,HandType::three_of_a_kind),
	TESTCASE_WITH_ARG(testcase_seven_p1_b, TEST_SEVEN_FILE_A,6440),
	DAY_SEVEN_TESTCASE_A(2, "QJJQ2", HandType::four_of_a_kind),
	DAY_SEVEN_TESTCASE_A(2, "JKKK2", HandType::four_of_a_kind),
	DAY_SEVEN_TESTCASE_A(2, "QQQQ2", HandType::four_of_a_kind),
	DAY_SEVEN_TESTCASE_A(2, TEST_SEVEN_A,HandType::one_pair),
	DAY_SEVEN_TESTCASE_A(2, TEST_SEVEN_B,HandType::four_of_a_kind),
	DAY_SEVEN_TESTCASE_A(2, TEST_SEVEN_C,HandType::two_pair),
	DAY_SEVEN_TESTCASE_A(2, TEST_SEVEN_D,HandType::four_of_a_kind),
	DAY_SEVEN_TESTCASE_A(2, TEST_SEVEN_E,HandType::four_of_a_kind),
	TESTCASE_WITH_ARG(testcase_seven_p2_b, TEST_SEVEN_FILE_A,5905),
	DAY(seven,DAY_07_1_SOLUTION,DAY_07_2_SOLUTION),
#undef DAY_SEVEN_TESTCASE
	TESTCASE_WITH_ARG(testcase_eight_p1, TEST_EIGHT_A, 2),
	TESTCASE_WITH_ARG(testcase_eight_p1, TEST_EIGHT_B, 6),
	TESTCASE_WITH_ARG(testcase_eight_p2, TEST_EIGHT_C, 6),
	DAY(eight,DAY_08_1_SOLUTION,DAY_08_2_SOLUTION),
	TESTCASE_WITH_ARG(testcase_nine_p1,TEST_NINE_A,18),
	TESTCASE_WITH_ARG(testcase_nine_p1,TEST_NINE_B,28),
	TESTCASE_WITH_ARG(testcase_nine_p1,TEST_NINE_C,68),
	TESTCASE_WITH_ARG(testcase_nine_p1,TEST_NINE_D,114),
	TESTCASE_WITH_ARG(testcase_nine_p2,TEST_NINE_A,-3),
	TESTCASE_WITH_ARG(testcase_nine_p2,TEST_NINE_B,0),
	TESTCASE_WITH_ARG(testcase_nine_p2,TEST_NINE_C,5),
	TESTCASE_WITH_ARG(testcase_nine_p2,TEST_NINE_D,2),
	DAY(nine,DAY_09_1_SOLUTION,DAY_09_2_SOLUTION),
	TESTCASE_WITH_ARG(testcase_ten_p1, TEST_TEN_A,4),
	TESTCASE_WITH_ARG(testcase_ten_p1, TEST_TEN_B,4),
	TESTCASE_WITH_ARG(testcase_ten_p1, TEST_TEN_C,8),
	TESTCASE_WITH_ARG(testcase_ten_p1, TEST_TEN_D,8),
	TESTCASE_WITH_ARG(testcase_ten_p2, TEST_TEN_E,4),
	TESTCASE_WITH_ARG(testcase_ten_p2, TEST_TEN_F,8),
	TESTCASE_WITH_ARG(testcase_ten_p2, TEST_TEN_G,10),
	TESTCASE_WITH_ARG(testcase_ten_p2, TEST_TEN_H,1),
	DAY(ten, DAY_10_1_SOLUTION, DAY_10_2_SOLUTION),
	TESTCASE(testcase_eleven<2>, 374),
	TESTCASE(testcase_eleven<10>, 1030),
	TESTCASE(testcase_eleven<100>, 8410),
	DAY(eleven, DAY_11_1_SOLUTION, DAY_11_2_SOLUTION),
	TESTCASE_WITH_ARG(testcase_twelve_p1, TEST_TWELVE_A,1),
	TESTCASE_WITH_ARG(testcase_twelve_p1, TEST_TWELVE_B,4),
	TESTCASE_WITH_ARG(testcase_twelve_p1, TEST_TWELVE_C,1),
	TESTCASE_WITH_ARG(testcase_twelve_p1, TEST_TWELVE_D,1),
	TESTCASE_WITH_ARG(testcase_twelve_p1, TEST_TWELVE_E,4),
	TESTCASE_WITH_ARG(testcase_twelve_p1, TEST_TWELVE_F,10),
	TESTCASE_WITH_ARG(testcase_twelve_p1, TEST_TWELVE_G,21),
	TESTCASE_WITH_ARG(testcase_twelve_p2, TEST_TWELVE_A, 1),
	TESTCASE_WITH_ARG(testcase_twelve_p2, TEST_TWELVE_B, 16384),
	TESTCASE_WITH_ARG(testcase_twelve_p2, TEST_TWELVE_C, 1),
	TESTCASE_WITH_ARG(testcase_twelve_p2, TEST_TWELVE_D, 16),
	TESTCASE_WITH_ARG(testcase_twelve_p2, TEST_TWELVE_E, 2500),
	TESTCASE_WITH_ARG(testcase_twelve_p2, TEST_TWELVE_F, 506250),
	TESTCASE_WITH_ARG(testcase_twelve_p2, TEST_TWELVE_G, 525152),
	DAY(twelve, DAY_12_1_SOLUTION, DAY_12_2_SOLUTION),
	TESTCASE_WITH_ARG(testcase_thirteen_p1, TEST_THIRTEEN_A,5),
	TESTCASE_WITH_ARG(testcase_thirteen_p1, TEST_THIRTEEN_B,400),
	TESTCASE_WITH_ARG(testcase_thirteen_p1, TEST_THIRTEEN_C,405),
	TESTCASE_WITH_ARG(testcase_thirteen_p2, TEST_THIRTEEN_A, 300),
	TESTCASE_WITH_ARG(testcase_thirteen_p2, TEST_THIRTEEN_B, 100),
	TESTCASE_WITH_ARG(testcase_thirteen_p2, TEST_THIRTEEN_C, 400),
	DAY(thirteen, DAY_13_1_SOLUTION, DAY_13_2_SOLUTION),
	TESTCASE_WITH_ARG(testcase_fourteen_p1, TEST_FOURTEEN_A, 136),
	TESTCASE_WITH_ARG(testcase_fourteen_p2, TEST_FOURTEEN_A, 64),
	DAY(fourteen, DAY_14_1_SOLUTION, DAY_14_2_SOLUTION),
	TESTCASE_WITH_ARG(testcase_fifteen_p1, "rn=1", 30),
	TESTCASE_WITH_ARG(testcase_fifteen_p1, "cm-", 253),
	TESTCASE_WITH_ARG(testcase_fifteen_p1, "qp=3", 97),
	TESTCASE_WITH_ARG(testcase_fifteen_p1, "cm=2", 47),
	TESTCASE_WITH_ARG(testcase_fifteen_p1, "qp-", 14),
	TESTCASE_WITH_ARG(testcase_fifteen_p1, "pc=4", 180),
	TESTCASE_WITH_ARG(testcase_fifteen_p1, "ot=9", 9),
	TESTCASE_WITH_ARG(testcase_fifteen_p1, "ab=5", 197),
	TESTCASE_WITH_ARG(testcase_fifteen_p1, "pc-", 48),
	TESTCASE_WITH_ARG(testcase_fifteen_p1, "pc=6", 214),
	TESTCASE_WITH_ARG(testcase_fifteen_p1, "ot=7", 231),
	TESTCASE_WITH_ARG(testcase_fifteen_p1, TEST_FIFTEEN_A,1320),
	TESTCASE_WITH_ARG(testcase_fifteen_p2, TEST_FIFTEEN_A,145),
	DAY(fifteen, DAY_15_1_SOLUTION, DAY_15_2_SOLUTION),
	TESTCASE_WITH_ARG(testcase_sixteen_p1, TEST_SIXTEEN_A, 46),
	TESTCASE_WITH_ARG(testcase_sixteen_p2, TEST_SIXTEEN_A, 51),
	DAY(sixteen, DAY_16_1_SOLUTION, DAY_16_2_SOLUTION),
	TESTCASE_WITH_ARG(testcase_seventeen_p1, TEST_SEVENTEEN_A, 102),
	TESTCASE_WITH_ARG(testcase_seventeen_p2, TEST_SEVENTEEN_A, 94),
	TESTCASE_WITH_ARG(testcase_seventeen_p2, TEST_SEVENTEEN_B, 71),
	DAY(seventeen, DAY_17_1_SOLUTION, DAY_17_2_SOLUTION),
	TESTCASE_WITH_ARG(testcase_eighteen_p1, TEST_EIGHTEEN_A, 62),
	TESTCASE_WITH_ARG(testcase_eighteen_p2, TEST_EIGHTEEN_A, 952408144115),
	DAY(eighteen, DAY_18_1_SOLUTION, DAY_18_2_SOLUTION),
	TESTCASE_WITH_ARG(testcase_nineteen_p1, TEST_NINETEEN_A, 19114),
	TESTCASE_WITH_ARG(testcase_nineteen_p2, TEST_NINETEEN_A, 167409079868000),
	DAY(nineteen, DAY_19_1_SOLUTION, DAY_19_2_SOLUTION),
	TESTCASE_WITH_ARG(testcase_twenty_p1, TEST_TWENTY_A, 32000000),
	TESTCASE_WITH_ARG(testcase_twenty_p1, TEST_TWENTY_B, 11687500),
	DAY(twenty, DAY_20_1_SOLUTION, DAY_20_2_SOLUTION),
	TESTCASE_WITH_ARG(testcase_twentyone_p1, TEST_TWENTYONE_A, 16),
	DAY(twentyone, DAY_21_1_SOLUTION, DAY_21_2_SOLUTION),
	TESTCASE_WITH_ARG(testcase_twentytwo_p1,TEST_TWENTYTWO_A, 5),
	TESTCASE_WITH_ARG(testcase_twentytwo_p2,TEST_TWENTYTWO_A, 7),
	DAY(twentytwo, DAY_22_1_SOLUTION, DAY_22_2_SOLUTION),
	TESTCASE_WITH_ARG(testcase_twentythree_p1, TEST_TWENTYTHREE_A, 94),
	TESTCASE_WITH_ARG(testcase_twentythree_p2, TEST_TWENTYTHREE_A, dummy),
	DAY(twentythree, DAY_23_1_SOLUTION, DAY_23_2_SOLUTION),
	DAY(twentyfour, DAY_24_1_SOLUTION, DAY_24_2_SOLUTION),
	DAY(twentyfive, DAY_25_1_SOLUTION,"MERRY CHRISTMAS!")
};

#undef ARG
#undef TESTCASE
#undef FUNC_NAME
#undef TEST_DECL
#undef DAY
#undef DUMMY
#undef DUMMY_DAY
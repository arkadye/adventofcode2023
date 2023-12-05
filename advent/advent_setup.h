#pragma once

#include "advent_testcase_setup.h"
#include "advent_test_inputs.h"
#include "advent_headers.h"

static const std::string DEFAULT_FILTER = "";

static const verification_test tests[] =
{
	TESTCASE_WITH_ARG(testcase_one_p1, TEST_ONE_A, 12),
	TESTCASE_WITH_ARG(testcase_one_p1, TEST_ONE_B, 38),
	TESTCASE_WITH_ARG(testcase_one_p1, TEST_ONE_C, 15),
	TESTCASE_WITH_ARG(testcase_one_p1, TEST_ONE_D, 77),
	TESTCASE_WITH_ARG(testcase_one_p1, TEST_FILE_A, 142),
	TESTCASE_WITH_ARG(testcase_one_p2, TEST_ONE_F, 29),
	TESTCASE_WITH_ARG(testcase_one_p2, TEST_ONE_G, 83),
	TESTCASE_WITH_ARG(testcase_one_p2, TEST_ONE_H, 13),
	TESTCASE_WITH_ARG(testcase_one_p2, TEST_ONE_I, 24),
	TESTCASE_WITH_ARG(testcase_one_p2, TEST_ONE_J, 42),
	TESTCASE_WITH_ARG(testcase_one_p2, TEST_ONE_K, 14),
	TESTCASE_WITH_ARG(testcase_one_p2, TEST_ONE_L, 76),
	TESTCASE_WITH_ARG(testcase_one_p2, TEST_FILE_B, 281),
	DAY(one,dummy,dummy),
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
	DAY(two,dummy,dummy),
	TESTCASE(testase_three_p1,4361),
	TESTCASE(testase_three_p2,467835),
	DAY(three,dummy,dummy),
	TESTCASE_WITH_ARG(testcase_four_p1, TEST_FOUR_A,8),
	TESTCASE_WITH_ARG(testcase_four_p1, TEST_FOUR_B,2),
	TESTCASE_WITH_ARG(testcase_four_p1, TEST_FOUR_C,2),
	TESTCASE_WITH_ARG(testcase_four_p1, TEST_FOUR_D,1),
	TESTCASE_WITH_ARG(testcase_four_p1, TEST_FOUR_E,0),
	TESTCASE_WITH_ARG(testcase_four_p1, TEST_FOUR_F,0),
	TESTCASE_WITH_ARG(testcase_four_p1, TEST_FOUR_G,13),
	TESTCASE_WITH_ARG(testcase_four_p2, TEST_FOUR_G,30),
	DAY(four,dummy,dummy),
	TESTCASE_WITH_ARG(testcase_five_p1_a<79>, TEST_FIVE_A,82),
	TESTCASE_WITH_ARG(testcase_five_p1_a<14>, TEST_FIVE_A,43),
	TESTCASE_WITH_ARG(testcase_five_p1_a<55>, TEST_FIVE_A,86),
	TESTCASE_WITH_ARG(testcase_five_p1_a<13>, TEST_FIVE_A,35),
	TESTCASE_WITH_ARG(testcase_five_p1_b, TEST_FIVE_A,35),
	TESTCASE_WITH_ARG(testcase_five_p2_b, TEST_FIVE_A,46),
	DAY(five,dummy,dummy),
	DAY(six,dummy,dummy),
	DAY(seven,dummy,dummy),
	DAY(eight,dummy,dummy),
	DAY(nine,dummy,dummy),
	DAY(ten,dummy,dummy),
	DAY(eleven,dummy,dummy),
	DAY(twelve,dummy,dummy),
	DAY(thirteen,dummy,dummy),
	DAY(fourteen,dummy,dummy),
	DAY(fifteen,dummy,dummy),
	DAY(sixteen,dummy,dummy),
	DAY(seventeen,dummy,dummy),
	DAY(eighteen,dummy,dummy),
	DAY(nineteen,dummy,dummy),
	DAY(twenty,dummy,dummy),
	DAY(twentyone,dummy,dummy),
	DAY(twentytwo,dummy,dummy),
	DAY(twentythree,dummy,dummy),
	DAY(twentyfour,dummy,dummy),
	DAY(twentyfive,dummy,"MERRY CHRISTMAS!")
};

#undef ARG
#undef TESTCASE
#undef FUNC_NAME
#undef TEST_DECL
#undef DAY
#undef DUMMY
#undef DUMMY_DAY
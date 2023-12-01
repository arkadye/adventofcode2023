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
	DAY(two,dummy,dummy),
	DAY(three,dummy,dummy),
	DAY(four,dummy,dummy),
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
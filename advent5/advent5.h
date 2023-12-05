#pragma once

#include "../advent/advent_types.h"

namespace advent5_internal
{
	ResultType p1_a(std::istream& input, uint64_t seed);
	ResultType p2_a(std::istream& input, uint64_t seed);
}

template <uint64_t ARG>
inline ResultType testcase_five_p1_a(std::istream& input) { return advent5_internal::p1_a(input,ARG); }
ResultType testcase_five_p1_b(std::istream& input);

template <uint64_t ARG>
inline ResultType testcase_five_p2_a(std::istream& input) { return advent5_internal::p2_a(input, ARG); }
ResultType testcase_five_p2_b(std::istream& input);

ResultType advent_five_p1();
ResultType advent_five_p2();
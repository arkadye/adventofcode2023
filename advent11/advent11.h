#pragma once

#include "../advent/advent_types.h"

namespace eleven_internal
{
	ResultType testcase(std::size_t factor);
}

template <std::size_t factor>
ResultType testcase_eleven() { return eleven_internal::testcase(factor); }

ResultType advent_eleven_p1();
ResultType advent_eleven_p2();
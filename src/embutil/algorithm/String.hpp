//
// String.hpp
//
// Created on: May 19, 2021
//     Author: Dmitry Murashov (d.murashov@geoscan.aero)
//

// TODO: constexpr whitespace alignment based on calculating lenght's of strings in an array

#ifndef UTILITY_UTILITY_ALGORITHM_STRING_HPP_
#define UTILITY_UTILITY_ALGORITHM_STRING_HPP_

#include <cstdint>

namespace Ut {
namespace Al {

bool stringIsEndswith(const char *aStr, const char *aSuffix);

/// \brief Constexpr evaluation of string length.
/// \pre `str` MUST be terminated w/ '\0'
constexpr std::size_t stringCxStrlen(const char *str, std::size_t pos = 0)
{
	return str[pos] == '\0' ? pos : stringCxStrlen(str, pos + 1);
}

/// \brief Performs constexpr comparison of 2 strings
/// \returns the same value as `strcmp`
/// (https://en.cppreference.com/w/cpp/string/byte/strcmp)
constexpr int stringCxStrcmp(const char *lhs, const char *rhs, std::size_t pos = 0)
{
	return lhs[pos] < rhs[pos] ? -1 :
		lhs[pos] > rhs[pos] ? 1 :
		lhs[pos] == 0 ? 0 : stringCxStrcmp(lhs, rhs, pos + 1);
}

static_assert(stringCxStrcmp("", "") == 0, "");
static_assert(stringCxStrcmp("a", "") == 1, "");
static_assert(stringCxStrcmp("", "a") == -1, "");
static_assert(stringCxStrcmp("echoe", "echo") == 1, "");
static_assert(stringCxStrcmp("echo", "echo") == 0, "");
static_assert(stringCxStrcmp("echo", "echoe") == -1, "");
static_assert(stringCxStrcmp("echoa", "echoz") == -1, "");
static_assert(stringCxStrcmp("echoz", "echoa") == 1, "");

}  // namespace Al
}  // namespace Ut

#endif // UTILITY_UTILITY_ALGORITHM_STRING_HPP_

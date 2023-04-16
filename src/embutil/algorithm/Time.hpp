//
// Time.hpp
//
// Created on: Mar 14, 2023
//     Author: Dmitry Murashov (d.murashov@geoscan.aero)
//

#ifndef UTILITY_UTILITY_ALGORITHM_TIME_HPP_
#define UTILITY_UTILITY_ALGORITHM_TIME_HPP_

#include <chrono>

namespace Ut {
namespace Al {

/// Fallback routine that enables convesion of time types into integral types.
template <class Rep, class Period>
static std::size_t timeToIntegral(const std::chrono::duration<Rep, Period> &aDuration)
{
	return static_cast<std::size_t>(aDuration.count());
}

template <class DirectIntegralCastType>
auto timeToIntegral(const DirectIntegralCastType &aTime) -> decltype(static_cast<std::size_t>(aTime))
{
	return static_cast<std::size_t>(aTime);
}

}  // namespace Al
}  // namespace Ut

#endif // UTILITY_UTILITY_ALGORITHM_TIME_HPP_

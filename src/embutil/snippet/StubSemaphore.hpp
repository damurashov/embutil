//
// StubSemaphore.hpp
//
// Created on: Mar 24, 2023
//     Author: Dmitry Murashov (d.murashov@geoscan.aero)
//

#ifndef UTILITY_UTILITY_SNIPPET_STUBSEMAPHORE_HPP
#define UTILITY_UTILITY_SNIPPET_STUBSEMAPHORE_HPP

namespace Ut {
namespace Sn {

struct StubSemaphore {

	constexpr bool acquire()
	{
		return false;
	}

	constexpr bool release()
	{
		return false;
	}

	constexpr bool tryAcquire()
	{
		return false;
	}

	template <class TimeType>
	constexpr bool tryAcquireFor(TimeType &&)
	{
		return false;
	}

};

}  // namespace Sn
}  // namespace Ut

#endif // UTILITY_UTILITY_SNIPPET_STUBSEMAPHORE_HPP

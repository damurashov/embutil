//
// StubMutex.hpp
//
// Created on: Feb 10, 2023
//     Author: Dmitry Murashov (d.murashov@geoscan.aero)
//

#ifndef UTILITY_UTILITY_SNIPPET_STUBMUTEX_HPP_
#define UTILITY_UTILITY_SNIPPET_STUBMUTEX_HPP_

namespace Ut {
namespace Sn {

struct StubMutex {
	constexpr StubMutex()
	{
	}

	constexpr bool lock()
	{
		return false;
	}

	constexpr bool unlock()
	{
		return false;
	}
};

}  // namespace Sn
}  // namespace Ut

#endif // UTILITY_UTILITY_SNIPPET_STUBMUTEX_HPP_

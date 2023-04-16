//
// Algorithm.hpp
//
// Created on: Feb 09, 2023
//     Author: Dmitry Murashov (d.murashov@geoscan.aero)
//

#ifndef UTILITY_UTILITY_ALGORITHM_ALGORITHM_HPP_
#define UTILITY_UTILITY_ALGORITHM_ALGORITHM_HPP_

#include <cstring>
#include <limits>
#include <utility>

namespace Ut {
namespace Al {


/// \brief Normalizes the value
/// \tparam T Input value type
/// \tparam O Output value type
template <typename O, typename T>
O normalize(T val, T from, T to)
{
	assert(to - from > std::numeric_limits<T>::epsilon());

	return static_cast<O>(val - from) / static_cast<O>(to - from);
}

/// \brief De-normalizes the value
/// \tparam T Input value type
/// \tparam O Output value type
template <class T, class O>
O scale(T val, O from, O to)
{
	assert(to - from > std::numeric_limits<T>::epsilon());

	return static_cast<O>((static_cast<T>(to) - static_cast<T>(from)) * val + static_cast<T>(from));
}

/// \brief Fits the value in a specified range
/// \tparam T Input value type
/// \tparam O Output value type
template <typename O, typename T>
O clamp(T val, T from, T to)
{
	assert(to - from > std::numeric_limits<float>::epsilon());

	return static_cast<O>(val) < static_cast<O>(from) ? static_cast<O>(from) :
		static_cast<O>(val) > static_cast<O>(to) ? to :
		val;
}

template <class T>
inline bool in(T &&base)
{
	return false;
}

template <class T, class T1, class ...Ts>
inline bool in(T &&base, T1 &&object, Ts &&...objects)
{
	return base == object || in(base, std::forward<Ts>(objects)...);
}

template <class T>
inline bool isPow2(T aInteger)
{
	return aInteger > 0 ?
		aInteger == 1 ?
			aInteger >> 1 == 0 :
			isPow2Ce(aInteger >> 1) :
		false;
}

template <class T>
inline constexpr bool isPow2Ce(T aInteger)
{
	return aInteger > 0 ?
		aInteger == 1 ?
			aInteger >> 1 == 0 :
			isPow2Ce(aInteger >> 1) :
		false;
}

template <class T1, class T2>
inline void bytewiseMove(T1 *aDst, T2 *aSrc, std::size_t aNbytes = sizeof(T1) > sizeof(T2) ? sizeof(T2) : sizeof(T1))
{
	memcpy(reinterpret_cast<void *>(aDst), reinterpret_cast<void *>(aSrc), aNbytes);
}

namespace Impl {

template <typename... Fns>
struct Visitor;

template <typename Fn>
struct Visitor<Fn> : Fn
{
    using Fn::operator();

    template<typename T>
    Visitor(T&& fn) : Fn(std::forward<T>(fn)) {}
};

template <typename Fn, typename... Fns>
struct Visitor<Fn, Fns...> : Fn, Visitor<Fns...>
{
    using Fn::operator();
    using Visitor<Fns...>::operator();

    template<typename T, typename... Ts>
    Visitor(T&& fn, Ts&&... fns)
        : Fn(std::forward<T>(fn))
        , Visitor<Fns...>(std::forward<Ts>(fns)...) {}
};

template <typename... Fns>
Visitor<typename std::decay<Fns>::type...> makeVisitor(Fns&&... fns)
{
    return Visitor<typename std::decay<Fns>::type...>
        (std::forward<Fns>(fns)...);
}

}  // namespace Impl

using Impl::makeVisitor;

}  // namespace Al
}  // namespace Ut

#endif // UTILITY_UTILITY_ALGORITHM_ALGORITHM_HPP_

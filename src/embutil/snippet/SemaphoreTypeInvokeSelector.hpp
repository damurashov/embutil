//
// SemaphoreTypeInvokeSelector.hpp
//
// Created on: Mar 23, 2023
//     Author: Dmitry Murashov (d.murashov@geoscan.aero)
//

#ifndef UTILITY_UTILITY_SNIPPET_SEMAPHORETYPEINVOKESELECTOR_HPP
#define UTILITY_UTILITY_SNIPPET_SEMAPHORETYPEINVOKESELECTOR_HPP

#include <utility>

namespace Ut {
namespace Sn {

struct SemaphoreTypeInvokeSelector {
	template <class S, class ...Ts>
	static inline auto release(S &s, Ts &&...aArguments) -> decltype(s.notify(std::forward<Ts>(aArguments)...))
	{
		return s.notify(std::forward<Ts>(aArguments)...);
	}

	template <class S, class ...Ts>
	static inline auto release(S &s, Ts &&...aArguments) -> decltype(s.post(std::forward<Ts>(aArguments)...))
	{
		return s.post(std::forward<Ts>(aArguments)...);
	}

	template <class S, class ...Ts>
	static inline auto release(S &s, Ts &&...aArguments) -> decltype(s.release(std::forward<Ts>(aArguments)...))
	{
		return s.release(std::forward<Ts>(aArguments)...);
	}

	template <class S, class ...Ts>
	static inline auto acquire(S &s, Ts &&...aArguments) -> decltype(s.acquire(std::forward<Ts>(aArguments)...))
	{
		return s.acquire(std::forward<Ts>(aArguments)...);
	}

	template <class S, class ...Ts>
	static inline auto acquire(S &s, Ts &&...aArguments) -> decltype(s.wait(std::forward<Ts>(aArguments)...))
	{
		return s.wait(std::forward<Ts>(aArguments)...);
	}

	template <class S, class ...Ts>
	static inline auto acquire(S &s, Ts &&...aArguments) -> decltype(s.take(std::forward<Ts>(aArguments)...))
	{
		return s.take(std::forward<Ts>(aArguments)...);
	}

	template <class S, class ...Ts>
	static inline auto tryAcquire(S &s, Ts &&...aArguments) -> decltype(s.tryWait(std::forward<Ts>(aArguments)...))
	{
		return s.tryWait(std::forward<Ts>(aArguments)...);
	}

	template <class S, class ...Ts>
	static inline auto tryAcquire(S &s, Ts &&...aArguments) -> decltype(s.try_wait(std::forward<Ts>(aArguments)...))
	{
		return s.try_wait(std::forward<Ts>(aArguments)...);
	}

	template <class S, class ...Ts>
	static inline auto tryAcquire(S &s, Ts &&...aArguments) -> decltype(s.tryAcquire(std::forward<Ts>(aArguments)...))
	{
		return s.tryAcquire(std::forward<Ts>(aArguments)...);
	}

	template <class S, class ...Ts>
	static inline auto tryAcquire(S &s, Ts &&...aArguments) -> decltype(s.try_acquire(std::forward<Ts>(aArguments)...))
	{
		return s.try_acquire(std::forward<Ts>(aArguments)...);
	}

	template <class S, class ...Ts>
	static inline auto tryAcquire(S &s, Ts &&...aArguments) -> decltype(s.tryTake(std::forward<Ts>(aArguments)...))
	{
		return s.tryTake(std::forward<Ts>(aArguments)...);
	}

	template <class S, class ...Ts>
	static inline auto tryAcquire(S &s, Ts &&...aArguments) -> decltype(s.try_take(std::forward<Ts>(aArguments)...))
	{
		return s.try_take(std::forward<Ts>(aArguments)...);
	}

	template <class S, class ...Ts>
	static inline auto tryAcquireFor(S &s, Ts &&...aArguments) -> decltype(s.tryAcquireFor(std::forward<Ts>(aArguments)...))
	{
		return s.tryAcquireFor(std::forward<Ts>(aArguments)...);
	}

	template <class S, class ...Ts>
	static inline auto tryAcquireFor(S &s, Ts &&...aArguments) -> decltype(s.try_acquire_for(std::forward<Ts>(aArguments)...))
	{
		return s.try_acquire_for(std::forward<Ts>(aArguments)...);
	}

	template <class S, class ...Ts>
	static inline auto tryAcquireFor(S &s, Ts &&...aArguments) -> decltype(s.tryTakeFor(std::forward<Ts>(aArguments)...))
	{
		return s.tryTakeFor(std::forward<Ts>(aArguments)...);
	}

	template <class S, class ...Ts>
	static inline auto tryAcquireFor(S &s, Ts &&...aArguments) -> decltype(s.try_take_for(std::forward<Ts>(aArguments)...))
	{
		return s.try_take_for(std::forward<Ts>(aArguments)...);
	}

	template <class S, class ...Ts>
	static inline auto tryAcquireFor(S &s, Ts &&...aArguments) -> decltype(s.tryWaitFor(std::forward<Ts>(aArguments)...))
	{
		return s.tryWaitFor(std::forward<Ts>(aArguments)...);
	}

	template <class S, class ...Ts>
	static inline auto tryAcquireFor(S &s, Ts &&...aArguments) -> decltype(s.try_wait_for(std::forward<Ts>(aArguments)...))
	{
		return s.try_wait_for(std::forward<Ts>(aArguments)...);
	}
};

}  // namespace Sn
}  // namespace Ut

#endif // UTILITY_UTILITY_SNIPPET_SEMAPHORETYPEINVOKESELECTOR_HPP

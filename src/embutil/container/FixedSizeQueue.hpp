//
// FixedSizeQueue.hpp
//
// Created on: Mar 06, 2023
//     Author: Dmitry Murashov (d.murashov@geoscan.aero)
//

#ifndef UTILITY_UTILITY_CONTAINER_FIXEDSIZEQUEUE_HPP_
#define UTILITY_UTILITY_CONTAINER_FIXEDSIZEQUEUE_HPP_

#include "utility/algorithm/Algorithm.hpp"
#include <array>
#include <cstdint>

namespace Ut {
namespace Ct {

/// Fixed-size queue for buffers and alike.
///
/// The queue is thread safe for as long as there are only 2 threads operating
/// on it: one reads, and the other one - pushes
template <class T, std::size_t N>
class FixedSizeQueue {
	static_assert(Ut::Al::isPow2Ce(N), "Queue size must be a power of 2");

private:
	using MemoryChunk = std::uint8_t[sizeof(T)];

public:
	void forcePush(const T &aInstance)
	{
		if (!tryPush(aInstance)) {
			T t;
			tryPop(t);
			tryPush(aInstance);
		}
	}

	bool tryPush(const T &aInstance)
	{
		if (count() < N) {
			new (reinterpret_cast<void *>(storage[absolutePosition(pushPosition)])) T{aInstance};
			++pushPosition;

			return true;
		} else {
			return false;
		}
	}

	template <class ...Ts>
	bool tryEmplace(Ts &&...aArgs)
	{
		if (count() < N) {
			new (reinterpret_cast<void *>(storage[absolutePosition(pushPosition)])) T{std::forward<Ts>(aArgs)...};
			++pushPosition;

			return true;
		} else {
			return false;
		}
	}

	bool tryPop(T &ret)
	{
		if (count()) {
			T *element = reinterpret_cast<T *>(storage[absolutePosition(popPosition)]);
			ret = std::move(*element);
			element->~T();
			++popPosition;

			return true;
		}

		return false;
	}

	/// Number of elements stored in the queue
	std::size_t count()
	{
		return pushPosition - popPosition;
	}

private:
	/// Implements fast modulo `a % 2^N == a & (2^N - 1)`
	std::size_t absolutePosition(std::size_t aAccumulatedPosition)
	{
		return aAccumulatedPosition & (N - 1);
	}

private:
	std::array<MemoryChunk, N> storage;
	std::size_t popPosition;
	std::size_t pushPosition;
};

}  // namespace Ct
}  // namespace Ut

#endif // UTILITY_UTILITY_CONTAINER_FIXEDSIZEQUEUE_HPP_

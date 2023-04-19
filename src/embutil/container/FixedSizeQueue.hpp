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
	template <class OwnerType, class ItemType>
	struct Iterator {

		inline friend bool operator!=(const Iterator &aLhs, const Iterator &aRhs)
		{
			return aLhs.position != aRhs.position;
		}

		OwnerType &owner;
		std::size_t position;

		ItemType &operator*()
		{
			return *reinterpret_cast<ItemType *>(owner.storage[owner.absolutePosition(position)]);
		}

		/// Pre-increment overload
		Iterator &operator++()
		{
			++position;
			return *this;
		}

		Iterator operator++(int)
		{
			auto iterator = *this;
			++position;

			return iterator;
		}
	};

	Iterator<FixedSizeQueue<T, N>, T> begin()
	{
		return {*this, popPosition};
	}

	Iterator<FixedSizeQueue<T, N>, T> end()
	{
		return {*this, pushPosition};
	}

	Iterator<const FixedSizeQueue<T, N>, const T> cbegin() const
	{
		return {*this, popPosition};
	}

	Iterator<const FixedSizeQueue<T, N>, const T> cend() const
	{
		return {*this, pushPosition};
	}

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
	std::size_t count() const
	{
		return pushPosition - popPosition;
	}

private:
	/// Implements fast modulo `a % 2^N == a & (2^N - 1)`
	std::size_t absolutePosition(std::size_t aAccumulatedPosition) const
	{
		return aAccumulatedPosition & (N - 1);
	}

private:
	std::array<MemoryChunk, N> storage;
	std::size_t popPosition = 0;
	std::size_t pushPosition = 0;
};

}  // namespace Ct
}  // namespace Ut

#endif // UTILITY_UTILITY_CONTAINER_FIXEDSIZEQUEUE_HPP_

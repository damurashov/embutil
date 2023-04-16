//
// Indexed.hpp
//
// Created on: Mar 15, 2023
//     Author: Dmitry Murashov (d.murashov@geoscan.aero)
//

#ifndef UTILITY_UTILITY_ALGORITHM_INDEXED_HPP_
#define UTILITY_UTILITY_ALGORITHM_INDEXED_HPP_

#include "utility/algorithm/Algorithm.hpp"
#include <cstring>

namespace Ut {
namespace Al {

/// Shifts elements residing to the right from `aOffset` to the left
template <class T>
void indexedShiftAt(T *aArray, std::size_t aArraySize, std::size_t aOffset)
{
	if (aArray != nullptr && aArraySize > 1 && aOffset < aArraySize - 1) {
		void *dst = reinterpret_cast<void *>(&aArray[aOffset]);
		void *src = reinterpret_cast<void *>(&aArray[aOffset + 1]);
		std::size_t nElements = aArraySize - aOffset - 1;
		// Overwrite
		memcpy(dst, src, nElements);
	}
}

/// Like `indexedShiftAt`, but preserves the element at `aOffsetAt` and moves
/// it to the end of the array
template <class T>
void indexedShiftBackAt(T *aArray, std::size_t aArraySize, std::size_t aOffsetAt)
{
	if (aArray != nullptr && aArraySize > 1 && aOffsetAt < aArraySize - 1) {
		std::uint8_t buffer[sizeof(T)] = {0};
		bytewiseMove(buffer, &aArray[aOffsetAt], sizeof(T));
		indexedShiftAt(aArray, aArraySize, aOffsetAt);
		bytewiseMove(aArray[aArraySize - 1], buffer, sizeof(T));
	}
}

}  // namespace Al
}  // namespace Ut

#endif // UTILITY_UTILITY_ALGORITHM_INDEXED_HPP_

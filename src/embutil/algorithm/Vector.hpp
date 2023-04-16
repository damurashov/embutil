//
// Vector.hpp
//
// Created on: Mar 07, 2023
//     Author: Dmitry Murashov (d.murashov@geoscan.aero)
//

#ifndef UTILITY_UTILITY_ALGORITHM_VECTOR_HPP_
#define UTILITY_UTILITY_ALGORITHM_VECTOR_HPP_

#include <vector>
#include <utility>

namespace Ut {
namespace Al {

/// Time-efficient removal of an element from a vector.
/// \returns Number of removed elements
template <class Item, class Callable>
std::size_t vectorSwapEraseIf(std::vector<Item> &aVector, Callable &&aCallable)
{
	std::size_t ret = 0;

	for (std::size_t i = 0; i < aVector.size();) {
		if (aCallable(aVector[i])) {
			std::swap(aVector[i], aVector[aVector.size() - 1]);
			aVector.pop_back();
			++ret;
		} else {
			++i;
		}
	}

	return ret;
}


template <class T>
void vectorSwapEraseAt(std::vector<T> &aVector, std::size_t aAt)
{
	if (aAt < aVector.size()) {
		if (aAt != aVector.size() - 1) {
			std::swap(aVector[aAt], aVector.back());
		}
		aVector.pop_back();
	}
}

/// Inserts an element in a vector in a sorted order
/// \tparam T element type
/// \tparam C callableType
/// \tparam ElementReferenceType serves as a boilerplate reducer through
///  exploiting "universal references"
///
/// Given that consistency is maintained, and \arg implements "is greater than"
/// comparison, the elements will be sorted in ascending order.
///
/// \arg aIsGreaterThan(aElement, aItem) returns true, if `aElement > aItem`.
/// Reverse this to ensure the opposite ordering.
template <class T, class C, class ElementReferenceType>
void vectorInsertSorted(std::vector<T> &aVector, C &&aIsGreaterThan, ElementReferenceType &&aElement)
{
	auto insertIterator = std::lower_bound(aElement, aVector.begin(), aVector.end(), aIsGreaterThan);  // Find the first element for which `aIsGreaterThan(aElement, aVectorElement)` is false
	aVector.insert(insertIterator, std::forward<ElementReferenceType>(aElement));
}

}  // namespace Al
}  // namespace Ut

#endif // UTILITY_UTILITY_ALGORITHM_VECTOR_HPP_

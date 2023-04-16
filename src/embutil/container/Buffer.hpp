//
// Buffer.hpp
//
// Created on: Mar 29, 2021
//     Author: Dmitry Murashov (d.murashov@geoscan.aero)
//

#ifndef COMPONENTS_UTILITY_UTILITY_BUFFER_HPP
#define COMPONENTS_UTILITY_UTILITY_BUFFER_HPP

#include <array>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <utility>

namespace Ut {
namespace Ct {

namespace Impl {

// void, const void, and const volatile void should be interpreted as std::uint8_t

template <typename T>
struct InterpType {
	using Type = T;
};

template <>
struct InterpType<void> {
	using Type = uint8_t;
};

template <>
struct InterpType<const void> {
	using Type = const uint8_t;
};

template <>
struct InterpType<const volatile void> {
	using Type = const uint8_t;
};

}  // namespace Impl

// Buffer

template <typename T>
struct Buffer : std::tuple<T *, std::size_t> {
	using std::tuple<T *, std::size_t>::tuple;
	using std::tuple<T *, std::size_t>::operator=;

	using Type = T;
	using InterpType = typename Impl::InterpType<T>::Type;  // T, or one of the uint8_t's derivatives, if T is void

	T *data() const;
	std::size_t size() const;
	InterpType &at(std::uint8_t aPos);
	Buffer<T> asSlice(std::size_t aOffset) const;  ///< Constructs a slice from a given offset
	Buffer<T> asSlice(std::size_t aOffsetBegin, std::size_t aOffsetEnd) const;  /// Constructs a slice satisfying the given range [aOffsetBegin; aOffsetEnd)
	Buffer<T> &slice(std::size_t aOffset)
	{
		*this = asSlice(aOffset);

		return *this;
	}
	Buffer<T> &slice(std::size_t aOffsetBegin, std::size_t aOffsetEnd)
	{
		*this = asSlice(aOffsetBegin, aOffsetEnd);

		return *this;
	}

	/// \pre Stored pointer does not equal `nullptr`
	inline T *begin() const
	{
		return data();
	}

	T *end() const
	{
		return data() + size();
	}

	inline const T *cbegin() const
	{
		return data();
	}

	inline const T *cend() const
	{
		return data() + size();
	}

	template <typename TypeTo>
	Buffer<TypeTo> as();  ///< Reinterpret the buffer for another type. The type's sizeof() is taken into consideration, so size() of the newly constructed instance may differ
};

template <typename Tto, typename Tfrom>
Buffer<Tto> toBuffer(Tfrom *aFromPtr, std::size_t aBufferSize);

///
/// \brief toBuffer Constructs buffer from an instance for which
/// ::data and ::size methods are defined
///
/// \tparam TdataTraitFrom - some instance defining ::data and ::size
/// parameters
///
/// \return
///
template <typename Tto, typename TdataTraitFrom>
Buffer<Tto> toBuffer(TdataTraitFrom &&);

using MemoryBuffer = Buffer<void>;
using ConstMemoryBuffer = Buffer<const void>;
using CharBuffer = Buffer<char>;
using ConstCharBuffer = Buffer<const char>;

}  // namespace Ct
}  // namespace Ut

#include "Buffer.impl"

#endif  // BUFFER_HPP

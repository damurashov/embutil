//
// IdentifiedInstance.hpp
//
// Created on: Mar 16, 2023
//     Author: Dmitry Murashov (d.murashov@geoscan.aero)
//

#ifndef UTILITY_UTILITY_SNIPPET_IDENTIFIEDINSTANCE_HPP_
#define UTILITY_UTILITY_SNIPPET_IDENTIFIEDINSTANCE_HPP_

#include "utility/snippet/LockWrapper.hpp"
#include "utility/snippet/StubMutex.hpp"
#include <atomic>
#include <cstdint>

namespace Ut {
namespace Sn {

/// Associates instance with a unique identifier. Copying causes atomic
/// indentifier increment
template <class DerivedType, class MutexType = Ut::Sn::StubMutex>
class IdentifiedInstance {
private:
	using AtomicIdentifierType = Ut::Sn::LockWrapper<std::size_t, MutexType>;

public:
	IdentifiedInstance() :
		mIdentifier{nextIdentifierSynchronized()}
	{
	}

	IdentifiedInstance(const IdentifiedInstance &aOther) :
		IdentifiedInstance()
	{
	}

	IdentifiedInstance(IdentifiedInstance &&aOther) :
		mIdentifier{aOther.identifiedInstance}
	{
	}

	IdentifiedInstance &operator=(const IdentifiedInstance &aOther) = default;
	IdentifiedInstance &operator=(IdentifiedInstance &&) = default;

	std::size_t identifier() const
	{
		return mIdentifier;
	}

private:
	static std::size_t nextIdentifierSynchronized()
	{
		auto lockedIndentifierBoundary = identifierBoundary.makeLock();
		std::size_t identifier = *lockedIndentifierBoundary;
		++(*lockedIndentifierBoundary);

		return identifier;
	}

private:
	std::size_t mIdentifier;
	static AtomicIdentifierType identifierBoundary;
};

template <class T1, class T2>
IdentifiedInstance<T1, T2>::AtomicIdentifierType IdentifiedInstance<T1, T2>::identifierBoundary{0};

}  // namespace Sn
}  // namespace Ut

#endif // UTILITY_UTILITY_SNIPPET_IDENTIFIEDINSTANCE_HPP_

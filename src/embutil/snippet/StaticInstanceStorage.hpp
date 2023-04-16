//
// StaticInstanceStorage.hpp
//
// Created on: Feb 10, 2023
//     Author: Dmitry Murashov (d.murashov@geoscan.aero)
//

#ifndef UTILITY_UTILITY_SNIPPET_STATICINSTANCESTORAGE_HPP_
#define UTILITY_UTILITY_SNIPPET_STATICINSTANCESTORAGE_HPP_

#include "utility/OhDebug.hpp"
#include "utility/algorithm/Vector.hpp"
#include "utility/snippet/LockWrapper.hpp"
#include "utility/snippet/StubMutex.hpp"
#include <algorithm>
#include <vector>

namespace Ut {
namespace Sn {

/// \brief Static instance storage w/ synchronization capabilities
template <class StoredType, class Mutex = StubMutex, std::size_t knInstances = 4U>
class StaticInstanceStorage {
private:
	struct StorageType : std::vector<StoredType> {
		StorageType() :
			std::vector<StoredType>{}
		{
			this->reserve(knInstances);
			this->clear();
		}
	};
	using InstanceStorage = typename Ut::Sn::LockWrapper<StorageType, Mutex>;

public:
	static std::size_t size()
	{
		return instanceStorage.instanceUnsafe().size();
	}

protected:
	/// \brief Multipurpose iterator over all of the storeed instances. The
	/// iteration stops, once `cb` returns false. Returns pointer to a current
	/// item, or `nullptr`, if the iterator reached the end.
	///
	/// \tparam `Callable` must be a callable type of the following signature:
	/// `bool cb(T &, Args...)`.
	///
	/// \example
	/// ```
	/// T *instance = StaticInstanceStorage::findIf([](StoredType &candidate, int id) {return candidate.id == id; }, 42);
	///
	/// if (instance != nullptr) {
	/// 	...
	/// }
	/// ```
	template <class Callable>
	static void iterInstancesWhile(Callable &&cb)
	{
		auto lockedInstanceStorage = instanceStorage.makeLock();

		for (auto &instance : *lockedInstanceStorage) {
			if (!cb(instance)) {

				break;
			}
		}
	}

	template <class Callable>
	static void removeInstanceIf(Callable &&aCallable)
	{
		auto lockedInstanceStorage = instanceStorage.makeLock();
		Al::vectorSwapEraseIf(*lockedInstanceStorage, aCallable);
		OHDEBUG("Ut::Sn::StaticInstanceStorage", "removed instance, size() =", lockedInstanceStorage->size());
	}

	static void storeInstance(const StoredType &aStoredType)
	{
		auto lockedInstanceStorage = instanceStorage.makeLock();
		lockedInstanceStorage->push_back(aStoredType);
		OHDEBUG("Ut::Sn::StaticInstanceStorage", "added instance, size() = ", lockedInstanceStorage->size());
	}

	/// Stores instance in the instance storage, and invokes the caller upon
	/// completion w/ the storeed instance, all this while preserving thread
	/// safety
	template <class InstanceTypeAcceptingCallableType>
	static void storeInstance(const StoredType &aStoredType, InstanceTypeAcceptingCallableType &&aCallable)
	{
		auto lockedInstanceStorage = instanceStorage.makeLock();
		lockedInstanceStorage->push_back(aStoredType);
		aCallable(lockedInstanceStorage->back());
		OHDEBUG("Ut::Sn::StaticInstanceStorage", "added instance, size() = ", lockedInstanceStorage->size());
	}

	/// \brief Add this instance to the static storage. It is not performed
	/// automatically, so an instance must destore itself
	///
	/// \tparam `IgnoreUnlessInvokedMetaType` makes the compiler to omit this
	/// method unless it is explicitly invoked, so the storage may store types
	/// that are not constructible from `this`.
	template <class IgnoreUnlessInvokedMetaType = void>
	void storeAsInstance()
	{
		storeInstance({static_cast<StoredType>(this)});
	}

	/// \brief Remove this instance from the static storage. It is not
	/// performed automatically, so an instance must destore itself
	template <class IgnoreUnlessInvokedMetaType = void>
	void removeAsInstance()
	{
		removeInstanceIf(
			[this](StoredType &aStoredType)
			{
				return aStoredType == this;
			});
	}

protected:
	static InstanceStorage instanceStorage;
};

template <class T1, class T2, std::size_t I>
typename StaticInstanceStorage<T1, T2, I>::InstanceStorage StaticInstanceStorage<T1, T2, I>::instanceStorage{};

}  // namespace Sn
}  // namespace Ut

#endif // UTILITY_UTILITY_SNIPPET_STATICINSTANCESTORAGE_HPP_

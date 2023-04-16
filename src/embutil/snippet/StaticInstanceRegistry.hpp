//
// StaticInstanceRegistry.hpp
//
// Created on: Mar 24, 2023
//     Author: Dmitry Murashov (d.murashov@geoscan.aero)
//

#ifndef UTILITY_UTILITY_SNIPPET_STATICINSTANCEREGISTRY_HPP
#define UTILITY_UTILITY_SNIPPET_STATICINSTANCEREGISTRY_HPP

#include "utility/snippet/StaticInstanceStorage.hpp"
#include <cstdint>

namespace Ut {
namespace Sn {

template <class StoredType>
struct ControlBlock {
	static std::size_t identifierBoundCounter;
	StoredType stored;
	std::size_t identifier;

	ControlBlock(const StoredType &aStored) :
		stored{aStored},
		identifier{identifierBoundCounter++}
	{
	}
};

template <class PayloadType>
std::size_t ControlBlock<PayloadType>::identifierBoundCounter{0};

/// Stores an instance under a unique identifier
template <class StoredType, class MutexType, std::size_t kDefaultSize = 4>
class StaticInstanceRegistry : public StaticInstanceStorage<ControlBlock<StoredType>, MutexType, kDefaultSize> {
private:
	using BaseType = StaticInstanceStorage<ControlBlock<StoredType>, MutexType, kDefaultSize>;
public:
	using BaseType::storeInstance;
	using BaseType::removeInstanceIf;
	using BaseType::iterInstancesWhile;

	static std::size_t registerInstance(const StoredType &aStored)
	{
		std::size_t res = 0;
		storeInstance(aStored,
			[&res](ControlBlock<StoredType> &aStored)
			{
				res = aStored.identifier;
			});
		OHDEBUG("Ut::Sn::StaticInstanceRegistry", "registered instance, identifier =", res);

		return res;
	}

	static void unregisterInstance(std::size_t aIdentifier)
	{
		removeInstanceIf(
			[aIdentifier](ControlBlock<StoredType> &aStored)
			{
				return aStored.identifier == aIdentifier;
			});
	}

	template <class InstanceAcceptingCallableType>
	static void unregisterInstanceIf(std::size_t aIdentifier, InstanceAcceptingCallableType &&aCallable)
	{
		removeInstanceIf(
			[&aIdentifier, aCallable](ControlBlock<StoredType> &aStored)
			{
				if (aIdentifier == aStored.identifier) {
					return aCallable(aStored.stored);
				} else {
					return false;
				}
			});
	}

	template <class InstanceAcceptingCallableType>
	static void withRegisteredInstance(std::size_t aIdentifier, InstanceAcceptingCallableType &&aCallable)
	{
		iterInstancesWhile(
			[aIdentifier, &aCallable](ControlBlock<StoredType> &aStored)
			{
				if (aStored.identifier == aIdentifier) {
					aCallable(aStored.stored);

					return false;
				} else {
					return true;
				}
			});
	}
};

}  // namespace Sn
}  // namespace Ut

#endif // UTILITY_UTILITY_SNIPPET_STATICINSTANCEREGISTRY_HPP

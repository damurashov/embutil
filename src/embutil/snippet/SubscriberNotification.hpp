//
// SubscriberNotification.hpp
//
// Created on: Mar 07, 2023
//     Author: Dmitry Murashov (d.murashov@geoscan.aero)
//

#ifndef UTILITY_UTILITY_SNIPPET_HPP_
#define UTILITY_UTILITY_SNIPPET_HPP_

#include "utility/algorithm/Vector.hpp"
#include "utility/snippet/LockWrapper.hpp"
#include <vector>

namespace Ut {
namespace Sn {

/// Reduces boilerplate while implementing subscriber notification mechanism
/// Unlike `SubscriberNotification`, `LightweightSubscriberNotification`
/// only stores reference to a storage
template <class SubscriberType, class MutexType, class ...SubscriberArgumentTypes>
class LightweightSubscriberNotification {
public:
	using LockedSubscriberStorageType = Ut::Sn::LockWrapper<std::vector<SubscriberType *>, MutexType>;
	using SubscriberMethod = void(SubscriberType::*)(SubscriberArgumentTypes...);

public:
	LightweightSubscriberNotification(SubscriberMethod aSubscriberMethod,
		LockedSubscriberStorageType &aLockedSubscriberStorage) :
		subscriberMethod{aSubscriberMethod},
		lockedSubscriberStorage{aLockedSubscriberStorage}
	{
	}

	virtual ~LightweightSubscriberNotification() = default;

	void addSubscriber(SubscriberType &aSubscriber)
	{
		lockedSubscriberStorage.makeLock()->push_back(&aSubscriber);
	}

	void removeSubscriber(SubscriberType &aSubscriber)
	{
		auto lock = lockedSubscriberStorage.makeLock();
		Al::vectorSwapEraseIf(*lock,
			[&aSubscriber](SubscriberType *aStored)
			{
				return aStored == &aSubscriber;
			});
	}

	template <class ...Ts>
	void notifySubscribers(Ts &&...aArgs)
	{
		if (subscriberMethod != nullptr) {
			auto lockedSubscribers = lockedSubscriberStorage.makeLock();

			for (auto subscriber : *lockedSubscribers) {
				(subscriber->*subscriberMethod)(std::forward<Ts>(aArgs)...);
			}
		}
	}

private:
	SubscriberMethod subscriberMethod;  // Must be defined by the implementor
	LockedSubscriberStorageType &lockedSubscriberStorage;
};

/// Reduces boilerplate while implementing subscriber notification mechanism
template <class SubscriberType, class MutexType, class ...SubscriberArgumentTypes>
class SubscriberNotification : public LightweightSubscriberNotification<SubscriberType, MutexType, SubscriberArgumentTypes...> {
private:
	using BaseType = LightweightSubscriberNotification<SubscriberType, MutexType, SubscriberArgumentTypes...>;

public:
	SubscriberNotification(BaseType::SubscriberMethod aSubscriberMethod, std::size_t aNsubscribers = 4U) :
		BaseType{aSubscriberMethod, subscribers},
		subscribers{aNsubscribers}
	{
		subscribers.instanceUnsafe().clear();
	}

private:
	BaseType::LockedSubscriberStorageType subscribers;
};

}  // namespace Sn
}  // namespace Ut

#endif // UTILITY_UTILITY_SNIPPET_HPP_

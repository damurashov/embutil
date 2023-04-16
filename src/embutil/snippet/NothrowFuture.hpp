//
// NothrowFuture.hpp
//
// Created on: Mar 23, 2023
//     Author: Dmitry Murashov (d.murashov@geoscan.aero)
//

// Utilities here are meant to simplify sync-async interation

#ifndef UTILITY_UTILITY_SNIPPET_NOTHROWFUTURE_HPP
#define UTILITY_UTILITY_SNIPPET_NOTHROWFUTURE_HPP

#include "utility/snippet/SemaphoreTypeInvokeSelector.hpp"
#include "utility/snippet/StaticInstanceRegistry.hpp"
#include "utility/snippet/StubSemaphore.hpp"
#include <cassert>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

namespace Ut {
namespace Sn {

constexpr std::size_t kDefaultStorageSize = 32U;
constexpr std::size_t kUninitializedPromiseIdentifier = std::numeric_limits<std::size_t>::max();

enum class FutureState {
	Pending,
	Fulfilled,
	Failed,
};

template <class T1, class T2, class T3, std::size_t>
class Future;

template <class PayloadType, class MutexType, class SemaphoreType = StubSemaphore,
	std::size_t kInitialStorageSize = kDefaultStorageSize>
using FutureInstanceRegistryType = StaticInstanceRegistry<Future<PayloadType, MutexType, SemaphoreType,
	kInitialStorageSize> *, MutexType, kInitialStorageSize>;

/// \brief Hides the complexities of asynchronous communication behind
/// sync-like API. Inspired by STL's "std::future", although this one
/// does not throw exceptions
///
/// Promise object can be thought of as a "key" used to access the
/// storage of `Future` objects expecting for updates.
///
/// \example
///
/// ```c++
/// #include <iostream>
/// #include <mutex>
/// #include <thread>
/// #include "utility/snippet/NothrowFuture.hpp"
///
/// using PromiseType = Ut::Sn::Promise<int, std::mutex>;
/// using FutureType = Ut::Sn::Future<int, std::mutex>;
///
/// void producerTask(PromiseType &&aPromise)
/// {
///     std::cout << "producer thread" << std::endl;
///     std::this_thread::sleep_for(std::chrono::seconds(2));
///     aPromise.fulfill(42);
///     std::this_thread::sleep_for(std::chrono::seconds(1));
///     std::cout << "end of producer thread" << std::endl;
/// }
///
/// void consumerTask(FutureType &&aFuture)
/// {
///     std::cout << "consumer task" << std::endl;
///     int val = 0;
///     auto state = aFuture.retrieve(val);
///
///     while (state == FutureType::State::Pending) {
///         state = aFuture.retrieve(val);
///     }
///
///     std::cout << val << std::endl;
///     std::cout << (int)state << std::endl;
///     std::cout << "end of consumer task" << std::endl;
/// }
///
/// int main(void)
/// {
///     PromiseType promise{};
///     // auto alreadyExistingFuture = promise.makeFuture();
///     std::thread producerThread{producerTask, std::move(promise)};
///     consumerTask(promise.makeFuture());
///     producerThread.join();
///
///     return 0;
/// }
/// ```
template <class PayloadType, class MutexType, class SemaphoreType = StubSemaphore,
	std::size_t kInitialStorageSize = kDefaultStorageSize>
class Promise {
private:
	using FutureType = Future<PayloadType, MutexType, SemaphoreType, kInitialStorageSize>;

public:
	using FutureInstanceRegistryConcreteType = FutureInstanceRegistryType<PayloadType, MutexType, SemaphoreType,
		kInitialStorageSize>;

public:
	Promise(std::size_t aPromiseIdentifier) :
		promiseIdentifier{aPromiseIdentifier}
	{
	}

	Promise() :
		promiseIdentifier{kUninitializedPromiseIdentifier}
	{
	}

	bool canMakeFuture() const
	{
		bool ret = true;
		FutureInstanceRegistryConcreteType::withRegisteredInstance(promiseIdentifier,
			[&ret](FutureType *&aFuture)
			{
				ret = (aFuture == nullptr);
			});

		return ret;
	}

	template <class ...Ts>
	FutureType makeFuture(Ts &&...aArguments) const
	{
		assert(canMakeFuture());
		return FutureType{promiseIdentifier, std::forward<Ts>(aArguments)...};
	}

	~Promise()
	{
	}

	// TODO: rewrite as `tryFulfill`
	void fulfill(const PayloadType &aPayload) const
	{
		setPromise(FutureType::State::Fulfilled, aPayload);
	}

	// TODO: rewrite as `tryFail`
	void fail() const
	{
		setPromise(FutureType::State::Failed);
	}

	/// Makes an attempt to immutably access payload stored in `Future` object.
	/// Returns false, if the `Future` object has been destroyed.
	///
	/// \warning `aCallable` should not perform any waits or delays, because
	/// the instance registry may be protected with a mutex (unless the stub
	/// mutex type is used).
	template <class PayloadTypeAcceptingCallableType>
	bool tryWithFuturePayload(PayloadTypeAcceptingCallableType &&aCallable) const
	{
		bool res = false;
		FutureInstanceRegistryConcreteType::withRegisteredInstance(promiseIdentifier,
			[&res, &aCallable](FutureType *&aFuture)
			{
				res = true;
				aCallable(static_cast<const PayloadType &>(aFuture->payload));
			});

		return res;
	}

	/// Future object might store a valuable info
	bool tryCloneFuturePayload(PayloadType &aPayload) const
	{
		return tryWithFuturePayload(
			[&aPayload](const PayloadType &aOtherPayload)
			{
				aPayload = aOtherPayload;
			});
	}

	std::size_t identifier() const
	{
		return promiseIdentifier;
	}

	bool tryAcquireFutureState(::Ut::Sn::FutureState &aState) const
	{
		bool res = false;
		FutureInstanceRegistryConcreteType::withRegisteredInstance(promiseIdentifier,
			[&res, &aState](FutureType *&aFuture)
			{
				if (aFuture != nullptr) {
					res = true;
					aState = aFuture->state();
				}
			});

		return res;
	}

	Promise(const Promise &) = default;
	Promise &operator=(const Promise &) = default;
	Promise(Promise &&aOther) = default;
	Promise &operator=(Promise &&aOther) = default;

private:
	// TODO: rewrite as `trySetPromise`
	void setPromise(typename FutureType::State aState, const PayloadType &aPayload = PayloadType{}) const
	{
		FutureInstanceRegistryConcreteType::withRegisteredInstance(promiseIdentifier,
			[this, aState, &aPayload](FutureType *&aFuture)
			{
				// no need to check nullptr, as Future removes itself on destruction
				if (aFuture != nullptr && aFuture->state() == FutureType::State::Pending) {
					aFuture->mState = aState;
					aFuture->payload = aPayload;
					SemaphoreTypeInvokeSelector::release(aFuture->mSemaphore);
				}
			});
	}

private:
	std::size_t promiseIdentifier;
};

inline const char *futureStateAsString(FutureState aFutureState)
{
	return aFutureState == FutureState::Pending ? "Pending" :
		aFutureState == FutureState::Fulfilled ? "Fulfilled" :
		aFutureState == FutureState::Failed ? "Failed" :
		"TBD";
}

/// \tparam SemaphoreType - binary semaphore, acquired by default
template <class PayloadType, class MutexType, class SemaphoreType = StubSemaphore,
	std::size_t kInitialStorageSize = kDefaultStorageSize>
class Future {
private:
	template <class T1, class T2, class T3, std::size_t>
	friend class Promise;

	using PromiseType = Promise<PayloadType, MutexType, SemaphoreType, kInitialStorageSize>;

public:
	using FutureInstanceRegistryConcreteType = FutureInstanceRegistryType<PayloadType, MutexType, SemaphoreType,
		kInitialStorageSize>;
	using State = FutureState;

public:
	~Future()
	{
		FutureInstanceRegistryConcreteType::unregisterInstance(promiseIdentifier);
	}

	template <class ...Ts>
	Future(Ts &&...aPayloadConstructionArguments) :
		promiseIdentifier{FutureInstanceRegistryConcreteType::registerInstance(this)},
		mState{State::Pending},
		mSemaphore{},
		payload{std::forward<Ts>(aPayloadConstructionArguments)...}
	{
	}

	State retrieve(PayloadType &aPayload) const
	{
		aPayload = payload;
		return mState;
	}

	PromiseType makePromise() const
	{
		return PromiseType{identifier()};
	}

	State wait(PayloadType &aPayload)
	{
		if (State::Pending == mState) {
			SemaphoreTypeInvokeSelector::acquire(mSemaphore);
		}

		return retrieve(aPayload);
	}

	State wait()
	{
		if (State::Pending == mState) {
			SemaphoreTypeInvokeSelector::acquire(mSemaphore);
		}

		return mState;
	}

	template <class TimeType>
	State tryWaitFor(const TimeType &aTime)
	{
		if (State::Pending == mState) {
			SemaphoreTypeInvokeSelector::tryAcquireFor(mSemaphore, aTime);
		}

		return mState;
	}

	const PayloadType &asPayload() const
	{
		return payload;
	}

	template <class TimeType>
	State tryWaitFor(PayloadType &aPayload, const TimeType &aTime)
	{
		if (State::Pending == mState) {
			SemaphoreTypeInvokeSelector::tryAcquireFor(mSemaphore, aTime);
		}

		return retrieve(aPayload);
	}

	bool ready() const
	{
		return mState != State::Pending;
	}

	State state() const
	{
		return mState;
	}

	Future(const Future &) = delete;
	Future &operator=(const Future &) = delete;

	Future(Future &&aOther)
	{
		registerAsInstance(&aOther);
	}

	Future &operator=(Future &&aOther) = delete;

	std::size_t identifier() const
	{
		return promiseIdentifier;
	}

private:

	void registerAsInstance(Future *aMoveFrom = nullptr)
	{
		if (promiseIdentifier != kUninitializedPromiseIdentifier) {
			FutureInstanceRegistryConcreteType::withRegisteredInstance(promiseIdentifier,
				[this, aMoveFrom](Future *&aInstance)
				{
					if (aMoveFrom != nullptr) {
						promiseIdentifier = aMoveFrom->promiseIdentifier;
						mState = aMoveFrom->mState;
						mSemaphore = std::move(aMoveFrom->mSemaphore);
						payload = std::move(aMoveFrom->payload);
						aMoveFrom->promiseIdentifier = kUninitializedPromiseIdentifier;
					}

					aInstance = this;
				});
		}
	}

private:
	std::size_t promiseIdentifier;
	State mState;
	SemaphoreType mSemaphore;
	PayloadType payload;
};

}  // namespace Sn
}  // namespace Ut

#endif // UTILITY_UTILITY_SNIPPET_NOTHROWFUTURE_HPP


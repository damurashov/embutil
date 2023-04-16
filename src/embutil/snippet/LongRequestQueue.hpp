//
// LongRequestQueue.hpp
//
// Created on: Mar 14, 2023
//     Author: Dmitry Murashov (d.murashov@geoscan.aero)
//

#ifndef UTILITY_UTILITY_SNIPPET_LONGREQUESTQUEUE_HPP
#define UTILITY_UTILITY_SNIPPET_LONGREQUESTQUEUE_HPP

#include "utility/OhDebug.hpp"
#include "utility/algorithm/Time.hpp"
#include "utility/algorithm/Vector.hpp"
#include "utility/snippet/LockWrapper.hpp"
#include "utility/snippet/StubMutex.hpp"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <type_traits>
#include <vector>

namespace Ut {
namespace Sn {

template <class RequestType>
struct RequestHandler {
	virtual void retryRequest(const RequestType &aRequest)
	{
		(void)aRequest;
	}

	/// Might be used for additional notification and garbage-collection.
	virtual void onRequestExpired(const RequestType &aRequest)
	{
		(void)aRequest;
	}
};

/// Decorates a `RequestType` with time information
template <class RequestType, class TimeType>
struct LongRequest {
	enum class UpdateResult {
		Invoke,  // Timeout milestone has been passed, but not all attempts are wasted
		NoInvoke,  // Timeout milestone is yet to be reached
		Expired,  // Timeout and all the attempts along with it have been expired
	};

	RequestType request;

	/// The time when the request has been issued
	TimeType startTime;

	/// Hold not less than `timeout` before the next attempt
	TimeType timeout;

	std::size_t nReattemptsLeft;

	operator RequestType &()
	{
		return request;
	}

	/// Updates number of attempts, returns time before the next timeout.
	///
	/// Employs "not-less" timeout policy, meaning that an invoker should wait
	/// AT LEAST `timeout` before issuing the next request, and use ALL
	/// attempts.
	UpdateResult tryUpdateUseAllAttempts(const TimeType &aNow, TimeType &aNextTimeout)
	{
		if (nReattemptsLeft == 0) {
			aNextTimeout = TimeType{0};

			return UpdateResult::Expired;
		} else if (aNow > startTime + timeout) {
			--nReattemptsLeft;
			startTime = aNow;
			aNextTimeout = timeout;

			return UpdateResult::Invoke;
		} else {
			aNextTimeout = startTime + timeout - aNow;

			return UpdateResult::NoInvoke;
		}
	}
};

/// An intermediate storage of encapsulated request entities that have a
/// delayed answer, or might need to be reissued due to communication drops.
///
/// Use case: a wired device that is being connected with some serial protocol.
/// It usually requires connection instantiator to repeat requests and commands
/// from time to time.
///
/// \tparam RequestType must be a lightweight, as it will be copied multiple times
template <class RequestType, class MutexType = Ut::Sn::StubMutex, class T = std::chrono::milliseconds,
	std::size_t kInitialSize = 4>
class LongRequestQueue {
	static_assert(std::is_copy_constructible<RequestType>::value, "The entity must be copy-constructible. "
		"This assertion may also be triggered, if `RequestType` is an incomplete type");
	static_assert(std::is_default_constructible<RequestType>::value, "The entity must be default-constructible. "
		"This assertion may also be triggered, if `RequestType` is an incomplete type");

public:
	using RequestHandlerType = RequestHandler<RequestType>;
	using TimeType = T;
	using LongRequestType = LongRequest<RequestType, TimeType>;

private:
	using LongRequestStorageType = std::vector<LongRequestType>;

public:
	LongRequestQueue() :
		requestQueue{}
	{
		auto lockedRequestQueue = requestQueue.makeLock();
		lockedRequestQueue->reserve(kInitialSize);
		lockedRequestQueue->clear();
	}

	std::size_t size() const
	{
		return requestQueue.instanceUnsafe().size();
	}

	void push(const RequestType &aRequestType, TimeType aTimeout, std::size_t aNattempts, TimeType aNow)
	{
		auto lockedRequestQueue = requestQueue.makeLock();
		lockedRequestQueue->push_back(LongRequestType{aRequestType, aNow, aTimeout, aNattempts});
		OHDEBUG("Ut::Sn::LongRequestQueue", "added long request, size() =", lockedRequestQueue->size());

		if (requestHandler != nullptr) {
			requestHandler->retryRequest(lockedRequestQueue->back().request);
		}
	}

	/// Iterates over the request queue and removes items that satisfy the
	/// condition.
	///
	/// `aCallable` condition checker, must comply `bool(RequestType &)`
	/// signature
	template <class CallableType>
	void removeIf(CallableType &&aCallable)
	{
		auto lockedRequestQueue = requestQueue.makeLock();
		Ut::Al::vectorSwapEraseIf(*lockedRequestQueue, std::move(aCallable));
		OHDEBUG("Ut::Sn::LongRequestQueue", "made an attempt to remove long request, size() =", lockedRequestQueue->size());
	}

	void setRequestHandler(RequestHandlerType &aRequestHandler)
	{
		auto lockedRequestQueue = requestQueue.makeLock();
		(void)lockedRequestQueue;
		requestHandler = &aRequestHandler;
	}

	/// Returns time before next timeout, or 0, if there are no pending requests
	TimeType onTick(TimeType aNow)
	{
		auto lockedRequestQueue = requestQueue.makeLock();
		TimeType minNextTimeout{0};

		// Cull expired, calculate `minNextTimeout`
		if (lockedRequestQueue->size() > 0 && requestHandler != nullptr) {
			for (std::size_t i = 0; i < lockedRequestQueue->size();) {
				TimeType nextTimeout{0};
				const auto updateResult = lockedRequestQueue->at(i).tryUpdateUseAllAttempts(aNow, nextTimeout);

				if (updateResult == LongRequestType::UpdateResult::Invoke) {
					requestHandler->retryRequest(lockedRequestQueue->at(i).request);
					++i;
				} else if (updateResult == LongRequestType::UpdateResult::Expired) {
					requestHandler->onRequestExpired(lockedRequestQueue->at(i).request);
					Ut::Al::vectorSwapEraseAt(*lockedRequestQueue, i);
					OHDEBUG("Ut::Sn::LongRequestQueue", "removed expired long request, size() =",
						lockedRequestQueue->size());
				} else {
					++i;
				}

				if (minNextTimeout == TimeType{0}) {
					minNextTimeout = nextTimeout;
				} else if (nextTimeout > TimeType{0} && nextTimeout < minNextTimeout) {
					minNextTimeout = nextTimeout;
				}
			}
		}

		return minNextTimeout;
	}

private:
	Ut::Sn::LockWrapper<LongRequestStorageType, MutexType> requestQueue;
	RequestHandlerType *requestHandler;
};

}  // namespace Sn
}  // namespace Ut

#endif // UTILITY_UTILITY_SNIPPET_LONGREQUESTQUEUE_HPP

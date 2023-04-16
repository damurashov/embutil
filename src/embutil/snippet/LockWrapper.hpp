//
// LockWrapper.hpp
//
// Created on: Mar 06, 2023
//     Author: Dmitry Murashov (d.murashov@geoscan.aero)
//

#ifndef SNIPPET_SNIPPET_LOCKWRAPPER_HPP_
#define SNIPPET_SNIPPET_LOCKWRAPPER_HPP_

#include <utility>

namespace Ut {
namespace Sn {

/// Lock guard-like wrapper
template <class T, class L>
class Lock {
public:
	Lock(T &aInstance, L &aLock) :
		instance{&aInstance},
		lock{&aLock}
	{
		lock->lock();
	}

	~Lock()
	{
		if (lock) {
			lock->unlock();
		}
	}

	void forceReleaseLock()
	{
		if (lock) {
			lock->unlock();
		}

		lock = nullptr;
	}

	T *operator->()
	{
		return instance;
	}

	T &operator*()
	{
		return *instance;
	}

private:
	T *instance;
	L *lock;
};

/// Mutex-based instance wrapper. Reduces boilerplate for synchronized
/// instances.
///
/// \tparam L Must have `lock` and `unlock` methods defined
///
/// \example
/// ```c++
/// LockWrapper<std::vector<int>, std::mutex> eWrapped;
///
/// void accessEwrapped()
/// {
/// 	eWrapped.makeLock()->push_back(42);
/// }
/// ```
template <class T, class L>
class LockWrapper {
public:
	template <class ...Ts>
	LockWrapper(Ts &&...aArgs) :
		instance{std::forward<Ts>(aArgs)...},
		lock{}
	{
	}

	Lock<T, L> makeLock()
	{
		return Lock<T, L>{instance, lock};
	}

	T &instanceUnsafe()
	{
		return instance;
	}

	const T &instanceUnsafe() const
	{
		return instance;
	}

private:
	T instance;
	L lock;
};

}  // namespace Sn
}  // namespace Ut

#endif // SNIPPET_SNIPPET_LOCKWRAPPER_HPP_

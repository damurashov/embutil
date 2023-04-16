//
// Singleton.hpp
//
// Created on: Feb 09, 2023
//     Author: Dmitry Murashov (d.murashov@geoscan.aero)
//

#ifndef UTILITY_UTILITY_SNIPPET_SINGLETON_HPP_
#define UTILITY_UTILITY_SNIPPET_SINGLETON_HPP_

#include <type_traits>

namespace Ut {
namespace Sn {

/// \brief Boilerplate wrapper for creating singletons.
/// \tparam kAutoregisterInstance - if true, the instance will be set right away.
template <class T, bool kAutoregisterInstance = true>
class Singleton {
public:
	Singleton()
	{
		if (kAutoregisterInstance) {
			setInstance();
		}
	}

	~Singleton()
	{
		resetInstance();
	}

	static T *instance()
	{
		return static_cast<T *>(inst);
	}

	Singleton(const Singleton &) = delete;
	Singleton(Singleton &&) = delete;
	Singleton &operator=(const Singleton &) = delete;
	Singleton &operator=(Singleton &&) = delete;

protected:
	void setInstance()
	{
		inst = this;
	}

	void resetInstance()
	{
		inst = nullptr;
	}

private:
	static Singleton<T, kAutoregisterInstance> *inst;
};

template <class T, bool F>
Singleton<T, F> *Singleton<T, F>::inst = nullptr;

}  // namespace Sn
}  // namespace Ut

#endif // UTILITY_UTILITY_SNIPPET_SINGLETON_HPP_

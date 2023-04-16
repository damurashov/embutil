//
// String.cpp
//
// Created on: May 19, 2021
//     Author: Dmitry Murashov (d.murashov@geoscan.aero)
//

#include "String.hpp"
#include <algorithm>
#include <cstring>

namespace Ut {
namespace Al {

bool stringIsEnswith(const char *aStr, const char *aSuffix)
{
	const auto aStrLen = strlen(aStr);
	const auto aSuffixLen = strlen(aSuffix);
	bool ret = false;

	if (aStrLen > 0 && aSuffixLen > 0 && aStrLen >= aSuffixLen) {
		auto *pos = std::find_end(aStr, aStr + aStrLen, aSuffix, aSuffix + aSuffixLen);

		if (aStr + aStrLen - aSuffixLen == pos) {
			ret = true;
		}
	}

	return ret;
}

}  // namespace Al
}  // namespace Ut

/*
 * 2023 by zenmumbler
 * This file is part of Rehatched
 */

#ifndef HPL_STRING_TYPES_H
#define HPL_STRING_TYPES_H

#include <string>

namespace hpl {

	#define _W(t) L ## t

	using tString = std::string;
	using tWString = std::wstring;

}

#endif

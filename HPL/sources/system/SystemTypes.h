/*
 * Copyright (C) 2006-2010 - Frictional Games
 *
 * This file is part of HPL1 Engine.
 *
 * HPL1 Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HPL1 Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HPL1 Engine.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef HPL_SYSTEM_TYPES_H
#define HPL_SYSTEM_TYPES_H

#include <cstdio>
#include <string>
#include <set>
#include <vector>

namespace hpl {

	//////////////////////////////////////////////////
	///////// TYPES //////////////////////////////////
	//////////////////////////////////////////////////

	#define _W(t) L ## t

	//--------------------------------------------------------

	typedef unsigned int tFlag;

	typedef std::string tString;
	typedef std::vector<tString> tStringVec;
	typedef std::set<tString> tStringSet;

	//--------------------------------------------------------

	typedef std::wstring tWString;
	typedef std::vector<tWString> tWStringVec;

	//--------------------------------------------------------

	typedef std::vector<unsigned int> tUIntVec;
	typedef std::vector<int> tIntVec;
	typedef std::vector<float> tFloatVec;

	//----------------------------------------------------------

}
#endif // HPL_SYSTEM_TYPES_H

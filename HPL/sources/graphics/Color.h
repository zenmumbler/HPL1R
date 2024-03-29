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
#ifndef HPL_COLOR_H
#define HPL_COLOR_H

#include "system/StringTypes.h"

namespace hpl {

	class cColor
	{
	public:
		float r,g,b,a;

		cColor(float afR, float afG, float afB, float afA);
		cColor(float afR, float afG, float afB);
		cColor();
		explicit cColor(float afVal);
		cColor(float afVal, float afA);

		cColor operator*(float afVal) const;
		cColor operator/(float afVal) const;

		cColor operator+(const cColor &aCol) const;
		cColor operator-(const cColor &aCol) const;
		cColor operator*(const cColor &aCol) const;
		cColor operator/(const cColor &aCol) const;

		bool operator==(cColor aCol) const;

		tString ToString() const;
		tString ToFileString() const;

		void FromVec(float *apV);

		static const cColor White;
		static const cColor Black;
	};

};
#endif // HPL_COLOR_H

/*
 * Copyright (C) 2006-2010 - Frictional Games
 *
 * This file is part of Penumbra Overture.
 *
 * Penumbra Overture is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Penumbra Overture is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Penumbra Overture.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Init.h"

int hplMain(const tString& asCommandLine)
{
	auto pInit = new cInit();
	bool bRet = pInit->Init(asCommandLine);
	
	if (bRet == false) {
		delete  pInit->mpGame ;
		CreateMessageBoxW(_W("Error!"), pInit->msErrorMessage.c_str());
		return 1;
	}

	pInit->Run();
	pInit->Exit();
	delete pInit;

	return 0;
}

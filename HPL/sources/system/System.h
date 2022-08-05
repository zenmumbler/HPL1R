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
#ifndef HPL_SYSTEM_H
#define HPL_SYSTEM_H

#include "system/SystemTypes.h"

namespace hpl {

//	extern void CreateMessageBox ( const char* asCaption, const char *fmt, ...);
//	extern void CreateMessageBox ( eMsgBoxType eType, const char* asCaption, const char *fmt, ...);

	typedef enum
	{
		eMsgBoxType_Info,
		eMsgBoxType_Error,
		eMsgBoxType_Warning,
		eMsgBoxType_Default
	} eMsgBoxType;

	void CreateMessageBoxW(const wchar_t* asCaption, const wchar_t* fmt, ...);
	void CreateMessageBoxW(eMsgBoxType eType, const wchar_t* asCaption, const wchar_t* fmt, ...);

	void OpenBrowserWindow(const tWString& asURL);

	void CopyTextToClipboard(const tWString &asText);
	tWString LoadTextFromClipboard();

	void SetWindowCaption(const tString &asName);
	bool HasWindowFocus(const tWString &asWindowCaption);

	void InitAppTime();
	uint64_t GetAppTimeMS();
	float GetAppTimeFloat();
	cDate GetDate();

};

#endif // HPL_SYSTEM_H

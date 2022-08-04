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

#include "system/MemoryManager.h"
#include "system/SystemTypes.h"


namespace hpl {

#ifdef UPDATE_TIMING_ENABLED
	#define START_TIMING_EX(x,y)	LogUpdate("Updating %s in file %s at line %d\n",x,__FILE__,__LINE__); \
									unsigned int y##_lTime = GetApplicationTime();
	#define START_TIMING(x)	LogUpdate("Updating %s in file %s at line %d\n",#x,__FILE__,__LINE__); \
								unsigned int x##_lTime = GetApplicationTime();
	#define STOP_TIMING(x)	LogUpdate(" Time spent: %d ms\n",GetApplicationTime() - x##_lTime);
	#define START_TIMING_TAB(x)	LogUpdate("\tUpdating %s in file %s at line %d\n",#x,__FILE__,__LINE__); \
							unsigned int x##_lTime = GetApplicationTime();
	#define STOP_TIMING_TAB(x)	LogUpdate("\t Time spent: %d ms\n",GetApplicationTime() - x##_lTime);
#else
	#define START_TIMING_EX(x,y)
	#define START_TIMING(x)
	#define STOP_TIMING(x)
	#define START_TIMING_TAB(x)
	#define STOP_TIMING_TAB(x)
#endif

	void SetLogFile(const tWString &asFile);
	void FatalError(const char* fmt,... );
	void Error(const char* fmt, ...);
	void Warning(const char* fmt, ...);
	void Log(const char* fmt, ...);

	void SetUpdateLogFile(const tWString &asFile);
	void ClearUpdateLogFile();
	void SetUpdateLogActive(bool abX);
	void LogUpdate(const char* fmt, ...);

//	extern void CreateMessageBox ( const char* asCaption, const char *fmt, ...);
//	extern void CreateMessageBox ( eMsgBoxType eType, const char* asCaption, const char *fmt, ...);

	void CreateMessageBoxW( const wchar_t* asCaption, const wchar_t* fmt, ...);
	void CreateMessageBoxW( eMsgBoxType eType, const wchar_t* asCaption, const wchar_t* fmt, ...);

	void OpenBrowserWindow ( const tWString& asURL );

	void CopyTextToClipboard(const tWString &asText);
	tWString LoadTextFromClipboard();

	tWString GetSystemSpecialPath(eSystemPath aPathType);

	bool FileExists(const tWString& asFileName);
	void RemoveFile(const tWString& asFileName);
	bool CloneFile(const tWString& asSrcFileName,const tWString& asDestFileName,
					bool abFailIfExists);
	bool CreateFolder(const tWString& asPath);
	bool FolderExists(const tWString& asPath);
	bool IsFileLink(const tWString& asPath);
	bool LinkFile(const tWString& asPointsTo, const tWString& asLink);
	bool RenameFile(const tWString& asFrom, const tWString& asTo);
	cDate FileModifiedDate(const tWString& asFilePath);
	cDate FileCreationDate(const tWString& asFilePath);

	void SetWindowCaption(const tString &asName);

	bool HasWindowFocus(const tWString &asWindowCaption);

	unsigned long GetApplicationTime();
	unsigned long GetTime();
	cDate GetDate();

};
#endif // HPL_SYSTEM_H

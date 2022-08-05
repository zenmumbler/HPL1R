/*
 * 2022 by zenmumbler
 * This file is part of Rehatched
 */

#include "system/Files.h"

#ifdef WIN32
	#define _UNICODE
	#include <io.h>
#else
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
	#include <dirent.h>
	#include <wctype.h>
#endif

#include <fstream>

namespace hpl {

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// FileReader
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	FileReader::FileReader(const tString& filePath) {
		f = fopen(filePath.c_str(), "rb");
		if (f) {
			fseek(f, 0, SEEK_END);
			sizeBytes = static_cast<uint32_t>(ftell(f));
			fseek(f, 0, SEEK_SET);
		}
	}
	
	FileReader::~FileReader() {
		if (f) {
			fclose(f);
		}
	}
	
	bool FileReader::readBytes(void *into, size_t byteLength) {
		return fread(into, byteLength, 1, f) == 1;
	}
	
	void FileReader::skip(uint32_t bytesToSkip) {
		fseek(f, bytesToSkip, SEEK_CUR);
	}


	char* LoadEntireFile(const tString& asFileName, int& alLength)
	{
		FILE *pFile = fopen(asFileName.c_str(), "rb");
		if (pFile == NULL) {
			return nullptr;
		}

		fseek(pFile, 0, SEEK_END);
		alLength = static_cast<uint32_t>(ftell(pFile));
		fseek(pFile, 0, SEEK_SET);

		char *pBuffer = new char[alLength];
		fread(pBuffer, alLength, 1, pFile);

		fclose(pFile);
		return pBuffer;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// File / Folder Actions
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	tWString GetSystemSpecialPath(eSystemPath aPathType)
	{
	#if defined(WIN32)
		int type;
		switch(aPathType)
		{
		case eSystemPath_Personal:	type = CSIDL_PERSONAL;
			break;
		default: return _W("");
		}

		TCHAR sPath[1024];
		if(SUCCEEDED(SHGetFolderPath(NULL,
			type | CSIDL_FLAG_CREATE,
			NULL,0,sPath)))
		{
			return tWString(sPath);
		}
		else
		{
			return _W("");
		}
	#else
		switch (aPathType)
		{
		case eSystemPath_Personal: {
			const char *home = getenv("HOME");
			return cString::To16Char(tString(home));
		}
		default:
			return _W("");
		}
	#endif
	}

	//-----------------------------------------------------------------------

	bool FileExists(const tWString& asFileName)
	{
	#ifdef WIN32
		FILE *f = _wfopen(asFileName.c_str(),_W("r"));
	#else
		FILE *f = fopen(cString::To8Char(asFileName).c_str(),"r");
	#endif
		if(f==NULL)
		{
			return false;
		}

		fclose(f);
		return true;
	}

	//-----------------------------------------------------------------------

	void RemoveFile(const tWString& asFilePath)
	{
	#ifdef WIN32
		_wremove(asFilePath.c_str());
	#else
		remove(cString::To8Char(asFilePath).c_str());
	#endif
	}

	//-----------------------------------------------------------------------

	bool CloneFile(const tWString& asSrcFileName,const tWString& asDestFileName,
		bool abFailIfExists)
	{
	#ifdef WIN32
		return CopyFile(asSrcFileName.c_str(),asDestFileName.c_str(),abFailIfExists)==TRUE;
	#else
		if (abFailIfExists && FileExists(asDestFileName)) {
			return true;
		}
		std::ifstream IN (cString::To8Char(asSrcFileName).c_str(), std::ios::binary);
		std::ofstream OUT (cString::To8Char(asDestFileName).c_str(), std::ios::binary);
		OUT << IN.rdbuf();
		OUT.flush();
		return true;
	#endif
	}

	//-----------------------------------------------------------------------

	bool CreateFolder(const tWString& asPath)
	{
	#ifdef WIN32
		return CreateDirectory(asPath.c_str(),NULL)==TRUE;
	#else
		return mkdir(cString::To8Char(asPath).c_str(),0755)==0;
	#endif
	}

	bool FolderExists(const tWString& asPath)
	{
	#ifdef WIN32
		return GetFileAttributes(asPath.c_str())==FILE_ATTRIBUTE_DIRECTORY;
	#else
		struct stat statbuf;
		return (stat(cString::To8Char(asPath).c_str(), &statbuf) != -1);
	#endif
	}

	bool IsFileLink(const tWString& asPath)
	{
	// Symbolic Links Not Supported under Windows
	#ifndef WIN32
		struct stat statbuf;
		if (lstat(cString::To8Char(asPath).c_str(), &statbuf) == 0) {
			return statbuf.st_mode == S_IFLNK;
		} else {
			return false;
		}
	#else
			return false;
	#endif
	}

	bool LinkFile(const tWString& asPointsTo, const tWString& asLink)
	{
	// Symbolic Links Not Supported under Windows
	#ifndef WIN32
		tWString cmd = _W("ln -s \"") + asPointsTo + _W("\" \"") + asLink + _W("\"");
		return (system(cString::To8Char(cmd).c_str()) == 0);
	#else
		return false;
	#endif
	}

	bool RenameFile(const tWString& asFrom, const tWString& asTo)
	{
	#ifdef WIN32
		return false;
	#else
		return (rename(cString::To8Char(asFrom).c_str(), cString::To8Char(asTo).c_str()) == 0);
	#endif
	}

	//-----------------------------------------------------------------------

	cDate FileModifiedDate(const tWString& asFilePath)
	{
		struct tm* pClock;
	#ifdef WIN32
		struct _stat attrib;
		_wstat(asFilePath.c_str(), &attrib);
	#else
		struct stat attrib;
		stat(cString::To8Char(asFilePath).c_str(), &attrib);
	#endif

		pClock = gmtime(&(attrib.st_mtime));	// Get the last modified time and put it into the time structure

		return cDate::FromGMTIme(pClock);
	}

	//-----------------------------------------------------------------------

	cDate FileCreationDate(const tWString& asFilePath)
	{
		struct tm* pClock;
	#ifdef WIN32
		struct _stat attrib;
		_wstat(asFilePath.c_str(), &attrib);
	#else
		struct stat attrib;
		stat(cString::To8Char(asFilePath).c_str(), &attrib);
	#endif

		pClock = gmtime(&(attrib.st_ctime));	// Get the last modified time and put it into the time structure

		return cDate::FromGMTIme(pClock);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// FindFilesInDir
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------


#ifdef WIN32

	void FindFilesInDir(tWStringList &alstStrings,tWString asDir, tWString asMask)
	{
		//ifdef WIN32 , these windows functions only works with "\".. sucks ...
		asDir = cString::ReplaceCharToW(asDir,_W("/"),_W("\\"));

		//Get the search string
		wchar_t sSpec[256];
		wchar_t end = asDir[asDir.size()-1];

		if(end == _W('\\') || end== _W('/'))
			swprintf(sSpec,256,_W("%s%s"),asDir.c_str(),asMask.c_str());
		else
		{
			//ifdef WIN32 , these windows functions only works with "\".. sucks ...
			swprintf(sSpec,256,_W("%s\\%s"),asDir.c_str(),asMask.c_str());
		}

		//The needed structs
		intptr_t lHandle;
		struct _wfinddata_t FileInfo;

		//Find the first file:
		lHandle = _wfindfirst(sSpec, &FileInfo );
		if(lHandle==-1L)return;

		//Check so it is not a subdir
		if((FileInfo.attrib & _A_SUBDIR)==0)
		{
			alstStrings.push_back(FileInfo.name);
		}

		//Get the other files.
		while( _wfindnext( lHandle, &FileInfo ) == 0 )
		{
			if((FileInfo.attrib & _A_SUBDIR)==0)
			{
				alstStrings.push_back(FileInfo.name);
			}
		}
	}

#else

	static inline int patiMatch (const wchar_t *pattern, const wchar_t *string) {
	  switch (pattern[0])
	  {
	  case _W('\0'):
			return !string[0];

	  case _W('*') :
			return patiMatch(pattern+1, string) || (string[0] && patiMatch(pattern, string+1));

	  case _W('?') :
			return string[0] && patiMatch(pattern+1, string+1);

	  default  :
			return (towupper(pattern[0]) == towupper(string[0])) && patiMatch(pattern+1, string+1);
	  }
	}

	void FindFilesInDir(tWStringList &alstStrings, tWString asDir, tWString asMask)
	{
		//Log("Find Files in '%ls' with mask '%ls'\n",asDir.c_str(), asMask.c_str());
		//Get the search string
		wchar_t sSpec[256];
		wchar_t end = asDir[asDir.size()-1];
		//The needed structs
		DIR *dirhandle;
		dirent *_entry;
		struct stat statbuff;
		tWString fileentry;

		if ((dirhandle = opendir(cString::To8Char(asDir).c_str()))==NULL) return;

		while ((_entry = readdir(dirhandle)) != NULL) {
			if (end==_W('/'))
				swprintf(sSpec,256,_W("%ls%s"),asDir.c_str(),_entry->d_name);
			else
				swprintf(sSpec,256,_W("%ls/%s"),asDir.c_str(),_entry->d_name);

			// skip unreadable
			if (stat(cString::To8Char(sSpec).c_str(),&statbuff) ==-1) continue;
			// skip directories
			if (S_ISDIR(statbuff.st_mode)) continue;

			fileentry.assign(cString::To16Char(_entry->d_name));

			if (!patiMatch(asMask.c_str(),fileentry.c_str())) continue;
			alstStrings.push_back(fileentry);
		}
		closedir(dirhandle);
	}

#endif // WIN32

}

/*
 * 2021-3 by zenmumbler
 * This file is part of Rehatched
 */

#include "resources/FileSearcher.h"

#include "system/String.h"
#include "system/Files.h"
#include "system/Log.h"

#include <map>
#include <set>

namespace hpl::FileSearcher {

	//-----------------------------------------------------------------------

	static std::map<tString, tString> s_mapFiles;
	static std::set<tString> s_setLoadedDirs;
	static std::set<tString> s_setIgnoredFiles = { ".ds_store", "thumbs.db" };

	//-----------------------------------------------------------------------

	void AddDirectory(tString asPath, tString asMask)
	{
		tWStringVec fileNames;
		//Make the path with only "/" and lower case.
		asPath = cString::ToLowerCase(cString::ReplaceCharTo(asPath, "\\", "/"));

		auto it = s_setLoadedDirs.find(asPath);
		//If the path is not allready added, add it!
		if (it == s_setLoadedDirs.end())
		{
			s_setLoadedDirs.insert(asPath);

			FindFilesInDir(fileNames, cString::To16Char(asPath), cString::To16Char(asMask));

			for(const tWString& sWFile : fileNames)
			{
				tString sFile = cString::To8Char(sWFile);
				tString sFileAllLower = cString::ToLowerCase(sFile);
				tString sFilePath = cString::SetFilePath(sFile, asPath);
				
				if (s_setIgnoredFiles.find(sFileAllLower) != s_setIgnoredFiles.end()) {
					// skip ignored files
					continue;
				}

				const auto [itElement, inserted] = s_mapFiles.insert({ sFileAllLower, sFilePath });
				if (inserted == false) {
					Log("Overriding resource '%s' from '%s' to '%s'\n", sFile.c_str(), cString::GetFilePath(itElement->second).c_str(), asPath.c_str());
					itElement->second.assign(sFilePath);
				}
			}
		}
	}

	//-----------------------------------------------------------------------

	void ClearDirectories()
	{
		s_mapFiles.clear();
		s_setLoadedDirs.clear();
	}

	//-----------------------------------------------------------------------

	tString GetFilePath(tString asName)
	{
		auto it = s_mapFiles.find(cString::ToLowerCase(asName));
		if (it == s_mapFiles.end()) return "";

		return it->second;
	}

	//-----------------------------------------------------------------------

}

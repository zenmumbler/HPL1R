/*
 * 2021-3 by zenmumbler
 * This file is part of Rehatched
 */

#include "resources/FileSearcher.h"

#include "system/String.h"
#include "system/Files.h"
#include "system/Log.h"

#include <unordered_map>
#include <set>
#include <span>

namespace hpl::FileSearcher {

	//-----------------------------------------------------------------------

	static std::unordered_map<tString, tString> s_mapFiles;
	static std::set<tString> s_setLoadedDirs;
	static std::set<tString> s_setIgnoredFiles = { ".ds_store", "thumbs.db" };


	//-----------------------------------------------------------------------

	void AddDirectory(const tString &path, const tString &mask)
	{
		tWStringVec fileNames;
		//Make the path with only "/" and lower case.
		auto normalisedPath = cString::ToLowerCase(cString::ReplaceCharTo(path, "\\", "/"));

		auto it = s_setLoadedDirs.find(normalisedPath);
		//If the path is not allready added, add it!
		if (it == s_setLoadedDirs.end())
		{
			s_setLoadedDirs.insert(normalisedPath);

			FindFilesInDir(fileNames, cString::To16Char(normalisedPath), cString::To16Char(mask));

			for(const tWString& sWFile : fileNames)
			{
				tString sFile = cString::To8Char(sWFile);
				tString sFileAllLower = cString::ToLowerCase(sFile);
				tString sFilePath = cString::SetFilePath(sFile, normalisedPath);
				
				if (s_setIgnoredFiles.find(sFileAllLower) != s_setIgnoredFiles.end()) {
					// skip ignored files
					continue;
				}

				const auto [itElement, inserted] = s_mapFiles.insert({ sFileAllLower, sFilePath });
				if (inserted == false) {
					Log("Overriding resource '%s' from '%s' to '%s'\n", sFile.c_str(), cString::GetFilePath(itElement->second).c_str(), normalisedPath.c_str());
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

	tString GetFilePath(const tString &asName)
	{
		auto it = s_mapFiles.find(cString::ToLowerCase(asName));
		if (it == s_mapFiles.end()) return "";

		return it->second;
	}

	//-----------------------------------------------------------------------

	tString ResolveAssetName(const tString& name, std::span<const tString> extensions) {
		tString sPath;
		auto normalisedName = cString::ToLowerCase(name);
		auto baseExt = cString::GetFileExt(name);
		bool explicitExtension = baseExt.length() > 0;

		if (explicitExtension) {
			// Handle a specific — very HPL1 — case where an asset name is provided with the wrong extension.
			// This is only the case where world maps are loaded and the diffuse texture file name is used
			// to find the corresponding material in game. In that case, we just ignore the specified extension.
			if (std::find(extensions.begin(), extensions.end(), baseExt) == extensions.end()) {
				explicitExtension = false;
			}
		}

		if (explicitExtension) {
			// A supported extension was specified, only check for that specific asset name in that case
			if (s_mapFiles.contains(normalisedName)) {
				return normalisedName;
			}
		}
		else {
			// No valid extension was provided, try each extension and return first hit, if any
			for (const auto& extension : extensions)
			{
				tString qualifiedName = cString::SetFileExt(normalisedName, extension);
				if (s_mapFiles.contains(qualifiedName)) {
					return qualifiedName;
				}
			}
		}

		return "";
	}

	//-----------------------------------------------------------------------

}

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
#include "resources/FileSearcher.h"
#include "system/String.h"
#include "system/Files.h"
#include "system/Log.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cFileSearcher::cFileSearcher()
	{
	}

	//-----------------------------------------------------------------------

	cFileSearcher::~cFileSearcher()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	static std::set<tString> g_setIgnoredFiles = { ".ds_store", "thumbs.db" };

	void cFileSearcher::AddDirectory(tString asPath, tString asMask)
	{
		tWStringList lstFileNames;
		//Make the path with only "/" and lower case.
		asPath = cString::ToLowerCase(cString::ReplaceCharTo(asPath,"\\","/"));

		auto it = m_setLoadedDirs.find(asPath);
		//If the path is not allready added, add it!
		if (it == m_setLoadedDirs.end())
		{
			m_setLoadedDirs.insert(asPath);

			FindFilesInDir(lstFileNames, cString::To16Char(asPath), cString::To16Char(asMask));

			for(const tWString& sExt : lstFileNames)
			{
				tString sFile = cString::To8Char(sExt);
				tString sFileAllLower = cString::ToLowerCase(sFile);
				tString sFilePath = cString::SetFilePath(sFile, asPath);
				
				if (g_setIgnoredFiles.find(sFileAllLower) != g_setIgnoredFiles.end()) {
					// skip ignored files
					continue;
				}

				const auto [itElement, bInserted] = m_mapFiles.insert({ sFileAllLower, sFilePath });
				if (bInserted == false) {
					Log("Overriding resource '%s' from '%s' to '%s'\n", sFile.c_str(), cString::GetFilePath(itElement->second).c_str(), asPath.c_str());
					itElement->second.assign(sFilePath);
				}
			}
		}
	}

	void cFileSearcher::ClearDirectories()
	{
		m_mapFiles.clear();
		m_setLoadedDirs.clear();
	}

	//-----------------------------------------------------------------------

	tString cFileSearcher::GetFilePath(tString asName)
	{
		tFilePathMapIt it = m_mapFiles.find(cString::ToLowerCase(asName));
		if(it == m_mapFiles.end())return "";

		return it->second;
	}

	//-----------------------------------------------------------------------

}

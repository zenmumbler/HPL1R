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
#ifndef HPL_FILESEARCHER_H
#define HPL_FILESEARCHER_H

#include "system/StringTypes.h"
#include <span>

namespace hpl {

	namespace FileSearcher
	{
		/**
		 * Adds a directory that will be searched when looking for files.
		 * \param asMask What files that should be searched for, for example: "*.jpeg".
		 * \param asPath The path to the directory.
		 */
		void AddDirectory(const tString &path, const tString &mask);

		/**
		 * Clears all directories
		 */
		void ClearDirectories();

		/**
		 * Gets a file pointer and searches through all added resources.
		 * \param name Name of the file.
		 * \return Path to the file. "" if file is not found.
		 */
		tString GetFilePath(const tString &name);

		tString ResolveAssetName(const tString& name, std::span<const tString> extensions);
	}

}

#endif // HPL_FILESEARCHER_H

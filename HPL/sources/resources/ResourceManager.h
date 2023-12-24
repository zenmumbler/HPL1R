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
#ifndef HPL_RESOURCEMANAGER_H
#define HPL_RESOURCEMANAGER_H

#include "system/StringTypes.h"

#include <unordered_map>
#include <vector>
#include <span>

namespace hpl {

	class iResourceBase;

	class iResourceManager
	{
	public:
		iResourceManager(const tString& resourceTypeName);
		virtual ~iResourceManager();

		iResourceBase* GetByName(const tString& name);
		iResourceBase* GetByHandle(unsigned long handle);
		iResourceBase* GetOrLoadResource(const tString& name);

		template <typename Fn>
		void ForEachResource(const Fn&& fn) {
			for (auto [_, resource] : _nameToResourceMap)
			{
				fn(resource);
			}
		}

		void Destroy(iResourceBase* resource);

		virtual iResourceBase* LoadAsset(const tString &name, const tString &fullPath) = 0;
		virtual std::span<const tString> SupportedExtensions() const = 0;

	protected:
		void BeginLoad(const tString& fileName);
		void EndLoad();

		/**
		 * Checks if a resource alllready is in the manager, else searches the resources.
		 * \param &name Name of the resource.
		 * \param &outFilePath If the file is not in the manager, the path is put here. "" if there is no such file.
		 * \return A pointer to the resource. NULL if not in manager.
		 */
		iResourceBase* FindLoadedResource(const tString &name, tString &outFilePath);
		void AddResource(iResourceBase* resource, bool log=true);

		uint32_t GetHandle();

	private:
		void RemoveResource(iResourceBase* resource);
		void DestroyAll();

		tString _resourceTypeName;
		uint32_t _nextHandle;
		unsigned long _timeStart;

		std::unordered_map<tString, iResourceBase*> _nameToResourceMap;
		std::unordered_map<unsigned long, iResourceBase*> _handleToResourceMap;

		static int _tabCount;
	};

}

#endif // HPL_RESOURCEMANAGER_H

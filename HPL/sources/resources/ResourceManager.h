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

namespace hpl {

	class iResourceBase;

	class iResourceManager
	{
	public:
		iResourceManager(const tString& resourceTypeName);
		virtual ~iResourceManager();

		iResourceBase* GetByName(const tString& name);
		iResourceBase* GetByHandle(unsigned long handle);

		template <typename Fn>
		void ForEachResource(const Fn&& fn) {
			for (auto [_, resource] : _nameToResourceMap)
			{
				fn(resource);
			}
		}

		void DestroyUnused(int maxToKeep);

		void Destroy(iResourceBase* resource);
		void DestroyAll();

		virtual void Update(float timeStep){}

	protected:
		void BeginLoad(const tString& fileName);
		void EndLoad();

		/**
		 * Checks if a resource alllready is in the manager, else searches the resources.
		 * \param &name Name of the resource.
		 * \param &filePath If the file is not in the manager, the path is put here. "" if there is no such file.
		 * \return A pointer to the resource. NULL if not in manager.
		 */
		iResourceBase* FindLoadedResource(const tString &name, tString &filePath);
		void AddResource(iResourceBase* resource, bool log=true);
		void RemoveResource(iResourceBase* resource);

		uint32_t GetHandle();

	private:
		tString _resourceTypeName;
		uint32_t _nextHandle;
		unsigned long _timeStart;

		std::unordered_map<tString, iResourceBase*> _nameToResourceMap;
		std::unordered_map<unsigned long, iResourceBase*> _handleToResourceMap;

		static int _tabCount;
	};

};
#endif // HPL_RESOURCEMANAGER_H

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

#include "system/Container.h"
#include "system/StringTypes.h"
#include <map>

namespace hpl {

	class iResourceBase;

	class iResourceManager
	{
	public:
		iResourceManager();
		virtual ~iResourceManager();

		iResourceBase* GetByName(const tString& asName);
		iResourceBase* GetByHandle(unsigned long alHandle);

		template <typename Fn>
		void ForEachResource(const Fn&& fn) {
			for (auto [_, resource] : m_mapHandleResources)
			{
				fn(resource);
			}
		}

		void DestroyUnused(int alMaxToKeep);

		virtual void Destroy(iResourceBase* apResource)=0;
		virtual void DestroyAll();

		virtual void Update(float afTimeStep){}

	protected:
		unsigned long mlHandleCount;
		std::map<tString, iResourceBase*> m_mapNameResources;
		std::map<unsigned long, iResourceBase*> m_mapHandleResources;

		void BeginLoad(const tString& asFile);
		void EndLoad();

		/**
		 * Checks if a resource alllready is in the manager, else searches the resources.
		 * \param &asName Name of the resource.
		 * \param &asFilePath If the file is not in the manager, the path is put here. "" if there is no such file.
		 * \return A pointer to the resource. NULL if not in manager.
		 */
		iResourceBase* FindLoadedResource(const tString &asName, tString &asFilePath);
		void AddResource(iResourceBase* apResource, bool abLog=true);
		void RemoveResource(iResourceBase* apResource);

		unsigned long GetHandle();

		static int mlTabCount;
		unsigned long mlTimeStart;
	};

};
#endif // HPL_RESOURCEMANAGER_H

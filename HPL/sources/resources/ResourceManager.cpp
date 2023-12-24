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
#include "resources/ResourceManager.h"
#include "system/String.h"

#include "resources/FileSearcher.h"
#include "resources/ResourceBase.h"
#include "system/System.h"
#include "system/Log.h"

#include <algorithm>

namespace hpl {

	int iResourceManager::_tabCount=0;

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iResourceManager::iResourceManager(const tString& resourceTypeName)
	: _resourceTypeName{ resourceTypeName }
	, _nextHandle{1}
	{}

	iResourceManager::~iResourceManager() {
		DestroyAll();
		Log(" Done with %s resources\n", _resourceTypeName.c_str());
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iResourceBase* iResourceManager::GetByName(const tString& name)
	{
		tString sName = cString::ToLowerCase(name);

		auto it = _nameToResourceMap.find(sName);
		if (it == _nameToResourceMap.end()) return NULL;

		return it->second;
	}

	//-----------------------------------------------------------------------

	iResourceBase* iResourceManager::GetByHandle(unsigned long handle)
	{
		auto it = _handleToResourceMap.find(handle);
		if (it == _handleToResourceMap.end()) return NULL;

		return it->second;
	}

	//-----------------------------------------------------------------------

	iResourceBase* iResourceManager::GetOrLoadResource(const tString& name) {
		BeginLoad(name);

		tString qualifiedName = FileSearcher::ResolveAssetName(name, SupportedExtensions());
		if (qualifiedName.length() == 0) {
			Error("Couldn't resolve %s resource name '%s'\n", _resourceTypeName.c_str(), qualifiedName.c_str());
			return nullptr;
		}

		tString fullPath;
		iResourceBase *resource = this->FindLoadedResource(qualifiedName, fullPath);

		if (resource == nullptr && fullPath.length() > 0)
		{
			resource = LoadAsset(qualifiedName, fullPath);
			if (resource == nullptr) {
				Error("Couldn't load %s resource from '%s'\n", _resourceTypeName.c_str(), fullPath.c_str());
				EndLoad();
				return nullptr;
			}

			AddResource(resource);
		}

		resource->IncUserCount();

		EndLoad();
		return resource;
	}

	//-----------------------------------------------------------------------

	void iResourceManager::Destroy(iResourceBase* apResource)
	{
		apResource->DecUserCount();
		if (apResource->HasUsers() == false) {
			RemoveResource(apResource);
			delete apResource;
		}
	}

	//-----------------------------------------------------------------------

	void iResourceManager::DestroyAll()
	{
		auto it = _handleToResourceMap.begin();
		while (it != _handleToResourceMap.end())
		{
			iResourceBase* resource = it->second;
			// bypass user counts
			RemoveResource(resource);
			delete resource;

			it = _handleToResourceMap.begin();
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void iResourceManager::BeginLoad(const tString& asFile)
	{
		_timeStart = GetAppTimeMS();
		_tabCount++;
	}

	//-----------------------------------------------------------------------

	void iResourceManager::EndLoad()
	{
		_tabCount--;
	}

	//-----------------------------------------------------------------------

	iResourceBase* iResourceManager::FindLoadedResource(const tString &name, tString &outFilePath)
	{
		iResourceBase* pResource = GetByName(name);
		if (pResource==NULL) {
			outFilePath = FileSearcher::GetFilePath(name);
		}
		else {
			outFilePath = "";
		}

		return pResource;
	}

	//-----------------------------------------------------------------------

	void iResourceManager::AddResource(iResourceBase* apResource, bool abLog)
	{
		apResource->SetHandle(GetHandle());

		tString sName = cString::ToLowerCase(apResource->GetName());

		_handleToResourceMap.insert({ apResource->GetHandle(), apResource });
		_nameToResourceMap.insert({ sName, apResource });

		if (abLog && iResourceBase::GetLogCreateAndDelete())
		{
			unsigned long lTime = GetAppTimeMS() - _timeStart;
			Log("%sLoaded resource %s in %d ms\n", tString(_tabCount, '\t').c_str(), apResource->GetName().c_str(),lTime);
			apResource->SetLogDestruction(true);
		}

		//Log("End resource: %s\n",apResource->GetName().c_str());
	}

	//-----------------------------------------------------------------------

	void iResourceManager::RemoveResource(iResourceBase* apResource)
	{
		_handleToResourceMap.erase(apResource->GetHandle());
		_nameToResourceMap.erase(cString::ToLowerCase(apResource->GetName()));
	}

	//-----------------------------------------------------------------------

	uint32_t iResourceManager::GetHandle()
	{
		return _nextHandle++;
	}

	//-----------------------------------------------------------------------

}

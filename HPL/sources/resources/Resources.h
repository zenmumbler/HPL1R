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
#ifndef HPL_RESOURCES_H
#define HPL_RESOURCES_H

#include <map>
#include "system/SystemTypes.h"

#include "graphics/GPUProgram.h"
#include "graphics/GPUProgram.h"

#include "game/Updateable.h"

class TiXmlElement;

namespace hpl {

	class cImageEntity;

	class iLowLevelGraphics;
	class iResourceManager;
	class cFileSearcher;
	class cImageManager;
	class cGpuProgramManager;
	class cParticleManager;
	class cSoundManager;
	class cFontManager;
	class cScriptManager;
	class cTextureManager;
	class cMaterialManager;
	class cSoundEntityManager;
	class cAnimationManager;
	class cMeshManager;
	class cConfigFile;

	class cSound;
	class cScript;
	class cMeshLoaderHandler;
	class cScene;
	class cGraphics;
	class iRenderable;
	class cWorld3D;
	class iEntity3D;
	class cLanguageFile;

	//-------------------------------------------------------

	class iEntity3DLoader
	{
	public:
		iEntity3DLoader(const tString& asName): msName(asName){}
		virtual ~iEntity3DLoader(){}

		const tString& GetName(){ return msName;}

		virtual iEntity3D* Load(const tString &asName, TiXmlElement* apRootElem, const cMatrixf &a_mtxTransform,
								cWorld3D *apWorld, const tString &asFileName, bool abLoadReferences)=0;

	protected:
		tString msName;
	};

	typedef std::map<tString,iEntity3DLoader*> tEntity3DLoaderMap;
	typedef tEntity3DLoaderMap::iterator tEntity3DLoaderMapIt;

	//-------------------------------------------------------

	class iArea3DLoader
	{
	public:
		iArea3DLoader(const tString& asName): msName(asName){}
		virtual ~iArea3DLoader(){}

		const tString& GetName(){ return msName;}

		virtual iEntity3D* Load(const tString &asName, const cVector3f &avSize, const cMatrixf &a_mtxTransform,cWorld3D *apWorld)=0;

	protected:
		tString msName;
	};

	typedef std::map<tString,iArea3DLoader*> tArea3DLoaderMap;
	typedef tArea3DLoaderMap::iterator tArea3DLoaderMapIt;

	//-------------------------------------------------------

	// Should this be made a map so you can do
	// GetManager(tString) when getting a manager?
	// This way you would be able to add your own resource types
	// easily.
	typedef std::list<iResourceManager*> tResourceManagerList;
	typedef tResourceManagerList::iterator tResourceManagerListIt;

	//-------------------------------------------------------


	class cResources : public iUpdateable
	{
	public:
		cResources(iLowLevelGraphics *apLowLevelGraphics);
		~cResources();

		void Init(cGraphics* apGraphics, cSound *apSound, cScript *apScript, cScene *apScene);

		void Update(float afTimeStep);

		cFileSearcher* GetFileSearcher();

		void SetupResourceDirsForLanguage(const tString &asLangFile);
		bool AddResourceDir(const tString &asDir, const tString &asMask = "*.*");
		bool LoadResourceDirsFile(const tString &asFile);

		const tWString& Translate(const tString& asCat, const tString& asName);

		void AddEntity3DLoader(iEntity3DLoader* apLoader, bool abSetAsDefault=false);
		iEntity3DLoader* GetEntity3DLoader(const tString& asName);

		void AddArea3DLoader(iArea3DLoader* apLoader, bool abSetAsDefault=false);
		iArea3DLoader* GetArea3DLoader(const tString& asName);

		cImageManager* GetImageManager(){return mpImageManager;}
		cGpuProgramManager* GetGpuProgramManager(){return mpGpuProgramManager;}
		cParticleManager* GetParticleManager(){ return mpParticleManager;}
		cSoundManager* GetSoundManager(){ return mpSoundManager;}
		cFontManager* GetFontManager(){ return mpFontManager;}
		cScriptManager* GetScriptManager(){ return mpScriptManager;}
		cTextureManager* GetTextureManager(){ return mpTextureManager;}
		cMaterialManager* GetMaterialManager(){ return mpMaterialManager;}
		cMeshManager* GetMeshManager(){ return mpMeshManager;}
		cMeshLoaderHandler* GetMeshLoaderHandler(){ return mpMeshLoaderHandler;}
		cSoundEntityManager* GetSoundEntityManager(){ return mpSoundEntityManager;}
		cAnimationManager* GetAnimationManager(){ return mpAnimationManager;}

	private:
		void AddBaseDirectories();
		bool SetLanguageFile(const tString &asFile);

		iLowLevelGraphics *mpLowLevelGraphics;
		cFileSearcher *mpFileSearcher;

		tResourceManagerList mlstManagers;
		cImageManager *mpImageManager;
		cGpuProgramManager *mpGpuProgramManager;
		cParticleManager* mpParticleManager;
		cSoundManager* mpSoundManager;
		cFontManager* mpFontManager;
		cScriptManager* mpScriptManager;
		cTextureManager* mpTextureManager;
		cMaterialManager* mpMaterialManager;
		cSoundEntityManager* mpSoundEntityManager;
		cAnimationManager *mpAnimationManager;

		cLanguageFile *mpLanguageFile;

		cMeshManager* mpMeshManager;
		cMeshLoaderHandler* mpMeshLoaderHandler;

		tEntity3DLoaderMap m_mEntity3DLoaders;
		iEntity3DLoader* mpDefaultEntity3DLoader;

		tArea3DLoaderMap m_mArea3DLoaders;
		iArea3DLoader* mpDefaultArea3DLoader;

		tWString mwsEmptyString;
	};

};
#endif // HPL_RESOURCES_H

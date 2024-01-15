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
#include "resources/TextureManager.h"
#include "resources/impl/LoadBitmapFile.h"
#include "resources/FileSearcher.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/Bitmap.h"
#include "system/String.h"
#include "system/Log.h"

namespace hpl {

	static tString s_CubeSideSuffixes[] {
		"_pos_x",
		"_neg_x",
		"_pos_y",
		"_neg_y",
		"_pos_z",
		"_neg_z"
	};

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cTextureManager::cTextureManager(iLowLevelGraphics *llGfx)
		: iResourceManager{"texture"}, iUpdateable{"HPL_TextureMgr"}
	{
		_llGfx = llGfx;

		GetSupportedImageFormats(mvFileFormats);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iResourceBase* cTextureManager::LoadAsset(const tString &name, const tString &fullPath) {
		return nullptr;
	}

	std::span<const tString> cTextureManager::SupportedExtensions() const {
		return { mvFileFormats };
	}

	//-----------------------------------------------------------------------

	iTexture* cTextureManager::Create1D(const tString& name)
	{
		return CreateFlatTexture(name, eTextureTarget_1D);
	}

	//-----------------------------------------------------------------------

	iTexture* cTextureManager::Create2D(const tString& name)
	{
		return CreateFlatTexture(name, eTextureTarget_2D);
	}

	//-----------------------------------------------------------------------

	iTexture* cTextureManager::CreateAnim2D(const tString& fileName, eTextureAnimMode animMode)
	{
		BeginLoad(fileName);

		iTexture* pTexture = static_cast<iTexture*>(GetByName(fileName));

		if(pTexture==NULL)
		{
			tString sFileExt = cString::GetFileExt(fileName);
			tString sFileName = cString::SetFileExt(cString::GetFileName(fileName),"");

			tString sTest = sFileName + "01."+sFileExt;
			int lNum = 2;
			tStringVec vPaths;

			while(true)
			{
				tString sPath = FileSearcher::GetFilePath(sTest);

				if(sPath == "")
				{
					break;
				}
				else
				{
					vPaths.push_back(sPath);
					if(lNum<10)
						sTest = sFileName + "0"+cString::ToString(lNum)+"."+sFileExt;
					else
						sTest = sFileName + cString::ToString(lNum)+"."+sFileExt;

					++lNum;
				}
			}

			if(vPaths.empty())
			{
				Error("No textures found for animation %s\n", fileName.c_str());
				EndLoad();
				return NULL;
			}

			std::vector<Bitmap> vBitmaps;
			for(size_t i =0; i< vPaths.size(); ++i)
			{
				auto bmp = LoadBitmapFile(vPaths[i]);
				if (! bmp){
					Error("Couldn't load bitmap '%s'!\n",vPaths[i].c_str());
					EndLoad();
					return NULL;
				}

				vBitmaps.push_back(std::move(*bmp));
			}

			//Create the animated texture
			pTexture = _llGfx->CreateTexture(fileName, eTextureTarget_2D);
			pTexture->SetAnimMode(animMode);

			if(pTexture->CreateAnimFromBitmapVec(vBitmaps)==false)
			{
				Error("Couldn't create animated texture '%s'!\n", fileName.c_str());
				delete pTexture;
				EndLoad();
				return NULL;
			}

			AddResource(pTexture);
		}

		if(pTexture)pTexture->IncUserCount();
		else Error("Couldn't texture '%s'\n", fileName.c_str());

		EndLoad();
		return pTexture;
	}

	//-----------------------------------------------------------------------

	iTexture* cTextureManager::CreateCubeMap(const tString& asPathName)
	{
		tString sName = cString::SetFileExt(asPathName,"");

		iTexture* pTexture = static_cast<iTexture*>(GetByName(sName));

		BeginLoad(asPathName);

		if(pTexture==NULL)
		{
			//See if files for all faces exist
			tStringVec vPaths;
			tString sPath="";
			for(int i=0;i <6 ;i++)
			{
				for(const tString& sExt : mvFileFormats)
				{
					tString sNewName = sName + s_CubeSideSuffixes[i] + "." + sExt;
					sPath = FileSearcher::GetFilePath(sNewName);

					if(sPath!="")break;
				}

				if(sPath=="")
				{
					tString sNewName = sName + s_CubeSideSuffixes[i];
					Error("Couldn't find %d-face '%s', for cubemap '%s'\n",i,sNewName.c_str(),sName.c_str());
					return NULL;
				}

				vPaths.push_back(sPath);
			}

			//Load bitmaps for all faces
			std::vector<Bitmap> vBitmaps;
			for(int i=0;i<6; i++)
			{
				auto bmp = LoadBitmapFile(vPaths[i]);
				if (! bmp) {
					Error("Couldn't load bitmap '%s'!\n",vPaths[i].c_str());
					EndLoad();
					return NULL;
				}

				vBitmaps.push_back(std::move(*bmp));
			}

			//Create the cubemap
			pTexture = _llGfx->CreateTexture(sName, eTextureTarget_CubeMap);

			if (pTexture->CreateCubeFromBitmapVec(vBitmaps)==false)
			{
				Error("Couldn't create cubemap '%s'!\n", sName.c_str());
				delete pTexture;
				EndLoad();
				return NULL;
			}

			AddResource(pTexture);
		}

		if(pTexture)pTexture->IncUserCount();
		else Error("Couldn't texture '%s'\n",sName.c_str());

		EndLoad();
		return pTexture;
	}

	//-----------------------------------------------------------------------

	iTexture* cTextureManager::CreateFromBitmap(const tString &name, const Bitmap& bitmap) {
		BeginLoad(name);

		auto texture = _llGfx->CreateTexture(name, eTextureTarget_2D);
		if (! texture->CreateFromBitmap(bitmap)) {
			delete texture;
			return nullptr;
		}
		AddResource(texture);
		texture->IncUserCount();

		EndLoad();
		return texture;
	}

	//-----------------------------------------------------------------------

	void cTextureManager::Update(float afTimeStep)
	{
		ForEachResource([=](iResourceBase *resource) {
			iTexture *pTexture = static_cast<iTexture*>(resource);
			pTexture->Update(afTimeStep);
		});
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iTexture* cTextureManager::CreateFlatTexture(const tString &name, eTextureTarget target)
	{
		tString sPath;
		iTexture* pTexture;

		BeginLoad(name);

		pTexture = FindTexture2D(name, sPath);

		if(pTexture==NULL && sPath!="")
		{
			//Load the bitmap
			auto pBmp = LoadBitmapFile(sPath);
			if (! pBmp)
			{
				Error("Texturemanager Couldn't load bitmap '%s'\n", sPath.c_str());
				EndLoad();
				return NULL;
			}

			//Create the texture and load from bitmap
			pTexture = _llGfx->CreateTexture(name, target);
			if(pTexture->CreateFromBitmap(*pBmp)==false)
			{
				delete pTexture;
				EndLoad();
				return NULL;
			}

			AddResource(pTexture);
		}

		if(pTexture)pTexture->IncUserCount();
		else Error("Couldn't texture '%s'\n", name.c_str());

		EndLoad();
		return pTexture;
	}

	//-----------------------------------------------------------------------

	iTexture* cTextureManager::FindTexture2D(const tString &name, tString &fullPath)
	{
		iTexture *pTexture = nullptr;

		if (cString::GetFileExt(name).length() == 0)
		{
			for (const tString& sExt : mvFileFormats)
			{
				auto qualifiedName = cString::SetFileExt(name, sExt);
				pTexture = static_cast<iTexture*> (FindLoadedResource(qualifiedName, fullPath));

				if((pTexture==nullptr && fullPath.length() > 0) || pTexture!=nullptr) break;
			}
		}
		else
		{
			pTexture = static_cast<iTexture*> (FindLoadedResource(name, fullPath));
		}

		return pTexture;
	}


	//-----------------------------------------------------------------------
}

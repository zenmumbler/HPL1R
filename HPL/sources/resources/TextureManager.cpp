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

	static std::vector<tString> s_fileFormats;


	static bool ResolveCubemapPaths(const tString &name, std::vector<tString> &paths) {
		auto baseName = cString::GetFileName(name);
		auto ext = cString::GetFileExt(name);

		for (int ix = 0; ix < 6; ++ix) {
			auto sideName = baseName + s_CubeSideSuffixes[ix] + "." + ext;
			auto sidePath = FileSearcher::ResolveAssetName(sideName, { s_fileFormats });
			if (sidePath.length() == 0) {
				return false;
			}
			paths.push_back(sidePath);
		}

		return true;
	}

	static bool ResolveSequencePaths(const tString &name, std::vector<tString> &paths) {
		auto baseName = cString::GetFileName(name);
		auto ext = cString::GetFileExt(name);

		for (int ix = 0; ix < 100; ++ix) {
			auto frameName = baseName + (ix < 10 ? "0" : "") + std::to_string(ix) + "." + ext;
			auto framePath = FileSearcher::ResolveAssetName(frameName, { s_fileFormats });
			if (framePath.length() == 0) {
				break;
			}
			paths.push_back(framePath);
		}

		return paths.size() > 0;
	}

	static std::optional<Bitmap> LoadBitmap(const tString &fullPath) {
		auto maybeBitmap = LoadBitmapFile(fullPath);
		if (! maybeBitmap) {
			Error("Bitmap file '%s' could not be loaded!", fullPath.c_str());
			return std::nullopt;
		}
		return maybeBitmap;
	}

	static bool LoadBitmaps(const std::vector<tString> &paths, std::vector<Bitmap> &bitmaps) {
		for (const auto& path : paths) {
			auto maybeBitmap = LoadBitmap(path);
			if (! maybeBitmap) {
				return false;
			}
			bitmaps.push_back(std::move(*maybeBitmap));
		}
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cTextureManager::cTextureManager(iLowLevelGraphics *llGfx)
		: iResourceManager{"texture"}, iUpdateable{"HPL_TextureMgr"}
	{
		llGfx_ = llGfx;

		GetSupportedBitmapFormats(s_fileFormats);
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
		return { s_fileFormats };
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
			std::vector<tString> paths;
			std::vector<Bitmap> bitmaps;

			if (! ResolveSequencePaths(fileName, paths)) {
				Error("No textures found for animation %s\n", fileName.c_str());
				EndLoad();
				return nullptr;
			}
			if (! LoadBitmaps(paths, bitmaps)) {
				EndLoad();
				return nullptr;
			}

			//Create the animated texture
			pTexture = llGfx_->CreateTexture(fileName, eTextureTarget_2D);
			pTexture->SetAnimMode(animMode);

			if (pTexture->CreateAnimFromBitmapVec(bitmaps)==false)
			{
				Error("Couldn't create animated texture '%s'!\n", fileName.c_str());
				delete pTexture;
				EndLoad();
				return nullptr;
			}

			AddResource(pTexture);
		}

		if(pTexture)pTexture->IncUserCount();
		else Error("Couldn't resolve texture '%s'\n", fileName.c_str());

		EndLoad();
		return pTexture;
	}

	//-----------------------------------------------------------------------

	iTexture* cTextureManager::CreateCubeMap(const tString& fileName)
	{
		BeginLoad(fileName);

		iTexture* pTexture = static_cast<iTexture*>(GetByName(fileName));

		if (pTexture==NULL)
		{
			std::vector<tString> paths;
			std::vector<Bitmap> bitmaps;

			if (! ResolveCubemapPaths(fileName, paths)) {
				Error("No textures found for animation %s\n", fileName.c_str());
				EndLoad();
				return nullptr;
			}
			if (! LoadBitmaps(paths, bitmaps)) {
				EndLoad();
				return nullptr;
			}

			//Create the cubemap
			pTexture = llGfx_->CreateTexture(fileName, eTextureTarget_CubeMap);

			if (pTexture->CreateCubeFromBitmapVec(bitmaps)==false)
			{
				Error("Couldn't create cubemap '%s'!\n", fileName.c_str());
				delete pTexture;
				EndLoad();
				return NULL;
			}

			AddResource(pTexture);
		}

		if(pTexture)pTexture->IncUserCount();
		else Error("Couldn't resolve texture '%s'\n", fileName.c_str());

		EndLoad();
		return pTexture;
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
			pTexture = llGfx_->CreateTexture(name, target);
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
			for (const tString& sExt : s_fileFormats)
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

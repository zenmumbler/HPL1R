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
#include "resources/ImageManager.h"
#include "system/String.h"
#include "resources/ResourceImage.h"
#include "resources/FrameBitmap.h"
#include "resources/FrameTexture.h"
#include "graphics/LowLevelGraphics.h"
#include "resources/LoadImage.h"
#include "system/Log.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cImageManager::cImageManager(cFileSearcher *apFileSearcher, iLowLevelGraphics *apLowLevelGraphics)
	: iResourceManager(apFileSearcher)
	{
	   mpLowLevelGraphics = apLowLevelGraphics;

	   GetSupportedImageFormats(mvFileFormats);

	   mvFrameSize = cVector2l(512, 512);
	   mlFrameHandle = 0;
	}

	cImageManager::~cImageManager()
	{
		DestroyAll();

		Log(" Done with images\n");
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iResourceBase* cImageManager::Create(const tString& asName)
	{
		return CreateImage(asName);
	}

	//-----------------------------------------------------------------------

	cResourceImage* cImageManager::CreateImage(const tString& asName)
	{
		cResourceImage *pImage = NULL;
		tString sPath;

		BeginLoad(asName);

		pImage = FindImage(asName, sPath);
		if(!pImage)
		{
			if(sPath != "")
			{
				auto bitmap = LoadBitmapFile(sPath);
				if(! bitmap) {
					Error("Imagemanager Couldn't load bitmap '%s'\n", sPath.c_str());
					EndLoad();
					return NULL;
				}

				pImage = AddToFrame(*bitmap);

				if(pImage == NULL){
					Error("Imagemanager couldn't create image '%s'\n", asName.c_str());
				}

				if(pImage) AddResource(pImage);
			}
		}
		else
		{
			//Log("Found '%s' in stock!\n",asName.c_str());
		}

		if(pImage)pImage->IncUserCount();
		else Error("Couldn't load image '%s'\n",asName.c_str());

		//Log("Loaded image %s, it has %d users!\n", pImage->GetName().c_str(),pImage->GetUserCount());
		//Log(" frame has %d pics\n", pImage->GetFrameTexture()->GetPicCount());

		EndLoad();
		return pImage;
	}

	//-----------------------------------------------------------------------

	cResourceImage* cImageManager::CreateFromBitmap(const Bitmap &bmp)
	{
		cResourceImage *pImage = AddToFrame(bmp);

		if(pImage){
			AddResource(pImage, false);
			pImage->IncUserCount();
		}

		return pImage;
	}

	//-----------------------------------------------------------------------

	void cImageManager::Destroy(iResourceBase* apResource)
	{
		//Lower the user num for the the resource. If it is 0 then lower the
		//user num for the TextureFrame and delete the resource. If the Texture
		//frame reaches 0 it is deleted as well.
		cResourceImage *pImage = static_cast<cResourceImage*>(apResource);
		cFrameTexture *pFrame = pImage->GetFrameTexture();
		cFrameBitmap *pBmpFrame = pImage->GetFrameBitmap();

		//pImage->GetFrameBitmap()->FlushToTexture(); Not needed?


		//Log("Users Before: %d\n",pImage->GetUserCount());
		//Log("Framepics Before: %d\n",pFrame->GetPicCount());

		pImage->DecUserCount();//dec frame count as well.. is that ok?

		//Log("---\n");
		//Log("Destroyed Image: '%s' Users: %d\n",pImage->GetName().c_str(),pImage->GetUserCount());
		//Log("Frame %d has left Pics: %d\n",pFrame,pFrame->GetPicCount());

		if(pImage->HasUsers()==false)
		{
			pFrame->DecPicCount(); // Doing it here now instead.
			pBmpFrame->DecPicCount();
			RemoveResource(apResource);
			delete apResource;

			//Log("deleting image and dec fram to %d images!\n",pFrame->GetPicCount());
		}


		if(pFrame->IsEmpty())
		{
			//Log(" Deleting frame...");

			//Delete the bitmap frame that has this this frame.
			for(tFrameBitmapListIt it=mlstBitmapFrames.begin();it!=mlstBitmapFrames.end();++it)
			{
				cFrameBitmap *pBmpFrame = *it;
				if(pBmpFrame->GetFrameTexture() == pFrame)
				{
					//Log("and bitmap...");
					delete pBmpFrame;
					mlstBitmapFrames.erase(it);
					break;
				}
			}

			//delete from list
			m_mapTextureFrames.erase(pFrame->GetHandle());
			delete pFrame;
			//Log(" Deleted frame!\n");
		}
		//Log("---\n");

	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cResourceImage *cImageManager::FindImage(const tString &asName, tString &asFilePath)
	{
		cResourceImage *pImage=NULL;

		if(cString::GetFileExt(asName)=="")
		{
			for (const tString& sExt : mvFileFormats)
			{
				tString sNewName = cString::SetFileExt(asName, sExt);
				pImage = static_cast<cResourceImage*> (FindLoadedResource(sNewName, asFilePath));

				if((pImage==NULL && asFilePath!="") || pImage!=NULL)break;
			}
		}
		else
		{
			pImage = static_cast<cResourceImage*> (FindLoadedResource(asName, asFilePath));
		}

		return pImage;
	}

	//-----------------------------------------------------------------------

	cResourceImage *cImageManager::AddToFrame(const Bitmap &aBmp)
	{
		cResourceImage *pImage = nullptr;

		for (auto frame : mlstBitmapFrames) {
			if (! frame->IsFull()) {
				pImage = frame->AddBitmap(aBmp);
				if (pImage) {
					return pImage;
				}
			}
		}

		// no frames had space, create new one
		cFrameBitmap *pFrame = CreateBitmapFrame(mvFrameSize);
		if (pFrame) {
			pImage = pFrame->AddBitmap(aBmp);
			if (pImage == nullptr)
			{
				Error("No fit in new frame!\n");
			}
		}

		return pImage;
	}

	//-----------------------------------------------------------------------

	cFrameBitmap *cImageManager::CreateBitmapFrame(cVector2l avSize)
	{
		iTexture *pTex = mpLowLevelGraphics->CreateTexture(false, eTextureType_Normal, eTextureTarget_2D);
		cFrameTexture *pTFrame = new cFrameTexture(pTex, mlFrameHandle);
		cFrameBitmap *pBFrame = new cFrameBitmap(avSize.x, avSize.y, pTFrame, mlFrameHandle);

		mlstBitmapFrames.push_back(pBFrame);

		std::pair<tFrameTextureMap::iterator, bool> ret = m_mapTextureFrames.insert(tFrameTextureMap::value_type(mlFrameHandle, pTFrame));
		if(ret.second == false)
		{
			Error("Could not add texture frame %d with handle %d! Handle already exist!\n", pTFrame, mlFrameHandle);
		}
		else
		{
			//Log("Added texture frame: %d\n",pTFrame);
		}

		mlFrameHandle++;
		return pBFrame;
	}


	//-----------------------------------------------------------------------
}

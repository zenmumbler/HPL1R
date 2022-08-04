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
#ifndef HPL_IMAGEMANAGER_H
#define HPL_IMAGEMANAGER_H

#include "resources/ResourceManager.h"
#include "math/MathTypes.h"

namespace hpl {

	class cResourceImage;
	class cFrameTexture;
	class cFrameBitmap;
	class iLowLevelGraphics;
	class Bitmap;

	typedef std::list<cFrameBitmap*> tFrameBitmapList;
	typedef tFrameBitmapList::iterator tFrameBitmapListIt;

	typedef std::map<int,cFrameTexture*> tFrameTextureMap;
	typedef tFrameTextureMap::iterator tFrameTextureMapIt;

	class cImageManager :public iResourceManager
	{
	public:
		cImageManager(cFileSearcher *apFileSearcher, iLowLevelGraphics *apLowLevelGraphics);
		~cImageManager();

		iResourceBase* Create(const tString& asName);
		void Destroy(iResourceBase* apResource);

		//Image specifc
		cResourceImage* CreateImage(const tString& asName);
		cResourceImage* CreateFromBitmap(const Bitmap &bmp);

	private:
		iLowLevelGraphics *mpLowLevelGraphics;

		tFrameBitmapList mlstBitmapFrames;
		tFrameTextureMap m_mapTextureFrames;

		tStringVec mvFileFormats;
		cVector2l mvFrameSize;
		int mlFrameHandle;

		cResourceImage *FindImage(const tString &asName, tString &asFilePath);
		cResourceImage *AddToFrame(const Bitmap &bmp);
		cFrameBitmap *CreateBitmapFrame(cVector2l avSize);

	};

};
#endif // HPL_RESOURCEMANAGER_H

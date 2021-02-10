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
#ifndef HPL_GFX_OBJECT_H
#define HPL_GFX_OBJECT_H

#include "graphics/GraphicsTypes.h"

namespace hpl {

	class iTexture;
	class cResourceImage;
	class cImageManager;
	class iLowLevelGraphics;

	enum eOldMaterialType
	{
		eOldMaterialType_Null,

		eOldMaterialType_DiffuseAlpha,
		eOldMaterialType_DiffuseAdditive,
		eOldMaterialType_Smoke,
		eOldMaterialType_FontNormal
   };


	class iOldMaterial {
	public:
		iOldMaterial(eOldMaterialType aiType, cImageManager* apImageManager);
		virtual ~iOldMaterial();

		virtual void StartRendering(iLowLevelGraphics* apLowLevelGraphics) const = 0;
		virtual void EndRendering(iLowLevelGraphics* apLowLevelGraphics) const = 0;

		void SetImage(cResourceImage* apImage) { mpImage = apImage; }
		cResourceImage* GetImage() const { return mpImage; }
		iTexture* GetTexture() const;
		
		const eOldMaterialType mType;
	
	private:
		cResourceImage* mpImage;
		cImageManager* mpImageManager;
	};


	class cGfxObject
	{
	public:
		cGfxObject(iOldMaterial* apMat, const tString& asFile);
		~cGfxObject();

		iOldMaterial* GetMaterial() const{ return mpMat; }
		cVertex* GetVtxPtr(int alNum){ return &mvVtx[alNum]; }
		tVertexVec* GetVertexVec(){ return &mvVtx; }

		const tString& GetSourceFile(){ return msSourceFile; }

	private:
		iOldMaterial* mpMat;
		tString msSourceFile;
		tVertexVec mvVtx;
	};

}

#endif // HPL_GFX_OBJECT_H

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
#ifndef HPL_GRAPHICSDRAWER_H
#define HPL_GRAPHICSDRAWER_H

#include "resources/ImageManager.h"
#include "resources/GpuProgramManager.h"
#include "graphics/GraphicsTypes.h"
#include "graphics/Bitmap.h"
#include "graphics/GfxBatch.h"
#include "graphics/GPUProgram.h"

#include <set>

namespace hpl {

	class iLowLevelGraphics;
	class iTexture;

	enum class eGfxMaterial
	{
		Null,

		DiffuseAlpha,
		DiffuseAdditive,
		Smoke
   };

	struct cGfxObject {
		tString sourceFile;
		cResourceImage* image;
		eGfxMaterial material;
		std::vector<cVector2f> uvs;
		bool isManaged;

		cVector2l GetSize() const;
		cVector2f GetFloatSize() const;
	};
	
	struct cGfxBufferObject
	{
		iTexture* texture;
		eGfxMaterial material;
		cVector3f mvPosition;
		cVector2f mvSize;
		cColor mColor;
		cVector2f uv0, uv1, uv2, uv3;
	};


	class cGfxBufferCompare
	{
	public:
		bool operator()(const cGfxBufferObject& aObjectA,const cGfxBufferObject& aObjectB) const;
	};

	typedef std::multiset<cGfxBufferObject,cGfxBufferCompare> tGfxBufferSet;

	class cResources;

	class cGraphicsDrawer
	{
	public:
		cGraphicsDrawer(iLowLevelGraphics *apLowLevelGraphics, cImageManager* apImageManager, cGpuProgramManager* programManager);
		~cGraphicsDrawer();

		void DrawGfxObject(const cGfxObject* apObject, const cVector3f& avPos);
		void DrawGfxObject(const cGfxObject* apObject, const cVector3f& avPos, const cVector2f& avSize, const cColor& aColor);
		void DrawTexture(iTexture *apTex, const cVector3f& avPos, const cVector2f& avSize, const cColor &aColor = cColor::White);

		void DrawAll();

		const cGfxObject* CreateGfxObject(const tString &fileName, eGfxMaterial material);
		const cGfxObject* CreateUnmanagedGfxObject(const Bitmap &bitmap, eGfxMaterial material);
		void DestroyGfxObject(const cGfxObject* apObject);

	private:
		void UseMaterial(eGfxMaterial material);

		iLowLevelGraphics *mpLowLevelGraphics;
		cImageManager *mpImageManager;
		cGpuProgramManager* _programManager;

		tGfxBufferSet m_setGfxBuffer;
		cGfxBatch _batch;
		iGpuProgram *_program;
		std::vector<const cGfxObject*> mvGfxObjects;
	};

};

#endif // HPL_GRAPHICSDRAWER_H

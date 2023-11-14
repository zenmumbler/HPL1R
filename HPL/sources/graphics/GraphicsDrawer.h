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
		union {
			cResourceImage* image;
			iTexture *texture;
		};
		eGfxMaterial material;
		std::vector<cVector2f> uvs;

		bool isImage;
		bool isManaged;

		cVector2l GetSize() const;
		cVector2f GetFloatSize() const;
	};
	
	class cGfxBufferObject
	{
	public:
		const cGfxObject* mpObject;
		cVector3f mvPosition;
		cColor mColor;
		cVector2f mvSize;

		iTexture *GetTexture() const;
		eGfxMaterial GetMaterial() const { return mpObject->material; }
		float GetZ() const { return mvPosition.z; }
	};


	class cGfxBufferCompare
	{
	public:
		bool operator()(const cGfxBufferObject& aObjectA,const cGfxBufferObject& aObjectB) const;
	};

	typedef std::multiset<cGfxBufferObject,cGfxBufferCompare> tGfxBufferSet;
	typedef tGfxBufferSet::iterator tGfxBufferSetIt;

	class cResources;

	class cGraphicsDrawer
	{
	public:
		cGraphicsDrawer(iLowLevelGraphics *apLowLevelGraphics, cImageManager* apImageManager, cGpuProgramManager* programManager);
		~cGraphicsDrawer();

		/**
		 * Draw Gfx object during next DrawAll call.
		 * \param apObject
		 * \param avPos
		 */
		void DrawGfxObject(const cGfxObject* apObject, const cVector3f& avPos);

		/**
		 * Draw Gfx object during next DrawAll call.
		 * \param apObject
		 * \param avPos
		 * \param avSize Size of object
		 * \param aColor color to use
		 */
		void DrawGfxObject(const cGfxObject* apObject, const cVector3f& avPos, const cVector2f& avSize, const cColor& aColor);


		/**
		 * Draw all gfx obejcts, Called after world is rendered by cScene.
		 */
		void DrawAll();


		const cGfxObject* CreateGfxObject(iTexture *texture, eGfxMaterial material);

		/**
		 * Create Gfx object from file
		 * \param &fileName Filename of image
		 * \param material material to use
		 * \return
		 */
		const cGfxObject* CreateGfxObject(const tString &fileName, eGfxMaterial material);


		/**
		 * Create unmanaged gfx object from Bitmap
		 * \param *bitmap bitmap
		 * \param material material to use
		 * \return
		 */
		const cGfxObject* CreateUnmanagedGfxObject(const Bitmap &bitmap, eGfxMaterial material);

		/**
		 * Destroys a gfx object.
		 */
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

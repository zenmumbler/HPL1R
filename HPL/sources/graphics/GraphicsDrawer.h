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
#include "graphics/GraphicsTypes.h"
#include "graphics/Bitmap2D.h"

namespace hpl {

	class iLowLevelGraphics;
	class cResourceImage;
	class cGfxObject;
	class iOldMaterial;

	class cGfxBufferObject
	{
	public:
		cGfxObject* mpObject;
		cVector3f mvTransform;

		bool mbIsColorAndSize;
		cColor mColor;
		cVector2f mvSize;

		iOldMaterial* GetMaterial() const;
		float GetZ() const { return mvTransform.z;}
	};


	class cGfxBufferCompare
	{
	public:
		bool operator()(const cGfxBufferObject& aObjectA,const cGfxBufferObject& aObjectB) const;
	};

	typedef std::multiset<cGfxBufferObject,cGfxBufferCompare> tGfxBufferSet;
	typedef tGfxBufferSet::iterator tGfxBufferSetIt;

	class cResources;

	typedef std::list<cGfxObject*> tGfxObjectList;
	typedef tGfxObjectList::iterator tGfxObjectListIt;

	class cGraphicsDrawer
	{
	public:
		cGraphicsDrawer(iLowLevelGraphics *apLowLevelGraphics, cImageManager* apImageManager);
		~cGraphicsDrawer();

		/**
		 * Draw Gfx object during next DrawAll call.
		 * \param apObject
		 * \param avPos
		 */
		void DrawGfxObject(cGfxObject* apObject, const cVector3f& avPos);

		/**
		 * Draw Gfx object during next DrawAll call.
		 * \param apObject
		 * \param avPos
		 * \param avSize Size of object
		 * \param aColor color to use
		 */
		void DrawGfxObject(cGfxObject* apObject, const cVector3f& avPos,
						   const cVector2f& avSize, const cColor& aColor);


		/**
		 * Draw all gfx obejcts, Called after world is rendered by cScene.
		 */
		void DrawAll();
		
		iOldMaterial* CreateMaterial(const tString& asMaterialName, cResourceImage* apImage);

		/**
		 * Create Gfx object from file
		 * \param &asFileName Filename of image
		 * \param &asMaterialName material to use
		 * \param abAddToList if the engine should delete object at exit, this means DestroyGfxObject must be used. Should almost always be true.
		 * \return
		 */
		cGfxObject* CreateGfxObject(const tString &asFileName, const tString &asMaterialName,
									bool abAddToList=true);
		/**
		 * Create gfx object from Bitmap
		 * \param *apBmp bitmap
		 * \param &asMaterialName material to use
		 * \param abAddToList if the engine should delete object at exit, this means DestroyGfxObject must be used. Should almost always be true.
		 * \return
		 */
		cGfxObject* CreateGfxObject(iBitmap2D *apBmp, const tString &asMaterialName,
									bool abAddToList=true);
		/**
		 * Destroys a gfx object.
		 */
		void DestroyGfxObject(cGfxObject* apObject);

	private:
		iLowLevelGraphics *mpLowLevelGraphics;
		cImageManager *mpImageManager;
		tGfxBufferSet m_setGfxBuffer;
		tGfxObjectList mlstGfxObjects;
	};

};
#endif // HPL_GRAPHICSDRAWER_H

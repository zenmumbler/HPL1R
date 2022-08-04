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
#ifndef HPL_FONTDATA_H
#define HPL_FONTDATA_H

#include <vector>
#include "math/MathTypes.h"
#include "system/SystemTypes.h"
#include "system/SystemTypes.h"
#include "graphics/GraphicsTypes.h"
#include "resources/ResourceBase.h"

namespace hpl {

	class ImageManager;
	class iLowLevelGraphics;
	class cGraphicsDrawer;
	struct cGfxObject;
	class Bitmap;

	//------------------------------------------------

	class cGlyph
	{
	public:
		cGlyph(	const cGfxObject *apObject, const cVector2f &avOffset,
				const cVector2f &avSize, float afAdvance);
		~cGlyph() = default;

		const cGfxObject *mpGfxObject;
		cVector2f mvOffset;
		cVector2f mvSize;
		float mfAdvance;
	};

	typedef std::vector<cGlyph*> tGlyphVec;
	typedef tGlyphVec::iterator tGlyphVecIt;

	class FontData : public iResourceBase
	{
	public:
		FontData(const tString &asName);
		~FontData();

		bool CreateFromBitmapFile(const tString &asFileName);


		void Destroy(){}

		/**
		 * Used internally
		 */
		void SetUp(cGraphicsDrawer *apGraphicsDrawer)
		{
			mpGraphicsDrawer = apGraphicsDrawer;
		}

		/**
		 * Used internally
		 * \param alNum
		 * \return
		 */
		inline cGlyph* GetGlyph(int alNum)const { return mvGlyphs[alNum];}

		inline unsigned short GetFirstChar(){ return mlFirstChar;}
		inline unsigned short GetLastChar(){ return mlLastChar;}

		inline const cVector2f& GetSizeRatio()const{ return mvSizeRatio;}

		/**
		 * Draw a string.
		 * \param avPos Screen pos
		 * \param avSize size of the characters
		 * \param aCol color
		 * \param mAlign alignment
		 * \param fmt
		 * \param ...
		 */
		void Draw(const cVector3f& avPos,const cVector2f& avSize, const cColor& aCol,eFontAlign mAlign,
					const wchar_t* fmt,...);
		/**
		 * Draw a string  with word wrap.
		 * \param avPos Screen pos
		 * \param afLength Max length of a line
		 * \param afFontHeight The distance from base of character above to base of character below
		 * \param avSize size of the characters
		 * \param aCol color
		 * \param aAlign alignment
		 * \param asString
		 * \return Extra number of rows generated.
		 */
		int DrawWordWrap(cVector3f avPos,float afLength,float afFontHeight,cVector2f avSize,const cColor& aCol,
							eFontAlign aAlign,	const tWString &asString);


		void GetWordWrapRows(float afLength,float afFontHeight,cVector2f avSize,const tWString& asString,
								tWStringVec *apRowVec);

		/**
		 * Get height of the font.
		 * \return
		 */
		inline float GetHeight()const{ return mfHeight; }

		/**
		 * Get the length in virtual screen size "pixels" of a formated string
		 * \param avSize size of the characters
		 * \param fmt
		 * \param ...
		 * \return
		 */
		float GetLengthFmt(const cVector2f& avSize,const wchar_t* fmt,...);
		/**
		 * Get the length in virtual screen size "pixels" of a string
		 * \param avSize size of the characters
		 * \param sText
		 * \return
		 */
		float GetLength(const cVector2f& avSize,const wchar_t* sText);

	protected:
		cGraphicsDrawer *mpGraphicsDrawer;

		tGlyphVec mvGlyphs;

		float mfHeight;
		unsigned short mlFirstChar;
		unsigned short mlLastChar;

		cVector2f mvSizeRatio;

		cGlyph* CreateGlyph(const Bitmap &aBmp, const cVector2l &avOffset,const cVector2l &avSize,
							const cVector2l& avFontSize, int alAdvance);
		void AddGlyph(cGlyph *apGlyph);
	};

};
#endif

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
#include "graphics/FontData.h"

#include "graphics/GraphicsDrawer.h"

#include "resources/ResourceImage.h"
#include "resources/LoadImage.h"
#include "system/Log.h"

#include "tinyXML/tinyxml.h"

namespace hpl {


	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////


	//-----------------------------------------------------------------------

	cGlyph::cGlyph(	const cGfxObject *apObject, const cVector2f &avOffset,
					const cVector2f &avSize, float afAdvance)
	{
		mpGfxObject = apObject;
		mvOffset = avOffset;
		mvSize = avSize;
		mfAdvance = afAdvance;
	}

	//-----------------------------------------------------------------------

	FontData::FontData(const tString &asName)
	: iResourceBase(asName,0)
	{
	}

	//-----------------------------------------------------------------------

	FontData::~FontData()
	{
		for(int i=0; i <(int)mvGlyphs.size(); i++)
		{
			if(mvGlyphs[i])
			{
				if (mvGlyphs[i]->mpGfxObject) {
					// [zm] Graphics has already been destroyed at this point...
					// mpGraphicsDrawer->DestroyGfxObject(mvGlyphs[i]->mpGfxObject);
				}
				delete mvGlyphs[i];
			}
		}
	}


	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool FontData::CreateFromBitmapFile(const tString &asFileName)
	{
		tString sPath = cString::GetFilePath(asFileName);

		////////////////////////////////////////////
		// Load xml file
		TiXmlDocument *pXmlDoc = new TiXmlDocument(asFileName.c_str());

		if(pXmlDoc->LoadFile()==false)
		{
			Error("Couldn't load angle code font file '%s'\n",asFileName.c_str());
			delete pXmlDoc;
			return false;
		}

		TiXmlElement *pRootElem = pXmlDoc->RootElement();

		////////////////////////////////////////////
		// Load Common info
		TiXmlElement *pCommonElem = pRootElem->FirstChildElement("common");

		int lLineHeight = cString::ToInt(pCommonElem->Attribute("lineHeight"),0);
		int lBase = cString::ToInt(pCommonElem->Attribute("base"),0);

		mlFirstChar =0;
		mlLastChar = 3000; //Get this num from something.

		mfHeight = (float)lLineHeight;

		mvGlyphs.resize(3000, NULL);

		mvSizeRatio.x = (float)lBase / (float)lLineHeight;
		mvSizeRatio.y = 1;

		////////////////////////////////////////////
		// Load bitmaps
		std::vector<Bitmap> vBitmaps;

		TiXmlElement *pPagesRootElem = pRootElem->FirstChildElement("pages");

		TiXmlElement *pPageElem = pPagesRootElem->FirstChildElement("page");
		for(; pPageElem != NULL; pPageElem = pPageElem->NextSiblingElement("page"))
		{
			tString sFileName = pPageElem->Attribute("file");
			tString sFilePath = cString::SetFilePath(sFileName,sPath);

			auto bitmap = LoadBitmapFile(sFilePath);
			if (! bitmap)
			{
				Error("Couldn't load bitmap %s for FNT file '%s'\n",sFilePath.c_str(),asFileName.c_str());
				delete pXmlDoc;
				return false;
			}

			vBitmaps.push_back(std::move(*bitmap));
		}

		////////////////////////////////////////////
		// Load glyphs
		TiXmlElement *pCharsRootElem = pRootElem->FirstChildElement("chars");

		TiXmlElement *pCharElem = pCharsRootElem->FirstChildElement("char");
		for(; pCharElem != NULL; pCharElem = pCharElem->NextSiblingElement("char"))
		{
			//Get the info on the character
			int lId = cString::ToInt(pCharElem->Attribute("id"),0);
			int lX = cString::ToInt(pCharElem->Attribute("x"),0);
			int lY = cString::ToInt(pCharElem->Attribute("y"),0);

			int lW = cString::ToInt(pCharElem->Attribute("width"),0);
			int lH = cString::ToInt(pCharElem->Attribute("height"),0);

			int lXOffset = cString::ToInt(pCharElem->Attribute("xoffset"),0);
			int lYOffset = cString::ToInt(pCharElem->Attribute("yoffset"),0);

			int lAdvance = cString::ToInt(pCharElem->Attribute("xadvance"),0);

			int lPage = cString::ToInt(pCharElem->Attribute("page"),0);

			//Get the bitmap where the character graphics is
			auto& sourceBitmap = vBitmaps[lPage];

			//Create a bitmap for the character.
			Bitmap bmp{lW, lH};

			//Copy from source to character bitmap
			bmp.CopyFromBitmap(sourceBitmap, lX, lY, lW, lH);

			//Set alpha to grayscale value of glyph pixel
			auto pixelData = bmp.GetRawData<uint8_t>();
			for (int y=0; y < bmp.GetHeight(); y++) {
				for (int x=0; x < bmp.GetWidth(); x++) {
					uint8_t *Pix = pixelData + (y * bmp.GetWidth() * 4) + x * 4;
					Pix[3] = Pix[0];
				}
			}

			//Create glyph and place it correctly.
			cGlyph *pGlyph = CreateGlyph(bmp, cVector2l(lXOffset,lYOffset), cVector2l(lW,lH),
										cVector2l(lBase,lLineHeight), lAdvance);

			mvGlyphs[lId] = pGlyph;
		}

		//Destroy XML
		delete pXmlDoc;
		return true;
	}

	//-----------------------------------------------------------------------

	void FontData::Draw(const cVector3f& avPos,const cVector2f& avSize, const cColor& aCol,
						eFontAlign aAlign,	const wchar_t* fmt,...)
	{
		wchar_t sText[256];
		va_list ap;
		if (fmt == NULL) return;
		va_start(ap, fmt);
		vswprintf(sText, 255, fmt, ap);
		va_end(ap);


		int lCount=0;
		float lXAdd =0;
		cVector3f vPos = avPos;

		if(aAlign == eFontAlign_Center){
			vPos.x -= GetLength(avSize, sText)/2;
		}
		else if(aAlign == eFontAlign_Right)
		{
			vPos.x -= GetLength(avSize, sText);
		}

		while(sText[lCount] != 0)
		{
			wchar_t lGlyphNum = ((wchar_t)sText[lCount]);
			if(lGlyphNum<mlFirstChar || lGlyphNum>mlLastChar){
				lCount++;
				continue;
			}
			lGlyphNum -= mlFirstChar;


			cGlyph *pGlyph = mvGlyphs[lGlyphNum];
			if(pGlyph)
			{
				cVector2f vOffset(pGlyph->mvOffset * avSize);
				cVector2f vSize(pGlyph->mvSize * avSize);

				mpGraphicsDrawer->DrawGfxObject(pGlyph->mpGfxObject,vPos + vOffset,vSize,aCol);

				vPos.x += pGlyph->mfAdvance*avSize.x;
			}
			lCount++;
		}
	}

	//-----------------------------------------------------------------------

	int FontData::DrawWordWrap(cVector3f avPos,float afLength,float afFontHeight,cVector2f avSize,const cColor& aCol,
								eFontAlign aAlign,	const tWString &asString)
	{
		int rows = 0;

		unsigned int pos;
		unsigned int first_letter=0;
		unsigned int last_space=0;

		tUIntList RowLengthList;

		float fTextLength;

		for(pos = 0; pos < asString.size();pos++)
		{
			if(asString[pos] == _W(' ') || asString[pos] == _W('\n'))
			{
				tWString temp = asString.substr(first_letter, pos-first_letter);
				fTextLength =  GetLength(avSize,temp.c_str());

				bool nothing = true;
				if(fTextLength > afLength)
				{
					rows++;
					RowLengthList.push_back(last_space);
					first_letter=last_space+1;
					last_space = pos;
					nothing = false;
				}
				if(asString[pos] == _W('\n'))
				{
					last_space = pos;
					first_letter=last_space+1;
					RowLengthList.push_back(last_space-1);
					rows++;
					nothing = false;
				}
				if(nothing)
				{
					last_space = pos;
				}
			}
		}
		tWString temp =  asString.substr(first_letter, pos-first_letter);
		fTextLength = GetLength(avSize,temp.c_str());
		if(fTextLength > afLength)
		{
			rows++;
			RowLengthList.push_back(last_space);
		}

		if(rows==0)
		{
			Draw(avPos,avSize,aCol,aAlign,_W("%ls"),asString.c_str());
		}
		else
		{
			first_letter=0;
			unsigned int i=0;

			for (unsigned int rowLen : RowLengthList)
			{
				Draw(cVector3f(avPos.x,avPos.y + i*afFontHeight,avPos.z),avSize,aCol,aAlign,
								_W("%ls"),asString.substr(first_letter, rowLen - first_letter).c_str());
				i++;
				first_letter = rowLen + 1;
			}
			Draw(cVector3f(avPos.x,avPos.y + i*afFontHeight,avPos.z),avSize,aCol,aAlign,
				_W("%ls"),asString.substr(first_letter).c_str());

		}

		return rows;
	}

	//-----------------------------------------------------------------------

	void FontData::GetWordWrapRows(float afLength,float afFontHeight,cVector2f avSize,
							const tWString& asString,tWStringVec *apRowVec)
	{
		int rows = 0;

		unsigned int pos;
		unsigned int first_letter=0;
		unsigned int last_space=0;

		tUIntList RowLengthList;

		float fTextLength;

		for(pos = 0; pos < asString.size();pos++)
		{
			//Log("char: %d\n",(char)asString[pos]);
			if(asString[pos] == _W(' ') || asString[pos] == _W('\n'))
			{
				tWString temp = asString.substr(first_letter, pos-first_letter);
				fTextLength =  GetLength(avSize,temp.c_str());

				//Log("r:%d p:%d f:%d l:%d Temp:'%s'\n",rows,pos,first_letter,last_space,
													//temp.c_str());
				bool nothing = true;
				if(fTextLength > afLength)
				{
					rows++;
					RowLengthList.push_back(last_space);
					first_letter=last_space+1;
					last_space = pos;
					nothing = false;
				}
				if(asString[pos] == _W('\n'))
				{
					last_space = pos;
					first_letter=last_space+1;
					RowLengthList.push_back(last_space);
					rows++;
					nothing = false;
				}
				if(nothing)
				{
					last_space = pos;
				}
			}
		}
		tWString temp =  asString.substr(first_letter, pos-first_letter);
		fTextLength = GetLength(avSize,temp.c_str());
		if(fTextLength > afLength)
		{
			rows++;
			RowLengthList.push_back(last_space);
		}

		if(rows==0)
		{
			apRowVec->push_back(asString.c_str());
		}
		else
		{
			first_letter = 0;
			unsigned int i = 0;

			for (unsigned int rowLen : RowLengthList)
			{
				apRowVec->push_back(asString.substr(first_letter, rowLen - first_letter).c_str());
				i++;
				first_letter = rowLen + 1;
			}
			apRowVec->push_back(asString.substr(first_letter).c_str());

		}
	}

	//-----------------------------------------------------------------------

	float FontData::GetLength(const cVector2f& avSize,const wchar_t* sText)
	{
		int lCount=0;
		float lXAdd =0;
		float fLength =0;
		while(sText[lCount] != 0)
		{
			unsigned short lGlyphNum = ((wchar_t)sText[lCount]);
			if(lGlyphNum<mlFirstChar || lGlyphNum>mlLastChar){
				lCount++;
				continue;
			}
			lGlyphNum -= mlFirstChar;


			cGlyph *pGlyph = mvGlyphs[lGlyphNum];
			if(pGlyph)
			{
				cVector2f vOffset(pGlyph->mvOffset * avSize);
				cVector2f vSize(pGlyph->mvSize * avSize);

				fLength += pGlyph->mfAdvance*avSize.x;
			}
			lCount++;
		}

		return fLength;
	}

	//-----------------------------------------------------------------------


	float FontData::GetLengthFmt(const cVector2f& avSize,const wchar_t* fmt,...)
	{
		wchar_t sText[256];
		va_list ap;
		if (fmt == NULL) return 0;
		va_start(ap, fmt);
		vswprintf(sText, 255, fmt, ap);
		va_end(ap);

		return GetLength(avSize, sText);
	}
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cGlyph* FontData::CreateGlyph(const Bitmap &aBmp, const cVector2l &avOffset,const cVector2l &avSize,
								const cVector2l& avFontSize, int alAdvance)
	{
		//Here the bitmap should be saved to diskk for faster loading.

		//////////////////////////
		//Gfx object
		const cGfxObject* pObject = mpGraphicsDrawer->CreateUnmanagedGfxObject(aBmp, eGfxMaterialType::DiffuseAlpha);

		//////////////////////////
		//Gui gfx
//		cGuiGfxElement* pGuiGfx = mpGui->CreateGfxFilledRect(cColor(1,1),eGuiMaterial_FontNormal,false);
//		cResourceImage *pImage = pObject->GetMaterial()->GetImage();
//		pImage->IncUserCount();
//		pGuiGfx->AddImage(pImage);

		//////////////////////////
		//Sizes
		cVector2f vSize;
		vSize.x = ((float)avSize.x)/((float)avFontSize.x) * mvSizeRatio.x;
		vSize.y = ((float)avSize.y)/((float)avFontSize.y) * mvSizeRatio.y;

		cVector2f vOffset;
		vOffset.x = ((float)avOffset.x)/((float)avFontSize.x);
		vOffset.y = ((float)avOffset.y)/((float)avFontSize.y);

		float fAdvance = ((float)alAdvance)/((float)avFontSize.x) * mvSizeRatio.x;

		cGlyph* pGlyph = new cGlyph(pObject,vOffset,vSize,fAdvance);

		return pGlyph;
	}

	//-----------------------------------------------------------------------

	void FontData::AddGlyph(cGlyph *apGlyph)
	{
		mvGlyphs.push_back(apGlyph);
	}



	//-----------------------------------------------------------------------

}

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
#include "graphics/LowLevelGraphics.h"

#include "resources/LoadImage.h"
#include "system/Log.h"

#include "tinyXML/tinyxml.h"

namespace hpl {


	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	FontData::FontData(const tString &name, iLowLevelGraphics *llGfx, cGraphicsDrawer *drawer)
	: iResourceBase(name)
	, _llGfx{llGfx}
	, _drawer{drawer}
	{
	}

	//-----------------------------------------------------------------------

	FontData::~FontData()
	{
		for (auto page : _pages) {
			delete page;
		}
	}


	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool FontData::LoadAngelBMFont(const tString &asFileName)
	{
		tString sPath = cString::GetFilePath(asFileName);

		////////////////////////////////////////////
		// Load xml file
		TiXmlDocument *pXmlDoc = new TiXmlDocument(asFileName.c_str());

		if(pXmlDoc->LoadFile()==false)
		{
			Error("Couldn't load angelcode BMFont file '%s'\n", asFileName.c_str());
			delete pXmlDoc;
			return false;
		}

		TiXmlElement *pRootElem = pXmlDoc->RootElement();

		////////////////////////////////////////////
		// Load Common info
		TiXmlElement *pCommonElem = pRootElem->FirstChildElement("common");

		mfLineHeight = cString::ToFloat(pCommonElem->Attribute("lineHeight"),0);
		mfBase = cString::ToInt(pCommonElem->Attribute("base"),0);
		_pagePixelWidth = cString::ToFloat(pCommonElem->Attribute("scaleW"), 32);
		_pagePixelHeight = cString::ToFloat(pCommonElem->Attribute("scaleH"), 32);

		mfBaseOverLineHeight = mfBase / mfLineHeight;

		_glyphs.reserve(500);


		////////////////////////////////////////////
		// Load bitmaps
		TiXmlElement *pPagesRootElem = pRootElem->FirstChildElement("pages");

		TiXmlElement *pPageElem = pPagesRootElem->FirstChildElement("page");
		for (int pageNumber = 0; pPageElem != NULL; pPageElem = pPageElem->NextSiblingElement("page"), pageNumber++)
		{
			tString sFileName = pPageElem->Attribute("file");
			tString sFilePath = cString::SetFilePath(sFileName,sPath);

			auto maybeBitmap = LoadBitmapFile(sFilePath);
			if (! maybeBitmap)
			{
				Error("Couldn't load bitmap %s for FNT file '%s'\n", sFilePath.c_str(), asFileName.c_str());
				delete pXmlDoc;
				return false;
			}

			auto bitmap = std::move(*maybeBitmap);
			auto bmWidth = bitmap.GetWidth();
			auto bmHeight = bitmap.GetHeight();

			// Set alpha to grayscale value of glyph pixel
			auto pixelData = bitmap.GetRawData<uint8_t>();
			for (int y=0; y < bmHeight; y++) {
				for (int x=0; x < bmWidth; x++) {
					pixelData[3] = pixelData[0];
					pixelData += 4;
				}
			}

			// create page texture and add to the list
			auto pageTexture = _llGfx->CreateTexture(GetName() + "_page" + std::to_string(pageNumber), eTextureTarget_2D);
			pageTexture->CreateFromBitmap(bitmap);
			_pages.push_back(pageTexture);
		}

		////////////////////////////////////////////
		// Load glyphs
		TiXmlElement *pCharsRootElem = pRootElem->FirstChildElement("chars");

		TiXmlElement *pCharElem = pCharsRootElem->FirstChildElement("char");
		for(; pCharElem != NULL; pCharElem = pCharElem->NextSiblingElement("char"))
		{
			//Get the info on the character
			int lId = cString::ToInt(pCharElem->Attribute("id"),0);
			float fX = cString::ToInt(pCharElem->Attribute("x"),0);
			float fY = cString::ToInt(pCharElem->Attribute("y"),0);

			float fW = cString::ToInt(pCharElem->Attribute("width"),0);
			float fH = cString::ToInt(pCharElem->Attribute("height"),0);

			float fXOffset = cString::ToInt(pCharElem->Attribute("xoffset"),0);
			float fYOffset = cString::ToInt(pCharElem->Attribute("yoffset"),0);

			float fAdvance = cString::ToInt(pCharElem->Attribute("xadvance"),0);

			int lPage = cString::ToInt(pCharElem->Attribute("page"),0);

			//Create glyph and place it correctly.
			cVector2f vSize {
				fW / mfBase * mfBaseOverLineHeight,
				fH / mfLineHeight
			};
			cVector2f vOffset {
				fXOffset / mfBase,
				fYOffset / mfLineHeight
			};
			float unitAdvance = fAdvance / mfBase * mfBaseOverLineHeight;

			float uvLeft = fX / _pagePixelWidth;
			float uvTop = fY / _pagePixelHeight;
			float uvRight = (fX + fW) / _pagePixelWidth;
			float uvBottom = (fY + fH) / _pagePixelHeight;

			_glyphs.insert({
				lId,
				{
					.page = lPage,
					.offset = vOffset,
					.size = vSize,
					.xAdvance = unitAdvance,
					.uvs = {
						.uv0 { uvLeft, uvTop },
						.uv1 { uvRight, uvTop },
						.uv2 { uvRight, uvBottom },
						.uv3 { uvLeft, uvBottom }
					}
				}
			});
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
			wchar_t codePoint = sText[lCount];

			if (_glyphs.find(codePoint) != _glyphs.end()) {
				auto &glyph = _glyphs[codePoint];
				cVector2f vOffset(glyph.offset * avSize);
				cVector2f vSize(glyph.size * avSize);

				_drawer->Draw({
					.texture = _pages[glyph.page],
					.mvPosition = vPos + vOffset,
					.mvSize = vSize,
					.mColor = aCol,
					.uvs = glyph.uvs
				});

				vPos.x += glyph.xAdvance * avSize.x;
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

		std::vector<unsigned int> RowLengthList;

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

		std::vector<unsigned int> RowLengthList;

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

	float FontData::GetLength(const cVector2f& avSize, const wchar_t* sText)
	{
		int lCount=0;
		float lXAdd =0;
		float fLength =0;
		while(sText[lCount] != 0)
		{
			unsigned short codePoint = sText[lCount];

			if (_glyphs.find(codePoint) != _glyphs.end())
			{
				auto& glyph = _glyphs[codePoint];
				cVector2f vOffset(glyph.offset * avSize);
				cVector2f vSize(glyph.size * avSize);

				fLength += glyph.xAdvance * avSize.x;
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

	//-----------------------------------------------------------------------

}

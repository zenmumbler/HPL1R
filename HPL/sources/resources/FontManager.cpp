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
#include "resources/FontManager.h"
#include "system/String.h"
#include "resources/Resources.h"
#include "graphics/Graphics.h"
#include "graphics/LowLevelGraphics.h"
#include "resources/ImageManager.h"
#include "graphics/FontData.h"
#include "system/Log.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cFontManager::cFontManager(cGraphics* apGraphics,cResources *apResources)
		: iResourceManager(apResources->GetFileSearcher())
	{
		mpGraphics = apGraphics;
		mpResources = apResources;
	}

	cFontManager::~cFontManager()
	{
		DestroyAll();
		Log(" Done with fonts\n");
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	FontData* cFontManager::CreateFontData(const tString& asName, int alSize,unsigned short alFirstChar,
											unsigned short alLastChar)
	{
		tString sPath;
		FontData* pFont;
		tString asNewName = cString::ToLowerCase(asName);

		BeginLoad(asName);

		pFont = static_cast<FontData*>(this->FindLoadedResource(asNewName,sPath));

		if(pFont==NULL && sPath!="")
		{
			pFont = new FontData(asNewName);
			pFont->SetUp(mpGraphics->GetDrawer());

			tString sExt = cString::ToLowerCase(cString::GetFileExt(asName));

			//Angel code font type
			if(sExt == "fnt")
			{
				if(pFont->CreateFromBitmapFile(sPath)==false){
					delete pFont;
					EndLoad();
					return NULL;
				}
			}
			else
			{
				Error("Font '%s' has an unkown extension!\n",asName.c_str());
				delete pFont;
				EndLoad();
				return NULL;
			}

			AddResource(pFont);
		}

		if(pFont)pFont->IncUserCount();
		else Error("Couldn't create font '%s'\n",asNewName.c_str());

		EndLoad();
		return pFont;
	}

	//-----------------------------------------------------------------------

	iResourceBase* cFontManager::Create(const tString& asName)
	{
		return CreateFontData(asName, 16, 32, 255);
	}

	//-----------------------------------------------------------------------

	void cFontManager::Destroy(iResourceBase* apResource)
	{
		apResource->DecUserCount();

		if(apResource->HasUsers()==false){
			RemoveResource(apResource);
			delete apResource;
		}
	}

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------


	//-----------------------------------------------------------------------
}

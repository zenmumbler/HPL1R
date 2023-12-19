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
#ifndef HPL_SDL_TEXTURE_H
#define HPL_SDL_TEXTURE_H

#include "graphics/Texture.h"

#if defined(__APPLE__)&&defined(__MACH__)
#include <OpenGL/gltypes.h>
#else
#include <GL/gltypes.h>
#endif

namespace hpl {

	class cSDLTexture : public iTexture
	{
	public:
		cSDLTexture(const tString &asName, eTextureTarget aTarget);
		~cSDLTexture();

		bool CreateFromBitmap(const Bitmap &bmp) override;

		bool CreateAnimFromBitmapVec(const std::vector<Bitmap>& bitmaps) override;
		bool CreateCubeFromBitmapVec(const std::vector<Bitmap>& bitmaps) override;

		bool CreateFromArray(unsigned char *apPixelData, int alChannels, const cVector3l &avSize) override;

		void SetPixels2D(	int alLevel, const cVector2l& avOffset, const cVector2l& avSize,
							eColorDataFormat aDataFormat, void *apPixelData) override;


		int GetHandle() override {return (int) mvTextureHandles[0];}

		void SetFilter(eTextureFilter aFilter) override;
		void SetAnisotropyDegree(float afX) override;

		void SetWrapAll(eTextureWrap aMode) override;
		void SetWrapS(eTextureWrap aMode) override;
		void SetWrapT(eTextureWrap aMode) override;
		void SetWrapR(eTextureWrap aMode) override;

		void Update(float afTimeStep) override;

		unsigned int GetCurrentLowlevelHandle() override;

	private:
		bool CreateFromBitmapToHandle(const Bitmap &bmp, int alHandleIdx);

		GLenum InitCreation(int alHandleIdx);
		void PostCreation(GLenum aGLTarget);

		void GetSettings(const Bitmap &bmp, int &channels, GLint &internalFormat, GLenum &format);

		tUIntVec mvTextureHandles;
		bool mbContainsData;

		float mfTimeCount;
		int mlTextureIndex;
		float mfTimeDir;
	};

};
#endif // HPL_SDL_TEXTURE_H

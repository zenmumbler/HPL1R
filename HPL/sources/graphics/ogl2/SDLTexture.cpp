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
#if defined(__APPLE__)&&defined(__MACH__)
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/gl3.h>
#endif

#include "graphics/Bitmap.h"
#include "graphics/Image.h"
#include "graphics/ogl2/SDLTexture.h"
#include "graphics/impl/LowLevelGraphicsSDL.h"

#include "math/Math.h"
#include "system/Log.h"

namespace hpl {

	//-----------------------------------------------------------------------

	static GLenum GetGLWrap(eTextureWrap aMode)
	{
		switch(aMode)
		{
			case eTextureWrap_ClampToEdge: return GL_CLAMP_TO_EDGE;
			case eTextureWrap_MirrorClampToEdge: return GL_MIRROR_CLAMP_TO_EDGE_EXT;
			case eTextureWrap_Repeat: return GL_REPEAT;
			case eTextureWrap_MirrorRepeat: return GL_MIRRORED_REPEAT;
			case eTextureWrap_ClampToBorder: return GL_CLAMP_TO_BORDER;
			default: return GL_REPEAT;
		}
	}

	struct GLPixelFormat {
		GLenum format;
		GLenum type; // GL_NONE for compressed formats
	};

	static GLPixelFormat PixelFormatToGL(PixelFormat format) {
		switch (format) {
			case PixelFormat::None: return { GL_NONE, GL_NONE };

			case PixelFormat::R8: return { GL_RED, GL_UNSIGNED_BYTE };
			case PixelFormat::RG8: return { GL_RG, GL_UNSIGNED_BYTE };
			case PixelFormat::RGB8: return { GL_RGB, GL_UNSIGNED_BYTE };
			case PixelFormat::RGBA8: return { GL_RGBA, GL_UNSIGNED_BYTE };

			case PixelFormat::SRGB8: return { GL_SRGB, GL_UNSIGNED_BYTE };
			case PixelFormat::SRGBA8: return { GL_SRGB_ALPHA, GL_UNSIGNED_BYTE };

			case PixelFormat::BGR8: return { GL_BGR, GL_UNSIGNED_BYTE };
			case PixelFormat::BGRA8: return { GL_BGRA, GL_UNSIGNED_BYTE };

			case PixelFormat::BC1_RGB: return { GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_NONE };
			case PixelFormat::BC1_RGBA: return { GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_NONE };
			case PixelFormat::BC3_RGBA: return { GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_NONE };
			case PixelFormat::BC5_RGBA: return { GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_NONE };

			case PixelFormat::BC1_SRGB: return { GL_COMPRESSED_SRGB_S3TC_DXT1_EXT, GL_NONE };
			case PixelFormat::BC1_SRGBA: return { GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, GL_NONE };
			case PixelFormat::BC3_SRGBA: return { GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, GL_NONE };
			case PixelFormat::BC5_SRGBA: return { GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, GL_NONE };
		}
		// Warning("Unknown pixelformat %d", format);
		return { GL_NONE, GL_NONE };
	}

	static GLenum PixelFormatToInternalGL(PixelFormat pf) {
		switch (pf) {
			case PixelFormat::None: return GL_NONE;

			case PixelFormat::R8: return GL_R8;
			case PixelFormat::RG8: return GL_RG8;
			case PixelFormat::RGB8: return GL_RGB8;
			case PixelFormat::RGBA8: return GL_RGBA8;

			case PixelFormat::SRGB8: return GL_SRGB8;
			case PixelFormat::SRGBA8: return GL_SRGB8_ALPHA8;

			case PixelFormat::BGR8: return GL_RGB8;   // BGR formats are converted to RGB internally
			case PixelFormat::BGRA8: return GL_RGBA8;

			// compressed formats do not use this method
			case PixelFormat::BC1_RGB:
			case PixelFormat::BC1_RGBA:
			case PixelFormat::BC3_RGBA:
			case PixelFormat::BC5_RGBA:

			case PixelFormat::BC1_SRGB:
			case PixelFormat::BC1_SRGBA:
			case PixelFormat::BC3_SRGBA:
			case PixelFormat::BC5_SRGBA:
				return GL_NONE;
				break;
		}
		// Warning("Unknown pixelformat %d", format);
		return GL_NONE;
	}

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cSDLTexture::cSDLTexture(const tString &asName, eTextureTarget aTarget)
	: iTexture(asName, aTarget)
	{
		mbContainsData = false;

		mlTextureIndex = 0;
		mfTimeCount = 0;
		mfTimeDir = 1;
	}

	cSDLTexture::~cSDLTexture()
	{
		glDeleteTextures((int)mvTextureHandles.size(), mvTextureHandles.data());
	}


	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cSDLTexture::CreateFromImage(const Image *image) {
		// TODO: method incomplete, proto impl
		GLenum target = InitCreation(0);

		mlWidth = image->GetWidth();
		mlHeight = image->GetHeight();
		auto pixels = image->GetData();

		auto format = image->GetPixelFormat();
		auto [glFormat, glType] = PixelFormatToGL(format);

		// Clear error flags
		glGetError();

		if (glType == GL_NONE) {
			// compressed
		}
		else {
			// uncompressed
			auto internalFormat = PixelFormatToInternalGL(format);
			if (mTarget == eTextureTarget_1D)
				glTexImage1D(target, 0, internalFormat, mlWidth, 0, glFormat, glType, pixels);
			else
				glTexImage2D(target, 0, internalFormat, mlWidth, mlHeight, 0, glFormat, glType, pixels);
		}

		auto texErr = glGetError();
		if (texErr != GL_NO_ERROR) {
			Error("Could not upload texture to GL! Err: %d", texErr);
			return false;
		}

		PostCreation(target);

		return true;

	}

	//-----------------------------------------------------------------------

	bool cSDLTexture::CreateFromBitmap(const Bitmap &bmp)
	{
		//Generate handles
		if (mvTextureHandles.empty())
		{
			mvTextureHandles.resize(1);
			glGenTextures(1, mvTextureHandles.data());
		}

		return CreateFromBitmapToHandle(bmp, 0);
	}

	//-----------------------------------------------------------------------

	bool cSDLTexture::CreateAnimFromBitmapVec(const std::vector<Bitmap>& bitmaps)
	{
		int count = static_cast<int>(bitmaps.size());
		mvTextureHandles.resize(count);
		glGenTextures(count, mvTextureHandles.data());

		for (int i=0; i<count; ++i)
		{
			if (CreateFromBitmapToHandle(bitmaps[i], i) == false)
			{
				return false;
			}
		}

		return true;
	}

	//-----------------------------------------------------------------------

	bool cSDLTexture::CreateCubeFromBitmapVec(const std::vector<Bitmap>& bitmaps)
	{
		if (mTarget != eTextureTarget_CubeMap)
		{
			return false;
		}

		if (bitmaps.size() < 6) {
			Error("Only %d bitmaps supplied for creation of cube map, 6 needed.", bitmaps.size());
			return false;
		}

		//Generate handles
		if(mvTextureHandles.empty())
		{
			mvTextureHandles.resize(1);
			glGenTextures(1,(GLuint *)&mvTextureHandles[0]);
		}
		else
		{
			glDeleteTextures(1,(GLuint *)&mvTextureHandles[0]);
			glGenTextures(1,(GLuint *)&mvTextureHandles[0]);
		}

		GLenum GLTarget = InitCreation(0);

		//Create the cube map sides
		for(int i=0; i< 6; i++)
		{
			const Bitmap &bmp = bitmaps[i];

			GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;

			int channels;
			GLint internalFormat;
			GLenum format;
			GetSettings(bmp, channels, internalFormat, format);

			glTexImage2D(target, 0, internalFormat, bmp.GetWidth(), bmp.GetHeight(), 0, format, GL_UNSIGNED_BYTE, bmp.GetRawData());

			mlWidth = bmp.GetWidth();
			mlHeight = bmp.GetHeight();

			if(!cMath::IsPow2(mlHeight) || !cMath::IsPow2(mlWidth))
			{
				Warning("Texture '%s' does not have a pow2 size!\n",msName.c_str());
			}
		}

		PostCreation(GLTarget);

		return true;
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::Update(float afTimeStep)
	{
		if(mvTextureHandles.size() > 1)
		{
			float fMax = (float)(mvTextureHandles.size());
			mfTimeCount += afTimeStep * (1.0f/mfFrameTime) * mfTimeDir;

			if(mfTimeDir > 0)
			{
				if(mfTimeCount >= fMax)
				{
					if(mAnimMode == eTextureAnimMode_Loop)
					{
						mfTimeCount =0;
					}
					else
					{
						mfTimeCount = fMax - 1.0f;
						mfTimeDir = -1.0f;
					}
				}
			}
			else
			{
				if(mfTimeCount < 0)
				{
					mfTimeCount =1;
					mfTimeDir = 1.0f;
				}
			}
		}
	}

	//-----------------------------------------------------------------------

	unsigned int cSDLTexture::GetCurrentLowlevelHandle()
	{
		if(mvTextureHandles.size() > 1)
		{
			int lFrame = (int) mfTimeCount;
			return mvTextureHandles[lFrame];
		}
		else
		{
			return mvTextureHandles[0];
		}
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::SetFilter(eTextureFilter aFilter)
	{
		if(mFilter == aFilter) return;

		mFilter = aFilter;
		if(mbContainsData)
		{
			GLenum GLTarget = TextureTargetToGL(mTarget);

			for(size_t i=0; i < mvTextureHandles.size(); ++i)
			{
				glBindTexture(GLTarget, mvTextureHandles[i]);

				if(UsesMipMaps()) {
					if(mFilter == eTextureFilter_Bilinear)
						glTexParameteri(GLTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
					else
						glTexParameteri(GLTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				}
				else{
					glTexParameteri(GLTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				}
			}
		}
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::SetAnisotropyDegree(float afX)
	{
		if(mfAnisotropyDegree == afX) return;
		mfAnisotropyDegree = afX;

		GLenum GLTarget = TextureTargetToGL(mTarget);

		for(size_t i=0; i < mvTextureHandles.size(); ++i)
		{
			glBindTexture(GLTarget, mvTextureHandles[i]);
			glTexParameterf(GLTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, mfAnisotropyDegree);
		}
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::SetWrapAll(eTextureWrap aMode)
	{
		if(mbContainsData)
		{
			GLenum GLTarget = TextureTargetToGL(mTarget);
			auto glWrap = GetGLWrap(aMode);

			for(size_t i=0; i < mvTextureHandles.size(); ++i)
			{
				glBindTexture(GLTarget, mvTextureHandles[i]);
				glTexParameteri(GLTarget, GL_TEXTURE_WRAP_S, glWrap);
				glTexParameteri(GLTarget, GL_TEXTURE_WRAP_T, glWrap);
				glTexParameteri(GLTarget, GL_TEXTURE_WRAP_R, glWrap);
			}
		}
	}

	void cSDLTexture::SetWrapS(eTextureWrap aMode)
	{
		if(mbContainsData)
		{
			GLenum GLTarget = TextureTargetToGL(mTarget);

			for(size_t i=0; i < mvTextureHandles.size(); ++i)
			{
				glBindTexture(GLTarget, mvTextureHandles[i]);
				glTexParameteri(GLTarget, GL_TEXTURE_WRAP_S, GetGLWrap(aMode));
			}
		}
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::SetWrapT(eTextureWrap aMode)
	{
		if(mbContainsData)
		{
			GLenum GLTarget = TextureTargetToGL(mTarget);

			for(size_t i=0; i < mvTextureHandles.size(); ++i)
			{
				glBindTexture(GLTarget, mvTextureHandles[i]);
				glTexParameteri(GLTarget, GL_TEXTURE_WRAP_T, GetGLWrap(aMode));
			}
		}
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::SetWrapR(eTextureWrap aMode)
	{
		if(mbContainsData)
		{
			GLenum GLTarget = TextureTargetToGL(mTarget);

			for(size_t i=0; i < mvTextureHandles.size(); ++i)
			{
				glBindTexture(GLTarget, mvTextureHandles[i]);
				glTexParameteri(GLTarget, GL_TEXTURE_WRAP_R, GetGLWrap(aMode));
			}
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cSDLTexture::CreateFromBitmapToHandle(const Bitmap &bmp, int alHandleIdx)
	{
		GLenum GLTarget = InitCreation(alHandleIdx);

		mlWidth = bmp.GetWidth();
		mlHeight = bmp.GetHeight();

		if (!cMath::IsPow2(mlHeight) || !cMath::IsPow2(mlWidth))
		{
			Warning("Texture '%s' does not have a pow2 size!\n",msName.c_str());
		}

		int channels;
		GLint internalFormat;
		GLenum format;
		GetSettings(bmp, channels, internalFormat, format);

		auto pPixelSrc = bmp.GetRawData<unsigned char>();

		//Log("Loading %s  %d x %d\n",msName.c_str(), pSrc->GetWidth(), pSrc->GetHeight());
		//Log("Channels: %d Format: %x\n",lChannels, format);

		//Clear error flags
		while(glGetError()!=GL_NO_ERROR);

		if (mTarget == eTextureTarget_1D)
			glTexImage1D(GLTarget, 0, internalFormat, mlWidth, 0, format, GL_UNSIGNED_BYTE, pPixelSrc);
		else
			glTexImage2D(GLTarget, 0, internalFormat, mlWidth, mlHeight, 0, format, GL_UNSIGNED_BYTE, pPixelSrc);

		auto texErr = glGetError();
		if(texErr != GL_NO_ERROR) {
			Error("Could not upload texture to GL! Err: %d", texErr);
			return false;
		}

		PostCreation(GLTarget);

		return true;
	}

	//-----------------------------------------------------------------------

	GLenum cSDLTexture::InitCreation(int alHandleIdx)
	{
		GLenum GLTarget = TextureTargetToGL(mTarget);
		glBindTexture(GLTarget, mvTextureHandles[alHandleIdx]);
		return GLTarget;
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::PostCreation(GLenum aGLTarget)
	{
		if(UsesMipMaps())
		{
			if(mFilter == eTextureFilter_Bilinear)
				glTexParameteri(aGLTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			else
				glTexParameteri(aGLTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		else{
			glTexParameteri(aGLTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		glTexParameteri(aGLTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(aGLTarget,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(aGLTarget,GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexParameteri(aGLTarget,GL_TEXTURE_WRAP_R,GL_REPEAT);

		mbContainsData = true;
	}

	//-----------------------------------------------------------------------

	void cSDLTexture::GetSettings(const Bitmap &bmp, int &channels, GLint &internalFormat, GLenum &format)
	{
		channels = bmp.GetNumChannels();

		if(channels==4)
		{
			internalFormat = GL_RGBA8;
			format = GL_BGRA;
		}
		else if(channels==3)
		{
			internalFormat = GL_RGB8;
			format = GL_BGR;
		}
		else if(channels==1)
		{
			internalFormat = GL_R8;
			format = GL_RED;
		}
	}

	//-----------------------------------------------------------------------

}

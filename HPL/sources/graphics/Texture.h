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
#ifndef HPL_TEXTURE_H
#define HPL_TEXTURE_H

#include <vector>
#include "graphics/Bitmap.h"
#include "graphics/GraphicsTypes.h"
#include "resources/ResourceBase.h"

namespace hpl {

	enum eTextureTarget
	{
		eTextureTarget_1D,
		eTextureTarget_2D,
		eTextureTarget_CubeMap,
		eTextureTarget_3D
	};

	//-----------------------------------------

	enum eTextureWrap
	{
		eTextureWrap_Repeat,
		eTextureWrap_Clamp,
		eTextureWrap_ClampToEdge,
		eTextureWrap_ClampToBorder
	};

	//-----------------------------------------

	enum eTextureFilter
	{
		eTextureFilter_Bilinear,
		eTextureFilter_Trilinear
	};

	//-----------------------------------------

	enum eTextureAnimMode
	{
		eTextureAnimMode_None,
		eTextureAnimMode_Loop,
		eTextureAnimMode_Oscillate
	};

	//-----------------------------------------

	class iLowLevelGraphics;

	class iTexture : public iResourceBase
	{
	public:
		iTexture(tString asName, eTextureTarget aTarget)
			: iResourceBase(asName,0),
				mlWidth(0), mlHeight(0), mlDepth(1),
				mTarget(aTarget),
				mWrapS(eTextureWrap_Repeat), mWrapT(eTextureWrap_Repeat),mWrapR(eTextureWrap_Repeat),
				mfFrameTime(1), mAnimMode(eTextureAnimMode_Loop), mlSizeLevel(0), mvMinLevelSize(16,16),
				mfAnisotropyDegree(1.0f),mFilter(eTextureFilter_Bilinear){}

		virtual ~iTexture(){}

		void SetSizeLevel(unsigned int alLevel){mlSizeLevel = alLevel;}
		void SetMinLevelSize(const cVector2l& avSize){ mvMinLevelSize = avSize;}

		void Destroy(){}

		/**
		 * Create a texture from a bitmap, work only for 1D, 2D and Rect targets. Doesn't work with render targets.
		 * \param pBmp
		 * \return
		 */
		virtual bool CreateFromBitmap(const Bitmap &bmp)=0;
		/**
		 * Create a cube map texture from a vector of bitmaps. Doesn't work with render targets.
		 * All bitmaps most be square, a power of 2 and the same same. The order must be: posX, negX, posY, negY, posZ and negZ.
		 * \param *avBitmaps a vector with at least 6 bitmaps
		 * \return
		 */
		virtual bool CreateCubeFromBitmapVec(const std::vector<Bitmap>& bitmaps)=0;
		virtual bool CreateAnimFromBitmapVec(const std::vector<Bitmap>& bitmaps)=0;
		virtual bool CreateFromArray(unsigned char *apPixelData, int alChannels, const cVector3l &avSize)=0;

		virtual void Update(float afTimeStep)=0;

		virtual void SetPixels2D(int alLevel, const cVector2l& avOffset, const cVector2l& avSize,
								eColorDataFormat aDataFormat, void *apPixelData)=0;

		virtual void SetFilter(eTextureFilter aFilter)=0;
		virtual void SetAnisotropyDegree(float afX)=0;
		eTextureFilter GetFilter(){ return mFilter;}
		float GetAnisotropyDegree(float afX){ return mfAnisotropyDegree;}

		virtual float GetGamma()=0;
		virtual void SetGamma(float afGamma)=0;
		virtual int GetHandle()=0;

		virtual bool HasAlpha(){return false;}

		virtual void SetWrapS(eTextureWrap aMode)=0;
		virtual void SetWrapT(eTextureWrap aMode)=0;
		virtual void SetWrapR(eTextureWrap aMode)=0;

		eTextureWrap GetWrapS(){ return mWrapS;}
		eTextureWrap GetWrapT(){ return mWrapT;}
		eTextureWrap GetWrapR(){ return mWrapR;}

		void SetFrameTime(float afX){ mfFrameTime = afX;}
		float GetFrameTime(){ return mfFrameTime;}

		eTextureAnimMode GetAnimMode() { return mAnimMode;}
		void SetAnimMode(eTextureAnimMode aMode) {mAnimMode = aMode;};

		bool UsesMipMaps(){ return false; }
		eTextureTarget GetTarget(){ return mTarget;}

		virtual bool HasAnimation()=0;
		virtual void NextFrame()=0;
		virtual void PrevFrame()=0;
		virtual float GetT()=0;
		virtual float GetTimeCount()=0;
		virtual void SetTimeCount(float afX)=0;
		virtual int GetCurrentLowlevelHandle()=0;

		int GetWidth() const { return mlWidth; }
		int GetHeight() const { return mlHeight; }
		int GetDepth() const { return mlDepth; }

	protected:
		int mlWidth;
		int mlHeight;
		int mlDepth;
		int mlBpp;

		eTextureTarget mTarget;
		eTextureWrap mWrapS;
		eTextureWrap mWrapT;
		eTextureWrap mWrapR;
		eTextureFilter mFilter;
		float mfAnisotropyDegree;

		float mfFrameTime;
		eTextureAnimMode mAnimMode;
		unsigned int mlSizeLevel;
		cVector2l mvMinLevelSize;
	};

	typedef std::vector<iTexture*> tTextureVec;
	typedef tTextureVec::iterator tTextureVecIt;
};
#endif // HPL_TEXTURE_H

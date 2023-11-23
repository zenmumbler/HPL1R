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
#ifndef HPL_RENDER_SET_H
#define HPL_RENDER_SET_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"
#include "graphics/Material.h"

namespace hpl {

	class cRenderSettings;
	class iRenderable;
	class iLight3D;
	class cSector;

	enum eRenderStateType
	{
		eRenderStateType_Sector = 0,		// uniform data (ambientColor)

		eRenderStateType_AlphaMode = 4,		// uniform data or shader variant
		eRenderStateType_BlendMode = 5,		// colorState
		eRenderStateType_GPUProgram = 6,	// pipeline
		eRenderStateType_Texture = 8,		// binding
		eRenderStateType_VertexBuffer = 9,	// render
		eRenderStateType_Matrix = 10,		// uniform data
		eRenderStateType_Render = 11,		// --redundant (follows after vertexbuffer)
		eRenderStateType_LastEnum = 12
	};
	///////////// INTERFACE ////////////////////

	class iRenderState
	{
	public:
		iRenderState(){}

		int Compare(const iRenderState* apState) const;
		void SetMode(cRenderSettings* apSettings);

		void Set(const iRenderState* apState);

		eRenderStateType mType;

		//Properties:
		//Sector
		cSector *mpSector;

		//Alpha
		eMaterialAlphaMode mAlphaMode;

		//Blend
		eMaterialBlendMode mBlendMode;
		eMaterialChannelMode mChannelMode;

		//Vertex program
		iGpuProgram *mpProgram;
		iMaterialProgramSetup* mpProgramSetup;

		//Texture
		iTexture* mpTexture[MAX_TEXTUREUNITS];

		//Vertex buffer
		iVertexBuffer *mpVtxBuffer;

		//Matrix
		cMatrixf modelMatrix;

		//Render
		// iRenderable *mpObject;

	private:
		void SetSectorMode(cRenderSettings* apSettings);
		void SetBlendMode(cRenderSettings* apSettings);
		void SetAlphaMode(cRenderSettings* apSettings);
		void SetGPUProgMode(cRenderSettings* apSettings);
		void SetTextureMode(cRenderSettings* apSettings);
		void SetVtxBuffMode(cRenderSettings* apSettings);
		void SetMatrixMode(cRenderSettings* apSettings);
		void SetRenderMode(cRenderSettings* apSettings);
	};
};

#endif // HPL_RENDER_SET_H

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
#include "graphics/RenderState.h"
#include "graphics/Renderer3D.h"

#include "graphics/Material.h"
#include "graphics/VertexBuffer.h"
#include "graphics/GPUProgram.h"
#include "graphics/LowLevelGraphics.h"
#include "scene/Light3D.h"
#include "scene/Light3DSpot.h"
#include "scene/Camera.h"
#include "scene/PortalContainer.h"
#include "math/Math.h"
#include "system/Log.h"


namespace hpl {

	template<class T>
	static int GetCompareVal(T a, T b)
	{
		if(a == b) return 0;
		else return a < b ? -1 : 1;
	}

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	int iRenderState::Compare(const iRenderState* apState)  const
	{
		switch(mType)
		{
		case eRenderStateType_Sector:				return CompareSector(apState);
		case eRenderStateType_Depth:				return CompareDepth(apState);
		case eRenderStateType_AlphaMode:			return CompareAlpha(apState);
		case eRenderStateType_BlendMode:			return CompareBlend(apState);
		case eRenderStateType_GPUProgram:		return CompareVtxProg(apState);
		case eRenderStateType_Texture:			return CompareTexture(apState);
		case eRenderStateType_VertexBuffer:		return CompareVtxBuff(apState);
		case eRenderStateType_Matrix:			return CompareMatrix(apState);
		case eRenderStateType_Render:			return CompareRender(apState);
		default: return 0;
		}
	}

	//-----------------------------------------------------------------------

	void iRenderState::SetMode(cRenderSettings* apSettings)
	{
		switch(mType)
		{
		case eRenderStateType_Sector:				SetSectorMode(apSettings); break;
		case eRenderStateType_Depth:				SetDepthMode(apSettings); break;
		case eRenderStateType_AlphaMode:			SetAlphaMode(apSettings); break;
		case eRenderStateType_BlendMode:			SetBlendMode(apSettings); break;
		case eRenderStateType_GPUProgram:		SetGPUProgMode(apSettings); break;
		case eRenderStateType_Texture:			SetTextureMode(apSettings); break;
		case eRenderStateType_VertexBuffer:		SetVtxBuffMode(apSettings); break;
		case eRenderStateType_Matrix:			SetMatrixMode(apSettings); break;
		case eRenderStateType_Render:			SetRenderMode(apSettings); break;
		default: break;
		}
	}

	//-----------------------------------------------------------------------

	void iRenderState::Set(const iRenderState* apState)
	{
		mType = apState->mType;
		switch(mType)
		{
		case eRenderStateType_Sector:			mpSector = apState->mpSector;
												break;
		case eRenderStateType_Depth:			mfZ = apState->mfZ;
												break;

		case eRenderStateType_AlphaMode:		mAlphaMode = apState->mAlphaMode;
												break;

		case eRenderStateType_BlendMode:		mBlendMode = apState->mBlendMode;
												mChannelMode = apState->mChannelMode;
												break;

		case eRenderStateType_GPUProgram:		mpProgram = apState->mpProgram;
												mpProgramSetup = apState->mpProgramSetup;
												break;

		case eRenderStateType_Texture:			for(int i=0;i<MAX_TEXTUREUNITS;i++)
													mpTexture[i] = apState->mpTexture[i];
												break;

		case eRenderStateType_VertexBuffer:		mpVtxBuffer = apState->mpVtxBuffer;
												break;

		case eRenderStateType_Matrix:			mpModelMatrix = apState->mpModelMatrix;
												mpInvModelMatrix = apState->mpInvModelMatrix;
												mvScale = apState->mvScale;
												break;

		case eRenderStateType_Render:			mpObject = apState->mpObject;
												break;
		default: break;
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void iRenderState::SetSectorMode(cRenderSettings* apSettings)
	{
		if(apSettings->mbLog) Log("Sector: %d\n",mpSector);
		apSettings->mpSector = mpSector;
	}

	//-----------------------------------------------------------------------

	void iRenderState::SetDepthTestMode(cRenderSettings* apSettings)
	{
		if(apSettings->mbDepthTest != mbDepthTest)
		{
			apSettings->mpLowLevel->SetDepthTestActive(mbDepthTest);
			apSettings->mbDepthTest = mbDepthTest;

			if(apSettings->mbLog) Log("Setting depth test: %d\n",mbDepthTest?1:0);
		}
	}
	//-----------------------------------------------------------------------

	void iRenderState::SetDepthMode(cRenderSettings* apSettings)
	{
	}

	//-----------------------------------------------------------------------

	void iRenderState::SetAlphaMode(cRenderSettings* apSettings)
	{
		if(mAlphaMode != apSettings->mAlphaMode)
		{
			if(apSettings->mbLog) Log("Setting alpha: ");
			apSettings->mAlphaMode = mAlphaMode;

			if(apSettings->mbLog)Log((mAlphaMode == eMaterialAlphaMode_Solid) ? "Solid\n" : "Trans\n");
		}
	}

	//-----------------------------------------------------------------------

	void iRenderState::SetBlendMode(cRenderSettings* apSettings)
	{
		if(mBlendMode != apSettings->mBlendMode)
		{
			if(apSettings->mbLog)Log("Setting blend mode: ");
			apSettings->mBlendMode = mBlendMode;

			if(mBlendMode == eMaterialBlendMode_None)
			{
				apSettings->mpLowLevel->SetBlendActive(false);
				if(apSettings->mbLog)Log("None");
			}
			else
			{
				apSettings->mpLowLevel->SetBlendActive(true);

				switch(mBlendMode)
				{
				case eMaterialBlendMode_Add:
					apSettings->mpLowLevel->SetBlendFunc(eBlendFunc_One,eBlendFunc_One);
					if(apSettings->mbLog)Log("Add");
					break;
				case eMaterialBlendMode_Replace:
					apSettings->mpLowLevel->SetBlendFunc(eBlendFunc_One,eBlendFunc_Zero);
					if(apSettings->mbLog)Log("Replace");
					break;
				case eMaterialBlendMode_Mul:
					apSettings->mpLowLevel->SetBlendFunc(eBlendFunc_Zero,eBlendFunc_SrcColor);
					if(apSettings->mbLog)Log("Mul");
					break;
				case eMaterialBlendMode_MulX2:
					apSettings->mpLowLevel->SetBlendFunc(eBlendFunc_DestColor,eBlendFunc_SrcColor);
					if(apSettings->mbLog)Log("MulX2");
					break;
				case eMaterialBlendMode_Alpha:
					apSettings->mpLowLevel->SetBlendFunc(eBlendFunc_SrcAlpha,eBlendFunc_OneMinusSrcAlpha);
					if(apSettings->mbLog)Log("Alpha");
					break;
				case eMaterialBlendMode_DestAlphaAdd:
					apSettings->mpLowLevel->SetBlendFunc(eBlendFunc_DestAlpha,eBlendFunc_One);
					if(apSettings->mbLog)Log("DestAlphaAdd");
					break;
				default:
					if(apSettings->mbLog)Log("Invalid Blend Mode !");
					break;
				}
			}

			if(apSettings->mbLog)Log("\n");
		}

		if(mChannelMode != apSettings->mChannelMode)
		{
			if(apSettings->mbLog)Log("Setting channel: ");
			apSettings->mChannelMode = mChannelMode;

			switch(mChannelMode)
			{
			case eMaterialChannelMode_RGBA:
				apSettings->mpLowLevel->SetColorWriteActive(true, true, true, true);
				if(apSettings->mbLog)Log("RGBA");
				break;
			case eMaterialChannelMode_Z:
				apSettings->mpLowLevel->SetColorWriteActive(false, false, false, false);
				if(apSettings->mbLog)Log("Z");
				break;
			default:
				if(apSettings->mbLog)Log("Invalid colour channel mode!");
				break;
			}

			if(apSettings->mbLog)Log("\n");
		}
	}

	//-----------------------------------------------------------------------

	void iRenderState::SetGPUProgMode(cRenderSettings* apSettings)
	{
		if(mpProgram != apSettings->mpProgram)
		{
			if(apSettings->mbLog){
				if(mpProgram)
					Log("Setting gpu program: '%s' ", mpProgram->GetName().c_str());
				else
					Log("Setting gpu program: NULL ");
			}

			if(mpProgram==NULL && apSettings->mpProgram)
			{
				if(apSettings->mbLog)Log("Unbinding old program");
				apSettings->mpProgram->UnBind();
			}
			apSettings->mpProgram = mpProgram;

			if(mpProgram)
			{
				if(apSettings->mbLog)Log("Binding new program");
				mpProgram->Bind();

				if(mpProgramSetup)
				{
					if(apSettings->mbLog)Log("Custom program setup %d ", mpProgram);
					mpProgramSetup->Setup(mpProgram, apSettings);
				}
				apSettings->mpProgramSetup = mpProgramSetup;
			}
			if(apSettings->mbLog)Log("\n");
		}
	}

	//-----------------------------------------------------------------------

	void iRenderState::SetTextureMode(cRenderSettings* apSettings)
	{
		for(int i=0;i<MAX_TEXTUREUNITS;i++)
		{
			if(apSettings->mpTexture[i] != mpTexture[i])
			{
				if(apSettings->mbLog)
				{
					if(mpTexture[i]==NULL)
						Log("Setting texture: %d : NULL\n", i);
					else
						Log("Setting texture: %d : '%s'\n", i, mpTexture[i]->GetName().c_str());
				}

				apSettings->mpLowLevel->SetTexture(i,mpTexture[i]);
				apSettings->mpTexture[i] = mpTexture[i];
			}
		}
	}

	//-----------------------------------------------------------------------

	void iRenderState::SetVtxBuffMode(cRenderSettings* apSettings)
	{
		if(mpVtxBuffer != apSettings->mpVtxBuffer)
		{
			if(apSettings->mbLog)Log("Setting vertex buffer: %d\n",(size_t)mpVtxBuffer);
			if(apSettings->mpVtxBuffer) apSettings->mpVtxBuffer->UnBind();
			apSettings->mpVtxBuffer = mpVtxBuffer;

			if(mpVtxBuffer)
			{
				mpVtxBuffer->Bind();
			}
		}
	}

	//-----------------------------------------------------------------------

	void iRenderState::SetMatrixMode(cRenderSettings* apSettings)
	{
		cMatrixf mvMatrix;

		//It is a normal matrix
		if(mpModelMatrix)
		{
			mvMatrix = cMath::MatrixMul(apSettings->mpCamera->GetViewMatrix(), *mpModelMatrix);

			if(apSettings->mbLog)Log("Setting model matrix: %s ", cMath::MatrixToChar(*mpModelMatrix));

			apSettings->mbMatrixWasNULL = false;
		}
		//NULL matrix
		else
		{
			//If NULL already is set, no need for changes.
			if(apSettings->mbMatrixWasNULL)return;

			if(apSettings->mbLog)Log("Setting model matrix: Identity (NULL) ");

			mvMatrix = apSettings->mpCamera->GetViewMatrix();

			apSettings->mbMatrixWasNULL = true;
		}

		if(apSettings->mpProgram)
		{
			// rehatched - use full mvp
			auto mvpMatrix = cMath::MatrixMul(apSettings->mpCamera->GetProjectionMatrix(), mvMatrix);
			apSettings->mpProgram->SetMatrixf("worldViewProj", mvpMatrix);

			if (apSettings->mpProgramSetup)
			{
				apSettings->mpProgramSetup->SetupMatrix(mpModelMatrix,apSettings);
			}
		}

		if(apSettings->mbLog)Log("\n");
	}

	//-----------------------------------------------------------------------

	void iRenderState::SetRenderMode(cRenderSettings* apSettings)
	{
		if(apSettings->mbLog)Log("Drawing\n-----------------\n");

		if(apSettings->mDebugFlags & eRendererDebugFlag_RenderLines)
		{
			apSettings->mpVtxBuffer->DrawWireframe();
		}
		else
		{
			apSettings->mpVtxBuffer->Draw();
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// COMPARE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	int iRenderState::CompareSector(const iRenderState* apState) const
	{
		return (size_t)mpSector < (size_t)apState->mpSector;
	}

	//-----------------------------------------------------------------------

	int iRenderState::CompareDepthTest(const iRenderState* apState) const
	{
		return (int)mbDepthTest < (int)apState->mbDepthTest;
	}

	//-----------------------------------------------------------------------

	int iRenderState::CompareDepth(const iRenderState* apState) const
	{
		if(std::abs(mfZ - apState->mfZ) < 0.00001f) return 0;
		else return mfZ < apState->mfZ ? 1 : -1;
	}

	//-----------------------------------------------------------------------
	int iRenderState::CompareAlpha(const iRenderState* apState) const
	{
		return GetCompareVal((int)mAlphaMode,(int)apState->mAlphaMode);
	}

	//-----------------------------------------------------------------------

	int iRenderState::CompareBlend(const iRenderState* apState) const
	{
		int lRet = GetCompareVal((int)mChannelMode,(int)apState->mChannelMode);
		if(lRet ==0)
		{
			return GetCompareVal((int)mBlendMode,(int)apState->mBlendMode);
		}
		return lRet;
	}

	//-----------------------------------------------------------------------

	int iRenderState::CompareVtxProg(const iRenderState* apState) const
	{
		return GetCompareVal(mpProgram, apState->mpProgram);
	}

	//-----------------------------------------------------------------------

	int iRenderState::CompareTexture(const iRenderState* apState) const
	{
		for(int i=0; i< MAX_TEXTUREUNITS-1;++i)
		{
			if(mpTexture[i] != apState->mpTexture[i])
				return GetCompareVal(mpTexture[i], apState->mpTexture[i]);
		}
		return GetCompareVal(mpTexture[MAX_TEXTUREUNITS-1], apState->mpTexture[MAX_TEXTUREUNITS-1]);
	}
	//-----------------------------------------------------------------------

	int iRenderState::CompareVtxBuff(const iRenderState* apState)const
	{
		return GetCompareVal(mpVtxBuffer, apState->mpVtxBuffer);
	}

	//-----------------------------------------------------------------------

	int iRenderState::CompareMatrix(const iRenderState* apState)const
	{
		return GetCompareVal(mpModelMatrix, apState->mpModelMatrix);
	}

	//-----------------------------------------------------------------------

	int iRenderState::CompareRender(const iRenderState* apState) const
	{
		return GetCompareVal(mpObject, apState->mpObject);
	}

	//-----------------------------------------------------------------------

}

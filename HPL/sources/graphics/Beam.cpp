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
#include "graphics/Beam.h"

#include "tinyXML/tinyxml.h"

#include "resources/MaterialManager.h"
#include "resources/FileSearcher.h"
#include "graphics/VertexBuffer.h"
#include "graphics/Material.h"
#include "graphics/LowLevelGraphics.h"
#include "scene/Camera.h"
#include "scene/World3D.h"
#include "scene/Scene.h"
#include "math/Math.h"

#include "game/Game.h"
#include "system/Log.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cBeam::cBeam(const tString asName, iLowLevelGraphics* llGfx, cMaterialManager* materialMgr) :
	iRenderable(asName)
	{
		mpMaterialManager = materialMgr;
		mpLowLevelGraphics = llGfx;

		msFileName = "";

		mvSize = 1;

		mbTileHeight = true;

		mColor = cColor(1,1,1,1);

		mpMaterial = NULL;

		mlLastRenderCount = -1;

		mpVtxBuffer = mpLowLevelGraphics->CreateVertexBuffer(
								VertexMask_Position | VertexMask_Color0 | VertexMask_UV0 | VertexMask_Normal | VertexMask_Tangent,
								VertexBufferPrimitiveType::Triangles, VertexBufferUsageType::Dynamic,
								4, 6);

		cVector3f vCoords[4] = {{  (mvSize.x/2), -(mvSize.y/2), 0 },
								{ -(mvSize.x/2), -(mvSize.y/2), 0 },
								{ -(mvSize.x/2),  (mvSize.y/2), 0 },
								{  (mvSize.x/2),  (mvSize.y/2), 0 }};

		cVector2f vTexCoords[4] = {{ 1,-1},
								   {-1,-1},
								   {-1, 1},
								   { 1, 1}};

		auto posView = mpVtxBuffer->GetVec3View(VertexAttr_Position);
		auto normalView = mpVtxBuffer->GetVec3View(VertexAttr_Normal);
		auto uvView = mpVtxBuffer->GetVec2View(VertexAttr_UV0);
		auto colorView = mpVtxBuffer->GetColorView(VertexAttr_Color0);

		for (int i=0; i<4; i++)
		{
			*posView++ = vCoords[i];
			*colorView++ = cColor::White;
			*uvView++ = (vTexCoords[i] + cVector2f(1,1))/2;
			*normalView++ = { 0,0,1 };
		}

		auto indexView = mpVtxBuffer->GetIndexView();
		*indexView++ = 0;
		*indexView++ = 1;
		*indexView++ = 2;

		*indexView++ = 2;
		*indexView++ = 3;
		*indexView++ = 0;

		mpVtxBuffer->GenerateTangents();
		mpVtxBuffer->Compile();

		mpEnd = new cBeamEnd(asName + "_end",this);
		mpEnd->AddCallback(&mEndCallback);

		//Some temp setup
		mBoundingVolume.SetSize(cVector3f(mvSize.x, mvSize.y, mvSize.x));

		mbApplyTransformToBV = false;
	}

	//-----------------------------------------------------------------------

	cBeam::~cBeam()
	{
		delete mpEnd;
		if(mpMaterial) mpMaterialManager->Destroy(mpMaterial);
		if(mpVtxBuffer) delete mpVtxBuffer;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cBeam::SetSize(const cVector2f& avSize)
	{
		mvSize = avSize;
		mBoundingVolume.SetSize(cVector3f(mvSize.x, mvSize.y, mvSize.x));

		SetTransformUpdated();
	}

	//-----------------------------------------------------------------------

	void cBeam::SetTileHeight(bool abX)
	{
		if(mbTileHeight == abX) return;

		mbTileHeight = abX;

		SetTransformUpdated();
	}

	//-----------------------------------------------------------------------

	void cBeam::SetMultiplyAlphaWithColor(bool abX)
	{
		if(mbMultiplyAlphaWithColor == abX) return;

		mbMultiplyAlphaWithColor = abX;
	}

	//-----------------------------------------------------------------------

	void cBeam::SetColor(const cColor &aColor)
	{
		if(mColor == aColor) return;

		mColor = aColor;

		float *pColors = mpVtxBuffer->GetArray(VertexAttr_Color0);
		int colorStride = mpVtxBuffer->GetArrayStride(VertexAttr_Color0);

		//Change "lower colors"
		if(mbMultiplyAlphaWithColor)
		{
			for(int i=0; i<2;++i)
			{
				pColors[0] = mColor.r * mColor.a;
				pColors[1] = mColor.g * mColor.a;
				pColors[2] = mColor.b * mColor.a;
				pColors[3] = mColor.a;
				pColors += colorStride;
			}
		}
		else
		{
			for(int i=0; i<2;++i)
			{
				pColors[0] = mColor.r;
				pColors[1] = mColor.g;
				pColors[2] = mColor.b;
				pColors[3] = mColor.a;
				pColors += colorStride;
			}
		}
		mpVtxBuffer->UpdateData(VertexMask_Color0,false);
	}

	//-----------------------------------------------------------------------

	void cBeam::SetMaterial(iMaterial * apMaterial)
	{
		mpMaterial = apMaterial;
	}

	//-----------------------------------------------------------------------

	cBoundingVolume* cBeam::GetBoundingVolume()
	{
		if(mbUpdateBoundingVolume)
		{
			cVector3f vMax = GetWorldPosition();
			cVector3f vMin = vMax;
			cVector3f vEnd = mpEnd->GetWorldPosition();

			if(vMax.x < vEnd.x) vMax.x = vEnd.x;
			if(vMax.y < vEnd.y) vMax.y = vEnd.y;
			if(vMax.z < vEnd.z) vMax.z = vEnd.z;

			if(vMin.x > vEnd.x) vMin.x = vEnd.x;
			if(vMin.y > vEnd.y) vMin.y = vEnd.y;
			if(vMin.z > vEnd.z) vMin.z = vEnd.z;

			vMin -= cVector3f(mvSize.x);
			vMax += cVector3f(mvSize.x);

			mBoundingVolume.SetLocalMinMax(vMin,vMax);

			mbUpdateBoundingVolume = false;
		}

		return &mBoundingVolume;
	}

	//-----------------------------------------------------------------------

	void cBeam::UpdateGraphics(cCamera *apCamera,float afFrameTime, cRenderList *apRenderList)
	{
		if(	mlStartTransformCount == GetTransformUpdateCount() &&
			mlEndTransformCount == GetTransformUpdateCount())
		{
			return;
		}

		////////////////////////////////
		//Get Axis
		mvAxis = mpEnd->GetWorldPosition() - GetWorldPosition();

		mvMidPosition =GetWorldPosition() + mvAxis*0.5f;
		float fDist = mvAxis.Length();

		mvAxis.Normalise();

		////////////////////////////////
		//Update vertex buffer
		cVector2f vBeamSize = cVector2f(mvSize.x, fDist);

		float *pPos = mpVtxBuffer->GetArray(VertexAttr_Position);
		float *pTex = mpVtxBuffer->GetArray(VertexAttr_UV0);
		int posStride = mpVtxBuffer->GetArrayStride(VertexAttr_Position);
		int texStride = mpVtxBuffer->GetArrayStride(VertexAttr_UV0);

		cVector3f vCoords[4] = {cVector3f((vBeamSize.x/2),-(vBeamSize.y/2),0),
								cVector3f(-(vBeamSize.x/2),-(vBeamSize.y/2),0),
								cVector3f(-(vBeamSize.x/2),(vBeamSize.y/2),0),
								cVector3f((vBeamSize.x/2),(vBeamSize.y/2),0)};

		cVector3f vTexCoords[4];
		if(mbTileHeight)
		{
			vTexCoords[0] = cVector3f(1,1,0);	//Bottom left
			vTexCoords[1] = cVector3f(0,1,0);	//Bottom right
			vTexCoords[2] = cVector3f(0,-fDist/mvSize.y,0);	//Top left
			vTexCoords[3] = cVector3f(1,-fDist/mvSize.y,0);	//Top right
		}
		else
		{
			vTexCoords[0] = cVector3f(1,1,0);	//Bottom left
			vTexCoords[1] = cVector3f(0,1,0);	//Bottom right
			vTexCoords[2] = cVector3f(0,0,0);	//Top left
			vTexCoords[3] = cVector3f(1,0,0);	//Top right
		}

		for(int i=0; i<4;++i)
		{
			pPos[0] = vCoords[i].x;
			pPos[1] = vCoords[i].y;
			pPos[2] = vCoords[i].z;
			pPos += posStride;

			pTex[0] = vTexCoords[i].x;
			pTex[1] = vTexCoords[i].y;
			pTex += texStride;
		}

		if(mpMaterial->IsTransperant())
		{
			mpVtxBuffer->UpdateData(VertexMask_Position | VertexMask_UV0,false);
		}
		else
		{
			mpVtxBuffer->UpdateData(VertexMask_Position | VertexMask_UV0,false);
		}



	}

	//-----------------------------------------------------------------------

	cMatrixf cBeam::GetModelMatrix(cCamera *apCamera)
	{
		if(apCamera==NULL) return GetWorldMatrix();

		cMatrixf xform = GetWorldMatrix();
		cVector3f vForward, vRight, vUp;

		cVector3f vCameraForward = apCamera->GetPosition() - GetWorldPosition();
		vCameraForward.Normalise();

		vUp = mvAxis;//cMath::MatrixMul(GetWorldMatrix().GetRotation(),mvAxis);
		//vUp.Normalise();

		if(vUp == vForward)
		{
			vRight = cMath::Vector3Cross(vUp, vCameraForward);
			Warning("Beam Right vector is not correct! Contact programmer!\n");
		}
		else
			vRight = cMath::Vector3Cross(vUp, vCameraForward);

		vRight.Normalise();
		vForward = cMath::Vector3Cross(vRight, vUp);

		//Set right vector
		xform.m[0][0] = vRight.x;
		xform.m[1][0] = vRight.y;
		xform.m[2][0] = vRight.z;

		//Set up vector
		xform.m[0][1] = vUp.x;
		xform.m[1][1] = vUp.y;
		xform.m[2][1] = vUp.z;

		//Set forward vector
		xform.m[0][2] = vForward.x;
		xform.m[1][2] = vForward.y;
		xform.m[2][2] = vForward.z;

		xform.SetTranslation(mvMidPosition);

		return xform;
	}

	//-----------------------------------------------------------------------

	int cBeam::GetMatrixUpdateCount()
	{
		return GetTransformUpdateCount();
	}

	//-----------------------------------------------------------------------


	bool cBeam::LoadXMLProperties(const tString asFile)
	{
		msFileName = asFile;

		tString sNewFile = cString::SetFileExt(asFile,"beam");
		tString sPath = FileSearcher::GetFilePath(sNewFile);
		if(sPath != "")
		{
			TiXmlDocument *pDoc = new TiXmlDocument(sPath.c_str());
			if(pDoc->LoadFile())
			{
				TiXmlElement *pRootElem = pDoc->RootElement();

				TiXmlElement *pMainElem = pRootElem->FirstChildElement("MAIN");
				if(pMainElem!=NULL)
				{
					tString sMaterial = cString::ToString(pMainElem->Attribute("Material"),"");
					cVector2f vSize = cString::ToVector2f(pMainElem->Attribute("Size"),1);

					bool bTileHeight = cString::ToBool(pMainElem->Attribute("TileHeight"),true);
					bool bMultiplyAlphaWithColor = cString::ToBool(pMainElem->Attribute("MultiplyAlphaWithColor"),false);

					cColor StartColor = cString::ToColor(pMainElem->Attribute("StartColor"),cColor(1,1));
					cColor EndColor = cString::ToColor(pMainElem->Attribute("EndColor"),cColor(1,1));


					SetSize(vSize);
					SetTileHeight(bTileHeight);
					SetMultiplyAlphaWithColor(bMultiplyAlphaWithColor);
					SetColor(StartColor);
					mpEnd->SetColor(EndColor);

					/////////////////
					//Load material
					iMaterial *pMat = mpMaterialManager->CreateMaterial(sMaterial);
					if(pMat)	{
						SetMaterial(pMat);
					}
					else{
						Error("Couldn't load material '%s' in Beam file '%s'",
											sMaterial.c_str(), sNewFile.c_str());
						return false;
					}
				}
				else
				{
					Error("Cannot find main element in %s\n",sNewFile.c_str());
					return false;
				}
			}
			else
			{
				Error("Couldn't load file '%s'\n",sNewFile.c_str());
			}
			delete pDoc;
		}
		else
		{
			Error("Couldn't find file '%s'\n",sNewFile.c_str());
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------

	bool cBeam::IsVisible()
	{
		if(mColor.r <= 0 && mColor.g <= 0 && mColor.b <= 0) return false;

		return IsRendered();
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// BEAM END TRANSFORM UPDATE CALLBACK
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cBeamEnd_UpdateCallback::OnTransformUpdate(iEntity3D * apEntity)
	{
		cBeamEnd *pEnd = static_cast<cBeamEnd*>(apEntity);

		pEnd->mpBeam->SetTransformUpdated(true);
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// BEAM END
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cBeamEnd::SetColor(const cColor &aColor)
	{
		if(mColor == aColor) return;

		mColor = aColor;

		float *pColors = mpBeam->mpVtxBuffer->GetArray(VertexAttr_Color0);
		int colorStride = mpBeam->mpVtxBuffer->GetArrayStride(VertexAttr_Color0);

		//Change "upper colors"
		pColors += colorStride * 2;
		if(mpBeam->mbMultiplyAlphaWithColor)
		{
			for(int i=0; i<2;++i)
			{
				pColors[0] = mColor.r * mColor.a;
				pColors[1] = mColor.g * mColor.a;
				pColors[2] = mColor.b * mColor.a;
				pColors[3] = mColor.a;
				pColors += colorStride;
			}
		}
		else
		{
			for(int i=0; i<2;++i)
			{
				pColors[0] = mColor.r;
				pColors[1] = mColor.g;
				pColors[2] = mColor.b;
				pColors[3] = mColor.a;
				pColors += colorStride;
			}
		}

		mpBeam->mpVtxBuffer->UpdateData(VertexMask_Color0,false);
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// SAVE OBJECT STUFF
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------

}

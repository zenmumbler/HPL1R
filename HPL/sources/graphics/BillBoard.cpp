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
#include "graphics/BillBoard.h"

#include "resources/MaterialManager.h"
#include "resources/FileSearcher.h"
#include "graphics/VertexBuffer.h"
#include "graphics/Material.h"
#include "graphics/MeshCreator.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/MaterialHandler.h"
#include "scene/Camera.h"
#include "scene/World3D.h"
#include "scene/Scene.h"
#include "math/Math.h"

#include "game/Game.h"
#include "system/Log.h"
#include "system/String.h"

#include "tinyXML/tinyxml.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cBillboard::cBillboard(const tString asName,const cVector2f& avSize, iLowLevelGraphics* llGfx, cMaterialManager* materialMgr) :
	iRenderable(asName)
	{
		mpMaterialManager = materialMgr;
		mpLowLevelGraphics = llGfx;

		mpHaloSourceBuffer = CreateBoxVertexBuffer(mpLowLevelGraphics, 1);

		mvSize = avSize;
		mvAxis = cVector3f(0,1,0);

		mColor = cColor(1,1,1,1);
		mfForwardOffset =0;
		mfHaloAlpha = 1.0f;

		mType = eBillboardType_Point;

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

		mbIsHalo = false;
		mbHaloSourceIsParent = false;
		mvHaloSourceSize = 1;

		mBoundingVolume.SetSize(cVector3f(mvSize.x, mvSize.y, mvSize.x));
	}

	//-----------------------------------------------------------------------

	cBillboard::~cBillboard()
	{
		if(mpMaterial) mpMaterialManager->Destroy(mpMaterial);
		if(mpVtxBuffer) delete mpVtxBuffer;
		if(mpHaloSourceBuffer) delete mpHaloSourceBuffer;

		if(mQueryObject.mpQuery)
			mpLowLevelGraphics->DestroyOcclusionQuery(mQueryObject.mpQuery);
		if(mMaxQueryObject.mpQuery)
			mpLowLevelGraphics->DestroyOcclusionQuery(mMaxQueryObject.mpQuery);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cBillboard::SetSize(const cVector2f& avSize)
	{
		mvSize = avSize;
		mBoundingVolume.SetSize(cVector3f(mvSize.x, mvSize.y, mvSize.x));

		float *pPos = mpVtxBuffer->GetArray(VertexAttr_Position);
		int posStride = mpVtxBuffer->GetArrayStride(VertexAttr_Position);

		cVector3f vCoords[4] = {cVector3f((mvSize.x/2),-(mvSize.y/2),0),
								cVector3f(-(mvSize.x/2),-(mvSize.y/2),0),
								cVector3f(-(mvSize.x/2),(mvSize.y/2),0),
								cVector3f((mvSize.x/2),(mvSize.y/2),0)};

		for(int i=0; i<4;++i)
		{
			pPos[0] = vCoords[i].x;
			pPos[1] = vCoords[i].y;
			pPos[2] = vCoords[i].z;
			pPos += posStride;
		}

		mpVtxBuffer->UpdateData(VertexMask_Position,false);

		if(mType == eBillboardType_Axis) SetAxis(mvAxis);

		SetTransformUpdated();
	}

	//-----------------------------------------------------------------------

	void cBillboard::SetType(eBillboardType aType)
	{
		mType = aType;

		SetAxis(mvAxis);
	}

	//-----------------------------------------------------------------------

	void cBillboard::SetAxis(const cVector3f& avAxis)
	{
		mvAxis = avAxis;
		mvAxis.Normalise();

		//This is a quick fix so the bounding box is correct for non up-pointing axises
		if(mType == eBillboardType_Axis && mvAxis != cVector3f(0,1,0))
		{
			float fMax = mvSize.x;
			if(fMax < mvSize.y) fMax = mvSize.y;

			fMax *= kSqrt2f;

			mBoundingVolume.SetSize(fMax);

			SetTransformUpdated();
		}
	}

	//-----------------------------------------------------------------------

	void cBillboard::SetColor(const cColor &aColor)
	{
		if(mColor == aColor) return;

		mColor = aColor;

		float *pColors = mpVtxBuffer->GetArray(VertexAttr_Color0);
		int colorStride = mpVtxBuffer->GetArrayStride(VertexAttr_Color0);

		for(int i=0; i<4;++i)
		{
			pColors[0] = mColor.r * mfHaloAlpha;
			pColors[1] = mColor.g * mfHaloAlpha;
			pColors[2] = mColor.b * mfHaloAlpha;
			pColors[3] = mColor.a * mfHaloAlpha;
			pColors += colorStride;
		}

		mpVtxBuffer->UpdateData(VertexMask_Color0,false);
	}

	//-----------------------------------------------------------------------

	void cBillboard::SetHaloAlpha(float afX)
	{
		if(mfHaloAlpha == afX)
		{
			return;
		}

		mfHaloAlpha = afX;

		float *pColors = mpVtxBuffer->GetArray(VertexAttr_Color0);
		int colorStride = mpVtxBuffer->GetArrayStride(VertexAttr_Color0);

		for(int i=0; i<4;++i)
		{
			pColors[0] = mColor.r * mfHaloAlpha;
			pColors[1] = mColor.g * mfHaloAlpha;
			pColors[2] = mColor.b * mfHaloAlpha;
			pColors[3] = mColor.a * mfHaloAlpha;
			pColors += colorStride;
		}

		mpVtxBuffer->UpdateData(VertexMask_Color0,false);
	}

	//-----------------------------------------------------------------------

	void cBillboard::SetForwardOffset(float afOffset)
	{
		mfForwardOffset = afOffset;
	}

	//-----------------------------------------------------------------------

	void cBillboard::SetMaterial(iMaterial * apMaterial)
	{
		mpMaterial = apMaterial;
	}

	//-----------------------------------------------------------------------

	cBoundingVolume* cBillboard::GetBoundingVolume()
	{
		return &mBoundingVolume;
	}

	//-----------------------------------------------------------------------

	void cBillboard::UpdateGraphics(cCamera *apCamera,float afFrameTime, cRenderList *apRenderList)
	{
		if(mbIsHalo==false) return;

		////////////////////////
		//Set the alpha
		float fAlpha = 0;

		if(mlLastRenderCount == apRenderList->GetLastRenderCount())
		{
			if(mMaxQueryObject.mpQuery->GetSampleCount()>0)
			{
				fAlpha =	(float)mQueryObject.mpQuery->GetSampleCount() /
							(float)mMaxQueryObject.mpQuery->GetSampleCount();
			}
		}
		SetHaloAlpha(fAlpha);
		mlLastRenderCount = apRenderList->GetRenderCount();

		////////////////////////
		//Add the queries
		if(mbHaloSourceIsParent)
		{
			iRenderable *pParent = static_cast<iRenderable*>(GetEntityParent());
			if(pParent==NULL)
			{
				Error("Billboard %s has no parent!\n",msName.c_str());
				return;
			}

			iVertexBuffer *pVtxBuffer = pParent->GetVertexBuffer();
			if(pVtxBuffer==NULL)
			{
				Error("Billboard '%s' parent '%s' as NULL vertex buffer!\n",msName.c_str(),
																		pParent->GetName().c_str());
				return;
			}

			mQueryObject._matrix = pParent->GetModelMatrix(apCamera);
			mQueryObject.mpVtxBuffer = pVtxBuffer;

			mMaxQueryObject._matrix = pParent->GetModelMatrix(apCamera);
			mMaxQueryObject.mpVtxBuffer = pVtxBuffer;
		}
		else
		{
			mQueryObject._matrix = GetWorldMatrix();
			mQueryObject.mpVtxBuffer = mpHaloSourceBuffer;

			mMaxQueryObject._matrix = GetWorldMatrix();
			mMaxQueryObject.mpVtxBuffer = mpHaloSourceBuffer;
		}

		mQueryObject.mbDepthTest = true;
		mMaxQueryObject.mbDepthTest = false;

		apRenderList->AddOcclusionQuery(&mQueryObject);
		apRenderList->AddOcclusionQuery(&mMaxQueryObject);

	}

	//-----------------------------------------------------------------------

	cMatrixf cBillboard::GetModelMatrix(cCamera *apCamera)
	{
		if(apCamera==NULL)return GetWorldMatrix();

		cMatrixf xform = GetWorldMatrix();
		cVector3f vForward, vRight, vUp;

		cVector3f vCameraForward = apCamera->GetPosition() - GetWorldPosition();
		vCameraForward.Normalise();

		if(mType == eBillboardType_Point)
		{
			vForward = vCameraForward;
			vRight = cMath::Vector3Cross(apCamera->GetViewMatrix().GetUp(), vForward);
			vUp = cMath::Vector3Cross(vForward,vRight);
		}
		else if(mType == eBillboardType_Axis)
		{
			vUp = cMath::MatrixMul(GetWorldMatrix().GetRotation(),mvAxis);
			vUp.Normalise();

			if(vUp == vForward)
			{
				vRight = cMath::Vector3Cross(vUp, vCameraForward);
				Warning("Billboard Right vector is not correct! Contact programmer!\n");
			}
			else
				vRight = cMath::Vector3Cross(vUp, vCameraForward);

			vRight.Normalise();
			vForward = cMath::Vector3Cross(vRight, vUp);

			//vForward.Normalise();
			//vUp.Normalise();
		}

		if(mfForwardOffset!=0)
		{
			cVector3f vPos = xform.GetTranslation();
			vPos += vCameraForward * mfForwardOffset;
			xform.SetTranslation(vPos);
		}

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

		return xform;
	}

	//-----------------------------------------------------------------------

	int cBillboard::GetMatrixUpdateCount()
	{
		return GetTransformUpdateCount();
	}

	//-----------------------------------------------------------------------


	void cBillboard::LoadXMLProperties(const tString asFile)
	{
		tString sNewFile = cString::SetFileExt(asFile,"bnt");
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
					mType = ToType(pMainElem->Attribute("Type"));
					tString sMaterial = cString::ToString(pMainElem->Attribute("Material"),"");
					bool bUsesOffset = cString::ToBool(pMainElem->Attribute("UseOffset"),false);
					if(bUsesOffset==false) mfForwardOffset =0;

					/////////////////
					//Halo stuff
					bool bIsHalo = cString::ToBool(pMainElem->Attribute("IsHalo"),false);
					SetIsHalo(bIsHalo);

					if(bIsHalo)
					{
						bool bHaloSourceIsParent  = cString::ToBool(pMainElem->Attribute("HaloSourceIsParent"),false);
						SetHaloSourceIsParent(bHaloSourceIsParent);

						if(bHaloSourceIsParent == false)
						{
							tString sSizeVec = cString::ToString(pMainElem->Attribute("HaloSourceSize"),"1 1 1");
							tFloatVec vSizeValues;
							cString::GetFloatVec(sSizeVec,vSizeValues,NULL);
							SetHaloSourceSize(cVector3f(vSizeValues[0],vSizeValues[1],vSizeValues[2]));
						}
					}

					/////////////////
					//Load material
					iMaterial *pMat = mpMaterialManager->CreateMaterial(sMaterial);
					if(pMat)
					{
						SetMaterial(pMat);
					}
					else
					{
						Error("Couldn't load material '%s' in billboard file '%s'",
													sMaterial.c_str(), sNewFile.c_str());
					}
				}
				else
				{
					Error("Cannot find main element in %s\n",sNewFile.c_str());
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
		}

	}

	//-----------------------------------------------------------------------

	bool cBillboard::IsVisible()
	{
		if(mColor.r <= 0 && mColor.g <= 0 && mColor.b <= 0) return false;

		return IsRendered();
	}

	//-----------------------------------------------------------------------

	void cBillboard::SetIsHalo(bool abX)
	{
		mbIsHalo = abX;

		if(mbIsHalo)
		{
			mQueryObject.mpQuery = mpLowLevelGraphics->CreateOcclusionQuery();
			mMaxQueryObject.mpQuery = mpLowLevelGraphics->CreateOcclusionQuery();

			mfHaloAlpha = 1; //THis is to make sure that the new alpha is set to the mesh.
			SetHaloAlpha(0);
		}
		else if(mQueryObject.mpQuery)
		{
			mpLowLevelGraphics->DestroyOcclusionQuery(mQueryObject.mpQuery);
			mpLowLevelGraphics->DestroyOcclusionQuery(mMaxQueryObject.mpQuery);
		}
	}

	//-----------------------------------------------------------------------

	void cBillboard::SetHaloSourceSize(const cVector3f &avSize)
	{
		mvHaloSourceSize = avSize;

		UpdateSourceBufferSize();
	}

	//-----------------------------------------------------------------------

	void cBillboard::SetHaloSourceIsParent(bool abX)
	{
		mbHaloSourceIsParent = abX;
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	eBillboardType cBillboard::ToType(const char* apString)
	{
		if(apString==NULL) return eBillboardType_Point;

		tString sType = cString::ToLowerCase(apString);

		if(sType == "point") return eBillboardType_Point;
		else if(sType == "axis") return eBillboardType_Axis;

		Warning("Invalid billboard type '%s'\n",apString);

		return eBillboardType_Point;
	}

	//-----------------------------------------------------------------------

	void cBillboard::UpdateSourceBufferSize()
	{
		int lNum = mpHaloSourceBuffer->GetVertexCount();
		float *pPositions = mpHaloSourceBuffer->GetArray(VertexAttr_Position);
		int posStride = mpHaloSourceBuffer->GetArrayStride(VertexAttr_Position);

		for(int i=0; i<lNum;++i)
		{
			//X
			if(pPositions[0] < 0)
				pPositions[0] = -mvHaloSourceSize.x*0.5f;
			else
				pPositions[0] = mvHaloSourceSize.x*0.5f;
			//Y
			if(pPositions[1] < 0)
				pPositions[1] = -mvHaloSourceSize.y*0.5f;
			else
				pPositions[1] = mvHaloSourceSize.y*0.5f;
			//Z
			if(pPositions[2] < 0)
				pPositions[2] = -mvHaloSourceSize.z*0.5f;
			else
				pPositions[2] = mvHaloSourceSize.z*0.5f;

			pPositions += posStride;
		}

		mpHaloSourceBuffer->UpdateData(VertexMask_Position, false);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// SAVE OBJECT STUFF
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	kBeginSerialize(cSaveData_cBillboard,cSaveData_iRenderable)
	kSerializeVar(msMaterial,eSerializeType_String)

	kSerializeVar(mType,eSerializeType_Int32)
	kSerializeVar(mvSize,eSerializeType_Vector3f)
	kSerializeVar(mvAxis,eSerializeType_Vector3f)
	kSerializeVar(mfForwardOffset,eSerializeType_Float32)
	kSerializeVar(mColor,eSerializeType_Color)
	kSerializeVar(mfHaloAlpha, eSerializeType_Float32)

	kSerializeVar(mbIsHalo, eSerializeType_Bool)
	kSerializeVar(mvHaloSourceSize, eSerializeType_Vector3f)
	kSerializeVar(mbHaloSourceIsParent, eSerializeType_Bool)
	kEndSerialize()

	//-----------------------------------------------------------------------

	iSaveObject* cSaveData_cBillboard::CreateSaveObject(cSaveObjectHandler *apSaveObjectHandler,cGame *apGame)
	{
		cWorld3D *pWorld = apGame->GetScene()->GetWorld3D();

		cBillboard *pBill = pWorld->CreateBillboard(msName,mvSize,msMaterial);

		return pBill;
	}

	//-----------------------------------------------------------------------

	int cSaveData_cBillboard::GetSaveCreatePrio()
	{
		return 3;
	}

	//-----------------------------------------------------------------------

	iSaveData* cBillboard::CreateSaveData()
	{
		return new cSaveData_cBillboard();
	}

	//-----------------------------------------------------------------------

	void cBillboard::SaveToSaveData(iSaveData *apSaveData)
	{
		kSaveData_SaveToBegin(cBillboard);

		pData->msMaterial = mpMaterial==NULL ? "" : mpMaterial->GetName();

		kSaveData_SaveTo(mType);
		kSaveData_SaveTo(mvSize);
		kSaveData_SaveTo(mvAxis);
		kSaveData_SaveTo(mfForwardOffset);
		kSaveData_SaveTo(mColor);
		kSaveData_SaveTo(mfHaloAlpha);

		kSaveData_SaveTo(mbIsHalo);
		kSaveData_SaveTo(mvHaloSourceSize);
		kSaveData_SaveTo(mbHaloSourceIsParent);
	}

	//-----------------------------------------------------------------------

	void cBillboard::LoadFromSaveData(iSaveData *apSaveData)
	{
		kSaveData_LoadFromBegin(cBillboard);

		mType = (eBillboardType) pData->mType;
		kSaveData_LoadFrom(mvSize);
		kSaveData_LoadFrom(mvAxis);
		kSaveData_LoadFrom(mfForwardOffset);
		kSaveData_LoadFrom(mColor);
		kSaveData_LoadFrom(mfHaloAlpha);

		SetIsHalo(pData->mbIsHalo);
		SetHaloSourceSize(pData->mvHaloSourceSize);
		SetHaloSourceIsParent(pData->mbHaloSourceIsParent);
	}

	//-----------------------------------------------------------------------

	void cBillboard::SaveDataSetup(cSaveObjectHandler *apSaveObjectHandler, cGame *apGame)
	{
		kSaveData_SetupBegin(cBillboard);

		if(mbIsHalo)
		{
			//Log("Setting up halo billboard '%s'\n",msName.c_str());
		}
	}

	//-----------------------------------------------------------------------
}

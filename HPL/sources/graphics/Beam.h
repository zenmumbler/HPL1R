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
#ifndef HPL_BEAM_H
#define HPL_BEAM_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "scene/Entity3D.h"
#include "graphics/Renderable.h"
#include "RenderList.h"
#include "system/StringTypes.h"

namespace hpl {

	class cMaterialManager;
	class cResources;
	class iLowLevelGraphics;
	class iMaterial;
	class iVertexBuffer;

	//------------------------------------------

	class cBeam;
	class cBeamEnd : public iEntity3D
	{
	friend class cBeam;
	friend class cBeamEnd_UpdateCallback;
	public:
		cBeamEnd(const tString asName, cBeam *apBeam) : iEntity3D(asName),
						mColor(1,1),mpBeam(apBeam) {}

		void SetColor(const cColor &aColor);
		const cColor& GetColor(){ return mColor;}

		/////////////////////////////////
		//Entity implementation
		tString GetEntityType(){ return "BeamEnd";}
	private:
		cColor mColor;
		cBeam *mpBeam;
	};

	//------------------------------------------

	class cBeamEnd_UpdateCallback : public iEntityCallback
	{
	public:
		void OnTransformUpdate(iEntity3D * apEntity);
	};

	//------------------------------------------

	class cBeam : public iRenderable
	{
	#ifdef __GNUC__
		typedef iRenderable __super;
	#endif
	friend class cBeamEnd;
	public:
		cBeam(const tString asName, iLowLevelGraphics* llGfx, cMaterialManager* materialMgr);
		~cBeam();

		void SetMaterial(iMaterial * apMaterial);

		const tString& GetFileName(){return msFileName;}

		/**
		 * Set the size. X= the thickness of the line, width of texture used. Y = the length that one texture height takes.
		 * \param avSize
		 */
		void SetSize(const cVector2f& avSize);
		cVector2f GetSize(){ return mvSize;}

		void SetColor(const cColor &aColor);
		const cColor& GetColor(){ return mColor;}

		void SetTileHeight(bool abX);
		bool GetTileHeight(){ return mbTileHeight;}

		void SetMultiplyAlphaWithColor(bool abX);
		bool GetMultiplyAlphaWithColor(){ return mbMultiplyAlphaWithColor;}

		cBeamEnd* GetEnd(){ return mpEnd;}

		bool LoadXMLProperties(const tString asFile);

		cVector3f GetAxis(){ return mvAxis;}
		cVector3f GetMidPosition(){ return mvMidPosition;}

		/////////////////////////////////
		//Entity implementation
		tString GetEntityType() override { return "Beam";}

		bool IsVisible() override;
		void SetVisible(bool abVisible) override { SetRendered(abVisible); }

		//Renderable implementations
		iMaterial *GetMaterial() override { return mpMaterial;}
		iVertexBuffer* GetVertexBuffer() override {return mpVtxBuffer;}

		void UpdateGraphics(cCamera *apCamera,float afFrameTime, cRenderList *apRenderList) override;

		bool IsShadowCaster() override { return false;}

		cBoundingVolume* GetBoundingVolume() override;

		cMatrixf GetModelMatrix(cCamera *apCamera) override;

		int GetMatrixUpdateCount() override;


		eRenderableType GetRenderType() override { return eRenderableType_ParticleSystem;}
	private:
		cMaterialManager* mpMaterialManager;
		iLowLevelGraphics* mpLowLevelGraphics;

		iMaterial *mpMaterial;
		iVertexBuffer* mpVtxBuffer;

		cBeamEnd *mpEnd;

		tString msFileName;

		int mlStartTransformCount;
		int mlEndTransformCount;

		int mlLastRenderCount;

		cBeamEnd_UpdateCallback mEndCallback;

		cVector2f mvSize;

		cVector3f mvAxis;
		cVector3f mvMidPosition;

		bool mbTileHeight;
		bool mbMultiplyAlphaWithColor;

		cColor mColor;
	};

};
#endif // HPL_BEAM_H

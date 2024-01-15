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

#include "resources/FrameBitmap.h"
#include "graphics/Bitmap.h"
#include "graphics/Texture.h"
#include "math/Math.h"
#include "system/Log.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cFrameBitmap::cFrameBitmap(int width, int height, iTexture *texture, int index)
		: mpTexture{texture}
	{
		mBitmap.Create(width, height);
		mlMinHole = 6;
		mbIsFull = false;
		_index = index;

		//Root node in rect tree
		mRects.Insert(cFBitmapRect(0, 0, mBitmap.GetWidth(), mBitmap.GetHeight(), -1));

		// Log("FB: Created with size [%d, %d]\n", width, height);
	}

	cFrameBitmap::~cFrameBitmap()
	{
		// Log("FB: Deleting [%d, %d]\n", mBitmap.GetWidth(), mBitmap.GetHeight());
		delete mpTexture;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	constexpr const bool DEBUG_BTREE = false;

	// [Rehatched]: see if stb_rect_pack is better than this at some point
	// unsure as that lib indicates it's better for all the rects at once
	cRect2l cFrameBitmap::AddBitmap(const Bitmap &aSrc)
	{
		//source size
		//+2 because we are gonna have a border to get rid if some antialiasing problems
		int lSW = aSrc.GetWidth()+2;
		int lSH = aSrc.GetHeight()+2;

		//destination size
		int lDW = mBitmap.GetWidth();
		int lDH = mBitmap.GetHeight();

		cVector2l vPos;

		bool bFoundEmptyNode = false;
		bool bFoundNode = false;
		//Debug
		int node=0;

		if(DEBUG_BTREE)Log("**** Image *****\n");

		//Get the leaves of the tree and search it for a good pos.
		tRectTreeNodeList lstNodes =  mRects.GetLeafList();
		tRectTreeNodeListIt it;
		for(it = lstNodes.begin();it!=lstNodes.end();++it)
		{
			if(DEBUG_BTREE)Log("Checking node %d:\n",node++);
			tRectTreeNode *TopNode = *it;
			cFBitmapRect* pData = TopNode->GetData();

			//Check if the space is free
			if(pData->mlHandle<0)
			{
				if(DEBUG_BTREE)Log("Found free node\n");
				bFoundEmptyNode = true; //An empty node was found.. bitmap not full yet.

				//Check if the Image fits in the rect
				cRect2l NewRect = cRect2l(pData->mRect.x,pData->mRect.y,lSW, lSH);
				if(DEBUG_BTREE)Log("Fit: [%d:%d:%d:%d] in [%d:%d:%d:%d]\n",
						NewRect.x,NewRect.y,NewRect.w,NewRect.h,
						pData->mRect.x,pData->mRect.y,pData->mRect.w,pData->mRect.h);

				if(cMath::BoxFit(NewRect, pData->mRect))
				{
					if(DEBUG_BTREE)Log("The node fits!\n");
					bFoundNode = true;

					//If the bitmap fits perfectly add the node without splitting
					if(MinimumFit(NewRect,pData->mRect))
					{
						if(DEBUG_BTREE)Log("Minimum fit!\n");
						pData->mRect = NewRect;
						pData->mlHandle = 1;
					}
					//If there is still space left, make new nodes.
					else
					{
						if(DEBUG_BTREE)Log("Normal fit!\n");
						//Insert 2 children for the top node (lower and upper part.
						tRectTreeNode* UpperNode;
						//Upper
						UpperNode = mRects.InsertAt(cFBitmapRect(NewRect.x,NewRect.y,
													pData->mRect.w,NewRect.h,-2),TopNode,
													eBinTreeNode_Left);

						//Lower
						mRects.InsertAt(cFBitmapRect(NewRect.x,NewRect.y+NewRect.h,
										pData->mRect.w,pData->mRect.h-NewRect.h,-3),TopNode,
										eBinTreeNode_Right);

						//Split the Upper Node into 2 nodes.
						pData = UpperNode->GetData();//Get the data for the upper node.
						//Upper split, this is the new bitmap
						mRects.InsertAt(cFBitmapRect(NewRect.x,NewRect.y,
										NewRect.w,NewRect.h,2),UpperNode,
										eBinTreeNode_Left);

						//Lower split, this is empty
						mRects.InsertAt(cFBitmapRect(NewRect.x+NewRect.w,NewRect.y,
										pData->mRect.w-NewRect.w,NewRect.h,-4),UpperNode,
										eBinTreeNode_Right);
					}

					vPos = cVector2l(NewRect.x+1,NewRect.y+1);//+1 for the right pos

					//Draw 4 times so we get a nice extra border
					for(int i=0;i<2;i++)for(int j=0;j<2;j++){
						aSrc.DrawToBitmap(mBitmap, NewRect.x+i*2, NewRect.y+j*2);
					}
					//Fix the border a little more:
					for(int i=-1;i<2;i++)for(int j=-1;j<2;j++)
						if((i==0 || j==0) && (i!=j)){
						aSrc.DrawToBitmap(mBitmap, NewRect.x+1+i, NewRect.y+1+j);
					}
					//Draw the final
					aSrc.DrawToBitmap(mBitmap, NewRect.x+1,NewRect.y+1);
					break;
				}
			}
		}

		cRect2l destRect{0, 0, 0, 0};
		if(bFoundNode)
		{
			destRect = cRect2l{vPos, cVector2l(lSW-2, lSH-2)};
			mbIsUpdated = true;
		}
		if(!bFoundEmptyNode)
		{
			mbIsFull = true;
		}


		/// LAST DEBUG ///
		if(DEBUG_BTREE)
		{
			Log("Current Tree begin:\n");
			tRectTreeNodeList lstNodes =  mRects.GetNodeList();
			tRectTreeNodeListIt it;
			int node=0;
			for(it = lstNodes.begin();it!=lstNodes.end();++it)
			{
				cRect2l Rect = (*it)->GetData()->mRect;
				int h = (*it)->GetData()->mlHandle;
				Log(" %d: [%d:%d:%d:%d]:%d\n",node,Rect.x,Rect.y,Rect.w,Rect.h,h);
				node++;
			}
			Log("Current Tree end:\n");
			Log("-----------------\n");

			Log("Current Leaves begin:\n");
			lstNodes =  mRects.GetLeafList();
			node=0;
			for(it = lstNodes.begin();it!=lstNodes.end();++it)
			{
				cRect2l Rect = (*it)->GetData()->mRect;
				int h = (*it)->GetData()->mlHandle;
				Log(" %d: [%d:%d:%d:%d]: %d\n",node,Rect.x,Rect.y,Rect.w,Rect.h,h);
				node++;
			}
			Log("Current Tree end:\n");
			Log("-----------------\n");
		}


		return destRect;
	}

	//-----------------------------------------------------------------------

	bool cFrameBitmap::MinimumFit(cRect2l aSrc, cRect2l aDest)
	{
		return
			aDest.w - aSrc.w < mlMinHole &&
			aDest.h - aSrc.h < mlMinHole;
	}

	//-----------------------------------------------------------------------

	bool cFrameBitmap::FlushToTexture()
	{
		if (mbIsUpdated)
		{
			// Log("FB: updating texture #%d\n", _index);
			mpTexture->CreateFromBitmap(mBitmap);
			mpTexture->SetWrapS(eTextureWrap_ClampToEdge);
			mpTexture->SetWrapT(eTextureWrap_ClampToEdge);

			mbIsUpdated = false;
			return true;
		}
		else
		{
			return false;
		}
	}

	//-----------------------------------------------------------------------
}

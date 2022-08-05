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
#ifndef HPL_MEMORYPOOL_H
#define HPL_MEMORYPOOL_H

#include <vector>

template <class T>
class cMemoryPool
{
public:
	//---------------------------------

	cMemoryPool(size_t alSize, T* (*apCreateFunc)())
	{
		mvData.resize(alSize,NULL);
		mlCurrentData = 0;

		mpCreateFunc = apCreateFunc;

		for(size_t i=0; i< mvData.size(); ++i)
		{
			if(mpCreateFunc)	mvData[i] = mpCreateFunc();
			else				mvData[i] = new T();
		}
	}

	//---------------------------------

	~cMemoryPool()
	{
		for(size_t i=0; i< mvData.size(); ++i) delete mvData[i];
	}

	//---------------------------------

	T* Create()
	{
		T*  pData = mvData[mlCurrentData];

		if(mlCurrentData == mvData.size()-1)
		{
			size_t lNewSize = mvData.size() * 2;
			size_t lStart = mvData.size();
			mvData.resize(lNewSize);

			for(size_t i=lStart; i< mvData.size(); ++i)
			{
				if(mpCreateFunc)	mvData[i] = mpCreateFunc();
				else				mvData[i] = new T();
			}

			++mlCurrentData;
		}
		else
		{
			++mlCurrentData;
		}

		return pData;
	}

	//---------------------------------

	void Release(T *apData)
	{
		if(mlCurrentData==0) return;

		--mlCurrentData;
		mvData[mlCurrentData] = apData;
	}

	//---------------------------------

	void ClearUnused()
	{
		for(size_t i=mlCurrentData+1; i< mvData.size(); ++i)
		{
			delete mvData[i];
		}
		mvData.resize(mlCurrentData+1);
	}

	//---------------------------------

private:
	std::vector<T*> mvData;
	size_t mlCurrentData;
	T* (*mpCreateFunc)();
};

#endif /* HPL_MEMORYPOOL_H */

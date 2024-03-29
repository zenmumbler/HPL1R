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
#ifndef HPL_CONTAINER_H
#define HPL_CONTAINER_H

#include <vector>
#include <list>
#include <map>

namespace hpl {

	//---------------------------------

	class iContainerIterator
	{
	friend class cSerializeClass;
	public:
		virtual ~iContainerIterator() {}
	protected:
		virtual bool HasNext()=0;

		virtual void* NextPtr()=0;
	};

	//---------------------------------

	class iContainer
	{
	friend class cSerializeClass;
	public:
		virtual ~iContainer() = default;
		virtual size_t Size()=0;
		virtual void Clear()=0;
	protected:

		virtual void AddVoidPtr(void **apPtr)=0;
		virtual void AddVoidClass(void *apClass)=0;

		virtual iContainerIterator* CreateIteratorPtr()=0;
	};

	//---------------------------------

	class iContainerKeyPair
	{
	public:
		virtual ~iContainerKeyPair() = default;
		virtual size_t Size()=0;

		virtual void AddVoidPtr(void *apKey, void **apClass)=0;
		virtual void AddVoidClass(void *apKey, void *apClass)=0;
	};

	//---------------------------------


	template<class T>
	class cContainerVecIterator : public iContainerIterator
	{
		void* NextPtr(){
			return &Next();
		}

	public:
		cContainerVecIterator(std::vector<T> *apVec)
		{
			mpVec = apVec;
			mIt = apVec->begin();
		}

		bool HasNext()
		{
			return mIt != mpVec->end();
		}

		T& Next()
		{
			T &val = *mIt;
			mIt++;
			return val;
		}

		T& PeekNext()
		{
			return *mIt;
		}

		void Erase()
		{
			if(mIt != mpVec->end())
				mIt = mpVec->erase(mIt);
		}

	private:
		std::vector<T> *mpVec;
		typename std::vector<T>::iterator mIt;
	};

	////////////////////////////

	template<class T>
	class cContainerVec : public iContainer
	{
	private:
		void AddVoidPtr(void **apPtr)
		{
			mvVector.push_back(*((T*)apPtr));
		}
		void AddVoidClass(void *apClass)
		{
			mvVector.push_back(*((T*)apClass));
		}
		iContainerIterator* CreateIteratorPtr()
		{
			return new cContainerVecIterator<T>(&mvVector);
		}

	public:
		cContainerVec(){}

		//////////////////////
		size_t Size()
		{
			return mvVector.size();
		}

		void Clear()
		{
			mvVector.clear();
		}

		//////////////////////

		void Reserve(size_t alSize)
		{
			mvVector.reserve(alSize);
		}

		void Resize(size_t alSize)
		{
			mvVector.resize(alSize);
		}

		void Add(T aVal)
		{
			mvVector.push_back(aVal);
		}

		//////////////////////

		cContainerVecIterator<T> GetIterator()
		{
			return cContainerVecIterator<T>(&mvVector);
		}

		//////////////////////

		T& operator [](size_t alX)
		{
			return mvVector[alX];
		}

		//////////////////////

		std::vector<T> mvVector;
	};

	//---------------------------------

	template<class T>
	class cContainerListIterator : public iContainerIterator
	{
		void* NextPtr(){
			return &Next();
		}

	public:
		cContainerListIterator(std::list<T> *apVec)
		{
			mpVec = apVec;
			mIt = apVec->begin();
		}

		bool HasNext()
		{
			return mIt != mpVec->end();
		}

		T& Next()
		{
			T &val = *mIt;
			mIt++;
			return val;
		}

		T& PeekNext()
		{
			return *mIt;
		}

		void Erase()
		{
			if(mIt != mpVec->end())
				mIt = mpVec->erase(mIt);
		}

	private:
		std::list<T> *mpVec;
		typename std::list<T>::iterator mIt;
	};

	////////////////////////////

	template<class T>
	class cContainerList : public iContainer
	{
	private:
		void AddVoidPtr(void **apPtr)
		{
			mvVector.push_back(*((T*)apPtr));
		}
		void AddVoidClass(void *apClass)
		{
			mvVector.push_back(*((T*)apClass));
		}
		iContainerIterator* CreateIteratorPtr()
		{
			return new cContainerListIterator<T>(&mvVector);
		}

	public:
		cContainerList(){}

		//////////////////////
		size_t Size()
		{
			return mvVector.size();
		}

		void Clear()
		{
			mvVector.clear();
		}
		//////////////////////

		void Add(T aVal)
		{
			mvVector.push_back(aVal);
		}

		//////////////////////

		cContainerListIterator<T> GetIterator()
		{
			return cContainerListIterator<T>(&mvVector);
		}

		//////////////////////

		std::list<T> mvVector;
	};

	//---------------------------------

	template <class T, class CONT, class IT>
	class cSTLIterator : public iContainerIterator
	{
	public:
		///////////////////////////
		cSTLIterator(CONT *apCont)
		{
			mpCont = apCont;
			mIt = mpCont->begin();

		}

		///////////////////////////

		bool HasNext()
		{
			return mIt != mpCont->end();
		}

		void* NextPtr()
		{
			if(mIt == mpCont->end())
				return NULL;
			else
			{
				T &temp = const_cast<T &> (*mIt);
				mIt++;
				return &temp;
			}
		}

		///////////////////////////

		T Next()
		{
			if(mIt == mpCont->end())
				return NULL;
			else
			{
				T &temp = const_cast<T &> (*mIt);
				mIt++;
				return temp;
			}
		}

		///////////////////////////

		T PeekNext()
		{
			if(mIt == mpCont->end())
				return NULL;
			else{
				return *mIt;
			}
		}

		///////////////////////////

	private:
		IT mIt;
		CONT* mpCont;

	};

	//--------------------------------------------------------

	template <class T, class CONT, class IT>
	class cSTLMapIterator : public iContainerIterator
	{
	public:
		///////////////////////////
		cSTLMapIterator(CONT *apCont)
		{
			mpCont = apCont;
			mIt = mpCont->begin();

		}

		///////////////////////////

		bool HasNext()
		{
			return mIt != mpCont->end();
		}

		void* NextPtr()
		{
			if(mIt == mpCont->end())
				return NULL;
			else
			{
				T& temp = mIt->second;
				mIt++;
				return &temp;
			}
		}

		///////////////////////////

		T Next()
		{
			if(mIt == mpCont->end())
				return NULL;
			else
			{
				T temp = mIt->second;
				mIt++;
				return temp;
			}
		}

		///////////////////////////

		T PeekNext()
		{
			if(mIt == mpCont->end())
				return NULL;
			else{
				return mIt->second;
			}
		}

		///////////////////////////

	private:
		IT mIt;
		CONT* mpCont;

	};


}
#endif // HPL_CONTAINER_H

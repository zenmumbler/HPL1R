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
#ifndef HPL_SYSTEM_TYPES_H
#define HPL_SYSTEM_TYPES_H

#include <cstdio>
#include <string>
#include <list>
#include <set>
#include <vector>

#include "system/Container.h"

namespace hpl {

	//////////////////////////////////////////////////
	///////// TYPES //////////////////////////////////
	//////////////////////////////////////////////////

	#define _W(t) L ## t

	//--------------------------------------------------------

	enum eSystemPath
	{
		eSystemPath_Personal,
		eSystemPath_LastEnum
	};

	//--------------------------------------------------------


	typedef unsigned int tFlag;

	typedef std::string tString;

	typedef std::list<tString> tStringList;
	typedef tStringList::iterator tStringListIt;

	typedef std::vector<tString> tStringVec;
	typedef tStringVec::iterator tStringVecIt;

	typedef std::set<tString> tStringSet;
	typedef tStringSet::iterator tStringSetIt;


	//--------------------------------------------------------

	typedef std::wstring tWString;
	typedef std::list<tWString> tWStringList;
	typedef tWStringList::iterator tWStringListIt;

	typedef std::vector<tWString> tWStringVec;
	typedef tWStringVec::iterator tWStringVecIt;

	typedef std::set<tWString> tWStringSet;
	typedef tWStringSet::iterator tWStringSetIt;

	//--------------------------------------------------------


	typedef std::vector<unsigned int> tUIntVec;
	typedef tUIntVec::iterator tUIntVecIt;

	typedef std::vector<int> tIntVec;
	typedef tIntVec::iterator tIntVecIt;

	typedef std::vector<int> tIntList;
	typedef tIntVec::iterator tIntListIt;

	typedef std::vector<float> tFloatVec;
	typedef tFloatVec::iterator tFloatVecIt;

	typedef std::list<unsigned int> tUIntList;
	typedef tUIntList::iterator tUIntListIt;


	typedef enum
	{
		eMsgBoxType_Info,
		eMsgBoxType_Error,
		eMsgBoxType_Warning,
		eMsgBoxType_Default
	} eMsgBoxType;


	//////////////////////////////////////////////////
	///////// DEFINES ///////////////////////////////
	//////////////////////////////////////////////////

	//--------------------------------------------------------

	#define STLCallInAll( ContType, aCont, aFunc )				\
	{															\
		##ContType##::iterator it = ##aCont##.begin();			\
		for(; it != ##aCont##.end(); ++it) (*it)->##aFunc##;	\
	}


	//--------------------------------------------------------

	//////////////////////////////////////////////////
	///////// CLASS //////////////////////////////
	//////////////////////////////////////////////////

	//--------------------------------------------------------

	class cDate
	{
	public:
		int seconds;
		int minutes;
		int hours;
		int month_day;
		int month;
		int year;
		int week_day;
		int year_day;

		tString ToString()
		{
			char buff[256];

			sprintf(buff,"%d/%d-%d %d:%d:%d",month_day,month,1900+year,hours, minutes, seconds);

			return buff;
		}

		bool operator==(const cDate &aDate) const
		{
			if(	seconds == aDate.seconds &&
				minutes == aDate.minutes &&
				hours == aDate.hours &&
				month_day == aDate.month_day &&
				month == aDate.month &&
				year == aDate.year)
				{
					return true;
				}

			return false;
		}

		bool operator!=(const cDate &aDate) const
		{
			return !(*this == aDate);
		}

		bool operator>(const cDate &aDate) const
		{
			if(year > aDate.year) return true;
			else if(year < aDate.year) return false;

			if(month > aDate.month) return true;
			else if(month < aDate.month) return false;

			if(month_day > aDate.month_day) return true;
			else if(month_day < aDate.month_day) return false;

			if(hours > aDate.hours) return true;
			else if(hours < aDate.hours) return false;

			if(minutes > aDate.minutes) return true;
			else if(minutes < aDate.minutes) return false;

			if(seconds > aDate.seconds) return true;
			else if(seconds < aDate.seconds) return false;

			return false;
		}

		bool operator<(const cDate &aDate) const
		{
			if(year < aDate.year) return true;
			else if(year > aDate.year) return false;

			if(month < aDate.month) return true;
			else if(month > aDate.month) return false;

			if(month_day < aDate.month_day) return true;
			else if(month_day > aDate.month_day) return false;

			if(hours < aDate.hours) return true;
			else if(hours > aDate.hours) return false;

			if(minutes < aDate.minutes) return true;
			else if(minutes > aDate.minutes) return false;

			if(seconds < aDate.seconds) return true;
			else if(seconds > aDate.seconds) return false;

			return false;
		}
	};

	//----------------------------------------------------------


	//////////////////////////////////////////////////
	///////// TEMPLATES //////////////////////////////
	//////////////////////////////////////////////////

	//--------------------------------------------------------

	template <class CONT,class T>
	void STLFindAndRemove(CONT &aCont, T* pObject)
	{
		typename CONT::iterator it = aCont.begin();
		for(;it != aCont.end();it++)
		{
			if(*it == pObject){
				aCont.erase(it);
			}
		}
	}

	//--------------------------------------------------------

	template <class CONT,class T>
	void STLFindAndDelete(CONT &aCont, T* pObject)
	{
		typename CONT::iterator it = aCont.begin();
		for(;it != aCont.end();it++)
		{
			if(*it == pObject){
				aCont.erase(it);
				break;
			}
		}
		hplDelete(pObject);
	}

	//--------------------------------------------------------

	template <class CONT>
	void* STLFindByName(CONT &aCont,const tString& asName )
	{
		typename CONT::iterator it = aCont.begin();
		for(;it != aCont.end();it++)
		{
			if((*it)->GetName() == asName){
				return *it;
			}
		}
		return NULL;
	}

	//--------------------------------------------------------

	template <class T>
	void STLDeleteAll(T &aCont){
		typename T::iterator it = aCont.begin();
		for(;it != aCont.end();it++)
		{
			hplDelete(*it);
		}
		aCont.clear();
	}

	//--------------------------------------------------------

	template <class T>
	void STLMapDeleteAll(T &aCont){
		typename T::iterator it = aCont.begin();
		for(;it != aCont.end();it++)
		{
			hplDelete(it->second);
		}
		aCont.clear();
	}

	//--------------------------------------------------------

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

};
#endif // HPL_SYSTEM_TYPES_H

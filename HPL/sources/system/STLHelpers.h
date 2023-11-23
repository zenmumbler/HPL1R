/*
 * 2023 by zenmumbler
 * This file is part of Rehatched
 */

#ifndef HPL_STL_HELPERS_H
#define HPL_STL_HELPERS_H

namespace hpl {

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
		delete pObject;
	}

	//--------------------------------------------------------

	template <class CONT, class STR, class ITEM = typename CONT::value_type>
	ITEM STLFindByName(CONT &cont,const STR& asName)
	{
		for (auto item : cont)
		{
			if (item->GetName() == asName) {
				return item;
			}
		}
		return nullptr;
	}

	//--------------------------------------------------------

	template <class T>
	void STLDeleteAll(T &aCont){
		typename T::iterator it = aCont.begin();
		for(;it != aCont.end();it++)
		{
			delete *it;
		}
		aCont.clear();
	}

	//--------------------------------------------------------

	template <class T>
	void STLMapDeleteAll(T &aCont){
		typename T::iterator it = aCont.begin();
		for(;it != aCont.end();it++)
		{
			delete it->second;
		}
		aCont.clear();
	}

}
#endif /* HPL_STL_HELPERS_H */

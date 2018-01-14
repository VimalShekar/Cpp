/*++
Class: CNamedObjectCache<T>
Description:
A simple cache to keep objects which have string names and easily look them up by name. 
Internally an std::map stores the object and an std::queue maintains the order of insertion.

When size of the queue reaches m_MaxObjects, we clear the top 10-15 elements of the queue to make space.

TODO: Add a synchronization mechanism for multi threading support.

--*/

#pragma once
#include <map>
#include <queue>
#include <iostream>
#include "tstring.h"
#include "stringpatterns.h"

template <class T>
class CNamedObjectCache
{
	//-- max number of objects to be stored
	size_t m_MaxObjects;

	//-- for multi-threaded access
	//SRWLOCK m_RWLock;

	//-- stores order of insertion
	std::queue<tstring> m_Names;

	//-- stores actual elements in search optmised manner
	std::map<tstring, T> m_ObjectCache;

	//-- trims the cache by 15 elements, removing the ones that were inserted first.
	//-- This is only called from within AddObjectToCache, while lock is held in exclusive.
	void TrimCache();


public:
	//-- Defaults to storing 100 objects	
	CNamedObjectCache(size_t MaxObjects = 100);

	~CNamedObjectCache();

	//-- get number of elements currently cached	
	size_t GetNumCachedElements();

	//-- adds an elements to the cache	
	bool AddObjectToCache(tstring Name, T Object);

	//-- empties the cache	
	void ClearCache();

	//-- Gets a cached object, returns false if object not found in cache.
	bool GetCachedObject(tstring Name, T &Object);

};



/////////////////////////////////////////
// Definitions for CNamedObjectCache members
/////////////////////////////////////////
template<class T>
CNamedObjectCache<T>::CNamedObjectCache(size_t MaxObjects)
{
	m_MaxObjects = MaxObjects;
}


template<class T>
CNamedObjectCache<T>::~CNamedObjectCache()
{
	m_ObjectCache.clear();
	m_Names = {};
}


template<class T>
size_t CNamedObjectCache<T>::GetNumCachedElements()
{
	return m_Names.size();
}


template<class T>
void CNamedObjectCache<T>::ClearCache()
{
	m_ObjectCache.clear();
	m_Names.clear();
}


template<class T>
void CNamedObjectCache<T>::TrimCache()
{
	std::map<tstring, T>::iterator mPos;
	while (m_Names.size() > (m_MaxObjects - 10))
	{
		tstring Name = m_Names.front();
		m_Names.pop();
		mPos = m_ObjectCache.find(Name);
		if (mPos != m_ObjectCache.end())
		{
			m_ObjectCache.erase(mPos);
		}
	}
}


template<class T>
bool CNamedObjectCache<T>::AddObjectToCache(tstring Name, T Object)
{
	std::map<tstring, T>::iterator mPos;

	if (!IfStringNullOrEmpty(Name))
	{
		mPos = m_ObjectCache.find(Name);
		if (mPos != m_ObjectCache.end())
		{
			//-- replace object in cache operation, but we dont update its order of creation (for now)
			m_ObjectCache[Name] = Object;
		}
		else
		{
			//-- insert to cache
			m_ObjectCache.insert({ Name, Object });
			m_Names.push(Name);

			if (m_Names.size() > m_MaxObjects)
			{
				TrimCache(); 
			}
		}
		return true;
	}
	return false;
}


template<class T>
bool CNamedObjectCache<T>::GetCachedObject(tstring Name, T & Object)
{
	std::map<tstring, T>::iterator mPos;

	if (!IfStringNullOrEmpty(Name))
	{
		mPos = m_ObjectCache.find(Name);
		if (mPos != m_ObjectCache.end())
		{
			//-- we dont keep track of how often an object is accessed.
			Object = m_ObjectCache[Name];
			return true;
		}
	}
	return false;
}

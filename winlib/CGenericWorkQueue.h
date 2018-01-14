#pragma once
#include <deque> 
#include <mutex>
#include <vector>
#include <crtdbg.h>
#include <stdio.h>

#ifdef __linux__

#elif _WIN32
#include <Windows.h>
#endif

/*- ERROR CODES -*/
#define TPERROR_SUCCESS					0L
#define TPERROR_NOT_READY				1 + TPERROR_SUCCESS
#define TPERROR_NOT_INITIALISED			1 + TPERROR_NOT_READY
#define TPERROR_INVALID_PARAMETERS		1 + TPERROR_NOT_INITIALISED
#define TPERROR_						1 + TPERROR_INVALID_PARAMETERS


///////////////////////////////////////////////////////
// CGenericWorkQueue template class implementation
//Be aware of:
//https://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
///////////////////////////////////////////////////////

typedef void (*PCBACK_FN)(void *object);

/*- Generic work queue -*/
template<class Job> class CGenericWorkQueue {
private:
	std::deque<Job> m_WorkQueue;
	std::mutex *m_Lock;
	PCBACK_FN m_pInsertCallback, m_pRemoveCallback;

public:
	CGenericWorkQueue()
	{
		m_Lock = new std::mutex();
		m_pInsertCallback = m_pRemoveCallback = NULL;
	}


	~CGenericWorkQueue()
	{
		delete m_Lock;
		m_Lock = NULL;
		m_pInsertCallback = m_pRemoveCallback = NULL;
	}


	void LockQueue()
	{
		_ASSERT(m_Lock == NULL); // lock was not initialised?
		m_Lock->lock();
	}

	
	bool TryLockQueue()
	{
		bool bRetval = false;
		_ASSERT(m_Lock == NULL); // lock was not initialised?
		bRetval = m_Lock->try_lock();
		return bRetval;
	}

	
	void UnlockQueue()
	{
		_ASSERT(m_Lock == NULL); // lock was not initialised?
		m_Lock->unlock();
	}
	
	void InsertWorkItem(Job WorkItem)
	{
		//LockQueue();
		m_WorkQueue.push_back(WorkItem);
		if(m_pInsertCallback)
		{
			m_pInsertCallback();
		}
		//UnlockQueue();		
	}
	
	Job RemoveWorkItem()
	{
		//LockQueue();
		Job& Item = m_WorkQueue.front();
		m_WorkQueue.pop_front();		
		if(m_pRemoveCallback)
		{
			m_pRemoveCallback();
		}
		//UnlockQueue();
		return Item;
	}
	
	bool IsQueueEmpty()
	{
		return (m_WorkQueue.size() == 0);
	}
	
	size_t GetNumQueuedItems()
	{
		return m_WorkQueue.size();
	}
	
	void RegisterCallbacks(PCBACK_FN pInsertCallback, PCBACK_FN pRemoveCallback)
	{
		//TODO : test access
		m_pInsertCallback = pInsertCallback;
		m_pRemoveCallback = pRemoveCallback;
	}


};



DWORD WINAPI PoolManagerThread(LPVOID lpThreadParameter);



///////////////////////////////////////////////////////
// CThreadPoolManager class 
// Generic configurable worker threads
///////////////////////////////////////////////////////

/*- Generic worker threads managing the workqueue -*/
class CThreadPoolManager {
private:
	bool m_bThreadPoolInitialised;
	bool m_bNeedMoreThreads;
	bool m_bTerminateByForce;
	bool m_bAbnormalCondition;
	size_t m_MaxWorkers;
	size_t m_MinWorkers;
	std::vector<HANDLE> m_ThreadHandles;
	LPTHREAD_START_ROUTINE m_WorkerThreadRoutine;
	LPTHREAD_START_ROUTINE m_ThreadBalancerRoutine;
	HANDLE hShutDown;
	HANDLE hBalancer;

	//-- Create workers
	bool CreateWorkerThread();

	//-- Trim the thread pool
	void TrimThreadPool();

	//-- Terminated all threads
	bool TerminateAllThreads();

public:
	CThreadPoolManager()
	{
		m_MaxWorkers = 0;
		m_MinWorkers = 0;
		m_bThreadPoolInitialised = m_bNeedMoreThreads = false;
		m_WorkerThreadRoutine = m_ThreadBalancerRoutine = NULL;
		hBalancer = hShutDown = NULL;
		m_ThreadHandles.clear();
	}

	~CThreadPoolManager()
	{
		m_MaxWorkers = 0;
		m_MinWorkers = 0;
		m_bThreadPoolInitialised = m_bNeedMoreThreads = false;
		m_WorkerThreadRoutine = m_ThreadBalancerRoutine = NULL;
		ShutdownThreadpool(true);
		hShutDown = NULL;
		hBalancer = NULL;
		m_ThreadHandles.clear();
	}

	//-- Initialize the threadpool
	bool InitThreadPool(LPTHREAD_START_ROUTINE pThreadFn, size_t MinWorkers = 2, size_t MaxWorkers = 50);

	//-- Checks if thread pool is initialised
	bool IsThreadPoolInitialised();

	//-- Create additional worker thread
	void CreateAdditionalWorkerThread();

	//-- Get number of worker threads
	size_t GetNumWorkerThreads();

	//-- Blocking call.
	void ShutdownThreadpool(bool bTerminateByForce = false);

	//-- Maintence task called from PoolManagerThread
	void WorkerThreadMaintenenceTask();
};

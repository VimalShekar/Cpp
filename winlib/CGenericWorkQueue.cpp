#include "CGenericWorkQueue.h"
#include <stdio.h>





bool CThreadPoolManager::InitThreadPool(LPTHREAD_START_ROUTINE pThreadFn, size_t MinWorkers, size_t MaxWorkers)
{
	m_bThreadPoolInitialised = false;

	if (pThreadFn != NULL)
	{
		m_MinWorkers = MinWorkers;
		m_MaxWorkers = MaxWorkers;
		m_WorkerThreadRoutine = pThreadFn;
		m_ThreadBalancerRoutine = PoolManagerThread;
		hShutDown = NULL;


		//-- Ensure that minworkers is < maxworkers, and both are > 0
		if (m_MinWorkers <= 0)
		{
			m_MinWorkers = 1;
		}

		if (m_MaxWorkers <= 0)
		{
			m_MaxWorkers = m_MinWorkers;
		}

		if (m_MinWorkers > m_MaxWorkers)
		{
			size_t currWorkers = m_MinWorkers;
			m_MinWorkers = m_MaxWorkers;
			m_MaxWorkers = currWorkers;
		}

		hShutDown = CreateEvent(NULL, // LPSECURITY_ATTRIBUTES default
			TRUE,   // Manual Reset
			FALSE,  // Initial state = not set
			NULL);  // Unnamed event
		if (hShutDown != NULL)
		{
			printf("\nInitThreadPool:ShutDown Event created successfully");

			hBalancer = CreateThread(NULL, 0, PoolManagerThread, (void*)this, 0, NULL);
			if (hBalancer != NULL)
			{
				//-- if we get here - we consider that threadpool has been initialised
				printf("\nInitThreadPool:Balancer Thread created successfully");
				m_bThreadPoolInitialised = true;
			}
			else {
				printf("\nInitThreadPool:Error:Balancer Thread creation failed: %x", GetLastError());
				CloseHandle(hShutDown);
				hShutDown = NULL;
			}
		}
		else {
			printf("\nInitThreadPool:InitThreadPool:Error:ShutDown Event creation failed: %x", GetLastError());
		}
	}
	else {
		printf("\nInitThreadPool:Error:Worker Thread function was empty...");
	}

	return m_bThreadPoolInitialised;
}

bool CThreadPoolManager::IsThreadPoolInitialised()
{
	if (m_bThreadPoolInitialised)
	{	//-- if m_bThreadPoolInitialised is true, also check if the events and handles are not set to null
		return ((m_WorkerThreadRoutine != NULL) && (m_ThreadBalancerRoutine != NULL) && (hShutDown != NULL) && (hBalancer != NULL));
	}
	return m_bThreadPoolInitialised;
}

void CThreadPoolManager::CreateAdditionalWorkerThread()
{
	m_bNeedMoreThreads = true;
}

size_t CThreadPoolManager::GetNumWorkerThreads()
{
	return m_ThreadHandles.size();
}


void CThreadPoolManager::ShutdownThreadpool(bool bTerminateByForce)
{
	SetEvent(hShutDown);
	m_bTerminateByForce = bTerminateByForce;
}



//-- Create additional worker thread
bool CThreadPoolManager::CreateWorkerThread()
{
	HANDLE hThread = NULL;
	bool bRetval = false;

	if (IsThreadPoolInitialised())
	{		
		if (GetNumWorkerThreads() < m_MaxWorkers)
		{
			hThread = CreateThread(NULL, 0, m_WorkerThreadRoutine, (void*)this, 0, NULL);
			if (hThread != NULL)
			{
				printf("\nNew worker thread created");
				m_ThreadHandles.push_back(hThread);
				bRetval = true;
			}
			else {
				printf("\nError: Failed to create new Worker Thread. Error: %x", GetLastError());
			}
		}
		else {
			printf("\nError: There are already %d threads in the threadpool. Will not create additional threads", GetNumWorkerThreads());
		}
	}
	else {
		printf("\nError:Thread pool has not been initialised");
	}
	return bRetval;
}


void CThreadPoolManager::TrimThreadPool()
{
	std::vector<HANDLE>::iterator pos;
	DWORD dwExitCode = 0;
	bool bAllWorkersExited = false, bBalancerExited = false;

	if (!IsThreadPoolInitialised())
	{
		printf("\nTrimThreadPool:Error:Thread pool was not initialised");
		return;
	}

	//-- Parse through the list of thread handles
	for (pos = m_ThreadHandles.begin(); pos != m_ThreadHandles.end(); pos++)
	{
		HANDLE currHandle = *pos;

		if (currHandle == NULL)
		{
			m_ThreadHandles.erase(pos);
			continue;
		}

		//-- Get the exit code of the thread
		if (GetExitCodeThread(currHandle, &dwExitCode))
		{
			if (dwExitCode != STILL_ACTIVE) //-- this means thread has terminated already
			{
				printf("\nTrimThreadPool:Thread with handle %p has already terminated with exit code %x", currHandle, dwExitCode);
				CloseHandle(currHandle);
				m_ThreadHandles.erase(pos);
				currHandle = NULL;
				continue;
			}
		}
		else {
			printf("\nTrimThreadPool:Failed to get exit status for thread with handle :%p, Error:%x", currHandle, GetLastError());
			CloseHandle(currHandle);
			m_ThreadHandles.erase(pos);
			m_bAbnormalCondition = true;
			continue;
		}
	}

	printf("\nTrimThreadPool: %d threads are currently active", m_ThreadHandles.size());
}


//-- This is a Blocking call.
bool CThreadPoolManager::TerminateAllThreads()
{
	std::vector<HANDLE>::iterator pos;
	DWORD dwExitCode = 0, dwWait = 0;
	bool bAllWorkersExited = false, bBalancerExited = false;
	size_t currWorkers;

	if (!IsThreadPoolInitialised())
	{
		printf("\nError:Thread pool was not initialised");
		return false;
	}

	//-- While these conditions are true, repeat
	while ( !(bAllWorkersExited && bBalancerExited) )
	{
		TrimThreadPool();
		currWorkers = GetNumWorkerThreads();
		if (currWorkers == 0)
		{
			// ( wait_time = wait until balancer has exited )
			bAllWorkersExited = true;
			dwWait = INFINITE;
		}
		else {
			// ( wait_time = MinWorkers * MaxWorkers * number of active worker threads )
			dwWait = m_MinWorkers * m_MaxWorkers * currWorkers;
		}

		//-- Get the exit code of the balancer thread
		dwExitCode = WaitForSingleObject(hBalancer, dwWait);
		if (GetExitCodeThread(hBalancer, &dwExitCode))
		{
			if (dwExitCode != STILL_ACTIVE) //-- this means thread has terminated already
			{
				printf("\nBalancer thread has exited: %d", dwExitCode);
				bBalancerExited = true;				
			}
		}

		if (m_bTerminateByForce)
		{
			//-- Parse through the list of thread handles
			for (pos = m_ThreadHandles.begin(); pos != m_ThreadHandles.end(); pos++)
			{
				HANDLE currHandle = *pos;

				if (currHandle == NULL)
				{
					m_ThreadHandles.erase(pos);
					continue;
				}
				else {
					if (TerminateThread(currHandle, 0))
					{
						printf("\nTerminated thread with handle %p by force", currHandle);
						CloseHandle(currHandle);
						m_ThreadHandles.erase(pos);
						currHandle = NULL;
					}
					else {
						printf("\nFailed to terminate thread with handle %p. Error:%x", currHandle, GetLastError());
						CloseHandle(currHandle);
						m_ThreadHandles.erase(pos);
						currHandle = NULL;
					}					
				}
			} //-- for loop
		}


		if (m_bAbnormalCondition)
		{
			break;
		}

	}//--while loop
		
	return (bAllWorkersExited && bBalancerExited);
}



//-- Actual Pool management is done here
void CThreadPoolManager::WorkerThreadMaintenenceTask()
{
	std::vector<HANDLE>::iterator pos;
	DWORD dwExitCode = 0, dwWait = 0;
	size_t currWorkers = 0, harmonicmean = m_MinWorkers;
	bool bShutdownSignalled = false, bThreadTrimmed = false;

	printf("\nWorkerThreadMaintenenceTask:Entry...");

	//-- If the thread pool was never initialised, nothing to do here, just return
	if (!IsThreadPoolInitialised())
	{
		printf("\nWorkerThreadMaintenenceTask:Error:Thread pool was not initialised");
		return;
	}


	while (true)
	{
		if (bShutdownSignalled)
		{
			printf("\nWorkerThreadMaintenenceTask: Shutdown signalled...");
			TerminateAllThreads();
			break;
		}

		if (m_bNeedMoreThreads)
		{
			printf("\nWorkerThreadMaintenenceTask: Create workers...");
			CreateWorkerThread();
			m_bNeedMoreThreads = false;
		}

		TrimThreadPool();
		currWorkers = GetNumWorkerThreads();
		while (currWorkers < m_MinWorkers)  //-- Aggressively create threads to maintain m_MinWorkers
		{
			printf("\nWorkerThreadMaintenenceTask: Create workers to meet min : %d...", m_MinWorkers);
			CreateWorkerThread();
			m_bNeedMoreThreads = false;
		}
				

		//-- Wait on shutdown event with a wait time proportional to # of current Workers, so that it scales well.		
		//-- If 0 current worker threads, we immediately start creating threads. 
		//-- If more current worker threads, we wait more before creating new ones. CPU usage is lesser when we wait.
		currWorkers = GetNumWorkerThreads();
		dwWait = m_MinWorkers * m_MaxWorkers * currWorkers;		// ( wait_time = MinWorkers * MaxWorkers * number of active worker threads )
		dwExitCode = WaitForSingleObject(hShutDown, dwWait);
		switch (dwExitCode)
		{
			// The shutdown event was signalled
		case WAIT_OBJECT_0:
			printf("\nWorkerThreadMaintenenceTask: Its time to exit the loop.");
			bShutdownSignalled = true;
			break;

			// No Shutdown event
		case WAIT_ABANDONED:
		default:			
			TrimThreadPool();
			currWorkers = GetNumWorkerThreads();
			printf("\nWorkerThreadMaintenenceTask:There are currently %d number of workers.", currWorkers);

			//-- Since shutdown hasnt been triggered, we calculate the harmonic mean of min, max and current workers.
			//-- We add new threads if current workers is < harmonic mean, and harmonic mean is always < max and is slightly > current
			//-- It normalises at around 70-80% This way, we dont rapidly increase the number of workers.
			harmonicmean = 3 / ((1 / m_MinWorkers) + (1 / m_MaxWorkers) + (1 / currWorkers));  // this will always be < m_MaxWorkers
			if (currWorkers < harmonicmean)
			{
				m_bNeedMoreThreads = true;
			}
		}

		printf("\nWorkerThreadMaintenenceTask: Sleep for %d...", dwWait);
		Sleep(dwWait);
	}

	printf("\nWorkerThreadMaintenenceTask:Exit...");	

}


//-- Pool Manager thread simply calls the Worker Maintenence task
DWORD WINAPI PoolManagerThread(LPVOID lpThreadParameter)
{
	CThreadPoolManager *pThreadPoolManager = (CThreadPoolManager*)lpThreadParameter;
	DWORD dwRetvalue = TPERROR_SUCCESS;

	if (pThreadPoolManager != NULL)
	{
		if (pThreadPoolManager->IsThreadPoolInitialised())
		{
			pThreadPoolManager->WorkerThreadMaintenenceTask();
			dwRetvalue = TPERROR_SUCCESS;
		}
		else {
			printf("\nPoolManagerThread:Error:Threadpool manager was not initialised");
			dwRetvalue = TPERROR_NOT_INITIALISED;
		}
	}
	else {
		printf("\nPoolManagerThread:Error:Threadpool manager was NULL");
		dwRetvalue = TPERROR_INVALID_PARAMETERS;
	}
	return dwRetvalue;
}
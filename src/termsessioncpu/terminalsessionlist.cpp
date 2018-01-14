/*++
File: terminalsessionlist.cpp

Author: vimalsh@live.com

Type: standalone exe, built from this file.

Description:
Demo/Sample to enumerate terminal sessions on the machine.
Then enumerate WMI class to get instantaneous values of CPU usage associated with the terminal sessions.

--*/

#define _WIN32_DCOM

#include <iostream>
#include <stdio.h>
#include <map>
using namespace std;

#include <windows.h>
#include <Wtsapi32.h> 
#include <atlbase.h>
#include <Wbemidl.h>

#pragma comment(lib, "wtsapi32.lib")
#pragma comment(lib, "wbemuuid.lib")

#ifdef UNICODE 
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif
 

int main(int argc, char **argv)
{
    HANDLE hServer = NULL;
    WTS_SESSION_INFO_1 *pSessionInfo = NULL;
    DWORD dwCount = 0, dwError = ERROR_SUCCESS, dwLevel = 1;
    std::map<tstring, tstring> SessionMap;
    std::map<tstring, tstring>::iterator session_it;
    HRESULT hr;

    CComBSTR bstrNameSpace ("\\\\.\\root\\cimv2");
    IWbemLocator *pWbemLocator = NULL;
    IWbemServices* pNameSpace = NULL;
    IWbemRefresher *pRefresher = NULL;   
    
    IWbemHiPerfEnum* pEnum = NULL;
    
    
    long lID;
    IWbemConfigureRefresher* pConfig;
    
    
    //-- Initialize COM and set Security 
    if (FAILED (hr = CoInitializeEx(NULL,COINIT_MULTITHREADED)))
    {
        printf("\n COM initialization failed with error: %d", hr);
        goto __cleanup;
    }

    if (FAILED (hr = CoInitializeSecurity( NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_NONE, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, 0)))
    {
        printf("\n COM Security initialization failed with error: %d", hr);
        goto __cleanup;
    }

    if (FAILED (hr = CoCreateInstance( CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (void**) &pWbemLocator)))
    {
        printf("\n Failed to create instance of IWbemLocator. Error: %d", hr);
        goto __cleanup;
    }

    //-- Connect to Root\CIMv2 namespace
    if (FAILED (hr = pWbemLocator->ConnectServer( bstrNameSpace, NULL, NULL, NULL, 0L, NULL, NULL, &pNameSpace)))
    {
        printf("\nNot able to connect to the namespace: %d", hr);
        goto __cleanup;
    }

    //-- Create a WbemRefresher instance
    if (FAILED (hr = CoCreateInstance( CLSID_WbemRefresher, NULL, CLSCTX_INPROC_SERVER, IID_IWbemRefresher,  (void**) &pRefresher)))
    {
        printf("\n failed to create WbemRefresher object : %d", hr);
        goto __cleanup;
    }

    //-- Get the Config interface to configure the refresher
    if (FAILED (hr = pRefresher->QueryInterface(IID_IWbemConfigureRefresher, (void **)&pConfig)))
    {
        printf("\n failed to create IWbemConfigureRefresher object : %d", hr);
        goto __cleanup;
    }



    //-- Enumerate TS sessions
    if ( ! WTSEnumerateSessionsEx(WTS_CURRENT_SERVER_HANDLE , &dwLevel, 1, &pSessionInfo, &dwCount))
    {
        printf("\n failed to enumerate sessions : %d", GetLastError());
        goto __cleanup;
    }

    //-- get the session ID and user name mappings
    printf("\n There are %d sessions including session 0(Services session)", dwCount);
    for(DWORD index = 0; index < dwCount; index++)
    {
        WTS_SESSION_INFO_1 *pCurrentSession = pSessionInfo + index;
        tstring tstrUserName, tstrSessionName; 
        
        if(pCurrentSession->pUserName != NULL)
        {
            tstrUserName.assign(pCurrentSession->pUserName);
            tstrSessionName.assign(pCurrentSession->pSessionName);
            printf("\nSession ID: %d, SessionName: %TS, UserName: %TS", pCurrentSession->SessionId, pCurrentSession->pSessionName, pCurrentSession->pUserName);
            SessionMap[tstrSessionName] = tstrUserName;
        }        
    }



    //-- Now getting performance data: https://msdn.microsoft.com/en-us/library/aa384724(v=vs.85).aspx
    //-- Add an enumerator to the refresher.
    if (FAILED (hr = pConfig->AddEnum( pNameSpace, L"Win32_PerfFormattedData_TermService_TerminalServicesSession", 0, NULL, &pEnum, &lID)))
    {
        printf("\n Failed to add Win32_PerfFormattedData_TermService_TerminalServicesSession class to Refreshed config");
        goto __cleanup;
    }


    //-- Refresh to get the data
    {
        DWORD dwNumReturned = 0, dwNumObjects = 0;
        IWbemObjectAccess **apEnumAccess = NULL;
        CIMTYPE PercentPrivilegedTime_type, PercentProcessorTime_type, PercentUserTime_type, SessionName;
        long lPercentPrivilegedTime_value, lPercentProcessorTime_value, lPercentUserTime_value, lSessionName;  // handles
        unsigned __int64 ui64PercentPrivilegedTime_value = 0, ui64PercentProcessorTime_value = 0, ui64lPercentUserTime_value = 0;


        if (FAILED (hr =pRefresher->Refresh(0L)))
        {
            printf("\n Failed to refresh data: %d", hr);
            goto __cleanup;
        }

        
        //-- call once to get the size of buffers to be allocated
        hr = pEnum->GetObjects(0L, dwNumObjects, apEnumAccess, &dwNumReturned);
        if (hr == WBEM_E_BUFFER_TOO_SMALL && dwNumReturned > dwNumObjects)
        {
            //-- Allocate a bigger buffer and retry call.
            apEnumAccess = new IWbemObjectAccess*[dwNumReturned];
            if (NULL == apEnumAccess)
            {
                hr = E_OUTOFMEMORY;
                goto __cleanup;
            }

            SecureZeroMemory(apEnumAccess, dwNumReturned*sizeof(IWbemObjectAccess*));
            dwNumObjects = dwNumReturned;

            if (FAILED (hr = pEnum->GetObjects(0L, dwNumObjects, apEnumAccess, &dwNumReturned)))
            {
                printf("\n Failed to get objects: %d", hr);
                goto __cleanup;
            }
        } else {
            printf("\n Unexpected error");
            goto __cleanup;
        }

        //-- get the handle for the values
        apEnumAccess[0]->GetPropertyHandle( L"PercentPrivilegedTime", &PercentPrivilegedTime_type, &lPercentPrivilegedTime_value);
        apEnumAccess[0]->GetPropertyHandle( L"PercentProcessorTime", &PercentProcessorTime_type, &lPercentProcessorTime_value);
        apEnumAccess[0]->GetPropertyHandle( L"PercentUserTime", &PercentUserTime_type, &lPercentUserTime_value);
        apEnumAccess[0]->GetPropertyHandle( L"Name", &SessionName, &lSessionName);
/* 
        printf("\n Type is PercentPrivilegedTime_type: %d", PercentPrivilegedTime_type);
        printf("\n Type is PercentProcessorTime_type: %d", PercentProcessorTime_type);
        printf("\n Type is PercentUserTime_type: %d", PercentUserTime_type);

 */
        

        for (DWORD i = 0; i < dwNumReturned; i++)
        {
            long lbytesout;
            byte *pData = (byte*) malloc (255);
            std::wstring pName;
            tstring tstrSessionName;

            if(pData == NULL)
            {
                printf("\nMemory allocation failures"); break;
            }

            if (FAILED (hr = apEnumAccess[i]->ReadPropertyValue(lSessionName, 255, &lbytesout, pData)))
            {
                printf("\nUnexpected error retrieving Session name");
                break;
            } else {                
                pName.assign((wchar_t*)pData);
                tstrSessionName.assign(pName.begin(), pName.end());
                
                session_it = SessionMap.find(tstrSessionName);
                if(session_it != SessionMap.end())
                {
                    printf("\nSessionName:%TS - UserName : %TS", session_it->first.c_str(), session_it->second.c_str());
                }
                else {
                    printf("\n SessionName: %ws", pName.c_str());
                }
            }

            if (FAILED (hr = apEnumAccess[i]->ReadQWORD( lPercentPrivilegedTime_value, &ui64PercentPrivilegedTime_value)))
            {
                printf("\nUnexpected error retrieve PercentPrivilegedTime_type");
                break;
            } else {
                printf("\n\tPercentPrivilegedTime_value:%I64d", ui64PercentPrivilegedTime_value);
            }

            if (FAILED (hr = apEnumAccess[i]->ReadQWORD( lPercentProcessorTime_value, &ui64PercentProcessorTime_value)))
            {
                printf("\nUnexpected error retrieve lPercentProcessorTime_value");
                break;
            } else {
                printf("\n\tPercentProcessorTime_value: %I64d", ui64PercentProcessorTime_value);
            }

            if (FAILED (hr = apEnumAccess[i]->ReadQWORD( lPercentUserTime_value, &ui64lPercentUserTime_value)))
            {
                printf("\nUnexpected error retrieve PercentUserTime_value");
                break;
            } else {
                printf("\n\tPercentUserTime_value: %I64d", ui64lPercentUserTime_value);
            }

            if(pData != NULL)
            {
                free(pData);

            }

        }


        //-- cleanup apEnumAccess
        if (NULL != apEnumAccess)
        {
            for (DWORD i = 0; i < dwNumReturned; i++)
            {
                if (apEnumAccess[i] != NULL)
                {
                    apEnumAccess[i]->Release();
                    apEnumAccess[i] = NULL;
                }
            }
            delete [] apEnumAccess;
            apEnumAccess = NULL;
        }
    }


__cleanup:
    if(pSessionInfo)
    {
        WTSFreeMemoryEx(WTSTypeSessionInfoLevel1, pSessionInfo, dwCount);
    }

    if(hServer)
    {        
        WTSCloseServer(hServer);
    }
    
    if(pConfig)
    {
        pConfig->Release();
    }

    if(pNameSpace)
    {
        pNameSpace->Release();
    }
    
    if(pWbemLocator)
    {
        pWbemLocator->Release();
    }

    return 0;
}
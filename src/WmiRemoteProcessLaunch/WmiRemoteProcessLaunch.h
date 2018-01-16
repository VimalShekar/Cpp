#pragma once

#define _WIN32_DCOM
#define UNICODE
#include <iostream>
#include <stdio.h>

using namespace std;
#include <comdef.h>
#include <Wbemidl.h>
#include <wincred.h>
#include <strsafe.h>
#include <Windows.h>


/*++
Create a process on remote machine, given the IWbemservices pointer to an active WMI connection on the machine.
--*/
DWORD CreateProcessOnRemote(IWbemServices *pSvc = NULL, std::wstring wstrProcessName = L"", std::wstring wstrProcessArgs = L"");
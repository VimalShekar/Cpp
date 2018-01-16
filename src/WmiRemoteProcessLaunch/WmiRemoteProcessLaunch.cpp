#include "WmiDeploy.h"

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "credui.lib")
#pragma comment(lib, "comsuppw.lib")



/*-Reference : https://msdn.microsoft.com/en-us/library/aa390422(v=vs.85).aspx -*/
int main(int argc, wchar_t ** argv)
{
	HRESULT hres;
	IWbemLocator *pLoc = NULL;				//<-- WMI locator
	IWbemServices *pSvc = NULL;				//<-- once WMI is connected - this is the service pointer
	IEnumWbemClassObject* pEnumerator = NULL; //<-- used to enumerate WMI classes, and run queries
	IWbemClassObject *pclsObj = NULL;		//<-- pointer to the actual WMI class we enumerated
	ULONG uReturn = 0;
	COAUTHIDENTITY *userAcct = NULL;
	COAUTHIDENTITY authIdent = { 0 };

	/*-Wmi Connection parameters-*/
	std::wstring wstrUserName(L"testuser@testdom.com"), wstrPassword(L"Test@Pass1"), wstrDomainAuth(L"");
	std::wstring wstrMachineName(L"testmac1.testdom.com");
	std::wstring wstrConnectPath;
	

	//-- Initialize COM.
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
	{
		cout << "\nFailed to initialize COM library. Error code = 0x"
			<< hex << hres << endl;
		return 1;                  // Program has failed.
	}
	cout << "\nCOM initialised";

	//-- Set general COM security levels
	hres = CoInitializeSecurity(
		NULL,
		-1,                          // COM authentication
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
		RPC_C_IMP_LEVEL_IDENTIFY,    // Default Impersonation  
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities 
		NULL                         // Reserved
		);

	if (FAILED(hres))
	{
		cout << "\nFailed to initialize security. Error code = 0x"
			<< hex << hres << endl;
		CoUninitialize();
		return 1;                    // Program has failed.
	}
	cout << "\nCOM security initialised";

	//-- Obtain the WMI locator to pointer
	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hres))
	{
		cout << "\nFailed to create IWbemLocator object."
			<< " Err code = 0x"
			<< hex << hres << endl;
		CoUninitialize();
		return 1;                 // Program has failed.
	}
	cout << "\nWbemLocator initialised";


	//-- Preparing the connection parameters
	wstrConnectPath.assign(L"\\\\");
	wstrConnectPath.append(wstrMachineName);
	wstrConnectPath.append(L"\\root\\cimv2");

	//-- Connect to the remote root\cimv2 namespace and obtain pointer pSvc to make IWbemServices calls.
	hres = pLoc->ConnectServer(
		_bstr_t(wstrConnectPath.c_str()),
		_bstr_t(wstrUserName.c_str()),		// User name
		_bstr_t(wstrPassword.c_str()),		// User password
		NULL,								// Locale             
		NULL,								// Security flags
		NULL,	//_bstr_t(wstrDomainAuth.c_str()),	// Authority        
		NULL,                              // Context object 
		&pSvc                              // IWbemServices proxy
		);

	if (FAILED(hres))
	{
		cout << "\nCould not connect. Error code = 0x"
			<< hex << hres << endl;		
		goto cleanup;
	}
	cout << "\nWbemServices connected to remote machine";


	/*
		After you retrieve a pointer to an IWbemServices proxy, you must set the security on the proxy to 
		access WMI on the remote machine through the proxy. 
		This step is compulsary because IWbemServices proxy will only grants access to an out-of-process or remote object 
		if the security properties are correct.
		Ref: https://msdn.microsoft.com/en-us/library/aa393619(v=vs.85).aspx
	*/

	authIdent.PasswordLength = wcslen(wstrPassword.c_str());
	authIdent.Password = (USHORT*)wstrPassword.c_str();
	authIdent.UserLength = wcslen(wstrUserName.c_str());
	authIdent.User = (USHORT*)wstrUserName.c_str();
	authIdent.DomainLength = 0;   //<-- size of domain name
	authIdent.Domain = (USHORT*) NULL;   //<-- this should be domain name	
	authIdent.Flags = SEC_WINNT_AUTH_IDENTITY_UNICODE;
	userAcct = &authIdent;


	//-- Set security levels on the new WMI connection
	hres = CoSetProxyBlanket(
		pSvc,                           // Indicates the proxy to set
		RPC_C_AUTHN_DEFAULT,            // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_DEFAULT,            // RPC_C_AUTHZ_xxx
		COLE_DEFAULT_PRINCIPAL,         // Server principal name 
		RPC_C_AUTHN_LEVEL_PKT_PRIVACY,  // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE,    // RPC_C_IMP_LEVEL_xxx
		userAcct,                       // client identity  <--- ideally you have to create COAUTHIDENTITY  class 
		EOAC_NONE                       // proxy capabilities 
		);
	if (FAILED(hres))
	{
		cout << "\nCould not set proxy blanket security. Error code = 0x"
			<< hex << hres << endl;
		goto cleanup;
	}
	cout << "\nWbemServices CoSetProxyBlanket success";

	

	// Map a network share on the remote machine, this caches the connection for the next step
	hres = CreateProcessOnRemote(pSvc, L"cmd.exe", L"/C \"net use \\\\fs2.testdom.com\\MyShare /user:testuser@testdom.com Test@Pass1\""); 
	if (FAILED(hres))
	{
		cout << "\nCould not create process to map network share. Error code = 0x"
			<< hex << hres << endl;
		goto cleanup;
	}
	cout << "\n Create process returned " << hres;

	// Launch a payload process from the network share
	hres = CreateProcessOnRemote(pSvc, L"\\\\fs2.testdom.com\\MyShare\\payload.exe");
	if (FAILED(hres))
	{
		cout << "\nCould not create payload process. Error code = 0x"
			<< hex << hres << endl;
		goto cleanup;
	}
	cout << "\n Create process returned " << hres;

	hres = CreateProcessOnRemote(pSvc, L"cmd.exe", L"/C \"net use \\\\fs2.testdom.com\\MyShare /delete\""); 
	if (FAILED(hres))
	{
		cout << "\nCould not unmap network share. Error code = 0x"
			<< hex << hres << endl;
		goto cleanup;
	}
	cout << "\n Create process returned " << hres;

cleanup:
	if (pSvc != NULL)
	{
		pSvc->Release();
	}

	if (pLoc != NULL)
	{
		pLoc->Release();
	}

	if (pEnumerator != NULL)
	{
		pEnumerator->Release();
	}

	if (pclsObj != NULL)
	{
		pclsObj->Release();
	}

	CoUninitialize();
	return 0;
}




/*-Refernce : https://msdn.microsoft.com/en-us/library/aa390421(v=vs.85).aspx -*/
DWORD CreateProcessOnRemote(IWbemServices *pSvc, std::wstring wstrProcessName, std::wstring wstrProcessArgs)
{
	HRESULT hres = 0;
	BSTR MethodName = SysAllocString(L"Create");
	BSTR ClassName = SysAllocString(L"Win32_Process");
	IWbemClassObject* pClass = NULL;
	IWbemClassObject* pInParamsDefinition = NULL;
	IWbemClassObject* pClassInstance = NULL;
	IWbemClassObject* pOutParams = NULL;
	std::wstring wstrCommandLine = wstrProcessName + L" " + wstrProcessArgs;
	VARIANT varCommand;
	VARIANT varReturnValue;

	hres = pSvc->GetObject(ClassName, 0, NULL, &pClass, NULL);
	//-- if above call is success, we are able to enumerate Win32_Process class in root\cimv2 on the remote machine
	if (FAILED(hres))
	{
		cout << "\n pSvc->GetObject failed."
			<< " Error code = 0x"
			<< hex << hres << endl;
		goto cleanup;
	}

	hres = pClass->GetMethod(MethodName, 0, &pInParamsDefinition, NULL);
	//-- if above call is success, we have a pointer to the Create method of Win32_Process class
	if (FAILED(hres))
	{
		cout << "\n pClass->GetMethod failed."
			<< " Error code = 0x"
			<< hex << hres << endl;
		goto cleanup;
	}


	hres = pInParamsDefinition->SpawnInstance(0, &pClassInstance);
	//-- Spawn instance of the Win32_Process::Create Parameters.
	if (FAILED(hres))
	{
		cout << "\n pInParamsDefinition->SpawnInstance failed."
			<< " Error code = 0x"
			<< hex << hres << endl;
		goto cleanup;
	}

	// Create the values for the in-parameters	
	varCommand.vt = VT_BSTR;
	varCommand.bstrVal = _bstr_t(wstrCommandLine.c_str());
	//-- you can also pass CurrentDirectory and ProcessStartupInformation (Win32_ProcessStartup)
	//-- See https://msdn.microsoft.com/en-us/library/aa389388(v=vs.85).aspx


	//-- Store the value for the in-parameters
	hres = pClassInstance->Put(L"CommandLine", 0, &varCommand, 0);
	if (FAILED(hres))
	{
		cout << "\n   pClassInstance->Put failed."
			<< " Error code = 0x"
			<< hex << hres << endl;
		goto cleanup;
	}

	cout << "\nThe command is:" << wstrCommandLine.c_str() << endl;

	//-- Execute Method	
	hres = pSvc->ExecMethod(ClassName, MethodName, 0, NULL, pClassInstance, &pOutParams, NULL);
	//-- if this call is successfull, then we have executed the method on the remote machine
	if (FAILED(hres))
	{
		cout << "\n  pSvc->ExecMethod failed."
			<< " Error code = 0x"
			<< hex << hres << endl;
		goto cleanup;
	}

	hres = pOutParams->Get(_bstr_t(L"ReturnValue"), 0, &varReturnValue, NULL, 0);
	if (FAILED(hres))
	{
		cout << "\n   pOutParams->Get failed."
			<< " Error code = 0x"
			<< hex << hres << endl;
	}
	cout << "\n Process return value:" << varReturnValue.intVal; 

cleanup:
	VariantClear(&varCommand);
	SysFreeString(ClassName);
	SysFreeString(MethodName);

	if (pClass != NULL)
	{
		pClass->Release();
	}

	if (pClassInstance != NULL)
	{
		pClassInstance->Release();
	}

	if (pInParamsDefinition != NULL)
	{
		pInParamsDefinition->Release();
	}

	if (pOutParams != NULL)
	{
		pOutParams->Release();
	}

	return hres;
}


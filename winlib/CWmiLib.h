
#include "..\CommonLib\tstring.h"

using namespace CommonLib;

const tstring DEFAULT_NAMESPACE = TEXT("\\root\cimv2");


///////////////////////////////////
// CWmiConnector class
//-- Should store information about WMI connection
//-- Should provide methods to open, close and test connection
///////////////////////////////////
class CWmiConnector {
protected:
    tstring m_ServerName;
    tstring m_ServerName;
    tstring m_ServerName;

public:
    void OpenConnection();
    void CloseConnection();
    bool TestConnection();

    //! The local computer path.
	static const tstring LOCALHOST;
	
    //! The default namespace.
	static const tstring DEFAULT_NAMESPACE;
}
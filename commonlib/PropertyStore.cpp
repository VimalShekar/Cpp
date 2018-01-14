#include "PropertyStore.h"

CPropertyValue::CPropertyValue()
{
	type = null_type;
	NumChildren = 0;
}

CPropertyValue::~CPropertyValue()
{

}

CPropertyValue::CPropertyValue(bool b)
{
	type = bool_type;
	bValue = b;
}

CPropertyValue::CPropertyValue(int i)
{
	type = int_type;
	iValue = i;
}

CPropertyValue::CPropertyValue(double d)
{
	type = real_type;
	dValue = d;
}

CPropertyValue::CPropertyValue(std::string s)
{
	type = str_type;
	szValue.assign(s);
}

CPropertyValue::CPropertyValue(std::vector<bool> v)
{
	type = array_type;
	std::vector<bool>::iterator Pos;

	for (Pos = v.begin(); Pos != v.end(); Pos++)
	{
		CPropertyValue NewVal(*Pos);
		arValue.push_back(NewVal);
	}
}

CPropertyValue::CPropertyValue(std::vector<int> v)
{
	type = array_type;
	std::vector<int>::iterator Pos;

	for (Pos = v.begin(); Pos != v.end(); Pos++)
	{
		CPropertyValue NewVal(*Pos);
		arValue.push_back(NewVal);
	}
}

CPropertyValue::CPropertyValue(std::vector<double> v)
{
	type = array_type;
	std::vector<double>::iterator Pos;

	for (Pos = v.begin(); Pos != v.end(); Pos++)
	{
		CPropertyValue NewVal(*Pos);
		arValue.push_back(NewVal);
	}
}

CPropertyValue::CPropertyValue(std::vector<std::string> v)
{
	type = array_type;
	std::vector<std::string>::iterator Pos;

	for (Pos = v.begin(); Pos != v.end(); Pos++)
	{
		std::string NewString(*Pos);
		CPropertyValue NewVal(NewString);
		arValue.push_back(NewVal);
	}
}

PropValue_Type CPropertyValue::GetType()
{
	return type;
}

void CPropertyValue::SetType(PropValue_Type t)
{
	type = t;
}

void CPropertyValue::SetValue(bool b)
{
	type = bool_type;
	bValue = b;
}

void CPropertyValue::SetValue(int i)
{
	type = int_type;
	iValue = i;
}

void CPropertyValue::SetValue(double d)
{
	type = real_type;
	dValue = d;
}

void CPropertyValue::SetValue(std::string s)
{
	type = str_type;
	szValue.assign(s);
}

void CPropertyValue::SetValue(CPropertyValue s)
{
	type = s.GetType();
	switch (type)
	{
	case str_type:
		szValue = s.GetStringValue();
		break;

	case bool_type:
		bValue = s.GetBoolValue();
		break;

	case int_type:
		iValue = s.GetIntValue();
		break;

    case real_type:
        dValue = s.GetDoubleValue();
        break;

	case obj_type:
		chValue = s.chValue;
		break;

	case array_type:
		arValue = s.arValue;
		break;

    default:
	case null_type:
		break;
	}
}

void CPropertyValue::SetValue(std::vector<bool> v)
{
	type = array_type;
	std::vector<bool>::iterator Pos;

	for (Pos = v.begin(); Pos != v.end(); Pos++)
	{
		CPropertyValue NewVal(*Pos);
		arValue.push_back(NewVal);
	}
}

void CPropertyValue::SetValue(std::vector<int> v)
{
	type = array_type;
	std::vector<int>::iterator Pos;

	for (Pos = v.begin(); Pos != v.end(); Pos++)
	{
		CPropertyValue NewVal(*Pos);
		arValue.push_back(NewVal);
	}
}

void CPropertyValue::SetValue(std::vector<double> v)
{
	type = array_type;
	std::vector<double>::iterator Pos;

	for (Pos = v.begin(); Pos != v.end(); Pos++)
	{
		CPropertyValue NewVal(*Pos);
		arValue.push_back(NewVal);
	}
}

void CPropertyValue::SetValue(std::vector<std::string> v)
{
	type = array_type;
	std::vector<std::string>::iterator Pos;

	for (Pos = v.begin(); Pos != v.end(); Pos++)
	{
		std::string NewString(*Pos);
		CPropertyValue NewVal(NewString);
		arValue.push_back(NewVal);
	}
}

void CPropertyValue::SetValue(std::vector<CPropertyValue> v)
{
	type = array_type;
	std::vector<CPropertyValue>::iterator Pos;

	for (Pos = v.begin(); Pos != v.end(); Pos++)
	{
		CPropertyValue NewVal = *Pos;		
		arValue.push_back(NewVal);
	}
}

int CPropertyValue::GetIntValue()
{
	return iValue;
}

bool CPropertyValue::GetBoolValue()
{
	return bValue;
}

double CPropertyValue::GetDoubleValue()
{
    return dValue;
}

std::string CPropertyValue::GetStringValue()
{
	return szValue;
}


bool CPropertyValue::AddChild(std::string Name, CPropertyValue val)
{
	type = obj_type;
	std::map<std::string, CPropertyValue>::iterator Pos;

	Pos = chValue.find(Name);
	if (Pos != chValue.end())
	{
		chValue[Name] = val;
	}
	else
	{
		chValue.insert({ Name, val });
	}

	NumChildren++;
	return true;
}


void CPropertyValue::RemoveChild(std::string Name)
{
	std::map<std::string, CPropertyValue>::iterator Pos;

	Pos = chValue.find(Name);
	if (Pos != chValue.end())
	{
		chValue.erase(Pos);
	}
}


bool CPropertyValue::GetChild(std::string Name, CPropertyValue &valRef)
{
	std::map<std::string, CPropertyValue>::iterator Pos;

	Pos = chValue.find(Name);
	if (Pos != chValue.end())
	{
		valRef = Pos->second;
		return true;
	}
	return false;
}


bool CPropertyValue::GetFirstArrayValue(CPropertyValue &valRef)
{
	arrPos = arValue.begin();
	if (arrPos != arValue.end())
	{
		valRef = *arrPos;
		return true;
	}
	return false;
}

bool CPropertyValue::GetNextArrayValue(CPropertyValue &valRef)
{
	arrPos++;
	if (arrPos != arValue.end())
	{
		valRef = *arrPos;
		return true;
	}
	return false;
}

bool CPropertyValue::GetFirstChildValue(CPropertyValue &valRef)
{
	childPos = chValue.begin();
	if (childPos != chValue.end())
	{
		valRef = childPos->second;
		return true;
	}
	return false;
}

bool CPropertyValue::GetNextChildValue(CPropertyValue &valRef)
{
	childPos++;
	if (childPos != chValue.end())
	{
		valRef = childPos->second;
		return true;
	}
	return false;
}

// cJSON * CPropertyValue::Jsonify()
// {
// 	cJSON *retVal = NULL;
// 	std::vector<CPropertyValue>::iterator aPos;
// 	std::map<std::string, CPropertyValue>::iterator cPos;

// 	switch (type)
// 	{
// 	case obj_type:
// 		retVal = cJSON_CreateObject();
// 		if (retVal != NULL)
// 		{
// 			for (cPos = chValue.begin(); cPos != chValue.end(); cPos++)
// 			{
// 				std::string ObjName(cPos->first);
// 				CPropertyValue &ValRef = cPos->second;
// 				cJSON *Object = ValRef.Jsonify();
// 				if (Object != NULL)
// 				{
// 					cJSON_AddItemToObject(retVal, ObjName.c_str(), Object);
// 				}
// 			}
// 		}
// 		break;

// 	case array_type:
// 		retVal = cJSON_CreateArray();
// 		if (retVal != NULL)
// 		{
// 			for (aPos = arValue.begin(); aPos != arValue.end(); aPos++)
// 			{
// 				CPropertyValue &ValRef = *aPos;
// 				cJSON *Object = ValRef.Jsonify();
// 				if (Object != NULL)
// 				{
// 					cJSON_AddItemToArray(retVal, Object);
// 				}
// 			}
// 		}
// 		break;

// 	case str_type:
// 		retVal = cJSON_CreateString(szValue.c_str());
// 		break;

// 	case bool_type:
// 		retVal = cJSON_CreateBool(bValue);
// 		break;

// 	case int_type:
// 		retVal = cJSON_CreateNumber(iValue);
// 		break;

// 	case real_type:
// 	case null_type:
// 	default:
// 		retVal = NULL;
// 	}

// 	return retVal;
// }













///////////////////////////////////////////////////////////////////
// CPropertyStore
///////////////////////////////////////////////////////////////////
CPropertyStore::CPropertyStore()
{
	m_RootName = "";
    m_RootValue.SetType(null_type);
}


CPropertyStore::~CPropertyStore()
{
	m_RootName = "";
    m_RootValue.SetType(null_type);
}

bool CPropertyStore::AddObjectProperty(std::string PropName, bool PropValue)
{
	CPropertyValue v(PropValue);
	return m_RootValue.AddChild(PropName, v);
}

bool CPropertyStore::AddObjectProperty(std::string PropName, int PropValue)
{
	CPropertyValue v(PropValue);
	return m_RootValue.AddChild(PropName, v);
}

bool CPropertyStore::AddObjectProperty(std::string PropName, double PropValue)
{
	CPropertyValue v(PropValue);
	return m_RootValue.AddChild(PropName, v);
}

bool CPropertyStore::AddObjectProperty(std::string PropName, std::string PropValue)
{
	CPropertyValue v(PropValue);
	return m_RootValue.AddChild(PropName, v);
}

bool CPropertyStore::AddObjectProperty(std::wstring PropName, std::wstring PropValue)
{
	std::string pName(PropName.begin(), PropName.end());
	std::string pVal(PropValue.begin(), PropValue.end());
	CPropertyValue v(pVal);
	return m_RootValue.AddChild(pName, v);
}

bool CPropertyStore::AddObjectProperty(std::string PropName, CPropertyValue PropValue)
{
    return m_RootValue.AddChild(PropName, PropValue);
}

bool CPropertyStore::AddObjectProperty(std::string PropName, CPropertyStore PropValue)
{
	return m_RootValue.AddChild(PropName, PropValue.m_RootValue);
}

bool CPropertyStore::AddObjectProperty(std::string ArrayName, std::vector<bool> Array)
{
	CPropertyValue v(Array);
	return m_RootValue.AddChild(ArrayName, v);
}

bool CPropertyStore::AddObjectProperty(std::string ArrayName, std::vector<int> Array)
{
	CPropertyValue v(Array);
	return m_RootValue.AddChild(ArrayName, v);
}

bool CPropertyStore::AddObjectProperty(std::string ArrayName, std::vector<double> Array)
{
	CPropertyValue v(Array);
	return m_RootValue.AddChild(ArrayName, v);
}

bool CPropertyStore::AddObjectProperty(std::string ArrayName, std::vector<std::string> Array)
{
	CPropertyValue v(Array);
	return m_RootValue.AddChild(ArrayName, v);
}

bool CPropertyStore::AddObjectProperty(std::string ArrayName, std::vector<CPropertyStore> Array)
{
	std::vector<CPropertyValue> localArray;
	std::vector<CPropertyStore>::iterator Pos;

	//First create an array containing all the corresponding CPropertyValues.
	for (Pos = Array.begin(); Pos != Array.end(); Pos++)
	{
		CPropertyStore coll = *Pos;
		CPropertyValue newVal(coll.m_RootValue);
		localArray.push_back(newVal);
	}

	// Then insert the array of CPropertyValues into a CPropertyValues and that is inserted into m_RootValue
	CPropertyValue v;
	v.SetValue(localArray);
	m_RootValue.AddChild(ArrayName, v);
	return true;
}

void CPropertyStore::RemoveObjectProperty(std::string propName)
{
	m_RootValue.RemoveChild(propName);
}

bool CPropertyStore::IsChildObjectPresent(std::string PropName)
{
	CPropertyValue valRef;
	bool bRetVal = m_RootValue.GetChild(PropName, valRef);
	return bRetVal;
}

bool CPropertyStore::GetObjectProperty(std::string PropName, CPropertyValue &PropValue)
{
	bool bRetVal = m_RootValue.GetChild(PropName, PropValue);
	return bRetVal;
}


bool CPropertyStore::AddChildObject(std::string ChildName, CPropertyStore Object)
{
	return m_RootValue.AddChild(ChildName, Object.m_RootValue);
}

void CPropertyStore::RemoveChildObject(std::string objectName)
{
	m_RootValue.RemoveChild(objectName);
}

bool CPropertyStore::GetChildObject(std::string ChildName, CPropertyStore &Object)
{
	if (m_RootValue.GetChild(ChildName, Object.m_RootValue))
	{
		return true;
	}
	return false;
}

// cJSON* CPropertyStore::Jsonify()
// {
// 	return m_RootValue.Jsonify();
// }



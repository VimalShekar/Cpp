#pragma once
#include <iostream>
#include <vector>
#include <map>


class CPropertyValue;
class CPropertyStore;
typedef CPropertyStore CPropertyStoreRoot;

////////////////////////////////////////////
// Type of elements in a JSON object
////////////////////////////////////////////
enum PropValue_Type {
	obj_type,
	array_type,
	str_type,
	bool_type,
	int_type,
	real_type,
	null_type
};


//////////////////////////////////////////////////////
//  Represents a JSON object
//////////////////////////////////////////////////////
class CPropertyValue
{
private:
	PropValue_Type type;
	int NumChildren;

	int iValue;				//int_type
	bool bValue;				//bool_type
    double dValue;
	std::string szValue;		//str_type
	std::vector<CPropertyValue> arValue;		// used if this object is an array of bool, int, double or string
	std::map<std::string, CPropertyValue> chValue;  // used to store this objects children

	std::vector<CPropertyValue>::iterator arrPos;
	std::map<std::string, CPropertyValue>::iterator childPos;

public:
	CPropertyValue();
	~CPropertyValue();
	CPropertyValue(bool b);
	CPropertyValue(int i);
    CPropertyValue(double d);
	CPropertyValue(std::string s);	
	CPropertyValue(std::vector<bool> v);
	CPropertyValue(std::vector<int> v);
    CPropertyValue(std::vector<double> d);
	CPropertyValue(std::vector<std::string> v);

	PropValue_Type GetType();
	void SetType(PropValue_Type t);

	void SetValue(bool b);
	void SetValue(int i);
    void SetValue(double d);
	void SetValue(std::string s);
	void SetValue(CPropertyValue s);
	void SetValue(std::vector<bool> v);
	void SetValue(std::vector<int> v);
    void SetValue(std::vector<double> v);
	void SetValue(std::vector<std::string> v);
	void SetValue(std::vector<CPropertyValue> v);

	int GetIntValue();
	bool GetBoolValue();
    double GetDoubleValue();
	std::string GetStringValue();

	bool AddChild(std::string Name, CPropertyValue val);
	void RemoveChild(std::string Name);
	bool GetChild(std::string Name, CPropertyValue &valRef);
	bool GetFirstChildValue(CPropertyValue &valRef);
	bool GetNextChildValue(CPropertyValue &valRef);

	bool GetFirstArrayValue(CPropertyValue &valRef);
	bool GetNextArrayValue(CPropertyValue &valRef);

	//char * Jsonify();
};



///////////////////////////////////////////////////////////////////
// Represents a Collection of properties
///////////////////////////////////////////////////////////////////
class CPropertyStore
{

public:	
	std::string m_RootName;
	CPropertyValue m_RootValue;									// keeps properties, arrays and children of this object
    

	CPropertyStore();
	~CPropertyStore();	

	bool AddObjectProperty(std::string PropName, bool PropValue);
	bool AddObjectProperty(std::string PropName, int PropValue);
    bool AddObjectProperty(std::string PropName, double PropValue);
	bool AddObjectProperty(std::string PropName, std::string PropValue);
	bool AddObjectProperty(std::wstring PropName, std::wstring PropValue);
    bool AddObjectProperty(std::string PropName, CPropertyValue PropValue);
	bool AddObjectProperty(std::string PropName, CPropertyStore PropValue);
	bool AddObjectProperty(std::string ArrayName, std::vector<bool> Array);
	bool AddObjectProperty(std::string ArrayName, std::vector<int> Array);
    bool AddObjectProperty(std::string ArrayName, std::vector<double> Array);
	bool AddObjectProperty(std::string ArrayName, std::vector<std::string> Array);    
	bool AddObjectProperty(std::string ArrayName, std::vector<CPropertyStore> Array);

	void RemoveObjectProperty(std::string PropName);
	bool IsChildObjectPresent(std::string PropName);
	bool GetObjectProperty(std::string PropName, CPropertyValue &PropValue);

	bool AddChildObject(std::string ChildName, CPropertyStore Object);
	bool GetChildObject(std::string ChildName, CPropertyStore &Object);
	void RemoveChildObject(std::string objectName);
	
	//cJSON* Jsonify();

};

typedef CPropertyStore CPropertyStoreRoot;
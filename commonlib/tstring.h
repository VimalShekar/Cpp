////////////////////////////////////////////////////////////////////////////////
// File : tstring.h
// Descrption : Contains definition for tstring --  which falls to std::string or
// std::wstring based on the build
// Idea reference : https://github.com/chrisoldwood/Core/blob/master/tstring.hpp
////////////////////////////////////////////////////////////////////////////////


#pragma once


////////////////////////////////////////////////////////////////////////////////
// Standard headers.
////////////////////////////////////////////////////////////////////////////////
#include <streambuf>
#include <string>
#include <string.h>


#ifdef ANSI_BUILD


// Basic types.
typedef char			tchar;		//universal character type. Maps to char or wchat_t.
typedef unsigned char	utchar;		//universal unsigned character type.
#define T_TEXT(x)			x			//universal string literal. Maps to "" or L"".

typedef	std::string		tstring;	//universal string type.
typedef std::ostream	tostream;	//universal output stream type.
typedef std::istream	tistream;	//universal input stream type.

// String functions.
#define tstrlen			strlen
#define tstrnlen		strnlen
#define tstrlwr			_strlwr
#define tstrupr			_strupr
#define tstrcmp			strcmp
#define tstricmp		_stricmp
#define tstrncmp		strncmp
#define tstrnicmp		_strnicmp
#define tstrcpy			strcpy
#define tstrncpy		strncpy
#define tstrcat			strcat
#define tstrtok			strtok
#define tstrchr			strchr
#define	tstrrchr		strrchr
#define	tstrstr			strstr
#define tstrtol			strtol
#define tstrtoul		strtoul
#define tstrtod			strtod

#ifdef _MSC_VER

#define tstrtoll		_strtoi64
#define tstrtoull		_strtoui64

#else

#define tstrtoll		strtoll
#define tstrtoull		strtoull

#endif

#define tatoi			atoi
#define tatol			atol
#define tisspace		isspace
#define ttolower		tolower


//
// UNICODE build.
//

#else



// Basic types.

typedef wchar_t			tchar;		//universal character type. Maps to char or wchat_t.
typedef wchar_t			utchar;		//universal unsigned character type.
#define T_TEXT(x)			L ## x		//universal string literal. Maps to "" or L"".

typedef	std::wstring	tstring;	//universal string type.
typedef std::wostream	tostream;	//universal output stream type.
typedef std::wistream	tistream;	//universal input stream type.

// String functions.
#define tstrlen			wcslen
#define tstrnlen		wcsnlen
#define tstrlwr			_wcslwr
#define tstrupr			_wcsupr
#define tstrcmp			wcscmp
#define tstricmp		_wcsicmp
#define tstrncmp		wcsncmp
#define tstrnicmp		_wcsnicmp
#define tstrcpy			wcscpy
#define tstrncpy		wcsncpy
#define tstrcat			wcscat
#define tstrtok			wcstok
#define tstrchr			wcschr
#define	tstrrchr		wcsrchr
#define	tstrstr			wcsstr
#define tstrtol			wcstol
#define tstrtoul		wcstoul
#define tstrtod			wcstod

#ifdef _MSC_VER
#define tstrtoll		_wcstoi64
#define tstrtoull		_wcstoui64
#else
#define tstrtoll		_wcstoll
#define tstrtoull		_wcstoull
#endif

#define tatoi			_wtoi
#define tatol			_wtol
#define tisspace		iswspace
#define ttolower		towlower

#endif

////////////////////////////////////////////////////////////////////////////////

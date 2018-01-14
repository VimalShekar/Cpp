#include <iostream>
#include <vector>
#include <algorithm>
#include <regex>


#define To_Lower_String(X) 	std::transform(X.begin(), X.end(), X.begin(), tolower);
#define To_Upper_String(X) 	std::transform(X.begin(), X.end(), X.begin(), toupper);

/*++
Checks if a given string is null or empty
Return Value:
	Returns true if string is null or empty, else returns false
--*/
bool IfStringNullOrEmpty(std::string const & strSrc );
bool IfStringNullOrEmpty(std::wstring const & wstrSrc );


/*++
This subset of functions check if the Source text contains the specified string or pattern.

Inputs:
	strSourceText	: Source text
	strSubstring / strRegExPattern	: String or pattern to search within Source text

Return Value:
	Returns true if string is present, else returns false
--*/
bool IfStringContainsSubstring(std::string const& strSourceText, std::string const& strSubstring);
bool IfStringContainsSubstring(std::wstring const& wstrSourceText, std::wstring const& wstrSubstring);
bool IfStringContainsRegExPattern(std::string const& strSourceText, std::string const& strRegExPattern);
bool IfStringContainsRegExPattern(std::wstring const& wstrSourceText, std::wstring const& wstrRegExPattern);
bool IfStringBeginsWith(std::string const& strSrc, std::string const& strMatch);
bool IfStringBeginsWith(std::wstring const& wstrSrc, std::wstring const& wstrMatch);
bool IfStringEndsWith(std::string const& strSrc, std::string const& strMatch);
bool IfStringEndsWith(std::wstring const& wstrSrc, std::wstring const& wstrMatch);

/*++
Matches a regex pattern and returns the first matched string. 
Inputs:
	strSourceText	: Source text
	strRegExPattern	: Pattern to search within Source text

Return Value:
	Returns empty string if pattern is not found, else returns the first match
--*/
std::string GetMatchedRegExPattern(std::string const& strSourceText, std::string const& strRegExPattern);
std::wstring GetMatchedRegExPattern(std::wstring const& wstrSourceText, std::wstring const& wstrRegExPattern);



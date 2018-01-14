

bool IfStringNullOrEmpty(std::string const & strSrc )
{
    return ((strSrc.empty()) || (strSrc == ""));
}

bool IfStringNullOrEmpty(std::wstring const & wstrSrc )
{
    return ((wstrSrc.empty()) || (wstrSrc == ""));
}

bool IfStringContainsSubstring(std::string const& strSourceText, std::string const& strSubstring)
{
    return (std::search(strSourceText.begin(), strSourceText.end(), strSubstring.begin(), strSubstring.end()) != strSourceText.end());
}

bool IfStringContainsSubstring(std::wstring const& wstrSourceText, std::wstring const& wstrSubstring)
{
    return (std::search(wstrSourceText.begin(), wstrSourceText.end(), wstrSubstring.begin(), wstrSubstring.end()) != wstrSourceText.end());
}

bool IfStringContainsRegExPattern(std::string const& strSourceText, std::string const& strRegExPattern)
{
	std::regex strPattern(strRegExPattern.c_str());
	
	if (std::regex_search(strSourceText, strPattern))
	{	
		return true;
	}	
	return false;
}

bool IfStringContainsRegExPattern(std::wstring const& wstrSourceText, std::wstring const& wstrRegExPattern)
{
	std::wregex strPattern(wstrRegExPattern.c_str());
	if (std::regex_search(wstrSourceText, strPattern))
	{
		return true;
	}
	return false;
}

bool IfStringBeginsWith(std::string const& strSrc, std::string const& strMatch)
{
    return ( (strSrc.size() >= strMatch.size()) &&
                 equal(strMatch.begin(), strMatch.end(), strSrc.begin()) );
}

bool IfStringBeginsWith(std::wstring const& wstrSrc, std::wstring const& wstrMatch)
{
    return ( (wstrSrc.size() >= wstrMatch.size()) &&
                equal(wstrMatch.begin(), wstrMatch.end(), wstrSrc.begin()) );
}

bool IfStringEndsWith(std::string const& strSrc, std::string const& strMatch)
{
	std::string strRegExPattern(strMatch);
	strRegExPattern.append("\\b");
	std::regex strPattern(strRegExPattern.c_str());
	if (std::regex_search(strSrc, strPattern))
	{
		return true;
	}
	return false;
}

bool IfStringEndsWith(std::wstring const& wstrSrc, std::wstring const& wstrMatch)
{
	std::wstring strRegExPattern(wstrMatch);
	strRegExPattern.append(L"\\b");
	std::wregex strPattern(strRegExPattern.c_str());
	if (std::regex_search(wstrSrc, strPattern))
	{
		return true;
	}
	return false;
}

std::string GetMatchedRegExPattern(std::string const& strSourceText, std::string const& strRegExPattern)
{
	std::regex strPattern(strRegExPattern.c_str());
	std::string temp("");
	std::match_results<std::wstring::const_iterator> strMatchResult;
	if (std::regex_search(strSourceText, strMatchResult, strPattern))
	{
		temp.assign(strMatchResult[0]);
	}
	return temp;
}

std::wstring GetMatchedRegExPattern(std::wstring const& wstrSourceText, std::wstring const& wstrRegExPattern)
{
	std::wregex strPattern(wstrRegExPattern.c_str());
	std::wstring temp(L"");
	std::match_results<std::wstring::const_iterator> strMatchResult;
	if (std::regex_search(wstrSourceText, strMatchResult, strPattern))
	{
		temp.assign(strMatchResult[0]);
	}
	return temp;
}



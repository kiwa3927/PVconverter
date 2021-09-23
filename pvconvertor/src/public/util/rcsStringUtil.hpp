



#ifndef UTSTRINGUTIL_HPP_
#define UTSTRINGUTIL_HPP_

#include <string>
#include <vector>
#include <algorithm>

#include "public/rcsTypes.h"

inline void
trimBegin(std::string &str, const char *pTrim = " \t\r\n")
{
    std::string::size_type nIndex = str.find_first_not_of(pTrim, 0);
    if(nIndex == std::string::npos)
    {
        str.clear();
    }
    else
    {
        str.erase(0, nIndex);
    }
}

inline void
trimEnd(std::string &str, const char *pTrim = " \t\r\n")
{
    std::string::size_type nIndex = str.find_last_not_of(pTrim);
    if(nIndex != std::string::npos)
    {
        str.erase(nIndex + 1, str.length());
    }
}

inline void
trim(std::string &str, const char *pTrim = " \t\r\n")
{
    trimBegin(str, pTrim);
    trimEnd(str, pTrim);
}

inline void
toUpper(std::string &str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

inline void
toLower(std::string &str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

inline std::string
itoa(hvUInt32 num)
{
    char buf[32];
    sprintf(buf,"%u",num);
    return std::string(buf);
}

inline std::string
ftoa(double fValue)
{
    char buf[32];
    sprintf(buf, "%lf", fValue);
    return std::string(buf);
}

hvUInt32 getNumEnvValue(const char *pName, double &fValue);



void _replaceBuildIn(const std::string &sBuildIn, std::string &sResult);

bool _replaceComment(const std::string &sCheckComment, std::string &sResult,
                bool isRuleCheckComment = true, bool isTvfFile = false);

template<class Container>
void tokenize(Container &v, const std::string &str,
                     const char *pToken = " ", const bool addEmpty = false)
{
    if(str.empty())
    {
        return;
    }

    std::string::size_type pre_index = 0, index = 0;
    while((index = str.find_first_of(pToken, pre_index)) != std::string::npos)
    {
        if(index != 0)
        {
            std::string element = str.substr(pre_index, index - pre_index);
            trim(element);
            
            if((!addEmpty && !element.empty()) || addEmpty)
                v.push_back(str.substr(pre_index, index - pre_index));
        }
        pre_index = index + 1;
    }

    std::string element = str.substr(pre_index);
    if((!addEmpty && !element.empty()) || addEmpty)
        v.push_back(element);
}


bool isTclPackageLine(const std::string &line);

inline bool isCoordinateValue(std::string str)
{
	trim(str);
	for(int i = 0; i<str.size(); i++)
	{
		char chTmp = str[i];
		if(!isdigit(chTmp))
		{
			if((chTmp != '-') && (chTmp != '+') && !isspace(chTmp)
			&& (chTmp != '(') && (chTmp != ')') && (chTmp != '.'))
			{
				return false;
			}
		}
	}


	return true;

}

#endif 





#include "rcsStringUtil.hpp"


#if 0
hvUInt32 getNumEnvValue(const char *pName, double &fValue)
{
    const char *pValue = getenv(pName);
    if(pValue == NULL)
    {
        return 1;
    }

    char suffix[100];
    if(1 == sscanf(pValue, "%lf%99s", &fValue, suffix))
    {
        return 0;
    }
    return 2;
}
#endif

bool _isdigit(const std::string &sValue)
{
    std::string::size_type i = 0;
    while(i < sValue.size())
    {
        if(!isdigit(sValue[i]))
            return false;
        ++i;
    }
    return true;
}

char _getChar(const char c)
{
    switch(c)
    {
    case 't':
        return '\t';
    case 'n':
        return '\n';
    case 'r':
        return '\r';
    case 'a':
        return '\a';
    case 'b':
        return '\b';
    case 'e':
        return '\e';
    case 'f':
        return '\f';
    case 'v':
        return '\v';
    case '1':
        return '\1';
    case '2':
        return '\2';
    case '3':
        return '\3';
    case '4':
        return '\4';
    case '5':
        return '\5';
    case '6':
        return '\6';
    case '7':
        return '\7';
    case '0':
        return '\0';
    default:
        return c;
    }
    return c;
}

bool _isSeparatorSymbol(const std::string &text, const std::string::size_type i)
{
    char c = text[i];

    if(isalnum(c))
        return false;

    switch(c)
    {
    case '?':
    case '`':
    case '#':
    case '$':
    case '_':
    case ';':
    case '\\':
        return false;
    case '&':
    case '|':
    case ':':
        return (i + 1) < text.size() && c == text[i + 1];
    default:
        return true;
    }
}

void _replaceBuildIn(const std::string &sBuildIn, std::string &sResult)
{
    sResult = sBuildIn;
    trim(sResult);
    // if buildin is not complete, use args "-full" the erase is too careless!, commented by st.233

    if (sResult.size() >= 2 && sResult[0] == '[' && sResult[sResult.size()-1] == ']')
    {
        sResult.erase(0, 1);
        sResult.erase(sResult.size() - 1);
        trim(sResult);
        if(sResult[0] == '\"' || sResult[0] == '\'')
        {
            trim(sResult, "\"\'");
            sResult.insert(0, "[\n\"");
            sResult.insert(sResult.size(), "\"\n]");
        }
        else
        {
            sResult.insert(0, "[\n");
            sResult.insert(sResult.size(), "\n]");
        }
    }
}

bool _replaceComment(const std::string &sCheckComment, std::string &sResult, bool isRuleCheckComment, bool isTvfFile)
{
    std::string::size_type i = isRuleCheckComment ? 1 : 0;
    bool isIn = false;
    while(i < sCheckComment.size())
    {
        std::string::size_type end = i;
        std::string sVarName;
        char c = sCheckComment[i];
        switch(c)
        {
        case '\\':
        {
            end += 2;
            break;
        }
        case '\'':
        case '\"':
        {
            isIn = !isIn;
            ++end;
            break;
        }
        case '^':
        {
            if (!isRuleCheckComment || isIn)
            {
                ++end;
                break;
            }

            end += 1;
            if (sCheckComment[end] == '\'' || sCheckComment[end] == '\"')
            {
                c = sCheckComment[end];
                ++end;
                while(end < sCheckComment.size() && sCheckComment[end] != c &&
                      sCheckComment[end] != '\n' && sCheckComment[end] != '\r')
                {
                    if (sCheckComment[end] == '\\')
                    {
                        sVarName += _getChar(sCheckComment[++end]);
                    }
                    else
                    {
                        sVarName += sCheckComment[end];
                    }
                    ++end;
                }
                if (sCheckComment[end] == c)
                {
                    ++end;
                }
            }
            else
            {
                while(end < sCheckComment.size() &&
                      (sCheckComment[end] == ' ' || sCheckComment[end] == '\t'))
                {
                    ++end;
                }

                while(end < sCheckComment.size() && !_isSeparatorSymbol
                      (sCheckComment, end))
                {
                    ++end;
                }
                sVarName = sCheckComment.substr(i + 1, end - i - 1);
                trimBegin(sVarName, " \t");
            }
            break;
        }
        case 't':
        case 'T':
        {
            ++end;
            if (end < sCheckComment.size() &&
                    (sCheckComment[end] == 'V' || sCheckComment[end] == 'v'))
                ++end;
            else
                break;
            if (end < sCheckComment.size() &&
                    (sCheckComment[end] == 'F' || sCheckComment[end] == 'f'))
                ++end;
            else
                break;

            i = end;
            sResult += "trs";
            break;
        }
        case 's':
        case 'S':
        {
            ++end;
            if (end < sCheckComment.size() &&
                    (sCheckComment[end] == 'V' || sCheckComment[end] == 'v'))
                ++end;
            else
                break;
            if (end < sCheckComment.size() &&
                    (sCheckComment[end] == 'R' || sCheckComment[end] == 'r'))
                ++end;
            else
                break;
            if (end < sCheckComment.size() &&
                    (sCheckComment[end] == 'F' || sCheckComment[end] == 'f'))
                ++end;
            else
                break;

            i = end;
            sResult += "pvrs";
            break;
        }
        case '$':
        {
            if(!isTvfFile)
            {
                sResult += "\\";
                ++end;
                break;
            }
        }
        default:
        {
            ++end;
            break;
        }
        }
        if (sVarName.empty() || _isdigit(sVarName))
        {
            sResult += sCheckComment.substr(i , end - i);
        }
        else
        {
            if(isTvfFile)
                sResult += "\\";
            sResult += "$";
            sResult += sVarName;
        }
        i = end;
    }

    return true;
}


bool isTclPackageLine(const std::string &line)
{
    if (line.size() <= 15 )
        return false;

    static const std::string str1 = "package";
    static const std::string str2 = "require";

    size_t index = line.find(str1);
    if (index != std::string::npos)
    {
        index += str1.size();
        if (index < line.size() && std::isspace(line[index]))
        {
            index = line.find(str2, index);
            if (index != std::string::npos)
            {
                index += str2.size();
                return index >= line.size() || std::isspace(line[index]);
            }
        }
    }
    return false;
}

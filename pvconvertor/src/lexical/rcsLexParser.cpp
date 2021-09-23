



#include "FlexLexer.h"
#include <fstream>
#include <sstream>

#include "public/manager/rcErrorManager.h"
#include "public/manager/rcsManager.h"
#include "public/rcsTypes.h"
#include "rcsLexParser.h"

rcsLexParser_T::rcsLexParser_T(std::list<rcsToken_T> &lTokenStream,
                               std::set<hvUInt32> &vUnstoredCommentLineNumbers) :
    m_listTokenStream(lTokenStream), m_pFileName(NULL),
    m_vUnstoredCommentLineNumbers(vUnstoredCommentLineNumbers)
{
}

static std::string
parseString(const std::string &src)
{
    char b = src[0];
    std::string res;
    std::string::size_type pos = 1;
    while(pos < src.length())
    {
        if('\\' == src[pos])
        {
            if(++pos >= src.length())
            {
                res += '\\';
                return res;
            }
            switch(src[pos])
            {
                case 't':
                    res += '\t';
                    break;
                case 'n':
                    res += '\n';
                    break;
                case 'r':
                    res += '\r';
                    break;
                case 'a':
                    res += '\a';
                    break;
                case 'b':
                    res += '\b';
                    break;
                case 'e':
                    res += '\e';
                    break;
                case 'f':
                    res += '\f';
                    break;
                case 'v':
                    res += '\v';
                    break;
                default:
                {
                    if((src[pos] >= '0') && (src[pos] <= '7'))
                    {
                        unsigned char ch = 0;
                        do
                        {
                            ch <<= 3;
                            ch += (src[pos++] - '0');
                        } while (pos < src.length() && (src[pos] >= '0') && (src[pos] <= '7'));
                        res += (char)ch;
                        continue;
                    }
                    else
                    {
                        res += src[pos];
                    }
                    break;
                }
            }
        }
        else if(src[pos] == b)
        {
            return res;
        }
        else
        {
            Utassert(src[pos] != '\n' && src[pos] != '\r');
            res += src[pos];
        }
        ++pos;
    }
    return res;
}

void
rcsLexParser_T::parse(const char *pFileName, const char* pValue)
{
    std::istream *pReader = NULL;

    if(pFileName != NULL)
    {
        m_pFileName = pFileName;
        pReader = new std::ifstream(m_pFileName);
    }
    else if(pValue != NULL)
    {
        pReader = new std::istringstream(pValue);
    }
    else
    {
        return;
    }

    FlexLexer *pLexer = new yyFlexLexer(pReader);

    hvInt32 eType = pLexer->yylex();
    bool isInBuildInLang = false;
    bool isComment       = false;
    hvUInt32 nCommentLineNo = 0;
    std::string sBuildInLangBegin;
    std::string sCommentValue;
    while(eType)
    {
        if(eType >= PRO_DEFINE && eType <= PRO_INCLUDE)
        {
            m_listTokenStream.push_back(rcsToken_T(PRO_KEYWORD,
                                        pLexer->lineno(), pLexer->YYText()));
            m_listTokenStream.back().eKeyType = static_cast<KEY_TYPE>(eType);
        }
        else if((eType >= AND) && (eType < DEVICE))
        {
            m_listTokenStream.push_back(rcsToken_T(LAYER_OPERATION_KEYWORD,
                                        pLexer->lineno(), pLexer->YYText()));
            m_listTokenStream.back().eKeyType = static_cast<KEY_TYPE>(eType);
        }
        else if((eType >= DEVICE) && (eType <= NON_SUPPORT_SPECIFICATION))
        {
            m_listTokenStream.push_back(rcsToken_T(SPECIFICATION_KEYWORD,
                                        pLexer->lineno(), pLexer->YYText()));
            m_listTokenStream.back().eKeyType = static_cast<KEY_TYPE>(eType);
        }
        else if(eType >= ABUT_ALSO)
        {
            
            m_listTokenStream.push_back(rcsToken_T(SECONDARY_KEYWORD,
                                        pLexer->lineno(), pLexer->YYText()));
            m_listTokenStream.back().eKeyType = static_cast<KEY_TYPE>(eType);
        }
        else
        {
            switch(eType)
            {
                case HEX_NUMBER:
                {
                    Utassert(!isComment);
                    m_listTokenStream.push_back(rcsToken_T(NUMBER,
                                                pLexer->lineno(), pLexer->YYText()));
                    m_listTokenStream.back().eNumberType = INT_NUMBER;
#ifdef BIT64
                    sscanf(pLexer->YYText(), "%lx", &(m_listTokenStream.back().iValue));
#else
                    sscanf(pLexer->YYText(), "%x", &(m_listTokenStream.back().iValue));
#endif
                    break;
                }
                case INT_NUMBER:
                {
                    Utassert(!isComment);
                    m_listTokenStream.push_back(rcsToken_T(NUMBER,
                                                pLexer->lineno(), pLexer->YYText()));
                    m_listTokenStream.back().eNumberType = INT_NUMBER;
#ifdef BIT64
                    sscanf(pLexer->YYText(), "%lu", &(m_listTokenStream.back().iValue));
#else
                    sscanf(pLexer->YYText(), "%u", &(m_listTokenStream.back().iValue));
#endif
                    break;
                }
                case FLOAT_NUMBER:
                case SCIENTIFIC_NUMBER:
                {
                    Utassert(!isComment);
                    m_listTokenStream.push_back(rcsToken_T(NUMBER, pLexer->lineno(),
                                                           pLexer->YYText()));
                    m_listTokenStream.back().eNumberType = FLOAT_NUMBER;
                    sscanf(pLexer->YYText(), "%lf", &(m_listTokenStream.back().dValue));
                    break;
                }
                case STRING_CONSTANTS:
                {
                    Utassert(!isComment);
                    bool isTwoLine = false;
                    std::string sValue = pLexer->YYText();
                    hvUInt32   lineno = pLexer->lineno();
                    
                    if(2 == sValue.size())
                    {
                        if(sValue[1] == '\n' || sValue[1] == '\r')
                            m_listTokenStream.push_back(rcsToken_T(BLANK_SPACE, lineno - 1, "\n"));
                        break;
                    }

                    if(sValue[sValue.size() - 1] == '\n' ||
                       sValue[sValue.size() - 1] == '\r')
                    {
                        sValue.erase(sValue.size() - 1);
                        --lineno;
                        isTwoLine = true;
                    }

                    sValue = parseString(sValue);
                    if(0 == strlen(sValue.c_str()))
                    {
                        if(isTwoLine)
                            m_listTokenStream.push_back(rcsToken_T(BLANK_SPACE, lineno, "\n"));
                        break;
                    }

                    m_listTokenStream.push_back(rcsToken_T(static_cast<TOKEN_TYPE>(eType),
                                                           lineno, sValue.c_str()));
                    if(isTwoLine)
                        m_listTokenStream.push_back(rcsToken_T(BLANK_SPACE, lineno, "\n"));

                    break;
                }
                case SEPARATOR:
                {
                    std::string sValue = pLexer->YYText();

                    if(!isInBuildInLang && sValue == "[")
                    {
                        isInBuildInLang = true;
                        sBuildInLangBegin = sValue;
                    }
                    else if(isInBuildInLang && sBuildInLangBegin == "[" && sValue == "]")
                    {
                        isInBuildInLang = false;
                        sBuildInLangBegin.clear();
                    }
                    else if(sValue == "/*")
                    {
                        isComment = true;
                        nCommentLineNo = pLexer->lineno();
                        sCommentValue += sValue;
                        break;
                    }
                    else if(sValue == "*/")
                    {
                        isComment = false;
                        sCommentValue += sValue;
                        
                        
                        
                        if(isInBuildInLang || (rcsManager_T::getInstance()->isNewPvrs() &&
                           rcsManager_T::getInstance()->outputComment()))
                        {
                            m_listTokenStream.push_back(rcsToken_T(COMMENT, nCommentLineNo,
                                                                   sCommentValue.c_str()));
                            m_listTokenStream.back().nEndLineNo = pLexer->lineno();
                        }
                        else
                        {
                            m_vUnstoredCommentLineNumbers.insert(nCommentLineNo);
                        }
                        sCommentValue.clear();
                        break;
                    }
                }
                case IDENTIFIER_NAME:
                {
                    if(strcasecmp("PLEASE_DEFINE_CHIP_BLOCK_OR_OTHER_CHECK_OPTION", pLexer->YYText()) == 0 &&
                      (m_listTokenStream.back().eType != SPECIFICATION_KEYWORD || m_listTokenStream.back().eKeyType != SVRF_ERROR))
                    {
                        m_listTokenStream.push_back(rcsToken_T(SPECIFICATION_KEYWORD,
                                                    pLexer->lineno(), pLexer->YYText()));
                        m_listTokenStream.back().eKeyType = SVRF_ERROR;
                    }
                }
                case OPERATOR:
                case BUILT_IN_LANG:
                case DMACRO:
                case CMACRO:
                {
                    Utassert(!isComment);
                    m_listTokenStream.push_back(rcsToken_T(static_cast<TOKEN_TYPE>(eType),
                                                pLexer->lineno(), pLexer->YYText()));
                    break;
                }
                case INCOMPLETE_KEYWORD:
                {
                    Utassert(!isComment);
                    if(isInBuildInLang)
                    {
                        m_listTokenStream.push_back(rcsToken_T(static_cast<TOKEN_TYPE>(eType),
                                                    pLexer->lineno(), pLexer->YYText()));
                    }
                    break;
                }
                case COMMENT:
                case BLANK_SPACE:
                {
                    Utassert(!isComment);
                    if(isInBuildInLang)
                    {
                        std::string sValue = pLexer->YYText();
                        m_listTokenStream.push_back(rcsToken_T(static_cast<TOKEN_TYPE>(eType),
                                                    pLexer->lineno(), pLexer->YYText()));
                        
                    }
                    else if(rcsManager_T::getInstance()->outputComment() &&
                            pLexer->lineno() != m_listTokenStream.back().nLineNo)
                    {
                        m_listTokenStream.push_back(rcsToken_T(static_cast<TOKEN_TYPE>(eType),
                                                    pLexer->lineno(), pLexer->YYText()));
                        m_listTokenStream.back().nEndLineNo = pLexer->lineno();
                    }
                    else if (eType == COMMENT)
                    {
                        m_vUnstoredCommentLineNumbers.insert(pLexer->lineno());
                    }
                    break;
                }
                case COMMENT_TEXT:
                {
                    Utassert(isComment);
                    sCommentValue += pLexer->YYText();
                    break;
                }
                default:
                    Utassert(false);
                    break;
            }
        }
        eType = pLexer->yylex();
    }

    delete pLexer;
    delete pReader;
}

#ifdef DEBUG
void
rcsLexParser_T::printTokenList()
{
	std::list<rcsToken_T>::iterator it;
	std::cout << "***************************************" << std::endl;
	for(it = m_listTokenStream.begin() ; it != m_listTokenStream.end(); it++ )
	{
		std::cout << "eType:" << (*it).eType << ", " << std::endl
				  << "nLineNo:" << (*it).nLineNo << ", " << std::endl
				  << "sValue:" << (*it).sValue << ", " << std::endl
				  << "namescopes:";
		printVector((*it).namescopes);
		std::cout << std::endl;
		std::cout << std::endl;
	}
	std::cout << "***************************************" << std::endl;
}
void
rcsLexParser_T::printVector(std::vector<std::string>& vec)
{
	std::vector<std::string>::iterator it;
	std::cout << "( ";
	for(it = vec.begin(); it != vec.end() ; it++)
	{
		std::cout << *it << ",";
	}
	std::cout << ")";
}
#endif

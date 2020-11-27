/*#########################################################################*/
/**
 * @file     synnode.inl
 * @brief    Implementation of class synnode
 *
 *
 * @author   anonymous
 * @date
 * @see
 *
 */
/*#########################################################################*/

#include "public/manager/rcsManager.h"

rcsSynNode_T::rcsSynNode_T()
{
}

inline void
rcsSynNode_T::insertTokens(std::list<rcsToken_T>::iterator first, std::list<rcsToken_T>::iterator last)
{
    m_listTokens.insert(m_listTokens.begin(), first, last);
}

inline std::list<rcsToken_T>::iterator
rcsSynNode_T::begin()
{
    return m_listTokens.begin();
}
inline std::list<rcsToken_T>::iterator
rcsSynNode_T::end()
{
    return m_listTokens.end();
}

inline hvUInt32
rcsSynNode_T::getNodeBeginLineNo() const
{
    return m_listTokens.begin()->nLineNo;
}

void
rcsSynNode_T::getCmdValue(std::string &sValue)
{
    Utassert(sValue.empty());
    for(std::list<rcsToken_T>::iterator iter = this->begin(); iter != this->end(); ++iter)
    {
        if(iter->eType == STRING_CONSTANTS)
        {
            const std::string &src = iter->sValue;
            std::string res;
            std::string::size_type pos = 0;
            while(pos < src.length())
            {
                switch(src[pos])
                {
                    case '\t':
                        res += "\\t";
                        break;
                    case '\n':
                        res += "\\n";
                        break;
                    case '\r':
                        res += "\\r";
                        break;
                    case '\a':
                        res += "\\a";
                        break;
                    case '\b':
                        res += "\\b";
                        break;
                    case '\e':
                        res += "\\e";
                        break;
                    case '\f':
                        res += "\\f";
                        break;
                    case '\v':
                        res += "\\v";
                        break;
                    case '\'':
                        res += "\\\'";
                        break;
                    case '\"':
                        res += "\\\"";
                        break;
                    case '\\':
                        res += "\\\\";
                        break;
                    case '\1':
                        res += "\\1";
                        break;
                    case '\2':
                        res += "\\2";
                        break;
                    case '\3':
                        res += "\\3";
                        break;
                    case '\4':
                        res += "\\4";
                        break;
                    case '\5':
                        res += "\\5";
                        break;
                    case '\6':
                        res += "\\6";
                        break;
                    case '\0':
                        res += "\\0";
                        break;
                    default:
                        res += src[pos];
                        break;
                }
                ++pos;
            }

            sValue += "\\\"";
            sValue += res;
            sValue += "\\\"";
        }
        else if(iter->eType == BUILT_IN_LANG)
        {

#ifdef DEBUG
            std::string sExp = iter->namescopes.back();
            sExp.erase(0, 1);
            sExp.insert(0, "\\[ ");
            sExp.erase(sExp.size() - 1);
            sExp.insert(sExp.size(), " \\]");
            hvUInt32 iValue = 0;
            while(iValue < sExp.size())
            {
                if(sExp[iValue] == '\n' || sExp[iValue] == '\r' ||
                   sExp[iValue] == '\"' || sExp[iValue] == '\'' ||
                   sExp[iValue] == ';' || sExp[iValue] == '$')
                {
                    sExp.insert(iValue, "\\");
                    ++iValue;
                    if((sExp[iValue] == '\n') && (iValue != sExp.size() - 1))
                    {
                    	iValue++;
                    	sExp.insert(iValue, "//");
                    }
                }
                ++iValue;
            }

            sValue += sExp;
#endif

        }
        else
        {
            std::string sName = iter->sValue;
            if(!rcsManager_T::getInstance()->isNewPvrs())
            {
                hvUInt32 iValue = 0;
                while(iValue < sName.size())
                {
                    if(sName[iValue] == '\n' || sName[iValue] == '\r' ||
                       sName[iValue] == '\"' || sName[iValue] == '\'' ||
                       sName[iValue] == ';' || sName[iValue] == '$')
                    {
                        sName.insert(iValue, "\\");
                        ++iValue;
                    }
                    ++iValue;
                }
            }
            sValue += sName;
        }
        sValue += " ";
    }
}


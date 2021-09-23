



#include <set>
#include <vector>
#include <iterator>
#include <sstream>

#include "public/manager/rcErrorManager.h"
#include "public/util/rcsStringUtil.hpp"
#include "syntax/rcsSynNodeConvertor.h"
#include "public/manager/rcsManager.h"
#include "rcsMacroParser.h"

rcsLexMacroParser_T::rcsLexMacroParser_T(std::list<rcsToken_T> &lTokenStream,
                                         std::map<hvUInt32, std::pair<hvUInt32, bool> > &blankLinesBefore,
                                         std::set<hvUInt32> &vUnstoredCommentLineNumbers,
                                         bool flattenMacro) :
    m_listTokenStream(lTokenStream), m_blankLinesBefore(blankLinesBefore),
    m_vUnstoredCommentLineNumbers(vUnstoredCommentLineNumbers), m_flattenMacro(flattenMacro)
{
}

template <class iterator> void
rcsLexMacroParser_T::skipBlankSpaceToken(iterator &iter)
{
    Utassert(iter->eType != BLANK_SPACE);
    ++iter;
    while(iter != m_listTokenStream.end())
    {
        if(iter->eType != BLANK_SPACE)
            break;
        ++iter;
    }
}

int
rcsLexMacroParser_T::parseMacroParas(std::list<rcsToken_T>::iterator &iter,
                                     std::set<std::string> &setParas)
{
    hvUInt32 iPara = 0;
    while(iter != m_listTokenStream.end())
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            {
                if(!isValidSvrfName(iter->sValue))
                {
                    
                    s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, MAC4,
                                                        iter->nLineNo, iter->sValue));
                }
            }
            case STRING_CONSTANTS:
            {
                std::string sPara = iter->sValue;
                toLower(sPara);
                if(!setParas.insert(sPara).second)
                {
                    
                    s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, MAC5,
                                                        iter->nLineNo, iter->sValue));
                }
                else
                {
                    ++iPara;
                }
                break;
            }
            case SEPARATOR:
            {
                if(iter->sValue == "{")
                    return iPara;
            }
            case BLANK_SPACE:
                break;
            default:
            {
                
                s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, MAC4,
                                                    iter->nLineNo, iter->sValue));
                break;
            }
        }
        ++iter;
    }

    Utassert(iter == m_listTokenStream.end());
    return iPara;
}


void
rcsLexMacroParser_T::replaceCMacro(std::list<rcsToken_T>::iterator &iCMacro,
                                   MACRO_TOKENS &macro)
{
    std::list<rcsToken_T>::iterator iCMacro_Begin = iCMacro;
    std::list<rcsToken_T>::iterator iCMacro_Name = iCMacro_Begin--;
    std::list<rcsToken_T>::const_iterator iDMacro = macro.tokens.begin();

    skipBlankSpaceToken(iDMacro);
    skipBlankSpaceToken(iDMacro);
    skipBlankSpaceToken(iCMacro);

    hvUInt32 iPara = 0;
    std::map<std::string, std::vector<rcsToken_T> > vArguments;
    while((iCMacro != m_listTokenStream.end()) && (iPara < macro.nParas))
    {
        switch(iCMacro->eType)
        {
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            {
                if(!isValidSvrfName(iCMacro->sValue))
                {
                    s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, MAC7,
                                                        iCMacro->nLineNo, iCMacro->sValue));
                }
            }
            case STRING_CONSTANTS:
            case NUMBER:
            {
                std::string sArgument = iDMacro->sValue;
                toLower(sArgument);
                std::vector<rcsToken_T> &tokens = vArguments[sArgument];
                tokens.push_back(*iCMacro);
                ++iPara;
                break;
            }
            case OPERATOR:
            {
                if(iCMacro->sValue == "+" || iCMacro->sValue == "-")
                {
                    hvUInt32 nLineNo = iCMacro->nLineNo;
                    std::list<rcsToken_T>::iterator iOperator = iCMacro;
                    skipBlankSpaceToken(iCMacro);
                    if(iCMacro != m_listTokenStream.end() &&
                       iCMacro->eType == NUMBER &&
                       iCMacro->nLineNo == nLineNo)
                    {
                        std::string sArgument = iDMacro->sValue;
                        toLower(sArgument);
                        std::vector<rcsToken_T> &tokens = vArguments[sArgument];
                        tokens.push_back(*iOperator);
                        tokens.push_back(*iCMacro);
                        ++iPara;
                        break;
                    }
                    else
                        iCMacro = iOperator;
                }
            }
            default:
            {
                s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, MAC7,
                                                    iCMacro->nLineNo, iCMacro->sValue));
                break;
            }
        }

        skipBlankSpaceToken(iDMacro);
        skipBlankSpaceToken(iCMacro);
    }

    if(iPara < macro.nParas)
    {
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, MAC9,
                                            iCMacro->nLineNo, iCMacro_Name->sValue));
    }
    Utassert(iDMacro->sValue == "{");

    std::vector<std::string> namescopes = iCMacro_Begin->namescopes;
    {
        std::string sSpace = iCMacro_Name->sValue + "<" + itoa(++macro.nCallTimes) + ">";
        namescopes.push_back(sSpace);
    }
    iCMacro = m_listTokenStream.erase(iCMacro_Begin, iCMacro);

    skipBlankSpaceToken(iDMacro);
    hvUInt32 nBrace = 1;
    while(iDMacro != m_listTokenStream.end())
    {
        if(iDMacro->eType == SEPARATOR)
        {
            if(iDMacro->sValue == "}")
            {
                --nBrace;
            }
            else if(iDMacro->sValue == "{")
            {
                ++nBrace;
            }
            if(nBrace == 0)
                break;
        }
        switch(iDMacro->eType)
        {
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            {
                std::string sArgument = iDMacro->sValue;
                toLower(sArgument);
                std::map<std::string, std::vector<rcsToken_T> >::iterator iter =
                        vArguments.find(sArgument);
                if(iter != vArguments.end())
                {
                    for(std::vector<rcsToken_T>::const_iterator itoken = iter->second.begin();
                        itoken != iter->second.end(); ++itoken)
                    {
                        iCMacro = m_listTokenStream.insert(iCMacro, *itoken);
                        iCMacro->nLineNo = iDMacro->nLineNo;
                        ++iCMacro;
                    }
                    ++iDMacro;
                    continue;
                }
            }
            default:
            {
                iCMacro = m_listTokenStream.insert(iCMacro, *iDMacro);
                iCMacro->namescopes = namescopes;
                break;
            }
        }

        ++iDMacro;
        ++iCMacro;
    }
    Utassert(iDMacro->sValue == "}");
}

void
rcsLexMacroParser_T::parseCMacro(std::list<rcsToken_T>::iterator &iterBegin)
{
    Utassert(iterBegin->eType == CMACRO);

    std::list<rcsToken_T>::iterator iter = iterBegin;
    skipBlankSpaceToken(iter);
    if(iter == m_listTokenStream.end())
    {
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, MAC6, iterBegin->nLineNo));
    }

    switch(iter->eType)
    {
        case SECONDARY_KEYWORD:
        case IDENTIFIER_NAME:
        {
            if(!isValidSvrfName(iter->sValue))
            {
                s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, MAC6,
                                                    iterBegin->nLineNo));
            }
        }
        case STRING_CONSTANTS:
        {
            std::string sName = iter->sValue;
            toLower(sName);
            std::map<std::string, MACRO_TOKENS>::iterator iterMacro = m_mapMacros.lower_bound(sName);
            if((iterMacro != m_mapMacros.end()) && (iterMacro->first == sName))
            {
                MACRO_TOKENS &macro = iterMacro->second;
                for(std::vector<std::string>::iterator iSpace = iterBegin->namescopes.begin();
                        iSpace != iterBegin->namescopes.end(); ++iSpace)
                {
                    std::string::size_type iSep = iSpace->find_last_of("<");
                    std::string sSpace = iSpace->substr(0, iSep);
                    if(0 == strcmp(sSpace.c_str(), sName.c_str()))
                    {
                        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, MAC10,
                                                            iter->nLineNo, iter->sValue));
                    }
                }
                replaceCMacro(iter, macro);
                iterBegin = iter;
            }
            else
            {
                s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, MAC8,
                                                    iterBegin->nLineNo, iter->sValue));
            }
            break;
        }
        default:
        {
            s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, MAC6, iterBegin->nLineNo));
            break;
        }
    }
}

void
rcsLexMacroParser_T::extractMacro(std::list<rcsToken_T>::iterator &iBegin)
{
    std::list<rcsToken_T>::iterator iter = iBegin;
    Utassert(iBegin->eType == DMACRO);
    skipBlankSpaceToken(iter);

    if(iter == m_listTokenStream.end())
    {
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, MAC1,
                                            iBegin->nLineNo));
    }

    std::set<std::string> vParas;
    switch(iter->eType)
    {
        case SECONDARY_KEYWORD:
        case IDENTIFIER_NAME:
        {
            if(!isValidSvrfName(iter->sValue))
            {
                s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, MAC1,
                                                    iBegin->nLineNo));
            }
        }
        case STRING_CONSTANTS:
        {
            std::string sMacroName = iter->sValue;
            parseMacroParas(++iter, vParas);

            if(iter == m_listTokenStream.end())
            {
                s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, MAC2,
                                                    iBegin->nLineNo,
                                                    (++iBegin)->sValue.c_str()));
            }

            hvUInt32 nBrace           = 0;
            bool isInBuiltInLang      = false;
            bool needReplaceMacro     = false;
            bool hasPrimaryKeyInMacro = false;
            std::list<rcsToken_T>::iterator begin = iter;
            std::list<rcsToken_T>::iterator iMacroBegin = iter;
            std::list<rcsToken_T>::iterator iMacroEnd   = iter;

            while(iter != m_listTokenStream.end())
            {
                if(iter->eType != BLANK_SPACE)
                    iMacroEnd = iter;

                if(iter->eType == SEPARATOR && iter->sValue == "{")
                {
                    if(0 == nBrace)
                    {
                        ++nBrace;
                        skipBlankSpaceToken(iter);
                        iMacroBegin = iter;
                        continue;
                    }
                    ++nBrace;
                }
                else if(iter->eType == SEPARATOR && iter->sValue == "}")
                {
                    --nBrace;
                    if(nBrace == 0)
                        break;
                }
                else if(iter->eType == SEPARATOR && iter->sValue == "[")
                {
                    isInBuiltInLang = true;
                }
                else if(iter->eType == SEPARATOR && iter->sValue == "]")
                {
                    isInBuiltInLang = false;
                }
                else if(!isInBuiltInLang && iter->eType == LAYER_OPERATION_KEYWORD)
                {
                    hasPrimaryKeyInMacro = true;
                }
                ++iter;
            }

            needReplaceMacro = m_flattenMacro;






            iter = begin;
            nBrace = 0;
            bool needExtractLayer = true;
            std::set<std::string> vDefLayers;
            while(iter != m_listTokenStream.end())
            {
                if(iter->eType == SEPARATOR)
                {
                    if(iter->sValue == "}")
                    {
                        --nBrace;
                    }
                    else if(iter->sValue == "{")
                    {
                        ++nBrace;
                    }
                    else if(iter->sValue == "[")
                    {
                        needExtractLayer = false;
                    }
                    else if(iter->sValue == "]")
                    {
                        needExtractLayer = true;
                    }
                    else if(!needReplaceMacro && needExtractLayer && iter->sValue == "=")
                    {
                        std::list<rcsToken_T>::iterator iterName = iter;
                        --iterName;
                        while(iterName != begin)
                        {
                            if(iterName->eType != BLANK_SPACE)
                                break;
                            --iterName;
                        }

                        if(iterName->eType == SECONDARY_KEYWORD ||
                           iterName->eType == IDENTIFIER_NAME ||
                           iterName->eType == STRING_CONSTANTS)
                        {
                            std::string sLayerName = iterName->sValue;
                            toLower(sLayerName);
                            if(sLayerName[0] != '$' ||
                               vParas.find(sLayerName.substr(1)) == vParas.end())
                            {
                                iterName->sValue = sLayerName;
                                vDefLayers.insert(sLayerName);
                            }
                        }
                    }

                    if(nBrace == 0)
                        break;
                }
                else if(!needReplaceMacro)
                {
                    switch(iter->eType)
                    {
                        case SECONDARY_KEYWORD:
                        case IDENTIFIER_NAME:
                        case STRING_CONSTANTS:
                        {
                            std::string sValue = iter->sValue;
                            toLower(sValue);
                            if(vParas.find(sValue) != vParas.end())
                            {
                                
                                iter->sValue = sValue;
								if(!rcsManager_T::getInstance()->isNewPvrs())
                                	iter->sValue.insert(0, "$");
                            }
                            break;
                        }
                        default:
                            break;
                    }
                }
                ++iter;
            }
            Utassert(iter != m_listTokenStream.end());

            nBrace = 0;
            iter = begin;
            while(iter != m_listTokenStream.end())
            {
                if(iter->eType == SEPARATOR)
                {
                    if(iter->sValue == "}")
                    {
                        --nBrace;
                    }
                    else if(iter->sValue == "{")
                    {
                        ++nBrace;
                    }
                    if(nBrace == 0)
                        break;
                }
                else if(!needReplaceMacro)
                {
                    switch(iter->eType)
                    {
                        case SECONDARY_KEYWORD:
                        case IDENTIFIER_NAME:
                        case STRING_CONSTANTS:
                        {
                            std::string sValue = iter->sValue;
                            toLower(sValue);
                            if(vDefLayers.find(sValue) != vDefLayers.end())
                            {
                                
                                iter->sValue = sValue;
								if(!rcsManager_T::getInstance()->isNewPvrs())
                                	iter->sValue.insert(0, "$");
                            }
                            break;
                        }
                        default:
                            break;
                    }
                }
                ++iter;
            }

            toLower(sMacroName);

            m_macrosParaSizeMap.insert(std::make_pair(sMacroName, vParas.size()));

            if(needReplaceMacro)
            {
                std::map<std::string, MACRO_TOKENS>::iterator iterInsert = m_mapMacros.lower_bound(sMacroName);
                if((iterInsert != m_mapMacros.end()) && (iterInsert->first == sMacroName))
                {
                    s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, MAC3,
                                                        iBegin->nLineNo, (++iBegin)->sValue));
                }

                m_mapMacros.insert(iterInsert, std::make_pair(sMacroName,
                                   MACRO_TOKENS(iBegin, ++iter, vParas.size())));

                iBegin = m_listTokenStream.erase(iBegin, iter);
            }
            else
            {
                iBegin = iter;
            }
            break;
        }
        default:
        {
            s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, MAC1, iBegin->nLineNo));
            break;
        }
    }
}

static bool
convertPreProcessCmd(std::string &sCmdValue, std::string &sValue)
{
    std::vector<std::string> vOperands;
    trim(sCmdValue);

    if(0 == strncasecmp(sCmdValue.c_str(), "#define", 7))
    {
        if(rcsManager_T::getInstance()->isNewPvrs())
        {
            sValue = sCmdValue;
            sValue += "\n";
            return true;
        }

        std::string sOperands = sCmdValue.substr(7);
        rcsSynNodeConvertor_T::parseOperands(sOperands, vOperands);

        if('$' == vOperands[0][0])
        {
            vOperands[0].erase(0, 1);
            sValue += "set env(";
            sValue += vOperands[0];
            sValue += ") ";
        }
        else
        {
            sValue += "set \"::";
            sValue += vOperands[0];
            sValue += "\" ";
        }
        if(2 == vOperands.size())
            sValue += vOperands[1];
        else
            sValue += "1";
    }
    else if(0 == strncasecmp(sCmdValue.c_str(), "#redefine", 9))
    {
        if(rcsManager_T::getInstance()->isNewPvrs())
        {
            sValue = sCmdValue;
            sValue += "\n";
            return true;
        }

        std::string sOperands = sCmdValue.substr(9);
        rcsSynNodeConvertor_T::parseOperands(sOperands, vOperands);

        if('$' == vOperands[0][0])
        {
            vOperands[0].erase(0, 1);
            sValue += "set_resident env(";
            sValue += vOperands[0];
            sValue += ") ";
        }
        else
        {
            sValue += "set_resident \"::";
            sValue += vOperands[0];
            sValue += "\" ";
        }
        if(2 == vOperands.size())
            sValue += vOperands[1];
        else
            sValue += "1";
    }
    else if(0 == strncasecmp(sCmdValue.c_str(), "#undefine", 9))
    {
        if(rcsManager_T::getInstance()->isNewPvrs())
        {
            sValue = sCmdValue;
            sValue += "\n";
            return true;
        }

        std::string sOperands = sCmdValue.substr(9);
        rcsSynNodeConvertor_T::parseOperands(sOperands, vOperands);

        if('$' != vOperands[0][0])
        {
            sValue += "if { [ info exists \"::";
            sValue += vOperands[0];
            sValue += "\" ] } { \n";
            sValue += "\tunset \"::";
            sValue += vOperands[0];
            sValue += "\"\n}";
        }
        else
        {
            vOperands[0].erase(0, 1);
            sValue += "if { [ info exists env(";
            sValue += vOperands[0];
            sValue += ") ] } { \n";
            sValue += "\tunset env";
            sValue += vOperands[0];
            sValue += ")\n}";
        }
    }
    else if(0 == strncasecmp(sCmdValue.c_str(), "#ifdef", 6))
    {
        if(rcsManager_T::getInstance()->isNewPvrs())
        {
            sValue = sCmdValue;
            sValue += "\n";
            return true;
        }

        std::string sOperands = sCmdValue.substr(6);
        rcsSynNodeConvertor_T::parseOperands(sOperands, vOperands);

        if('$' != vOperands[0][0])
        {
            if(2 == vOperands.size())
            {
                sValue += "if { [ info exists \"::";
                sValue += vOperands[0];
                sValue += "\" ] && [ string equal \"$::";
                sValue += vOperands[0];
                sValue += "\" ";
                sValue += vOperands[1];
                sValue += " ] } { ";
            }
            else
            {
                sValue += "if { [ info exists \"::";
                sValue += vOperands[0];
                sValue += "\" ] } { ";
            }
        }
        else
        {
            vOperands[0].erase(0, 1);
            if(2 == vOperands.size())
            {
                sValue += "if { [ info exists env(";
                sValue += vOperands[0];
                sValue += ") ] && [ string equal $env(";
                sValue += vOperands[0];
                sValue += ") ";
                sValue += vOperands[1];
                sValue += " ] } { ";
            }
            else
            {
                sValue += "if { [ info exists env(";
                sValue += vOperands[0];
                sValue += ") ] } { ";
            }
        }
    }
    else if(0 == strncasecmp(sCmdValue.c_str(), "#ifndef", 7))
    {
        if(rcsManager_T::getInstance()->isNewPvrs())
        {
            sValue = sCmdValue;
            sValue += "\n";
            return true;
        }

        std::string sOperands = sCmdValue.substr(7);
        rcsSynNodeConvertor_T::parseOperands(sOperands, vOperands);

        if('$' != vOperands[0][0])
        {
            if(2 == vOperands.size())
            {
                sValue += "if { ![ info exists \"::";
                sValue += vOperands[0];
                sValue += "\" ] || ![ string equal \"$::";
                sValue += vOperands[0];
                sValue += "\" ";
                sValue += vOperands[1];
                sValue += " ] } { ";
            }
            else
            {
                sValue += "if { ![ info exists \"::";
                sValue += vOperands[0];
                sValue += "\" ] } { ";
            }
        }
        else
        {
            vOperands[0].erase(0, 1);
            if(2 == vOperands.size())
            {
                sValue += "if { ![ info exists env(";
                sValue += vOperands[0];
                sValue += ") ] || ![ string equal $env(";
                sValue += vOperands[0];
                sValue += ") ";
                sValue += vOperands[1];
                sValue += " ] } { ";
            }
            else
            {
                sValue += "if { ![ info exists env(";
                sValue += vOperands[0];
                sValue += ") ] } { ";
            }
        }
    }
    else if(0 == strncasecmp(sCmdValue.c_str(), "#else", 5))
    {
        if(rcsManager_T::getInstance()->isNewPvrs())
        {
            sValue = sCmdValue;
            sValue += "\n";
            return true;
        }

        sValue += "} else {";
    }
    else if(0 == strncasecmp(sCmdValue.c_str(), "#endif", 6))
    {
        if(rcsManager_T::getInstance()->isNewPvrs())
        {
            sValue = sCmdValue;
            sValue += "\n";
            return true;
        }

        sValue += "}";
    }
    else
    {
        return false;
    }

    sValue += "\n";
    return true;
}

template <>
void
rcsLexMacroParser_T::parseBuildInLang<false>()
{
    std::list<rcsToken_T>::iterator iter = m_listTokenStream.begin();
    std::list<rcsToken_T>::iterator iKey = m_listTokenStream.begin();
    bool needTransBuiltIn = true;
    bool isInDMacro       = false;
    while(iter != m_listTokenStream.end())
    {
        if(iter->eType == SPECIFICATION_KEYWORD)
        {
            needTransBuiltIn = iter->eKeyType != TVF_FUNCTION;
            iKey = iter;
        }
        else if(iter->eType == LAYER_OPERATION_KEYWORD)
        {
            needTransBuiltIn = iter->eKeyType != EXTERNAL && iter->eKeyType != ENCLOSURE &&
                               iter->eKeyType != INTERNAL && iter->eKeyType != TDDRC;
            iKey = iter;
        }
        else if(iter->eType == DMACRO)
        {
            isInDMacro = true;
            while(iter != m_listTokenStream.end())
            {
                if(iter->eType == SEPARATOR && iter->sValue == "{")
                    break;
                ++iter;
            }
        }
        else if(isInDMacro && iter->eType == SEPARATOR && iter->sValue == "}")
        {
            isInDMacro = false;
        }
        else if(iter->eType == SEPARATOR && iter->sValue == "[")
        {
            std::list<rcsToken_T>::iterator begin = iter;
            rcsToken_T token(BUILT_IN_LANG, iter->nLineNo, iter->sValue.c_str());
            token.namescopes = iter->namescopes;
            while(++iter != m_listTokenStream.end())
            {
                if(iter->eType == SEPARATOR && iter->sValue == "]")
                {
                    token.sValue += iter->sValue;
                    std::string sValue = token.sValue;
                    token.namescopes.push_back(sValue);

                    if(needTransBuiltIn)
                    {
                        sValue.erase(0, 1);
                        sValue.erase(sValue.size() - 1);
                        std::vector<std::string> vLines;
                        tokenize(vLines, sValue, "\n\r");
                        sValue.clear();
                        sValue += "[\n";
                        for(hvUInt32 iLine = 0; iLine < vLines.size(); ++iLine)
                        {
                            std::string sLine = vLines[iLine];
                            std::string sResult;
                            if(convertPreProcessCmd(sLine, sResult))
                            {
                                sValue += sResult;
                            }
                            else
                            {
                                trim(vLines[iLine]);
                                if(vLines[iLine].empty() &&
                                   vLines.size() == iLine + 1)
                                    continue;

                                if(!rcsManager_T::getInstance()->isNewPvrs())
                                {
									sValue += "rcn::built_in { ";
									sValue += vLines[iLine];
									if(vLines.size() > 1)
										sValue += " }\n";
									else
										sValue += " }";
                                }
                                else
                                {
									sValue += vLines[iLine];
									if(vLines.size() > 1)
										sValue += " \n";
                                }
                            }
                        }
                        sValue += "]";
                    }
                    else if(iKey->eKeyType == TVF_FUNCTION)
                    {
                        sValue.erase(0, 1);
                        sValue.erase(sValue.size() - 1);
                        std::vector<std::string> vLines;
                        tokenize(vLines, sValue, "\n\r");
                        sValue.clear();
                        sValue += "[\n";

                        for(hvUInt32 iLine = 0; iLine < vLines.size(); ++iLine)
                        {
                            std::string sLine = vLines[iLine];
                            trim(sLine);
                            if(isTclPackageLine(sLine))
                            {
                                
                                sValue += vLines[iLine];
                                sValue += "\n";
                                continue;
                            }

                            sLine = vLines[iLine];
                            std::string sResult;
                            for(std::string::size_type i = 0; i < sLine.size(); ++i)
                            {
                                if(sLine[i] == '[')
                                {
                                    sResult += sLine[i];
                                    ++i;
                                    while(i < sLine.size() && (sLine[i] == ' ' || sLine[i] == '\t'))
                                    {
                                        sResult += sLine[i];
                                        ++i;
                                    }

                                    if(i < sLine.size() && sLine[i] == '$')
                                    {
                                        while(i < sLine.size() && sLine[i] != ' ' && sLine[i] != '\t')
                                        {
                                            sResult += sLine[i];
                                            ++i;
                                        }

                                        while(i < sLine.size() && (sLine[i] == ' ' || sLine[i] == '\t'))
                                        {
                                            sResult += sLine[i];
                                            ++i;
                                        }

                                        if(i < sLine.size())
                                        {
                                            std::string::size_type j = sLine.find_first_of(" \t", i);
                                            std::string sCode = (j != std::string::npos) ? sLine.substr(i, j - i)
                                                                                         : sLine.substr(i);

                                            if(strcasecmp(sCode.c_str(), "a") == 0)
                                            {
                                                sResult += "u";
                                            }
                                            else if(strcasecmp(sCode.c_str(), "b") == 0)
                                            {
                                                sResult += "v";
                                            }
                                            else if(strcasecmp(sCode.c_str(), "finger_count") == 0)
                                            {
                                                sResult += "group_count";
                                            }
                                            else if(strcasecmp(sCode.c_str(), "slice_count") == 0)
                                            {
                                                sResult += "cut_count";
                                            }
                                            else if(strcasecmp(sCode.c_str(), "row_count") == 0)
                                            {
                                                sResult += "tuple_count";
                                            }
                                            else if(strcasecmp(sCode.c_str(), "measurement_count") == 0)
                                            {
                                                sResult += "tuple_size";
                                            }
                                            else
                                            {
                                                sResult += sCode;
                                            }

                                            i = j != std::string::npos ? j - 1 : sLine.size();
                                        }
                                    }
                                    else
                                    {
                                        if(i < sLine.size())
                                        {
                                            std::string::size_type j = sLine.find_first_of(" \t", i);
                                            std::string sCode = (j != std::string::npos) ? sLine.substr(i, j - i)
                                                                                         : sLine.substr(i);

                                            if(strcasecmp(sCode.c_str(), "device::dfm_vec_measurements") == 0 ||
                                                    strcasecmp(sCode.c_str(), "dfm_vec_measurements") == 0)
                                            {
                                                sResult += "device::convert_to_enc";
                                                i = j != std::string::npos ? j - 1 : sLine.size();
                                            }
                                            else if(strcasecmp(sCode.c_str(), "device::scaled_dfm_vec_measurements") == 0 ||
                                                    strcasecmp(sCode.c_str(), "scaled_dfm_vec_measurements") == 0)
                                            {
                                                sResult += "device::convert_to_scaled_enc";
                                                i = j != std::string::npos ? j - 1 : sLine.size();
                                            }
                                            else
                                            {
                                                sResult += sLine[i];
                                            }
                                        }
                                        continue;
                                    }
                                }
                                else
                                {
                                    sResult += sLine[i];
                                }
                            }
                            sValue += sResult;
                            sValue += "\n";
                        }
                        sValue += "]";
                    }

                    token.sValue = sValue;
                    iter = m_listTokenStream.erase(begin, ++iter);
                    iter = m_listTokenStream.insert(iter, token);

                    break;
                }

                if(iter->eType == STRING_CONSTANTS)
                {
                    token.sValue += "\"";
                    token.sValue += iter->sValue;
                    token.sValue += "\"";
                }
                else if(!rcsManager_T::getInstance()->isNewPvrs() && iter->eType == SEPARATOR &&
                        (iter->sValue == "{" || iter->sValue == "}"))
                {
                    token.sValue += "\\";
                    token.sValue += iter->sValue;
                }
                else if(needTransBuiltIn && (iter->eType == SECONDARY_KEYWORD || iter->eType == IDENTIFIER_NAME))
                {
                    std::list<rcsToken_T>::iterator iToken = iter;
                    skipBlankSpaceToken(iToken);
                    if(iToken->eType != SEPARATOR || iToken->sValue != "(")
                    {
                        if(iter->eType == IDENTIFIER_NAME &&
                           (strncasecmp(iter->sValue.c_str(), "tvf_number_function", 19) == 0 ||
                            strncasecmp(iter->sValue.c_str(), "tvf_number_func", 15) == 0 ||
                            strncasecmp(iter->sValue.c_str(), "tvf_number_fun", 14) == 0 ||
                            strncasecmp(iter->sValue.c_str(), "tvf_num_function", 16) == 0 ||
                            strncasecmp(iter->sValue.c_str(), "tvf_num_func", 12) == 0 ||
                            strncasecmp(iter->sValue.c_str(), "tvf_num_fun", 11) == 0 ||
                            strncasecmp(iter->sValue.c_str(), "tvf_numeric_function", 20) == 0))
                        {
                            
                            token.sValue += "TRS_NUMBER_FUNCTION";
                            if(strncasecmp(iter->sValue.c_str(), "tvf_number_function", 19) == 0)
                            {
                                if(iter->sValue.size() > 19)
                                    token.sValue += iter->sValue.substr(19);
                            }
                            else if(strncasecmp(iter->sValue.c_str(), "tvf_number_func", 15) == 0)
                            {
                                if(iter->sValue.size() > 15)
                                    token.sValue += iter->sValue.substr(15);
                            }
                            else if(strncasecmp(iter->sValue.c_str(), "tvf_number_fun", 14) == 0)
                            {
                                if(iter->sValue.size() > 14)
                                    token.sValue += iter->sValue.substr(14);
                            }
                            else if(strncasecmp(iter->sValue.c_str(), "tvf_num_function", 16) == 0)
                            {
                                if(iter->sValue.size() > 16)
                                    token.sValue += iter->sValue.substr(16);
                            }
                            else if(strncasecmp(iter->sValue.c_str(), "tvf_num_func", 12) == 0)
                            {
                                if(iter->sValue.size() > 12)
                                    token.sValue += iter->sValue.substr(12);
                            }
                            else if(strncasecmp(iter->sValue.c_str(), "tvf_num_fun", 11) == 0)
                            {
                                if(iter->sValue.size() > 11)
                                    token.sValue += iter->sValue.substr(11);
                            }
                            else if(iter->sValue.size() > 20)
                            {
                                token.sValue += iter->sValue.substr(20);
                            }
                        }
                        else if(iter->eType == IDENTIFIER_NAME && (strncasecmp(iter->sValue.c_str(), "tvf_str_fun", 11) == 0 ||
                                strncasecmp(iter->sValue.c_str(), "tvf_string_function", 19) == 0))
                        {
                            
                            token.sValue += "TRS_STRING_FUNCTION";
                            if(strncasecmp(iter->sValue.c_str(), "tvf_str_fun", 11) == 0 && iter->sValue.size() > 11)
                                token.sValue += iter->sValue.substr(11);
                            else if(iter->sValue.size() > 19)
                                token.sValue += iter->sValue.substr(19);
                        }
                        else if(iter->eType == IDENTIFIER_NAME && iter->sValue.find(';') != std::string::npos)
                        {
                            token.sValue += "\"";
                            token.sValue += iter->sValue;
                            token.sValue += "\"";
                        }
                        else
                        {
                            token.sValue += iter->sValue;
                        }
                    }
                    else
                    {
                        if(strcasecmp(iter->sValue.c_str(), "area_common") == 0 ||
                           strcasecmp(iter->sValue.c_str(), "area_com") == 0)
                        {
                            token.sValue += "area_overlap";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "perimeter") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "perim") == 0)
						{
							token.sValue += "perimeter";
						}
                        else if(strcasecmp(iter->sValue.c_str(), "PERIMETERX") == 0 ||
                        		strcasecmp(iter->sValue.c_str(), "PERIMX") == 0)
                        {
                            token.sValue += "PERIMETER_X";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "PERIMETERXP") == 0 ||
                        		strcasecmp(iter->sValue.c_str(), "PERIMXP") == 0)
                        {
                            token.sValue += "PERIMETER_XS";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "PERIMETERY") == 0 ||
                        		strcasecmp(iter->sValue.c_str(), "PERIMY") == 0)
                        {
                            token.sValue += "PERIMETER_Y";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "PERIMETERYP") == 0 ||
                        		strcasecmp(iter->sValue.c_str(), "PERIMYP") == 0)
                        {
                            token.sValue += "PERIMETER_YS";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "LENGTHX") == 0)
                        {
                            token.sValue += "LENGTH_X";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "LENGTHXP") == 0)
                        {
                            token.sValue += "LENGTH_XS";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "LENGTHY") == 0)
                        {
                            token.sValue += "LENGTH_Y";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "LENGTHYP") == 0)
                        {
                            token.sValue += "LENGTH_YS";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "EC") == 0)
                        {
                            token.sValue += "EP";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "ECMAX") == 0)
                        {
                            token.sValue += "EP_MAX";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "ECMIN") == 0)
                        {
                            token.sValue += "EP_MIN";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "ECBEGIN") == 0)
                        {
                            token.sValue += "EP_BEGIN";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "ECEND") == 0)
                        {
                            token.sValue += "EP_END";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "ECX") == 0)
                        {
                            token.sValue += "EP_X";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "ECXP") == 0)
                        {
                            token.sValue += "EP_XS";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "ECY") == 0)
                        {
                            token.sValue += "EP_Y";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "ECYP") == 0)
                        {
                            token.sValue += "EP_YS";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "EW") == 0)
                        {
                            token.sValue += "ED";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "EWP") == 0)
                        {
                            token.sValue += "ED_P";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "EWX") == 0)
                        {
                            token.sValue += "ED_X";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "EWXP") == 0)
                        {
                            token.sValue += "ED_XS";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "EWY") == 0)
                        {
                            token.sValue += "ED_Y";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "EWYP") == 0)
                        {
                            token.sValue += "ED_YS";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "SPROPERTY") == 0)
                        {
                            token.sValue += "S_PROPERTY";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "PSPROPERTY") == 0)
                        {
                            token.sValue += "PS_PROPERTY";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "VPROPERTY") == 0)
                        {
                            token.sValue += "V_PROPERTY";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "SVPROPERTY") == 0)
                        {
                            token.sValue += "SV_PROPERTY";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "NETPROPERTY") == 0)
                        {
                            token.sValue += "N_PROPERTY";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "NETVPROPERTY") == 0)
                        {
                            token.sValue += "NV_PROPERTY";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "NIPROPERTY") == 0)
                        {
                            token.sValue += "NI_PROPERTY";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "NETPROPERTY_REF") == 0)
                        {
                            token.sValue += "N_PROPERTY_REF";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "NETVPROPERTY_REF") == 0)
                        {
                            token.sValue += "NV_PROPERTY_REF";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "SPROPERTY_REF") == 0)
                        {
                            token.sValue += "S_PROPERTY_REF";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "PSPROPERTY_REF") == 0)
                        {
                            token.sValue += "PS_PROPERTY_REF";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "VPROPERTY_REF") == 0)
                        {
                            token.sValue += "V_PROPERTY_REF";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "SVPROPERTY_REF") == 0)
                        {
                            token.sValue += "SV_PROPERTY_REF";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "SVPROPERTY_REF") == 0)
						{
							token.sValue += "SV_PROPERTY_REF";
						}
                        else if(strcasecmp(iter->sValue.c_str(), "GLOBALNETID") == 0)
						{
							token.sValue += "GLOBAL_NETID";
						}
                        else if(strcasecmp(iter->sValue.c_str(), "VNETID") == 0)
                        {
                            token.sValue += "V_NETID";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "RANGE_XXY") == 0)
                        {
                            token.sValue += "SELECT_XXY";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "SELECT_MERGE_XXY") == 0)
                        {
                            token.sValue += "SORT_XXY_N";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "SORT_MERGE_XXY") == 0)
                        {
                            token.sValue += "SORT_XXY";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "VBYNETMIN") == 0)
                        {
                            token.sValue += "V_NET_MIN";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "VBYNETMAX") == 0)
                        {
                            token.sValue += "V_NET_MAX";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "VMIN") == 0)
                        {
                            token.sValue += "V_MIN";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "VMAX") == 0)
                        {
                            token.sValue += "V_MAX";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "VNETIDMIN") == 0)
                        {
                            token.sValue += "V_NETID_MIN";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "VNETIDMAX") == 0)
                        {
                            token.sValue += "V_NETID_MAX";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "VNETSUM") == 0)
                        {
                            token.sValue += "V_NET_SUM";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "VSTRING") == 0)
                        {
                            token.sValue += "V_STRING";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "bends") == 0)
                        {
                            token.sValue += "count_bend";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "enclosure_parallel") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "enc_par") == 0)
                        {
                            token.sValue += "enc_parallel";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "enclosure_parallel_multifinger") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "enc_par_mul") == 0)
                        {
                            token.sValue += "enc_parallel_fingers";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "enclosure_perpendicular") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "enc_per") == 0)
                        {
                            token.sValue += "enc_perpendicular";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "enclosure_perpendicular_multifinger") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "enc_per_mul") == 0)
                        {
                            token.sValue += "enc_perpendicular_fingers";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "enclosure_vector") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "enc_vec") == 0)
                        {
                            token.sValue += "enc_tuple";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "dfm_numeric_value") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "dfm_num_val") == 0)
                        {
                            token.sValue += "dfm_value";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "dfm_numeric_array") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "dfm_num_arr") == 0)
                        {
                            token.sValue += "dfm_tuple";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "dfm_vector_value") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "dfm_vec_val") == 0)
                        {
                            token.sValue += "dfm_multi_value";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "dfm_vector_array") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "dfm_vec_arr") == 0)
                        {
                            token.sValue += "dfm_multi_tuple";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "perimeter_inside") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "perim_in") == 0)
                        {
                            token.sValue += "inside_length";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "perimeter_outside") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "perim_out") == 0)
                        {
                            token.sValue += "outside_length";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "perimeter_coincide") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "perim_co") == 0)
                        {
                            token.sValue += "common_length";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "perimeter_coincide_inside") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "perim_co_in") == 0)
                        {
                            token.sValue += "inside_common_length";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "perimeter_coincide_outside") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "perim_co_out") == 0)
                        {
                            token.sValue += "outside_common_length";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "instance") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "inst") == 0)
                        {
                            token.sValue += "device_id";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "named_net") == 0)
                        {
                            token.sValue += "name_to_net_id";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "pin_net") == 0)
                        {
                            token.sValue += "net_id_of_pin";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "unit_capacitance") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "unit_cap") == 0)
                        {
                            token.sValue += "cap_unit";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "unit_length") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "unit_len") == 0)
                        {
                            token.sValue += "length_unit";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "unit_resistance") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "unit_res") == 0)
                        {
                            token.sValue += "res_unit";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "x_location") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "x_loc") == 0)
                        {
                            token.sValue += "x_coord";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "y_location") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "y_loc") == 0)
                        {
                            token.sValue += "y_coord";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "text_string") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "text_str") == 0)
                        {
                            token.sValue += "label_name";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "text_numeric") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "text_num") == 0)
                        {
                            token.sValue += "label_value";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "layout_numeric_value") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "lay_num_val") == 0)
                        {
                            token.sValue += "layout_value";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "source_numeric_value") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "src_num_val") == 0)
                        {
                            token.sValue += "schematic_value";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "layout_string_value") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "lay_str_val") == 0)
                        {
                            token.sValue += "layout_string_value";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "source_string_value") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "src_str_val") == 0)
                        {
                            token.sValue += "schematic_string_value";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "string_compare") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "str_comp") == 0)
                        {
                            token.sValue += "compare_string";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "report_numeric_discrepancy") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "rep_num_disc") == 0)
                        {
                            token.sValue += "report_error_percentage";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "report_numeric_value") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "rep_num_val") == 0)
                        {
                            token.sValue += "report_error_value";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "report_string_discrepancy") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "rep_str_disc") == 0)
                        {
                            token.sValue += "report_error_string";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "report_message") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "rep_msg") == 0)
                        {
                            token.sValue += "report_error_message";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "input_numeric_value") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "inp_num_val") == 0)
                        {
                            token.sValue += "get_value";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "is_missing") == 0)
                        {
                            token.sValue += "is_value_not_exist";
                        }
                        else if( (strncasecmp(iter->sValue.c_str(), "tvf_num_function", 16) == 0) ||
                                 (strncasecmp(iter->sValue.c_str(), "tvf_num_func", 12) == 0) ||
                                 (strncasecmp(iter->sValue.c_str(), "tvf_num_fun", 11) == 0) )
                        {
                            
                        	token.sValue += "TRS_NUMBER_FUNCTION";
                            if(strncasecmp(iter->sValue.c_str(), "tvf_num_function", 16) == 0)
                            {
                                if(iter->sValue.size() > 16)
                                    token.sValue += iter->sValue.substr(16);
                            }
                            else if(strncasecmp(iter->sValue.c_str(), "tvf_num_func", 12) == 0)
                            {
                                if(iter->sValue.size() > 12)
                                    token.sValue += iter->sValue.substr(12);
                            }
                            else if(iter->sValue.size() > 11)
                            {
                                token.sValue += iter->sValue.substr(11);
                            }
                        }
                        else if( (strncasecmp(iter->sValue.c_str(), "tvf_number_function", 19) == 0) ||
                                 (strncasecmp(iter->sValue.c_str(), "tvf_number_func", 15) == 0) ||
                                 (strncasecmp(iter->sValue.c_str(), "tvf_number_fun", 14) == 0) )
                        {

                            token.sValue += "TRS_NUMBER_FUNCTION";
                            if(strncasecmp(iter->sValue.c_str(), "tvf_number_function", 19) == 0)
                            {
                                if(iter->sValue.size() > 19)
                                    token.sValue += iter->sValue.substr(19);
                            }
                            else if(strncasecmp(iter->sValue.c_str(), "tvf_number_func", 15) == 0)
                            {
                                if(iter->sValue.size() > 15)
                                    token.sValue += iter->sValue.substr(15);
                            }
                            else if(iter->sValue.size() > 14)
                            {
                                token.sValue += iter->sValue.substr(14);
                            }
                        }
                        else if(strncasecmp(iter->sValue.c_str(), "tvf_numeric_function", 20) == 0)
                        {
                            
                            token.sValue += "TRS_NUMBER_FUNCTION";
                            if(iter->sValue.size() > 20)
                                token.sValue += iter->sValue.substr(20);
                        }
                        else if(strncasecmp(iter->sValue.c_str(), "tvf_numeric_fun", 15) == 0)
                        {
                            
                            token.sValue += "TRS_NUMBER_FUNCTION";
                            if(iter->sValue.size() > 15)
                                token.sValue += iter->sValue.substr(15);
                        }
                        else if(strncasecmp(iter->sValue.c_str(), "tvf_str_function", 16) == 0)
                        {
                            
                            token.sValue += "TRS_STRING_FUNCTION";
                            if(iter->sValue.size() > 16)
                                token.sValue += iter->sValue.substr(16);
                        }
                        else if(strncasecmp(iter->sValue.c_str(), "tvf_str_fun", 11) == 0 ||
                                strncasecmp(iter->sValue.c_str(), "tvf_string_function", 19) == 0)
                        {
                            
                            token.sValue += "TRS_STRING_FUNCTION";
                            if(strncasecmp(iter->sValue.c_str(), "tvf_str_fun", 11) == 0 && iter->sValue.size() > 11)
                                token.sValue += iter->sValue.substr(11);
                            else if(iter->sValue.size() > 19)
                                token.sValue += iter->sValue.substr(19);
                        }
                        else if(strncasecmp(iter->sValue.c_str(), "tvf_string_fun", 14) == 0)
                        {
                            
                            token.sValue += "TRS_STRING_FUNCTION";
                            if(iter->sValue.size() > 14)
                                token.sValue += iter->sValue.substr(14);
                        }
                        else if(strncasecmp(iter->sValue.c_str(), "EFFECTIVE", 11) == 0)
                        {
                        	token.sValue += "PROPERTY";
                        }
                        else if(strncasecmp(iter->sValue.c_str(), "EFFECTIVE STRING", 11) == 0)
                        {
                        	token.sValue += "STRING PROPERTY";
                        }
                        else
                        {
                            token.sValue += iter->sValue;
                        }
                    }
                }
                else if(iKey->eKeyType == TVF_FUNCTION && (iter->eType == SECONDARY_KEYWORD || iter->eType == IDENTIFIER_NAME))
                {
                    if(strcasecmp(iter->sValue.c_str(), "device::dfm_vec_measurements") == 0)
                    {
                        token.sValue += "device::convert_to_enc";
                    }
                    else if(strcasecmp(iter->sValue.c_str(), "device::scaled_dfm_vec_measurements") == 0)
                    {
                        token.sValue += "device::convert_to_scaled_enc";
                    }
                    else if(strcasecmp(iter->sValue.c_str(), "device::eval_dfm_func") == 0)
                    {
                        token.sValue += "device::dfm_lookup_table";
                    }
                    else
                    {
                        token.sValue += iter->sValue;
                    }
                }
                else if(iter->eType == IDENTIFIER_NAME && iter->sValue.find(';') != std::string::npos)
                {
                    token.sValue += "\"";
                    token.sValue += iter->sValue;
                    token.sValue += "\"";
                }
                else
                {
                    token.sValue += iter->sValue;
                }
            }

            if(iter == m_listTokenStream.end())
            {
                Utassert(false);
                break;
            }
        }

        if(!rcsManager_T::getInstance()->outputComment())
            Utassert(iter->eType != COMMENT);

        ++iter;
    }
}

static bool
isSepElement(std::list<rcsToken_T>::iterator iter)
{
    return (iter->eType == PRO_KEYWORD && (iter->eKeyType == PRO_IFDEF ||
            iter->eKeyType == PRO_IFNDEF || iter->eKeyType == PRO_ELSE ||
            iter->eKeyType == PRO_ENDIF)) || (iter->eType == SEPARATOR && (
            iter->sValue == "[" || iter->sValue == "]"));
}


void
rcsLexMacroParser_T::constructBuildInFragments()
{
#ifdef __DEBUG__
    {
        std::cout << "ENTER rcsLexMacroParser_T::constructBuildInFragments" << std::endl;
    }
#endif
    typedef std::pair<TOKEN_ITER, hvUInt32> TOKEN_ITER_ID;
    typedef std::vector<std::pair<TOKEN_ITER_ID, TOKEN_ITER_ID> > FRAGMENT_CONTAINER;

    
    std::vector<TOKEN_ITER_ID> vSepElements;
    {
        hvUInt32 nTokenID = 0;
        for (TOKEN_ITER iter = m_listTokenStream.begin();
                iter != m_listTokenStream.end(); iter++)
        {
            if (isSepElement(iter))
            {
                vSepElements.push_back(std::make_pair(iter, nTokenID));
            }
            nTokenID++;
        }
        if (vSepElements.empty()) return;
        TOKEN_ITER begin = m_listTokenStream.begin();
        TOKEN_ITER end = --m_listTokenStream.end();
        if (vSepElements.front().first != begin)
        {
            vSepElements.insert(vSepElements.begin(), std::make_pair(begin, 0));
        }
        if (vSepElements.back().first != end)
        {
            vSepElements.push_back(std::make_pair(end, nTokenID - 1));
        }
    }
#ifdef __DEBUG__
    {
        std::cout << "----------- SEP ELEMENTS ------------" << std::endl;
        hvUInt32 nLineNo = vSepElements.begin()->first->nLineNo;
        for (std::vector<TOKEN_ITER_ID>::iterator iter = vSepElements.begin();
                iter != vSepElements.end(); iter++)
        {
            if (iter->first->nLineNo != nLineNo)
            {
                nLineNo = iter->first->nLineNo;
                std::cout << std::endl;
            }
            std::cout << iter->first->sValue << "(line:" << iter->first->nLineNo << ") ";
        }
        std::cout << std::endl;
    }
#endif

    FRAGMENT_CONTAINER vForwardsFragments;
    {
        bool isInBuildIn = false;
        TOKEN_ITER_ID begin;
        std::vector<KEY_TYPE> vCondStack;
        for (std::vector<TOKEN_ITER_ID>::iterator iter = vSepElements.begin();
                iter != vSepElements.end(); iter++)
        {
            if (iter->first->eType == SEPARATOR)
            {
                if (iter->first->sValue == "[" && !isInBuildIn)
                {
                    isInBuildIn = true;
                    begin = *iter;
                    vCondStack.clear();
                }
                else if (iter->first->sValue == "]" && isInBuildIn)
                {
                    isInBuildIn = false;
                    vForwardsFragments.push_back(std::make_pair(begin, *iter));
                }
            }
            else if (iter->first->eType == PRO_KEYWORD && isInBuildIn)
            {
                switch (iter->first->eKeyType)
                {
                    case PRO_IFDEF:
                    case PRO_IFNDEF:
                        vCondStack.push_back(PRO_IFDEF);
                        break;
                    case PRO_ELSE:
                        if (vCondStack.empty())
                        {
                            isInBuildIn = false;
                            vForwardsFragments.push_back(std::make_pair(begin, *iter));
                        }
                        break;
                    case PRO_ENDIF:
                        if (vCondStack.empty())
                        {
                            isInBuildIn = false;
                            vForwardsFragments.push_back(std::make_pair(begin, *iter));
                        }
                        else
                        {
                            vCondStack.pop_back();
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        if (isInBuildIn)
        {
            vForwardsFragments.push_back(std::make_pair(begin, vSepElements.back()));
        }
    }
#ifdef __DEBUG__
    {
        std::cout << "--------- FORWARDS FRAGMENTS ---------" << std::endl;
        for (FRAGMENT_CONTAINER::iterator iter = vForwardsFragments.begin();
                iter != vForwardsFragments.end(); iter++)
        {
            std::cout << iter->first.first->sValue << "(line:" << iter->first.first->nLineNo
                      << ") -> "
                      << iter->second.first->sValue << "(line:" << iter->second.first->nLineNo
                      << ")" << std::endl;
        }
    }
#endif

    FRAGMENT_CONTAINER vBackwardsFragments;
    {
        TOKEN_ITER_ID end;
        bool isInBuildIn = false;
        std::vector<KEY_TYPE> vCondStack;
        for (std::vector<TOKEN_ITER_ID>::iterator iter = vSepElements.end();
                iter != vSepElements.begin();)
        {
            --iter;
            if (iter->first->eType == SEPARATOR)
            {
                if (iter->first->sValue == "]" && !isInBuildIn)
                {
                    isInBuildIn = true;
                    vCondStack.clear();
                    end = *iter;
                }
                else if (iter->first->sValue == "[" && isInBuildIn)
                {
                    isInBuildIn = false;
                    vBackwardsFragments.push_back(std::make_pair(*iter, end));
                }
            }
            else if (iter->first->eType == PRO_KEYWORD && isInBuildIn)
            {
                switch (iter->first->eKeyType)
                {
                    case PRO_ENDIF:
                        vCondStack.push_back(PRO_ENDIF);
                        break;
                    case PRO_IFDEF:
                    case PRO_IFNDEF:
                        if (vCondStack.empty())
                        {
                            isInBuildIn = false;
                            vBackwardsFragments.push_back(std::make_pair(*iter, end));
                        }
                        else
                        {
                            vCondStack.pop_back();
                        }
                        break;
                    case PRO_ELSE:
                        if (vCondStack.empty())
                        {
                            isInBuildIn = false;
                            vBackwardsFragments.push_back(std::make_pair(*iter, end));
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        if (isInBuildIn)
        {
            vForwardsFragments.push_back(std::make_pair(vSepElements.front(), end));
        }
    }
#ifdef __DEBUG__
    {
        std::cout << "--------- BACKWARDS FRAGMENTS ---------" << std::endl;
        for (FRAGMENT_CONTAINER::iterator iter = vBackwardsFragments.begin();
                iter != vBackwardsFragments.end(); iter++)
        {
            std::cout << iter->first.first->sValue << "(line:" << iter->first.first->nLineNo
                      << ") -> "
                      << iter->second.first->sValue << "(line:" << iter->second.first->nLineNo
                      << ")" << std::endl;
        }
    }
#endif

    {
        // && may be change 2  "||" , it will core at someday, commented by st.233
        if (vForwardsFragments.empty() && vBackwardsFragments.empty()) return;
        std::reverse(vBackwardsFragments.begin(), vBackwardsFragments.end());
        FRAGMENT_CONTAINER::iterator itF = vForwardsFragments.begin();
        FRAGMENT_CONTAINER::iterator itFEnd = vForwardsFragments.end();
        FRAGMENT_CONTAINER::iterator itB = vBackwardsFragments.begin();
        FRAGMENT_CONTAINER::iterator itBEnd = vBackwardsFragments.end();
        TOKEN_ITER_ID begin;
        TOKEN_ITER_ID end;
        {
            FRAGMENT_CONTAINER::iterator &it = itF->first.second < itB->first.second ? itF : itB;
            begin = it->first;
            end = it->second;
            it++;
        }
        while (itF != itFEnd || itB != itBEnd)
        {
            FRAGMENT_CONTAINER::iterator &it = (itF == itFEnd) ? itB :
                    (itB == itBEnd ? itF : (itF->first.second < itB->first.second ? itF : itB));
            if (end.second < it->first.second)
            {
                m_vBuildInFragments.push_back(BUILDIN_FRAGMENT(begin.first, end.first));
                begin = it->first;
                end = it->second;
            }
            else if (end.second < it->second.second)
            {
                end = it->second;
            }
            it++;
        }
        m_vBuildInFragments.push_back(BUILDIN_FRAGMENT(begin.first, end.first));
    }
#ifdef __DEBUG__
    {
        std::cout << "---------- THE BULIDIN FRAGMENTS -------------" << std::endl;
        for (BUILDIN_FRAGMENT_CONTAINER::iterator iter = m_vBuildInFragments.begin();
                iter != m_vBuildInFragments.end(); iter++)
        {
            std::cout << iter->begin->sValue << "(line:" << iter->begin->nLineNo
                      << ") -> " << iter->end->sValue << "(line:"
                      << iter->end->nLineNo << ")" << std::endl;
        }
    }
#endif

    {
        
        BUILDIN_FRAGMENT_CONTAINER::iterator iter = m_vBuildInFragments.begin();
        while (iter != m_vBuildInFragments.end())
        {
            BUILDIN_FRAGMENT_CONTAINER::iterator preIter = iter;
            iter++;
            
            
            if (iter != m_vBuildInFragments.end() && preIter->end->eType != SEPARATOR &&
                    iter->begin->eType != SEPARATOR)
            {
                preIter->end = iter->end;
                iter = m_vBuildInFragments.erase(iter);
            }
        }
    }

#ifdef __DEBUG__
    {
        std::cout << "EXIT rcsLexMacroParser_T::constructBuildInFragments" << std::endl;
    }
#endif
}

template<>
void
rcsLexMacroParser_T::parseBuildInLang<true>()
{
    constructBuildInFragments();
    if (m_vBuildInFragments.empty()) return;
    BUILDIN_FRAGMENT_CONTAINER::iterator itFrag = m_vBuildInFragments.begin();

    std::list<rcsToken_T>::iterator iter = m_listTokenStream.begin();
    std::list<rcsToken_T>::iterator iKey = m_listTokenStream.begin();
    bool needTransBuiltIn = true;
    bool isInDMacro       = false;
    while(iter != m_listTokenStream.end())
    {
        if(iter->eType == SPECIFICATION_KEYWORD)
        {
            needTransBuiltIn = iter->eKeyType != TVF_FUNCTION;
            iKey = iter;
        }
        else if(iter->eType == LAYER_OPERATION_KEYWORD)
        {
            needTransBuiltIn = iter->eKeyType != EXTERNAL && iter->eKeyType != ENCLOSURE &&
                               iter->eKeyType != INTERNAL && iter->eKeyType != TDDRC;
            iKey = iter;
        }
        else if(iter->eType == DMACRO)
        {
            isInDMacro = true;
            while(iter != m_listTokenStream.end())
            {
                if(iter->eType == SEPARATOR && iter->sValue == "{")
                    break;
                ++iter;
            }
        }
        else if(isInDMacro && iter->eType == SEPARATOR && iter->sValue == "}")
        {
            isInDMacro = false;
        }
        else if(isSepElement(iter) && iter == itFrag->begin)
        {
            std::list<rcsToken_T>::iterator begin = iter;
            rcsToken_T token(BUILT_IN_LANG, iter->nLineNo, iter->sValue.c_str());
            if (iter->eType == PRO_KEYWORD)
                token.sValue += "\n";

            token.namescopes = iter->namescopes;
            while(++iter != m_listTokenStream.end())
            {
                if(isSepElement(iter) && iter == itFrag->end)
                {
                    token.sValue += iter->sValue;
                    std::string sValue = token.sValue;
                    token.namescopes.push_back(sValue);

                    if(needTransBuiltIn)
                    {
                        if (itFrag->begin->eType == SEPARATOR && itFrag->begin->sValue == "[")
                        {
                            sValue.erase(0, 1);
                        }
                        if (itFrag->end->eType == SEPARATOR && itFrag->end->sValue == "]")
                        {
                            sValue.erase(sValue.size() - 1);
                        }
                        std::vector<std::string> vLines;
                        tokenize(vLines, sValue, "\n\r");
                        sValue.clear();
                        if (itFrag->begin->eType == SEPARATOR && itFrag->begin->sValue == "[")
                        {
                            sValue += "[\n";
                        }
                        for(hvUInt32 iLine = 0; iLine < vLines.size(); ++iLine)
                        {
                            std::string sLine = vLines[iLine];
                            std::string sResult;
                            if(convertPreProcessCmd(sLine, sResult))
                            {
                                sValue += sResult;
                            }
                            else
                            {
                                trim(vLines[iLine]);
                                if(vLines[iLine].empty() &&
                                   vLines.size() == iLine + 1)
                                    continue;

                                if(!rcsManager_T::getInstance()->isNewPvrs())
                                {
                                    sValue += "rcn::built_in { ";
                                    sValue += vLines[iLine];
                                    if(vLines.size() > 1)
                                        sValue += " }\n";
                                    else
                                        sValue += " }";
                                }
                                else
                                {
                                    sValue += vLines[iLine];
                                    if(vLines.size() > 1)
                                        sValue += " \n";
                                }
                            }
                        }
                        if (itFrag->end->eType == SEPARATOR && itFrag->end->sValue == "]")
                        {
                            sValue += "]";
                        }
                    }
                    else if(iKey->eKeyType == TVF_FUNCTION)
                    {
                        if (itFrag->begin->eType == SEPARATOR && itFrag->begin->sValue == "[")
                        {
                            sValue.erase(0, 1);
                        }
                        if (itFrag->end->eType == SEPARATOR && itFrag->end->sValue == "]")
                        {
                            sValue.erase(sValue.size() - 1);
                        }
                        std::vector<std::string> vLines;
                        tokenize(vLines, sValue, "\n\r");
                        sValue.clear();
                        if (itFrag->begin->eType == SEPARATOR && itFrag->begin->sValue == "[")
                        {
                            sValue += "[\n";
                        }

                        for(hvUInt32 iLine = 0; iLine < vLines.size(); ++iLine)
                        {
                            std::string sLine = vLines[iLine];
                            trim(sLine);
                            if(isTclPackageLine(sLine))
                            {
                                
                                sValue += vLines[iLine];
                                sValue += "\n";
                                continue;
                            }

                            sLine = vLines[iLine];
                            std::string sResult;
                            for(std::string::size_type i = 0; i < sLine.size(); ++i)
                            {
                                if(sLine[i] == '[')
                                {
                                    sResult += sLine[i];
                                    ++i;
                                    while(i < sLine.size() && (sLine[i] == ' ' || sLine[i] == '\t'))
                                    {
                                        sResult += sLine[i];
                                        ++i;
                                    }

                                    if(i < sLine.size() && sLine[i] == '$')
                                    {
                                        while(i < sLine.size() && sLine[i] != ' ' && sLine[i] != '\t')
                                        {
                                            sResult += sLine[i];
                                            ++i;
                                        }

                                        while(i < sLine.size() && (sLine[i] == ' ' || sLine[i] == '\t'))
                                        {
                                            sResult += sLine[i];
                                            ++i;
                                        }

                                        if(i < sLine.size())
                                        {
                                            std::string::size_type j = sLine.find_first_of(" \t", i);
                                            std::string sCode = (j != std::string::npos) ? sLine.substr(i, j - i)
                                                                                         : sLine.substr(i);

                                            if(strcasecmp(sCode.c_str(), "a") == 0)
                                            {
                                                sResult += "u";
                                            }
                                            else if(strcasecmp(sCode.c_str(), "b") == 0)
                                            {
                                                sResult += "v";
                                            }
                                            else if(strcasecmp(sCode.c_str(), "finger_count") == 0)
                                            {
                                                sResult += "group_count";
                                            }
                                            else if(strcasecmp(sCode.c_str(), "slice_count") == 0)
                                            {
                                                sResult += "cut_count";
                                            }
                                            else if(strcasecmp(sCode.c_str(), "row_count") == 0)
                                            {
                                                sResult += "tuple_count";
                                            }
                                            else if(strcasecmp(sCode.c_str(), "measurement_count") == 0)
                                            {
                                                sResult += "tuple_size";
                                            }
                                            else
                                            {
                                                sResult += sCode;
                                            }

                                            i = j != std::string::npos ? j - 1 : sLine.size();
                                        }
                                    }
                                    else
                                    {
                                        if(i < sLine.size())
                                        {
                                            std::string::size_type j = sLine.find_first_of(" \t", i);
                                            std::string sCode = (j != std::string::npos) ? sLine.substr(i, j - i)
                                                                                         : sLine.substr(i);

                                            if(strcasecmp(sCode.c_str(), "device::dfm_vec_measurements") == 0 ||
                                                    strcasecmp(sCode.c_str(), "dfm_vec_measurements") == 0)
                                            {
                                                sResult += "device::convert_to_enc";
                                                i = j != std::string::npos ? j - 1 : sLine.size();
                                            }
                                            else if(strcasecmp(sCode.c_str(), "device::scaled_dfm_vec_measurements") == 0 ||
                                                    strcasecmp(sCode.c_str(), "scaled_dfm_vec_measurements") == 0)
                                            {
                                                sResult += "device::convert_to_scaled_enc";
                                                i = j != std::string::npos ? j - 1 : sLine.size();
                                            }
                                            else
                                            {
                                                sResult += sLine[i];
                                            }
                                        }
                                        continue;
                                    }
                                }
                                else
                                {
                                    sResult += sLine[i];
                                }
                            }
                            sValue += sResult;
                            sValue += "\n";
                        }
                        if (itFrag->end->eType == SEPARATOR && itFrag->end->sValue == "]")
                        {
                            sValue += "]";
                        }
                    }

                    token.sValue = sValue;
                    iter = m_listTokenStream.erase(begin, ++iter);
                    iter = m_listTokenStream.insert(iter, token);

                    itFrag++;
                    break;
                }

                if(iter->eType == STRING_CONSTANTS)
                {
                    token.sValue += "\"";
                    token.sValue += iter->sValue;
                    token.sValue += "\"";
                }
                else if(!rcsManager_T::getInstance()->isNewPvrs() && iter->eType == SEPARATOR &&
                        (iter->sValue == "{" || iter->sValue == "}"))
                {
                    token.sValue += "\\";
                    token.sValue += iter->sValue;
                }
                else if(needTransBuiltIn && (iter->eType == SECONDARY_KEYWORD || iter->eType == IDENTIFIER_NAME))
                {
                    std::list<rcsToken_T>::iterator iToken = iter;
                    skipBlankSpaceToken(iToken);
                    if(iToken->eType != SEPARATOR || iToken->sValue != "(")
                    {
                        if(iter->eType == IDENTIFIER_NAME &&
                          (strncasecmp(iter->sValue.c_str(), "tvf_number_function", 19) == 0 ||
                           strncasecmp(iter->sValue.c_str(), "tvf_number_func", 15) == 0 ||
                           strncasecmp(iter->sValue.c_str(), "tvf_number_fun", 14) == 0 ||
                           strncasecmp(iter->sValue.c_str(), "tvf_num_function", 16) == 0 ||
                           strncasecmp(iter->sValue.c_str(), "tvf_num_func", 12) == 0 ||
                           strncasecmp(iter->sValue.c_str(), "tvf_num_fun", 11) == 0 ||
                           strncasecmp(iter->sValue.c_str(), "tvf_numeric_function", 20) == 0))
                        {
                            
                            token.sValue += "TRS_NUMBER_FUNCTION";
                            if(strncasecmp(iter->sValue.c_str(), "tvf_number_function", 19) == 0)
                            {
                                if(iter->sValue.size() > 19)
                                    token.sValue += iter->sValue.substr(19);
                            }
                            else if(strncasecmp(iter->sValue.c_str(), "tvf_number_func", 15) == 0)
                            {
                                if(iter->sValue.size() > 15)
                                    token.sValue += iter->sValue.substr(15);
                            }
                            else if(strncasecmp(iter->sValue.c_str(), "tvf_number_fun", 14) == 0)
                            {
                                if(iter->sValue.size() > 14)
                                    token.sValue += iter->sValue.substr(14);
                            }
                            else if(strncasecmp(iter->sValue.c_str(), "tvf_num_function", 16) == 0)
                            {
                                if(iter->sValue.size() > 16)
                                    token.sValue += iter->sValue.substr(16);
                            }
                            else if(strncasecmp(iter->sValue.c_str(), "tvf_num_func", 12) == 0)
                            {
                                if(iter->sValue.size() > 12)
                                    token.sValue += iter->sValue.substr(12);
                            }
                            else if(strncasecmp(iter->sValue.c_str(), "tvf_num_fun", 11) == 0)
                            {
                                if(iter->sValue.size() > 11)
                                    token.sValue += iter->sValue.substr(11);
                            }
                            else if(iter->sValue.size() > 20)
                            {
                                token.sValue += iter->sValue.substr(20);
                            }
                        }
                        else if(iter->eType == IDENTIFIER_NAME && (strncasecmp(iter->sValue.c_str(), "tvf_str_fun", 11) == 0 ||
                                strncasecmp(iter->sValue.c_str(), "tvf_string_function", 19) == 0))
                        {
                            
                            token.sValue += "TRS_STRING_FUNCTION";
                            if(strncasecmp(iter->sValue.c_str(), "tvf_str_fun", 11) == 0 && iter->sValue.size() > 11)
                                token.sValue += iter->sValue.substr(11);
                            else if(iter->sValue.size() > 19)
                                token.sValue += iter->sValue.substr(19);
                        }
                        else if(iter->eType == IDENTIFIER_NAME && iter->sValue.find(';') != std::string::npos)
                        {
                            token.sValue += "\"";
                            token.sValue += iter->sValue;
                            token.sValue += "\"";
                        }
                        else
                        {
                            token.sValue += iter->sValue;
                        }
                    }
                    else
                    {
                        if(strcasecmp(iter->sValue.c_str(), "area_common") == 0 ||
                           strcasecmp(iter->sValue.c_str(), "area_com") == 0)
                        {
                            token.sValue += "area_overlap";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "perimeter") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "perim") == 0)
                        {
                            token.sValue += "perimeter";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "PERIMETERX") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "PERIMX") == 0)
                        {
                            token.sValue += "PERIMETER_X";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "PERIMETERXP") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "PERIMXP") == 0)
                        {
                            token.sValue += "PERIMETER_XS";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "PERIMETERY") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "PERIMY") == 0)
                        {
                            token.sValue += "PERIMETER_Y";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "PERIMETERYP") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "PERIMYP") == 0)
                        {
                            token.sValue += "PERIMETER_YS";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "LENGTHX") == 0)
                        {
                            token.sValue += "LENGTH_X";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "LENGTHXP") == 0)
                        {
                            token.sValue += "LENGTH_XS";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "LENGTHY") == 0)
                        {
                            token.sValue += "LENGTH_Y";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "LENGTHYP") == 0)
                        {
                            token.sValue += "LENGTH_YS";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "EC") == 0)
                        {
                            token.sValue += "EP";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "ECMAX") == 0)
                        {
                            token.sValue += "EP_MAX";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "ECMIN") == 0)
                        {
                            token.sValue += "EP_MIN";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "ECX") == 0)
                        {
                            token.sValue += "EP_X";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "ECXP") == 0)
                        {
                            token.sValue += "EP_XS";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "ECY") == 0)
                        {
                            token.sValue += "EP_Y";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "ECYP") == 0)
                        {
                            token.sValue += "EP_YS";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "EW") == 0)
                        {
                            token.sValue += "ED";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "EWP") == 0)
                        {
                            token.sValue += "ED_P";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "EWX") == 0)
                        {
                            token.sValue += "ED_X";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "EWXP") == 0)
                        {
                            token.sValue += "ED_XS";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "EWY") == 0)
                        {
                            token.sValue += "ED_Y";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "EWYP") == 0)
                        {
                            token.sValue += "ED_YS";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "SPROPERTY") == 0)
                        {
                            token.sValue += "S_PROPERTY";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "PSPROPERTY") == 0)
                        {
                            token.sValue += "PS_PROPERTY";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "VPROPERTY") == 0)
                        {
                            token.sValue += "V_PROPERTY";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "SVPROPERTY") == 0)
                        {
                            token.sValue += "SV_PROPERTY";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "NETPROPERTY") == 0)
                        {
                            token.sValue += "N_PROPERTY";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "NETVPROPERTY") == 0)
                        {
                            token.sValue += "NV_PROPERTY";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "NIPROPERTY") == 0)
                        {
                            token.sValue += "NI_PROPERTY";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "NETPROPERTY_REF") == 0)
                        {
                            token.sValue += "N_PROPERTY_REF";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "NETVPROPERTY_REF") == 0)
                        {
                            token.sValue += "NV_PROPERTY_REF";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "SPROPERTY_REF") == 0)
                        {
                            token.sValue += "S_PROPERTY_REF";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "PSPROPERTY_REF") == 0)
                        {
                            token.sValue += "PS_PROPERTY_REF";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "VPROPERTY_REF") == 0)
                        {
                            token.sValue += "V_PROPERTY_REF";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "SVPROPERTY_REF") == 0)
                        {
                            token.sValue += "SV_PROPERTY_REF";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "SVPROPERTY_REF") == 0)
                        {
                            token.sValue += "SV_PROPERTY_REF";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "GLOBALNETID") == 0)
                        {
                            token.sValue += "GLOBAL_NETID";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "VNETID") == 0)
                        {
                            token.sValue += "V_NETID";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "RANGE_XXY") == 0)
                        {
                            token.sValue += "SELECT_XXY";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "SELECT_MERGE_XXY") == 0)
                        {
                            token.sValue += "SORT_XXY_N";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "SORT_MERGE_XXY") == 0)
                        {
                            token.sValue += "SORT_XXY";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "VBYNETMIN") == 0)
                        {
                            token.sValue += "V_NET_MIN";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "VBYNETMAX") == 0)
                        {
                            token.sValue += "V_NET_MAX";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "VMIN") == 0)
                        {
                            token.sValue += "V_MIN";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "VMAX") == 0)
                        {
                            token.sValue += "V_MAX";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "VNETIDMIN") == 0)
                        {
                            token.sValue += "V_NETID_MIN";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "VNETIDMAX") == 0)
                        {
                            token.sValue += "V_NETID_MAX";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "VNETSUM") == 0)
                        {
                            token.sValue += "V_NET_SUM";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "VSTRING") == 0)
                        {
                            token.sValue += "V_STRING";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "bends") == 0)
                        {
                            token.sValue += "count_bend";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "enclosure_parallel") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "enc_par") == 0)
                        {
                            token.sValue += "enc_parallel";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "enclosure_parallel_multifinger") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "enc_par_mul") == 0)
                        {
                            token.sValue += "enc_parallel_fingers";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "enclosure_perpendicular") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "enc_per") == 0)
                        {
                            token.sValue += "enc_perpendicular";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "enclosure_perpendicular_multifinger") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "enc_per_mul") == 0)
                        {
                            token.sValue += "enc_perpendicular_fingers";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "enclosure_vector") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "enc_vec") == 0)
                        {
                            token.sValue += "enc_tuple";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "dfm_numeric_value") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "dfm_num_val") == 0)
                        {
                            token.sValue += "dfm_value";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "dfm_numeric_array") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "dfm_num_arr") == 0)
                        {
                            token.sValue += "dfm_tuple";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "dfm_vector_value") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "dfm_vec_val") == 0)
                        {
                            token.sValue += "dfm_multi_value";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "dfm_vector_array") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "dfm_vec_arr") == 0)
                        {
                            token.sValue += "dfm_multi_tuple";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "perimeter_inside") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "perim_in") == 0)
                        {
                            token.sValue += "inside_length";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "perimeter_outside") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "perim_out") == 0)
                        {
                            token.sValue += "outside_length";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "perimeter_coincide") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "perim_co") == 0)
                        {
                            token.sValue += "common_length";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "perimeter_coincide_inside") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "perim_co_in") == 0)
                        {
                            token.sValue += "inside_common_length";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "perimeter_coincide_outside") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "perim_co_out") == 0)
                        {
                            token.sValue += "outside_common_length";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "instance") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "inst") == 0)
                        {
                            token.sValue += "device_id";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "named_net") == 0)
                        {
                            token.sValue += "name_to_net_id";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "pin_net") == 0)
                        {
                            token.sValue += "net_id_of_pin";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "unit_capacitance") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "unit_cap") == 0)
                        {
                            token.sValue += "cap_unit";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "unit_length") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "unit_len") == 0)
                        {
                            token.sValue += "length_unit";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "unit_resistance") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "unit_res") == 0)
                        {
                            token.sValue += "res_unit";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "x_location") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "x_loc") == 0)
                        {
                            token.sValue += "x_coord";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "y_location") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "y_loc") == 0)
                        {
                            token.sValue += "y_coord";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "text_string") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "text_str") == 0)
                        {
                            token.sValue += "label_name";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "text_numeric") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "text_num") == 0)
                        {
                            token.sValue += "label_value";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "layout_numeric_value") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "lay_num_val") == 0)
                        {
                            token.sValue += "layout_value";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "source_numeric_value") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "src_num_val") == 0)
                        {
                            token.sValue += "schematic_value";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "layout_string_value") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "lay_str_val") == 0)
                        {
                            token.sValue += "layout_string_value";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "source_string_value") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "src_str_val") == 0)
                        {
                            token.sValue += "schematic_string_value";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "string_compare") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "str_comp") == 0)
                        {
                            token.sValue += "compare_string";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "report_numeric_discrepancy") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "rep_num_disc") == 0)
                        {
                            token.sValue += "report_error_percentage";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "report_numeric_value") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "rep_num_val") == 0)
                        {
                            token.sValue += "report_error_value";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "report_string_discrepancy") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "rep_str_disc") == 0)
                        {
                            token.sValue += "report_error_string";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "report_message") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "rep_msg") == 0)
                        {
                            token.sValue += "report_error_message";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "input_numeric_value") == 0 ||
                                strcasecmp(iter->sValue.c_str(), "inp_num_val") == 0)
                        {
                            token.sValue += "get_value";
                        }
                        else if(strcasecmp(iter->sValue.c_str(), "is_missing") == 0)
                        {
                            token.sValue += "is_value_not_exist";
                        }
                        else if( (strncasecmp(iter->sValue.c_str(), "tvf_num_function", 16) == 0) ||
                                 (strncasecmp(iter->sValue.c_str(), "tvf_num_func", 12) == 0) ||
                                 (strncasecmp(iter->sValue.c_str(), "tvf_num_fun", 11) == 0) )
                        {

                            token.sValue += "TRS_NUMBER_FUNCTION";
                            if(strncasecmp(iter->sValue.c_str(), "tvf_num_function", 16) == 0)
                            {
                                if(iter->sValue.size() > 16)
                                    token.sValue += iter->sValue.substr(16);
                            }
                            else if(strncasecmp(iter->sValue.c_str(), "tvf_num_func", 12) == 0)
                            {
                                if(iter->sValue.size() > 12)
                                    token.sValue += iter->sValue.substr(12);
                            }
                            else if(iter->sValue.size() > 11)
                            {
                                token.sValue += iter->sValue.substr(11);
                            }
                        }
                        else if( (strncasecmp(iter->sValue.c_str(), "tvf_number_function", 19) == 0) ||
                                 (strncasecmp(iter->sValue.c_str(), "tvf_number_func", 15) == 0) ||
                                 (strncasecmp(iter->sValue.c_str(), "tvf_number_fun", 14) == 0) )
                        {

                            token.sValue += "TRS_NUMBER_FUNCTION";
                            if(strncasecmp(iter->sValue.c_str(), "tvf_number_function", 19) == 0)
                            {
                                if(iter->sValue.size() > 19)
                                    token.sValue += iter->sValue.substr(19);
                            }
                            else if(strncasecmp(iter->sValue.c_str(), "tvf_number_func", 15) == 0)
                            {
                                if(iter->sValue.size() > 15)
                                    token.sValue += iter->sValue.substr(15);
                            }
                            else if(iter->sValue.size() > 14)
                            {
                                token.sValue += iter->sValue.substr(14);
                            }
                        }
                        else if(strncasecmp(iter->sValue.c_str(), "tvf_numeric_function", 20) == 0)
                        {

                            token.sValue += "TRS_NUMBER_FUNCTION";
                            if(iter->sValue.size() > 20)
                                token.sValue += iter->sValue.substr(20);
                        }
                        else if(strncasecmp(iter->sValue.c_str(), "tvf_numeric_fun", 15) == 0)
                        {
                            
                            token.sValue += "TRS_NUMBER_FUNCTION";
                            if(iter->sValue.size() > 15)
                                token.sValue += iter->sValue.substr(15);
                        }
                        else if(strncasecmp(iter->sValue.c_str(), "tvf_str_function", 16) == 0)
                        {
                            
                            token.sValue += "TRS_STRING_FUNCTION";
                            if(iter->sValue.size() > 16)
                                token.sValue += iter->sValue.substr(16);
                        }
                        else if(strncasecmp(iter->sValue.c_str(), "tvf_str_fun", 11) == 0 ||
                                strncasecmp(iter->sValue.c_str(), "tvf_string_function", 19) == 0)
                        {
                            
                            token.sValue += "TRS_STRING_FUNCTION";
                            if(strncasecmp(iter->sValue.c_str(), "tvf_str_fun", 11) == 0 && iter->sValue.size() > 11)
                                token.sValue += iter->sValue.substr(11);
                            else if(iter->sValue.size() > 19)
                                token.sValue += iter->sValue.substr(19);
                        }
                        else if(strncasecmp(iter->sValue.c_str(), "tvf_string_fun", 14) == 0)
                        {
                            
                            token.sValue += "TRS_STRING_FUNCTION";
                            if(iter->sValue.size() > 14)
                                token.sValue += iter->sValue.substr(14);
                        }
                        else if(strncasecmp(iter->sValue.c_str(), "EFFECTIVE", 11) == 0)
                        {
                            token.sValue += "PROPERTY";
                        }
                        else if(strncasecmp(iter->sValue.c_str(), "EFFECTIVE STRING", 11) == 0)
                        {
                            token.sValue += "STRING PROPERTY";
                        }
                        else
                        {
                            token.sValue += iter->sValue;
                        }
                    }
                }
                else if(iKey->eKeyType == TVF_FUNCTION && (iter->eType == SECONDARY_KEYWORD || iter->eType == IDENTIFIER_NAME))
                {
                    if(strcasecmp(iter->sValue.c_str(), "device::dfm_vec_measurements") == 0)
                    {
                        token.sValue += "device::convert_to_enc";
                    }
                    else if(strcasecmp(iter->sValue.c_str(), "device::scaled_dfm_vec_measurements") == 0)
                    {
                        token.sValue += "device::convert_to_scaled_enc";
                    }
                    else if(strcasecmp(iter->sValue.c_str(), "device::eval_dfm_func") == 0)
                    {
                        token.sValue += "device::dfm_lookup_table";
                    }
                    else
                    {
                        token.sValue += iter->sValue;
                    }
                }
                else if(iter->eType == IDENTIFIER_NAME && iter->sValue.find(';') != std::string::npos)
                {
                    token.sValue += "\"";
                    token.sValue += iter->sValue;
                    token.sValue += "\"";
                }
                else
                {
                    token.sValue += iter->sValue;
                }
            }

            if(iter == m_listTokenStream.end())
            {
                Utassert(false);
                break;
            }
        }

        if(!rcsManager_T::getInstance()->outputComment())
            Utassert(iter->eType != COMMENT);

        ++iter;
    }
}

void
rcsLexMacroParser_T::removeUnusedToken()
{
    if(!rcsManager_T::getInstance()->outputComment())
    {
        std::list<rcsToken_T>::iterator iter = m_listTokenStream.begin();
        while(iter != m_listTokenStream.end())
        {
            if(iter->eType == COMMENT || iter->eType == BLANK_SPACE)
            {
                iter = m_listTokenStream.erase(iter);
                continue;
            }
            ++iter;
        }
    }
    else
    {
        std::list<rcsToken_T>::iterator iter = m_listTokenStream.begin();
        while(iter != m_listTokenStream.end())
        {
            if(iter->eType == BLANK_SPACE)
            {
                iter = m_listTokenStream.erase(iter);
                continue;
            }
            ++iter;
        }
    }
}

void
rcsLexMacroParser_T::parseBlankLines()
{
    std::list<rcsToken_T>::iterator iter = m_listTokenStream.begin();
    hvUInt32 nPrevLineNo = -1u - 1;  
    const bool isOutputCommment = rcsManager_T::getInstance()->outputComment();
    while (iter != m_listTokenStream.end())
    {
        if (iter->eType != BLANK_SPACE && iter->nLineNo != nPrevLineNo)
        {
            if (iter->eType == COMMENT && !isOutputCommment) 
            {
                iter++;
                continue;
            }
            bool hasComment = false;
            if (iter->eType != COMMENT)
            {
                std::set<hvUInt32>::iterator iterLow =
                        m_vUnstoredCommentLineNumbers.lower_bound(iter->nLineNo);
                if (iterLow != m_vUnstoredCommentLineNumbers.begin())
                {
                    --iterLow;
                    hasComment = *iterLow > nPrevLineNo;
                }
            }
            if (hasComment)
            {
                m_blankLinesBefore[iter->nLineNo] = std::make_pair(1, true);
            }
            else if (iter->nLineNo > nPrevLineNo + 1)
            {
                m_blankLinesBefore[iter->nLineNo] = std::make_pair(
                        iter->nLineNo - nPrevLineNo - 1, true);
            }
            if (iter->eType == COMMENT)
            {
                nPrevLineNo = iter->nEndLineNo;
            }
            else
            {
                nPrevLineNo = iter->nLineNo;
            }
        }
        iter++;
    }
}

void
rcsLexMacroParser_T::parse()
{
    parseBlankLines();

    std::list<rcsToken_T>::iterator iter = m_listTokenStream.begin();
    while(iter != m_listTokenStream.end())
    {
        rcsToken_T &token = *iter;
        if(token.eType == DMACRO)
        {
            extractMacro(iter);
            continue;
        }
        ++iter;
    }
    Utassert(iter == m_listTokenStream.end());

    iter = m_listTokenStream.begin();
    bool hasCmacro = true;

    while(hasCmacro)
    {
        iter = m_listTokenStream.begin();
        while(iter != m_listTokenStream.end())
        {
            if(iter->eType == CMACRO)
            {
                ++iter;
                std::string sMacroName = iter->sValue;
                toLower(sMacroName);
                if(m_mapMacros.find(sMacroName) != m_mapMacros.end())
                {
                    --iter;
                    parseCMacro(iter);
                }
                continue;
            }
            ++iter;
        }

        hasCmacro = false;
        iter = m_listTokenStream.begin();
        while(iter != m_listTokenStream.end())
        {
            if(iter->eType == CMACRO)
            {
                ++iter;
                std::string sMacroName = iter->sValue;
                toLower(sMacroName);
                if(m_mapMacros.find(sMacroName) != m_mapMacros.end())
                    hasCmacro = true;
                break;
            }
            ++iter;
        }
    }

    bool needBreakIntoFragments = false;
    if (rcsManager_T::getInstance()->isConvertSwitch())
    {
        std::vector<hvInt32> vUnMatchedCounts; 
        vUnMatchedCounts.push_back(0);
        for (TOKEN_ITER iter = m_listTokenStream.begin();
                iter != m_listTokenStream.end() && !needBreakIntoFragments; iter++)
        {
            if (iter->eType == PRO_KEYWORD)
            {
                switch (iter->eKeyType)
                {
                    case PRO_IFDEF:
                    case PRO_IFNDEF:
                        vUnMatchedCounts.push_back(0);
                        break;
                    case PRO_ELSE:
                    case PRO_ENDIF:
                        if (!vUnMatchedCounts.empty() && vUnMatchedCounts.back() != 0)
                        {
                            needBreakIntoFragments = true;
                        }
                        if (iter->eKeyType == PRO_ENDIF)
                        {
                            vUnMatchedCounts.pop_back();
                        }
                        break;
                    default:
                        break;
                }
            }
            else if (iter->eType == SEPARATOR)
            {
                if (iter->sValue == "[")
                {
                    vUnMatchedCounts.back() += 1;
                }
                else if (iter->sValue == "]")
                {
                    if (vUnMatchedCounts.back() == 0)
                    {
                        needBreakIntoFragments = true;
                    }
                    else
                    {
                        vUnMatchedCounts.back() -= 1;
                    }
                }
            }
        }
    }
    if (needBreakIntoFragments)
    {
        parseBuildInLang<true>();
    }
    else
    {
        parseBuildInLang<false>();
    }

    removeUnusedToken();
}

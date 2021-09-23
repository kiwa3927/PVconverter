



#include "public/manager/rcErrorManager.h"
#include "public/util/rcsStringUtil.hpp"
#include "public/synnode/rcsSynRulecheckNode.h"
#include "public/synnode/rcsSynSpecificationNode.h"
#include "public/synnode/rcsSynLayerOperationNode.h"
#include "public/synnode/rcsSynLayerDefinitionNode.h"
#include "public/synnode/rcsSynProcessNode.h"
#include "public/synnode/rcsSynProcNode.h"

#include "public/manager/rcsManager.h"
#include "rcsNumberParser.h"
#include "rcsSynNodeConvertor.h"

const std::string rcsSynNodeConvertor_T::m_sTmpLayerPrefix = "pvrs_tmp_layer_";
const hvUInt32 MAX_OPTIONS_PER_LINE = 3;

rcsSynNodeConvertor_T::rcsSynNodeConvertor_T()
: m_oPVRSStream(rcsManager_T::getInstance()->getPVRSOutStream()),
  m_oLogStream(rcsManager_T::getInstance()->getPVRSSumStream()),
  m_sPVRSCmdPrefix(rcsManager_T::getInstance()->getPVRSCmdPrefix()),
  m_isInLayerDefinition(false), m_hasPrecisionDef(false), m_iProcLevel(0)
{
}

rcsSynNodeConvertor_T::~rcsSynNodeConvertor_T()
{
    m_vVariableNames.clear();
}

void
rcsSynNodeConvertor_T::outputCon1Error(rcsSynNode_T *pNode)
{
    std::string sValue;
    pNode->getCmdValue(sValue);
    m_oLogStream << "This Operation or Specification in line " << pNode->begin()->nLineNo
                 << " can not be Converted correctly: " << std::endl << sValue << std::endl << std::endl;

    s_errManager.addError(rcErrorNode_T(rcErrorNode_T::WARNING, CON1,
                          pNode->getNodeBeginLineNo(), pNode->begin()->sValue));
}

void
rcsSynNodeConvertor_T::outputCon2Error(rcsSynNode_T *pNode, const rcsToken_T &token)
{
    if(token.eType == SECONDARY_KEYWORD && (token.eKeyType == MASK || token.eKeyType == DIRECT))
        return;

    std::string sValue;
    pNode->getCmdValue(sValue);
    m_oLogStream << "This Option " << token.sValue << " in line " << token.nLineNo
                 << " can not be Converted correctly in this operation: " << std::endl
                 << sValue << std::endl << std::endl;

    s_errManager.addError(rcErrorNode_T(rcErrorNode_T::WARNING, CON2,
                                        token.nLineNo, token.sValue));
}

void
rcsSynNodeConvertor_T::outputText(const char *pOption, const char *pValue,
                                  const hvUInt32 nOption)
{
    if(pOption != NULL && pValue != NULL && nOption != 0 && nOption % MAX_OPTIONS_PER_LINE == 0)
        m_oPVRSStream << "\\\n";
    else if(pValue != NULL && nOption != 0 && nOption % (MAX_OPTIONS_PER_LINE * 2) == 0)
        m_oPVRSStream << "\\\n";
    else if(pOption != NULL && nOption != 0 && nOption % (MAX_OPTIONS_PER_LINE * 2) == 0)
        m_oPVRSStream << "\\\n";

    if(pOption != NULL)
    {
        m_oPVRSStream << pOption << " ";
    }

    if(pValue != NULL)
    {
        m_oPVRSStream << pValue << " ";
    }
}

static inline bool
isConstraintOperator(const rcsToken_T &token)
{
    if(token.eType != OPERATOR)
        return false;
    const std::string &sOperator = token.sValue;
    return ("==" == sOperator) || (">" == sOperator) || ("<" == sOperator) ||
           ("!=" == sOperator) || (">=" == sOperator) || ("<=" == sOperator);
}

static inline bool
hasModOperator(std::list<rcsToken_T>::iterator iter, std::list<rcsToken_T>::iterator end)
{
    while(iter != end)
    {
        if(iter->eType == OPERATOR && iter->sValue == "%")
            return true;
        ++iter;
    }
    return false;
}

bool
rcsSynNodeConvertor_T::getNumberExp(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end,
                                    std::string &sNumExp, bool withParentheses, bool isAllowExpression)
{
    bool hasMod = hasModOperator(iter, end);

    if(isAllowExpression)
    {
        if(!rcsExpressionParser_T::parse(iter, end, sNumExp))
            return false;
    }
    else if(!rcsExpressionParser_T::parseNumber(iter, end, sNumExp))
    {
        return false;
    }

#if 0
    if(sNumExp[0] == '(')
        withParentheses = true;
#endif

    if(!hasMod)
    {
        sNumExp.insert(0, "[expr ( ");
        sNumExp.insert(sNumExp.size(), " )]");
    }

    if(withParentheses && isAllowExpression)
    {
        sNumExp.insert(0, "(");
        sNumExp.insert(sNumExp.size(), ")");
    }

    return true;
}

bool
rcsSynNodeConvertor_T::parseFollowNums(std::list<rcsToken_T>::iterator &iter,
                                       std::list<rcsToken_T>::iterator end,
                                       std::vector<std::string> &vNumExps,
                                       bool (*isValidKey)(const rcsToken_T &),
                                       const hvUInt32 nMinCount,
                                       const hvUInt32 nMaxCount)
{
    hvUInt32 nCount = 0;
    std::list<rcsToken_T>::iterator begin = iter++;
    while(nCount < nMaxCount)
    {
        if(iter != end && !isValidKey(*iter) && !isConstraintOperator(*iter))
        {
            std::string sNumExp;
            if(!getNumberExp(iter, end, sNumExp))
                return false;

            vNumExps.insert(vNumExps.end(), sNumExp);
            ++nCount;
        }
        else if(nCount >= nMinCount)
        {
            return true;
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP11,
                                begin->nLineNo, begin->sValue);
            return false;
        }
    }
    return true;
}

bool
rcsSynNodeConvertor_T::getConstraint(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end,
                                     std::string &sConstraint)
{
    if(isConstraintOperator(*iter))
    {
        sConstraint += iter->sValue;
        sConstraint += " ";

        ++iter;
        if(iter == end)
            return false;

        std::string sNumExp;
        if(!getNumberExp(iter, end, sNumExp))
            return false;
        sConstraint += sNumExp;
    }

    if(iter != end && isConstraintOperator(*iter))
    {
        sConstraint += " ";
        sConstraint += iter->sValue;
        sConstraint += " ";

        ++iter;
        if(iter == end)
            return false;
        std::string sNumExp;
        if(!getNumberExp(iter, end, sNumExp))
            return false;
        sConstraint += sNumExp;
    }

    return true;
}

static std::string
parseTokenString(const rcsToken_T &token)
{
    const std::string &src = token.sValue;
    std::string res;
    std::string::size_type pos = 0;
    if(token.eType == STRING_CONSTANTS)
    {
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
    }
    else
    {
        while(pos < src.length())
        {
            switch(src[pos])
            {
                case '\\':
                    res += "\\\\";
                    break;
                default:
                    res += src[pos];
                    break;
            }
            ++pos;
        }
    }
    return res;
}

bool
rcsSynNodeConvertor_T::getStringName(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end,
                                     std::string &sValue, bool transString)
{
    if(iter == end)
        return false;

    switch(iter->eType)
    {
        case SECONDARY_KEYWORD:
        case IDENTIFIER_NAME:
        {
            if(!isValidSvrfName(iter->sValue))
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
        }
        case STRING_CONSTANTS:
        {
            sValue = parseTokenString(*iter);
            break;
        }
        default:
            return false;
    }

    if(!iter->namescopes.empty())
    {
        sValue.insert(1, "{");
        sValue.insert(sValue.size(), "}");
        return true;
    }

    if(iter->eType != STRING_CONSTANTS)
    {
        bool hasSpecificSymbol = false;
        hvUInt32 iValue = 0;
        while(iValue < sValue.size())
        {
            if(sValue[iValue] == '$' || sValue[iValue] == ';' || sValue[iValue] == ' ' || sValue[iValue] == '\\' || sValue[iValue] == '.')
            {
                hasSpecificSymbol = true;
                break;
            }
            ++iValue;
        }

        if(hasSpecificSymbol)
        {
            sValue.insert(0, "{");
            sValue.insert(sValue.size(), "}");
            return true;
        }
    }

    if(iter->eType == STRING_CONSTANTS)
    {
        if(transString)
        {
            hvUInt32 iValue = 0;
            while(iValue < sValue.size())
            {
                if(sValue[iValue] == '{' || sValue[iValue] == '}')
                {
                    sValue.insert(iValue, "\\");
                    ++iValue;
                }
                ++iValue;
            }

            sValue.insert(0, "{\"");
            sValue.insert(sValue.size(), "\"}");
        }
        else
        {
            sValue.insert(0, "\"");
            sValue.insert(sValue.size(), "\"");
        }
    }

    return true;
}

bool
rcsSynNodeConvertor_T::getLayerRefName(std::list<rcsToken_T>::iterator &iter,
                                       std::list<rcsToken_T>::iterator end,
                                       std::string &sLayerName)
{
    switch(iter->eType)
    {
        case SECONDARY_KEYWORD:
        case IDENTIFIER_NAME:
        case STRING_CONSTANTS:
        {
            getStringName(iter, end, sLayerName);
            if(m_iProcLevel > 0 && strncmp(sLayerName.c_str(), m_sTmpLayerPrefix.c_str(), strlen(m_sTmpLayerPrefix.c_str())) == 0)
            {
                sLayerName.insert(0, "$");
            }
            ++iter;
            break;
        }
        case OPERATOR:
        {
            if(iter->sValue != "+")
            {
                return false;
            }
            else
            {
                hvUInt32 nLineNo = iter->nLineNo;
                if(++iter == end || iter->eType != NUMBER || iter->nLineNo != nLineNo)
                {
                    --iter;
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
            }
        }
        case NUMBER:
        {
            sLayerName = iter->sValue;
            ++iter;
            break;
        }
        default:
            return false;
    }
    return true;
}

static bool
isRuleMapKeyword(const rcsToken_T &token)
{
    if(token.eType == SECONDARY_KEYWORD)
    {
        switch(token.eKeyType)
        {
            case APPEND:
            case PREFIX:
            case TEXTTAG:
            case MAXIMUM:
            case GDSII:
            case OASIS:
            case BINARY:
            case ASCII:
            case USER:
            case PSEUDO:
            case AREF:
            case AUTOREF:
            case SUBSTITUTE:
                return true;
            default:
                return false;
        }
    }
    return false;
}

void
rcsSynNodeConvertor_T::convertRuleMapSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "drc_rule_map ";

    std::list<rcsToken_T>::iterator iter  = pNode->begin();
    std::list<rcsToken_T>::iterator begin = pNode->begin();
    ++iter;
    switch(iter->eType)
    {
        case SECONDARY_KEYWORD:
        case IDENTIFIER_NAME:
        {
            if(!isValidSvrfName(iter->sValue))
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, DCM1,
                                    begin->nLineNo);
            }
        }
        case STRING_CONSTANTS:
        {
            std::string sRuleName;
            if(getStringName(iter, pNode->end(), sRuleName))
                outputText("-rule_name", sRuleName.c_str(), nOption++);
            ++iter;
            break;
        }
        default:
            throw rcErrorNode_T(rcErrorNode_T::ERROR, DCM1,
                                begin->nLineNo);
    }

     if(iter->eType == SECONDARY_KEYWORD)
     {
         switch(iter->eKeyType)
         {
             case GDSII:
             case OASIS:
             {
                 if(iter->eKeyType == GDSII)
                     outputText("-format", "GDS", nOption++);
                 else
                     outputText("-format", "OASIS", nOption++);

                 ++iter;
                 if(iter == pNode->end() || (iter->eType != NUMBER && iter->eType != OPERATOR))
                     break;

                 std::string sLayerNum;
                 if(getNumberExp(iter, pNode->end(), sLayerNum, false, false))
                     outputText("-out_layer", sLayerNum.c_str(), nOption++);
                 else
                     break;

                 if(iter == pNode->end() || (iter->eType != NUMBER && iter->eType != OPERATOR))
                     break;

                 std::string sDataType;
                 if(getNumberExp(iter, pNode->end(), sDataType, false, false))
                     outputText(NULL, sDataType.c_str(), nOption++);
                 break;
             }
             case ASCII:
             {
                 outputText("-format", "ascii", nOption++);
                 ++iter;
                 break;
             }
             case BINARY:
             {
                 outputText("-format", "binary", nOption++);
                 ++iter;
                 break;
             }
             default:
                 break;
         }
     }
     else
     {
         if(iter != pNode->end() && (iter->eType == NUMBER || iter->eType == OPERATOR))
         {
             std::string sLayerNum;
             if(getNumberExp(iter, pNode->end(), sLayerNum, false, false))
                 outputText("-out_layer", sLayerNum.c_str(), nOption++);

              if(iter != pNode->end() && (iter->eType == NUMBER || iter->eType == OPERATOR))
              {
                  std::string sDataType;
                  if(getNumberExp(iter, pNode->end(), sDataType, false, false))
                      outputText(NULL, sDataType.c_str(), nOption++);
              }
         }
     }

     while(iter != pNode->end())
     {
         if(iter->eType == SECONDARY_KEYWORD)
         {
             switch(iter->eKeyType)
             {
                 case APPEND:
                 {
                     std::string sAppend;
                     if(!getStringName(++iter, pNode->end(), sAppend))
                     {
                         --iter;
                         throw rcErrorNode_T(rcErrorNode_T::ERROR, INP4,
                                             iter->nLineNo, iter->sValue);
                     }

                     outputText("-append", sAppend.c_str(), nOption++);
                     ++iter;
                     continue;
                 }
                 case PREFIX:
                 {
                     std::string sPrefix;
                     if(!getStringName(++iter, pNode->end(), sPrefix))
                     {
                         --iter;
                         throw rcErrorNode_T(rcErrorNode_T::ERROR, INP4,
                                             iter->nLineNo, iter->sValue);
                     }

                     outputText("-prefix", sPrefix.c_str(), nOption++);
                     ++iter;
                     continue;
                 }
                 case TEXTTAG:
                 {
                     std::string sTextTag;
                     if(!getStringName(++iter, pNode->end(), sTextTag))
                     {
                         --iter;
                         throw rcErrorNode_T(rcErrorNode_T::ERROR, INP4,
                                             iter->nLineNo, iter->sValue);
                     }

                     outputText("-texttag", sTextTag.c_str(), nOption++);
                     ++iter;
                     continue;
                 }
                 case MAXIMUM:
                 {
                     if(++iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                        (iter->eKeyType == VERTICES || iter->eKeyType == RESULTS))
                     {
                         std::string sOption;
                         if(iter->eKeyType == VERTICES)
                             sOption = "-max_vertices";
                         else
                             sOption = "-max_results";

                         if(++iter != pNode->end())
                         {
                             if(SECONDARY_KEYWORD == iter->eType && iter->eKeyType == ALL)
                             {
                                 outputText(sOption.c_str(), "all", nOption++);
                                 ++iter;
                             }
                             else
                             {
                                 std::string sMax;
                                 if(getNumberExp(iter, pNode->end(), sMax))
                                     outputText(sOption.c_str(), sMax.c_str(), nOption++);
                             }
                         }
                         else
                         {
                             throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                 begin->nLineNo, begin->sValue);
                         }
                     }
                     else
                     {
                         --iter;
                         throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                             iter->nLineNo, iter->sValue);
                     }
                     continue;
                 }
                 case USER:
                 {
                     if(++iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                        iter->eKeyType == MERGED)
                     {
                         outputText("-pseudo_cell", "merged", nOption++);
                     }
                     else
                     {
                         outputText("-pseudo_cell", "user", nOption++);
                         --iter;
                     }
                     ++iter;
                     continue;
                 }
                 case PSEUDO:
                 {
                     outputText("-pseudo_cell", "undo", nOption++);
                     ++iter;
                     continue;
                 }
                 case TOP:
                 {
                     outputText("-trans_to_top", NULL, nOption++);
                     ++iter;
                     continue;
                 }
                 case PREC:
                 {
                     ++iter;
                     std::string sNumExp;
                     if(getNumberExp(iter, pNode->end(), sNumExp))
                     {
                         outputText("-prec_value", sNumExp.c_str(), nOption++);
                     }
                     continue;
                 }
                 case MAG:
                 {
                     ++iter;
                     std::string sNumExp;
                     if(getNumberExp(iter, pNode->end(), sNumExp))
                     {
                         outputText("-mag_value", sNumExp.c_str(), nOption++);
                     }
                     continue;
                 }
                 case AREF:
                 {
                     std::list<rcsToken_T>::iterator begin = iter;
                     std::string sValue;
                     ++iter;
                     if(getStringName(iter, pNode->end(), sValue))
                     {
                         if(m_vVariableNames.find(iter->sValue) != m_vVariableNames.end())
                         {
                             sValue = iter->sValue;
                             toLower(sValue);
                             sValue.insert(0, "$");
                         }
                         outputText("-aref", sValue.c_str(), nOption++);
                     }
                     else
                     {
                         --iter;
                         throw rcErrorNode_T(rcErrorNode_T::ERROR, INP4,
                                             begin->nLineNo, begin->sValue);
                     }

                     ++iter;
                     std::string sNumExp;
                     if(getNumberExp(iter, pNode->end(), sNumExp))
                     {
                         outputText(NULL, sNumExp.c_str(), nOption);
                     }
                     sNumExp.clear();
                     if(getNumberExp(iter, pNode->end(), sNumExp))
                     {
                         outputText(NULL, sNumExp.c_str(), nOption);
                     }
                     if(iter != pNode->end() && !isRuleMapKeyword(*iter))
                     {
                         sNumExp.clear();
                         if(getNumberExp(iter, pNode->end(), sNumExp))
                         {
                             outputText(NULL, sNumExp.c_str(), nOption);
                         }
                     }

                     while(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SUBSTITUTE)
                     {
                         if(++iter == pNode->end() || isRuleMapKeyword(*iter))
                         {
                             throw rcErrorNode_T(rcErrorNode_T::ERROR, DCM13,
                                                 begin->nLineNo, sValue);
                         }

                         std::vector<std::string> vPoints;
                         while(iter != pNode->end())
                         {
                             std::string sNumExp;
                             if(!getNumberExp(iter, pNode->end(), sNumExp, false, false))
                                 break;
                             vPoints.push_back(sNumExp);

                             if(iter == pNode->end() || isRuleMapKeyword(*iter))
                                 break;
                         }

                         Utassert(!vPoints.empty());
                         outputText("-substitute", vPoints[0].c_str(), nOption++);
                         for(hvUInt32 iPoint = 1; iPoint < vPoints.size(); ++iPoint)
                         {
                             outputText(NULL, vPoints[iPoint].c_str(), nOption);
                         }
                     }
                     continue;
                 }
                 case AUTOREF:
                 {
                     std::string sValue;
                     ++iter;
                     if(iter != pNode->end() && getStringName(iter, pNode->end(), sValue))
                     {
                         if(m_vVariableNames.find(iter->sValue) != m_vVariableNames.end())
                         {
                             sValue = iter->sValue;
                             toLower(sValue);
                             sValue.insert(0, "$");
                         }
                         outputText("-autoref", sValue.c_str(), nOption++);
                         ++iter;
                     }
                     else
                     {
                         outputText("-autoref", NULL, nOption++);
                     }
                     continue;
                 }
                 default:
                     break;
             }
         }

         std::string sFileName;
         if(getStringName(iter, pNode->end(), sFileName))
         {
             outputText("-file", sFileName.c_str(), nOption++);
         }
         else
         {
             throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo, iter->sValue);
         }

         ++iter;
     }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertRuleSelectSpec(rcsSynSpecificationNode_T *pNode, const std::string &sCmd)
{
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << sCmd << " ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    while(iter != pNode->end())
    {
        std::string sValue;
        if(!getStringName(iter, pNode->end(), sValue))
            sValue = iter->sValue;

        outputText(NULL, sValue.c_str());
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertLayerListSpec(rcsSynSpecificationNode_T *pNode, const std::string &sCmd)
{
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << sCmd << " ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    while(iter != pNode->end())
    {
        std::string sValue;
        if(!getStringName(iter, pNode->end(), sValue))
            sValue = iter->sValue;

        outputText(NULL, sValue.c_str());
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertCellListSpec(rcsSynSpecificationNode_T *pNode, const std::string &sCmd)
{
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << sCmd << " ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    while(iter != pNode->end())
    {
        std::string sValue;
        if(!getStringName(iter, pNode->end(), sValue))
        {
            sValue = iter->sValue;
        }

        if(m_vVariableNames.find(iter->sValue) != m_vVariableNames.end())
        {
            sValue = "${";
            sValue += iter->sValue;
            sValue += "}";
            toLower(sValue);
        }

        outputText(NULL, sValue.c_str());
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

bool
rcsSynNodeConvertor_T::parsePlaceCellInfo(std::list<rcsToken_T>::iterator &iter,
                                          std::list<rcsToken_T>::iterator end)
{
    m_oPVRSStream << "\\" << std::endl;

    std::string sCellName;
    if(getStringName(iter, end, sCellName))
        outputText("-cell", sCellName.c_str());
    else
        return false;

    if(++iter == end)
        return false;

    std::string sValue;
    if(getNumberExp(iter, end, sValue, false, false))
        outputText("-mag", sValue.c_str());
    else
        return false;

    if(iter == end)
        return false;

    sValue.clear();
    if(getNumberExp(iter, end, sValue, false, false))
        outputText("-rotate", sValue.c_str());
    else
        return false;

    if(iter == end)
        return false;

    sValue.clear();
    if(getNumberExp(iter, end, sValue, false, false))
        outputText("-reflect", sValue.c_str());
    else
        return false;

    if(iter == end)
        return false;

    sValue.clear();
    if(getNumberExp(iter, end, sValue, false, false))
        outputText("-origin", sValue.c_str());
    else
        return false;

    if(iter == end)
        return false;

    sValue.clear();
    if(getNumberExp(iter, end, sValue, false, false))
        outputText(NULL, sValue.c_str());
    else
        return false;

    return true;
}

void
rcsSynNodeConvertor_T::convertUserDefDbSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "db_in_option -user_defined_db ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    if(++iter == pNode->end())
    {
        --iter;
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2, iter->nLineNo,
                            iter->sValue);
    }

    std::string sFileName;
    if(getStringName(iter, pNode->end(), sFileName))
        outputText(NULL, sFileName.c_str(), nOption++);

    if(++iter == pNode->end())
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2, pNode->begin()->nLineNo,
                            pNode->begin()->sValue);
    }

    std::string sCellName;
    switch(iter->eType)
    {
        case SECONDARY_KEYWORD:
        {
            if(GDSII == iter->eKeyType)
            {
                outputText("-format", "gds", nOption++);
                break;
            }
            else if(OASIS == iter->eKeyType)
            {
                outputText("-format", "oasis", nOption++);
                break;
            }
        }
        case IDENTIFIER_NAME:
        case STRING_CONSTANTS:
        {
            if(getStringName(iter, pNode->end(), sCellName))
                outputText("-top_cell", sCellName.c_str(), nOption++);
            break;
        }
        default:
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
            break;
    }

    if(++iter == pNode->end())
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2, pNode->begin()->nLineNo,
                            pNode->begin()->sValue);
    }

    if(sCellName.empty())
    {
        if(getStringName(iter, pNode->end(), sCellName))
            outputText("-top_cell", sCellName.c_str(), nOption++);
        if(++iter == pNode->end())
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2, pNode->begin()->nLineNo,
                                pNode->begin()->sValue);
        }
    }

    if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == EMPTY)
    {
        ++iter;
        if(iter == pNode->end())
        {
            outputText("-no_placed_cell", NULL, nOption++);
        }
        else
        {
            --iter;
        }
    }

    while(iter != pNode->end())
    {
        if(!parsePlaceCellInfo(iter, pNode->end()))
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2, pNode->begin()->nLineNo,
                                pNode->begin()->sValue);
        }
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertDBPathSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    std::string sCmd = "db_in_option -db_path";
    if(pNode->getSpecificationType() == LAYOUT_PATH2)
        sCmd += "2";

    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << sCmd << " ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                if(iter->eKeyType == STDIN && pNode->getSpecificationType() == LAYOUT_PATH)
                {
                    outputText("-stdin", NULL, nOption++);
                    ++iter;
                    continue;
                }
            }
            case IDENTIFIER_NAME:
            {
                if(!isValidSvrfName(iter->sValue))
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
            }
            case STRING_CONSTANTS:
            {
                std::string sValue;
                if(getStringName(iter, pNode->end(), sValue))
                    outputText(NULL, sValue.c_str(), nOption++);
                break;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }

        ++iter;
        while(iter != pNode->end())
        {
            if(iter->eType == SECONDARY_KEYWORD)
            {
                if(iter->eKeyType == PREC)
                {
                    if(++iter != pNode->end() && iter->eKeyType != MAG &&
                       iter->eKeyType != PREC)
                    {
                        std::string sNumExp;
                        if(getNumberExp(iter, pNode->end(), sNumExp))
                            outputText("-prec", sNumExp.c_str(), nOption++);
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP11,
                                            iter->nLineNo, iter->sValue);
                    }

                    if(iter != pNode->end() && (iter->eType == NUMBER ||
                       iter->eType == OPERATOR))
                    {
                        std::string sNumExp;
                        if(getNumberExp(iter, pNode->end(), sNumExp))
                            outputText(NULL, sNumExp.c_str(), nOption++);
                    }
                    continue;
                }
                else if(iter->eKeyType == MAG)
                {
                    if(++iter != pNode->end() && iter->eKeyType != MAG &&
                       iter->eKeyType != PREC && iter->eKeyType != AUTO)
                    {
                        std::string sNumExp;
                        if(getNumberExp(iter, pNode->end(), sNumExp))
                            outputText("-mag", sNumExp.c_str(), nOption++);
                    }
                    else if(iter != pNode->end() &&
                            iter->eType == SECONDARY_KEYWORD &&
                            iter->eKeyType == AUTO)
                    {
                        outputText("-mag", "auto", nOption++);
                        ++iter;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP11,
                                            iter->nLineNo, iter->sValue);
                    }
                    continue;
                }
                else if(iter->eKeyType == GOLDEN)
                {
                    outputCon2Error(pNode, *iter);
                    ++iter;
                    continue;
                }
            }
            break;
        }
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertNameSpec(rcsSynSpecificationNode_T *pNode, const std::string &sCmd)
{
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << sCmd << " ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    if(iter != pNode->end())
    {
        std::string sValue;
        if(!getStringName(iter, pNode->end(), sValue))
            sValue = iter->sValue;

        outputText(NULL, sValue.c_str());
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertCoordListSpec(rcsSynSpecificationNode_T *pNode, const std::string &sCmd)
{
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << sCmd << " ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    bool isAllowExpression = !(pNode->getSpecificationType() == LAYOUT_WINDOW ||
                               pNode->getSpecificationType() == LAYOUT_WINDEL);
    while(iter != pNode->end())
    {
        std::string sValue;
        if(!getNumberExp(iter, pNode->end(), sValue, true, isAllowExpression))
        {
            sValue = iter->sValue;
        }
        outputText(NULL, sValue.c_str());
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertNameListSpec(rcsSynSpecificationNode_T *pNode, const std::string &sCmd)
{
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << sCmd << " ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    while(iter != pNode->end())
    {
        std::string sValue;
        if(m_vVariableNames.find(iter->sValue) != m_vVariableNames.end())
        {
            sValue = "${";
            sValue += iter->sValue;
            sValue += "}";
            toLower(sValue);
        }
        else if(!getStringName(iter, pNode->end(), sValue))
        {
            sValue = iter->sValue;
        }

        outputText(NULL, sValue.c_str());
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertMergeOnReadSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "db_in_option -merge_on_read ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if(iter->eType == SECONDARY_KEYWORD && (iter->eKeyType == YES || iter->eKeyType == NO))
    {
        if(iter->eKeyType == YES)
            outputText(NULL, "yes");
        else if(iter->eKeyType == NO)
            outputText(NULL, "no");
        return;
    }

    while(iter != pNode->end())
    {
        std::string sLayerName;
        if(getLayerRefName(iter, pNode->end(), sLayerName))
            outputText(NULL, sLayerName.c_str(), nOption++);
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertYesNoSpec(rcsSynSpecificationNode_T *pNode, const std::string &sCmd)
{
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if(SECONDARY_KEYWORD == iter->eType && iter->eKeyType == NONE)
    {
        outputCon2Error(pNode, *iter);
        return;
    }

    iter = pNode->begin();
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << sCmd << " ";
    ++iter;
    if(SECONDARY_KEYWORD == iter->eType && (iter->eKeyType == YES || iter->eKeyType == NO ||
       iter->eKeyType == ENABLE || iter->eKeyType == DISABLE || iter->eKeyType == NONE))
    {
       if(iter->eKeyType == YES)
           outputText(NULL, "yes");
       else if(iter->eKeyType == NO)
           outputText(NULL, "no");
       else if(iter->eKeyType == DISABLE)
           outputText(NULL, "disable");
       else if(iter->eKeyType == ENABLE)
           outputText(NULL, "enable");
       else if(iter->eKeyType == NONE)
       {
           Utassert(false);
       }

       ++iter;
       if(pNode->getSpecificationType() == VIRTUAL_CONNECT_REPORT && iter != pNode->end() &&
          (iter->eType == IDENTIFIER_NAME || iter->eType == SECONDARY_KEYWORD) &&
          strcasecmp(iter->sValue.c_str(), "UNSATISFIED") == 0)
       {
           
           outputText(NULL, "unsatisfied");

       }
       else if(iter != pNode->end())
       {
          throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                              iter->nLineNo, iter->sValue);
       }
    }
    else
    {
       throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                           iter->nLineNo, iter->sValue);
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertMaxLimitSpec(rcsSynSpecificationNode_T *pNode, const std::string &sCmd,
                                           bool canSpecAll)
{
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << sCmd << " ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sNumExp;
    if(SECONDARY_KEYWORD == iter->eType && iter->eKeyType == ALL && canSpecAll)
    {
        outputText(NULL, "all");
    }
    else if(getNumberExp(iter, pNode->end(), sNumExp))
    {
        outputText(NULL, sNumExp.c_str());
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertOutputMagSpec(rcsSynSpecificationNode_T *pNode)
{
    if(pNode->getSpecificationType() == DRC_MAGNIFY_RESULTS)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "drc_output_option -mag_drc_result ";
    else if(pNode->getSpecificationType() == DRC_MAGNIFY_DENSITY)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "drc_output_option -mag_density_result ";
    else if(pNode->getSpecificationType() == DRC_MAGNIFY_NAR)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "drc_output_option -mag_nar_result ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sNumExp;
    if(getNumberExp(iter ,pNode->end(), sNumExp))
        outputText(NULL, sNumExp.c_str());

    if(iter != pNode->end())
    {
        if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == PLACE)
        {
            outputText(NULL, "place");
        }
        else
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertResolutionSpec(rcsSynSpecificationNode_T *pNode, const std::string &sCmd)
{
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << sCmd << " ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sNumExp;
    if(getNumberExp(iter, pNode->end(), sNumExp))
    {
        outputText(NULL, sNumExp.c_str());
    }
    sNumExp.clear();
    if(iter != pNode->end())
    {
        if(getNumberExp(iter, pNode->end(), sNumExp))
        {
            outputText(NULL, sNumExp.c_str());
        }
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertExceptionSeveritySpec(rcsSynSpecificationNode_T *pNode)
{
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "db_in_option -exception_severity ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    if(iter != pNode->end())
    {
        std::string sValue = iter->sValue;
        toLower(sValue);
        outputText(NULL, sValue.c_str());
        ++iter;
        if(iter != pNode->end())
            outputText(NULL, iter->sValue.c_str());
        else
            throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2,
                                pNode->begin()->nLineNo, pNode->begin()->sValue);

        if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == RDB)
        {
            ++iter;
            if(iter != pNode->end())
            {
                std::string sRdbName;
                if(getStringName(iter, pNode->end(), sRdbName))
                    outputText("-rdb", sRdbName.c_str());
            }
        }
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2,
                            pNode->begin()->nLineNo, pNode->begin()->sValue);
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertExceptionDBSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "db_in_option -exception_database ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case TOP_SPACE:
                {
                    outputText("-coord_space", "top", nOption++);
                    ++iter;
                    continue;
                }
                case CELL_SPACE:
                {
                    outputText("-coord_space", "cell", nOption++);
                    ++iter;
                    continue;
                }
                case BY_CELL:
                {
                    outputText("-output_mode", "cell", nOption++);
                    ++iter;
                    continue;
                }
                case BY_LAYER:
                {
                    outputText("-output_mode", "layer", nOption++);
                    ++iter;
                    continue;
                }
                case BY_EXCEPTION:
                {
                    outputText("-output_mode", "exception", nOption++);
                    ++iter;
                    continue;
                }
                case EXIT:
                {
                    if(++iter != pNode->end() && SECONDARY_KEYWORD ==
                       iter->eType && NONEMPTY == iter->eKeyType)
                    {
                        outputText("-exit_if_no_empty", NULL, nOption++);
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                    ++iter;
                    continue;
                }
                case MAG:
                {
                    ++iter;
                    std::string sNumExp;
                    if(getNumberExp(iter, pNode->end(), sNumExp))
                        outputText("-mag", sNumExp.c_str(), nOption++);
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                if(!isValidSvrfName(iter->sValue))
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            {
                std::string sFileName;
                if(getStringName(iter, pNode->end(), sFileName))
                {
                    outputText("-file_name", sFileName.c_str(), nOption++);
                    break;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertDepthSpec(rcsSynSpecificationNode_T *pNode, const std::string &sCmd)
{
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << sCmd << " ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sNumExp;
    if(iter->eType == SECONDARY_KEYWORD && (iter->eKeyType == PRIMARY || iter->eKeyType == ALL))
    {
        if(iter->eKeyType == ALL)
            outputText(NULL, "all");
        else
            outputText(NULL, "primary");
    }
    else if(getNumberExp(iter, pNode->end(), sNumExp))
    {
        outputText(NULL, sNumExp.c_str());
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertMapTextLayerSpec(rcsSynSpecificationNode_T *pNode)
{
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "drc_output_option -map_text_layer ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    outputText("-use_layer", NULL);
    while(iter != pNode->end())
    {
        if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == IGNORE)
            break;

        std::string sLayerName;
        if(!getStringName(iter, pNode->end(), sLayerName))
            sLayerName = iter->sValue;

        if(!sLayerName.empty())
            outputText(NULL, sLayerName.c_str());
        ++iter;
    }

    if(iter != pNode->end())
    {
        outputText("-ignore_layer", NULL);

        ++iter;
        while(iter != pNode->end())
        {
            std::string sLayerName;
            if(!getStringName(iter, pNode->end(), sLayerName))
                sLayerName = iter->sValue;

            if(!sLayerName.empty())
                outputText(NULL, sLayerName.c_str());
            ++iter;
        }
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertDrcOutputLibSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "drc_output_option ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sFileName;
    if(getStringName(iter, pNode->end(), sFileName))
    {
        outputText("-lib", sFileName.c_str(), nOption++);
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertDrcOutPrecisionSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "drc_output_option -prec ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sNumExp;
    if(getNumberExp(iter, pNode->end(), sNumExp))
    {
        outputText(NULL, sNumExp.c_str(), nOption);
    }
    sNumExp.clear();
    if(iter != pNode->end())
    {
        if(getNumberExp(iter, pNode->end(), sNumExp))
        {
            outputText(NULL, sNumExp.c_str(), nOption);
        }
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertCorrespondingCellSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "corresponding_cell ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    std::string sLayoutName;
    if(getStringName(iter, pNode->end(), sLayoutName))
    {
        outputText("-layout_name", sLayoutName.c_str(), nOption++);
    }

    ++iter;
    std::string sSourceName;
    if(getStringName(iter, pNode->end(), sSourceName))
    {
        outputText("-source_name", sSourceName.c_str(), nOption++);
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertLayerResolutionSpec(rcsSynSpecificationNode_T *pNode)
{
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "db_layer_resolution ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    outputText("-layer", NULL);
    while(iter != pNode->end() && (iter->eType == IDENTIFIER_NAME || iter->eType == STRING_CONSTANTS ||
          iter->eType == SECONDARY_KEYWORD))
    {
        std::string sLayerName;
        if(!getStringName(iter, pNode->end(), sLayerName))
            break;
        outputText(NULL, sLayerName.c_str());
        ++iter;
    }

    std::string sNumExp;
    if(getNumberExp(iter, pNode->end(), sNumExp))
    {
        outputText("-value", sNumExp.c_str());
    }

    if(iter != pNode->end())
    {
        sNumExp.clear();
        if(getNumberExp(iter, pNode->end(), sNumExp))
        {
            outputText(NULL, sNumExp.c_str());
        }
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertTextToLayerSpec(rcsSynSpecificationNode_T *pNode)
{
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "db_in_option -text_to_layer ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sTextValue;
    if(getStringName(iter, pNode->end(), sTextValue))
    {
        outputText("-text", sTextValue.c_str());
        ++iter;
    }

    std::string sNumExp;
    if(getNumberExp(iter, pNode->end(), sNumExp, false, false))
    {
        outputText("-coord", sNumExp.c_str());
    }

    if(iter != pNode->end())
    {
        sNumExp.clear();
        if(getNumberExp(iter, pNode->end(), sNumExp, false, false))
        {
            outputText(NULL, sNumExp.c_str());
        }
    }

    if(iter != pNode->end())
    {
        std::string sLayerName;
        if(getLayerRefName(iter, pNode->end(), sLayerName))
            outputText("-layer", sLayerName.c_str());
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertTextToDBSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "db_in_option -text_to_db ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sTextValue;
    if(getStringName(iter, pNode->end(), sTextValue))
    {
        outputText("-text", sTextValue.c_str(), nOption++);
        ++iter;
    }

    std::string sNumExp;
    if(getNumberExp(iter, pNode->end(), sNumExp, false, false))
    {
        outputText("-coord", sNumExp.c_str(), nOption++);
    }

    if(iter != pNode->end())
    {
        sNumExp.clear();
        if(getNumberExp(iter, pNode->end(), sNumExp, false, false))
        {
            outputText(NULL, sNumExp.c_str());
        }
    }

    if(iter != pNode->end())
    {
        std::string sLayerName;
        if(getLayerRefName(iter, pNode->end(), sLayerName))
            outputText("-layer", sLayerName.c_str(), nOption++);
    }

    if(iter != pNode->end())
    {
        std::string sCellName;
        std::string sTextType;
        if(getStringName(iter, pNode->end(), sCellName))
            outputText("-place_cell", sCellName.c_str(), nOption++);
        else if(getLayerRefName(iter, pNode->end(), sTextType))
            outputText(NULL, sTextType.c_str(), nOption++);

        if(sCellName.empty() && iter != pNode->end())
            if(getStringName(iter, pNode->end(), sCellName))
                        outputText("-place_cell", sCellName.c_str(), nOption++);
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertPolygonToLayerSpec(rcsSynSpecificationNode_T *pNode)
{
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "db_in_option -polygon_to_layer ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    std::list<rcsToken_T>::iterator end  = pNode->end();
    ++iter;
    --end;
    outputText("-coords", NULL);
    while(iter != end)
    {
        std::string sNumExp;
        if(getNumberExp(iter, end, sNumExp, false, false))
            outputText(NULL, sNumExp.c_str());
    }

    if(iter != pNode->end())
    {
        std::string sLayerName;
        if(getLayerRefName(iter, pNode->end(), sLayerName))
            outputText("-layer", sLayerName.c_str());
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertPolygonToDBSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "db_in_option -polygon_to_db ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sVertexValue;
    if(getNumberExp(iter, pNode->end(), sVertexValue, false, false))
        outputText("-vertex", sVertexValue.c_str(), nOption++);

    outputText("-coords", NULL, nOption++);
    while(iter != pNode->end())
    {
        if((iter->eType == SECONDARY_KEYWORD || iter->eType == IDENTIFIER_NAME ||
           iter->eType == STRING_CONSTANTS) &&
           m_vVariableNames.find(iter->sValue) == m_vVariableNames.end())
            break;

        std::string sValue;
        if(getNumberExp(iter, pNode->end(), sValue, false, false))
            outputText(NULL, sValue.c_str(), nOption);
    }

    if(iter != pNode->end())
    {
        std::string sLayerName;
        if(getLayerRefName(iter, pNode->end(), sLayerName))
            outputText("-layer", sLayerName.c_str(), nOption++);
    }

    if(iter != pNode->end())
    {
        if(iter->eType == SECONDARY_KEYWORD || iter->eType == IDENTIFIER_NAME ||
           iter->eType == STRING_CONSTANTS)
        {
            std::string sCellName;
            if(getStringName(iter, pNode->end(), sCellName))
                outputText("-cell", sCellName.c_str(), nOption++);
            ++iter;
        }
        else
        {
            std::string sNumExp;
            if(getNumberExp(iter, pNode->end(), sNumExp))
                outputText(NULL, sNumExp.c_str(), nOption);
        }
    }

    if(iter != pNode->end())
    {
        if(iter->eType == SECONDARY_KEYWORD || iter->eType == IDENTIFIER_NAME ||
           iter->eType == STRING_CONSTANTS)
        {
            std::string sCellName;
            if(getStringName(iter, pNode->end(), sCellName))
                outputText("-cell", sCellName.c_str(), nOption++);
        }
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertLayerMapSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "layer_map ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if(NUMBER == iter->eType || (OPERATOR == iter->eType &&
       ("+" == iter->sValue || "-" == iter->sValue)))
    {
        std::string sNumExp;
        if(getNumberExp(iter, pNode->end(), sNumExp))
            outputText("-source", sNumExp.c_str(), nOption++);
    }
    else if(OPERATOR == iter->eType)
    {
        std::string sCons;
        if(getConstraint(iter, pNode->end(), sCons))
            outputText("-source", sCons.c_str(), nOption++);
    }

    bool isTextType = false;
    std::string sType;
    if(iter != pNode->end() && SECONDARY_KEYWORD == iter->eType && (DATATYPE == iter->eKeyType ||
       TEXTTYPE == iter->eKeyType))
    {
        if(iter->eKeyType == DATATYPE)
        {
            sType = "-data_type";
            isTextType = false;
        }
        else
        {
            sType = "-text_type";
            isTextType = true;
        }
    }

    ++iter;
    if(NUMBER == iter->eType || (OPERATOR == iter->eType &&
       ("+" == iter->sValue || "-" == iter->sValue)))
    {
        std::string sNumExp;
        if(getNumberExp(iter, pNode->end(), sNumExp))
            outputText(sType.c_str(), sNumExp.c_str(), nOption++);
    }
    else if(OPERATOR == iter->eType)
    {
        std::string sCons;
        if(getConstraint(iter, pNode->end(), sCons))
            outputText(sType.c_str(), sCons.c_str(), nOption++);
    }

    std::string sNumExp;
    if(getNumberExp(iter, pNode->end(), sNumExp))
        outputText("-target", sNumExp.c_str(), nOption++);

    if(isTextType && iter != pNode->end())
    {
        std::string sNumExp;
        if(getNumberExp(iter, pNode->end(), sNumExp))
            outputText(NULL, sNumExp.c_str(), nOption++);
        else
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
    }
    else if(!isTextType && iter != pNode->end())
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertCheckSpec(rcsSynSpecificationNode_T *pNode)
{
    if(pNode->getSpecificationType() == FLAG_OFFGRID)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "db_in_option -offgrid_check ";
    else if(pNode->getSpecificationType() == FLAG_ACUTE)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "db_in_option -acute_check ";
    else if(pNode->getSpecificationType() == FLAG_SKEW)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "db_in_option -non45edge_check ";
    else if(pNode->getSpecificationType() == FLAG_ANGLED)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "db_in_option -non45angle_check ";
    else if(pNode->getSpecificationType() == FLAG_NONSIMPLE)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "db_in_option -non_simple_geom_check ";
    else if(pNode->getSpecificationType() == FLAG_NONSIMPLE_PATH)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "db_in_option -non_simple_path_check ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    switch(iter->eType)
    {
        case SECONDARY_KEYWORD:
        {
            if(YES == iter->eKeyType)
            {
                outputText(NULL, "yes");
                break;
            }
            else if(NO == iter->eKeyType)
            {
                outputText(NULL, "no");
                break;
            }
        }
        default:
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
            break;
        }
    }

    ++iter;
    if(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                if(MAXIMUM == iter->eKeyType)
                {
                    if(++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                       WARNINGS == iter->eKeyType)
                    {
                        ++iter;
                        if(iter != pNode->end())
                        {
                            if(SECONDARY_KEYWORD == iter->eType &&
                               ALL == iter->eKeyType)
                            {
                                outputText("-max_warnings", "all");
                                break;
                            }
                            else
                            {
                                std::string sNumExp;
                                if(getNumberExp(iter, pNode->end(), sNumExp))
                                    outputText("-max_warnings", sNumExp.c_str());
                                break;
                            }
                        }
                        else
                        {
                            --iter;
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP11,
                                                iter->nLineNo, "maximum warnings");
                        }
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                }
            }
            default:
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
            }
        }
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertCellListDefSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    if(pNode->getSpecificationType() == LAYOUT_CELL_LIST)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "db_cell_list ";
    else if(pNode->getSpecificationType() == LVS_CELL_LIST)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "lvs_cell_list ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    std::string sListName;
    if(getStringName(iter, pNode->end(), sListName))
        outputText("-list_name", sListName.c_str(), nOption++);

    ++iter;
    while(iter != pNode->end())
    {
        std::string sPattern;
        if(getStringName(iter, pNode->end(), sPattern))
            outputText("-pattern", sPattern.c_str(), nOption++);
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertExpandTextInCellSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "db_in_option -expand_cell_text ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    std::string sOriginalCellName;
    std::string sTargetCellName;
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                if(PRIMARY == iter->eKeyType)
                {
                    if(sOriginalCellName.empty() || !sTargetCellName.empty())
                    {
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                            iter->nLineNo, iter->sValue);
                    }
                    getStringName(iter, pNode->end(), sTargetCellName);
                    break;
                }
            }
            case IDENTIFIER_NAME:
            {
                if(!isValidSvrfName(iter->sValue))
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                    break;
                }
            }
            case STRING_CONSTANTS:
            {
                if(sOriginalCellName.empty())
                {
                    getStringName(iter, pNode->end(), sOriginalCellName);
                    break;
                }
                if(sTargetCellName.empty())
                {
                    getStringName(iter, pNode->end(), sTargetCellName);
                    break;
                }
            }
            default:
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
            }
        }
        ++iter;
    }

    outputText("-original_cell", sOriginalCellName.c_str(), nOption++);
    outputText("-target_cell", sTargetCellName.c_str(), nOption++);

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertPrecSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    if(pNode->getSpecificationType() == LAYOUT_PRECISION)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "db_in_option -check_prec ";
    else if(pNode->getSpecificationType() == PRECISION)
    {
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "db_in_option -rule_prec ";
    }

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if(iter == pNode->end())
    {
        --iter;
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2,
                            iter->nLineNo, iter->sValue);
    }

    std::string sNumExp1;
    std::string sNumExp2;
    if(getNumberExp(iter, pNode->end(), sNumExp1))
    {
        outputText(NULL, sNumExp1.c_str(), nOption);
    }
    else
    {
        --iter;
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    if(iter != pNode->end())
    {
        if(getNumberExp(iter, pNode->end(), sNumExp2))
        {
            outputText(NULL, sNumExp2.c_str(), nOption);
        }
    }
    else
    {
        sNumExp2 = "[ expr ( 1 ) ]";
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;

    if(pNode->getSpecificationType() == PRECISION)
    {
        m_hasPrecisionDef = true;
        m_oPVRSStream << "set PRECISION [ expr (" << sNumExp1 << " / " << sNumExp2 << ") ]" << std::endl;
        m_oPVRSStream << "set PRECISION_N " << sNumExp1 << std::endl;
        m_oPVRSStream << "set PRECISION_D " << sNumExp2 << std::endl;
    }
}

void
rcsSynNodeConvertor_T::convertRenameTextSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    if(pNode->getSpecificationType() == LAYOUT_IGNORE_TEXT)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "db_in_option -ignore_text ";
    else
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "db_in_option -rename_text ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                if(DATABASE == iter->eKeyType)
                {
                    outputText("-db_level", NULL, nOption++);
                    break;
                }
                else if(RULES == iter->eKeyType)
                {
                    outputText("-rule_level", NULL, nOption++);
                    break;
                }
                else if(BY_LAYER == iter->eKeyType)
                {
                    std::string sLayerName;
                    if(!getLayerRefName(++iter, pNode->end(), sLayerName))
                        sLayerName = iter->sValue;
                    outputText("-by_layer", sLayerName.c_str(), nOption++);

                    if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD
                       && TO == iter->eKeyType)
                    {
                        sLayerName.clear();
                        if(!getLayerRefName(++iter, pNode->end(), sLayerName))
                            sLayerName = iter->sValue;
                        outputText("-to_layer", sLayerName.c_str(), nOption++);
                    }
                    continue;
                }
                else if(CELL == iter->eKeyType)
                {
                    if(++iter != pNode->end() && iter->eKeyType == LIST)
                    {
                        switch(iter->eType)
                        {
                            case SECONDARY_KEYWORD:
                            case IDENTIFIER_NAME:
                            {
                                if(!isValidSvrfName(iter->sValue))
                                {
                                    throw rcErrorNode_T(rcErrorNode_T::ERROR,
                                                        INP1,
                                                        iter->nLineNo,
                                                        iter->sValue);
                                }
                            }
                            case STRING_CONSTANTS:
                            {
                                std::string sCellListName;
                                if(!getStringName(++iter, pNode->end(), sCellListName))
                                    sCellListName = iter->sValue;
                                outputText("-in_cell_list",  sCellListName.c_str(), nOption++);
                                break;
                            }
                            default:
                                throw rcErrorNode_T(rcErrorNode_T::ERROR,
                                                    INP1, iter->nLineNo,
                                                    iter->sValue);
                                break;
                        }
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                    break;
                }
            }
            case IDENTIFIER_NAME:
            {
                if(!isValidSvrfName(iter->sValue))
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
            }
            case STRING_CONSTANTS:
            {
                std::string sValue;
                if(!getStringName(iter, pNode->end(), sValue))
                    sValue = iter->sValue;
                outputText("-pattern", sValue.c_str(), nOption++);
                break;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC14,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertRenameCellSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "db_in_option -cell_rename ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sName;
    if(getStringName(iter, pNode->end(), sName))
    {
        outputText("-source", sName.c_str(), nOption++);
    }
    sName.clear();
    ++iter;
    if(getStringName(iter, pNode->end(), sName))
    {
        outputText("-target", sName.c_str(), nOption++);
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertDBMagnifySpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "db_in_option -mag_layout ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    switch(iter->eType)
    {
        case SECONDARY_KEYWORD:
        {
            if(AUTO == iter->eKeyType)
            {
                outputText(NULL, "auto", nOption++);
                if(++iter != pNode->end())
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
            }
        }
        default:
        {
            std::string sNumExp;
            if(getNumberExp(iter, pNode->end(), sNumExp))
                outputText(NULL, sNumExp.c_str(), nOption++);
            break;
        }
    }

    if(iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
       PLACE == iter->eKeyType)
    {
        outputText(NULL, "place", nOption++);
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGroupGlobalNameSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "global_name_group ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    std::string sGroupName;
    if(getStringName(iter, pNode->end(), sGroupName))
        outputText("-group_name", sGroupName.c_str(), nOption++);

    ++iter;
    outputText("-net_names", NULL, nOption++);
    while(iter != pNode->end())
    {
        std::string sValue;
        if(!getStringName(iter, pNode->end(), sValue))
        {
           sValue = iter->sValue;
        }

        outputText(NULL, sValue.c_str(), nOption);
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertErrorSpec(rcsSynSpecificationNode_T *pNode)
{
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if(iter == pNode->end())
    {
        --iter;
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2, iter->nLineNo, iter->sValue);
    }

    std::string sValue;
    if(getStringName(iter, pNode->end(), sValue))
        m_oPVRSStream << "error " << sValue << std::endl;

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertPutsSpec(rcsSynSpecificationNode_T *pNode)
{
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if(iter == pNode->end())
    {
        --iter;
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2, iter->nLineNo, iter->sValue);
    }

    std::string sMessage;
    if(getStringName(iter, pNode->end(), sMessage))
        m_oPVRSStream << "puts " << sMessage << std::endl;

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertUnitOptionSpec(rcsSynSpecificationNode_T *pNode)
{
    if(pNode->getSpecificationType() == UNIT_CAPACITANCE)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "unit -cap ";
    else if(pNode->getSpecificationType() == UNIT_INDUCTANCE)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "unit -ind ";
    else if(pNode->getSpecificationType() == UNIT_LENGTH)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "unit -length ";
    else if(pNode->getSpecificationType() == UNIT_RESISTANCE)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "unit -res ";
    else if(pNode->getSpecificationType() == UNIT_TIME)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "unit -time ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if(iter == pNode->end())
    {
        --iter;
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2, iter->nLineNo, iter->sValue);
    }

    std::string sValue;
    if(getStringName(iter, pNode->end(), sValue))
    {
        outputText(NULL, sValue.c_str());
    }
    else if(getNumberExp(iter, pNode->end(), sValue))
    {
        outputText(NULL, sValue.c_str());
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertPropTextSpec(rcsSynSpecificationNode_T *pNode)
{
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "db_in_option ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sValue;
    if(getNumberExp(iter, pNode->end(), sValue))
        outputText("-gds_prop_as_text", sValue.c_str());

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertPropResolutionMaxSpec(rcsSynSpecificationNode_T *pNode)
{
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "compare_option -maximum_prop_resolution ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sValue;
    if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == ALL)
        outputText(NULL, "all");
    else if(getNumberExp(iter, pNode->end(), sValue))
        outputText(NULL, sValue.c_str());

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertOutputLvsDBSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "output_lvs_db ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sValue;
    if(getStringName(iter, pNode->end(), sValue))
        outputText("-file_name", sValue.c_str(), nOption++);

    ++iter;
    bool hasOption = false;
    while(iter != pNode->end())
    {
        if(strcasecmp(iter->sValue.c_str(), "xcalibre") == 0)
        {
            ++iter;
            continue;
        }

        if(!hasOption)
        {
            hasOption = true;
            outputText("-option", NULL, nOption++);
        }

        std::string sValue = iter->sValue;
        toLower(sValue);
        outputText(NULL, sValue.c_str(), nOption++);
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertMapDeviceTypeSpec(rcsSynSpecificationNode_T *pNode)
{
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "map_device_type ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    bool hasBuiltInType = false;
    while(iter != pNode->end())
    {
        if(iter->eType == BUILT_IN_LANG)
        {
            std::string sValue = iter->sValue;
            sValue.erase(0, 1);
            sValue.insert(0, "{ ");
            sValue.erase(sValue.size() - 1);
            sValue.insert(sValue.size(), " }");
            outputText("-pin_map", sValue.c_str());
        }
        else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == LAYOUT)
        {
            outputText("-design", "layout");
        }
        else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SOURCE)
        {
            outputText("-design", "source");
        }
        else if(hasBuiltInType)
        {
            std::string sNewType;
            getStringName(iter, pNode->end(), sNewType);
            outputText(NULL, sNewType.c_str());
        }
        else
        {
            std::string sBuiltInType;
            getStringName(iter, pNode->end(), sBuiltInType);
            outputText("-type_map", sBuiltInType.c_str());
            hasBuiltInType = true;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertMapDeviceSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "map_device_name ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sOldComponentType;
    if(getStringName(iter, pNode->end(), sOldComponentType))
        outputText("-old", sOldComponentType.c_str(), nOption++);

    ++iter;
    if(iter!= pNode->end() && SEPARATOR == iter->eType && "(" == iter->sValue)
    {
        ++iter;
        std::string sOldSubType;
        if(getStringName(iter, pNode->end(), sOldSubType))
        {
            sOldSubType.insert(0, "( ");
            sOldSubType += " )";
            outputText(NULL, sOldSubType.c_str());
        }
        ++iter;
        Utassert(iter->eType == SEPARATOR && iter->sValue == ")");
        ++iter;
    }

    std::string sNewComponentType;
    if(getStringName(iter, pNode->end(), sNewComponentType))
        outputText("-new", sNewComponentType.c_str(), nOption++);

    ++iter;
    if(iter!= pNode->end() && SEPARATOR == iter->eType && "(" == iter->sValue)
    {
        ++iter;
        std::string sNewSubType;
        if(getStringName(iter, pNode->end(), sNewSubType))
        {
            sNewSubType.insert(0, "( ");
            sNewSubType += " )";
            outputText(NULL, sNewSubType.c_str());
        }
        ++iter;
        Utassert(iter->eType == SEPARATOR && iter->sValue == ")");
        ++iter;
    }

    if(iter != pNode->end())
    {
        if(iter->eType == SECONDARY_KEYWORD && (iter->eKeyType == LAYOUT || iter->eKeyType == SOURCE))
        {
            if(iter->eKeyType == LAYOUT)
            {
                outputText("-design", "layout", nOption++);
            }
            else if(iter->eKeyType == SOURCE)
            {
                outputText("-design", "source", nOption++);
            }
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertDeviceSpec(rcsSynSpecificationNode_T *pNode)
{
    static hvInt8 iPinLayer              = 1;
    static hvInt8 iTextPropertyLayer     = 2;
    static hvInt8 iSignatureContextLayer = 3;

    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "device ";

    bool hasElementName = false;
    bool hasDeviceLayer = false;
    hvInt8 iLayerType   = 1;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case BY_NET:
                {
                    iLayerType = iPinLayer;
                    outputText("-pin_recognize_by", "net", nOption++);
                    ++iter;
                    continue;
                }
                case BY_SHAPE:
                {
                    iLayerType = iPinLayer;
                    outputText("-pin_recognize_by", "shape", nOption++);
                    ++iter;
                    continue;
                }
                case NETLIST:
                {
                    iLayerType = iPinLayer;
                    ++iter;
                    std::string sOption;
                    if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == MODEL)
                    {
                        sOption = "-netlist_model_name";
                    }
                    else if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == ELEMENT)
                    {
                        sOption = "-netlist_element_name";
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }

                    ++iter;
                    std::string sValue;
                    if(getStringName(iter, pNode->end(), sValue))
                        outputText(sOption.c_str(), sValue.c_str(), nOption++);
                    ++iter;
                    continue;
                }
                case TEXT_MODEL_LAYER:
                {
                    iLayerType = iPinLayer;
                    ++iter;
                    std::string sLayerName;
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText("-text_model_layer", sLayerName.c_str(), nOption++);
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP10,
                                            iter->nLineNo, iter->sValue);
                    }
                    continue;
                }
                case TEXT_PROPERTY_LAYER:
                {
                    iLayerType = iTextPropertyLayer;
                    ++iter;
                    std::string sLayerName;
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText("-text_property_layer", sLayerName.c_str(), nOption++);
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP10,
                                            iter->nLineNo, iter->sValue);
                    }
                    continue;
                }
                case SIGNATURE:
                {
                    std::string sValue;
                    pNode->getCmdValue(sValue);
                    m_oLogStream << "This Option " << iter->sValue << " in line " << iter->nLineNo
                                 << " can not be Converted correctly in this operation: " << std::endl
                                 << sValue << std::endl << std::endl;

                    s_errManager.addError(rcErrorNode_T(rcErrorNode_T::WARNING, CON2,
                                                        iter->nLineNo, iter->sValue));

                    iLayerType = iSignatureContextLayer;
                    ++iter;
                    if(iter == pNode->end())
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP18,
                                            iter->nLineNo, iter->sValue);
                    }

                    ++iter;
                    if(iter != pNode->end())
                    {
                        std::string sLayerName;
                        getLayerRefName(iter, pNode->end(), sLayerName);
                    }

                    if(iter != pNode->end())
                    {
                        std::string sLayerName;
                        getLayerRefName(iter, pNode->end(), sLayerName);
                    }
                    continue;
                }
                default:
                    break;
            }
        }
        else if(OPERATOR == iter->eType && iter->sValue=="<" )
        {
            iLayerType = iPinLayer;
            ++iter;
            std::string sLayerName;
            if(getLayerRefName(iter, pNode->end(), sLayerName))
                outputText("-auxiliary_layer", sLayerName.c_str(), nOption++);

            if(iter == pNode->end() || OPERATOR != iter->eType || iter->sValue != ">")
            {
                --iter;
                throw rcErrorNode_T(rcErrorNode_T::ERROR, DEV10,
                                    iter->nLineNo);
            }
            ++iter;
            continue;
        }
        else if(SEPARATOR == iter->eType && iter->sValue=="(")
        {
            iLayerType = iPinLayer;
            ++iter;
            std::string sSwapPinList = "{";
            while(iter != pNode->end() && iter->eType != SEPARATOR && iter->sValue != ")")
            {
                std::string sValue;
                if(getStringName(iter, pNode->end(), sValue, false))
                    sSwapPinList += sValue;
                sSwapPinList += " ";
                ++iter;
            }
            sSwapPinList.erase(sSwapPinList.size() - 1);
            sSwapPinList += "}";
            outputText("-swap_pin_list", sSwapPinList.c_str(), nOption++);
            ++iter;
            continue;
        }
        else if(BUILT_IN_LANG == iter->eType)
        {
            iLayerType = iPinLayer;
            std::string sValue = iter->sValue;
            sValue.erase(0, 1);
            sValue.insert(0, "{ ");
            sValue.erase(sValue.size() - 1);
            sValue.insert(sValue.size(), " }");
            outputText("-device_prop_def", sValue.c_str(), nOption++);
            ++iter;
            continue;
        }

        if(!hasElementName)
        {
            std::string sElementName;
            getStringName(iter, pNode->end(), sElementName);
            ++iter;
            if(iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
            {
                sElementName += " ( ";
                std::string sModelName;
                ++iter;
                getStringName(iter, pNode->end(), sModelName);
                sElementName += sModelName;
                ++iter;
                if(iter == pNode->end() || iter->eType != SEPARATOR || iter->sValue != ")")
                {
                    if(iter == pNode->end())
                        --iter;
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
                sElementName += " )";
                ++iter;
            }
            outputText("-element_name", sElementName.c_str(), nOption++);
            hasElementName = true;
            continue;
        }
        else if(!hasDeviceLayer)
        {
            std::string sLayerName;
            if(getLayerRefName(iter, pNode->end(), sLayerName))
                outputText("-device_layer", sLayerName.c_str(), nOption++);
            hasDeviceLayer = true;
            continue;
        }
        else if(iPinLayer == iLayerType)
        {
            std::string sLayerName;
            getLayerRefName(iter, pNode->end(), sLayerName);
            if(iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
            {
                std::list<rcsToken_T>::iterator iTmp = iter;
                ++iTmp;
                ++iTmp;
                if(iTmp == pNode->end() || iTmp->eType != SEPARATOR || iTmp->sValue != ")")
                {
                    outputText("-pin_layer", sLayerName.c_str(), nOption++);
                    continue;
                }

                sLayerName += " ( ";
                std::string sPinName;
                ++iter;
                getStringName(iter, pNode->end(), sPinName);
                sLayerName += sPinName;
                ++iter;
                if(iter == pNode->end() || iter->eType != SEPARATOR || iter->sValue != ")")
                {
                    if(iter == pNode->end())
                        --iter;
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
                sLayerName += " )";
                ++iter;
            }
            outputText("-pin_layer", sLayerName.c_str(), nOption++);
            continue;
        }
        else if(iTextPropertyLayer == iLayerType)
        {
            std::string sLayerName;
            if(getLayerRefName(iter, pNode->end(), sLayerName))
                outputText(NULL, sLayerName.c_str(), nOption++);
            else
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            continue;
        }
        else if(iSignatureContextLayer == iLayerType)
        {
            std::string sLayerName;
            if(!getLayerRefName(iter, pNode->end(), sLayerName))
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            continue;
        }

        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertBlackBoxSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "black_box ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    while(iter != pNode->end())
    {
        std::string sCellName;
        if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == LAYOUT)
            outputText("-design", "layout", nOption++);
        else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SOURCE)
            outputText("-design", "source", nOption++);
        else if(getStringName(iter, pNode->end(), sCellName))
        {
            if(m_vVariableNames.find(iter->sValue) != m_vVariableNames.end())
            {
                sCellName = iter->sValue;
                toLower(sCellName);
                sCellName.insert(0, "$");
            }
            outputText("-cell_name", sCellName.c_str(), nOption++);
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertSconnectCheckSpec(rcsSynSpecificationNode_T *pNode)
{
    if(_hasOnlyDirectKeyword(pNode))
    {
        outputCon1Error(pNode);
        return;
    }

    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "sconnect_check ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sLayerName;
    if(getLayerRefName(iter, pNode->end(), sLayerName))
        outputText("-lower_layer", sLayerName.c_str(), nOption++);
    if(iter != pNode->end())
        outputText("-option", NULL, nOption++);
    while(iter != pNode->end())
    {
        std::string sValue = iter->sValue;
        toLower(sValue);
        outputText(NULL, sValue.c_str(), nOption++);
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertDeviceOnPathSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "device_on_path ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    while(iter != pNode->end())
    {
        if(iter->eType == SECONDARY_KEYWORD)
        {
            switch(iter->eKeyType)
            {
                case BIPOLAR:
                    outputText("-bipolar", NULL, nOption++);
                    ++iter;
                    continue;
                case CAPACITOR:
                    outputText("-capacitor", NULL, nOption++);
                    ++iter;
                    continue;
                case DIODE:
                    outputText("-diode", NULL, nOption++);
                    ++iter;
                    continue;
                default:
                    break;
            }
        }
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertInitialNetSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "initial_net ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sLayoutName;
    if(getStringName(iter, pNode->end(), sLayoutName))
        outputText("-layout_net", sLayoutName.c_str(), nOption++);

    ++iter;
    std::string sSourceName;
    if(getStringName(iter, pNode->end(), sSourceName))
        outputText("-source_net", sSourceName.c_str(), nOption++);

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertInitialPropertySpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "initial_device_property ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sComponentType;
    if(!getStringName(iter, pNode->end(), sComponentType))
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);

    ++iter;
    if(iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
    {
        sComponentType += "( ";
        std::string sSubType;
        ++iter;
        if(!getStringName(iter, pNode->end(), sSubType))
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        sComponentType += sSubType;

        ++iter;
        if(iter == pNode->end() || iter->eType != SEPARATOR || iter->sValue != ")")
        {
            if(iter == pNode->end())
                --iter;
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
        sComponentType += " )";
        ++iter;
    }

    outputText("-component_type", sComponentType.c_str(), nOption++);

    if(iter != pNode->end() && iter->eType == BUILT_IN_LANG)
    {
        std::string sValue = iter->sValue;
        sValue.erase(0, 1);
        sValue.insert(0, "{ ");
        sValue.erase(sValue.size() - 1);
        sValue.insert(sValue.size(), " }");
        outputText("-program", sValue.c_str(), nOption++);
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertMapPropertySpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "map_property_name ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sComponentType;
    if(getStringName(iter, pNode->end(), sComponentType))
        outputText("-component_type", sComponentType.c_str(), nOption++);

    ++iter;
    std::string sTargetName;
    if(getStringName(iter, pNode->end(), sTargetName))
        outputText("-target_name", sTargetName.c_str(), nOption++);

    ++iter;
    std::string sDatabaseName;
    if(getStringName(iter, pNode->end(), sDatabaseName))
    {
        ++iter;
        if(iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
        {
            sDatabaseName += "( ";
            ++iter;
            std::string sSpicePara;

            if(!getStringName(iter, pNode->end(), sSpicePara))
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            sDatabaseName += sSpicePara;
            ++iter;
            sDatabaseName += " )";
            ++iter;
        }
    }
    outputText("-db_name", sDatabaseName.c_str(), nOption++);

    if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == LAYOUT)
    {
        outputText("-design", "layout", nOption++);
    }
    else if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SOURCE)
    {
        outputText("-design", "source", nOption++);
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertFindShortsSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "find_shorts ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD)
    {
        if(iter->eKeyType == YES)
            outputText(NULL, "yes", nOption++);
        else if(iter->eKeyType == NO)
            outputText(NULL, "no", nOption++);
        ++iter;
    }

    if(iter != pNode->end())
        outputText("-output_option", NULL, nOption++);

    std::string sNameValue;
    while(iter != pNode->end())
    {
        if(iter->eType == SECONDARY_KEYWORD)
        {
            switch(iter->eKeyType)
            {
                case BY_CELL:
                {
                    ++iter;
                    if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       iter->eKeyType == ALSO)
                    {
                        outputText(NULL, "by_cell_also", nOption++);
                        ++iter;
                    }
                    else
                    {
                        outputText(NULL, "by_cell", nOption++);
                    }
                    continue;
                }
                case BY_LAYER:
                {
                    ++iter;
                    if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       iter->eKeyType == ALSO)
                    {
                        outputText(NULL, "by_layer_also", nOption++);
                        ++iter;
                    }
                    else
                    {
                        outputText(NULL, "by_layer", nOption++);
                    }
                    continue;
                }
                case ACCUMULATE:
                {
                    outputText(NULL, "accumulate", nOption++);
                    ++iter;
                    continue;
                }
                case NO:
                {
                    ++iter;
                    if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       iter->eKeyType == CONTACTS)
                    {
                        outputText(NULL, "no_contacts", nOption++);
                        ++iter;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                            iter->nLineNo, iter->sValue);
                    }
                    continue;
                }
                case CELL:
                {
                    ++iter;
                    if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       (iter->eKeyType == ALL || iter->eKeyType == PRIMARY))
                    {
                        if(iter->eKeyType == ALL)
                            outputText(NULL, "all_cells", nOption++);
                        else
                            outputText(NULL, "top_cell", nOption++);
                        ++iter;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                            iter->nLineNo, iter->sValue);
                    }
                    continue;
                }
                case FLAT:
                {
                    outputText(NULL, "flat", nOption++);
                    ++iter;
                    continue;
                }
                case UNMERGED:
                {
                    outputText(NULL, "unmerged", nOption++);
                    ++iter;
                    continue;
                }
                default:
                    break;
            }
        }
        else if(OPERATOR == iter->eType && (iter->sValue == "||" || iter->sValue == "&&"))
        {
            if(iter->sValue == "||")
            {
                sNameValue = "-or_names";
            }
            else if(iter->sValue == "&&")
            {
                sNameValue = "-and_names";
            }
            ++iter;
            if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == NAME)
            {
                std::string sName;
                ++iter;
                while(iter != pNode->end())
                {
                    if(iter->eType != SECONDARY_KEYWORD || (iter->eKeyType != BY_CELL &&
                       iter->eKeyType != BY_LAYER && iter->eKeyType != FLAT &&
                       iter->eKeyType != UNMERGED && iter->eKeyType != NO &&
                       iter->eKeyType != ACCUMULATE && iter->eKeyType != CELL))
                    {
                        std::string sValue;
                        if(getStringName(iter, pNode->end(), sValue))
                        {
                            sName += sValue;
                            sName += " ";
                        }
                    }
                    else
                        break;
                    ++iter;
                }
                outputText(sNameValue.c_str(), sName.c_str(), nOption++);
            }
            else
            {
                --iter;
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
            continue;
        }

        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::outputNetListOutputSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "output_netlist ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    if(iter->eType == SPECIFICATION_KEYWORD)
    {
        if(iter->eKeyType == LVS_WRITE_INJECTED_LAYOUT_NETLIST)
        {
            outputText("-type", "inject_layout", nOption++);
        }
        else if(iter->eKeyType == LVS_WRITE_INJECTED_SOURCE_NETLIST)
        {
            outputText("-type", "inject_source", nOption++);
        }
        else if(iter->eKeyType == LVS_WRITE_LAYOUT_NETLIST)
        {
            outputText("-type", "layout", nOption++);
        }
        else if(iter->eKeyType == LVS_WRITE_SOURCE_NETLIST)
        {
            outputText("-type", "source", nOption++);
        }
    }

    ++iter;
    std::string sFileName;
    if(getStringName(iter, pNode->end(), sFileName))
        outputText("-file", sFileName.c_str(), nOption++);

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertFilterUnusedOptionSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "filter_unused_option ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    for(; iter != pNode->end(); ++iter)
    {
        if(0 == strcasecmp(iter->sValue.c_str(), "AB"))
        {
            outputText("-connected_gsd", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "AC"))
        {
            outputText("-floating_g_power_sd", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "AD"))
        {
            outputText("-floating_g_ground_sd", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "AE"))
        {
            outputText("-floating_gsd", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "AF"))
        {
            outputText("-connected_sd", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "AG"))
        {
            outputText("-connected_all", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "AH") == 0)
        {
            outputText("-connected_sdb_floating_g", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "AI") == 0)
        {
            outputText("-connected_sgdb", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "B"))
        {
            outputText("-nopath_g_floating_sord", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "C"))
        {
            outputText("-supply_g_floating_sord", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "D"))
        {
            outputText("-floating_g_groundpath_sd", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "E"))
        {
            outputText("-floating_g_powerpath_sord", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "F"))
        {
            outputText("-ground_ngate", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "FF") == 0)
        {
            outputText("-ground_g_floating_s_or_d", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "FY"))
        {
            outputText("-ground_ngate2", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "GY"))
        {
            outputText("-power_pgate2", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "G"))
        {
            outputText("-power_pgate", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "GF") == 0)
        {
            outputText("-power_g_floating_s_or_d", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "H"))
        {
            outputText("-onlypower_sd", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "I"))
        {
            outputText("-onlyground_sd", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "IH"))
        {
            outputText("-power_ground_sd", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "INV"))
        {
            outputText("-inv", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "J"))
        {
            outputText("-supply_g_connected_sd", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "K"))
        {
            outputText("-nopath_sd", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "L"))
        {
            outputText("-onlyfloating_sord", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "M"))
        {
            outputText("-ground_pgate2", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "MG") == 0)
        {
            outputText("-ground_g_power_sd", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "N"))
        {
            outputText("-power_ngate2", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "NP") == 0)
        {
            outputText("-power_g_ground_sd", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "O"))
        {
            outputText("-filter_all", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "P"))
        {
            outputText("-nopath_device", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "Q"))
        {
            outputText("-supply_device", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "RB"))
        {
            outputText("-floating_r", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "RC"))
        {
            outputText("-connected_r", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "RD"))
        {
            outputText("-floating_c", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "RE"))
        {
            outputText("-connected_c", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "RF"))
        {
            outputText("-floating_d", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "RG"))
        {
            outputText("-connected_d", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "S"))
        {
            outputText("-floating_q", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "T"))
        {
            outputText("-connected_ec", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "U"))
        {
            outputText("-floating_sd", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "V"))
        {
            outputText("-floating_mos", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "YB"))
        {
            outputText("-connected_be", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "YC"))
        {
            outputText("-connected_q", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "ZB"))
        {
            outputText("-floatingall_r", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "ZC"))
        {
            outputText("-floatingall_c", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "ZD"))
        {
            outputText("-floatingall_d", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "ZE"))
        {
            outputText("-floatingall_q", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "NONE"))
        {
            outputText("-no_set", NULL, nOption++);
        }
        else
        {
            if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == LAYOUT)
            {
                outputText("-design", "layout", nOption++);
            }
            else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SOURCE)
            {
                outputText("-design", "source", nOption++);
            }
            else
            {
                std::string sValue;
                pNode->getCmdValue(sValue);
                m_oLogStream << "This Option " << iter->sValue << " in line " << iter->nLineNo
                             << " can not be Converted correctly in this operation: " << std::endl
                             << sValue << std::endl << std::endl;

                s_errManager.addError(rcErrorNode_T(rcErrorNode_T::WARNING, CON2,
                                                    iter->nLineNo, iter->sValue));

                if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == DEVICES)
                    return;
            }
        }
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

static bool
isPathChkKeyword(const std::list<rcsToken_T>::iterator iter)
{
    if(iter->eType == SECONDARY_KEYWORD)
    {
        switch(iter->eKeyType)
        {
            case PRINT:
            case BY_LAYER:
            case BY_CELL:
            case FLAT:
            case PORTS_ALSO:
            case NOFLOAT:
            case EXCLUDE_UNUSED:
            case EXCLUDE_SUPPLY:
            case POWERNAME:
            case GROUNDNAME:
            case LABELNAME:
            case BREAKNAME:
                return true;
            default:
                return false;
        }
    }
    else if(iter->eType == LAYER_OPERATION_KEYWORD)
    {
        return true;
    }
    return false;
}

void
rcsSynNodeConvertor_T::parsePathChkName(std::list<rcsToken_T>::iterator &iter,
                                        std::list<rcsToken_T>::iterator end,
                                        std::string &sValues)
{
    ++iter;
    while(iter != end)
    {
        std::string sValue;
        if(!isPathChkKeyword(iter) && getStringName(iter, end, sValue))
        {
            sValues += sValue;
            sValues += " ";
        }
        else
            break;
        ++iter;
    }

    if(sValues.empty())
    {
        --iter;
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo,
                            iter->sValue);
    }
}

void
rcsSynNodeConvertor_T::parseBreakName(std::list<rcsToken_T>::iterator &iter,
                                      std::list<rcsToken_T>::iterator end,
                                      std::string &sValues)
{
    std::list<rcsToken_T>::iterator begin = iter;
    ++iter;
    while(iter != end)
    {
        if(!isPathChkKeyword(iter) && getStringName(iter, end, sValues))
        {
            if(++iter != end && iter->eType == SEPARATOR && iter->sValue == "(")
            {
                sValues += " ( ";
                ++iter;
                while(!(iter->eType == SEPARATOR && iter->sValue == ")"))
                {
                    std::string sNetName;
                    if(!isPathChkKeyword(iter) && getStringName(iter, end, sNetName))
                        sValues += sNetName;
                    else
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                            begin->nLineNo, begin->sValue);
                    sValues += " ";
                    ++iter;
                }
                sValues += ")";
                ++iter;
            }
        }
        else
            break;
    }
}

void
rcsSynNodeConvertor_T::convertPathchkCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "pathchk ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    while(iter != pNode->end())
    {
        if(iter->eType == SECONDARY_KEYWORD)
        {
            switch(iter->eKeyType)
            {
                case LABELED:
                {
                    outputText("-labeled_flag", "labeled", nOption++);
                    ++iter;
                    continue;
                }
                case POWER:
                {
                    outputText("-power_flag", "power", nOption++);
                    ++iter;
                    continue;
                }
                case GROUND:
                {
                    outputText("-ground_flag", "ground", nOption++);
                    ++iter;
                    continue;
                }
                case PRINT:
                {
                    if(++iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       (iter->eKeyType == NETS || iter->eKeyType == POLYGONS))
                    {
                        std::string sOption;
                        if(iter->eKeyType == NETS)
                        {
                            sOption = "-print_nets";
                        }
                        else
                        {
                            sOption = "-print_polygons";
                        }

                        if(++iter != pNode->end())
                        {
                            std::string sFileName;
                            if(getStringName(iter, pNode->end(), sFileName))
                                outputText(sOption.c_str(), sFileName.c_str(), nOption++);
                            ++iter;
                        }
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                    continue;
                }
                case BY_LAYER:
                {
                    outputText("-polygon_print_option", "by_layer", nOption++);
                    ++iter;
                    continue;
                }
                case BY_CELL:
                {
                    outputText("-polygon_print_option", "by_cell", nOption++);
                    ++iter;
                    continue;
                }
                case FLAT:
                {
                    outputText("-polygon_print_option", "flat", nOption++);
                    ++iter;
                    continue;
                }
                case PORTS_ALSO:
                {
                    outputText("-port_also", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case NOFLOAT:
                {
                    outputText("-no_float", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case EXCLUDE_UNUSED:
                {
                    outputText("-exclude_unused", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case EXCLUDE_SUPPLY:
                {
                    outputText("-exclude_supply", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case POWERNAME:
                {
                    std::string sPowerName;
                    parsePathChkName(iter, pNode->end(), sPowerName);
                    outputText("-power_name", sPowerName.c_str(), nOption++);
                    continue;
                }
                case GROUNDNAME:
                {
                    std::string sGroundName;
                    parsePathChkName(iter, pNode->end(), sGroundName);
                    outputText("-ground_name", sGroundName.c_str(), nOption++);
                    continue;
                }
                case LABELNAME:
                {
                    std::string sLabelName;
                    parsePathChkName(iter, pNode->end(), sLabelName);
                    outputText("-label_name", sLabelName.c_str(), nOption++);
                    continue;
                }
                case BREAKNAME:
                {
                    std::string sBreakName;
                    parseBreakName(iter, pNode->end(), sBreakName);
                    if(!sBreakName.empty())
                        outputText("-break_name", sBreakName.c_str(), nOption++);
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                    outputText("-in_layer", sLayerName.c_str(), nOption++);
                continue;
            }
            case OPERATOR:
            {
                if(iter->sValue == "+" || iter->sValue == "-")
                {
                    std::string sLayerName;
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                        outputText("-in_layer", sLayerName.c_str(), nOption++);
                    continue;
                }
                else if("!" == iter->sValue)
                {
                    ++iter;
                    if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD)
                    {
                        if(iter->eKeyType == LABELED)
                        {
                            outputText("-labeled_flag", "!labeled", nOption++);
                        }
                        else if(iter->eKeyType == POWER)
                        {
                            outputText("-power_flag", "!power", nOption++);
                        }
                        else if(iter->eKeyType == GROUND)
                        {
                            outputText("-ground_flag", "!ground", nOption++);
                        }
                        else
                        {
                            --iter;
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo,
                                                iter->sValue);
                        }
                    }
                    break;
                }
                else if( "&&" == iter->sValue)
                {
                    outputText("-operand", "and", nOption++);
                    break;
                }
                else if( "||" == iter->sValue)
                {
                    outputText("-operand", "or", nOption++);
                    break;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo,
                                    iter->sValue);
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertTclProcSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "tcl_proc_in_exec ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    Utassert(iter->eType == SPECIFICATION_KEYWORD && iter->eKeyType == TVF_FUNCTION);
    ++iter;
    std::string sNameSpace;
    if(getStringName(iter, pNode->end(), sNameSpace))
        outputText("-name_space", sNameSpace.c_str(), nOption++);

    ++iter;
    if(iter != pNode->end() && iter->eType == BUILT_IN_LANG)
    {
        std::string sValue = iter->sValue;
        sValue.erase(0, 1);
        sValue.insert(0, "{ ");
        sValue.erase(sValue.size() - 1);
        sValue.insert(sValue.size(), "}");

        outputText("-proc_define", sValue.c_str(), nOption++);
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertFileHeadNameSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "file_head_name ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    std::string sValue;
    if(getStringName(iter, pNode->end(), sValue))
        outputText(NULL, sValue.c_str(), nOption++);

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertErcPathchkSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "erc_pathchk ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    while(iter != pNode->end())
    {
        if(iter->eType == SECONDARY_KEYWORD)
        {
            switch(iter->eKeyType)
            {
                case LABELED:
                {
                    outputText("-labeled_flag", "labeled", nOption++);
                    ++iter;
                    continue;
                }
                case POWER:
                {
                    outputText("-power_flag", "power", nOption++);
                    ++iter;
                    continue;
                }
                case GROUND:
                {
                    outputText("-ground_flag", "ground", nOption++);
                    ++iter;
                    continue;
                }
                case POLYGONS:
                case NETS:
                {
                    outputCon2Error(pNode, *iter);
                    
                    ++iter;
                    continue;
                }
                case BY_LAYER:
                {
                    outputCon2Error(pNode, *iter);
                    
                    ++iter;
                    continue;
                }
                case BY_CELL:
                {
                    outputCon2Error(pNode, *iter);
                    
                    ++iter;
                    continue;
                }
                case FLAT:
                {
                    outputCon2Error(pNode, *iter);
                    
                    ++iter;
                    continue;
                }
                case PORTS_ALSO:
                {
                    outputText("-port_also", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case NOFLOAT:
                {
                    outputText("-no_float", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case EXCLUDE_UNUSED:
                {
                    outputText("-exclude_unused", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case EXCLUDE_SUPPLY:
                {
                    outputText("-exclude_supply", NULL, nOption++);
                    ++iter;
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case OPERATOR:
            {
                if(iter->sValue == "+" || iter->sValue == "-")
                {
                    std::string sLayerName;
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                        outputText("-in_layer", sLayerName.c_str(), nOption++);
                    continue;
                }
                else if("!" == iter->sValue)
                {
                    ++iter;
                    if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD)
                    {
                        if(iter->eKeyType == LABELED)
                        {
                            outputText("-labeled_flag", "!labeled", nOption++);
                        }
                        else if(iter->eKeyType == POWER)
                        {
                            outputText("-power_flag", "!power", nOption++);
                        }
                        else if(iter->eKeyType == GROUND)
                        {
                            outputText("-ground_flag", "!ground", nOption++);
                        }
                        else
                        {
                            --iter;
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo,
                                                iter->sValue);
                        }
                    }
                    break;
                }
                else if( "&&" == iter->sValue)
                {
                    outputText("-operand", "and", nOption++);
                    break;
                }
                else if( "||" == iter->sValue)
                {
                    outputText("-operand", "or", nOption++);
                    break;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo,
                                    iter->sValue);
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertReduceGenericSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "reduce_generic ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sComponentType;
    if(!getStringName(iter, pNode->end(), sComponentType))
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);

    ++iter;
    if(iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
    {
        sComponentType += "( ";
        ++iter;
        std::string sSubType;

        if(!getStringName(iter, pNode->end(), sSubType))
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        sComponentType += sSubType;
        ++iter;
        sComponentType += " )";
        ++iter;
    }
    outputText("-component_type", sComponentType.c_str(), nOption++);

    if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == PARALLEL_ONLY)
    {
        outputText("-mode", "parallel", nOption++);
        ++iter;
    }
    else if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SERIES)
    {
        outputText("-mode", "series", nOption++);
        ++iter;
        std::string sPinName1;
        if(getStringName(iter, pNode->end(), sPinName1))
        {
            outputText("-series_pin_name1", sPinName1.c_str(), nOption++);
            ++iter;
            std::string sPinName2;
            if(getStringName(iter, pNode->end(), sPinName2))
                outputText("-series_pin_name2", sPinName2.c_str(), nOption++);
            ++iter;
        }
    }

    while(iter != pNode->end())
    {
        if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == YES)
        {
            outputText("-action", "yes", nOption++);
        }
        else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == CELL)
        {
            ++iter;
            if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == LIST)
            {
                ++iter;
                std::string sCellListName;
                if(getStringName(iter, pNode->end(), sCellListName))
                    outputText("-cell_list", sCellListName.c_str(), nOption++);
                ++iter;
            }
            continue;
        }
        else if(iter->eType == BUILT_IN_LANG)
        {
            std::string sValue = iter->sValue;
            sValue.erase(0, 1);
            sValue.insert(0, "{ ");
            sValue.erase(sValue.size() - 1);
            sValue.insert(sValue.size(), " }");
            outputText("-reduce_program", sValue.c_str(), nOption++);
        }
        else if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == NO)
        {
            outputText("-action", "no", nOption++);
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertIdentifyToleranceSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "identify_gates_tolerance ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sComponentType;
    if(!getStringName(iter, pNode->end(), sComponentType))
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);

    ++iter;
    if(iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
    {
        sComponentType += "( ";
        ++iter;
        std::string sSubType;

        if(!getStringName(iter, pNode->end(), sSubType))
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        sComponentType += sSubType;
        ++iter;
        sComponentType += " )";
        ++iter;
    }
    outputText("-component_type", sComponentType.c_str(), nOption++);

    std::string sPropertyName;
    if(!getStringName(iter, pNode->end(), sPropertyName))
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);

    ++iter;
    if(iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
    {
        sPropertyName += "( ";
        ++iter;
        std::string sSpicePara;

        if(!getStringName(iter, pNode->end(), sSpicePara))
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        sPropertyName += sSpicePara;
        ++iter;
        sPropertyName += " )";
        ++iter;
    }
    outputText("-property_name", sPropertyName.c_str(), nOption++);

    bool hasSerieSTol   = false;
    bool hasParallelTol = false;
    while(iter != pNode->end())
    {
        if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == STRING)
        {
            ++iter;
            if(iter != pNode->end() && (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SERIES))
            {
                ++iter;
                if(iter != pNode->end() && (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == PARALLEL_ONLY))
                {
                    outputText("-property_as_string", "all", nOption++);
                    ++iter;
                }
                else
                {
                    outputText("-property_as_string", "series", nOption++);
                }
            }
            else if(iter != pNode->end() && (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == PARALLEL_ONLY))
            {
                ++iter;
                if(iter != pNode->end() && (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SERIES))
                {
                    outputText("-property_as_string", "all", nOption++);
                    ++iter;
                }
                else
                {
                    outputText("-property_as_string", "parallel", nOption++);
                }
            }
            else
            {
                outputText("-property_as_string", "all", nOption++);
            }
        }
        else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == PARALLEL_ONLY)
        {
            ++iter;
            if(iter != pNode->end())
            {
                std::string sTolerance;
                if(getNumberExp(iter, pNode->end(), sTolerance))
                    outputText("-parallel_tolerance", sTolerance.c_str(), nOption++);
                hasParallelTol = true;
            }
        }
        else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SERIES)
        {
            ++iter;
            if(iter != pNode->end())
            {
                std::string sTolerance;
                if(getNumberExp(iter, pNode->end(), sTolerance))
                    outputText("-series_tolerance", sTolerance.c_str(), nOption++);
                hasSerieSTol = true;
            }
        }
        else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == LAYOUT)
        {
            outputText("-design", "layout", nOption++);
            ++iter;
        }
        else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SOURCE)
        {
            outputText("-design", "source", nOption++);
            ++iter;
        }
        else if(!hasSerieSTol && !hasParallelTol)
        {
            std::string sTolerance;
            if(getNumberExp(iter, pNode->end(), sTolerance))
                outputText("-tolerance", sTolerance.c_str(), nOption++);

            hasSerieSTol   = true;
            hasParallelTol = true;
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertSplitGateRatioSpec(rcsSynSpecificationNode_T *pNode)
{
    if(_hasOnlyDirectKeyword(pNode))
    {
        outputCon1Error(pNode);
        return;
    }

    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "reduce_split_gate_ratio ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sComponentType;
    if(!getStringName(iter, pNode->end(), sComponentType))
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    outputText("-component_type", sComponentType.c_str(), nOption++);

    ++iter;
    std::string sPropertyName;
    if(!getStringName(iter, pNode->end(), sPropertyName))
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);

    ++iter;
    if(iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
    {
        sPropertyName += iter->sValue;
        ++iter;
        std::string sSpicePara;

        if(!getStringName(iter, pNode->end(), sSpicePara))
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        sPropertyName += sSpicePara;
        ++iter;
        sPropertyName += iter->sValue;
        ++iter;
    }
    outputText("-property_name", sPropertyName.c_str(), nOption++);

    std::string sTolerance;
    if(getNumberExp(iter, pNode->end(), sTolerance))
        outputText("-tolerance", sTolerance.c_str(), nOption++);

    while(iter != pNode->end())
    {
        if(iter->eType == SECONDARY_KEYWORD && (iter->eKeyType == MASK || iter->eKeyType == DIRECT))
        {
            outputCon2Error(pNode, *iter);
            ++iter;
            continue;
        }
        else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == LAYOUT)
        {
            outputText("-design", "layout", nOption++);
        }
        else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SOURCE)
        {
            outputText("-design", "source", nOption++);
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertReduceSpecificSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "reduce_specific ";

    switch(pNode->getSpecificationType())
    {
        case LVS_REDUCE_PARALLEL_BIPOLAR:
            outputText("-mode", "parallel", nOption++);
            outputText("-type", "q", nOption++);
            break;
        case LVS_REDUCE_PARALLEL_CAPACITORS:
            outputText("-mode", "parallel", nOption++);
            outputText("-type", "c", nOption++);
            break;
        case LVS_REDUCE_PARALLEL_DIODES:
            outputText("-mode", "parallel", nOption++);
            outputText("-type", "d", nOption++);
            break;
        case LVS_REDUCE_PARALLEL_MOS:
            outputText("-mode", "parallel", nOption++);
            outputText("-type", "m", nOption++);
            break;
        case LVS_REDUCE_PARALLEL_RESISTORS:
            outputText("-mode", "parallel", nOption++);
            outputText("-type", "r", nOption++);
            break;
        case LVS_REDUCE_SEMI_SERIES_MOS:
            outputText("-mode", "semi_series", nOption++);
            outputText("-type", "m", nOption++);
            break;
        case LVS_REDUCE_SERIES_CAPACITORS:
            outputText("-mode", "series", nOption++);
            outputText("-type", "c", nOption++);
            break;
        case LVS_REDUCE_SERIES_MOS:
            outputText("-mode", "series", nOption++);
            outputText("-type", "m", nOption++);
            break;
        case LVS_REDUCE_SERIES_RESISTORS:
            outputText("-mode", "series", nOption++);
            outputText("-type", "r", nOption++);
            break;
        default:
            break;
    }

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == YES)
    {
        outputText("-action", "yes", nOption++);
    }
    else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == NO)
    {
        outputText("-action", "no", nOption++);
    }

    ++iter;
    if(iter != pNode->end() && iter->eType == BUILT_IN_LANG)
    {
        std::string sValue = iter->sValue;
        sValue.erase(0, 1);
        sValue.insert(0, "{ ");
        sValue.erase(sValue.size() - 1);
        sValue.insert(sValue.size(), " }");
        outputText("-reduce_program", sValue.c_str(), nOption++);
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertReduceSplitGatesSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "reduce_split_gates ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == YES)
    {
        outputText("-action", "yes", nOption++);
    }
    else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == NO)
    {
        outputText("-action", "no", nOption++);
    }

    ++iter;
    for(; iter != pNode->end(); ++iter)
    {
        if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SEMI)
        {
            ++iter;
            if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == ALSO)
            {
                outputText("-semi", NULL, nOption++);
                continue;
            }
        }
        else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SAME)
        {
            ++iter;
            if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == ORDER)
            {
                outputText("-same_order", NULL, nOption++);
                continue;
            }
        }
        else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == WITHIN)
        {
            ++iter;
            if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == TOLERANCE)
            {
                outputText("-tolerance", NULL, nOption++);
                continue;
            }
        }
        else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == MIX)
        {
            ++iter;
            if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == TYPES)
            {
                outputText("-mixed_types", NULL, nOption++);
                continue;
            }
        }
        else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SP)
        {
            ++iter;
            if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == ALSO)
            {
                outputText("-parallel_series", NULL, nOption++);
                continue;
            }
        }
        else if(iter->eType == BUILT_IN_LANG)
        {
            std::string sValue = iter->sValue;
            sValue.erase(0, 1);
            sValue.insert(0, "{ ");
            sValue.erase(sValue.size() - 1);
            sValue.insert(sValue.size(), " }");
            outputText("-reduce_program", sValue.c_str(), nOption++);
            continue;
        }

        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertShortEquivalentNodesSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "lvs_short_equivalent_net ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == NO)
    {
        outputText("-action", "no", nOption++);
    }
    else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == PARALLEL)
    {
		outputText("-parallel", NULL, nOption++);
    }
    else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SPLIT)
    {
		outputText("-split", NULL, nOption++);
    }

    ++iter;
    for(; iter != pNode->end(); ++iter)
    {
        if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == WITHIN)
        {
            ++iter;
            if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == TOLERANCE)
			{
            	outputText("-in_tolerance", NULL, nOption++);
                continue;
			}
        }

        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertReducePreferSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "reduce_prefer ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if(strcasecmp(iter->sValue.c_str(), "PARALLEL") == 0)
    {
        outputText("-type", "parallel", nOption++);
    }
    else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SERIES)
    {
        outputText("-type", "series", nOption++);
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertLvsReportOptionSpec(rcsSynSpecificationNode_T *pNode)
{
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    bool hasSupportOption = false;
    for(; iter != pNode->end(); ++iter)
    {
        if(0 == strcasecmp(iter->sValue.c_str(), "A"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "AV"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "B"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "BV"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "C"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "CV"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "D"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "E"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "E1"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "F"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "G"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "H"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "I"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "LPE"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "MC"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "N"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "NOK"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "O"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "P"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "PG"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "R"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "RA"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "S"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "SP"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "SPE"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "V"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "W"))
        {
            hasSupportOption = true;
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "X"))
        {
            hasSupportOption = true;
        }
    }

    if(!hasSupportOption)
    {
        std::string sValue;
        pNode->getCmdValue(sValue);
        m_oLogStream << "This Operation or Specification in line " << pNode->begin()->nLineNo
                     << " can not be Converted correctly: " << std::endl << sValue << std::endl << std::endl;

        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::WARNING, CON1,
                              pNode->getNodeBeginLineNo(), pNode->begin()->sValue));
        return;
    }

    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "lvs_report_option ";

    iter = pNode->begin();
    ++iter;
    for(; iter != pNode->end(); ++iter)
    {
        if(0 == strcasecmp(iter->sValue.c_str(), "A"))
        {
            outputText("-net", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "AV"))
        {
            outputText("-net_supply", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "B"))
        {
            outputText("-shortopen", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "BV"))
        {
            outputText("-shortopen_supply", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "C"))
        {
            outputText("-missing_net", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "CV"))
        {
            outputText("-missing_net_supply", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "D"))
        {
            outputText("-missing_instance", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "E"))
        {
            outputText("-missing_properties", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "E1"))
        {
            outputText("-missing_properties_tol", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "F"))
        {
            outputText("-smashed_mos", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "FX"))
        {
            outputCon2Error(pNode, *iter);
            
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "G"))
        {
            outputText("-properties", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "H"))
        {
            outputText("-missing_cell", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "I"))
        {
            outputText("-isolated_net", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "LPE"))
        {
            outputText("-layout_passthrough", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "MC"))
        {
            outputText("-count_by_model", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "N"))
        {
            outputText("-missing_name", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "NOK"))
        {
            outputText("-corresponding_cell", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "O"))
        {
            outputText("-resolution_warning", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "P"))
        {
            outputText("-short_port", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "PG"))
        {
            outputText("-corresponding_cell_supple", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "R"))
        {
            outputText("-ambiguity", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "RA"))
        {
            outputText("-ambiguity_all", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "S"))
        {
            outputText("-show_virtual_connections", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "SP"))
        {
            outputText("-source_passthrough_warning", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "SPE"))
        {
            outputText("-source_passthrough", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "V"))
        {
            outputText("-show_sconnect", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "W"))
        {
            outputText("-same_data", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "X"))
        {
            outputText("-not_identical_pin", NULL, nOption++);
        }
        else if(0 == strcasecmp(iter->sValue.c_str(), "NONE"))
        {
            outputText("-not_set", NULL, nOption++);
        }
        else
        {
            std::string sValue;
            pNode->getCmdValue(sValue);
            m_oLogStream << "This Option " << iter->sValue << " in line " << iter->nLineNo
                         << " can not be Converted correctly in this operation: " << std::endl
                         << sValue << std::endl << std::endl;

            s_errManager.addError(rcErrorNode_T(rcErrorNode_T::WARNING, CON2,
                                                iter->nLineNo, iter->sValue));
        }
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertRenameSpiceParameterSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "rename_netlist ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sValue;
    if(getStringName(iter, pNode->end(), sValue))
        outputText("-old_name", sValue.c_str(), nOption++);

    ++iter;
    sValue.clear();
    if(getStringName(iter, pNode->end(), sValue))
        outputText("-new_name", sValue.c_str(), nOption++);

    ++iter;
    if(iter != pNode->end())
    {
        outputText("-option", NULL, nOption++);
    }

    while(iter != pNode->end())
    {
        if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SOURCE)
        {
            outputText("-design", "source", nOption++);
        }
        else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == LAYOUT)
        {
            outputText("-design", "layout", nOption++);
        }
        else
        {
            outputText(NULL, iter->sValue.c_str(), nOption++);
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertSpiceOptionSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    switch(pNode->getSpecificationType())
    {
        case LVS_SPICE_MULTIPLIER_NAME:
        {
            m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "spice_option -multiplier_name_list ";
            for(; iter != pNode->end(); ++iter)
            {
                std::string sValue;
                if(getStringName(iter, pNode->end(), sValue))
                    outputText(NULL, sValue.c_str(), nOption++);
            }
            break;
        }
        case LVS_SPICE_OPTION:
        {
            for(; iter != pNode->end(); ++iter)
            {
                std::string sValue = rcsManager_T::getInstance()->getPVRSCmdPrefix();
                sValue += "spice_option ";
                if(0 == strcasecmp(iter->sValue.c_str(), "F"))
                {
                    sValue += "-floating_pins";
                    outputText(sValue.c_str(), "warning", nOption);
                    m_oPVRSStream << std::endl;
                }
                else if(0 == strcasecmp(iter->sValue.c_str(), "S"))
                {
                    sValue += "-duplicate_name";
                    outputText(sValue.c_str(), "yes", nOption);
                    m_oPVRSStream << std::endl;
                }
                else if(0 == strcasecmp(iter->sValue.c_str(), "X"))
                {
                    sValue += "-keep_name";
                    outputText(sValue.c_str(), "yes", nOption);
                    m_oPVRSStream << std::endl;
                }
                else
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
            }
            break;
        }
        case LVS_SPICE_SCALE_X_PARAMETERS:
        {
            m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "spice_option ";
            if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == YES)
            {
                outputText("-x_scale", "yes", nOption++);
            }
            else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == NO)
            {
                outputText("-x_scale", "no", nOption++);
            }
            else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == MOS)
            {
                outputText("-x_scale", "mos", nOption++);
                ++iter;
                if(iter != pNode->end())
                {
                    if(iter->eType == SECONDARY_KEYWORD)
                    {
                        if(iter->eKeyType == YES)
                            outputText(NULL, "yes", nOption++);
                        else if(iter->eKeyType == NO)
                            outputText(NULL, "no", nOption++);
                        else
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                iter->nLineNo, iter->sValue);
                    }
                    else
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                            iter->nLineNo, iter->sValue);
                }
            }
            else
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }

            break;
        }
        default:
            Utassert(false);
            break;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertFilterSpec(rcsSynSpecificationNode_T *pNode)
{
    if(_hasOnlyDirectKeyword(pNode))
    {
        outputCon1Error(pNode);
        return;
    }

    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "filter ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sComponentType;
    if(!getStringName(iter, pNode->end(), sComponentType))
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);

    ++iter;
    if(iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
    {
        sComponentType += "( ";
        ++iter;
        std::string sSubTypes;

        if(!getStringName(iter, pNode->end(), sSubTypes))
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        sComponentType += sSubTypes;
        ++iter;
        sComponentType += " )";
        ++iter;
    }
    outputText("-component_type", sComponentType.c_str(), nOption++);

    std::string sPropertyName;
    if(iter != pNode->end() && (iter->eType != SECONDARY_KEYWORD ||
      (iter->eKeyType != SHORT && iter->eKeyType != OPEN &&
       iter->eKeyType != LAYOUT && iter->eKeyType != SOURCE &&
       iter->eKeyType != MASK && iter->eKeyType != DIRECT)))
    {
        if(!getStringName(iter, pNode->end(), sPropertyName))
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);

        ++iter;
        if(iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
        {
            sPropertyName += iter->sValue;
            ++iter;
            std::string sSpicePara;

            if(!getStringName(iter, pNode->end(), sSpicePara))
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            sPropertyName += sSpicePara;
            ++iter;
            sPropertyName += iter->sValue;
            ++iter;
        }
        outputText("-property_name", sPropertyName.c_str(), nOption++);

        if(iter != pNode->end() && iter->eType == OPERATOR)
        {
            std::string sFilterLimit;
            if(getConstraint(iter, pNode->end(), sFilterLimit))
                outputText("-limit", sFilterLimit.c_str(), nOption++);
        }
    }

    while(iter != pNode->end())
    {
        if(iter->eType == SECONDARY_KEYWORD &&
          (iter->eKeyType == DIRECT || iter->eKeyType == MASK))
        {
            outputCon2Error(pNode, *iter);
            ++iter;
            continue;
        }

        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                if(iter->eKeyType == SHORT)
                {
                    ++iter;
                    std::string sPinName;
                    while(iter != pNode->end() && (iter->eType != SECONDARY_KEYWORD ||
                         (iter->eKeyType != OPEN && iter->eKeyType != LAYOUT &&
                          iter->eKeyType != SOURCE && iter->eKeyType != SHORT &&
                          iter->eKeyType != DIRECT && iter->eKeyType != MASK)))
                    {
                        std::string sValue;
                        if(!getStringName(iter, pNode->end(), sValue))
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                iter->nLineNo, iter->sValue);
                        sPinName += sValue;
                        sPinName += " ";
                        ++iter;
                    }
                    outputText("-net", "short", nOption++);
                    if(!sPinName.empty())
                        outputText("-short_pin_list", sPinName.c_str(), nOption++);
                    continue;
                }
                else if(iter->eKeyType == OPEN)
                {
                    outputText("-net", "open", nOption++);
                    ++iter;
                    continue;
                }
                else if(iter->eKeyType == LAYOUT)
                {
                    outputText("-design", "layout", nOption++);
                    ++iter;
                    continue;
                }
                else if(iter->eKeyType == SOURCE)
                {
                    outputText("-design", "source", nOption++);
                    ++iter;
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertPushDeviceSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "push_device ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    if(pNode->getSpecificationType() == LVS_PUSH_DEVICES_SEPARATE_PROPERTIES)
    {
        ++iter;
        std::string sFileName;
        if(getStringName(iter, pNode->end(), sFileName))
            outputText("-with_property_separate_file", sFileName.c_str(), nOption++);
        return;
    }

    ++iter;
    while(iter != pNode->end())
    {
        if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == YES)
        {
            outputText(NULL, "yes", nOption++);
        }
        else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == NO)
        {
            outputText(NULL, "no", nOption++);
        }
        else
        {
            std::string sValue;
            pNode->getCmdValue(sValue);
            m_oLogStream << "This Option " << iter->sValue << " in line " << iter->nLineNo
                         << " can not be Converted correctly in this operation: " << std::endl
                         << sValue << std::endl << std::endl;

            s_errManager.addError(rcErrorNode_T(rcErrorNode_T::WARNING, CON2,
                                                iter->nLineNo, iter->sValue));
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertCheckPropertySpec(rcsSynSpecificationNode_T *pNode)
{
    if(_hasOnlyDirectKeyword(pNode))
    {
        outputCon1Error(pNode);
        return;
    }

    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "check_property ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sComponentType;
    if(!getStringName(iter, pNode->end(), sComponentType))
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);

    ++iter;
    if(iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
    {
        sComponentType += "( ";
        ++iter;
        std::string sSubTypes;

        if(!getStringName(iter, pNode->end(), sSubTypes))
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        sComponentType += sSubTypes;
        ++iter;
        sComponentType += " )";
        ++iter;
    }

    outputText("-component_type", sComponentType.c_str(), nOption++);

    if(iter != pNode->end() && iter->eType == BUILT_IN_LANG)
    {
        std::string sValue = iter->sValue;
        sValue.erase(0, 1);
        sValue.insert(0, "{ ");
        sValue.erase(sValue.size() - 1);
        sValue.insert(sValue.size(), " }");
        outputText("-check_program", sValue.c_str(), nOption++);

        if(!m_isInLayerDefinition)
            m_oPVRSStream << std::endl;
        return;
    }

    std::string sSourceProperty;
    if(!getStringName(iter, pNode->end(), sSourceProperty))
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);

    ++iter;
    if(iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
    {
        sSourceProperty += iter->sValue;
        ++iter;
        std::string sSpicePara;

        if(!getStringName(iter, pNode->end(), sSpicePara))
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        sSourceProperty += sSpicePara;
        ++iter;
        sSourceProperty += iter->sValue;
        ++iter;
    }

    outputText("-source", sSourceProperty.c_str(), nOption++);

    std::string sLayoutProperty;
    if(!getStringName(iter, pNode->end(), sLayoutProperty))
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);

    ++iter;
    if(iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
    {
        sLayoutProperty += iter->sValue;
        ++iter;
        std::string sSpicePara;

        if(!getStringName(iter, pNode->end(), sSpicePara))
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        sLayoutProperty += sSpicePara;
        ++iter;
        sLayoutProperty += iter->sValue;
        ++iter;
    }

    outputText("-layout", sLayoutProperty.c_str(), nOption++);

    bool hasTolerance1 = false;
    bool hasTolerance2 = false;
    while(iter != pNode->end())
    {
        if(iter->eType == SECONDARY_KEYWORD)
        {
            if(iter->eKeyType == STRING)
            {
                ++iter;
                if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == NO)
                {
                    outputText("-property_as_string", "no", nOption++);
                    ++iter;
                }
                else
                {
                    outputText("-property_as_string", NULL, nOption++);
                }
                continue;
            }
            else if(iter->eKeyType == NO)
            {
                outputText("-not_check_in_specified_cells", NULL, nOption++);
                ++iter;
                continue;
            }
            else if(iter->eKeyType == ABSOLUTE)
            {
                outputText("-tolerance_type", "absolute", nOption++);
                ++iter;
                continue;
            }
            else if(iter->eKeyType == CELL)
            {
                ++iter;
                if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == LIST)
                {
                    ++iter;
                    std::string sCellListName;
                    if(!getStringName(iter, pNode->end(), sCellListName))
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                            iter->nLineNo, iter->sValue);
                    outputText("-cell_list", sCellListName.c_str(), nOption++);
                    ++iter;
                }
                continue;
            }
            else if(iter->eKeyType == DIRECT || iter->eKeyType == MASK)
            {
                outputCon2Error(pNode, *iter);
                ++iter;
                continue;
            }
        }

        if(iter->eType == BUILT_IN_LANG)
        {
            std::string sValue = iter->sValue;
            sValue.erase(0, 1);
            sValue.insert(0, "{ ");
            sValue.erase(sValue.size() - 1);
            sValue.insert(sValue.size(), " }");
            outputText("-check_program", sValue.c_str(), nOption++);
            ++iter;
            continue;
        }
        else
        {
            if(hasTolerance1 && hasTolerance2)
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            std::string sNumExp;
            if(!getNumberExp(iter, pNode->end(), sNumExp))
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            if(iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
            {
                ++iter;
                std::string sTolProp;
                getStringName(iter, pNode->end(), sTolProp);
                sNumExp += " (";
                sNumExp += sTolProp;
                ++iter;
                sNumExp += ")";
                ++iter;
            }
            if(!hasTolerance1)
            {
                outputText("-tolerance1", sNumExp.c_str(), nOption++);
                hasTolerance1 = true;
            }
            else if(!hasTolerance2)
            {
                outputText("-tolerance2", sNumExp.c_str(), nOption++);
                hasTolerance2 = true;
            }
            continue;
        }
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertSourceDbSpec(rcsSynSpecificationNode_T *pNode)
{
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "source_db ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    if(iter->eType == SPECIFICATION_KEYWORD && iter->eKeyType == SOURCE_PATH)
    {
        ++iter;
        std::string sFilePath;
        if(getStringName(iter, pNode->end(), sFilePath))
            outputText("-path", sFilePath.c_str());
    }
    else if(iter->eType == SPECIFICATION_KEYWORD && iter->eKeyType == SOURCE_PRIMARY)
    {
        ++iter;
        if(iter != pNode->end())
        {
            std::string sTopCell;
            if(getStringName(iter, pNode->end(), sTopCell))
                outputText("-top_cell", sTopCell.c_str());
        }
    }
    else if(iter->eType == SPECIFICATION_KEYWORD && iter->eKeyType == SOURCE_SYSTEM)
    {
        ++iter;
        if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == CNET)
        {
            outputText("-format", "cnet");
        }
        else if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SPICE)
        {
            outputText("-format", "spice");
        }
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertIdentifyGatesSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "identify_gates ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    while(iter != pNode->end())
    {
        if(iter->eType == SECONDARY_KEYWORD)
        {
            switch(iter->eKeyType)
            {
                case ALL:
                    outputText("-type", "all", nOption++);
                    break;
                case SIMPLE:
                    outputText("-type", "simple", nOption++);
                    break;
                case NONE:
                    outputText("-type", "none", nOption++);
                    break;
                case XALSO:
                    outputText("-option", "x", nOption++);
                    break;
                case MIX:
                {
                    ++iter;
                    if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SUBTYPES)
                    {
                        outputText("-option", "diff_subtypes", nOption++);
                        break;
                    }
                    break;
                }
                case WITHIN:
                {
                    ++iter;
                    if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == TOLERANCE)
                    {
                        outputText("-option", "tolerance", nOption++);
                        break;
                    }
                    break;
                }
                case CELL:
                {
                    ++iter;
                    if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == LIST)
                    {
                        ++iter;
                        std::string sValue;
                        if(getStringName(iter, pNode->end(), sValue))
                            outputText("-cell_list", sValue.c_str(), nOption++);
                    }
                    break;
                }
                default:
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                    break;
            }
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertNonMatchNameSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    if(pNode->getSpecificationType() == LVS_NON_USER_NAME_INSTANCE)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "non_match_instance_name ";
    else if(pNode->getSpecificationType() == LVS_NON_USER_NAME_NET)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "non_match_net_name ";
    else if(pNode->getSpecificationType() == LVS_NON_USER_NAME_PORT)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "non_match_port_name ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    while(iter != pNode->end())
    {
        std::string sValue;
        if(getStringName(iter, pNode->end(), sValue))
            outputText("-pattern", sValue.c_str(), nOption++);
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertCompareCaseSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "compare_case ";
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    outputText("-option", NULL, nOption++);
    if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == YES)
    {
        outputText(NULL, "all", nOption++);
    }
    else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == NO)
    {
        outputText(NULL, "no", nOption++);
    }
    else
    {
        while(iter != pNode->end())
        {
            switch(iter->eType)
            {
                case SECONDARY_KEYWORD:
                {
                    switch(iter->eKeyType)
                    {
                        case NAMES:
                            outputText(NULL, "names", nOption++);
                            ++iter;
                            continue;
                        case TYPES:
                            outputText(NULL, "types", nOption++);
                            ++iter;
                            continue;
                        case SUBTYPES:
                            outputText(NULL, "subtypes", nOption++);
                            ++iter;
                            continue;
                        case VALUES:
                            outputText(NULL, "values", nOption++);
                            ++iter;
                            continue;
                        default:
                            break;
                    }
                }
                default:
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                    break;
                }
            }
            ++iter;
        }
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertSumFileSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    if(pNode->getSpecificationType() == DRC_SUMMARY_REPORT)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "drc_output_option ";
    else if(pNode->getSpecificationType() == ERC_SUMMARY_REPORT)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "erc_output_option ";
    else if(pNode->getSpecificationType() == DFM_SUMMARY_REPORT)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "dfm_output_option ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    std::string sFileName;
    if(getStringName(iter, pNode->end(), sFileName))
    {
        outputText("-sum_file", sFileName.c_str(), nOption++);
    }

    ++iter;
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                if(REPLACE == iter->eKeyType || APPEND == iter->eKeyType ||
                   HIER == iter->eKeyType)
                {
                    switch(iter->eKeyType)
                    {
                        case APPEND:
                            outputText("-mode", "append", nOption++);
                            break;
                        case HIER:
                            outputText(NULL, "hier", nOption);
                            break;
                        case REPLACE:
                            outputText("-mode", "replace", nOption++);
                            break;
                        default:
                            break;
                    }
                    break;
                }
            }
            default:
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
            }
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertDatabaseSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    if(pNode->getSpecificationType() == DRC_RESULTS_DATABASE)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "drc_output_option ";
    else if(pNode->getSpecificationType() == ERC_RESULTS_DATABASE)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "erc_output_option ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sFileName;
    if(getStringName(iter, pNode->end(), sFileName))
    {
        outputText("-out_file", sFileName.c_str(), nOption++);
    }

    std::string sAppend;
    std::string sPrefix;
    ++iter;
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                switch(iter->eKeyType)
                {
                    case ASCII:
                    {
                        outputText("-format", "ascii", nOption++);
                        break;
                    }
                    case BINARY:
                    {
                        outputText("-format", "binary", nOption++);
                        break;
                    }
                    case OASIS:
                    {
                        outputText("-format", "oasis", nOption++);
                        break;
                    }
                    case GDSII:
                    {
                        outputText("-format", "gds", nOption++);
                        break;
                    }
                    case USER:
                    {
                        if(++iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                           iter->eKeyType == MERGED)
                        {
                            outputText("-pseudo_cell", "merged", nOption++);
                        }
                        else
                        {
                            outputText("-pseudo_cell", "user", nOption++);
                            --iter;
                        }
                        break;
                    }
                    case PSEUDO:
                    {
                        outputText("-pseudo_cell", "undo", nOption++);
                        break;
                    }
                    case PREFIX:
                    case APPEND:
                    {
                        if(iter->eKeyType == PREFIX)
                        {
                            ++iter;
                            if(getStringName(iter, pNode->end(), sPrefix))
                            {
                                outputText("-prefix", sPrefix.c_str(), nOption++);
                            }
                            break;
                         }

                         if(!sAppend.empty())
                         {
                             throw rcErrorNode_T(rcErrorNode_T::ERROR, KEY1,
                                                 iter->nLineNo, iter->sValue);
                         }

                         ++iter;
                         if(getStringName(iter, pNode->end(), sAppend))
                         {
                             outputText("-append", sAppend.c_str(), nOption++);
                         }
                         break;
                     }
                     case CBLOCK:
                     {
                         outputText("-cblock", NULL, nOption++);
                         break;
                     }
                     case STRICT:
                     {
                         outputText("-strict", NULL, nOption++);
                         break;
                     }
                     default:
                     {
                         if(!sAppend.empty() || !isValidSvrfName(iter->sValue))
                         {
                             throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                 iter->nLineNo, iter->sValue);
                         }

                         if(getStringName(iter, pNode->end(), sAppend))
                         {
                             outputText("-append", sAppend.c_str(), nOption++);
                         }
                         break;
                     }
                 }
                 break;
             }
             case IDENTIFIER_NAME:
             {
                 if(!sAppend.empty() || !isValidSvrfName(iter->sValue))
                 {
                     throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                         iter->nLineNo, iter->sValue);
                     break;
                 }
             }
             case STRING_CONSTANTS:
             {
                 if(!sAppend.empty())
                     throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                         iter->nLineNo, iter->sValue);
                 if(getStringName(iter, pNode->end(), sAppend))
                 {
                     outputText("-append", sAppend.c_str(), nOption++);
                 }
                 break;
             }
             default:
                 throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                     iter->nLineNo, iter->sValue);
                 break;
         }
         ++iter;
     }

     if(!m_isInLayerDefinition)
         m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertAssignLayerSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "assign_layer ";

    std::list<rcsToken_T>::iterator iter  = pNode->begin();
    std::list<rcsToken_T>::iterator begin = pNode->begin();

    ++iter;
    std::string sLayerName;
    if(!getStringName(iter, pNode->end(), sLayerName))
        throw rcErrorNode_T(rcErrorNode_T::ERROR, OLS1, begin->nLineNo);

    outputText("-name", sLayerName.c_str(), nOption++);

    ++iter;
    outputText("-contain", NULL, nOption++);
    while(iter != pNode->end())
    {
        outputText(NULL, iter->sValue.c_str(), nOption);
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertMakeRuleGroupSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "make_rule_group ";

    std::list<rcsToken_T>::iterator iter  = pNode->begin();
    std::list<rcsToken_T>::iterator begin = pNode->begin();

    ++iter;
    std::string sGroupName;
    if(!getStringName(iter, pNode->end(), sGroupName))
        throw rcErrorNode_T(rcErrorNode_T::ERROR, OLS1, begin->nLineNo);

    outputText("-name", sGroupName.c_str(), nOption++);

    ++iter;
    outputText("-pattern", NULL, nOption++);
    while(iter != pNode->end())
    {
        std::string sValue;
        if(!getStringName(iter, pNode->end(), sValue))
        {
           sValue = iter->sValue;
        }

        outputText(NULL, sValue.c_str(), nOption);
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertSetSpec(rcsSynSpecificationNode_T *pNode)
{
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    Utassert(iter->eKeyType == VARIABLE);
    if(++iter == pNode->end())
    {
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::ERROR, VAR1,
                                            pNode->begin()->nLineNo));

        return;
    }

    m_oPVRSStream << "set ";
    std::string sVarName;
    if(!getStringName(iter, pNode->end(), sVarName, false))
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::ERROR, VAR1,
                                            pNode->begin()->nLineNo));

    toLower(sVarName);
    if(++iter == pNode->end())
    {
        --iter;
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::ERROR, VAR2,
                                            iter->nLineNo, iter->sValue));
        return;
    }

    if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == ENVIRONMENT)
    {
        m_oPVRSStream << sVarName << " $env(" << sVarName << ")" << std::endl;
        return;
    }

    bool hasOnlyString = true;
    std::list<rcsToken_T>::iterator i = iter;
    while(i != pNode->end())
    {
        if((i->eType != SECONDARY_KEYWORD || i->eKeyType != ENVIRONMENT) &&
           i->eType != IDENTIFIER_NAME && i->eType != STRING_CONSTANTS)
        {
            hasOnlyString = false;
            break;
        }
        ++i;
    }

    hvUInt32 iSize = std::distance(iter, pNode->end());
    if(iSize > 1)
    {
        if(hasOnlyString)
        {
            m_oPVRSStream << sVarName << " { ";
            while(iter != pNode->end())
            {
                std::string sValue;
                if(iter->eType == STRING_CONSTANTS)
                    getStringName(iter, pNode->end(), sValue, false);
                else
                    sValue = iter->sValue;
                m_oPVRSStream << sValue << " ";
                ++iter;
            }
            m_oPVRSStream << "}";
        }
        else
        {
            std::string sNumExp;
            if(getNumberExp(iter, pNode->end(), sNumExp, false))
                outputText(sVarName.c_str(), sNumExp.c_str());
        }
    }
    else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == ENVIRONMENT)
    {
        m_oPVRSStream << sVarName << " env( " << sVarName << " )";
    }
    else if(m_vVariableNames.find(iter->sValue) != m_vVariableNames.end())
    {
        std::string sValue;
        if(!getStringName(iter, pNode->end(), sValue))
             sValue = iter->sValue;
        toLower(sValue);
        sValue .insert(0, "$");
        outputText(sVarName.c_str(), sValue.c_str());
    }
    else
    {
        std::string sValue;
        if(iter->eType == STRING_CONSTANTS)
            getStringName(iter, pNode->end(), sValue, false);
        else
            sValue = iter->sValue;
        m_oPVRSStream << sVarName << " { " << sValue << " }";
    }

    m_vVariableNames.insert(sVarName);

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertRuleTextSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    if(pNode->getSpecificationType() == DRC_CHECK_TEXT)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "drc_output_option -rule_text ";
    else
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "erc_output_option -rule_text ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    for(; iter != pNode->end(); ++iter)
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                switch(iter->eKeyType)
                {
                    case ALL:
                    {
                        outputText(NULL, "all", nOption);
                        break;
                    }
                    case COMMENTS:
                    {
                        outputText(NULL, "comments", nOption);
                        break;
                    }
                    case RFI:
                    {
                        outputText(NULL, "rfi", nOption);
                        break;
                    }
                    case NONE:
                    {
                        outputText(NULL, "none", nOption);
                        break;
                    }
                    default:
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                            iter->nLineNo, iter->sValue);
                }
                break;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
        }
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertCellNameSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    if(pNode->getSpecificationType() == DRC_CELL_NAME)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "drc_output_option ";
    else
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "erc_output_option ";

    bool isCellName = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if(SECONDARY_KEYWORD == iter->eType && (iter->eKeyType == YES || iter->eKeyType == NO))
    {
        if(iter->eKeyType == YES)
            outputText("-cell_name", "yes", nOption);
        else
            outputText("-cell_name", "no", nOption);

        isCellName = iter->eKeyType == YES;
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    ++iter;
    if(isCellName)
    {
        for(; iter != pNode->end(); ++iter)
        {
            switch(iter->eType)
            {
                case SECONDARY_KEYWORD:
                {
                    switch(iter->eKeyType)
                    {
                        case XFORM:
                        {
                            outputText("-append", "trans", nOption);
                            break;
                        }
                        case CELL_SPACE:
                        {
                            outputText("-coord_space", "cell", nOption);
                            break;
                        }
                        case ALL:
                        {
                            outputText("-append", "all", nOption);
                            break;
                        }
                        default:
                        {
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                iter->nLineNo, iter->sValue);
                            break;
                        }
                    }
                    break;
                }
                default:
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                    break;
                }
            }
        }
    }
    else if(iter != pNode->end())
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomTopoCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_select_by_topo ";

    switch(pNode->getOperationType())
    {
        case INSIDE:
            outputText("-type", "inside", nOption++);
            break;
        case NOT_INSIDE:
            outputText("-type", "not_inside", nOption++);
            break;
        case OUTSIDE:
            outputText("-type", "outside", nOption++);
            break;
        case NOT_OUTSIDE:
            outputText("-type", "not_outside", nOption++);
            break;
        case TOUCH:
            outputText("-type", "touch", nOption++);
            break;
        case NOT_TOUCH:
            outputText("-type", "not_touch", nOption++);
            break;
        case ENCLOSE:
            outputText("-type", "enclose", nOption++);
            break;
        case NOT_ENCLOSE:
            outputText("-type", "not_enclose", nOption++);
            break;
        case CUT:
            outputText("-type", "cut", nOption++);
            break;
        case NOT_CUT:
            outputText("-type", "not_cut", nOption++);
            break;
        case INTERACT:
            outputText("-type", "overlap", nOption++);
            break;
        case NOT_INTERACT:
            outputText("-type", "not_overlap", nOption++);
            break;
        default:
            break;
    }

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if(BY_NET == iter->eKeyType)
                {
                    outputText("-by_net", NULL, nOption++);
                    break;
                }
                else if(EVEN == iter->eKeyType)
                {
                    outputText("-odd_even_limit", "even", nOption++);
                    break;
                }
                else if(ODD == iter->eKeyType)
                {
                    outputText("-odd_even_limit", "odd", nOption++);
                    break;
                }
                else if((SINGULAR_ONLY == iter->eKeyType || SINGULAR_ALSO == iter->eKeyType)
                        && (pNode->getOperationType() == INTERACT ||
                            pNode->getOperationType() == NOT_INTERACT))
                {
                    if(SINGULAR_ONLY == iter->eKeyType)
                    {
                        outputText("-point_touch", "only", nOption++);
                    }
                    else
                    {
                        outputText("-point_touch", "also", nOption++);
                    }
                    break;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
                else if(iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    outputText("-limit", sConstraint.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertExecProcCmd(rcsSynLayerOperationNode_T *pNode)
{
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if(!m_isInLayerDefinition)
        m_oPVRSStream << "eval ";

    std::string sMacroName;
    if(iter != pNode->end())
    {
        getStringName(iter, pNode->end(), sMacroName);
        toLower(sMacroName);
        if(m_iProcLevel)
            m_oPVRSStream << "\\[ " << sMacroName << " ";
        else
            m_oPVRSStream << "[ " << sMacroName << " ";

        sMacroName = iter->sValue;
        toLower(sMacroName);
    }

    if(m_vProcUsedTimes.find(sMacroName) == m_vProcUsedTimes.end())
        m_vProcUsedTimes[sMacroName] = 0;
    ++m_vProcUsedTimes[sMacroName];

    ++iter;
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            {
                std::string sValue;
                if(getStringName(iter, pNode->end(), sValue))
                {
                    m_oPVRSStream << "{" << sValue << "} ";
                }
                break;
            }
            default:
                m_oPVRSStream << iter->sValue << " ";
                break;
        }
        ++iter;
    }

    if(m_iProcLevel)
        m_oPVRSStream << "[expr (" << m_vProcUsedTimes[sMacroName] << " + $index) ]";
    else
        m_oPVRSStream << m_vProcUsedTimes[sMacroName];

    if(m_iProcLevel)
        m_oPVRSStream << "\\]";
    else
        m_oPVRSStream << "]";

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

bool
rcsSynNodeConvertor_T::_hasOnlyDirectKeyword(rcsSynNode_T *pNode)
{
    bool hasDirect = false;
    bool hasMask   = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
       if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == DIRECT)
           hasDirect = true;
       else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == MASK)
           hasMask = true;

        ++iter;
    }

    if(hasDirect && !hasMask)
        return true;

    return false;
}

void
rcsSynNodeConvertor_T::convertAttachCmd(rcsSynLayerOperationNode_T *pNode)
{
    if(_hasOnlyDirectKeyword(pNode))
    {
        outputCon1Error(pNode);
        return;
    }

    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "attach ";

    bool hasTextLayer = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if(iter->eKeyType == MASK || iter->eKeyType == DIRECT)
                {
                    outputCon2Error(pNode, *iter);
                    break;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(!hasTextLayer)
                {
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        hasTextLayer = true;
                        outputText("-text_layer", sLayerName.c_str(), nOption++);
                        continue;
                    }
                }
                else if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertConnectCmd(rcsSynLayerOperationNode_T *pNode)
{
    if(_hasOnlyDirectKeyword(pNode))
    {
        outputCon1Error(pNode);
        return;
    }

    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "connect ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if(MASK == iter->eKeyType)
                {
                    outputCon2Error(pNode, *iter);
                    break;
                }
                else if(DIRECT == iter->eKeyType)
                {
                    outputCon2Error(pNode, *iter);
                    break;
                }
                if(iter->eKeyType == BY)
                {
                    ++iter;
                    std::string sLayerName;
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText("-contact_layer", sLayerName.c_str(), nOption++);
                        continue;
                    }
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertSconnectCmd(rcsSynLayerOperationNode_T *pNode)
{
    if(_hasOnlyDirectKeyword(pNode))
    {
        outputCon1Error(pNode);
        return;
    }

    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "sconnect ";

    bool hasUpperLayer = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if(MASK == iter->eKeyType)
                {
                    outputCon2Error(pNode, *iter);
                    break;
                }
                else if(DIRECT == iter->eKeyType)
                {
                    outputCon2Error(pNode, *iter);
                    break;
                }
                else if(BY == iter->eKeyType)
                {
                    ++iter;
                    std::string sLayerName;
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText("-contact_layer", sLayerName.c_str(), nOption++);
                        continue;
                    }
                }
                else if(LINK == iter->eKeyType)
                {
                    ++iter;
                    outputText("-link", iter->sValue.c_str(), nOption++);
                    break;
                }
                else if(ABUT_ALSO == iter->eKeyType)
                {
                    outputText("-abut", NULL, nOption++);
                    break;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    if(hasUpperLayer)
                    {
                        outputText("-lower_layer", sLayerName.c_str(), nOption++);
                    }
                    else
                    {
                        outputText("-upper_layer", sLayerName.c_str(), nOption++);
                        hasUpperLayer = true;
                    }
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertDisconnectCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "clean_connect ";

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertAttachOrderCmd(rcsSynLayerOperationNode_T *pNode)
{
    if(_hasOnlyDirectKeyword(pNode))
    {
        outputCon1Error(pNode);
        return;
    }

    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "attach_order ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if(MASK == iter->eKeyType)
                {
                    outputCon2Error(pNode, *iter);
                    break;
                }
                else if(DIRECT == iter->eKeyType)
                {
                    outputCon2Error(pNode, *iter);
                    break;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomSelectDeviceLayerCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_select_device_layer ";

    bool hasElementName = false;
    bool hasModelName   = false;
    std::list<rcsToken_T>::iterator iterElement;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if(iter->eKeyType == EXCLUDE_UNUSED)
                {
                    outputText("-unused_device", "exclude", nOption++);
                    break;
                }
                else if(iter->eKeyType == PROPERTY)
                {
                    outputCon2Error(pNode, *iter);
                    break;
                }
                else if(iter->eKeyType == ANNOTATE)
                {
                    outputCon2Error(pNode, *iter);
                    return;
                }
            }
            case IDENTIFIER_NAME:
            {
                if(hasElementName)
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo,
                                        iter->sValue);
                }
                else if(!isValidSvrfName(iter->sValue))
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, DVL1, iter->nLineNo,
                                        "device layer");
                }
            }
            case STRING_CONSTANTS:
            {
                if(hasElementName)
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo,
                                        iter->sValue);
                }
                std::string sElementName;
                sElementName = iter->sValue;
                if(iter->eType == STRING_CONSTANTS)
                {
                    sElementName.insert(0, "\\\"");
                    sElementName.insert(sElementName.size(), "\\\"");
                }

                outputText("-element_name", sElementName.c_str(), nOption++);
                hasElementName = true;
                iterElement    = iter;
                break;
            }
            case SEPARATOR:
            {
                if(iter->sValue == "(" && !hasModelName && hasElementName)
                {
                    ++iter;
                    switch(iter->eType)
                    {
                        case SECONDARY_KEYWORD:
                        case IDENTIFIER_NAME:
                        {
                            if(!isValidSvrfName(iter->sValue))
                            {
                                throw rcErrorNode_T(rcErrorNode_T::ERROR, DVL2,
                                                    iterElement->nLineNo,
                                                    iterElement->sValue);
                            }
                        }
                        case STRING_CONSTANTS:
                        {
                            std::string sModelName;
                            sModelName = iter->sValue;
                            if(iter->eType == STRING_CONSTANTS)
                            {
                                sModelName.insert(0, "\\\"");
                                sModelName.insert(sModelName.size(), "\\\"");
                            }
                            sModelName.insert(0, "(");
                            sModelName.insert(sModelName.size(), ")");
                            outputText("-model_name", sModelName.c_str(), nOption++);
                            break;
                        }
                        default:
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, DVL2,
                                                iterElement->nLineNo,
                                                iterElement->sValue);
                    }
                    ++iter;
                    Utassert(iter->sValue == ")");
                    break;
                }
            }
            default:
            {
                if(!hasElementName)
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, DVL1, iter->nLineNo,
                                        "device layer");
                }
                else
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo,
                                        iter->sValue);
                }
                break;
            }
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomGetNetByAreaCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_get_net_by_area ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            {
                if(!isValidSvrfName(iter->sValue))
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
            }
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                }
                else if(isConstraintOperator(*iter))
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    outputText("-area_limit", sConstraint.c_str(), nOption++);
                }
                continue;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

bool
rcsSynNodeConvertor_T::parseNetAreaRatioLayer(std::list<rcsToken_T>::iterator &iter,
                                              std::list<rcsToken_T>::iterator end,
                                              hvUInt32 &nOption)
{
    std::string sLayerName;
    if(getLayerRefName(iter, end, sLayerName))
    {
        outputText("-in_layer", sLayerName.c_str(), nOption++);
    }
    else
    {
        return false;
    }

    if(iter != end)
    {
        while(iter != end)
        {
            if(iter->eType == SECONDARY_KEYWORD)
            {
                if(iter->eKeyType == SCALE)
                {
                    if(++iter != end && iter->eKeyType == BY)
                    {
                        ++iter;
                        std::string sNumExp;
                        if(getNumberExp(iter, end, sNumExp))
                        {
                            outputText("-scale_by", sNumExp.c_str(), nOption++);
                        }
                        continue;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                }
                else if(iter->eKeyType == COUNT)
                {
                    if(++iter != end && iter->eKeyType == ONLY)
                    {
                        outputText("-func_type count", NULL, nOption++);
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                }
                else if(iter->eKeyType == PERIMETER_ONLY)
                {
                    outputText("-func_type perim", NULL, nOption++);
                }
                else
                    break;
            }
            else
                break;
            ++iter;
        }

    }
    return true;
}

void
rcsSynNodeConvertor_T::parseNARRDB(std::list<rcsToken_T>::iterator &iter,
                                   std::list<rcsToken_T>::iterator end,
                                   hvUInt32 &nOption)
{
    bool isOnly = false;
    std::string sErrValue = iter->sValue;
    if(++iter != end && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == ONLY)
    {
        isOnly = true;
        sErrValue += " ";
        sErrValue += iter->sValue;
        ++iter;
    }

    if(iter != end)
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            {
                if(!isValidSvrfName(iter->sValue))
                {
                    --iter;
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP4, iter->nLineNo,
                                        sErrValue);
                }
            }
            case STRING_CONSTANTS:
            {
                std::string sFileName;
                if(getStringName(iter, end, sFileName))
                {
                    if(!isOnly)
                        outputText("-db_file", sFileName.c_str(), nOption++);
                    else
                        outputText("-only_db_file", sFileName.c_str(), nOption++);
                }
                break;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP4, iter->nLineNo,
                                    sErrValue);
        }
    }
    else
    {
        --iter;
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP4, iter->nLineNo, sErrValue);
    }

    ++iter;
    while(iter != end)
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                return;
            case SECONDARY_KEYWORD:
            {
                if(iter->eKeyType == BY_LAYER)
                {
                    outputText("-by_layer", NULL, nOption++);
                    break;
                }
                else if(iter->eKeyType == MAG)
                {
                    ++iter;
                    std::string sMagValue;
                    if(getNumberExp(iter, end, sMagValue))
                        outputText("-mag", sMagValue.c_str(), nOption++);
                    continue;
                }
                else if(ACCUMULATE == iter->eKeyType || OVER == iter->eKeyType)
                {
                    return;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, end, sLayerName))
                {
                    outputText(NULL, sLayerName.c_str(), nOption++);
                    if(iter != end && iter->eType == SECONDARY_KEYWORD &&
                       iter->eKeyType == MAXIMUM)
                    {
                        if(++iter != end)
                        {
                            std::string sNumExp;
                            if(getNumberExp(iter, end, sNumExp))
                                outputText("-maximum", sNumExp.c_str(), nOption++);
                        }
                        else
                        {
                            --iter;
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                iter->nLineNo, iter->sValue);
                        }
                    }
                    continue;
                }
            }
            case BUILT_IN_LANG:
                return;
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo,
                                    iter->sValue);
        }
        ++iter;
    }
}

static bool
isNARKeyword(const rcsToken_T &token)
{
    if(token.eType == SECONDARY_KEYWORD)
    {
        switch(token.eKeyType)
        {
            case SCALE:
            case PERIMETER_ONLY:
            case INSIDE_OF_LAYER:
            case OVER:
            case RDB:
            case ACCUMULATE:
            case BY_LAYER:
            case COUNT:
                return true;
            default:
                return false;
        }
    }
    else if(token.eType == LAYER_OPERATION_KEYWORD || token.eType == BUILT_IN_LANG)
    {
        return true;
    }
    return false;
}

void
rcsSynNodeConvertor_T::convertNetAreaRatioRDBCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "net_area_ratio_rdb ";

    bool hasInLayer = false;
    bool hasFile    = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            {
                if(!isValidSvrfName(iter->sValue))
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo,
                                        iter->sValue);
                }
            }
            case STRING_CONSTANTS:
            {
                if(!hasInLayer)
                {
                    std::string sLayerName;
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                        outputText("-in_layer", sLayerName.c_str(), nOption++);
                    hasInLayer = true;
                    continue;
                }
                else if(!hasFile)
                {
                    hasFile = true;
                    std::string sFileName;
                    if(getStringName(iter, pNode->end(), sFileName))
                    {
                        outputText("-file_name", sFileName.c_str(), nOption++);
                    }
                }
                else
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo,
                                        iter->sValue);
                }
                break;
            }
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                    outputText("-in_layer", sLayerName.c_str(), nOption++);
                hasInLayer = true;
                continue;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo,
                                    iter->sValue);
        }
        ++iter;
    }
}

void
rcsSynNodeConvertor_T::convertCheckNetAreaRatioByAccumulateCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "check_net_area_ratio_by_accumulate ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText("-in_layer", sLayerName.c_str(), nOption++);
                }
                else if(iter->eType == OPERATOR && isConstraintOperator(*iter))
                {
                    std::string sCons;
                    if(getConstraint(iter, pNode->end(), sCons))
                    {
                        outputText("-ratio_limit", sCons.c_str(), nOption++);
                    }
                }
                continue;
            }
            case BUILT_IN_LANG:
            {
                std::string sValue = iter->sValue;
                sValue.erase(0, 1);
                sValue.insert(0, "{ ");
                sValue.erase(sValue.size() - 1);
                sValue.insert(sValue.size(), " }");
                outputText("-exp", sValue.c_str(), nOption++);
                break;
            }
            default:
                break;
        }
        ++iter;
    }

}

void
rcsSynNodeConvertor_T::convertCheckNetAreaRatioCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "check_net_area_ratio ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(iter->eType == SECONDARY_KEYWORD)
        {
            switch(iter->eKeyType)
            {
                case OVER:
                {
                    outputText("-over", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case BY_NET:
                {
                    outputText("-by_net", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case INSIDE_OF_LAYER:
                {
                    ++iter;
                    std::string sLayerName;
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText("-inside_layer", sLayerName.c_str(), nOption++);
                    }
                    continue;
                }
                case ACCUMULATE:
                {
                    outputText("-accumulate", NULL, nOption++);
                    ++iter;
                    while(iter != pNode->end())
                    {
                        std::string sLayerName;
                        if(isNARKeyword(*iter))
                        {
                            break;
                        }
                        else if(getLayerRefName(iter, pNode->end(), sLayerName))
                        {
                            outputText(NULL, sLayerName.c_str(), nOption++);
                        }
                        else
                            break;
                    }
                    continue;
                }
                case RDB:
                    parseNARRDB(iter, pNode->end(), nOption);
                    continue;
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case BUILT_IN_LANG:
            {
                std::string sValue = iter->sValue;
                sValue.erase(0, 1);
                sValue.insert(0, "{ ");
                sValue.erase(sValue.size() - 1);
                sValue.insert(sValue.size(), " }");
                outputText("-exp", sValue.c_str(), nOption++);
                break;
            }
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if(parseNetAreaRatioLayer(iter, pNode->end(), nOption))
                {
                    continue;
                }
                else if(iter->eType == OPERATOR && isConstraintOperator(*iter))
                {
                    std::string sCons;
                    if(getConstraint(iter, pNode->end(), sCons))
                    {
                        outputText("-ratio_limit", sCons.c_str(), nOption++);
                    }
                }
                continue;
            }
            default:
                break;
        }
        ++iter;
    }
}

void
rcsSynNodeConvertor_T::convertBuildDfmPropertyCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "build_dfm_property ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case INTERSECTING_ONLY:
                {
                    outputText("-intersect", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case OVERLAP:
                {
                    outputText("-overlap", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case ABUT_ALSO:
                {
                    outputText("-abut", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case SINGULAR:
                {
                    outputText("-point_touch", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case MULTI:
                {
                    outputText("-multi", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case NOMULTI:
                {
                    outputText("-no_multi", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case NOPUSH:
                {
                	outputText("-no_push", NULL, nOption++);
                	++iter;
                	continue;
                }
                case REGION:
                {
                    outputText("-error_as_region", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case NODAL:
                {
                    outputText("-nodal", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case CORNER:
                {
                    outputText("-corner", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case GLOBALXY:
                {
                	outputText("-globalxy", NULL, nOption++);
                	++iter;
                	continue;
                }
                case CONNECTED:
                {
                    outputText("-same_net", "yes", nOption++);
                    ++iter;
                    continue;
                }
                case NOT_CONNECTED:
                {
                    outputText("-same_net", "no", nOption++);
                    ++iter;
                    continue;
                }
                case INSIDE_OF:
                {
                    std::string sLeftX;
                    std::string sBottomY;
                    std::string sRightX;
                    std::string sTopY;

                    ++iter;
                    getNumberExp(iter, pNode->end(), sLeftX, true);
                    getNumberExp(iter, pNode->end(), sBottomY, true);
                    getNumberExp(iter, pNode->end(), sRightX, true);
                    getNumberExp(iter, pNode->end(), sTopY, true);

                    outputText("-inside_box", sLeftX.c_str(), nOption);
                    outputText(NULL, sBottomY.c_str(), nOption);
                    outputText(NULL, sRightX.c_str(), nOption);
                    outputText(NULL, sTopY.c_str(), nOption++);
                    continue;
                }
                case SPLIT:
                {
                    ++iter;
                    if(iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                       ALL == iter->eKeyType)
                    {
                        outputText("-split", "all", nOption++);
                    }
                    else if(iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                            PRIMARY == iter->eKeyType)
                    {
                        outputText("-split", "primary", nOption++);
                    }
                    else
                    {
                        outputText("-split", NULL, nOption++);
                        continue;
                    }
                    ++iter;
                    continue;
                }
                case ACCUMULATE:
                {
                    ++iter;
                    if(iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                       ONLY == iter->eKeyType)
                    {
                        outputText("-accumulate", "only", nOption++);
                    }
                    else
                    {
                        outputText("-accumulate", NULL, nOption++);
                        continue;
                    }
                    ++iter;
                    continue;
                }
                case BY_NET:
                {
                    rcsToken_T key = *iter;
                    ++iter;
                    if(iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                       ONLY == iter->eKeyType)
                    {
                        outputText("-by_net", "only", nOption++);
                    }
                    else
                    {
                        outputText("-by_net", NULL, nOption++);
                        continue;
                    }
                    ++iter;
                    continue;
                }
                case NOHIER:
                {
                    outputText("-no_hier", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case NOPSEUDO:
                {
                    outputText("-no_pseudo", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case BY_CELL:
                {
                    rcsToken_T key = *iter;
                    ++iter;
                    if(iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                       ONLY == iter->eKeyType)
                    {
                        outputText("-by_cell", "only", nOption++);
                    }
                    else
                    {
                        outputText("-by_cell", NULL, nOption++);
                        continue;
                    }
                    ++iter;
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case BUILT_IN_LANG:
            {
                std::string sValue = iter->sValue;
                sValue.erase(0, 1);
                sValue.insert(0, "{ ");
                sValue.erase(sValue.size() - 1);
                sValue.insert(sValue.size(), " }");
                outputText("\\\n", NULL, nOption);
                outputText("-prop", sValue.c_str(), nOption);

                ++iter;
                if(iter != pNode->end() && iter->eType == OPERATOR &&
                  (iter->sValue == "!" || isConstraintOperator(*iter)))
                {
                    std::string sCons;
                    if(iter->sValue == "!")
                    {
                        sCons += "!";
                        ++iter;
                    }
                    if(getConstraint(iter, pNode->end(), sCons))
                    {
                        outputText("-prop_limit", sCons.c_str(), nOption++);
                    }
                }
                continue;
            }
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                    outputText("-in_layer", sLayerName.c_str(), nOption++);
                continue;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

static bool
_isOpcbiasKeyword(const rcsToken_T &token)
{
    if(token.eType == SECONDARY_KEYWORD)
    {
        switch(token.eKeyType)
        {
            case V1:
            case V2:
            case IGNORE:
            case MINBIASLENGTH:
            case CLOSEREDGE:
            case CLOSERSYMMETRIC:
            case GRID:
            case IMPLICITBIAS:
            case OPTION:
            case SPACE:
            case SPACELAYER:
            case ANGLED:
                return true;
            default:
                return false;
        }
    }
    else if(token.eType == LAYER_OPERATION_KEYWORD)
    {
        return true;
    }
    return false;
}

void
rcsSynNodeConvertor_T::convertOpcbiasRule(std::list<rcsToken_T>::iterator &iter,
                                          std::list<rcsToken_T>::iterator end,
                                          hvUInt32 &nOption)
{
    bool hasSpaceMetric   = false;
    bool hasWidthMetric   = false;
    bool hasWidth2Metric  = false;
    bool hasLength1Metric = false;
    bool hasLength2Metric = false;
    bool hasSpaceLayer    = false;
    while(iter != end)
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                switch(iter->eKeyType)
                {
                    case MOVE:
                    {
                        ++iter;
                        std::string sValue;
                        if(getNumberExp(iter, end, sValue))
                            outputText("-bias", sValue.c_str(), nOption++);
                        continue;
                    }
                    case SPACE:
                    {
                        if(hasSpaceMetric)
                            return;

                        hasSpaceMetric = true;
                        ++iter;
                        std::string sCons;
                        if(getConstraint(iter, end, sCons))
                            outputText("-ext", sCons.c_str(), nOption++);
                        continue;
                    }
                    case WIDTH:
                    {
                        if(hasWidthMetric)
                            return;

                        hasWidthMetric = true;
                        ++iter;
                        std::string sCons;
                        if(getConstraint(iter, end, sCons))
                            outputText("-int", sCons.c_str(), nOption++);
                        continue;
                    }
                    case WIDTH2:
                    {
                        if(hasWidth2Metric)
                            return;

                        hasWidth2Metric = true;
                        ++iter;
                        std::string sCons;
                        if(getConstraint(iter, end, sCons))
                            outputText("-int_opposite", sCons.c_str(), nOption++);
                        continue;
                    }
                    case LENGTH1:
                    {
                        if(hasLength1Metric)
                            return;

                        hasLength1Metric = true;
                        ++iter;
                        std::string sCons;
                        if(getConstraint(iter, end, sCons))
                            outputText("-len", sCons.c_str(), nOption++);
                        continue;
                    }
                    case LENGTH2:
                    {
                        if(hasLength2Metric)
                            return;

                        hasLength2Metric = true;
                        ++iter;
                        std::string sCons;
                        if(getConstraint(iter, end, sCons))
                            outputText("-len_opposite", sCons.c_str(), nOption++);
                        continue;
                    }
                    case SPACELAYER:
                    {
                        if(hasSpaceLayer)
                            return;

                        hasSpaceLayer = true;
                        ++iter;
                        std::string sLayerName;
                        if(getLayerRefName(iter, end, sLayerName))
                            outputText("-edge_layer", sLayerName.c_str(), nOption++);
                        continue;
                    }
                    case OPPOSITE:
                    {
                        outputText("-metric", "opposite", nOption++);
                        ++iter;
                        continue;
                    }
                    case OPPOSITE_SYMMETRIC:
                    {
                        outputText("-metric", "opposite_symmetric", nOption++);
                        ++iter;
                        continue;
                    }
                    case OPPOSITE_FSYMMETRIC:
                    {
                        outputText("-metric", "opposite_fsymmetric", nOption++);
                        ++iter;
                        continue;
                    }
                    case OPPOSITE_EXTENDED:
                    {
                        outputText("-metric", "opposite_extended", nOption++);
                        ++iter;
                        std::string sValue;
                        if(getNumberExp(iter, end, sValue))
                            outputText(NULL, sValue.c_str(), nOption++);
                        continue;
                    }
                    case OPPOSITE_EXTENDED_SYMMETRIC:
                    {
                        outputText("-metric", "opposite_extended_symmetric", nOption++);
                        ++iter;
                        std::string sValue;
                        if(getNumberExp(iter, end, sValue))
                            outputText(NULL, sValue.c_str(), nOption++);
                        continue;
                    }
                    case OPPOSITE_EXTENDED_FSYMMETRIC:
                    {
                        outputText("-metric", "opposite_extended_fsymmetric", nOption++);
                        ++iter;
                        std::string sValue;
                        if(getNumberExp(iter, end, sValue))
                            outputText(NULL, sValue.c_str(), nOption++);
                        continue;
                    }
                    default:
                        return;
                }
            }
            default:
                return;
        }
        ++iter;
    }
}

void
rcsSynNodeConvertor_T::convertOpcBiasCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "opc_bias ";

    hvUInt32 iLayer = 0;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case V1:
                {
                    outputText("-advanced_hier_policy", "yes", nOption++);
                    ++iter;
                    continue;
                }
                case V2:
                {
                    outputText("-advanced_hier_policy", "no", nOption++);
                    ++iter;
                    continue;
                }
                case IGNORE:
                {
                    ++iter;
                    std::string sValue;
                    if(getNumberExp(iter, pNode->end(), sValue))
                        outputText("-jog_limit", sValue.c_str(), nOption++);
                    continue;
                }
                case MINBIASLENGTH:
                {
                    ++iter;
                    std::string sValue;
                    if(getNumberExp(iter, pNode->end(), sValue))
                        outputText("-limit", sValue.c_str(), nOption++);

                    if(iter != pNode->end() && !_isOpcbiasKeyword(*iter))
                    {
                        std::string sValue;
                        if(getNumberExp(iter, pNode->end(), sValue))
                            outputText("-convex_limit", sValue.c_str(), nOption++);

                        if(iter != pNode->end() && !_isOpcbiasKeyword(*iter))
                        {
                            std::string sValue;
                            if(getNumberExp(iter, pNode->end(), sValue))
                                outputText("-concave_limit", sValue.c_str(), nOption++);
                        }
                    }

                    if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == ANGLED)
                    {
                        ++iter;
                        std::string sValue;
                        if(getNumberExp(iter, pNode->end(), sValue))
                            outputText("-slant_limit", sValue.c_str(), nOption++);
                        if(iter != pNode->end() && !_isOpcbiasKeyword(*iter))
                        {
                            std::string sValue;
                            if(getNumberExp(iter, pNode->end(), sValue))
                                outputText("-slant_convex_limit", sValue.c_str(), nOption++);

                            if(iter != pNode->end() && !_isOpcbiasKeyword(*iter))
                            {
                                std::string sValue;
                                if(getNumberExp(iter, pNode->end(), sValue))
                                    outputText("-slant_concave_limit", sValue.c_str(), nOption++);
                            }
                        }
                    }
                    continue;
                }
                case CLOSEREDGE:
                {
                    ++iter;
                    if(iter != pNode->end())
                    {
                        if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == V1)
                        {
                            outputText("-advanced_neighbor_policy", "yes", nOption++);
                            ++iter;
                            continue;
                        }
                        else if(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == V1)
                        {
                            outputText("-advanced_neighbor_policy", "no", nOption++);
                            ++iter;
                            continue;
                        }
                    }
                    else
                    {
                        outputText("-advanced_neighbor_policy", "no", nOption++);
                    }
                    continue;
                }
                case CLOSERSYMMETRIC:
                {
                    outputText("-symmetric_policy", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case GRID:
                {
                    ++iter;
                    std::string sValue;
                    if(getNumberExp(iter, pNode->end(), sValue))
                        outputText("-diagonal", sValue.c_str(), nOption++);
                    continue;
                }
                case IMPLICITBIAS:
                {
                    outputText("-default_action", "yes", nOption++);
                    ++iter;
                    continue;
                }
                case OPTION:
                {
                    ++iter;
                    if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD)
                    {
                        std::string sOption;
                        switch(iter->eKeyType)
                        {
                            case ACUTE_ANGLE_ABUT:
                                sOption = "-acute_also";
                                break;
                            case IMPLICIT_METRIC:
                                sOption = "-neighbor_metric";
                                break;
                            case FAST_CLASSIFY:
                                sOption = "-alternate_classify";
                                break;
                            case SPIKE_CLEANUP:
                                sOption = "-repair_skew";
                                break;
                            case CAREFUL_SKEW:
                                sOption = "-fill_gap";
                                break;
                            case CORNER_FILL:
                                sOption = "-fill_corner";
                                break;
                            case PROJECTING_ONLY:
                                sOption = "-project_classify";
                                break;
                            case CAREFUL_MEASURE:
                                sOption = "-careful_hier";
                                break;
                            default:
                                break;
                        }

                        ++iter;
                        if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                          (iter->eKeyType == YES || iter->eKeyType == NO))
                        {
                            if(iter->eKeyType == YES)
                                outputText(sOption.c_str(), "yes", nOption++);
                            else
                                outputText(sOption.c_str(), "no", nOption++);
                            ++iter;
                            continue;
                        }
                        else
                        {
                            --iter;
                        }
                    }
                    --iter;
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP11,
                                        iter->nLineNo, iter->sValue);
                }
                case SPACE:
                case SPACELAYER:
                    convertOpcbiasRule(iter, pNode->end(), nOption);
                    continue;
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if(iLayer < 3)
                {
                    ++iLayer;
                    std::string sLayerName;
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText("-in_layer", sLayerName.c_str(), nOption++);
                    }
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertDfmSpaceCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case BY_INT:
                    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "check_dfm_space_by_int ";
                    break;
                case BY_ENC:
                    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "check_dfm_space_by_enc ";
                    break;
                case BY_EXT:
                    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "check_dfm_space_by_ext ";
                    break;
                case ALL:
                    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "check_dfm_space_by_all ";
                    break;
                default:
                    break;
            }
        }
        ++iter;
    }

    iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case BY_INT:
                case BY_ENC:
                case BY_EXT:
                case ALL:
                {
                    ++iter;
                    continue;
                }
                case GRID:
                {
                    std::string sNumExp;
                    if(getNumberExp(++iter, pNode->end(), sNumExp))
                        outputText("-grid", sNumExp.c_str(), nOption++);
                    continue;
                }
                case HORIZONTAL:
                {
                    outputText("-horizontal", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case VERTICAL:
                {
                    outputText("-vertical", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case MEASURE_ALL:
                {
                    outputText("-measure_option", "all", nOption++);
                    ++iter;
                    continue;
                }
                case BY_LAYER:
                {
                    std::string sLayerName;
                    if(!getLayerRefName(++iter, pNode->end(), sLayerName))
                        sLayerName = iter->sValue;
                    outputText("-by_layer", sLayerName.c_str(), nOption++);
                    continue;
                }
                case CONNECTED:
                {
                    outputText("-same_net", "yes", nOption++);
                    ++iter;
                    continue;
                }
                case NOT_CONNECTED:
                {
                    outputText("-same_net", "no", nOption++);
                    ++iter;
                    continue;
                }
                case COUNT:
                {
                    std::string sCons;
                    if(getConstraint(++iter, pNode->end(), sCons))
                        outputText("-shield_count", sCons.c_str(), nOption++);
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText("-in_layer", sLayerName.c_str(), nOption++);
                }
                else if(iter->eType == OPERATOR && isConstraintOperator(*iter))
                {
                    std::string sCons;
                    if(getConstraint(iter, pNode->end(), sCons))
                        outputText("-dist_limit", sCons.c_str(), nOption++);
                }
                continue;
            }
            case BUILT_IN_LANG:
            {
                std::string sValue = iter->sValue;
                if(sValue.find("rcn::built_in { ") != std::string::npos)
                {
                    std::string::size_type begin = sValue.find("rcn::built_in { ");
                    std::string::size_type end = sValue.find_last_of("}");
                    Utassert(end != std::string::npos);
                    sValue = sValue.substr(begin + 15, end - begin - 15);
                    sValue.insert(0, "\\[");
                    if(m_iProcLevel > 0)
                    {
                        sValue.insert(0, "\\\\");
                        sValue.insert(sValue.size(), "\\\\");
                    }
                    sValue.insert(sValue.size(), "\\]");
                }
                else
                {
                    sValue.erase(0, 1);
                    sValue.erase(sValue.size() - 1);
                    trim(sValue);
                    sValue.insert(0, "\\[");
                    if(m_iProcLevel > 0)
                    {
                        sValue.insert(0, "\\\\");
                        sValue.insert(sValue.size(), "\\\\");
                    }
                    sValue.insert(sValue.size(), "\\]");
                }
                outputText("-in_layer", sValue.c_str(), nOption++);

                break;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomCopyDfmLayerCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_copy_dfm_layer ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case REGION:
                {
                    if(++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                       UNMERGED == iter->eKeyType)
                    {
                        outputText("-output", "unmerged_region", nOption++);
                    }
                    else
                    {
                        outputText("-output", "region", nOption++);
                        --iter;
                    }
                    ++iter;
                    continue;
                }
                case TIE:
                {
                    if(++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                       CENTER == iter->eKeyType)
                    {
                        outputText("-output", "tie_center", nOption++);
                    }
                    else
                    {
                        outputText("-output", "tie", nOption++);
                        --iter;
                    }
                    ++iter;
                    continue;
                }
                case CLUSTER:
                {
                    if(++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                       LAYERID == iter->eKeyType)
                    {
                        outputText("-output", "cluster_layerid", nOption++);
                    }
                    else
                    {
                        outputText("-output", "cluster", nOption++);
                        --iter;
                    }
                    ++iter;
                    continue;
                }
                case EDGE:
                    outputText("-output", "edge", nOption++);
                    ++iter;
                    continue;
                case CENTERLINE:
                    outputText("-output", "centerline", nOption++);
                    ++iter;
                    continue;
                case UNMERGED:
                    outputText("-output", "unmerged", nOption++);
                    ++iter;
                    continue;
                case CELL:
                {
                    if(++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                       LIST == iter->eKeyType)
                    {
                        if(++iter == pNode->end())
                        {
                            --iter;
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2,
                                                iter->nLineNo, "CELL LIST");
                        }
                        else
                        {
                            switch(iter->eType)
                            {
                                case SECONDARY_KEYWORD:
                                case IDENTIFIER_NAME:
                                case STRING_CONSTANTS:
                                {
                                    std::string sCellListName;
                                    if(getStringName(iter, pNode->end(), sCellListName))
                                        outputText("-cell_list", sCellListName.c_str(), nOption++);
                                    break;
                                }
                                default:
                                    throw rcErrorNode_T(rcErrorNode_T::ERROR,
                                                        INP1, iter->nLineNo,
                                                        iter->sValue);
                                    break;
                            }
                        }
                        ++iter;
                        continue;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertDfmStampCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_stamp_dfm_layer ";
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType && iter->eKeyType == BY)
        {
            ++iter;
            if(iter == pNode->end() || (iter->eType != STRING_CONSTANTS &&
               iter->eType != SECONDARY_KEYWORD && iter->eType != IDENTIFIER_NAME) ||
               (iter->eType == IDENTIFIER_NAME && !isValidSvrfName(iter->sValue)))
            {
                --iter;
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP15, iter->nLineNo,
                                    iter->sValue);
            }

            Utassert(iter->eType == STRING_CONSTANTS || iter->eType ==
                     SECONDARY_KEYWORD || iter->eType == IDENTIFIER_NAME);
            std::string sValue;
            if(getStringName(iter, pNode->end(), sValue))
                outputText("-by_prop",  sValue.c_str(), nOption++);
            ++iter;
            continue;
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertDfmRDBCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    if(pNode->getOperationType() == DFM_RDB)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "dfm_rdb ";
#if 0
    else if(pNode->getOperationType() == DFM_RDB_GDS)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "dfm_rdb -format gds";
    else if(pNode->getOperationType() == DFM_RDB_OASIS)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "dfm_rdb -format oasis";
#endif


    hvUInt32 iLayer = 0;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case CSG:
                {
                    outputText("-csg", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case NULL_SECOND:
                {
                    outputText("-null", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case NOPSEUDO:
                {
                    outputText("-no_pseudo", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case NODAL:
                {
                    ++iter;
                    if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       (iter->eKeyType == ALL || iter->eKeyType == OTHER))
                    {
                        if(iter->eKeyType == ALL)
                        {
                            outputText("-nodal", "all", nOption++);
                        }
                        else
                        {
                            outputText("-nodal", "other", nOption++);
                        }
                        ++iter;
                    }
                    else
                    {
                        outputText("-nodal", NULL, nOption++);
                    }
                    continue;
                }
                case OUTPUT:
                {
                    outputText("-output", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case JOINED:
                {
                    outputText("-joined", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case TRANSITION:
                {
                    outputText("-transition", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case ABUT_ALSO:
                {
                    outputText("-abut", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case NOEMPTY:
                {
                    outputText("-no_empty", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case SAME:
                {
                    ++iter;
                    if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       iter->eKeyType == CELL)
                    {
                        outputText("-same_cell", NULL, nOption++);
                        ++iter;
                        continue;
                    }
                    --iter;
                    break;
                }
                case RESULT_CELLS:
                {
                    ++iter;
                    std::string sListName;
                    if(getStringName(iter, pNode->end(), sListName))
                        outputText("-result_cell_list", sListName.c_str(), nOption++);
                    ++iter;
                    continue;
                }
                case ALL_CELLS:
                {
                    ++iter;
                    outputText("-all_cells", NULL, nOption++);
                    continue;
                }
                case CHECKNAME:
                {
                    ++iter;
                    std::string sRuleName;
                    if(getStringName(iter, pNode->end(), sRuleName))
                        outputText("-rule_name", sRuleName.c_str(), nOption++);
                    ++iter;
                    continue;
                }
                case COMMENT_SECOND:
                {
                    ++iter;
                    std::string sComment;
                    if(getStringName(iter, pNode->end(), sComment))
                        outputText("-comment", sComment.c_str(), nOption++);
                    ++iter;
                    continue;
                }
                case ANNOTATE:
                {
                    ++iter;
                    if(iter != pNode->end() && iter->eType == BUILT_IN_LANG)
                    {
                        std::string sValue = iter->sValue;
                        sValue.erase(0, 1);
                        sValue.insert(0, "{ ");
                        sValue.erase(sValue.size() - 1);
                        sValue.insert(sValue.size(), " }");
                        outputText("-annotate", sValue.c_str(), nOption++);
                        break;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP18,
                                            iter->nLineNo, iter->sValue);
                    }
                    ++iter;
                    continue;
                }
                case MAXIMUM:
                {
                    ++iter;
                    if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       iter->eKeyType == ALL)
                    {
                       outputText("-maximum_results", "all", nOption++);
                        ++iter;
                        continue;
                    }
                    else
                    {
                        std::string sNumExp;
                        if(getNumberExp(iter, pNode->end(), sNumExp))
                            outputText("-maximum_results", sNumExp.c_str(), nOption++);
                        continue;
                    }
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            {
                if(!isValidSvrfName(iter->sValue))
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
            }
            case STRING_CONSTANTS:
            case OPERATOR:
            case NUMBER:
            {
                std::string sLayerName;
                if(iLayer < 1)
                {
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                       outputText("-in_layer", sLayerName.c_str(), nOption++);
                    }
                    ++iLayer;
                    continue;
                }
                else if(iLayer == 1)
                {
                    std::string sFileName;
                    if(getStringName(iter, pNode->end(), sFileName))
                    {
                       outputText("-db_file", sFileName.c_str(), nOption++);
                    }
                    ++iter;
                    continue;
                }
                else if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText("-in_layer", sLayerName.c_str(), nOption++);
                    ++iLayer;
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo, iter->sValue);
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertDfmNARACCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "dfm_narac ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType && iter->eKeyType == BY)
        {
            ++iter;
            if(iter == pNode->end() || (iter->eType != STRING_CONSTANTS &&
               iter->eType != SECONDARY_KEYWORD && iter->eType != IDENTIFIER_NAME) ||
               (iter->eType == IDENTIFIER_NAME && !isValidSvrfName(iter->sValue)))
            {
                --iter;
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP15, iter->nLineNo,
                                    iter->sValue);
            }

            Utassert(iter->eType == STRING_CONSTANTS || iter->eType ==
                     SECONDARY_KEYWORD || iter->eType == IDENTIFIER_NAME);
            std::string sValue;
            if(getStringName(iter, pNode->end(), sValue))
                outputText("-by_prop",  sValue.c_str(), nOption++);
            ++iter;
            continue;
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertDfmSizeCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_dfm_size ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case BY:
                {
                    ++iter;
                    std::list<rcsToken_T>::iterator begin = iter;
                    std::string sPropName;
                    getStringName(iter, pNode->end(), sPropName);
                    std::string sNumExp;
                    getNumberExp(iter, pNode->end(), sNumExp);
                    if(!sPropName.empty())
                        outputText("-size_value", sPropName.c_str(), nOption++);
                    else if(!sNumExp.empty())
                        outputText("-size_value", sNumExp.c_str(), nOption++);
                    continue;
                }
                case INSIDE_OF:
                {
                    ++iter;
                    std::string sLayerName;
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                       outputText("-inside_layer", sLayerName.c_str(), nOption++);
                       continue;
                    }
                }
                case OUTSIDE_OF:
                {
                    ++iter;
                    std::string sLayerName;
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                       outputText("-outside_layer", sLayerName.c_str(), nOption++);
                       continue;
                    }
                }
                case STEP:
                {
                    ++iter;
                    std::list<rcsToken_T>::iterator begin = iter;
                    std::string sPropName;
                    getStringName(iter, pNode->end(), sPropName);
                    std::string sNumExp;
                    getNumberExp(iter, pNode->end(), sNumExp);
                    if(!sPropName.empty())
                        outputText("-step_value", sPropName.c_str(), nOption++);
                    else if(!sNumExp.empty())
                        outputText("-step_value", sNumExp.c_str(), nOption++);
                    continue;
                }
                case TRUNCATE:
                {
                    ++iter;
                    std::string sNumExp;
                    if(getNumberExp(iter, pNode->end(), sNumExp))
                        outputText("-truncate_value", sNumExp.c_str(), nOption++);
                    continue;
                }
                case BEVEL:
                {
                    ++iter;
                    std::string sNumExp;
                    if(getNumberExp(iter, pNode->end(), sNumExp))
                        outputText("-bevel_value", sNumExp.c_str(), nOption++);
                    continue;
                }
                case OVERLAP_ONLY:
                {
                    outputText("-output_overlap", NULL, nOption++);
                    ++iter;
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomMergeNetCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_merge_net ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if(BY_NET == iter->eKeyType)
                {
                    outputText("-merge_overlap", NULL, nOption++);
                    break;
                }
                else if(BY_SHAPE == iter->eKeyType)
                {
                    outputText("-merge_overlap", "no", nOption++);
                    break;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomGetNetByNameCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    if(pNode->getOperationType() == NET)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_get_net_by_name ";
    else if(pNode->getOperationType() == NOT_NET)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_not_get_net_by_name ";

    bool hasInLayer = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            {
                if(!isValidSvrfName(iter->sValue))
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
            }
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if(!hasInLayer)
                {
                    hasInLayer = true;
                    std::string sLayerName;
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                       outputText("-in_layer", sLayerName.c_str(), nOption++);
                       continue;
                    }
                }
                else if(iter->eType != NUMBER && iter->eType != OPERATOR)
                {
                    std::string sValue;
                    if(getStringName(iter, pNode->end(), sValue))
                    {
                        if(m_vVariableNames.find(iter->sValue) != m_vVariableNames.end())
                        {
                            sValue = iter->sValue;
                            toLower(sValue);
                            sValue.insert(0, "$");
                        }
                        outputText("-name", sValue.c_str(), nOption++);
                        break;
                    }
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }


    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::parseRuleValue(std::list<rcsToken_T>::iterator &iter,
                                      std::list<rcsToken_T>::iterator end)
{
    std::string sValue;
    if(getNumberExp(iter, end, sValue))
        outputText(NULL, sValue.c_str());
    if(iter != end && SECONDARY_KEYWORD == iter->eType)
    {
        switch(iter->eKeyType)
        {
            case OPPOSITE_EXTENDED:
            {
                outputText("-metric", "opposite_extended");

                std::string sNumExp;
                if(getNumberExp(++iter, end, sNumExp))
                    outputText(NULL, sNumExp.c_str());
                return;
            }
            case OPPOSITE:
                outputText("-metric", "opposite");
                ++iter;
                return;
            case SQUARE:
                outputText("-metric", "square");
                ++iter;
                return;
            default:
                break;
        }
    }
}

void
rcsSynNodeConvertor_T::parseRectRule(std::list<rcsToken_T>::iterator &iter,
                                     std::list<rcsToken_T>::iterator end)
{
    Utassert(GOOD == iter->eKeyType || BAD == iter->eKeyType);
    m_oPVRSStream << "\\" << std::endl;

    if(iter->eKeyType == GOOD)
        outputText("-rect_rule", "good");
    else
        outputText("-rect_rule", "bad");

    ++iter;
    parseRuleValue(iter, end);
    parseRuleValue(iter, end);
    parseRuleValue(iter, end);
    parseRuleValue(iter, end);

}

void
rcsSynNodeConvertor_T::convertCheckRectEncCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "check_rect_enc ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case OUTSIDE_ALSO:
                {
                    outputText("-output_outside", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case ORTHOGONAL:
                {
                    if(++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType
                       && ONLY == iter->eKeyType)
                    {
                        outputText("-rect_direction", "orth", nOption++);
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                    ++iter;
                    continue;
                }
                case GOOD:
                case BAD:
                {
                    parseRectRule(iter, pNode->end());
                    continue;
                }
                case ABUT:
                {
                    ++iter;
                    if(iter != pNode->end() && isConstraintOperator(*iter))
                    {
                        std::string sCons;
                        if(getConstraint(iter, pNode->end(), sCons))
                            outputText("-abut", sCons.c_str(), nOption++);
                    }
                    else
                    {
                        outputText("-abut", NULL, nOption++);
                    }
                    continue;
                }
                case SINGULAR:
                    outputText("-point_touch", NULL, nOption++);
                    ++iter;
                    continue;
                default:
                    break;
            }
        }
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomBooleanCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_boolean ";
    switch(pNode->getOperationType())
    {
        case AND:
            outputText("-type", "and", nOption++);
            break;
        case OR:
            outputText("-type", "or", nOption++);
            break;
        case NOT:
            outputText("-type", "not", nOption++);
            break;
        case XOR:
            outputText("-type", "xor", nOption++);
            break;
        default:
            break;
    }

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if(pNode->getOperationType() == AND)
                {
                    switch(iter->eKeyType)
                    {
                        case CONNECTED:
                            outputText("-connect", "yes", ++nOption);
                            ++iter;
                            continue;
                        case NOT_CONNECTED:
                            outputText("-connect", "no", ++nOption);
                            ++iter;
                            continue;
                        default:
                            break;
                    }
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
                else if(iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    outputText("-overlap_limit", sConstraint.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomOriginalCheckCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_original_check ";
    switch(pNode->getOperationType())
    {
        case DRAWN_ACUTE:
            outputText("-type", "acute_angle", nOption++);
            break;
        case DRAWN_ANGLED:
            outputText("-type", "slant_edge", nOption++);
            break;
        case DRAWN_SKEW:
            outputText("-type", "not45_edge", nOption++);
            break;
        case DRAWN_OFFGRID:
            outputText("-type", "offgrid_vertice", nOption++);
            break;
        default:
            break;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomTransformCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_transform ";
    switch(pNode->getOperationType())
    {
        case GROW:
            outputText("-type", "grow", nOption++);
            break;
        case SHRINK:
            outputText("-type", "shrink", nOption++);
            break;
        case MAGNIFY:
            outputText("-type", "mag", nOption++);
            break;
        case ROTATE:
            outputText("-type", "rotate", nOption++);
            break;
        case SHIFT:
            outputText("-type", "shift", nOption++);
            break;
        default:
            break;
    }

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if(RIGHT == iter->eKeyType)
                {
                    if(++iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       iter->eKeyType == BY)
                    {
                        ++iter;
                        std::string sNumExp;
                        if(getNumberExp(iter, pNode->end(), sNumExp))
                            outputText("-right", sNumExp.c_str(), nOption++);
                        continue;
                    }
                    --iter;
                }
                else if(LEFT == iter->eKeyType)
                {
                    if(++iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       iter->eKeyType == BY)
                    {
                        ++iter;
                        std::string sNumExp;
                        if(getNumberExp(iter, pNode->end(), sNumExp))
                            outputText("-left", sNumExp.c_str(), nOption++);
                        continue;
                    }
                    --iter;
                }
                else if(BOTTOM == iter->eKeyType)
                {
                    if(++iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       iter->eKeyType == BY)
                    {
                        ++iter;
                        std::string sNumExp;
                        if(getNumberExp(iter, pNode->end(), sNumExp))
                            outputText("-bottom", sNumExp.c_str(), nOption++);
                        continue;
                    }
                    --iter;
                }
                else if(TOP == iter->eKeyType)
                {
                    if(++iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       iter->eKeyType == BY)
                    {
                        ++iter;
                        std::string sNumExp;
                        if(getNumberExp(iter, pNode->end(), sNumExp))
                            outputText("-top", sNumExp.c_str(), nOption++);
                        continue;
                    }
                    --iter;
                }
                else if(BY == iter->eKeyType)
                {
                    std::list<rcsToken_T>::iterator begin = iter;
                    ++iter;
                    std::string sNumExp;
                    if(getNumberExp(iter, pNode->end(), sNumExp))
                        outputText("-by", sNumExp.c_str(), nOption++);
                    if(SHIFT == pNode->getOperationType())
                    {
                        if(iter == pNode->end())
                        {
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP11,
                                                begin->nLineNo, begin->sValue);
                        }
                        sNumExp.clear();
                        if(getNumberExp(iter, pNode->end(), sNumExp, true))
                            outputText(NULL, sNumExp.c_str(), nOption);
                    }
                    continue;
                }
                else if(SEQUENTIAL == iter->eKeyType && GROW == pNode->getOperationType())
                {
                    outputText("-sequential", NULL, nOption++);
                    ++iter;
                    continue;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertEdgeExpandToRectCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "edge_expand_to_rect ";

    std::list<rcsToken_T>::iterator  iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case INSIDE_BY:
                case INSIDE_BY_FACTOR:
                {
                    std::string sOption = "-inside_by";
                    if(INSIDE_BY_FACTOR == iter->eKeyType)
                        sOption = "-inside_by_factor";
                    ++iter;
                    std::string sNumExp;
                    if(getNumberExp(iter, pNode->end(), sNumExp))
                        outputText(sOption.c_str(), sNumExp.c_str(), nOption++);
                    continue;
                }
                case OUTSIDE_BY:
                case OUTSIDE_BY_FACTOR:
                {
                    std::string sOption = "-outside_by";
                    if(OUTSIDE_BY_FACTOR == iter->eKeyType)
                        sOption = "-outside_by_factor";
                    ++iter;
                    std::string sNumExp;
                    if(getNumberExp(iter, pNode->end(), sNumExp))
                        outputText(sOption.c_str(), sNumExp.c_str(), nOption++);
                    continue;
                }
                case BY:
                case BY_FACTOR:
                {
                    std::string sOption = "-both_side_by";
                    if(BY_FACTOR == iter->eKeyType)
                        sOption = "-both_side_by_factor";
                    ++iter;
                    std::string sNumExp;
                    if(getNumberExp(iter, pNode->end(), sNumExp))
                        outputText(sOption.c_str(), sNumExp.c_str(), nOption++);
                    continue;
                }
                case EXTEND_BY:
                case EXTEND_BY_FACTOR:
                {
                    std::string sOption = "-extend_by";
                    if(EXTEND_BY_FACTOR == iter->eKeyType)
                        sOption = "-extend_by_factor";
                    ++iter;
                    std::string sNumExp;
                    if(getNumberExp(iter, pNode->end(), sNumExp))
                        outputText(sOption.c_str(), sNumExp.c_str(), nOption++);
                    continue;
                }
                case CORNER_FILL:
                {
                    outputText("-fill_corner", NULL, nOption++);
                    ++iter;
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}
void
rcsSynNodeConvertor_T::convertSizeCmd(rcsSynLayerOperationNode_T *pNode)
{
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    bool isUnderOver = false;
    bool isOverUnder = false;
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case UNDEROVER:
                    isUnderOver = true;
                    break;
                case OVERUNDER:
                    isOverUnder = true;
                default:
                    break;
            }
        }
        ++iter;
    }

    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix();
    if(isOverUnder)
        m_oPVRSStream << "geom_size_overunder ";
    else if(isUnderOver)
        m_oPVRSStream << "geom_size_underover ";
    else
        m_oPVRSStream << "geom_size ";

    iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case BY:
                {
                    ++iter;
                    std::string sNumExp;
                    if(getNumberExp(iter, pNode->end(), sNumExp))
                        outputText("-size_value", sNumExp.c_str(), nOption++);
                    continue;
                }
                case INSIDE_OF:
                {
                    ++iter;
                    std::string sLayerName;
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                       outputText("-inside_layer", sLayerName.c_str(), nOption++);
                       continue;
                    }
                }
                case OUTSIDE_OF:
                {
                    ++iter;
                    std::string sLayerName;
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                       outputText("-outside_layer", sLayerName.c_str(), nOption++);
                       continue;
                    }
                }
                case STEP:
                {
                    ++iter;
                    std::string sNumExp;
                    if(getNumberExp(iter, pNode->end(), sNumExp))
                        outputText("-step_value", sNumExp.c_str(), nOption++);
                    continue;
                }
                case TRUNCATE:
                {
                    ++iter;
                    std::string sNumExp;
                    if(getNumberExp(iter, pNode->end(), sNumExp))
                        outputText("-truncate_value", sNumExp.c_str(), nOption++);
                    continue;
                }
                case BEVEL:
                {
                    ++iter;
                    std::string sNumExp;
                    if(getNumberExp(iter, pNode->end(), sNumExp))
                        outputText("-bevel_value", sNumExp.c_str(), nOption++);
                    continue;
                }
                case OVERLAP_ONLY:
                {
                    outputText("-output_overlap", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case UNDEROVER:
                case OVERUNDER:
                    ++iter;
                    continue;
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomSelectHolesCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_select_holes ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if(INNER == iter->eKeyType)
                {
                    outputText("-inner", NULL, nOption++);
                    break;
                }
                else if(EMPTY == iter->eKeyType)
                {
                    outputText("-empty", NULL, nOption++);
                    break;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
                else if(iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    outputText("-area_limit", sConstraint.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertEdgeConvertAgnledToRegularCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "edge_convert_angled_to_regular ";

    bool hasMaxOffset = false;
    bool hasInLayer   = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
   {
       if(SECONDARY_KEYWORD == iter->eType)
       {
           switch(iter->eKeyType)
           {
               case ORTHOGONAL:
               {
                   if(++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                      ONLY == iter->eKeyType)
                   {
                       outputText("-orth_edge_only", NULL, nOption++);
                   }
                   else
                   {
                       --iter;
                       throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                           iter->nLineNo, iter->sValue);
                   }
                   ++iter;
                   continue;
               }
               case SKEW:
                   outputText("-option", "skew", nOption++);
                   ++iter;
                   continue;
               case ALL:
                   outputText("-option", "all", nOption++);
                   ++iter;
                   continue;
               default:
                   break;
           }
       }

       switch(iter->eType)
       {
           case LAYER_OPERATION_KEYWORD:
               break;
           case SECONDARY_KEYWORD:
           case IDENTIFIER_NAME:
           case STRING_CONSTANTS:
           case NUMBER:
           case OPERATOR:
           {
               std::string sLayerName;
               if(!hasInLayer && getLayerRefName(iter, pNode->end(), sLayerName))
               {
                   hasInLayer = true;
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
               }
               else if(!hasMaxOffset)
               {
                   std::string sNumExp;
                   if(getNumberExp(iter, pNode->end(), sNumExp))
                   {
                       outputText("-max_offset", sNumExp.c_str(), nOption++);
                       hasMaxOffset = true;
                       continue;
                   }
               }
               else
               {
                   throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                       iter->nLineNo, iter->sValue);
               }
               continue;
           }
           default:
               throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                   iter->nLineNo, iter->sValue);
               break;
       }
       ++iter;
   }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomVerticeSnapToGridCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_vertices_snap_to_grid ";

    bool hasInLayer = false;
    bool hasValue1  = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            case SEPARATOR:
            {
                std::string sLayerName;
                if(!hasInLayer && getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    hasInLayer = true;
                    outputText("-in_layer", sLayerName.c_str(), nOption++);
                    continue;
                }
                else
                {
                    std::string sNumExp;
                    if(getNumberExp(iter, pNode->end(), sNumExp))
                    {
                        if(!hasValue1)
                        {
                            outputText("-grid", sNumExp.c_str(), nOption);
                            hasValue1 = true;
                            continue;
                        }
                        else
                        {
                            outputText(NULL, sNumExp.c_str(), nOption++);
                            hasValue1 = true;
                            continue;
                        }
                    }
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomCopyLayerCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_copy_layer ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            case SEPARATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText("-in_layer", sLayerName.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomFlattenLayerCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_flatten_layer ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            case SEPARATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText("-in_layer", sLayerName.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomMergeLayerCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_merge_layer ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if(BY == iter->eKeyType)
                {
                    std::string sNumExp;
                    if(getNumberExp(++iter, pNode->end(), sNumExp))
                    {
                        outputText("-by", sNumExp.c_str(), nOption++);
                        continue;
                    }
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            case SEPARATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText("-in_layer", sLayerName.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomPushLayerCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_push_layer ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if(iter->eKeyType == LOCATE)
                {
                    ++iter;
                    std::string sLayerName;
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText("-locate_layer", sLayerName.c_str(), nOption++);
                    }
                    continue;
                }
                else if(iter->eKeyType == LIGHT)
                {
                    outputText("-light", NULL, nOption++);
                    break;
                }
                else if(iter->eKeyType == MEDIUM)
                {
                    outputText("-medium", NULL, nOption++);
                    break;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText("-in_layer", sLayerName.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomStampLayerCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_stamp_layer ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if(iter->eKeyType == BY)
                {
                    ++iter;
                    std::string sLayerName;
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText("-by_layer", sLayerName.c_str(), nOption++);
                    }
                    continue;
                }
                else if(iter->eKeyType == ABUT_ALSO)
                {
                    outputText("-abut", NULL, nOption++);
                    break;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText("-in_layer", sLayerName.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomGetMultiLayersBBoxCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_get_multi_layers_bbox ";

    bool hasIgnore = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if(IGNORE == iter->eKeyType)
                {
                    hasIgnore = true;
                    ++iter;
                    if(iter == pNode->end())
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP10,
                                            iter->nLineNo, iter->sValue);
                    }

                    outputText("-ignore_layer", NULL, nOption++);
                    while(iter != pNode->end())
                    {
                        if(SECONDARY_KEYWORD == iter->eType &&
                           ORIGINAL == iter->eKeyType)
                            break;

                        std::string sLayerName;
                        if(getLayerRefName(iter, pNode->end(), sLayerName))
                        {
                            outputText(NULL, sLayerName.c_str(), nOption++);
                        }
                        else
                            break;
                    }
                    continue;
                }
                else if(ORIGINAL == iter->eKeyType)
                {
                    outputText("-original", NULL, nOption++);
                    break;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(hasIgnore)
                {
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText("-ignore_layer", sLayerName.c_str(), nOption++);
                        continue;
                    }
                }
                else if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText("-in_layer", sLayerName.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomGetBBoxCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_get_bbox ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if(CENTERS == iter->eKeyType)
                {
                    outputText("-output", "centers", nOption);
                    ++iter;
                    if(iter == pNode->end())
                        continue;

                    std::string sNumExp;
                    if(getNumberExp(iter, pNode->end(), sNumExp))
                    {
                        outputText(NULL, sNumExp.c_str(), nOption);
                    }
                    nOption++;
                    continue;
                }
                else if(SQUARES == iter->eKeyType)
                {
                    outputText("-output", "squares", nOption++);
                    break;
                }
                else if(INSIDE_OF_LAYER == iter->eKeyType)
                {
                    ++iter;
                    std::string sLayerName;
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText("-inside_layer", sLayerName.c_str(), nOption++);
                        continue;
                    }
                    continue;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText("-in_layer", sLayerName.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomGetCellBBoxCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_get_cell_bbox ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case ORIGINAL:
                {
                    outputText("-data", "original", nOption++);
                    ++iter;
                    continue;
                }
                case MAPPED:
                {
                    outputText("-data", "mapped", nOption++);
                    ++iter;
                    continue;
                }
                case OCCUPIED:
                {
                    outputText(NULL, "occupied", nOption++);
                    ++iter;
                    continue;
                }
                case WITH_MATCH:
                {
                    outputText("-match", NULL, nOption++);
                    ++iter;
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            {
                std::string sValue;
                if(getStringName(iter, pNode->end(), sValue))
                {
                    if(m_vVariableNames.find(iter->sValue) != m_vVariableNames.end())
                    {
                        sValue = iter->sValue;
                        toLower(sValue);
                        sValue.insert(0, "$");
                    }
                    outputText("-cell_name", sValue.c_str(), nOption++);
                    break;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomGetLayerBBoxCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_get_layer_bbox ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            case SEPARATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText("-in_layer", sLayerName.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertFillRectanglesCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "fill_rectangles ";

    std::string sWOffset;
    std::string sLOffset;
    std::string sLength;
    std::string sWidth;
    std::string sWSpacing;
    std::string sLSpacing;
    std::string sLeftX;
    std::string sRightX;
    std::string sBottomY;
    std::string sTopY;
    std::string sInsideLayer;
    bool        isMaintainSpacing = false;

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case OFFSET:
                {
                    ++iter;
                    getNumberExp(iter, pNode->end(), sWOffset);

                    if(iter != pNode->end())
                    {
                        if(SECONDARY_KEYWORD == iter->eType && (MAINTAIN == iter->eKeyType
                           || OFFSET == iter->eKeyType || !isValidSvrfName(iter->sValue)))
                            continue;

                        getNumberExp(iter, pNode->end(), sLOffset);
                    }
                    continue;
                }
                case INSIDE_OF:
                {
                    ++iter;
                    getNumberExp(iter, pNode->end(), sLeftX, true);
                    getNumberExp(iter, pNode->end(), sBottomY, true);
                    getNumberExp(iter, pNode->end(), sRightX, true);
                    getNumberExp(iter, pNode->end(), sTopY, true);
                    continue;
                }
                case INSIDE_OF_LAYER:
                {
                    std::string sLayerName;
                    if(getLayerRefName(++iter, pNode->end(), sInsideLayer))
                        continue;
                }
                case MAINTAIN:
                {
                    if(++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType && SPACING == iter->eKeyType)
                    {
                        isMaintainSpacing = true;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                    ++iter;
                    continue;
                }
                default:
                    break;
            }
        }
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            case SEPARATOR:
            {
                if(sWidth.empty())
                {
                    getNumberExp(iter, pNode->end(), sWidth);
                    continue;
                }
                else if(sLength.empty())
                {
                    getNumberExp(iter, pNode->end(), sLength);
                    continue;
                }
                else if(sWSpacing.empty())
                {
                    getNumberExp(iter, pNode->end(), sWSpacing);
                    continue;
                }
                else if(sLSpacing.empty())
                {
                    getNumberExp(iter, pNode->end(), sLSpacing);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(sLSpacing.empty())
        sLSpacing = sWSpacing;

    outputText("-width", sWidth.c_str(), nOption++);
    outputText("-length", sLength.c_str(), nOption++);
    outputText("-spacing", sWSpacing.c_str(), nOption);
    outputText(NULL, sLSpacing.c_str(), nOption++);
    if(!sWOffset.empty())
    {
        outputText("-offset", sWOffset.c_str(), nOption);
        if(sLOffset.empty())
            sLOffset = sWOffset;
        outputText(NULL, sLOffset.c_str(), nOption++);
    }

    if(!sLeftX.empty())
    {
        outputText("-inside_box", sLeftX.c_str(), nOption);
        outputText(NULL, sBottomY.c_str(), nOption);
        outputText(NULL, sRightX.c_str(), nOption);
        outputText(NULL, sTopY.c_str(), nOption++);
    }
    else if(!sInsideLayer.empty())
    {
        outputText("-inside_layer", sInsideLayer.c_str(), nOption++);
    }

    if(isMaintainSpacing)
        outputText("-keep_spacing", NULL, nOption++);

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomGetTextBBoxCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_get_text_bbox ";

    bool hasText      = false;
    bool hasTextLayer = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if(PRIMARY == iter->eKeyType)
                {
                    if(++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType && ONLY == iter->eKeyType)
                    {
                        outputText("-depth", "primary", nOption++);
                        break;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                }
                else if(CASE == iter->eKeyType)
                {
                    if(++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                       SENSITIVE == iter->eKeyType)
                    {
                        outputText("-text_case", "sensitive", nOption++);
                        break;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                }
                else if(BY == iter->eKeyType)
                {
                    std::string sNumExp;
                    if(getNumberExp(++iter, pNode->end(), sNumExp))
                    {
                        outputText("-expand_value", sNumExp.c_str(), nOption++);
                    }
                    continue;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if(!hasText)
                {
                    std::string sValue;
                    if(getStringName(iter, pNode->end(), sValue))
                    {
                        outputText("-text", sValue.c_str(), nOption++);
                        hasText = true;
                        break;
                    }
                }
                else if(!hasTextLayer)
                {
                    std::string sLayerName;
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText("-text_layer", sLayerName.c_str(), nOption++);
                    }
                    hasTextLayer = true;
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomSelectTextedCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    if(pNode->getOperationType() == WITH_TEXT)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_select_texted ";
    else if((pNode->getOperationType() == NOT_WITH_TEXT))
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_select_untexted ";

    bool hasGeomLayer = false;
    bool hasText      = false;
    bool hasTextLayer = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if(PRIMARY == iter->eKeyType)
                {
                    if(++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType && ONLY == iter->eKeyType)
                    {
                        outputText("-depth", "primary", nOption++);
                        break;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                }
                else if(CASE == iter->eKeyType)
                {
                    if(++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType && SENSITIVE == iter->eKeyType)
                    {
                        outputText("-text_case", "sensitive", nOption++);
                        break;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if(!hasGeomLayer)
                {
                    std::string sLayerName;
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText("-in_layer", sLayerName.c_str(), nOption++);
                    }
                    hasGeomLayer = true;
                    continue;
                }
                else if(!hasText)
                {
                    std::string sValue;
                    if(getStringName(iter, pNode->end(), sValue))
                    {
                        outputText("-text", sValue.c_str(), nOption++);
                        hasText = true;
                        break;
                    }
                }
                else if(!hasTextLayer)
                {
                    std::string sLayerName;
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText("-text_layer", sLayerName.c_str(), nOption++);
                    }
                    hasTextLayer = true;
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertTableDRCSpaceRule(std::list<rcsToken_T>::iterator &iter,
                                                std::list<rcsToken_T>::iterator end)
{
    m_oPVRSStream << "\\" << std::endl;

    bool hasSpaceCons = false;
    while(iter != end)
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                switch(iter->eKeyType)
                {
                    case SPACE:
                    {
                        if(hasSpaceCons)
                            return;

                        hasSpaceCons = true;
                        std::string sCons;
                        if(getConstraint(++iter ,end, sCons))
                            outputText("-space_limit", sCons.c_str());
                        continue;
                    }
                    case EXCLUDE_SHIELDED:
                    {
                        if(++iter != end)
                        {
                            if(((SECONDARY_KEYWORD == iter->eType ||
                                IDENTIFIER_NAME == iter->eType) &&
                                !isValidSvrfName(iter->sValue)) ||
                                COUNT == iter->eKeyType)
                            {
                                outputText("-shielded_count", "4");
                                continue;
                            }
                            std::string sNumExp;
                            if(!getNumberExp(iter, end, sNumExp))
                                sNumExp = "4";
                            outputText("-shielded_count", sNumExp.c_str());
                        }
                        else
                        {
                            outputText("-shielded_count", "4");
                        }
                        continue;
                    }
                    case OPPOSITE_EXTENDED:
                    {
                        outputText("-metric", "opposite_extended");

                        std::string sNumExp;
                        if(getNumberExp(++iter, end, sNumExp))
                            outputText(NULL, sNumExp.c_str());
                        continue;
                    }
                    case OPPOSITE:
                        outputText("-metric", "opposite");
                        ++iter;
                        continue;
                    case SQUARE:
                        outputText("-metric", "square");
                        ++iter;
                        continue;
                    case PROJECTING:
                    {
                        std::string sCons;
                        if(getConstraint(++iter, end, sCons))
                        {
                            outputText("-project", sCons.c_str());
                        }
                        continue;
                    }
                    case COUNT:
                    {
                        std::string sNumExp;
                        if(getNumberExp(++iter, end, sNumExp))
                            outputText("-count", sNumExp.c_str());
                        continue;
                    }
                    case WIDTH1:
                    {
                        std::string sCons;
                        if(getConstraint(++iter ,end, sCons))
                            outputText("-width_limit1", sCons.c_str());
                        continue;
                    }
                    case WIDTH2:
                    {
                        std::string sCons;
                        if(getConstraint(++iter ,end, sCons))
                            outputText("-width_limit2", sCons.c_str());
                        continue;
                    }
                    default:
                        return;
                }
            }
            default:
                return;
        }
        ++iter;
    }
}

void
rcsSynNodeConvertor_T::convertTableDRCWidthRule(std::list<rcsToken_T>::iterator &iter,
                                                std::list<rcsToken_T>::iterator end)
{
    m_oPVRSStream << "\\" << std::endl;

    bool hasWidthCons = false;
    while(iter != end)
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                switch(iter->eKeyType)
                {
                    case WIDTH:
                    {
                        if(hasWidthCons)
                            return;

                        hasWidthCons = true;
                        std::string sCons;
                        if(getConstraint(++iter ,end, sCons))
                            outputText("-width_limit", sCons.c_str());
                        continue;
                    }
                    case OPPOSITE_EXTENDED:
                    {
                        outputText("-metric", "opposite_extended");

                        std::string sNumExp;
                        if(getNumberExp(++iter, end, sNumExp))
                            outputText(NULL, sNumExp.c_str());
                        continue;
                    }
                    case OPPOSITE:
                        outputText("-metric", "opposite");
                        ++iter;
                        continue;
                    case SQUARE:
                        outputText("-metric", "square");
                        ++iter;
                        continue;
                    case PROJECTING:
                    {
                        std::string sCons;
                        if(getConstraint(++iter, end, sCons))
                        {
                            outputText("-project", sCons.c_str());
                        }
                        continue;
                    }
                    default:
                        return;
                }
            }
            default:
                return;
        }
        ++iter;
    }
}

void
rcsSynNodeConvertor_T::convertCheckTableDRCCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "check_table_drc ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case REGION:
                    outputText("-output", "region", nOption++);
                    ++iter;
                    continue;
                case BY_POLYGON:
                    outputText("-measure_option", "polygon", nOption++);
                    ++iter;
                    continue;
                case MEASURE_ALL:
                    outputText(NULL, "all", nOption++);
                    ++iter;
                    continue;
                case SPACE:
                    convertTableDRCSpaceRule(iter, pNode->end());
                    continue;
                case WIDTH:
                    convertTableDRCWidthRule(iter, pNode->end());
                    continue;
                default:
                    break;
            }
        }
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText("-in_layer", sLayerName.c_str(), nOption++);
                }
                continue;
            }
            case BUILT_IN_LANG:
            {
                std::string sValue = iter->sValue;
                if(sValue.find("rcn::built_in { ") != std::string::npos)
                {
                    std::string::size_type begin = sValue.find("rcn::built_in { ");
                    std::string::size_type end = sValue.find_last_of("}");
                    Utassert(end != std::string::npos);
                    sValue = sValue.substr(begin + 15, end - begin - 15);
                    trim(sValue);
                    sValue.insert(0, "\\[");
                    if(m_iProcLevel > 0)
                    {
                        sValue.insert(0, "\\\\");
                        sValue.insert(sValue.size(), "\\\\");
                    }
                    sValue.insert(sValue.size(), "\\]");
                }
                else
                {
                    sValue.erase(0, 1);
                    sValue.erase(sValue.size() - 1);
                    trim(sValue);
                    sValue.insert(0, "\\[");
                    if(m_iProcLevel > 0)
                    {
                        sValue.insert(0, "\\\\");
                        sValue.insert(sValue.size(), "\\\\");
                    }
                    sValue.insert(sValue.size(), "\\]");
                }
                outputText("-in_layer", sValue.c_str(), nOption++);

                break;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}


static bool
isValidDensitySecondaryKey(const rcsToken_T &token)
{
    if(token.eType == SECONDARY_KEYWORD)
    {
        switch(token.eKeyType)
        {
            case TRUNCATE:
            case BACKUP:
            case IGNORE:
            case WRAP:
            case INSIDE_OF_EXTENT:
            case BY_EXTENT:
            case BY_POLYGON:
            case BY_RECTANGLE:
            case WINDOW:
            case STEP:
            case INSIDE_OF:
            case INSIDE_OF_LAYER:
            case CENTERED:
            case GRADIENT:
            case RELATIVE:
            case ABSOLUTE:
            case CORNER:
            case MAGNITUDE:
            case CENTERS:
            case PRINT:
            case RDB:
            case MAG:
            case COMBINE:
                return true;
            default:
                return false;
        }
    }
    else if(LAYER_OPERATION_KEYWORD == token.eType || BUILT_IN_LANG == token.eType)
    {
        return true;
    }
    return false;
}

void
rcsSynNodeConvertor_T::convertCheckDensityCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "check_density ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case TRUNCATE:
                {
                    outputText("-limit", "truncate", nOption++);
                    ++iter;
                    continue;
                }
                case BACKUP:
                {
                    outputText("-limit", "backup", nOption++);
                    ++iter;
                    continue;
                }
                case IGNORE:
                {
                    outputText("-limit", "ignore", nOption++);
                    ++iter;
                    continue;
                }
                case WRAP:
                {
                    outputText("-limit", "wrap", nOption++);
                    ++iter;
                    continue;
                }
                case INSIDE_OF_EXTENT:
                {
                    outputText("-inside_box", "extent", nOption++);
                    ++iter;
                    continue;
                }
                case INSIDE_OF:
                {
                    std::string sLeftX;
                    std::string sBottomY;
                    std::string sRightX;
                    std::string sTopY;
                    ++iter;
                    getNumberExp(iter, pNode->end(), sLeftX, true);
                    getNumberExp(iter, pNode->end(), sBottomY, true);
                    getNumberExp(iter, pNode->end(), sRightX, true);
                    getNumberExp(iter, pNode->end(), sTopY, true);

                    outputText("-inside_box", sLeftX.c_str(), nOption);
                    outputText(NULL, sBottomY.c_str(), nOption);
                    outputText(NULL, sRightX.c_str(), nOption);
                    outputText(NULL, sTopY.c_str(), nOption++);
                    continue;
                }
                case INSIDE_OF_LAYER:
                {
                    ++iter;
                    std::string sLayerName;
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText("-window_layer", sLayerName.c_str(), nOption++);
                    }
                    continue;
                }
                case BY_EXTENT:
                {
                    outputText(NULL, "by_extent", nOption);
                    ++iter;
                    continue;
                }
                case BY_POLYGON:
                {
                    outputText(NULL, "by_polygon", nOption);
                    ++iter;
                    continue;
                }
                case BY_RECTANGLE:
                {
                    outputText(NULL, "by_rectangle", nOption);
                    ++iter;
                    continue;
                }
                case CORNER:
                {
                    outputText(NULL, "corner", nOption);
                    ++iter;
                    continue;
                }
                case WINDOW:
                {
                    std::vector<std::string> vFValues;
                    parseFollowNums(iter, pNode->end(), vFValues,
                                    isValidDensitySecondaryKey);
                    if(vFValues.size() > 1)
                    {
                        outputText("-window_size", vFValues[0].c_str(), nOption++);
                        outputText(NULL, vFValues[1].c_str(), nOption);
                    }
                    else
                    {
                        outputText("-window_size", vFValues[0].c_str(), nOption++);
                    }
                    continue;
                }
                case STEP:
                {
                    std::vector<std::string> vFValues;
                    parseFollowNums(iter, pNode->end(), vFValues,
                                    isValidDensitySecondaryKey);
                    if(vFValues.size() > 1)
                    {
                        outputText("-step_value", vFValues[0].c_str(), nOption++);
                        outputText(NULL, vFValues[1].c_str(), nOption);
                    }
                    else
                    {
                        outputText("-step_value", vFValues[0].c_str(), nOption++);
                    }
                    continue;
                }
                case CENTERS:
                {
                    std::vector<std::string> vFValues;
                    parseFollowNums(iter, pNode->end(), vFValues,
                                    isValidDensitySecondaryKey, 1, 1);
                    outputText("-centers", vFValues[0].c_str(), nOption++);
                    continue;
                }
                case MAG:
                {
                    std::vector<std::string> vFValues;
                    parseFollowNums(iter, pNode->end(), vFValues,
                                    isValidDensitySecondaryKey, 1, 1);
                    outputText("-rdb_mag", vFValues[0].c_str(), nOption++);
                    continue;
                }
                case CENTERED:
                {
                    std::vector<std::string> vFValues;
                    parseFollowNums(iter, pNode->end(), vFValues,
                                    isValidDensitySecondaryKey, 1, 1);
                    outputText("centered", vFValues[0].c_str(), nOption++);
                    continue;
                }
                case GRADIENT:
                case MAGNITUDE:
                case COMBINE:
                {
                    std::string sOption;
                    if(iter->eKeyType == GRADIENT)
                        sOption = "-gradient_option";
                    else if(iter->eKeyType == MAGNITUDE)
                        sOption = "-magnitude_option";
                    else
                        sOption = "-combine_option";

                    if(++iter != pNode->end() && !isValidDensitySecondaryKey(*iter)
                       && isConstraintOperator(*iter))
                    {
                        std::string sConstraint;
                        if(getConstraint(iter, pNode->end(), sConstraint))
                            outputText(sOption.c_str(), sConstraint.c_str(), nOption++);
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP12,
                                            iter->nLineNo, iter->sValue);
                    }
                    continue;
                }
                case RELATIVE:
                {
                    outputText(NULL, "relative", nOption);
                    ++iter;
                    continue;
                }
                case ABSOLUTE:
                {
                    outputText(NULL, "absolute", nOption);
                    ++iter;
                    continue;
                }
                case PRINT:
                {
                    ++iter;
                    bool isOnly = false;
                    if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == ONLY)
                    {
                        isOnly = true;
                        ++iter;
                    }

                    std::string sFileName;
                    getStringName(iter, pNode->end(), sFileName);
                    if(!isOnly)
                        outputText("-log_file", sFileName.c_str(), nOption++);
                    else
                        outputText("-only_log_file", sFileName.c_str(), nOption++);
                    ++iter;
                    continue;
                }
                case RDB:
                {
                    ++iter;
                    bool isOnly = false;
                    if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == ONLY)
                    {
                        isOnly = true;
                        ++iter;
                    }

                    std::string sFileName;
                    getStringName(iter, pNode->end(), sFileName);
                    if(!isOnly)
                        outputText("-db_file", sFileName.c_str(), nOption++);
                    else
                        outputText("-only_db_file", sFileName.c_str(), nOption++);

                    ++iter;
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case BUILT_IN_LANG:
            {
                std::string sValue = iter->sValue;
                sValue.erase(0, 1);
                sValue.insert(0, "{ ");
                sValue.erase(sValue.size() - 1);
                sValue.insert(sValue.size(), " }");
                outputText("-exp", sValue.c_str(), nOption++);
                break;
            }
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
                else if(iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    outputText("-density_limit", sConstraint.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertCheckWidthCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    if(pNode->getOperationType() == WITH_WIDTH)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_select_by_width ";
    else
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_not_select_by_width ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
                else if(iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    outputText("-width_limit", sConstraint.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

static bool
isDimensionalCheckKeyword(const std::list<rcsToken_T>::iterator iter,
                          const std::list<rcsToken_T>::iterator end)
{
    if(iter == end || iter->eType == LAYER_OPERATION_KEYWORD)
    {
        return true;
    }
    else if(iter->eType == SECONDARY_KEYWORD)
    {
        switch(iter->eKeyType)
        {
            case SQUARE:
            case SQUARE_ORTHOGONAL:
            case OPPOSITE:
            case OPPOSITE1:
            case OPPOSITE2:
            case OPPOSITE_EXTENDED:
            case OPPOSITE_EXTENDED1:
            case OPPOSITE_EXTENDED2:
            case OPPOSITE_SYMMETRIC:
            case OPPOSITE_EXTENDED_SYMMETRIC:
            case OPPOSITE_FSYMMETRIC:
            case OPPOSITE_EXTENDED_FSYMMETRIC:
            case ACUTE_ALSO:
            case ACUTE_ONLY:
            case NOT_ACUTE:
            case PARALLEL_ALSO:
            case PARALLEL_ONLY:
            case NOT_PARALLEL:
            case PERPENDICULAR_ALSO:
            case PERPENDICULAR_ONLY:
            case NOT_PERPENDICULAR:
            case NOT_OBTUSE:
            case OBTUSE_ONLY:
            case OBTUSE_ALSO:
            case PROJECTING:
            case NOT_PROJECTING:
            case ANGLED:
            case CORNER_TO_CORNER:
            case CORNER_TO_EDGE:
            case NOT_CORNER:
            case CORNER:
            case REGION:
            case REGION_EXTENTS:
            case REGION_CENTERLINE:
            case MEASURE_ALL:
            case MEASURE_COINCIDENT:
            case NOTCH:
            case SPACE:
            case CONNECTED:
            case NOT_CONNECTED:
            case ABUT:
            case OVERLAP:
            case SINGULAR:
            case INSIDE_ALSO:
            case OUTSIDE_ALSO:
            case EXCLUDE_FALSE:
            case EXCLUDE_SHIELDED:
            case INTERSECTING_ONLY:
                return true;
            default:
            {
                if(!isValidSvrfName(iter->sValue))
                {
                    return true;
                }
                break;
            }
        }
    }
    else if(iter->eType == IDENTIFIER_NAME && !isValidSvrfName(iter->sValue))
    {
        return true;
    }
    return false;
}

void
rcsSynNodeConvertor_T::convertCheckSpaceCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "check_space ";
    if(pNode->getOperationType() == INTERNAL)
        outputText("-type", "int", nOption++);
    else if(pNode->getOperationType() == EXTERNAL)
        outputText("-type", "ext", nOption++);
    else if(pNode->getOperationType() == ENCLOSURE)
        outputText("-type", "enc", nOption++);

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case SQUARE:
                    outputText("-metric", "square", nOption++);
                    ++iter;
                    continue;
                case SQUARE_ORTHOGONAL:
                    outputText("-metric", "square_orth", nOption++);
                    ++iter;
                    continue;
                case OPPOSITE:
                    outputText("-metric", "opposite", nOption++);
                    ++iter;
                    continue;
                case OPPOSITE1:
                    outputText("-metric", "opposite1", nOption++);
                    ++iter;
                    continue;
                case OPPOSITE2:
                    outputText("-metric", "opposite2", nOption++);
                    ++iter;
                    continue;
                case OPPOSITE_SYMMETRIC:
                    outputText("-metric", "opposite_symmetric", nOption++);
                    ++iter;
                    continue;
                case OPPOSITE_FSYMMETRIC:
                    outputText("-metric", "opposite_fsymmetric", nOption++);
                    ++iter;
                    continue;
                case OPPOSITE_EXTENDED:
                {
                    outputText("-metric", "opposite_extended", nOption++);
                    ++iter;
                    if(iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if(getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            outputText(NULL, sNumExp.c_str(), nOption);
                        }
                    }
                    continue;
                }
                case OPPOSITE_EXTENDED1:
                {
                    outputText("-metric", "opposite_extended1", nOption++);
                    ++iter;
                    if(iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if(getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            outputText(NULL, sNumExp.c_str(), nOption);
                        }
                    }
                    continue;
                }
                case OPPOSITE_EXTENDED2:
                {
                    outputText("-metric", "opposite_extended2", nOption++);
                    ++iter;
                    if(iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if(getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            outputText(NULL, sNumExp.c_str(), nOption);
                        }
                    }
                    continue;
                }
                case OPPOSITE_EXTENDED_SYMMETRIC:
                {
                    outputText("-metric", "opposite_extended_symmetric", nOption++);
                    ++iter;
                    if(iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if(getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            outputText(NULL, sNumExp.c_str(), nOption);
                        }
                    }
                    continue;
                }
                case OPPOSITE_EXTENDED_FSYMMETRIC:
                {
                    outputText("-metric", "opposite_extended_fsymmetric", nOption++);
                    ++iter;
                    if(iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if(getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            outputText(NULL, sNumExp.c_str(), nOption);
                        }
                    }
                    continue;
                }
                case ACUTE_ALSO:
                case ACUTE_ONLY:
                case NOT_ACUTE:
                case PARALLEL_ALSO:
                case PARALLEL_ONLY:
                case NOT_PARALLEL:
                case PERPENDICULAR_ALSO:
                case PERPENDICULAR_ONLY:
                case NOT_PERPENDICULAR:
                case NOT_OBTUSE:
                case OBTUSE_ONLY:
                case OBTUSE_ALSO:
                    ++iter;
                    continue;
                case PROJECTING:
                {
                    ++iter;
                    if(iter != pNode->end() && isConstraintOperator(*iter))
                    {
                        std::string sCons;
                        if(getConstraint(iter, pNode->end(), sCons))
                        {
                            continue;
                        }
                    }
                    continue;
                }
                case NOT_PROJECTING:
                    ++iter;
                    continue;
                case ANGLED:
                {
                    ++iter;
                    if(iter != pNode->end() && isConstraintOperator(*iter))
                    {
                        std::string sCons;
                        if(getConstraint(iter, pNode->end(), sCons))
                        {
                            continue;
                        }
                    }
                    continue;
                }
                case CORNER_TO_CORNER:
                {
                    ++iter;
                    if(iter != pNode->end() && isConstraintOperator(*iter))
                    {
                        std::list<rcsToken_T>::iterator begin = iter;
                        std::string sCons;
                        if(getConstraint(iter, pNode->end(), sCons))
                        {
                            continue;
                        }
                    }
                    continue;
                }
                case CORNER_TO_EDGE:
                    ++iter;
                    continue;
                case NOT_CORNER:
                    ++iter;
                    continue;
                case CORNER:
                    ++iter;
                    continue;
                case REGION:
                    ++iter;
                    continue;
                case REGION_EXTENTS:
                    ++iter;
                    continue;
                case REGION_CENTERLINE:
                {
                    ++iter;
                    if(iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if(getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            continue;
                        }
                    }
                    continue;
                }
                case MEASURE_ALL:
                    ++iter;
                    continue;
                case MEASURE_COINCIDENT:
                    ++iter;
                    continue;
                case NOTCH:
                    ++iter;
                    continue;
                case SPACE:
                    ++iter;
                    continue;
                case CONNECTED:
                    ++iter;
                    continue;
                case NOT_CONNECTED:
                    ++iter;
                    continue;
                case ABUT:
                {
                    ++iter;
                    if(iter != pNode->end() && isConstraintOperator(*iter))
                    {
                        std::string sCons;
                        if(getConstraint(iter, pNode->end(), sCons))
                        {
                            continue;
                        }
                    }
                    continue;
                }
                case OVERLAP:
                    ++iter;
                    continue;
                case SINGULAR:
                    ++iter;
                    continue;
                case INSIDE_ALSO:
                    ++iter;
                    continue;
                case OUTSIDE_ALSO:
                    ++iter;
                    continue;
                case EXCLUDE_FALSE:
                case EXCLUDE_FALSE_NOTCH:
                    ++iter;
                    continue;
                case EXCLUDE_SHIELDED:
                {
                    if(!isDimensionalCheckKeyword(++iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if(getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            continue;
                        }
                    }
                    continue;
                }
                case INTERSECTING_ONLY:
                    ++iter;
                    continue;
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
                else if(iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    outputText("-space_limit", sConstraint.c_str(), nOption++);
                    continue;
                }
            }
            case BUILT_IN_LANG:
            {
                std::string sValue = iter->sValue;
                if(sValue.find("rcn::built_in { ") != std::string::npos)
                {
                    std::string::size_type begin = sValue.find("rcn::built_in { ");
                    std::string::size_type end = sValue.find_last_of("}");
                    Utassert(end != std::string::npos);
                    sValue = sValue.substr(begin + 15, end - begin - 15);
                    trim(sValue);
                    sValue.insert(0, "\\[");
                    if(m_iProcLevel > 0)
                    {
                        sValue.insert(0, "\\\\");
                        sValue.insert(sValue.size(), "\\\\");
                    }
                    sValue.insert(sValue.size(), "\\]");
                }
                else
                {
                    sValue.erase(0, 1);
                    sValue.erase(sValue.size() - 1);
                    trim(sValue);
                    sValue.insert(0, "\\[");
                    if(m_iProcLevel > 0)
                    {
                        sValue.insert(0, "\\\\");
                        sValue.insert(sValue.size(), "\\\\");
                    }
                    sValue.insert(sValue.size(), "\\]");
                }
                outputText("-in_layer", sValue.c_str(), nOption++);

                break;
            }
            case SEPARATOR:
            {
                if(iter->sValue == "(")
                {
                    std::string sValue;
                    while(iter != pNode->end())
                    {
                        sValue += iter->sValue;

                        if(iter->eType == SEPARATOR && iter->sValue == ")")
                            break;
                        ++iter;
                    }
                    outputText("-in_layer", sValue.c_str(), nOption++);
                    break;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }

        ++iter;
    }

    iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case SQUARE:
                    ++iter;
                    continue;
                case SQUARE_ORTHOGONAL:
                    ++iter;
                    continue;
                case OPPOSITE:
                    ++iter;
                    continue;
                case OPPOSITE1:
                    ++iter;
                    continue;
                case OPPOSITE2:
                    ++iter;
                    continue;
                case OPPOSITE_SYMMETRIC:
                    ++iter;
                    continue;
                case OPPOSITE_FSYMMETRIC:
                    ++iter;
                    continue;
                case OPPOSITE_EXTENDED:
                {
                    ++iter;
                    if(iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if(getNumberExp(iter, pNode->end(), sNumExp))
                        {
                        }
                    }
                    continue;
                }
                case OPPOSITE_EXTENDED1:
                {
                    ++iter;
                    if(iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if(getNumberExp(iter, pNode->end(), sNumExp))
                        {
                        }
                    }
                    continue;
                }
                case OPPOSITE_EXTENDED2:
                {
                    ++iter;
                    if(iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if(getNumberExp(iter, pNode->end(), sNumExp))
                        {
                        }
                    }
                    continue;
                }
                case OPPOSITE_EXTENDED_SYMMETRIC:
                {
                    ++iter;
                    if(iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if(getNumberExp(iter, pNode->end(), sNumExp))
                        {
                        }
                    }
                    continue;
                }
                case OPPOSITE_EXTENDED_FSYMMETRIC:
                {
                    ++iter;
                    if(iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if(getNumberExp(iter, pNode->end(), sNumExp))
                        {
                        }
                    }
                    continue;
                }
                case ACUTE_ALSO:
                    outputText("-acute", "also", nOption++);
                    ++iter;
                    continue;
                case ACUTE_ONLY:
                    outputText("-acute", "only", nOption++);
                    ++iter;
                    continue;
                case NOT_ACUTE:
                    outputText("-acute", "no", nOption++);
                    ++iter;
                    continue;
                case PARALLEL_ALSO:
                    outputText("-para", "also", nOption++);
                    ++iter;
                    continue;
                case PARALLEL_ONLY:
                    outputText("-para", "only", nOption++);
                    ++iter;
                    continue;
                case NOT_PARALLEL:
                    outputText("-para", "no", nOption++);
                    ++iter;
                    continue;
                case PERPENDICULAR_ALSO:
                    outputText("-perp", "also", nOption++);
                    ++iter;
                    continue;
                case PERPENDICULAR_ONLY:
                    outputText("-perp", "only", nOption++);
                    ++iter;
                    continue;
                case NOT_PERPENDICULAR:
                    outputText("-perp", "no", nOption++);
                    ++iter;
                    continue;
                case NOT_OBTUSE:
                    outputText("-obtuse", "no", nOption++);
                    ++iter;
                    continue;
                case OBTUSE_ONLY:
                    outputText("-obtuse", "only", nOption++);
                    ++iter;
                    continue;
                case OBTUSE_ALSO:
                    outputText("-obtuse", "also", nOption++);
                    ++iter;
                    continue;
                case PROJECTING:
                {
                    ++iter;
                    if(iter != pNode->end() && isConstraintOperator(*iter))
                    {
                        std::string sCons;
                        if(getConstraint(iter, pNode->end(), sCons))
                        {
                            outputText("-proj", sCons.c_str(), nOption++);
                            continue;
                        }
                    }
                    outputText("-proj", NULL, nOption++);
                    continue;
                }
                case NOT_PROJECTING:
                    outputText("-proj", "no", nOption++);
                    ++iter;
                    continue;
                case ANGLED:
                {
                    ++iter;
                    if(iter != pNode->end() && isConstraintOperator(*iter))
                    {
                        std::string sCons;
                        if(getConstraint(iter, pNode->end(), sCons))
                        {
                            outputText("-angled", sCons.c_str(), nOption++);
                            continue;
                        }
                    }
                    outputText("-angled", NULL, nOption++);
                    continue;
                }
                case CORNER_TO_CORNER:
                {
                    ++iter;
                    if(iter != pNode->end() && isConstraintOperator(*iter))
                    {
                        std::list<rcsToken_T>::iterator begin = iter;
                        std::string sCons;
                        if(getConstraint(iter, pNode->end(), sCons))
                        {
                            if(begin->sValue == "!=")
                                outputText("-corner", "corner_neq_45", nOption++);
                            else if(begin->sValue == "==")
                                outputText("-corner", "corner_eq_45", nOption++);
                            continue;
                        }
                    }
                    outputText("-corner", "corner", nOption++);
                    continue;
                }
                case CORNER_TO_EDGE:
                    outputText("-corner", "edge", nOption++);
                    ++iter;
                    continue;
                case NOT_CORNER:
                    outputText("-corner", "no", nOption++);
                    ++iter;
                    continue;
                case CORNER:
                    outputText("-corner", "all", nOption++);
                    ++iter;
                    continue;
                case REGION:
                    outputText("-output", "region", nOption++);
                    ++iter;
                    continue;
                case REGION_EXTENTS:
                    outputText("-output", "region_extent", nOption++);
                    ++iter;
                    continue;
                case REGION_CENTERLINE:
                    outputText("-output", "region_centerline", nOption++);
                    ++iter;
                    if(iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if(getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            outputText(NULL, sNumExp.c_str(), nOption);
                            continue;
                        }
                    }
                    continue;
                case MEASURE_ALL:
                    outputText("-measure", "all", nOption++);
                    ++iter;
                    continue;
                case MEASURE_COINCIDENT:
                    outputText("-measure", "coin", nOption++);
                    ++iter;
                    continue;
                case NOTCH:
                    outputText("-poly", "notch", nOption++);
                    ++iter;
                    continue;
                case SPACE:
                    outputText("-poly", "space", nOption++);
                    ++iter;
                    continue;
                case CONNECTED:
                    outputText("-same_net", "yes", nOption++);
                    ++iter;
                    continue;
                case NOT_CONNECTED:
                    outputText("-same_net", "no", nOption++);
                    ++iter;
                    continue;
                case ABUT:
                {
                    ++iter;
                    if(iter != pNode->end() && isConstraintOperator(*iter))
                    {
                        std::string sCons;
                        if(getConstraint(iter, pNode->end(), sCons))
                        {
                            outputText("-abut", sCons.c_str(), nOption++);
                            continue;
                        }
                    }
                    outputText("-abut", NULL, nOption++);
                    continue;
                }
                case OVERLAP:
                    outputText("-overlap", NULL, nOption++);
                    ++iter;
                    continue;
                case SINGULAR:
                    outputText("-point_touch", NULL, nOption++);
                    ++iter;
                    continue;
                case INSIDE_ALSO:
                    outputText("-spec_output", "inside", nOption++);
                    ++iter;
                    continue;
                case OUTSIDE_ALSO:
                    outputText("-spec_output", "outside", nOption++);
                    ++iter;
                    continue;
                case EXCLUDE_FALSE:
                case EXCLUDE_FALSE_NOTCH:
                    outputText("-exclude_false_error", NULL, nOption++);
                    ++iter;
                    continue;
                case EXCLUDE_SHIELDED:
                {
                    if(!isDimensionalCheckKeyword(++iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if(getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            outputText("-exclude_shielded", sNumExp.c_str(), nOption);
                            continue;
                        }
                    }
                    outputText("-exclude_shielded", NULL, nOption++);
                    continue;
                }
                case INTERSECTING_ONLY:
                    outputText("-intersect", NULL, nOption++);
                    ++iter;
                    continue;
                default:
                    break;
            }
        }
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   continue;
                }
                else if(iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    continue;
                }
            }
            case BUILT_IN_LANG:
            {
                break;
            }
            case SEPARATOR:
            {
                if(iter->sValue == "(")
                {
                    std::string sValue;
                    while(iter != pNode->end())
                    {
                        sValue += iter->sValue;

                        if(iter->eType == SEPARATOR && iter->sValue == ")")
                            break;
                        ++iter;
                    }
                    break;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomSelectByNeighborCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    if(pNode->getOperationType() == WITH_NEIGHBOR)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_select_with_neighbor ";
    else if(pNode->getOperationType() == NOT_WITH_NEIGHBOR)
        m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_not_select_with_neighbor ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case ORTHOGONAL:
                case OCTAGONAL:
                {
                    std::list<rcsToken_T>::iterator begin = iter;
                    if(++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType && ONLY == iter->eKeyType)
                    {
                        if(begin->eKeyType == ORTHOGONAL)
                            outputText(NULL, "orth", nOption);
                        else
                            outputText(NULL, "octa", nOption);
                        ++iter;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                    continue;
                }
                case SPACE:
                {
                    std::string sConstraint;
                    if(getConstraint(++iter, pNode->end(), sConstraint))
                        outputText("-space_limit", sConstraint.c_str(), nOption++);
                    continue;
                }
                case SQUARE:
                    outputText(NULL, "square", nOption);
                    ++iter;
                    continue;
                case CENTERS:
                    outputText("-center", NULL, nOption++);
                    ++iter;
                    continue;
                case INSIDE_OF_LAYER:
                {
                    std::string sLayerName;
                    if(getLayerRefName(++iter, pNode->end(), sLayerName))
                    {
                       outputText("-inside_layer", sLayerName.c_str(), nOption++);
                    }
                    continue;
                }
                case CONNECTED:
                    outputText("-same_net", "yes", nOption++);
                    ++iter;
                    continue;
                case NOT_CONNECTED:
                    outputText("-same_net", "no", nOption++);
                    ++iter;
                    continue;
                default:
                    break;
            }
        }
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
                else if(iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    outputText("-limit", sConstraint.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::_parseOffsetOption(std::list<rcsToken_T>::iterator &iter,
                                          const std::list<rcsToken_T>::iterator end,
                                          hvUInt32 &nOption)
{
    std::list<rcsToken_T>::iterator begin = iter;

    outputText("-offset", NULL, nOption++);
    Utassert(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == OFFSET);
    ++iter;
    if(iter->eType == SECONDARY_KEYWORD && (iter->eKeyType == INSIDE_BY ||
       iter->eKeyType == OUTSIDE_BY))
    {
        bool isOutside = iter->eKeyType == OUTSIDE_BY;
        std::string sNumExp;
        if(getNumberExp(++iter, end, sNumExp))
        {
            if(isOutside)
                outputText("-outside_value", sNumExp.c_str(), nOption++);
            else
                outputText("-inside_value", sNumExp.c_str(), nOption++);
        }
        return;
    }
    else
    {
        std::string sXExp;
        std::string sYExp;
        if(getNumberExp(iter, end, sXExp, true))
        {
            outputText(NULL, sXExp.c_str(), nOption);
            if(getNumberExp(iter, end, sYExp, true))
            {
                outputText(NULL, sYExp.c_str(), nOption);
            }
        }
    }

    if(iter->eType == SECONDARY_KEYWORD && (iter->eKeyType == INSIDE_BY ||
        iter->eKeyType == OUTSIDE_BY))
     {
        bool isOutside = iter->eKeyType == OUTSIDE_BY;
        std::string sNumExp;
        if(getNumberExp(++iter, end, sNumExp))
        {
            if(isOutside)
                outputText("-outside_value", sNumExp.c_str(), nOption++);
            else
                outputText("-inside_value", sNumExp.c_str(), nOption++);
        }
        return;
    }
}

void
rcsSynNodeConvertor_T::convertCheckOffgridCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "check_offgrid ";

    bool hasLayerIn = false;
    bool hasXGrid = false;
    bool hasYGrid = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                switch(iter->eKeyType)
                {
                    case INSIDE_OF_LAYER:
                    {
                        std::string sLayerName;
                        if(getLayerRefName(++iter, pNode->end(), sLayerName))
                        {
                           outputText("-inside_layer", sLayerName.c_str(), nOption++);
                        }
                        continue;
                    }
                    case ABSOLUTE:
                    {
                        outputText(NULL, "absolute", nOption);
                        ++iter;
                        continue;
                    }
                    case RELATIVE:
                    {
                        outputText(NULL, "relative", nOption);
                        ++iter;
                        continue;
                    }
                    case CENTERS:
                    {
                        outputText("-centers", NULL, nOption++);
                        ++iter;
                        continue;
                    }
                    case EDGE:
                    {
                        outputText("-output", "edge", nOption++);
                        ++iter;
                        continue;
                    }
                    case REGION:
                    {
                        outputText("-output", "region", nOption++);
                        ++iter;
                        if(iter->eType != SECONDARY_KEYWORD || iter->eKeyType != INSIDE_OF_LAYER)
                        {
                            std::string sNumExp;
                            if(getNumberExp(iter, pNode->end(), sNumExp))
                                outputText(NULL, sNumExp.c_str(), nOption);
                        }
                        continue;
                    }
                    case OFFSET:
                    {
                        _parseOffsetOption(iter, pNode->end(), nOption);
                        continue;
                    }
                    case HINT:
                    {
                        outputText("-hint", NULL, nOption++);
                        ++iter;
                        continue;
                    }
                    default:
                        break;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            case SEPARATOR:
            {
                std::string sLayerName;
                if(!hasLayerIn && getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    hasLayerIn = true;
                    outputText("-in_layer", sLayerName.c_str(), nOption++);
                    continue;
                }
                else if(!hasXGrid)
                {
                    std::string sNumExp;
                    if(getNumberExp(iter, pNode->end(), sNumExp))
                    {
                        hasXGrid = true;
                        outputText("-grid", sNumExp.c_str(), nOption++);
                        continue;
                    }
                }
                else if(!hasYGrid)
                {
                    std::string sNumExp;
                    if(getNumberExp(iter, pNode->end(), sNumExp))
                    {
                        hasYGrid = true;
                        outputText(NULL, sNumExp.c_str(), nOption);
                        continue;
                    }
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertEdgeSelectByConvexCornerCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "edge_select_by_convex_corner ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case ANGLE1:
                {
                    std::string sConstraint;
                    getConstraint(++iter, pNode->end(), sConstraint);
                    continue;
                }
                case ANGLE2:
                {
                    std::string sConstraint;
                    getConstraint(++iter, pNode->end(), sConstraint);
                    continue;
                }
                case LENGTH1:
                {
                    std::string sConstraint;
                    getConstraint(++iter, pNode->end(), sConstraint);
                    continue;
                }
                case LENGTH2:
                {
                    std::string sConstraint;
                    getConstraint(++iter, pNode->end(), sConstraint);
                    continue;
                }
                case WITH_LENGTH:
                {
                    std::string sConstraint;
                    getConstraint(++iter, pNode->end(), sConstraint);
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
                else if(iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    outputText("-corner_limit", sConstraint.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case ANGLE1:
                {
                    std::string sConstraint;
                    getConstraint(++iter, pNode->end(), sConstraint);
                    outputText("-corner_angle1_limit", sConstraint.c_str(), nOption++);
                    continue;
                }
                case ANGLE2:
                {
                    std::string sConstraint;
                    getConstraint(++iter, pNode->end(), sConstraint);
                    outputText("-corner_angle2_limit", sConstraint.c_str(), nOption++);
                    continue;
                }
                case LENGTH1:
                {
                    std::string sConstraint;
                    getConstraint(++iter, pNode->end(), sConstraint);
                    outputText("-touch_edge1_limit", sConstraint.c_str(), nOption++);
                    continue;
                }
                case LENGTH2:
                {
                    std::string sConstraint;
                    getConstraint(++iter, pNode->end(), sConstraint);
                    outputText("-touch_edge2_limit", sConstraint.c_str(), nOption++);
                    continue;
                }
                case WITH_LENGTH:
                {
                    std::string sConstraint;
                    getConstraint(++iter, pNode->end(), sConstraint);
                    outputText("-length_limit", sConstraint.c_str(), nOption++);
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   continue;
                }
                else if(iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertEdgeSelectByAttrCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "edge_select_by_attr ";
    switch(pNode->getOperationType())
    {
        case PATH_LENGTH:
            outputText("-attr", "path_length", nOption++);
            break;
        case LENGTH:
            outputText("-attr", "length", nOption++);
            break;
        case NOT_LENGTH:
            outputText("-attr", "not_length", nOption++);
            break;
        case ANGLE:
            outputText("-attr", "angle", nOption++);
            break;
        case NOT_ANGLE:
            outputText("-attr", "not_angle", nOption++);
            break;
        default:
            break;
    }

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
                else if(iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    outputText("-limit", sConstraint.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertEdgeMergeCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "edge_merge ";
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertCheckFittingRectangleCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix();
    switch(pNode->getOperationType())
    {
        case ENCLOSE_RECTANGLE:
            m_oPVRSStream << "geom_check_fitting_rectangle ";
            break;
        case NOT_ENCLOSE_RECTANGLE:
            m_oPVRSStream << "geom_check_not_fitting_rectangle ";
            break;
        default:
            break;
    }

    hvUInt32 iLayer = 0;
    bool hasWidth  = false;
    bool hasLength = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if(ORTHOGONAL == iter->eKeyType)
                {
                    if(++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                       ONLY == iter->eKeyType)
                    {
                        outputText("-orth", "yes", nOption++);
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                    break;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            case SEPARATOR:
            {
                if(iLayer == 0)
                {
                    std::string sLayerName;
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        ++iLayer;
                       outputText("-in_layer", sLayerName.c_str(), nOption++);
                       continue;
                    }
                }
                else if(!hasWidth)
                {
                    std::string sNumExp;
                    if(getNumberExp(iter, pNode->end(), sNumExp))
                        outputText("-width", sNumExp.c_str(), nOption++);
                    hasWidth = true;
                }
                else if(!hasLength)
                {
                    std::string sNumExp;
                    if(getNumberExp(iter, pNode->end(), sNumExp))
                        outputText("-length", sNumExp.c_str(), nOption++);
                    hasLength = true;
                }
                else
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
                continue;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomSelectInCellCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix();
    switch(pNode->getOperationType())
    {
        case INSIDE_CELL:
            m_oPVRSStream << "geom_select_in_cell ";
            break;
        case NOT_INSIDE_CELL:
            m_oPVRSStream << "geom_not_select_in_cell ";
            break;
        default:
            break;
    }

    hvUInt32 iLayer = 0;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case PRIMARY:
                {
                   if(++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType
                      && ONLY == iter->eKeyType)
                   {
                       outputText("-depth", "primary", nOption++);
                       ++iter;
                       continue;
                   }
                   else
                   {
                       --iter;
                       throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                           iter->nLineNo, iter->sValue);
                   }
                    break;
                }
                case WITH_LAYER:
                {
                    ++iter;
                    std::string sLayerName;
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                       outputText("-with_layer_in_cell", sLayerName.c_str(), nOption++);
                       continue;
                    }
                    break;
                }
                case WITH_MATCH:
                    outputText("-match", NULL, nOption++);
                    ++iter;
                    continue;
                case GOLDEN:
                    outputCon2Error(pNode, *iter);
                    ++iter;
                    continue;
                case RULE:
                case WITH_PROPERTY:
                case NOT_WITH_PROPERTY:
                {
                    outputCon2Error(pNode, *iter);
                    ++iter;
                    ++iter;
                    continue;
                }
                default:
                    break;
            }
        }
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if(iLayer == 0)
                {
                    std::string sLayerName;
                    if(getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        ++iLayer;
                        outputText("-in_layer", sLayerName.c_str(), nOption++);
                        continue;
                    }
                }
                else if(iter->eType != NUMBER && iter->eType != OPERATOR)
                {
                    std::string sCellName;
                    if(getStringName(iter, pNode->end(), sCellName))
                    {
                        outputText("-cell_names", sCellName.c_str(), nOption++);
                        break;
                    }
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomSelectByCoinEdgeCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix();
    switch(pNode->getOperationType())
    {
        case WITH_EDGE:
            m_oPVRSStream << "geom_select_by_edge_coin ";
            break;
        case NOT_WITH_EDGE:
            m_oPVRSStream << "geom_not_select_by_edge_coin ";
            break;
        default:
            break;
    }

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
                else if(iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    outputText("-edge_limit", sConstraint.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomSelectRectangleCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix();
    switch(pNode->getOperationType())
    {
        case RECTANGLE:
            m_oPVRSStream << "geom_select_rectangle ";
            break;
        case NOT_RECTANGLE:
            m_oPVRSStream << "geom_not_select_rectangle ";
            break;
        default:
            break;
    }

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case BY:
                {
                    std::string sConstraint;
                    getConstraint(++iter, pNode->end(), sConstraint);
                    outputText("-length_limit", sConstraint.c_str(), nOption++);
                    continue;
                }
                case ASPECT:
                {
                    std::string sConstraint;
                    getConstraint(++iter, pNode->end(), sConstraint);
                    outputText("-aspect_ratio_limit", sConstraint.c_str(), nOption++);
                    continue;
                }
                case ORTHOGONAL:
                {
                    if(++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                       ONLY == iter->eKeyType)
                    {
                        outputText("-direction", "orth", nOption++);
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                    ++iter;
                    continue;
                }
                case MEASURE_EXTENTS:
                    outputText("-direction", "extent", nOption++);
                    ++iter;
                    continue;
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
                else if(iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    outputText("-width_limit", sConstraint.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertGeomAttrCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "geom_select_by_attr ";
    switch(pNode->getOperationType())
    {
        case AREA:
            outputText("-attr", "area", nOption++);
            break;
        case NOT_AREA:
            outputText("-attr", "not_area", nOption++);
            break;
        case PERIMETER:
            outputText("-attr", "perimeter", nOption++);
            break;
        case DONUT:
            outputText("-attr", "donut", nOption++);
            break;
        case NOT_DONUT:
            outputText("-attr", "not_donut", nOption++);
            break;
        case VERTEX:
            outputText("-attr", "vertex", nOption++);
            break;
        default:
            break;
    }

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
                else if(iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    outputText("-limit", sConstraint.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeConvertor_T::convertEdgeTopoCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    m_oPVRSStream << rcsManager_T::getInstance()->getPVRSCmdPrefix() << "edge_select_by_topo ";
    switch(pNode->getOperationType())
    {
        case INSIDE_EDGE:
            outputText("-type", "inside", nOption++);
            break;
        case NOT_INSIDE_EDGE:
            outputText("-type", "not_inside", nOption++);
            break;
        case OUTSIDE_EDGE:
            outputText("-type", "outside", nOption++);
            break;
        case NOT_OUTSIDE_EDGE:
            outputText("-type", "not_outside", nOption++);
            break;
        case COINCIDENT_EDGE:
            outputText("-type", "coin", nOption++);
            break;
        case NOT_COINCIDENT_EDGE:
            outputText("-type", "not_coin", nOption++);
            break;
        case COINCIDENT_INSIDE_EDGE:
            outputText("-type", "inside_coin", nOption++);
            break;
        case NOT_COINCIDENT_INSIDE_EDGE:
            outputText("-type", "not_inside_coin", nOption++);
            break;
        case COINCIDENT_OUTSIDE_EDGE:
            outputText("-type", "outside_coin", nOption++);
            break;
        case NOT_COINCIDENT_OUTSIDE_EDGE:
            outputText("-type", "not_outside_coin", nOption++);
            break;
        case TOUCH_EDGE:
            outputText("-type", "touch", nOption++);
            break;
        case NOT_TOUCH_EDGE:
            outputText("-type", "not_touch", nOption++);
            break;
        case TOUCH_INSIDE_EDGE:
            outputText("-type", "inside_touch", nOption++);
            break;
        case NOT_TOUCH_INSIDE_EDGE:
            outputText("-type", "not_inside_touch", nOption++);
            break;
        case TOUCH_OUTSIDE_EDGE:
            outputText("-type", "outside_touch", nOption++);
            break;
        case NOT_TOUCH_OUTSIDE_EDGE:
            outputText("-type", "not_outside_touch", nOption++);
            break;
        default:
            break;
    }

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if(iter->eKeyType == ENDPOINT)
                {
                    ++iter;
                    if(pNode->end() != iter && iter->eType == SECONDARY_KEYWORD &&
                       (iter->eKeyType == ONLY || iter->eKeyType == ALSO))
                    {
                        if(iter->eKeyType == ONLY)
                            outputText("-point_touch", "only", nOption++);
                        else
                            outputText("-point_touch", "also", nOption++);

                        ++iter;
                        continue;
                    }
                    else
                    {
                        --iter;
                        switch(pNode->getOperationType())
                        {
                            case TOUCH_EDGE:
                            case NOT_TOUCH_EDGE:
                            case TOUCH_INSIDE_EDGE:
                            case NOT_TOUCH_INSIDE_EDGE:
                            case TOUCH_OUTSIDE_EDGE:
                            case NOT_TOUCH_OUTSIDE_EDGE:
                                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                                    iter->nLineNo, iter->sValue);
                                break;
                            default:
                                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                    iter->nLineNo, iter->sValue);
                        }
                    }
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if(getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText("-in_layer", sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

static void
parseOperands(std::string &sValue, bool isFirst)
{
    hvUInt32 iValue = 0;
    while(iValue < sValue.size())
    {
        if(sValue[iValue] == '{' || sValue[iValue] == '}')
        {
            sValue.insert(iValue, "\\");
            ++iValue;
        }
        else if(!isFirst && sValue[iValue] == '$')
        {
            sValue.insert(iValue, "\\");
            ++iValue;
        }
        ++iValue;
    }

    if(sValue[0] != '$')
        toUpper(sValue);
}

void
rcsSynNodeConvertor_T::parseOperands(std::string &str, std::vector<std::string> &vOperands)
{
    trim(str);
    if(str.empty())
        return;

    if(str[0] == '\"' || str[0] == '\'')
    {
        std::string::size_type iRight = str.find_first_of(str[0], 1);
        if(iRight == (str.length() - 1))
        {
            std::string sValue = str.substr(1, iRight - 1);
            ::parseOperands(sValue, vOperands.empty());
            vOperands.push_back(sValue);
        }
        else if(iRight == std::string::npos)
        {
            std::string sValue = str.substr(1);
            ::parseOperands(sValue, vOperands.empty());
            vOperands.push_back(sValue);

            return;
        }
        else
        {
            std::string sValue = str.substr(1, iRight - 1);
            ::parseOperands(sValue, vOperands.empty());
            vOperands.push_back(sValue);

            std::string remainder = str.substr(iRight + 1);
            trim(remainder);
            if(0 == strncmp(remainder.c_str(), "//", 2))
            {
                return;
            }
            parseOperands(remainder, vOperands);
        }
    }
    else
    {
        std::string::size_type iRight = str.find_first_of(" \t\r\"\'", 1);
        std::string::size_type iComment = str.find("//", 0);
        if(0 == iComment)
            return;

        else if(iComment < iRight)
        {
            std::string sValue = str.substr(0, iComment);
            ::parseOperands(sValue, vOperands.empty());
            vOperands.push_back(sValue);
            return;
        }
        if(iRight == std::string::npos)
        {
            Utassert(iComment == std::string::npos);
            std::string sValue = str.substr(0);
            ::parseOperands(sValue, vOperands.empty());
            vOperands.push_back(sValue);
            return;
        }

        std::string sValue = str.substr(0, iRight);
        ::parseOperands(sValue, vOperands.empty());
        vOperands.push_back(sValue);

        std::string remainder = str.substr(iRight);
        parseOperands(remainder, vOperands);
    }
}

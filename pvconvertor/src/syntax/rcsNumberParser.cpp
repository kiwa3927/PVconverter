



#include <iostream>
#include <math.h>
#include "public/manager/rcErrorManager.h"
#include "rcsSynNodeConvertor.h"
#include "rcsSynPvrsConvertor.h"
#include "rcsNumberParser.h"



bool rcsExpressionParser_T::m_isNewPvrs = false;
bool rcsExpressionParser_T::m_isNegativeNumber = false;


bool
rcsExpressionParser_T::exp(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end, std::string &sNumExpr)
{
    std::string sLeftValue;
    if(!term(iter, end, sLeftValue))
        return false;

    while(iter != end &&iter->eType == OPERATOR &&
          (iter->sValue == "+" ||iter->sValue == "-"))
    {
        if(iter->sValue == "+")
        {
            sLeftValue += "+ ";
            std::string sRightValue;
            if(!term(++iter, end, sRightValue))
                return false;
            sLeftValue += sRightValue;
        }
        else if(iter->sValue == "-")
        {
        	if(rcsExpressionParser_T::m_isNegativeNumber)
        	{
        		sLeftValue += "-";
        	}else
        	{
        		sLeftValue += "- ";
        	}

            std::string sRightValue;
            if(!term(++iter, end, sRightValue))
                return false;
            sLeftValue += sRightValue;
        }
    }

    sNumExpr += sLeftValue;
    return true;
}

bool
rcsExpressionParser_T::term(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end, std::string &sNumExpr)
{
    std::string sLeftValue;
    if(!factor(iter, end, sLeftValue))
        return false;

    while(iter != end &&iter->eType == OPERATOR &&
          (iter->sValue == "/" ||iter->sValue == "*" ||
           iter->sValue == "%" || iter->sValue == "^"))
    {
        if(iter->sValue == "/")
        {
            sLeftValue += "/ ";
            std::string sRightValue;
            if(!factor(++iter, end, sRightValue))
                return false;
            sLeftValue += sRightValue;
        }
        else if(iter->sValue == "*")
        {
            sLeftValue += "* ";
            std::string sRightValue;
            if(!factor(++iter, end, sRightValue))
                return false;
            sLeftValue += sRightValue;
        }
        else if(iter->sValue == "%")
        {
            sLeftValue += "% ";
            std::string sRightValue;
            if(!factor(++iter, end, sRightValue))
                return false;
            sLeftValue += sRightValue;
        }
        else if(iter->sValue == "^")
        {
            if(m_isNewPvrs)
            {
                sLeftValue += "^ ";
                std::string sRightValue;
                if(!factor(++iter, end, sRightValue))
                    return false;
                sLeftValue += sRightValue;
            }
            else
            {
                std::string sTmpValue = sLeftValue;
                sLeftValue.clear();
                sLeftValue += "pow(";
                sLeftValue += sTmpValue;
                sLeftValue += ", ";

                std::string sRightValue;
                if(!factor(++iter, end, sRightValue))
                    return false;

                sLeftValue += sRightValue;
                sLeftValue += ") ";
            }
        }
    }

    sNumExpr += sLeftValue;
    return true;
}

bool
rcsExpressionParser_T::isFuncName(const std::string &funcName)
{
    //toUpper(str);
    // to check
    if (0 == strcasecmp(funcName.c_str(), "CEIL"))
    {
        return true;
    }
    else if (0 == strcasecmp(funcName.c_str(), "FLOOR"))
    {
        return true;
    }
    else if (0 == strcasecmp(funcName.c_str(), "TRUNC"))
    {
        return true;
    }
    else if (0 == strcasecmp(funcName.c_str(), "SQRT"))
    {
        return true;
    }
    else if (0 == strcasecmp(funcName.c_str(), "ABS"))
    {
        return true;
    }
    else if (0 == strcasecmp(funcName.c_str(), "EXP"))
    {
        return true;
    }
    else if (0 == strcasecmp(funcName.c_str(), "LOG"))
    {
        return true;
    }
    else if (0 == strcasecmp(funcName.c_str(), "SIN"))
    {
        return true;
    }
    else if (0 == strcasecmp(funcName.c_str(), "COS"))
    {
        return true;
    }
    else if (0 == strcasecmp(funcName.c_str(), "TAN"))
    {
        return true;
    }
    else if (0 == strcasecmp(funcName.c_str(), "MIN"))
    {
        return true;
    }
    else if (0 == strcasecmp(funcName.c_str(), "MAX"))
    {
        return true;
    }
    return false;
}

bool
rcsExpressionParser_T::parseFunc(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end, std::string &sNumExpr)
{
    // get func name
    sNumExpr += iter->sValue;
    //sNumExpr += " ";

    // skip and check end
    if (++iter == end)
        return false;

    // check (
    if (iter->eType != SEPARATOR || iter->sValue != "(")
        return false;

    // get (
    sNumExpr += "( ";

    // skip (
    ++iter;
    while (iter != end)
    {
        if(false == exp(iter, end, sNumExpr))
            return false;

        if (iter != end && iter->eType == SEPARATOR && iter->sValue == ",")
        {
            sNumExpr += ", ";;
            ++iter;
            continue;
        }
        break;
    }

    // check )
    if (iter == end || iter->eType != SEPARATOR || iter->sValue != ")")
        return false;

    // get )
    sNumExpr += ") ";;

    // next
    ++iter;
    return true;
}

bool
rcsExpressionParser_T::factor(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end, std::string &sNumExpr)
{
    if(iter == end)
        return false;

    switch(iter->eType)
    {
        case SEPARATOR:
        {
            if(iter->sValue == "(")
            {
                sNumExpr += iter->sValue;
                sNumExpr += " ";
                exp(++iter, end, sNumExpr);
                ++iter;
                sNumExpr += ") ";
                return true;
            }
            break;
        }
        case NUMBER:
        {
            if(iter->eNumberType == INT_NUMBER)
            {
                sNumExpr += itoa(iter->iValue);
                
            }
            else
            {
                sNumExpr += iter->sValue;
            }

            sNumExpr += " ";
            ++iter;
            return true;
        }
        case OPERATOR:
        {
            if(iter->sValue == "-")
            {
            	if(rcsExpressionParser_T::m_isNegativeNumber)
            	{
            		sNumExpr += "-";
            	}else
            	{
            		sNumExpr += "- ";
            	}
                return factor(++iter, end, sNumExpr);
            }
            else if(iter->sValue == "+")
            {
                return factor(++iter, end, sNumExpr);
            }
            break;
        }
        case IDENTIFIER_NAME:
        case SECONDARY_KEYWORD:
        {
            // to check func
            if (isFuncName(iter->sValue))
            {
                return parseFunc(iter, end, sNumExpr);
            }
            if(!isValidSvrfName(iter->sValue))
                return false;
        }
        case STRING_CONSTANTS:
        {
            if(m_isNewPvrs)
            {
                std::string sValue;
                rcsSynPvrsConvertor::getStringName(iter, end, sValue);
                sNumExpr += sValue;
            }
            else if(iter->sValue[0] != '$')
            {
                sNumExpr += "$";

                sNumExpr += "{::";
                sNumExpr += iter->sValue;
                sNumExpr += "} ";
                toLower(sNumExpr);
            }
            else if(strcasecmp("$precision", iter->sValue.c_str()) == 0 ||
                    strcasecmp("$precision_n", iter->sValue.c_str()) == 0 ||
                    strcasecmp("$precision_d", iter->sValue.c_str()) == 0)
            {
                std::string sValue = iter->sValue;
                sValue.erase(0, 1);

                sNumExpr += "$";
                sNumExpr += "{::";
                toUpper(sValue);
                sNumExpr += sValue;
                sNumExpr += "} ";
            }
            else if(iter->sValue.size() > 1 && iter->sValue[1] != '{')
            {
                std::string sValue = iter->sValue;
                sValue.erase(0, 1);

                sNumExpr += "$";
                sNumExpr += "{";
                sNumExpr += sValue;
                sNumExpr += "} ";
                toLower(sNumExpr);
            }
            else
            {
                sNumExpr += iter->sValue;
            }
            ++iter;
            return true;
        }
        default:
            break;
    }
    return false;
}

bool
rcsExpressionParser_T::parse(std::list<rcsToken_T>::iterator &iter,
                             std::list<rcsToken_T>::iterator end,
                             std::string &sNumExpr)
{
    return exp(iter, end, sNumExpr);
}

bool
rcsExpressionParser_T::parseNumberJustForNegative(std::list<rcsToken_T>::iterator &iter,
                                   std::list<rcsToken_T>::iterator end,
                                   std::string &sNumExpr)
{
    switch(iter->eType)
    {
        case IDENTIFIER_NAME:
        case SECONDARY_KEYWORD:
        {
            if(!isValidSvrfName(iter->sValue))
                return false;
        }
        case STRING_CONSTANTS:
        {
            if(m_isNewPvrs)
            {
                std::string sValue;
                rcsSynPvrsConvertor::getStringName(iter, end, sValue);
                sNumExpr += sValue;
            }
            else if(iter->sValue[0] != '$')
            {
                sNumExpr += "$";

                sNumExpr += "{::";
                sNumExpr += iter->sValue;
                sNumExpr += "} ";
                toLower(sNumExpr);
            }
            else if(strcasecmp("$precision", iter->sValue.c_str()) == 0 ||
                    strcasecmp("$precision_n", iter->sValue.c_str()) == 0 ||
                    strcasecmp("$precision_d", iter->sValue.c_str()) == 0)
            {
                std::string sValue = iter->sValue;
                sValue.erase(0, 1);

                sNumExpr += "$";
                sNumExpr += "{::";
                toUpper(sValue);
                sNumExpr += sValue;
                sNumExpr += "} ";
            }
            else if(iter->sValue.size() > 1 && iter->sValue[1] != '{')
            {
                std::string sValue = iter->sValue;
                sValue.erase(0, 1);

                sNumExpr += "$";
                sNumExpr += "{";
                sNumExpr += sValue;
                sNumExpr += "} ";
                toLower(sNumExpr);
            }
            else
            {
                sNumExpr += iter->sValue;
            }

            ++iter;
            return true;
        }
        case OPERATOR:
        {
            if("+" != iter->sValue && "-" != iter->sValue)
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR,INP1,iter->nLineNo,
                                    iter->sValue);
            }
            else
            {
                if(iter->sValue == "-")
                    sNumExpr += "-";

                hvUInt32 nLineNo = iter->nLineNo;
                ++iter;
                if(!(iter != end && iter->eType == NUMBER && iter->nLineNo == nLineNo))
                {
                    if(iter == end)
                        --iter;
                    throw rcErrorNode_T(rcErrorNode_T::ERROR,INP1,iter->nLineNo,
                                        iter->sValue);
                }
            }
        }
        case NUMBER:
        {
            if(iter->eNumberType == INT_NUMBER)
            {
                sNumExpr += itoa(iter->iValue);
                
            }
            else
            {
                sNumExpr += iter->sValue;
            }

            sNumExpr += " ";
            ++iter;
            return true;
        }
        default:
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo, iter->sValue);
            break;
        }
    }
    return false;
}

bool
rcsExpressionParser_T::parseNumber(std::list<rcsToken_T>::iterator &iter,
                                   std::list<rcsToken_T>::iterator end,
                                   std::string &sNumExpr)
{
    switch(iter->eType)
    {
        case IDENTIFIER_NAME:
        case SECONDARY_KEYWORD:
        {
            if(!isValidSvrfName(iter->sValue))
                return false;
        }
        case STRING_CONSTANTS:
        {
            if(m_isNewPvrs)
            {
                std::string sValue;
                rcsSynPvrsConvertor::getStringName(iter, end, sValue);
                sNumExpr += sValue;
            }
            else if(iter->sValue[0] != '$')
            {
                sNumExpr += "$";

                sNumExpr += "{::";
                sNumExpr += iter->sValue;
                sNumExpr += "} ";
                toLower(sNumExpr);
            }
            else if(strcasecmp("$precision", iter->sValue.c_str()) == 0 ||
                    strcasecmp("$precision_n", iter->sValue.c_str()) == 0 ||
                    strcasecmp("$precision_d", iter->sValue.c_str()) == 0)
            {
                std::string sValue = iter->sValue;
                sValue.erase(0, 1);

                sNumExpr += "$";
                sNumExpr += "{::";
                toUpper(sValue);
                sNumExpr += sValue;
                sNumExpr += "} ";
            }
            else if(iter->sValue.size() > 1 && iter->sValue[1] != '{')
            {
                std::string sValue = iter->sValue;
                sValue.erase(0, 1);

                sNumExpr += "$";
                sNumExpr += "{";
                sNumExpr += sValue;
                sNumExpr += "} ";
                toLower(sNumExpr);
            }
            else
            {
                sNumExpr += iter->sValue;
            }

            ++iter;
            return true;
        }
        case OPERATOR:
        {
            if("+" != iter->sValue && "-" != iter->sValue)
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR,INP1,iter->nLineNo,
                                    iter->sValue);
            }
            else
            {
                if(iter->sValue == "-")
                {
                	if(rcsExpressionParser_T::m_isNegativeNumber)
					{
						sNumExpr += "-";
					}else
					{
						sNumExpr += "- ";
					}
                }

                hvUInt32 nLineNo = iter->nLineNo;
                ++iter;
                if(!(iter != end && iter->eType == NUMBER && iter->nLineNo == nLineNo))
                {
                    if(iter == end)
                        --iter;
                    throw rcErrorNode_T(rcErrorNode_T::ERROR,INP1,iter->nLineNo,
                                        iter->sValue);
                }
            }
        }
        case NUMBER:
        {
            if(iter->eNumberType == INT_NUMBER)
            {
                sNumExpr += itoa(iter->iValue);
                
            }
            else
            {
                sNumExpr += iter->sValue;
            }

            sNumExpr += " ";
            ++iter;
            return true;
        }
        default:
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo, iter->sValue);
            break;
        }
    }
    return false;
}

void
rcsExpressionParser_T::setNewPvrs()
{
    m_isNewPvrs = true;
}

void
rcsExpressionParser_T::setNegativeNumber()
{
	m_isNegativeNumber = true;
}

void
rcsExpressionParser_T::unsetNegativeNumber()
{
	m_isNegativeNumber = false;
}

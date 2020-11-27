



#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "rcsSynNodePreConvertor.h"
#include "rcsSynNodeProConvertor.h"
#include "rcsSynPvrsConvertor.h"

#include "public/manager/rcErrorManager.h"
#include "public/util/rcsStringUtil.hpp"

#include "public/synnode/rcsSynRootNode.h"
#include "public/synnode/rcsSynRulecheckNode.h"
#include "public/synnode/rcsSynSpecificationNode.h"
#include "public/synnode/rcsSynLayerOperationNode.h"
#include "public/synnode/rcsSynLayerDefinitionNode.h"
#include "public/synnode/rcsSynCommentNode.h"
#include "public/synnode/rcsSynBuildinNode.h"
#include "public/synnode/rcsSynProcessNode.h"
#include "public/synnode/rcsSynProcNode.h"
#include "public/manager/rcsManager.h"
#include "rcsSynConvertor.h"
#include "main/rcsSvrf2Pvrs.h"
#include "public/synnode/rcssyndmacrodummynode.h"

rcsSynConvertor::rcsSynConvertor(std::list<rcsToken_T> &lTokenStream):
                 m_listTokenStream(lTokenStream)
{
}

static std::list<rcsToken_T>::iterator
getOperationNodeEnd(std::list<rcsToken_T>::iterator &iterKeyWord,
                    std::list<rcsToken_T>::iterator &iterCur)
{
    if(iterKeyWord->nLineNo == iterCur->nLineNo)
    {
        return iterCur;
    }
    else
    {
        std::list<rcsToken_T>::iterator iterEnd = iterKeyWord;
        while(iterEnd->nLineNo != iterCur->nLineNo)
        {
            ++iterEnd;
        }
        return iterEnd;
    }
}


static inline std::list<rcsToken_T>::iterator
removeCommentTokens(std::list<rcsToken_T> &tokens, std::list<rcsToken_T>::iterator &first,
                    const std::list<rcsToken_T>::iterator &last)
{
    if (!rcsManager_T::getInstance()->outputComment()) return first;
    std::list<rcsToken_T>::iterator firstAfterRemoval = last;
    while (first != last)
    {
        if (first->eType == COMMENT)
        {
            first = tokens.erase(first);
        }
        else
        {
            if (firstAfterRemoval == last) firstAfterRemoval = first;
            first++;
        }
    }
    return firstAfterRemoval;
}

static inline void
skipTailCommentTokens(std::list<rcsToken_T>::iterator &first, std::list<rcsToken_T>::iterator &last)
{
    while (first != last-- && last->eType == COMMENT)
    {}
    last++;
}

bool
rcsSynConvertor::parseLayerOperation(std::list<rcsToken_T>::iterator &iter)
{
    Utassert(iter->eType == LAYER_OPERATION_KEYWORD || iter->eType == CMACRO);
    std::list<rcsToken_T>::iterator iterPrimaryKeyWord = iter++;
    while(iter != m_listTokenStream.end())
    {
        if(LAYER_OPERATION_KEYWORD == iter->eType)
        {
            std::list<rcsToken_T>::iterator iterEnd = getOperationNodeEnd(iterPrimaryKeyWord, iter);
            iter = iterEnd;
            break;
        }
        else if(SPECIFICATION_KEYWORD == iter->eType || PRO_KEYWORD == iter->eType ||
                CMACRO == iter->eType || DMACRO == iter->eType)
        {
            break;
        }
        else if(COMMENT == iter->eType)
        {
            Utassert(rcsManager_T::getInstance()->outputComment());
            
        }
        else if(SEPARATOR == iter->eType)
        {
            if(("=" == iter->sValue) || ("{" == iter->sValue))
            {
                --iter;
                break;
            }
            else if("}" == iter->sValue)
            {
                break;
            }
        }
        ++iter;
    }

    
    skipTailCommentTokens(iterPrimaryKeyWord, iter);
    removeCommentTokens(m_listTokenStream, iterPrimaryKeyWord, iter);

    return true;
}

bool
rcsSynConvertor::parseSpecification(std::list<rcsToken_T>::iterator &iter)
{
    std::list<rcsToken_T>::iterator iterPrimaryKeyWord = iter++;
    while(iter != m_listTokenStream.end())
    {
        if(LAYER_OPERATION_KEYWORD == iter->eType)
        {
            std::list<rcsToken_T>::iterator iterEnd = getOperationNodeEnd(iterPrimaryKeyWord, iter);
            iter = iterEnd;
            break;
        }
        else if(SPECIFICATION_KEYWORD == iter->eType || PRO_KEYWORD == iter->eType ||
                CMACRO == iter->eType || DMACRO == iter->eType)
        {
            break;
        }
        else if(COMMENT == iter->eType)
        {
            Utassert(rcsManager_T::getInstance()->outputComment());
            
        }
        else if(SEPARATOR == iter->eType)
        {
            if(("=" == iter->sValue) || ("{" == iter->sValue))
            {
                --iter;
                break;
            }
            else if("}" == iter->sValue)
            {
                break;
            }
        }
        ++iter;
    }
    skipTailCommentTokens(iterPrimaryKeyWord, iter);
    removeCommentTokens(m_listTokenStream, iterPrimaryKeyWord, iter);

    return true;
}

bool
rcsSynConvertor::parseLayerDefinition(rcsSynLayerDefinitionNode_T *pLayerDefNode,
                                      std::list<rcsToken_T>::iterator &iter)
{
    Utassert(pLayerDefNode != NULL);
    Utassert(iter->eType == SEPARATOR && iter->sValue == "=");
    std::list<rcsToken_T>::iterator iterLayerDefName = iter;
    --iterLayerDefName;
    std::list<rcsToken_T>::iterator iterBegin = ++iter;
    while(iter != m_listTokenStream.end())
    {
        if(iter->eType == LAYER_OPERATION_KEYWORD || iter->eType == CMACRO)
        {
            rcsSynLayerOperationNode_T *pLayerOperationNode =
                    new rcsSynLayerOperationNode_T(iter->eKeyType, iter->eType == CMACRO);
            if(parseLayerOperation(iter))
            {
                iterBegin = removeCommentTokens(m_listTokenStream, iterBegin, iter);
                pLayerOperationNode->insertTokens(iterBegin, iter);
                pLayerDefNode->setLayerOperationNode(pLayerOperationNode);
                return true;
            }

            delete pLayerOperationNode;
            pLayerOperationNode = NULL;
            return false;
        }
        else if(SPECIFICATION_KEYWORD == iter->eType || PRO_KEYWORD == iter->eType)
        {
            break;
        }
        else if(COMMENT == iter->eType)
        {
            Utassert(rcsManager_T::getInstance()->outputComment());
            
        }
        else if(iter->eType == SEPARATOR)
        {
            if(("=" == iter->sValue) || ("{" == iter->sValue))
            {
                --iter;
                break;
            }
            else if("}" == iter->sValue)
            {
                break;
            }
        }























        ++iter;
    }

    if(iterBegin != iter)
    {
        
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::ERROR, SYN1, iterBegin->nLineNo));

    }
    else
    {




    }
    return false;
}

static bool isDevice_Auxiliary_layer(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end)
{
    if (iter == end || iter->eType != OPERATOR  || iter->sValue != "<")
    {
        return false;
    }

    if (++iter == end || iter->eType != IDENTIFIER_NAME)
    {
        return false;
    }

    if (++iter == end || iter->eType != OPERATOR || iter->sValue != ">")
    {
        return false;
    }

    ++iter;
    return true;
}

template<class Node_Type>
void
rcsSynConvertor::parseRuleCheck(Node_Type *pNode, std::list<rcsToken_T>::iterator &iter, bool isInMacro)
{
    Utassert(iter->eType == SEPARATOR && iter->sValue == "{");
    std::list<rcsToken_T>::iterator iterBegin = ++iter;
    while(iter != m_listTokenStream.end())
    {
        switch(iter->eType)
        {
            case CMACRO:
            case LAYER_OPERATION_KEYWORD:
            {
                rcsSynLayerOperationNode_T *pLayerOperationNode =
                        new rcsSynLayerOperationNode_T(iter->eKeyType, iter->eType == CMACRO);
                if(parseLayerOperation(iter))
                {
                    iterBegin = removeCommentTokens(m_listTokenStream, iterBegin, iter);
                    pLayerOperationNode->insertTokens(iterBegin, iter);
                    pNode->addChildNode(pLayerOperationNode);
                }
                else
                {
                    delete pLayerOperationNode;
                    pLayerOperationNode = NULL;
                }
                iterBegin = iter;
                continue;
            }
            case SPECIFICATION_KEYWORD:
            {
                std::list<rcsToken_T>::iterator iterKey = iter;
                rcsSynSpecificationNode_T *pSpecificationNode =
                        new rcsSynSpecificationNode_T(iter->eKeyType, true, isInMacro);
                parseSpecification(iter);

                pSpecificationNode->insertTokens(iterBegin, iter);
                pNode->addChildNode(pSpecificationNode);

                iterBegin = iter;
                continue;
            }
            case SEPARATOR:
            {
                if("=" == iter->sValue)
                {
                    
                    std::list<rcsToken_T>::iterator iLayerDefBegin = iter;
                    --iLayerDefBegin;
                    iterBegin = removeCommentTokens(m_listTokenStream, iterBegin, iter);
                    if(iterBegin != iLayerDefBegin)
                    {
                        
                        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::ERROR,
                                                            SYN1, iterBegin->nLineNo));
                    }
                    rcsSynLayerDefinitionNode_T *pLayerDefinitionNode =
                            new rcsSynLayerDefinitionNode_T();
                    pLayerDefinitionNode->insertTokens(iLayerDefBegin, iter);
                    if(parseLayerDefinition(pLayerDefinitionNode, iter))
                    {
                        pNode->addChildNode(pLayerDefinitionNode);
                    }
                    else
                    {
                        delete pLayerDefinitionNode;
                        pLayerDefinitionNode = NULL;
                    }
                    iterBegin = iter;

                    continue;
                }
                else if("}" == iter->sValue)
                {
                    
                    if(iterBegin != iter)
                    {
                        
                        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::ERROR,
                                                            SYN1, iterBegin->nLineNo));
                    }
                    ++iter;
                    return;
                }
                break;
            }
            case PRO_KEYWORD:
            {
                std::list<rcsToken_T>::iterator begin = iter;
                rcsSynPreProcessNode_T *pPreNode = new rcsSynPreProcessNode_T();
                pPreNode->insertTokens(begin, ++iter); 
                pNode->addChildNode(pPreNode);
                iterBegin = iter;

                continue;
            }
            case COMMENT:
            {
                Utassert(rcsManager_T::getInstance()->outputComment());
                std::list<rcsToken_T>::iterator begin = iter;
                rcsSynCommentNode_T *pCommentNode = new rcsSynCommentNode_T();
                pCommentNode->insertTokens(begin, ++iter);
                pNode->addChildNode(pCommentNode);
                iterBegin = iter;

                continue;
            }
            case BUILT_IN_LANG:
            {
            	if(isInMacro)
            	{
					std::list<rcsToken_T>::iterator begin = iter;
					rcsSynBuildInNode_T *pBuildInNode = new rcsSynBuildInNode_T();
					pBuildInNode->insertTokens(begin, ++iter);
					pNode->addChildNode(pBuildInNode);
					iterBegin = iter;
					continue;
            	}
            	else
            	{
            		break;
            	}
            }
            default:
            {
                if (isInMacro)
                {
                    std::list<rcsToken_T>::iterator endIter = iter;
                    if (isDevice_Auxiliary_layer(endIter, m_listTokenStream.end()))
                    {
                        rcsSynDMACRODummyNode_T *p = new rcsSynDMACRODummyNode_T;
                        p->insertTokens(iter, endIter);
                        pNode->addChildNode(p);
                        iter = endIter;
                        iterBegin = iter;
                        continue;
                    }
                }
            }
                break;
        }
        ++iter;
    }
    Utassert(false);
}

void
rcsSynConvertor::parseImplicitLayerDefinition(std::list<rcsToken_T>::iterator &iter,
                                              std::list<rcsToken_T>::iterator &iterInsert)
{

	std::string sImplicitLayerPrfix = rcsManager_T::getInstance()->getImplicitLayerPrefix() +
									  rcsManager_T::getInstance()->getTmpLayerSuffix() + "_";

    static hvUInt32 nTmpNo = rcsManager_T::getInstance()->getImplicitLayerID();
    bool isImplicitLayerDefinition = false;
    Utassert(iter->eType == SEPARATOR && iter->sValue == "(");
    std::list<rcsToken_T>::iterator iterBegin = iter++;
    while(iter != m_listTokenStream.end())
    {
        if((iter->eType == SEPARATOR))
        {
            if(iter->sValue == ")")
            {
                if(std::distance(iterBegin,iter) == 1)
                {
                    
                    s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, LAY6,
                                                        iter->nLineNo, iter->sValue));
                }
                else if(std::distance(iterBegin, iter) == 2)
                {
                    std::list<rcsToken_T>::iterator iterIn = iter;
                    --iterIn;

                    if(iterIn->eType == IDENTIFIER_NAME &&
                       0 == strncasecmp(iterIn->sValue.c_str(),
                       sImplicitLayerPrfix.c_str(), sImplicitLayerPrfix.size()))
                    {
                        
                        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL,
                                                            LAY6, iterBegin->nLineNo,
                                                            iterBegin->sValue));
                    }
                }

                if(isImplicitLayerDefinition)
                {
                    rcsManager_T::getInstance()->setHasTmpLayerDefinition(true);

                    std::string sTmpName = sImplicitLayerPrfix;
                    sTmpName += itoa(++nTmpNo);
                    rcsManager_T::getInstance()->setImplicitLayerID(nTmpNo);

                    hvUInt32 nLineNo = iterBegin->nLineNo;  
                    
                    m_listTokenStream.insert(iterInsert, rcsToken_T(IDENTIFIER_NAME,
                                             nLineNo, sTmpName.c_str()));
                    
                    m_listTokenStream.insert(iterInsert, rcsToken_T(SEPARATOR,
                                             nLineNo, "="));
                    
                    m_listTokenStream.insert(iterInsert, ++iterBegin, iter);
                    
                    iter = m_listTokenStream.erase(--iterBegin, ++iter);
                    
                    iter = m_listTokenStream.insert(iter, rcsToken_T(IDENTIFIER_NAME,
                                                    iterBegin->nLineNo, sTmpName.c_str()));
                }
                return;
            }
            else if(iter->sValue  == "=")
            {
                
                s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, LAY6,
                                                    iter->nLineNo, iter->sValue));
            }
            else if(iter->sValue == "(")
            {
                parseImplicitLayerDefinition(iter, iterInsert);
            }
        }
        else if(iter->eType == SPECIFICATION_KEYWORD)
        {
            
            isImplicitLayerDefinition = true;
            s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, LAY6,
                                                iter->nLineNo, iter->sValue));
        }
        else if(iter->eType == LAYER_OPERATION_KEYWORD || iter->eType == CMACRO)
        {
            if(isImplicitLayerDefinition)
            {
                
                s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, LAY6,
                                                    iter->nLineNo, iter->sValue));
            }
            isImplicitLayerDefinition = true;
        }
        ++iter;
    }
}

void
rcsSynConvertor::recognizeImplicitLayerDefinitions()
{
    std::list<rcsToken_T>::iterator iter = m_listTokenStream.begin();
    
    std::list<rcsToken_T>::iterator iterInsert = m_listTokenStream.end();
    bool isInRuleCheck = false;
    while(iter != m_listTokenStream.end())
    {
        if(iter->eType == SEPARATOR)
        {
            if(iter->sValue == "{")
            {
                isInRuleCheck = true;
                iterInsert = iter;
                while(iterInsert != m_listTokenStream.end())
                {
                    if(iterInsert->eType == SEPARATOR && iterInsert->sValue == "}")
                        break;
                    ++iterInsert;
                }
            }
            else if(iter->sValue == "}")
            {
                isInRuleCheck = false;
                iterInsert = m_listTokenStream.end();
            }
            else if(!isInRuleCheck && iter->sValue == "=")
            {
                iterInsert = iter;
                --iterInsert;
            }
            else if(iter->sValue == "(")
            {
                std::list<rcsToken_T>::iterator begin = iter;
                parseImplicitLayerDefinition(iter, iterInsert);
                if(iter == m_listTokenStream.end())
                    s_errManager.addError(rcErrorNode_T(rcErrorNode_T::ERROR, SYN6,
                                                        begin->nLineNo));
            }
        }
        else if(iter->eType == PRO_KEYWORD)
        {
            iterInsert = iter;
            if(iter->eKeyType == PRO_IFDEF || iter->eKeyType == PRO_IFNDEF ||
               iter->eKeyType == PRO_ELSE || iter->eKeyType == PRO_ENDIF)
            {
                ++iterInsert;
                while(iterInsert != m_listTokenStream.end())
                {
                    if(iterInsert->eType == PRO_KEYWORD &&
                      (iterInsert->eKeyType == PRO_IFDEF || iterInsert->eKeyType == PRO_IFNDEF ||
                       iterInsert->eKeyType == PRO_ELSE || iterInsert->eKeyType == PRO_ENDIF))
                    {
                        break;
                    }
                    else if(isInRuleCheck && iterInsert->eType == SEPARATOR && iterInsert->sValue == "}")
                    {
                        break;
                    }
                    else if(!isInRuleCheck && iterInsert->eType == SEPARATOR && iterInsert->sValue == "{")
                    {
                        --iterInsert;
                        break;
                    }
                    else if(!isInRuleCheck && iterInsert->sValue == "=")
                    {
                        --iterInsert;
                        break;
                    }
                    ++iterInsert;
                }
            }
        }
        ++iter;
    }
}

void
rcsSynConvertor::checkDefinedNameValid()
{
	std::list<rcsToken_T>::iterator iter = m_listTokenStream.begin();
    
    while(iter != m_listTokenStream.end())
    {
        if(iter->eType == SEPARATOR)
        {
            if((iter->sValue == "=") || (iter->sValue == "{"))
            {
                
                std::list<rcsToken_T>::iterator iterSep = iter;
                std::list<rcsToken_T>::iterator iterName = iter;
                while (iterName != m_listTokenStream.begin() && (--iterName)->eType == COMMENT)
                {}
                switch(iterName->eType)
                {
                    case STRING_CONSTANTS:
                        break;
                    case SECONDARY_KEYWORD:
                    case IDENTIFIER_NAME:
                    {
                        if(isValidSvrfName(iterName->sValue))
                        {
                            break;
                        }
                    }
                    default:
                    {
                        
                        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::FATAL, SYN3,
                                                            iterSep->nLineNo, iterSep->sValue));
                        break;
                    }
                }
            }
        }
        ++iter;
    }
}

rcsSynRootNode_T *
rcsSynConvertor::execute(std::map<hvUInt32, std::pair<hvUInt32, bool> > &blankLinesBefore)
{
	checkDefinedNameValid();
    recognizeImplicitLayerDefinitions();

#ifdef __DEBUG__
    std::cout << "---------------- AFTER RECOGNIZING IMPLICIT LAYER DEFINITIONS -------------" << std::endl;
    printTokenStream(m_listTokenStream.begin(), m_listTokenStream.end());
    std::cout << std::endl;
#endif

    std::list<rcsToken_T>::iterator iter = m_listTokenStream.begin();
    rcsSynRootNode_T *pRootNode = new rcsSynRootNode_T();
    std::list<rcsToken_T>::iterator iterBegin = iter;

    while(iter != m_listTokenStream.end())
    {
        switch(iter->eType)
        {
            case CMACRO:
            case LAYER_OPERATION_KEYWORD:
            {
                rcsSynLayerOperationNode_T *pLayerOperationNode =
                        new rcsSynLayerOperationNode_T(iter->eKeyType, iter->eType == CMACRO);
                if(parseLayerOperation(iter))
                {
                    pLayerOperationNode->insertTokens(iterBegin, iter);
                    pRootNode->addChildNodes(pLayerOperationNode);
                }
                else
                {
                    delete pLayerOperationNode;
                    pLayerOperationNode = NULL;
                }
                iterBegin = iter;
                continue;
            }
            case DMACRO:
            {
                
                std::list<rcsToken_T>::iterator begin = iter;
                if(iterBegin != begin)
                {
                    
                	rcsSvrf2Pvrs::setCurLineNo(iterBegin->nLineNo);
                    s_errManager.addError(rcErrorNode_T(rcErrorNode_T::ERROR, SYN1,
                    		iterBegin->nLineNo));
                }
                while(iter != m_listTokenStream.end())
                {
                    if(iter->eType == SEPARATOR && iter->sValue == "{")
                        break;
                    ++iter;
                }
                rcsSynProcNode_T *pNode = new rcsSynProcNode_T();
                parseRuleCheck(pNode, iter, true);
                pNode->insertTokens(begin, iter);
                pRootNode->addChildNodes(pNode);
                iterBegin = iter;

                continue;
            }
            case SPECIFICATION_KEYWORD:
            {
                if(iterBegin != iter)
                {
                    
                	rcsSvrf2Pvrs::setCurLineNo(iterBegin->nLineNo);
                    s_errManager.addError(rcErrorNode_T(rcErrorNode_T::ERROR, SYN1,
                    		iterBegin->nLineNo));
                    iterBegin = iter;
                }
                rcsSynSpecificationNode_T *pNode = new rcsSynSpecificationNode_T(iter->eKeyType);

                if(strcasecmp(iter->sValue.c_str(), "LVS REDUCE SPLIT GATES") == 0)
				{
                	parseSpecification(iter);
                	std::list<rcsToken_T>::iterator iBLBegin = iter;


                	if(iBLBegin->eType == COMMENT && (++iBLBegin)->eType == BUILT_IN_LANG)
                	{
                		pNode->insertTokens(iBLBegin, ++iBLBegin);
                		pNode->insertTokens(iterBegin, iter);
                		iter = iBLBegin;
                	}
                	else
                	{
                		pNode->insertTokens(iterBegin, iter);
                	}
				}
                else
                {
                	parseSpecification(iter);
                	pNode->insertTokens(iterBegin, iter);
                }
                pRootNode->addChildNodes(pNode);
                iterBegin = iter;

                continue;
            }
            case SEPARATOR:
            {
                if(iter->sValue == "{")
                {
                    
                    std::list<rcsToken_T>::iterator iRuleCheckBegin = iter;
                    --iRuleCheckBegin;
                    if(iterBegin != iRuleCheckBegin)
                    {
                        
                    	rcsSvrf2Pvrs::setCurLineNo(iterBegin->nLineNo);
                        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::ERROR, SYN1,
                        		iterBegin->nLineNo));
                    }
                    rcsSynRuleCheckNode_T *pRuleCheckNode = new rcsSynRuleCheckNode_T();
                    parseRuleCheck(pRuleCheckNode, iter);
                    pRuleCheckNode->insertTokens(iRuleCheckBegin, iter);
                    pRootNode->addChildNodes(pRuleCheckNode);
                    iterBegin = iter;

                    continue;
                }
                else if(iter->sValue == "=")
                {
                    
                    std::list<rcsToken_T>::iterator iLayerDefBegin = iter;
                    iterBegin = removeCommentTokens(m_listTokenStream, iterBegin, iter);
                    --iLayerDefBegin;
                    if(iterBegin != iLayerDefBegin)
                    {
                        
                    	rcsSvrf2Pvrs::setCurLineNo(iterBegin->nLineNo);
                        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::ERROR, SYN1,
                        		iterBegin->nLineNo));
                    }
                    rcsSynLayerDefinitionNode_T *pLayerDefinitionNode = new rcsSynLayerDefinitionNode_T();
                    pLayerDefinitionNode->insertTokens(iLayerDefBegin, iter);
                    if(parseLayerDefinition(pLayerDefinitionNode, iter))
                    {
                        pRootNode->addChildNodes(pLayerDefinitionNode);
                    }
                    else
                    {
                        delete pLayerDefinitionNode;
                        pLayerDefinitionNode = NULL;
                    }
                    iterBegin = iter;

                    continue;
                }
                break;
            }
            case PRO_KEYWORD:
            {
                if(iterBegin != iter)
                {
                    
                	rcsSvrf2Pvrs::setCurLineNo(iterBegin->nLineNo);
                    s_errManager.addError(rcErrorNode_T(rcErrorNode_T::ERROR, SYN1,
                    		iterBegin->nLineNo));
                }

                std::list<rcsToken_T>::iterator begin = iter;
                std::list<rcsToken_T>::iterator includeFile = iter;
                rcsSynPreProcessNode_T *pNode = new rcsSynPreProcessNode_T();

                if(strcasecmp("INCLUDE", includeFile->sValue.c_str()) == 0 && (++includeFile)->eType == STRING_CONSTANTS)
                {
                	pNode->insertTokens(begin, ++includeFile);
                	iter = includeFile;
                }
                else
                {
                	pNode->insertTokens(begin, ++iter);
                }

                pRootNode->addChildNodes(pNode);
                iterBegin = iter;

                continue;
            }
            case COMMENT:
            {
                Utassert(rcsManager_T::getInstance()->outputComment());
                std::list<rcsToken_T>::iterator begin = iter;
                rcsSynCommentNode_T *pNode = new rcsSynCommentNode_T();
                pNode->insertTokens(begin, ++iter);
                pRootNode->addChildNodes(pNode);
                iterBegin = iter;

                continue;
            }
            case BUILT_IN_LANG:
            {
                
                Utassert(rcsManager_T::getInstance()->isConvertSwitch());
                std::list<rcsToken_T>::iterator begin = iter;
                rcsSynBuildInNode_T *pNode = new rcsSynBuildInNode_T();
                pNode->insertTokens(begin, ++iter);
                pRootNode->addChildNodes(pNode);
                iterBegin = iter;

                continue;
            }
            default:
                break;
        }
        iter++;
    }













    if(rcsManager_T::getInstance()->isNewPvrs())
    {
        rcsSynPvrsConvertor pvrsConvertor(blankLinesBefore);
        if(rcsManager_T::getInstance()->hasTmpLayerDefinition() &&
           !rcsManager_T::getInstance()->needExpandTmpLayer())
        {
            pRootNode->accept(pvrsConvertor);
        }

        pvrsConvertor.setHasGetTmpLayerValues();
        pRootNode->accept(pvrsConvertor);
    }
    else
    {
        rcsSynNodePreConvertor_T preConvertor;
        pRootNode->accept(preConvertor);
        rcsSynNodeProConvertor_T proConvertor;
        proConvertor.setVariableSet(preConvertor.getVariableSet());
        pRootNode->accept(proConvertor);
    }

    return pRootNode;
}

#ifdef DEBUG
void
rcsSynConvertor::printTokenList()
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
rcsSynConvertor::printVector(std::vector<std::string>& vec)
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

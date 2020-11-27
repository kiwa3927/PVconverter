



#include "rcsSynNodeVisitor.h"
#include "rcsSynRulecheckNode.h"

rcsSynRuleCheckNode_T::rcsSynRuleCheckNode_T()
{
}

rcsSynRuleCheckNode_T::~rcsSynRuleCheckNode_T()
{
    for(std::vector<rcsSynNode_T *>::iterator iter = m_vChildNodes.begin();
        iter != m_vChildNodes.end(); ++iter)
    {
        delete (*iter);
    }
}

void
rcsSynRuleCheckNode_T::addChildNode(rcsSynNode_T *pNode)
{
    m_vChildNodes.push_back(pNode);
}

const char *
rcsSynRuleCheckNode_T::getRuleCheckName()
{
    return m_listTokens.begin()->sValue.c_str();
}

std::string
rcsSynRuleCheckNode_T::getRuleCheckText() const
{
    std::string sCheckText;
    hvUInt32 nLineNo  = m_listTokens.begin()->nLineNo;
    for(std::list<rcsToken_T>::const_iterator iter = m_listTokens.begin(); iter != m_listTokens.end(); ++iter)
    {
        if(nLineNo < iter->nLineNo)
        {
            sCheckText += "\n";
            nLineNo = iter->nLineNo;
        }
        sCheckText += iter->sValue;
        sCheckText += " ";
    }
    return sCheckText;
}

hvUInt32
rcsSynRuleCheckNode_T::getEndLineNo() const
{
    return m_listTokens.back().nLineNo;
}

void
rcsSynRuleCheckNode_T::accept(rcsNodeVisitor_T &visitor)
{
    visitor.beginVisitRuleCheckNode(this);
    for(std::vector<rcsSynNode_T *>::iterator iter = m_vChildNodes.begin();
        iter != m_vChildNodes.end(); ++iter)
    {
        (*iter)->accept(visitor);
    }
    visitor.endVisitRuleCheckNode(this);
}


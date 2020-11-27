



#include "rcsSynNodeVisitor.h"
#include "rcsSynProcessNode.h"

rcsSynPreProcessNode_T::rcsSynPreProcessNode_T(): m_eType(INVALID_KEY)
{
}

rcsSynPreProcessNode_T::~rcsSynPreProcessNode_T()
{
}

KEY_TYPE
rcsSynPreProcessNode_T::getPreProcessType()
{
    if(m_eType == INVALID_KEY)
    {
        if(!m_listTokens.empty() && m_listTokens.begin()->eType == PRO_KEYWORD)
            m_eType = m_listTokens.begin()->eKeyType;
    }

    return m_eType;
}

void
rcsSynPreProcessNode_T::accept(rcsNodeVisitor_T &visitor)
{
    visitor.beginVisitPreProcessNode(this);
    visitor.endVisitPreProcessNode(this);
}


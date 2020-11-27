



#include "rcsSynRootNode.h"

rcsSynRootNode_T::rcsSynRootNode_T()
{
}

rcsSynRootNode_T::~rcsSynRootNode_T()
{
    for(std::vector<rcsSynNode_T *>::iterator iter = m_vChildNodes.begin();
        iter != m_vChildNodes.end(); ++iter)
    {
        delete (*iter);
    }
}

void
rcsSynRootNode_T::addChildNodes(rcsSynNode_T *pNode)
{
    m_vChildNodes.push_back(pNode);
}

void
rcsSynRootNode_T::accept(rcsNodeVisitor_T &visitor)
{
    for(std::vector<rcsSynNode_T *>::const_iterator iter = m_vChildNodes.begin();
        iter != m_vChildNodes.end(); ++iter)
    {
        (*iter)->accept(visitor);
    }
}

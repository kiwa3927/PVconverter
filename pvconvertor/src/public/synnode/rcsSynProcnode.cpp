



#include "rcsSynNodeVisitor.h"
#include "rcsSynProcNode.h"

rcsSynProcNode_T::rcsSynProcNode_T()
{
}

rcsSynProcNode_T::~rcsSynProcNode_T()
{
    for(std::vector<rcsSynNode_T *>::iterator iter = m_vChildNodes.begin();
        iter != m_vChildNodes.end(); ++iter)
    {
        delete (*iter);
    }
}

void
rcsSynProcNode_T::addChildNode(rcsSynNode_T *pNode)
{
    m_vChildNodes.push_back(pNode);
}

const std::vector<rcsSynNode_T *> &
rcsSynProcNode_T::getChildNodes() const
{
    return m_vChildNodes;
}

void
rcsSynProcNode_T::accept(rcsNodeVisitor_T &visitor)
{
    visitor.beginVisitProcNode(this);
    for(std::vector<rcsSynNode_T *>::iterator iter = m_vChildNodes.begin();
        iter != m_vChildNodes.end(); ++iter)
    {
        (*iter)->accept(visitor);
    }
    visitor.endVisitProcNode(this);
}

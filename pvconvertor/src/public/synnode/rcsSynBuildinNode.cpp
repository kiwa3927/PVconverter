

#include "rcsSynNodeVisitor.h"
#include "rcsSynBuildinNode.h"

rcsSynBuildInNode_T::rcsSynBuildInNode_T()
{
}

const std::string &
rcsSynBuildInNode_T::getBuildIn() const
{
    return m_listTokens.front().sValue;
}

void
rcsSynBuildInNode_T::accept(rcsNodeVisitor_T &visitor)
{
    visitor.beginVisitBuildInNode(this);
    visitor.endVisitBuildInNode(this);
}

#include "rcsSynNodeVisitor.h"
#include "rcsSynCommentNode.h"

rcsSynCommentNode_T::rcsSynCommentNode_T()
{
}

const std::string &
rcsSynCommentNode_T::getComment() const
{
    return m_listTokens.front().sValue;
}

void
rcsSynCommentNode_T::accept(rcsNodeVisitor_T &visitor)
{
    visitor.beginVisitCommentNode(this);
    visitor.endVisitCommentNode(this);
}

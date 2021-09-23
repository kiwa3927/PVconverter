#include "rcssyndmacrodummynode.h"
#include "rcsSynNodeVisitor.h"

void rcsSynDMACRODummyNode_T::accept(rcsNodeVisitor_T &visitor)
{
    visitor.beginVisitDMACRODummyNode(this);
    visitor.endVisitDMACRODummyNode(this);
}


void rcsSynSwitchUnKnowNode_T::accept(rcsNodeVisitor_T &visitor)
{
    // full copy
    visitor.beginVisitSwitchUnknowNode(this);
    visitor.endVisitSwitchUnknowNode(this);
}

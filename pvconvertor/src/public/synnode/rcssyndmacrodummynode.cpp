#include "rcssyndmacrodummynode.h"
#include "rcsSynNodeVisitor.h"

void rcsSynDMACRODummyNode_T::accept(rcsNodeVisitor_T &visitor)
{
    visitor.beginVisitDMACRODummyNode(this);
    visitor.endVisitDMACRODummyNode(this);
}

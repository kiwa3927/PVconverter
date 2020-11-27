#ifndef RCSSYNDMACRODUMMYNODE_H
#define RCSSYNDMACRODUMMYNODE_H

#include "rcsSynNode.h"

class rcsSynDMACRODummyNode_T : public rcsSynNode_T
{
public:
    void accept(rcsNodeVisitor_T &visitor);
};

#endif 



#ifndef RCSSYNBUILDINNODE_H_
#define RCSSYNBUILDINNODE_H_

#include "rcsSynNode.h"

class rcsSynBuildInNode_T: public rcsSynNode_T
{
public:
	rcsSynBuildInNode_T();

    const std::string &getBuildIn() const;

    virtual void accept(rcsNodeVisitor_T &visitor);
};

#endif 

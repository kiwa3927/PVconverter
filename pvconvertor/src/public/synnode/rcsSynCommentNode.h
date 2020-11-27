#ifndef RCSSYNCOMMENTNODE_H_
#define RCSSYNCOMMENTNODE_H_

#include "rcsSynNode.h"

class rcsSynCommentNode_T: public rcsSynNode_T
{
public:
    rcsSynCommentNode_T();

    const std::string &getComment() const;

    virtual void accept(rcsNodeVisitor_T &visitor);
};

#endif 

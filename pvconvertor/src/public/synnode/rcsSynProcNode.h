



#ifndef RCSSYNPROCNODE_H_
#define RCSSYNPROCNODE_H_

#include "rcsSynNode.h"

class rcsSynProcNode_T: public rcsSynNode_T
{
public:
    rcsSynProcNode_T();
    ~rcsSynProcNode_T();

    void addChildNode(rcsSynNode_T *);

    const std::vector<rcsSynNode_T *> &getChildNodes() const;

    virtual void accept(rcsNodeVisitor_T &visitor);

private:
    std::vector<rcsSynNode_T *> m_vChildNodes;
};

#endif 

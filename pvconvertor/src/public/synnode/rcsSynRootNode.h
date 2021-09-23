



#ifndef SYNROOTNODE_H_
#define SYNROOTNODE_H_

#include <vector>


#include "rcsSynNode.h"

class rcsSynRootNode_T: public rcsSynNode_T
{
public:
    rcsSynRootNode_T();
    ~rcsSynRootNode_T();

    void addChildNodes(rcsSynNode_T *);

    virtual void accept(rcsNodeVisitor_T &);

    rcsSynNode_T * refLastNodeForCMacro()
    { if (m_vChildNodes.empty()) return NULL; return m_vChildNodes.back(); }

private:

    std::vector<rcsSynNode_T *> m_vChildNodes;
};

#endif 

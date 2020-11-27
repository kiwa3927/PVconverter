



#ifndef SYNRULECHECKNODE_H_
#define SYNRULECHECKNODE_H_


#include "rcsSynNode.h"

class rcsSynLayerDefinitionNode_T;
class rcsSynLayerOperationNode_T;
class rcsSynSpecificationNode_T;

class rcsSynRuleCheckNode_T: public rcsSynNode_T
{
public:
    rcsSynRuleCheckNode_T();
    ~rcsSynRuleCheckNode_T();

    void addChildNode(rcsSynNode_T *);

    const char *getRuleCheckName();

    virtual void accept(rcsNodeVisitor_T &visitor);

    std::string getRuleCheckText() const;

    hvUInt32 getEndLineNo() const;

private:
    std::vector<rcsSynNode_T *> m_vChildNodes;
};

#endif 

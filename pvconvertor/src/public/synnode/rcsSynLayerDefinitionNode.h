



#ifndef SYNLAYERDEFINITIONNODE_H_
#define SYNLAYERDEFINITIONNODE_H_

#include <string>

#include "rcsSynNode.h"

class rcsSynLayerOperationNode_T;
class rcsSynRuleCheckNode_T;

class rcsSynLayerDefinitionNode_T: public rcsSynNode_T
{
public:
    rcsSynLayerDefinitionNode_T();
    ~rcsSynLayerDefinitionNode_T();

    void setLayerOperationNode(rcsSynLayerOperationNode_T *);

    rcsSynLayerOperationNode_T *getLayerOperationNode() const;

    virtual bool isDefinitionNode() const;

    virtual void accept(rcsNodeVisitor_T &visitor);

    void setTmpLayer(bool isTmpLayer);

    bool isTmpLayer() const;

protected:
    bool                        m_isTmpLayer;
    std::string                 m_sLayerName;
    rcsSynLayerOperationNode_T *m_pLayerOpNode;
};

#endif 

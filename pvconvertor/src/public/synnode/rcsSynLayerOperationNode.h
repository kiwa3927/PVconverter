



#ifndef SYNLAYEROPERATIONNODE_H_
#define SYNLAYEROPERATIONNODE_H_


#include "rcsSynNode.h"

class rcsSynRuleCheckNode_T;

class rcsSynLayerOperationNode_T: public rcsSynNode_T
{
public:
    rcsSynLayerOperationNode_T(KEY_TYPE eKeyType, bool isCMacro = false);
    virtual void accept(rcsNodeVisitor_T &visitor);

    KEY_TYPE getOperationType() const;

    bool isCMacro() const;

protected:
    bool                   m_isCMacro;
    KEY_TYPE               m_eOpType;
};

#endif 

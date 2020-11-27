



#include "rcsSynNodeVisitor.h"
#include "rcsSynLayerOperationNode.h"

rcsSynLayerOperationNode_T::rcsSynLayerOperationNode_T(KEY_TYPE eKeyType, bool isCMacro) :
    m_isCMacro(isCMacro), m_eOpType(eKeyType)
{
}

void
rcsSynLayerOperationNode_T::accept(rcsNodeVisitor_T &visitor)
{
    visitor.beginVisitLayerOperationNode(this);
    visitor.endVisitLayerOperationNode(this);
}

bool
rcsSynLayerOperationNode_T::isCMacro() const
{
    return m_isCMacro;
}

KEY_TYPE
rcsSynLayerOperationNode_T::getOperationType() const
{
    return m_eOpType;
}

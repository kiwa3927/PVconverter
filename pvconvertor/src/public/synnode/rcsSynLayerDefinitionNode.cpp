



#include "rcsSynNodeVisitor.h"
#include "rcsSynLayerOperationNode.h"
#include "rcsSynLayerDefinitionNode.h"

rcsSynLayerDefinitionNode_T::rcsSynLayerDefinitionNode_T()
{
    m_isTmpLayer    = false;
    m_pLayerOpNode  = NULL;
}

rcsSynLayerDefinitionNode_T::~rcsSynLayerDefinitionNode_T()
{
    delete m_pLayerOpNode;
}

void
rcsSynLayerDefinitionNode_T::setLayerOperationNode(rcsSynLayerOperationNode_T *pOpNode)
{
    m_pLayerOpNode = pOpNode;
}

rcsSynLayerOperationNode_T *
rcsSynLayerDefinitionNode_T::getLayerOperationNode() const
{
    return m_pLayerOpNode;
}

bool
rcsSynLayerDefinitionNode_T::isDefinitionNode() const
{
    return true;
}

void
rcsSynLayerDefinitionNode_T::setTmpLayer(bool isTmpLayer)
{
    m_isTmpLayer = isTmpLayer;
}

bool
rcsSynLayerDefinitionNode_T::isTmpLayer() const
{
    return m_isTmpLayer;
}

void
rcsSynLayerDefinitionNode_T::accept(rcsNodeVisitor_T &visitor)
{
    visitor.beginVisitLayerDefinitionNode(this);
    m_pLayerOpNode->accept(visitor);
    visitor.endVisitLayerDefinitionNode(this);
}

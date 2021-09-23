



#ifndef SYNNODEVISITOR_H_
#define SYNNODEVISITOR_H_

class rcsSynLayerDefinitionNode_T;
class rcsSynRuleCheckNode_T;
class rcsSynLayerOperationNode_T;
class rcsSynSpecificationNode_T;
class rcsSynPreProcessNode_T;
class rcsSynProcNode_T;
class rcsSynCommentNode_T;
class rcsSynBuildInNode_T;
class rcsSynDMACRODummyNode_T;
class rcsSynSwitchUnKnowNode_T;


class rcsNodeVisitor_T
{
public:
    virtual void beginVisitLayerDefinitionNode(rcsSynLayerDefinitionNode_T *pNode) {}
    virtual void beginVisitRuleCheckNode(rcsSynRuleCheckNode_T *pNode) {}
    virtual void beginVisitLayerOperationNode(rcsSynLayerOperationNode_T *pNode) {}
    virtual void beginVisitSpecificationNode(rcsSynSpecificationNode_T *pNode) {}
    virtual void beginVisitPreProcessNode(rcsSynPreProcessNode_T *pNode) {}
    virtual void beginVisitProcNode(rcsSynProcNode_T *pNode) {}
    virtual void beginVisitCommentNode(rcsSynCommentNode_T *pNode) {}
    virtual void beginVisitBuildInNode(rcsSynBuildInNode_T *pNode) {}
    virtual void endVisitBuildInNode(rcsSynBuildInNode_T *pNode) {}
    virtual void endVisitCommentNode(rcsSynCommentNode_T *pNode) {}
    virtual void endVisitLayerDefinitionNode(rcsSynLayerDefinitionNode_T *pNode) {}
    virtual void endVisitRuleCheckNode(rcsSynRuleCheckNode_T *pNode) {}
    virtual void endVisitLayerOperationNode(rcsSynLayerOperationNode_T *pNode) {}
    virtual void endVisitSpecificationNode(rcsSynSpecificationNode_T *pNode) {}
    virtual void endVisitPreProcessNode(rcsSynPreProcessNode_T *pNode) {}
    virtual void endVisitProcNode(rcsSynProcNode_T *pNode) {}
    virtual void beginVisitDMACRODummyNode(rcsSynDMACRODummyNode_T *pNode) {}
    virtual void endVisitDMACRODummyNode(rcsSynDMACRODummyNode_T *pNode) {}
    virtual void beginVisitSwitchUnknowNode(rcsSynSwitchUnKnowNode_T *pNode) {}
    virtual void endVisitSwitchUnknowNode(rcsSynSwitchUnKnowNode_T *pNode) {}
};

#endif 

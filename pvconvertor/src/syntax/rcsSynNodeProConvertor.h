

#ifndef RCSSYNNODEPROCONVERTOR_H_
#define RCSSYNNODEPROCONVERTOR_H_

#include "rcsSynNodeConvertor.h"

class rcsSynNodeProConvertor_T: public rcsSynNodeConvertor_T
{
public:
    rcsSynNodeProConvertor_T();

    void setVariableSet(const std::set<std::string, ltStringCase> &vNameSet);

    virtual void beginVisitLayerDefinitionNode(rcsSynLayerDefinitionNode_T *pNode);
    virtual void endVisitLayerDefinitionNode(rcsSynLayerDefinitionNode_T *pNode);

    virtual void beginVisitRuleCheckNode(rcsSynRuleCheckNode_T *pNode);
    virtual void endVisitRuleCheckNode(rcsSynRuleCheckNode_T *pNode);

    virtual void beginVisitLayerOperationNode(rcsSynLayerOperationNode_T *pNode);
    virtual void endVisitLayerOperationNode(rcsSynLayerOperationNode_T *pNode);

    virtual void beginVisitSpecificationNode(rcsSynSpecificationNode_T *pNode);

    virtual void beginVisitPreProcessNode(rcsSynPreProcessNode_T *pNode);

    virtual void beginVisitProcNode(rcsSynProcNode_T *pNode);
    virtual void endVisitProcNode(rcsSynProcNode_T *pNode);


    virtual void beginVisitCommentNode(rcsSynCommentNode_T *pNode);
private:
    bool m_inProcReturn;
};

#endif 

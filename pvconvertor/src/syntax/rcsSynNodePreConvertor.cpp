

#include "public/manager/rcErrorManager.h"
#include "public/util/rcsStringUtil.hpp"
#include "public/synnode/rcsSynProcNode.h"
#include "public/synnode/rcsSynLayerDefinitionNode.h"
#include "public/synnode/rcsSynSpecificationNode.h"

#include "public/manager/rcsManager.h"
#include "rcsSynNodePreConvertor.h"

rcsSynNodePreConvertor_T::rcsSynNodePreConvertor_T() : rcsSynNodeConvertor_T()
{
}

const std::set<std::string, rcsSynNodeConvertor_T::ltStringCase> &
rcsSynNodePreConvertor_T::getVariableSet() const
{
    return m_vVariableNames;
}

void
rcsSynNodePreConvertor_T::beginVisitSpecificationNode(rcsSynSpecificationNode_T *pNode)
{
    if(pNode->getSpecificationType() != VARIABLE)
        return;

    std::string sValue;
    pNode->getCmdValue(sValue);
    if(rcsManager_T::getInstance()->isSvrfComment())
    {
       m_oPVRSStream << "#";
       m_oPVRSStream << sValue << std::endl;
    }

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if(pNode->getSpecificationType() != SOURCE_PRIMARY && iter == pNode->end())
    {
        --iter;
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2,
                            pNode->getNodeBeginLineNo(), iter->sValue);
    }

    switch(pNode->getSpecificationType())
    {
        case VARIABLE:
            convertSetSpec(pNode);
            break;
        default:
            break;
    }

    m_oPVRSStream << std::endl;
}

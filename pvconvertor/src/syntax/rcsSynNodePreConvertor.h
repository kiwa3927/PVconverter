

#ifndef RCSSYNNODEPRECONVERTOR_H_
#define RCSSYNNODEPRECONVERTOR_H_

#include "rcsSynNodeConvertor.h"

class rcsSynNodePreConvertor_T: public rcsSynNodeConvertor_T
{
public:
    rcsSynNodePreConvertor_T();

    const std::set<std::string, ltStringCase> &getVariableSet() const;

    virtual void beginVisitSpecificationNode(rcsSynSpecificationNode_T *pNode);
};

#endif 

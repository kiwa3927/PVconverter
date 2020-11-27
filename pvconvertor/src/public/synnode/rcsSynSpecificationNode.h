



#ifndef SYNSPECIFICATIONNODE_H_
#define SYNSPECIFICATIONNODE_H_


#include "rcsSynNode.h"

class rcsSynSpecificationNode_T: public rcsSynNode_T
{
public:
    rcsSynSpecificationNode_T(KEY_TYPE eKeyType, bool isLocal = false, bool isInMacro = false);

    virtual void accept(rcsNodeVisitor_T &visitor);

    KEY_TYPE getSpecificationType() const;

    bool isLocal() const;

    bool isInMacro() const;

protected:
    KEY_TYPE m_eSpecType;
    bool     m_isLocal;
    bool 	 m_isInMacro;
};

#endif 

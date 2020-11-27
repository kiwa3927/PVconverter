



#include "rcsSynNodeVisitor.h"
#include "rcsSynSpecificationNode.h"

rcsSynSpecificationNode_T::rcsSynSpecificationNode_T(KEY_TYPE eKeyType, bool isLocal, bool isInMacro) :
    m_eSpecType(eKeyType), m_isLocal(isLocal), m_isInMacro(isInMacro)
{
}

void
rcsSynSpecificationNode_T::accept(rcsNodeVisitor_T &visitor)
{
    visitor.beginVisitSpecificationNode(this);
    visitor.endVisitSpecificationNode(this);
}

KEY_TYPE
rcsSynSpecificationNode_T::getSpecificationType() const
{
    return m_eSpecType;
}

bool
rcsSynSpecificationNode_T::isLocal() const
{
    return m_isLocal;
}

bool
rcsSynSpecificationNode_T::isInMacro() const
{
    return m_isInMacro;
}

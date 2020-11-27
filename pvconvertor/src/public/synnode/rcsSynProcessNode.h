



#ifndef RCSSYNPROCESSNODE_H_
#define RCSSYNPROCESSNODE_H_

#include "rcsSynNode.h"

class rcsSynPreProcessNode_T: public rcsSynNode_T
{
public:
    rcsSynPreProcessNode_T();
    virtual ~rcsSynPreProcessNode_T();

    KEY_TYPE getPreProcessType();

    virtual void accept(rcsNodeVisitor_T &visitor);

private:
    KEY_TYPE m_eType;
};

#endif 

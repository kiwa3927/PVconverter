



#ifndef SYNNODE_H_
#define SYNNODE_H_

#include "public/rcsTypes.h"
#include "public/token/rcsToken.h"

class rcsNodeVisitor_T;

class rcsSynNode_T
{
public:
    inline rcsSynNode_T();
	virtual ~rcsSynNode_T(){}

    inline void insertTokens(std::list<rcsToken_T>::iterator first,
                             std::list<rcsToken_T>::iterator last);
    inline std::list<rcsToken_T>::iterator begin();
    inline std::list<rcsToken_T>::iterator end();

    inline hvUInt32 getNodeBeginLineNo() const;

    virtual void accept(rcsNodeVisitor_T &visitor) = 0;

    virtual bool isDefinitionNode() const
    {
        return false;
    }

    inline void getCmdValue(std::string &sValue);

    void appendTokenForCMacro(std::list<rcsToken_T>::iterator first,
                              std::list<rcsToken_T>::iterator last)
    {
        m_listTokens.insert(m_listTokens.end(), first, last);
    }

protected:
    std::list<rcsToken_T> m_listTokens;
};

#include "rcsSynNode.inl"
#endif 

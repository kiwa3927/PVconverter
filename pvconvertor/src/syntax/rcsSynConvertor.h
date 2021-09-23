



#ifndef RCSSYNCONVERTOR_H_
#define RCSSYNCONVERTOR_H_

#include "public/token/rcsToken.h"
#include "public/rcsTypes.h"

class rcsSynRuleCheckNode_T;
class rcsSynLayerOperationNode_T;
class rcsSynLayerDefinitionNode_T;
class rcsSynSpecificationNode_T;
class rcsSynNodeCreater_T;
class rcsSynRootNode_T;
class rcsSynNodeConvertor_T;

class rcsSynConvertor
{
public:
    rcsSynConvertor(std::list<rcsToken_T> &lTokenStream, bool bTvfConvertor = false);
    void setMacroParaMap(const std::map<std::string, hvUInt32> *pMap) { m_pMacroParasMap = pMap; }
    rcsSynRootNode_T *execute(std::map<hvUInt32, std::pair<hvUInt32, bool> > &blankLinesBefore);

#ifdef DEBUG
    void printTokenList();
    void printVector(std::vector<std::string>& vec);
#endif

private:
    void checkDefinedNameValid();
    bool parseLayerOperation(std::list<rcsToken_T>::iterator&);
    bool parseSpecification(std::list<rcsToken_T>::iterator &iter);
    template<class Node_Type>
    void parseRuleCheck(Node_Type *pRuleCheckNode,
                        std::list<rcsToken_T>::iterator &iter,
                        bool isInMacro = false);
    bool parseLayerDefinition(rcsSynLayerDefinitionNode_T*,
                              std::list<rcsToken_T>::iterator&);

    bool parseProKeyword(std::list<rcsToken_T>::iterator &iter);

    void recognizeImplicitLayerDefinitions();

    void parseImplicitLayerDefinition(std::list<rcsToken_T>::iterator&,
                                      std::list<rcsToken_T>::iterator&);

private:
    std::list<rcsToken_T> &m_listTokenStream;
    const std::map<std::string, hvUInt32> *m_pMacroParasMap;
    bool m_bTvfConvertor;
};

#endif 

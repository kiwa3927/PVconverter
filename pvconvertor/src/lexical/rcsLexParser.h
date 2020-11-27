



#ifndef LEXPARSER_H_
#define LEXPARSER_H_

#include <set>

#include "public/rcsTypes.h"
#include "public/token/rcsToken.h"

class FlexLexer;


class rcsLexParser_T
{
public:
    rcsLexParser_T(std::list<rcsToken_T> &lTokenStream,
                   std::set<hvUInt32> &vUnstoredCommentLineNumbers);

    void parse(const char *pFileName, const char* pValue = NULL);

#ifdef DEBUG
    void printTokenList();
    void printVector(std::vector<std::string>& vec);
#endif

private:
    std::list<rcsToken_T>    &m_listTokenStream;
    const char               *m_pFileName;
    std::set<hvUInt32>       &m_vUnstoredCommentLineNumbers;
};

#endif 

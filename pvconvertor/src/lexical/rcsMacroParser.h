



#ifndef LEXMACROPARSER_H_
#define LEXMACROPARSER_H_

#include <vector>
#include <map>
#include <set>
#include <string>

#include "public/rcsTypes.h"
#include "public/token/rcsToken.h"


class rcsLexMacroParser_T
{
public:
    rcsLexMacroParser_T(std::list<rcsToken_T> &lTokenStream,
                        std::map<hvUInt32, std::pair<hvUInt32, bool> > &blankLinesBefore,
                        std::set<hvUInt32> &vUnstoredCommentLineNumbers,
                        bool flattenMacro = false);

    void parse();

private:
    void parseBlankLines();
    void extractMacro(std::list<rcsToken_T>::iterator &iBegin);
    int  parseMacroParas(std::list<rcsToken_T>::iterator &iBegin, std::set<std::string> &vParas);
    template<bool needBreak> void parseBuildInLang();
    void removeUnusedToken();

    template <class iterator> void skipBlankSpaceToken(iterator &iter);

private:
    typedef std::list<rcsToken_T>::iterator TOKEN_ITER;
    std::list<rcsToken_T>        &m_listTokenStream;
    std::map<hvUInt32, std::pair<hvUInt32, bool> > &m_blankLinesBefore;
    std::set<hvUInt32>           &m_vUnstoredCommentLineNumbers;
    bool                          m_flattenMacro;

private:
    struct MACRO_TOKENS
    {
        std::list<rcsToken_T>    tokens;
        hvUInt32                 nParas;
        hvUInt32                 nCallTimes;

        MACRO_TOKENS(std::list<rcsToken_T>::iterator begin, std::list<rcsToken_T>::iterator end,
                     hvUInt32 nParas) : nParas(nParas), nCallTimes(0)
        {
            tokens.insert(tokens.begin(), begin, end);
        }
    };
    std::map<std::string, MACRO_TOKENS> m_mapMacros;
public:
    std::map<std::string, hvUInt32> m_macrosParaSizeMap;

private:
    void replaceCMacro(std::list<rcsToken_T>::iterator &iCMacro, MACRO_TOKENS &macro);
    void parseCMacro(std::list<rcsToken_T>::iterator &iterBegin);

private:
    struct BUILDIN_FRAGMENT
    {
        
        TOKEN_ITER begin;
        TOKEN_ITER end;

        BUILDIN_FRAGMENT(const TOKEN_ITER &begin, const TOKEN_ITER &end) :
            begin(begin), end(end)
        {}
    };
    typedef std::vector<BUILDIN_FRAGMENT> BUILDIN_FRAGMENT_CONTAINER;
    BUILDIN_FRAGMENT_CONTAINER m_vBuildInFragments;

    void constructBuildInFragments();
};

#endif 

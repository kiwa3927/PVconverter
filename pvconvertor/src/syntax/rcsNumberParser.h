



#ifndef RCSNUMBERPARSER_H_
#define RCSNUMBERPARSER_H_

#include "public/rcsTypes.h"
#include "public/token/rcsToken.h"

class rcsExpressionParser_T
{
public:
    static bool parse(std::list<rcsToken_T>::iterator &begin, std::list<rcsToken_T>::iterator end, std::string &sNumExpr);
    static bool parseNumber(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end, std::string &sNumExpr);
    static bool parseNumberJustForNegative(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end,
                                       std::string &sNumExpr);
    static void setNewPvrs();
    static void setNegativeNumber();
    static void unsetNegativeNumber();

private:
    static bool exp(std::list<rcsToken_T>::iterator &begin, std::list<rcsToken_T>::iterator end, std::string &sNumExpr);
    static bool factor(std::list<rcsToken_T>::iterator &begin, std::list<rcsToken_T>::iterator end, std::string &sNumExpr);
    static bool term(std::list<rcsToken_T>::iterator &begin, std::list<rcsToken_T>::iterator end, std::string &sNumExpr);
    static bool isFuncName(const std::string &funcName);
    static bool parseFunc(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end, std::string &sNumExpr);

    static bool m_isNewPvrs;
    static bool m_isNegativeNumber;

};

#endif 

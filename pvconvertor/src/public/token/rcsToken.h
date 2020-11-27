



#ifndef TOKEN_H_
#define TOKEN_H_

#include <string>
#include <list>
#include <vector>

#include "public/rcsTypes.h"
#include "rcsTokenTypes.h"

struct  rcsToken_T
{
    TOKEN_TYPE   eType;
    hvUInt32     nLineNo;  

    union
    {
        KEY_TYPE      eKeyType;
        NUMBER_TYPE   eNumberType;
    };

    union
    {
        hvUIntp       iValue;
        double        dValue;
        hvUInt32 nEndLineNo;  
    };

    std::string  sValue;

    std::vector<std::string> namescopes;

    rcsToken_T(TOKEN_TYPE eType, hvUInt32 nLineNo,
               const char *pValue) :
        eType(eType), nLineNo(nLineNo), sValue(pValue)
    {
    }
    rcsToken_T()
    {}
};

#endif 

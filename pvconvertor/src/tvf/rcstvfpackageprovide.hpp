#ifndef RCSTVFPACKAGEPROVIDE_H
#define RCSTVFPACKAGEPROVIDE_H

#include <stdlib.h>
#include <vector>
#include <string>
#include <fstream>
#include "tcl.h"
#include "public/util/rcsStringUtil.hpp"

static
const char * _fakePKG_pgkString = "CalibreDFM_DEVICE 1.0; CalibreLVS_DEVICE 1.0; CalibreLVS_DEVICE_DFM 1.0";

inline
std::string _fakePKG_getFake_v(const std::vector<std::string> &vec)
{
    static const std::string s_pp = "package provide ";
    std::string str;
    for (size_t i=0; i<vec.size(); ++i)
    {
        str += s_pp + vec[i] + '\n';
    }
    return str;
}

inline
std::string _fakePKG_getIngoreString(const char *pStr)
{
    std::string str = pStr;
    std::vector<std::string> vec;
    tokenize(vec, str, ";");
    return _fakePKG_getFake_v(vec);
}

inline
void _fakePKG_setPackage(Tcl_Interp *pInterp)
{
    if( getenv("PV_IGNORE_PACKAGE_DISABLE") != NULL)
        return;

    
    Tcl_RecordAndEval(pInterp, _fakePKG_getIngoreString(_fakePKG_pgkString).c_str(), 0);

    const char *pStr = getenv("PV_IGNORE_PACKAGE");
    if (pStr)
    {
        Tcl_RecordAndEval(pInterp, _fakePKG_getIngoreString(pStr).c_str(), 0);
    }

    const char *pFile = getenv("PV_IGNORE_PACKAGE_FILE");
    if (pFile)
    {
        std::ifstream in(pFile);
        if (in)
        {
            std::string line;
            std::getline(in, line);
            Tcl_RecordAndEval(pInterp, _fakePKG_getIngoreString(line.c_str()).c_str(), 0);
        }
    }
}

inline
Tcl_Interp * g_warpper_CreateInterp()
{
    Tcl_Interp *pInterp = Tcl_CreateInterp();

    if (pInterp)
        _fakePKG_setPackage(pInterp);

    return pInterp;
}

#endif 

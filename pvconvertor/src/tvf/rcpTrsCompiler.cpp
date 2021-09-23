

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "tcl.h"
#include "rcpTrsCompiler.h"
#include "public/util/rcsStringUtil.hpp"
#include "public/manager/rcErrorManager.h"
#include "rcstvfpackageprovide.hpp"


bool rcpTRSCompiler::m_isInRulecheck = false;
std::string rcpTRSCompiler::m_pTrsArg         ;
std::string rcpTRSCompiler::m_pTrsFilename    ;
std::string rcpTRSCompiler::m_pPvrsFileName   ;
Tcl_Interp* rcpTRSCompiler::m_pInterp = NULL;

std::vector<std::string> *rcpTRSCompiler::m_pvSourceFiles = NULL;

std::ofstream rcpTRSCompiler::m_fPvrs;
std::map<std::string, std::string, rcpTRSCompiler::LTstr> rcpTRSCompiler::layermap_g;
std::map<std::string, std::string, rcpTRSCompiler::LTstr> rcpTRSCompiler::layermap_l;
std::map<std::string, std::string, rcpTRSCompiler::LTstr> rcpTRSCompiler::varmap_g;

#define TCL_CREATE_COMMAND(command) Tcl_CreateCommand(pInterp, command, (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0)

static inline void
setTRSCompileError(Tcl_Interp *pInterp, int argc, const char *argv[])
{
    std::string sMessage;
    for(int iValue = 0; iValue < argc; ++iValue)
    {
        sMessage += argv[iValue];
        sMessage += " ";
    }

    Tcl_Obj *pResult = Tcl_NewStringObj(sMessage.c_str(), sMessage.size());
    Tcl_SetObjResult(pInterp, pResult);
}

int
rcpTRSCompiler::blockcomment_proc(ClientData d, Tcl_Interp *pInterp,
                                  int argc, const char * argv[])
{
    
    m_fPvrs << "// " ;
    for(int i=1; i< argc; ++i)
    {
        if( i!= 1)
            m_fPvrs << " ";
        m_fPvrs << argv[i];
    }
    m_fPvrs << std::endl;
    return TCL_OK;
}

int
rcpTRSCompiler::rulecomment_proc(ClientData d, Tcl_Interp *pInterp,
                                 int argc, const char * argv[])
{
    
    m_fPvrs << ";";
    for(int i = 1; i < argc; ++i)
    {
        m_fPvrs << " " << argv[i];
    }
    m_fPvrs << "\n";
    return TCL_OK;
}

int
rcpTRSCompiler::linecomment_proc(ClientData d, Tcl_Interp *pInterp,
                                 int argc, const char * argv[])
{
    
    m_fPvrs << "//";
    for(int i = 1; i < argc; ++i)
    {
        m_fPvrs << " " << argv[i];
    }
    m_fPvrs << "\n";
    return TCL_OK;
}

int
rcpTRSCompiler::print_proc(ClientData d, Tcl_Interp *pInterp,
                           int argc, const char * argv[])
{
    if(m_isInRulecheck)
    {
        m_fPvrs << "\t";
    }

    for(int i = 1; i < argc; ++i)
    {
        if( i == 1)
            m_fPvrs << argv[i];
        else
            m_fPvrs << " " << argv[i];
    }
    m_fPvrs << std::endl;
    return TCL_OK;
}

int
rcpTRSCompiler::echo_to_pvrs_file_proc(ClientData d, Tcl_Interp *pInterp,
                                         int argc, const char * argv[])
{
    if(argc < 2)
    {
        setTRSCompileError(pInterp, argc, argv);
        return TCL_ERROR;
    }

    if(m_isInRulecheck)
    {
        m_fPvrs << "\t";
    }

    for( int i = 1; i < argc; ++i)
    {
        if(i == 1)
            m_fPvrs << argv[i];
        else
            m_fPvrs << " " << argv[i];
    }
    m_fPvrs << std::endl;
    return TCL_OK;
}

int rcpTRSCompiler::output_to_pvrs_proc(ClientData d, Tcl_Interp *pInterp, int argc, const char *argv[])
{
    return echo_to_pvrs_file_proc(d, pInterp, argc, argv);
}


int
rcpTRSCompiler::pvrscmd_proc(ClientData d, Tcl_Interp *pInterp,
                             int argc, const char * argv[])
{
    if(m_isInRulecheck)
        m_fPvrs << "\t";

    char *pCmd = strncmp(argv[0], "trs::", 5) == 0 ? const_cast<char *>(argv[0]) + 5
                                                   : const_cast<char *>(argv[0]);
    m_fPvrs << pCmd;
    for(int i = 1; i < argc; ++i)
    {
        m_fPvrs << " " << argv[i];
    }
    m_fPvrs << std::endl;
    return TCL_OK;
}

int
rcpTRSCompiler::pvrs_block_proc(ClientData d, Tcl_Interp *pInterp,
                                int argc, const char * argv[])
{
    for(int i = 1; i < argc; ++i)
    {
        if(i != 1)
            m_fPvrs << " ";
        m_fPvrs << argv[i];
    }
    m_fPvrs << std::endl;
    return TCL_OK;
}

int
rcpTRSCompiler::trs_proc(ClientData d, Tcl_Interp *pInterp,
                         int argc, const char * argv[])
{

    m_fPvrs << "TRS ";
    for(int i = 1; i < argc; ++i)
    {
        
        if( i != 1 )
            m_fPvrs << " ";
        m_fPvrs << argv[i];
    }
    m_fPvrs << std::endl;
    return TCL_OK;
}

int
rcpTRSCompiler::get_global_var_proc(ClientData d, Tcl_Interp *pInterp,
                                    int argc, const char * argv[])
{
    if(argc < 2)
    {
        setTRSCompileError(pInterp, argc, argv);
        return TCL_ERROR;
    }

    std::string sVariableName;
    for(int i = 1; i < argc; ++i)
    {
        std::string sValue = argv[i];
        trim(sValue, "(),");
        if(sValue.empty())
            continue;

        sVariableName += sValue;
        sVariableName += " ";
    }

    if(varmap_g.find(sVariableName) != varmap_g.end())
    {
        Tcl_SetResult(pInterp, const_cast<char*>(varmap_g[sVariableName].c_str()), TCL_VOLATILE);
    }
    else
    {
        Tcl_SetResult(pInterp, NULL, TCL_STATIC);
    }
    return TCL_OK;
}

int
rcpTRSCompiler::set_global_var_proc(ClientData d, Tcl_Interp *pInterp,
                                    int argc, const char * argv[])
{
    if(argc < 3)
    {
        setTRSCompileError(pInterp, argc, argv);
        return TCL_ERROR;
    }

    std::string sVariableName;
    for(int i = 1; i < argc - 1; ++i)
    {
        std::string sValue = argv[i];
        trim(sValue, "(),");
        if(sValue.empty())
            continue;

        sVariableName += sValue;
        sVariableName += " ";
    }

    pInterp->result = const_cast<char*>(argv[argc - 1]);
    varmap_g[sVariableName] = std::string(argv[argc - 1]);
    return TCL_OK;
}

int
rcpTRSCompiler::unset_global_var_proc(ClientData d, Tcl_Interp *pInterp,
                                      int argc, const char * argv[])
{
    if(argc < 2)
    {
        setTRSCompileError(pInterp, argc, argv);
        return TCL_ERROR;
    }

    std::string sVariableName;
    for(int i = 1; i < argc; ++i)
    {
        std::string sValue = argv[i];
        trim(sValue, "(),");
        if(sValue.empty())
            continue;

        sVariableName += sValue;
        sVariableName += " ";
    }

    if(varmap_g.find(sVariableName) != varmap_g.end())
        varmap_g.erase(sVariableName);

    return TCL_OK;
}

int
rcpTRSCompiler::exists_global_var_proc(ClientData d, Tcl_Interp *pInterp,
                                       int argc, const char * argv[])
{
    if(argc < 2)
    {
        setTRSCompileError(pInterp, argc, argv);
        return TCL_ERROR;
    }

    std::string sVariableName;
    for(int i = 1; i < argc; ++i)
    {
        std::string sValue = argv[i];
        trim(sValue, "(),");
        if(sValue.empty())
            continue;

        sVariableName += sValue;
        sVariableName += " ";
    }

    if(varmap_g.find(sVariableName) != varmap_g.end())
        Tcl_SetResult(pInterp, "1\0", TCL_STATIC);
    else
        Tcl_SetResult(pInterp, "0\0", TCL_STATIC);

    return TCL_OK;
}


int
rcpTRSCompiler::gettrsarg_proc(ClientData d, Tcl_Interp *pInterp,
                               int argc, const char * argv[])
{
    Tcl_SetResult(pInterp, &m_pTrsArg[0], TCL_VOLATILE);
    return TCL_OK;
}

int
rcpTRSCompiler::rulecheck_proc(ClientData d, Tcl_Interp *pInterp,
                               int argc, const char * argv[])
{
    
    m_isInRulecheck = true;
    if(argc < 3)
    {
        setTRSCompileError(pInterp, argc, argv);
        return TCL_ERROR;
    }

    std::string str = "";
    for(int i = 2; i < argc; ++i)
    {
        str += argv[i];
    }

    m_fPvrs << "RULE " << argv[1] << " {\n";
    int iRet = Tcl_RecordAndEval(pInterp, (char*)(str.c_str()), 0);
    if(iRet == TCL_ERROR)
    {
        setTRSCompileError(pInterp, argc, argv);
        return TCL_ERROR;
    }
    m_fPvrs << "}" << std::endl;

    m_isInRulecheck = false;
    return TCL_OK;
}

void
rcpTRSCompiler::freeResult(char *pValue)
{
    ::free(pValue);
}

static inline int
convertEnclosureMeasurementsToDrcCmds(Tcl_Interp *pInterp, int argc, const char * argv[],
                                      std::string &sValue, std::ofstream &fSvrf)
{
    static hvUInt32 iLayer = 1;
    static std::string sBreak = "__";
    std::string sBaseLayer;
    std::string sMeasLayer;
    std::string sOrientLayer;
    std::string sMaxPar;
    std::string sMaxPer;

    bool        isConnect = false;
    for(int iPara = 1; iPara < argc; ++iPara)
    {
        if(strcmp(argv[iPara], "-base") == 0)
        {
            if(!sBaseLayer.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sBaseLayer = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-measurement") == 0)
        {
            if(!sMeasLayer.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sMeasLayer = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-orient") == 0)
        {
            if(!sOrientLayer.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sOrientLayer = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-max_par") == 0)
        {
            if(!sMaxPar.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sMaxPar = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-max_per") == 0)
        {
            if(!sMaxPer.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sMaxPer = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-connect") == 0)
        {
            isConnect = true;
        }
        else if(strcmp(argv[iPara], "-help") == 0 || strcmp(argv[iPara], "-?") == 0)
        {
            return TCL_ERROR;
        }
    }

    if(sMaxPar.empty())
    {
        const char *pValue = Tcl_GetVar(pInterp, "device::max_par", TCL_NAMESPACE_ONLY);
        if(pValue != NULL)
            sMaxPar = pValue;
        else
            return TCL_ERROR;
    }

    if(sMaxPer.empty())
    {
        const char *pValue = Tcl_GetVar(pInterp, "device::max_per", TCL_NAMESPACE_ONLY);
        if(pValue != NULL)
            sMaxPer = pValue;
        else
            return TCL_ERROR;
    }

    if(sMaxPer.empty() || sMaxPar.empty() || sBaseLayer.empty() ||
       sMeasLayer.empty() || sOrientLayer.empty())
        return TCL_ERROR;

    double fMaxPer;
    if(TCL_ERROR == Tcl_GetDouble(pInterp, sMaxPer.c_str(), &fMaxPer))
        return TCL_ERROR;

    double fMaxPar;
    if(TCL_ERROR == Tcl_GetDouble(pInterp, sMaxPar.c_str(), &fMaxPar))
        return TCL_ERROR;

    std::string sTmpLayerPrefix;

    sTmpLayerPrefix += sBaseLayer;
    sTmpLayerPrefix += sBreak;
    sTmpLayerPrefix += sMeasLayer;
    sTmpLayerPrefix += sBreak;
    sTmpLayerPrefix += "enc__TRS_tmp_";
    sTmpLayerPrefix += itoa(iLayer);

    std::string side_orient = sTmpLayerPrefix;
    side_orient += ".side_orient";

    fSvrf << side_orient << " = edge_coincident( both_side " << sBaseLayer << " " << sOrientLayer << ")\n";

    std::string end_orient = sTmpLayerPrefix;
    end_orient += ".end_orient";

    fSvrf << end_orient << " = ~edge_coincident( both_side " << sBaseLayer << " " << sOrientLayer << ")\n";

    std::string side_enclosure_dfmspace = sTmpLayerPrefix;
    side_enclosure_dfmspace += ".side_enclosure.dfmspace";

    fSvrf << side_enclosure_dfmspace << " = dfm_check_space( by_enc " << side_orient << " " << sMeasLayer
          << " <= " << sMaxPer << " shield_layer " << sMeasLayer << " shield_level == 0" << ")\n";

    std::string end_enclosure_dfmspace = sTmpLayerPrefix;
    end_enclosure_dfmspace += ".end_enclosure.dfmspace";

    fSvrf << end_enclosure_dfmspace << " = dfm_check_space( by_enc " << end_orient << " " << sMeasLayer
          << " <= " << sMaxPar << " shield_layer " << sMeasLayer << " shield_level == 0" << ")\n";

    if(!isConnect)
    {
        std::string side = sTmpLayerPrefix;
        side += ".side";

        fSvrf << side << " = dfm_build_property( " << side_orient << " " << side_enclosure_dfmspace
              << " overlapped adjacent multi_cluster\n [PER = SELECT_XXY(SORT_XXY(CONCAT(VECTOR(EP_MIN("
              << side_enclosure_dfmspace << "), EP_MAX(" << side_enclosure_dfmspace << "), ED("
              << side_enclosure_dfmspace << ")), VECTOR(EP_MIN(" << side_orient << "), EP_MAX(" << side_orient
              << "), " << sMaxPer << "))), VECTOR(EP_MIN(" << side_orient << "), EP_MAX(" << side_orient << ")))]" << ")\n\n";

        std::string end = sTmpLayerPrefix;
        end += ".end";

        fSvrf << end << " = dfm_build_property( " << end_orient << " " << end_enclosure_dfmspace
              << " overlapped adjacent multi_cluster\n [PAR = SELECT_XXY(SORT_XXY(CONCAT(VECTOR(EP_MIN("
              << end_enclosure_dfmspace << "), EP_MAX(" << end_enclosure_dfmspace << "), ED(" << end_enclosure_dfmspace
              << ")), VECTOR(EP_MIN(" << end_orient << "), EP_MAX(" << end_orient << "), " << sMaxPar << "))), VECTOR(EP_MIN("
              << end_orient << "), EP_MAX(" << end_orient << ")))]" << ")\n\n";

        sValue.clear();

        sValue += "dfm_build_property( ";
        sValue += sBaseLayer;
        sValue += " ";
        sValue += side;
        sValue += " ";
        sValue += end;
        sValue += " overlapped adjacent\n \t[PER_1 = V_PROPERTY(";
        sValue += side;
        sValue += ", PER, 1)]\n";
        sValue += "\t[PER_2 = V_PROPERTY(";
        sValue += side;
        sValue += ", PER, 2)]\n";
        sValue += "\t[PAR_1 = V_PROPERTY(";
        sValue += end;
        sValue += ", PAR, 1)]\n";
        sValue += "\t[PAR_2 = V_PROPERTY(";
        sValue += end;
        sValue += ", PAR, 2)])\n";
    }
    else
    {
        std::string side_netid = sTmpLayerPrefix;
        side_netid += ".side.netid";

        fSvrf << side_netid << " = DFM PROPERTY " << sOrientLayer << " "
              << "[NETID = NETID(" << sOrientLayer << ")]\n";

        std::string side = sTmpLayerPrefix;
        side += ".side";

        fSvrf << side << " = DFM PROPERTY " << side_orient << " " << side_enclosure_dfmspace
              << " " << side_netid << " OVERLAP ABUT ALSO MULTI\n\t[PER = RANGE_XXY(SORT_MERGE_XXY(CONCAT(VECTOR(ECMIN("
              << side_enclosure_dfmspace << "), ECMAX(" << side_enclosure_dfmspace << "), EW(" << side_enclosure_dfmspace
              << ")), VECTOR(ECMIN(" << side_orient << "), ECMAX(" << side_orient << "), " << sMaxPer << "))), VECTOR(ECMIN("
              << side_orient << "), ECMAX(" << side_orient << ")))]" << "\n\t[NETID = NETPROPERTY(" << side_netid << ", \"NETID\", 1)]" << "\n\n";

        std::string end = sTmpLayerPrefix;
        end += ".end";

        fSvrf << end << " = DFM PROPERTY " << end_orient << " " << end_enclosure_dfmspace
              << " OVERLAP ABUT ALSO MULTI\n [PAR = RANGE_XXY(SORT_MERGE_XXY(CONCAT(VECTOR(ECMIN("
              << end_enclosure_dfmspace << "), ECMAX(" << end_enclosure_dfmspace << "), EW(" << end_enclosure_dfmspace
              << ")), VECTOR(ECMIN(" << end_orient << "), ECMAX(" << end_orient << "), " << sMaxPar << "))), VECTOR(ECMIN("
              << end_orient << "), ECMAX(" << end_orient << ")))]" << "\n\n";

        sValue.clear();

        sValue += "DFM PROPERTY ";
        sValue += sBaseLayer;
        sValue += " ";
        sValue += side;
        sValue += " ";
        sValue += end;
        sValue += " OVERLAP ABUT ALSO\n \t[PER_1 = VPROPERTY(";
        sValue += side;
        sValue += ", PER, 1)]\n";
        sValue += "\t[PER_2 = VPROPERTY(";
        sValue += side;
        sValue += ", PER, 2)]\n";
        sValue += "\t[PAR_1 = VPROPERTY(";
        sValue += end;
        sValue += ", PAR, 1)]\n";
        sValue += "\t[PAR_2 = VPROPERTY(";
        sValue += end;
        sValue += ", PAR, 2)]\n";
        sValue += "\t[PER_NETID_1 = NETPROPERTY(";
        sValue += side;
        sValue += ", NETID, 1) ]\n";
        sValue += "\t[PER_NETID_2 = NETPROPERTY(";
        sValue += side;
        sValue += ", NETID, 2) ]\n";
    }
    sValue += "\n";

    ++iLayer;

    return TCL_OK;
}

int
rcpTRSCompiler::enclosure_measurements_proc(ClientData d, Tcl_Interp *pInterp,
                                            int argc, const char * argv[])
{
    static std::string sMessage = "\nlvs : device::enclosure_measurements [options] \noptions:\n\
    -base value          Base layer (usually the gate layer) <>\n\
    -measurement value   Measurement layer (usually the well layer) <>\n\
    -orient value        Orientation layer (usually the src/drn layer) <>\n\
    -max_par value       Base layer to measurement layer maximum search distance override (PAR) <-1.0>\n\
    -max_per value       Base layer to measurement layer maximum search distance override (PER) <-1.0>\n\
    -connect             Store PER_NETID_1 and PER_NETID_2, -orient must be connected layer\n\
    -help                Print this message\n\
    -?                   Print this message\n\n";

    std::string sResult;
    if(TCL_ERROR == convertEnclosureMeasurementsToDrcCmds(pInterp, argc, argv, sResult, m_fPvrs))
    {
        Tcl_Obj *pResult = Tcl_NewStringObj(sMessage.c_str(), sMessage.size());
        Tcl_SetObjResult(pInterp, pResult);

        return TCL_ERROR;
    }

    Tcl_Obj *pResult = Tcl_NewStringObj(sResult.c_str(), sResult.size());
    Tcl_SetObjResult(pInterp, pResult);

    return TCL_OK;
}

static inline int
convertStrainedSiliconMeasurementsToDrcCmds(Tcl_Interp *pInterp, int argc, const char * argv[],
                                            std::string &sValue, std::ofstream &fSvrf)
{
    static hvUInt32 iLayer = 1;
    static std::string sBreak = "__";
    std::string sGateLayer;
    std::string sSdLayer;
    std::string sActiveLayer;
    std::string sPolyLayer;
    std::string sLayer1;
    std::string sMaxSp;
    std::string sMaxSp2;
    std::string sMaxSd;
    std::string sMaxSe;
    std::string sMaxSf;
    std::string sMaxSode;
    std::string sMaxSodf;
    std::string sMaxSle;
    std::string sMaxSlf;
    std::string sMaxSodlf;
    std::string sLatSodlf;

    for(int iPara = 1; iPara < argc; ++iPara)
    {
        if(strcmp(argv[iPara], "-gate") == 0)
        {
            if(!sGateLayer.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sGateLayer = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-sd") == 0)
        {
            if(!sSdLayer.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sSdLayer = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-active") == 0)
        {
            if(!sActiveLayer.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sActiveLayer = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-poly") == 0)
        {
            if(!sPolyLayer.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sPolyLayer = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-layer1") == 0)
        {
            if(!sLayer1.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sLayer1 = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-max_sp") == 0)
        {
            if(!sMaxSp.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sMaxSp = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-max_sp2") == 0)
        {
            if(!sMaxSp2.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sMaxSp2 = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-max_sd") == 0)
        {
            if(!sMaxSd.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sMaxSd = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-max_se") == 0)
        {
            if(!sMaxSe.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sMaxSe = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-max_sf") == 0)
        {
            if(!sMaxSf.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sMaxSf = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-max_sode") == 0)
        {
            if(!sMaxSode.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sMaxSode = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-max_sodf") == 0)
        {
            if(!sMaxSodf.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sMaxSodf = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-max_sle") == 0)
        {
            if(!sMaxSle.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sMaxSle = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-max_slf") == 0)
        {
            if(!sMaxSlf.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sMaxSlf = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-max_sodlf") == 0)
        {
            if(!sMaxSodlf.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sMaxSodlf = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-lat_sodlf") == 0)
        {
            if(!sLatSodlf.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sLatSodlf = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-help") == 0 || strcmp(argv[iPara], "-?") == 0)
        {
            return TCL_ERROR;
        }
    }

    if(sMaxSp.empty())
    {
        const char *pValue = Tcl_GetVar(pInterp, "device::max_sp", TCL_NAMESPACE_ONLY);
        if(pValue != NULL)
            sMaxSp = pValue;
        else
            return TCL_ERROR;
    }

    if(sMaxSp2.empty())
    {
        const char *pValue = Tcl_GetVar(pInterp, "device::max_sp2", TCL_NAMESPACE_ONLY);
        if(pValue != NULL)
            sMaxSp2 = pValue;
        else
            return TCL_ERROR;
    }
    if(sMaxSd.empty())
    {
        const char *pValue = Tcl_GetVar(pInterp, "device::max_sd", TCL_NAMESPACE_ONLY);
        if(pValue != NULL)
            sMaxSd = pValue;
        else
            return TCL_ERROR;
    }
    if(sMaxSe.empty())
    {
        const char *pValue = Tcl_GetVar(pInterp, "device::max_se", TCL_NAMESPACE_ONLY);
        if(pValue != NULL)
            sMaxSe = pValue;
        else
            return TCL_ERROR;
    }
    if(sMaxSf.empty())
    {
        const char *pValue = Tcl_GetVar(pInterp, "device::max_sf", TCL_NAMESPACE_ONLY);
        if(pValue != NULL)
            sMaxSf = pValue;
        else
            return TCL_ERROR;
    }
    if(sMaxSode.empty())
    {
        const char *pValue = Tcl_GetVar(pInterp, "device::max_sode", TCL_NAMESPACE_ONLY);
        if(pValue != NULL)
            sMaxSode = pValue;
        else
            return TCL_ERROR;
    }
    if(sMaxSodf.empty())
    {
        const char *pValue = Tcl_GetVar(pInterp, "device::max_sodf", TCL_NAMESPACE_ONLY);
        if(pValue != NULL)
            sMaxSodf = pValue;
        else
            return TCL_ERROR;
    }
    if(sMaxSle.empty())
    {
        const char *pValue = Tcl_GetVar(pInterp, "device::max_sle", TCL_NAMESPACE_ONLY);
        if(pValue != NULL)
            sMaxSle = pValue;
        else
            return TCL_ERROR;
    }
    if(sMaxSlf.empty())
    {
        const char *pValue = Tcl_GetVar(pInterp, "device::max_slf", TCL_NAMESPACE_ONLY);
        if(pValue != NULL)
            sMaxSlf = pValue;
        else
            return TCL_ERROR;
    }
    if(sMaxSodlf.empty())
    {
        const char *pValue = Tcl_GetVar(pInterp, "device::max_sodlf", TCL_NAMESPACE_ONLY);
        if(pValue != NULL)
            sMaxSodlf = pValue;
        else
            return TCL_ERROR;
    }
    if(sLatSodlf.empty())
    {
        const char *pValue = Tcl_GetVar(pInterp, "device::lat_sodlf", TCL_NAMESPACE_ONLY);
        if(pValue != NULL)
            sLatSodlf = pValue;
        else
            return TCL_ERROR;
    }

    if(sMaxSp2.empty() || sMaxSp.empty() || sMaxSd.empty() || sMaxSf.empty() ||
       sMaxSode.empty() || sMaxSodf.empty()  || sMaxSle.empty() || sMaxSlf.empty() ||
       sLatSodlf.empty() || sMaxSodlf.empty() || sGateLayer.empty() || sSdLayer.empty() ||
       sActiveLayer.empty() || sPolyLayer.empty() || sLayer1.empty())
        return TCL_ERROR;

    double fMaxSp;
    if(TCL_ERROR == Tcl_GetDouble(pInterp, sMaxSp.c_str(), &fMaxSp))
        return TCL_ERROR;
    double fMaxSp2;
    if(TCL_ERROR == Tcl_GetDouble(pInterp, sMaxSp2.c_str(), &fMaxSp2))
        return TCL_ERROR;
    double fMaxSd;
    if(TCL_ERROR == Tcl_GetDouble(pInterp, sMaxSd.c_str(), &fMaxSd))
        return TCL_ERROR;
    double fMaxSe;
    if(TCL_ERROR == Tcl_GetDouble(pInterp, sMaxSe.c_str(), &fMaxSe))
        return TCL_ERROR;
    double fMaxSf;
    if(TCL_ERROR == Tcl_GetDouble(pInterp, sMaxSf.c_str(), &fMaxSf))
        return TCL_ERROR;
    double fMaxSode;
    if(TCL_ERROR == Tcl_GetDouble(pInterp, sMaxSode.c_str(), &fMaxSode))
        return TCL_ERROR;
    double fMaxSodf;
    if(TCL_ERROR == Tcl_GetDouble(pInterp, sMaxSodf.c_str(), &fMaxSodf))
        return TCL_ERROR;
    double fMaxSle;
    if(TCL_ERROR == Tcl_GetDouble(pInterp, sMaxSle.c_str(), &fMaxSle))
        return TCL_ERROR;
    double fMaxSlf;
    if(TCL_ERROR == Tcl_GetDouble(pInterp, sMaxSlf.c_str(), &fMaxSlf))
        return TCL_ERROR;
    double fMaxSodlf;
    if(TCL_ERROR == Tcl_GetDouble(pInterp, sMaxSodlf.c_str(), &fMaxSodlf))
        return TCL_ERROR;
    double fLatSodlf;
    if(TCL_ERROR == Tcl_GetDouble(pInterp, sLatSodlf.c_str(), &fLatSodlf))
        return TCL_ERROR;

    std::string sTmpLayerPrefix;

    sTmpLayerPrefix += sGateLayer;
    sTmpLayerPrefix += sBreak;
    sTmpLayerPrefix += sLayer1;
    sTmpLayerPrefix += sBreak;
    sTmpLayerPrefix += "strsil__TRS_tmp_";
    sTmpLayerPrefix += itoa(iLayer);

    std::string side_orient = sTmpLayerPrefix;
    side_orient += ".side_orient";

    fSvrf << side_orient << " = COINCIDENT EDGE " << sGateLayer << " " << sSdLayer << std::endl;

    std::string end_orient = sTmpLayerPrefix;
    end_orient += ".end_orient";

    fSvrf << end_orient << " = NOT COINCIDENT EDGE " << sGateLayer << " " << sSdLayer << "\n";

    std::string g2slfod_lat_sodlf_prep = sTmpLayerPrefix;
    g2slfod_lat_sodlf_prep += ".g2slfod.lat_sodlf.prep";

    fSvrf << g2slfod_lat_sodlf_prep << " = " << sGateLayer << " OR (EXPAND EDGE "
          << side_orient << " BY " << sLatSodlf << ")\n";

    std::string g2slfod_lat_sodlf = sTmpLayerPrefix;
    g2slfod_lat_sodlf += ".g2slfod.lat_sodlf";

    fSvrf << g2slfod_lat_sodlf << " = TOUCH EDGE " << g2slfod_lat_sodlf_prep << " " << end_orient << "\n";

    std::string g2sp_dfmspace = sTmpLayerPrefix;
    g2sp_dfmspace += ".g2sp.dfmspace";

    fSvrf << g2sp_dfmspace << " = DFM SPACE " << side_orient << " " << sPolyLayer
          << " <= " << sMaxSp2 << " BY EXT BY LAYER " << sPolyLayer << " COUNT <= 2\n";

    std::string g2sd_dfmspace = sTmpLayerPrefix;
    g2sd_dfmspace += ".g2sd.dfmspace";

    fSvrf << g2sd_dfmspace << " = DFM SPACE " << side_orient << " " << sActiveLayer
          << " <= " << sMaxSd << " BY ENC BY LAYER " << sActiveLayer << " COUNT == 0\n";

    std::string g2se_dfmspace = sTmpLayerPrefix;
    g2se_dfmspace += ".g2se.dfmspace";

    fSvrf << g2se_dfmspace << " = DFM SPACE " << side_orient << " " << sLayer1
          << " <= " << sMaxSe << " BY ENC BY LAYER " << sLayer1 << " COUNT == 0\n";

    std::string g2sf_dfmspace = sTmpLayerPrefix;
    g2sf_dfmspace += ".g2sf.dfmspace";

    fSvrf << g2sf_dfmspace << " = DFM SPACE " << side_orient << " " << sLayer1
          << " <= " << sMaxSf << " BY EXT BY LAYER " << sLayer1 << " COUNT <= 1 MEASURE ALL\n";

    std::string g2seod_dfmspace = sTmpLayerPrefix;
    g2seod_dfmspace += ".g2seod.dfmspace";

    fSvrf << g2seod_dfmspace << " = DFM SPACE " << side_orient << " " << sActiveLayer
          << " <= " << sMaxSode << " BY ENC BY LAYER " << sActiveLayer << " COUNT == 0\n";

    std::string g2sfod_dfmspace = sTmpLayerPrefix;
    g2sfod_dfmspace += ".g2sfod.dfmspace";

    fSvrf << g2sfod_dfmspace << " = DFM SPACE " << side_orient << " " << sActiveLayer
          << " <= " << sMaxSodf << " BY EXT BY LAYER " << sActiveLayer << " COUNT <= 1 MEASURE ALL\n";

    std::string g2sle_dfmspace = sTmpLayerPrefix;
    g2sle_dfmspace += ".g2sle.dfmspace";

    fSvrf << g2sle_dfmspace << " = DFM SPACE " << end_orient << " " << sLayer1
          << " <= " << sMaxSle << " BY ENC BY LAYER " << sLayer1 << " COUNT == 0\n";

    std::string g2slf_dfmspace = sTmpLayerPrefix;
    g2slf_dfmspace += ".g2slf.dfmspace";

    fSvrf << g2slf_dfmspace << " = DFM SPACE " << end_orient << " " << sLayer1 << " <= "
          << sMaxSlf << " BY EXT BY LAYER " << sLayer1 << " COUNT <= 1 MEASURE ALL\n";

    std::string g2slfod_dfmspace = sTmpLayerPrefix;
    g2slfod_dfmspace += ".g2slfod.dfmspace";

    fSvrf << g2slfod_dfmspace << " = DFM SPACE " << g2slfod_lat_sodlf << " " << sActiveLayer
          << " <= " << sMaxSodlf << " BY EXT BY LAYER " << sActiveLayer << " COUNT <= 1 MEASURE ALL\n";

    std::string pse_combine = sTmpLayerPrefix;
    pse_combine += ".pse.combine";

    fSvrf << pse_combine << " = DFM PROPERTY " << side_orient << " " << g2sp_dfmspace << " "
          << g2sd_dfmspace << " OVERLAP ABUT ALSO MULTI \n [PSE_RANGE = VECTOR(ECMIN(" << side_orient
          << "), ECMAX(" << side_orient << "))]\n [PSE_POLY_SPACE_1 = CONCAT(VECTOR(ECMIN(" << g2sp_dfmspace
          << "), ECMAX(" << g2sp_dfmspace << "), EW(" << g2sp_dfmspace << ")), VECTOR(ECMIN(" << side_orient
          << "), ECMAX(" << side_orient << "), " << sMaxSp << "))]\n [PSE_POLY_SPACE_2 = CONCAT(VECTOR(ECMIN("
          << g2sp_dfmspace << "), ECMAX(" << g2sp_dfmspace << "), EW(" << g2sp_dfmspace << ")), VECTOR(ECMIN("
          << side_orient << "), ECMAX(" << side_orient << "), " << sMaxSp2 << "))]\n [PSE_DIFF_ENC = CONCAT(VECTOR(ECMIN("
          << g2sd_dfmspace << "), ECMAX(" << g2sd_dfmspace << "), EW(" << g2sd_dfmspace << ")), VECTOR(ECMIN("
          << side_orient << "), ECMAX(" << side_orient << "), " << sMaxSd << "))]\n\n";

    std::string pse_select = sTmpLayerPrefix;
    pse_select += ".pse.select";

    fSvrf << pse_select << " = DFM PROPERTY " << pse_combine << "\n [PSE = RANGE_XXY(SELECT_MERGE_XXY(1, VPROPERTY("
          << pse_combine << ", \"PSE_POLY_SPACE_1\"), 2, VPROPERTY(" << pse_combine << ", \"PSE_POLY_SPACE_2\"), 1, VPROPERTY("
          << pse_combine << ", \"PSE_DIFF_ENC\")), VPROPERTY(" << pse_combine << ", \"PSE_RANGE\"))]\n\n";

    std::string be_side_combine = sTmpLayerPrefix;
    be_side_combine += ".be_side.combine";

    fSvrf << be_side_combine << " = DFM PROPERTY " << side_orient << " " << g2sp_dfmspace
          << " " << g2se_dfmspace << " " << g2sf_dfmspace << " OVERLAP ABUT ALSO MULTI \n [BE_RANGE = VECTOR(ECMIN("
          << side_orient << "), ECMAX(" << side_orient << "))]\n [BE_POLY_SPACE = CONCAT(VECTOR(ECMIN("
          << g2sp_dfmspace << "), ECMAX(" << g2sp_dfmspace << "), EW(" << g2sp_dfmspace << ")), VECTOR(ECMIN("
          << side_orient << "), ECMAX(" << side_orient << "), " << sMaxSp << "))]\n [BE_LAYER1_ENC = CONCAT(VECTOR(ECMIN("
          << g2se_dfmspace << "), ECMAX(" << g2se_dfmspace << "), EW(" << g2se_dfmspace << ")), VECTOR(ECMIN(" << side_orient
          << "), ECMAX(" << side_orient << "), " << sMaxSe << "))]\n [BE_LAYER1_EXT = CONCAT(VECTOR(ECMIN(" << g2sf_dfmspace
          << "), ECMAX(" << g2sf_dfmspace << "), EW(" << g2sf_dfmspace << ")), VECTOR(ECMIN(" << side_orient << "), ECMAX("
          << side_orient << "), " << sMaxSf << "))]\n\n";

    std::string be_side_select = sTmpLayerPrefix;
    be_side_select += ".be_side.select";

    fSvrf << be_side_select << " = DFM PROPERTY " << be_side_combine << "\n [PSE = RANGE_XXY(SORT_MERGE_XXY(VPROPERTY("
          << be_side_combine << ", \"BE_POLY_SPACE\"), VPROPERTY(" << be_side_combine << ", \"BE_LAYER1_ENC\"), VPROPERTY("
          << be_side_combine << ", \"BE_LAYER1_EXT\")), VPROPERTY(" << be_side_combine << ", \"BE_RANGE\"))]\n\n";

    std::string lod_side_combine = sTmpLayerPrefix;
    lod_side_combine += ".lod_side.combine";

    fSvrf << lod_side_combine << " = DFM PROPERTY " << side_orient << " " << g2sp_dfmspace << " "
          << g2seod_dfmspace << " " << g2sfod_dfmspace << " OVERLAP ABUT ALSO MULTI \n [LOD_RANGE = VECTOR(ECMIN("
          << side_orient << "), ECMAX(" << side_orient << "))]\n [LOD_POLY_SPACE = CONCAT(VECTOR(ECMIN(" << g2sp_dfmspace
          << "), ECMAX(" << g2sp_dfmspace << "), EW(" << g2sp_dfmspace << ")), VECTOR(ECMIN(" << side_orient << "), ECMAX("
          << side_orient << "), " << sMaxSp << "))]\n [LOD_DIFF_ENC = CONCAT(VECTOR(ECMIN(" << g2seod_dfmspace << "), ECMAX("
          << g2seod_dfmspace << "), EW(" << g2seod_dfmspace << ")), VECTOR(ECMIN(" << side_orient << "), ECMAX(" << side_orient
          << "), " << sMaxSode << "))]\n [LOD_DIFF_EXT = CONCAT(VECTOR(ECMIN(" << g2sfod_dfmspace << "), ECMAX(" << g2sfod_dfmspace
          << "), EW(" << g2sfod_dfmspace << ")), VECTOR(ECMIN(" << side_orient << "), ECMAX(" << side_orient << "), " << sMaxSodf << "))]\n\n";

    std::string lod_side_select = sTmpLayerPrefix;
    lod_side_select += ".lod_side.select";

    fSvrf << lod_side_select << " = DFM PROPERTY " << lod_side_combine << "\n [PSE = RANGE_XXY(SORT_MERGE_XXY(VPROPERTY("
          << lod_side_combine << ", \"LOD_POLY_SPACE\"), VPROPERTY(" << lod_side_combine << ", \"LOD_DIFF_ENC\"), VPROPERTY("
          << lod_side_combine << ", \"LOD_DIFF_EXT\")), VPROPERTY(" << lod_side_combine << ", \"LOD_RANGE\"))]\n\n";

    std::string side = sTmpLayerPrefix;
    side += ".side";

    fSvrf << side << " = DFM PROPERTY " << side_orient << " " << pse_select << " " << be_side_select
          << " " << lod_side_select << " OVERLAP ABUT ALSO MULTI\n [PSE = VPROPERTY(" << pse_select
          << ", \"PSE\")]\n [BE = VPROPERTY(" << be_side_select << ", \"BE\")]\n [LOD = VPROPERTY("
          << lod_side_select << ", \"LOD\")]\n\n";

    std::string be_end_combine = sTmpLayerPrefix;
    be_end_combine += ".be_end.combine";

    fSvrf << be_end_combine << " = DFM PROPERTY " << end_orient << " " << g2sle_dfmspace
          << " " << g2slf_dfmspace << " OVERLAP ABUT ALSO MULTI \n [BE_RANGE = VECTOR(ECMIN("
          << end_orient << "), ECMAX(" << end_orient << "))]\n [BE_LAYER1_ENC = CONCAT(VECTOR(ECMIN("
          << g2sle_dfmspace << "), ECMAX(" << g2sle_dfmspace << "), EW(" << g2sle_dfmspace << ")), VECTOR(ECMIN("
          << end_orient << "), ECMAX(" << end_orient << "), " << sMaxSle << "))]\n [BE_LAYER1_EXT = CONCAT(VECTOR(ECMIN("
          << g2slf_dfmspace << "), ECMAX(" << g2slf_dfmspace << "), EW(" << g2slf_dfmspace << ")), VECTOR(ECMIN("
          << end_orient << "), ECMAX(" << end_orient << "), " << sMaxSlf << "))]\n\n";

    std::string be_end_select = sTmpLayerPrefix;
    be_end_select += ".be_end.select";

    fSvrf << be_end_select << " = DFM PROPERTY " << be_end_combine << "\n [BE = RANGE_XXY(SORT_MERGE_XXY(VPROPERTY("
          << be_end_combine << ", \"BE_LAYER1_ENC\"), VPROPERTY(" << be_end_combine << ", \"BE_LAYER1_EXT\")), VPROPERTY("
          << be_end_combine << ", \"BE_RANGE\"))]\n\n";

    std::string lod_end_combine = sTmpLayerPrefix;
    lod_end_combine += ".lod_end.combine";

    fSvrf << lod_end_combine << " = DFM PROPERTY " << g2slfod_lat_sodlf << " "
          << g2slfod_dfmspace << " OVERLAP ABUT ALSO MULTI \n [LOD_DIFF_EXT = CONCAT(VECTOR(ECMIN("
          << g2slfod_dfmspace << "), ECMAX(" << g2slfod_dfmspace << "), EW(" << g2slfod_dfmspace << ")), VECTOR(ECMIN("
          << g2slfod_lat_sodlf << "), ECMAX(" << g2slfod_lat_sodlf << ")," << sMaxSodlf << "))]\n\n";

    std::string lod_end_select = sTmpLayerPrefix;
    lod_end_select += ".lod_end.select";

    fSvrf << lod_end_select << " = DFM PROPERTY " << end_orient << " " << lod_end_combine
          << " OVERLAP ABUT ALSO MULTI \n [LOD = RANGE_XXY(SORT_MERGE_XXY(VPROPERTY("
          << lod_end_combine << ", \"LOD_DIFF_EXT\")), VECTOR(ECMIN(" << end_orient << ") - "
          << std::setiosflags(std::ios::fixed) << fLatSodlf * 0.999999 << ", ECMAX(" << end_orient
          << ") + " << fLatSodlf * 0.999999 << "))] \n [LOD_RANGE = VECTOR(ECMIN(" << end_orient
          << "), ECMAX(" << end_orient << "))]\n\n";

    std::string end = sTmpLayerPrefix;
    end += ".end";

    fSvrf << end << " = DFM PROPERTY " << end_orient << " " << be_end_select << " " << lod_end_select
          << " OVERLAP ABUT ALSO MULTI \n [BE = VPROPERTY(" << be_end_select << ", \"BE\")]\n [LOD = VPROPERTY("
          << lod_end_select << ", \"LOD\")]\n [LOD_RANGE = VPROPERTY(" << lod_end_select << ", \"LOD_RANGE\")]";

    sValue.clear();

    sValue += "DFM PROPERTY ";
    sValue += sGateLayer;
    sValue += " ";
    sValue += side;
    sValue += " ";
    sValue += end;
    sValue += " OVERLAP ABUT ALSO \n [G2SP_P_1 = VPROPERTY(";
    sValue += side;
    sValue += ", PSE, 1)]\n [G2SP_P_2 = VPROPERTY(";
    sValue += side;
    sValue += ", PSE, 2)]\n [G2SP_E_1 = VECTOR()]\n [G2SP_E_1 = VECTOR()]\n [PER_1 = VPROPERTY(";
    sValue += side;
    sValue += ", BE, 1)]\n [PER_2 = VPROPERTY(";
    sValue += side;
    sValue += ", BE, 2)]\n [PAR_1 = VPROPERTY(";
    sValue += end;
    sValue += ", BE, 1)]\n [PAR_2 = VPROPERTY(";
    sValue += end;
    sValue += ", BE, 2)]\n [LPEPER_1 = VPROPERTY(";
    sValue += side;
    sValue += ", LOD, 1)]\n [LPEPER_2 = VPROPERTY(";
    sValue += side;
    sValue += ", LOD, 2)]\n [LPEPAR_1 = VPROPERTY(";
    sValue += end;
    sValue += ", LOD, 1)]\n [LPEPAR_1_RANGE = VPROPERTY(";
    sValue += end;
    sValue += ", LOD_RANGE, 1)]\n [LPEPAR_2 = VPROPERTY(";
    sValue += end;
    sValue += ", LOD, 2)]\n [LPEPAR_2_RANGE = VPROPERTY(";
    sValue += end;
    sValue += ", LOD_RANGE, 2)]\n\n";

    sValue += "\n";

    ++iLayer;

    return TCL_OK;
}

int
rcpTRSCompiler::strained_silicon_measurements_proc(ClientData d, Tcl_Interp *pInterp,
                                                   int argc, const char * argv[])
{
    std::string sMessage = "\nlvs : device::strained_silicon_measurements [options] \
options:\n\
 -gate value          Derived gate layer <>\n\
 -sd value            Derived source/drain layer <>\n\
 -active value        Drawn active area layer <>\n\
 -poly value          Drawn poly layer <>\n\
 -layer1 value        Derived mystery layer <>\n\
 -max_sp value        Gate to first poly spacing maximum search distance override [PSE] <-1.0>\n\
 -max_sp2 value       Gate to second poly spacing maximum search distance override [PSE] <-1.0>\n\
 -max_sd value        Gate to diffusion enclosure maximum search distance override [PSE] <-1.0>\n\
 -max_se value        Gate to layer1 enclosure maximum search distance override (sides) [BE] <-1.0>\n\
 -max_sf value        Gate to layer1 spacing maximum search distance override (sides) [BE] <-1.0>\n\
 -max_sode value      Gate to diffusion enclosure maximum search distance override (sides) [LOD] <-1.0>\n\
 -max_sodf value      Gate to diffusion spacing maximum search distance override (sides) [LOD] <-1.0>\n\
 -max_sle value       Gate to layer1 enclosure maximum search distance override (ends) [BE] <-1.0>\n\
 -max_slf value       Gate to layer1 spacing maximum search distance override (ends) [BE] <-1.0>\n\
 -max_sodlf value     Gate to diffusion spacing maximum search distance override (ends) [LOD] <-1.0>\n\
 -lat_sodlf value     Distance to search laterally from gate to nearest external diffusion (ends) [LOD] <-1.0>\n\
 -help                Print this message\n\
 -?                   Print this message\n\n";

    std::string sResult;
    if(TCL_ERROR == convertStrainedSiliconMeasurementsToDrcCmds(pInterp, argc, argv, sResult, m_fPvrs))
    {
        Tcl_Obj *pResult = Tcl_NewStringObj(sMessage.c_str(), sMessage.size());
        Tcl_SetObjResult(pInterp, pResult);

        return TCL_ERROR;
    }

    Tcl_Obj *pResult = Tcl_NewStringObj(sResult.c_str(), sResult.size());
    Tcl_SetObjResult(pInterp, pResult);

    return TCL_OK;
}

static inline int
convertContactResistanceMeasurementsToDrcCmds(Tcl_Interp *pInterp, int argc, const char * argv[],
                                              std::string &sValue, std::ofstream &fSvrf)
{
    std::string sMessage = "tvf : device::contact_resistance_measurements [options] \
options:\
 -gate value          Derived gate layer <>\
 -sd value            Derived source/drain layer <>\
 -contact value       Contact layer <>\
 -max_ct value        Gate to contact maximum search distance override <-1.0>\
 -help                Print this message\
 -?                   Print this message";

    static hvUInt32 iLayer = 1;
    static std::string sBreak = "__";
    std::string sGateLayer;
    std::string sSdLayer;
    std::string sContactLayer;
    std::string sMaxCt;

    for(int iPara = 1; iPara < argc; ++iPara)
    {
        if(strcmp(argv[iPara], "-gate") == 0)
        {
            if(!sGateLayer.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sGateLayer = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-sd") == 0)
        {
            if(!sSdLayer.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sSdLayer = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-contact") == 0)
        {
            if(!sContactLayer.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sContactLayer = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-max_ct") == 0)
        {
            if(!sMaxCt.empty())
                return TCL_ERROR;

            if(++iPara < argc)
            {
                sMaxCt = argv[iPara];
            }
            else
            {
                return TCL_ERROR;
            }
        }
        else if(strcmp(argv[iPara], "-help") == 0 || strcmp(argv[iPara], "-?") == 0)
        {
            return TCL_ERROR;
        }
    }

    if(sMaxCt.empty())
    {
        const char *pValue = Tcl_GetVar(pInterp, "device::max_ct", TCL_NAMESPACE_ONLY);
        if(pValue != NULL)
            sMaxCt = pValue;
        else
            return TCL_ERROR;
    }

    if(sMaxCt.empty() || sGateLayer.empty() || sSdLayer.empty() || sContactLayer.empty())
        return TCL_ERROR;

    double fMaxCt;
    if(TCL_ERROR == Tcl_GetDouble(pInterp, sMaxCt.c_str(), &fMaxCt))
        return TCL_ERROR;

    std::string sTmpLayerPrefix;

    sTmpLayerPrefix += sGateLayer;
    sTmpLayerPrefix += sBreak;
    sTmpLayerPrefix += sSdLayer;
    sTmpLayerPrefix += sBreak;
    sTmpLayerPrefix += sContactLayer;
    sTmpLayerPrefix += sBreak;
    sTmpLayerPrefix += "cr__TRS_tmp_";
    sTmpLayerPrefix += itoa(iLayer);

    std::string contact = sTmpLayerPrefix;
    contact += ".contact";

    fSvrf << contact << " = AND " << sContactLayer << " " << sSdLayer << std::endl;

    std::string side = sTmpLayerPrefix;
    side += ".side";

    fSvrf << side << " = COINCIDENT EDGE " << sGateLayer << " " << sSdLayer << "\n";

    std::string gate2sd = sTmpLayerPrefix;
    gate2sd += ".gate2sd";

    fSvrf << gate2sd << " = DFM SPACE " << side << " " << sSdLayer
          << " <= " << sMaxCt << " BY ENC BY LAYER " << sSdLayer << " COUNT == 0 MEASURE ALL\n";

    std::string gate2ct_all = sTmpLayerPrefix;
    gate2ct_all += ".gate2ct_all";

    fSvrf << gate2ct_all << " = DFM SPACE " << side << " " << contact
          << " <= " << sMaxCt << " BY EXT BY LAYER " << contact << " COUNT == 0 \n";

    std::string sd_edge = sTmpLayerPrefix;
    sd_edge += ".sd_edge";

    fSvrf << sd_edge << " = LENGTH " << sSdLayer << " > 0\n";

    std::string gate2ct_same_sd = sTmpLayerPrefix;
    gate2ct_same_sd += ".gate2ct_same_sd";

    fSvrf << gate2ct_same_sd << " = DFM PROPERTY " << gate2ct_all << " " << sd_edge
          << " OVERLAP ABUT ALSO REGION MULTI [ -= COUNT(" << sd_edge << ") ] < 3\n";

    std::string cont_edge = sTmpLayerPrefix;
    cont_edge += ".cont_edge";

    fSvrf << cont_edge << " = LENGTH " << sContactLayer << " > 0\n";

    std::string cont_edge_p = sTmpLayerPrefix;
    cont_edge_p += ".cont_edge_p";

    fSvrf << cont_edge_p << " = DFM PROPERTY " << cont_edge << " [ ct_length = (ECMAX("
          << cont_edge << ")-ECMIN(" << cont_edge << "))/2 ]\n";

    std::string gate2ct = sTmpLayerPrefix;
    gate2ct += ".gate2ct";

    fSvrf << gate2ct << " = DFM PROPERTY " << gate2ct_same_sd << " " << cont_edge_p
          << " OVERLAP ABUT ALSO [ ct_length = PROPERTY(" << cont_edge_p << ", ct_length) ]\n";

    std::string sdnet = sTmpLayerPrefix;
    sdnet += ".sdnet";

    fSvrf << sdnet << " = DFM PROPERTY " << sSdLayer
          << " [ NETID = NETID( " << sSdLayer <<  ") ]\n";

    std::string sd_p = sTmpLayerPrefix;
    sd_p += ".sd_p";

    fSvrf << sd_p << " = DFM PROPERTY " << sSdLayer << " " << sGateLayer
          << " OVERLAP ABUT ALSO MULTI [ share_factor = COUNT(" << sGateLayer << ") ]\n";

    std::string sideprp = sTmpLayerPrefix;
    sideprp += ".sideprp";

    fSvrf << sideprp << " = DFM PROPERTY " << side << " " << gate2ct << " "
          << gate2sd << " " << sdnet << " " << sd_p << " OVERLAP ABUT ALSO MULTI [ SF = PROPERTY("
          << sd_p << ", share_factor) ]\n [ NET = NETPROPERTY(" << sdnet
          << ", NETID, 1) ]\n [ CT_RANGE = CONCAT(VECTOR(ECMIN(" << gate2ct << "), ECMAX("
          << gate2ct << "), PROPERTY(" << gate2ct << ", ct_length))) ]\n "
          << "[ G2SD = SORT_MERGE_XXY(VECTOR(ECMIN(" << gate2sd << "), ECMAX("
          << gate2sd << "), EW(" << gate2sd << "))) ]\n";

    sValue.clear();

    sValue += "DFM PROPERTY ";
    sValue += sGateLayer;
    sValue += " ";
    sValue += sideprp;
    sValue += " OVERLAP ABUT ALSO MULTI [ SF_1 = PROPERTY(";
    sValue += sideprp;
    sValue += " , SF, 1) ]\n [ NET_1 = NETPROPERTY(";
    sValue += sideprp;
    sValue += ", NET, 1) ]\n [ G2SD_1 = VPROPERTY(";
    sValue += sideprp;
    sValue += ", G2SD, 1) ]\n [ CT_RANGE1 = VPROPERTY(";
    sValue += sideprp;
    sValue += ", CT_RANGE, 1) ]\n  [ SF_2 = PROPERTY(";
    sValue += sideprp;
    sValue += ", SF, 2) ]\n [ NET_2 = NETPROPERTY(";
    sValue += sideprp;
    sValue += ", NET, 2) ]\n [ G2SD_2 = VPROPERTY(";
    sValue += sideprp;
    sValue += ", G2SD, 2) ]\n [ CT_RANGE2 = VPROPERTY(";
    sValue += sideprp;
    sValue += ", CT_RANGE, 2) ]\n";
    sValue += "\n";

    ++iLayer;

    return TCL_OK;
}

int
rcpTRSCompiler::contact_resistance_measurements_proc(ClientData d, Tcl_Interp *pInterp,
                                                     int argc, const char * argv[])
{
    std::string sMessage = "tvf : device::contact_resistance_measurements [options] \n\
options:\n\
 -gate value          Derived gate layer <>\n\
 -sd value            Derived source/drain layer <>\n\
 -contact value       Contact layer <>\n\
 -max_ct value        Gate to contact maximum search distance override <-1.0>\n\
 -help                Print this message\n\
 -?                   Print this message\n";

    std::string sResult;
    if(TCL_ERROR == convertContactResistanceMeasurementsToDrcCmds(pInterp, argc, argv, sResult, m_fPvrs))
    {
        Tcl_Obj *pResult = Tcl_NewStringObj(sMessage.c_str(), sMessage.size());
        Tcl_SetObjResult(pInterp, pResult);

        return TCL_ERROR;
    }

    Tcl_Obj *pResult = Tcl_NewStringObj(sResult.c_str(), sResult.size());
    Tcl_SetObjResult(pInterp, pResult);

    return TCL_OK;
}


int
rcpTRSCompiler::void_proc(ClientData d, Tcl_Interp *pInterp,
                          int argc, const char * argv[])
{
    setTRSCompileError(pInterp, argc, argv);
    return TCL_OK;
}

int
rcpTRSCompiler::setlayer_proc(ClientData d, Tcl_Interp *pInterp,
                              int argc, const char * argv[])
{

    if(argc == 3)
    {
        if(strcmp(argv[1], "-getnames") == 0)
        {
            std::string res = "";
            if(strcmp( argv[2], "-global") == 0)
            {
                for(std::map<std::string, std::string, LTstr>::iterator iter = layermap_g.begin();
                    iter != layermap_g.end(); ++iter)
                {
                    res = res + iter->first + " ";
                }
                Tcl_SetResult(pInterp, const_cast<char*>(res.c_str()), TCL_VOLATILE);
            }
            else if(strcmp(argv[2], "-local") == 0)
            {

                for(std::map<std::string, std::string, LTstr>::iterator iter = layermap_g.begin();
                    iter != layermap_g.end(); ++iter)
                {
                    res = res + iter->first + " ";
                }

                for(std::map<std::string, std::string, LTstr>::iterator iter = layermap_l.begin();
                    iter != layermap_l.end(); ++iter)
                {
                    res = res + iter->first + " ";
                }

                Tcl_SetResult(pInterp, const_cast<char*>(res.c_str()), TCL_VOLATILE);
            }
            else
            {
                setTRSCompileError(pInterp, argc, argv);
                return TCL_ERROR;
            }
        }
        else if(strcmp( argv[2],"-getexpr" ) == 0)
        {
            std::map<std::string, std::string, LTstr>::iterator iter = layermap_g.find(argv[1]);
            if(iter != layermap_g.end())
            {
                sprintf(pInterp->result,"{%s}",iter->second.c_str());
            }
            else
            {
                iter = layermap_l.find(argv[1]);
                if(iter != layermap_l.end())
                {
                    sprintf(pInterp->result,"{%s}",iter->second.c_str());
                }
                else
                {
#ifdef DEBUG
                    printf("dump layer define\n");
                    for(iter = layermap_g.begin(); iter != layermap_g.end(); ++iter )
                        printf("%s = %s \n",iter->first.c_str(), iter->second.c_str());
#endif
                    setTRSCompileError(pInterp, argc, argv);
                }
            }
        }
        else
        {
            setTRSCompileError(pInterp, argc, argv);
            return TCL_ERROR;
        }
    }
    else
    {
        Utassert(argc > 3);
        if(strcmp( argv[2], "=") != 0)
        {
            setTRSCompileError(pInterp, argc, argv);
            return TCL_ERROR;
        }

        std::string aa = "";
        if(m_isInRulecheck)
            m_fPvrs << "\t";
        for(int i = 1; i < argc; ++i)
        {
            if(i != 1)
                m_fPvrs << " " ;
            m_fPvrs << argv[i];
            if(i > 2)
                aa += argv[i];
        }
        m_fPvrs << std::endl;
        std::string key = argv[1]; 
        std::string buf = aa;
        if(m_isInRulecheck)
            layermap_l[key] = buf;
        else
            layermap_g[key] = buf;
    }
    return TCL_OK;
}

int
rcpTRSCompiler::InitProc(Tcl_Interp *pInterp)
{
    
    int iRet = Tcl_Init(pInterp);
    if(iRet == TCL_ERROR)
    {
        return iRet;
    }

    
    const char* pTrsCmd =
        "namespace eval trs { \
        namespace export // RULE_COMMENT rule_comment COMMENT comment ECHO_TO_PVRS_FILE echo_to_pvrs_file OUTPUT_TO_PVRS output_to_pvrs GET_TRS_ARG get_trs_arg OUTPUTLAYER outputlayer ECHO_PVRS echo_pvrs; \
        namespace export RULE rule set_disclaimer set_rule_check_indentation set_traceback_line SETLAYER setlayer PVRS_BLOCK pvrs_block cleanup CLEANUP; \
        namespace export exists_global_variable EXISTS_GLOBAL_VARIABLE get_global_variable GET_GLOBAL_VARIABLE;\
        namespace export set_global_variable SET_GLOBAL_VARIABLE unset_global_variable UNSET_GLOBAL_VARIABLE;\
        namespace export ATTACH FLAG LVS SNAP CAPACITANCE FLATTEN MASK SOURCE CONNECT FRACTURE PEX TEXT INCLUDE; \
        namespace export COPY GROUP POLYGON TITLE DEVICE HCELL PORT TRACE DISCONNECT PRECISION TVF DRC DFM LABEL PUSH UNIT ERC; \
        namespace export LAYER RESISTANCE VARIABLE EXCLUDE LAYOUT RESOLUTION VIRTUAL EXPAND LITHO SCONNECT; \
        namespace export attach flag lvs snap capacitance flatten mask connect fracture pex text include; \
        namespace export copy group polygon title device hcell port disconnect precision tvf drc dfm label push erc; \
        namespace export layer resistance exclude layout resolution virtual expand litho sconnect; \
        namespace export is_log_blocked IS_LOG_BLOCKED unblock_log UNBLOCK_LOG block_log BLOCK_LOG output_call_stack OUTPUT_CALL_STACK; \
        }";

    iRet = Tcl_RecordAndEval(pInterp, const_cast<char*>(pTrsCmd), 0);
    if( iRet == TCL_ERROR )
    {
        return TCL_ERROR;
    }

    
    Tcl_CreateCommand(pInterp, "trs:://", (&rcpTRSCompiler::blockcomment_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::rule_comment", (&rcpTRSCompiler::rulecomment_proc),(ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::RULE_COMMENT", (&rcpTRSCompiler::rulecomment_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::comment", (&rcpTRSCompiler::linecomment_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::COMMENT", (&rcpTRSCompiler::linecomment_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::echo_to_pvrs_file", (&rcpTRSCompiler::echo_to_pvrs_file_proc),(ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ECHO_TO_PVRS_FILE", (&rcpTRSCompiler::echo_to_pvrs_file_proc),(ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::output_to_pvrs", (&rcpTRSCompiler::output_to_pvrs_proc),(ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::OUTPUT_TO_PVRS", (&rcpTRSCompiler::output_to_pvrs_proc),(ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::get_trs_arg", (&rcpTRSCompiler::gettrsarg_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GET_TRS_ARG", (&rcpTRSCompiler::gettrsarg_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::echo_pvrs", (&rcpTRSCompiler::void_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ECHO_PVRS", (&rcpTRSCompiler::void_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::outputlayer", (&rcpTRSCompiler::print_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::OUTPUTLAYER", (&rcpTRSCompiler::print_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::rule", (&rcpTRSCompiler::rulecheck_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::RULE", (&rcpTRSCompiler::rulecheck_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::set_header", (&rcpTRSCompiler::void_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::SET_HEADER", (&rcpTRSCompiler::void_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::set_rule_indent", (&rcpTRSCompiler::void_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::SET_RULE_INDENT", (&rcpTRSCompiler::void_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::set_trace_line", (&rcpTRSCompiler::void_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::SET_TRACE_LINE", (&rcpTRSCompiler::void_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::get_argus_version", (&rcpTRSCompiler::void_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GET_ARGUS_VERSION", (&rcpTRSCompiler::void_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::output_call_stack ", (&rcpTRSCompiler::void_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::OUTPUT_CALL_STACK", (&rcpTRSCompiler::void_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::cleanup ", (&rcpTRSCompiler::void_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::CLEANUP", (&rcpTRSCompiler::void_proc), (ClientData)0, 0);

    
    Tcl_CreateCommand(pInterp, "trs::exists_global_var", (&rcpTRSCompiler::exists_global_var_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::EXISTS_GLOBAL_VAR", (&rcpTRSCompiler::exists_global_var_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::get_global_var", (&rcpTRSCompiler::get_global_var_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GET_GLOBAL_VAR", (&rcpTRSCompiler::get_global_var_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::set_global_var", (&rcpTRSCompiler::set_global_var_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::SET_GLOBAL_VAR", (&rcpTRSCompiler::set_global_var_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::unset_global_var", (&rcpTRSCompiler::unset_global_var_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::UNSET_GLOBAL_VAR", (&rcpTRSCompiler::unset_global_var_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::is_log_blocked", (&rcpTRSCompiler::unset_global_var_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::IS_LOG_BLOCKED", (&rcpTRSCompiler::unset_global_var_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::unblock_log", (&rcpTRSCompiler::unset_global_var_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::UNBLOCK_LOG", (&rcpTRSCompiler::unset_global_var_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::block_log", (&rcpTRSCompiler::unset_global_var_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::BLOCK_LOG", (&rcpTRSCompiler::unset_global_var_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::setlayer", (&rcpTRSCompiler::setlayer_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::SETLAYER", (&rcpTRSCompiler::setlayer_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::pvrs_block", (&rcpTRSCompiler::pvrs_block_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::PVRS_BLOCK", (&rcpTRSCompiler::pvrs_block_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::attach", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ATTACH", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::attach_order", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ATTACH_ORDER", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::print_log", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::PRINT_LOG", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::uniconnect", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::UNICONNECT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::resolution", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::RESOLUTION", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::var", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::VAR", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layer", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYER", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layer_map", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYER_MAP", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::connect", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::CONNECT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::remove_connect", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::REMOVE_CONNECT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::precision", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::PRECISION", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::group_rule", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GROUP_RULE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::trs", (&rcpTRSCompiler::trs_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::TRS", (&rcpTRSCompiler::trs_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::include", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::INCLUDE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::add_polygon", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ADD_POLYGON", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::add_text", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ADD_TEXT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_add_polygon", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_ADD_POLYGON", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_add_text", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_ADD_TEXT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::label_level", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LABEL_LEVEL", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::label_layer", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LABEL_LAYER", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::copy_text", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::COPY_TEXT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::copy", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::COPY", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_push", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_PUSH", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::snap_offgrid", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::SNAP_OFFGRID", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_result", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_RESULT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_result", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_RESULT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_summary", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_SUMMARY", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_summary", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_SUMMARY", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::layout_input", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_INPUT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_input2", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_INPUT2", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_max_result", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_MAX_RESULT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_max_vertex", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_MAX_VERTEX", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_max_result", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_MAX_RESULT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_max_vertex", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_MAX_VERTEX", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_max_cell_name", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_MAX_CELL_NAME", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_max_nar_result", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_MAX_NAR_RESULT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_max_unattached_label", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_MAX_UNATTACHED_LABEL", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_output_cell_name", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_OUTPUT_CELL_NAME", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_process_cell_text", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_PROCESS_CELL_TEXT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::output_net", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::OUTPUT_NET", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_rule_map", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_RULE_MAP", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_output_rule_text", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_OUTPUT_RULE_TEXT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_output_rule_text", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_OUTPUT_RULE_TEXT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::incremental_connect", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::INCREMENTAL_CONNECT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::incremental_connect_warning", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::INCREMENTAL_CONNECT_WARNING", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_progressive_connect_warning", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_PROGRESSIVE_CONNECT_WARNING", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_output_reduce_false_error", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_OUTPUT_REDUCE_FALSE_ERROR", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_output_empty_rule", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_OUTPUT_EMPTY_RULE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_output_empty_rule", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_OUTPUT_EMPTY_RULE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_output_empty", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_OUTPUT_EMPTY", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_output_empty", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_OUTPUT_EMPTY", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_use_db_precision", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_USE_DB_PRECISION", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::gui_priority", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GUI_PRIORITY", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_magnify_result", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_MAGNIFY_RESULT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_ignore_rule", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_IGNORE_RULE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_select_rule", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_SELECT_RULE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_tolerance", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_TOLERANCE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_waive_error", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_WAIVE_ERROR", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::check_original_layer", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::CHECK_ORIGINAL_LAYER", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::dfm_ignore_rule", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DFM_IGNORE_RULE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::dfm_select_rule", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DFM_SELECT_RULE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_ignore_rule", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_IGNORE_RULE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_select_rule", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_SELECT_RULE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_find_open", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_FIND_OPEN", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_max_nets_per_open", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_MAX_NETS_PER_OPEN", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_max_opens", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_MAX_OPENS", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_find_open_nets", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_FIND_OPEN_NETS", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_find_open_ignore_nets", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_FIND_OPEN_IGNORE_NETS", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_find_shorts_ignore_nets", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_FIND_SHORTS_IGNORE_NETS", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_max_shorts", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_MAX_SHORTS", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_find_short", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_FIND_SHORT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::path_check", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::PATH_CHECK", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_path_check", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_PATH_CHECK", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_path_device", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_PATH_DEVICE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::layout_add_cell", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_ADD_CELL", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::flatten", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::FLATTEN", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::head_name", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::HEAD_NAME", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ignore_cell", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::IGNORE_CELL", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ignore_original_layer_check", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::IGNORE_ORIGINAL_LAYER_CHECK", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layer_dir", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYER_DIR", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_metal_layer", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_METAL_LAYER", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_device_cell", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_DEVICE_CELL", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_device_layer", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_DEVICE_LAYER", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_read_level", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_READ_LEVEL", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_read", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_READ", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::promote_cell", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::PROMOTE_CELL", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::promote_cell_text", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::PROMOTE_CELL_TEXT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_cell_group", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_CELL_GROUP", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_reserve_cell_group", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_RESERVE_CELL_GROUP", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_rename_cell", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_RENAME_CELL", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_rename_text", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_RENAME_TEXT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_remove_text", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_REMOVE_TEXT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_exception", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_EXCEPTION", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::precision_db", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::PRECISION_DB", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_select_window", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_SELECT_WINDOW", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_exclude_window", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_EXCLUDE_WINDOW", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_magnify", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_MAGNIFY", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::lvs_report", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_REPORT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_subcell_power_ground", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_SUBCELL_POWER_GROUND", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_check_top_cell_port", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_CHECK_TOP_CELL_PORT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_match_strict_subtype", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_MATCH_STRICT_SUBTYPE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_keep_floating_top_net", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_KEEP_FLOATING_TOP_NET", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_keep_pcell", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_KEEP_PCELL", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_precise_overlap", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_PRECISE_OVERLAP", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_ignore_extra_pin", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_IGNORE_EXTRA_PIN", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_match_accurate_subtype", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_MATCH_ACCURATE_SUBTYPE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_run_erc", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_RUN_ERC", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_global_net_as_port", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_GLOBAL_NET_AS_PORT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_inject_hierarchy", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_INJECT_HIERARCHY", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_output_device_promotion", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_OUTPUT_DEVICE_PROMOTION", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_ignore_substrate_pin", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_IGNORE_SUBSTRATE_PIN", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_zero_property_with_tol", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_ZERO_PROPERTY_WITH_TOL", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_ignore_device_model", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_IGNORE_DEVICE_MODEL", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_output_device_promotion_max", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_OUTPUT_DEVICE_PROMOTION_MAX", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::uniconnect_check_max_result", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::UNICONNECT_CHECK_MAX_RESULT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_power", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_POWER", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_ground", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_GROUND", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_db", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_DB", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_non_user_define_name", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_NON_USER_DEFINE_NAME", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_keep_black_box", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_KEEP_BLACK_BOX", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_output_netlist", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_OUTPUT_NETLIST", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_cell_group", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_CELL_GROUP", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_check_property_radius", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_CHECK_PROPERTY_RADIUS", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_black_box", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_BLACK_BOX", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_identify_gate", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_IDENTIFY_GATE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_corresponding_net", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_CORRESPONDING_NET", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_ignore_hcell", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_IGNORE_HCELL", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_expand_hcell", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_EXPAND_HCELL", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_layout_global_net_group", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_LAYOUT_GLOBAL_NET_GROUP", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_temp_dir", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_TEMP_DIR", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_ingore_port", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_IGNORE_PORT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::uniconnect_check", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::UNICONNECT_CHECK", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::check_property", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::CHECK_PROPERTY", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::schematic_input", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::SCHEMATIC_INPUT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::device", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DEVICE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::device_map", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DEVICE_MAP", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::device_push", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DEVICE_PUSH", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::push_device", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::PUSH_DEVICE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::device_merge", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DEVICE_MERGE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::device_merge_priority", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DEVICE_MERGE_PRIORITY", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::device_filter", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DEVICE_FILTER", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::device_filter_unused", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DEVICE_FILTER_UNUSED", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::device_filter_option", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DEVICE_FILTER_OPTION", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::device_pin_swap", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DEVICE_PIN_SWAP", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::device_property", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DEVICE_PROPERTY", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::device_type_map", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DEVICE_TYPE_MAP", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::hcell", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::HCELL", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::unit", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::UNIT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::quit", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::QUIT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::virtual_connect", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::VIRTUAL_CONNECT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::push_cell_lowest", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::PUSH_CELL_LOWEST", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::spice", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::SPICE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::netlist", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::NETLIST", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::attach(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ATTACH(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::attach_order(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ATTACH_ORDER(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::print_log(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::PRINT_LOG(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::uniconnect(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::UNICONNECT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::resolution(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::RESOLUTION(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::var(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::VAR(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layer(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYER(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layer_map(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYER_MAP(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::connect(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::CONNECT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::remove_connect(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::REMOVE_CONNECT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::precision(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::PRECISION(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::group_rule(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GROUP_RULE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::trs(", (&rcpTRSCompiler::trs_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::TRS(", (&rcpTRSCompiler::trs_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::include(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::INCLUDE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::add_polygon(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ADD_POLYGON(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::add_text(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ADD_TEXT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_add_polygon(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_ADD_POLYGON(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_add_text(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_ADD_TEXT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::label_level(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LABEL_LEVEL(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::label_layer(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LABEL_LAYER(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::copy_text(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::COPY_TEXT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::copy(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::COPY(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_push(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_PUSH(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::snap_offgrid(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::SNAP_OFFGRID(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_result(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_RESULT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_result(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_RESULT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_summary(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_SUMMARY(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_summary(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_SUMMARY(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::layout_input(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_INPUT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_input2(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_INPUT2(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_max_result(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_MAX_RESULT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_max_vertex(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_MAX_VERTEX(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_max_result(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_MAX_RESULT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_max_vertex(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_MAX_VERTEX(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_max_cell_name(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_MAX_CELL_NAME(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_max_nar_result(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_MAX_NAR_RESULT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_max_unattached_label(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_MAX_UNATTACHED_LABEL(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_output_cell_name(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_OUTPUT_CELL_NAME(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_process_cell_text(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_PROCESS_CELL_TEXT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::output_net(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::OUTPUT_NET(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_rule_map(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_RULE_MAP(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_output_rule_text(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_OUTPUT_RULE_TEXT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_output_rule_text(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_OUTPUT_RULE_TEXT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::incremental_connect(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::INCREMENTAL_CONNECT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::incremental_connect_warning(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::INCREMENTAL_CONNECT_WARNING(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_progressive_connect_warning(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_PROGRESSIVE_CONNECT_WARNING(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_output_reduce_false_error(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_OUTPUT_REDUCE_FALSE_ERROR(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_output_empty_rule(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_OUTPUT_EMPTY_RULE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_output_empty_rule(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_OUTPUT_EMPTY_RULE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_output_empty(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_OUTPUT_EMPTY(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_output_empty(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_OUTPUT_EMPTY(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_use_db_precision(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_USE_DB_PRECISION(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::gui_priority(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GUI_PRIORITY(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_magnify_result(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_MAGNIFY_RESULT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_ignore_rule(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_IGNORE_RULE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_select_rule(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_SELECT_RULE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_tolerance(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_TOLERANCE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::drc_waive_error(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DRC_WAIVE_ERROR(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::check_original_layer(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::CHECK_ORIGINAL_LAYER(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::dfm_ignore_rule(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DFM_IGNORE_RULE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::dfm_select_rule(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DFM_SELECT_RULE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_ignore_rule(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_IGNORE_RULE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_select_rule(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_SELECT_RULE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_find_open(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_FIND_OPEN(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_max_nets_per_open(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_MAX_NETS_PER_OPEN(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_max_opens(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_MAX_OPENS(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_find_open_nets(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_FIND_OPEN_NETS(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_find_open_ignore_nets(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_FIND_OPEN_IGNORE_NETS(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_find_shorts_ignore_nets(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_FIND_SHORTS_IGNORE_NETS(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_max_shorts(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_MAX_SHORTS(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_find_short(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_FIND_SHORT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::path_check(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::PATH_CHECK(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_path_check(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_PATH_CHECK(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::erc_path_device(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ERC_PATH_DEVICE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::layout_add_cell(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_ADD_CELL(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::flatten(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::FLATTEN(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::head_name(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::HEAD_NAME(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ignore_cell(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::IGNORE_CELL(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ignore_original_layer_check(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::IGNORE_ORIGINAL_LAYER_CHECK(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layer_dir(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYER_DIR(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_connect_layer(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_CONNECT_LAYER(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_device_cell(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_DEVICE_CELL(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_device_layer(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_DEVICE_LAYER(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_read_level(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_READ_LEVEL(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_read(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_READ(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::promote_cell(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::PROMOTE_CELL(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::promote_cell_text(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::PROMOTE_CELL_TEXT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_cell_group(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_CELL_GROUP(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_reserve_cell_group(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_RESERVE_CELL_GROUP(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_rename_cell(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_RENAME_CELL(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_rename_text(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_RENAME_TEXT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_remove_text(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_REMOVE_TEXT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_exception(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_EXCEPTION(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::precision_db(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::PRECISION_DB(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_select_window(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_SELECT_WINDOW(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_exclude_window(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_EXCLUDE_WINDOW(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::layout_magnify(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LAYOUT_MAGNIFY(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::lvs_report(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_REPORT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_subcell_power_ground(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_SUBCELL_POWER_GROUND(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_check_top_cell_port(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_CHECK_TOP_CELL_PORT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_match_strict_subtype(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_MATCH_STRICT_SUBTYPE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_keep_floating_top_net(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_KEEP_FLOATING_TOP_NET(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_keep_pcell(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_KEEP_PCELL(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_precise_overlap(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_PRECISE_OVERLAP(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_ignore_extra_pin(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_IGNORE_EXTRA_PIN(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_match_accurate_subtype(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_MATCH_ACCURATE_SUBTYPE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_run_erc(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_RUN_ERC(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_global_net_as_port(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_GLOBAL_NET_AS_PORT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_inject_hierarchy(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_INJECT_HIERARCHY(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_output_device_promotion(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_OUTPUT_DEVICE_PROMOTION(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_ignore_substrate_pin(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_IGNORE_SUBSTRATE_PIN(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_zero_property_with_tol(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_ZERO_PROPERTY_WITH_TOL(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_ignore_device_model(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_IGNORE_DEVICE_MODEL(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_output_device_promotion_max(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_OUTPUT_DEVICE_PROMOTION_MAX(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::uniconnect_check_max_result(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::UNICONNECT_CHECK_MAX_RESULT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_power(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_POWER(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_ground(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_GROUND(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_db(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_DB(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_non_user_define_name(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_NON_USER_DEFINE_NAME(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_keep_black_box(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_KEEP_BLACK_BOX(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_output_netlist(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_OUTPUT_NETLIST(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_cell_group(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_CELL_GROUP(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_check_property_radius(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_CHECK_PROPERTY_RADIUS(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_black_box(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_BLACK_BOX(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_identify_gate(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_IDENTIFY_GATE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_corresponding_net(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_CORRESPONDING_NET(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_ignore_hcell(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_IGNORE_HCELL(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_expand_hcell(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_EXPAND_HCELL(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_layout_global_net_group(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_LAYOUT_GLOBAL_NET_GROUP(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_temp_dir(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_TEMP_DIR(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_ingore_port(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_IGNORE_PORT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::uniconnect_check(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::UNICONNECT_CHECK(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::check_property(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::CHECK_PROPERTY(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::schematic_input(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::SCHEMATIC_INPUT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::device(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DEVICE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::device_map(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DEVICE_MAP(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::device_push(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DEVICE_PUSH(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::push_device(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::PUSH_DEVICE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::device_merge(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DEVICE_MERGE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::device_merge_priority(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DEVICE_MERGE_PRIORITY(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::device_filter(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DEVICE_FILTER(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::device_filter_unused(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DEVICE_FILTER_UNUSED(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::device_filter_option(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DEVICE_FILTER_OPTION(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::device_pin_swap(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DEVICE_PIN_SWAP(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::device_property(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DEVICE_PROPERTY(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::device_type_map(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DEVICE_TYPE_MAP(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::hcell(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::HCELL(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::unit(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::UNIT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::quit(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::QUIT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::virtual_connect(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::VIRTUAL_CONNECT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::push_cell_lowest(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::PUSH_CELL_LOWEST(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::spice(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::SPICE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::netlist(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::NETLIST(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_AND", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_and", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_AND(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_and(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_OR", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_or", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_OR(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_or(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_XOR", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_xor", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_XOR(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_xor(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_NOT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_not", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_NOT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_not(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::EDGE_OR", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::edge_or", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::EDGE_OR(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::edge_or(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_CUT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_cut", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_CUT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_cut(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_CUT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_cut", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_CUT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_cut(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_INSIDE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_inside", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_INSIDE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_inside(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_INSIDE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_inside", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_INSIDE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_inside(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_OUTSIDE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_outside", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_OUTSIDE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_outside(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_OUTSIDE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_outside", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_OUTSIDE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_outside(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_INTERACT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_interact", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_INTERACT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_interact(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_INTERACT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_interact", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_INTERACT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_interact(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_ADJACENT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_adjacent", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_ADJACENT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_adjacent(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_ADJACENT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_adjacent", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_ADJACENT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_adjacent(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_ENCLOSE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_enclose", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_ENCLOSE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_enclose(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_ENCLOSE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_enclose", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_ENCLOSE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_enclose(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_AREA", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_area", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_AREA(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_area(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_AREA", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_area", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_AREA(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_area(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_DONUT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_donut", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_DONUT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_donut(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_DONUT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_donut", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_DONUT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_donut(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_PERIMETER", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_perimeter", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_PERIMETER(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_perimeter(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_VERTEX", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_vertex", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_VERTEX(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_vertex(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_FLATTEN", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_flatten", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_FLATTEN(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_flatten(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_WITH_WIDTH", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_with_width", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_WITH_WIDTH(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_with_width(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_WITH_WIDTH", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_with_width", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_WITH_WIDTH(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_with_width(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_WITH_EDGE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_with_edge", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_WITH_EDGE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_with_edge(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_WITH_EDGE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_with_edge", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_WITH_EDGE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_with_edge(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::EDGE_ANGLE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::edge_angle", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::EDGE_ANGLE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::edge_angle(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~EDGE_ANGLE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~edge_angle", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~EDGE_ANGLE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~edge_angle(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::EDGE_LENGTH", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::edge_length", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::EDGE_LENGTH(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::edge_length(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~EDGE_LENGTH", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~edge_length", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~EDGE_LENGTH(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~edge_length(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::EDGE_PATH_LENGTH", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::edge_path_length", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::EDGE_PATH_LENGTH(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::edge_path_length(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::EDGE_ADJACENT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::edge_adjacent", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::EDGE_ADJACENT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::edge_adjacent(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~EDGE_ADJACENT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~edge_adjacent", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~EDGE_ADJACENT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~edge_adjacent(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::EDGE_INSIDE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::edge_inside", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::EDGE_INSIDE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::edge_inside(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~EDGE_INSIDE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~edge_inside", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~EDGE_INSIDE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~edge_inside(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::EDGE_OUTSIDE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::edge_outside", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::EDGE_OUTSIDE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::edge_outside(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~EDGE_OUTSIDE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~edge_outside", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~EDGE_OUTSIDE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~edge_outside(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::EDGE_COINCIDENT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::edge_coincident", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::EDGE_COINCIDENT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::edge_coincident(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~EDGE_COINCIDENT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~edge_coincident", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~EDGE_COINCIDENT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~edge_coincident(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::EDGE_CONVEX_POINT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::edge_convex_point", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::EDGE_CONVEX_POINT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::edge_convex_point(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_RECTANGLE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_rectangle", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_RECTANGLE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_rectangle(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_RECTANGLE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_rectangle", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_RECTANGLE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_rectangle(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_ORTHOGONALIZE_ANGLE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_orthogonalize_angle", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_ORTHOGONALIZE_ANGLE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_orthogonalize_angle(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_HOLES", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_holes", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_HOLES(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_holes(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_SIZE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_size", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_SIZE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_size(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::DFM_GEOM_SIZE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::dfm_geom_size", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DFM_GEOM_SIZE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::dfm_geom_size(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_INSIDE_CELL", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_inside_cell", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_INSIDE_CELL(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_inside_cell(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_INSIDE_CELL", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_inside_cell", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_INSIDE_CELL(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_inside_cell(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_NET", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_net", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_NET(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_net(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_NET", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_net", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_NET(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_net(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_ENCLOSE_RECTANGLE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_enclose_rectangle", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_ENCLOSE_RECTANGLE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_enclose_rectangle(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_ENCLOSE_RECTANGLE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_enclose_rectangle", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_ENCLOSE_RECTANGLE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_enclose_rectangle(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_GET_BOUNDARY", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_get_boundary", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_GET_BOUNDARY(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_get_boundary(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_GET_BOUNDARIES", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_get_boundaries", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_GET_BOUNDARIES(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_get_boundaries(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::SPACE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::space", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::SPACE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::space(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::WIDTH", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::width", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::WIDTH(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::width(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::OVERLAP", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::overlap", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::OVERLAP(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::overlap(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::EXTENSION", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::extension", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::EXTENSION(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::extension(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::CHECK_DENSITY", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::check_density", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::CHECK_DENSITY(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::check_density(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::CHECK_NAR", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::check_nar", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::CHECK_NAR(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::check_nar(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::CHECK_INCREMENTAL_NAR", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::check_incremental_nar", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::CHECK_INCREMENTAL_NAR(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::check_incremental_nar(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::CHECK_OFFGRID", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::check_offgrid", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::CHECK_OFFGRID(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::check_offgrid(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::DEVICE_SEED_LAYER", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::device_seed_layer", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DEVICE_SEED_LAYER(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::device_seed_layer(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::ENC_RECT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::enc_rect", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::ENC_RECT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::enc_rect(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_EDGE_TO_RECT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_edge_to_rect", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_EDGE_TO_RECT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_edge_to_rect(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_FILL_RECTANGLES", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_fill_rectangles", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_FILL_RECTANGLES(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_fill_rectangles(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_GET_CELL_BOUNDARY", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_get_cell_boundary", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_GET_CELL_BOUNDARY(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_get_cell_boundary(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_GET_LAYOUT_BOUNDARY", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_get_layout_boundary", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_GET_LAYOUT_BOUNDARY(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_get_layout_boundary(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_MAGNIFY", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_magnify", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_MAGNIFY(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_magnify(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_MERGE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_merge", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_MERGE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_merge(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_ROTATE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_rotate", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_ROTATE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_rotate(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_SHIFT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_shift", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_SHIFT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_shift(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_SNAP", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_snap", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_SNAP(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_snap(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_OR_NET", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_or_net", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_OR_NET(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_or_net(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_ORTH_SIZE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_orth_size", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_ORTH_SIZE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_orth_size(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_TEXT_TO_RECT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_text_to_rect", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_TEXT_TO_RECT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_text_to_rect(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_TRANSFER_NETID", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_transfer_netid", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_TRANSFER_NETID(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_transfer_netid(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_WITH_ADJACENT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_with_adjacent", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_WITH_ADJACENT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_with_adjacent(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_WITH_ADJACENT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_with_adjacent", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_WITH_ADJACENT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_with_adjacent(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::GEOM_WITH_LABEL", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_with_label", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::GEOM_WITH_LABEL(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::geom_with_label(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_WITH_LABEL", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_with_label", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~GEOM_WITH_LABEL(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::~geom_with_label(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::TABLE_DRC", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::table_drc", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::TABLE_DRC(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::table_drc(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::DFM_TEXT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::dfm_text", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DFM_TEXT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::dfm_text(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::DFM_CREATE_NAR", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::dfm_create_nar", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DFM_CREATE_NAR(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::dfm_create_nar(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::DFM_MERGE_PROPERTY", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::dfm_merge_property", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DFM_MERGE_PROPERTY(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::dfm_merge_property(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::DFM_BUILD_PROPERTY", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::dfm_build_property", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DFM_BUILD_PROPERTY(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::dfm_build_property(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::DFM_RESULT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::dfm_result", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DFM_RESULT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::dfm_result(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::DFM_COPY", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::dfm_copy", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DFM_COPY(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::dfm_copy(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::DFM_CHECK_SPACE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::dfm_check_space", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DFM_CHECK_SPACE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::dfm_check_space(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::DFM_TRANSFER_NETID", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::dfm_transfer_netid", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DFM_TRANSFER_NETID(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::dfm_transfer_netid(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::OPC_BIAS", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::opc_bias", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::OPC_BIAS(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::opc_bias(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::CASE_SENSITIVE(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::CASE_SENSITIVE", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::case_sensitive(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::case_sensitive", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::LVS_IGNORE_PORT(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::LVS_IGNORE_PORT", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_ignore_port(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::lvs_ignore_port", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "trs::DEVICE_GROUP(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::DEVICE_GROUP", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::device_group(", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "trs::device_group", (&rcpTRSCompiler::pvrscmd_proc), (ClientData)0, 0);

#if 0
    const char* deviceCmd =
        "namespace eval device { \
        namespace export enclosure_measurements ENCLOSURE_MEASUREMENTS strained_silicon_measurements STRAINED_SILICON_MEASUREMENTS contact_resistance_measurements CONTACT_RESISTANCE_MEASUREMENTS; \
        }";
    iRet = Tcl_RecordAndEval(pInterp, const_cast<char*>(deviceCmd), 0);
    if(iRet == TCL_ERROR)
    {
        return TCL_ERROR;
    }
#endif

    Tcl_CreateCommand(pInterp, "device::enclosure_measurements", (&rcpTRSCompiler::enclosure_measurements_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "device::ENCLOSURE_MEASUREMENTS", (&rcpTRSCompiler::enclosure_measurements_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "device::strained_silicon_measurements", (&rcpTRSCompiler::strained_silicon_measurements_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "device::STRAINED_SILICON_MEASUREMENTS", (&rcpTRSCompiler::strained_silicon_measurements_proc), (ClientData)0, 0);

    Tcl_CreateCommand(pInterp, "device::contact_resistance_measurements", (&rcpTRSCompiler::contact_resistance_measurements_proc), (ClientData)0, 0);
    Tcl_CreateCommand(pInterp, "device::CONTACT_RESISTANCE_MEASUREMENTS", (&rcpTRSCompiler::contact_resistance_measurements_proc), (ClientData)0, 0);

    return TCL_OK;
}

















































void
rcpTRSCompiler::sourceCmdTracrProc(ClientData clientData, Tcl_Interp *pInterp,
                                   int level, char *command, Tcl_CmdProc *proc,
                                   ClientData cmdClientData,
                                   int argc, CONST84 char *argv[])
{
    if((0 == strcmp(argv[0], "source") || 0 == strcmp(argv[0], "include") ||
       0 == strcmp(argv[0], "INCLUDE") || 0 == strcmp(argv[0], "trs::INCLUDE") ||
       0 == strcmp(argv[0], "trs::include")) && argc >= 2)
    {
        std::string sFileName = argv[1];
        if(m_pvSourceFiles != NULL)
            m_pvSourceFiles->push_back(sFileName);
#ifdef DEBUG
        std::cout << "tcl source file debug info: " << sFileName << std::endl;
#endif
    }
}























































int
rcpTRSCompiler::trs_compiler()
{
    if(access(m_pTrsFilename.c_str(), R_OK) != 0)
    {
        printf("no file %s exist or has no permission to read\n!", m_pTrsFilename);
        return TCL_ERROR;
    }

#if 0 

    std::ifstream reader(m_pTrsFilename);
    if(!reader)
        return TCL_ERROR;

    std::ofstream outer(".mergetrs.cmd");
    std::string sLineBuf;
    while(std::getline(reader, sLineBuf))
    {
        std::string sValue = sLineBuf;
        trim(sLineBuf);
        
        if(sLineBuf.size() >= 23 && isTclPackageLine(sLineBuf) && std::string::npos != sLineBuf.find("cmdline"))
        {
        }
        else if(isTclPackageLine(sLineBuf))
        {
            sLineBuf.insert(0, "#");
        }
        outer << sLineBuf << std::endl;
    }
    outer.close();
#endif

    m_fPvrs.open(m_pPvrsFileName);
    if(m_fPvrs.fail())
    {
        printf("can't open %s to write\n", m_pPvrsFileName);
        return TCL_ERROR;
    }

    int iRet = Tcl_EvalFile(m_pInterp, m_pTrsFilename.c_str());
    if(iRet == TCL_ERROR)
    {
        printf("\nERROR occurs at line %d: %s\n", m_pInterp->errorLine,
               Tcl_GetString(Tcl_GetObjResult(m_pInterp)));
        return TCL_ERROR;
    }
    

    return TCL_OK;
}











































































































































































rcpTRSCompiler::~rcpTRSCompiler()
{
    if(m_pInterp != NULL)
        Tcl_DeleteInterp(m_pInterp);
    m_pInterp = NULL;
    m_pvSourceFiles = NULL;
    m_fPvrs.close();
}

rcpTRSCompiler::rcpTRSCompiler(const std::string &pInputFile, const std::string &pOutPvrsFile,
                               const std::string &pTrsArg)
{
	m_pTrsFilename  = pInputFile;
	m_pPvrsFileName = pOutPvrsFile;
	m_pTrsArg       = pTrsArg;

    if( m_pPvrsFileName.empty() )
    {
        m_pPvrsFileName = ".trs2pvrs.tmp";
    }

    m_pInterp = g_warpper_CreateInterp();
    int iret = InitProc( m_pInterp );
    if(iret == TCL_ERROR)
    {
        printf("Error to init TVF-Preprocessor\n");
        return;
    }

    return ;
}

const char*
rcpTRSCompiler::getPvrsFileName()
{
    return m_pPvrsFileName.c_str();
}

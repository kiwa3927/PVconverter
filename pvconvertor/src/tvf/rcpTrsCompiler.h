

#ifndef RCPTRSCOMPILER_H_
#define RCPTRSCOMPILER_H_

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <string.h>

#include "tcl.h"
#include "public/rcsTypes.h"

class rcpTRSCompiler
{
    struct LTstr
    {
        bool operator()(const std::string &s1, const std::string &s2) const
        {
            return strcmp(s1.c_str(), s2.c_str()) < 0;
        }
    };

public:
    rcpTRSCompiler(const std::string& pInputFile, const std::string& pOutPvrsFile = "", const std::string& pTrsArg = "");


    ~rcpTRSCompiler();

    int   execute();
    const char* getPvrsFileName();

    static int trs_compiler();






private:
    
    static int InitProc(Tcl_Interp *pInterp);

    static int setlayer_proc(ClientData d, Tcl_Interp *pInterp,
                             int argc, const char * argv[]);

    static int void_proc(ClientData d, Tcl_Interp *pInterp,
                         int argc, const char * argv[]);

    static int rulecheck_proc(ClientData d, Tcl_Interp *pInterp,
                              int argc, const char * argv[]);

    static int get_global_var_proc(ClientData d, Tcl_Interp *pInterp,
                                        int argc, const char * argv[]);

    static int set_global_var_proc(ClientData d, Tcl_Interp *pInterp,
                                        int argc, const char * argv[]);

    static int unset_global_var_proc(ClientData d, Tcl_Interp *pInterp,
                                          int argc, const char * argv[]);

    static int exists_global_var_proc(ClientData d, Tcl_Interp *pInterp,
                                      int argc, const char * argv[]);

    static int gettrsarg_proc(ClientData d, Tcl_Interp *pInterp,
                              int argc, const char * argv[]);

    static int trs_proc(ClientData d, Tcl_Interp *pInterp,
                        int argc, const char * argv[]);

    static int pvrs_block_proc(ClientData d, Tcl_Interp *pInterp,
                               int argc, const char * argv[]);

    static int pvrscmd_proc(ClientData d, Tcl_Interp *pInterp,
                            int argc, const char * argv[]);

    static int print_proc(ClientData d, Tcl_Interp *pInterp,
                          int argc, const char * argv[]);

    static int echo_to_pvrs_file_proc(ClientData d, Tcl_Interp *pInterp,
                                      int argc, const char * argv[]);

    static int output_to_pvrs_proc(ClientData d, Tcl_Interp *pInterp,
                                      int argc, const char * argv[]);

    static int linecomment_proc(ClientData d, Tcl_Interp *pInterp,
                                int argc, const char * argv[]);

    static int rulecomment_proc(ClientData d, Tcl_Interp *pInterp,
                                int argc, const char * argv[]);

    static int blockcomment_proc(ClientData d, Tcl_Interp *pInterp,
                                 int argc, const char * argv[]);

    static int enclosure_measurements_proc(ClientData d, Tcl_Interp *pInterp,
                                          int argc, const char * argv[]);

    static int strained_silicon_measurements_proc(ClientData d, Tcl_Interp *pInterp,
                                                  int argc, const char * argv[]);

    static int contact_resistance_measurements_proc(ClientData d, Tcl_Interp *pInterp,
                                                    int argc, const char * argv[]);

    static void freeResult(char *pValue);

private:
    static void sourceCmdTracrProc(ClientData clientData, Tcl_Interp *pInterp,
                                   int level, char *command, Tcl_CmdProc *proc,
                                   ClientData cmdClientData,
                                   int argc, CONST84 char *argv[]);

private:
    static bool          m_isInRulecheck;
    static std::ofstream m_fPvrs;
    static std::string   m_pTrsArg;
    static std::string   m_pTrsFilename;
    static std::string   m_pPvrsFileName;
    static Tcl_Interp*   m_pInterp;

    static std::vector<std::string> *m_pvSourceFiles;

    static std::map<std::string, std::string, LTstr> layermap_g;
    static std::map<std::string, std::string, LTstr> layermap_l;
    static std::map<std::string, std::string, LTstr> varmap_g;
};

#endif 

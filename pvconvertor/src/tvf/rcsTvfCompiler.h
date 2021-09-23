







#ifndef _H_RCS_TVFCOMPILER_H_
#define _H_RCS_TVFCOMPILER_H_ 

#include <map>
#include <fstream>
#include <string.h>

#include "tcl.h"
#include "public/rcsTypes.h"

class rcsTVFCompiler_T
{
    struct LTstr
    {
        bool operator()(const std::string &s1, const std::string &s2) const
        {
            return strcmp(s1.c_str(), s2.c_str()) < 0;
        }
    };

public:
    rcsTVFCompiler_T(const std::string& pInputFile, const std::string& pOutSvrfFile = "", const std::string& ptvfarg = "");
    ~rcsTVFCompiler_T();

    int   tvf_compiler();
    static const char* getsvrffilename();

private:
    
    static int InitProc(Tcl_Interp *interp);

    static int setlayer_proc(ClientData d, Tcl_Interp *interp,
                             int argc, const char * argv[]);

    static int void_proc(ClientData d, Tcl_Interp *interp,
                         int argc, const char * argv[]);

    static int rulecheck_proc(ClientData d, Tcl_Interp *interp,
                              int argc, const char * argv[]);

    static int gettvfarg_proc(ClientData d, Tcl_Interp *interp,
                              int argc, const char * argv[]);

    static int tvf_proc(ClientData d, Tcl_Interp *interp,
                        int argc, const char * argv[]);

    static int verbatim_proc(ClientData d, Tcl_Interp *interp,
                             int argc, const char * argv[]);

    static int svrfcmd_proc(ClientData d, Tcl_Interp *interp,
                            int argc, const char * argv[]);

    static int print_proc(ClientData d, Tcl_Interp *interp,
                          int argc, const char * argv[]);

    static int echo_to_svrf_file_proc(ClientData d, Tcl_Interp *interp,
                                      int argc, const char * argv[]);

    static int output_to_svrf_proc(ClientData d, Tcl_Interp *interp,
                                      int argc, const char * argv[]);

    static int linecomment_proc(ClientData d, Tcl_Interp *interp,
                                int argc, const char * argv[]);

    static int blockcomment_proc(ClientData d, Tcl_Interp *interp,
                                 int argc, const char * argv[]);

    static int get_global_var_proc(ClientData d, Tcl_Interp *pInterp,
                                        int argc, const char * argv[]);

    static int set_global_var_proc(ClientData d, Tcl_Interp *pInterp,
                                        int argc, const char * argv[]);

    static int unset_global_var_proc(ClientData d, Tcl_Interp *pInterp,
                                          int argc, const char * argv[]);

    static int exists_global_var_proc(ClientData d, Tcl_Interp *pInterp,
                                      int argc, const char * argv[]);

    static int enclosure_measurements_proc(ClientData d, Tcl_Interp *interp,
                                          int argc, const char * argv[]);

    static int strained_silicon_measurements_proc(ClientData d, Tcl_Interp *interp,
                                                  int argc, const char * argv[]);

    static int contact_resistance_measurements_proc(ClientData d, Tcl_Interp *interp,
                                                    int argc, const char * argv[]);

    static int get_tmp_layer_name_proc(ClientData d, Tcl_Interp *interp,
                                       int argc, const char * argv[]);

    static int dfm_vec_measurements_proc(ClientData d, Tcl_Interp *interp,
                                              int argc, const char * argv[]);

    static int scaled_dfm_vec_measurements_proc(ClientData d, Tcl_Interp *interp,
                                              int argc, const char * argv[]);

    static int set_search_distance_proc(ClientData d, Tcl_Interp *interp,
                                        int argc, const char * argv[]);

    static int build_drc_layer_debug_proc(ClientData d, Tcl_Interp *interp,
                                          int argc, const char * argv[]);

    static int build_dfm_layer_debug_proc(ClientData d, Tcl_Interp *interp,
                                          int argc, const char * argv[]);

    static int build_nlayer_concat_vectors_string_proc(ClientData d, Tcl_Interp *interp,
                                                       int argc, const char * argv[]);

    static int device_n20_measurements_proc(ClientData d, Tcl_Interp *interp,
                                                       int argc, const char * argv[]);
    static int multi_layer_measurements_proc(ClientData d, Tcl_Interp *interp,
                                             int argc, const char * argv[]);
    static int cpo_measurements_proc(ClientData d, Tcl_Interp *interp,
                                             int argc, const char * argv[]);

    static void freeResult(char *pValue);

private:
    static bool          m_isInRulecheck;
    static std::ofstream m_fSvrf;
    static std::string   m_pTvfArg;
    static std::string   m_pTvfFilename;
    static std::string   m_pSvrfFileName;
    static Tcl_Interp*   m_pInterp;

    static std::map<std::string, std::string, LTstr> layermap_g;
    static std::map<std::string, std::string, LTstr> layermap_l;
    static std::map<std::string, std::string, LTstr> varmap_g;
};

#endif

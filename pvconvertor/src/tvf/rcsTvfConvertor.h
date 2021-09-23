

#ifndef RCSTVFCONVERTOR_H_
#define RCSTVFCONVERTOR_H_

#include <string>
#include <vector>

struct Tcl_Parse;
struct Tcl_Interp;
struct Tcl_Token;

class rcsTvfConvertor
{
public:
    rcsTvfConvertor(const std::string &pFileName);
    virtual ~rcsTvfConvertor();

    bool execute(const char* pScript = NULL);

private:
    bool parseScript(Tcl_Interp *interp, const char *script, const size_t size);
    bool parseTclCommand(Tcl_Parse &parse);
    bool parseTvfCommand(Tcl_Parse &parse);
    bool parseTvfComment(Tcl_Parse &parse);
    bool parseTvfRuleCheck(Tcl_Parse &parse);
    bool parseTvfRuleCheckComment(Tcl_Parse &parse);
    bool parseTvfSvrfCommand(Tcl_Parse &parse);
    bool parseTvfVerbatimCommand(Tcl_Parse &parse);
    bool parseTvfSetLayerCommand(Tcl_Parse &parse);
    bool parseTvfDeleteLayerCommand(Tcl_Parse &parse);
    bool parseTvfOutLayerCommand(Tcl_Parse &parse);
    bool parseTvfSvrfVarCommand(Tcl_Parse &parse);
    bool parseTvfSystemVarCommand(Tcl_Parse &parse);
    bool parseTvfBlockTranscriptCommand(Tcl_Parse &parse);
    bool parseTvfUnblockTranscriptCommand(Tcl_Parse &parse);
    bool parseTvfIsTranscriptBlockedCommand(Tcl_Parse &parse);
    bool parseTvfEchoSvrfCommand(Tcl_Parse &parse);
    bool parseTvfEchoToSvrfFileCommand(Tcl_Parse &parse);
    bool parseTvfOutputToSvrfCommand(Tcl_Parse &parse);
    bool parseTvfEpilogCommand(Tcl_Parse &parse);
    bool parseTvfSetGlobalVariableCommand(Tcl_Parse &parse);
    bool parseTvfGetGlobalVariableCommand(Tcl_Parse &parse);
    bool parseTvfUnsetGlobalVariableCommand(Tcl_Parse &parse);
    bool parseTvfExistsGlobalVariableCommand(Tcl_Parse &parse);
    bool parseTvfGetCalibreVersionCommand(Tcl_Parse &parse);
    bool parseTvfGetTvfArgCommand(Tcl_Parse &parse);
    bool parseTvfGetLayerArgsCommand(Tcl_Parse &parse);
    bool parseTvfGetLayerExtentCommand(Tcl_Parse &parse);
    bool parseTvfGetLayerNameCommand(Tcl_Parse &parse);
    bool parseTvfIsLayerEmptyCommand(Tcl_Parse &parse);
    bool parseTvfPrintCallStackCommand(Tcl_Parse &parse);
    bool parseTvfSetDisclaimerCommand(Tcl_Parse &parse);
    bool parseTvfSetRuleCheckIndentationCommand(Tcl_Parse &parse);
    bool parseTvfSetTracebackLineCommand(Tcl_Parse &parse);
    bool parseTvfIncludeFileCommand(Tcl_Parse &parse);
    bool parseTvfIncludeFileCommandRespectively(Tcl_Parse &parse);
    bool parseDeviceCommand(std::string sInFile, std::string sOutFile, std::string& result);
    bool convertSvrfToPvrs(std::string sInFile, std::string sOutFile);
    //add erctcl func
    bool parseErcSetupParallelDeviceParamsCommand(Tcl_Parse &parse);
    bool parseErcExecuteParallelDeviceParamsCommand(Tcl_Parse &parse);

private:
    static bool hasTvfKeyword(const char *p, const char *end);
    static bool isTvfKeyword(const char *p, const char *end);
    bool parseCommandArgv(Tcl_Parse &parse, std::string &script);
    bool parseTvfCommandArgv(Tcl_Parse &parse, std::string &script,
                             const bool needParseArgv0 = false);
    bool parseStringArgv(Tcl_Parse &parse, Tcl_Token *tokenPtr,
                                std::string &script);
    bool parseSetLayerDefinition(Tcl_Parse &parse);
    bool parseIncludeSvrfFile(std::string sInFile, const std::string &outFile, std::ostream &out);
    bool parseSourceTvfFile(std::string sInFile, const std::string &outFile, std::ostream &out);
    bool isTVFRule(const char* pFilename);
    /* params
     * return: result string
     * std::string strSrc: source input string
     * std::string strSub: the original substring to be replaced
     * std::string strSpec: the specified substring to replace origin
     */
    std::string outputSpecifiedSubString(const std::string& strSrc, const std::string& strSub, const std::string& strSpec);

private:
    std::string   m_sFileName;
    std::ostream *m_oPVRSStream;
    std::ostream &m_oLogStream;
    Tcl_Interp   *m_pInterp;
    const char   *m_pScript;

private:
    static bool                     m_shasImportInclude;
    static std::vector<std::string> m_svTvfKeyword;
    static bool 					m_bOutputAllNamespace;
};

#endif 

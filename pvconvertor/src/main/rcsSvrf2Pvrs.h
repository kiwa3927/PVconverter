#ifndef RCSSVRF2PVRS_H_
#define RCSSVRF2PVRS_H_

#include <string>
#include "public/token/rcsToken.h"

class rcsPreProcessor_T;

class rcsSvrf2Pvrs
{
public:
    rcsSvrf2Pvrs() : m_lastGlobalLine(0) {}
    void setLastGlobalLine(hvUInt32 n) { m_lastGlobalLine = n; }
    hvUInt32 getLastGlobalLine() const { return m_lastGlobalLine; }
    void transGlobalLineForIncFile(std::list<rcsToken_T> &tokens) const
    {
        if (getLastGlobalLine() != 0)
        {
            for (auto &t : tokens)
                t.nLineNo += this->getLastGlobalLine();
        }
    }
    ~rcsSvrf2Pvrs();
    static bool isTVFRule(const char* pFilename, bool* isTVF = NULL, bool* isTRS = NULL);

    bool execute(std::string sInFile, const std::string &sSwitchFile,
                        const std::string &sOutFile,
                        bool outSvrf, bool convertSwitch, bool flattenMacro,
                        bool outComment, std::ostream &out,
                        bool isNewPvrs = false, bool needExpandTmpLayer = true,
                        bool convertIncludeFile = false, bool trsFlag = false,
                        const std::string &sTmpLayerSuffix = "");
    static bool executeIncludeFile(std::string sInFile, const std::string &sSwitchFile,
                        const std::string &sOutFile, std::ostream &out);
    static bool executeTvfPreprocess(std::string sInFile, const std::string &sOutFile, std::ostream &out);

    static bool executeForTvfCommand(std::string &script);
    static void processIncludeFileRespectively(rcsPreProcessor_T &preProcessor);

    
    static hvUInt32 increaseCurLineNo(hvUInt32 step = 1);
    static hvUInt32 decreaseCurLineNo();
    static hvUInt32 setCurLineNo(hvUInt32 lineNo);
    static hvUInt32 getCurLineNo();
    static void setCompleteStr(std::string& str);
    static void setStrSearchBegin(hvUInt32 pos = 0);
    static hvUInt32 findLineNumInTvfFun(std::string command);
private:
    static hvUInt32 mCurLineNo;
    static hvUInt32 mStrSearchBegin;
    static std::string mCompleteStr;

    hvUInt32 m_lastGlobalLine; // only for -oi option
};

#ifdef DEBUG
#include "public/token/rcsToken.h"
#include <list>
void printTokenStream(std::list<rcsToken_T>::iterator first, std::list<rcsToken_T>::iterator last);
#endif

#endif 

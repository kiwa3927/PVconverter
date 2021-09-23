



#ifndef PREPROCESSOR_H_
#define PREPROCESSOR_H_

#include <string>
#include <vector>
#include <map>
#include <set>

#include "public/rcsTypes.h"

enum CONDITIONALS_TYPE
{
    IFDEF,
    ELSE,
    ENDIF,
    IFNDEF,
    DEFINE,
    REDEFINE,
    UNDEFINE,
    REUNDEFINE,
    INCL,
    DECRYPT,
    ENDCRYPT,
    COND_PRAGMA,

    INVALID,
};

class AutoFileRemover
{
public:
    AutoFileRemover() {}
    explicit AutoFileRemover(const std::string &f) :m_file(f) {}
    void setFile(const std::string &f)
    {
        Utassert(m_file.empty());
        m_file = f;
    }
    ~AutoFileRemover()
    {
        if (!m_file.empty())
            unlink(m_file.c_str());
    }
private:
    std::string m_file;
    AutoFileRemover(const AutoFileRemover &);
    AutoFileRemover& operator=(const AutoFileRemover &);
};

class rcsPreProcessor_T
{
public:
    class GUI_Directive_Warpper
    {
    public:
        GUI_Directive_Warpper()
            :m_eType(INVALID)
        {}
        bool isNone() const { return m_eType == INVALID; }
        bool isRedef() const  { return m_eType == REDEFINE; }
        bool isReundef() const  { return m_eType == REUNDEFINE; }

        void setDirective(CONDITIONALS_TYPE type)
        {
            Utassert(type == REDEFINE || type == REUNDEFINE);
            m_eType = type;
        }
    private:
        CONDITIONALS_TYPE m_eType;
    };
public:
    rcsPreProcessor_T(const char *pFileName, bool convertSwitch, std::ofstream &fOutStream);

    typedef std::vector<std::pair<CONDITIONALS_TYPE, bool> > CONDITIONALS_STACK;
    struct NAME_VALUE
    {
        NAME_VALUE(): isDefined(false)
        {}
#if 0
        NAME_VALUE(bool b, bool r, const std::string &s):
            isDefined(b), isRedefined(r), sValue(s)
        {}
#endif

        bool isDefined;
        GUI_Directive_Warpper guiPro;
        std::string sValue;
    };

    typedef std::map<std::string, NAME_VALUE> DEFINITION_MAP;

    void initial(hvUInt32 nGlobalLine = 0);

    bool execute();
    static hvUInt32 lastGlobalLine() { return m_sgnGlobalLineNo; }

    hvUInt32 getIncludeFileVectorSize();
    std::string getIncludeFile(hvUInt32 index);
    void pushIncludeFileName(std::string includeFileName);

private:
    void processEnv(std::string &str);
    void processInclude(const std::string &sLineBuf);
    void processDefine(const std::string &sLineBuf);
    void processRedefine(const std::string &sLineBuf);
    void processUndefine(const std::string &sLineBuf);
    void processReundefine(const std::string &sLineBuf);
    void processIfdef(const std::string &sLineBuf);
    void processIfndef(const std::string &sLineBuf);
    void processElse(const std::string &sLineBuf);
    void processEndif(const std::string &sLineBuf);
    void processPragma(const std::string &sLineBuf);

    void process_GetLayerArgs(std::string sLineBuf);
    void process_SetLayer(std::string &sLineBuf);

private:
    bool                  m_convertSwitch;
    const char           *m_pFileName;
    std::ofstream        &m_fOutStream;
    std::vector<std::string> 		m_vIncludeFile;

    std::set<std::string>   m_sLayerArgs;

    static std::set<std::string>    m_sgsetFileCollection;
    static hvUInt32                 m_sgnGlobalLineNo;
    static DEFINITION_MAP           m_sgmapNameDefinition;
    static CONDITIONALS_STACK       m_sgvConditionalsStack;
    static bool                     m_sgisEnabled;

private:
    
    static void insert_def(std::string key, const std::string &val);
    static void insert_def_force(std::string key, const std::string &val);
    static void insert_undef(std::string key);
    static void insert_undef_force(std::string key);
    static void insert_redef(std::string key, const std::string &val);
    static void insert_reundef(std::string key);

    static bool has_def(std::string key);
    static bool is_def(std::string key);
    static bool is_def_val(std::string key, const std::string &val);
};

#endif 

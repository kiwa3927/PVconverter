



#ifndef RCSMANAGER_H_
#define RCSMANAGER_H_

#include <map>
#include <string>
#include <vector>
#include "rcspragmamanager.h"
#include "public/manager/rcsLithoFileSpec.h"
class rcsSynLayerDefinitionNode_T;
class rcsManager_T
{
public:
    static rcsManager_T *getInstance();
    static void deleteInstance();

    rcsPragmaManager &getPragmaManager() { return m_pragmaManager; }

    void setOutputSvrfComment(bool isOut);
    bool isSvrfComment() const;

    void setOutputComment(bool isOut);
    bool outputComment() const;

    void setFirstTime(bool isFirst);
    bool isFirstTime() const;

    void setTvf2Trs(bool isTvf);
    bool isTvf2Trs() const;

    void setTrsFlag(bool isTvf);
    bool isTrsFlag() const;

    void setCallFromSetLayerCommand(bool isSetLayer);
    bool isCallFromSetLayerCommand() const;

    std::map<std::string, rcsSynLayerDefinitionNode_T*>& getTmpLayers() { return this->m_vTmpLayers; }

    std::map<std::string, rcsLithoFileSpec>& getLithoFileSpec(){ return this->m_vLithoFileSpec; }

    std::ostream&  getPVRSOutStream() const;
    std::ostream&  getPVRSSumStream() const;

    bool beginWrite();
    void endWrite();

    void setPVRSOutFile(const std::string &sFileName);

    void setTmpLayerSuffix(const std::string &sSuffix);
    std::string& getTmpLayerSuffix();

    void setGui(const bool isGui);
    bool isGui() const;

    void setNewPvrs(bool isNew);
    bool isNewPvrs() const;

    void setTvfFile(bool isTvf);
    bool isTvfFile() const;

    void setTvfFunction(bool isTvfFunction);
    bool isTvfFunction() const;

    void setIncludeFile(bool isIncludeFile);
    bool isIncludeFile() const;

    void setFlattenMacro(bool isFlatten);
    bool isFlattenMacro() const;

    void setConvertSwitch(bool isConvert);
    bool isConvertSwitch() const;

    void setConvertIncludeFile(bool isConvert);
    bool isConvertIncludeFile() const;

    void setRuleFileHasPrefix(bool isHas);
    bool isRuleFileHasPrefix() const;

    void addText(const hvUInt32 nLineNo, const std::string &sText);

    const std::map<hvUInt32, std::string> &getCheckComment() const;

    const std::map<hvUInt32, std::string> &getFileText() const;

    void addCheckComment(const hvUInt32 nLineNo, const char *sText);

    void clearMapData();

    const char* getTmpFileName() const;

    const std::string &getPVRSCmdPrefix() const;

    const std::string &getImplicitLayerPrefix() const;
    void setImplicitLayerPrefix(const std::string &sPrefix);
    bool isTmpLayer(const std::string &sLayerName) const;

    const hvUInt32 getImplicitLayerID() const;
    void setImplicitLayerID(const hvUInt32 id);

    void setExpandTmpLayer(bool needExpand);
    bool needExpandTmpLayer() const;
    void recoverExpandTmpLayerFlag();

    void setHasTmpLayerDefinition(bool has);
    bool hasTmpLayerDefinition() const;

    void pushIncludeFileName(std::string includeFileName);
    bool isEmptyForIncludeFileName();
    hvUInt32 getIncludeFileVectorSize();
    std::string getIncludeFile(hvUInt32 index);

private:
    static rcsManager_T*  m_spManager;
    rcsManager_T();

private:
    std::string    m_sPVRSOutFile;
    std::string    m_sTmpFile;
    std::string    m_sPVRSCmdPrefix;
    std::string    m_sImplicitLayerPrefix;
    hvUInt32       m_sImplicitLayerID;
    std::ofstream *m_pPVRSOutStream;
    std::ofstream *m_pPVRSSumStream;

    std::string       m_sTmpLayerSuffix;

    hvUInt32       m_nGlobalLineNo;

private:
    std::map<hvUInt32, std::string> m_vFileText;
    std::map<hvUInt32, std::string> m_vCheckComment;
    std::vector<std::string>         m_vIncludeFile;
    std::map<std::string, rcsSynLayerDefinitionNode_T*> m_vTmpLayers;
    std::map<std::string, rcsLithoFileSpec>              m_vLithoFileSpec;
    rcsPragmaManager m_pragmaManager;

private:
    bool m_outComment;
    bool m_isComment;
    bool m_isGui;
    bool m_isNewPvrs;
    bool m_isFlattenMacro;
    bool m_isConvertSwitch;
    bool m_isConvertIncludeFile;
    bool m_needExpandTmpLayer;
    bool m_backupExpandTmpLayer;
    bool m_isTvf2Trs;
    bool m_isTrsFlag;
    bool m_hasTmpLayerDefinition;
    bool m_isIncludeFile;
    bool m_isIncludeFileHasPrefix;      
    bool m_isCallFromSetLayerCommand;
    bool m_isTvfFile;
    bool m_isInTvfFunction;
    bool m_isFirstTime;
    hvUInt32 m_nTmpLayer;
};

bool parseFileName(const std::string &sFileName, std::string &sValue);

inline const char *g_warpper_getenv(const char *pEnv)
{
    return rcsManager_T::getInstance()->getPragmaManager().warpper_getenv(pEnv);
}

#endif 

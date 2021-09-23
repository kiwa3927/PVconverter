



#include <iostream>
#include <fstream>

#include "public/rcsTypes.h"
#include "rcsManager.h"
#include "public/util/rcsStringUtil.hpp"

rcsManager_T *rcsManager_T::m_spManager = NULL;

rcsManager_T::rcsManager_T() : m_sTmpFile(".svrf2pvrs.tmp"), m_sPVRSCmdPrefix("rcn::"),
                               m_sImplicitLayerPrefix("pvrs_tmp_layer_"), m_sImplicitLayerID(0), m_pPVRSOutStream(NULL), m_pPVRSSumStream(NULL),
                               m_nGlobalLineNo(0), m_isComment(false), m_isNewPvrs(false), m_isFlattenMacro(false),
                               m_isConvertSwitch(false), m_hasTmpLayerDefinition(false), m_needExpandTmpLayer(true),
                               m_isConvertIncludeFile(true), m_isIncludeFile(false), m_isCallFromSetLayerCommand(false),
                               m_isTvfFile(false), m_isFirstTime(true), m_nTmpLayer(0), m_sTmpLayerSuffix("")
{
}

void
rcsManager_T::setHasTmpLayerDefinition(bool has)
{
    m_hasTmpLayerDefinition = has;
}

bool rcsManager_T::hasTmpLayerDefinition() const
{
    return m_hasTmpLayerDefinition;
}

void
rcsManager_T::setNewPvrs(bool isNew)
{
    m_isNewPvrs = isNew;
}

bool rcsManager_T::isNewPvrs() const
{
    return m_isNewPvrs;
}

void
rcsManager_T::setTvfFile(bool isTvf)
{
	m_isTvfFile = isTvf;
}

bool
rcsManager_T::isTvfFile() const
{
	return m_isTvfFile;
}

void
rcsManager_T::setTvfFunction(bool isTvfFunction)
{
	m_isInTvfFunction = isTvfFunction;
}

bool
rcsManager_T::isTvfFunction() const
{
	return m_isInTvfFunction;
}

void
rcsManager_T::setIncludeFile(bool isIncludeFile)
{
	m_isIncludeFile = isIncludeFile;
}

bool rcsManager_T::isIncludeFile() const
{
    return m_isIncludeFile;
}

void
rcsManager_T::setConvertIncludeFile(bool isConvert)
{
	m_isConvertIncludeFile = isConvert;
}

bool rcsManager_T::isConvertIncludeFile() const
{
    return m_isConvertIncludeFile;
}

void
rcsManager_T::setRuleFileHasPrefix(bool isHas)
{
	m_isIncludeFileHasPrefix = isHas;
}

bool rcsManager_T::isRuleFileHasPrefix() const
{
    return m_isIncludeFileHasPrefix;
}

void
rcsManager_T::setOutputComment(bool isOut)
{
    m_outComment = isOut;
}

bool
rcsManager_T::outputComment() const
{
    return m_outComment;
}

void
rcsManager_T::setExpandTmpLayer(bool needExpand)
{
    m_needExpandTmpLayer = needExpand;
    m_backupExpandTmpLayer = needExpand;
}

bool
rcsManager_T::needExpandTmpLayer() const
{
    if(!m_isNewPvrs)
        return true;

    return m_needExpandTmpLayer;
}

void
rcsManager_T::recoverExpandTmpLayerFlag()
{
	m_needExpandTmpLayer = m_backupExpandTmpLayer;
}

void
rcsManager_T::setFlattenMacro(bool isFlatten)
{
	m_isFlattenMacro = isFlatten;
}

bool
rcsManager_T::isFlattenMacro() const
{
    return m_isFlattenMacro;
}

void
rcsManager_T::setConvertSwitch(bool isConvert)
{
	m_isConvertSwitch = isConvert;
}

bool
rcsManager_T::isConvertSwitch() const
{
    return m_isConvertSwitch;
}

void
rcsManager_T::setTvf2Trs(bool isTvf)
{
    m_isTvf2Trs = isTvf;




}

bool
rcsManager_T::isTvf2Trs() const
{
    return m_isTvf2Trs;
}

void
rcsManager_T::setTrsFlag(bool isTvf)
{
	m_isTrsFlag = isTvf;




}

bool
rcsManager_T::isTrsFlag() const
{
    return m_isTrsFlag;
}

void
rcsManager_T::setFirstTime(bool isFirst)
{
	m_isFirstTime = isFirst;
}

bool
rcsManager_T::isFirstTime() const
{
	return m_isFirstTime;
}

void
rcsManager_T::setCallFromSetLayerCommand(bool isSetLayer)
{
	m_isCallFromSetLayerCommand = isSetLayer;
}

bool
rcsManager_T::isCallFromSetLayerCommand() const
{
	return m_isCallFromSetLayerCommand;
}

void
rcsManager_T::setOutputSvrfComment(bool isOut)
{
    m_isComment = isOut;
}

bool
rcsManager_T::isSvrfComment() const
{
    return m_isComment;
}

rcsManager_T *
rcsManager_T::getInstance()
{
    if(m_spManager == NULL)
    {
        m_spManager =  new rcsManager_T();
    }
    return m_spManager;
}

void
rcsManager_T::deleteInstance()
{
    delete m_spManager;
    m_spManager = NULL;
}

const std::string &
rcsManager_T::getPVRSCmdPrefix() const
{
    return m_sPVRSCmdPrefix;
}

const std::string &
rcsManager_T::getImplicitLayerPrefix() const
{
    return m_sImplicitLayerPrefix;
}

void
rcsManager_T::setImplicitLayerPrefix(const std::string &sPrefix)
{
    m_sImplicitLayerPrefix = sPrefix;
}

bool rcsManager_T::isTmpLayer(const std::string &sLayerName) const
{
    return 0 == strncmp(sLayerName.c_str(), m_sImplicitLayerPrefix.c_str(), m_sImplicitLayerPrefix.size());
}

const hvUInt32
rcsManager_T::getImplicitLayerID() const
{
    return m_sImplicitLayerID;
}

void
rcsManager_T::setImplicitLayerID(const hvUInt32 id)
{
	m_sImplicitLayerID = id;
}

const char*
rcsManager_T::getTmpFileName() const
{
    return m_sTmpFile.c_str();
}

void
rcsManager_T::setPVRSOutFile(const std::string &sFileName)
{
    m_sPVRSOutFile = sFileName;
}

void
rcsManager_T::setTmpLayerSuffix(const std::string &sSuffix)
{
	m_sTmpLayerSuffix = sSuffix;
}

std::string&
rcsManager_T::getTmpLayerSuffix()
{
	return m_sTmpLayerSuffix;
}

void
rcsManager_T::addText(const hvUInt32 nLineNo, const std::string &sText)
{
    if(m_vFileText.find(nLineNo) == m_vFileText.end())
        m_vFileText[nLineNo] = sText;
    else
        m_vFileText[nLineNo] += sText;
}

void
rcsManager_T::addCheckComment(const hvUInt32 nLineNo, const char *sText)
{
    Utassert(m_vCheckComment.find(nLineNo) == m_vCheckComment.end());
    m_vCheckComment[nLineNo] = sText;
}

void
rcsManager_T::clearMapData()
{
	m_vCheckComment.clear();
	m_vFileText.clear();
}

const std::map<hvUInt32, std::string> &
rcsManager_T::getCheckComment() const
{
    return m_vCheckComment;
}

const std::map<hvUInt32, std::string> &
rcsManager_T::getFileText() const
{
    return m_vFileText;
}

bool
rcsManager_T::beginWrite()
{
    if(m_pPVRSOutStream == NULL)
        m_pPVRSOutStream = new std::ofstream(m_sPVRSOutFile.c_str());
    else
    	m_pPVRSOutStream->open(m_sPVRSOutFile.c_str());

    if(m_pPVRSSumStream == NULL)
        m_pPVRSSumStream = new std::ofstream("svrf2pvrs.log");

    return m_pPVRSOutStream->is_open();
}

void
rcsManager_T::endWrite()
{
    if(m_pPVRSOutStream != NULL)
        m_pPVRSOutStream->close();
    if(m_pPVRSSumStream != NULL)
        m_pPVRSSumStream->close();
}

std::ostream&
rcsManager_T::getPVRSOutStream() const
{
    return *m_pPVRSOutStream;
}

std::ostream&
rcsManager_T::getPVRSSumStream() const
{
    return *m_pPVRSSumStream;
}

void
rcsManager_T::setGui(const bool isGui)
{
    m_isGui = isGui;
}

bool
rcsManager_T::isGui() const
{
    return m_isGui;
}

void
rcsManager_T::pushIncludeFileName(std::string includeFileName)
{
	m_vIncludeFile.push_back(includeFileName);
}

bool
rcsManager_T::isEmptyForIncludeFileName()
{
	return m_vIncludeFile.empty() ? true : false;
}

bool parseFileName(const std::string &sFileName, std::string &sValue)
{
    std::vector<std::string> v;
    if(sFileName[0] == '/')
    {
        sValue += "/";
    }
    tokenize(v, sFileName, "/");
    for(std::vector<std::string>::iterator iter = v.begin();
        iter != v.end(); ++iter)
    {
        if((*iter)[0] != '$')
        {
            sValue += *iter;
        }
        else
        {
            std::string sVarName;
            sVarName = iter->substr(1);
            const char *pValue = g_warpper_getenv(sVarName.c_str());
            if(pValue == NULL || strlen(pValue) == 0)
            {
                sValue = sVarName;
                return false;
            }
            sValue += pValue;
        }
        sValue += "/";
    }
    sValue.erase(--sValue.end());
    return true;
}

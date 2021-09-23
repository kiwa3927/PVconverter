#ifndef RCSSYNPVRSCONVERTOR_H_
#define RCSSYNPVRSCONVERTOR_H_

#include <set>
#include <map>
#include <list>
#include <string>
#include <ostream>
#include <stack>
#include <map>
#include "unistd.h"
#include <fstream>
#include "public/util/rcsStringUtil.hpp"

#include "public/synnode/rcsSynNodeVisitor.h"
#include "public/token/rcsToken.h"

class rcsSynRuleCheckNode_T;
class rcsSynNode_T;

class rcsSynPvrsConvertor: public rcsNodeVisitor_T
{
public:
    rcsSynPvrsConvertor(std::map<hvUInt32, std::pair<hvUInt32, bool> > &blankLinesBefore, bool bTvfConvertor = false);
    virtual ~rcsSynPvrsConvertor();

    virtual void beginVisitLayerDefinitionNode(rcsSynLayerDefinitionNode_T *pNode);
    virtual void endVisitLayerDefinitionNode(rcsSynLayerDefinitionNode_T *pNode);

    virtual void beginVisitRuleCheckNode(rcsSynRuleCheckNode_T *pNode);
    virtual void endVisitRuleCheckNode(rcsSynRuleCheckNode_T *pNode);

    virtual void beginVisitLayerOperationNode(rcsSynLayerOperationNode_T *pNode);
    virtual void endVisitLayerOperationNode(rcsSynLayerOperationNode_T *pNode);

    virtual void beginVisitSpecificationNode(rcsSynSpecificationNode_T *pNode);

    virtual void beginVisitPreProcessNode(rcsSynPreProcessNode_T *pNode);

    virtual void beginVisitProcNode(rcsSynProcNode_T *pNode);
    virtual void endVisitProcNode(rcsSynProcNode_T *pNode);

    virtual void beginVisitCommentNode(rcsSynCommentNode_T *pNode);

    virtual void beginVisitBuildInNode(rcsSynBuildInNode_T *pNode);

    virtual void beginVisitDMACRODummyNode(rcsSynDMACRODummyNode_T *pNode);

    virtual void beginVisitSwitchUnknowNode(rcsSynSwitchUnKnowNode_T *pNode);


    static bool getStringName(std::list<rcsToken_T>::iterator &iter,
                              std::list<rcsToken_T>::iterator end,
                              std::string &sValue);

    void setHasGetTmpLayerValues();

    void closeDebugFile();

protected:
    static bool isPossibleNoParaKeyword(rcsSynSpecificationNode_T *pNode);
    static void parseOperands(std::string &str, std::vector<std::string> &vOperands);

    static bool getNumberExp(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end, std::string &sExp,
                             bool withParentheses = false, bool isAllowExpression = true);
    static bool getNumberExpForTvfCmdWithNegativePara(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end,
            std::string &sExp, bool withParentheses = false, bool isAllowExpression = true);
    static bool getConstraint(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end,
            std::string &sConstraint);
    static bool getConstraintNoExpr(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end,
            std::string &sConstraint);

    bool outputBuiltInLangInTrs(const rcsToken_T &token, hvUInt32 &nOption);
    void outputBuiltInLang(KEY_TYPE eType, const rcsToken_T &token, hvUInt32 &nOption);

    bool getLayerName(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end, std::string &sLayerName);
    std::string getRealLayerName(const std::vector<std::string> &namescopes, const std::string &name);
    bool getLayerRefName(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end, std::string &sLayerName);

    static bool parseFollowNums(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end,
                                std::vector<std::string> &vNumExps, bool (*isValidKey)(const rcsToken_T &),
                                const hvUInt32 nMinCount = 1, const hvUInt32 nMaxCount = 2);
    bool transLayoutExceptionOption(const std::string &sExceptionName, int iSeverityValue,std::string &sCmd);
    void outputText(const char *pOption, const char *pValue, const hvUInt32 nOption = 0);
    void outputOriginString(std::list<rcsToken_T>::iterator iter);
    void outputCon2Error(rcsSynNode_T *pNode, const rcsToken_T &token);
    void outputCon1Error(rcsSynNode_T *pNode);
    bool parsePlaceCellInfo(std::list<rcsToken_T>::iterator &iter,
                            std::list<rcsToken_T>::iterator end);
    void parseNARRDB(std::list<rcsToken_T>::iterator &iter,
                     std::list<rcsToken_T>::iterator end, hvUInt32 &nOption);
    bool parseNetAreaRatioLayer(std::list<rcsToken_T>::iterator &iter,
                                std::list<rcsToken_T>::iterator end,
                                hvUInt32 &nOption);
    void _parseOffsetOption(std::list<rcsToken_T>::iterator &iter,
                            const std::list<rcsToken_T>::iterator end,
                            hvUInt32 &nOption);
    
    void convLayoutInputSpec(rcsSynSpecificationNode_T *pNode);
    void convMaxLimitSpec(rcsSynSpecificationNode_T *pNode);
    void convSumFileSpec(rcsSynSpecificationNode_T *pNode);
    void convDrcMapTextLayer(rcsSynSpecificationNode_T *pNode);
    void convResultDBSpec(rcsSynSpecificationNode_T *pNode);
    void convNameSpec(rcsSynSpecificationNode_T *pNode);
    void convPrecSpec(rcsSynSpecificationNode_T *pNode);
    void convResolutionSpec(rcsSynSpecificationNode_T *pNode);
    void convAssignLayerSpec(rcsSynSpecificationNode_T *pNode);
    void convYesNoSpec(rcsSynSpecificationNode_T *pNode);
    void convSourceDbSpec(rcsSynSpecificationNode_T *pNode);
    void convLayerListSpec(rcsSynSpecificationNode_T *pNode);
    void convDeviceSpec(rcsSynSpecificationNode_T *pNode);
    void convCellNameSpec(rcsSynSpecificationNode_T *pNode);
    void convRuleSelectSpec(rcsSynSpecificationNode_T *pNode);
    void convMakeRuleGroupSpec(rcsSynSpecificationNode_T *pNode);
    void convRuleMapSpec(rcsSynSpecificationNode_T *pNode);
    void convRuleTextSpec(rcsSynSpecificationNode_T *pNode);
    void convLayerMapSpec(rcsSynSpecificationNode_T *pNode);
    void convCompareCaseSpec(rcsSynSpecificationNode_T *pNode);
    void convNetListOutputSpec(rcsSynSpecificationNode_T *pNode);
    void convCheckPropertySpec(rcsSynSpecificationNode_T *pNode);
    void convReduceSpecificSpec(rcsSynSpecificationNode_T *pNode);
    void convReduceSplitGatesSpec(rcsSynSpecificationNode_T *pNode);
    void convShortEquivalentNodesSpec(rcsSynSpecificationNode_T *pNode);
    void convReduceGenericSpec(rcsSynSpecificationNode_T *pNode);
    void convDepthSpec(rcsSynSpecificationNode_T *pNode);
    void convUserDefDbSpec(rcsSynSpecificationNode_T *pNode);
    void convOutputLvsDBSpec(rcsSynSpecificationNode_T *pNode);
    void convVarSpec(rcsSynSpecificationNode_T *pNode);

    void convDfmFunction(rcsSynSpecificationNode_T *pNode);

    void convPolygonToLayerSpec(rcsSynSpecificationNode_T *pNode);
    void convTextToLayerSpec(rcsSynSpecificationNode_T *pNode);
    void convFilterSpec(rcsSynSpecificationNode_T *pNode);
    void convFilterUnusedOptionSpec(rcsSynSpecificationNode_T *pNode);
    void convMapDeviceSpec(rcsSynSpecificationNode_T *pNode);
    void convReducePreferSpec(rcsSynSpecificationNode_T *pNode);
    void convInitialPropertySpec(rcsSynSpecificationNode_T *pNode);
    void convMapPropertySpec(rcsSynSpecificationNode_T *pNode);
    void convPushDeviceSpec(rcsSynSpecificationNode_T *pNode);
    void convMapDeviceTypeSpec(rcsSynSpecificationNode_T *pNode);
    void convCorrespondingCellSpec(rcsSynSpecificationNode_T *pNode);

    void convLvsSplitGateRatio(rcsSynSpecificationNode_T *pNode);
    void convLvsNetlistProperty(rcsSynSpecificationNode_T *pNode);
    void convLvsAutoExpandHcell(rcsSynSpecificationNode_T *pNode);

    void convOutputMagSpec(rcsSynSpecificationNode_T *pNode);
    void convDrcOutputLibSpec(rcsSynSpecificationNode_T *pNode);
    void convDrcOutPrecisionSpec(rcsSynSpecificationNode_T *pNode);
    void convFindShortsSpec(rcsSynSpecificationNode_T *pNode);
    void convErcPathchkSpec(rcsSynSpecificationNode_T *pNode);

    void convErcTclSpec(rcsSynSpecificationNode_T *pNode);

    void convDeviceOnPathSpec(rcsSynSpecificationNode_T *pNode);
    void convCellListSpec(rcsSynSpecificationNode_T *pNode);
    void convFileHeadNameSpec(rcsSynSpecificationNode_T *pNode);
    void convPolygonToDBSpec(rcsSynSpecificationNode_T *pNode);
    void convTextToDBSpec(rcsSynSpecificationNode_T *pNode);
    void convLayoutTextFileSpec(rcsSynSpecificationNode_T *pNode);
    void convCellListDefSpec(rcsSynSpecificationNode_T *pNode);
    void convExceptionDBSpec(rcsSynSpecificationNode_T *pNode);
    void convExceptionSeveritySpec(rcsSynSpecificationNode_T *pNode);
    void convCoordListSpec(rcsSynSpecificationNode_T *pNode);
    void convDBMagnifySpec(rcsSynSpecificationNode_T *pNode);
    void convertDBMergeOnInput(rcsSynSpecificationNode_T *pNode);
    void convRenameTextSpec(rcsSynSpecificationNode_T *pNode);
    void convLayerResolutionSpec(rcsSynSpecificationNode_T *pNode);
    void convRenameCellSpec(rcsSynSpecificationNode_T *pNode);
    void convBlackBoxSpec(rcsSynSpecificationNode_T *pNode);
    void convLvsBlackBoxPortSpec(rcsSynSpecificationNode_T *pNode);
    void convLvsBlackBoxPortDepthSpec(rcsSynSpecificationNode_T *pNode);
    void convLvsBoxCutExtentSpec(rcsSynSpecificationNode_T *pNode);
    void convLvsBoxPeekLayerSpec(rcsSynSpecificationNode_T *pNode);
    void convPropResolutionMaxSpec(rcsSynSpecificationNode_T *pNode);
    void convInitialNetSpec(rcsSynSpecificationNode_T *pNode);
    void convNameListSpec(rcsSynSpecificationNode_T *pNode);
    void convIdentifyGatesSpec(rcsSynSpecificationNode_T *pNode);
    void convLVSFlattenInsideCell(rcsSynSpecificationNode_T *pNode);
    void convGroupGlobalNameSpec(rcsSynSpecificationNode_T *pNode);
    void convNonMatchNameSpec(rcsSynSpecificationNode_T *pNode);
    void convLvsReportOptionSpec(rcsSynSpecificationNode_T *pNode);
    void convCheckSpec(rcsSynSpecificationNode_T *pNode);
    void convPutsSpec(rcsSynSpecificationNode_T *pNode);
    void convExpandTextInCellSpec(rcsSynSpecificationNode_T *pNode);
    void convErrorSpec(rcsSynSpecificationNode_T *pNode);
    void convSpiceOptionSpec(rcsSynSpecificationNode_T *pNode);
    void convRenameSpiceParameterSpec(rcsSynSpecificationNode_T *pNode);
    void covDeviceGroupSpec(rcsSynSpecificationNode_T *pNode);

    void convSconnectCheckSpec(rcsSynSpecificationNode_T *pNode);
    void convSoftChkMaxResSpec(rcsSynSpecificationNode_T *pNode);
    void convUnitOptionSpec(rcsSynSpecificationNode_T *pNode);
    void convCustomizedSpec(rcsSynSpecificationNode_T *pNode);
    void convOldIsolateNetSpec(rcsSynSpecificationNode_T *pNode);
    void convTvfFuncSpec(rcsSynSpecificationNode_T *pNode);
    std::string  dealVariable(std::string sScript);
    
    void convExecProcCmd(rcsSynLayerOperationNode_T *pNode);
    void convCheckSpaceCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomBooleanCmd(rcsSynLayerOperationNode_T *pNode);
    void convEdgeMergeCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomTopoCmd(rcsSynLayerOperationNode_T *pNode);
    void convAttachCmd(rcsSynLayerOperationNode_T *pNode);
    void convConnectCmd(rcsSynLayerOperationNode_T *pNode);
    void convAttachOrderCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomAttrCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomCopyLayerCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomSelectHolesCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomSelectRectangleCmd(rcsSynLayerOperationNode_T *pNode);
    void convEdgeTopoCmd(rcsSynLayerOperationNode_T *pNode);
    void convCheckFittingRectangleCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomSelectInCellCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomGetBBoxCmd(rcsSynLayerOperationNode_T *pNode);
    void convSizeCmd(rcsSynLayerOperationNode_T *pNode);
    void convSize_XYCmd(rcsSynLayerOperationNode_T *pNode);
    void convCheckDensityCmd(rcsSynLayerOperationNode_T *pNode);
    void convCheckNetAreaRatioCmd(rcsSynLayerOperationNode_T *pNode);
    void convCheckNetAreaRatioByAccumulateCmd(rcsSynLayerOperationNode_T *pNode);
    void convCheckOffgridCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomOriginalCheckCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomSelectDeviceLayerCmd(rcsSynLayerOperationNode_T *pNode);
    void convDfmOrEdge(rcsSynLayerOperationNode_T *pNode);
    void convBuildDfmPropertyCmd(rcsSynLayerOperationNode_T *pNode);
    void convDfmPropertySelectSecondaryCmd(rcsSynLayerOperationNode_T *pNode);
    void convBuildDfmPropertyNetCmd(rcsSynLayerOperationNode_T *pNode);
    void convDfmSpaceCmd(rcsSynLayerOperationNode_T *pNode);
    void convDfmShiftEdge(rcsSynLayerOperationNode_T *pNode);
    void convDfmDV(rcsSynLayerOperationNode_T *pNode);
    void convDfmSizeCmd(rcsSynLayerOperationNode_T *pNode);
    void convEdgeSelectByConvexCornerCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomCopyDfmLayerCmd(rcsSynLayerOperationNode_T *pNode);
    void convDfmMergePropertyCmd(rcsSynLayerOperationNode_T *pNode);
    void convDfmNARACCmd(rcsSynLayerOperationNode_T *pNode);
    void convDfmRDBCmd(rcsSynLayerOperationNode_T *pNode);
    void convDfmTextCmd(rcsSynLayerOperationNode_T *pNode);
    void convTvfCmd(rcsSynLayerOperationNode_T *pNode);

    void convGeomGetNetByNameCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomGetLayerBBoxCmd(rcsSynLayerOperationNode_T *pNode);

    void convDfmStampCmd(rcsSynLayerOperationNode_T *pNode);
    void convEdgeSelectByAttrCmd(rcsSynLayerOperationNode_T *pNode);
    void parseRuleValue(std::list<rcsToken_T>::iterator &iter,
                        std::list<rcsToken_T>::iterator end);
    void parseRectRule(std::list<rcsToken_T>::iterator &iter,
                       std::list<rcsToken_T>::iterator end);
    void convCheckRectEncCmd(rcsSynLayerOperationNode_T *pNode);

    void convGeomFlattenLayerCmd(rcsSynLayerOperationNode_T *pNode);
    void convEdgeExpandToRectCmd(rcsSynLayerOperationNode_T *pNode);
    void convFillRectanglesCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomGetCellBBoxCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomGetMultiLayersBBoxCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomTransformCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomMergeLayerCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomGetNetByAreaCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomMergeNetCmd(rcsSynLayerOperationNode_T *pNode);
    void convEdgeAngledToRegularCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomPushLayerCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomVerticeSnapToGridCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomGetTextBBoxCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomStampLayerCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomSelectByNeighborCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomSelectByCoinEdgeCmd(rcsSynLayerOperationNode_T *pNode);
    void convGeomSelectTextedCmd(rcsSynLayerOperationNode_T *pNode);
    void convCheckWidthCmd(rcsSynLayerOperationNode_T *pNode);
    void convOpcbiasRule(std::list<rcsToken_T>::iterator &iter,
                            std::list<rcsToken_T>::iterator end,
                            hvUInt32 &nOption);
    void convOpcBiasCmd(rcsSynLayerOperationNode_T *pNode);
    static void parsePathChkName(std::list<rcsToken_T>::iterator &iter,
                                 std::list<rcsToken_T>::iterator end,
                                 std::string &sValues);
    static void parseBreakName(std::list<rcsToken_T>::iterator &iter,
                               std::list<rcsToken_T>::iterator end,
                               std::string &sValues);
    void convPathchkCmd(rcsSynLayerOperationNode_T *pNode);
    void convNetAreaRatioRDBCmd(rcsSynLayerOperationNode_T *pNode);
    void convDisconnectCmd(rcsSynLayerOperationNode_T *pNode);
    void convTableDRCSpaceRule(std::list<rcsToken_T>::iterator &iter,
                                  std::list<rcsToken_T>::iterator end);
    void convTableDRCWidthRule(std::list<rcsToken_T>::iterator &iter,
                                  std::list<rcsToken_T>::iterator end);
    void convCheckTableDRCCmd(rcsSynLayerOperationNode_T *pNode);
    void convSconnectCmd(rcsSynLayerOperationNode_T *pNode);
    void convNetInteract(rcsSynLayerOperationNode_T *pNode);
    void convLayoutCloneRotatedPlacements(rcsSynSpecificationNode_T *pNode);
    void convLayoutCloneTransformedPlacements(rcsSynSpecificationNode_T *pNode);
    void convLayoutPathWidthMultiple(rcsSynSpecificationNode_T *pNode);
    void convDFMDefaultsRDB(rcsSynSpecificationNode_T *pNode);

    void convDFMFill(rcsSynLayerOperationNode_T *pNode);
    
    void outputTrsPrefix(rcsSynSpecificationNode_T *pNode);
    void outputTrsPrefix(rcsSynLayerOperationNode_T *pNode);

    void outputBlankLinesBefore(hvUInt32 nCurLine, bool isReset = true);


    bool convCommonNode(KEY_TYPE type, rcsSynNode_T *pNode);

    bool parseTmpLayerForOperation(std::string& tmpLayer);

    void parseLithoFileSpec(rcsSynSpecificationNode_T *pNode);
    void convRetNMDPCCmd(rcsSynLayerOperationNode_T *pNode);
    void convRetTPCmd(rcsSynLayerOperationNode_T *pNode);
    void convRetQPCmd(rcsSynLayerOperationNode_T *pNode);
private:
    typedef std::map<KEY_TYPE, std::string> rcs_OptionMap;
    struct rcs_SvrfRuleInfo
    {
        KEY_TYPE      m_key;
        std::string   m_pvrsKey;
        rcs_OptionMap m_optMap;
    };

    typedef std::map<KEY_TYPE, rcs_SvrfRuleInfo> rcs_KeyMap;

    rcs_KeyMap m_simplePVRSmap;
    void initSimplePVRSmap();
    rcs_SvrfRuleInfo& addSvrfKey(KEY_TYPE t, const char *pvrs);
    void addSvrfOption(rcs_SvrfRuleInfo &info, KEY_TYPE t, const char *pvrs);
protected:
    static bool _hasOnlyDirectKeyword(rcsSynNode_T *pNode);

protected:
    std::ostream *m_oPVRSStream;
    std::ostream &m_oLogStream;
    std::string   m_sPVRSCmdPrefix;
    std::string   m_sTmpLayerName;

    std::ofstream  m_oCommandStream;
    std::string   m_sCurCommandString;
    std::set<std::string> m_debugCommandSet;
protected:
    bool m_isTmpLayerDefinition;
    bool m_isInLayerDefinition;
    bool m_hasPrecisionDef;
    bool m_hasGetTmpLayerValue;
    hvUInt32 m_nParsingTmpLayers;
protected:
    struct ltStringCase
    {
        bool operator()(const char *pValue1, const char *pValue2)const
        {
            return (strcasecmp(pValue1, pValue2) < 0);
        }

        bool operator()(const std::string &sValue1, const std::string &sValue2)const
        {
            return (strcasecmp(sValue1.c_str(), sValue2.c_str()) < 0);
        }
    };

    static const std::string            m_sTmpLayerPrefix;
    std::map<std::string, std::string>  m_vTmpLayerValues;
    std::set<std::string, ltStringCase> m_vVariableNames;
    std::map<std::string, hvUInt32>     m_vProcUsedTimes;
    std::vector<std::string>            m_vProcStacks;
    std::set<std::string>               m_vDefLayers;
    hvUInt32                            m_iProcLevel;
    bool                                m_inProcReturn;

private:
    std::map<hvUInt32, std::pair<hvUInt32, bool> > &m_blankLinesBefore;
    bool m_bTvfConvertor;

private:
    
    

    class _rcsBuildinLayerReplacer
    {
    public:
        void insert(const std::string &origName, const std::string &scopeName)
        {
            if (0 == strcasecmp(origName.c_str(), scopeName.c_str()))
                return;

            m_layerMap.insert(make_pair(origName, scopeName));
        }

        std::string replaceBuildInLang(const std::string &s) const
        {
            if (m_layerMap.empty())
                return s;

            std::string ret = s;

            for (_CaseStrMap::const_iterator it=m_layerMap.begin(); it!=m_layerMap.end(); ++it)
            {
                ret = replace_one(ret, it->first, it->second);
            }

            return ret;
        }
    private:
        std::string replace_one(const std::string &str, const std::string &key, const std::string &value) const
        {
            
            Utassert(!key.empty() && !value.empty());

            std::string ret;
            std::stack<bool> stack;
            const char *p = str.c_str();
            const char *end = str.c_str() + str.size();

            while (p < end)
            {
                switch (*p)
                {
                    case '(' :
                        stack.push(true);
                        break;
                    case ')':
                        if (!stack.empty())
                            stack.pop();
                        break;
                    default:
                    {
                        if (!stack.empty() && 0 == strncasecmp(p, key.c_str(), key.size()))
                        {
                            ret += '\"';
                            ret += value;
                            ret += '\"';
                            p += key.size();
                            continue;
                        }
                        break;
                    }
                }

                ret += *p++;
            }

            return ret;
        }
    private:
        typedef std::map<std::string, std::string, ltStringCase> _CaseStrMap;
        _CaseStrMap m_layerMap;
    };

    bool getLayerRefName_addScopedLayerMap(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end, std::string &sLayerName, _rcsBuildinLayerReplacer &scopedMap);
};

#endif 

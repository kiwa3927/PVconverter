



#ifndef SYNRULECHECKVISITOR_H_
#define SYNRULECHECKVISITOR_H_

#include <set>
#include <map>
#include <list>
#include <string>
#include <ostream>

#include "public/synnode/rcsSynNodeVisitor.h"
#include "public/token/rcsToken.h"

class rcsSynRuleCheckNode_T;
class rcsSynNode_T;

class rcsSynNodeConvertor_T: public rcsNodeVisitor_T
{
public:
    rcsSynNodeConvertor_T();
    virtual ~rcsSynNodeConvertor_T();

public:
    static void parseOperands(std::string &str, std::vector<std::string> &vOperands);
    static bool getNumberExp(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end, std::string &sExp,
                             bool withParentheses = false, bool isAllowExpression = true);
    static bool getConstraint(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end, std::string &sConstraint);
    bool getLayerRefName(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end, std::string &sLayerName);
    static bool getStringName(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end, std::string &sValue, bool transString = true);
    static bool parseFollowNums(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end,
                                std::vector<std::string> &vNumExps, bool (*isValidKey)(const rcsToken_T &),
                                const hvUInt32 nMinCount = 1, const hvUInt32 nMaxCount = 2);

protected:
    void _parseOffsetOption(std::list<rcsToken_T>::iterator &iter,
                            const std::list<rcsToken_T>::iterator end,
                            hvUInt32 &nOption);

    void convertAssignLayerSpec(rcsSynSpecificationNode_T *pNode);
    void convertSetSpec(rcsSynSpecificationNode_T *pNode);
    void convertMakeRuleGroupSpec(rcsSynSpecificationNode_T *pNode);
    void convertCellNameSpec(rcsSynSpecificationNode_T *pNode);
    void convertRuleMapSpec(rcsSynSpecificationNode_T *pNode);
    void convertRuleTextSpec(rcsSynSpecificationNode_T *pNode);
    void convertOutputMagSpec(rcsSynSpecificationNode_T *pNode);
    void convertMapTextLayerSpec(rcsSynSpecificationNode_T *pNode);
    void convertDatabaseSpec(rcsSynSpecificationNode_T *pNode);
    void convertDrcOutputLibSpec(rcsSynSpecificationNode_T *pNode);
    void convertDrcOutPrecisionSpec(rcsSynSpecificationNode_T *pNode);
    void convertSumFileSpec(rcsSynSpecificationNode_T *pNode);
    void convertCorrespondingCellSpec(rcsSynSpecificationNode_T *pNode);
    void convertLayerResolutionSpec(rcsSynSpecificationNode_T *pNode);
    void convertTextToLayerSpec(rcsSynSpecificationNode_T *pNode);
    void convertTextToDBSpec(rcsSynSpecificationNode_T *pNode);
    void convertPolygonToLayerSpec(rcsSynSpecificationNode_T *pNode);
    void convertPolygonToDBSpec(rcsSynSpecificationNode_T *pNode);
    void convertLayerMapSpec(rcsSynSpecificationNode_T *pNode);
    void convertCheckSpec(rcsSynSpecificationNode_T *pNode);
    void convertCellListDefSpec(rcsSynSpecificationNode_T *pNode);
    void convertExpandTextInCellSpec(rcsSynSpecificationNode_T *pNode);
    void convertPrecSpec(rcsSynSpecificationNode_T *pNode);
    void convertRenameTextSpec(rcsSynSpecificationNode_T *pNode);
    void convertRenameCellSpec(rcsSynSpecificationNode_T *pNode);
    void convertDBMagnifySpec(rcsSynSpecificationNode_T *pNode);
    void convertDBPathSpec(rcsSynSpecificationNode_T *pNode);
    bool parsePlaceCellInfo(std::list<rcsToken_T>::iterator &iter,
                            std::list<rcsToken_T>::iterator end);
    void convertUserDefDbSpec(rcsSynSpecificationNode_T *pNode);

    void convertGroupGlobalNameSpec(rcsSynSpecificationNode_T *pNode);
    void convertUnitOptionSpec(rcsSynSpecificationNode_T *pNode);

    void convertErrorSpec(rcsSynSpecificationNode_T *pNode);
    void convertPutsSpec(rcsSynSpecificationNode_T *pNode);

    void convertCompareCaseSpec(rcsSynSpecificationNode_T *pNode);
    void convertNonMatchNameSpec(rcsSynSpecificationNode_T *pNode);
    void convertPropTextSpec(rcsSynSpecificationNode_T *pNode);
    void convertIdentifyGatesSpec(rcsSynSpecificationNode_T *pNode);
    void convertMapDeviceSpec(rcsSynSpecificationNode_T *pNode);
    void convertPropResolutionMaxSpec(rcsSynSpecificationNode_T *pNode);
    void convertMapDeviceTypeSpec(rcsSynSpecificationNode_T *pNode);
    void convertOutputLvsDBSpec(rcsSynSpecificationNode_T *pNode);

    void convertDeviceSpec(rcsSynSpecificationNode_T *pNode);
    void convertSconnectCheckSpec(rcsSynSpecificationNode_T *pNode);
    void convertBlackBoxSpec(rcsSynSpecificationNode_T *pNode);
    void convertDeviceOnPathSpec(rcsSynSpecificationNode_T *pNode);
    void convertInitialNetSpec(rcsSynSpecificationNode_T *pNode);
    void convertInitialPropertySpec(rcsSynSpecificationNode_T *pNode);
    void convertMapPropertySpec(rcsSynSpecificationNode_T *pNode);
    void convertSourceDbSpec(rcsSynSpecificationNode_T *pNode);
    void convertCheckPropertySpec(rcsSynSpecificationNode_T *pNode);
    void convertFindShortsSpec(rcsSynSpecificationNode_T *pNode);
    void convertPushDeviceSpec(rcsSynSpecificationNode_T *pNode);
    void convertReduceGenericSpec(rcsSynSpecificationNode_T *pNode);
    void convertIdentifyToleranceSpec(rcsSynSpecificationNode_T *pNode);
    void outputNetListOutputSpec(rcsSynSpecificationNode_T *pNode);
    void convertFilterSpec(rcsSynSpecificationNode_T *pNode);
    void convertErcPathchkSpec(rcsSynSpecificationNode_T *pNode);
    void convertFileHeadNameSpec(rcsSynSpecificationNode_T *pNode);
    void convertTclProcSpec(rcsSynSpecificationNode_T *pNode);
    void convertFilterUnusedOptionSpec(rcsSynSpecificationNode_T *pNode);
    void convertSplitGateRatioSpec(rcsSynSpecificationNode_T *pNode);
    void convertShortEquivalentNodesSpec(rcsSynSpecificationNode_T *pNode);
    void convertReduceSpecificSpec(rcsSynSpecificationNode_T *pNode);
    void convertReduceSplitGatesSpec(rcsSynSpecificationNode_T *pNode);
    void convertReducePreferSpec(rcsSynSpecificationNode_T *pNode);
    void convertLvsReportOptionSpec(rcsSynSpecificationNode_T *pNode);
    void convertRenameSpiceParameterSpec(rcsSynSpecificationNode_T *pNode);
    void convertSpiceOptionSpec(rcsSynSpecificationNode_T *pNode);

    static void parsePathChkName(std::list<rcsToken_T>::iterator &iter,
                                 std::list<rcsToken_T>::iterator end,
                                 std::string &sValues);
    static void parseBreakName(std::list<rcsToken_T>::iterator &iter,
                               std::list<rcsToken_T>::iterator end,
                               std::string &sValues);
    void convertPathchkCmd(rcsSynLayerOperationNode_T *pNode);
    void convertResolutionSpec(rcsSynSpecificationNode_T *pNode, const std::string &sCmd);
    void convertDepthSpec(rcsSynSpecificationNode_T *pNode, const std::string &sCmd);
    void convertLayerListSpec(rcsSynSpecificationNode_T *pNode, const std::string &sCmd);
    void convertCellListSpec(rcsSynSpecificationNode_T *pNode, const std::string &sCmd);
    void convertRuleSelectSpec(rcsSynSpecificationNode_T *pNode, const std::string &sCmd);
    void convertMaxLimitSpec(rcsSynSpecificationNode_T *pNode, const std::string &sCmd, bool canSpecAll = false);
    void convertYesNoSpec(rcsSynSpecificationNode_T *pNode, const std::string &sCmd);
    void convertNameSpec(rcsSynSpecificationNode_T *pNode, const std::string &sCmd);
    void convertNameListSpec(rcsSynSpecificationNode_T *pNode, const std::string &sCmd);
    void convertCoordListSpec(rcsSynSpecificationNode_T *pNode, const std::string &sCmd);

    void convertMergeOnReadSpec(rcsSynSpecificationNode_T *pNode);
    void convertExecProcCmd(rcsSynLayerOperationNode_T *pNode);
    void convertAttachCmd(rcsSynLayerOperationNode_T *pNode);
    void convertConnectCmd(rcsSynLayerOperationNode_T *pNode);
    void convertDisconnectCmd(rcsSynLayerOperationNode_T *pNode);
    void convertAttachOrderCmd(rcsSynLayerOperationNode_T *pNode);
    void convertSconnectCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomBooleanCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomTopoCmd(rcsSynLayerOperationNode_T *pNode);
    void convertEdgeTopoCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomAttrCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomSelectRectangleCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomSelectByCoinEdgeCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomSelectInCellCmd(rcsSynLayerOperationNode_T *pNode);
    void convertCheckFittingRectangleCmd(rcsSynLayerOperationNode_T *pNode);
    void convertEdgeMergeCmd(rcsSynLayerOperationNode_T *pNode);
    void convertEdgeSelectByAttrCmd(rcsSynLayerOperationNode_T *pNode);
    void convertEdgeSelectByConvexCornerCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomOriginalCheckCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomTransformCmd(rcsSynLayerOperationNode_T *pNode);
    void convertEdgeExpandToRectCmd(rcsSynLayerOperationNode_T *pNode);
    void convertSizeCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomSelectHolesCmd(rcsSynLayerOperationNode_T *pNode);
    void convertEdgeConvertAgnledToRegularCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomVerticeSnapToGridCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomMergeLayerCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomCopyLayerCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomFlattenLayerCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomGetBBoxCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomGetCellBBoxCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomGetLayerBBoxCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomGetMultiLayersBBoxCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomGetTextBBoxCmd(rcsSynLayerOperationNode_T *pNode);
    void convertFillRectanglesCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomSelectTextedCmd(rcsSynLayerOperationNode_T *pNode);
    void convertTableDRCSpaceRule(std::list<rcsToken_T>::iterator &iter,
                                  std::list<rcsToken_T>::iterator end);
    void convertTableDRCWidthRule(std::list<rcsToken_T>::iterator &iter,
                                  std::list<rcsToken_T>::iterator end);
    void convertCheckTableDRCCmd(rcsSynLayerOperationNode_T *pNode);
    void convertCheckDensityCmd(rcsSynLayerOperationNode_T *pNode);
    void convertCheckWidthCmd(rcsSynLayerOperationNode_T *pNode);
    void convertCheckSpaceCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomSelectByNeighborCmd(rcsSynLayerOperationNode_T *pNode);
    void convertCheckOffgridCmd(rcsSynLayerOperationNode_T *pNode);
    void parseRuleValue(std::list<rcsToken_T>::iterator &iter,
                        std::list<rcsToken_T>::iterator end);
    void parseRectRule(std::list<rcsToken_T>::iterator &iter,
                       std::list<rcsToken_T>::iterator end);
    void convertCheckRectEncCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomSelectDeviceLayerCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomGetNetByAreaCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomGetNetByNameCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomMergeNetCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomPushLayerCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomStampLayerCmd(rcsSynLayerOperationNode_T *pNode);

    bool parseNetAreaRatioLayer(std::list<rcsToken_T>::iterator &iter,
                                std::list<rcsToken_T>::iterator end,
                                hvUInt32 &nOption);
    void parseNARRDB(std::list<rcsToken_T>::iterator &iter,
                     std::list<rcsToken_T>::iterator end, hvUInt32 &nOption);
    void convertCheckNetAreaRatioCmd(rcsSynLayerOperationNode_T *pNode);
    void convertCheckNetAreaRatioByAccumulateCmd(rcsSynLayerOperationNode_T *pNode);
    void convertNetAreaRatioRDBCmd(rcsSynLayerOperationNode_T *pNode);

    void convertBuildDfmPropertyCmd(rcsSynLayerOperationNode_T *pNode);
    void convertDfmSpaceCmd(rcsSynLayerOperationNode_T *pNode);
    void convertDfmSizeCmd(rcsSynLayerOperationNode_T *pNode);
    void convertGeomCopyDfmLayerCmd(rcsSynLayerOperationNode_T *pNode);
    void convertDfmStampCmd(rcsSynLayerOperationNode_T *pNode);
    void convertDfmRDBCmd(rcsSynLayerOperationNode_T *pNode);
    void convertDfmNARACCmd(rcsSynLayerOperationNode_T *pNode);

    void convertExceptionSeveritySpec(rcsSynSpecificationNode_T *pNode);
    void convertExceptionDBSpec(rcsSynSpecificationNode_T *pNode);

    void outputText(const char *pOption, const char *pValue = NULL,
                    const hvUInt32 nOption = 0);

    void convertOpcbiasRule(std::list<rcsToken_T>::iterator &iter,
                            std::list<rcsToken_T>::iterator end,
                            hvUInt32 &nOption);
    void convertOpcBiasCmd(rcsSynLayerOperationNode_T *pNode);

    void outputCon2Error(rcsSynNode_T *pNode, const rcsToken_T &token);
    void outputCon1Error(rcsSynNode_T *pNode);

protected:
    static bool _hasOnlyDirectKeyword(rcsSynNode_T *pNode);

protected:
    std::ostream &m_oPVRSStream;
    std::ostream &m_oLogStream;
    std::string   m_sPVRSCmdPrefix;

protected:
    bool m_isInLayerDefinition;
    bool m_hasPrecisionDef;

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
    std::set<std::string, ltStringCase> m_vVariableNames;
    std::map<std::string, hvUInt32>     m_vProcUsedTimes;
    std::vector<std::string>            m_vProcStacks;
    hvUInt32                            m_iProcLevel;
};

#endif 

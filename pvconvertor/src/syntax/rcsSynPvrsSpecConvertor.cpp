#include "public/manager/rcErrorManager.h"
#include "public/util/rcsStringUtil.hpp"
#include "public/synnode/rcsSynRulecheckNode.h"
#include "public/synnode/rcsSynSpecificationNode.h"
#include "public/synnode/rcsSynLayerOperationNode.h"
#include "public/synnode/rcsSynLayerDefinitionNode.h"
#include "public/synnode/rcsSynProcessNode.h"
#include "public/synnode/rcsSynCommentNode.h"
#include "public/synnode/rcsSynProcNode.h"
#include "public/manager/rcsManager.h"
#include "rcsNumberParser.h"
#include "rcsSynPvrsConvertor.h"

bool rcsSynPvrsConvertor::isPossibleNoParaKeyword(rcsSynSpecificationNode_T *pNode)
{
    switch(pNode->getSpecificationType())
    {
        case SOURCE_PRIMARY:
        case PEX_REDUCE_RONLY:
            return true;
        default:
            return false;
    }
}

void rcsSynPvrsConvertor::beginVisitSpecificationNode(rcsSynSpecificationNode_T *pNode)
{
    if(!m_hasGetTmpLayerValue)
        return;

    outputBlankLinesBefore(pNode->getNodeBeginLineNo());

    std::string sValue;
    pNode->getCmdValue(sValue);
    if(rcsManager_T::getInstance()->isSvrfComment())
    {
       (*m_oPVRSStream) << "// ";
       (*m_oPVRSStream) << sValue << std::endl;
    }

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if(!isPossibleNoParaKeyword(pNode) && iter == pNode->end())
    {
        --iter;
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2,
                            pNode->getNodeBeginLineNo(), iter->sValue);
    }

    





    switch(pNode->getSpecificationType())
    {
        case LAYOUT_PATH:
        case LAYOUT_PATH2:
        case LAYOUT_PRIMARY:
        case LAYOUT_PRIMARY2:
        case LAYOUT_SYSTEM:
        case LAYOUT_SYSTEM2:
            convLayoutInputSpec(pNode);
            break;
        case LAYOUT_PATH_WIDTH_MULTIPLE:
        {
            convLayoutPathWidthMultiple(pNode);
            break;
        }
        case DRC_MAXIMUM_RESULTS:
        case DRC_MAXIMUM_VERTEX:
        case ERC_MAXIMUM_RESULTS:
        case ERC_MAXIMUM_VERTEX:
        case DRC_TOLERANCE_FACTOR:
        case DRC_TOLERANCE_FACTOR_NAR:
        case LVS_SHOW_SEED_PROMOTIONS_MAXIMUM:
        case LVS_REPORT_MAXIMUM:
        case LVS_MAXIMUM_ISOLATE_SHORTS:
        case LVS_MAXIMUM_ISOLATE_OPENS:
        case LVS_MAXIMUM_NETS_PER_ISOLATE_OPEN:
        case TEXT_PRINT_MAXIMUM:
        case LAYOUT_BUMP2:
        case DRC_MAXIMUM_UNATTACHED_LABEL_WARNINGS:
        case DRC_MAXIMUM_CELL_NAME_LENGTH:
        case DRC_MAXIMUM_RESULTS_NAR:
            convMaxLimitSpec(pNode);
            break;
        case DRC_RESULTS_DATABASE:
        case ERC_RESULTS_DATABASE:
            convResultDBSpec(pNode);
            break;
        case LVS_REPORT:
        case LVS_HEAP_DIRECTORY:
        case LAYER_DIRECTORY:
            convNameSpec(pNode);
            break;
        case PRECISION:
        case LAYOUT_PRECISION:
            convPrecSpec(pNode);
            break;
        case RESOLUTION:
            convResolutionSpec(pNode);
            break;
        case DRC_SUMMARY_REPORT:
        case ERC_SUMMARY_REPORT:
            convSumFileSpec(pNode);
            break;
        case LAYER:
            convAssignLayerSpec(pNode);
            break;
        case DRC_BOOLEAN_NOSNAP45:
        case DRC_CELL_TEXT:
        case DRC_EXCLUDE_FALSE_NOTCH:
        case DRC_INCREMENTAL_CONNECT:
        case DRC_INCREMENTAL_CONNECT_WARNING:
        case DRC_ICSTATION:
        case DRC_KEEP_EMPTY:
        case ERC_KEEP_EMPTY:
        case DRC_MAP_TEXT:
        case VIRTUAL_CONNECT_BOX_COLON:
        case VIRTUAL_CONNECT_COLON:
        case VIRTUAL_CONNECT_INCREMENTAL:
        case VIRTUAL_CONNECT_REPORT:
        case VIRTUAL_CONNECT_SEMICOLON_AS_COLON:
        case LAYOUT_PRESERVE_CASE:
        case LAYOUT_PRESERVE_NET_CASE:
        case LAYOUT_PRESERVE_PROPERTY_CASE:
        case LAYOUT_PROCESS_BOX_RECORD:
        case LAYOUT_PROCESS_NODE_RECORD:
        case LAYOUT_ALLOW_DUPLICATE_CELL:
        case LVS_ABORT_ON_ERC_ERROR:
        case LVS_ABORT_ON_SOFTCHK:
        case LVS_ABORT_ON_SUPPLY_ERROR:
        case LVS_ALL_CAPACITOR_PINS_SWAPPABLE:
        case LVS_BUILTIN_DEVICE_PIN_SWAP:
        case LVS_CELL_SUPPLY:
        case LVS_CHECK_PORT_NAMES:
        case LVS_DISCARD_PINS_BY_DEVICE:
        case LVS_DOWNCASE_DEVICE:
        case LVS_EXACT_SUBTYPES:
        case LVS_EXECUTE_ERC:
        case LVS_EXPAND_SEED_PROMOTIONS:
        case LVS_EXPAND_UNBALANCED_CELLS:
        case LVS_FILTER_UNUSED_BIPOLAR:
        case LVS_FILTER_UNUSED_CAPACITORS:
        case LVS_FILTER_UNUSED_DIODES:
        case LVS_FILTER_UNUSED_MOS:
        case LVS_FILTER_UNUSED_RESISTORS:
        case LVS_GLOBALS_ARE_PORTS:
        case LVS_IGNORE_PORTS:
        case LVS_IGNORE_TRIVIAL_NAMED_PORTS:
        case LVS_INJECT_LOGIC:

        case LVS_NETLIST_ALL_TEXTED_PINS:
        case LVS_NETLIST_ALLOW_INCONSISTENT_MODEL:
        case LVS_NETLIST_BOX_CONTENTS:
        case LVS_NETLIST_COMMENT_CODED_SUBSTRATE:
        case LVS_NETLIST_COMMENT_CODED_PROPERTIES:
        case LVS_NETLIST_UNNAMED_BOX_PINS:
        case LVS_OUT_OF_RANGE_EXCLUDE_ZERO:
        case LVS_PRESERVE_BOX_CELLS:
        case LVS_PRESERVE_BOX_PORTS:
        case LVS_PRESERVE_FLOATING_TOP_NETS:
        case LVS_PRESERVE_PARAMETERIZED_CELLS:
        case LVS_REPORT_UNITS:
        case LVS_REPORT_WARNINGS_HCELL_ONLY:
        case LVS_REPORT_WARNINGS_TOP_ONLY:
        case LVS_REVERSE_WL:
        case LVS_SHOW_SEED_PROMOTIONS:
        case LVS_SOFT_SUBSTRATE_PINS:
        case LVS_SPICE_ALLOW_DUPLICATE_SUBCIRCUIT_NAMES:
        case LVS_SPICE_ALLOW_FLOATING_PINS:
        case LVS_SPICE_ALLOW_INLINE_PARAMETERS:
        case LVS_SPICE_ALLOW_UNQUOTED_STRINGS:
        case LVS_SPICE_CONDITIONAL_LDD:
        case LVS_SPICE_CULL_PRIMITIVE_SUBCIRCUITS:
        case LVS_SPICE_IMPLIED_MOS_AREA:
        case LVS_SPICE_OVERRIDE_GLOBALS:
        case LVS_SPICE_PREFER_PINS:
        case LVS_SPICE_REDEFINE_PARAM:
        case LVS_SPICE_REPLICATE_DEVICES:
        case LVS_SPICE_SLASH_IS_SPACE:
        case LVS_SPICE_STRICT_WL:
        case LVS_STRICT_SUBTYPES:
        case LVS_PRECISE_INTERACTION:
        case SOURCE_CASE:
        case LAYOUT_CASE:
        case LAYOUT_WINDOW_CLIP:
        case LAYOUT_USE_DATABASE_PRECISION:
        case LAYOUT_ERROR_ON_INPUT:
        case SNAP_OFFGRID:
        case FLATTEN_VERY_SMALL_CELLS:
        case LVS_NL_PIN_LOCATIONS:
            convYesNoSpec(pNode);
            break;
        case LVS_ISOLATE_OPENS:
            convCustomizedSpec(pNode);
            break;
        case SOURCE_PATH:
        case SOURCE_PRIMARY:
        case SOURCE_SYSTEM:
            convSourceDbSpec(pNode);
            break;
        case TEXT_LAYER:
        case PORT_LAYER_POLYGON:
        case PORT_LAYER_TEXT:
        case EXCLUDE_ACUTE:
        case EXCLUDE_ANGLED:
        case EXCLUDE_OFFGRID:
        case EXCLUDE_SKEW:
        case LAYOUT_TOP_LAYER:
        case LAYOUT_BASE_LAYER:
        case DRC_ERROR_WAIVE_LAYER:
        case LAYOUT_WINDEL_LAYER:
        case LAYOUT_WINDOW_LAYER:
        case DRC_PRINT_AREA:
        case DRC_PRINT_PERIMETER:
            convLayerListSpec(pNode);
            break;
        case DEVICE:
            convDeviceSpec(pNode);
            break;
        case DRC_CELL_NAME:
        case ERC_CELL_NAME:
            convCellNameSpec(pNode);
            break;
        case DRC_SELECT_CHECK:
        case DRC_UNSELECT_CHECK:
        case DRC_SELECT_CHECK_BY_LAYER:
        case DRC_UNSELECT_CHECK_BY_LAYER:
        case ERC_SELECT_CHECK:
        case ERC_SELECT_CHECK_ABORT_LVS:
        case ERC_UNSELECT_CHECK:
        case DFM_SELECT_CHECK:
        case DFM_UNSELECT_CHECK:
            convRuleSelectSpec(pNode);
            break;
        case GROUP:
            convMakeRuleGroupSpec(pNode);
            break;
        case DRC_CHECK_MAP:
            convRuleMapSpec(pNode);
            break;
        case DRC_CHECK_TEXT:
        case ERC_CHECK_TEXT:
            convRuleTextSpec(pNode);
            break;
        case LAYER_MAP:
            convLayerMapSpec(pNode);
            break;
        case LVS_COMPARE_CASE:
            convCompareCaseSpec(pNode);
            break;
        case LVS_WRITE_INJECTED_LAYOUT_NETLIST:
        case LVS_WRITE_INJECTED_SOURCE_NETLIST:
        case LVS_WRITE_LAYOUT_NETLIST:
        case LVS_WRITE_SOURCE_NETLIST:
            convNetListOutputSpec(pNode);
            break;
        case TRACE_PROPERTY:
            convCheckPropertySpec(pNode);
            break;
        case LVS_REDUCE_PARALLEL_BIPOLAR:
        case LVS_REDUCE_PARALLEL_CAPACITORS:
        case LVS_REDUCE_PARALLEL_DIODES:
        case LVS_REDUCE_PARALLEL_MOS:
        case LVS_REDUCE_PARALLEL_RESISTORS:
        case LVS_REDUCE_SEMI_SERIES_MOS:
        case LVS_REDUCE_SERIES_CAPACITORS:
        case LVS_REDUCE_SERIES_MOS:
        case LVS_REDUCE_SERIES_RESISTORS:
            convReduceSpecificSpec(pNode);
            break;
        case LVS_REDUCE:
            convReduceGenericSpec(pNode);
            break;
        case DRC_MAP_TEXT_DEPTH:
        case LAYOUT_DEPTH:
        case TEXT_DEPTH:
        case PORT_DEPTH:
        case VIRTUAL_CONNECT_DEPTH:
            convDepthSpec(pNode);
            break;
        case LAYOUT_PLACE_CELL:
            convUserDefDbSpec(pNode);
            break;
        case VARIABLE:
            convVarSpec(pNode);
            break;
        case POLYGON:
            convPolygonToLayerSpec(pNode);
            break;
        case TEXT:
            convTextToLayerSpec(pNode);
            break;
        case LVS_FILTER:
            convFilterSpec(pNode);
            break;
        case LVS_FILTER_UNUSED_OPTION:
            convFilterUnusedOptionSpec(pNode);
            break;
        case LVS_MAP_DEVICE:
            convMapDeviceSpec(pNode);
            break;
        case LVS_REDUCTION_PRIORITY:
            convReducePreferSpec(pNode);
            break;
        case LVS_PROPERTY_INITIALIZE:
            convInitialPropertySpec(pNode);
            break;
        case LVS_PROPERTY_MAP:
            convMapPropertySpec(pNode);
            break;
        case LVS_PUSH_DEVICES:
        case LVS_PUSH_DEVICES_SEPARATE_PROPERTIES:
            convPushDeviceSpec(pNode);
            break;
        case LVS_DEVICE_TYPE:
            convMapDeviceTypeSpec(pNode);
            break;
        case LVS_REDUCE_SPLIT_GATES:
            convReduceSplitGatesSpec(pNode);
            break;
        case LVS_SHORT_EQUIVALENT_NODES:
            convShortEquivalentNodesSpec(pNode);
            break;
        case HCELL:
            convCorrespondingCellSpec(pNode);
            break;
        case MASK_SVDB_DIRECTORY:
            convOutputLvsDBSpec(pNode);
            break;
        case DRC_MAGNIFY_RESULTS:
        case DRC_MAGNIFY_DENSITY:
        case DRC_MAGNIFY_NAR:
            convOutputMagSpec(pNode);
            break;
        case DRC_RESULTS_DATABASE_LIBNAME:
            convDrcOutputLibSpec(pNode);
            break;
        case DRC_RESULTS_DATABASE_PRECISION:
            convDrcOutPrecisionSpec(pNode);
            break;
        case LVS_ISOLATE_SHORTS:
            convFindShortsSpec(pNode);
            break;
        case ERC_PATHCHK:
            convErcPathchkSpec(pNode);
            break;
        case ERC_PATH_ALSO:
            convDeviceOnPathSpec(pNode);
            break;
        case LAYOUT_PRESERVE_CELL_LIST:
        case DRC_ERROR_WAIVE_CELL:
        case EXCLUDE_CELL:
        case PUSH_CELL:
        case EXPAND_CELL:
        case FLATTEN_CELL:
        case FLATTEN_INSIDE_CELL:
        case LAYOUT_BASE_CELL:
        case LAYOUT_WINDEL_CELL:
        case LAYOUT_WINDOW_CELL:
            convCellListSpec(pNode);
            break;
        case TITLE:
            convFileHeadNameSpec(pNode);
            break;
        case LAYOUT_POLYGON:
            convPolygonToDBSpec(pNode);
            break;
        case LAYOUT_TEXT:
            convTextToDBSpec(pNode);
            break;
        case LAYOUT_TEXT_FILE:
            convLayoutTextFileSpec(pNode);
            break;
        case LAYOUT_CELL_LIST:
        case LVS_CELL_LIST:
            convCellListDefSpec(pNode);
            break;
        case LAYOUT_INPUT_EXCEPTION_RDB:
            convExceptionDBSpec(pNode);
            break;
        case LAYOUT_INPUT_EXCEPTION_SEVERITY:
            convExceptionSeveritySpec(pNode);
            break;
        case LAYOUT_WINDEL:
        case LAYOUT_WINDOW:
            convCoordListSpec(pNode);
            break;
        case LAYOUT_MAGNIFY:
            convDBMagnifySpec(pNode);
            break;
        case LAYOUT_MERGE_ON_INPUT:
            convertDBMergeOnInput(pNode);
            break;
        case LAYOUT_RENAME_TEXT:
        case LAYOUT_IGNORE_TEXT:
            convRenameTextSpec(pNode);
            break;
        case LAYER_RESOLUTION:
            convLayerResolutionSpec(pNode);
            break;
        case LAYOUT_RENAME_CELL:
            convRenameCellSpec(pNode);
            break;
        case LVS_BOX:
            convBlackBoxSpec(pNode);
            break;
        case LVS_BLACK_BOX_PORT:
            convLvsBlackBoxPortSpec(pNode);
            break;
        case LVS_BLACK_BOX_PORT_DEPTH:
            convLvsBlackBoxPortDepthSpec(pNode);
            break;
        case LVS_BOX_CUT_EXTENT:
        	convLvsBoxCutExtentSpec(pNode);
            break;
        case LVS_BOX_PEEK_LAYER:
        	convLvsBoxPeekLayerSpec(pNode);
            break;
        case LVS_PROPERTY_RESOLUTION_MAXIMUM:
            convPropResolutionMaxSpec(pNode);
            break;
        case LVS_CPOINT:
            convInitialNetSpec(pNode);
            break;
        case VIRTUAL_CONNECT_BOX_NAME:
        case VIRTUAL_CONNECT_NAME:
        case LVS_EXCLUDE_HCELL:
        case LVS_GROUND_NAME:
        case LVS_POWER_NAME:
            convNameListSpec(pNode);
            break;
        case LVS_RECOGNIZE_GATES:
            convIdentifyGatesSpec(pNode);
            break;
        case LVS_FLATTEN_INSIDE_CELL:
            convLVSFlattenInsideCell(pNode);
            break;
        case LVS_GLOBAL_LAYOUT_NAME:
            convGroupGlobalNameSpec(pNode);
            break;
        case LVS_NON_USER_NAME_INSTANCE:
        case LVS_NON_USER_NAME_NET:
        case LVS_NON_USER_NAME_PORT:
            convNonMatchNameSpec(pNode);
            break;
        case LVS_REPORT_OPTION:
            convLvsReportOptionSpec(pNode);
            break;
        case FLAG_ACUTE:
        case FLAG_ANGLED:
        case FLAG_NONSIMPLE:
        case FLAG_NONSIMPLE_PATH:
        case FLAG_OFFGRID:
        case FLAG_SKEW:
            convCheckSpec(pNode);
            break;
        case SVRF_MESSAGE:
            convPutsSpec(pNode);
            break;
        case EXPAND_CELL_TEXT:
            convExpandTextInCellSpec(pNode);
            break;
        case SVRF_ERROR:
            convErrorSpec(pNode);
            break;
        case LVS_SPICE_OPTION:
        case LVS_SPICE_SCALE_X_PARAMETERS:
        case LVS_SPICE_MULTIPLIER_NAME:
            convSpiceOptionSpec(pNode);
            break;
        case LVS_SPICE_RENAME_PARAMETER:
            convRenameSpiceParameterSpec(pNode);
            break;
        case LVS_SOFTCHK:
            convSconnectCheckSpec(pNode);
            break;
        case LVS_SOFTCHK_MAXIMUM_RESULTS:
        case VIRTUAL_CONNECT_REPORT_MAXIMUM:
            convSoftChkMaxResSpec(pNode);
            break;
        case UNIT_CAPACITANCE:
        case UNIT_RESISTANCE:
        case UNIT_INDUCTANCE:
        case UNIT_LENGTH:
        case UNIT_TIME:
            convUnitOptionSpec(pNode);
            break;
        case SVRF_VERSION:
        case LVS_SIGNATURE_MAXIMUM:
        case MASK_RESULTS_DATABASE:

            break;
        case LVS_ISOLATE_SHORTS_IGNORE_NETS:
        case LVS_ISOLATE_OPENS_NETS:
        case LVS_ISOLATE_OPENS_IGNORE_NETS:
            convOldIsolateNetSpec(pNode);
            break;
        case DEVICE_GROUP:
            covDeviceGroupSpec(pNode);
            break;
        case TVF_FUNCTION:
            convTvfFuncSpec(pNode);
            break;
        case LVS_SPLIT_GATE_RATIO:
            convLvsSplitGateRatio(pNode);
            break;
        case DRC_MAP_TEXT_LAYER:
            convDrcMapTextLayer(pNode);
            break;
        case DFM_FUNCTION:
            convDfmFunction(pNode);
            break;
        case LAYOUT_CLONE_ROTATED_PLACEMENTS:
            convLayoutCloneRotatedPlacements(pNode);
            break;
        case LAYOUT_CLONE_TRANSFORMED_PLACEMENTS:
            convLayoutCloneTransformedPlacements(pNode);
            break;
        case LVS_NETLIST_PROPERTY:
            convLvsNetlistProperty(pNode);
            break;
        case LVS_AUTO_EXPAND_HCELL:
            convLvsAutoExpandHcell(pNode);
            break;
        case DFM_DEFAULTS_RDB:
            convDFMDefaultsRDB(pNode);
            break;
        case ERC_TVF:
        case DFM_SPEC_FILL_SHAPE:
        case DFM_SPEC_SPACE:
        case DFM_SPEC_FILL_GUIDE:
        case DFM_SPEC_FILL_OPTIMIZER:
        case DFM_SPEC_FILL_REGION:
        case DFM_SPEC_FILL:
        case DFM_SPEC_FILL_WRAP:
        case DFM_DEFAULTS_FILL:
        case LVS_PATHCHK_MAXIMUM_RESULTS:
            convCommonNode(pNode->getSpecificationType(), pNode);
            break;
        case LITHO_FILE:
            parseLithoFileSpec(pNode);
            break;
        case PORT_LAYER_MERGED_POLYGON:
        case LVS_NETLIST_CONNECT_PORT_NAMES:
        case LAYOUT_PROPERTY_TEXT:
        case DFM_SELECT_CHECK_NODAL:
        case DFM_SUMMARY_REPORT:
        case LVS_RECOGNIZE_GATES_TOLERANCE:
        case LAYOUT_PROPERTY_AUDIT:

        case DFM_DATABASE:
        case DFM_DATABASE_DIRECTORY:
        case DFM_SPEC_SPATIAL_SAMPLE:
        case DFM_SPEC_VIA_REDUNDANCY:
        case DFM_SPEC_VIA:
        case DFM_SPEC_RANDOM_SPOT_FAULT:
        case DFM_ASSERT:
        case DFM_YS_AUTOSTART:
        case LVS_PIN_NAME_PROPERTY:
        case LVS_COMPONENT_SUBTYPE_PROPERTY:
        case LVS_COMPONENT_TYPE_PROPERTY:
        case LVS_CENTER_DEVICE_PINS:
        case LVS_MOS_SWAPPABLE_PROPERTIES:
        case LVS_ANNOTATE_DEVICES:
        case LVS_CONFIGURE_UNUSED_FILTER:
        case LVS_APPLY_UNUSED_FILTER:
        case DFM_DEFAULTS:
        case LAYOUT_PROPERTY_TEXT_OASIS:
        case PEX_CMD:
        case CAPACITANCE_CMD:
        case RESISTANCE_CMD:
        case DRC_PRINT_EXTENT:
        case NON_SUPPORT_SPECIFICATION:
        case LAYOUT_ULTRA_FLEX:
        //add lvs_netlist_unnamed_isolated_nets support
        case LVS_NETLIST_UNNAMED_ISOLATED_NETS:
        default:
        {
            std::string sValue;
            pNode->getCmdValue(sValue);

            m_oLogStream << "This Operation or Specification in line " << pNode->begin()->nLineNo
                         << " can not be Converted correctly: " << std::endl << sValue << std::endl << std::endl;

            s_errManager.addError(rcErrorNode_T(rcErrorNode_T::WARNING, CON1,
                                  pNode->getNodeBeginLineNo(), pNode->begin()->sValue));
            break;
        }
    }

#if 0
    if(!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
#endif
}

void
rcsSynPvrsConvertor::beginVisitProcNode(rcsSynProcNode_T *pNode)
{
    if(!m_hasGetTmpLayerValue)
        return;

    outputBlankLinesBefore(pNode->getNodeBeginLineNo());

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    if (iter->eType == DMACRO)
    {
        ++iter;
        std::string sMacroName;
        getStringName(iter, pNode->end(), sMacroName);
        ++iter;
        (*m_oPVRSStream) << "define_fun " << sMacroName << " ";
        while (iter != pNode->end() && iter->sValue != "{")
        {
            std::string sParaName;
            getStringName(iter, pNode->end(), sParaName);
            (*m_oPVRSStream) << sParaName << " ";
            ++iter;
        }
        (*m_oPVRSStream) << "{" << std::endl;
    }

}

void
rcsSynPvrsConvertor::endVisitProcNode(rcsSynProcNode_T *pNode)
{
    if(!m_hasGetTmpLayerValue)
        return;

    (*m_oPVRSStream) << "}" << std::endl;
}

void
rcsSynPvrsConvertor::beginVisitPreProcessNode(rcsSynPreProcessNode_T *pNode)
{
    if(!m_hasGetTmpLayerValue)
        return;

    outputBlankLinesBefore(pNode->getNodeBeginLineNo());

    std::string sCmdValue;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    if(rcsManager_T::getInstance()->isConvertIncludeFile() && 
        strcasecmp("INCLUDE", iter->sValue.c_str()) == 0 && (++iter)->eType == STRING_CONSTANTS)
    {
        sCmdValue = "include (\"" + iter->sValue + "\")";
    }
    else
    {
        pNode->getCmdValue(sCmdValue);
    }
    (*m_oPVRSStream) << sCmdValue << std::endl;
}


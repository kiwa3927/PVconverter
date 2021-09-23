

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
#include "rcsSynNodeProConvertor.h"

rcsSynNodeProConvertor_T::rcsSynNodeProConvertor_T(): rcsSynNodeConvertor_T(),
                                                      m_inProcReturn(false)
{
}

void
rcsSynNodeProConvertor_T::beginVisitProcNode(rcsSynProcNode_T *pNode)
{
    Utassert(m_iProcLevel == 0 && !m_inProcReturn);

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    if(rcsManager_T::getInstance()->isSvrfComment())
        m_oPVRSStream << "#" << iter->sValue;
    ++iter;
    if(rcsManager_T::getInstance()->isSvrfComment())
        m_oPVRSStream << " " << iter->sValue << std::endl;

    std::string sMacroName;
    getStringName(iter, pNode->end(), sMacroName);
    toLower(sMacroName);
    m_oPVRSStream << "proc " << sMacroName << " { ";
    sMacroName = iter->sValue;
    toLower(sMacroName);

    ++m_iProcLevel;

    if(m_vProcUsedTimes.find(sMacroName) == m_vProcUsedTimes.end())
        m_vProcUsedTimes[sMacroName] = 0;

    ++iter;
    std::set<std::string> vProcInputParas;
    while(iter != pNode->end())
    {
        if(iter->eType == SEPARATOR && iter->sValue == "{")
        {
            const std::vector<rcsSynNode_T *> &vChilds = pNode->getChildNodes();
            m_oPVRSStream << "index } {\n";
            for(std::vector<rcsSynNode_T *>::const_iterator iNode = vChilds.begin();
                iNode != vChilds.end(); ++iNode)
            {
                if((*iNode)->isDefinitionNode())
                {
                    std::string sLayerName;
                    rcsSynLayerDefinitionNode_T *pData =
                            static_cast<rcsSynLayerDefinitionNode_T*>(const_cast<rcsSynNode_T*>(*iNode));

                    std::list<rcsToken_T>::iterator iLayerName = pData->begin();
                    sLayerName = iLayerName->sValue;
                    if(sLayerName[0] == '$')
                        sLayerName.erase(0, 1);
                    std::string sName = sLayerName;
                    toLower(sName);
                    if(vProcInputParas.find(sName) != vProcInputParas.end())
                        continue;

                    m_oPVRSStream << "set {" << sLayerName << "} [format " << "{\""
                                  << sMacroName << "_%d_" << sLayerName<< "\"} "
                                  << "$index]" << std::endl;
                }
            }
            
            break;
        }
        std::string sValue = iter->sValue;
        toLower(sValue);
        vProcInputParas.insert(sValue);
        m_oPVRSStream << sValue << " ";
        ++iter;
    }
}

void
rcsSynNodeProConvertor_T::endVisitProcNode(rcsSynProcNode_T *pNode)
{
    --m_iProcLevel;
    if(m_inProcReturn)
        m_oPVRSStream << "\"\n}\n";
    else
        m_oPVRSStream << "}\n";

    m_inProcReturn = false;
}

void
rcsSynNodeProConvertor_T::setVariableSet(const std::set<std::string, ltStringCase> &vNameSet)
{
    m_vVariableNames = vNameSet;
}

void
rcsSynNodeProConvertor_T::beginVisitLayerDefinitionNode(rcsSynLayerDefinitionNode_T *pNode)
{
    if(m_iProcLevel > 0 && !m_inProcReturn)
    {
        m_oPVRSStream << "return \"\n" << std::endl;
        m_inProcReturn = true;
    }

    m_isInLayerDefinition = true;
    if(rcsManager_T::getInstance()->isSvrfComment())
    {
        std::string sValue;
        pNode->getCmdValue(sValue);
        m_oPVRSStream << "#" << sValue << "= ";
    }

    if(pNode->getLayerOperationNode()->isCMacro())
    {
        if(rcsManager_T::getInstance()->isSvrfComment())
        {
            std::string sValue;
            pNode->getLayerOperationNode()->getCmdValue(sValue);
            m_oPVRSStream << sValue << std::endl;
        }
        if(m_iProcLevel > 0)
            m_oPVRSStream << "eval \\[concat ";
        else
            m_oPVRSStream << "eval [concat ";
    }
}

void
rcsSynNodeProConvertor_T::endVisitLayerDefinitionNode(rcsSynLayerDefinitionNode_T *pNode)
{
    std::string sLayerName;
    std::list<rcsToken_T>::iterator iter = pNode->begin();

    if(m_iProcLevel)
    {
        getLayerRefName(iter, pNode->end(), sLayerName);
        m_oPVRSStream << "-out_layer " << sLayerName;
    }
    else
    {
        getLayerRefName(iter, pNode->end(), sLayerName);
        m_oPVRSStream << "-out_layer " << sLayerName;
    }

    if(pNode->getLayerOperationNode()->isCMacro())
    {
        if(m_iProcLevel > 0)
            m_oPVRSStream << "\\]";
        else
            m_oPVRSStream << "]";
    }

    m_oPVRSStream << std::endl << std::endl;
    m_isInLayerDefinition = false;
}

static bool
_isBlankLine(const std::string &text)
{
    std::string s = text;
    trim(s);
    return s.empty();
}

static void
_getCheckText(const hvUInt32 nBegin, const hvUInt32 nEnd,
              std::string &sCheckText, std::string &sCheckComment)
{
    const std::map<hvUInt32, std::string> &vFileText =
            rcsManager_T::getInstance()->getFileText();
    const std::map<hvUInt32, std::string> &vCheckComment =
            rcsManager_T::getInstance()->getCheckComment();
    std::map<hvUInt32, std::string>::const_iterator iter = vFileText.lower_bound(nBegin);
    for(; iter != vFileText.end(); ++iter)
    {
        if(iter->first > nEnd)
            break;

        
        if(_isBlankLine(iter->second))
            continue;

        sCheckText += iter->second;
        std::map<hvUInt32, std::string>::const_iterator iComment =
                vCheckComment.find(iter->first);
        if(iComment != vCheckComment.end())
        {
            std::string sResult;
            if(!_replaceComment(iComment->second, sResult))
            {
                rcErrManager_T::Singleton().addError
                        (rcErrorNode_T(rcErrorNode_T::ERROR, RES1, iter->first, sResult));
                sCheckComment += iter->second;
            }
            else
            {
                std::string sValue = sResult;
                trim(sResult, "\r ");
                if(!_isBlankLine(sValue))
                    sCheckComment += sValue;

                sCheckText.erase(sCheckText.size() - iComment->second.size());
                sCheckText += sResult;
            }
        }
    }
}

void
rcsSynNodeProConvertor_T::beginVisitCommentNode(rcsSynCommentNode_T *pNode)
{
    Utassert(rcsManager_T::getInstance()->outputComment());
    if(rcsManager_T::getInstance()->outputComment())
    {
        std::string sValue;
        _replaceComment(pNode->getComment(), sValue, false);
        hvUInt32 iValue = 0;
        while(iValue < sValue.size())
        {
            if(sValue[iValue] == '{' || sValue[iValue] == '}' || sValue[iValue] == '$')
            {
                sValue.insert(iValue, "\\");
                ++iValue;
            }
            ++iValue;
        }
        m_oPVRSStream << "#### " << sValue << std::endl;
    }
}

void
rcsSynNodeProConvertor_T::beginVisitRuleCheckNode(rcsSynRuleCheckNode_T *pNode)
{
    Utassert(!m_inProcReturn);

    std::string sCheckText;
    std::string sCheckComment;
    _getCheckText(pNode->getNodeBeginLineNo(), pNode->getEndLineNo(),
                  sCheckText, sCheckComment);

    std::string sCheckName;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    getStringName(iter, pNode->end(), sCheckName);

    if(rcsManager_T::getInstance()->isSvrfComment())
    {
        m_oPVRSStream << "#";
        m_oPVRSStream << pNode->getRuleCheckName() << std::endl;
    }

    m_oPVRSStream << m_sPVRSCmdPrefix << "check_rule -name " << sCheckName << " ";
    if(!sCheckComment.empty())
    {
        m_oPVRSStream << "\\\n-comment {";
        std::string::size_type iValue = 0;
        for(; iValue != sCheckComment.size(); ++iValue)
        {
            if((sCheckComment[iValue] == '\n' || sCheckComment[iValue] == '\r'))
            {
                if(iValue != sCheckComment.size() - 1)
                {
                    m_oPVRSStream << "}\\\n-comment {";
                }
            }
            else
            {
                m_oPVRSStream << sCheckComment[iValue];
            }
        }
        m_oPVRSStream << "}";
    }

    m_oPVRSStream << "\\\n-exec {\n\n";
}

void
rcsSynNodeProConvertor_T::endVisitRuleCheckNode(rcsSynRuleCheckNode_T *pNode)
{
    m_oPVRSStream << "}\n\n";
}

void rcsSynNodeProConvertor_T::beginVisitSpecificationNode(rcsSynSpecificationNode_T *pNode)
{
    if(m_iProcLevel > 0 && !m_inProcReturn)
    {
        m_oPVRSStream << "return \"\n" << std::endl;
        m_inProcReturn = true;
    }

    std::string sValue;
    pNode->getCmdValue(sValue);
    if(rcsManager_T::getInstance()->isSvrfComment())
    {
       m_oPVRSStream << "#";
       m_oPVRSStream << sValue << std::endl;
    }

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if(pNode->getSpecificationType() != SOURCE_PRIMARY && iter == pNode->end())
    {
        --iter;
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2,
                            pNode->getNodeBeginLineNo(), iter->sValue);
    }

    switch(pNode->getSpecificationType())
    {
        case LAYER:
            convertAssignLayerSpec(pNode);
            break;
        case PRECISION:
            convertPrecSpec(pNode);
            break;
        case VARIABLE:
            break;
        case GROUP:
            convertMakeRuleGroupSpec(pNode);
            break;
        case DRC_CELL_NAME:
        case ERC_CELL_NAME:
            convertCellNameSpec(pNode);
            break;
            break;
        case DRC_CELL_TEXT:
            convertYesNoSpec(pNode, "drc_cell_text");
            break;
        case DRC_CHECK_TEXT:
        case ERC_CHECK_TEXT:
            convertRuleTextSpec(pNode);
            break;
        case DRC_EXCLUDE_FALSE_NOTCH:
            convertYesNoSpec(pNode, "drc_exclude_false_error");
            break;
        case DRC_INCREMENTAL_CONNECT:
            convertYesNoSpec(pNode, "incremental_connect");
            break;
        case DRC_INCREMENTAL_CONNECT_WARNING:
            convertYesNoSpec(pNode, "incremental_connect_warning");
            break;
        case DRC_ICSTATION:
            convertYesNoSpec(pNode, "ignore_following_duplicate_commands");
            break;
        case DRC_KEEP_EMPTY:
            convertYesNoSpec(pNode, "drc_output_option -keep_empty_rule");
            break;
        case ERC_KEEP_EMPTY:
            convertYesNoSpec(pNode, "erc_output_option -keep_empty_rule");
            break;
        case DRC_MAGNIFY_RESULTS:
        case DRC_MAGNIFY_DENSITY:
        case DRC_MAGNIFY_NAR:
            convertOutputMagSpec(pNode);
            break;
        case DRC_MAP_TEXT:
            convertYesNoSpec(pNode, "drc_output_option -map_text");
            break;
        case DRC_MAP_TEXT_DEPTH:
            convertDepthSpec(pNode, "drc_output_option -map_text_depth");
            break;
        case DRC_MAXIMUM_CELL_NAME_LENGTH:
            convertMaxLimitSpec(pNode, "drc_output_option -max_cell_name_length");
            break;
        case DRC_MAXIMUM_UNATTACHED_LABEL_WARNINGS:
            convertMaxLimitSpec(pNode, "max_unattached_label");
            break;
        case DRC_MAXIMUM_RESULTS:
            convertMaxLimitSpec(pNode, "drc_output_option -max_results", true);
            break;
        case DRC_MAXIMUM_VERTEX:
            convertMaxLimitSpec(pNode, "drc_output_option -max_vertexs", true);
            break;
        case ERC_MAXIMUM_RESULTS:
            convertMaxLimitSpec(pNode, "erc_output_option -max_results", true);
            break;
        case ERC_MAXIMUM_VERTEX:
            convertMaxLimitSpec(pNode, "erc_output_option -max_vertexs", true);
            break;
        case DRC_RESULTS_DATABASE:
        case ERC_RESULTS_DATABASE:
            convertDatabaseSpec(pNode);
            break;
        case DRC_RESULTS_DATABASE_LIBNAME:
            convertDrcOutputLibSpec(pNode);
            break;
        case DRC_RESULTS_DATABASE_PRECISION:
            convertDrcOutPrecisionSpec(pNode);
            break;
        case DRC_SUMMARY_REPORT:
        case ERC_SUMMARY_REPORT:
            convertSumFileSpec(pNode);
            break;
        case DRC_TOLERANCE_FACTOR:
            convertMaxLimitSpec(pNode, "drc_tolerance");
            break;
        case DRC_TOLERANCE_FACTOR_NAR:
            convertMaxLimitSpec(pNode, "net_ratio_tolerance");
            break;
        case DRC_SELECT_CHECK:
            convertRuleSelectSpec(pNode, "select_exec_drc_rule -name");
            break;
        case DRC_UNSELECT_CHECK:
            convertRuleSelectSpec(pNode, "unselect_exec_drc_rule -name");
            break;
        case DRC_SELECT_CHECK_BY_LAYER:
            convertRuleSelectSpec(pNode, "select_exec_drc_rule -layer");
            break;
        case DRC_UNSELECT_CHECK_BY_LAYER:
            convertRuleSelectSpec(pNode, "unselect_exec_drc_rule -layer");
            break;
        case ERC_SELECT_CHECK:
            convertRuleSelectSpec(pNode, "select_exec_erc_rule -name");
            break;
        case ERC_SELECT_CHECK_ABORT_LVS:
            convertRuleSelectSpec(pNode, "select_exec_erc_rule -abort_lvs -name");
            break;
        case ERC_UNSELECT_CHECK:
            convertRuleSelectSpec(pNode, "unselect_exec_erc_rule -name");
            break;
        case EXCLUDE_ACUTE:
            convertLayerListSpec(pNode, "original_check_ignore -acute");
            break;
        case EXCLUDE_ANGLED:
            convertLayerListSpec(pNode, "original_check_ignore -not45_angle");
            break;
        case EXCLUDE_OFFGRID:
            convertLayerListSpec(pNode, "original_check_ignore -offgrid");
            break;
        case EXCLUDE_SKEW:
            convertLayerListSpec(pNode, "original_check_ignore -not45_edge");
            break;
        case LAYOUT_TOP_LAYER:
            convertLayerListSpec(pNode, "db_in_option -top_layer_list");
            break;
        case LAYOUT_BASE_LAYER:
            convertLayerListSpec(pNode, "db_in_option -base_layer_list");
            break;
        case LAYOUT_PRESERVE_CELL_LIST:
            convertCellListSpec(pNode, "db_in_option -preserve_cell_list");
            break;
        case LAYER_RESOLUTION:
            convertLayerResolutionSpec(pNode);
            break;
        case DRC_ERROR_WAIVE_LAYER:
            convertLayerListSpec(pNode, "drc_output_option -waive_layer_list");
            break;
        case LAYOUT_WINDEL_LAYER:
            convertLayerListSpec(pNode, "db_in_option -exclude_window_layer_list");
            break;
        case LAYOUT_WINDOW_LAYER:
            convertLayerListSpec(pNode, "db_in_option -select_window_layer_list");
            break;
        case DRC_PRINT_AREA:
            convertLayerListSpec(pNode, "drc_print_layer -info area -print_layer_list");
            break;
        case DRC_PRINT_PERIMETER:
            convertLayerListSpec(pNode, "drc_print_layer -info perimeter -print_layer_list");
            break;
        case TEXT:
            convertTextToLayerSpec(pNode);
            break;
        case LAYOUT_TEXT:
            convertTextToDBSpec(pNode);
            break;
        case LAYOUT_POLYGON:
            convertPolygonToDBSpec(pNode);
            break;
        case POLYGON:
            convertPolygonToLayerSpec(pNode);
            break;
        case LAYOUT_RENAME_CELL:
            convertRenameCellSpec(pNode);
            break;
        case LAYOUT_RENAME_TEXT:
            convertRenameTextSpec(pNode);
            break;
        case TEXT_LAYER:
            convertLayerListSpec(pNode, "assign_text_layer");
            break;
        case PORT_LAYER_POLYGON:
            convertLayerListSpec(pNode, "assign_port_polygon");
            break;
        case PORT_LAYER_TEXT:
            convertLayerListSpec(pNode, "assign_port_layer");
            break;
        case DRC_MAP_TEXT_LAYER:
            convertMapTextLayerSpec(pNode);
            break;
        case LAYOUT_IGNORE_TEXT:
            convertRenameTextSpec(pNode);
            break;
        case DRC_CHECK_MAP:
            convertRuleMapSpec(pNode);
            break;
        case DRC_ERROR_WAIVE_CELL:
            convertCellListSpec(pNode, "drc_output_option -waive_cell");
            break;
        case EXCLUDE_CELL:
            convertCellListSpec(pNode, "db_in_option -no_load_cell");
            break;
        case PUSH_CELL:
            convertCellListSpec(pNode, "db_in_option -push_cell");
            break;
        case EXPAND_CELL:
            convertCellListSpec(pNode, "db_in_option -expand_cell");
            break;
        case EXPAND_CELL_TEXT:
            convertExpandTextInCellSpec(pNode);
            break;
        case FLAG_ACUTE:
            convertCheckSpec(pNode);
            break;
        case FLAG_ANGLED:
            convertCheckSpec(pNode);
            break;
        case FLAG_NONSIMPLE:
            convertCheckSpec(pNode);
            break;
        case FLAG_NONSIMPLE_PATH:
            convertCheckSpec(pNode);
            break;
        case FLAG_OFFGRID:
            convertCheckSpec(pNode);
            break;
        case FLAG_SKEW:
            convertCheckSpec(pNode);
            break;
        case FLATTEN_CELL:
            convertCellListSpec(pNode, "db_in_option -flatten_cell");
            break;
        case FLATTEN_INSIDE_CELL:
            convertCellListSpec(pNode, "db_in_option -flatten_in_cell");
            break;
        case LAYER_DIRECTORY:
            convertNameSpec(pNode, "layer_dir");
            break;
        case LAYER_MAP:
            convertLayerMapSpec(pNode);
            break;
        case LAYOUT_ALLOW_DUPLICATE_CELL:
            convertYesNoSpec(pNode, "db_in_option -merge_duplicate_cell");
            break;
        case LAYOUT_MERGE_ON_INPUT:
            convertMergeOnReadSpec(pNode);
            break;
        case LAYOUT_BASE_CELL:
            convertCellListSpec(pNode, "db_in_option -base_cell_list");
            break;
        case LAYOUT_BUMP2:
            convertMaxLimitSpec(pNode, "db_in_option -bump");
            break;
        case LAYOUT_CASE:
            convertYesNoSpec(pNode, "db_in_option -keep_layout_netlist_case");
            break;
        case LAYOUT_CELL_LIST:
            convertCellListDefSpec(pNode);
            break;
        case LAYOUT_DEPTH:
            convertDepthSpec(pNode, "db_in_option -depth");
            break;
        case LAYOUT_INPUT_EXCEPTION_SEVERITY:
            convertExceptionSeveritySpec(pNode);
            break;
        case LAYOUT_INPUT_EXCEPTION_RDB:
            convertExceptionDBSpec(pNode);
            break;
        case LAYOUT_ERROR_ON_INPUT:
            convertYesNoSpec(pNode, "db_in_option -input_error_break");
            break;
        case LAYOUT_MAGNIFY:
            convertDBMagnifySpec(pNode);
            break;
        case LAYOUT_PATH:
            convertDBPathSpec(pNode);
            break;
        case LAYOUT_PATH2:
            convertDBPathSpec(pNode);
            break;
        case LAYOUT_PLACE_CELL:
            convertUserDefDbSpec(pNode);
            break;
        case LAYOUT_PRECISION:
            convertPrecSpec(pNode);
            break;
        case LAYOUT_PRIMARY:
            convertNameSpec(pNode, "db_in_option -top_cell");
            break;
        case LAYOUT_PRIMARY2:
            convertNameSpec(pNode, "db_in_option -top_cell2");
            break;
        case LAYOUT_PROCESS_BOX_RECORD:
            convertYesNoSpec(pNode, "db_in_option -read_box_record");
            break;
        case LAYOUT_PROCESS_NODE_RECORD:
            convertYesNoSpec(pNode, "db_in_option -read_node_record");
            break;
        case LAYOUT_PROPERTY_TEXT:
            convertPropTextSpec(pNode);
            break;
        case LAYOUT_SYSTEM:
            convertNameSpec(pNode, "db_in_option -format");
            break;
        case LAYOUT_SYSTEM2:
            convertNameSpec(pNode, "db_in_option -format2");
            break;
        case LAYOUT_USE_DATABASE_PRECISION:
            convertYesNoSpec(pNode, "db_in_option -use_db_prec");
            break;
        case LAYOUT_WINDEL:
            convertCoordListSpec(pNode, "db_in_option -exclude_window");
            break;
        case LAYOUT_WINDOW:
            convertCoordListSpec(pNode, "db_in_option -select_window");
            break;
        case LAYOUT_WINDEL_CELL:
            convertCellListSpec(pNode, "db_in_option -exclude_window_cell");
            break;
        case LAYOUT_WINDOW_CELL:
            convertCellListSpec(pNode, "db_in_option -select_window_cell");
            break;
        case LAYOUT_WINDOW_CLIP:
            convertYesNoSpec(pNode, "db_in_option -clip_window");
            break;
        case SNAP_OFFGRID:
            convertYesNoSpec(pNode, "db_in_option -snap_offgrid");
            break;
        case TEXT_DEPTH:
            convertDepthSpec(pNode, "db_in_option -text_depth");
            break;
        case PORT_DEPTH:
            convertDepthSpec(pNode, "db_in_option -port_depth");
            break;
        case RESOLUTION:
            convertResolutionSpec(pNode, "db_in_option -resolution");
            break;
        case TEXT_PRINT_MAXIMUM:
            convertMaxLimitSpec(pNode, "drc_output_option -max_text_print", true);
            break;
        case VIRTUAL_CONNECT_BOX_COLON:
            convertYesNoSpec(pNode, "net_name_connect_colon_black_box");
            break;
        case VIRTUAL_CONNECT_BOX_NAME:
            convertNameListSpec(pNode, "net_name_connect_black_box");
            break;
        case VIRTUAL_CONNECT_COLON:
            convertYesNoSpec(pNode, "net_name_connect_colon");
            break;
        case VIRTUAL_CONNECT_DEPTH:
            convertDepthSpec(pNode, "net_name_connect_depth");
            break;
        case VIRTUAL_CONNECT_INCREMENTAL:
            convertYesNoSpec(pNode, "incremental_net_name_connect");
            break;
        case VIRTUAL_CONNECT_NAME:
            convertNameListSpec(pNode, "net_name_connect");
            break;
        case VIRTUAL_CONNECT_REPORT:
            convertYesNoSpec(pNode, "net_name_connect_report");
            break;
        case VIRTUAL_CONNECT_SEMICOLON_AS_COLON:
            convertYesNoSpec(pNode, "net_name_connect_semicolon");
            break;
        case ERC_PATH_ALSO:
            convertDeviceOnPathSpec(pNode);
            break;
        case HCELL:
            convertCorrespondingCellSpec(pNode);
            break;
        case LAYOUT_PRESERVE_NET_CASE:
            convertYesNoSpec(pNode, "db_in_option -preserve_net_case");
            break;
        case LAYOUT_PRESERVE_PROPERTY_CASE:
            convertYesNoSpec(pNode, "db_in_option -preserve_property_case");
            break;
        case LVS_ABORT_ON_ERC_ERROR:
            convertYesNoSpec(pNode, "erc_error_abort");
            break;
        case LVS_ABORT_ON_SOFTCHK:
            convertYesNoSpec(pNode, "sconnect_error_abort");
            break;
        case LVS_ABORT_ON_SUPPLY_ERROR:
            convertYesNoSpec(pNode, "supply_error_abort");
            break;
        case LVS_ALL_CAPACITOR_PINS_SWAPPABLE:
            convertYesNoSpec(pNode, "compare_option -cap_pins_swap");
            break;
        case LVS_BOX:
            convertBlackBoxSpec(pNode);
            break;
        case LVS_BUILTIN_DEVICE_PIN_SWAP:
            convertYesNoSpec(pNode, "compare_option -basic_device_pins_swap");
            break;
        case LVS_CELL_LIST:
            convertCellListDefSpec(pNode);
            break;
        case LVS_CELL_SUPPLY:
            convertYesNoSpec(pNode, "compare_option -propagate_supply");
            break;
        case LVS_CHECK_PORT_NAMES:
            convertYesNoSpec(pNode, "compare_option -check_port_names");
            break;
        case LVS_COMPARE_CASE:
            convertCompareCaseSpec(pNode);
            break;
        case LVS_CPOINT:
            convertInitialNetSpec(pNode);
            break;
        case LVS_DISCARD_PINS_BY_DEVICE:
            convertYesNoSpec(pNode, "compare_option -discard_extra_pins");
            break;
        case LVS_DOWNCASE_DEVICE:
            convertYesNoSpec(pNode, "lowcase_device");
            break;
        case LVS_EXACT_SUBTYPES:
            convertYesNoSpec(pNode, "compare_option -exact_subtypes");
            break;
        case LVS_EXCLUDE_HCELL:
            convertNameListSpec(pNode, "exclude_corresponding_cells");
            break;
        case LVS_EXECUTE_ERC:
            convertYesNoSpec(pNode, "execute_erc");
            break;
        case LVS_EXPAND_SEED_PROMOTIONS:
            convertYesNoSpec(pNode, "promote_device");
            break;
        case LVS_EXPAND_UNBALANCED_CELLS:
            convertYesNoSpec(pNode, "compare_option -expand_unbalanced_cells");
            break;
        case LVS_FILTER:
            convertFilterSpec(pNode);
            break;
        case LVS_FILTER_UNUSED_BIPOLAR:
            convertYesNoSpec(pNode, "filter_unused_device -type q");
            break;
        case LVS_FILTER_UNUSED_CAPACITORS:
            convertYesNoSpec(pNode, "filter_unused_device -type c");
            break;
        case LVS_FILTER_UNUSED_DIODES:
            convertYesNoSpec(pNode, "filter_unused_device -type d");
            break;
        case LVS_FILTER_UNUSED_MOS:
            convertYesNoSpec(pNode, "filter_unused_device -type m");
            break;
        case LVS_FILTER_UNUSED_OPTION:
            convertFilterUnusedOptionSpec(pNode);
            break;
        case LVS_FILTER_UNUSED_RESISTORS:
            convertYesNoSpec(pNode, "filter_unused_device -type r");
            break;
        case LVS_GLOBAL_LAYOUT_NAME:
            convertGroupGlobalNameSpec(pNode);
            break;
        case LVS_GLOBALS_ARE_PORTS:
            convertYesNoSpec(pNode, "compare_option -add_globals_as_ports");
            break;
        case LVS_GROUND_NAME:
            convertNameListSpec(pNode, "specify_ground_name");
            break;
        case LVS_HEAP_DIRECTORY:
            convertNameSpec(pNode, "temp_data_dir");
            break;
        case LVS_IGNORE_PORTS:
            convertYesNoSpec(pNode, "compare_option -ignore_top_ports");
            break;
        case LVS_IGNORE_TRIVIAL_NAMED_PORTS:
            convertYesNoSpec(pNode, "compare_option -ignore_trivial_ports");
            break;
        case LVS_INJECT_LOGIC:
            convertYesNoSpec(pNode, "inject_hierarchy");
            break;
        case LVS_ISOLATE_OPENS:
            convertYesNoSpec(pNode, "find_opens");
            break;
        case LVS_ISOLATE_SHORTS:
            convertFindShortsSpec(pNode);
            break;
        case LVS_ISOLATE_SHORTS_IGNORE_NETS:
            convertNameListSpec(pNode, "find_shorts -ignore_nets");
            break;
        case LVS_MAXIMUM_ISOLATE_SHORTS:
            convertMaxLimitSpec(pNode, "find_shorts -max", true);
            break;
        case LVS_MAXIMUM_ISOLATE_OPENS:
            convertMaxLimitSpec(pNode, "find_opens -max", true);
            break;
        case LVS_MAXIMUM_NETS_PER_ISOLATE_OPEN:
            convertMaxLimitSpec(pNode, "find_opens -max_nets_per_open", true);
            break;
        case LVS_ISOLATE_OPENS_NETS:
            convertNameListSpec(pNode, "find_opens -nets");
            break;
        case LVS_ISOLATE_OPENS_IGNORE_NETS:
            convertNameListSpec(pNode, "find_opens -ignore_nets");
            break;
        case LVS_NETLIST_ALL_TEXTED_PINS:
            convertYesNoSpec(pNode, "all_labeled_pins_in_netlist");
            break;
        case LVS_NETLIST_ALLOW_INCONSISTENT_MODEL:
            convertYesNoSpec(pNode, "compare_option -allow_diff_model");
            break;
        case LVS_NETLIST_BOX_CONTENTS:
            convertYesNoSpec(pNode, "black_box_contents_in_netlist");
            break;
        case LVS_NETLIST_COMMENT_CODED_SUBSTRATE:
            convertYesNoSpec(pNode, "substrate_in_netlist_comment");
            break;
        case LVS_NETLIST_COMMENT_CODED_PROPERTIES:
            convertYesNoSpec(pNode, "properties_in_netlist_comment");
            break;
        case LVS_NETLIST_UNNAMED_BOX_PINS:
            convertYesNoSpec(pNode, "unnamed_black_box_pins_in_netlist");
            break;
        case LVS_NON_USER_NAME_INSTANCE:
        case LVS_NON_USER_NAME_NET:
        case LVS_NON_USER_NAME_PORT:
            convertNonMatchNameSpec(pNode);
            break;
        case LVS_OUT_OF_RANGE_EXCLUDE_ZERO:
            convertYesNoSpec(pNode, "compare_option -exclude_zero");
            break;
        case LVS_POWER_NAME:
            convertNameListSpec(pNode, "specify_power_name");
            break;
        case LVS_PRESERVE_BOX_CELLS:
            convertYesNoSpec(pNode, "keep_device_in_black_box");
            break;
        case LVS_PRESERVE_BOX_PORTS:
            convertYesNoSpec(pNode, "compare_option -keep_box_ports");
            break;
        case LVS_PRESERVE_FLOATING_TOP_NETS:
            convertYesNoSpec(pNode, "compare_option -keep_floating_top_nets");
            break;
        case LVS_PRESERVE_PARAMETERIZED_CELLS:
            convertYesNoSpec(pNode, "compare_option -keep_pcells");
            break;
        case LVS_PROPERTY_INITIALIZE:
            convertInitialPropertySpec(pNode);
            break;
        case LVS_PROPERTY_MAP:
            convertMapPropertySpec(pNode);
            break;
        case LVS_PROPERTY_RESOLUTION_MAXIMUM:
            convertPropResolutionMaxSpec(pNode);
            break;
        case LVS_PUSH_DEVICES:
            convertPushDeviceSpec(pNode);
            break;
        case LVS_PUSH_DEVICES_SEPARATE_PROPERTIES:
            convertPushDeviceSpec(pNode);
            break;
        case LVS_REDUCE:
            convertReduceGenericSpec(pNode);
            break;
        case LVS_REDUCE_PARALLEL_BIPOLAR:
            convertReduceSpecificSpec(pNode);
            break;
        case LVS_REDUCE_PARALLEL_CAPACITORS:
            convertReduceSpecificSpec(pNode);
            break;
        case LVS_REDUCE_PARALLEL_DIODES:
            convertReduceSpecificSpec(pNode);
            break;
        case LVS_REDUCE_PARALLEL_MOS:
            convertReduceSpecificSpec(pNode);
            break;
        case LVS_REDUCE_PARALLEL_RESISTORS:
            convertReduceSpecificSpec(pNode);
            break;
        case LVS_REDUCE_SEMI_SERIES_MOS:
            convertReduceSpecificSpec(pNode);
            break;
        case LVS_REDUCE_SERIES_CAPACITORS:
            convertReduceSpecificSpec(pNode);
            break;
        case LVS_REDUCE_SERIES_MOS:
            convertReduceSpecificSpec(pNode);
            break;
        case LVS_REDUCE_SERIES_RESISTORS:
            convertReduceSpecificSpec(pNode);
            break;
        case LVS_REDUCE_SPLIT_GATES:
            convertReduceSplitGatesSpec(pNode);
            break;
        case LVS_SHORT_EQUIVALENT_NODES:
        	convertShortEquivalentNodesSpec(pNode);
            break;
        case LVS_REDUCTION_PRIORITY:
            convertReducePreferSpec(pNode);
            break;
        case LVS_REPORT:
            convertNameSpec(pNode, "lvs_report_option -file_name");
            break;
        case LVS_REPORT_MAXIMUM:
            convertMaxLimitSpec(pNode, "lvs_report_option -max", true);
            break;
        case LVS_REPORT_OPTION:
            convertLvsReportOptionSpec(pNode);
            break;
        case LVS_REPORT_UNITS:
            convertYesNoSpec(pNode, "lvs_report_option -units");
            break;
        case LVS_REPORT_WARNINGS_HCELL_ONLY:
            convertYesNoSpec(pNode, "lvs_report_option -warning_only_in_corresponding_cell");
            break;
        case LVS_REPORT_WARNINGS_TOP_ONLY:
            convertYesNoSpec(pNode, "lvs_report_option -warning_only_in_top_cell");
            break;
        case LVS_REVERSE_WL:
            convertYesNoSpec(pNode, "compare_option -reverse_wl");
            break;
        case LVS_SHOW_SEED_PROMOTIONS:
            convertYesNoSpec(pNode, "output_promoted_device");
            break;
        case LVS_SHOW_SEED_PROMOTIONS_MAXIMUM:
            convertMaxLimitSpec(pNode, "output_promoted_device -max", true);
            break;
        case LVS_SOFT_SUBSTRATE_PINS:
            convertYesNoSpec(pNode, "compare_option -sconnect_substrate_pins");
            break;
        case LVS_SPICE_ALLOW_DUPLICATE_SUBCIRCUIT_NAMES:
            convertYesNoSpec(pNode, "spice_option -duplicate_subcircuit");
            break;
        case LVS_SPICE_ALLOW_FLOATING_PINS:
            convertYesNoSpec(pNode, "spice_option -allow_floating_pins");
            break;
        case LVS_SPICE_ALLOW_INLINE_PARAMETERS:
            convertYesNoSpec(pNode, "spice_option -inline_parameters");
            break;
        case LVS_SPICE_ALLOW_UNQUOTED_STRINGS:
            convertYesNoSpec(pNode, "spice_option -unquoted_strings");
            break;
        case LVS_SPICE_CONDITIONAL_LDD:
            convertYesNoSpec(pNode, "spice_option -conditional_ldd");
            break;
        case LVS_SPICE_CULL_PRIMITIVE_SUBCIRCUITS:
            convertYesNoSpec(pNode, "spice_option -cull_primitive_subcircuits");
            break;
        case LVS_SPICE_IMPLIED_MOS_AREA:
            convertYesNoSpec(pNode, "spice_option -calculate_mos_area");
            break;
        case LVS_SPICE_MULTIPLIER_NAME:
            convertSpiceOptionSpec(pNode);
            break;
        case LVS_SPICE_OPTION:
            convertSpiceOptionSpec(pNode);
            break;
        case LVS_SPICE_OVERRIDE_GLOBALS:
            convertYesNoSpec(pNode, "spice_option -override_global");
            break;
        case LVS_SPICE_PREFER_PINS:
            convertYesNoSpec(pNode, "spice_option -prefer_pins");
            break;
        case LVS_SPICE_REDEFINE_PARAM:
            convertYesNoSpec(pNode, "spice_option -redefine_parameter");
            break;
        case LVS_SPICE_RENAME_PARAMETER:
            convertRenameSpiceParameterSpec(pNode);
            break;
        case LVS_SPICE_REPLICATE_DEVICES:
            convertYesNoSpec(pNode, "spice_option -replicate_devices");
            break;
        case LVS_SPICE_SCALE_X_PARAMETERS:
            convertSpiceOptionSpec(pNode);
            break;
        case LVS_SPICE_SLASH_IS_SPACE:
            convertYesNoSpec(pNode, "spice_option -ignore_slash");
            break;
        case LVS_SPICE_STRICT_WL:
            convertYesNoSpec(pNode, "spice_option -strict_wl");
            break;
        case LVS_SPLIT_GATE_RATIO:
            convertSplitGateRatioSpec(pNode);
            break;
        case LVS_STRICT_SUBTYPES:
            convertYesNoSpec(pNode, "compare_option -strict_subtypes");
            break;
        case LVS_WRITE_INJECTED_LAYOUT_NETLIST:
            outputNetListOutputSpec(pNode);
            break;
        case LVS_WRITE_INJECTED_SOURCE_NETLIST:
            outputNetListOutputSpec(pNode);
            break;
        case LVS_WRITE_LAYOUT_NETLIST:
            outputNetListOutputSpec(pNode);
            break;
        case LVS_WRITE_SOURCE_NETLIST:
            outputNetListOutputSpec(pNode);
            break;
        case LVS_PRECISE_INTERACTION:
            convertYesNoSpec(pNode, "precise_device_overlap");
            break;
        case MASK_SVDB_DIRECTORY:
            convertOutputLvsDBSpec(pNode);
            break;
        case SOURCE_CASE:
            convertYesNoSpec(pNode, "source_case");
            break;
        case SOURCE_PATH:
            convertSourceDbSpec(pNode);
            break;
        case SOURCE_PRIMARY:
            convertSourceDbSpec(pNode);
            break;
        case SOURCE_SYSTEM:
            convertSourceDbSpec(pNode);
            break;
        case TRACE_PROPERTY:
            convertCheckPropertySpec(pNode);
            break;
        case UNIT_CAPACITANCE:
            convertUnitOptionSpec(pNode);
            break;
        case UNIT_RESISTANCE:
            convertUnitOptionSpec(pNode);
            break;
        case UNIT_INDUCTANCE:
            convertUnitOptionSpec(pNode);
            break;
        case UNIT_LENGTH:
            convertUnitOptionSpec(pNode);
            break;
        case UNIT_TIME:
            convertUnitOptionSpec(pNode);
            break;
        case SVRF_ERROR:
            convertErrorSpec(pNode);
            break;
        case SVRF_MESSAGE:
            convertPutsSpec(pNode);
            break;
        case DEVICE:
            convertDeviceSpec(pNode);
            break;
        case LVS_RECOGNIZE_GATES:
            convertIdentifyGatesSpec(pNode);
            break;
        case LVS_SOFTCHK:
            convertSconnectCheckSpec(pNode);
            break;
        case LVS_DEVICE_TYPE:
            convertMapDeviceTypeSpec(pNode);
            break;
        case DFM_SELECT_CHECK:
            convertRuleSelectSpec(pNode, "select_exec_dfm_rule -name");
            break;
        case DFM_SELECT_CHECK_NODAL:
            break;
        case DFM_UNSELECT_CHECK:
            convertRuleSelectSpec(pNode, "unselect_exec_dfm_rule -name");
            break;
        case DFM_SUMMARY_REPORT:
            convertSumFileSpec(pNode);
            break;
        case ERC_PATHCHK:
            convertErcPathchkSpec(pNode);
            break;
        case TITLE:
            convertFileHeadNameSpec(pNode);
            break;
        case TVF_FUNCTION:
            convertTclProcSpec(pNode);
            break;
        case LVS_MAP_DEVICE:
            convertMapDeviceSpec(pNode);
            break;
        case LVS_RECOGNIZE_GATES_TOLERANCE:


        case LAYOUT_PROPERTY_AUDIT:
        case DRC_BOOLEAN_NOSNAP45:
        case DFM_FUNCTION:
        case DFM_DATABASE:
        case DFM_DATABASE_DIRECTORY:
        case DFM_DEFAULTS_RDB:
        case DFM_SPEC_SPATIAL_SAMPLE:
        case DFM_SPEC_VIA_REDUNDANCY:
        case DFM_ASSERT:
        case DFM_SPEC_FILL:
        case DFM_YS_AUTOSTART:
        case FLATTEN_VERY_SMALL_CELLS:
        case LVS_NL_PIN_LOCATIONS:
        case LVS_PIN_NAME_PROPERTY:
        case MASK_RESULTS_DATABASE:;
        case LVS_COMPONENT_SUBTYPE_PROPERTY:
        case LVS_COMPONENT_TYPE_PROPERTY:
        case LVS_AUTO_EXPAND_HCELL:
        case LVS_SIGNATURE_MAXIMUM:
        case LVS_SOFTCHK_MAXIMUM_RESULTS:
        case LVS_CENTER_DEVICE_PINS:
        case LVS_MOS_SWAPPABLE_PROPERTIES:
        case LVS_ANNOTATE_DEVICES:
        case LAYOUT_TEXT_FILE:
        case LVS_CONFIGURE_UNUSED_FILTER:
        case DFM_DEFAULTS:
        case LAYOUT_PROPERTY_TEXT_OASIS:
        case PEX_CMD:
        case CAPACITANCE_CMD:
        case RESISTANCE_CMD:
        case NON_SUPPORT_SPECIFICATION:
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

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeProConvertor_T::endVisitLayerOperationNode(rcsSynLayerOperationNode_T *pNode)
{
}

void
rcsSynNodeProConvertor_T::beginVisitLayerOperationNode(rcsSynLayerOperationNode_T *pNode)
{
    if(m_iProcLevel > 0 && !m_inProcReturn)
    {
        m_oPVRSStream << "return \"\n" << std::endl;
        m_inProcReturn = true;
    }

    std::string sValue;
    pNode->getCmdValue(sValue);

    if(rcsManager_T::getInstance()->isSvrfComment())
    {
        if(!m_isInLayerDefinition)
            m_oPVRSStream << "#";
    }

    if(pNode->isCMacro())
    {
        if(rcsManager_T::getInstance()->isSvrfComment())
        {
            if(!m_isInLayerDefinition)
                m_oPVRSStream << sValue << std::endl;
        }

        convertExecProcCmd(pNode);
        if(!m_isInLayerDefinition)
            m_oPVRSStream << std::endl;
        return;
    }

   if(rcsManager_T::getInstance()->isSvrfComment())
       m_oPVRSStream << sValue << std::endl;

    switch(pNode->getOperationType())
    {
        case ATTACH:
            convertAttachCmd(pNode);
            break;
        case CONNECT:
            convertConnectCmd(pNode);
            break;
        case SCONNECT:
            convertSconnectCmd(pNode);
            break;
        case DISCONNECT:
            convertDisconnectCmd(pNode);
            break;
        case LABEL_ORDER:
            convertAttachOrderCmd(pNode);
            break;
        case AND:
        case OR:
        case XOR:
        case NOT:
            convertGeomBooleanCmd(pNode);
            break;
        case INSIDE:
        case NOT_INSIDE:
        case OUTSIDE:
        case NOT_OUTSIDE:
        case TOUCH:
        case NOT_TOUCH:
        case ENCLOSE:
        case NOT_ENCLOSE:
        case CUT:
        case NOT_CUT:
        case INTERACT:
        case NOT_INTERACT:
            convertGeomTopoCmd(pNode);
            break;
        case INSIDE_EDGE:
        case NOT_INSIDE_EDGE:
        case OUTSIDE_EDGE:
        case NOT_OUTSIDE_EDGE:
        case COINCIDENT_EDGE:
        case NOT_COINCIDENT_EDGE:
        case COINCIDENT_INSIDE_EDGE:
        case NOT_COINCIDENT_INSIDE_EDGE:
        case COINCIDENT_OUTSIDE_EDGE:
        case NOT_COINCIDENT_OUTSIDE_EDGE:
        case TOUCH_EDGE:
        case NOT_TOUCH_EDGE:
        case TOUCH_INSIDE_EDGE:
        case NOT_TOUCH_INSIDE_EDGE:
        case TOUCH_OUTSIDE_EDGE:
        case NOT_TOUCH_OUTSIDE_EDGE:
            convertEdgeTopoCmd(pNode);
            break;
        case AREA:
        case NOT_AREA:
        case PERIMETER:
        case DONUT:
        case NOT_DONUT:
        case VERTEX:
            convertGeomAttrCmd(pNode);
            break;
        case RECTANGLE:
        case NOT_RECTANGLE:
            convertGeomSelectRectangleCmd(pNode);
            break;
        case NOT_WITH_EDGE:
        case WITH_EDGE:
            convertGeomSelectByCoinEdgeCmd(pNode);
            break;
        case NOT_INSIDE_CELL:
        case INSIDE_CELL:
            convertGeomSelectInCellCmd(pNode);
            break;
        case NOT_ENCLOSE_RECTANGLE:
        case ENCLOSE_RECTANGLE:
            convertCheckFittingRectangleCmd(pNode);
            break;
        case OR_EDGE:
            convertEdgeMergeCmd(pNode);
            break;
        case ANGLE:
        case NOT_ANGLE:
        case LENGTH:
        case NOT_LENGTH:
        case PATH_LENGTH:
            convertEdgeSelectByAttrCmd(pNode);
            break;
        case CONVEX_EDGE:
            convertEdgeSelectByConvexCornerCmd(pNode);
            break;
        case DRAWN_ACUTE:
        case DRAWN_SKEW:
        case DRAWN_OFFGRID:
        case DRAWN_ANGLED:
            convertGeomOriginalCheckCmd(pNode);
            break;
        case GROW:
        case SHRINK:
        case MAGNIFY:
        case ROTATE:
        case DFM_SHIFT:
        case SHIFT:
            convertGeomTransformCmd(pNode);
            break;
        case EXPAND_EDGE:
            convertEdgeExpandToRectCmd(pNode);
            break;
        case SIZE:
            convertSizeCmd(pNode);
            break;
        case HOLES:
            convertGeomSelectHolesCmd(pNode);
            break;
        case DEANGLE:
            convertEdgeConvertAgnledToRegularCmd(pNode);
            break;
        case SNAP:
            convertGeomVerticeSnapToGridCmd(pNode);
            break;
        case MERGE:
            convertGeomMergeLayerCmd(pNode);
            break;
        case COPY:
            convertGeomCopyLayerCmd(pNode);
            break;
        case FLATTEN:
            convertGeomFlattenLayerCmd(pNode);
            break;
        case EXTENTS:
            convertGeomGetBBoxCmd(pNode);
            break;
        case EXTENT_CELL:
            convertGeomGetCellBBoxCmd(pNode);
            break;
        case EXTENT:
            convertGeomGetLayerBBoxCmd(pNode);
            break;
        case RECTANGLES:
            convertFillRectanglesCmd(pNode);
            break;
        case EXPAND_TEXT:
            convertGeomGetTextBBoxCmd(pNode);
            break;
        case WITH_TEXT:
        case NOT_WITH_TEXT:
            convertGeomSelectTextedCmd(pNode);
            break;
        case TDDRC:
            convertCheckTableDRCCmd(pNode);
            break;
        case DENSITY:
        case DENSITY_CONVOLVE:
            convertCheckDensityCmd(pNode);
            break;
        case WITH_WIDTH:
        case NOT_WITH_WIDTH:
            convertCheckWidthCmd(pNode);
            break;
        case ENCLOSURE:
        case EXTERNAL:
        case INTERNAL:
            convertCheckSpaceCmd(pNode);
            break;
        case NOT_WITH_NEIGHBOR:
        case WITH_NEIGHBOR:
            convertGeomSelectByNeighborCmd(pNode);
            break;
        case OFFGRID:
            convertCheckOffgridCmd(pNode);
            break;
        case RECTANGLE_ENCLOSURE:
            convertCheckRectEncCmd(pNode);
            break;
        case DEVICE_LAYER:
            convertGeomSelectDeviceLayerCmd(pNode);
            break;
        case NET_AREA:
            convertGeomGetNetByAreaCmd(pNode);
            break;
        case NOT_NET:
        case NET:
            convertGeomGetNetByNameCmd(pNode);
            break;
        case ORNET:
            convertGeomMergeNetCmd(pNode);
            break;
        case EXTENT_DRAWN:
            convertGeomGetMultiLayersBBoxCmd(pNode);
            break;
        case PUSH:
            convertGeomPushLayerCmd(pNode);
            break;
        case STAMP:
            convertGeomStampLayerCmd(pNode);
            break;
        case NET_AREA_RATIO_PRINT:
            convertNetAreaRatioRDBCmd(pNode);
            break;
        case NET_AREA_RATIO_ACCUMULATE:
            convertCheckNetAreaRatioByAccumulateCmd(pNode);
            break;
        case NET_AREA_RATIO:
            convertCheckNetAreaRatioCmd(pNode);
            break;
        case DFM_PROPERTY:
            convertBuildDfmPropertyCmd(pNode);
            break;
        case DFM_SPACE:
            convertDfmSpaceCmd(pNode);
            break;
        case DFM_SIZE:
            convertDfmSizeCmd(pNode);
            break;
        case DFM_COPY:
            convertGeomCopyDfmLayerCmd(pNode);
            break;
        case DFM_NARAC:
            convertDfmNARACCmd(pNode);
            break;
        case DFM_RDB:
#if 0
        case DFM_RDB_GDS:
        case DFM_RDB_OASIS:
#endif
            convertDfmRDBCmd(pNode);
            break;
        case DFM_STAMP:
            convertDfmStampCmd(pNode);
            break;
        case PATHCHK:
            convertPathchkCmd(pNode);
            break;
        case OPCBIAS:
            convertOpcBiasCmd(pNode);
            break;
        case POLYNET:
        case TVF:
        case DFM_CLASSIFY:
        case DFM_COMPRESS:
        case DFM_CONNECTIVITY_REDUNDANT:
        case DFM_CONNECTIVITY_NOT_REDUNDANT:
        case DFM_CREATE_LAYER:
        case DFM_CREATE_LAYER_POLYGONS:
        case DFM_EXPAND_EDGE:
        case DFM_EXPAND_ENCLOSURE:
        case DFM_FILL:
        case DFM_HISTOGRAM:
        case DFM_GROW:
        case DFM_MEASURE:
        case DFM_PROPERTY_MERGE:
        case DFM_SHIFT_EDGE:
        case DFM_TEXT:
        case DFM_TRANSFORM:
        case DFM_TRANSITION:
        case DFM_READ:
        case DFM_REDUNDANT_VIAS:
        case DFM_NON_REDUNDANT_VIAS:
        case DFM_ANALYZE:
        case DFM_CAF:
        case DFM_GCA:
        case DFM_CRITICAL_AREA:
        case DFM_OR_EDGE:
        case NET_INTERACT:
        case PINS:
        case PORTS:
        case TOPEX:
        case PEX_CMD:
        case NON_SUPPORT_OPERTION:
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

    if(!m_isInLayerDefinition)
        m_oPVRSStream << std::endl;
}

void
rcsSynNodeProConvertor_T::beginVisitPreProcessNode(rcsSynPreProcessNode_T *pNode)
{
    if(m_iProcLevel > 0 && m_inProcReturn)
    {
        m_oPVRSStream << "\"" << std::endl;
    }

    std::string sCmdValue;
    pNode->getCmdValue(sCmdValue);
    if(rcsManager_T::getInstance()->isSvrfComment())
        m_oPVRSStream << "#";

    std::vector<std::string> vOperands;
    switch(pNode->getPreProcessType())
    {
        case PRO_DEFINE:
        {
            Utassert(0 == strncasecmp(sCmdValue.c_str(), "#define", 7));
            if(rcsManager_T::getInstance()->isSvrfComment())
                m_oPVRSStream << "#DEFINE ";

            std::string sOperands = sCmdValue.substr(7);
            parseOperands(sOperands, vOperands);

            if(rcsManager_T::getInstance()->isSvrfComment())
            {
                for(hvUInt32 i = 0; i < vOperands.size(); ++i)
                    m_oPVRSStream << vOperands[i] << " ";
                m_oPVRSStream << std::endl;
            }

            if('$' == vOperands[0][0])
            {
                vOperands[0].erase(0, 1);
                m_oPVRSStream << "set env(" << vOperands[0]<< ")";
            }
            else
            {
                if(0 == m_iProcLevel)
                    m_oPVRSStream << "set \"" << vOperands[0]<< "\"";
                else
                    m_oPVRSStream << "set \"::" << vOperands[0]<< "\"";
            }

            if(2 == vOperands.size())
                m_oPVRSStream << " " << vOperands[1];
            else
                m_oPVRSStream << " 1";
            m_oPVRSStream << std::endl << std::endl;
            break;
        }
        case PRO_REDEFINE:
        {
            Utassert(0 == strncasecmp(sCmdValue.c_str(), "#redefine", 9));
            if(rcsManager_T::getInstance()->isSvrfComment())
                m_oPVRSStream << "#REDEFINE ";

            std::string sOperands = sCmdValue.substr(9);
            parseOperands(sOperands, vOperands);

            if(rcsManager_T::getInstance()->isSvrfComment())
            {
                for(hvUInt32 i = 0; i < vOperands.size(); ++i)
                    m_oPVRSStream << vOperands[i] << " ";
                m_oPVRSStream << std::endl;
            }

            if('$' == vOperands[0][0])
            {
                vOperands[0].erase(0, 1);
                m_oPVRSStream << "set_resident env(" << vOperands[0]<< ")";
            }
            else
            {
                if(0 == m_iProcLevel)
                    m_oPVRSStream << "set_resident \"" << vOperands[0]<< "\"";
                else
                    m_oPVRSStream << "set_resident \"::" << vOperands[0]<< "\"";
            }

            if(2 == vOperands.size())
                m_oPVRSStream << " " << vOperands[1];
            else
                m_oPVRSStream << " 1";
            m_oPVRSStream << std::endl << std::endl;
            break;
        }
        case PRO_UNDEFINE:
        {
            Utassert(0 == strncasecmp(sCmdValue.c_str(), "#undefine", 9));
            if(rcsManager_T::getInstance()->isSvrfComment())
                m_oPVRSStream << "#UNDEFINE ";
            std::string sOperands = sCmdValue.substr(9);
            parseOperands(sOperands, vOperands);

            if(rcsManager_T::getInstance()->isSvrfComment())
            {
                for(hvUInt32 i = 0; i < vOperands.size(); ++i)
                    m_oPVRSStream << vOperands[i] << " ";
                m_oPVRSStream << std::endl;
            }
            if('$' == vOperands[0][0])
            {
                vOperands[0].erase(0, 1);
                m_oPVRSStream << "if { [ info exists env(" << vOperands[0] << ") ] } {" << std::endl;
                m_oPVRSStream << "\tunset env(" << vOperands[0] << ")" << std::endl;
            }
            else
            {
                if(0 == m_iProcLevel)
                {
                    m_oPVRSStream << "if { [ info exists \"" << vOperands[0] << "\" ] } {" << std::endl;
                    m_oPVRSStream << "\tunset \"" << vOperands[0] << "\"" << std::endl;
                }
                else
                {
                    m_oPVRSStream << "if { [ info exists \"::" << vOperands[0] << "\" ] } {" << std::endl;
                    m_oPVRSStream << "\tunset \"::" << vOperands[0] << "\"" << std::endl;
                }
            }
            m_oPVRSStream << "}" << std::endl << std::endl;
            break;
        }
        case PRO_IFDEF:
        {
            Utassert(0 == strncasecmp(sCmdValue.c_str(), "#ifdef", 6));

            if(rcsManager_T::getInstance()->isSvrfComment())
                m_oPVRSStream << "#IFDEF ";

            std::string sOperands = sCmdValue.substr(6);
            parseOperands(sOperands, vOperands);

            if(rcsManager_T::getInstance()->isSvrfComment())
            {
                for(hvUInt32 i = 0; i < vOperands.size(); ++i)
                    m_oPVRSStream << vOperands[i] << " ";
                m_oPVRSStream << std::endl;
            }

            if('$' != vOperands[0][0])
            {
                if(2 == vOperands.size())
                {
                    if(0 == m_iProcLevel)
                        m_oPVRSStream << "if { [ info exists \"" << vOperands[0]
                                      << "\" ] && [ string equal \"$" << vOperands[0]
                                      << "\" " << vOperands[1] << " ] } { " << std::endl;
                    else
                        m_oPVRSStream << "if { [ info exists \"::" << vOperands[0]
                                      << "\" ] && [ string equal \"$::" << vOperands[0]
                                      << "\" " << vOperands[1] << " ] } { " << std::endl;
                }
                else
                {
                    if(0 == m_iProcLevel)
                        m_oPVRSStream << "if { [ info exists \"" << vOperands[0] << "\" ] } { " << std::endl;
                    else
                        m_oPVRSStream << "if { [ info exists \"::" << vOperands[0] << "\" ] } { " << std::endl;
                }
            }
            else
            {
                vOperands[0].erase(0, 1);
                if(2 == vOperands.size())
                {
                    m_oPVRSStream << "if { [ info exists env(" << vOperands[0]
                                  << ") ] && [ string equal $env(" << vOperands[0]
                                  << ") " << vOperands[1] << " ] } { " << std::endl;
                }
                else
                {
                    m_oPVRSStream << "if { [ info exists env(" << vOperands[0] << ") ] } { " << std::endl;
                }
            }
            m_oPVRSStream << std::endl;
            break;
        }
        case PRO_IFNDEF:
        {
            Utassert(0 == strncasecmp(sCmdValue.c_str(), "#ifndef", 7));
            if(rcsManager_T::getInstance()->isSvrfComment())
                m_oPVRSStream << "#IFNDEF ";

            std::string sOperands = sCmdValue.substr(7);
            parseOperands(sOperands, vOperands);

            if(rcsManager_T::getInstance()->isSvrfComment())
            {
                for(hvUInt32 i = 0; i < vOperands.size(); ++i)
                    m_oPVRSStream << vOperands[i] << " ";
                m_oPVRSStream << std::endl;
            }

            if('$' != vOperands[0][0])
            {
                if(2 == vOperands.size())
                {
                    if(0 == m_iProcLevel)
                        m_oPVRSStream << "if { ![ info exists \"" << vOperands[0]
                                      << "\" ] || ![ string equal \"$" << vOperands[0]
                                      << "\" " << vOperands[1] << " ] } { " << std::endl;
                    else
                        m_oPVRSStream << "if { ![ info exists \"::" << vOperands[0]
                                      << "\" ] || ![ string equal \"$::" << vOperands[0]
                                      << "\" " << vOperands[1] << " ] } { " << std::endl;
                }
                else
                {
                    if(0 == m_iProcLevel)
                        m_oPVRSStream << "if { ![ info exists \"" << vOperands[0] << "\" ] } { " << std::endl;
                    else
                        m_oPVRSStream << "if { ![ info exists \"::" << vOperands[0] << "\" ] } { " << std::endl;
                }
            }
            else
            {
                vOperands[0].erase(0, 1);
                if(2 == vOperands.size())
                {
                    m_oPVRSStream << "if { ![ info exists env(" << vOperands[0]
                                  << ") ] || ![ string equal $env(" << vOperands[0]
                                  << ") " << vOperands[1] << " ] } { " << std::endl;
                }
                else
                {
                    m_oPVRSStream << "if { ![ info exists env(" << vOperands[0] << ") ] } { " << std::endl;
                }
            }
            m_oPVRSStream << std::endl;
            break;
        }
        case PRO_ELSE:
        {
            if(rcsManager_T::getInstance()->isSvrfComment())
                m_oPVRSStream << "#ELSE\n";

            m_oPVRSStream << "} else {" << std::endl << std::endl;
            break;
        }
        case PRO_ENDIF:
        {
            if(rcsManager_T::getInstance()->isSvrfComment())
                m_oPVRSStream << "#ENDIF\n";

            m_oPVRSStream << "}" << std::endl << std::endl;
            break;
        }
        default:
            Utassert(false);
            break;
    }

    if(m_iProcLevel > 0 && m_inProcReturn)
        m_inProcReturn = false;
}

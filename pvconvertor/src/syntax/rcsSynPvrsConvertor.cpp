#include "lexical/FlexLexer.h"
#include "lexical/rcsLexParser.h"
#include "public/manager/rcErrorManager.h"
#include "public/util/rcsStringUtil.hpp"
#include "public/synnode/rcsSynRulecheckNode.h"
#include "public/synnode/rcsSynSpecificationNode.h"
#include "public/synnode/rcsSynLayerOperationNode.h"
#include "public/synnode/rcsSynLayerDefinitionNode.h"
#include "public/synnode/rcsSynProcessNode.h"
#include "public/synnode/rcsSynCommentNode.h"
#include "public/synnode/rcsSynBuildinNode.h"
#include "public/synnode/rcsSynProcNode.h"
#include "public/manager/rcsManager.h"
#include "tvf/rcsTvfConvertor.h"
#include "main/rcsSvrf2Pvrs.h"

#include "rcsNumberParser.h"
#include "rcsSynPvrsConvertor.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>
#include "public/util/rcscommentutil.h"
#include "public/synnode/rcssyndmacrodummynode.h"

using namespace rcsCommentUtil;

const hvUInt32 MAX_OPTIONS_PER_LINE = 6;

rcsSynPvrsConvertor::rcsSynPvrsConvertor(std::map<hvUInt32, std::pair<hvUInt32, bool> > &blankLinesBefore, bool bTvfConvertor)
: m_oPVRSStream(&(rcsManager_T::getInstance()->getPVRSOutStream())),
  m_oLogStream(rcsManager_T::getInstance()->getPVRSSumStream()),
  m_sPVRSCmdPrefix(rcsManager_T::getInstance()->getPVRSCmdPrefix()),
  m_isTmpLayerDefinition(false), m_isInLayerDefinition(false),
  m_hasPrecisionDef(false), m_hasGetTmpLayerValue(false), m_iProcLevel(0),
  m_inProcReturn(false), m_blankLinesBefore(blankLinesBefore), m_nParsingTmpLayers(0),m_bTvfConvertor(bTvfConvertor)
{
    rcsExpressionParser_T::setNewPvrs();
    initSimplePVRSmap();

    char* _debugStr = getenv("__DEBUG_COMMAND_CONNECT");
    if(_debugStr != NULL)
    {
        char* _path = getenv("HOME");
        std::ifstream _iCommandStream = std::ifstream(std::string(_path) + "/debug_command_connect.file");
        if(_iCommandStream.is_open())
        {
            std::string _commandLine;
            while(std::getline(_iCommandStream, _commandLine))
            {
                this->m_debugCommandSet.insert(_commandLine);
            }
        }
        _iCommandStream.close();
        m_oCommandStream = std::ofstream(std::string(_path) + "/debug_command_connect.file", std::ios_base::trunc | std::ios_base::out);
    }
}

rcsSynPvrsConvertor::~rcsSynPvrsConvertor()
{

}

void
rcsSynPvrsConvertor::closeDebugFile()
{
    char* _debugStr = getenv("__DEBUG_COMMAND_CONNECT");
    if(_debugStr != NULL)
    {
        for(auto& _command : m_debugCommandSet)
        {
            m_oCommandStream << _command << std::endl;
        }
        m_oCommandStream.close();
    }
}

void
rcsSynPvrsConvertor::setHasGetTmpLayerValues()
{
    m_hasGetTmpLayerValue = true;
}

void
rcsSynPvrsConvertor::parseOperands(std::string &str, std::vector<std::string> &vOperands)
{
}

bool
rcsSynPvrsConvertor::getNumberExpForTvfCmdWithNegativePara(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end,
        std::string &sExp, bool withParentheses, bool isAllowExpression)
{
    if (!rcsExpressionParser_T::parseNumberJustForNegative(iter, end, sExp))
    {
        return false;
    }

    return true;
}


bool
rcsSynPvrsConvertor::getNumberExp(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end, std::string &sExp,
                                  bool withParentheses, bool isAllowExpression)
{
    if (isAllowExpression)
    {
        if (!rcsExpressionParser_T::parse(iter, end, sExp))
            return false;
    }
    else if (!rcsExpressionParser_T::parseNumber(iter, end, sExp))
    {
        return false;
    }

    if (withParentheses && isAllowExpression)
    {
        sExp.insert(0, "(");
        sExp.insert(sExp.size(), ")");
    }

    return true;
}

static std::string
parseTokenString(const rcsToken_T &token)
{
    const std::string &src = token.sValue;
    std::string res;
    std::string::size_type pos = 0;
    if (token.eType == STRING_CONSTANTS)
    {
        while(pos < src.length())
        {
            switch(src[pos])
            {
                case '\t':
                    res += "\\t";
                    break;
                case '\n':
                    res += "\\n";
                    break;
                case '\r':
                    res += "\\r";
                    break;
                case '\a':
                    res += "\\a";
                    break;
                case '\b':
                    res += "\\b";
                    break;
                case '\e':
                    res += "\\e";
                    break;
                case '\f':
                    res += "\\f";
                    break;
                case '\v':
                    res += "\\v";
                    break;
                case '\'':
                    res += "\\\'";
                    break;
                case '\"':
                    res += "\\\"";
                    break;
                case '\\':
                    res += "\\\\";
                    break;
                case '\1':
                    res += "\\1";
                    break;
                case '\2':
                    res += "\\2";
                    break;
                case '\3':
                    res += "\\3";
                    break;
                case '\4':
                    res += "\\4";
                    break;
                case '\5':
                    res += "\\5";
                    break;
                case '\6':
                    res += "\\6";
                    break;
                case '\0':
                    res += "\\0";
                    break;
                default:
                    res += src[pos];
                    break;
            }
            ++pos;
        }
    }
    else
    {
        while(pos < src.length())
        {
            switch(src[pos])
            {
                case '\\':
                    res += "\\\\";
                    break;
                default:
                    res += src[pos];
                    break;
            }
            ++pos;
        }
    }
    return res;
}

#define PVRS_KEYWORD_COUNT 873

static const
char* rcsPvrsKeyword[] =
{
    "LAYOUT_INPUT",
    "LAYOUT_INPUT2",
    "LAYOUT_OA_OPTION",
    "FORMAT",
    "PATH",
    "TOP_CELL",
    "VIEW",
    "PREC",
    "ELLIPSE_SEGMENTS",
    "DONUT_SEGMENTS",
    "PRECISION",
    "RESOLUTION",
    "DB",
    "DRC_RESULT",
    "ERC_RESULT",
    "LIBNAME",
    "TEXT_FORMAT",
    "WITH_PREFIX",
    "WITH_APPEND",
    "MERGED",
    "KEEP_INJECTED_CELL",
    "USER_CELL",
    "DRC_SUMMARY",
    "ERC_SUMMARY",
    "REWRITE",
    "OVERWRITE",
    "APPEND",
    "BY_CELL",
    "DRC_MAX_RESULT",
    "DRC_MAX_VERTEX",
    "ERC_MAX_RESULT",
    "ERC_MAX_VERTEX",
    "ALL",
    "LVS_REPORT",
    "MAX",
    "UNIT",
    "ONLY_WARNING_HCELL",
    "ONLY_WARNING_TOP",
    "OPTION",
    "NOT_SET",
    "NONE",
    "CONNECTION",
    "CONNECTION_EXCLUDE_PG",
    "SHORT_OPEN",
    "SHORT_OPEN_EXCLUDE_PG",
    "MISSING_NET",
    "MISSING_NET_EXCLUDE_PG",
    "MISSING_INSTANCE",
    "MISSING_PROPERTY",
    "MISSING_PROPERTY_TOLERANCE",
    "MERGE_MOS",
    "DETAIL_INFORMATION",
    "PROPERTY",
    "MISSING_CELL",
    "ISOLATED_NET",
    "LAYOUT_PASSTHROUGH",
    "COUNT_BY_MODEL",
    "MISSING_NAME",
    "HCELL",
    "RESOLUTION_WARNING",
    "SHORT_PORT",
    "HCELL_PG",
    "AMBIGUITY",
    "AMBIGUITY_ALL",
    "SHOW_UNICONNECT",
    "SCHEMATIC_PASSTHROUGH_WARNING",
    "SCHEMATIC_PASSTHROUGH",
    "SAME_DATA",
    "SHOW_VIRTUAL_CONNECTION",
    "NOT_IDENTICAL_PIN",
    "PIN_BY_NET",
    "PIN_BY_SHAPE",
    "RC_MODEL",
    "RC_ELEMENT",
    "TEXT_MODEL",
    "TEXT_PROPERTY",
    "DRC_OUTPUT_CELL_NAME",
    "ERC_OUTPUT_CELL_NAME",
    "CELL_COORD",
    "TRAN_TO_TOP",
    "DRC_PROCESS_CELL_TEXT",
    "OUTPUT_NET",
    "TEXT_FORMAT",
    "DRC_RULE_MAP",
    "TEXT_FORMAT",
    "WITH_PREFIX",
    "WITH_APPEND",
    "MAX_RESULT",
    "MAX_VERTEX",
    "ADD_TEXT",
    "DB_PRECISION",
    "DB_MAGNIFY",
    "INJECT_ARRAY",
    "WITH_POLYGON",
    "BY_POLYGON",
    "AUTO_INJECT_ARRAY",
    "DRC_OUTPUT_RULE_TEXT",
    "ERC_OUTPUT_RULE_TEXT",
    "COMMENT",
    "INCREMENTAL_CONNECT_WARNING",
    "DRC_PROGRESSIVE_CONNECT_WARNING",
    "INCREMENTAL_CONNECT",
    "DRC_OUTPUT_REDUCE_FALSE_ERROR",
    "DRC_PROGRESSIVE_CONNECT",
    "DRC_OUTPUT_EMPTY_RULE",
    "DRC_OUTPUT_EMPTY",
    "ERC_OUTPUT_EMPTY_RULE",
    "ERC_OUTPUT_EMPTY",
    "LVS_CHECK_TOP_CELL_PORT",
    "DEVICE_PUSH",
    "PUSH_DEVICE",
    "LVS_SHORT_EQUIVALENT_NET",
    "LVS_SUBCELL_POWER_GROUND",
    "LVS_MATCH_STRICT_SUBTYPE",
    "LVS_KEEP_FLOATING_TOP_NET",
    "LVS_KEEP_PCELL",
    "LVS_PRECISE_OVERLAP",
    "LAYOUT_USE_DB_PRECISION",
    "LVS_IGNORE_EXTRA_PIN",
    "LVS_MATCH_ACCURATE_SUBTYPE",
    "LVS_RUN_ERC",
    "LVS_GLOBAL_NET_AS_PORT",
    "LVS_INJECT_HIERARCHY",
    "LVS_OUTPUT_DEVICE_PROMOTION",
    "LVS_IGNORE_SUBSTRATE_PIN",
    "SNAP_OFFGRID",
    "GUI_PRIORITY",
    "LVS_ZERO_PROPERTY_WITH_TOL",
    "LVS_IGNORE_DEVICE_MODEL",
    "FPD_TOUCH_IN_CELL",
    "LVS_OUTPUT_DEVICE_PROMOTION_MAX",
    "UNICONNECT_CHECK_MAX_RESULT",
    "LVS_POWER",
    "LVS_GROUND",
    "LVS_NON_USER_DEFINE_NAME",
    "INSTANCE",
    "PORT",
    "LVS_KEEP_BLACK_BOX",
    "CELL",
    "DRC_MAGNIFY_RESULT",
    "NAR",
    "SIMULATE_CELL",
    "NEW_CELL",
    "PRINT_LOG",
    "COMPILE",
    "TEXT_MAX",
    "ACUTE",
    "ANGLED",
    "ACUTE_EDGE",
    "ANGLED_EDGE",
    "SELF_INTERSECTING_POLYGON",
    "SELF_INTERSECTING_PATH",
    "OFFGRID_VERTEX",
    "SKEW",
    "SKEW_EDGE",
    "MAX_WARNINGS",
    "CASE_SENSITIVE",
    "LAYOUT",
    "COMPARE",
    "SCHEMATIC",
    "LOWER_DEVICE_CASE",
    "LAYOUT_PROPERTY_NET",
    "LAYOUT_PROPERTY",
    "LAYOUT_NET",
    "NAME",
    "TYPE",
    "SUBTYPE",
    "STR_VALUE",
    "CHECK_PROPERTY",
    "ABSOLUTE_VALUE",
    "STRING",
    "NOT_STRING",
    "CELL_GROUP",
    "DEVICE_MERGE",
    "PARALLEL_BJT",
    "SPLIT",
    "SPLIT_GATE",
    "USER_DEFINE",
    "PARALLEL_CAP",
    "PARALLEL_DIO",
    "PARALLEL_MOS",
    "PARALLEL_RES",
    "SERIES_CAP",
    "SERIES_MOS",
    "SERIES_RES",
    "PARTIAL_SERIES_MOS",
    "PARTIAL",
    "IN_ORDER",
    "IN_TOLERANCE",
    "DIFFERENT_TYPE",
    "SERIES_PARALLEL",
    "PARALLEL",
    "SERIES",
    "CELL_GROUP",
    "DRC_IGNORE_RULE",
    "DFM_IGNORE_RULE",
    "BY_NAME",
    "BY_LAYER",
    "DRC_SELECT_RULE",
    "DFM_SELECT_RULE",
    "ERC_IGNORE_RULE",
    "ERC_SELECT_RULE",
    "QUIT_LVS",
    "DRC_TOLERANCE",
    "DISTANCE",
    "GROUP_RULE",
    "HCELL",
    "LABEL_LEVEL",
    "PORT",
    "TEXT",
    "LABEL_LAYER",
    "PORT_POLYGON",
    "LAYER_MAP",
    "LAYOUT_ADD_CELL",
    "LVS_DB",
    "LVS_OUTPUT_NETLIST",
    "SCHEMATIC",
    "INJECTED_LAYOUT",
    "INJECTED_SCHEMATIC",
    "SCHEMATIC_INPUT",
    "FORMAT",
    "PATH",
    "VAR",
    "LVS_CELL_GROUP",
    "ADD_POLYGON",
    "ADD_TEXT",
    "LAYOUT_ADD_POLYGON",
    "LAYOUT_ADD_TEXT",
    "DEVICE_FILTER",
    "SHORT",
    "OPEN",
    "SCHEMATIC",
    "DEVICE_FILTER_UNUSED",
    "BJT",
    "CAP",
    "DIO",
    "MOS",
    "RES",
    "DEVICE_FILTER_OPTION",
    "CONNECTED_GSD",
    "FLOATING_G_POWER_SD",
    "FLOATING_G_GROUND_SD",
    "FLOATING_GSD",
    "CONNECTED_SD",
    "CONNECTED_ALL",
    "NOPATH_G_FLOATING_SORD",
    "NOPATH_G_FLOATING_S_OR_D",
    "SUPPLY_G_FLOATING_SORD",
    "POWER_GROUND_G_FLOATING_S_OR_D",
    "FLOATING_G_GROUNDPATH_SD",
    "FLOATING_G_GROUND_PATH_SD",
    "FLOATING_G_POWERPATH_SORD",
    "FLOATING_G_POWER_PATH_S_OR_D",
    "GROUND_NGATE",
    "GROUND_NGATE2",
    "POWER_PGATE",
    "POWER_PGATE2",
    "ONLYPOWER_SD",
    "ONLY_POWER_SD",
    "ONLYGROUND_SD",
    "ONLY_GROUND_SD",
    "POWERGROUND_SD",
    "POWER_GROUND_SD",
    "SUPPLY_G_CONNECTED_SD",
    "POWER_GROUND_G_CONNECTED_SD",
    "NOPATH_SD",
    "ONLYFLOATING_SORD",
    "ONLY_FLOATING_S_OR_D",
    "GROUND_PGATE2",
    "POWER_NGATE2",
    "FILTERALL",
    "FILTER_ALL",
    "REPEAT_FILTER_ALL",
    "NOPATH_DEVICE",
    "SUPPLY_DEVICE",
    "POWER_GROUND_DEVICE",
    "FLOATING_R",
    "CONNECTED_R",
    "FLOATING_C",
    "CONNECTED_C",
    "FLOATING_D",
    "CONNECTED_D",
    "FLOATING_Q",
    "CONNECTED_EC",
    "FLOATING_SD",
    "FLOATING_MOS",
    "CONNECTED_BE",
    "CONNECTED_Q",
    "FLOATINGALL_R",
    "FLOATINGALL_C",
    "FLOATINGALL_D",
    "FLOATINGALL_Q",
    "FLOATING_R_ALL",
    "FLOATING_C_ALL",
    "FLOATING_D_ALL",
    "FLOATING_Q_ALL",
    "DEVICE_MAP",
    "DEVICE_MERGE_PRIORITY",
    "DEVICE_PIN_SWAP",
    "CAP",
    "MOS_RES",
    "DEVICE_PROPERTY",
    "MAP",
    "INIT",
    "LVS_CHECK_PROPERTY_RADIUS",
    "DEVICE_TYPE_MAP",
    "ERC_FIND_SHORT",
    "ERC_MAX_SHORTS",
    "ERC_FIND_SHORTS_IGNORE_NETS",
    "BY_CELL_ALSO",
    "BY_LAYER_ALSO",
    "ALL_CELL",
    "IGNORE_CONTACT",
    "KEEP_UNMERGED",
    "HOT_SPOT",
    "FLAT",
    "AMONG_NET",
    "AMONG_NAME",
    "IGNORE_NET",
    "ERC_FIND_OPEN",
    "ERC_FIND_OPEN_NETS",
    "ERC_FIND_OPEN_IGNORE_NETS",
    "ERC_MAX_OPENS",
    "ERC_MAX_NETS_PER_OPEN",
    "IGNORE_CONTACT",
    "MAX_NETS_PER_OPEN",
    "FPD_SHOT_ANALYSIS",
    "GLASS_CELL",
    "MASK_GDS",
    "MASK_LAYER",
    "MASK_PITCH",
    "MASK_ROW_COL",
    "FIRST_MASK_CENTER_IN_GLASS",
    "MASK_CELL_INFO",
    "MASK_CELL",
    "MARK_FILTER_FILE",
    "ERC_PATH_CHECK",
    "PATH_CHECK",
    "OUTPUT_POLYGON",
    "OUTPUT_NET",
    "WITH_PORT",
    "IGNORE_FLOATING",
    "IGNORE_FLOATTING",
    "ONLY_USED",
    "EXCLUDE_POWER_GROUND",
    "POWERED",
    "GROUNDED",
    "LABELED",
    "POWER",
    "GROUND",
    "LABEL",
    "STOP",
    "ERC_PATH_DEVICE",
    "CELL",
    "IN_CELL",
    "HEAD_NAME",
    "IGNORE_ORIGINAL_LAYER_CHECK",
    "ACUTE_EDGE",
    "ANGLED_EDGE",
    "OFFGRID_VERTEX",
    "SKEW_EDGE",
    "LAYER_DIR",
    "LAYOUT_CONNECT_LAYER",
    "LAYOUT_DEVICE_CELL",
    "LAYOUT_DEVICE_LAYER",
    "LAYOUT_READ_LEVEL",
    "LAYOUT_READ",
    "DUPLICATE_CELL",
    "BOX_RECORD",
    "NODE_RECORD",
    "LVS_BLACK_BOX",
    "IND",
    "LEN",
    "RES",
    "TIME",
    "VIRTUAL_CONNECT",
    "BLACK_BOX_COLON",
    "BLACK_BOX_NAME",
    "COLON",
    "LEVEL",
    "PROGRESSIVE_CONNECT",
    "INCREMENTAL_CONNECT",
    "NAME",
    "REPORT_WARNING",
    "REPORT_MAX",
    "SEMICOLON",
    "UNICONNECT",
    "FLOATING_POLYGON_NAME",
    "ADJACENT",
    "WITH",
    "QUIT",
    "LAYOUT_ERROR",
    "ERC_ERROR",
    "UNICONNECT_CHECK",
    "POWER_GROUND_ERROR",
    "COMPILE_ERROR",
    "PUSH_CELL_LOWEST",
    "SPICE",
    "XSCALE",
    "SWAP_WL",
    "STRICT_WIDTH_LENGTH",
    "STRICT_WL",
    "TREAT_SLASH_AS_SPACE",
    "SPLIT_MULTIPLER_DEVICE",
    "RENAME_PARAM",
    "REDEFINE_PARAM_VALUE",
    "USE_SUBCELL_PIN",
    "REWRITE_GLOBAL_NET",
    "MULTIPLIER_FACTOR_NAME",
    "CALCULATE_MOS_AREA",
    "IGNORE_PRIMITIVE_SUBCIRCUIT",
    "CONDITIONAL_COMMENT_LDD",
    "PERMIT_UNQUOTED_STRING",
    "PERMIT_INLINE_PARAMETER",
    "PERMIT_DUPLICATE_SUBCIRCUIT",
    "PERMIT_FLOATING_PIN",
    "SWAP_WL",
    "STRICT_WIDTH_LENGTH",
    "STRICT_WL",
    "TREAT_SLASH_AS_SPACE",
    "SPLIT_MULTIPLER_DEVICE",
    "RENAME_PARAM",
    "REDEFINE_PARAM_VALUE",
    "USE_SUBCELL_PIN",
    "REWRITE_GLOBAL_NET",
    "MULTIPLIER_FACTOR_NAME",
    "CALCULATE_MOS_AREA",
    "IGNORE_PRIMITIVE_SUBCIRCUIT",
    "CONDITIONAL_COMMENT_LDD",
    "PERMIT_UNQUOTED_STRING",
    "PERMIT_INLINE_PARAMETER",
    "PERMIT_DUPLICATE_SUBCIRCUIT",
    "PERMIT_FLOATING_PIN",
    "FLOATING_PIN",
    "DUPLICATE_NAME",
    "KEEP_NAME",
    "FLOATING_PIN",
    "DUPLICATE_NAME",
    "KEEP_NAME",
    "JFET",
    "IND",
    "X_INSTANCE",
    "VOLTAGE",
    "SBUCKT",
    "PARAM",
    "LEFT_SIDE",
    "RIGHT_SIDE",
    "COMMENT_CODED",
    "LVS_IDENTIFY_GATE",
    "BASIC",
    "DIFFERENT_SUBTYPE",
    "DIFF_SUBTYPE",
    "X_SUBTYPE",
    "IN_TOLERANCE",
    "CELL_GROUP",
    "DRC_WAIVE_ERROR",
    "NETLIST",
    "EXTRACT_ALL_TEXTED_PIN",
    "EXTRACT_INCONSISTENT_MODEL",
    "EXTRACT_BLACK_BOX_CONTENT",
    "PROPERTY_IN_COMMENT",
    "SUBSTRATE_IN_COMMENT",
    "UNNAMED_BLACK_BOX_PIN",
    "EXTRACT_PIN_LOCATION",
    "PROMOTE_CELL",
    "PROMOTE_CELL_TEXT",
    "LAYOUT_CELL_GROUP",
    "WITH_TEXT",
    "LAYOUT_REMOVE_TEXT",
    "LAYOUT_RENAME_TEXT",
    "CELL_GROUP",
    "IN_DB",
    "IN_RULE",
    "IN_LAYER",
    "LAYOUT_EXCEPTION",
    "SEVERITY",
    "RESULT_DB",
    "SORT_BY_CELL",
    "SORT_BY_LAYER",
    "SORT_BY_EXCEPTION",
    "IN_TOP",
    "IN_CELL",
    "MAG_DB",
    "BOX_RECORD",
    "NSTRING_CELL_NAME",
    "NODE_RECORD",
    "NSTRING_PROPNAME",
    "ASTRING_TEXTSTRING",
    "XELEMENT",
    "XNAME",
    "LAYOUT_ADD_TEXT",
    "TEXT_LARGE",
    "TEXT_DEFECTIVE",
    "METRIC_LAYOUT",
    "METRIC_RULE_DECK",
    "PRECISION_LAYOUT",
    "PRECISION_CONFLICT",
    "PRECISION_RULE_DECK",
    "PRECISION_DB",
    "LAYOUT_RESERVE_CELL_GROUP",
    "LAYOUT_RENAME_CELL",
    "LAYOUT_EXCLUDE_WINDOW",
    "REGION",
    "ORIGINAL_LAYER",
    "USED_LAYER",
    "LAYOUT_SELECT_WINDOW",
    "CLIP",
    "ORIGINAL",
    "USED",
    "ORIGINAL_LAYER",
    "USED_LAYER",
    "LVS_CORRESPONDING_NET",
    "LVS_IGNORE_HCELL",
    "LVS_EXPAND_HCELL",
    "PROMOTION",
    "DEVICE_PROMOTION",
    "UNBALANCE",
    "LVS_LAYOUT_GLOBAL_NET_GROUP",
    "LVS_TEMP_DIR",
    "LVS_IGNORE_PORT",
    "SUBCELL",
    "UNICONNECT_CHECK",
    "LOWER_LAYER",
    "VIA_LAYER",
    "UPPER_LAYER",
    "ALL_LAYER",
    "LAYOUT_MAGNIFY",
    "NEW_CELL",
    "DEFAULT",
    "GEOM_AND",
    "GEOM_OR",
    "GEOM_XOR",
    "GEOM_NOT",
    "EDGE_OR",
    "SAME_NET",
    "DIFF_NET",
    "GEOM_CUT",
    "~GEOM_CUT",
    "GEOM_INSIDE",
    "~GEOM_INSIDE",
    "GEOM_OUTSIDE",
    "~GEOM_OUTSIDE",
    "GEOM_INTERACT",
    "~GEOM_INTERACT",
    "GEOM_ADJACENT",
    "~GEOM_ADJACENT",
    "GEOM_ENCLOSE",
    "~GEOM_ENCLOSE",
    "COUNT_BY_NET",
    "POINT_TOUCH",
    "ONLY_POINT_TOUCH",
    "ATTACH_ORDER",
    "WITH",
    "GEOM_AREA",
    "~GEOM_AREA",
    "GEOM_DONUT",
    "~GEOM_DONUT",
    "GEOM_PERIMETER",
    "GEOM_VERTEX",
    "GEOM_FLATTEN",
    "GEOM_WITH_WIDTH",
    "~GEOM_WITH_WIDTH",
    "GEOM_WITH_EDGE",
    "~GEOM_WITH_EDGE",
    "EDGE_ANGLE",
    "~EDGE_ANGLE",
    "EDGE_LENGTH",
    "~EDGE_LENGTH",
    "EDGE_PATH_LENGTH",
    "EDGE_ADJACENT",
    "~EDGE_ADJACENT",
    "INSIDE",
    "OUTSIDE",
    "BOTH_SIDE",
    "INSIDE",
    "OUTSIDE",
    "BOTH_SIDE",
    "POINT",
    "ONLY_POINT",
    "EDGE_INSIDE",
    "~EDGE_INSIDE",
    "EDGE_OUTSIDE",
    "~EDGE_OUTSIDE",
    "EDGE_",
    "~EDGE_",
    "INSIDE",
    "OUTSIDE",
    "BOTH_SIDE",
    "INSIDE",
    "OUTSIDE",
    "BOTH_SIDE",
    "GEOM_RECTANGLE",
    "~GEOM_RECTANGLE",
    "RATIO",
    "ORTHOGONAL",
    "ONLY_ORTHOGONAL",
    "BY_BOUNDARIES",
    "EDGE_CONVEX_POINT",
    "LENGTH",
    "WITH_EDGE_LENGTH",
    "ADJACENT_EDGE_ANGLE1",
    "ADJACENT_EDGE_LENGTH1",
    "ADJACENT_EDGE_ANGLE2",
    "ADJACENT_EDGE_LENGTH2",
    "GEOM_ORTHOGONALIZE_ANGLE",
    "BY_SKEW_EDGE",
    "ONLY_BY_ORTHOGONAL",
    "ALL_EDGE",
    "CHECK_ORIGINAL_LAYER",
    "ACUTE_EDGE",
    "ANGLED_EDGE",
    "OFFGRID_VERTEX",
    "SKEW_EDGE",
    "IGNORE_ACUTE_EDGE",
    "IGNORE_ANGLED_EDGE",
    "IGNORE_OFFGRID_VERTEX",
    "IGNORE_SKEW_EDGE",
    "GEOM_HOLES",
    "INNER_MOST",
    "HOLLOW",
    "GEOM_SIZE",
    "FPD_SIZE",
    "DFM_GEOM_SIZE",
    "OUTPUT_OVERLAP_REGION",
    "ZOOM_OUT_IN",
    "ZOOM_IN_OUT",
    "OUT_IN",
    "IN_OUT",
    "INSIDE_OF_LAYER",
    "OUTSIDE_OF_LAYER",
    "CLIP",
    "CLIP_CORNER",
    "GEOM_INSIDE_CELL",
    "~GEOM_INSIDE_CELL",
    "PATTERN_MATCH",
    "WITH_LAYER",
    "GEOM_NET",
    "~GEOM_NET",
    "BY_AREA",
    "GEOM_ENCLOSE_RECTANGLE",
    "~GEOM_ENCLOSE_RECTANGLE",
    "ORTHOGONAL",
    "GEOM_GET_BOUNDARY",
    "GEOM_GET_BOUNDARIES",
    "OUTPUT_CENTER",
    "OUTPUT_SQUARE",
    "INSIDE_OF_LAYER",
    "SPACE",
    "WIDTH",
    "OVERLAP",
    "EXTENSION",
    "REDUCE_FALSE",
    "SAME_POLYGON",
    "DIFF_POLYGON",
    "NO_SHIELDED_EDGE",
    "COIN_EDGE_ALSO",
    "ACUTE_ALSO",
    "ACUTE_ONLY",
    "NOT_ACUTE",
    "PARA_ALSO",
    "PARA_ONLY",
    "NOT_PARA",
    "PERP_ALSO",
    "PERP_ONLY",
    "NOT_PERP",
    "OBTUSE_ALSO",
    "OBTUSE_ONLY",
    "NOT_OBTUSE",
    "APPOSITION",
    "NO_APPOSITION",
    "ANGLED_EDGE",
    "CORNER_CORNER",
    "CORNER_EDGE",
    "ALL_CORNER",
    "NOT_CORNER",
    "INTERSECTING_ONLY",
    "ADJACENT",
    "OVERLAPPED",
    "POINT_TOUCH",
    "INSIDE_ALSO",
    "OUTSIDE_ALSO",
    "REGION_BOUNDARIES",
    "REGION_CENTERLINE",
    "SHIELDED_LEVEL",
    "CHECK_DENSITY",
    "BY_WINDOW",
    "DELTA",
    "BOUNDARY_TRUNCATE",
    "BOUNDARY_BACKUP",
    "BOUNDARY_IGNORE",
    "BOUNDARY_REPEAT",
    "INSIDE_OF_BOUNDARY",
    "INSIDE_OF_REGION",
    "INSIDE_OF_LAYER",
    "IN_BOUNDARY",
    "IN_BOUNDARIES",
    "IN_POLYGON",
    "IN_RECTANGLE",
    "IN_BOUNDARIES_CENTER",
    "CHECK_GRADIENT",
    "CHECK_GRADIENT_IN_SEPARATE_WINDOW",
    "CHECK_SEPARATE_GRADIENT",
    "RELATIVE_RATIO",
    "ABSOLUTE_VALUE",
    "OUTPUT_CENTER",
    "OUTPUT_LOG",
    "ONLY_OUTPUT_LOG",
    "RESULT_DB",
    "ONLY_RESULT_DB",
    "MAG",
    "CORNER_ALSO",
    "CHECK_NAR",
    "CHECK_INCREMENTAL_NAR",
    "MAG_BY",
    "COUNT",
    "PERIMETER",
    "DIVIDE",
    "INSIDE_OF_LAYER",
    "INCREMENTAL",
    "RESULT_DB",
    "RESULT_DB_ONLY",
    "ONLY_RESULT_DB",
    "MAG",
    "OUTPUT_BY_LAYER",
    "CHECK_OFFGRID",
    "INSIDE_OF_LAYER",
    "NEW_COORD",
    "ORIGINAL_COORD",
    "RELATIVE_RATIO",
    "ABSOLUTE_VALUE",
    "OUTPUT_CENTER",
    "OUTPUT_EDGE",
    "OUTPUT_REGION",
    "OFFSET_VALUE",
    "INSIDE_VALUE",
    "OUTSIDE_VALUE",
    "WITH_HINT",
    "DEVICE_SEED_LAYER",
    "WITH_PROPERTY",
    "ONLY_USED",
    "ENC_RECT",
    "OUTSIDE_ALSO",
    "ADJACENT",
    "POINT_TOUCH",
    "ONLY_ORTHOGONAL",
    "CORRECT",
    "INCORRECT",
    "GEOM_EDGE_TO_RECT",
    "INSIDE_BY_FACTOR",
    "OUTSIDE_BY_FACTOR",
    "BOTH_SIDE",
    "BOTH_SIDE_BY_FACTOR",
    "EXTEND",
    "EXTEND_BY_FACTOR",
    "FILL_CORNER",
    "GEOM_FILL_RECTANGLES",
    "INSIDE_OF_LAYER",
    "INSIDE_OF_REGION",
    "DELTA",
    "KEEP_SPACING",
    "GEOM_GET_CELL_BOUNDARY",
    "ORIGINAL_LAYER",
    "MAPPED_LAYER",
    "USED_LAYER",
    "PATTERN_MATCH",
    "GEOM_GET_LAYOUT_BOUNDARY",
    "ORIGINAL_LAYER",
    "IGNORE_LAYER",
    "GEOM_MAGNIFY",
    "GEOM_MERGE",
    "GEOM_ROTATE",
    "GEOM_SHIFT",
    "GEOM_SNAP",
    "GEOM_OR_NET",
    "KEEP_SHAPE",
    "GEOM_ORTH_SIZE",
    "IN_ORDER",
    "GEOM_PUSH",
    "WITH_LAYER",
    "LEVEL",
    "GEOM_TEXT_TO_RECT",
    "BY_LEVEL",
    "TOP_CELL",
    "KEEP_CASE",
    "GEOM_TRANSFER_NETID",
    "INCLUDE_ADJACENT",
    "GEOM_WITH_ADJACENT",
    "~GEOM_WITH_ADJACENT",
    "DISTANCE",
    "SQUARE",
    "FROM_CENTER",
    "OCTAGONAL_RECT",
    "ORTHOGONAL_RECT",
    "INSIDE_OF_LAYER",
    "GEOM_WITH_LABEL",
    "~GEOM_WITH_LABEL",
    "KEEP_CASE",
    "CASE_SENSITIVE",
    "TABLE_DRC",
    "REGION",
    "NO_SHIELDED_EDGE",
    "APPOSITION",
    "CHECK_SPACE",
    "CHECK_WIDTH",
    "WITH_WIDTH1",
    "WITH_WIDTH2",
    "SHIELDED_LEVEL",
    "DFM_CREATE_NAR",
    "BY_PROPERTY",
    "DFM_MERGE_PROPERTY",
    "ADJACENT",
    "MULTI_CLUSTER",
    "SINGLE_CLUSTER",
    "DFM_BUILD_PROPERTY",
    "CLIP",
    "OVERLAPPED",
    "ADJACENT",
    "POINT_TOUCH",
    "MULTI_CLUSTER",
    "SINGLE_CLUSTER",
    "REGION",
    "DFM_RESULT",
    "FIRST_NODE",
    "ALL_NODE",
    "OTHER_NODE",
    "FLATTEN_INJECTION",
    "IGNORE_EMPTY",
    "OUTPUT_BOUNDARY",
    "MAX_RESULT",
    "DFM_COPY",
    "REGION",
    "UNMERGED_REGION",
    "MIDDLE_LINE",
    "CONNECTING_LINE",
    "CONNECTING_MIDPOINT",
    "EDGE_COLLECTION",
    "LAYER_ID",
    "UNMERGED_EDGE",
    "CELL_GROUP",
    "DFM_CHECK_SPACE",
    "DELTA",
    "BY_EXT",
    "BY_INT",
    "BY_ENC",
    "BY_ALL",
    "X_DIRECTION",
    "Y_DIRECTION",
    "SHIELD_LEVEL",
    "SHIELD_LAYER",
    "CHECK_ALL",
    "DFM_TRANSFER_NETID",
    "OPC_BIAS",
    "LIMIT",
    "CONVEX_LIMIT",
    "CONCAVE_LIMIT",
    "SLANT_LIMIT",
    "SLANT_CONVEX_LIMIT",
    "SLANT_CONCAVE_LIMIT",
    "DIAGONAL",
    "WITH_SPACE",
    "WITH_WIDTH1",
    "WITH_WIDTH2",
    "WITH_LENGTH1",
    "WITH_LENGTH2",
    "BIAS",
    "DEVICE_GROUP",
    "TRS_FUNCTION",
    "DEFINE_FUN",
    "CALL_FUN",
    "FPD_PIXEL_CELL",
    "FPD_PANEL_CELL",
    "FPD_MASK_CELL",
    "FPD_MASK_SHOT_CELL",
    "FPD_MASK_GLASS_CELL",
    "REMOVE_CONNECT",
};

static bool
isPvrsKeyword(const std::string &sName)
{
    if(0 == strncasecmp(sName.c_str(), "DRC_", 4) || 0 == strncasecmp(sName.c_str(), "ERC_", 4) ||
       0 == strncasecmp(sName.c_str(), "LAYOUT_", 7) || 0 == strncasecmp(sName.c_str(), "GEOM_", 5) ||
       0 == strncasecmp(sName.c_str(), "~GEOM_", 6) || 0 == strncasecmp(sName.c_str(), "LVS_", 4) ||
       0 == strncasecmp(sName.c_str(), "DFM_", 4) || 0 == strncasecmp(sName.c_str(), "FPD_", 4))
        return true;



    const char *pValue = g_warpper_getenv("PVCONVERTOR_HOME");
    if(NULL != pValue)
    {
        string strPvrsKeyWordCfg(pValue);
        strPvrsKeyWordCfg.append("/pvconvertor/config/pvrsKeyWord.list");
        std::ifstream fRead;
        fRead.open(strPvrsKeyWordCfg.c_str());
        string strTmp;
        while(getline(fRead,strTmp))
        {
            trim(strTmp);
            if(0 == strcasecmp(sName.c_str(), strTmp.c_str()))
            {
                return true;
            }
        }
        fRead.close();
    }


    for(hvUInt32 index = 0; index < PVRS_KEYWORD_COUNT; ++index)
    {
        const char* prefix = rcsPvrsKeyword[index];
        if(0 == strcasecmp(sName.c_str(), prefix))
        {
            return true;
        }
    }
    return false;
}

bool
rcsSynPvrsConvertor::getStringName(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end, std::string &sValue)
{
    if (iter == end)
        return false;

    switch(iter->eType)
    {
        case SECONDARY_KEYWORD:
        case IDENTIFIER_NAME:
        {
            if (!isValidSvrfName(iter->sValue))
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
        }
        case STRING_CONSTANTS:
        {
            sValue = parseTokenString(*iter);
            break;
        }
        case SPECIFICATION_KEYWORD:
            if(strcasecmp(iter->sValue.c_str(), "TRACE PROPERTY") == 0)
            {
                iter++;
                sValue = parseTokenString(*iter);
            }
            break;
        default:
            return false;
    }

    if (iter->eType == STRING_CONSTANTS)
    {
        sValue.insert(0, "\"");
        sValue.insert(sValue.size(), "\"");
    }
    else if (iter->eType != STRING_CONSTANTS)
    {
        bool hasSpecificSymbol = false;
        hvUInt32 iValue = 0;
        while(iValue < sValue.size())
        {
            if(sValue[iValue] == ';' || sValue[iValue] == ' ')
            {
                hasSpecificSymbol = true;
                break;
            }
            ++iValue;
        }

        if(hasSpecificSymbol)
        {
            sValue.insert(0, "\"");
            sValue.insert(sValue.size(), "\"");
        }
        else if(strcasecmp(sValue.c_str(), "RULE") == 0 || isPvrsKeyword(sValue))
        {
            sValue.insert(0, "\"");
            sValue.insert(sValue.size(), "\"");
        }
    }
    if (!iter->namescopes.empty())
    {
        return true;
    }
    return true;
}

static inline bool
isConstraintOperator(const rcsToken_T &token)
{
    if (token.eType != OPERATOR)
        return false;
    const std::string &sOperator = token.sValue;
    return ("==" == sOperator) || (">" == sOperator) || ("<" == sOperator) ||
           ("!=" == sOperator) || (">=" == sOperator) || ("<=" == sOperator);
}

bool
rcsSynPvrsConvertor::parseFollowNums(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end,
                                     std::vector<std::string> &vNumExps, bool (*isValidKey)(const rcsToken_T &),
                                     const hvUInt32 nMinCount, const hvUInt32 nMaxCount)
{
    hvUInt32 nCount = 0;
    std::list<rcsToken_T>::iterator begin = iter++;
    while(nCount < nMaxCount)
    {
        if (iter != end && !isValidKey(*iter) && !isConstraintOperator(*iter))
        {
            std::string sNumExp;
            if (!getNumberExp(iter, end, sNumExp))
            {
                return false;
            }

            vNumExps.insert(vNumExps.end(), sNumExp);
            ++nCount;
        }
        else if (nCount >= nMinCount)
        {
            return true;
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP11,
                                begin->nLineNo, begin->sValue);
            return false;
        }
    }
    return true;
}

void
rcsSynPvrsConvertor::beginVisitLayerDefinitionNode(rcsSynLayerDefinitionNode_T *pNode)
{
    std::map<std::string, rcsSynLayerDefinitionNode_T*>& _tmpLayers = rcsManager_T::getInstance()->getTmpLayers();
    if(_tmpLayers.find(pNode->begin()->sValue) != _tmpLayers.end())
    {
        return;
    }

    m_isInLayerDefinition = true;
    std::string sLayerName;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    getLayerName(iter, pNode->end(), sLayerName);
    if(strncasecmp(sLayerName.c_str(),
       rcsManager_T::getInstance()->getImplicitLayerPrefix().c_str(),
       rcsManager_T::getInstance()->getImplicitLayerPrefix().size()) == 0)
    {
        m_isTmpLayerDefinition = true;
        if(m_hasGetTmpLayerValue && !rcsManager_T::getInstance()->needExpandTmpLayer())
            return;
    }
    else if(!m_hasGetTmpLayerValue)
    {
        return;
    }

    if(m_hasGetTmpLayerValue && rcsManager_T::getInstance()->isSvrfComment() &&
       !rcsManager_T::getInstance()->isTvf2Trs())
    {
        std::string sValue;
        pNode->getCmdValue(sValue);
#if 0
        if(rcsManager_T::getInstance()->isTvf2Trs())
            (*m_oPVRSStream) << "trs::";
#endif
        (*m_oPVRSStream) << "//" << sValue << "= ";
        std::string sValueCmd;
        pNode->getLayerOperationNode()->getCmdValue(sValueCmd);
        (*m_oPVRSStream) << sValueCmd << std::endl;
    }

    if(!m_hasGetTmpLayerValue && m_isTmpLayerDefinition &&
       !rcsManager_T::getInstance()->needExpandTmpLayer())
    {
        m_sTmpLayerName = sLayerName;
        std::ostream *pStream = new std::ostringstream(m_vTmpLayerValues[sLayerName]);
        m_oPVRSStream = pStream;
    }
    else
    {
        outputBlankLinesBefore(pNode->getNodeBeginLineNo(), !m_isTmpLayerDefinition);

        if(!rcsManager_T::getInstance()->isTvf2Trs())
        {
            (*m_oPVRSStream) << sLayerName << " = ";
        }
        else
        {
            std::string prefix ;
            if(rcsManager_T::getInstance()->isCallFromSetLayerCommand() ||
                    rcsManager_T::getInstance()->isTmpLayer(sLayerName))
            {
                prefix = "trs::setlayer ";
            }
            else
            {
                prefix = "";
            }














            (*m_oPVRSStream) << prefix << sLayerName << " = ";
        }
        
    }

    if(pNode->getLayerOperationNode()->isCMacro())
    {
        if(rcsManager_T::getInstance()->isSvrfComment())
        {
            std::string sValue;
            
            
            
        }
    }
}

void
rcsSynPvrsConvertor::endVisitLayerDefinitionNode(rcsSynLayerDefinitionNode_T *pNode)
{
    if (pNode->getLayerOperationNode()->isCMacro())
    {
        ;
    }
    std::map<std::string, rcsSynLayerDefinitionNode_T*>& _tmpLayers = rcsManager_T::getInstance()->getTmpLayers();
    if(_tmpLayers.find(pNode->begin()->sValue) != _tmpLayers.end())
    {
        return;
    }


        (*m_oPVRSStream) << std::endl;

    if(!m_hasGetTmpLayerValue && m_isTmpLayerDefinition &&
            !rcsManager_T::getInstance()->needExpandTmpLayer())
    {
        Utassert(!static_cast<std::ostringstream *>(m_oPVRSStream)->str().empty());
        std::string tmplayerstr = static_cast<std::ostringstream *>(m_oPVRSStream)->str();
        delete m_oPVRSStream;
        if (!tmplayerstr.empty() && tmplayerstr[tmplayerstr.size()-1] == '\n')
            tmplayerstr.resize(tmplayerstr.size()-1);
        m_vTmpLayerValues[m_sTmpLayerName] = tmplayerstr;

        m_oPVRSStream = &(rcsManager_T::getInstance()->getPVRSOutStream());
        m_sTmpLayerName.clear();
    }

    m_isInLayerDefinition = false;
    m_isTmpLayerDefinition = false;
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
        if (iter->first > nEnd)
            break;

        
        if (_isBlankLine(iter->second))
            continue;

        sCheckText += iter->second;

        std::map<hvUInt32, std::string>::const_iterator iComment =
                vCheckComment.find(iter->first);
        if (iComment != vCheckComment.end())
        {
            std::string sResult;
            if (!_replaceComment(iComment->second, sResult, true, rcsManager_T::getInstance()->isTvf2Trs()))
            {
                rcErrManager_T::Singleton().addError
                        (rcErrorNode_T(rcErrorNode_T::ERROR, RES1, iter->first, sResult));
                sCheckComment += iter->second;
            }
            else
            {
                std::string sValue = sResult;
                trim(sResult, "\r ");
                if (!_isBlankLine(sValue))
                {
                    sCheckComment += sValue;
                }

                sCheckText.erase(sCheckText.size() - iComment->second.size());
                sCheckText += sResult;
            }
        }
    }
}

static const char* vCommonWords[] = {
        "able",
        "about",
        "above",
        "abroad",
        "absolutely",
        "across",
        "actually",
        "after",
        "again",
        "against",
        "ago",
        "ahead",
        "all",
        "almost",
        "alone",
        "along",
        "already",
        "also",
        "although",
        "altogether",
        "always",
        "am",
        "among",
        "and",
        "any",
        "anybody",
        "anyone",
        "anything",
        "anyway",
        "anywhere",
        "apart",
        "are",
        "around",
        "as",
        "aside",
        "at",
        "automatically",
        "away",
        "back",
        "bad",
        "basically",
        "be",
        "because",
        "been",
        "before",
        "behind",
        "below",
        "best",
        "better",
        "between",
        "beyond",
        "big",
        "black",
        "both",
        "briefly",
        "bright",
        "but",
        "by",
        "can",
        "cannot",
        "carefully",
        "certain",
        "certainly",
        "cheap",
        "clean",
        "clear",
        "clearly",
        "close",
        "closely",
        "collect",
        "completely",
        "concerning",
        "constantly",
        "cool",
        "could",
        "currently",
        "daily",
        "deeply",
        "definitely",
        "deliberately",
        "despite",
        "did",
        "different",
        "direct",
        "directly",
        "do",
        "does",
        "done",
        "down",
        "downtown",
        "during",
        "each",
        "early",
        "easily",
        "easy",
        "economic",
        "effectively",
        "either",
        "else",
        "elsewhere",
        "enough",
        "equally",
        "especially",
        "essentially",
        "eventually",
        "ever",
        "everybody",
        "everyone",
        "everything",
        "everywhere",
        "exactly",
        "except",
        "extra",
        "extremely",
        "fair",
        "fairly",
        "false",
        "far",
        "fast",
        "federal",
        "few",
        "finally",
        "fine",
        "first",
        "flat",
        "following",
        "for",
        "forever",
        "forth",
        "forward",
        "free",
        "frequently",
        "fresh",
        "from",
        "full",
        "fully",
        "generally",
        "gently",
        "good",
        "great",
        "greatly",
        "gross",
        "hard",
        "hardly",
        "has",
        "have",
        "he",
        "her",
        "here",
        "hers",
        "herself",
        "high",
        "highly",
        "him",
        "himself",
        "his",
        "honest",
        "honestly",
        "hopefully",
        "how",
        "however",
        "human",
        "i",
        "ill",
        "immediately",
        "important",
        "in",
        "including",
        "initially",
        "instead",
        "international",
        "into",
        "is",
        "it",
        "item",
        "its",
        "itself",
        "just",
        "large",
        "last",
        "late",
        "later",
        "least",
        "left",
        "less",
        "like",
        "likely",
        "literally",
        "little",
        "local",
        "long",
        "lot",
        "loud",
        "low",
        "lower",
        "mainly",
        "major",
        "many",
        "may",
        "maybe",
        "me",
        "merely",
        "might",
        "military",
        "mine",
        "more",
        "moreover",
        "morning",
        "most",
        "mostly",
        "much",
        "must",
        "my",
        "myself",
        "national",
        "naturally",
        "near",
        "nearby",
        "nearly",
        "necessarily",
        "need",
        "neither",
        "never",
        "new",
        "next",
        "no",
        "nobody",
        "none",
        "nor",
        "normally",
        "not",
        "nothing",
        "now",
        "nowhere",
        "obviously",
        "occasionally",
        "of",
        "off",
        "often",
        "ok",
        "old",
        "on",
        "once",
        "one",
        "only",
        "or",
        "originally",
        "other",
        "otherwise",
        "ought",
        "our",
        "ours",
        "ourselves",
        "out",
        "over",
        "overall",
        "perfectly",
        "personally",
        "physically",
        "please",
        "plus",
        "political",
        "possible",
        "possibly",
        "pray",
        "pretty",
        "previously",
        "primarily",
        "probably",
        "proper",
        "properly",
        "public",
        "quick",
        "quickly",
        "quite",
        "rarely",
        "rather",
        "readily",
        "real",
        "really",
        "recent",
        "recently",
        "regularly",
        "relatively",
        "right",
        "roughly",
        "seriously",
        "several",
        "shall",
        "she",
        "short",
        "should",
        "significantly",
        "similarly",
        "simply",
        "since",
        "slightly",
        "slow",
        "slowly",
        "small",
        "smooth",
        "so",
        "social",
        "some",
        "somebody",
        "somehow",
        "someone",
        "something",
        "sometimes",
        "somewhat",
        "somewhere",
        "soon",
        "special",
        "specifically",
        "straight",
        "strange",
        "strong",
        "strongly",
        "successfully",
        "such",
        "suddenly",
        "super",
        "sure",
        "terribly",
        "that",
        "the",
        "their",
        "theirs",
        "them",
        "themselves",
        "then",
        "there",
        "therefore",
        "these",
        "they",
        "thick",
        "thin",
        "this",
        "those",
        "though",
        "through",
        "throughout",
        "thus",
        "tight",
        "till",
        "to",
        "today",
        "together",
        "tomorrow",
        "tonight",
        "too",
        "totally",
        "tourist",
        "towards",
        "true",
        "truly",
        "twice",
        "ultimately",
        "under",
        "unfortunately",
        "unless",
        "until",
        "up",
        "upon",
        "us",
        "usually",
        "very",
        "virtually",
        "was",
        "we",
        "weekly",
        "well",
        "were",
        "what",
        "whatever",
        "when",
        "whenever",
        "where",
        "wherever",
        "whether",
        "which",
        "whichever",
        "while",
        "white",
        "who",
        "whoever",
        "whole",
        "whom",
        "whomever",
        "whose",
        "why",
        "wide",
        "widely",
        "wild",
        "will",
        "with",
        "within",
        "without",
        "would",
        "wrong",
        "yeah",
        "yesterday",
        "yet",
        "you",
        "young",
        "your",
        "yours",
        "yourself",
        "yourselves",
};

namespace
{
struct ltString
{
    bool operator()(const char* pValue1, const char* pValue2)const
    {
        return (strcmp(pValue1, pValue2) < 0);
    }
    bool operator()(const std::string& sValue1, const std::string& sValue2)const
    {
        return (strcmp(sValue1.c_str(), sValue2.c_str()) < 0);
    }
};
}

static inline bool
__isCommonWord(const std::string &sWord)
{
    std::string sLowerWord = sWord;
    toLower(sLowerWord);
    static hvUInt32 nCommonWords = sizeof(vCommonWords) / sizeof(vCommonWords[0]);
    bool res = std::binary_search(vCommonWords, vCommonWords + nCommonWords, sLowerWord.c_str(), ltString());
    return res;
}



static bool
__isRealKeywordInComment(KEY_TYPE eKeyType,
                         const std::vector<rcsToken_T> &vTokens,
                         bool hasBadTokens)
{
    if (hasBadTokens) return false;

    bool hasCommonWords = false;
    {
        for (std::vector<rcsToken_T>::const_iterator iter = vTokens.begin();
                iter != vTokens.end(); iter++)
        {
            if (iter->eType == IDENTIFIER_NAME && __isCommonWord(iter->sValue))
            {
                hasCommonWords = true;
                break;
            }
        }
    }
    if (hasCommonWords) return false;

    bool isRealKeyword = false;
    switch (eKeyType)
    {
        case AND:
        case ANGLE:
        case AREA:
        case ATTACH:
        case CONNECT:
        case COPY:
        case CUT:
        case DENSITY:
        case ENCLOSE:
        case EXTENT:
        case EXTENTS:
        case EXTERNAL:
        case FLATTEN:
        case GROUP:
        case GROW:
        case HCELL:
        case HOLES:
        case INSIDE:
        case INTERACT:
        case INTERNAL:
        case LAYER:
        case LENGTH:
        case MAGNIFY:
        case MERGE:
        case NET:
        case NOT:
        case OR:
        case OUTSIDE:
        case POLYGON:
        case PRECISION:
        case RECTANGLE:
        case RECTANGLES:
        case RESOLUTION:
        case ROTATE:
        case TEXT:
        case TITLE:
        case TOUCH:
        case VARIABLE:
        case VERTEX:
        case SIZE:
        {
            if (vTokens.size() > 3)
            {
                if (vTokens[0].eType == IDENTIFIER_NAME || vTokens[0].eType == STRING_CONSTANTS)
                {
                    if (vTokens[1].eType == SEPARATOR && vTokens[1].sValue == "=")
                    {
                        isRealKeyword = true;
                    }
                }
            }
            if (!isRealKeyword && !vTokens.empty())
            {
                if (vTokens[0].eKeyType == eKeyType)
                {
                    isRealKeyword = true;
                }
            }
            break;
        }
        default:
            isRealKeyword = true;
            break;
    }

    return isRealKeyword;
}

static void
skipTokenUntilNot(const std::vector<rcsToken_T> & vTokens,
                  std::vector<rcsToken_T>::iterator &iter, TOKEN_TYPE eType)
{
    while (iter != vTokens.end() && iter->eType == eType)
    {
        iter++;
    }
}

static
std::vector<rcsToken_T> removeBlank(const std::vector<rcsToken_T>& tokens, bool *hasBadTokens)
{
    std::vector<rcsToken_T> ret;
    for (std::vector<rcsToken_T>::const_iterator iter = tokens.begin();
            iter != tokens.end(); ++iter)
    {
        if (iter->eType == BLANK_SPACE)
        {
            continue;
        }
        else if (iter->eType == BAD_CHARACTER)
        {
            *hasBadTokens = true;
        }
        else if (iter->eType == SEPARATOR && iter->sValue == ",")
        {
            if (!maybe_in_buildin(tokens, iter))
                *hasBadTokens = true;
        }
        ret.push_back(*iter);
    }
    return ret;
}


static void
replaceKeywordsInComment(const string &sSvrf, std::string &sPvrs)
{
    if (sSvrf.length() < 8 || _isBlankLine(sSvrf))
    {
        sPvrs = sSvrf;
        return;
    }

    bool hasBadTokens = false;
    std::vector<rcsToken_T> vAllTokens = getTokens(sSvrf, &hasBadTokens);
    std::vector<rcsToken_T> vTokensNoWS = removeBlank(vAllTokens, &hasBadTokens);

    std::ostringstream writer;

    for (std::vector<rcsToken_T>::iterator iter = vAllTokens.begin();
            iter != vAllTokens.end(); iter++)
    {
        switch (iter->eKeyType)
        {
            
            case CMACRO: writer << "call_fun"; break;
            case DMACRO: writer << "def_fun"; break;

            
            
            case AND: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "GEOM_AND"; } else {  writer << iter->sValue; } break;
            case ANGLE: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "EDGE_ANGLE"; } else {  writer << iter->sValue; } break;
            case AREA: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "GEOM_AREA"; } else {  writer << iter->sValue; } break;
            case ATTACH: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "ATTACH"; } else {  writer << iter->sValue; } break;
            case COINCIDENT_EDGE: writer << "edge_coincident both_side"; break;
            case COINCIDENT_INSIDE_EDGE: writer << "edge_coincident inside"; break;
            case COINCIDENT_OUTSIDE_EDGE: writer << "edge_coincident outside"; break;
            case CONNECT: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "CONNECT"; } else {  writer << iter->sValue; } break;
            case CONVEX_EDGE: writer << "edge_convex_point"; break;
            case COPY: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "COPY"; } else {  writer << iter->sValue; } break;
            case CUT: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "GEOM_CUT"; } else {  writer << iter->sValue; } break;
            case DEANGLE: writer << "geom_orthogonalize_angle"; break;
            case DENSITY: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "CHECK_DENSITY"; } else {  writer << iter->sValue; } break;
            case DEVICE: writer << "DEVICE"; break;
            case DEVICE_GROUP: writer << "device_group"; break;
            case DEVICE_LAYER: writer << "device_seed_layer"; break;
            case DFM_COPY: writer << "dfm_copy"; break;
            case DFM_FUNCTION: writer << "dfm_function"; break;
            case DFM_NARAC: writer << "dfm_create_nar"; break;
            case DFM_PROPERTY: writer << "dfm_build_property"; break;
            case DFM_PROPERTY_MERGE: writer << "dfm_merge_property"; break;
            case DFM_PROPERTY_NET: writer << "dfm_build_property_by_net"; break;
            case DFM_RDB: writer << "dfm_result"; break;
            case DFM_SELECT_CHECK: writer << "dfm_select_rule by_name"; break;
            case DFM_SIZE: writer << "dfm_geom_size"; break;
            case DFM_SPACE: writer << "dfm_check_space"; break;
            case DFM_STAMP: writer << "dfm_transfer_netid"; break;
            case DFM_TEXT: writer << "dfm_text"; break;
            case DFM_UNSELECT_CHECK: writer << "drc_ignore_rule by_name"; break;
            case DISCONNECT: writer << "remove_connect"; break;
            case DONUT: writer << "geom_donut"; break;
            case DRAWN_ACUTE: writer << "check_original_layer acute_edge"; break;
            case DRAWN_ANGLED: writer << "check_original_layer angled_edge"; break;
            case DRAWN_OFFGRID: writer << "check_original_layer offgrid_vertex"; break;
            case DRAWN_SKEW: writer << "check_original_layer skew_edge"; break;
            case DRC_CELL_NAME: writer << "drc_output_cell_name"; break;
            case DRC_CELL_TEXT: writer << "drc_process_cell_text"; break;
            case DRC_CHECK_MAP: writer << "drc_rule_map"; break;
            case DRC_CHECK_TEXT: writer << "drc_output_rule_text"; break;
            case DRC_ERROR_WAIVE_CELL: writer << "drc_waive_error by_cell"; break;
            case DRC_ERROR_WAIVE_LAYER: writer << "drc_waive_error by_layer"; break;
            case DRC_EXCLUDE_FALSE_NOTCH: writer << "drc_output_reduce_false_error"; break;
            case DRC_ICSTATION: writer << "gui_priority"; break;
            case DRC_MAGNIFY_DENSITY: writer << "drc_magnify_result density"; break;
            case DRC_MAGNIFY_NAR: writer << "drc_magnify_result nar"; break;
            case DRC_MAP_TEXT: writer << "drc_map_text"; break;
            case DRC_MAP_TEXT_DEPTH: writer << "drc_map_text_level"; break;
            case DRC_MAP_TEXT_LAYER: writer << "drc_map_text_layer"; break;
            case DRC_MAXIMUM_CELL_NAME_LENGTH: writer << "drc_max_cell_name"; break;
            case DRC_MAXIMUM_RESULTS: writer << "drc_max_result"; break;
            case DRC_MAXIMUM_RESULTS_NAR: writer << "drc_max_nar_result"; break;
            case DRC_MAXIMUM_VERTEX: writer << "drc_max_vertex"; break;
            case DRC_PRINT_AREA: writer << "print_log area"; break;
            case DRC_PRINT_PERIMETER: writer << "print_log perimeter"; break;
            case DRC_RESULTS_DATABASE: writer << "drc_result db"; break;
            case DRC_RESULTS_DATABASE_LIBNAME: writer << "drc_result libname"; break;
            case DRC_RESULTS_DATABASE_PRECISION: writer << "drc_result precision"; break;
            case DRC_SELECT_CHECK: writer << "drc_select_rule by_name"; break;
            case DRC_SELECT_CHECK_BY_LAYER: writer << "drc_select_rule by_layer"; break;
            case DRC_SUMMARY_REPORT: writer << "drc_summary"; break;
            case DRC_TOLERANCE_FACTOR: writer << "drc_tolerance distance"; break;
            case DRC_TOLERANCE_FACTOR_NAR: writer << "drc_tolerance nar"; break;
            case DRC_UNSELECT_CHECK: writer << "drc_ignore_rule by_name"; break;
            case DRC_UNSELECT_CHECK_BY_LAYER: writer << "drc_ignore_rule by_layer"; break;
            case ENCLOSE: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "geom_enclose"; } else {  writer << iter->sValue; } break;
            case ENCLOSE_RECTANGLE: writer << "geom_enclose_rectangle"; break;
            case ENCLOSURE: writer << "enc"; break;
            case ERC_CELL_NAME: writer << "erc_output_cell_name"; break;
            case ERC_CHECK_TEXT: writer << "erc_output_rule_text"; break;
            case ERC_MAXIMUM_RESULTS: writer << "erc_max_result"; break;
            case ERC_MAXIMUM_VERTEX: writer << "erc_max_vertex"; break;
            case ERC_PATHCHK: writer << "erc_path_check"; break;
            case ERC_PATH_ALSO: writer << "erc_path_device"; break;
            case ERC_RESULTS_DATABASE: writer << "erc_result db"; break;
            case ERC_SELECT_CHECK: writer << "erc_select_rule by_name"; break;
            case ERC_SUMMARY_REPORT: writer << "erc_summary"; break;
            case ERC_UNSELECT_CHECK: writer << "erc_ignore_rule by_name"; break;
            case EXCLUDE_CELL: writer << "ignore_cell"; break;
            case EXPAND_CELL: writer << "promote_cell"; break;
            case EXPAND_CELL_TEXT: writer << "promote_cell_text"; break;
            case EXPAND_EDGE: writer << "geom_edge_to_rect"; break;
            case EXPAND_TEXT: writer << "geom_text_to_rect"; break;
            case EXTENT: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "geom_get_boundary"; } else {  writer << iter->sValue; } break;
            case EXTENTS: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "geom_get_boundaries"; } else {  writer << iter->sValue; } break;
            case EXTENT_CELL: writer << "geom_get_cell_boundary"; break;
            case EXTENT_DRAWN: writer << "geom_get_layout_boundary"; break;
            case EXTERNAL: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "ext"; } else {  writer << iter->sValue; } break;
            case FLAG_NONSIMPLE: writer << "print_log self_intersecting_polygon"; break;
            case FLAG_NONSIMPLE_PATH: writer << "print_log self_intersecting_path"; break;
            case FLATTEN: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "geom_flatten"; } else {  writer << iter->sValue; } break;
            case FLATTEN_CELL: writer << "flatten cell"; break;
            case FLATTEN_INSIDE_CELL: writer << "flatten in_cell"; break;
            case FLATTEN_VERY_SMALL_CELLS: writer << "flatten very_small_cell"; break;
            case GROUP: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "GROUP_RULE"; } else {  writer << iter->sValue; } break;
            case GROW: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "GEOM_ORTH_SIZE"; } else {  writer << iter->sValue; } break;
            case HCELL: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "hcell"; } else {  writer << iter->sValue; } break;
            case HOLES: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "geom_holes"; } else {  writer << iter->sValue; } break;
            case INSIDE: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "GEOM_INSIDE"; } else {  writer << iter->sValue; } break;
            case INSIDE_CELL: writer << "geom_inside_cell"; break;
            case INSIDE_EDGE: writer << "edge_inside"; break;
            case INTERACT: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "geom_interact"; } else {  writer << iter->sValue; } break;
            case INTERNAL: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "int"; } else {  writer << iter->sValue; } break;
            case LABEL_ORDER: writer << "attach_order"; break;
            case LAYER: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "LAYER"; } else {  writer << iter->sValue; } break;
            case LAYER_DIRECTORY: writer << "layer_dir"; break;
            case LAYER_MAP: writer << "layer_map"; break;
            case LAYER_RESOLUTION: writer << "resolution layer"; break;
            case LAYOUT_ALLOW_DUPLICATE_CELL: writer << "layout_read duplicate_cell"; break;
            case LAYOUT_BASE_CELL: writer << "layout_device_cell"; break;
            case LAYOUT_BASE_LAYER: writer << "layout_device_layer"; break;
            case LAYOUT_CASE: writer << "case_sensitive layout"; break;
            case LAYOUT_CELL_LIST: writer << "layout_cell_group"; break;
            case LAYOUT_DEPTH: writer << "layout_read_level"; break;
            case LAYOUT_ERROR_ON_INPUT: writer << "quit layout_error"; break;
            case LAYOUT_IGNORE_TEXT: writer << "layout_remove_text"; break;
            case LAYOUT_INPUT_EXCEPTION_RDB: writer << "layout_exception db"; break;
            case LAYOUT_INPUT_EXCEPTION_SEVERITY: writer << "layout_exception severity"; break;
            case LAYOUT_MAGNIFY: writer << "layout_magnify"; break;
            case LAYOUT_MERGE_ON_INPUT: writer << "layout_merge_on_input"; break;
            case LAYOUT_PATH: writer << "layout_input path"; break;
            case LAYOUT_PATH2: writer << "layout_input2 path"; break;
            case LAYOUT_PLACE_CELL: writer << "layout_add_cell"; break;
            case LAYOUT_POLYGON: writer << "layout_add_polygon"; break;
            case LAYOUT_PRECISION: writer << "precision_db"; break;
            case LAYOUT_PRESERVE_CELL_LIST: writer << "layout_reserve_cell_group"; break;
            case LAYOUT_PRESERVE_PROPERTY_CASE: writer << "case_sensitive layout_property"; break;
            case LAYOUT_PRIMARY: writer << "layout_input top_cell"; break;
            case LAYOUT_PRIMARY2: writer << "layout_input2 top_cell"; break;
            case LAYOUT_PROCESS_BOX_RECORD: writer << "layout_read box_record"; break;
            case LAYOUT_PROCESS_NODE_RECORD: writer << "layout_read node_record"; break;
            case LAYOUT_RENAME_CELL: writer << "layout_rename_cell"; break;
            case LAYOUT_RENAME_TEXT: writer << "layout_rename_text"; break;
            case LAYOUT_SYSTEM: writer << "layout_input format"; break;
            case LAYOUT_SYSTEM2: writer << "layout_input2 format"; break;
            case LAYOUT_TEXT: writer << "layout_add_text"; break;
            case LAYOUT_TOP_LAYER: writer << "layout_metal_layer"; break;
            case LAYOUT_USE_DATABASE_PRECISION: writer << "layout_use_db_precision"; break;
            case LAYOUT_WINDEL: writer << "layout_exclude_window region"; break;
            case LAYOUT_WINDEL_CELL: writer << "layout_exclude_window cell"; break;
            case LAYOUT_WINDEL_LAYER: writer << "layout_exclude_window layer"; break;
            case LAYOUT_WINDOW: writer << "layout_select_window region"; break;
            case LAYOUT_WINDOW_CELL: writer << "layout_select_window cell"; break;
            case LAYOUT_WINDOW_CLIP: writer << "layout_select_window clip"; break;
            case LAYOUT_WINDOW_LAYER: writer << "layout_select_window layer"; break;
            case LENGTH: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "EDGE_LENGTH"; } else {  writer << iter->sValue; } break;
            case LVS_ABORT_ON_ERC_ERROR: writer << "quit erc_error"; break;
            case LVS_ABORT_ON_SOFTCHK: writer << "quit uniconnect_check"; break;
            case LVS_ABORT_ON_SUPPLY_ERROR: writer << "quit power_ground_error"; break;
            case LVS_ALL_CAPACITOR_PINS_SWAPPABLE: writer << "device_pin_swap cap"; break;
            case LVS_BOX: writer << "lvs_black_box"; break;
            case LVS_BUILTIN_DEVICE_PIN_SWAP: writer << "device_pin_swap mos_res"; break;
            case LVS_CELL_LIST: writer << "lvs_cell_group"; break;
            case LVS_CELL_SUPPLY: writer << "lvs_subcell_power_ground"; break;
            case LVS_CHECK_PORT_NAMES: writer << "lvs_check_top_cell_port"; break;
            case LVS_COMPARE_CASE: writer << "case_sensitive compare"; break;
            case LVS_CPOINT: writer << "lvs_corresponding_net"; break;
            case LVS_DEVICE_TYPE: writer << "device_type_map"; break;
            case LVS_DISCARD_PINS_BY_DEVICE: writer << "lvs_ignore_extra_pin"; break;
            case LVS_DOWNCASE_DEVICE: writer << "case_sensitive lower_device_case"; break;
            case LVS_EXACT_SUBTYPES: writer << "lvs_match_accurate_subtype"; break;
            case LVS_EXCLUDE_HCELL: writer << "lvs_ignore_hcell"; break;
            case LVS_EXECUTE_ERC: writer << "lvs_run_erc"; break;
            case LVS_EXPAND_UNBALANCED_CELLS: writer << "lvs_expand_hcell unbalance"; break;
            case LVS_FILTER: writer << "device_filter"; break;
            case LVS_FILTER_UNUSED_BIPOLAR: writer << "device_filter_unused bjt"; break;
            case LVS_FILTER_UNUSED_CAPACITORS: writer << "device_filter_unused cap"; break;
            case LVS_FILTER_UNUSED_DIODES: writer << "device_filter_unused dio"; break;
            case LVS_FILTER_UNUSED_MOS: writer << "device_filter_unused mos"; break;
            case LVS_FILTER_UNUSED_OPTION: writer << "device_filter_option"; break;
            case LVS_FILTER_UNUSED_RESISTORS: writer << "device_filter_unused res"; break;
            case LVS_GLOBALS_ARE_PORTS: writer << "lvs_global_net_as_port"; break;
            case LVS_GLOBAL_LAYOUT_NAME: writer << "lvs_layout_global_net_group"; break;
            case LVS_GROUND_NAME: writer << "lvs_ground"; break;
            case LVS_HEAP_DIRECTORY: writer << "lvs_temp_dir"; break;
            case LVS_IGNORE_PORTS: writer << "lvs_ignore_port top_cell"; break;
            case LVS_IGNORE_TRIVIAL_NAMED_PORTS: writer << "lvs_ignore_port subcell"; break;
            case LVS_INJECT_LOGIC: writer << "lvs_inject_hierarchy"; break;
            case LVS_ISOLATE_OPENS: writer << "erc_find_open"; break;
            case LVS_ISOLATE_OPENS_IGNORE_NETS: writer << "erc_find_open_ignore_nets"; break;
            case LVS_ISOLATE_OPENS_NETS: writer << "erc_find_open_nets"; break;
            case LVS_ISOLATE_SHORTS: writer << "erc_find_short"; break;
            case LVS_ISOLATE_SHORTS_IGNORE_NETS: writer << "erc_find_shorts_ignore_nets"; break;
            case LVS_MAP_DEVICE: writer << "device_map"; break;
            case LVS_MAXIMUM_ISOLATE_OPENS: writer << "erc_max_opens"; break;
            case LVS_MAXIMUM_ISOLATE_SHORTS: writer << "erc_max_shorts"; break;
            case LVS_MAXIMUM_NETS_PER_ISOLATE_OPEN: writer << "erc_max_nets_per_open"; break;
            case LVS_NETLIST_ALLOW_INCONSISTENT_MODEL: writer << "netlist extract_inconsistent_model"; break;
            case LVS_NETLIST_ALL_TEXTED_PINS: writer << "netlist extract_all_texted_pin"; break;
            case LVS_NETLIST_BOX_CONTENTS: writer << "netlist extract_black_box_content"; break;
            case LVS_NETLIST_COMMENT_CODED_PROPERTIES: writer << "netlist property_in_comment"; break;
            case LVS_NETLIST_COMMENT_CODED_SUBSTRATE: writer << "netlist substrate_in_comment"; break;
            case LVS_NETLIST_UNNAMED_BOX_PINS: writer << "netlist unnamed_black_box_pin"; break;
            case LVS_NL_PIN_LOCATIONS: writer << "netlist extract_pin_location"; break;
            case LVS_NON_USER_NAME_INSTANCE: writer << "lvs_non_user_define_name instance"; break;
            case LVS_NON_USER_NAME_NET: writer << "lvs_non_user_define_name net"; break;
            case LVS_NON_USER_NAME_PORT: writer << "lvs_non_user_define_name port"; break;
            case LVS_OUT_OF_RANGE_EXCLUDE_ZERO: writer << "lvs_zero_property_with_tol"; break;
            case LVS_POWER_NAME: writer << "lvs_power"; break;
            case LVS_PRECISE_INTERACTION: writer << "lvs_precise_overlap"; break;
            case LVS_PRESERVE_BOX_CELLS: writer << "lvs_keep_black_box cell"; break;
            case LVS_PRESERVE_BOX_PORTS: writer << "lvs_keep_black_box port"; break;
            case LVS_PRESERVE_FLOATING_TOP_NETS: writer << "lvs_keep_floating_top_net"; break;
            case LVS_PRESERVE_PARAMETERIZED_CELLS: writer << "lvs_keep_pcell"; break;
            case LVS_PROPERTY_INITIALIZE: writer << "device_property init"; break;
            case LVS_PROPERTY_MAP: writer << "device_property map"; break;
            case LVS_PROPERTY_RESOLUTION_MAXIMUM: writer << "lvs_check_property_radius"; break;
            case LVS_RECOGNIZE_GATES: writer << "lvs_identify_gate"; break;
            case LVS_REDUCE: writer << "device_merge user_define"; break;
            case LVS_REDUCE_PARALLEL_BIPOLAR: writer << "device_merge parallel_bjt"; break;
            case LVS_REDUCE_PARALLEL_CAPACITORS: writer << "device_merge parallel_cap"; break;
            case LVS_REDUCE_PARALLEL_DIODES: writer << "device_merge parallel_dio"; break;
            case LVS_REDUCE_PARALLEL_MOS: writer << "device_merge parallel_mos"; break;
            case LVS_REDUCE_PARALLEL_RESISTORS: writer << "device_merge parallel_res"; break;
            case LVS_REDUCE_SEMI_SERIES_MOS: writer << "device_merge partial_series_mos"; break;
            case LVS_REDUCE_SERIES_CAPACITORS: writer << "device_merge series_cap"; break;
            case LVS_REDUCE_SERIES_MOS: writer << "device_merge series_mos"; break;
            case LVS_REDUCE_SERIES_RESISTORS: writer << "device_merge series_res"; break;
            case LVS_REDUCE_SPLIT_GATES: writer << "device_merge split_gate"; break;
            case LVS_REDUCTION_PRIORITY: writer << "device_merge_priority"; break;
            case LVS_REPORT: writer << "lvs_report db"; break;
            case LVS_REPORT_MAXIMUM: writer << "lvs_report max"; break;
            case LVS_REPORT_OPTION: writer << "lvs_report option"; break;
            case LVS_REPORT_UNITS: writer << "lvs_report unit"; break;
            case LVS_REPORT_WARNINGS_HCELL_ONLY: writer << "lvs_report only_warning_hcell"; break;
            case LVS_REPORT_WARNINGS_TOP_ONLY: writer << "lvs_report only_warning_top"; break;
            case LVS_SHOW_SEED_PROMOTIONS: writer << "lvs_output_device_promotion"; break;
            case LVS_SHOW_SEED_PROMOTIONS_MAXIMUM: writer << "lvs_output_device_promotion_max"; break;
            case LVS_SOFTCHK: writer << "uniconnect_check"; break;
            case LVS_SOFTCHK_MAXIMUM_RESULTS: writer << "uniconnect_check_max_result"; break;
            case LVS_SOFT_SUBSTRATE_PINS: writer << "lvs_ignore_substrate_pin"; break;
            case LVS_SPICE_SCALE_X_PARAMETERS: writer << "spice xscale"; break;
            case LVS_SPLIT_GATE_RATIO: writer << "device_merge split_gate_ratio"; break;
            case LVS_STRICT_SUBTYPES: writer << "lvs_match_strict_subtype"; break;
            case LVS_WRITE_INJECTED_LAYOUT_NETLIST: writer << "lvs_output_netlist injected_layout"; break;
            case LVS_WRITE_INJECTED_SOURCE_NETLIST: writer << "lvs_output_netlist injected_schematic"; break;
            case LVS_WRITE_LAYOUT_NETLIST: writer << "lvs_output_netlist layout"; break;
            case LVS_WRITE_SOURCE_NETLIST: writer << "lvs_output_netlist schematic"; break;
            case MAGNIFY: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "geom_magnify"; } else {  writer << iter->sValue; } break;
            case MASK_SVDB_DIRECTORY: writer << "lvs_db"; break;
            case MERGE: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "geom_merge"; } else {  writer << iter->sValue; } break;
            case NET: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "GEOM_NET BY_LAYER"; } else {  writer << iter->sValue; } break;
            case NET_AREA: writer << "geom_net by_area"; break;
            case NET_AREA_RATIO: writer << "check_nar"; break;
            case NET_AREA_RATIO_ACCUMULATE: writer << "check_incremental_nar"; break;
            case NET_AREA_RATIO_PRINT: writer << "print_log nar"; break;
            case NOT: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "NOT"; } else {  writer << iter->sValue; } break;
            case NOT_COINCIDENT_EDGE: writer << "not_edge_coincident both_side"; break;
            case NOT_COINCIDENT_INSIDE_EDGE: writer << "not_edge_coincident inside"; break;
            case NOT_COINCIDENT_OUTSIDE_EDGE: writer << "not_edge_coincident outside"; break;
            case NOT_NET: writer << "not_geom_net by_layer"; break;
            case NOT_TOUCH_EDGE: writer << "not_edge_adjacent both_side"; break;
            case NOT_TOUCH_INSIDE_EDGE: writer << "not_edge_adjacent inside"; break;
            case NOT_TOUCH_OUTSIDE_EDGE: writer << "not_edge_adjacent outside"; break;
            case OFFGRID: writer << "check_offgrid"; break;
            case OPCBIAS: writer << "opc_bias"; break;
            case OR: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "GEOM_OR"; } else {  writer << iter->sValue; } break;
            case ORNET: writer << "geom_or_net"; break;
            case OR_EDGE: writer << "edge_or"; break;
            case OUTSIDE: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "GEOM_OUTSIDE"; } else {  writer << iter->sValue; } break;
            case OUTSIDE_EDGE: writer << "edge_outside"; break;
            case PATHCHK: writer << "path_check"; break;
            case PATH_LENGTH: writer << "edge_path_length"; break;
            case PERIMETER: writer << "geom_perimeter"; break;
            case POLYGON: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "ADD_POLYGON"; } else {  writer << iter->sValue; } break;
            case PORT_DEPTH: writer << "label_level port"; break;
            case PORT_LAYER_POLYGON: writer << "label_layer port_polygon"; break;
            case PORT_LAYER_TEXT: writer << "label_layer port"; break;
            case PRECISION: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "PRECISION"; } else {  writer << iter->sValue; } break;
            case PUSH: writer << "geom_push"; break;
            case PUSH_CELL: writer << "push_cell_lowest"; break;
            case RECTANGLE: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "geom_rectangle"; } else {  writer << iter->sValue; } break;
            case RECTANGLES: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "geom_fill_rectangles"; } else {  writer << iter->sValue; } break;
            case RECTANGLE_ENCLOSURE: writer << "enc_rect"; break;
            case RESOLUTION: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "resolution db"; } else {  writer << iter->sValue; } break;
            case ROTATE: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "geom_rotate"; } else {  writer << iter->sValue; } break;
            case SCONNECT: writer << "uniconnect"; break;
            case SHIFT: writer << "geom_shift"; break;
            case SNAP: writer << "geom_snap"; break;
            case SNAP_OFFGRID: writer << "snap_offgrid"; break;
            case SOURCE_CASE: writer << "case_sensitive schematic"; break;
            case SOURCE_PATH: writer << "schematic_input path"; break;
            case SOURCE_PRIMARY: writer << "schematic_input top_cell"; break;
            case SOURCE_SYSTEM: writer << "schematic_input format"; break;
            case STAMP: writer << "geom_transfer_netid"; break;
            case SVRF_ERROR: writer << "quit compile_error"; break;
            case SVRF_MESSAGE: writer << "print_log compile"; break;
            case TDDRC: writer << "table_drc"; break;
            case TEXT: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "ADD_TEXT"; } else {  writer << iter->sValue; } break;
            case TEXT_DEPTH: writer << "label_level text"; break;
            case TEXT_LAYER: writer << "label_layer text"; break;
            case TEXT_PRINT_MAXIMUM: writer << "print_log text_max"; break;
            case TITLE: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "head_name"; } else {  writer << iter->sValue; } break;
            case TOUCH: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "geom_adjacent"; } else {  writer << iter->sValue; } break;
            case TOUCH_EDGE: writer << "edge_adjacent both_side"; break;
            case TOUCH_INSIDE_EDGE: writer << "edge_adjacent inside"; break;
            case TOUCH_OUTSIDE_EDGE: writer << "edge_adjacent outside"; break;
            case TRACE_PROPERTY: writer << "check_property"; break;
            case TVF: writer << "trs"; break;
            case TVF_FUNCTION: writer << "trs_function"; break;
            case UNIT_CAPACITANCE: writer << "unit cap"; break;
            case UNIT_INDUCTANCE: writer << "unit ind"; break;
            case UNIT_LENGTH: writer << "unit len"; break;
            case UNIT_RESISTANCE: writer << "unit res"; break;
            case UNIT_TIME: writer << "unit time"; break;
            case VARIABLE: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "VAR"; } else {  writer << iter->sValue; } break;
            case VERTEX: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "GEOM_VERTEX"; } else {  writer << iter->sValue; } break;
            case VIRTUAL_CONNECT_BOX_COLON: writer << "virtual_connect black_box_colon"; break;
            case VIRTUAL_CONNECT_BOX_NAME: writer << "virtual_connect black_box_name"; break;
            case VIRTUAL_CONNECT_COLON: writer << "virtual_connect colon"; break;
            case VIRTUAL_CONNECT_DEPTH: writer << "virtual_connect level"; break;
            case VIRTUAL_CONNECT_NAME: writer << "virtual_connect name"; break;
            case VIRTUAL_CONNECT_REPORT: writer << "virtual_connect report_warning"; break;
            case VIRTUAL_CONNECT_REPORT_MAXIMUM: writer << "virtual_connect report_max"; break;
            case VIRTUAL_CONNECT_SEMICOLON_AS_COLON: writer << "virtual_connect semicolon"; break;
            case WITH_EDGE: writer << "geom_with_edge"; break;
            case WITH_NEIGHBOR: writer << "geom_with_adjacent"; break;
            case WITH_TEXT: writer << "geom_with_label"; break;
            case WITH_WIDTH: writer << "geom_with_width"; break;
            case XOR: writer << "XOR"; break;

            
            case DRC_INCREMENTAL_CONNECT: writer << "incremental_connect"; break;
            case DRC_INCREMENTAL_CONNECT_WARNING: writer << "incremental_connect_warning"; break;
            case DRC_KEEP_EMPTY: writer << "drc_output_empty"; break;
            case DRC_MAGNIFY_RESULTS: writer << "drc_magnify_result db"; break;
            case DRC_MAXIMUM_UNATTACHED_LABEL_WARNINGS: writer << "drc_max_unattached_label_warning"; break;
            case ERC_KEEP_EMPTY: writer << "erc_output_empty"; break;
            case ERC_SELECT_CHECK_ABORT_LVS: writer << "erc_select_rule quit_lvs by_name"; break;
            case EXCLUDE_ACUTE: writer << "check_original_layer ignore_acute_edge "; break;
            case EXCLUDE_ANGLED: writer << "check_original_layer ignore_angled_edge "; break;
            case EXCLUDE_OFFGRID: writer << "check_original_layer ignore_offgrid_vertex"; break;
            case EXCLUDE_SKEW: writer << "check_original_layer ignore_skew_edge "; break;
            case FLAG_ACUTE: writer << "print_log acute_edge"; break;
            case FLAG_ANGLED: writer << "print_log angled_edge"; break;
            case FLAG_OFFGRID: writer << "print_log offgrid_vertex"; break;
            case FLAG_SKEW: writer << "print_log skew_edge"; break;
            case LAYOUT_PRESERVE_NET_CASE: writer << "case_sensitive layout_property_net"; break;
            case LVS_EXPAND_SEED_PROMOTIONS: writer << "lvs_expand_hcell device_promotion"; break;
            case LVS_PUSH_DEVICES: writer << "device_push"; break;
            case LVS_REVERSE_WL: writer << "spice swap_wl"; break;
            case LVS_SPICE_ALLOW_DUPLICATE_SUBCIRCUIT_NAMES: writer << "spice permit_duplicate_subcircuit"; break;
            case LVS_SPICE_ALLOW_FLOATING_PINS: writer << "spice permit_floating_pin"; break;
            case LVS_SPICE_ALLOW_INLINE_PARAMETERS: writer << "spice permit_inline_parameter"; break;
            case LVS_SPICE_ALLOW_UNQUOTED_STRINGS: writer << "spice permit_unquoted_string"; break;
            case LVS_SPICE_CONDITIONAL_LDD: writer << "spice conditional_comment"; break;
            case LVS_SPICE_CULL_PRIMITIVE_SUBCIRCUITS: writer << "spice ignore_primitive_subcircuit"; break;
            case LVS_SPICE_IMPLIED_MOS_AREA: writer << "spice calculate_mos_area"; break;
            case LVS_SPICE_MULTIPLIER_NAME: writer << "spice multiplier_factor_name"; break;
            case LVS_SPICE_OVERRIDE_GLOBALS: writer << "spice rewrite_global_net"; break;
            case LVS_SPICE_PREFER_PINS: writer << "spice use_subcell_pin"; break;
            case LVS_SPICE_REDEFINE_PARAM: writer << "spice redefine_param_value"; break;
            case LVS_SPICE_RENAME_PARAMETER: writer << "spice rename_param"; break;
            case LVS_SPICE_REPLICATE_DEVICES: writer << "spice split_multipler_device"; break;
            case LVS_SPICE_SLASH_IS_SPACE: writer << "spice treat_slash_as_space"; break;
            case LVS_SPICE_STRICT_WL: writer << "spice strict_wl"; break;
            case SIZE: if (__isRealKeywordInComment(iter->eKeyType, vTokensNoWS, hasBadTokens)) { writer << "GEOM_SIZE"; } else {  writer << iter->sValue; } break;
            case VIRTUAL_CONNECT_INCREMENTAL: writer << "virtual_connect incremental_connect"; break;

            
            case NETLIST:
            {
                std::vector<rcsToken_T>::iterator it = iter;
                ++it;
                skipTokenUntilNot(vAllTokens, it, BLANK_SPACE);
                if (it != vAllTokens.end())
                {
                    if (it->eKeyType == MODEL)
                    {
                       writer << "rc_model";
                       iter = it;
                       break;
                    }
                    else if (it->eKeyType == ELEMENT)
                    {
                       writer << "rc_element";
                       iter = it;
                       break;
                    }
                }

                writer << iter->sValue;
                break;
            }
            case TEXT_MODEL_LAYER: writer << "text_model"; break;
            case TEXT_PROPERTY_LAYER: writer << "text_property"; break;

            default: writer << iter->sValue; break;
        }
    }

    sPvrs = writer.str();
}


void
rcsSynPvrsConvertor::beginVisitCommentNode(rcsSynCommentNode_T *pNode)
{
    if(!m_hasGetTmpLayerValue)
        return;

    //outputBlankLinesBefore(pNode->getNodeBeginLineNo());

    Utassert(rcsManager_T::getInstance()->outputComment());
    if (rcsManager_T::getInstance()->outputComment())
    {
        std::string sComment = pNode->getComment();
        std::string::size_type iStart = 0;
        std::string::size_type iEnd = sComment.size();
        Utassert(iEnd >= 2);

        bool isBlockCommnet = getCommentContent(sComment, iStart, iEnd);

        std::string sCommentContent = sComment.substr(iStart, iEnd - iStart);

        std::string sNewCommentContent;
        if (!isBlockCommnet)
        {
            replaceKeywordsInComment(sCommentContent, sNewCommentContent);
            if(sNewCommentContent[0] == '@')
                sNewCommentContent[0] = ';';
        }
        else
        {
            if (!replaceBlockComment(sCommentContent, sNewCommentContent))
                replaceKeywordsInComment(sCommentContent, sNewCommentContent);
        }

        std::string sNewComment = sComment.substr(0, iStart) + sNewCommentContent + sComment.substr(iEnd);
        std::string sValue;
        _replaceComment(sNewComment, sValue, false);

        (*m_oPVRSStream) << sValue << std::endl;
    }
}

void
rcsSynPvrsConvertor::beginVisitBuildInNode(rcsSynBuildInNode_T *pNode)
{
    if(!m_hasGetTmpLayerValue)
        return;

    outputBlankLinesBefore(pNode->getNodeBeginLineNo());

    std::string sValue;
    _replaceBuildIn(pNode->getBuildIn(), sValue);
    (*m_oPVRSStream) << sValue << std::endl;
}

void rcsSynPvrsConvertor::beginVisitDMACRODummyNode(rcsSynDMACRODummyNode_T *pNode)
{
    for (std::list<rcsToken_T>::iterator it=pNode->begin(); it!=pNode->end(); ++it)
    {
        
        (*m_oPVRSStream) << it->sValue << " ";
    }
}

void rcsSynPvrsConvertor::beginVisitSwitchUnknowNode(rcsSynSwitchUnKnowNode_T *pNode)
{
    (*m_oPVRSStream) << "/* == begin switch unknow segment == */\n";
    for (std::list<rcsToken_T>::iterator it=pNode->begin(); it!=pNode->end(); ++it)
    {
        (*m_oPVRSStream) << it->sValue << " ";
    }
    (*m_oPVRSStream) << "\n";
    (*m_oPVRSStream) << "/* == end switch unknow segment == */\n";
}

void
rcsSynPvrsConvertor::beginVisitRuleCheckNode(rcsSynRuleCheckNode_T *pNode)
{
    Utassert(!m_inProcReturn);

    if(!m_hasGetTmpLayerValue)
        return;

    //outputBlankLinesBefore(pNode->getNodeBeginLineNo());

    std::string sCheckText;
    std::string sCheckComment;
    _getCheckText(pNode->getNodeBeginLineNo(), pNode->getEndLineNo(),
                  sCheckText, sCheckComment);
    
#if 0
    {
        std::string::size_type iStart = sCheckComment.find_first_not_of(" @\t\f\v\n\r");
        std::string sOldCheckCommnet;
        std::swap(sOldCheckCommnet, sCheckComment);
        if (iStart != std::string::npos)
        {
            replaceKeywordsInComment(sOldCheckCommnet.substr(iStart), sCheckComment);
        }
    }
#endif

    std::string sCheckName;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    getStringName(iter, pNode->end(), sCheckName);
    (*m_oPVRSStream) << "\n";
    if (rcsManager_T::getInstance()->isSvrfComment())
    {
        (*m_oPVRSStream) << "// RULE ";
        (*m_oPVRSStream) << pNode->getRuleCheckName() << std::endl;
    }

    (*m_oPVRSStream) << "RULE " << sCheckName << " { \n";
#if 0
    if (!sCheckComment.empty())
    {
        (*m_oPVRSStream) << "; ";
        std::string::size_type iValue = 0;
        for(; iValue != sCheckComment.size(); ++iValue)
        {
            if ((sCheckComment[iValue] == '\n' || sCheckComment[iValue] == '\r'))
            {
                if (iValue != sCheckComment.size() - 1)
                {
                    (*m_oPVRSStream) << "\n; ";
                }
            }
            else
            {
                (*m_oPVRSStream) << sCheckComment[iValue];
            }
        }
        (*m_oPVRSStream) << "\n";
    }
#endif
    
}

void
rcsSynPvrsConvertor::endVisitRuleCheckNode(rcsSynRuleCheckNode_T *pNode)
{
    if(!m_hasGetTmpLayerValue)
        return;

    (*m_oPVRSStream) << "}\n";
}

void
rcsSynPvrsConvertor::endVisitLayerOperationNode(rcsSynLayerOperationNode_T *pNode)
{
}

void
rcsSynPvrsConvertor::convExecProcCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "PROC";
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    std::string sMacroName;
    if (iter != pNode->end())
    {
        sMacroName = iter->sValue;
        toLower(sMacroName);
    }

    if (m_vProcUsedTimes.find(sMacroName) == m_vProcUsedTimes.end())
    {
        m_vProcUsedTimes[sMacroName] = 0;
    }
    ++m_vProcUsedTimes[sMacroName];
    getStringName(iter, pNode->end(), sMacroName);
    toLower(sMacroName);
    (*m_oPVRSStream) << "call_fun (" << sMacroName << " ";
    ++iter;
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }

                std::string sValue;
                if (getStringName(iter, pNode->end(), sValue))
                {
                    (*m_oPVRSStream) << sValue << " ";
                }
                break;
            }
                /*
                 * cmacro pro layer1  1  -2  lex to  cmacro pro layer1 1 -  2
                 * the -2 become two token, so args number may be more
                */
            case OPERATOR:
                (*m_oPVRSStream) << iter->sValue;
                if (iter->sValue != "+" && iter->sValue != "-")
                    (*m_oPVRSStream) << " ";
                break;
            default:
                (*m_oPVRSStream) << iter->sValue << " ";
                break;
        }
        ++iter;
    }

    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::beginVisitLayerOperationNode(rcsSynLayerOperationNode_T *pNode)
{
    if(m_isTmpLayerDefinition && !rcsManager_T::getInstance()->needExpandTmpLayer() && m_hasGetTmpLayerValue)
        return;

    if(!m_isTmpLayerDefinition && !m_hasGetTmpLayerValue)
        return;

    std::map<std::string, rcsSynLayerDefinitionNode_T*>& _tmpLayers = rcsManager_T::getInstance()->getTmpLayers();
    for(auto& tmpLayer : _tmpLayers)
    {
        if( (m_nParsingTmpLayers <= 0) && (tmpLayer.second != NULL) && ( pNode == tmpLayer.second->getLayerOperationNode() ) )
        {
            return;
        }
    }

    std::string sValue;
    pNode->getCmdValue(sValue);

    if (!m_isInLayerDefinition)
    {
        outputBlankLinesBefore(pNode->getNodeBeginLineNo());
    }

    if (!m_isInLayerDefinition && rcsManager_T::getInstance()->isSvrfComment())
    {
        std::string sValue;
        pNode->getCmdValue(sValue);
        (*m_oPVRSStream) << "// " << sValue << std::endl;
    }

    if (pNode->isCMacro())
    {
        if (rcsManager_T::getInstance()->isSvrfComment())
        {
            if (!m_isInLayerDefinition)
                ;
        }

        convExecProcCmd(pNode);

        if (!m_isInLayerDefinition)
        {
            (*m_oPVRSStream) << std::endl;
        }
        return;
    }

    





    switch(pNode->getOperationType())
    {

        case ENCLOSURE:
        case EXTERNAL:
        case INTERNAL:
            convCheckSpaceCmd(pNode);
            break;
        case AND:
        case OR:
        case XOR:
        case NOT:
            convGeomBooleanCmd(pNode);
            break;
        case OR_EDGE:
            convEdgeMergeCmd(pNode);
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
            convGeomTopoCmd(pNode);
            break;
        case ATTACH:
            convAttachCmd(pNode);
            break;
        case CONNECT:
            convConnectCmd(pNode);
            break;
        case LABEL_ORDER:
            convAttachOrderCmd(pNode);
            break;
        case AREA:
        case NOT_AREA:
        case PERIMETER:
        case DONUT:
        case NOT_DONUT:
        case VERTEX:
            convGeomAttrCmd(pNode);
            break;
        case COPY:
            convGeomCopyLayerCmd(pNode);
            break;
        case HOLES:
            convGeomSelectHolesCmd(pNode);
            break;
        case RECTANGLE:
        case NOT_RECTANGLE:
            convGeomSelectRectangleCmd(pNode);
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
            convEdgeTopoCmd(pNode);
            break;
        case NOT_ENCLOSE_RECTANGLE:
        case ENCLOSE_RECTANGLE:
            convCheckFittingRectangleCmd(pNode);
            break;
        case EXTENTS:
            convGeomGetBBoxCmd(pNode);
            break;
        case NOT_INSIDE_CELL:
        case INSIDE_CELL:
            convGeomSelectInCellCmd(pNode);
            break;
        case SIZE:
            convSizeCmd(pNode);
            break;
        case SIZE_XY:
            convSize_XYCmd(pNode);
            break;
        case DENSITY:
        case DENSITY_CONVOLVE:
            convCheckDensityCmd(pNode);
            break;
        case NET_AREA_RATIO:
            convCheckNetAreaRatioCmd(pNode);
            break;
        case NET_AREA_RATIO_ACCUMULATE:
            convCheckNetAreaRatioByAccumulateCmd(pNode);
            break;
        case DRAWN_ACUTE:
        case DRAWN_SKEW:
        case DRAWN_OFFGRID:
        case DRAWN_ANGLED:
            convGeomOriginalCheckCmd(pNode);
            break;
        case DEVICE_LAYER:
            convGeomSelectDeviceLayerCmd(pNode);
            break;
        case DFM_OR_EDGE:
            convDfmOrEdge(pNode);
            break;
        case DFM_PROPERTY:
            convBuildDfmPropertyCmd(pNode);
            break;
        case DFM_PROPERTY_SELECT_SECONDARY:
            convDfmPropertySelectSecondaryCmd(pNode);
            break;
        case DFM_PROPERTY_NET:
            convBuildDfmPropertyNetCmd(pNode);
            break;
        case DFM_SPACE:
            convDfmSpaceCmd(pNode);
            break;
        case CONVEX_EDGE:
            convEdgeSelectByConvexCornerCmd(pNode);
            break;
        case DFM_SHIFT:
            convGeomTransformCmd(pNode);
            break;
        case DFM_SHIFT_EDGE:
            convDfmShiftEdge(pNode);
            break;
        case DFM_DV:
            convDfmDV(pNode);
            break;
        case DFM_SIZE:
            convDfmSizeCmd(pNode);
            break;

        case DFM_COPY:
            convGeomCopyDfmLayerCmd(pNode);
            break;
        case DFM_PROPERTY_MERGE:
            convDfmMergePropertyCmd(pNode);
            break;
        case DFM_NARAC:
            convDfmNARACCmd(pNode);
            break;
#if 0
        case DFM_RDB:
            convDfmRDBCmd(pNode);
#endif
            break;
        case NOT_NET:
        case NET:
            convGeomGetNetByNameCmd(pNode);
            break;
        case EXTENT:
            convGeomGetLayerBBoxCmd(pNode);
            break;

        case DFM_STAMP:
            convDfmStampCmd(pNode);
            break;
        case ANGLE:
        case NOT_ANGLE:
        case LENGTH:
        case NOT_LENGTH:
        case PATH_LENGTH:
            convEdgeSelectByAttrCmd(pNode);
            break;
        case RECTANGLE_ENCLOSURE:
            convCheckRectEncCmd(pNode);
            break;
        case FLATTEN:
            convGeomFlattenLayerCmd(pNode);
            break;
        case EXPAND_EDGE:
            convEdgeExpandToRectCmd(pNode);
            break;
        case RECTANGLES:
            convFillRectanglesCmd(pNode);
            break;
        case EXTENT_CELL:
            convGeomGetCellBBoxCmd(pNode);
            break;
        case EXTENT_DRAWN:
            convGeomGetMultiLayersBBoxCmd(pNode);
            break;
        case GROW:
        case SHRINK:
        case MAGNIFY:
        case ROTATE:
        case SHIFT:
            convGeomTransformCmd(pNode);
            break;
        case MERGE:
            convGeomMergeLayerCmd(pNode);
            break;
        case NET_AREA:
            convGeomGetNetByAreaCmd(pNode);
            break;
        case ORNET:
            convGeomMergeNetCmd(pNode);
            break;
        case DEANGLE:
            convEdgeAngledToRegularCmd(pNode);
            break;
        case PUSH:
            convGeomPushLayerCmd(pNode);
            break;
        case SNAP:
            convGeomVerticeSnapToGridCmd(pNode);
            break;
        case EXPAND_TEXT:
            convGeomGetTextBBoxCmd(pNode);
            break;
        case STAMP:
            convGeomStampLayerCmd(pNode);
            break;
        case NOT_WITH_NEIGHBOR:
        case WITH_NEIGHBOR:
            convGeomSelectByNeighborCmd(pNode);
            break;
        case NOT_WITH_EDGE:
        case WITH_EDGE:
            convGeomSelectByCoinEdgeCmd(pNode);
            break;

        case WITH_TEXT:
        case NOT_WITH_TEXT:
            convGeomSelectTextedCmd(pNode);
            break;
        case WITH_WIDTH:
        case NOT_WITH_WIDTH:
            convCheckWidthCmd(pNode);
            break;
        case OPCBIAS:
            convOpcBiasCmd(pNode);
            break;
        case NET_AREA_RATIO_PRINT:
            convNetAreaRatioRDBCmd(pNode);
            break;
        case SCONNECT:
            convSconnectCmd(pNode);
            break;
        case DISCONNECT:
            convDisconnectCmd(pNode);
            break;
        case TDDRC:
            convCheckTableDRCCmd(pNode);
            break;
        case PATHCHK:
            convPathchkCmd(pNode);
            break;
        case OFFGRID:
            convCheckOffgridCmd(pNode);
            break;
        case DFM_TEXT:
            convDfmTextCmd(pNode);
            break;
        case TVF:
            convTvfCmd(pNode);
            break;
        case NET_INTERACT:
            convNetInteract(pNode);
            break;
        case DFM_FILL:
        case DFM_FILL_REGION:
        case DFM_FILL_WRAP:
        case DFM_MAT:
        case DFM_COMPRESS:
        case DFM_RDB:
        case DFM_DP:
        case DFM_MP:
        case DFM_READ:
        case DFM_PROPERTY_SINGLETON:
        case DFM_EXPAND_EDGE:
            //convDFMFill(pNode);
            convCommonNode(pNode->getOperationType(), pNode);
            break;
        case RET_NMDPC:
            convRetNMDPCCmd(pNode);
            break;
        case RET_TP:
            convRetTPCmd(pNode);
            break;
        case RET_QP:
            convRetQPCmd(pNode);
            break;
        case POLYNET:
        case DFM_CLASSIFY:
        case DFM_CONNECTIVITY_REDUNDANT:
        case DFM_CONNECTIVITY_NOT_REDUNDANT:
        case DFM_CREATE_LAYER:
        case DFM_CREATE_LAYER_POLYGONS:
        case DFM_EXPAND_ENCLOSURE:
        case DFM_HISTOGRAM:
        case DFM_GROW:
        case DFM_MEASURE:
        case DFM_TRANSFORM:
        case DFM_TRANSITION:
        case DFM_REDUNDANT_VIAS:
        case DFM_NON_REDUNDANT_VIAS:
        case DFM_ANALYZE:
        case DFM_CAF:
        case DFM_GCA:
        case DFM_CRITICAL_AREA:
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
}

void
rcsSynPvrsConvertor::outputCon1Error(rcsSynNode_T *pNode)
{
    std::string sValue;
    pNode->getCmdValue(sValue);
    m_oLogStream << "This Operation or Specification in line " << pNode->begin()->nLineNo
                 << " can not be Converted correctly: " << std::endl << sValue << std::endl << std::endl;

    s_errManager.addError(rcErrorNode_T(rcErrorNode_T::WARNING, CON1,
                          pNode->getNodeBeginLineNo(), pNode->begin()->sValue));
}

void
rcsSynPvrsConvertor::outputCon2Error(rcsSynNode_T *pNode, const rcsToken_T &token)
{
    if (token.eType == SECONDARY_KEYWORD && (token.eKeyType == MASK || token.eKeyType == DIRECT))
        return;

    std::string sValue;
    pNode->getCmdValue(sValue);
    m_oLogStream << "This Option " << token.sValue << " in line " << token.nLineNo
                 << " can not be Converted correctly in this operation: " << std::endl
                 << sValue << std::endl << std::endl;

    s_errManager.addError(rcErrorNode_T(rcErrorNode_T::WARNING, CON2,
                                        token.nLineNo, token.sValue));
}

bool
rcsSynPvrsConvertor::parsePlaceCellInfo(std::list<rcsToken_T>::iterator &iter,
                                          std::list<rcsToken_T>::iterator end)
{
    std::string sCellName;
    if (getStringName(iter, end, sCellName))
        outputText(NULL, sCellName.c_str());
    else
        return false;

    if (++iter == end)
        return false;

    std::string sValue;
    if (getNumberExp(iter, end, sValue, false, false))
        outputText(NULL, sValue.c_str());
    else
        return false;

    if (iter == end)
        return false;

    sValue.clear();
    if (getNumberExp(iter, end, sValue, false, false))
        outputText(NULL, sValue.c_str());
    else
        return false;

    if (iter == end)
        return false;

    sValue.clear();
    if (getNumberExp(iter, end, sValue, false, false))
        outputText(NULL, sValue.c_str());
    else
        return false;

    if (iter == end)
        return false;

    sValue.clear();
    if (getNumberExp(iter, end, sValue, false, false))
        outputText(NULL, sValue.c_str());
    else
        return false;

    if (iter == end)
        return false;

    sValue.clear();
    if (getNumberExp(iter, end, sValue, false, false))
        outputText(NULL, sValue.c_str());
    else
        return false;

    return true;
}



void
rcsSynPvrsConvertor::outputText(const char *pOption, const char *pValue,
                                const hvUInt32 nOption)
{
    if (pOption != NULL && pValue != NULL && nOption != 0 && nOption % MAX_OPTIONS_PER_LINE == 0)
    {
        if(rcsManager_T::getInstance()->isTvf2Trs())
            (*m_oPVRSStream) << "\\";
        (*m_oPVRSStream) << "\n";
    }
    else if (pValue != NULL && nOption != 0 && nOption % (MAX_OPTIONS_PER_LINE * 2) == 0)
    {
        if(rcsManager_T::getInstance()->isTvf2Trs())
            (*m_oPVRSStream) << "\\";
        (*m_oPVRSStream) << "\n";
    }
    else if (pOption != NULL && nOption != 0 && nOption % (MAX_OPTIONS_PER_LINE * 2) == 0)
    {
        if(rcsManager_T::getInstance()->isTvf2Trs())
            (*m_oPVRSStream) << "\\";
        (*m_oPVRSStream) << "\n";
    }

    if (pOption != NULL)
    {
        (*m_oPVRSStream) << pOption << " ";
    }

    if (pValue != NULL)
    {
        (*m_oPVRSStream) << pValue << " ";
    }
}

void rcsSynPvrsConvertor::outputOriginString(std::list<rcsToken_T>::iterator iter)
{
    if (iter->eType != STRING_CONSTANTS)
    {
    	if((iter->eType == IDENTIFIER_NAME) && isPvrsKeyword(iter->sValue))
    	{
    		 (*m_oPVRSStream) << '\"' << iter->sValue << "\" ";
    	}
    	else
    	{
    		(*m_oPVRSStream) << iter->sValue << " ";
    	}
    }
    else
    {
    	if('\"' == iter->sValue[0])
    	{
    		iter->sValue.insert(0, "\\");
    	}
    	if('\"' == iter->sValue.back())
    	{
    		iter->sValue.insert(iter->sValue.size() - 1, "\\");
    	}

        (*m_oPVRSStream) << '\"' << iter->sValue << "\" ";
    }
}

#define PVRS_TMP_RETUAN  outputCon1Error(pNode); return;

void
rcsSynPvrsConvertor::convLayoutInputSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    std::string sCmd = "layout_input (";
    if(rcsManager_T::getInstance()->isTvf2Trs())
    {
        sCmd = "layout_input ";
    }
    bool bLayoutPrimary = false;
    bool bLayoutSystem  = false;
    switch (pNode->getSpecificationType())
    {
        case  LAYOUT_PRIMARY:
        {
            sCmd += " top_cell";
            bLayoutPrimary = true;
            break;
        }
        case LAYOUT_PRIMARY2:
        {
            //PVRS_TMP_RETUAN
            sCmd = "layout_input2 ( top_cell";
            if(rcsManager_T::getInstance()->isTvf2Trs())
            {
                sCmd = "layout_input2 top_cell";
            }
            bLayoutPrimary = true;
            break;
        }
        case  LAYOUT_SYSTEM:
        {
            sCmd += " format";
            bLayoutSystem = true;
            break;
        }
        case LAYOUT_SYSTEM2:
        {
            //PVRS_TMP_RETUAN
            sCmd = "layout_input2 ( format";
            if(rcsManager_T::getInstance()->isTvf2Trs())
            {
                sCmd = "layout_input2 format";
            }
            bLayoutSystem = true;
            break;
        }
        default:
        {
            break;
        }
    }
    if (bLayoutPrimary)
    {
        (*m_oPVRSStream) << sCmd << " ";
        std::list<rcsToken_T>::iterator iter = pNode->begin();
        ++iter;
        if (iter != pNode->end())
        {
            std::string sValue;
            if (getStringName(iter, pNode->end(), sValue))
            {
                outputText(NULL, sValue.c_str());
            }
            else
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP11,
                                iter->nLineNo, iter->sValue);
        }
        if(!rcsManager_T::getInstance()->isTvf2Trs())
            (*m_oPVRSStream) << " ) ";
        if (!m_isInLayerDefinition)
        {
            (*m_oPVRSStream) << std::endl;
        }
        return;
    }

    if (bLayoutSystem)
    {
        std::list<rcsToken_T>::iterator iter = pNode->begin();
        ++iter;
        if (iter != pNode->end())
        {
            switch (iter->eKeyType)
            {
                case GDSII:
                    (*m_oPVRSStream) << sCmd << " ";
                    outputText(NULL, "gds");
                    break;
                case OPENACCESS:
                    (*m_oPVRSStream) << sCmd << " ";
                    outputText(NULL, "oa");
                    break;
                case OASIS:
                    (*m_oPVRSStream) << sCmd << " ";
                    outputText(NULL, "oasis");
                    break;
                case SPICE:
                    (*m_oPVRSStream) << sCmd << " ";
                    outputText(NULL, "spice");
                    break;
                default:
                    outputCon2Error(pNode,*iter);
                    return;
            }
        }
        ++iter;
        if (iter != pNode->end())
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
        if(!rcsManager_T::getInstance()->isTvf2Trs())
        {
            (*m_oPVRSStream) << " ) ";
        }
        if (!m_isInLayerDefinition)
        {
            (*m_oPVRSStream) << std::endl;
        }
        return;
    }

    if (pNode->getSpecificationType() == LAYOUT_PATH)
    {
        sCmd += " path";
    }
    else if (pNode->getSpecificationType() == LAYOUT_PATH2)
    {
        //PVRS_TMP_RETUAN
        sCmd = "layout_input2 ( path";
        if(rcsManager_T::getInstance()->isTvf2Trs())
        {
            sCmd = "layout_input2 path";
        }
    }
    else
    {
        return;
    }

    (*m_oPVRSStream) << sCmd << " ";
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                if (iter->eKeyType == STDIN && pNode->getSpecificationType() == LAYOUT_PATH)
                {
                    outputCon2Error(pNode,*iter);
                    ++iter;
                    continue;
                }
            }
            case IDENTIFIER_NAME:
            {
                if (!isValidSvrfName(iter->sValue))
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
            }
            case STRING_CONSTANTS:
            {
                std::string sValue;
                if (getStringName(iter, pNode->end(), sValue))
                    outputText(NULL, sValue.c_str(), nOption++);
                else
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                        iter->nLineNo, iter->sValue);
                break;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }

        ++iter;
        while(iter != pNode->end())
        {
            if (iter->eType == SECONDARY_KEYWORD)
            {
                if (iter->eKeyType == PREC)
                {
                    if (++iter != pNode->end() && iter->eKeyType != MAG &&
                       iter->eKeyType != PREC)
                    {
                        std::string sNumExp;
                        if (getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            outputText("precision", sNumExp.c_str(), nOption++);
                        }
                        else
                        {
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                 iter->nLineNo, iter->sValue);
                        }
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP11,
                                            iter->nLineNo, iter->sValue);
                    }

                    if (iter != pNode->end() && (iter->eType == NUMBER ||
                       iter->eType == OPERATOR))
                    {
                        std::string sNumExp;
                        if (getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            outputText(NULL, sNumExp.c_str(), nOption++);
                        }
                        else
                        {
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP11,
                                                iter->nLineNo, iter->sValue);
                        }
                    }
                    continue;
                }
                else if (iter->eKeyType == MAG)
                {




























                    outputText(NULL, "magnify", nOption++);

                    if(++iter != pNode->end() && iter->eKeyType != MAG &&
                       iter->eKeyType != PREC && iter->eKeyType != AUTO)
                    {
                        std::string sNumExp;
                        if (!getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP11,
                                                iter->nLineNo, iter->sValue);
                        }
                        else
                        {
                            outputText(NULL, sNumExp.c_str(), nOption++);
                        }
                    }
                    else if(iter != pNode->end() &&
                            iter->eType == SECONDARY_KEYWORD &&
                            iter->eKeyType == AUTO)
                    {
                        outputText(NULL, "auto", nOption++);
                        ++iter;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP11,
                                            iter->nLineNo, iter->sValue);
                    }
                    continue;
                    iter++;
                }
                else if (iter->eKeyType == GOLDEN)
                {
                    outputCon2Error(pNode, *iter);
                    ++iter;
                    continue;
                }
            }
            break;
        }
    }
    if(!rcsManager_T::getInstance()->isTvf2Trs())
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convMaxLimitSpec(rcsSynSpecificationNode_T *pNode)
{
    std::string sCmd = "";
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    switch (pNode->getSpecificationType())
    {
        case DRC_MAXIMUM_RESULTS:
        {
            sCmd += (_isTvf2Trs ? "drc_max_result " : "drc_max_result ( ");
            break;
        }
        case DRC_MAXIMUM_RESULTS_NAR:
        {
            sCmd += (_isTvf2Trs ? "drc_max_nar_result " : "drc_max_nar_result ( ");
            break;
        }
        case DRC_MAXIMUM_VERTEX:
        {
            sCmd += (_isTvf2Trs ? "drc_max_vertex " : "drc_max_vertex ( ");
            break;
        }
        case ERC_MAXIMUM_RESULTS:
        {
            sCmd += (_isTvf2Trs ? "erc_max_result " : "erc_max_result ( ");
            break;
        }
        case ERC_MAXIMUM_VERTEX:
        {
            sCmd += (_isTvf2Trs ? "erc_max_vertex " : "erc_max_vertex ( ");
            break;
        }
        case DRC_TOLERANCE_FACTOR:
        {
            sCmd += (_isTvf2Trs ? "drc_tolerance distance " : "drc_tolerance ( distance ");
            break;
        }
        case DRC_TOLERANCE_FACTOR_NAR:
        {
            sCmd += (_isTvf2Trs ? "drc_tolerance nar " : "drc_tolerance ( nar ");
            break;
        }
        case LVS_SHOW_SEED_PROMOTIONS_MAXIMUM:
        {
            sCmd += (_isTvf2Trs ? "lvs_output_device_promotion_max " : "lvs_output_device_promotion_max ( ");
            break;
        }
        case LVS_REPORT_MAXIMUM:
        {
            sCmd += (_isTvf2Trs ? "lvs_report max " : "lvs_report ( max ");
            break;
        }
        case LVS_MAXIMUM_ISOLATE_SHORTS:
        {
            sCmd += (_isTvf2Trs ? "erc_max_shorts " : "erc_max_shorts ( ");
            break;
        }
        case LVS_MAXIMUM_ISOLATE_OPENS:
        {
            sCmd += (_isTvf2Trs ? "erc_max_opens " : "erc_max_opens ( ");
            break;
        }
        case LVS_MAXIMUM_NETS_PER_ISOLATE_OPEN:
        {
            sCmd += (_isTvf2Trs ? "erc_max_nets_per_open " : "erc_max_nets_per_open ( ");
            break;
        }
        case TEXT_PRINT_MAXIMUM:
        {
            sCmd += (_isTvf2Trs ? "print_log text_max " : "print_log ( text_max ");
            break;
        }
        case LAYOUT_BUMP2:
        {
            sCmd += (_isTvf2Trs ? "layout2_bump_number " : "layout2_bump_number ( ");
            break;
        }
        case DRC_MAXIMUM_UNATTACHED_LABEL_WARNINGS:
        {
            sCmd += (_isTvf2Trs ? "drc_max_unattached_label " : "drc_max_unattached_label ( ");
            break;
        }
        case DRC_MAXIMUM_CELL_NAME_LENGTH:
        {
            sCmd += (_isTvf2Trs ? "drc_max_cell_name " : "drc_max_cell_name ( ");
            break;
        }
        default:
        {
            return;
        }
    }
    (*m_oPVRSStream) << sCmd << " ";
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sNumExp;
    if (iter == pNode->end())
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2,
                pNode->begin()->nLineNo, pNode->begin()->sValue);
    }
    if (SECONDARY_KEYWORD == iter->eType && iter->eKeyType == ALL)
    {
        outputText(NULL, "all");
        ++iter;
    }
    else if (getNumberExp(iter, pNode->end(), sNumExp))
    {
        outputText(NULL, sNumExp.c_str());
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }
    if (iter != pNode->end())
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }
    if(!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convSumFileSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    if (pNode->getSpecificationType() == DRC_SUMMARY_REPORT)
        (*m_oPVRSStream) << (_isTvf2Trs ? "drc_summary " : "drc_summary ( ");
    else if (pNode->getSpecificationType() == ERC_SUMMARY_REPORT)
        (*m_oPVRSStream) << (_isTvf2Trs ? "erc_summary " : "erc_summary ( ");
    else if (pNode->getSpecificationType() == DFM_SUMMARY_REPORT)
        (*m_oPVRSStream) << (_isTvf2Trs ? "dfm_summary " : "dfm_summary ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sFileName;
    if (getStringName(iter, pNode->end(), sFileName))
    {
        outputText(NULL, sFileName.c_str(), nOption++);
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    ++iter;
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                if (REPLACE == iter->eKeyType || APPEND == iter->eKeyType ||
                   HIER == iter->eKeyType)
                {
                    switch(iter->eKeyType)
                    {
                        case APPEND:
                            outputText(NULL, "append", nOption++);
                            break;
                        case HIER:
                            outputText(NULL, "by_cell", nOption);
                            break;
                        case REPLACE:
                            outputText(NULL, "overwrite", nOption++);
                            break;
                        default:
                            break;
                    }
                    break;
                }
                else
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
            }
            default:
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
            }
        }
        ++iter;
    }
    if(!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convDrcMapTextLayer(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "drc_map_text_layer " : "drc_map_text_layer ( ");
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    while(iter != pNode->end())
    {
        std::string sValue;
        if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == IGNORE)
        {
            sValue = "ignore_layer";
        }
        else if (!getStringName(iter, pNode->end(), sValue))
        {
            sValue = iter->sValue;
        }
        outputText(NULL, sValue.c_str());
        ++iter;
    }

    if(!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
}

void
rcsSynPvrsConvertor::convDfmFunction(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "dfm_function " : "dfm_function ( ");
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    while(iter != pNode->end())
    {
        std::string sValue = iter->sValue;
        if(0 == strcasecmp(sValue.c_str(), "TVF_NUMber_FUNction"))
        {
            outputText(NULL, "TRS_NUMBER_FUNCTION");
        }
        else
        {
            outputText(NULL, sValue.c_str());
        }

        ++iter;
    }

    if(!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
}

void
rcsSynPvrsConvertor::convResultDBSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    std::string sCmd = "";
    if (pNode->getSpecificationType() == DRC_RESULTS_DATABASE)
    {
        sCmd += (_isTvf2Trs ? "drc_result db " : "drc_result ( db ");
    }
    else if (pNode->getSpecificationType() == ERC_RESULTS_DATABASE)
    {
        sCmd += (_isTvf2Trs ? "erc_result db " : "erc_result ( db ");
    }
    else
    {
        return;
    }

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sFileName;
    if (iter != pNode->end())
    {
        if (getStringName(iter, pNode->end(), sFileName))
        {
            sCmd += sFileName + " ";
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC4,
                pNode->begin()->nLineNo, pNode->begin()->sValue);
    }

    (*m_oPVRSStream) << sCmd;
    std::string sAppend;
    std::string sPrefix;
    ++iter;
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                switch(iter->eKeyType)
                {
                    case ASCII:
                    {
                        outputText(NULL, "text_format", nOption++);
                        break;
                    }
                    case BINARY:
                    {
                        outputCon2Error(pNode, *iter);
                        break;
                    }
                    case OASIS:
                    {
                        outputText(NULL, "oasis", nOption++);
                        break;
                    }
                    case INDEX:
                    {
                        outputCon2Error(pNode,*iter);
                        if (++iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                            iter->eKeyType == NOVIEW)
                        {
                            outputCon2Error(pNode,*iter);
                        }
                        else
                        {
                            --iter;
                        }
                    }
                    case GDSII:
                    {
                        outputText(NULL, "gds", nOption++);
                        break;
                    }
                    case USER:
                    {
                        if (++iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                           iter->eKeyType == MERGED)
                        {
                            outputText(NULL, "merged", nOption++);
                        }
                        else
                        {
                            outputText(NULL, "user_cell", nOption++);
                            --iter;
                        }
                        break;
                    }
                    case PSEUDO:
                    {
                        outputText(NULL, "keep_injected_cell", nOption++);
                        break;
                    }
                    case PREFIX:
                    case APPEND:
                    {
                        if (iter->eKeyType == PREFIX)
                        {
                            ++iter;
                            if (getStringName(iter, pNode->end(), sPrefix))
                            {
                                outputText("with_prefix", sPrefix.c_str(), nOption++);
                            }
                            else
                            {
                                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                            }
                            break;
                         }

                         if (!sAppend.empty())
                         {
                             throw rcErrorNode_T(rcErrorNode_T::ERROR, KEY1,
                                                 iter->nLineNo, iter->sValue);
                         }

                         ++iter;
                         if (getStringName(iter, pNode->end(), sAppend))
                         {
                             outputText("with_append", sAppend.c_str(), nOption++);
                         }
                         else
                         {
                             throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                     iter->nLineNo, iter->sValue);
                         }
                         break;
                     }
                     case CBLOCK:
                     {
                         outputText(NULL, "cblock", nOption++);
                         break;
                     }
                     case TOP:
                     {
                         outputText(NULL, "top_cell", nOption++);
                         break;
                     }
                     case STRICT:
                     {
                         outputText(NULL, "strict", nOption++);
                         break;
                     }
                     default:
                     {
                         if (!sAppend.empty() || !isValidSvrfName(iter->sValue))
                         {
                             throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                 iter->nLineNo, iter->sValue);
                         }

                         if (getStringName(iter, pNode->end(), sAppend))
                         {
                             outputText(NULL, sAppend.c_str(), nOption++);
                         }
                         else
                         {
                             throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                     iter->nLineNo, iter->sValue);
                         }
                         break;
                     }
                 }
                 break;
             }
             case IDENTIFIER_NAME:
             {
                 if (!sAppend.empty() || !isValidSvrfName(iter->sValue))
                 {
                     throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                         iter->nLineNo, iter->sValue);
                     break;
                 }
             }
             case STRING_CONSTANTS:
             {
                 if (!sAppend.empty())
                 {
                     throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                         iter->nLineNo, iter->sValue);
                 }
                 if (getStringName(iter, pNode->end(), sAppend))
                 {
                     outputText(NULL, sAppend.c_str(), nOption++);
                 }
                 else
                 {
                     throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                             iter->nLineNo, iter->sValue);
                 }
                 break;
             }
             default:
                 throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                     iter->nLineNo, iter->sValue);
                 break;
         }
         ++iter;
     }

     if(!_isTvf2Trs)
     {
        (*m_oPVRSStream) << " ) ";
     }

     if (!m_isInLayerDefinition)
     {
         (*m_oPVRSStream) << std::endl;
     }
}

static bool __isBlankCh(char ch)
{
    return ((ch == ' ') || (ch == '\t') || (ch == '\n') || (ch == '\r') || (ch == '"'));
}

static bool __getHDFileName(std::string &fileName)
{
    std::string::size_type fnbp = 0;
    trim(fileName);
    fnbp = 0;
    while (fnbp < fileName.size() && !__isBlankCh(fileName[fnbp]))
    {
        fnbp++;
    }
    if (fnbp < fileName.size())
    {
        fileName.erase(fnbp);
        return false;
    }
    return true;
}

void
rcsSynPvrsConvertor::convNameSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    std::string sCmd = "";
    switch (pNode->getSpecificationType())
    {
        case LVS_REPORT:
            sCmd += (_isTvf2Trs ? "lvs_report db " : "lvs_report ( db ");
            break;
        case LVS_HEAP_DIRECTORY:
            sCmd += (_isTvf2Trs ? "lvs_temp_dir " : "lvs_temp_dir ( ");
            break;
        case LAYER_DIRECTORY:
            sCmd += (_isTvf2Trs ? "layer_dir " : "layer_dir ( ");
            break;
        default:
            return;
    }
    (*m_oPVRSStream) << sCmd << " ";
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    if (iter != pNode->end())
    {
        std::string sValue;
        if (pNode->getSpecificationType() == LVS_HEAP_DIRECTORY)
        {
            sValue = iter->sValue;
            if (!__getHDFileName(sValue))
            {
                ;
            }
            sValue.insert(0, "\"");
            sValue.insert(sValue.size(), "\"");
            outputText(NULL, sValue.c_str());
            
        }
        else if (getStringName(iter, pNode->end(), sValue))
        {
            outputText(NULL, sValue.c_str());
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
    }
#if 0
    ++iter;
    if (iter != pNode->end())
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }
#endif
    if(!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convPrecSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;
    if (pNode->getSpecificationType() == LAYOUT_PRECISION)
    {
        (*m_oPVRSStream) << (_isTvf2Trs ? "precision_db " : "precision_db ( ");
    }
    else if (pNode->getSpecificationType() == PRECISION)
    {
        (*m_oPVRSStream) << (_isTvf2Trs ? "precision " : "precision ( ");
    }
    else
    {
        return;
    }

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sNumExp1;
    std::string sNumExp2;
    if (iter == pNode->end())
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC4,
                            iter->nLineNo, iter->sValue);
    }

    if (getNumberExp(iter, pNode->end(), sNumExp1))
    {
        outputText(NULL, sNumExp1.c_str(), nOption);
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SYN5,
                            iter->nLineNo, iter->sValue);
    }

    if (iter != pNode->end())
    {
        if (getNumberExp(iter, pNode->end(), sNumExp2))
        {
            outputText(NULL, sNumExp2.c_str(), nOption);
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, SYN5,
                                iter->nLineNo, iter->sValue);
        }
    }
    if (iter != pNode->end())
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }
    if(!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convResolutionSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "resolution db " : "resolution ( db ");
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sNumExp;
    if (getNumberExp(iter, pNode->end(), sNumExp))
    {
        outputText(NULL, sNumExp.c_str());
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SYN5,
                            iter->nLineNo, iter->sValue);
    }
    sNumExp.clear();
    if (iter != pNode->end())
    {
        if (getNumberExp(iter, pNode->end(), sNumExp))
        {
            outputText(NULL, sNumExp.c_str());
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, SYN5,
                                iter->nLineNo, iter->sValue);
        }
    }
    if (iter != pNode->end())
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }
    if(!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convAssignLayerSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << (_isTvf2Trs ? "layer " : "layer ( ");

    std::list<rcsToken_T>::iterator iter  = pNode->begin();
    std::list<rcsToken_T>::iterator begin = pNode->begin();

    ++iter;
    std::string sLayerName;
    if (!getStringName(iter, pNode->end(), sLayerName))
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, OLS1, begin->nLineNo);
    }

    outputText(NULL, sLayerName.c_str(), nOption++);

    ++iter;
    while(iter != pNode->end())
    {
        outputText(NULL, iter->sValue.c_str(), nOption);
        ++iter;
    }
    if(!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convYesNoSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    std::string sCmd = "";
    switch(pNode->getSpecificationType())
    {
        case DRC_BOOLEAN_NOSNAP45:
            sCmd += (_isTvf2Trs ? "drc_merge_nosnap45 " : "drc_merge_nosnap45 ( ");
            break;
        case DRC_CELL_TEXT:
            sCmd += (_isTvf2Trs ? "drc_process_cell_text " : "drc_process_cell_text ( ");
            break;
        case DRC_EXCLUDE_FALSE_NOTCH:
            sCmd += (_isTvf2Trs ? "drc_output_reduce_false_error " : "drc_output_reduce_false_error ( ");
            break;
        case DRC_INCREMENTAL_CONNECT:
            sCmd += (_isTvf2Trs ? "incremental_connect " : "incremental_connect ( ");
            break;
        case DRC_INCREMENTAL_CONNECT_WARNING:
            sCmd += (_isTvf2Trs ? "incremental_connect_warning " : "incremental_connect_warning ( ");
            break;
        case DRC_ICSTATION:
            sCmd += (_isTvf2Trs ? "gui_priority " : "gui_priority ( ");
            break;
        case DRC_KEEP_EMPTY:
            sCmd += (_isTvf2Trs ? "drc_output_empty " : "drc_output_empty ( ");
            break;
        case ERC_KEEP_EMPTY:
            sCmd += (_isTvf2Trs ? "erc_output_empty " : "erc_output_empty ( ");
            break;
        case DRC_MAP_TEXT:

            sCmd += (_isTvf2Trs ? "drc_map_text " : "drc_map_text ( ");
            break;





        case VIRTUAL_CONNECT_BOX_COLON:
            sCmd += (_isTvf2Trs ? "virtual_connect black_box_colon " : "virtual_connect ( black_box_colon ");
            break;
        case VIRTUAL_CONNECT_COLON:
            sCmd += (_isTvf2Trs ? "virtual_connect colon " : "virtual_connect ( colon ");
            break;
        case VIRTUAL_CONNECT_INCREMENTAL:
            sCmd += (_isTvf2Trs ? "virtual_connect incremental_connect " : "virtual_connect ( incremental_connect ");
            break;
        case VIRTUAL_CONNECT_REPORT:
            sCmd += (_isTvf2Trs ? "virtual_connect report_warning " : "virtual_connect ( report_warning ");
            break;
        case VIRTUAL_CONNECT_SEMICOLON_AS_COLON:
            sCmd += (_isTvf2Trs ? "virtual_connect semicolon " : "virtual_connect ( semicolon ");
            break;
        case LAYOUT_PRESERVE_CASE:
            sCmd += (_isTvf2Trs ? "case_sensitive layout_property_net " : "case_sensitive ( layout_property_net ");
            break;
        case LAYOUT_PRESERVE_NET_CASE:
            sCmd += (_isTvf2Trs ? "case_sensitive layout_net " : "case_sensitive ( layout_net ");
            break;
        case LAYOUT_PRESERVE_PROPERTY_CASE:
            sCmd += (_isTvf2Trs ? "case_sensitive layout_property " : "case_sensitive ( layout_property ");
            break;
        case LVS_ABORT_ON_ERC_ERROR:
            sCmd += (_isTvf2Trs ? "quit erc_error " : "quit ( erc_error ");
            break;
        case LVS_ABORT_ON_SOFTCHK:
            sCmd += (_isTvf2Trs ? "quit uniconnect_check  " : "quit ( uniconnect_check ");
            break;
        case LVS_ABORT_ON_SUPPLY_ERROR:
            sCmd += (_isTvf2Trs ? "quit power_ground_error " : "quit ( power_ground_error ");
            break;
        case LVS_ALL_CAPACITOR_PINS_SWAPPABLE:
            sCmd += (_isTvf2Trs ? "device_pin_swap cap " : "device_pin_swap ( cap ");
            break;
        case LVS_BUILTIN_DEVICE_PIN_SWAP:
            sCmd += (_isTvf2Trs ? "device_pin_swap mos_res " : "device_pin_swap ( mos_res ");
            break;
        case LVS_CELL_SUPPLY:
            sCmd += (_isTvf2Trs ? "lvs_subcell_power_ground " : "lvs_subcell_power_ground ( ");
            break;
        case LVS_CHECK_PORT_NAMES:
            sCmd += (_isTvf2Trs ? "lvs_check_top_cell_port " : "lvs_check_top_cell_port ( ");
            break;
        case LVS_DISCARD_PINS_BY_DEVICE:
            sCmd += (_isTvf2Trs ? "lvs_ignore_extra_pin " : "lvs_ignore_extra_pin ( ");
            break;
        case LVS_DOWNCASE_DEVICE:
            sCmd += (_isTvf2Trs ? "case_sensitive lower_device_case " : "case_sensitive ( lower_device_case ");
            break;
        case LVS_EXACT_SUBTYPES:
            sCmd += (_isTvf2Trs ? "lvs_match_accurate_subtype " : "lvs_match_accurate_subtype ( ");
            break;
        case LVS_EXECUTE_ERC:
            sCmd += (_isTvf2Trs ? "lvs_run_erc " : "lvs_run_erc ( ");
            break;
        case LVS_EXPAND_SEED_PROMOTIONS:
            sCmd += (_isTvf2Trs ? "lvs_expand_hcell device_promotion " : "lvs_expand_hcell ( device_promotion ");
            break;
        case LVS_EXPAND_UNBALANCED_CELLS:
            sCmd += (_isTvf2Trs ? "lvs_expand_hcell unbalance " : "lvs_expand_hcell ( unbalance ");
            break;
        case LVS_FILTER_UNUSED_BIPOLAR:
            sCmd += (_isTvf2Trs ? "device_filter_unused bjt " : "device_filter_unused ( bjt ");
            break;
        case LVS_FILTER_UNUSED_CAPACITORS:
            sCmd += (_isTvf2Trs ? "device_filter_unused cap " : "device_filter_unused ( cap ");
            break;
        case LVS_FILTER_UNUSED_DIODES:
            sCmd += (_isTvf2Trs ? "device_filter_unused dio " : "device_filter_unused ( dio ");
            break;
        case LVS_FILTER_UNUSED_MOS:
            sCmd += (_isTvf2Trs ? "device_filter_unused mos " : "device_filter_unused ( mos ");
            break;
        case LVS_FILTER_UNUSED_RESISTORS:
            sCmd += (_isTvf2Trs ? "device_filter_unused res " : "device_filter_unused ( res ");
            break;
        case LVS_GLOBALS_ARE_PORTS:
            sCmd += (_isTvf2Trs ? "lvs_global_net_as_port " : "lvs_global_net_as_port ( ");
            break;
        case LVS_IGNORE_PORTS:
            sCmd += (_isTvf2Trs ? "lvs_ignore_port top_cell " : "lvs_ignore_port ( top_cell ");
            break;
        case LVS_IGNORE_TRIVIAL_NAMED_PORTS:
            sCmd += (_isTvf2Trs ? "lvs_ignore_port sub_cell " : "lvs_ignore_port ( sub_cell ");
            break;
        case LVS_INJECT_LOGIC:
            sCmd += (_isTvf2Trs ? "lvs_inject_hierarchy " : "lvs_inject_hierarchy ( ");
            break;
        case LVS_NETLIST_ALL_TEXTED_PINS:
            sCmd += (_isTvf2Trs ? "netlist extract_all_texted_pin " : "netlist ( extract_all_texted_pin ");
            break;
        case LVS_NETLIST_ALLOW_INCONSISTENT_MODEL:
            sCmd += (_isTvf2Trs ? "netlist extract_inconsistent_model " : "netlist ( extract_inconsistent_model ");
            break;
        case LVS_NETLIST_BOX_CONTENTS:
            sCmd += (_isTvf2Trs ? "netlist extract_black_box_content " : "netlist ( extract_black_box_content ");
            break;
        case LVS_NETLIST_COMMENT_CODED_SUBSTRATE:
            sCmd += (_isTvf2Trs ? "netlist substrate_in_comment " : "netlist ( substrate_in_comment ");
            break;
        case LVS_NETLIST_COMMENT_CODED_PROPERTIES:
            sCmd += (_isTvf2Trs ? "netlist property_in_comment " : "netlist ( property_in_comment ");
            break;
        case LVS_NETLIST_UNNAMED_BOX_PINS:
            sCmd += (_isTvf2Trs ? "netlist unnamed_black_box_pin " : "netlist ( unnamed_black_box_pin ");
            break;
        case LVS_OUT_OF_RANGE_EXCLUDE_ZERO:
            sCmd += (_isTvf2Trs ? "lvs_zero_property_with_tol " : "lvs_zero_property_with_tol ( ");
            break;
        case LVS_PRESERVE_BOX_CELLS:
            sCmd += (_isTvf2Trs ? "lvs_keep_black_box cell " : "lvs_keep_black_box ( cell ");
            break;
        case LVS_PRESERVE_BOX_PORTS:
            sCmd += (_isTvf2Trs ? "lvs_keep_black_box port " : "lvs_keep_black_box ( port ");
            break;
        case LVS_PRESERVE_FLOATING_TOP_NETS:
            sCmd += (_isTvf2Trs ? "lvs_keep_floating_top_net " : "lvs_keep_floating_top_net ( ");
            break;
        case LVS_PRESERVE_PARAMETERIZED_CELLS:
            sCmd += (_isTvf2Trs ? "lvs_keep_pcell " : "lvs_keep_pcell ( ");
            break;
        case LVS_REPORT_UNITS:
            sCmd += (_isTvf2Trs ? "lvs_report unit " : "lvs_report ( unit ");
            break;
        case LVS_REPORT_WARNINGS_HCELL_ONLY:
            sCmd += (_isTvf2Trs ? "lvs_report only_warning_hcell " : "lvs_report ( only_warning_hcell");
            break;
        case LVS_REPORT_WARNINGS_TOP_ONLY:
            sCmd += (_isTvf2Trs ? "lvs_report only_warning_top " : "lvs_report ( only_warning_top ");
            break;
        case LVS_REVERSE_WL:
            sCmd += (_isTvf2Trs ? "spice swap_wl " : "spice ( swap_wl ");
            break;
        case LVS_SHOW_SEED_PROMOTIONS:
            sCmd += (_isTvf2Trs ? "lvs_output_device_promotion " : "lvs_output_device_promotion ( ");
            break;
        case LVS_SOFT_SUBSTRATE_PINS:
            sCmd += (_isTvf2Trs ? "lvs_ignore_substrate_pin " : "lvs_ignore_substrate_pin ( ");
            break;
        case LVS_SPICE_ALLOW_DUPLICATE_SUBCIRCUIT_NAMES:
            sCmd += (_isTvf2Trs ? "spice permit_duplicate_subcircuit " : "spice ( permit_duplicate_subcircuit ");
            break;
        case LVS_SPICE_ALLOW_FLOATING_PINS:
            sCmd += (_isTvf2Trs ? "spice permit_floating_pin " : "spice ( permit_floating_pin ");
            break;
        case LVS_SPICE_ALLOW_INLINE_PARAMETERS:
            sCmd += (_isTvf2Trs ? "spice permit_inline_parameter " : "spice ( permit_inline_parameter ");
            break;
        case LVS_SPICE_ALLOW_UNQUOTED_STRINGS:
            sCmd += (_isTvf2Trs ? "spice permit_unquoted_string " : "spice ( permit_unquoted_string ");
            break;
        case LVS_SPICE_CONDITIONAL_LDD:
            sCmd += (_isTvf2Trs ? "spice conditional_comment_ldd " : "spice ( conditional_comment_ldd ");
            break;
        case LVS_SPICE_CULL_PRIMITIVE_SUBCIRCUITS:
            sCmd += (_isTvf2Trs ? "spice ignore_primitive_subcircuit " : "spice ( ignore_primitive_subcircuit ");
            break;
        case LVS_SPICE_IMPLIED_MOS_AREA:
            sCmd += (_isTvf2Trs ? "spice calculate_mos_area " : "spice ( calculate_mos_area ");
            break;
        case LVS_SPICE_OVERRIDE_GLOBALS:
            sCmd += (_isTvf2Trs ? "spice rewrite_global_net " : "spice ( rewrite_global_net ");
            break;
        case LVS_SPICE_PREFER_PINS:
            sCmd += (_isTvf2Trs ? "spice use_subcell_pin " : "spice ( use_subcell_pin ");
            break;
        case LVS_SPICE_REDEFINE_PARAM:
            sCmd += (_isTvf2Trs ? "spice redefine_param_value " : "spice ( redefine_param_value ");
            break;
        case LVS_SPICE_REPLICATE_DEVICES:
            sCmd += (_isTvf2Trs ? "spice split_multipler_device " : "spice ( split_multipler_device ");
            break;
        case LVS_SPICE_SLASH_IS_SPACE:
            sCmd += (_isTvf2Trs ? "spice treat_slash_as_space " : "spice ( treat_slash_as_space ");
            break;
        case LVS_SPICE_STRICT_WL:
            sCmd += (_isTvf2Trs ? "spice strict_wl " : "spice ( strict_wl ");
            break;
        case LVS_STRICT_SUBTYPES:
            sCmd += (_isTvf2Trs ? "lvs_match_strict_subtype " : "lvs_match_strict_subtype ( ");
            break;
        case LVS_PRECISE_INTERACTION:
            sCmd += (_isTvf2Trs ? "lvs_precise_overlap " : "lvs_precise_overlap ( ");
            break;
        case SOURCE_CASE:
            sCmd += (_isTvf2Trs ? "case_sensitive schematic " : "case_sensitive ( schematic ");
            break;
        case LAYOUT_CASE:
            sCmd += (_isTvf2Trs ? "case_sensitive layout " : "case_sensitive ( layout ");
            break;
        case LAYOUT_PROCESS_BOX_RECORD:
            sCmd += (_isTvf2Trs ? "layout_read box_record " : "layout_read ( box_record ");
            break;
        case LAYOUT_PROCESS_NODE_RECORD:
            sCmd += (_isTvf2Trs ? "layout_read node_record " : "layout_read ( node_record ");
            break;
        case LAYOUT_ALLOW_DUPLICATE_CELL:
            sCmd += (_isTvf2Trs ? "layout_read duplicate_cell " : "layout_read ( duplicate_cell ");
            break;
        case LAYOUT_WINDOW_CLIP:
            sCmd += (_isTvf2Trs ? "layout_select_window clip " : "layout_select_window ( clip ");
            break;
        case LAYOUT_USE_DATABASE_PRECISION:
            sCmd += (_isTvf2Trs ? "layout_use_db_precision " : "layout_use_db_precision ( ");
            break;
        case LAYOUT_ERROR_ON_INPUT:
            sCmd += (_isTvf2Trs ? "quit layout_error " : "quit ( layout_error ");
            break;
        case SNAP_OFFGRID:
            sCmd += (_isTvf2Trs ? "snap_offgrid " : "snap_offgrid ( ");
            break;
        case FLATTEN_VERY_SMALL_CELLS:
            sCmd += (_isTvf2Trs ? "flatten very_small_cell " : "flatten ( very_small_cell ");
            break;
        case LVS_NL_PIN_LOCATIONS:
            sCmd += (_isTvf2Trs ? "netlist extract_pin_location " : "netlist ( extract_pin_location  ");
            break;
        default:
            return;
    }

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if (SECONDARY_KEYWORD == iter->eType && iter->eKeyType == NONE)
    {
        outputCon2Error(pNode, *iter);
        return;
    }

    iter = pNode->begin();
    (*m_oPVRSStream) << sCmd << " ";
    ++iter;
    if (SECONDARY_KEYWORD == iter->eType && (iter->eKeyType == YES || iter->eKeyType == NO ||
       iter->eKeyType == ENABLE || iter->eKeyType == DISABLE || iter->eKeyType == NONE ||
       iter->eKeyType == LPI ))
    {
       if (iter->eKeyType == YES)
           outputText(NULL, "yes");
       else if (iter->eKeyType == NO)
           outputText(NULL, "no");
       else if (iter->eKeyType == DISABLE)
           outputText(NULL, "no");
       else if (iter->eKeyType == ENABLE)
           outputText(NULL, "yes");
       else if (iter->eKeyType == LPI)
           outputText(NULL, "lpi");
       else if (iter->eKeyType == NONE)
       {
           Utassert(false);
       }

       ++iter;
       if (pNode->getSpecificationType() == VIRTUAL_CONNECT_REPORT && iter != pNode->end() &&
          (iter->eType == IDENTIFIER_NAME || iter->eType == SECONDARY_KEYWORD) &&
          strcasecmp(iter->sValue.c_str(), "UNSATISFIED") == 0)
       {
           
           outputText(NULL, "unsatisfied");
       }
       else if (iter != pNode->end())
       {
          throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                              iter->nLineNo, iter->sValue);
       }
    }
    else
    {
       throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                           iter->nLineNo, iter->sValue);
    }
    if(!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convSourceDbSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    std::string sCmd = (_isTvf2Trs ? "schematic_input " : "schematic_input ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    if (iter->eType == SPECIFICATION_KEYWORD && iter->eKeyType == SOURCE_PATH)
    {
        (*m_oPVRSStream) << sCmd;
        outputText("path", NULL);
        ++iter;
        while(iter != pNode->end())
        {
            std::string sFilePath;
            if (!getStringName(iter, pNode->end(), sFilePath))
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                        iter->nLineNo, iter->sValue);
            }

            outputText(NULL, sFilePath.c_str());
            ++iter;
        }
    }
    else if (iter->eType == SPECIFICATION_KEYWORD && iter->eKeyType == SOURCE_PRIMARY)
    {
        (*m_oPVRSStream) << sCmd;
        ++iter;
        if (iter != pNode->end())
        {
            std::string sTopCell;
            if (getStringName(iter, pNode->end(), sTopCell))
            {
                outputText("top_cell", sTopCell.c_str());
            }
            else
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                        iter->nLineNo, iter->sValue);
            }
            ++iter;
            if (iter != pNode->end())
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                        iter->nLineNo, iter->sValue);
            }
        }
        else
        {
            outputText("top_cell", NULL);
        }
    }
    else if (iter->eType == SPECIFICATION_KEYWORD && iter->eKeyType == SOURCE_SYSTEM)
    {
        ++iter;
        if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SPICE)
        {
            (*m_oPVRSStream) << sCmd;
            outputText("format", "spice");
        }
        else
        {
            outputCon2Error(pNode,*iter);
            return;
        }
    }
    if(!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convLayerListSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    std::string sCmd = "";
    switch(pNode->getSpecificationType())
    {
        case TEXT_LAYER:
            sCmd += (_isTvf2Trs ? "label_layer text " : "label_layer ( text ");
            break;
        case PORT_LAYER_POLYGON:
            sCmd += (_isTvf2Trs ? "label_layer port_polygon " : "label_layer ( port_polygon ");
            break;
        case PORT_LAYER_TEXT:
            sCmd += (_isTvf2Trs ? "label_layer port " : "label_layer ( port ");
            break;
        case EXCLUDE_ACUTE:
            sCmd += (_isTvf2Trs ? "check_original_layer ignore_acute_edge " : "check_original_layer ( ignore_acute_edge ");
            break;
        case EXCLUDE_ANGLED:
            sCmd += (_isTvf2Trs ? "check_original_layer ignore_angled_edge " : "check_original_layer ( ignore_angled_edge ");
            break;
        case EXCLUDE_OFFGRID:
            sCmd += (_isTvf2Trs ? "check_original_layer ignore_offgrid_vertex " : "check_original_layer ( ignore_offgrid_vertex ");
            break;
        case EXCLUDE_SKEW:
            sCmd += (_isTvf2Trs ? "check_original_layer ignore_skew_edge " : "check_original_layer ( ignore_skew_edge ");
            break;
        case LAYOUT_TOP_LAYER:
            sCmd += (_isTvf2Trs ? "layout_metal_layer " : "layout_metal_layer ( ");
            break;
        case LAYOUT_BASE_LAYER:
            sCmd += (_isTvf2Trs ? "layout_device_layer " : "layout_device_layer ( ");
            break;
        case DRC_ERROR_WAIVE_LAYER:
            sCmd += (_isTvf2Trs ? "drc_waive_error by_layer " : "drc_waive_error ( by_layer ");
            break;
        case LAYOUT_WINDEL_LAYER:
            sCmd += (_isTvf2Trs ? "layout_exclude_window layer " : "layout_exclude_window ( layer ");
            break;
        case LAYOUT_WINDOW_LAYER:
            sCmd += (_isTvf2Trs ? "layout_select_window layer " : "layout_select_window ( layer ");
            break;
        case DRC_PRINT_AREA:
            sCmd += (_isTvf2Trs ? "print_log area " : "print_log ( area ");
            break;
        case DRC_PRINT_PERIMETER:
            sCmd += (_isTvf2Trs ? "print_log perimeter " : "print_log ( perimeter ");
            break;
        default:
            return;
    }
    (*m_oPVRSStream) << sCmd;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    while(iter != pNode->end())
    {
        std::string sValue;
        if (!getStringName(iter, pNode->end(), sValue))
        {
            sValue = iter->sValue;
        }
        outputText(NULL, sValue.c_str());
        ++iter;
    }

    if(!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convDeviceSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    _rcsBuildinLayerReplacer scopedLayerMap;

    static hvInt8 iPinLayer              = 1;
    static hvInt8 iTextPropertyLayer     = 2;
    static hvInt8 iSignatureContextLayer = 3;

    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << (_isTvf2Trs ? "device " : "device ( ");

    bool hasElementName = false;
    bool hasDeviceLayer = false;
    bool hasInCompleteBuiltIn = false;
    hvInt8 iLayerType   = 1;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case BY_NET:
                {
                    iLayerType = iPinLayer;
                    outputText("pin_by_net", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case BY_SHAPE:
                {
                    iLayerType = iPinLayer;
                    outputText("pin_by_shape", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case NETLIST:
                {
                    iLayerType = iPinLayer;
                    ++iter;
                    std::string sOption;
                    if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == MODEL)
                    {
                        sOption = "rc_model";
                    }
                    else if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == ELEMENT)
                    {
                        sOption = "rc_element";
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }

                    ++iter;
                    std::string sValue;
                    if (getStringName(iter, pNode->end(), sValue))
                        outputText(sOption.c_str(), sValue.c_str(), nOption++);
                    else
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                              iter->nLineNo, iter->sValue);
                    ++iter;
                    continue;
                }
                case TEXT_MODEL_LAYER:
                {
                    iLayerType = iPinLayer;
                    ++iter;
                    std::string sLayerName;
                    if (getLayerRefName_addScopedLayerMap(iter, pNode->end(), sLayerName, scopedLayerMap))
                    {
                        outputText("text_model", sLayerName.c_str(), nOption++);
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP10,
                                            iter->nLineNo, iter->sValue);
                    }
                    continue;
                }
                case TEXT_PROPERTY_LAYER:
                {
                    iLayerType = iTextPropertyLayer;
                    ++iter;
                    std::string sLayerName;
                    if (getLayerRefName_addScopedLayerMap(iter, pNode->end(), sLayerName, scopedLayerMap))
                    {
                        outputText("text_property", sLayerName.c_str(), nOption++);
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP10,
                                            iter->nLineNo, iter->sValue);
                    }
                    continue;
                }
                case SIGNATURE:
                {
                    std::string sValue;
                    pNode->getCmdValue(sValue);
                    m_oLogStream << "This Option " << iter->sValue << " in line " << iter->nLineNo
                                 << " can not be Converted correctly in this operation: " << std::endl
                                 << sValue << std::endl << std::endl;

                    s_errManager.addError(rcErrorNode_T(rcErrorNode_T::WARNING, CON2,
                                                        iter->nLineNo, iter->sValue));

                    iLayerType = iSignatureContextLayer;
                    ++iter;
                    if (iter == pNode->end())
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP18,
                                            iter->nLineNo, iter->sValue);
                    }

                    ++iter;
                    if (iter != pNode->end())
                    {
                        std::string sLayerName;
                        if (!getLayerRefName_addScopedLayerMap(iter, pNode->end(), sLayerName, scopedLayerMap))
                        {
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP10,
                                    iter->nLineNo, iter->sValue);
                        }

                    }

                    if (iter != pNode->end())
                    {
                        std::string sLayerName;
                        if (!getLayerRefName_addScopedLayerMap(iter, pNode->end(), sLayerName, scopedLayerMap))
                        {
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP10,
                                    iter->nLineNo, iter->sValue);
                        }
                    }
                    continue;
                }
                default:
                    break;
            }
        }
        else if (OPERATOR == iter->eType && iter->sValue=="<" )
        {
            iLayerType = iPinLayer;
            ++iter;
            std::string sLayerName;
            if (getLayerRefName_addScopedLayerMap(iter, pNode->end(), sLayerName, scopedLayerMap))
            {
                (*m_oPVRSStream) << "< ";
                outputText(NULL, sLayerName.c_str(), nOption++);
                (*m_oPVRSStream) << " >";
            }
            else
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP10,
                        iter->nLineNo, iter->sValue);
            }

            if (iter == pNode->end() || OPERATOR != iter->eType || iter->sValue != ">")
            {
                --iter;
                throw rcErrorNode_T(rcErrorNode_T::ERROR, DEV10,
                                    iter->nLineNo);
            }
            ++iter;
            continue;
        }
        else if (SEPARATOR == iter->eType && iter->sValue=="(")
        {
            iLayerType = iPinLayer;
            ++iter;
            std::string sSwapPinList = "( ";
            while(iter != pNode->end() && iter->eType != SEPARATOR && iter->sValue != ")")
            {
                std::string sValue;
                if (getStringName(iter, pNode->end(), sValue))
                {
                    sSwapPinList += sValue;
                }
                else
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
                sSwapPinList += " ";
                ++iter;
            }
            sSwapPinList.erase(sSwapPinList.size() - 1);
            sSwapPinList += " )";
            outputText(NULL, sSwapPinList.c_str(), nOption++);
            ++iter;
            continue;
        }
        else if (BUILT_IN_LANG == iter->eType)
        {
            iLayerType = iPinLayer;
            hasInCompleteBuiltIn = iter->sValue[iter->sValue.size() - 1] != ']';
            iter->sValue = scopedLayerMap.replaceBuildInLang(iter->sValue);
            outputBuiltInLang(pNode->getSpecificationType(), *iter, nOption);
            ++iter;
            continue;
        }

        if (!hasElementName)
        {
            std::string sElementName;
            if (!getStringName(iter, pNode->end(), sElementName))
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
            ++iter;
            if (iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
            {
                sElementName += " ( ";
                std::string sModelName;
                ++iter;
                if (!getStringName(iter, pNode->end(), sModelName))
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                      iter->nLineNo, iter->sValue);
                }
                sElementName += sModelName;
                ++iter;
                if (iter == pNode->end() || iter->eType != SEPARATOR || iter->sValue != ")")
                {
                    if (iter == pNode->end())
                        --iter;
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
                sElementName += " )";
                ++iter;
            }
            outputText(NULL, sElementName.c_str(), nOption++);
            hasElementName = true;
            continue;
        }
        else if (!hasDeviceLayer)
        {
            std::string sLayerName;
            if (getLayerRefName_addScopedLayerMap(iter, pNode->end(), sLayerName, scopedLayerMap))
            {
                outputText(NULL, sLayerName.c_str(), nOption++);
            }
            else
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, RES2,
                                    iter->nLineNo, iter->sValue);
            }
            hasDeviceLayer = true;
            continue;
        }
        else if (iPinLayer == iLayerType)
        {
            std::string sLayerName;
            if (!getLayerRefName_addScopedLayerMap(iter, pNode->end(), sLayerName, scopedLayerMap))
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, RES2,
                                    iter->nLineNo, iter->sValue);
            }
            if (iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
            {
                std::list<rcsToken_T>::iterator iTmp = iter;
                ++iTmp;
                ++iTmp;
                if (iTmp == pNode->end() || iTmp->eType != SEPARATOR || iTmp->sValue != ")")
                {
                    outputText(NULL, sLayerName.c_str(), nOption++);
                    continue;
                }

                sLayerName += " ( ";
                std::string sPinName;
                ++iter;
                getStringName(iter, pNode->end(), sPinName);
                sLayerName += sPinName;
                ++iter;
                if (iter == pNode->end() || iter->eType != SEPARATOR || iter->sValue != ")")
                {
                    if (iter == pNode->end())
                        --iter;
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
                sLayerName += " )";
                ++iter;
            }
            outputText(NULL, sLayerName.c_str(), nOption++);
            continue;
        }
        else if (iTextPropertyLayer == iLayerType)
        {
            std::string sLayerName;
            if (getLayerRefName_addScopedLayerMap(iter, pNode->end(), sLayerName, scopedLayerMap))
            {
                outputText(NULL, sLayerName.c_str(), nOption++);
            }
            else
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
            continue;
        }
        else if (iSignatureContextLayer == iLayerType)
        {
            std::string sLayerName;
            if (!getLayerRefName_addScopedLayerMap(iter, pNode->end(), sLayerName, scopedLayerMap))
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
            continue;
        }

        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    if(!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convCellNameSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;
    if (pNode->getSpecificationType() == DRC_CELL_NAME)
    {
        (*m_oPVRSStream) << (_isTvf2Trs ? "drc_output_cell_name " : "drc_output_cell_name ( " );
    }
    else if (pNode->getSpecificationType() == ERC_CELL_NAME)
    {
        (*m_oPVRSStream) << (_isTvf2Trs ? "erc_output_cell_name " : "erc_output_cell_name ( " );
    }
    else
    {
        return;
    }

    bool isCellName = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if (SECONDARY_KEYWORD == iter->eType && (iter->eKeyType == YES || iter->eKeyType == NO))
    {
        if (iter->eKeyType == YES)
        {
            outputText(NULL, "yes", nOption);
        }
        else
        {
            outputText(NULL, "no", nOption);
        }

        isCellName = iter->eKeyType == YES;
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    ++iter;
    if (isCellName)
    {
        for(; iter != pNode->end(); ++iter)
        {
            switch(iter->eType)
            {
                case SECONDARY_KEYWORD:
                {
                    switch(iter->eKeyType)
                    {
                        case XFORM:
                        {
                            outputText("transform_to_top", NULL, nOption);
                            break;
                        }
                        case CELL_SPACE:
                        {
                            outputText("cell_coordinate", NULL, nOption);
                            break;
                        }
                        case ALL:
                        {
                            outputText("all", NULL, nOption);
                            break;
                        }
                        default:
                        {
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                iter->nLineNo, iter->sValue);
                            break;
                        }
                    }
                    break;
                }
                default:
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                    break;
                }
            }
        }
    }
    else if (iter != pNode->end())
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }
    if(!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convRuleSelectSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    std::string sCmd = "";
    switch(pNode->getSpecificationType())
    {
        case DRC_SELECT_CHECK:
            sCmd += (_isTvf2Trs ? "drc_select_rule by_name " : "drc_select_rule ( by_name ");
            break;
        case DRC_UNSELECT_CHECK:
            sCmd += (_isTvf2Trs ? "drc_ignore_rule by_name " : "drc_ignore_rule ( by_name ");
            break;
        case DRC_SELECT_CHECK_BY_LAYER:
            sCmd += (_isTvf2Trs ? "drc_select_rule by_layer " : "drc_select_rule ( by_layer ");
            break;
        case DRC_UNSELECT_CHECK_BY_LAYER:
            sCmd += (_isTvf2Trs ? "drc_ignore_rule by_layer " : "drc_ignore_rule ( by_layer ");
            break;
        case ERC_SELECT_CHECK:
            sCmd += (_isTvf2Trs ? "erc_select_rule by_name " : "erc_select_rule ( by_name ");
            break;
        case ERC_SELECT_CHECK_ABORT_LVS:
            sCmd += (_isTvf2Trs ? "erc_select_rule quit_lvs by_name " : "erc_select_rule ( quit_lvs by_name ");
            break;
        case ERC_UNSELECT_CHECK:
            sCmd += (_isTvf2Trs ? "erc_ignore_rule by_name " : "erc_ignore_rule ( by_name ");
            break;
        case DFM_SELECT_CHECK:
            sCmd += (_isTvf2Trs ? "dfm_select_rule by_name " : "dfm_select_rule ( by_name ");
            break;
        case DFM_UNSELECT_CHECK:
            sCmd += (_isTvf2Trs ? "dfm_ignore_rule by_name " : "dfm_ignore_rule ( by_name ");
            break;
        default:
            return;
    }

    (*m_oPVRSStream) << sCmd;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    while(iter != pNode->end())
    {
        std::string sValue;
        if (!getStringName(iter, pNode->end(), sValue))
        {
            sValue = iter->sValue;
        }
        outputText(NULL, sValue.c_str());
        ++iter;
    }
    if(!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convMakeRuleGroupSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << (_isTvf2Trs ? "group_rule " : "group_rule ( ");

    std::list<rcsToken_T>::iterator iter  = pNode->begin();
    std::list<rcsToken_T>::iterator begin = pNode->begin();

    ++iter;
    std::string sGroupName;
    if (!getStringName(iter, pNode->end(), sGroupName))
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, OLS1, begin->nLineNo);
    }

    outputText(NULL, sGroupName.c_str(), nOption++);

    ++iter;
    while(iter != pNode->end())
    {
        std::string sValue;
        if (!getStringName(iter, pNode->end(), sValue))
        {
           sValue = iter->sValue;
        }

        outputText(NULL, sValue.c_str(), nOption);
        ++iter;
    }
    if(!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

static bool
isRuleMapKeyword(const rcsToken_T &token)
{
    if (token.eType == SECONDARY_KEYWORD)
    {
        switch(token.eKeyType)
        {
            case APPEND:
            case PREFIX:
            case TEXTTAG:
            case MAXIMUM:
            case GDSII:
            case OASIS:
            case BINARY:
            case ASCII:
            case USER:
            case PSEUDO:
            case AREF:
            case AUTOREF:
            case SUBSTITUTE:
                return true;
            default:
                return false;
        }
    }
    return false;
}

static void __getPipeName(std::string &fileName)
{
    std::string::size_type fnbp = 0;
    while (fnbp < fileName.size() && __isBlankCh(fileName[fnbp]))
    {
        fnbp++;
    }
    if (fnbp > 0)
    {
        fileName.erase(0,fnbp);
    }
    fnbp = 0;
    while (fnbp < fileName.size() && !__isBlankCh(fileName[fnbp]))
    {
        fnbp++;
    }
    if (fnbp < fileName.size())
    {
        fileName.erase(fnbp);
    }
}

void
rcsSynPvrsConvertor::convRuleMapSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;
    bool bHasFileName = false;
    (*m_oPVRSStream) << (_isTvf2Trs ? "drc_rule_map " : "drc_rule_map ( ");
    std::list<rcsToken_T>::iterator iter  = pNode->begin();
    std::list<rcsToken_T>::iterator begin = pNode->begin();
    ++iter;
    switch(iter->eType)
    {
        case SECONDARY_KEYWORD:
        case IDENTIFIER_NAME:
        {
            if (!isValidSvrfName(iter->sValue))
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, DCM1,
                                    begin->nLineNo);
            }
        }
        case STRING_CONSTANTS:
        {
            std::string sRuleName;
            if (getStringName(iter, pNode->end(), sRuleName))
            {
                outputText(NULL, sRuleName.c_str(), nOption++);
            }
            else
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, DCM1,
                                    iter->nLineNo, iter->sValue);
            }
            ++iter;
            break;
        }
        default:
            throw rcErrorNode_T(rcErrorNode_T::ERROR, DCM1,
                                begin->nLineNo);
    }

     if (iter->eType == SECONDARY_KEYWORD)
     {
         switch(iter->eKeyType)
         {
              case OASIS:
             case GDSII:
             {
                 if(iter->eKeyType == OASIS)
                 {
                     outputText(NULL, "oasis", nOption++);
                 }
                 else
                 {
                     outputText(NULL, "gds", nOption++);
                 }
                 ++iter;
                 if (iter->eKeyType == PROPERTIES)
                 {
                     outputCon2Error(pNode,*iter);
                     ++iter;
                     break;
                 }
                 if (iter == pNode->end() || (iter->eType != NUMBER && iter->eType != OPERATOR))
                 {
                     std::string value = iter->sValue;
                     if(value.find("[expr") != std::string::npos)
                     {
                         outputText(NULL, value.c_str(), nOption++);
                     }
                     else
                     {
                         break;
                     }
                 }

                 std::string sLayerNum;
                 if (getNumberExp(iter, pNode->end(), sLayerNum, false, false))
                 {
                     outputText(NULL, sLayerNum.c_str(), nOption++);
                 }
                 else
                 {
                     throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                         iter->nLineNo, iter->sValue);
                 }

                 if (iter->eKeyType == PROPERTIES)
                 {
                     outputCon2Error(pNode,*iter);
                     ++iter;
                     break;
                 }
                 if (iter == pNode->end() || (iter->eType != NUMBER && iter->eType != OPERATOR))
                 {
                     break;
                 }

                 std::string sDataType;
                 if (getNumberExp(iter, pNode->end(), sDataType, false, false))
                 {
                     outputText(NULL, sDataType.c_str(), nOption++);
                 }
                 else
                 {
                     throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                         iter->nLineNo, iter->sValue);
                 }
                 if (iter->eKeyType == PROPERTIES)
                 {
                     outputCon2Error(pNode,*iter);
                     ++iter;
                     break;
                 }
                 break;
             }
             case ASCII:
             {
                 outputText("text_format", NULL, nOption++);
                 ++iter;
                 break;
             }
             case BINARY:
             {
                 outputCon2Error(pNode,*iter);
                 ++iter;
                 break;
             }
             default:
                 break;
         }
     }
     else
     {
         if (iter->eKeyType == PROPERTIES)
         {
             outputCon2Error(pNode,*iter);
             ++iter;
         }
         if (iter != pNode->end() && (iter->eType == NUMBER || iter->eType == OPERATOR))
         {
             std::string sLayerNum;
             if (getNumberExp(iter, pNode->end(), sLayerNum, false, false))
             {
                 outputText(NULL, sLayerNum.c_str(), nOption++);
             }
             else
             {
                 throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                     iter->nLineNo, iter->sValue);
             }

             if (iter->eKeyType == PROPERTIES)
             {
                 outputCon2Error(pNode,*iter);
                 ++iter;
             }
             if (iter != pNode->end() && (iter->eType == NUMBER || iter->eType == OPERATOR))
             {
                 std::string sDataType;
                 if (getNumberExp(iter, pNode->end(), sDataType, false, false))
                 {
                     outputText(NULL, sDataType.c_str(), nOption++);
                 }
                 else
                 {
                     throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                         iter->nLineNo, iter->sValue);
                 }
                 if (iter->eKeyType == PROPERTIES)
                 {
                     outputCon2Error(pNode,*iter);
                     ++iter;
                 }
             }
         }
     }

     while(iter != pNode->end())
     {
         if (iter->eType == SECONDARY_KEYWORD)
         {
             switch(iter->eKeyType)
             {
                 case APPEND:
                 {
                     std::string sAppend;
                     if (!getStringName(++iter, pNode->end(), sAppend))
                     {
                         --iter;
                         throw rcErrorNode_T(rcErrorNode_T::ERROR, INP4,
                                             iter->nLineNo, iter->sValue);
                     }

                     outputText("with_append", sAppend.c_str(), nOption++);
                     ++iter;
                     continue;
                 }
                 case PREFIX:
                 {
                     std::string sPrefix;
                     if (!getStringName(++iter, pNode->end(), sPrefix))
                     {
                         --iter;
                         throw rcErrorNode_T(rcErrorNode_T::ERROR, INP4,
                                             iter->nLineNo, iter->sValue);
                     }

                     outputText("with_prefix", sPrefix.c_str(), nOption++);
                     ++iter;
                     continue;
                 }
                 case TEXTTAG:
                 {
                     std::string sTextTag;
                     if (!getStringName(++iter, pNode->end(), sTextTag))
                     {
                         --iter;
                         throw rcErrorNode_T(rcErrorNode_T::ERROR, INP4,
                                             iter->nLineNo, iter->sValue);
                     }

                     outputText("add_text", sTextTag.c_str(), nOption++);
                     ++iter;
                     continue;
                 }
                 case MAXIMUM:
                 {
                     if (++iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                        (iter->eKeyType == VERTICES || iter->eKeyType == RESULTS))
                     {
                         std::string sOption;
                         if (iter->eKeyType == VERTICES)
                         {
                             sOption = "max_vertex";
                         }
                         else
                         {
                             sOption = "max_result";
                         }

                         if (++iter != pNode->end())
                         {
                             if (SECONDARY_KEYWORD == iter->eType && iter->eKeyType == ALL)
                             {
                                 outputText(sOption.c_str(), "all", nOption++);
                                 ++iter;
                             }
                             else
                             {
                                 std::string sMax;
                                 if (getNumberExp(iter, pNode->end(), sMax))
                                 {
                                     outputText(sOption.c_str(), sMax.c_str(), nOption++);
                                 }
                                 else
                                 {
                                     throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                         iter->nLineNo, iter->sValue);
                                 }
                             }
                         }
                         else
                         {
                             throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                 begin->nLineNo, begin->sValue);
                         }
                     }
                     else
                     {
                         --iter;
                         throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                             iter->nLineNo, iter->sValue);
                     }
                     continue;
                 }
                 case USER:
                 {
                     if (++iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                        iter->eKeyType == MERGED)
                     {
                         outputText(NULL, "merged", nOption++);
                     }
                     else
                     {
                         outputText(NULL, "user_cell", nOption++);
                         --iter;
                     }
                     ++iter;
                     continue;
                 }
                 case INDEX:
                 {
                     if (++iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                        iter->eKeyType == NOVIEW)
                     {
                         outputCon2Error(pNode,*iter);
                     }
                     else
                     {
                         --iter;
                         outputCon2Error(pNode,*iter);
                     }
                     ++iter;
                     continue;
                 }
                 case PSEUDO:
                 {
                     outputText("keep_injected_cell", NULL, nOption++);
                     ++iter;
                     continue;
                 }
                 case TOP:
                 {
                     outputText("top_cell", NULL, nOption++);
                     ++iter;
                     continue;
                 }
                 case PREC:
                 {
                     ++iter;
                     std::string sNumExp;
                     if (getNumberExp(iter, pNode->end(), sNumExp))
                     {
                         outputText("db_precision", sNumExp.c_str(), nOption++);
                     }
                     else
                     {
                         throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                             iter->nLineNo, iter->sValue);
                     }
                     continue;
                 }
                 case MAG:
                 {
                     ++iter;
                     std::string sNumExp;
                     if (getNumberExp(iter, pNode->end(), sNumExp))
                     {
                         outputText("db_magnify", sNumExp.c_str(), nOption++);
                     }
                     else
                     {
                         throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                             iter->nLineNo, iter->sValue);
                     }
                     continue;
                 }
                 case AREF:
                 {
                     std::list<rcsToken_T>::iterator begin = iter;
                     std::string sValue;
                     ++iter;
                     if (getStringName(iter, pNode->end(), sValue))
                     {
                         sValue = iter->sValue;
                         toLower(sValue);
                         outputText("inject_array", sValue.c_str(), nOption++);
                     }
                     else
                     {
                         --iter;
                         throw rcErrorNode_T(rcErrorNode_T::ERROR, INP4,
                                             begin->nLineNo, begin->sValue);
                     }

                     ++iter;
                     std::string sNumExp;
                     if (getNumberExp(iter, pNode->end(), sNumExp))
                     {
                         outputText(NULL, sNumExp.c_str(), nOption);
                     }
                     else
                     {
                         throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                             iter->nLineNo, iter->sValue);
                     }
                     sNumExp.clear();
                     if (getNumberExp(iter, pNode->end(), sNumExp))
                     {
                         outputText(NULL, sNumExp.c_str(), nOption);
                     }
                     else
                     {
                         throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                             iter->nLineNo, iter->sValue);
                     }
                     if (iter != pNode->end() && !isRuleMapKeyword(*iter))
                     {
                         sNumExp.clear();
                         if (getNumberExp(iter, pNode->end(), sNumExp))
                         {
                             outputText(NULL, sNumExp.c_str(), nOption);
                         }
                         else
                         {
                             throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                 iter->nLineNo, iter->sValue);
                         }
                     }
                     if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == MAXIMUM)
                     {
                         outputCon2Error(pNode,*iter);
                         ++iter;
                         if (iter == pNode->end())
                         {
                             --iter;
                             throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                 iter->nLineNo, iter->sValue);
                         }
                         if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == PITCH)
                         {
                             outputCon2Error(pNode,*iter);
                         }
                         else
                         {
                             throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                 iter->nLineNo, iter->sValue);
                         }
                         ++iter;
                         if (iter == pNode->end())
                         {
                             --iter;
                             throw rcErrorNode_T(rcErrorNode_T::ERROR, INP11,
                                                 iter->nLineNo, iter->sValue);
                         }
                         if (!getNumberExp(iter, pNode->end(), sNumExp))

                         {
                             throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                     iter->nLineNo, iter->sValue);
                         }
                     }

                     while(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SUBSTITUTE)
                     {
                         if (++iter == pNode->end() || isRuleMapKeyword(*iter))
                         {
                             throw rcErrorNode_T(rcErrorNode_T::ERROR, DCM13,
                                                 begin->nLineNo, sValue);
                         }

                         std::vector<std::string> vPoints;
                         while(iter != pNode->end())
                         {
                             std::string sNumExp;
                             if (!getNumberExp(iter, pNode->end(), sNumExp, false, false))
                                 break;
                             vPoints.push_back(sNumExp);

                             if (iter == pNode->end() || isRuleMapKeyword(*iter))
                                 break;
                         }

                         Utassert(!vPoints.empty());
                         outputText("by_polygon ", vPoints[0].c_str(), nOption++);
                         for(hvUInt32 iPoint = 1; iPoint < vPoints.size(); ++iPoint)
                         {
                             outputText(NULL, vPoints[iPoint].c_str(), nOption);
                         }
                     }
                     continue;
                 }
                 case AUTOREF:
                 {
                     std::string sValue;
                     ++iter;
                     if (iter != pNode->end() && getStringName(iter, pNode->end(), sValue))
                     {
                         toLower(sValue);
                         outputText("auto_inject_array", sValue.c_str(), nOption++);
                         ++iter;
                     }
                     else
                     {
                         outputText("auto_inject_array", NULL, nOption++);
                     }
                     continue;
                 }
                 default:
                     break;
             }
         }

         std::string sFileName;
         if (!bHasFileName && getStringName(iter, pNode->end(), sFileName))
         {
             std::string sTmpName = sFileName;
             __getPipeName(sTmpName);






                 outputText(NULL, sFileName.c_str(), nOption++);


         }
         else
         {
             throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo, iter->sValue);
         }
         ++iter;
     }
     if(!_isTvf2Trs)
     {
         (*m_oPVRSStream) << " ) ";
     }
     if (!m_isInLayerDefinition)
     {
        (*m_oPVRSStream) << std::endl;
     }
}

void
rcsSynPvrsConvertor::convRuleTextSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if (iter->eKeyType == RFI)
    {
        ++iter;
        if (iter == pNode->end())
        {
            iter--;
            outputCon2Error(pNode,*iter);
            return;
        }
    }
    iter = pNode->begin();
    ++iter;
    if (pNode->getSpecificationType() == DRC_CHECK_TEXT)
    {
        (*m_oPVRSStream) << (_isTvf2Trs ? "drc_output_rule_text " : "drc_output_rule_text ( ");
    }
    else if (pNode->getSpecificationType() == ERC_CHECK_TEXT)
    {
        (*m_oPVRSStream) << (_isTvf2Trs ? "erc_output_rule_text " : "erc_output_rule_text ( ");
    }
    else
    {
        return;
    }
    for(; iter != pNode->end(); ++iter)
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                switch(iter->eKeyType)
                {
                    case ALL:
                    {
                        outputText(NULL, "all", nOption++);
                        break;
                    }
                    case COMMENTS:
                    {
                        outputText(NULL, "comment", nOption++);
                        break;
                    }
                    case RFI:
                    {
                        break;
                    }
                    case NONE:
                    {
                        outputText(NULL, "none", nOption++);
                        break;
                    }
                    default:
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                            iter->nLineNo, iter->sValue);
                }
                break;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
        }
    }
    if(!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convLayerMapSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;

    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    
    (*m_oPVRSStream) << (_isTvf2Trs ? "layer_map " : "layer_map ( ");
    ++iter;
    if (NUMBER == iter->eType || (OPERATOR == iter->eType &&
       ("+" == iter->sValue || "-" == iter->sValue)))
    {
        std::string sNumExp;
        if (getNumberExp(iter, pNode->end(), sNumExp))
        {
            outputText(NULL, sNumExp.c_str(), nOption++);
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
    }
    else if (OPERATOR == iter->eType)
    {
        std::string sCons;
        if (getConstraint(iter, pNode->end(), sCons))
        {
            outputText(NULL, sCons.c_str(), nOption++);
        }
        else
        {
            if (iter == pNode->end())
            {
                --iter;
            }
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    bool isTextType = false;
    std::string sType;
    if (iter != pNode->end() && SECONDARY_KEYWORD == iter->eType && (DATATYPE == iter->eKeyType ||
       TEXTTYPE == iter->eKeyType))
    {
        if (iter->eKeyType == DATATYPE)
        {
            sType = "datatype";
        }
        else
        {
            sType = "texttype";
            isTextType = true;
        }
    }
    else
    {
        if (iter == pNode->end())
        {
            --iter;
        }
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    ++iter;
    if (iter == pNode->end())
    {
        --iter;
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2,
                            iter->nLineNo, iter->sValue);
    }

    if (NUMBER == iter->eType || (OPERATOR == iter->eType &&
       ("+" == iter->sValue || "-" == iter->sValue)))
    {
        std::string sNumExp;
        if (getNumberExp(iter, pNode->end(), sNumExp))
        {
            outputText(sType.c_str(), sNumExp.c_str(), nOption++);
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
    }
    else if (OPERATOR == iter->eType)
    {
        std::string sCons;
        if (getConstraintNoExpr(iter, pNode->end(), sCons))
        {
            outputText(sType.c_str(), sCons.c_str(), nOption++);
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    if (iter == pNode->end())
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2,
                            pNode->begin()->nLineNo, pNode->begin()->sValue);
    }

    std::string sNumExp;
    if ((OPERATOR == iter->eType) && ("-" == iter->sValue))
    {
        ++iter;
        if (iter == pNode->end())
        {
            --iter;
            throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2,
                                iter->nLineNo, iter->sValue);
        }
        if ((NUMBER == iter->eType) && ("0" == iter->sValue))
        {
            outputText(NULL,"0",nOption++);
            ++iter;
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
    }
    else if (getNumberExp(iter, pNode->end(), sNumExp))
    {
        outputText(NULL, sNumExp.c_str(), nOption++);
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    if(iter != pNode->end())
    {
        if(isTextType)
        {
            std::string sNumExp;
            if(getNumberExp(iter, pNode->end(), sNumExp))
            {
                outputText(NULL, sNumExp.c_str(), nOption++);
            }
            else
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
    }

    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convCompareCaseSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << (_isTvf2Trs ? "case_sensitive compare " : "case_sensitive ( compare ");
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == YES)
    {
        outputText(NULL, "yes", nOption++);
        if (++iter != pNode->end())
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                pNode->begin()->nLineNo, pNode->begin()->sValue);
        }
    }
    else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == NO)
    {
        outputText(NULL, "no", nOption++);
        if (++iter != pNode->end())
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                pNode->begin()->nLineNo, pNode->begin()->sValue);
        }
    }
    else
    {
        while(iter != pNode->end())
        {
            switch(iter->eType)
            {
                case SECONDARY_KEYWORD:
                {
                    switch(iter->eKeyType)
                    {
                        case NAMES:
                            outputText(NULL, "name", nOption++);
                            ++iter;
                            continue;
                        case TYPES:
                            outputText(NULL, "type", nOption++);
                            ++iter;
                            continue;
                        case SUBTYPES:
                            outputText(NULL, "subtype", nOption++);
                            ++iter;
                            continue;
                        case VALUES:
                            outputText(NULL, "string_value", nOption++);
                            ++iter;
                            continue;
                        default:
                            break;
                    }
                }
                default:
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                    break;
                }
            }
            ++iter;
        }
    }
    if(!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convNetListOutputSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << (_isTvf2Trs ? "lvs_output_netlist " : "lvs_output_netlist ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    if (iter->eType == SPECIFICATION_KEYWORD)
    {
        if (iter->eKeyType == LVS_WRITE_INJECTED_LAYOUT_NETLIST)
        {
            outputText(NULL, "injected_layout", nOption++);
        }
        else if (iter->eKeyType == LVS_WRITE_INJECTED_SOURCE_NETLIST)
        {
            outputText(NULL, "injected_schematic", nOption++);
        }
        else if (iter->eKeyType == LVS_WRITE_LAYOUT_NETLIST)
        {
            outputText(NULL, "layout", nOption++);
        }
        else if (iter->eKeyType == LVS_WRITE_SOURCE_NETLIST)
        {
            outputText(NULL, "schematic", nOption++);
        }
    }

    ++iter;
    std::string sFileName;
    if (getStringName(iter, pNode->end(), sFileName))
    {
        outputText(NULL, sFileName.c_str(), nOption++);
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }
    ++iter;
    if (iter != pNode->end())
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    if(!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convCheckPropertySpec(rcsSynSpecificationNode_T *pNode)
{
    if (_hasOnlyDirectKeyword(pNode))
    {
        outputCon1Error(pNode);
        return;
    }
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << (_isTvf2Trs ? "check_property " : "check_property ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sComponentType;
    if (!getStringName(iter, pNode->end(), sComponentType))
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    ++iter;

    if (iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
    {
        sComponentType += "( ";
        ++iter;
        std::string sSubTypes;

        if (!getStringName(iter, pNode->end(), sSubTypes))
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
        sComponentType += sSubTypes;
        ++iter;
        sComponentType += " )";
        ++iter;
    }

    outputText(NULL, sComponentType.c_str(), nOption++);

    if(pNode->isInMacro())
    {
        while(iter != pNode->end())
        {
            (*m_oPVRSStream) << iter->sValue << " ";
            iter++;
        }

        if(!_isTvf2Trs)
        {
            (*m_oPVRSStream) << " ) ";
        }

        if (!m_isInLayerDefinition)
        {
            (*m_oPVRSStream) << std::endl;
        }
        return;
    }

    if (iter != pNode->end() && iter->eType == BUILT_IN_LANG)
    {
        outputBuiltInLang(pNode->getSpecificationType(), *iter, nOption);

        if(!_isTvf2Trs)
        {
            (*m_oPVRSStream) << " ) ";
        }

        if (!m_isInLayerDefinition)
            (*m_oPVRSStream) << std::endl;
        return;
    }

    std::string sSourceProperty;
    if (!getStringName(iter, pNode->end(), sSourceProperty))
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    ++iter;
    if (iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
    {
        sSourceProperty += iter->sValue;
        ++iter;
        std::string sSpicePara;

        if (!getStringName(iter, pNode->end(), sSpicePara))
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
        sSourceProperty += sSpicePara;
        ++iter;
        sSourceProperty += iter->sValue;
        ++iter;
    }

    outputText(NULL, sSourceProperty.c_str(), nOption++);

    std::string sLayoutProperty;
    if (!getStringName(iter, pNode->end(), sLayoutProperty))
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    ++iter;
    if (iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
    {
        sLayoutProperty += iter->sValue;
        ++iter;
        std::string sSpicePara;

        if (!getStringName(iter, pNode->end(), sSpicePara))
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
        sLayoutProperty += sSpicePara;
        ++iter;
        sLayoutProperty += iter->sValue;
        ++iter;
    }

    outputText(NULL, sLayoutProperty.c_str(), nOption++);

    bool hasTolerance1 = false;
    bool hasTolerance2 = false;
    while(iter != pNode->end())
    {
        if (iter->eType == SECONDARY_KEYWORD)
        {
            if (iter->eKeyType == STRING)
            {
                ++iter;
                if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == NO)
                {
                    outputText("not_string", NULL, nOption++);
                    ++iter;
                }
                else
                {
                    outputText("string", NULL, nOption++);
                }
                continue;
            }
            else if (iter->eKeyType == NO)
            {
                outputText("no", NULL, nOption++);
                ++iter;
                continue;
            }
            else if (iter->eKeyType == ABSOLUTE)
            {
                outputText("absolute_value", NULL, nOption++);
                ++iter;
                continue;
            }
            else if (iter->eKeyType == CELL)
            {
                ++iter;
                if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == LIST)
                {
                    ++iter;
                    std::string sCellListName;
                    if (!getStringName(iter, pNode->end(), sCellListName))
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                            iter->nLineNo, iter->sValue);
                    outputText("cell_group", sCellListName.c_str(), nOption++);
                    ++iter;
                }
                continue;
            }
            else if (iter->eKeyType == DIRECT || iter->eKeyType == MASK)
            {
                outputCon2Error(pNode, *iter);
                ++iter;
                continue;
            }
        }

        if (iter->eType == BUILT_IN_LANG)
        {
            outputBuiltInLang(pNode->getSpecificationType(), *iter, nOption);
            ++iter;
            continue;
        }
        else
        {
            if (hasTolerance1 && hasTolerance2)
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
            std::string sNumExp;
            if (!getNumberExp(iter, pNode->end(), sNumExp))
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
            if (iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
            {
                ++iter;
                std::string sTolProp;
                if (!getStringName(iter, pNode->end(), sTolProp))
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
                sNumExp += " (";
                sNumExp += sTolProp;
                ++iter;
                sNumExp += ")";
                ++iter;
            }
            if (!hasTolerance1)
            {
                outputText(NULL, sNumExp.c_str(), nOption++);
                hasTolerance1 = true;
            }
            else if (!hasTolerance2)
            {
                outputText(NULL, sNumExp.c_str(), nOption++);
                hasTolerance2 = true;
            }
            continue;
        }
    }

    if(!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convReduceSpecificSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;
    std::string cSpec = (_isTvf2Trs ? "device_merge " : "device_merge ( ");
    switch(pNode->getSpecificationType())
    {
        case LVS_REDUCE_PARALLEL_BIPOLAR:
            cSpec += "parallel_bjt ";
            break;
        case LVS_REDUCE_PARALLEL_CAPACITORS:
            cSpec += "parallel_cap ";
            break;
        case LVS_REDUCE_PARALLEL_DIODES:
            cSpec += "parallel_dio ";
            break;
        case LVS_REDUCE_PARALLEL_MOS:
            cSpec += "parallel_mos ";
            break;
        case LVS_REDUCE_PARALLEL_RESISTORS:
            cSpec += "parallel_res ";
            break;
        case LVS_REDUCE_SEMI_SERIES_MOS:
            cSpec += "partial_series_mos ";
            break;
        case LVS_REDUCE_SERIES_CAPACITORS:
            cSpec += "series_cap ";
            break;
        case LVS_REDUCE_SERIES_MOS:
            cSpec += "series_mos ";
            break;
        case LVS_REDUCE_SERIES_RESISTORS:
            cSpec += "series_res ";
            break;
        default:
            return;
    }
    (*m_oPVRSStream) << cSpec;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == YES)
    {
        outputText(NULL, "yes", nOption++);
    }
    else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == NO)
    {
        outputText(NULL, "no", nOption++);
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    ++iter;
    if (iter != pNode->end() && iter->eType == BUILT_IN_LANG)
    {
        outputBuiltInLang(pNode->getSpecificationType(), *iter, nOption);
    }

    if(!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convReduceSplitGatesSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << (_isTvf2Trs ? "device_merge split_gate " : "device_merge ( split_gate ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == YES)
    {
        outputText(NULL, "yes", nOption++);
    }
    else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == NO)
    {
        outputText(NULL, "no", nOption++);
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    ++iter;
    for(; iter != pNode->end(); ++iter)
    {
        if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SEMI)
        {
            ++iter;
            if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == ALSO)
            {
                outputText("partial", NULL, nOption++);
                continue;
            }
        }
        else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SAME)
        {
            ++iter;
            if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == ORDER)
            {
                outputText("in_order", NULL, nOption++);
                continue;
            }
        }
        else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == WITHIN)
        {
            ++iter;
            if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == TOLERANCE)
            {
                outputText("in_tolerance", NULL, nOption++);
                continue;
            }
        }
        else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == MIX)
        {
            ++iter;
            if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == TYPES)
            {
                outputText("different_type", NULL, nOption++);
                continue;
            }
        }
        else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SP)
        {
            ++iter;
            if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == ALSO)
            {
                outputText("series_parallel", NULL, nOption++);
                continue;
            }
        }
        else if (iter->eType == BUILT_IN_LANG)
        {
            outputBuiltInLang(pNode->getSpecificationType(), *iter, nOption);
            continue;
        }

        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convShortEquivalentNodesSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << (_isTvf2Trs ? "lvs_short_equivalent_net " : "lvs_short_equivalent_net ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();

    ++iter;
    if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == NO)
    {
        outputText(NULL, "no", nOption++);
    }
    else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == PARALLEL_ONLY)
    {
        outputText(NULL, "parallel", nOption++);
    }
    else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SPLIT)
    {
        outputText(NULL, "split", nOption++);
    }

    ++iter;
    for(; iter != pNode->end(); ++iter)
    {
        if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == WITHIN)
        {
            ++iter;
            if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == TOLERANCE)
            {
                outputText(NULL, "in_tolerance", nOption++);
                continue;
            }
        }

        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convReduceGenericSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << (_isTvf2Trs ? "device_merge user_define " : "device_merge ( user_define ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sComponentType;
    if (!getStringName(iter, pNode->end(), sComponentType))
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);

    ++iter;

    if (iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
    {
        sComponentType += "( ";
        ++iter;
        std::string sSubType;

        if (!getStringName(iter, pNode->end(), sSubType))
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        sComponentType += sSubType;
        ++iter;
        sComponentType += " )";
        ++iter;
    }
    outputText(NULL, sComponentType.c_str(), nOption++);

    if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == PARALLEL_ONLY)
    {
        outputText(NULL, "parallel", nOption++);
        ++iter;
    }
    else if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SERIES)
    {
        outputText(NULL, "series", nOption++);
        ++iter;
        std::string sPinName1;
        if (getStringName(iter, pNode->end(), sPinName1))
        {
            outputText(NULL, sPinName1.c_str(), nOption++);
            ++iter;
            std::string sPinName2;
            if (getStringName(iter, pNode->end(), sPinName2))
            {
                outputText(NULL, sPinName2.c_str(), nOption++);
            }
            else
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
            ++iter;
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
    }

    if(pNode->isInMacro())
    {
        while(iter != pNode->end())
        {
            (*m_oPVRSStream) << iter->sValue << " ";
            iter++;
        }

        if (!_isTvf2Trs)
        {
            (*m_oPVRSStream) << " ) ";
        }

        if (!m_isInLayerDefinition)
        {
            (*m_oPVRSStream) << std::endl;
        }
        return;
    }

    while(iter != pNode->end())
    {
        if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == YES)
        {
            outputText(NULL, "yes", nOption++);
        }
        else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == CELL)
        {
            ++iter;
            if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == LIST)
            {
                ++iter;
                std::string sCellListName;
                if (getStringName(iter, pNode->end(), sCellListName))
                {
                    outputText("cell_group", sCellListName.c_str(), nOption++);
                }
                else
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
                ++iter;
            }
            continue;
        }
        else if (iter->eType == BUILT_IN_LANG)
        {
            outputBuiltInLang(pNode->getSpecificationType(), *iter, nOption);
        }
        else if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == NO)
        {
            outputText(NULL, "no", nOption++);
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
        ++iter;
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convDepthSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    std::string sCmd = "";
    switch (pNode->getSpecificationType())
    {
        case DRC_MAP_TEXT_DEPTH:


            sCmd += (_isTvf2Trs ? "drc_map_text_level " : "drc_map_text_level ( ");
            break;
        case LAYOUT_DEPTH:
            sCmd += (_isTvf2Trs ? "layout_read_level " : "layout_read_level ( ");
            break;
        case TEXT_DEPTH:
            sCmd += (_isTvf2Trs ? "label_level text " : "label_level ( text ");
            break;
        case PORT_DEPTH:
            sCmd += (_isTvf2Trs ? "label_level port " : "label_level ( port ");
            break;
        case VIRTUAL_CONNECT_DEPTH:
            sCmd += (_isTvf2Trs ? "virtual_connect level " : "virtual_connect ( level ");
            break;
        default:
            return;
    }
    (*m_oPVRSStream) << sCmd;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sNumExp;
    if (iter->eType == SECONDARY_KEYWORD && (iter->eKeyType == PRIMARY || iter->eKeyType == ALL))
    {
        if (iter->eKeyType == ALL)
            outputText(NULL, "all");
        else
            outputText(NULL, "top_cell");
        ++iter;
    }
    else if (getNumberExp(iter, pNode->end(), sNumExp))
    {
        outputText(NULL, sNumExp.c_str());
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    if (iter != pNode->end())
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }


    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convUserDefDbSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << (_isTvf2Trs ? "layout_add_cell " : "layout_add_cell ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    if (++iter == pNode->end())
    {
        --iter;
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2, iter->nLineNo,
                            iter->sValue);
    }

    std::string sFileName;
    if (getStringName(iter, pNode->end(), sFileName))
    {
        outputText(NULL, sFileName.c_str(), nOption++);
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    if (++iter == pNode->end())
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2, pNode->begin()->nLineNo,
                            pNode->begin()->sValue);
    }

    std::string sCellName;
    switch(iter->eType)
    {
        case SECONDARY_KEYWORD:
        {
            if (GDSII == iter->eKeyType)
            {
                outputText(NULL, "gdsII", nOption++);
                break;
            }
            else if (OASIS == iter->eKeyType)
            {

                outputText(NULL, "oasis", nOption++);
                break;
            }
        }
        case IDENTIFIER_NAME:
        case STRING_CONSTANTS:
        {
            if (getStringName(iter, pNode->end(), sCellName))
            {
                outputText(NULL, sCellName.c_str(), nOption++);
            }
            else
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
            break;
        }
        default:
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
            break;
    }

    if (++iter == pNode->end())
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2, pNode->begin()->nLineNo,
                            pNode->begin()->sValue);
    }

    if (sCellName.empty())
    {
        if (getStringName(iter, pNode->end(), sCellName))
        {
            outputText(NULL, sCellName.c_str(), nOption++);
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
        if (++iter == pNode->end())
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2, pNode->begin()->nLineNo,
                                pNode->begin()->sValue);
        }
    }

    if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == EMPTY)
    {
        outputCon2Error(pNode,*iter);
        ++iter;
        if (iter == pNode->end())
        {
            ;
        }
        else
        {
            --iter;
        }
    }

    while(iter != pNode->end())
    {
        if (!parsePlaceCellInfo(iter, pNode->end()))
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2, pNode->begin()->nLineNo,
                                pNode->begin()->sValue);
        }
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convOutputLvsDBSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << (_isTvf2Trs ? "lvs_db " : "lvs_db ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sValue;
    if (getStringName(iter, pNode->end(), sValue))
    {
        outputText(NULL, sValue.c_str(), nOption++);
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }


    ++iter;
    while (iter != pNode->end())
    {

        ++iter;
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convVarSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << (_isTvf2Trs ? "var " : "var ( ");
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sVarName;
    if (getStringName(iter, pNode->end(), sVarName))
    {
        outputText(NULL, sVarName.c_str(), nOption++);
        m_vVariableNames.insert(sVarName);
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    ++iter;
    if (iter->eKeyType == ENVIRONMENT)
    {
        outputText(NULL, "env", nOption++);
    }
    else
    {
        while (iter != pNode->end())
        {
            switch (iter->eType)
            {
                case NUMBER:
                case SEPARATOR:
                {
                    std::string sNumExp;
                    if (getNumberExp(iter, pNode->end(), sNumExp))
                    {
                        outputText(NULL, sNumExp.c_str(), nOption++);
                    }
                    else
                    {
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                            iter->nLineNo, iter->sValue);
                    }
                    break;
                }
                case IDENTIFIER_NAME:
                case SECONDARY_KEYWORD:
                case STRING_CONSTANTS:
                {
                    std::string sCellName;
                    if (getStringName(iter, pNode->end(), sCellName))
                    {
                         outputText(NULL, sCellName.c_str(), nOption++);
                    }
                    else
                    {
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                            iter->nLineNo, iter->sValue);
                    }
                    ++iter;
                    break;
                }
                case OPERATOR:
                {
                    if (iter->sValue == "+" || iter->sValue == "*"
                            || iter->sValue == "/" || iter->sValue == "-")
                    {
                        outputText(NULL, iter->sValue.c_str(), nOption++);
                        ++iter;
                        break;
                    }
                    else
                    {
                        std::string sNumExp;
                        if (getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            outputText(NULL, sNumExp.c_str(), nOption++);
                        }
                        else
                        {
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                iter->nLineNo, iter->sValue);
                        }
                        break;
                    }
                }
                default:
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                    break;
            }
        }
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convPolygonToLayerSpec(rcsSynSpecificationNode_T *pNode)
{
    m_sCurCommandString = "POLYGON";
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "add_polygon " : "add_polygon ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    std::list<rcsToken_T>::iterator end  = pNode->end();
    ++iter;
    --end;
    while(iter != end)
    {
        std::string sNumExp;
        if (getNumberExp(iter, end, sNumExp, false, false))
        {
            outputText(NULL, sNumExp.c_str());
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
    }

    if (iter != pNode->end())
    {
        if ( (iter->eType != IDENTIFIER_NAME) || !parseTmpLayerForOperation(iter->sValue) )
        {
            std::string sLayerName;
            if (getLayerRefName(iter, pNode->end(), sLayerName))
            {
                outputText(NULL, sLayerName.c_str());
            }
            else
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
        }
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convTextToLayerSpec(rcsSynSpecificationNode_T *pNode)
{
    m_sCurCommandString = "TEXT";
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "add_text " : "add_text ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sTextValue;
    if (getStringName(iter, pNode->end(), sTextValue))
    {
        outputText(NULL, sTextValue.c_str());
        ++iter;
    }

    std::string sNumExp;
    if (getNumberExp(iter, pNode->end(), sNumExp, false, false))
    {
        outputText(NULL, sNumExp.c_str());
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    if (iter != pNode->end())
    {
        sNumExp.clear();
        if (getNumberExp(iter, pNode->end(), sNumExp, false, false))
        {
            outputText(NULL, sNumExp.c_str());
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
    }

    if (iter != pNode->end())
    {
        if ( (iter->eType != IDENTIFIER_NAME) || !parseTmpLayerForOperation(iter->sValue) )
        {
            std::string sLayerName;
            if (getLayerRefName(iter, pNode->end(), sLayerName))
            {
                outputText(NULL, sLayerName.c_str());
            }
            else
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
        }
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convFilterSpec(rcsSynSpecificationNode_T *pNode)
{
    if (_hasOnlyDirectKeyword(pNode))
    {
        outputCon1Error(pNode);
        return;
    }

    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << (_isTvf2Trs ? "device_filter " : "device_filter ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sComponentType;
    if (!getStringName(iter, pNode->end(), sComponentType))
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    ++iter;
    if (iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
    {
        sComponentType += "( ";
        ++iter;
        std::string sSubTypes;

        if (!getStringName(iter, pNode->end(), sSubTypes))
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
        sComponentType += sSubTypes;
        ++iter;
        sComponentType += " )";
        ++iter;
    }
    outputText(NULL, sComponentType.c_str(), nOption++);

    std::string sPropertyName;
    if (iter != pNode->end() && (iter->eType != SECONDARY_KEYWORD ||
      (iter->eKeyType != SHORT && iter->eKeyType != OPEN &&
       iter->eKeyType != LAYOUT && iter->eKeyType != SOURCE &&
       iter->eKeyType != MASK && iter->eKeyType != DIRECT)))
    {
        if (!getStringName(iter, pNode->end(), sPropertyName))
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }

        ++iter;
        if (iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
        {
            sPropertyName += iter->sValue;
            ++iter;
            std::string sSpicePara;

            if (!getStringName(iter, pNode->end(), sSpicePara))
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
            sPropertyName += sSpicePara;
            ++iter;
            sPropertyName += iter->sValue;
            ++iter;
        }
        outputText(NULL, sPropertyName.c_str(), nOption++);

        if (iter != pNode->end() && iter->eType == OPERATOR)
        {
            std::string sFilterLimit;
            if (getConstraint(iter, pNode->end(), sFilterLimit))
            {
                outputText(NULL, sFilterLimit.c_str(), nOption++);
            }
            else
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
        }
    }

    while(iter != pNode->end())
    {
        if (iter->eType == SECONDARY_KEYWORD &&
          (iter->eKeyType == DIRECT || iter->eKeyType == MASK))
        {
            outputCon2Error(pNode, *iter);
            ++iter;
            continue;
        }

        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                if (iter->eKeyType == SHORT)
                {
                    ++iter;
                    std::string sPinName;
                    while(iter != pNode->end() && (iter->eType != SECONDARY_KEYWORD ||
                         (iter->eKeyType != OPEN && iter->eKeyType != LAYOUT &&
                          iter->eKeyType != SOURCE && iter->eKeyType != SHORT &&
                          iter->eKeyType != DIRECT && iter->eKeyType != MASK)))
                    {
                        std::string sValue;
                        if (!getStringName(iter, pNode->end(), sValue))
                        {
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                iter->nLineNo, iter->sValue);
                        }
                        sPinName += sValue;
                        sPinName += " ";
                        ++iter;
                    }
                    outputText(NULL, "short", nOption++);
                    if (!sPinName.empty())
                        outputText(NULL, sPinName.c_str(), nOption++);
                    continue;
                }
                else if (iter->eKeyType == OPEN)
                {
                    outputText(NULL, "open", nOption++);
                    ++iter;
                    continue;
                }
                else if (iter->eKeyType == LAYOUT)
                {
                    outputText(NULL, "layout", nOption++);
                    ++iter;
                    continue;
                }
                else if (iter->eKeyType == SOURCE)
                {
                    outputText(NULL, "schematic", nOption++);
                    ++iter;
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convFilterUnusedOptionSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << (_isTvf2Trs ? "device_filter_option " : "device_filter_option ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    for(; iter != pNode->end(); ++iter)
    {
        if (strcasecmp(iter->sValue.c_str(), "ab") == 0)
        {
            outputText("connected_gsd", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "ac") == 0)
        {
            outputText("floating_g_power_sd", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "ad") == 0)
        {
            outputText("floating_g_ground_sd", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "ae") == 0)
        {
            outputText("floating_gsd", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "af") == 0)
        {
            outputText("connected_sd", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "ag") == 0)
        {
            outputText("connected_all", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "AH") == 0)
        {
            outputText("connected_sdb_floating_g", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "AI") == 0)
        {
            outputText("connected_sgdb", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "b") == 0)
        {
            outputText("nopath_g_floating_s_or_d", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "c") == 0)
        {
            outputText("power_ground_g_floating_s_or_d", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "d") == 0)
        {
            outputText("floating_g_ground_path_sd", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "e") == 0)
        {
            outputText("floating_g_power_path_s_or_d", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "f") == 0)
        {
            outputText("ground_ngate", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "FF") == 0)
        {
            outputText("ground_g_floating_s_or_d", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "fy") == 0)
        {
            outputText("ground_ngate2", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "g") == 0)
        {
            outputText("power_pgate", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "GF") == 0)
        {
            outputText("power_g_floating_s_or_d", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "gy") == 0)
        {
            outputText("power_pgate2", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "h") == 0)
        {
            outputText("only_power_sd", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "i") == 0)
        {
            outputText("only_ground_sd", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "ih") == 0)
        {
            outputText("power_ground_sd", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "inv") == 0)
        {
            outputText("inv", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "j") == 0)
        {
            outputText("power_ground_g_connected_sd", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "k") == 0)
        {
            outputText("nopath_sd", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "l") == 0)
        {
            outputText("only_floating_s_or_d", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "m") == 0)
        {
            outputText("ground_pgate2", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "MG") == 0)
        {
            outputText("ground_g_power_sd", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "n") == 0)
        {
            outputText("power_ngate2", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "NP") == 0)
        {
            outputText("power_g_ground_sd", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "o") == 0)
        {
            outputText("repeat_filter_all", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "p") == 0)
        {
            outputText("nopath_device", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "q") == 0)
        {
            outputText("power_ground_device", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "rb") == 0)
        {
            outputText("floating_r", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "rc") == 0)
        {
            outputText("connected_r", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "rd") == 0)
        {
            outputText("floating_c", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "re") == 0)
        {
            outputText("connected_c", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "rf") == 0)
        {
            outputText("floating_d", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "rg") == 0)
        {
            outputText("connected_d", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "s") == 0)
        {
            outputText("floating_q", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "t") == 0)
        {
            outputText("connected_ec", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "u") == 0)
        {
            outputText("floating_sd", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "v") == 0)
        {
            outputText("floating_mos", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "yb") == 0)
        {
            outputText("connected_be", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "yc") == 0)
        {
            outputText("connected_q", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "zb") == 0)
        {
            outputText("floating_r_all", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "zc") == 0)
        {
            outputText("floating_c_all", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "zd") == 0)
        {
            outputText("floating_d_all", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "ze") == 0)
        {
            outputText("floating_q_all", NULL, nOption++);
        }
        else if (strcasecmp(iter->sValue.c_str(), "ah") == 0)
        {
            outputCon2Error(pNode,*iter);
        }
        else if (strcasecmp(iter->sValue.c_str(), "ff") == 0)
        {
            outputCon2Error(pNode,*iter);
        }
        else if (strcasecmp(iter->sValue.c_str(), "gf") == 0)
        {
            outputCon2Error(pNode,*iter);
        }
        else if (strcasecmp(iter->sValue.c_str(), "mg") == 0)
        {
            outputCon2Error(pNode,*iter);
        }
        else if (strcasecmp(iter->sValue.c_str(), "np") == 0)
        {
            outputCon2Error(pNode,*iter);
        }
        else
        {
            if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == LAYOUT)
            {
                outputText(NULL, "layout", nOption++);
            }
            else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SOURCE)
            {
                outputText(NULL, "schematic", nOption++);
            }
            else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == DEVICES)
            {
                outputCon2Error(pNode,*iter);
                ++iter;
                if (iter == pNode->end())
                {
                    --iter;
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, DEV37,
                            iter->nLineNo, iter->sValue);
                }

                s_errManager.addError(rcErrorNode_T(rcErrorNode_T::WARNING, DEV35,
                        iter->nLineNo, iter->sValue));

                std::string sComponent = iter->sValue;
                ++iter;
                if (iter == pNode->end())
                {
                    --iter;
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, DEV37,
                            iter->nLineNo, iter->sValue);
                }
                if (iter->sValue != "(")
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, DEV37,
                            iter->nLineNo, iter->sValue);
                }
                ++iter;
                if (iter == pNode->end())
                {
                    --iter;
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, DEV37,
                            iter->nLineNo, iter->sValue);
                }

                s_errManager.addError(rcErrorNode_T(rcErrorNode_T::WARNING, DEV36,
                        iter->nLineNo, iter->sValue));

                std::string sSubComponent = iter->sValue;
                ++iter;
                if (iter == pNode->end())
                {
                    --iter;
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, DEV37,
                            iter->nLineNo, iter->sValue);
                }
                if (iter->sValue != ")")
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, DEV37,
                            iter->nLineNo, iter->sValue);
                }
                ++iter;
                while (iter != pNode->end()) {
                    sComponent = iter->sValue;
                    ++iter;
                    if (iter == pNode->end() || iter->sValue != "(")
                    {
                        --iter;
                        --iter;
                        break;
                    }
                    --iter;
                    s_errManager.addError(rcErrorNode_T(rcErrorNode_T::WARNING, DEV35,
                            iter->nLineNo, iter->sValue));
                    ++iter;
                    ++iter;
                    if (iter == pNode->end())
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, DEV37,
                                iter->nLineNo, iter->sValue);
                    }

                    s_errManager.addError(rcErrorNode_T(rcErrorNode_T::WARNING, DEV36,
                            iter->nLineNo, iter->sValue));

                    sSubComponent = iter->sValue;
                    ++iter;
                    if (iter == pNode->end())
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, DEV37,
                                iter->nLineNo, iter->sValue);
                    }
                    if (iter->sValue != ")")
                    {
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, DEV37,
                                iter->nLineNo, iter->sValue);
                    }
                    ++iter;
                }
                if (iter == pNode->end())
                {
                    break;
                }
            }
            else
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                        iter->nLineNo, iter->sValue);
            }
        }
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convMapDeviceSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << (_isTvf2Trs ? "device_map " : "device_map ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sOldComponentType;
    if (getStringName(iter, pNode->end(), sOldComponentType))
        outputText(NULL, sOldComponentType.c_str(), nOption++);

    ++iter;
    if (iter!= pNode->end() && SEPARATOR == iter->eType && "(" == iter->sValue)
    {
        ++iter;
        std::string sOldSubType;
        if (getStringName(iter, pNode->end(), sOldSubType))
        {
            sOldSubType.insert(0, "( ");
            sOldSubType += " )";
            outputText(NULL, sOldSubType.c_str());
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
        ++iter;
        Utassert(iter->eType == SEPARATOR && iter->sValue == ")");
        ++iter;
    }

    std::string sNewComponentType;
    if (getStringName(iter, pNode->end(), sNewComponentType))
    {
        outputText(NULL, sNewComponentType.c_str(), nOption++);
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    ++iter;
    if (iter!= pNode->end() && SEPARATOR == iter->eType && "(" == iter->sValue)
    {
        ++iter;
        std::string sNewSubType;
        if (getStringName(iter, pNode->end(), sNewSubType))
        {
            sNewSubType.insert(0, "( ");
            sNewSubType += " )";
            outputText(NULL, sNewSubType.c_str());
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
        ++iter;
        Utassert(iter->eType == SEPARATOR && iter->sValue == ")");
        ++iter;
    }

    while(iter != pNode->end())
    {
        if (iter->eType == SECONDARY_KEYWORD && (iter->eKeyType == LAYOUT || iter->eKeyType == SOURCE))
        {
            if (iter->eKeyType == LAYOUT)
            {
                outputText(NULL, "layout", nOption++);
            }
            else if (iter->eKeyType == SOURCE)
            {
                outputText(NULL, "schematic", nOption++);
            }
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
        ++iter;
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convReducePreferSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "device_merge_priority " : "device_merge_priority ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if (strcasecmp(iter->sValue.c_str(), "PARALLEL") == 0)
    {
        outputText(NULL, "parallel", nOption++);
    }
    else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SERIES)
    {
        outputText(NULL, "series", nOption++);
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convInitialPropertySpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "device_property init " : "device_property ( init ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sComponentType;
    if (!getStringName(iter, pNode->end(), sComponentType))
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    ++iter;
    if (iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
    {
        sComponentType += "( ";
        std::string sSubType;
        ++iter;
        if (!getStringName(iter, pNode->end(), sSubType))
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
        sComponentType += sSubType;

        ++iter;
        if (iter == pNode->end() || iter->eType != SEPARATOR || iter->sValue != ")")
        {
            if (iter == pNode->end())
                --iter;
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
        sComponentType += " )";
        ++iter;
    }

    outputText(NULL, sComponentType.c_str(), nOption++);

    if (iter != pNode->end() && iter->eType == BUILT_IN_LANG)
    {
        outputBuiltInLang(pNode->getSpecificationType(), *iter, nOption);
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convMapPropertySpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "device_property map " : "device_property ( map ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sComponentType;
    if (getStringName(iter, pNode->end(), sComponentType))
    {
        outputText(NULL, sComponentType.c_str(), nOption++);
    }
    ++iter;
    std::string sTargetName;
    if (getStringName(iter, pNode->end(), sTargetName))
    {
        outputText(NULL, sTargetName.c_str(), nOption++);
    }

    ++iter;
    std::string sDatabaseName;
    if (getStringName(iter, pNode->end(), sDatabaseName))
    {
        ++iter;
        if (iter != pNode->end() && iter->eType == SEPARATOR && iter->sValue == "(")
        {
            sDatabaseName += "( ";
            ++iter;
            std::string sSpicePara;

            if (!getStringName(iter, pNode->end(), sSpicePara))
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            sDatabaseName += sSpicePara;
            ++iter;
            sDatabaseName += " )";
            ++iter;
        }
    }
    outputText(NULL, sDatabaseName.c_str(), nOption++);

    if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == LAYOUT)
    {
        outputText(NULL, "layout", nOption++);
    }
    else if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SOURCE)
    {
        outputText(NULL, "schematic", nOption++);
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convPushDeviceSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    std::list<rcsToken_T>::iterator iter = pNode->begin();

    if (pNode->getSpecificationType() == LVS_PUSH_DEVICES_SEPARATE_PROPERTIES)
    {
        (*m_oPVRSStream) << (_isTvf2Trs ? "device_push_separate_property " : "device_push_separate_property ( ");
    }
    else
    {
        (*m_oPVRSStream) << (_isTvf2Trs ? "device_push " : "device_push ( ");
    }


    while (++iter != pNode->end())
    {
        switch(iter->eKeyType)
        {
            case LDE:
                outputText("ldc", NULL, nOption++);
                break;
            case AGF:
                outputText("agf", NULL, nOption++);
                break;
            default:
                outputText(iter->sValue.c_str(), NULL, nOption++);
                break;
        }
    }

    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}


void
rcsSynPvrsConvertor::convMapDeviceTypeSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "device_type_map " : "device_type_map ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    bool hasBuiltInType = false;
    while(iter != pNode->end())
    {
        if (iter->eType == BUILT_IN_LANG)
        {
            hvUInt32 nOption = 0;
#if 0
            if(!outputBuiltInLangInTrs(*iter, nOption))
            {
                std::string sValue = iter->sValue;
                sValue.erase(0, 1);
                sValue.insert(0, "[ ");
                sValue.erase(sValue.size() - 1);
                sValue.insert(sValue.size(), " ]");
                outputText(NULL, sValue.c_str());
            }
#else
            outputBuiltInLang(pNode->getSpecificationType(), *iter, nOption);
#endif
        }
        else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == LAYOUT)
        {
            outputText(NULL, "layout");
        }
        else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SOURCE)
        {
            outputText(NULL, "schematic");
        }
        else if (hasBuiltInType)
        {
            std::string sNewType;
            if (!getStringName(iter, pNode->end(), sNewType))
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
            outputText(NULL, sNewType.c_str());
        }
        else
        {
            std::string sBuiltInType;
            if (!getStringName(iter, pNode->end(), sBuiltInType))
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
            outputText(NULL, sBuiltInType.c_str());
            hasBuiltInType = true;
        }
        ++iter;
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convLvsSplitGateRatio(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (iter->eType == SECONDARY_KEYWORD)
        {
            if(iter->eKeyType == DIRECT)
            {
                std::string sValue;
                pNode->getCmdValue(sValue);

                m_oLogStream << "This Operation or Specification in line " << pNode->begin()->nLineNo
                             << " can not be Converted correctly: " << std::endl << sValue << std::endl << std::endl;

                s_errManager.addError(rcErrorNode_T(rcErrorNode_T::WARNING, CON1,
                                      pNode->getNodeBeginLineNo(), pNode->begin()->sValue));
                return;
            }
        }
        ++iter;
    }

    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << (_isTvf2Trs ? "device_merge split_gate_ratio " : "device_merge ( split_gate_ratio ");

    iter = pNode->begin();
    ++iter;
    std::string sComponentType;
    if (getStringName(iter, pNode->end(), sComponentType))
        outputText(NULL, sComponentType.c_str(), nOption++);

    ++iter;
    std::string sPropName;
    if (getStringName(iter, pNode->end(), sPropName))
    {
        outputText(NULL, sPropName.c_str(), nOption++);
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    ++iter;
    if (iter!= pNode->end() && SEPARATOR == iter->eType && "(" == iter->sValue)
    {
        ++iter;
        std::string sSpiceParameter;
        if (getStringName(iter, pNode->end(), sSpiceParameter))
        {
            sSpiceParameter.insert(0, "( ");
            sSpiceParameter += " )";
            outputText(NULL, sSpiceParameter.c_str());
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
        ++iter;
        Utassert(iter->eType == SEPARATOR && iter->sValue == ")");
        ++iter;
    }

    std::string sNumExp;
    if (getNumberExp(iter ,pNode->end(), sNumExp))
    {
        outputText(NULL, sNumExp.c_str());
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    while(iter != pNode->end())
    {
        if (iter->eType == SECONDARY_KEYWORD)
        {
            if (iter->eKeyType == LAYOUT)
            {
                outputText(NULL, "layout", nOption++);
            }
            else if (iter->eKeyType == SOURCE)
            {
                outputText(NULL, "schematic", nOption++);
            }
            else if(iter->eKeyType == MASK)
            {

            }
            else if(iter->eKeyType == DIRECT)
            {

            }
            else
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
        ++iter;
    }

    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void rcsSynPvrsConvertor::convLvsNetlistProperty(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "netlist property " : "netlist ( property ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();

    while (++iter != pNode->end())
    {
        outputText(NULL, iter->sValue.c_str(), nOption++);
    }

    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void rcsSynPvrsConvertor::convLvsAutoExpandHcell(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "lvs_auto_expand_hcell " : "lvs_auto_expand_hcell ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();

    while (++iter != pNode->end())
    {
        outputText(NULL, iter->sValue.c_str(), nOption++);
    }

    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convCorrespondingCellSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "hcell " : "hcell ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    std::string sLayoutName;
    if (getStringName(iter, pNode->end(), sLayoutName))
    {
        outputText(NULL, sLayoutName.c_str(), nOption++);
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    ++iter;
    std::string sSourceName;
    if (getStringName(iter, pNode->end(), sSourceName))
    {
        outputText(NULL, sSourceName.c_str(), nOption++);
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convOutputMagSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    if (pNode->getSpecificationType() == DRC_MAGNIFY_RESULTS)
        (*m_oPVRSStream) << (_isTvf2Trs ? "drc_magnify_result db " : "drc_magnify_result ( db ");
    else if (pNode->getSpecificationType() == DRC_MAGNIFY_DENSITY)
        (*m_oPVRSStream) << (_isTvf2Trs ? "drc_magnify_result density " : "drc_magnify_result ( density ");
    else if (pNode->getSpecificationType() == DRC_MAGNIFY_NAR)
        (*m_oPVRSStream) << (_isTvf2Trs ? "drc_magnify_result nar " : "drc_magnify_result ( nar ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sNumExp;
    if (getNumberExp(iter ,pNode->end(), sNumExp))
    {
        outputText(NULL, sNumExp.c_str());
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    if (iter != pNode->end())
    {
        if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == PLACE)
        {
            outputText(NULL, "new_cell");
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convDrcOutputLibSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << (_isTvf2Trs ? "drc_result " : "drc_result ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sFileName;
    if (getStringName(iter, pNode->end(), sFileName))
    {
        outputText("libname", sFileName.c_str(), nOption++);
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convDrcOutPrecisionSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "drc_result precision " : "drc_result ( precision ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sNumExp;
    if (getNumberExp(iter, pNode->end(), sNumExp))
    {
        outputText(NULL, sNumExp.c_str(), nOption);
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }
    sNumExp.clear();
    if (iter != pNode->end())
    {
        if (getNumberExp(iter, pNode->end(), sNumExp))
        {
            outputText(NULL, sNumExp.c_str(), nOption);
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convFindShortsSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "erc_find_short " : "erc_find_short ( ");
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD)
    {
        if (iter->eKeyType == YES)
        {
            outputText(NULL, "yes", nOption++);
            ++iter;
        }
        else if (iter->eKeyType == NO)
        {
            outputText(NULL, "no", nOption++);
            ++iter;
            if (iter != pNode->end())
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
    }

    std::string sNameValue;
    while(iter != pNode->end())
    {
        if (iter->eType == SECONDARY_KEYWORD)
        {
            switch(iter->eKeyType)
            {
                case BY_CELL:
                {
                    ++iter;
                    if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       iter->eKeyType == ALSO)
                    {
                        outputText(NULL, "by_cell_also", nOption++);
                        ++iter;
                    }
                    else
                    {
                        outputText(NULL, "by_cell", nOption++);
                    }
                    continue;
                }
                case BY_LAYER:
                {
                    ++iter;
                    if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       iter->eKeyType == ALSO)
                    {
                        outputText(NULL, "by_layer_also", nOption++);
                        ++iter;
                    }
                    else
                    {
                        outputText(NULL, "by_layer", nOption++);
                    }
                    continue;
                }
                case ACCUMULATE:
                {
                    outputText(NULL, "hot_spot", nOption++);
                    ++iter;
                    continue;
                }
                case NO:
                {
                    ++iter;
                    if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       iter->eKeyType == CONTACTS)
                    {
                        outputText(NULL, "ignore_contact", nOption++);
                        ++iter;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                            iter->nLineNo, iter->sValue);
                    }
                    continue;
                }
                case CELL:
                {
                    ++iter;
                    if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       (iter->eKeyType == ALL || iter->eKeyType == PRIMARY))
                    {
                        if (iter->eKeyType == ALL)
                            outputText(NULL, "all_cell", nOption++);
                        else
                            outputText(NULL, "top_cell", nOption++);
                        ++iter;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                            iter->nLineNo, iter->sValue);
                    }
                    continue;
                }
                case FLAT:
                {
                    outputText(NULL, "flatten", nOption++);
                    ++iter;
                    continue;
                }
                case UNMERGED:
                {
                    outputText(NULL, "keep_unmerged", nOption++);
                    ++iter;
                    continue;
                }
                case MAX_SECOND:
                {
                    ++iter;
                    std::string sNumExp;
                    if (iter != pNode->end() && getNumberExp(iter, pNode->end(), sNumExp))
                    {
                        outputText("max", sNumExp.c_str(), nOption);
                    }
                    continue;
                }
                default:
                    break;
            }
        }
        else if (OPERATOR == iter->eType && (iter->sValue == "||" || iter->sValue == "&&"))
        {
            sNameValue = iter->sValue;
            ++iter;
            if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && (iter->eKeyType == NAME || iter->eKeyType == IGNORE_NAME))
            {
                std::string sName;
                if (iter->eKeyType == NAME)
                {
                    sName = "among_net ";
                }
                else
                {
                    sName = "ignore_net ";
                }
                ++iter;
                while(iter != pNode->end())
                {
                    if (iter->eType != SECONDARY_KEYWORD || (iter->eKeyType != BY_CELL &&
                       iter->eKeyType != BY_LAYER && iter->eKeyType != FLAT &&
                       iter->eKeyType != UNMERGED && iter->eKeyType != NO &&
                       iter->eKeyType != ACCUMULATE && iter->eKeyType != CELL &&
                       iter->eKeyType != MAX_SECOND))
                    {
                        std::string sValue;
                        if (getStringName(iter, pNode->end(), sValue))
                        {
                            sName += sValue;
                            sName += " ";
                        }
                        else
                        {
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                iter->nLineNo, iter->sValue);
                        }
                    }
                    else
                        break;
                    ++iter;
                }
                outputText(sNameValue.c_str(), sName.c_str(), nOption++);
            }
            else
            {
                --iter;
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
            continue;
        }

        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convErcPathchkSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "erc_path_check " : "erc_path_check ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    while(iter != pNode->end())
    {
        if (iter->eType == SECONDARY_KEYWORD)
        {
            switch(iter->eKeyType)
            {
                case LABELED:
                {
                    outputText(NULL, "labeled", nOption++);
                    ++iter;
                    continue;
                }
                case POWER:
                {
                    outputText(NULL, "powered", nOption++);
                    ++iter;
                    continue;
                }
                case GROUND:
                {
                    outputText(NULL, "grounded", nOption++);
                    ++iter;
                    continue;
                }
                case POLYGONS:
                case NETS:
                {
                    if (iter->eKeyType == NETS)
                    {
                        outputText("output_net", NULL, nOption++);
                    }
                    else
                    {
                        outputText("output_polygon", NULL, nOption++);
                    }
                    ++iter;
                    continue;
                }
                case BY_LAYER:
                {
                    outputText(NULL, "by_layer", nOption++);
                    ++iter;
                    continue;
                }
                case BY_CELL:
                {
                    outputText(NULL, "by_cell", nOption++);
                    ++iter;
                    continue;
                }
                case FLAT:
                {
                    outputText(NULL, "flatten", nOption++);
                    ++iter;
                    continue;
                }
                case PORTS_ALSO:
                {
                    outputText("with_port", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case NOFLOAT:
                {
                    outputText("ignore_floatting", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case EXCLUDE_UNUSED:
                {
                    outputText("only_used", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case EXCLUDE_SUPPLY:
                {
                    outputText("exclude_power_ground", NULL, nOption++);
                    ++iter;
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case OPERATOR:
            {
                if (iter->sValue == "+" || iter->sValue == "-")
                {
                    std::string sLayerName;
                    if (getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText(NULL, sLayerName.c_str(), nOption++);
                    }
                    else
                    {
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                            iter->nLineNo, iter->sValue);
                    }
                    continue;
                }
                else if ("!" == iter->sValue)
                {
                    ++iter;
                    if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD)
                    {
                        if (iter->eKeyType == LABELED)
                        {
                            outputText(NULL, "!labeled", nOption++);
                        }
                        else if (iter->eKeyType == POWER)
                        {
                            outputText(NULL, "!powered", nOption++);
                        }
                        else if (iter->eKeyType == GROUND)
                        {
                            outputText(NULL, "!grounded", nOption++);
                        }
                        else
                        {
                            --iter;
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo,
                                                iter->sValue);
                        }
                    }
                    break;
                }
                else if ( "&&" == iter->sValue)
                {
                    outputText(NULL, "&&", nOption++);
                    break;
                }
                else if ( "||" == iter->sValue)
                {
                    outputText(NULL, "||", nOption++);
                    break;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo,
                                    iter->sValue);
        }
        ++iter;
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void rcsSynPvrsConvertor::convErcTclSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    std::string sCmd = "erc_trs ( ";
    std::string sCheckName = "";
    std::string sLibName = "";
    std::vector<std::string> vProcNames;

    std::list<rcsToken_T>::iterator iterKey = pNode->begin();
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    if (iter->eType == SPECIFICATION_KEYWORD && iter->eKeyType == ERC_TVF)
    {
        ++iter;
        if(iter != pNode->end())
        {
            if(iter->eType == IDENTIFIER_NAME) //CheckName
            {
                if (!isValidSvrfName(iter->sValue)) //missing or invalid check name error code
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, ERCTVF4,
                                        iterKey->nLineNo, iterKey->sValue);
                }
                sCheckName = iter->sValue.c_str();
                ++iter;
                if(iter != pNode->end())
                {
                    if(iter->eType == IDENTIFIER_NAME) //LibName
                    {
                        if (!isValidSvrfName(iter->sValue)) //missing or invalid lib name error code
                        {
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, ERCTVF1,
                                                iterKey->nLineNo, iterKey->sValue);
                        }
                        sLibName = iter->sValue.c_str();
                        ++iter;
                        if(iter == pNode->end()) //missing or invalid procedure names' list error code
                        {
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, ERCTVF2,
                                                iterKey->nLineNo, iterKey->sValue);
                        }
                        while(iter != pNode->end())
                        {
                            if (!isValidSvrfName(iter->sValue)) //missing or invalid procedure names' list error code, here use the INP1 error code
                            {
                                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                    iter->nLineNo, iter->sValue);
                            }
                            vProcNames.push_back(iter->sValue.c_str());
                            ++iter;
                        }
                        if(!vProcNames.empty())
                        {
                            (*m_oPVRSStream) << sCmd;
                            outputText(NULL, sCheckName.c_str(), nOption++);
                            outputText(NULL, sLibName.c_str(), nOption++);
                            for(auto iter : vProcNames)
                            {
                                outputText(NULL, iter.c_str(), nOption++);
                            }
                        }
                    }
                    else //missing or invalid lib name error code
                    {
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, ERCTVF1,
                                            iterKey->nLineNo, iterKey->sValue);
                    }
                }
                else //missing or invalid lib name error code
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, ERCTVF1,
                            iterKey->nLineNo, iterKey->sValue);
                }
            }
            else //missing or invalid check name error code
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, ERCTVF4,
                                    iterKey->nLineNo, iterKey->sValue);
            }
        }
        else //missing or invalid check name error code
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, ERCTVF4,
                    iterKey->nLineNo, iterKey->sValue);
        }
    }
    if(true/*!_isTvf2Trs*/)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convDeviceOnPathSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "erc_path_device " : "erc_path_device ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    while(iter != pNode->end())
    {
        if (iter->eType == SECONDARY_KEYWORD)
        {
            switch(iter->eKeyType)
            {
                case BIPOLAR:
                    outputText("bjt", NULL, nOption++);
                    ++iter;
                    continue;
                case CAPACITOR:
                    outputText("cap", NULL, nOption++);
                    ++iter;
                    continue;
                case DIODE:
                    outputText("dio", NULL, nOption++);
                    ++iter;
                    continue;
                default:
                    break;
            }
        }
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convCellListSpec(rcsSynSpecificationNode_T *pNode)
{
    std::string sCmd = "";
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    switch(pNode->getSpecificationType())
    {
        case LAYOUT_PRESERVE_CELL_LIST:
            sCmd += (_isTvf2Trs ? "layout_reserve_cell_group " : "layout_reserve_cell_group ( ");
            break;
        case DRC_ERROR_WAIVE_CELL:
            sCmd += (_isTvf2Trs ? "drc_waive_error by_cell " : "drc_waive_error ( by_cell ");
            break;
        case EXCLUDE_CELL:
            sCmd += (_isTvf2Trs ? "ignore_cell " : "ignore_cell ( ");
            break;
        case PUSH_CELL:
            sCmd += (_isTvf2Trs ? "push_cell_lowest " : "push_cell_lowest ( ");
            break;
        case EXPAND_CELL:
            sCmd += (_isTvf2Trs ? "promote_cell " : "promote_cell ( ");
            break;
        case FLATTEN_CELL:
            sCmd += (_isTvf2Trs ? "flatten cell " : "flatten ( cell ");
            break;
        case FLATTEN_INSIDE_CELL:
            sCmd += (_isTvf2Trs ? "flatten in_cell " : "flatten ( in_cell ");
            break;
        case LAYOUT_BASE_CELL:
            sCmd += (_isTvf2Trs ? "layout_device_cell " : "layout_device_cell ( ");
            break;
        case LAYOUT_WINDEL_CELL:
            sCmd += (_isTvf2Trs ? "layout_exclude_window cell " : "layout_exclude_window ( cell ");
            break;
        case LAYOUT_WINDOW_CELL:
            sCmd += (_isTvf2Trs ? "layout_select_window cell " : "layout_select_window ( cell ");
            break;
        default:
            return;
    }
    (*m_oPVRSStream) << sCmd;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    while(iter != pNode->end())
    {
        std::string sValue;
        if (iter->eKeyType == ORIGINAL)
        {
            outputText("original_layer",NULL);
            ++iter;
            continue;
        }
        if (iter->eKeyType == OCCUPIED)
        {
            outputText("used_layer",NULL);
            ++iter;
            continue;
        }
        if (!getStringName(iter, pNode->end(), sValue))
        {
            sValue = iter->sValue;
        }
        if((pNode->getSpecificationType() == FLATTEN_CELL || pNode->getSpecificationType() == LAYOUT_WINDEL_CELL ||
            pNode->getSpecificationType() == LAYOUT_WINDOW_CELL) && strcasecmp(sValue.c_str(), "cell") == 0)
        {
            sValue.insert(0, "\"");
            sValue.push_back('\"');
        }
        outputText(NULL, sValue.c_str());
        ++iter;
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convFileHeadNameSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "head_name " : "head_name ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    while(iter != pNode->end())
    {
        std::string sValue;
        if (getStringName(iter, pNode->end(), sValue))
        {
            outputText(NULL, sValue.c_str(), nOption++);
        }
        else if(!rcsManager_T::getInstance()->isTvf2Trs())
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
        }
        ++iter;
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convPolygonToDBSpec(rcsSynSpecificationNode_T *pNode)
{
    m_sCurCommandString = "LAYOUT POLYGON";
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "layout_add_polygon " : "layout_add_polygon ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sVertexValue;
    if (getNumberExp(iter, pNode->end(), sVertexValue, false, false))
    {
        outputText(NULL, sVertexValue.c_str(), nOption++);
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
    }

    while(iter != pNode->end())
    {
        if ((iter->eType == SECONDARY_KEYWORD || iter->eType == IDENTIFIER_NAME ||
           iter->eType == STRING_CONSTANTS) &&
           m_vVariableNames.find(iter->sValue) == m_vVariableNames.end())
        {

            break;
        }

        std::string sValue;
        if (getNumberExp(iter, pNode->end(), sValue, false, false))
        {
            outputText(NULL, sValue.c_str(), nOption);
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
    }

    if (iter != pNode->end())
    {
        if ( (iter->eType != IDENTIFIER_NAME) || !parseTmpLayerForOperation(iter->sValue) )
        {
            std::string sLayerName;
            if (getLayerRefName(iter, pNode->end(), sLayerName))
            {
                outputText(NULL, sLayerName.c_str(), nOption++);
            }
            else
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
        }
    }

    if (iter != pNode->end())
    {
        if (iter->eType == SECONDARY_KEYWORD || iter->eType == IDENTIFIER_NAME ||
           iter->eType == STRING_CONSTANTS)
        {
            std::string sCellName;
            if (getStringName(iter, pNode->end(), sCellName))
            {
                outputText(NULL, sCellName.c_str(), nOption++);
            }
            else
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
            ++iter;
        }
        else
        {
            std::string sNumExp;
            if (getNumberExp(iter, pNode->end(), sNumExp))
            {
                outputText(NULL, sNumExp.c_str(), nOption);
            }
            else
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
        }
    }

    if (iter != pNode->end())
    {
        if (iter->eType == SECONDARY_KEYWORD || iter->eType == IDENTIFIER_NAME ||
           iter->eType == STRING_CONSTANTS)
        {
            std::string sCellName;
            if (getStringName(iter, pNode->end(), sCellName))
            {
                outputText(NULL, sCellName.c_str(), nOption++);
            }
            else
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
        }
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convTextToDBSpec(rcsSynSpecificationNode_T *pNode)
{
    m_sCurCommandString = "LAYOUT TEXT";
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "layout_add_text " : "layout_add_text ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sTextValue;
    if (getStringName(iter, pNode->end(), sTextValue))
    {
        outputText(NULL, sTextValue.c_str(), nOption++);
        ++iter;
    }

    std::string sNumExp;
    if (getNumberExp(iter, pNode->end(), sNumExp, false, false))
    {
        outputText(NULL, sNumExp.c_str(), nOption++);
    }

    if (iter != pNode->end())
    {
        sNumExp.clear();
        if (getNumberExp(iter, pNode->end(), sNumExp, false, false))
        {
            outputText(NULL, sNumExp.c_str());
        }
    }

    if (iter != pNode->end())
    {
        if ( (iter->eType != IDENTIFIER_NAME) || !parseTmpLayerForOperation(iter->sValue) )
        {
            std::string sLayerName;
            if (getLayerRefName(iter, pNode->end(), sLayerName))
            {
                outputText(NULL, sLayerName.c_str(), nOption++);
            }
            else
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
        }
    }

    if (iter != pNode->end())
    {
        std::string sCellName;
        std::string sTextType;
        if (getStringName(iter, pNode->end(), sCellName))
            outputText(NULL, sCellName.c_str(), nOption++);
        else
        {
            if ( (iter->eType != IDENTIFIER_NAME) || !parseTmpLayerForOperation(iter->sValue) )
            {
                if (getLayerRefName(iter, pNode->end(), sTextType))
                    outputText(NULL, sTextType.c_str(), nOption++);
            }
        }

        if (sCellName.empty() && iter != pNode->end())
            if (getStringName(iter, pNode->end(), sCellName))
                        outputText(NULL, sCellName.c_str(), nOption++);
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

static void _removeLayoutText(std::list<std::string> &vec)
{
    
    if (vec.size() >= 2)
    {
        std::list<std::string>::iterator it = vec.begin();
        if (0 == strcasecmp(it->c_str(), "layout"))
        {
            ++it;
            if (0 == strcasecmp(it->c_str(), "text"))
            {
                ++it;
                vec.erase(vec.begin(), it);
            }
        }
    }
}

void rcsSynPvrsConvertor::convLayoutTextFileSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if (iter == pNode->end())
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2, pNode->begin()->nLineNo);

    std::string filePath;
    parseFileName(iter->sValue, filePath);

    std::ifstream in(filePath.c_str());
    if (!in)
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, OPEN1, iter->nLineNo, filePath);
    }

    std::string line;
    hvUInt32 nLine = 0;
    while (std::getline(in, line))
    {
        nLine++;
        trim(line);
        if (line.empty())
            continue;

        if (line.size() >= 2 && line[0] == '/' && line[1] == '/') 
            continue;

        std::list<std::string> vec;
        tokenize(vec, line, " \t");
        _removeLayoutText(vec);

        (*m_oPVRSStream) << (_isTvf2Trs ? "layout_add_text " : "layout_add_text ( ");

        for (std::list<std::string>::const_iterator it=vec.begin(); it!=vec.end(); ++it)
            (*m_oPVRSStream) << *it << " ";

        if (!_isTvf2Trs)
        {
            (*m_oPVRSStream) << " ) ";
        }
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convCellListDefSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;
    if (pNode->getSpecificationType() == LAYOUT_CELL_LIST)
        (*m_oPVRSStream) << (_isTvf2Trs ? "layout_cell_group " : "layout_cell_group ( ");
    else if (pNode->getSpecificationType() == LVS_CELL_LIST)
        (*m_oPVRSStream) << (_isTvf2Trs ? "lvs_cell_group " : "lvs_cell_group ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    std::string sListName;
    if (getStringName(iter, pNode->end(), sListName))
        outputText(NULL, sListName.c_str(), nOption++);

    ++iter;
    while(iter != pNode->end())
    {
        std::string sPattern;
        if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == TEXTED)
        {
            outputText("with_text",NULL,nOption++);

        }
        else if(getStringName(iter, pNode->end(), sPattern))
            outputText(NULL, sPattern.c_str(), nOption++);
        else
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        ++iter;
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convExceptionDBSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "layout_exception db " : "layout_exception ( db ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case TOP_SPACE:
                {
                    outputText("in_top", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case CELL_SPACE:
                {
                    outputText("in_cell", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case BY_CELL:
                {
                    outputText("sort_by_cell", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case BY_LAYER:
                {
                    outputText("sort_by_layer", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case BY_EXCEPTION:
                {
                    outputText("sort_by_exception", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case EXIT:
                {
                    if (++iter != pNode->end() && SECONDARY_KEYWORD ==
                       iter->eType && NONEMPTY == iter->eKeyType)
                    {
                        outputText("quit", NULL, nOption++);
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                    ++iter;
                    continue;
                }
                case MAG:
                {
                    ++iter;
                    std::string sNumExp;
                    if (getNumberExp(iter, pNode->end(), sNumExp))
                        outputText("mag_db", sNumExp.c_str(), nOption++);
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                if (!isValidSvrfName(iter->sValue))
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            {
                std::string sFileName;
                if (getStringName(iter, pNode->end(), sFileName))
                {
                    outputText(NULL, sFileName.c_str(), nOption++);
                    break;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
        }
        ++iter;
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

bool
rcsSynPvrsConvertor::transLayoutExceptionOption(const std::string &sExceptionName, int iSeverityValue, std::string &sCmd)
{
    if (0 == strcasecmp(sExceptionName.c_str(), "array_pitch_zero"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "AREF_PITCH_EQUAL_ZERO ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "AREF_PITCH_EQUAL_ZERO ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "AREF_PITCH_EQUAL_ZERO ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "aref_angled"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "AREF_ROTATION_ANGLED ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "AREF_ROTATION_ANGLED ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "AREF_ROTATION_ANGLED ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "aref_placement"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "AREF_ALLOWED ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "AREF_ALLOWED ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "AREF_ALLOWED ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "duplicate_cell"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "CELL_DUPLICATE ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "CELL_DUPLICATE ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "CELL_DUPLICATE ";
            sCmd += "ERROR ";
        }
        else if (iSeverityValue == 3)
        {
            sCmd += "CELL_DUPLICATE ";
            sCmd += "COMBINE ";
        }
        else if (iSeverityValue == 4)
        {
            sCmd += "CELL_DUPLICATE ";
            sCmd += "WARNING_COMBINE ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "exclude_cell_name"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "CELL_EXCLUDE ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "CELL_EXCLUDE ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "CELL_EXCLUDE ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "extent_cell"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "CELL_BOUNDRY ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "CELL_BOUNDRY ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "CELL_BOUNDRY ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "inside_cell"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "CELL_INSIDE ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "CELL_INSIDE ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "CELL_INSIDE ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "missing_reference"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "CELL_MISSING ";
            sCmd += "CREATE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "CELL_MISSING ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "CELL_MISSING ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "top_cell_chosen"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "TOP_CELL_WITH_WILDCARD ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "TOP_CELL_WITH_WILDCARD ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "TOP_CELL_WITH_WILDCARD ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "layer_directory"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "LAYER_DIR ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "LAYER_DIR ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "LAYER_DIR ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "datatype_map_source"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "MAP_SOURCE_DATATYPE ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "MAP_SOURCE_DATATYPE ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "MAP_SOURCE_DATATYPE ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "datatype_map_target"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "MAP_TARGET_DATATYPE ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "MAP_TARGET_DATATYPE ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "MAP_TARGET_DATATYPE ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "empty_base_layer"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "EMPTY_LAYOUT_DEVICE_LAYER ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "EMPTY_LAYOUT_DEVICE_LAYER ";
            sCmd += "PRINT ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "EMPTY_LAYOUT_DEVICE_LAYER ";
            sCmd += "WARNING_SEPERATRLY ";
        }
        else if (iSeverityValue == 3)
        {
            sCmd += "EMPTY_LAYOUT_DEVICE_LAYER ";
            sCmd += "WARNING_TOGETHER ";
        }
        else if (iSeverityValue == 4)
        {
            sCmd += "EMPTY_LAYOUT_DEVICE_LAYER ";
            sCmd += "ERROR ";
        }
        else if (iSeverityValue == 5)
        {
            sCmd += "NO_BASE_LAYER ";
            sCmd += "PRINT_WARNING ";
        }
        else if (iSeverityValue == 6)
        {
            sCmd += "NO_BASE_LAYER ";
            sCmd += "PRINT_ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "empty_layer"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "EMPTY_LAYER ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "EMPTY_LAYER ";
            sCmd += "PRINT ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "EMPTY_LAYER ";
            sCmd += "WARNING_SEPERATRLY ";
        }
        else if (iSeverityValue == 3)
        {
            sCmd += "EMPTY_LAYER ";
            sCmd += "WARNING_TOGETHER ";
        }
        else if (iSeverityValue == 4)
        {
            sCmd += "EMPTY_LAYER ";
            sCmd += "ERROR ";
        }
        else if (iSeverityValue == 5)
        {
            sCmd += "EMPTY_LAYER ";
            sCmd += "PRINT_WARNING ";
        }
        else if (iSeverityValue == 6)
        {
            sCmd += "EMPTY_LAYER ";
            sCmd += "PRINT_ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "unused_data"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "INVALID_DATA ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "INVALID_DATA ";
            sCmd += "PRINT ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "INVALID_DATA ";
            sCmd += "WARNING_SEPERATRLY ";
        }
        else if (iSeverityValue == 3)
        {
            sCmd += "INVALID_DATA ";
            sCmd += "WARNING_TOGETHER ";
        }
        else if (iSeverityValue == 4)
        {
            sCmd += "INVALID_DATA ";
            sCmd += "ERROR ";
        }
        else if (iSeverityValue == 5)
        {
            sCmd += "INVALID_DATA ";
            sCmd += "PRINT_WARNING ";
        }
        else if (iSeverityValue == 6)
        {
            sCmd += "INVALID_DATA ";
            sCmd += "PRINT_ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "circle_radius_zero"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "CIRCLE_RADIUS_EQUAL_ZERO ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "CIRCLE_RADIUS_EQUAL_ZERO ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "CIRCLE_RADIUS_EQUAL_ZERO ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "circle"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "CIRCLE ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "CIRCLE ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "CIRCLE ";
            sCmd += "ERROR ";
        }
        else if (iSeverityValue == 3)
        {
            sCmd += "CIRCLE ";
            sCmd += "CONVERT ";
        }
        else if (iSeverityValue == 4)
        {
            sCmd += "CIRCLE ";
            sCmd += "WARNING_CONVERT ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "ctrapezoid_area_zero"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "CTRAPEZOID_AREA_EQUAL_ZERO ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "CTRAPEZOID_AREA_EQUAL_ZERO ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "CTRAPEZOID_AREA_EQUAL_ZERO ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "ctrapezoid_degenerate"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "CTRAPEZOID_DEFECTIVE ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "CTRAPEZOID_DEFECTIVE ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "CTRAPEZOID_DEFECTIVE ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "trapezoid_area_zero"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "TRAPEZOID_AREA_EQUAL_ZERO ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "TRAPEZOID_AREA_EQUAL_ZERO ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "TRAPEZOID_AREA_EQUAL_ZERO ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "trapezoid_degenerate"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "TRAPEZOID_DEFECTIVE ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "TRAPEZOID_DEFECTIVE ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "TRAPEZOID_DEFECTIVE ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "xgeometry_unsupported"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "XGEOMETRY ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "XGEOMETRY ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "XGEOMETRY ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "path_acute"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "PATH_ACUTE ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "PATH_ACUTE ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "PATH_ACUTE ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 3)
        {
            sCmd += "PATH_ACUTE ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 4)
        {
            sCmd += "PATH_ACUTE ";
            sCmd += "ERROR ";
        }
        else if (iSeverityValue == 5)
        {
            sCmd += "PATH_ACUTE ";
            sCmd += "NORMAL_CLIP ";
        }
        else if (iSeverityValue == 6)
        {
            sCmd += "PATH_ACUTE ";
            sCmd += "WARNING_CLIP ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "path_angled"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "PATH_ANGLED_SEGMENTS ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "PATH_ANGLED_SEGMENTS ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "PATH_ANGLED_SEGMENTS ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 3)
        {
            sCmd += "PATH_ANGLED_SEGMENTS ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 4)
        {
            sCmd += "PATH_ANGLED_SEGMENTS ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "path_big"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "PATH_LARGE ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "PATH_LARGE ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "PATH_LARGE ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "path_circular"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "PATH_CIRCULAR ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "PATH_CIRCULAR ";
            sCmd += "CONVERT_FLUSH ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "PATH_CIRCULAR ";
            sCmd += "CONVERT_SQUARE ";
        }
        else if (iSeverityValue == 3)
        {
            sCmd += "PATH_CIRCULAR ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 4)
        {
            sCmd += "PATH_CIRCULAR ";
            sCmd += "WARNING_CONVERT_FLUSH ";
        }
        else if (iSeverityValue == 5)
        {
            sCmd += "PATH_CIRCULAR ";
            sCmd += "WARNING_CONVERT_SQUARE ";
        }
        else if (iSeverityValue == 6)
        {
            sCmd += "PATH_CIRCULAR ";
            sCmd += "ERROR ";
        }
        else if (iSeverityValue == 7)
        {
            sCmd += "PATH_CIRCULAR ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 8)
        {
            sCmd += "PATH_CIRCULAR ";
            sCmd += "WARNING ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "path_coincident"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "PATH_LENGTH_EQUAL_ZERO ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "PATH_LENGTH_EQUAL_ZERO ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "PATH_LENGTH_EQUAL_ZERO ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 3)
        {
            sCmd += "PATH_LENGTH_EQUAL_ZERO ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 4)
        {
            sCmd += "PATH_LENGTH_EQUAL_ZERO ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "path_degenerate"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "PATH_DEFECTIVE ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "PATH_DEFECTIVE ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "PATH_DEFECTIVE ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "path_endsegment_short"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "PATH_SHORT ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "PATH_SHORT ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "PATH_SHORT ";
            sCmd += "ERROR ";
        }
        else if (iSeverityValue == 3)
        {
            sCmd += "PATH_SHORT ";
            sCmd += "EXTEND ";
        }
        else if (iSeverityValue == 4)
        {
            sCmd += "PATH_SHORT ";
            sCmd += "WARNING_EXTEND ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "path_extension"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "PATH_EXTENSION ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "PATH_EXTENSION ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "PATH_EXTENSION ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "path_negative"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "PATH_NEGATIVE_EXTENSION ";
            sCmd += "IGNORE_EXTENSION ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "PATH_NEGATIVE_EXTENSION ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "PATH_NEGATIVE_EXTENSION ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 3)
        {
            sCmd += "PATH_NEGATIVE_EXTENSION ";
            sCmd += "WARNING_IGNORE_EXTENSION ";
        }
        else if (iSeverityValue == 4)
        {
            sCmd += "PATH_NEGATIVE_EXTENSION ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 5)
        {
            sCmd += "PATH_NEGATIVE_EXTENSION ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 6)
        {
            sCmd += "PATH_NEGATIVE_EXTENSION ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "path_nonorientable"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "PATH_NON_ORIENTABLE ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "PATH_NON_ORIENTABLE ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "PATH_NON_ORIENTABLE ";
            sCmd += "ERROR ";
        }
        else if (iSeverityValue == 3)
        {
            sCmd += "PATH_NON_ORIENTABLE ";
            sCmd += "FIX ";
        }
        else if (iSeverityValue == 4)
        {
            sCmd += "PATH_NON_ORIENTABLE ";
            sCmd += "WARNING_FIX ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "path_nonsimple"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "PATH_NON_SIMPLE ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "PATH_NON_SIMPLE ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "PATH_NON_SIMPLE ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "path_spike"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "PATH_RETRACE ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "PATH_RETRACE ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "PATH_RETRACE ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "path_variable"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "PATH_VAR ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "PATH_VAR ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "PATH_VAR ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 3)
        {
            sCmd += "PATH_VAR ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 4)
        {
            sCmd += "PATH_VAR ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "path_width_absolute"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "PATH_ABSOLUTE_WIDTH ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "PATH_ABSOLUTE_WIDTH ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "PATH_ABSOLUTE_WIDTH ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "path_width_odd"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "PATH_ODD_WIDTH ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "PATH_ODD_WIDTH ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "PATH_ODD_WIDTH ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "path_width_zero"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "PATH_ZERO_WIDTH ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "PATH_ZERO_WIDTH ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "PATH_ZERO_WIDTH ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "placement_angle_absolute"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "PLACEMENT_ABSOLUTE_ANGLE ";
            sCmd += "IGNORE_ROTATION ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "PLACEMENT_ABSOLUTE_ANGLE ";
            sCmd += "WARNING_IGNORE_ROTATION ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "PLACEMENT_ABSOLUTE_ANGLE ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "placement_magnification_absolute"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "PLACEMENT_ABSOLUTE_MAG ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "PLACEMENT_ABSOLUTE_MAG ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "PLACEMENT_ABSOLUTE_MAG ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "placement_magnification_nonpositive"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "PLACEMENT_NON_POSITIVE_MAG ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "PLACEMENT_NON_POSITIVE_MAG ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "PLACEMENT_NON_POSITIVE_MAG ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "placement_magnified"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "PLACEMENT_MAG ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "PLACEMENT_MAG ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "PLACEMENT_MAG ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "layout_polygon"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "LAYOUT_ADD_POLYGON ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "LAYOUT_ADD_POLYGON ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "LAYOUT_ADD_POLYGON ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "polygon_acute"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "POLYGON_ACUTE ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "POLYGON_ACUTE ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "POLYGON_ACUTE ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "polygon_area_zero"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "POLYGON_AREA_EQUAL_ZERO ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "POLYGON_AREA_EQUAL_ZERO ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "POLYGON_AREA_EQUAL_ZERO ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "polygon_degenerate"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "POLYGON_DEFECTIVE ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "POLYGON_DEFECTIVE ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "POLYGON_DEFECTIVE ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "polygon_is_rectangle"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "POLYGON_TO_RECTANGLE ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "POLYGON_TO_RECTANGLE ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "POLYGON_TO_RECTANGLE ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "polygon_nonorientable"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "POLYGON_NON_ORIENTABLE ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "POLYGON_NON_ORIENTABLE ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "POLYGON_NON_ORIENTABLE ";
            sCmd += "ERROR ";
        }
        else if (iSeverityValue == 3)
        {
            sCmd += "POLYGON_NON_ORIENTABLE ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 4)
        {
            sCmd += "POLYGON_NON_ORIENTABLE ";
            sCmd += "WARNING ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "polygon_nonsimple"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "POLYGON_NON_SIMPLE ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "POLYGON_NON_SIMPLE ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "POLYGON_NON_SIMPLE ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "polygon_not_closed"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "POLYGON_NOT_CLOSED ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "POLYGON_NOT_CLOSED ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "POLYGON_NOT_CLOSED ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "polygon_spike"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "POLYGON_RETRACE ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "POLYGON_RETRACE ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "POLYGON_RETRACE ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "rectangle_side_zero"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "RECTANGLE_WIDTH_EQUAL_ZERO ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "RECTANGLE_WIDTH_EQUAL_ZERO ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "RECTANGLE_WIDTH_EQUAL_ZERO ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "box_record"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "BOX_RECORD ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "BOX_RECORD ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "BOX_RECORD ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 3)
        {
            sCmd += "BOX_RECORD ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 4)
        {
            sCmd += "BOX_RECORD ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "cellname_nstring"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "NSTRING_CELL_NAME ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "NSTRING_CELL_NAME ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "NSTRING_CELL_NAME ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "node_record"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "NODE_RECORD ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "NODE_RECORD ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "NODE_RECORD ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 3)
        {
            sCmd += "NODE_RECORD ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 4)
        {
            sCmd += "NODE_RECORD ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "propname_nstring"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "NSTRING_PROPNAME ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "NSTRING_PROPNAME ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "NSTRING_PROPNAME ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "textstring_astring"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "ASTRING_TEXTSTRING ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "ASTRING_TEXTSTRING ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "ASTRING_TEXTSTRING ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "xelement_unsupported"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "XELEMENT ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "XELEMENT ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "XELEMENT ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "xname_unsupported"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "XNAME ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "XNAME ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "XNAME ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "layout_text"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "LAYOUT_ADD_TEXT ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "LAYOUT_ADD_TEXT ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "LAYOUT_ADD_TEXT ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "text_big"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "TEXT_LARGE ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "TEXT_LARGE ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "TEXT_LARGE ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "text_degenerate"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "TEXT_DEFECTIVE ";
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "TEXT_DEFECTIVE ";
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "TEXT_DEFECTIVE ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "metric_input_file"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "METRIC_LAYOUT ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "METRIC_LAYOUT ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "METRIC_LAYOUT ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "metric_rule_file"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "METRIC_RULE_DECK ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "METRIC_RULE_DECK ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "METRIC_RULE_DECK ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "precision_input_file"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "PRECISION_LAYOUT ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "PRECISION_LAYOUT ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "PRECISION_LAYOUT ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "precision_layout"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "PRECISION_CONFLICT ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "PRECISION_CONFLICT ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "PRECISION_CONFLICT ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "precision_rule_file"))
    {
        if (iSeverityValue == 0)
        {
            sCmd += "PRECISION_RULE_DECK ";
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "PRECISION_RULE_DECK ";
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "PRECISION_RULE_DECK ";
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else if (0 == strcasecmp(sExceptionName.c_str(), "path_width_multiple"))
    {
        sCmd += "PATH_WIDTH_MULTIPLE ";
        if (iSeverityValue == 0)
        {
            sCmd += "IGNORE ";
        }
        else if (iSeverityValue == 1)
        {
            sCmd += "NORMAL ";
        }
        else if (iSeverityValue == 2)
        {
            sCmd += "WARNING_IGNORE ";
        }
        else if (iSeverityValue == 3)
        {
            sCmd += "WARNING ";
        }
        else if (iSeverityValue == 4)
        {
            sCmd += "ERROR ";
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    return true;
}

static bool
_isExceptInteger(string &num)
{
    trim(num);
    std::string::size_type idx  = 0;
    std::string::size_type len = num.size();
    if (len == 0)
    {
        return false;
    }
    if (num[idx] == '+' || num[idx] == '-')
    {
        idx++;
        if (idx == len)
        {
            return false;
        }
    }
    while (idx < len)
    {
        if (isdigit(num[idx]))
        {
            ++idx;
        }
        else if (num[idx] == '.')
        {
            int ptPos = idx;
            ++idx;
            if (idx == len)
            {
                return false;
            }
            while (idx < len)
            {
                if (num[idx] != '0')
                {
                    return false;
                }
                ++idx;
            }
            num.erase(ptPos);
        }
        else
        {
            return false;
        }
    }
    return true;
}

void
rcsSynPvrsConvertor::convExceptionSeveritySpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sCmd = (_isTvf2Trs ? "layout_exception severity " : "layout_exception ( severity ");
    if (iter != pNode->end())
    {
        std::string sExceptionName     = iter->sValue;
        std::string sSeverityValue;
        std::string sOp;
        toLower(sExceptionName);
        std::list<rcsToken_T>::iterator err_iter = iter;
        ++iter;
        if (iter == pNode->end())
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2,
                                pNode->begin()->nLineNo, pNode->begin()->sValue);
        }

        if (iter->eType == OPERATOR && (iter->sValue == "+" || iter->sValue == "-"))
        {
            ++iter;
            sOp = iter->sValue;
        }

        if (iter == pNode->end() || !getNumberExp(iter, pNode->end(), sSeverityValue, false,false))
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2,
                                pNode->begin()->nLineNo, pNode->begin()->sValue);
        }

        if (_isExceptInteger(sSeverityValue))
        {
            if (!transLayoutExceptionOption(sExceptionName,atoi(sSeverityValue.insert(0,sOp).c_str()),sCmd))
            {
                outputCon2Error(pNode,*err_iter);
                return;
            }
        }
        else
        {
            outputCon2Error(pNode,*err_iter);
            return;
        }

        (*m_oPVRSStream) << sCmd;
        
        if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == RDB)
        {
            outputText("result_db ", NULL);
            ++iter;
            if (iter != pNode->end())
            {
                std::string sRdbName;
                if (getStringName(iter, pNode->end(), sRdbName))
                    outputText(NULL, sRdbName.c_str());
                else
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                            iter->nLineNo, iter->sValue);
            }
        }
    }
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2,
                            pNode->begin()->nLineNo, pNode->begin()->sValue);
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convCoordListSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if (pNode->getSpecificationType() == LAYOUT_WINDOW)
    {
        (*m_oPVRSStream) << (_isTvf2Trs ? "layout_select_window region " : "layout_select_window ( region ");
    }
    else if (pNode->getSpecificationType() == LAYOUT_WINDEL)
    {
        (*m_oPVRSStream) << (_isTvf2Trs ? "layout_exclude_window region " : "layout_exclude_window ( region ");
    }

    bool isAllowExpression = !(pNode->getSpecificationType() == LAYOUT_WINDOW ||
                               pNode->getSpecificationType() == LAYOUT_WINDEL);
    while(iter != pNode->end())
    {
        std::string sValue;
        if (!getNumberExp(iter, pNode->end(), sValue, true, isAllowExpression))
        {
            sValue = iter->sValue;
        }
        outputText(NULL, sValue.c_str());
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convertDBMergeOnInput(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "layout_merge_on_input " : "layout_merge_on_input ( ");
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if(iter->eType == SECONDARY_KEYWORD && (iter->eKeyType == YES || iter->eKeyType == NO))
    {
        if(iter->eKeyType == YES)
            outputText(NULL, "yes");
        else if(iter->eKeyType == NO)
            outputText(NULL, "no");
    }
    else
    {
        outputText(NULL, "yes");
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
    return;
}

void
rcsSynPvrsConvertor::convDBMagnifySpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "layout_magnify " : "layout_magnify ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    switch(iter->eType)
    {
        case SECONDARY_KEYWORD:
        {
            if (AUTO == iter->eKeyType)
            {
                outputText(NULL, "default", nOption++);
                if (++iter != pNode->end())
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
            }
        }
        default:
        {
            std::string sNumExp;
            if (getNumberExp(iter, pNode->end(), sNumExp))
                outputText(NULL, sNumExp.c_str(), nOption++);
            break;
        }
    }

    if (iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
       PLACE == iter->eKeyType)
    {
        outputText(NULL, "new_cell", nOption++);
        ++iter;
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}


void
rcsSynPvrsConvertor::convRenameTextSpec(rcsSynSpecificationNode_T *pNode)
{
    if (pNode->getSpecificationType() == LAYOUT_IGNORE_TEXT)
        m_sCurCommandString = "LAYOUT IGNORE TEXT";
    else
        m_sCurCommandString = "LAYOUT RENAME TEXT";
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    if (pNode->getSpecificationType() == LAYOUT_IGNORE_TEXT)
        (*m_oPVRSStream) << (_isTvf2Trs ? "layout_remove_text " : "layout_remove_text (");
    else
        (*m_oPVRSStream) << (_isTvf2Trs ? "layout_rename_text " : "layout_rename_text (");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                if (DATABASE == iter->eKeyType)
                {
                    outputText("in_db", NULL, nOption++);
                    break;
                }
                else if (RULES == iter->eKeyType)
                {
                    outputText("in_rule", NULL, nOption++);
                    break;
                }
                else if (BY_LAYER == iter->eKeyType)
                {
                    ++iter;
                    std::string sLayerName;
                    outputText("in_layer", NULL, nOption++);
                    if ( (iter->eType != IDENTIFIER_NAME) || !parseTmpLayerForOperation(iter->sValue) )
                    {
                        if (!getLayerRefName(iter, pNode->end(), sLayerName))
                            sLayerName = iter->sValue;
                        outputText(NULL, sLayerName.c_str(), nOption++);
                    }

                    if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD
                       && TO == iter->eKeyType)
                    {
                        sLayerName.clear();
                        outputText("to", NULL, nOption++);
                        ++iter;
                        if ( (iter->eType != IDENTIFIER_NAME) || !parseTmpLayerForOperation(iter->sValue) )
                        {
                            if (!getLayerRefName(iter, pNode->end(), sLayerName))
                                sLayerName = iter->sValue;
                            outputText(NULL, sLayerName.c_str(), nOption++);
                        }
                    }
                    continue;
                }
                else if (CELL == iter->eKeyType)
                {
                    if (++iter != pNode->end() && iter->eKeyType == LIST)
                    {
                        switch(iter->eType)
                        {
                            case SECONDARY_KEYWORD:
                            case IDENTIFIER_NAME:
                            {
                                if (!isValidSvrfName(iter->sValue))
                                {
                                    throw rcErrorNode_T(rcErrorNode_T::ERROR,
                                                        INP1,
                                                        iter->nLineNo,
                                                        iter->sValue);
                                }
                            }
                            case STRING_CONSTANTS:
                            {
                                std::string sCellListName;
                                if (!getStringName(++iter, pNode->end(), sCellListName))
                                    sCellListName = iter->sValue;
                                outputText("cell_group",  sCellListName.c_str(), nOption++);
                                break;
                            }
                            default:
                                throw rcErrorNode_T(rcErrorNode_T::ERROR,
                                                    INP1, iter->nLineNo,
                                                    iter->sValue);
                                break;
                        }
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                    break;
                }
            }
            case IDENTIFIER_NAME:
            {
                if (!isValidSvrfName(iter->sValue))
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
            }
            case STRING_CONSTANTS:
            {
                std::string sValue;
                if (!getStringName(iter, pNode->end(), sValue))
                    sValue = iter->sValue;
                outputText(NULL, sValue.c_str(), nOption++);
                break;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC14,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convLayerResolutionSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "resolution layer " : "resolution ( layer ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    while(iter != pNode->end() && (iter->eType == IDENTIFIER_NAME || iter->eType == STRING_CONSTANTS ||
          iter->eType == SECONDARY_KEYWORD))
    {
        std::string sLayerName;
        if (!getStringName(iter, pNode->end(), sLayerName))
            break;
        outputText(NULL, sLayerName.c_str());
        ++iter;
    }

    std::string sNumExp;
    if (getNumberExp(iter, pNode->end(), sNumExp))
    {
        outputText(NULL, sNumExp.c_str());
    }

    if (iter != pNode->end())
    {
        sNumExp.clear();
        if (getNumberExp(iter, pNode->end(), sNumExp))
        {
            outputText(NULL, sNumExp.c_str());
        }
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convRenameCellSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << (_isTvf2Trs ? "layout_rename_cell " : "layout_rename_cell ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sName;
    if (getStringName(iter, pNode->end(), sName))
    {
        outputText(NULL, sName.c_str(), nOption++);
    }
    sName.clear();
    ++iter;
    if (getStringName(iter, pNode->end(), sName))
    {
        outputText(NULL, sName.c_str(), nOption++);
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convBlackBoxSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << (_isTvf2Trs ? "lvs_black_box " : "lvs_black_box ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    while(iter != pNode->end())
    {
#if 0
        std::string sCellName;
        if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == LAYOUT)
            outputText(NULL, "layout", nOption++);
        else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SOURCE)
            outputText(NULL, "schematic", nOption++);
        else if (getStringName(iter, pNode->end(), sCellName))
        {
            if (m_vVariableNames.find(iter->sValue) != m_vVariableNames.end())
            {
                sCellName = iter->sValue;
                toLower(sCellName);
            }
            outputText(NULL, sCellName.c_str(), nOption++);
        }
#endif
        switch (iter->eKeyType)
        {
            case SOURCE:
                outputText(NULL, "schematic", nOption++);
                break;
            case LAYOUT:
            case BLACK:
            case GRAY:
            case ONLY:
            case DEVICES:
            {
                string str = iter->sValue;
                toLower(str);
                outputText(NULL, str.c_str(), nOption++);
                break;
            }
            default:
                outputText(NULL, iter->sValue.c_str(), nOption++);
                break;
        }
        ++iter;
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void rcsSynPvrsConvertor::convLvsBlackBoxPortSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "lvs_black_box_port " : "lvs_black_box_port ( ");
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(++iter != pNode->end())
    {
        std::string sValue;
        if (!getStringName(iter, pNode->end(), sValue))
        {
            sValue = iter->sValue;
        }
        outputText(NULL, sValue.c_str(), nOption++);
    }

    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;

}

void rcsSynPvrsConvertor::convLvsBlackBoxPortDepthSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "lvs_black_box_port_level " : "lvs_black_box_port_level (");
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(++iter != pNode->end())
    {
        if (iter->eKeyType == PRIMARY || iter->eKeyType == ALL)
        {
            std::string svalue = iter->sValue;
            toLower(svalue);
            outputText(NULL, svalue.c_str(), nOption++);
        }
        else
        {
             outputText(NULL, iter->sValue.c_str(), nOption++);
        }
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}
void
rcsSynPvrsConvertor::convLvsBoxCutExtentSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "LVS_BLACK_BOX_CUT_BOUNDARY " : "LVS_BLACK_BOX_CUT_BOUNDARY ( ");
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    if(++iter != pNode->end())
    {
        std::string sValue;
        if (!getStringName(iter, pNode->end(), sValue))
        {
            sValue = iter->sValue;
        }
        outputText(NULL, sValue.c_str(), nOption++);
    }

    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convLvsBoxPeekLayerSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "LVS_BLACK_BOX_VISITABLE_LAYER " : "LVS_BLACK_BOX_VISITABLE_LAYER ( ");
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(++iter != pNode->end())
    {
        std::string sValue;
        if (!getStringName(iter, pNode->end(), sValue))
        {
            sValue = iter->sValue;
        }
        outputText(NULL, sValue.c_str(), nOption++);
    }

    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convPropResolutionMaxSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "lvs_check_property_radius " : "lvs_check_property_radius ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sValue;
    if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == ALL)
        outputText(NULL, "all");
    else if (getNumberExp(iter, pNode->end(), sValue))
        outputText(NULL, sValue.c_str());
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convInitialNetSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "lvs_corresponding_net " : "lvs_corresponding_net ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sLayoutName;
    if (getStringName(iter, pNode->end(), sLayoutName))
        outputText(NULL, sLayoutName.c_str(), nOption++);

    ++iter;
    std::string sSourceName;
    if (getStringName(iter, pNode->end(), sSourceName))
        outputText(NULL, sSourceName.c_str(), nOption++);
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convNameListSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    switch (pNode->getSpecificationType())
    {
        case VIRTUAL_CONNECT_BOX_NAME:
            (*m_oPVRSStream) << (_isTvf2Trs ? "virtual_connect black_box_name " : "virtual_connect ( black_box_name ");
            break;
        case VIRTUAL_CONNECT_NAME:
            (*m_oPVRSStream) << (_isTvf2Trs ? "virtual_connect name " : "virtual_connect ( name ");
            break;
        case LVS_EXCLUDE_HCELL:
            (*m_oPVRSStream) << (_isTvf2Trs ? "lvs_ignore_hcell " : "lvs_ignore_hcell ( ");
            break;
        case LVS_GROUND_NAME:
            (*m_oPVRSStream) << (_isTvf2Trs ? "lvs_ground " : "lvs_ground ( ");
            break;
        case LVS_POWER_NAME:
            (*m_oPVRSStream) << (_isTvf2Trs ? "lvs_power " : "lvs_power ( ");
            break;
        default:
            return;
    }
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    while(iter != pNode->end())
    {
        std::string sValue;
        if (!getStringName(iter, pNode->end(), sValue))
        {
            sValue = iter->sValue;
        }
        outputText(NULL, sValue.c_str());
        ++iter;
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convIdentifyGatesSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "lvs_identify_gate " : "lvs_identify_gate ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    while(iter != pNode->end())
    {
        if (iter->eType == SECONDARY_KEYWORD)
        {
            switch(iter->eKeyType)
            {
                case ALL:
                    outputText(NULL, "all", nOption++);
                    break;
                case SIMPLE:
                    outputText(NULL, "basic", nOption++);
                    break;
                case NONE:
                    outputText(NULL, "none", nOption++);
                    break;
                case XALSO:
                    outputText(NULL, "x_subtype", nOption++);
                    break;
                case MIX:
                {
                    ++iter;
                    if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SUBTYPES)
                    {
                        outputText(NULL, "diff_subtype", nOption++);
                        break;
                    }
                    break;
                }
                case WITHIN:
                {
                    ++iter;
                    if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == TOLERANCE)
                    {
                        outputText(NULL, "in_tolerance", nOption++);
                        break;
                    }
                    break;
                }
                case CELL:
                {
                    ++iter;
                    if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == LIST)
                    {
                        ++iter;
                        std::string sValue;
                        if (getStringName(iter, pNode->end(), sValue))
                            outputText("cell_group", sValue.c_str(), nOption++);
                    }
                    break;
                }
                default:
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                    break;
            }
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
        ++iter;
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convLVSFlattenInsideCell(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "lvs_flatten_inside_cell " : "lvs_flatten_inside_cell ( ");
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while (++iter != pNode->end())
    {
        if (iter->eType == SECONDARY_KEYWORD)
        {
            switch (iter->eKeyType)
            {
                case LAYOUT:
                    outputText(NULL, "layout", nOption++);
                    continue;
                case SOURCE:
                    outputText(NULL, "schematic", nOption++);
                    continue;
                case YES:
                    outputText(NULL, "yes", nOption++);
                    continue;
                case NO:
                    outputText(NULL, "no", nOption++);
                    continue;
                default:
                    break;
            }
        }
        std::string str;
        getStringName(iter, pNode->end(), str);
        outputText(NULL, str.c_str(), nOption++);
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGroupGlobalNameSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "lvs_layout_global_net_group " : "lvs_layout_global_net_group ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    std::string sGroupName;
    if (getStringName(iter, pNode->end(), sGroupName))
        outputText(NULL, sGroupName.c_str(), nOption++);

    ++iter;
    outputText(NULL, NULL, nOption++);
    while(iter != pNode->end())
    {
        std::string sValue;
        if (!getStringName(iter, pNode->end(), sValue))
        {
           sValue = iter->sValue;
        }

        outputText(NULL, sValue.c_str(), nOption);
        ++iter;
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convNonMatchNameSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;
    if (pNode->getSpecificationType() == LVS_NON_USER_NAME_INSTANCE)
        (*m_oPVRSStream) << (_isTvf2Trs ? "lvs_non_user_define_name instance " : "lvs_non_user_define_name ( instance ");
    else if (pNode->getSpecificationType() == LVS_NON_USER_NAME_NET)
        (*m_oPVRSStream) << (_isTvf2Trs ? "lvs_non_user_define_name net " : "lvs_non_user_define_name ( net ");
    else if (pNode->getSpecificationType() == LVS_NON_USER_NAME_PORT)
        (*m_oPVRSStream) << (_isTvf2Trs ? "lvs_non_user_define_name port " : "lvs_non_user_define_name ( port ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    while(iter != pNode->end())
    {
        std::string sValue;
        if (getStringName(iter, pNode->end(), sValue))
            outputText(NULL, sValue.c_str(), nOption++);
        ++iter;
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convLvsReportOptionSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    bool hasSupportOption = false;
    for(; iter != pNode->end(); ++iter)
    {
        if (0 == strcasecmp(iter->sValue.c_str(), "A"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "AV"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "B"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "BV"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "C"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "CV"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "D"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "E"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "E1"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "EC"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "F"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "FX"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "G"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "H"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "I"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "LPE"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "MC"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "N"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "NOK"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "O"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "P"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "PG"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "R"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "RA"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "S"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "SP"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "SPE"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "V"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "W"))
        {
            hasSupportOption = true;
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "X"))
        {
            hasSupportOption = true;
        }
    }

    if (!hasSupportOption)
    {
        std::string sValue;
        pNode->getCmdValue(sValue);
        m_oLogStream << "This Operation or Specification in line " << pNode->begin()->nLineNo
                     << " can not be Converted correctly: " << std::endl << sValue << std::endl << std::endl;

        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::WARNING, CON1,
                              pNode->getNodeBeginLineNo(), pNode->begin()->sValue));
        return;
    }

    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << (_isTvf2Trs ? "lvs_report option " : "lvs_report ( option ");

    iter = pNode->begin();
    ++iter;
    for(; iter != pNode->end(); ++iter)
    {
        if (0 == strcasecmp(iter->sValue.c_str(), "A"))
        {
            outputText("connection", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "AV"))
        {
            outputText("connection_exclude_pg", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "B"))
        {
            outputText("short_open", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "BV"))
        {
            outputText("short_open_exclude_pg", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "C"))
        {
            outputText("missing_net", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "CV"))
        {
            outputText("missing_net_exclude_pg", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "D"))
        {
            outputText("missing_instance", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "E"))
        {
            outputText("missing_property", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "E1"))
        {
            outputText("missing_property_tolerance", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "EC"))
        {
            outputText("uniconnect_error", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "F"))
        {
            outputText("merge_mos", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "FX"))
        {
            outputText("detail_information", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "G"))
        {
            outputText("property", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "H"))
        {
            outputText("missing_cell", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "I"))
        {
            outputText("isolated_net", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "LPE"))
        {
            outputText("layout_passthrough", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "MC"))
        {
            outputText("count_by_model", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "N"))
        {
            outputText("missing_name", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "NOK"))
        {
            outputText("hcell", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "O"))
        {
            outputText("resolution_warning", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "P"))
        {
            outputText("short_port", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "PG"))
        {
            outputText("hcell_pg", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "R"))
        {
            outputText("ambiguity", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "RA"))
        {
            outputText("ambiguity_all", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "S"))
        {
            outputText("show_uniconnect", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "SP"))
        {
            outputText("schematic_passthrough_warning", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "SPE"))
        {
            outputText("schematic_passthrough", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "V"))
        {
            outputText("show_virtual_connection", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "W"))
        {
            outputText("same_data", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "X"))
        {
            outputText("not_identical_pin", NULL, nOption++);
        }
        else if (0 == strcasecmp(iter->sValue.c_str(), "NONE"))
        {
            outputCon2Error(pNode,*iter);
        }
        else
        {
            std::string sValue;
            pNode->getCmdValue(sValue);
            m_oLogStream << "This Option " << iter->sValue << " in line " << iter->nLineNo
                         << " can not be Converted correctly in this operation: " << std::endl
                         << sValue << std::endl << std::endl;

            s_errManager.addError(rcErrorNode_T(rcErrorNode_T::WARNING, CON2,
                                                iter->nLineNo, iter->sValue));
        }
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convCheckSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    if (pNode->getSpecificationType() == FLAG_OFFGRID)
        (*m_oPVRSStream) << (_isTvf2Trs ? "print_log offgrid_vertex " : "print_log ( offgrid_vertex ");
    else if (pNode->getSpecificationType() == FLAG_ACUTE)
        (*m_oPVRSStream) << (_isTvf2Trs ? "print_log acute_edge " : "print_log ( acute_edge ");
    else if (pNode->getSpecificationType() == FLAG_SKEW)
        (*m_oPVRSStream) << (_isTvf2Trs ? "print_log skew_edge " : "print_log ( skew_edge ");
    else if (pNode->getSpecificationType() == FLAG_ANGLED)
        (*m_oPVRSStream) << (_isTvf2Trs ? "print_log angled_edge " : "print_log ( angled_edge ");
    else if (pNode->getSpecificationType() == FLAG_NONSIMPLE)
        (*m_oPVRSStream) << (_isTvf2Trs ? "print_log self_intersecting_polygon " : "print_log ( self_intersecting_polygon ");
    else if (pNode->getSpecificationType() == FLAG_NONSIMPLE_PATH)
        (*m_oPVRSStream) << (_isTvf2Trs ? "print_log self_intersecting_path " : "print_log ( self_intersecting_path ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    switch(iter->eType)
    {
        case SECONDARY_KEYWORD:
        {
            if (YES == iter->eKeyType)
            {
                outputText(NULL, "yes");
                break;
            }
            else if (NO == iter->eKeyType)
            {
                outputText(NULL, "no");
                break;
            }
        }
        default:
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
            break;
        }
    }

    ++iter;
    if (iter != pNode->end())
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                if (MAXIMUM == iter->eKeyType)
                {
                    if (++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                       WARNINGS == iter->eKeyType)
                    {
                        ++iter;
                        if (iter != pNode->end())
                        {
                            if (SECONDARY_KEYWORD == iter->eType &&
                               ALL == iter->eKeyType)
                            {
                                outputText("max_warnings", "all");
                                break;
                            }
                            else
                            {
                                std::string sNumExp;
                                if (getNumberExp(iter, pNode->end(), sNumExp))
                                    outputText("max_warnings", sNumExp.c_str());
                                break;
                            }
                        }
                        else
                        {
                            --iter;
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP11,
                                                iter->nLineNo, "maximum warnings");
                        }
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                }
            }
            default:
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
            }
        }
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convPutsSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if (iter == pNode->end())
    {
        --iter;
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2, iter->nLineNo, iter->sValue);
    }

    std::string sMessage;
    if (getStringName(iter, pNode->end(), sMessage))
    {
        (*m_oPVRSStream) << (_isTvf2Trs ? "print_log compile " : "print_log ( compile ")<< sMessage;
        if (!_isTvf2Trs)
        {
            (*m_oPVRSStream) << " ) ";
        }
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convExpandTextInCellSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    std::string sCmd = (_isTvf2Trs ? "promote_cell_text " : "promote_cell_text ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sOriginalCellName;
    std::string sTargetCellName;
    if (iter != pNode->end())
    {
        getStringName(iter, pNode->end(), sOriginalCellName);
        sCmd += sOriginalCellName + " ";
        ++iter;
        if (iter != pNode->end())
        {
            if ((iter->eType == SECONDARY_KEYWORD) && (PRIMARY == iter->eKeyType))
            {
                sCmd += "top_cell";
            }
            else
            {
                getStringName(iter, pNode->end(), sTargetCellName);
                sCmd += sTargetCellName;
            }
            ++iter;
            if (iter != pNode->end())
            {
                outputCon1Error(pNode);
                return;
            }
        }
    }
    else
    {
        outputCon1Error(pNode);
        return;
    }
    (*m_oPVRSStream) << sCmd;
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convErrorSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if (iter == pNode->end())
    {
        --iter;
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2, iter->nLineNo, iter->sValue);
    }

    std::string sValue;
    if (getStringName(iter, pNode->end(), sValue))
    {
        (*m_oPVRSStream) << (_isTvf2Trs ? "quit compile_error " : "quit ( compile_error ") << sValue << std::endl;
        if (!_isTvf2Trs)
        {
            (*m_oPVRSStream) << " ) ";
        }
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convSpiceOptionSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    hvUInt32 nOption = 0;

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    bool bTwoOpt = true;
    switch(pNode->getSpecificationType())
    {
        case LVS_SPICE_MULTIPLIER_NAME:
        {
            (*m_oPVRSStream) << (_isTvf2Trs ? "spice multiplier_factor_name " : "spice ( multiplier_factor_name ");
            for(; iter != pNode->end(); ++iter)
            {
                std::string sValue;
                if (getStringName(iter, pNode->end(), sValue))
                    outputText(NULL, sValue.c_str(), nOption++);
            }
            break;
        }
        case LVS_SPICE_OPTION:
        {
            bool bF = false;
            bool bS = false;
            bool bX = false;
            (*m_oPVRSStream) << (_isTvf2Trs ? "spice " : "spice ( ");
            for(; iter != pNode->end(); ++iter)
            {
                if (0 == strcasecmp(iter->sValue.c_str(), "F"))
                {
                    bF = true;
                }
                else if (0 == strcasecmp(iter->sValue.c_str(), "S"))
                {
                    bS = true;
                }
                else if (0 == strcasecmp(iter->sValue.c_str(), "X"))
                {
                    bX = true;
                }
                else
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
            }
            if (bF)
            {
                outputText(NULL, "floating_pin yes", nOption);
            }
            else
            {
                outputText(NULL, "floating_pin no", nOption);
            }
            if (bS)
            {
                outputText(NULL, "duplicate_name yes", nOption);
            }
            else
            {
                outputText(NULL, "duplicate_name no", nOption);
            }
            if (bX)
            {
                outputText(NULL, "keep_name yes", nOption);
            }
            else
            {
                outputText(NULL, "keep_name no", nOption);
            }
            break;
        }
        case LVS_SPICE_SCALE_X_PARAMETERS:
        {
            (*m_oPVRSStream) << (_isTvf2Trs ? "spice xscale " : "spice ( xscale ");
            if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == YES)
            {
                outputText(NULL, "all", nOption++);
            }
            else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == NO)
            {
                outputText(NULL, "none", nOption++);
            }
            else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == MOS)
            {
                outputText(NULL, "mos", nOption++);
                ++iter;
                if(iter != pNode->end())
                {
                    if(iter->eType == SECONDARY_KEYWORD)
                    {
                        if(iter->eKeyType == YES)
                            outputText(NULL, "all", nOption++);
                        else if(iter->eKeyType == NO)
                            outputText(NULL, "none", nOption++);
                        else
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                iter->nLineNo, iter->sValue);
                    }
                    else
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                            iter->nLineNo, iter->sValue);
                }
                else
                {
                    bTwoOpt = false;
                }
            }
            else
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
            if (bTwoOpt)
            {
                ++iter;
            }
            if (iter != pNode->end())
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
            break;
        }
        default:
            Utassert(false);
            break;
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::covDeviceGroupSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "device_group " : "device_group ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    while(iter != pNode->end())
    {
        std::string sMessage;
        if (getStringName(iter, pNode->end(), sMessage))
            (*m_oPVRSStream) << sMessage << " ";
        else if(iter->eType == SEPARATOR && (iter->sValue == "(" || iter->sValue == ")"))
            (*m_oPVRSStream) << iter->sValue << " ";
        else
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo, iter->sValue);
        ++iter;
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convRenameSpiceParameterSpec(rcsSynSpecificationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "spice rename_param " : "spice ( rename_param ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    std::string sValue;
    if (getStringName(iter, pNode->end(), sValue))
        outputText(NULL, sValue.c_str(), nOption++);

    ++iter;
    sValue.clear();
    if (getStringName(iter, pNode->end(), sValue))
        outputText(NULL, sValue.c_str(), nOption++);

    ++iter;

    while(iter != pNode->end())
    {
        if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == SOURCE)
        {
            outputText(NULL, "schematic", nOption++);
        }
        else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == LAYOUT)
        {
            outputText(NULL, "layout", nOption++);
        }
        else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == LEFT)
        {
            outputText(NULL, "left_side", nOption++);
        }
        else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == RIGHT)
        {
            outputText(NULL, "right_side", nOption++);
        }
        else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == COMMENT_SECOND)
        {
            outputText("comment_coded", "yes", nOption++);
        }
        else if (((iter->eType == SECONDARY_KEYWORD) || (iter->eType == IDENTIFIER_NAME))
                && (0 == strcasecmp(iter->sValue.c_str(), "NOCOMMENT")))
        {
            outputText("comment_coded", "no", nOption++);
        }
        else if (((iter->eType == SECONDARY_KEYWORD) || (iter->eType == IDENTIFIER_NAME))
                && (0 == strcasecmp(iter->sValue.c_str(), "C")))
        {
            outputText("cap", NULL, nOption++);
        }
        else if (((iter->eType == SECONDARY_KEYWORD) || (iter->eType == IDENTIFIER_NAME))
                && (0 == strcasecmp(iter->sValue.c_str(), "D")))
        {
            outputText("dio", NULL, nOption++);
        }
        else if (((iter->eType == SECONDARY_KEYWORD) || (iter->eType == IDENTIFIER_NAME))
                && (0 == strcasecmp(iter->sValue.c_str(), "J")))
        {
            outputText("jfet", NULL, nOption++);
        }
        else if (((iter->eType == SECONDARY_KEYWORD) || (iter->eType == IDENTIFIER_NAME))
                && (0 == strcasecmp(iter->sValue.c_str(), "L")))
        {
            outputText("ind", NULL, nOption++);
        }
        else if (((iter->eType == SECONDARY_KEYWORD) || (iter->eType == IDENTIFIER_NAME))
                && (0 == strcasecmp(iter->sValue.c_str(), "M")))
        {
            outputText("mos", NULL, nOption++);
        }
        else if (((iter->eType == SECONDARY_KEYWORD) || (iter->eType == IDENTIFIER_NAME))
                && (0 == strcasecmp(iter->sValue.c_str(), "Q")))
        {
            outputText("bjt", NULL, nOption++);
        }
        else if (((iter->eType == SECONDARY_KEYWORD) || (iter->eType == IDENTIFIER_NAME))
                && (0 == strcasecmp(iter->sValue.c_str(), "R")))
        {
            outputText("res", NULL, nOption++);
        }
        else if (((iter->eType == SECONDARY_KEYWORD) || (iter->eType == IDENTIFIER_NAME))
                && (0 == strcasecmp(iter->sValue.c_str(), "SUBCKT")))
        {
            outputText("subckt", NULL, nOption++);
        }
        else if (((iter->eType == SECONDARY_KEYWORD) || (iter->eType == IDENTIFIER_NAME))
                && (0 == strcasecmp(iter->sValue.c_str(), "X")))
        {
            outputText("x_instance", NULL, nOption++);
        }
        else if (((iter->eType == SECONDARY_KEYWORD) || (iter->eType == IDENTIFIER_NAME))
                && (0 == strcasecmp(iter->sValue.c_str(), "V")))
        {
            outputText("voltage", NULL, nOption++);
        }
        else if (((iter->eType == SECONDARY_KEYWORD) || (iter->eType == IDENTIFIER_NAME))
                && (0 == strcasecmp(iter->sValue.c_str(), "PARAM")))
        {
            outputText("param", NULL, nOption++);
        }
        else if (((iter->eType == SECONDARY_KEYWORD) || (iter->eType == IDENTIFIER_NAME))
                && (0 == strcasecmp(iter->sValue.c_str(), "OPTIONS")))
        {
            outputText("option", NULL, nOption++);
        }
        else
        {
            outputText(NULL, iter->sValue.c_str(), nOption++);
        }
        ++iter;
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convSconnectCheckSpec(rcsSynSpecificationNode_T *pNode)
{
    if (_hasOnlyDirectKeyword(pNode))
    {
        outputCon1Error(pNode);
        return;
    }
    m_sCurCommandString = "LVS SOFTCHK";
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "uniconnect_check " : "uniconnect_check ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if ( (iter->eType != IDENTIFIER_NAME) || !parseTmpLayerForOperation(iter->sValue) )
    {
        std::string sLayerName;
        if (getLayerRefName(iter, pNode->end(), sLayerName))
            outputText(NULL, sLayerName.c_str(), nOption++);
    }
    while(iter != pNode->end())
    {
        std::string sValue = "";
        if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == LOWER)
        {
            sValue = "lower_layer";
        }
        else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == CONTACT)
        {
            sValue = "via_layer";
        }
        else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == UPPER)
        {
            sValue = "upper_layer";
        }
        else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == ALL)
        {
            sValue = "all_layer";
        }
        else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == DIRECT)
        {
            ++iter;
            continue;
        }
        else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == MASK)
        {
            ++iter;
            continue;
        }
        else
        {
            outputCon2Error(pNode,*iter);
            ++iter;
            continue;
        }
        outputText(NULL, sValue.c_str(), nOption++);
        ++iter;
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convSoftChkMaxResSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    switch (pNode->getSpecificationType())
    {
        case LVS_SOFTCHK_MAXIMUM_RESULTS:
            (*m_oPVRSStream) << (_isTvf2Trs ? "uniconnect_check_max_result " : "uniconnect_check_max_result ( ");
            break;
        case VIRTUAL_CONNECT_REPORT_MAXIMUM:
            (*m_oPVRSStream) << (_isTvf2Trs ? "virtual_connect report_max " : "virtual_connect ( report_max ");
            break;
        default:
            return;
    }
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    iter++;
    if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == ALL)
    {
        (*m_oPVRSStream) << "all ";
    }
    else
    {
        std::string sValue = "";
        getNumberExp(iter,pNode->end(),sValue);
        (*m_oPVRSStream) << sValue << " ";
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convUnitOptionSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    if (pNode->getSpecificationType() == UNIT_CAPACITANCE)
        (*m_oPVRSStream) << (_isTvf2Trs ? "unit cap " : "unit ( cap ");
    else if (pNode->getSpecificationType() == UNIT_INDUCTANCE)
        (*m_oPVRSStream) << (_isTvf2Trs ? "unit ind " : "unit ( ind ");
    else if (pNode->getSpecificationType() == UNIT_LENGTH)
        (*m_oPVRSStream) << (_isTvf2Trs ? "unit len " : "unit ( len ");
    else if (pNode->getSpecificationType() == UNIT_RESISTANCE)
        (*m_oPVRSStream) << (_isTvf2Trs ? "unit res " : "unit ( res ");
    else if (pNode->getSpecificationType() == UNIT_TIME)
        (*m_oPVRSStream) << (_isTvf2Trs ? "unit time " : "unit ( time ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    if (iter == pNode->end())
    {
        --iter;
        throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2, iter->nLineNo, iter->sValue);
    }

    std::string sValue;
    if (getStringName(iter, pNode->end(), sValue))
    {
        outputText(NULL, sValue.c_str());
    }
    else if (getNumberExp(iter, pNode->end(), sValue))
    {
        outputText(NULL, sValue.c_str());
    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}



void
rcsSynPvrsConvertor::convCustomizedSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << (_isTvf2Trs ? "erc_find_open " : "erc_find_open ( ");
    ++iter;
    if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD)
    {
        if (iter->eKeyType == YES)
        {
            outputText(NULL, "yes", nOption++);
            ++iter;
        }
        else if (iter->eKeyType == NO)
        {
            outputText(NULL, "no", nOption++);
            ++iter;
            if (iter != pNode->end())
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
        }
        else
        {
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
        }
    }
    std::string sNameValue;
    while(iter != pNode->end())
    {
        switch(iter->eKeyType)
        {
            case CELL:
            {
                ++iter;
                if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                   (iter->eKeyType == ALL || iter->eKeyType == PRIMARY))
                {
                    if (iter->eKeyType == ALL)
                        outputText(NULL, "all_cell", nOption++);
                    else
                        outputText(NULL, "top_cell", nOption++);
                    ++iter;
                }
                else
                {
                    --iter;
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
                continue;
            }
            case MAX_SECOND:
            {
                ++iter;
                std::string sNumExp;
                if (iter != pNode->end() && getNumberExp(iter, pNode->end(), sNumExp))
                {
                    outputText("max", sNumExp.c_str(), nOption);
                }
                continue;
            }
            case MAX_PER_OPEN:
            {
                ++iter;
                std::string sNumExp;
                if (iter != pNode->end() && getNumberExp(iter, pNode->end(), sNumExp))
                {
                    outputText("max_nets_per_open", sNumExp.c_str(), nOption);
                }
                continue;
            }
            case NAME:
            case IGNORE_NAME:
            {
                std::string sName;
                if (iter->eKeyType == NAME)
                {
                    sName = "among_net ";
                }
                else
                {
                    sName = "ignore_net ";
                }
                ++iter;
                while(iter != pNode->end())
                {
                    if (iter->eType != SECONDARY_KEYWORD || (iter->eKeyType != CELL &&
                       iter->eKeyType != MAX_SECOND && iter->eKeyType != MAX_PER_OPEN))
                    {
                        std::string sValue;
                        if (getStringName(iter, pNode->end(), sValue))
                        {
                            sName += sValue;
                            sName += " ";
                        }
                    }
                    else
                        break;
                    ++iter;
                }
                outputText(sNameValue.c_str(), sName.c_str(), nOption++);
                break;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                    iter->nLineNo, iter->sValue);
        }


    }
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}

std::string  rcsSynPvrsConvertor::dealVariable(std::string sScript)
{
    std::set<string> setVariable;
    std::map<string,string> mapVariable;
    std::stringstream ssScript(sScript);
    std::string sLineBuf;
    std::string sNewScript;
    while(std::getline(ssScript, sLineBuf))
    {
        string strLine = sLineBuf;
        trim(strLine);
        if(strLine[0] =='#')
        {
            sNewScript +='\n';
            sNewScript +=sLineBuf;
            continue;
        }

        string::size_type pos = sLineBuf.find("tvf::GET_LAYER_ARGS ");
        if(string::npos != pos)
        {
            string::size_type nStart = pos + sizeof("tvf::GET_LAYER_ARGS")-1;
            std::string strVariables = sLineBuf.substr(nStart);
            std::stringstream ssArgs(strVariables);
            std::string strVariable;
            while(ssArgs >> strVariable)
            {
                std::string sTmp = strVariable;
                while(!isValidSvrfName(strVariable))
                {
                    strVariable = strVariable+"_";
                    if(setVariable.count(strVariable)==0)
                    {
                        mapVariable[sTmp] = strVariable;
                        nStart = sLineBuf.find(sTmp,nStart+1);
                        sLineBuf = sLineBuf.replace(nStart,sTmp.size(),strVariable);
                        nStart = nStart+strVariable.size();
                        break;
                    }
                }
                setVariable.insert(strVariable);
            }
        }
        pos = sLineBuf.find("set ");
        if(string::npos != pos)
        {
            string::size_type nStart = pos + sizeof("set")-1;
            std::string strVariables = sLineBuf.substr(nStart);
            std::stringstream ssArgs(strVariables);
            std::string strVariable;
            if(ssArgs >> strVariable)
            {
                std::string sTmp = strVariable;
                while(!isValidSvrfName(strVariable))
                {
                    strVariable = strVariable+"_";
                    if(setVariable.count(strVariable)==0)
                    {
                        mapVariable[sTmp] = strVariable;
                        nStart = sLineBuf.find(sTmp,nStart+1);
                        sLineBuf = sLineBuf.replace(nStart,sTmp.size(),strVariable);
                        nStart = nStart+strVariable.size();
                        break;
                    }
                }
                setVariable.insert(strVariable);
            }
        }
        pos = sLineBuf.find("global ");
        if(string::npos != pos)
        {
            string::size_type nStart = pos + sizeof("global")-1;
            std::string strVariables = sLineBuf.substr(nStart);
            std::stringstream ssArgs(strVariables);
            std::string strVariable;
            if(ssArgs >> strVariable)
            {
                std::string sTmp = strVariable;
                while(!isValidSvrfName(strVariable))
                {
                    strVariable = strVariable+"_";
                    if(setVariable.count(strVariable)==0)
                    {
                        mapVariable[sTmp] = strVariable;
                        nStart = sLineBuf.find(sTmp,nStart+1);
                        sLineBuf = sLineBuf.replace(nStart,sTmp.size(),strVariable);
                        nStart = nStart+strVariable.size();
                        break;
                    }
                }
                setVariable.insert(strVariable);
            }
        }
        pos = sLineBuf.find("tvf::SETLAYER ");
        if(string::npos != pos)
        {
            string::size_type nStart = pos + sizeof("tvf::SETLAYER")-1;
            std::string strVariables = sLineBuf.substr(nStart);
            std::stringstream ssArgs(strVariables);
            std::string strVariable;
            if(ssArgs >> strVariable)
            {
                std::string sTmp = strVariable;
                while(!isValidSvrfName(strVariable))
                {
                    strVariable = strVariable+"_";
                    if(setVariable.count(strVariable)==0)
                    {
                        mapVariable[sTmp] = strVariable;
                        nStart = sLineBuf.find(sTmp,nStart+1);
                        sLineBuf = sLineBuf.replace(nStart,sTmp.size(),strVariable);
                        nStart = nStart+strVariable.size();
                        break;
                    }
                }
                setVariable.insert(strVariable);
            }
        }

        sNewScript +='\n';
        sNewScript +=sLineBuf;
    }



    std::map<string,string>::iterator iter = mapVariable.begin();
    while(iter!=mapVariable.end())
    {
        std::string strOrg = "$"+iter->first;
        std::string strDest = "$"+iter->second;
        string::size_type pos = sNewScript.find(strOrg);
        while(pos!=string::npos)
        {
            sNewScript = sNewScript.replace(pos,strOrg.size(),strDest);
            pos = sNewScript.find(strOrg,pos+strDest.size()+1);
        }

        iter++;
    }

    return sNewScript;

}
void
rcsSynPvrsConvertor::convTvfFuncSpec(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    rcsManager_T::getInstance()->setTvfFunction(true);
    std::string sCmd = (_isTvf2Trs ? "TRS_FUNCTION " : "TRS_FUNCTION ( ");
    std::string sTvfName;
    std::string sOpt;
    bool bHasOpt = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();

    ++iter;
    if (iter->eType == SECONDARY_KEYWORD && (iter->eKeyType == EDGE || iter->eKeyType == ERROR))
    {
        bHasOpt = true;
        if (iter->eKeyType == EDGE)
        {
            sOpt = "edge";
        }
        else
        {
            sOpt = "error";
        }
        ++iter;
    }
    if (iter == pNode->end())
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
    }
    sTvfName = iter->sValue;
    (*m_oPVRSStream) << sCmd << sTvfName << " " << (bHasOpt ? sOpt : "") << " ";
    ++iter;



    rcsSvrf2Pvrs::setCurLineNo(iter->nLineNo);

    while (iter != pNode->end())
    {
        std::string script = iter->sValue;
        std::string namescopes = iter->namescopes[0];
        rcsSvrf2Pvrs::setCompleteStr(namescopes);
        rcsSvrf2Pvrs::setStrSearchBegin();

        trim(script);
        script.erase(0, 1);
        script.erase(script.find_last_of("]"), 1);
        trim(script);
        if(script.find_first_of("/*") == 0)
        {
            Utassert(script.find_last_of("/") == script.length() - 1 &&
                     script.find_last_of("*") == script.length() - 2);
            script.erase(0,2);
            script.erase(script.find_last_of("*"), 2);
        }

        trim(script);

        (*m_oPVRSStream) << "[" << std::endl << "/*" << std::endl;
        (*m_oPVRSStream) << "\t";
        bool isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
        rcsManager_T::getInstance()->setTvf2Trs(true);
        rcsTvfConvertor convertor("");
        string sNewScript = dealVariable(script);
        convertor.execute(sNewScript.c_str());

        rcsManager_T::getInstance()->setTvf2Trs(isTvf2Trs);

        (*m_oPVRSStream) << std::endl << "*/" << std::endl << "]";
        ++iter;
    }

    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }

    rcsManager_T::getInstance()->setTvfFunction(false);
}

void
rcsSynPvrsConvertor::convOldIsolateNetSpec(rcsSynSpecificationNode_T *pNode)
{
    std::string sCmd;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    switch (pNode->getSpecificationType())
    {
        case LVS_ISOLATE_SHORTS_IGNORE_NETS:
            sCmd = (_isTvf2Trs ? "erc_find_shorts_ignore_nets " : "erc_find_shorts_ignore_nets ( ");
            break;
        case LVS_ISOLATE_OPENS_NETS:
            sCmd = (_isTvf2Trs ? "erc_find_open_nets " : "erc_find_open_nets ( ");
            break;
        case LVS_ISOLATE_OPENS_IGNORE_NETS:
            sCmd = (_isTvf2Trs ? "erc_find_open_ignore_nets " : "erc_find_open_ignore_nets ( ");
            break;
        default:
            outputCon1Error(pNode);
            return;
    }
    ++iter;
    while (iter != pNode->end())
    {
        sCmd += iter->sValue;
        sCmd += " ";
        ++iter;
    }

    (*m_oPVRSStream) << sCmd ;
    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
    {
        (*m_oPVRSStream) << std::endl;
    }
}


static bool
isDimensionalCheckKeyword(const std::list<rcsToken_T>::iterator iter,
                          const std::list<rcsToken_T>::iterator end)
{
    if (iter == end || iter->eType == LAYER_OPERATION_KEYWORD)
    {
        return true;
    }
    else if (iter->eType == SECONDARY_KEYWORD)
    {
        switch(iter->eKeyType)
        {
            case SQUARE:
            case SQUARE_ORTHOGONAL:
            case OPPOSITE:
            case OPPOSITE1:
            case OPPOSITE2:
            case OPPOSITE_EXTENDED:
            case OPPOSITE_EXTENDED1:
            case OPPOSITE_EXTENDED2:
            case OPPOSITE_SYMMETRIC:
            case OPPOSITE_EXTENDED_SYMMETRIC:
            case OPPOSITE_FSYMMETRIC:
            case OPPOSITE_EXTENDED_FSYMMETRIC:
            case ACUTE_ALSO:
            case ACUTE_ONLY:
            case NOT_ACUTE:
            case PARALLEL_ALSO:
            case PARALLEL_ONLY:
            case NOT_PARALLEL:
            case PERPENDICULAR_ALSO:
            case PERPENDICULAR_ONLY:
            case NOT_PERPENDICULAR:
            case NOT_OBTUSE:
            case OBTUSE_ONLY:
            case OBTUSE_ALSO:
            case PROJECTING:
            case NOT_PROJECTING:
            case ANGLED:
            case CORNER_TO_CORNER:
            case CORNER_TO_EDGE:
            case NOT_CORNER:
            case CORNER:
            case REGION:
            case REGION_EXTENTS:
            case REGION_CENTERLINE:
            case MEASURE_ALL:
            case MEASURE_COINCIDENT:
            case NOTCH:
            case SPACE:
            case CONNECTED:
            case NOT_CONNECTED:
            case ABUT:
            case OVERLAP:
            case SINGULAR:
            case INSIDE_ALSO:
            case OUTSIDE_ALSO:
            case EXCLUDE_FALSE:
            case EXCLUDE_SHIELDED:
            case BY_LAYER:
            case COUNT:
            case INTERSECTING_ONLY:
                return true;
            default:
            {
                if (!isValidSvrfName(iter->sValue))
                {
                    return true;
                }
                break;
            }
        }
    }
    else if (iter->eType == IDENTIFIER_NAME && !isValidSvrfName(iter->sValue))
    {
        return true;
    }
    return false;
}


bool
rcsSynPvrsConvertor::_hasOnlyDirectKeyword(rcsSynNode_T *pNode)
{
    bool hasDirect = false;
    bool hasMask   = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
       if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == DIRECT)
           hasDirect = true;
       else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == MASK)
           hasMask = true;

        ++iter;
    }

    if (hasDirect && !hasMask)
        return true;

    return false;
}

bool
rcsSynPvrsConvertor::getConstraint(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end, std::string &sConstraint)
{
    if (isConstraintOperator(*iter))
    {
        sConstraint += iter->sValue;
        sConstraint += " ";

        ++iter;
        if (iter == end)
            return false;

        std::string sNumExp;
        if (!getNumberExp(iter, end, sNumExp))
            return false;
        sConstraint += sNumExp;
    }
    else
    {
        return false;
    }

    if (iter != end && isConstraintOperator(*iter))
    {
        sConstraint += " ";
        sConstraint += iter->sValue;
        sConstraint += " ";

        ++iter;
        if (iter == end)
            return false;
        std::string sNumExp;
        if (!getNumberExp(iter, end, sNumExp))
            return false;
        sConstraint += sNumExp;
    }
    return true;
}

bool
rcsSynPvrsConvertor::outputBuiltInLangInTrs(const rcsToken_T &token, hvUInt32 &nOption)
{
    if(!rcsManager_T::getInstance()->isTvf2Trs())
        return false;

    std::string sValue = token.sValue;
	std::list<rcsToken_T> __BILTokens;
	std::set<hvUInt32> vUnstoredCommentLineNumbers;
	rcsLexParser_T _lexparser(__BILTokens, vUnstoredCommentLineNumbers);
	_lexparser.parse(NULL, sValue.c_str());
	sValue.clear();
	for(auto& _elem : __BILTokens)
	{
		switch(_elem.eType)
		{
		case STRING_CONSTANTS:
		case IDENTIFIER_NAME:
		{
			_elem.sValue = this->getRealLayerName(token.namescopes, _elem.sValue);
			if(token.namescopes.size() > 1)
			{
				if( (_elem.sValue.find("::", 0) != std::string::npos) &&
					(_elem.sValue.find("<", 0) != std::string::npos) &&
					(_elem.sValue.find(">", 0) != std::string::npos) )
				{
					_elem.sValue.insert(0, "\"");
					_elem.sValue.push_back('\"');
				}
			}
			if(_elem.eType == STRING_CONSTANTS)
			{
				if( !_elem.sValue.empty() && (_elem.sValue[0] != '\"') && (_elem.sValue.back() != '\"') )
				{
					_elem.sValue.insert(0, "\"");
				    _elem.sValue.push_back('\"');
				}
			}
			_elem.eType = STRING_CONSTANTS;
			break;
		}
		default:
			break;
		}

		sValue += _elem.sValue;
	}
    if (sValue[0] == '[')
    {
        sValue.erase(0, 1);
        sValue.insert(0, "\\[ ");
    }
    if (sValue[sValue.size() - 1] == ']')
    {
        sValue.erase(sValue.size() - 1);
        sValue.insert(sValue.size(), " \\]");
    }
    std::string::size_type index = 0;
    while(index != sValue.size())
    {
        if(sValue[index] == '\n' || sValue[index] == '\r')
        {
            sValue.insert(index, "\\");
            index += 2;
        }
        else
        {
            ++index;
        }
    }
    outputText(NULL, sValue.c_str(), nOption++);

    return true;
}

void
rcsSynPvrsConvertor::outputBuiltInLang(KEY_TYPE eType, const rcsToken_T &token, hvUInt32 &nOption)
{
    if(outputBuiltInLangInTrs(token, nOption)) return;

    std::string sValue = token.sValue;
	std::list<rcsToken_T> __BILTokens;
	std::set<hvUInt32> vUnstoredCommentLineNumbers;
	rcsLexParser_T _lexparser(__BILTokens, vUnstoredCommentLineNumbers);
	_lexparser.parse(NULL, sValue.c_str());
	sValue.clear();
	for(auto& _elem : __BILTokens)
	{
		switch(_elem.eType)
		{
		case STRING_CONSTANTS:
		case IDENTIFIER_NAME:
		{
			_elem.sValue = this->getRealLayerName(token.namescopes, _elem.sValue);
			if(token.namescopes.size() > 1)
			{
				if( (_elem.sValue.find("::", 0) != std::string::npos) &&
					(_elem.sValue.find("<", 0) != std::string::npos) &&
					(_elem.sValue.find(">", 0) != std::string::npos) )
				{
					_elem.sValue.insert(0, "\"");
					_elem.sValue.push_back('\"');
				}
			}

			if(_elem.eType == STRING_CONSTANTS)
			{
				if( !_elem.sValue.empty() && (_elem.sValue[0] != '\"') && (_elem.sValue.back() != '\"') )
				{
					_elem.sValue.insert(0, "\"");
				    _elem.sValue.push_back('\"');
				}
			}
			_elem.eType = STRING_CONSTANTS;
			break;
		}
		default:
			break;
		}

		sValue += _elem.sValue;
	}

    bool hasLeftBracket = false;
    bool hasRightBracket = false;
    if (sValue[0] == '[')
    {
        sValue.erase(0, 1);
        hasLeftBracket = true;
    }
    if (sValue[sValue.size() - 1] == ']')
    {
        sValue.erase(sValue.size() - 1);
        hasRightBracket = true;
    }
    switch (eType)
    {
        case DFM_SPACE:
        case TDDRC:
        case ENCLOSURE:
        case EXTERNAL:
        case INTERNAL:
        {
            trim(sValue);
            bool hasLeftQuotes = false;
            bool hasRightQuotes = false;
            if(sValue[0] == '\"' || sValue[0] == '\'')
            {
                hasLeftQuotes = true;
                trimBegin(sValue, "\"\'");
            }
            if (sValue[sValue.size() - 1] == '\"' || sValue[sValue.size() - 1] == '\'')
            {
                hasRightQuotes = true;
                trimEnd(sValue, "\"\'");
            }
            if (hasLeftBracket) sValue.insert(0, "[");
            if (hasLeftQuotes) sValue.insert(0, "\"");
            if (hasRightQuotes) sValue.push_back('\"');
            if (hasRightBracket) sValue.push_back(']');
            outputText(NULL, sValue.c_str(), nOption++);
            break;
        }
        default:
        {
            if (hasLeftBracket) sValue.insert(0, "[ ");
            if (hasRightBracket) sValue.insert(sValue.size(), " ]");
            outputText(NULL, sValue.c_str(), nOption++);
        }
    }
}

bool
rcsSynPvrsConvertor::getConstraintNoExpr(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end, std::string &sConstraint)
{
    if (isConstraintOperator(*iter))
    {
        sConstraint += iter->sValue;
        sConstraint += " ";

        ++iter;
        if (iter == end)
            return false;

        std::string sNumExp;
        if (!getNumberExp(iter, end, sNumExp,false,false))
            return false;
        sConstraint += sNumExp;
    }
    else
    {
        return false;
    }

    if (iter != end && isConstraintOperator(*iter))
    {
        sConstraint += " ";
        sConstraint += iter->sValue;
        sConstraint += " ";

        ++iter;
        if (iter == end)
            return false;
        std::string sNumExp;
        if (!getNumberExp(iter, end, sNumExp,false,false))
            return false;
        sConstraint += sNumExp;
    }
    return true;
}

static std::string
getAllName(const std::vector<std::string> &scope, const std::string &name)
{
    std::ostringstream oss;
    std::copy(scope.begin(), scope.end(), std::ostream_iterator<std::string>(oss, "::"));
    oss << name;
    return oss.str();
}

static std::string
getAllName(std::vector<std::string>::const_iterator first,
           std::vector<std::string>::const_iterator last, const std::string &name)
{
    std::ostringstream oss;
    std::copy(first, last, std::ostream_iterator<std::string>(oss, "::"));
    oss << name;
    return oss.str();
}

std::string
rcsSynPvrsConvertor::getRealLayerName(const std::vector<std::string> &namescopes, const std::string &name)
{
    std::string sLayerName;

    std::vector<std::string>::const_iterator iterScope = namescopes.end();
    while(iterScope != namescopes.begin())
    {
        sLayerName = getAllName(namescopes.begin(), iterScope, name);
        std::string sLowerName = sLayerName;
        toLower(sLowerName);
        if(m_vDefLayers.find(sLowerName) != m_vDefLayers.end())
        {
            break;
        }
        sLayerName.clear();
        --iterScope;
    }

    return sLayerName.empty() ? name : sLayerName;
}

bool
rcsSynPvrsConvertor::getLayerRefName(std::list<rcsToken_T>::iterator &iter,
                                     std::list<rcsToken_T>::iterator end,
                                     std::string &sLayerName)
{
    switch(iter->eType)
    {
        case SECONDARY_KEYWORD:
        case IDENTIFIER_NAME:
        case STRING_CONSTANTS:
        {
            bool isTmpLayer = rcsManager_T::getInstance()->isTmpLayer(iter->sValue);
            if (!isTmpLayer && !iter->namescopes.empty())
            {
                iter->sValue = getRealLayerName(iter->namescopes, iter->sValue);
                iter->eType = STRING_CONSTANTS;
            }
            getStringName(iter, end, sLayerName);

            if(isTmpLayer)
            {
                if(!rcsManager_T::getInstance()->needExpandTmpLayer())
                {
                    Utassert(m_vTmpLayerValues.find(sLayerName) != m_vTmpLayerValues.end());
                    std::map<std::string, std::string>::iterator iterValue =
                            m_vTmpLayerValues.find(sLayerName);

                    if(iterValue != m_vTmpLayerValues.end())
                    {
                        Utassert(!iterValue->second.empty());
                        sLayerName = " ";
                        sLayerName += iterValue->second;
                        sLayerName += " ";
                    }
                }

                if(m_iProcLevel > 0)
                {
                    Utassert(false);
                    sLayerName.insert(0, "$");
                }
            }
            ++iter;
            break;
        }
        case OPERATOR:
        {
            if (iter->sValue != "+")
            {
                return false;
            }
            else
            {
                hvUInt32 nLineNo = iter->nLineNo;
                if (++iter == end || iter->eType != NUMBER || iter->nLineNo != nLineNo)
                {
                    --iter;
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
            }
        }
        case NUMBER:
        {
            sLayerName = iter->sValue;
            ++iter;
            break;
        }
        default:
            return false;
    }
    return true;
}


bool
rcsSynPvrsConvertor::getLayerRefName_addScopedLayerMap(std::list<rcsToken_T>::iterator &iter,
                                     std::list<rcsToken_T>::iterator end,
                                     std::string &sLayerName, _rcsBuildinLayerReplacer &scopedMap)
{
    switch(iter->eType)
    {
        case SECONDARY_KEYWORD:
        case IDENTIFIER_NAME:
        case STRING_CONSTANTS:
        {
            bool isTmpLayer = rcsManager_T::getInstance()->isTmpLayer(iter->sValue);
            if (!isTmpLayer && !iter->namescopes.empty())
            {
                std::string origName = iter->sValue;
                iter->sValue = getRealLayerName(iter->namescopes, iter->sValue);
                iter->eType = STRING_CONSTANTS;
                scopedMap.insert(origName, iter->sValue);
            }
            getStringName(iter, end, sLayerName);
            if(isTmpLayer)
            {
                if(!rcsManager_T::getInstance()->needExpandTmpLayer())
                {
                    Utassert(m_vTmpLayerValues.find(sLayerName) != m_vTmpLayerValues.end());
                    std::map<std::string, std::string>::iterator iterValue =
                            m_vTmpLayerValues.find(sLayerName);

                    if(iterValue != m_vTmpLayerValues.end())
                    {
                        Utassert(!iterValue->second.empty());
                        sLayerName = " ";
                        sLayerName += iterValue->second;
                        sLayerName += " ";
                    }
                }

                if(m_iProcLevel > 0)
                {
                    Utassert(false);
                    sLayerName.insert(0, "$");
                }
            }
            ++iter;
            break;
        }
        case OPERATOR:
        {
            if (iter->sValue != "+")
            {
                return false;
            }
            else
            {
                hvUInt32 nLineNo = iter->nLineNo;
                if (++iter == end || iter->eType != NUMBER || iter->nLineNo != nLineNo)
                {
                    --iter;
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
            }
        }
        case NUMBER:
        {
            sLayerName = iter->sValue;
            ++iter;
            break;
        }
        default:
            return false;
    }
    return true;
}


bool
rcsSynPvrsConvertor::getLayerName(std::list<rcsToken_T>::iterator &iter,
                                  std::list<rcsToken_T>::iterator end,
                                  std::string &sLayerName)
{
    switch(iter->eType)
    {
        case SECONDARY_KEYWORD:
        case IDENTIFIER_NAME:
        case STRING_CONSTANTS:
        {
            bool isTmpLayer = rcsManager_T::getInstance()->isTmpLayer(iter->sValue);
            if (!isTmpLayer && !iter->namescopes.empty())
            {
                iter->sValue = getAllName(iter->namescopes, iter->sValue);
                iter->eType = STRING_CONSTANTS;

                std::string sLowerName = iter->sValue;
                toLower(sLowerName);
                m_vDefLayers.insert(sLowerName);
            }
            getStringName(iter, end, sLayerName);

            if(isTmpLayer)
            {
                if(m_iProcLevel > 0)
                {
                    Utassert(false);
                    sLayerName.insert(0, "$");
                }
            }
            ++iter;
            break;
        }
        case OPERATOR:
        {
            if (iter->sValue != "+")
            {
                return false;
            }
            else
            {
                hvUInt32 nLineNo = iter->nLineNo;
                if (++iter == end || iter->eType != NUMBER || iter->nLineNo != nLineNo)
                {
                    --iter;
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
            }
        }
        case NUMBER:
        {
            sLayerName = iter->sValue;
            ++iter;
            break;
        }
        default:
            return false;
    }
    return true;
}

void
rcsSynPvrsConvertor::convCheckSpaceCmd(rcsSynLayerOperationNode_T *pNode)
{
    if (pNode->getOperationType() == INTERNAL)
        m_sCurCommandString = "INTERNAL";
    else if (pNode->getOperationType() == EXTERNAL)
        m_sCurCommandString = "EXTERNAL";
    else if (pNode->getOperationType() == ENCLOSURE)
        m_sCurCommandString = "ENCLOSURE";
    hvUInt32 nOption = 0;
    if (pNode->getOperationType() == INTERNAL)
    {
        outputText(NULL, "width ( ", nOption++);
    }
    else if (pNode->getOperationType() == EXTERNAL)
    {
        outputText(NULL, "space ( ", nOption++);
    }
    else if (pNode->getOperationType() == ENCLOSURE)
    {
        outputText(NULL, "extension ( ", nOption++);
    }
    else
    {
        return;
    }
    bool bIntersectOnly = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case SQUARE:
                    outputText(NULL, "square", nOption++);
                    ++iter;
                    continue;
                case SQUARE_ORTHOGONAL:
                    outputText(NULL, "square_orthogonal", nOption++);

                    ++iter;
                    continue;
                case OPPOSITE:
                    outputText(NULL, "opposite", nOption++);
                    ++iter;
                    continue;
                case OPPOSITE1:
                    outputText(NULL, "opposite1", nOption++);
                    ++iter;
                    continue;
                case OPPOSITE2:
                    outputText(NULL, "opposite2", nOption++);
                    ++iter;
                    continue;
                case OPPOSITE_SYMMETRIC:
                    outputText(NULL, "opp_sym", nOption++);
                    ++iter;
                    continue;
                case OPPOSITE_FSYMMETRIC:
                    outputText(NULL, "opp_fsym", nOption++);
                    ++iter;
                    continue;
                case OPPOSITE_EXTENDED:
                {
                    outputText(NULL, "extended_opposite", nOption++);
                    ++iter;
                    if (iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if (getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            outputText(NULL, sNumExp.c_str(), nOption);
                        }
                    }
                    continue;
                }
                case OPPOSITE_EXTENDED1:
                {
                    outputCon2Error(pNode,*iter);
                    ++iter;
                    if (iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if (getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            ;
                        }
                    }
                    continue;
                }
                case OPPOSITE_EXTENDED2:
                {
                    outputCon2Error(pNode,*iter);
                    ++iter;
                    if (iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if (getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            ;
                        }
                    }
                    continue;
                }
                case OPPOSITE_EXTENDED_SYMMETRIC:
                {
                    outputText(NULL, "opp_sym_extended", nOption++);
                    ++iter;
                    if (iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if (getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            outputText(NULL, sNumExp.c_str(), nOption);
                        }
                    }
                    continue;
                }
                case OPPOSITE_EXTENDED_FSYMMETRIC:
                {
                    outputText(NULL, "opp_fsym_extended", nOption++);
                    ++iter;
                    if (iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if (getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            outputText(NULL, sNumExp.c_str(), nOption);
                        }
                    }
                    continue;
                }
                case ACUTE_ALSO:
                case ACUTE_ONLY:
                case NOT_ACUTE:
                case PARALLEL_ALSO:
                case PARALLEL_ONLY:
                case NOT_PARALLEL:
                case PERPENDICULAR_ALSO:
                case PERPENDICULAR_ONLY:
                case NOT_PERPENDICULAR:
                case NOT_OBTUSE:
                case OBTUSE_ONLY:
                case OBTUSE_ALSO:
                    ++iter;
                    continue;
                case PROJECTING:
                {
                    ++iter;
                    if (iter != pNode->end() && isConstraintOperator(*iter))
                    {
                        std::string sCons;
                        if (getConstraint(iter, pNode->end(), sCons))
                        {
                            continue;
                        }
                    }
                    continue;
                }
                case NOT_PROJECTING:
                    ++iter;
                    continue;
                case ANGLED:
                {
                    ++iter;
                    if (iter != pNode->end() && isConstraintOperator(*iter))
                    {
                        std::string sCons;
                        if (getConstraint(iter, pNode->end(), sCons))
                        {
                            continue;
                        }
                    }
                    continue;
                }
                case CORNER_TO_CORNER:
                {
                    ++iter;
                    if (iter != pNode->end() && isConstraintOperator(*iter))
                    {
                        std::list<rcsToken_T>::iterator begin = iter;
                        std::string sCons;
                        if (getConstraint(iter, pNode->end(), sCons))
                        {
                            continue;
                        }
                    }
                    continue;
                }
                case CORNER_TO_EDGE:
                    ++iter;
                    continue;
                case NOT_CORNER:
                    ++iter;
                    continue;
                case CORNER:
                    ++iter;
                    continue;
                case REGION:
                    ++iter;
                    continue;
                case REGION_EXTENTS:
                    ++iter;
                    continue;
                case REGION_CENTERLINE:
                {
                    ++iter;
                    if (iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if (getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            continue;
                        }
                    }
                    continue;
                }
                case MEASURE_ALL:
                    ++iter;
                    continue;
                case MEASURE_COINCIDENT:
                    ++iter;
                    continue;
                case NOTCH:
                    ++iter;
                    continue;
                case SPACE:
                    ++iter;
                    continue;
                case CONNECTED:
                    ++iter;
                    continue;
                case NOT_CONNECTED:
                    ++iter;
                    continue;
                case ABUT:
                {
                    ++iter;
                    if (iter != pNode->end() && isConstraintOperator(*iter))
                    {
                        std::string sCons;
                        if (getConstraint(iter, pNode->end(), sCons))
                        {
                            continue;
                        }
                    }
                    continue;
                }
                case OVERLAP:
                    ++iter;
                    continue;
                case SINGULAR:
                    ++iter;
                    continue;
                case INSIDE_ALSO:
                    ++iter;
                    continue;
                case OUTSIDE_ALSO:
                    ++iter;
                    continue;
                case EXCLUDE_FALSE:
                case EXCLUDE_FALSE_NOTCH:
                    ++iter;
                    continue;
                case EXCLUDE_SHIELDED:
                {
                    if (!isDimensionalCheckKeyword(++iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if (getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            continue;
                        }
                    }
                    continue;
                }
                case BY_LAYER:
                {
                    ++iter;
                    if (iter != pNode->end())
                        ++iter;

                    continue;
                }
                case COUNT:
                {
                    std::string sNumExp;
                    if (getNumberExp(++iter, pNode->end(), sNumExp))
                    {
                        continue;
                    }
                    continue;
                }
                case INTERSECTING_ONLY:
                    bIntersectOnly = true;
                    ++iter;
                    continue;
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }

                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
                else if (iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    std::string::size_type nPos = sConstraint.find("\\$");
                    while(nPos != std::string::npos)
                    {
                    	if(nPos > 0)
                    		sConstraint.erase(nPos - 1, 1);
                    	nPos = sConstraint.find("\\$", nPos + 1);
                    }
                    outputText(NULL, sConstraint.c_str(), nOption++);
                    continue;
                }
            }
            case BUILT_IN_LANG:
            {
#if 0
                if(!outputBuiltInLangInTrs(*iter, nOption))
                {
                    std::string sValue = iter->sValue;
                    trim(sValue);
                    sValue.erase(0, 1);
                    sValue.erase(sValue.size() - 1);
                    trim(sValue);
                    if(sValue[0] == '\"' || sValue[0] == '\'')
                    {
                        trim(sValue, "\"\'");
                        sValue.insert(0, "[\"");
                        sValue.insert(sValue.size(), "\"]");
                    }
                    else
                    {
                        sValue.insert(0, "[");
                        sValue.insert(sValue.size(), "]");
                    }
                    outputText(NULL, sValue.c_str(), nOption++);
                }
#else
               //outputBuiltInLang(pNode->getOperationType(), *iter, nOption);
                std::string sValue = iter->sValue;
                bool hasLeftBracket = false;
                bool hasRightBracket = false;
                if (sValue[0] == '[')
                {
                    sValue.erase(0, 1);
                    hasLeftBracket = true;
                }
                if (sValue[sValue.size() - 1] == ']')
                {
                    sValue.erase(sValue.size() - 1);
                    hasRightBracket = true;
                }

                trim(sValue);
                std::string sLayerName = getRealLayerName(iter->namescopes, sValue);
    			if(sLayerName.find("::", 0) != std::string::npos)
    			{
    				sLayerName.insert(0, "\"");
    				sLayerName.push_back('\"');
    			}
                /*sLayerName.insert(sLayerName.begin(), '\"');
                sLayerName.insert(sLayerName.end(), '\"');*/
                if (hasLeftBracket)
                    sLayerName.insert(sLayerName.begin(), '[');
                if (hasRightBracket)
                    sLayerName.insert(sLayerName.end(), ']');

                outputText(NULL, sLayerName.c_str(), nOption++);
#endif
                break;
            }
            case SEPARATOR:
            {
                if (iter->sValue == "(")
                {
                    std::string sValue;
                    while(iter != pNode->end())
                    {
                        sValue += iter->sValue;

                        if (iter->eType == SEPARATOR && iter->sValue == ")")
                            break;
                        ++iter;
                    }
                    outputText(NULL, sValue.c_str(), nOption++);
                    break;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }

        ++iter;
    }

    iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case SQUARE:
                    ++iter;
                    continue;
                case SQUARE_ORTHOGONAL:
                    ++iter;
                    continue;
                case OPPOSITE:
                    ++iter;
                    continue;
                case OPPOSITE1:
                    ++iter;
                    continue;
                case OPPOSITE2:
                    ++iter;
                    continue;
                case OPPOSITE_SYMMETRIC:
                    ++iter;
                    continue;
                case OPPOSITE_FSYMMETRIC:
                    ++iter;
                    continue;
                case OPPOSITE_EXTENDED:
                {
                    ++iter;
                    if (iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if (getNumberExp(iter, pNode->end(), sNumExp))
                        {
                        }
                    }
                    continue;
                }
                case OPPOSITE_EXTENDED1:
                {
                    ++iter;
                    if (iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if (getNumberExp(iter, pNode->end(), sNumExp))
                        {
                        }
                    }
                    continue;
                }
                case OPPOSITE_EXTENDED2:
                {
                    ++iter;
                    if (iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if (getNumberExp(iter, pNode->end(), sNumExp))
                        {
                        }
                    }
                    continue;
                }
                case OPPOSITE_EXTENDED_SYMMETRIC:
                {
                    ++iter;
                    if (iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if (getNumberExp(iter, pNode->end(), sNumExp))
                        {
                        }
                    }
                    continue;
                }
                case OPPOSITE_EXTENDED_FSYMMETRIC:
                {
                    ++iter;
                    if (iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if (getNumberExp(iter, pNode->end(), sNumExp))
                        {
                        }
                    }
                    continue;
                }
                case ACUTE_ALSO:
                    outputText(NULL, "acute_also", nOption++);
                    ++iter;
                    continue;
                case ACUTE_ONLY:
                    outputText(NULL, "acute_only", nOption++);
                    ++iter;
                    continue;
                case NOT_ACUTE:
                    outputText(NULL, "not_acute", nOption++);
                    ++iter;
                    continue;
                case PARALLEL_ALSO:
                    outputText(NULL, "parallel_also", nOption++);
                    ++iter;
                    continue;
                case PARALLEL_ONLY:
                    outputText(NULL, "parallel_only", nOption++);
                    ++iter;
                    continue;
                case NOT_PARALLEL:
                    outputText(NULL,"not_parallel", nOption++);
                    ++iter;
                    continue;
                case PERPENDICULAR_ALSO:
                    outputText(NULL,"perpendicular_also", nOption++);
                    ++iter;
                    continue;
                case PERPENDICULAR_ONLY:
                    outputText(NULL, "perpendicular_only", nOption++);
                    ++iter;
                    continue;
                case NOT_PERPENDICULAR:
                    outputText(NULL, "not_perpendicular", nOption++);
                    ++iter;
                    continue;
                case NOT_OBTUSE:
                    outputText(NULL, "not_obtuse", nOption++);
                    ++iter;
                    continue;
                case OBTUSE_ONLY:
                    outputText(NULL, "obtuse_only", nOption++);
                    ++iter;
                    continue;
                case OBTUSE_ALSO:
                    outputText(NULL, "obtuse_also", nOption++);
                    ++iter;
                    continue;
                case PROJECTING:
                {
                    ++iter;
                    outputText("apposition", NULL, nOption++);
                    if (iter != pNode->end() && isConstraintOperator(*iter))
                    {
                        std::string sCons;
                        if (getConstraint(iter, pNode->end(), sCons))
                        {
                            outputText(NULL, sCons.c_str(), nOption++);
                            continue;
                        }
                    }
                    outputText(NULL, NULL, nOption++);
                    continue;
                }
                case NOT_PROJECTING:
                    outputText("no_apposition", NULL, nOption++);
                    ++iter;
                    continue;
                case ANGLED:
                {
                    outputText("angled_edge", NULL,nOption++);
                    ++iter;
                    if (iter != pNode->end() && isConstraintOperator(*iter))
                    {
                        std::string sCons;
                        if (getConstraint(iter, pNode->end(), sCons))
                        {
                            outputText(NULL, sCons.c_str(), nOption++);
                            continue;
                        }
                    }
                    outputText(NULL, NULL, nOption++);
                    continue;
                }
                case CORNER_TO_CORNER:
                {
                    outputText("corner_corner",NULL, nOption++);
                    ++iter;
                    if (iter != pNode->end() && isConstraintOperator(*iter))
                    {
                        std::list<rcsToken_T>::iterator begin = iter;
                        std::string sCons;
                        if (getConstraint(iter, pNode->end(), sCons))
                        {
                            outputText(NULL, sCons.c_str(), nOption++);
                            continue;
                        }
                    }
                    continue;
                }
                case CORNER_TO_EDGE:
                    outputText("corner_edge", NULL, nOption++);
                    ++iter;
                    continue;
                case NOT_CORNER:
                    outputText("not_corner", NULL, nOption++);
                    ++iter;
                    continue;
                case CORNER:
                    outputText("all_corner", NULL, nOption++);
                    ++iter;
                    continue;
                case REGION:
                    outputText("region", NULL, nOption++);
                    ++iter;
                    continue;
                case REGION_EXTENTS:
                    outputText("region_boundaries", NULL, nOption++);
                    ++iter;
                    continue;
                case REGION_CENTERLINE:
                    outputText("region_centerline", NULL, nOption++);
                    ++iter;
                    if (iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if (getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            outputText(NULL, sNumExp.c_str(), nOption);
                            continue;
                        }
                    }
                    continue;
                case MEASURE_ALL:
                    outputText("all_edge", NULL, nOption++);
                    ++iter;
                    continue;
                case MEASURE_COINCIDENT:
                    outputText("coincident_edge_also", NULL, nOption++);
                    ++iter;
                    continue;
                case NOTCH:
                    outputText("same_polygon", NULL, nOption++);
                    ++iter;
                    continue;
                case SPACE:
                    outputText("diff_polygon", NULL, nOption++);
                    ++iter;
                    continue;
                case CONNECTED:
                    outputText("same_net", NULL, nOption++);
                    ++iter;
                    continue;
                case NOT_CONNECTED:
                    outputText("diff_net", NULL, nOption++);
                    ++iter;
                    continue;
                case ABUT:
                {
                    outputText("adjacent", NULL, nOption++);
                    ++iter;
                    if (iter != pNode->end() && isConstraintOperator(*iter))
                    {
                        std::string sCons;
                        if (getConstraint(iter, pNode->end(), sCons))
                        {
                            outputText(NULL, sCons.c_str(), nOption++);
                            continue;
                        }
                    }
                    continue;
                }
                case OVERLAP:
                    outputText("overlapped", NULL, nOption++);
                    ++iter;
                    continue;
                case SINGULAR:
                    outputText("point_touch", NULL, nOption++);
                    ++iter;
                    continue;
                case INSIDE_ALSO:
                    outputText("inside_also", NULL,  nOption++);
                    ++iter;
                    continue;
                case OUTSIDE_ALSO:
                    outputText("outside_also", NULL,  nOption++);
                    ++iter;
                    continue;
                case EXCLUDE_FALSE:
                case EXCLUDE_FALSE_NOTCH:
                    outputText("reduce_false", NULL, nOption++);
                    ++iter;
                    continue;
                case EXCLUDE_SHIELDED:
                {
                    if (!isDimensionalCheckKeyword(++iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if (getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            outputText("shielded_level",sNumExp.c_str(), nOption++);
                        }
                    }
                    else
                    {
                        outputText("shielded_level", "4", nOption++);
                    }
                    continue;
                }
                case BY_LAYER:
                {
                    outputText("by_layer", NULL, nOption++);
                    iter++;
                    if ( (iter->eType != IDENTIFIER_NAME) || !parseTmpLayerForOperation(iter->sValue) )
                    {
                        std::string sLayerName;
                        if (!getLayerRefName(iter, pNode->end(), sLayerName))
                        {
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                iter->nLineNo, iter->sValue);
                        }
                        outputText(NULL, sLayerName.c_str(), nOption++);
                    }
                    continue;
                }
                case COUNT:
                {
                    std::list<rcsToken_T>::iterator countIt = iter++;
                    std::string sNumExp;
                    if (getNumberExp(iter, pNode->end(), sNumExp))
                    {
                        outputText("count", sNumExp.c_str(), nOption++);
                    }
                    else
                    {
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                            countIt->nLineNo, countIt->sValue);
                    }
                    continue;
                }

                case INTERSECTING_ONLY:
                    outputText("intersecting_only", NULL, nOption++);
                    ++iter;
                    continue;
                default:
                    break;
            }
        }
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   continue;
                }
                else if (iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    continue;
                }
            }
            case BUILT_IN_LANG:
            {
                break;
            }
            case SEPARATOR:
            {
                if (iter->sValue == "(")
                {
                    std::string sValue;
                    while(iter != pNode->end())
                    {
                        sValue += iter->sValue;

                        if (iter->eType == SEPARATOR && iter->sValue == ")")
                            break;
                        ++iter;
                    }
                    break;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomBooleanCmd(rcsSynLayerOperationNode_T *pNode)
{
    if (pNode->getOperationType() == AND)
        m_sCurCommandString = "AND";
    else if (pNode->getOperationType() == OR)
        m_sCurCommandString = "OR";
    else if (pNode->getOperationType() == NOT)
        m_sCurCommandString = "NOT";
    else if (pNode->getOperationType() == XOR)
        m_sCurCommandString = "XOR";
    hvUInt32 nOption = 0;
    switch(pNode->getOperationType())
    {
        case AND:
            outputText("geom_and ( ", NULL, nOption++);
            break;
        case OR:
            outputText("geom_or ( ", NULL, nOption++);
            break;
        case NOT:
            outputText("geom_not ( ", NULL, nOption++);
            break;
        case XOR:
            outputText("geom_xor ( ", NULL, nOption++);
            break;
        default:
            break;
    }

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if (pNode->getOperationType() == AND)
                {
                    switch(iter->eKeyType)
                    {
                        case CONNECTED:
                            outputText("same_net", NULL, ++nOption);
                            ++iter;
                            continue;
                        case NOT_CONNECTED:
                            outputText("diff_net", NULL, ++nOption);
                            ++iter;
                            continue;
                        default:
                            break;
                    }
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }

                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
                else if (iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    std::list<rcsToken_T>::iterator iter_cur = iter;
                    if (getConstraint(iter, pNode->end(), sConstraint))
                    {
                        outputText(NULL, sConstraint.c_str(), nOption++);
                    }
                    else
                    {
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                            iter->nLineNo, iter->sValue);
                    }
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convEdgeMergeCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "OR EDGE";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "edge_or ( ";
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }

                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomTopoCmd(rcsSynLayerOperationNode_T *pNode)
{
    switch(pNode->getOperationType())
    {
        case INSIDE:
            m_sCurCommandString = "INSIDE";
            break;
        case NOT_INSIDE:
            m_sCurCommandString = "NOT INSIDE";
            break;
        case OUTSIDE:
            m_sCurCommandString = "OUTSIDE";
            break;
        case NOT_OUTSIDE:
            m_sCurCommandString = "NOT OUTSIDE";
            break;
        case TOUCH:
            m_sCurCommandString = "TOUCH";
            break;
        case NOT_TOUCH:
            m_sCurCommandString = "NOT TOUCH";
            break;
        case ENCLOSE:
            m_sCurCommandString = "ENCLOSE";
            break;
        case NOT_ENCLOSE:
            m_sCurCommandString = "NOT ENCLOSE";
            break;
        case CUT:
            m_sCurCommandString = "CUT";
            break;
        case NOT_CUT:
            m_sCurCommandString = "NOT CUT";
            break;
        case INTERACT:
            m_sCurCommandString = "INTERACT";
            break;
        case NOT_INTERACT:
            m_sCurCommandString = "NOT INTERACT";
            break;
        default:
            break;
    }
    hvUInt32 nOption = 0;
    switch(pNode->getOperationType())
    {
        case INSIDE:
            outputText("geom_inside (", NULL, nOption++);
            break;
        case NOT_INSIDE:
            outputText("~geom_inside (", NULL, nOption++);
            break;
        case OUTSIDE:
            outputText("geom_outside (", NULL, nOption++);
            break;
        case NOT_OUTSIDE:
            outputText("~geom_outside (", NULL, nOption++);
            break;
        case TOUCH:
            outputText("geom_adjacent (", NULL, nOption++);
            break;
        case NOT_TOUCH:
            outputText("~geom_adjacent (", NULL, nOption++);
            break;
        case ENCLOSE:
            outputText("geom_enclose (", NULL, nOption++);
            break;
        case NOT_ENCLOSE:
            outputText("~geom_enclose (", NULL, nOption++);
            break;
        case CUT:
            outputText("geom_cut (", NULL, nOption++);
            break;
        case NOT_CUT:
            outputText("~geom_cut (", NULL, nOption++);
            break;
        case INTERACT:
            outputText("geom_interact (", NULL, nOption++);
            break;
        case NOT_INTERACT:
            outputText("~geom_interact (", NULL, nOption++);
            break;
        default:
            break;
    }
    (*m_oPVRSStream) << " ";
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if (BY_NET == iter->eKeyType)
                {
                    outputText("count_by_net", NULL, nOption++);
                    break;
                }
                else if (EVEN == iter->eKeyType)
                {
                    outputText("even", NULL, nOption++);
                    break;
                }
                else if (ODD == iter->eKeyType)
                {
                    outputText("odd", NULL, nOption++);
                    break;
                }
                else if ((SINGULAR_ONLY == iter->eKeyType || SINGULAR_ALSO == iter->eKeyType)
                        && (pNode->getOperationType() == INTERACT ||
                            pNode->getOperationType() == NOT_INTERACT))
                {
                    if (SINGULAR_ONLY == iter->eKeyType)
                    {
                        outputText("only_point_touch", NULL, nOption++);
                    }
                    else
                    {
                        outputText("point_touch", NULL, nOption++);
                    }
                    break;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }

                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
                else if (iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    if(getConstraint(iter, pNode->end(), sConstraint))
                    {
                        outputText(NULL, sConstraint.c_str(), nOption++);
                        continue;
                    }
                }
            }
            case SEPARATOR:
            {
                 outputText(iter->sValue.c_str(), NULL, nOption++);
                 break;

            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convAttachCmd(rcsSynLayerOperationNode_T *pNode)
{
    if (_hasOnlyDirectKeyword(pNode))
    {
        outputCon1Error(pNode);
        return;
    }
    m_sCurCommandString = "ATTACH";
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "attach " : "attach ( ");

    bool hasTextLayer = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if (iter->eKeyType == MASK || iter->eKeyType == DIRECT)
                {
                    outputCon2Error(pNode, *iter);
                    break;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }

                std::string sLayerName;
                if (!hasTextLayer)
                {
                    if (getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        hasTextLayer = true;
                        outputText(NULL, sLayerName.c_str(), nOption++);
                        continue;
                    }
                }
                else if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    if(!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convConnectCmd(rcsSynLayerOperationNode_T *pNode)
{
    if (_hasOnlyDirectKeyword(pNode))
    {
        outputCon1Error(pNode);
        return;
    }
    m_sCurCommandString = "CONNECT";
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "connect " : "connect ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if (MASK == iter->eKeyType)
                {
                    outputCon2Error(pNode, *iter);
                    break;
                }
                else if (DIRECT == iter->eKeyType)
                {
                    outputCon2Error(pNode, *iter);
                    break;
                }
                else if(iter->eKeyType == BY)
                {
                    ++iter;
                    outputText("by", NULL, nOption++);
                    if (iter->eType == IDENTIFIER_NAME)
                    {
                        if(parseTmpLayerForOperation(iter->sValue))
                        {
                            break;
                        }
                    }
                    std::string sLayerName;
                    if (getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText(NULL, sLayerName.c_str(), nOption++);
                        continue;
                    }
                }
                else if(iter->eKeyType == WITH)
                {
                    ++iter;
                    outputText("with", NULL, nOption++);
                    if (iter->eType == IDENTIFIER_NAME)
                    {
                        if(parseTmpLayerForOperation(iter->sValue))
                        {
                            break;
                        }
                    }
                    std::string sLayerName;
                    if (getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText(NULL, sLayerName.c_str(), nOption++);
                        continue;
                    }
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }

                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    if(!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convAttachOrderCmd(rcsSynLayerOperationNode_T *pNode)
{
    if (_hasOnlyDirectKeyword(pNode))
    {
        outputCon1Error(pNode);
        return;
    }
    m_sCurCommandString = "LABEL ORDER";
    hvUInt32 nOption = 0;
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    (*m_oPVRSStream) << (_isTvf2Trs ? "attach_order " : "attach_order ( ");

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if (MASK == iter->eKeyType)
                {
                    outputCon2Error(pNode, *iter);
                    break;
                }
                else if (DIRECT == iter->eKeyType)
                {
                    outputCon2Error(pNode, *iter);
                    break;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }

                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
        }
        ++iter;
    }
    if(!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomAttrCmd(rcsSynLayerOperationNode_T *pNode)
{
    switch(pNode->getOperationType())
    {
        case AREA:
            m_sCurCommandString = "AREA";
            break;
        case NOT_AREA:
            m_sCurCommandString = "NOT AREA";
            break;
        case PERIMETER:
            m_sCurCommandString = "PERIMETER";
            break;
        case DONUT:
            m_sCurCommandString = "DONUT";
            break;
        case NOT_DONUT:
            m_sCurCommandString = "NOT DONUT";
            break;
        case VERTEX:
            m_sCurCommandString = "VERTEX";
            break;
        default:
            break;
    }
    hvUInt32 nOption = 0;
    switch(pNode->getOperationType())
    {
        case AREA:
            outputText("geom_area", NULL, nOption++);
            break;
        case NOT_AREA:
            outputText("~geom_area", NULL, nOption++);
            break;
        case PERIMETER:
            outputText("geom_perimeter", NULL, nOption++);
            break;
        case DONUT:
            outputText("geom_donut", NULL, nOption++);
            break;
        case NOT_DONUT:
            outputText("~geom_donut", NULL, nOption++);
            break;
        case VERTEX:
            outputText("geom_vertex", NULL, nOption++);
            break;
        default:
            return;
    }
    (*m_oPVRSStream) << " ( ";
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }

                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
                else if (iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    outputText(NULL, sConstraint.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomCopyLayerCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "COPY";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "copy ( ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            case SEPARATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }

                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText(NULL, sLayerName.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomSelectHolesCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "HOLES";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "geom_holes ( ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if (INNER == iter->eKeyType)
                {
                    outputText("inner_most", NULL, nOption++);
                    break;
                }
                else if (EMPTY == iter->eKeyType)
                {
                    outputText("hollow", NULL, nOption++);
                    break;
                }
                else if (SINGULAR_ALSO == iter->eKeyType)
                {
                    outputText("point_touch", NULL, nOption++);
                    break;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }

                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
                else if (iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    outputText(NULL, sConstraint.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomSelectRectangleCmd(rcsSynLayerOperationNode_T *pNode)
{
    switch(pNode->getOperationType())
    {
        case RECTANGLE:
            m_sCurCommandString = "RECTANGLE";
            break;
        case NOT_RECTANGLE:
            m_sCurCommandString = "NOT RECTANGLE";
            break;
        default:
            break;
    }
    hvUInt32 nOption = 0;
    switch(pNode->getOperationType())
    {
        case RECTANGLE:
            (*m_oPVRSStream) << "geom_rectangle ( ";
            break;
        case NOT_RECTANGLE:
            (*m_oPVRSStream) << "~geom_rectangle ( ";
            break;
        default:
            return;
    }

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case BY:
                {
                    std::string sConstraint;
                    getConstraint(++iter, pNode->end(), sConstraint);
                    outputText("by", sConstraint.c_str(), nOption++);
                    continue;
                }
                case ASPECT:
                {
                    std::string sConstraint;
                    getConstraint(++iter, pNode->end(), sConstraint);
                    outputText("ratio", sConstraint.c_str(), nOption++);
                    continue;
                }
                case ORTHOGONAL:
                {
                    if (++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                       ONLY == iter->eKeyType)
                    {
                        outputText("orthogonal", NULL, nOption++);
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                    ++iter;
                    continue;
                }
                case FIXED_SECOND:
                {
                    outputText("fixed", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case MEASURE_EXTENTS:
                    outputText("by_boundaries", NULL, nOption++);
                    ++iter;
                    continue;
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }

                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
                else if (iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    outputText(NULL, sConstraint.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convEdgeTopoCmd(rcsSynLayerOperationNode_T *pNode)
{
    switch(pNode->getOperationType())
    {
        case INSIDE_EDGE:
            m_sCurCommandString = "INSIDE EDGE";
            break;
        case NOT_INSIDE_EDGE:
            m_sCurCommandString = "NOT INSIDE EDGE";
            break;
        case OUTSIDE_EDGE:
            m_sCurCommandString = "OUTSIDE EDGE";
            break;
        case NOT_OUTSIDE_EDGE:
            m_sCurCommandString = "NOT OUTSIDE EDGE";
            break;
        case COINCIDENT_EDGE:
            m_sCurCommandString = "COINCIDENT EDGE";
            break;
        case NOT_COINCIDENT_EDGE:
            m_sCurCommandString = "NOT COINCIDENT EDGE";
            break;
        case COINCIDENT_INSIDE_EDGE:
            m_sCurCommandString = "COINCIDENT INSIDE EDGE";
            break;
        case NOT_COINCIDENT_INSIDE_EDGE:
            m_sCurCommandString = "NOT COINCIDENT INSIDE EDGE";
            break;
        case COINCIDENT_OUTSIDE_EDGE:
            m_sCurCommandString = "COINCIDENT OUTSIDE EDGE";
            break;
        case NOT_COINCIDENT_OUTSIDE_EDGE:
            m_sCurCommandString = "NOT COINCIDENT OUTSIDE EDGE";
            break;
        case TOUCH_EDGE:
            m_sCurCommandString = "TOUCH EDGE";
            break;
        case NOT_TOUCH_EDGE:
            m_sCurCommandString = "NOT TOUCH EDGE";
            break;
        case TOUCH_INSIDE_EDGE:
            m_sCurCommandString = "TOUCH INSIDE EDGE";
            break;
        case NOT_TOUCH_INSIDE_EDGE:
            m_sCurCommandString = "NOT TOUCH INSIDE EDGE";
            break;
        case TOUCH_OUTSIDE_EDGE:
            m_sCurCommandString = "TOUCH OUTSIDE EDGE";
            break;
        case NOT_TOUCH_OUTSIDE_EDGE:
            m_sCurCommandString = "NOT TOUCH OUTSIDE EDGE";
            break;
        default:
            break;
    }
    hvUInt32 nOption = 0;
    switch(pNode->getOperationType())
    {
        case INSIDE_EDGE:
            outputText("edge_inside (", NULL, nOption++);
            break;
        case NOT_INSIDE_EDGE:
            outputText("~edge_inside (", NULL, nOption++);
            break;
        case OUTSIDE_EDGE:
            outputText("edge_outside (", NULL, nOption++);
            break;
        case NOT_OUTSIDE_EDGE:
            outputText("~edge_outside (", NULL, nOption++);
            break;
        case COINCIDENT_EDGE:
            outputText("edge_coincident (", "both_side", nOption++);
            break;
        case NOT_COINCIDENT_EDGE:
            outputText("~edge_coincident (", "both_side", nOption++);
            break;
        case COINCIDENT_INSIDE_EDGE:
            outputText("edge_coincident (", "inside", nOption++);
            break;
        case NOT_COINCIDENT_INSIDE_EDGE:
            outputText("~edge_coincident (", "inside", nOption++);
            break;
        case COINCIDENT_OUTSIDE_EDGE:
            outputText("edge_coincident (", "outside", nOption++);
            break;
        case NOT_COINCIDENT_OUTSIDE_EDGE:
            outputText("~edge_coincident (", "outside", nOption++);
            break;
        case TOUCH_EDGE:
            outputText("edge_adjacent (", "both_side", nOption++);
            break;
        case NOT_TOUCH_EDGE:
            outputText("~edge_adjacent (", "both_side", nOption++);
            break;
        case TOUCH_INSIDE_EDGE:
            outputText("edge_adjacent (", "inside", nOption++);
            break;
        case NOT_TOUCH_INSIDE_EDGE:
            outputText("~edge_adjacent (", "inside", nOption++);
            break;
        case TOUCH_OUTSIDE_EDGE:
            outputText("edge_adjacent (", "outside", nOption++);
            break;
        case NOT_TOUCH_OUTSIDE_EDGE:
            outputText("~edge_adjacent (", "outside", nOption++);
            break;
        default:
            return;
    }
    (*m_oPVRSStream) << " ";
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if (iter->eKeyType == ENDPOINT)
                {
                    ++iter;
                    if (pNode->end() != iter && iter->eType == SECONDARY_KEYWORD &&
                       (iter->eKeyType == ONLY || iter->eKeyType == ALSO))
                    {
                        if (iter->eKeyType == ONLY)
                            outputText("only_point", NULL, nOption++);
                        else
                            outputText("point", NULL, nOption++);

                        ++iter;
                        continue;
                    }
                    else
                    {
                        --iter;
                        switch(pNode->getOperationType())
                        {
                            case TOUCH_EDGE:
                            case NOT_TOUCH_EDGE:
                            case TOUCH_INSIDE_EDGE:
                            case NOT_TOUCH_INSIDE_EDGE:
                            case TOUCH_OUTSIDE_EDGE:
                            case NOT_TOUCH_OUTSIDE_EDGE:
                                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                                    iter->nLineNo, iter->sValue);
                                break;
                            default:
                                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                    iter->nLineNo, iter->sValue);
                        }
                    }
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }

                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convCheckFittingRectangleCmd(rcsSynLayerOperationNode_T *pNode)
{
    switch(pNode->getOperationType())
    {
        case ENCLOSE_RECTANGLE:
            m_sCurCommandString = "ENCLOSE RECTANGLE";
            break;
        case NOT_ENCLOSE_RECTANGLE:
            m_sCurCommandString = "NOT ENCLOSE RECTANGLE";
            break;
        default:
            break;
    }
    hvUInt32 nOption = 0;
    switch(pNode->getOperationType())
    {
        case ENCLOSE_RECTANGLE:
            (*m_oPVRSStream) << "geom_enclose_rectangle ( ";
            break;
        case NOT_ENCLOSE_RECTANGLE:
            (*m_oPVRSStream) << "~geom_enclose_rectangle ( ";
            break;
        default:
            return;
    }

    hvUInt32 iLayer = 0;
    bool hasWidth  = false;
    bool hasLength = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if (ORTHOGONAL == iter->eKeyType)
                {
                    if (++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                       ONLY == iter->eKeyType)
                    {
                        outputText("orthogonal", NULL, nOption++);
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                    break;
                }
                else if (FIXED_SECOND == iter->eKeyType)
                {
                    outputText("fixed", NULL, nOption++);
                    break;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            case SEPARATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }


                if(iter->eType == OPERATOR)
                {
                    outputText(NULL, iter->sValue.c_str(), nOption++);
                }

                if (iLayer == 0)
                {
                    std::string sLayerName;
                    if (getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        ++iLayer;
                       outputText(NULL, sLayerName.c_str(), nOption++);
                       continue;
                    }
                }
                else if (!hasWidth)
                {
                    std::string sNumExp;
                    if (getNumberExp(iter, pNode->end(), sNumExp))
                        outputText(NULL, sNumExp.c_str(), nOption++);
                    hasWidth = true;
                }
                else if (!hasLength)
                {
                    std::string sNumExp;
                    if (getNumberExp(iter, pNode->end(), sNumExp))
                        outputText(NULL, sNumExp.c_str(), nOption++);
                    hasLength = true;
                }
                else
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
                continue;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomSelectInCellCmd(rcsSynLayerOperationNode_T *pNode)
{
    switch(pNode->getOperationType())
    {
        case INSIDE_CELL:
            m_sCurCommandString = "INSIDE CELL";
            break;
        case NOT_INSIDE_CELL:
            m_sCurCommandString = "NOT INSIDE CELL";
            break;
        default:
            break;
    }
    hvUInt32 nOption = 0;
    switch(pNode->getOperationType())
    {
        case INSIDE_CELL:
            (*m_oPVRSStream) << "geom_inside_cell ( ";
            break;
        case NOT_INSIDE_CELL:
            (*m_oPVRSStream) << "~geom_inside_cell ( ";
            break;
        default:
            return;
    }

    hvUInt32 iLayer = 0;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case PRIMARY:
                {
                   if (++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType
                      && ONLY == iter->eKeyType)
                   {
                       outputText("top_cell", NULL, nOption++);
                       ++iter;
                       continue;
                   }
                   else
                   {
                       --iter;
                       throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                           iter->nLineNo, iter->sValue);
                   }
                    break;
                }
                case WITH_LAYER:
                {
                    ++iter;
                    outputText("with_layer", NULL, nOption++);
                    if (iter->eType == IDENTIFIER_NAME)
                    {
                        if(parseTmpLayerForOperation(iter->sValue))
                        {
                            break;
                        }
                    }
                    std::string sLayerName;
                    if (getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                       outputText(NULL, sLayerName.c_str(), nOption++);
                       continue;
                    }
                    break;
                }
                case WITH_MATCH:
                    outputText("pattern_match", NULL, nOption++);
                    ++iter;
                    if (iter != pNode->end() && iter->eKeyType == GOLDEN)
                    {
                        outputText("golden", NULL, nOption++);
                        ++iter;
                    }
                    continue;
                case GOLDEN:
                    outputCon2Error(pNode, *iter);
                    ++iter;
                    continue;
                case RULE:
                case WITH_PROPERTY:
                case NOT_WITH_PROPERTY:
                {
                    outputCon2Error(pNode, *iter);
                    ++iter;
                    
                    if (iter != pNode->end())
                        ++iter;
                    continue;
                }
                default:
                    break;
            }
        }
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }

                if (iLayer == 0)
                {
                    std::string sLayerName;
                    if (getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        ++iLayer;
                        outputText(NULL, sLayerName.c_str(), nOption++);
                        continue;
                    }
                }
                else if (iter->eType != NUMBER && iter->eType != OPERATOR)
                {
                    std::string sCellName;
                    if (getStringName(iter, pNode->end(), sCellName))
                    {
                        outputText(NULL, sCellName.c_str(), nOption++);
                        break;
                    }
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomGetBBoxCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "EXTENTS";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "geom_get_boundaries ( ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if (CENTERS == iter->eKeyType)
                {
                    outputText("output_center", NULL, nOption);
                    ++iter;
                    if (iter == pNode->end())
                        continue;

                    std::string sNumExp;
                    if (getNumberExp(iter, pNode->end(), sNumExp))
                    {
                        outputText(NULL, sNumExp.c_str(), nOption);
                    }
                    nOption++;
                    continue;
                }
                else if (SQUARES == iter->eKeyType)
                {
                    outputText("output_square", NULL, nOption++);
                    break;
                }
                else if (INSIDE_OF_LAYER == iter->eKeyType)
                {
                    ++iter;
                    outputText("inside_of_layer", NULL, nOption++);
                    if( ( iter != pNode->end()) && (iter->eType == IDENTIFIER_NAME))
                    {
                        if(parseTmpLayerForOperation(iter->sValue))
                        {
                            ++iter;
                            continue;
                        }
                    }
                    std::string sLayerName;
                    if (getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText(NULL, sLayerName.c_str(), nOption++);
                        continue;
                    }
                    continue;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }

                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText(NULL, sLayerName.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convSizeCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "SIZE";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "geom_size ( ";
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case BY:
                {
                    ++iter;
                    std::string sNumExp;
                    
                    rcsExpressionParser_T::setNegativeNumber();
                    if (getNumberExp(iter, pNode->end(), sNumExp))
                    {
                        outputText("by", sNumExp.c_str(), nOption++);
                        rcsExpressionParser_T::unsetNegativeNumber();
                    }

                    continue;
                }
                case INSIDE_OF:
                {
                    ++iter;
                    outputText("inside_of_layer", " ", nOption);
                    if( ( iter != pNode->end()) && (iter->eType == IDENTIFIER_NAME))
                    {
                        if(parseTmpLayerForOperation(iter->sValue))
                        {
                            ++iter;
                            continue;
                        }
                    }
                    std::string sLayerName;
                    if (getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                       //outputText("inside_of_layer", sLayerName.c_str(), nOption++);
                        outputText(" ", sLayerName.c_str(), nOption++);
                       continue;
                    }
                }
                case OUTSIDE_OF:
                {
                    ++iter;
                    outputText("outside_of_layer", " ", nOption);
                    if( ( iter != pNode->end()) && (iter->eType == IDENTIFIER_NAME))
                    {
                        if(parseTmpLayerForOperation(iter->sValue))
                        {
                            ++iter;
                            continue;
                        }
                    }
                    std::string sLayerName;
                    if (getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                       //outputText("outside_of_layer", sLayerName.c_str(), nOption++);
                        outputText(" ", sLayerName.c_str(), nOption++);
                       continue;
                    }
                }
                case STEP:
                {
                    ++iter;
                    std::string sNumExp;
                    if (getNumberExp(iter, pNode->end(), sNumExp))
                        outputText("delta", sNumExp.c_str(), nOption++);
                    continue;
                }
                case TRUNCATE:
                {
                    ++iter;
                    std::string sNumExp;
                    if (getNumberExp(iter, pNode->end(), sNumExp))
                        outputText("clip", sNumExp.c_str(), nOption++);
                    continue;
                }
                case BEVEL:
                {
                    ++iter;
                    std::string sNumExp;
                    if (getNumberExp(iter, pNode->end(), sNumExp))
                        outputText("clip_corner", sNumExp.c_str(), nOption++);
                    continue;
                }
                case OVERLAP_ONLY:
                {
                    outputText("output_overlap_region", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case UNDEROVER:
                {
                    outputText("in_out", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case OVERUNDER:
                {
                    outputText("out_in", NULL, nOption++);
                    ++iter;
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }

                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convSize_XYCmd(rcsSynLayerOperationNode_T *pNode)
{
    (*m_oPVRSStream) << "geom_size_xy ( ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while (++iter != pNode->end())
        (*m_oPVRSStream) << iter->sValue << " ";

    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

static bool
isValidDensitySecondaryKey(const rcsToken_T &token)
{
    if (token.eType == SECONDARY_KEYWORD)
    {
        switch(token.eKeyType)
        {
            case TRUNCATE:
            case BACKUP:
            case IGNORE:
            case WRAP:
            case INSIDE_OF_EXTENT:
            case BY_EXTENT:
            case BY_POLYGON:
            case BY_RECTANGLE:
            case WINDOW:
            case STEP:
            case INSIDE_OF:
            case INSIDE_OF_LAYER:
            case CENTERED:
            case GRADIENT:
            case RELATIVE:
            case ABSOLUTE:
            case CORNER:
            case MAGNITUDE:
            case CENTERS:
            case PRINT:
            case RDB:
            case MAG:
            case COMBINE:
                return true;
            default:
                return false;
        }
    }
    else if (LAYER_OPERATION_KEYWORD == token.eType || BUILT_IN_LANG == token.eType)
    {
        return true;
    }
    return false;
}

void
rcsSynPvrsConvertor::convCheckDensityCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "DENSITY";
    if (pNode->getOperationType() == DENSITY_CONVOLVE)
    {
        outputCon1Error(pNode);
        return;
    }
    hvUInt32 nOption = 0;
    bool bCombineOpt = false;
    (*m_oPVRSStream) << "check_density ( ";
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType && iter->eKeyType == COMBINE)
        {
            bCombineOpt = true;
        }
        ++iter;
    }
    iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case TRUNCATE:
                {
                    outputText(NULL, "boundary_truncate", nOption++);
                    ++iter;
                    continue;
                }
                case BACKUP:
                {
                    outputText(NULL, "boundary_backup", nOption++);
                    ++iter;
                    continue;
                }
                case IGNORE:
                {
                    outputText(NULL, "boundary_ignore", nOption++);
                    ++iter;
                    continue;
                }
                case WRAP:
                {
                    outputText(NULL, "boundary_repeat", nOption++);
                    ++iter;
                    continue;
                }
                case INSIDE_OF_EXTENT:
                {
                    outputText(NULL, "inside_of_boundary", nOption++);
                    ++iter;
                    continue;
                }
                case INSIDE_OF:
                {
                    std::string sLeftX;
                    std::string sBottomY;
                    std::string sRightX;
                    std::string sTopY;
                    ++iter;
                    vector<string> vecCoorValue;
                    rcsExpressionParser_T::setNegativeNumber();
                    getNumberExp(iter, pNode->end(), sLeftX);
                    getNumberExp(iter, pNode->end(), sBottomY);
                    getNumberExp(iter, pNode->end(), sRightX);
                    getNumberExp(iter, pNode->end(), sTopY);
                    rcsExpressionParser_T::unsetNegativeNumber();

                    vecCoorValue.push_back(sLeftX);
                    vecCoorValue.push_back(sBottomY);
                    vecCoorValue.push_back(sRightX);
                    vecCoorValue.push_back(sTopY);

                    outputText("inside_of_region", NULL, nOption++);

                    vector<string>::iterator it = vecCoorValue.begin();
                    while(it != vecCoorValue.end())
                    {
                        string strTmp = *it;
                        if(isCoordinateValue(strTmp))
                        {
                            outputText(NULL, strTmp.c_str(), nOption);
                        }
                        else
                        {
                                iter--;
                        }

                        it++;
                    }

                    /*outputText(NULL, sLeftX.c_str(), nOption);
                    outputText(NULL, sBottomY.c_str(), nOption);
                    outputText(NULL, sRightX.c_str(), nOption);
                    outputText(NULL, sTopY.c_str(), nOption++);*/
                    continue;
                }
                case INSIDE_OF_LAYER:
                {
                    ++iter;
                    outputText("inside_of_layer", NULL, nOption++);
                    if( ( iter != pNode->end()) && (iter->eType == IDENTIFIER_NAME))
                    {
                        if(parseTmpLayerForOperation(iter->sValue))
                        {
                            ++iter;
                            continue;
                        }
                    }
                    std::string sLayerName;
                    if (getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText(NULL, sLayerName.c_str(), nOption++);
                    }
                    continue;
                }
                case BY_EXTENT:
                {
                    outputText(NULL, "in_boundaries", nOption);
                    ++iter;
                    continue;
                }
                case BY_POLYGON:
                {
                    outputText(NULL, "in_polygon", nOption);
                    ++iter;
                    continue;
                }
                case BY_RECTANGLE:
                {
                    outputText(NULL, "in_rectangle", nOption);
                    ++iter;
                    continue;
                }
                case CORNER:
                {
                    outputText(NULL, "corner_also", nOption);
                    std::vector<std::string> vFValues;
                    parseFollowNums(iter, pNode->end(), vFValues,
                                    isValidDensitySecondaryKey, 0, 1);
                    if (!vFValues.empty())
                        outputText(NULL, vFValues[0].c_str(), nOption++);

                    continue;
                }
                case WINDOW:
                {
                    std::vector<std::string> vFValues;
                    parseFollowNums(iter, pNode->end(), vFValues,
                                    isValidDensitySecondaryKey);
                    if (vFValues.size() > 1)
                    {
                        outputText("by_window", vFValues[0].c_str(), nOption++);
                        outputText(NULL, vFValues[1].c_str(), nOption);
                    }
                    else if(!vFValues.empty())
                    {
                        outputText("by_window", vFValues[0].c_str(), nOption++);
                    }
                    continue;
                }
                case STEP:
                {
                    std::vector<std::string> vFValues;
                    parseFollowNums(iter, pNode->end(), vFValues,
                                    isValidDensitySecondaryKey);
                    if (vFValues.size() > 1)
                    {
                        outputText("delta", vFValues[0].c_str(), nOption++);
                        outputText(NULL, vFValues[1].c_str(), nOption);
                    }
                    else if(!vFValues.empty())
                    {
                        outputText("delta", vFValues[0].c_str(), nOption++);
                    }
                    continue;
                }
                case CENTERS:
                {
                    std::vector<std::string> vFValues;
                    parseFollowNums(iter, pNode->end(), vFValues,
                                    isValidDensitySecondaryKey, 1, 1);
                    outputText("output_center", vFValues[0].c_str(), nOption++);
                    continue;
                }
                case MAG:
                {
                    std::vector<std::string> vFValues;
                    parseFollowNums(iter, pNode->end(), vFValues,
                                    isValidDensitySecondaryKey, 1, 1);
                    outputText("mag", vFValues[0].c_str(), nOption++);
                    continue;
                }
                case CENTERED:
                {
                    std::vector<std::string> vFValues;
                    parseFollowNums(iter, pNode->end(), vFValues,
                                    isValidDensitySecondaryKey, 1, 1);
                    outputText("in_boundaries_center", vFValues[0].c_str(), nOption++);
                    continue;
                }
                case GRADIENT:
                case MAGNITUDE:
                {
                    std::string sOption;
                    bCombineOpt = false;
                    if (iter->eKeyType == GRADIENT)
                        sOption = "check_gradient";
                    else
                        sOption = "check_separate_gradient";

                    if (++iter != pNode->end() && !isValidDensitySecondaryKey(*iter)
                       && isConstraintOperator(*iter))
                    {
                        std::string sConstraint;
                        if (getConstraint(iter, pNode->end(), sConstraint))
                            outputText(sOption.c_str(), sConstraint.c_str(), nOption++);
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP12,
                                            iter->nLineNo, iter->sValue);
                    }
                    continue;
                }
                case COMBINE:
                {
                    outputCon2Error(pNode,*iter);
                    if (++iter != pNode->end() && !isValidDensitySecondaryKey(*iter)
                       && isConstraintOperator(*iter))
                    {
                        std::string sConstraint;
                        if (getConstraint(iter, pNode->end(), sConstraint))
                            bCombineOpt = true;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP12,
                                            iter->nLineNo, iter->sValue);
                    }
                    continue;
                }
                case RELATIVE:
                {
                    if (!bCombineOpt)
                    {
                        outputText(NULL, "relative_ratio", nOption);
                    }
                    else
                    {
                        outputCon2Error(pNode,*iter);
                    }
                    ++iter;
                    continue;
                }
                case ABSOLUTE:
                {
                    if (!bCombineOpt)
                    {
                        outputText(NULL, "absolute_value", nOption);
                    }
                    else
                    {
                        outputCon2Error(pNode,*iter);
                    }
                    ++iter;
                    continue;
                }
                case PRINT:
                {
                    ++iter;
                    bool isOnly = false;
                    if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == ONLY)
                    {
                        isOnly = true;
                        ++iter;
                    }

                    std::string sFileName;
                    getStringName(iter, pNode->end(), sFileName);
                    if (isOnly)
                        outputText("only_output_log", sFileName.c_str(), nOption++);
                    else
                        outputText("output_log", sFileName.c_str(), nOption++);
                    ++iter;
                    continue;
                }
                case RDB:
                {
                    ++iter;
                    bool isOnly = false;
                    if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == ONLY)
                    {
                        isOnly = true;
                        ++iter;
                    }

                    std::string sFileName;
                    getStringName(iter, pNode->end(), sFileName);
                    if (isOnly)
                        outputText("only_result_db", sFileName.c_str(), nOption++);
                    else
                        outputText("result_db", sFileName.c_str(), nOption++);

                    ++iter;
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case BUILT_IN_LANG:
            {
                outputBuiltInLang(pNode->getOperationType(), *iter, nOption);
                break;
            }
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }

                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
                else if (iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    outputText(NULL, sConstraint.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::parseNARRDB(std::list<rcsToken_T>::iterator &iter,
                                   std::list<rcsToken_T>::iterator end,
                                   hvUInt32 &nOption)
{
    bool isOnly = false;
    std::string sErrValue = iter->sValue;
    if (++iter != end && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == ONLY)
    {
        isOnly = true;
        sErrValue += " ";
        sErrValue += iter->sValue;
        ++iter;
    }

    if (iter != end)
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            {
                if (!isValidSvrfName(iter->sValue))
                {
                    --iter;
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP4, iter->nLineNo,
                                        sErrValue);
                }
            }
            case STRING_CONSTANTS:
            {
                std::string sFileName;
                if (getStringName(iter, end, sFileName))
                {
                    if (!isOnly)
                        outputText("result_db", sFileName.c_str(), nOption++);
                    else
                        outputText("only_result_db", sFileName.c_str(), nOption++);
                }
                break;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP4, iter->nLineNo,
                                    sErrValue);
        }
    }
    else
    {
        --iter;
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP4, iter->nLineNo, sErrValue);
    }

    ++iter;
    while(iter != end)
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                return;
            case SECONDARY_KEYWORD:
            {
                if (iter->eKeyType == BY_LAYER)
                {
                    outputText("output_by_layer", NULL, nOption++);
                    break;
                }
                else if (iter->eKeyType == MAG)
                {
                    ++iter;
                    std::string sMagValue;
                    if (getNumberExp(iter, end, sMagValue))
                        outputText("mag", sMagValue.c_str(), nOption++);
                    continue;
                }
                else if (ACCUMULATE == iter->eKeyType || OVER == iter->eKeyType)
                {
                    return;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, end, sLayerName))
                {
                    outputText(NULL, sLayerName.c_str(), nOption++);
                    if (iter != end && iter->eType == SECONDARY_KEYWORD &&
                       iter->eKeyType == MAXIMUM)
                    {
                        if (++iter != end)
                        {
                            std::string sNumExp;
                            if (getNumberExp(iter, end, sNumExp))
                                outputText("max", sNumExp.c_str(), nOption++);
                        }
                        else
                        {
                            --iter;
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                                iter->nLineNo, iter->sValue);
                        }
                    }
                    continue;
                }
            }
            case BUILT_IN_LANG:
                return;
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo,
                                    iter->sValue);
        }
        ++iter;
    }
}

bool
rcsSynPvrsConvertor::parseNetAreaRatioLayer(std::list<rcsToken_T>::iterator &iter,
                                              std::list<rcsToken_T>::iterator end,
                                              hvUInt32 &nOption)
{
    if( (iter->eType != IDENTIFIER_NAME) || !parseTmpLayerForOperation(iter->sValue) )
    {
        std::string sLayerName;
        if (getLayerRefName(iter, end, sLayerName))
        {
            outputText(NULL, sLayerName.c_str(), nOption++);
        }
        else
        {
            return false;
        }
    }

    if (iter != end)
    {
        while(iter != end)
        {
            if (iter->eType == SECONDARY_KEYWORD)
            {
                if (iter->eKeyType == SCALE)
                {
                    if (++iter != end && iter->eKeyType == BY)
                    {
                        ++iter;
                        std::string sNumExp;
                        if (getNumberExp(iter, end, sNumExp))
                        {
                            outputText("mag_by", sNumExp.c_str(), nOption++);
                        }
                        continue;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                }
                else if (iter->eKeyType == COUNT)
                {
                    if (++iter != end && iter->eKeyType == ONLY)
                    {
                        outputText("count", NULL, nOption++);
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                }
                else if (iter->eKeyType == PERIMETER_ONLY)
                {
                    outputText("perimeter", NULL, nOption++);
                }
                else
                    break;
            }
            else
                break;
            ++iter;
        }

    }
    return true;
}

static bool
isNARKeyword(const rcsToken_T &token)
{
    if (token.eType == SECONDARY_KEYWORD)
    {
        switch(token.eKeyType)
        {
            case SCALE:
            case PERIMETER_ONLY:
            case INSIDE_OF_LAYER:
            case OVER:
            case RDB:
            case ACCUMULATE:
            case BY_LAYER:
            case COUNT:
                return true;
            default:
                return false;
        }
    }
    else if (token.eType == LAYER_OPERATION_KEYWORD || token.eType == BUILT_IN_LANG)
    {
        return true;
    }
    return false;
}

void
rcsSynPvrsConvertor::convCheckNetAreaRatioCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "NET AREA RATIO";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "check_nar ( ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (iter->eType == SECONDARY_KEYWORD)
        {
            switch(iter->eKeyType)
            {
                case OVER:
                {
                    outputText("divide", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case BY_NET:
                {
                    outputText("same_net", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case INSIDE_OF_LAYER:
                {
                    ++iter;
                    outputText("inside_of_layer", NULL, nOption++);
                    if (iter->eType == IDENTIFIER_NAME)
                    {
                        if(parseTmpLayerForOperation(iter->sValue))
                        {
                            break;
                        }
                    }
                    std::string sLayerName;
                    if (getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText(NULL, sLayerName.c_str(), nOption++);
                    }
                    continue;
                }
                case ACCUMULATE:
                {
                    outputText("incremental", NULL, nOption++);
                    ++iter;
                    while(iter != pNode->end())
                    {
                        std::string sLayerName;
                        if (isNARKeyword(*iter))
                        {
                            break;
                        }
                        else
                        {
                            if( (iter->eType != IDENTIFIER_NAME) || !parseTmpLayerForOperation(iter->sValue) )
                            {
                                if (getLayerRefName(iter, pNode->end(), sLayerName))
                                    outputText(NULL, sLayerName.c_str(), nOption++);
                            }
                        }
                    }
                    continue;
                }
                case RDB:
                    parseNARRDB(iter, pNode->end(), nOption);
                    continue;
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case BUILT_IN_LANG:
            {
                outputBuiltInLang(pNode->getOperationType(), *iter, nOption);
                break;
            }
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }

                if (parseNetAreaRatioLayer(iter, pNode->end(), nOption))
                {
                    continue;
                }
                else if (iter->eType == OPERATOR && isConstraintOperator(*iter))
                {
                    std::string sCons;
                    if (getConstraint(iter, pNode->end(), sCons))
                    {
                        outputText(NULL, sCons.c_str(), nOption++);
                        continue;
                    }
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}


void
rcsSynPvrsConvertor::convCheckNetAreaRatioByAccumulateCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "NET AREA RATIO ACCUMULATE";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "check_incremental_nar ( ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText(NULL, sLayerName.c_str(), nOption++);
                }
                else if (iter->eType == OPERATOR && isConstraintOperator(*iter))
                {
                    std::string sCons;
                    if (getConstraint(iter, pNode->end(), sCons))
                    {
                        outputText(NULL, sCons.c_str(), nOption++);
                    }
                }
                continue;
            }
            case BUILT_IN_LANG:
            {
                outputBuiltInLang(pNode->getOperationType(), *iter, nOption);
                break;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::_parseOffsetOption(std::list<rcsToken_T>::iterator &iter,
                                          const std::list<rcsToken_T>::iterator end,
                                          hvUInt32 &nOption)
{
    std::list<rcsToken_T>::iterator begin = iter;

    outputText("offset_value", NULL, nOption++);
    Utassert(iter->eType == SECONDARY_KEYWORD && iter->eKeyType == OFFSET);
    ++iter;
    if (iter->eType == SECONDARY_KEYWORD && (iter->eKeyType == INSIDE_BY ||
       iter->eKeyType == OUTSIDE_BY))
    {
        bool isOutside = iter->eKeyType == OUTSIDE_BY;
        std::string sNumExp;
        if (getNumberExp(++iter, end, sNumExp))
        {
            if (isOutside)
                outputText("outside_value", sNumExp.c_str(), nOption++);
            else
                outputText("inside_value", sNumExp.c_str(), nOption++);
        }
        return;
    }
    else
    {
        std::string sXExp;
        std::string sYExp;
        if (getNumberExp(iter, end, sXExp, true))
        {
            outputText(NULL, sXExp.c_str(), nOption);
            if (getNumberExp(iter, end, sYExp, true))
            {
                outputText(NULL, sYExp.c_str(), nOption);
            }
        }
    }

    if (iter->eType == SECONDARY_KEYWORD && (iter->eKeyType == INSIDE_BY ||
        iter->eKeyType == OUTSIDE_BY))
     {
        bool isOutside = iter->eKeyType == OUTSIDE_BY;
        std::string sNumExp;
        if (getNumberExp(++iter, end, sNumExp))
        {
            if (isOutside)
                outputText("outside_value", sNumExp.c_str(), nOption++);
            else
                outputText("inside_value", sNumExp.c_str(), nOption++);
        }
        return;
    }
}


void
rcsSynPvrsConvertor::convCheckOffgridCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "OFFGRID";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "check_offgrid ( ";

    bool hasLayerIn = false;
    bool hasXGrid = false;
    bool hasYGrid = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                switch(iter->eKeyType)
                {
                    case INSIDE_OF_LAYER:
                    {
                        ++iter;
                        outputText("inside_of_layer", NULL, nOption++);
                        if (iter->eType == IDENTIFIER_NAME)
                        {
                            if(parseTmpLayerForOperation(iter->sValue))
                            {
                                break;
                            }
                        }
                        std::string sLayerName;
                        if (getLayerRefName(iter, pNode->end(), sLayerName))
                        {
                           outputText(NULL, sLayerName.c_str(), nOption++);
                        }
                        continue;
                    }
                    case ABSOLUTE:
                    {
                        outputText(NULL, "original_coord", nOption);
                        ++iter;
                        continue;
                    }
                    case RELATIVE:
                    {
                        outputText(NULL, "new_coord", nOption);
                        ++iter;
                        continue;
                    }
                    case CENTERS:
                    {
                        outputText("output_center", NULL, nOption++);
                        ++iter;
                        continue;
                    }
                    case EDGE:
                    {
                        outputText("output_edge", NULL, nOption++);
                        ++iter;
                        continue;
                    }
                    case REGION:
                    {
                        outputText("output_region", NULL, nOption++);
                        ++iter;
                        if (iter->eType != SECONDARY_KEYWORD || iter->eKeyType != INSIDE_OF_LAYER)
                        {
                            std::string sNumExp;
                            if (getNumberExp(iter, pNode->end(), sNumExp))
                                outputText(NULL, sNumExp.c_str(), nOption);
                        }
                        continue;
                    }
                    case OFFSET:
                    {
                        _parseOffsetOption(iter, pNode->end(), nOption);
                        continue;
                    }
                    case HINT:
                    {
                        outputText("with_hint", NULL, nOption++);
                        ++iter;
                        continue;
                    }
                    default:
                        break;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            case SEPARATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (!hasLayerIn && getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    hasLayerIn = true;
                    outputText(NULL, sLayerName.c_str(), nOption++);
                    continue;
                }
                else if (!hasXGrid)
                {
                    std::string sNumExp;
                    if (getNumberExp(iter, pNode->end(), sNumExp))
                    {
                        hasXGrid = true;
                        outputText(NULL, sNumExp.c_str(), nOption++);
                        continue;
                    }
                }
                else if (!hasYGrid)
                {
                    std::string sNumExp;
                    if (getNumberExp(iter, pNode->end(), sNumExp))
                    {
                        hasYGrid = true;
                        outputText(NULL, sNumExp.c_str(), nOption);
                        continue;
                    }
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomOriginalCheckCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "check_original_layer ( ";
    switch(pNode->getOperationType())
    {
        case DRAWN_ACUTE:
            outputText(NULL, "acute_edge", nOption++);
            break;
        case DRAWN_ANGLED:
            outputText(NULL, "angled_edge", nOption++);
            break;
        case DRAWN_SKEW:
            outputText(NULL, "skew_edge", nOption++);
            break;
        case DRAWN_OFFGRID:
            outputText(NULL, "offgrid_vertex", nOption++);
            break;
        default:
            break;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomSelectDeviceLayerCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "device_seed_layer ( ";
    
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;
    outputText(NULL, iter->sValue.c_str(), nOption++);
    ++iter;
    if ((iter != pNode->end()) && (iter->sValue == "("))
    {
         ++iter;
         if (iter == pNode->end())
         {
             throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                 iter->nLineNo, iter->sValue);
         }
         switch(iter->eType)
         {
             case SECONDARY_KEYWORD:
             case IDENTIFIER_NAME:
             {
                 if (iter->eType == IDENTIFIER_NAME)
                 {
                     if(parseTmpLayerForOperation(iter->sValue))
                     {
                         break;
                     }
                 }
                 if (!isValidSvrfName(iter->sValue))
                 {
                     throw rcErrorNode_T(rcErrorNode_T::ERROR, DVL2,
                                         iter->nLineNo,
                                         iter->sValue);
                 }
             }
             case STRING_CONSTANTS:
             {
                 std::string sModelName;
                 sModelName = iter->sValue;
                 sModelName.insert(0, "(");
                 sModelName.insert(sModelName.size(), ")");
                 outputText(NULL, sModelName.c_str(), nOption++);
                 break;
             }
             default:
                 throw rcErrorNode_T(rcErrorNode_T::ERROR, DVL2,
                                     iter->nLineNo,
                                     iter->sValue);
         }
         ++iter;
         Utassert(iter->sValue == ")");
         ++iter;
    }

    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                if (iter->eKeyType == EXCLUDE_UNUSED)
                {
                    outputText("only_used", NULL, nOption++);
                    break;
                }
                else if (iter->eKeyType == PROPERTY)
                {
                    outputCon2Error(pNode, *iter);
                    break;
                }
                else if (iter->eKeyType == ANNOTATE)
                {
                    outputCon2Error(pNode, *iter);
                    break;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            default:
            {
                break;
            }
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convDfmOrEdge(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "dfm_edge_or ( ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch (iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            default:
            {
                switch (iter->eKeyType)
                {
                    case CONFLICTS:
                        outputText("conflict", NULL, nOption++);
                        break;
                    case BOTH:
                        outputText("both", NULL, nOption++);
                        break;
                    default:
                    {
                        if (iter->eType == IDENTIFIER_NAME)
                        {
                            if(parseTmpLayerForOperation(iter->sValue))
                            {
                                break;
                            }
                        }
                    }
                        outputText(NULL, iter->sValue.c_str(), nOption++);
                        break;
                }
            }
                break;
        }
        ++iter;
    }

    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convBuildDfmPropertyCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "DFM PROPERTY";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "dfm_build_property ( ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case INTERSECTING_ONLY:
                {
                    outputText("clip", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case OVERLAP:
                {
                    outputText("overlapped", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case ABUT_ALSO:
                {
                    outputText("adjacent", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case SINGULAR:
                {
                    outputText("point_touch", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case MULTI:
                {
                    outputText("multi_cluster", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case NOMULTI:
                {
                    outputText("single_cluster", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case NOPUSH:
                {
                    outputText("nopush", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case REGION:
                {
                    outputText("region", NULL, nOption++);
                    ++iter;
                    continue;
                }



















                case NODAL:
                {
                    outputText("nodal", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case CORNER:
                {
                    outputText("corner", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case GLOBALXY:
                {
                    outputText("globalxy", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case CONNECTED:
                {
                    outputText("same_net", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case NOT_CONNECTED:
                {
                    outputText("diff_net", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case INSIDE_OF:
                {
                    std::string sLeftX;
                    std::string sBottomY;
                    std::string sRightX;
                    std::string sTopY;
                    outputText("inside_of_region", NULL, nOption++);
                    ++iter;
                    getNumberExp(iter, pNode->end(), sLeftX, false);
                    getNumberExp(iter, pNode->end(), sBottomY, false);
                    getNumberExp(iter, pNode->end(), sRightX, false);
                    getNumberExp(iter, pNode->end(), sTopY, false);
                    outputText( sLeftX.c_str(), NULL, nOption++);
                    outputText( sBottomY.c_str(), NULL, nOption++);
                    outputText( sRightX.c_str(), NULL, nOption++);
                    outputText( sTopY.c_str(), NULL, nOption++);
                    continue;
                }
                case SPLIT:
                {
                    outputText("split", NULL, nOption++);
                    ++iter;
                    if (iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                       ALL == iter->eKeyType)
                    {
                        outputText("all", NULL, nOption++);
                    }
                    else if (iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                            PRIMARY == iter->eKeyType)
                    {
                        outputText("top_cell", NULL, nOption++);
                    }
                    else
                    {
                        ;
                        continue;
                    }
                    ++iter;
                    continue;
                }
                case ACCUMULATE:
                {
                    outputText("accumulate", NULL, nOption++);
                    ++iter;
                    if (iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                       ONLY == iter->eKeyType)
                    {
                        outputText("only", NULL, nOption++);
                        ++iter;
                    }
                    continue;
                }
                case BY_NET:
                {
#if 0
                    outputCon2Error(pNode, *iter);
                    rcsToken_T key = *iter;
                    ++iter;
                    if (iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                       ONLY == iter->eKeyType)
                    {
                        ;
                    }
                    else
                    {
                        continue;
                    }
                    ++iter;
                    continue;
#endif
                    outputText("output_by_net", NULL, nOption++);
                    ++iter;

                    
                    continue;
                }
                case NOHIER:
                {
                    outputText("isolate_cell", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case NOPSEUDO:
                {
                    outputText("user_cell", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case BY_CELL:
                {
                    outputText("by_cell", NULL, nOption++);
                    ++iter;
                    if (iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                       ONLY == iter->eKeyType)
                    {
                        outputText("only", NULL, nOption++);
                        ++iter;
                    }
                    continue;
                }
                case POINT:
                {
                    ++iter; 
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case BUILT_IN_LANG:
            {
#if 0
                if(!outputBuiltInLangInTrs(*iter, nOption))
                {
                    std::string sValue = iter->sValue;
                    sValue.erase(0, 1);
                    sValue.insert(0, "[ ");
                    sValue.erase(sValue.size() - 1);
                    sValue.insert(sValue.size(), " ]");
                    outputText(NULL, sValue.c_str(), nOption);
                }
#else
                outputBuiltInLang(pNode->getOperationType(), *iter, nOption);
#endif
                ++iter;
                if (iter != pNode->end() && iter->eType == OPERATOR &&
                  (iter->sValue == "!" || isConstraintOperator(*iter)))
                {
                    std::string sCons;
                    if (iter->sValue == "!")
                    {
                        sCons += "!";
                        ++iter;
                    }
                    if (getConstraint(iter, pNode->end(), sCons))
                    {
                        outputText(NULL, sCons.c_str(), nOption++);
                    }
                }
                continue;
            }
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                    outputText(NULL, sLayerName.c_str(), nOption++);
                continue;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convDfmPropertySelectSecondaryCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "DFM PROPERTY SELECT SECONDARY";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "dfm_build_property_select_assistant ( ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case INTERSECTING_ONLY:
                {
                    outputText("clip", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case OVERLAP:
                {
                    outputText("overlapped", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case ABUT_ALSO:
                {
                    outputText("adjacent", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case SINGULAR:
                {
                    outputText("point_touch", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case MULTI:
                {
                    outputText("multi_cluster", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case NOMULTI:
                {
                    outputText("single_cluster", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case NOPUSH:
                {
                    outputText("nopush", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case REGION:
                {
                    outputText("region", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case UNMERGED:
                {
                    outputText("unmerged_region", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case CORNER:
                {
                    outputText("corner", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case SELECT:
                {
                    outputText("select", NULL, nOption++);
                    ++iter;
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case BUILT_IN_LANG:
            {
                outputBuiltInLang(pNode->getOperationType(), *iter, nOption);
                ++iter;
                if (iter != pNode->end() && iter->eType == OPERATOR &&
                  (iter->sValue == "!" || isConstraintOperator(*iter)))
                {
                    std::string sCons;
                    if (iter->sValue == "!")
                    {
                        sCons += "!";
                        ++iter;
                    }
                    if (getConstraint(iter, pNode->end(), sCons))
                    {
                        outputText(NULL, sCons.c_str(), nOption++);
                    }
                }
                continue;
            }
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                    outputText(NULL, sLayerName.c_str(), nOption++);
                continue;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}
void
rcsSynPvrsConvertor::convBuildDfmPropertyNetCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "dfm_build_property_by_net ( ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    iter++;
    while(iter != pNode->end())
    {
        (*m_oPVRSStream) << iter->sValue << " ";
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convDfmSpaceCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "DFM SPACE";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "dfm_check_space ( ";
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case BY_INT:
                    (*m_oPVRSStream) << "by_int ";
                    break;
                case BY_ENC:
                    (*m_oPVRSStream) << "by_enc ";
                    break;
                case BY_EXT:
                    (*m_oPVRSStream) << "by_ext ";
                    break;
                case ALL:
                    (*m_oPVRSStream) << "by_all ";
                    break;
                default:
                    break;
            }
        }
        ++iter;
    }

    iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case BY_INT:
                case BY_ENC:
                case BY_EXT:
                case ALL:
                {
                    ++iter;
                    continue;
                }
                case GRID:
                {
                    std::string sNumExp;
                    if (getNumberExp(++iter, pNode->end(), sNumExp))
                        outputText("delta", sNumExp.c_str(), nOption++);
                    continue;
                }
                case HORIZONTAL:
                {
                    outputText("x_direction", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case VERTICAL:
                {
                    outputText("y_direction", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case MEASURE_ALL:
                {
                    outputText("check_all", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case BY_LAYER:
                {
                    ++iter;
                    outputText("shield_layer", NULL, nOption++);
                    if (iter->eType == IDENTIFIER_NAME)
                    {
                        if(parseTmpLayerForOperation(iter->sValue))
                        {
                            break;
                        }
                    }
                    std::string sLayerName;
                    if (!getLayerRefName(iter, pNode->end(), sLayerName))
                        sLayerName = iter->sValue;
                    outputText(NULL, sLayerName.c_str(), nOption++);
                    continue;
                }
                case CONNECTED:
                {
                    outputText("same_net", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case NOT_CONNECTED:
                {
                    outputText("diff_net", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case COUNT:
                {
                    std::string sCons;
                    if (getConstraint(++iter, pNode->end(), sCons))
                        outputText("shield_level", sCons.c_str(), nOption++);
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText(NULL, sLayerName.c_str(), nOption++);
                }
                else if (iter->eType == OPERATOR && isConstraintOperator(*iter))
                {
                    std::string sCons;
                    if (getConstraint(iter, pNode->end(), sCons))
                        outputText(NULL, sCons.c_str(), nOption++);
                }
                continue;
            }
            case BUILT_IN_LANG:
            {
#if 0
                if(!outputBuiltInLangInTrs(*iter, nOption))
                {
                    std::string sValue = iter->sValue;
                    sValue.erase(0, 1);
                    sValue.erase(sValue.size() - 1);
                    trim(sValue);
                    if(sValue[0] == '\"' || sValue[0] == '\'')
                    {
                        trim(sValue, "\"\'");
                        sValue.insert(0, "[\"");
                        sValue.insert(sValue.size(), "\"]");
                    }
                    else
                    {
                        sValue.insert(0, "[");
                        sValue.insert(sValue.size(), "]");
                    }
                    outputText(NULL, sValue.c_str(), nOption++);
                }
#else
                outputBuiltInLang(pNode->getOperationType(), *iter, nOption);
#endif
                break;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void rcsSynPvrsConvertor::convDfmShiftEdge(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "DFM SHIFT EDGE";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "dfm_move_edge ( ";
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while (iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case OUTSIDE_BY:
                {
                    ++iter;
                    std::string sPropName;
                    getStringName(iter, pNode->end(), sPropName);
                    std::string sNumExp;
                    getNumberExp(iter, pNode->end(), sNumExp);
                    std::string str = sPropName.empty() ? sNumExp : sPropName;
                    outputText("outside_by", str.c_str(), nOption++);
                    continue;
                }
                case INSIDE_BY:
                {
                    ++iter;
                    std::string sPropName;
                    getStringName(iter, pNode->end(), sPropName);
                    std::string sNumExp;
                    getNumberExp(iter, pNode->end(), sNumExp);
                    std::string str = sPropName.empty() ? sNumExp : sPropName;
                    outputText("inside_by", str.c_str(), nOption++);
                    continue;
                }
                case EXTEND_PRODUCED_BY:
                case EXTEND_BY:
                {
                    bool isProduced = iter->eKeyType == EXTEND_PRODUCED_BY;
                    ++iter;
                    std::string sPropName;
                    getStringName(iter, pNode->end(), sPropName);
                    std::string sNumExp;
                    getNumberExp(iter, pNode->end(), sNumExp);
                    std::string str = sPropName.empty() ? sNumExp : sPropName;
                    std::string key = isProduced ? "extend_produced_by" : "extend_by";
                    outputText(key.c_str(), str.c_str(), nOption++);
                    continue;
                }
                case OFFSET:
                {
                    ++iter;
                    std::string sPropName;
                    getStringName(iter, pNode->end(), sPropName);
                    std::string sNumExp;
                    getNumberExp(iter, pNode->end(), sNumExp);
                    std::string str = sPropName.empty() ? sNumExp : sPropName;
                    outputText("offset", str.c_str(), nOption++);
                    continue;
                }
                case SINGLE:
                {
                    outputText("single", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case FLIP:
                {
                    outputText("flip", NULL, nOption++);
                    ++iter;
                    continue;
                }
                default:
                    break;
            }
        }

        switch (iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            case BUILT_IN_LANG:
            {
#if 0
                if(!outputBuiltInLangInTrs(*iter, nOption))
                {
                    std::string sValue = iter->sValue;
                    sValue.erase(0, 1);
                    sValue.erase(sValue.size() - 1);
                    trim(sValue);
                    if(sValue[0] == '\"' || sValue[0] == '\'')
                    {
                        trim(sValue, "\"\'");
                        sValue.insert(0, "[\"");
                        sValue.insert(sValue.size(), "\"]");
                    }
                    else
                    {
                        sValue.insert(0, "[");
                        sValue.insert(sValue.size(), "]");
                    }
                    outputText(NULL, sValue.c_str(), nOption++);
                }
#else
                outputBuiltInLang(pNode->getOperationType(), *iter, nOption);
#endif
                break;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;


        }
        ++iter;
    }

    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void rcsSynPvrsConvertor::convDfmDV(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "DFM DV";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "dfm_dv ( ";
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case DVPARAMS:
                {
                    outputText("dv_param ", NULL, nOption++);

                    while (++iter != pNode->end() && SECONDARY_KEYWORD != iter->eType)
                    {
                        outputText(iter->sValue.c_str(), NULL, nOption++);
                    }

                    continue;
                }
                case ANNOTATE:
                {
                    outputText("remark", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case UNIDIRECTIONAL:
                {
                    outputText("unidirectional", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case NONTRIVIAL_ONLY:
                {
                    outputText("nontrivial_only", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case OPPOSITE:
                {
                    outputText(NULL, "opposite", nOption++);
                    ++iter;
                    continue;
                }
                case OPPOSITE_EXTENDED:
                {
                    outputText(NULL, "extended_opposite", nOption++);
                    ++iter;
                    if (iter != pNode->end() && !isDimensionalCheckKeyword(iter, pNode->end()))
                    {
                        std::string sNumExp;
                        if (getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            outputText(NULL, sNumExp.c_str(), nOption);
                        }
                    }
                    continue;
                }
                case MEASURE_ALL:
                {
                    outputText("no_shielded_edge", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case CONNECTED:
                {
                    outputText("same_net", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case NOT_CONNECTED:
                {
                    outputText("diff_net", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case CORNER_TO_CORNER:
                {
                    outputText("corner_corner",NULL, nOption++);
                    ++iter;
                    if (iter != pNode->end() && isConstraintOperator(*iter))
                    {
                        std::list<rcsToken_T>::iterator begin = iter;
                        std::string sCons;
                        if (getConstraint(iter, pNode->end(), sCons))
                        {
                            outputText(NULL, sCons.c_str(), nOption++);
                            continue;
                        }
                    }
                    continue;
                }
                case SINGULAR:
                {
                    outputText("point_touch", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case INSIDE_ALSO:
                {
                    outputText("inside_also", NULL,  nOption++);
                    ++iter;
                    continue;
                }
                case PROJECTING:
                {
                   outputText("apposition", NULL,  nOption++);
                   ++iter;
                   continue;
                }
                case WITH_DEFAULT_VOLTAGES:
                {
                    outputText("with_default_voltage", NULL,  nOption++);
                    ++iter;
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText(NULL, sLayerName.c_str(), nOption++);
                }
                else if (iter->eType == OPERATOR && isConstraintOperator(*iter))
                {
                    std::string sCons;
                    if (getConstraint(iter, pNode->end(), sCons))
                        outputText(NULL, sCons.c_str(), nOption++);
                }
                continue;
            }

            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convDfmSizeCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "DFM SIZE";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "dfm_geom_size ( ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case BY:
                {
                    ++iter;
                    std::list<rcsToken_T>::iterator begin = iter;
                    std::string sPropName;
                    getStringName(iter, pNode->end(), sPropName);
                    std::string sNumExp;
                    getNumberExp(iter, pNode->end(), sNumExp);
                    if (!sPropName.empty())
                        outputText("by", sPropName.c_str(), nOption++);
                    else if (!sNumExp.empty())
                        outputText("by", sNumExp.c_str(), nOption++);
                    continue;
                }
                case INSIDE_OF:
                {
                    ++iter;
                    outputText("inside_of_layer", NULL, nOption++);
                    if (iter->eType == IDENTIFIER_NAME)
                    {
                        if(parseTmpLayerForOperation(iter->sValue))
                        {
                            break;
                        }
                    }
                    std::string sLayerName;
                    if (getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                       outputText(NULL, sLayerName.c_str(), nOption++);
                       continue;
                    }
                }
                case OUTSIDE_OF:
                {
                    ++iter;
                    outputText("outside_of_layer", NULL, nOption++);
                    if (iter->eType == IDENTIFIER_NAME)
                    {
                        if(parseTmpLayerForOperation(iter->sValue))
                        {
                            break;
                        }
                    }

                    std::string sLayerName;
                    if (getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                       outputText(NULL, sLayerName.c_str(), nOption++);
                       continue;
                    }
                }
                case STEP:
                {
                    ++iter;
                    std::list<rcsToken_T>::iterator begin = iter;
                    std::string sPropName;
                    getStringName(iter, pNode->end(), sPropName);
                    std::string sNumExp;
                    getNumberExp(iter, pNode->end(), sNumExp);
                    if (!sPropName.empty())
                        outputText("delta", sPropName.c_str(), nOption++);
                    else if (!sNumExp.empty())
                        outputText("delta", sNumExp.c_str(), nOption++);
                    continue;
                }
                case TRUNCATE:
                {
                    ++iter;
                    std::string sNumExp;
                    if (getNumberExp(iter, pNode->end(), sNumExp))
                        outputText("clip", sNumExp.c_str(), nOption++);
                    continue;
                }
                case BEVEL:
                {
                    ++iter;
                    std::string sNumExp;
                    if (getNumberExp(iter, pNode->end(), sNumExp))
                        outputText("clip_corner", sNumExp.c_str(), nOption++);
                    continue;
                }
                case OVERLAP_ONLY:
                {
                    outputText("output_overlap_region", NULL, nOption++);
                    ++iter;
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}


void
rcsSynPvrsConvertor::convEdgeSelectByConvexCornerCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "CONVEX EDGE";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "edge_convex_point ( ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case ANGLE1:
                {
                    std::string sConstraint;
                    getConstraint(++iter, pNode->end(), sConstraint);
                    continue;
                }
                case ANGLE2:
                {
                    std::string sConstraint;
                    getConstraint(++iter, pNode->end(), sConstraint);
                    continue;
                }
                case LENGTH1:
                {
                    std::string sConstraint;
                    getConstraint(++iter, pNode->end(), sConstraint);
                    continue;
                }
                case LENGTH2:
                {
                    std::string sConstraint;
                    getConstraint(++iter, pNode->end(), sConstraint);
                    continue;
                }
                case WITH_LENGTH:
                {
                    std::string sConstraint;
                    getConstraint(++iter, pNode->end(), sConstraint);
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
                else if (iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    outputText(NULL, sConstraint.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case ANGLE1:
                {
                    std::string sConstraint;
                    getConstraint(++iter, pNode->end(), sConstraint);
                    outputText("adjacent_edge_angle1", sConstraint.c_str(), nOption++);
                    continue;
                }
                case ANGLE2:
                {
                    std::string sConstraint;
                    getConstraint(++iter, pNode->end(), sConstraint);
                    outputText("adjacent_edge_angle2", sConstraint.c_str(), nOption++);
                    continue;
                }
                case LENGTH1:
                {
                    std::string sConstraint;
                    getConstraint(++iter, pNode->end(), sConstraint);
                    outputText("adjacent_edge_length1", sConstraint.c_str(), nOption++);
                    continue;
                }
                case LENGTH2:
                {
                    std::string sConstraint;
                    getConstraint(++iter, pNode->end(), sConstraint);
                    outputText("adjacent_edge_length2", sConstraint.c_str(), nOption++);
                    continue;
                }
                case WITH_LENGTH:
                {
                    std::string sConstraint;
                    getConstraint(++iter, pNode->end(), sConstraint);
                    outputText("with_edge_length", sConstraint.c_str(), nOption++);
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   continue;
                }
                else if (iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomCopyDfmLayerCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "DFM COPY";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "dfm_copy ( ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case REGION:
                {
                    if (++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                       UNMERGED == iter->eKeyType)
                    {
                        outputText(NULL, "unmerged_region", nOption++);
                    }
                    else
                    {
                        outputText(NULL, "region", nOption++);
                        --iter;
                    }
                    ++iter;
                    continue;
                }
                case TIE:
                {
                    if (++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                       CENTER == iter->eKeyType)
                    {
                        outputText(NULL, "connecting_midpoint", nOption++);
                    }
                    else
                    {
                        outputText(NULL, "connecting_line", nOption++);
                        --iter;
                    }
                    ++iter;
                    continue;
                }
                case CLUSTER:
                {
                    if (++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                       LAYERID == iter->eKeyType)
                    {
                        outputText("edge_collection", "layer_id", nOption++);
                    }
                    else
                    {
                        outputText(NULL, "edge_collection", nOption++);
                        --iter;
                    }
                    ++iter;
                    continue;
                }
                case TIE_CENTER:
                {
                    outputText(NULL, "connecting_midpoint", nOption++);
                    ++iter;
                    continue;
                }
                case CENTERLINE_CLUSTER:
                {
                    outputText(NULL, "middle_line edge_collection", nOption++);
                    ++iter;
                    continue;
                }
                case TIE_CENTER_CLUSTER:
                {
                    outputText(NULL, "connecting_midpoint edge_collection", nOption++);
                    ++iter;
                    continue;
                }
                case TIE_CLUSTER:
                {
                    outputText(NULL, "connecting_line edge_collection", nOption++);
                    ++iter;
                    continue;
                }
                case EDGE:
                    outputText(NULL, "edge", nOption++);
                    ++iter;
                    continue;
                case CENTERLINE:
                    outputText(NULL, "middle_line", nOption++);
                    ++iter;
                    continue;
                case UNMERGED:
                    outputText(NULL, "unmerged_edge", nOption++);
                    ++iter;
                    continue;
                case CELL:
                {
                    if (++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                       LIST == iter->eKeyType)
                    {
                        if (++iter == pNode->end())
                        {
                            --iter;
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, SPC2,
                                                iter->nLineNo, "CELL LIST");
                        }
                        else
                        {
                            switch(iter->eType)
                            {
                                case SECONDARY_KEYWORD:
                                case IDENTIFIER_NAME:
                                case STRING_CONSTANTS:
                                {
                                    std::string sCellListName;
                                    if (getStringName(iter, pNode->end(), sCellListName))
                                        outputText("cell_group", sCellListName.c_str(), nOption++);
                                    break;
                                }
                                default:
                                    throw rcErrorNode_T(rcErrorNode_T::ERROR,
                                                        INP1, iter->nLineNo,
                                                        iter->sValue);
                                    break;
                            }
                        }
                        ++iter;
                        continue;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void rcsSynPvrsConvertor::convDfmMergePropertyCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "DFM PROPERTY MERGE";
    (*m_oPVRSStream) << "dfm_merge_property ( ";
    hvUInt32 nOption = 0;
    std::list<rcsToken_T>::iterator itr = pNode->begin();
    while (itr != pNode->end())
    {
        if (itr->eType == SECONDARY_KEYWORD)
        {
            switch (itr->eKeyType)
            {
                case ABUT_ALSO:
                    outputText("adjacent", "", nOption++);
                    break;
                case MULTI:
                    outputText("multi_cluster", "", nOption++);
                    break;
                case NOMULTI:
                    outputText("single_cluster", "", nOption++);
                    break;
                case EXACT:
                    outputText("match", "", nOption++);
                    break;
                default:
                    outputCon2Error(pNode,*itr);
                    break;
            }

        }
        else if (itr->eType == BUILT_IN_LANG)
        {
            outputBuiltInLang(pNode->getOperationType(), *itr, nOption);
            ++itr;

            if (itr != pNode->end() && itr->eType == OPERATOR &&
              (itr->sValue == "!" || isConstraintOperator(*itr)))
            {
                std::string sCons;
                if (itr->sValue == "!")
                {
                    sCons += "!";
                    ++itr;
                }

                if (getConstraint(itr, pNode->end(), sCons))
                {
                    outputText(NULL, sCons.c_str(), nOption++);
                }
                else
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, itr->nLineNo,
                                        itr->sValue);
                }
                continue;
            }
            else
            {
                --itr;
            }
        }
        else if(itr->eType == LAYER_OPERATION_KEYWORD)
        {

        }
        else
        {
            if (itr->eType == IDENTIFIER_NAME)
            {
                if(parseTmpLayerForOperation(itr->sValue))
                {
                    ++itr;
                    continue;
                }
            }

            std::string sLayerName;
            if (getLayerRefName(itr, pNode->end(), sLayerName))
            {
                outputText(NULL, sLayerName.c_str(), nOption++);
                continue;
            }
        }
        ++itr;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
    {
        (*m_oPVRSStream) << std::endl;
    }
}

void
rcsSynPvrsConvertor::convDfmNARACCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "DFM NARAC";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "dfm_create_nar ( ";
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType && iter->eKeyType == BY)
        {
            ++iter;
            if (iter == pNode->end() || (iter->eType != STRING_CONSTANTS &&
               iter->eType != SECONDARY_KEYWORD && iter->eType != IDENTIFIER_NAME) ||
               (iter->eType == IDENTIFIER_NAME && !isValidSvrfName(iter->sValue)))
            {
                --iter;
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP15, iter->nLineNo,
                                    iter->sValue);
            }

            Utassert(iter->eType == STRING_CONSTANTS || iter->eType ==
                     SECONDARY_KEYWORD || iter->eType == IDENTIFIER_NAME);
            std::string sValue;
            if (getStringName(iter, pNode->end(), sValue))
                outputText("by_property",  sValue.c_str(), nOption++);
            ++iter;
            continue;
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convTvfCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    (*m_oPVRSStream) << "trs (";
    ++iter;
    while(iter != pNode->end())
    {
        outputText("",  iter->sValue.c_str(), nOption);
        ++iter;
    }

    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convDfmTextCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "DFM TEXT";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "dfm_text ( ";
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType && iter->eKeyType == PROPERTY)
        {
            outputText("text_property", NULL, nOption++);

            ++iter;
            if(iter != pNode->end())
            {
                if((iter->eType == SECONDARY_KEYWORD && iter->eKeyType == STRING) ||
                   (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == NUMBER_SECOND))
                {
                    if(iter->eKeyType == STRING)
                        outputText("string", NULL, nOption++);
                    else
                        outputText("number", NULL,  nOption++);

                    ++iter;
                    std::string sPropName;
                    if(getStringName(iter, pNode->end(), sPropName))
                    {
                        outputText(sPropName.c_str(), NULL, nOption++);
                        ++iter;
                        continue;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP15, iter->nLineNo,
                                            iter->sValue);
                    }
                }
            }
            continue;
        }
        else if (iter->eType == SECONDARY_KEYWORD && PRIMARY == iter->eKeyType)
        {
            if (++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType && ONLY == iter->eKeyType)
            {
                outputText("top_cell", NULL, nOption++);
                ++iter;
                continue;
            }
            else
            {
                --iter;
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                    iter->nLineNo, iter->sValue);
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText(NULL, sLayerName.c_str(), nOption++);
                    continue;
                }
                else
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convDfmRDBCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "DFM RDB";
    hvUInt32 nOption = 0;

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    ++iter;

    (*m_oPVRSStream) << "dfm_result ( ";
    if( (iter->eType != IDENTIFIER_NAME) || !parseTmpLayerForOperation(iter->sValue))
    {
        std::string sLayerName;
        if (!getLayerRefName(iter, pNode->end(), sLayerName))
        {
           outputCon1Error(pNode);
           return;
        }
        outputText(NULL, sLayerName.c_str(), nOption++);
    }

    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case CSG:
                {
                    outputCon2Error(pNode,*iter);
                    ++iter;
                    continue;
                }
                case NULL_SECOND:
                {
                    outputCon2Error(pNode,*iter);
                    ++iter;
                    continue;
                }
                case NOPSEUDO:
                {
                    
                    
                    outputText("not_keep_injected_cell", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case NODAL:
                {
#if 0
                    ++iter;
                    if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       (iter->eKeyType == ALL || iter->eKeyType == OTHER))
                    {
                        if (iter->eKeyType == ALL)
                        {
                            outputText("all_node", NULL, nOption++);
                        }
                        else
                        {
                            outputText("other_node", NULL, nOption++);
                        }
                        ++iter;
                    }
                    else
                    {
                        outputText("first_node", NULL, nOption++);
                    }
                    continue;
#endif
                    outputText("node", NULL, nOption++);
                    if (++iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       (iter->eKeyType == ALL || iter->eKeyType == OTHER))
                    {
                        if (iter->eKeyType == ALL)
                        {
                            outputText("all", NULL, nOption++);
                        }
                        else
                        {
                            outputText("other", NULL, nOption++);
                        }
                        ++iter;
                    }
                    continue;
                }
                case OUTPUT:
                {
                    outputText("output_boundary", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case JOINED:
                {
                    outputCon2Error(pNode,*iter);
                    ++iter;
                    continue;
                }
                case TRANSITION:
                {
                    outputCon2Error(pNode,*iter);
                    ++iter;
                    continue;
                }
                case ABUT_ALSO:
                {
                    outputCon2Error(pNode,*iter);
                    ++iter;
                    continue;
                }
                case NOEMPTY:
                {
                    outputText("ignore_empty", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case SAME:
                {
                    outputCon2Error(pNode,*iter);
                    ++iter;
                    if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       iter->eKeyType == CELL)
                    {
                        ++iter;
                        continue;
                    }
                    --iter;
                    break;
                }
                case RESULT_CELLS:
                {
                    outputText("result_cells", NULL, nOption++);
                    ++iter;
                    std::string sListName;
                    if (getStringName(iter, pNode->end(), sListName))
                    {
                        outputText(NULL, sListName.c_str(), nOption++);;
                    }
                    ++iter;
                    continue;
                }
                case ALL_CELLS:
                {
                    ++iter;
                    outputText("all_cell", NULL, nOption++);
                    continue;
                }
                case CHECKNAME:
                {
                    outputText("rule_name", NULL, nOption++);
                    ++iter;
                    std::string checkName;
                    getStringName(iter, pNode->end(), checkName);
                    outputText(checkName.c_str(), NULL);
                    ++iter;
                    continue;
                }
                case COMMENT_SECOND:
                {

                    outputText("comment", NULL, nOption++);
                    ++iter;
                    std::string sComment;
                    getStringName(iter, pNode->end(), sComment);
                    outputText(sComment.c_str(), NULL);
                    ++iter;
                    continue;
                }
                case ANNOTATE:
                {
                    outputCon2Error(pNode,*iter);
                    ++iter;
                    if (iter != pNode->end() && iter->eType == BUILT_IN_LANG)
                    {
                        std::string sValue = iter->sValue;
                        sValue.erase(0, 1);
                        sValue.insert(0, "[ ");
                        sValue.erase(sValue.size() - 1);
                        sValue.insert(sValue.size(), " ]");
                        break;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP18,
                                            iter->nLineNo, iter->sValue);
                    }
                    ++iter;
                    continue;
                }
                case MAXIMUM:
                {
                    outputText("max_result", NULL, nOption++);
                    if (++iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       iter->eKeyType == ALL)
                    {
                        outputText("all", NULL, nOption++);
                        ++iter;
                        continue;
                    }
                    else
                    {
                        std::string sNumExp;
                        if (getNumberExp(iter, pNode->end(), sNumExp))
                            outputText(NULL, sNumExp.c_str(), nOption++);
                        continue;
                    }
                }
                default:
                    break;
            }
        }

        outputOriginString(iter);
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomGetLayerBBoxCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "EXTENT";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "geom_get_boundary ( ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            case SEPARATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText(NULL, sLayerName.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomGetNetByNameCmd(rcsSynLayerOperationNode_T *pNode)
{
    if (pNode->getOperationType() == NET)
        m_sCurCommandString = "NET";
    else if (pNode->getOperationType() == NOT_NET)
        m_sCurCommandString = "NOT NET";
    hvUInt32 nOption = 0;
    if (pNode->getOperationType() == NET)
        (*m_oPVRSStream) << "geom_net ( by_layer ";
    else if (pNode->getOperationType() == NOT_NET)
        (*m_oPVRSStream) << "~geom_net ( by_layer ";

    bool hasInLayer = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                if (!isValidSvrfName(iter->sValue))
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
            }
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (!hasInLayer)
                {
                    hasInLayer = true;
                    std::string sLayerName;
                    if (getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                       outputText(NULL, sLayerName.c_str(), nOption++);
                       continue;
                    }
                }
                else if (iter->eType != NUMBER && iter->eType != OPERATOR)
                {
                    std::string sValue;
                    if (getStringName(iter, pNode->end(), sValue))
                    {
                        if (m_vVariableNames.find(iter->sValue) != m_vVariableNames.end())
                        {
                            sValue = iter->sValue;
                            toLower(sValue);
                        }
                        outputText(NULL, sValue.c_str(), nOption++);
                        break;
                    }
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convDfmStampCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "DFM STAMP";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "dfm_transfer_netid ( ";
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType && iter->eKeyType == BY)
        {
            ++iter;
            if (iter == pNode->end() || (iter->eType != STRING_CONSTANTS &&
               iter->eType != SECONDARY_KEYWORD && iter->eType != IDENTIFIER_NAME) ||
               (iter->eType == IDENTIFIER_NAME && !isValidSvrfName(iter->sValue)))
            {
                --iter;
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP15, iter->nLineNo,
                                    iter->sValue);
            }

            Utassert(iter->eType == STRING_CONSTANTS || iter->eType ==
                     SECONDARY_KEYWORD || iter->eType == IDENTIFIER_NAME);
            outputText(" by",NULL,nOption++);
            std::string sValue;
            if (getStringName(iter, pNode->end(), sValue))
                outputText(NULL,  sValue.c_str(), nOption++);
            ++iter;
            continue;
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convEdgeSelectByAttrCmd(rcsSynLayerOperationNode_T *pNode)
{
    switch(pNode->getOperationType())
    {
        case PATH_LENGTH:
            m_sCurCommandString = "PATH LENGTH";
            break;
        case LENGTH:
            m_sCurCommandString = "LENGTH";
            break;
        case NOT_LENGTH:
            m_sCurCommandString = "NOT LENGTH";
            break;
        case ANGLE:
            m_sCurCommandString = "ANGLE";
            break;
        case NOT_ANGLE:
            m_sCurCommandString = "NOT ANGLE";
            break;
        default:
            break;
    }
    hvUInt32 nOption = 0;
    switch(pNode->getOperationType())
    {
        case PATH_LENGTH:
            outputText(NULL, "edge_path_length ( ", nOption++);
            break;
        case LENGTH:
            outputText(NULL, "edge_length ( ", nOption++);
            break;
        case NOT_LENGTH:
            outputText(NULL, "~edge_length ( ", nOption++);
            break;
        case ANGLE:
            outputText(NULL, "edge_angle ( ", nOption++);
            break;
        case NOT_ANGLE:
            outputText(NULL, "~edge_angle ( ", nOption++);
            break;
        default:
            break;
    }

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
                else if (iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    outputText(NULL, sConstraint.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::parseRuleValue(std::list<rcsToken_T>::iterator &iter,
                                      std::list<rcsToken_T>::iterator end)
{
    std::string sValue;
    if (getNumberExp(iter, end, sValue))
        outputText(NULL, sValue.c_str());
    if (iter != end && SECONDARY_KEYWORD == iter->eType)
    {
        switch(iter->eKeyType)
        {
            case OPPOSITE_EXTENDED:
            {
                outputText(NULL, "extended_opposite");

                std::string sNumExp;
                if (getNumberExp(++iter, end, sNumExp))
                    outputText(NULL, sNumExp.c_str());
                return;
            }
            case OPPOSITE:
                outputText(NULL, "opposite");
                ++iter;
                return;
            case SQUARE:
                outputText(NULL, "square");
                ++iter;
                return;
            default:
                break;
        }
    }
}

void
rcsSynPvrsConvertor::parseRectRule(std::list<rcsToken_T>::iterator &iter,
                                     std::list<rcsToken_T>::iterator end)
{
    Utassert(GOOD == iter->eKeyType || BAD == iter->eKeyType);
    

    if (iter->eKeyType == GOOD)
        outputText(NULL, "correct");
    else
        outputText(NULL, "incorrect");

    ++iter;
    parseRuleValue(iter, end);
    parseRuleValue(iter, end);
    parseRuleValue(iter, end);
    parseRuleValue(iter, end);

}

void
rcsSynPvrsConvertor::convCheckRectEncCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "RECTANGLE ENCLOSURE";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "enc_rect ( ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case OUTSIDE_ALSO:
                {
                    outputText("outside_also", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case ORTHOGONAL:
                {
                    if (++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType
                       && ONLY == iter->eKeyType)
                    {
                        outputText(NULL, "only_orthogonal", nOption++);
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                    ++iter;
                    continue;
                }
                case GOOD:
                case BAD:
                {
                    parseRectRule(iter, pNode->end());
                    continue;
                }
                case ABUT:
                {
                    ++iter;
                    if (iter != pNode->end() && isConstraintOperator(*iter))
                    {
                        std::string sCons;
                        if (getConstraint(iter, pNode->end(), sCons))
                            outputText("adjacent", sCons.c_str(), nOption++);
                    }
                    else
                    {
                        outputText("adjacent", NULL, nOption++);
                    }
                    continue;
                }
                case SINGULAR:
                    outputText("point_touch", NULL, nOption++);
                    ++iter;
                    continue;
                default:
                    break;
            }
        }
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomFlattenLayerCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "FLATTEN";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "geom_flatten ( ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            case SEPARATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText(NULL, sLayerName.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convEdgeExpandToRectCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "EXPAND EDGE";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "geom_edge_to_rect ( ";

    std::list<rcsToken_T>::iterator  iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case INSIDE_BY:
                case INSIDE_BY_FACTOR:
                {
                    std::string sOption = "inside";
                    if (INSIDE_BY_FACTOR == iter->eKeyType)
                        sOption = "inside_by_factor";
                    ++iter;
                    std::string sNumExp;
                    if (getNumberExp(iter, pNode->end(), sNumExp))
                        outputText(sOption.c_str(), sNumExp.c_str(), nOption++);
                    continue;
                }
                case OUTSIDE_BY:
                case OUTSIDE_BY_FACTOR:
                {
                    std::string sOption = "outside";
                    if (OUTSIDE_BY_FACTOR == iter->eKeyType)
                        sOption = "outside_by_factor";
                    ++iter;
                    std::string sNumExp;
                    if (getNumberExp(iter, pNode->end(), sNumExp))
                        outputText(sOption.c_str(), sNumExp.c_str(), nOption++);
                    continue;
                }
                case BY:
                case BY_FACTOR:
                {
                    std::string sOption = "both_side";
                    if (BY_FACTOR == iter->eKeyType)
                        sOption = "both_side_by_factor";
                    ++iter;
                    std::string sNumExp;
                    if (getNumberExp(iter, pNode->end(), sNumExp))
                        outputText(sOption.c_str(), sNumExp.c_str(), nOption++);
                    continue;
                }
                case EXTEND_BY:
                case EXTEND_BY_FACTOR:
                {
                    std::string sOption = "extend";
                    if (EXTEND_BY_FACTOR == iter->eKeyType)
                        sOption = "extend_by_factor";
                    ++iter;
                    std::string sNumExp;
                    if (getNumberExp(iter, pNode->end(), sNumExp))
                        outputText(sOption.c_str(), sNumExp.c_str(), nOption++);
                    continue;
                }
                case CORNER_FILL:
                {
                    outputText("fill_corner", NULL, nOption++);
                    ++iter;
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convFillRectanglesCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "RECTANGLES";

    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "geom_fill_rectangles ( ";

    std::string sWOffset;
    std::string sLOffset;
    std::string sLength;
    std::string sWidth;
    std::string sWSpacing;
    std::string sLSpacing;
    std::string sLeftX;
    std::string sRightX;
    std::string sBottomY;
    std::string sTopY;
    std::string sInsideLayer;
    TOKEN_TYPE _eType;
    bool        isMaintainSpacing = false;

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case OFFSET:
                {
                    ++iter;
                    getNumberExp(iter, pNode->end(), sWOffset);

                    if (iter != pNode->end())
                    {
                        if (SECONDARY_KEYWORD == iter->eType && (MAINTAIN == iter->eKeyType
                           || OFFSET == iter->eKeyType || !isValidSvrfName(iter->sValue)))
                            continue;

                        getNumberExp(iter, pNode->end(), sLOffset);
                    }
                    continue;
                }
                case INSIDE_OF:
                {
                    ++iter;
                    rcsExpressionParser_T::setNegativeNumber();
                    getNumberExp(iter, pNode->end(), sLeftX);
                    getNumberExp(iter, pNode->end(), sBottomY);
                    getNumberExp(iter, pNode->end(), sRightX);
                    getNumberExp(iter, pNode->end(), sTopY);
                    rcsExpressionParser_T::unsetNegativeNumber();
                    continue;
                }
                case INSIDE_OF_LAYER:
                {
                    ++iter;
                    _eType = iter->eType;
                    std::string sLayerName;
                    if (getLayerRefName(iter, pNode->end(), sInsideLayer))
                        continue;
                }
                case MAINTAIN:
                {
                    if (++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType && SPACING == iter->eKeyType)
                    {
                        isMaintainSpacing = true;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                    ++iter;
                    continue;
                }
                default:
                    break;
            }
        }
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            case SEPARATOR:
            {
                if (sWidth.empty())
                {
                    getNumberExp(iter, pNode->end(), sWidth);
                    continue;
                }
                else if (sLength.empty())
                {
                    getNumberExp(iter, pNode->end(), sLength);
                    continue;
                }
                else if (sWSpacing.empty())
                {
                    getNumberExp(iter, pNode->end(), sWSpacing);
                    continue;
                }
                else if (sLSpacing.empty())
                {
                    getNumberExp(iter, pNode->end(), sLSpacing);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    if (sLSpacing.empty())
        sLSpacing = sWSpacing;

    outputText(NULL, sWidth.c_str(), nOption++);
    outputText(NULL, sLength.c_str(), nOption++);
    outputText(NULL, sWSpacing.c_str(), nOption);
    outputText(NULL, sLSpacing.c_str(), nOption++);
    if (!sWOffset.empty())
    {
        outputText("delta", sWOffset.c_str(), nOption);
        if (sLOffset.empty())
            sLOffset = sWOffset;
        outputText(NULL, sLOffset.c_str(), nOption++);
    }

    if (!sLeftX.empty())
    {
        outputText("inside_of_region", sLeftX.c_str(), nOption);
        outputText(NULL, sBottomY.c_str(), nOption);
        outputText(NULL, sRightX.c_str(), nOption);
        outputText(NULL, sTopY.c_str(), nOption++);
    }
    else if (!sInsideLayer.empty())
    {
        outputText("inside_of_layer", NULL, nOption++);
        bool _tempFlag = false;
        if (_eType == IDENTIFIER_NAME)
        {
            if(parseTmpLayerForOperation(sInsideLayer))
            {
                _tempFlag = true;
            }
        }
        if(!_tempFlag)
        {
            outputText(NULL, sInsideLayer.c_str(), nOption++);
        }
    }

    if (isMaintainSpacing)
        outputText("keep_spacing", NULL, nOption++);

    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}


void
rcsSynPvrsConvertor::convGeomGetCellBBoxCmd(rcsSynLayerOperationNode_T *pNode)
{
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "geom_get_cell_boundary ( ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case ORIGINAL:
                {
                    outputText("original_layer", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case MAPPED:
                {
                    outputText("mapped_layer", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case OCCUPIED:
                {
                    outputText("used_layer", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case WITH_MATCH:
                {
                    outputText("pattern_match", NULL, nOption++);
                    ++iter;
                    if (iter != pNode->end() && iter->eKeyType == GOLDEN)
                    {
                        outputText("golden", NULL, nOption++);
                        ++iter;
                    }
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            {
                std::string sValue;
                if (getStringName(iter, pNode->end(), sValue))
                {
                    if (m_vVariableNames.find(iter->sValue) != m_vVariableNames.end())
                    {
                        sValue = iter->sValue;
                        toLower(sValue);
                    }
                    outputText(NULL, sValue.c_str(), nOption++);
                    break;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomGetMultiLayersBBoxCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "EXTENT DRAWN";
    hvUInt32 nOption = 0;
    
    (*m_oPVRSStream) << "geom_get_layout_boundary ( ";

    bool hasIgnore = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if (IGNORE == iter->eKeyType)
                {
                    hasIgnore = true;
                    ++iter;
                    if (iter == pNode->end())
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP10,
                                            iter->nLineNo, iter->sValue);
                    }

                    outputText("ignore_layer", NULL, nOption++);
                    while(iter != pNode->end())
                    {
                        if (SECONDARY_KEYWORD == iter->eType &&
                           ORIGINAL == iter->eKeyType)
                            break;

                        if( (iter->eType != IDENTIFIER_NAME) || !parseTmpLayerForOperation(iter->sValue))
                        {
                            std::string sLayerName;
                            if (getLayerRefName(iter, pNode->end(), sLayerName))
                            {
                                outputText(NULL, sLayerName.c_str(), nOption++);
                            }
                            else
                                break;
                        }
                        else
                        {
                            iter++;
                        }
                    }
                    continue;
                }
                else if (ORIGINAL == iter->eKeyType)
                {
                    outputText("original_layer", NULL, nOption++);
                    break;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (hasIgnore)
                {
                    if (getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText("ignore_layer", sLayerName.c_str(), nOption++);
                        continue;
                    }
                }
                else if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText(NULL, sLayerName.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }

    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomTransformCmd(rcsSynLayerOperationNode_T *pNode)
{
     switch(pNode->getOperationType())
     {
         case GROW:
             m_sCurCommandString = "GROW";
             break;
         case SHRINK:
             m_sCurCommandString = "SHRINK";
             break;
         case MAGNIFY:
             m_sCurCommandString = "MAGNIFY";
             break;
         case ROTATE:
             m_sCurCommandString = "ROTATE";
             break;
         case DFM_SHIFT:
             m_sCurCommandString = "DFM SHIFT";
             break;
         case SHIFT:
             m_sCurCommandString = "SHIFT";
             break;
         default:
             break;
     }
    hvUInt32 nOption = 0;
    bool bShrink = false;
    switch(pNode->getOperationType())
    {
        case GROW:
            outputText("geom_orth_size ( ", NULL, nOption++);
            break;
        case SHRINK:
            bShrink = true;
            outputText("geom_orth_size ( ", NULL, nOption++);
            break;
        case MAGNIFY:
            outputText("geom_magnify ( ", NULL, nOption++);
            break;
        case ROTATE:
            outputText("geom_rotate ( ", NULL, nOption++);
            break;
        case DFM_SHIFT:
            outputText("dfm_move_layer ( ", NULL, nOption++);
            break;
        case SHIFT:
            outputText("geom_shift ( ", NULL, nOption++);
            break;
        default:
            return;
    }

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if (RIGHT == iter->eKeyType)
                {
                    if (++iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       iter->eKeyType == BY)
                    {
                        ++iter;
                        std::string sNumExp;
                        if (getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            if (bShrink)
                            {
                                sNumExp.insert(0,"-");
                                outputText("right", sNumExp.c_str(), nOption++);
                            }
                            else
                            {
                                outputText("right", sNumExp.c_str(), nOption++);
                            }
                        }
                        continue;
                    }
                    --iter;
                }
                else if (LEFT == iter->eKeyType)
                {
                    if (++iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       iter->eKeyType == BY)
                    {
                        ++iter;
                        std::string sNumExp;
                        if (getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            if (bShrink)
                            {
                                sNumExp.insert(0,"-");
                                outputText("left", sNumExp.c_str(), nOption++);
                            }
                            else
                            {
                                outputText("left", sNumExp.c_str(), nOption++);
                            }
                        }
                        continue;
                    }
                    --iter;
                }
                else if (BOTTOM == iter->eKeyType)
                {
                    if (++iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       iter->eKeyType == BY)
                    {
                        ++iter;
                        std::string sNumExp;
                        if (getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            if (bShrink)
                            {
                                sNumExp.insert(0,"-");
                                outputText("bottom", sNumExp.c_str(), nOption++);
                            }
                            else
                            {
                                outputText("bottom", sNumExp.c_str(), nOption++);
                            }
                        }
                        continue;
                    }
                    --iter;
                }
                else if (TOP == iter->eKeyType)
                {
                    if (++iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       iter->eKeyType == BY)
                    {
                        ++iter;
                        std::string sNumExp;
                        if (getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            if (bShrink)
                            {
                                sNumExp.insert(0,"-");
                                outputText("top", sNumExp.c_str(), nOption++);
                            }
                            else
                            {
                                outputText("top", sNumExp.c_str(), nOption++);
                            }
                        }
                        continue;
                    }
                    --iter;
                }
                else if (BY == iter->eKeyType)
                {
                    std::list<rcsToken_T>::iterator begin = iter;
                    ++iter;
                    std::string sNumExp;
                    if (getNumberExp(iter, pNode->end(), sNumExp))
                    {
                        if (bShrink)
                        {
                            sNumExp.insert(0,"-");
                            outputText("by", sNumExp.c_str(), nOption++);
                        }
                        else
                        {
                            outputText("by", sNumExp.c_str(), nOption++);
                        }
                    }
                    if (SHIFT == pNode->getOperationType() || DFM_SHIFT == pNode->getOperationType())
                    {
                        if (iter == pNode->end())
                        {
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP11,
                                                begin->nLineNo, begin->sValue);
                        }
                        sNumExp.clear();
                        if (getNumberExp(iter, pNode->end(), sNumExp))
                        {
                            outputText(NULL, sNumExp.c_str(), nOption++);
                        }
                    }
                    continue;
                }
                else if (SEQUENTIAL == iter->eKeyType && GROW == pNode->getOperationType())
                {
                    outputText("in_order", NULL, nOption++);
                    ++iter;
                    continue;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomMergeLayerCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "MERGE";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "geom_merge ( ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if (BY == iter->eKeyType)
                {
                    std::string sNumExp;
                    
                    rcsExpressionParser_T::setNegativeNumber();
                    if (getNumberExp(++iter, pNode->end(), sNumExp))
                    {
                        outputText("by", sNumExp.c_str(), nOption++);
                        rcsExpressionParser_T::unsetNegativeNumber();
                        continue;
                    }
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            case SEPARATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText(NULL, sLayerName.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomGetNetByAreaCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "NET AREA";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "geom_net ( by_area ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                if (!isValidSvrfName(iter->sValue))
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                        iter->nLineNo, iter->sValue);
                }
            }
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                }
                else if (isConstraintOperator(*iter))
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    outputText(NULL, sConstraint.c_str(), nOption++);
                }
                continue;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomMergeNetCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "ORNET";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "geom_or_net ( ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if (BY_NET == iter->eKeyType)
                {
                    outputText("same_net", NULL, nOption++);
                    break;
                }
                else if (BY_SHAPE == iter->eKeyType)
                {
                    outputText("keep_shape", NULL, nOption++);
                    break;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convEdgeAngledToRegularCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "DEANGLE";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "geom_orthogonalize_angle ( ";

    bool hasMaxOffset = false;
    bool hasInLayer   = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
   {
       if (SECONDARY_KEYWORD == iter->eType)
       {
           switch(iter->eKeyType)
           {
               case ORTHOGONAL:
               {
                   if (++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                      ONLY == iter->eKeyType)
                   {
                       outputText("only_by_orthogonal", NULL, nOption++);
                   }
                   else
                   {
                       --iter;
                       throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                           iter->nLineNo, iter->sValue);
                   }
                   ++iter;
                   continue;
               }
               case SKEW:
                   outputText("by_skew_edge", NULL, nOption++);
                   ++iter;
                   continue;
               case ALL:
                   outputText("all_edge", NULL, nOption++);
                   ++iter;
                   continue;
               default:
                   break;
           }
       }

       switch(iter->eType)
       {
           case LAYER_OPERATION_KEYWORD:
               break;
           case SECONDARY_KEYWORD:
           case IDENTIFIER_NAME:
           case STRING_CONSTANTS:
           case NUMBER:
           case OPERATOR:
           {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
               std::string sLayerName;
               if (!hasInLayer && getLayerRefName(iter, pNode->end(), sLayerName))
               {
                   hasInLayer = true;
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
               }
               else if (!hasMaxOffset)
               {
                   std::string sNumExp;
                   if (getNumberExp(iter, pNode->end(), sNumExp))
                   {
                       outputText(NULL, sNumExp.c_str(), nOption++);
                       hasMaxOffset = true;
                       continue;
                   }
               }
               else
               {
                   throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                       iter->nLineNo, iter->sValue);
               }
               continue;
           }
           default:
               throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                   iter->nLineNo, iter->sValue);
               break;
       }
       ++iter;
   }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomPushLayerCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "PUSH";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "geom_push ( ";
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if (iter->eKeyType == LOCATE)
                {
                    ++iter;
                    outputText("with_layer", NULL, nOption++);
                    if (iter->eType == IDENTIFIER_NAME)
                    {
                        if(parseTmpLayerForOperation(iter->sValue))
                        {
                            break;
                        }
                    }
                    std::string sLayerName;
                    if (getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText(NULL, sLayerName.c_str(), nOption++);
                    }
                    continue;
                }
                else if (iter->eKeyType == LIGHT)
                {
                    outputText("level", "1", nOption++);
                    break;
                }
                else if (iter->eKeyType == MEDIUM)
                {
                    outputText("level", "2", nOption++);
                    break;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText(NULL, sLayerName.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomVerticeSnapToGridCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "SNAP";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "geom_snap ( ";
    bool hasInLayer = false;
    bool hasValue1  = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            case SEPARATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (!hasInLayer && getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    hasInLayer = true;
                    outputText(NULL, sLayerName.c_str(), nOption++);
                    continue;
                }
                else
                {
                    std::string sNumExp;
                    if (getNumberExp(iter, pNode->end(), sNumExp))
                    {
                        if (!hasValue1)
                        {
                            outputText(NULL, sNumExp.c_str(), nOption);
                            hasValue1 = true;
                            continue;
                        }
                        else
                        {
                            outputText(NULL, sNumExp.c_str(), nOption++);
                            hasValue1 = true;
                            continue;
                        }
                    }
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}


void
rcsSynPvrsConvertor::convGeomGetTextBBoxCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "EXPAND TEXT";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "geom_text_to_rect ( ";

    bool hasText      = false;
    bool hasTextLayer = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if (PRIMARY == iter->eKeyType)
                {
                    if (++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType && ONLY == iter->eKeyType)
                    {
                        outputText("top_cell", NULL, nOption++);
                        break;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                }
                else if (CASE == iter->eKeyType)
                {
                    if (++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType &&
                       SENSITIVE == iter->eKeyType)
                    {
                        outputText("keep_case", NULL, nOption++);
                        break;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                }
                else if (BY == iter->eKeyType)
                {
                    std::string sNumExp;
                    if (getNumberExp(++iter, pNode->end(), sNumExp))
                    {
                        outputText("by", sNumExp.c_str(), nOption++);
                    }
                    continue;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                if (!hasText)
                {
                    std::string sValue;
                    if (getStringName(iter, pNode->end(), sValue))
                    {
                        outputText(NULL, sValue.c_str(), nOption++);
                        hasText = true;
                        break;
                    }
                }
                else if (!hasTextLayer)
                {
                    std::string sLayerName;
                    if (getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText(NULL, sLayerName.c_str(), nOption++);
                    }
                    hasTextLayer = true;
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomStampLayerCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "STAMP";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "geom_transfer_netid ( ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if (iter->eKeyType == BY)
                {
                    ++iter;
                    outputText("by", NULL, nOption++);
                    if (iter->eType == IDENTIFIER_NAME)
                    {
                        if(parseTmpLayerForOperation(iter->sValue))
                        {
                            break;
                        }
                    }
                    std::string sLayerName;
                    if (getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText(NULL, sLayerName.c_str(), nOption++);
                    }
                    continue;
                }
                else if (iter->eKeyType == ABUT_ALSO)
                {
                    outputText("include_adjacent", NULL, nOption++);
                    break;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText(NULL, sLayerName.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomSelectByNeighborCmd(rcsSynLayerOperationNode_T *pNode)
{
     switch(pNode->getOperationType())
     {
         case WITH_NEIGHBOR:
             m_sCurCommandString = "WITH NEIGHBOR";
             break;
         case NOT_WITH_NEIGHBOR:
             m_sCurCommandString = "NOT WITH NEIGHBOR";
             break;
         default:
             break;
     }
    hvUInt32 nOption = 0;
    if (pNode->getOperationType() == WITH_NEIGHBOR)
        (*m_oPVRSStream) << "geom_with_adjacent ( ";
    else if (pNode->getOperationType() == NOT_WITH_NEIGHBOR)
        (*m_oPVRSStream) << "~geom_with_adjacent ( ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case ORTHOGONAL:
                case OCTAGONAL:
                {
                    std::list<rcsToken_T>::iterator begin = iter;
                    if (++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType && ONLY == iter->eKeyType)
                    {
                        if (begin->eKeyType == ORTHOGONAL)
                            outputText(NULL, "orthogonal_rect", nOption);
                        else
                            outputText(NULL, "octagonal_rect", nOption);
                        ++iter;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                    continue;
                }
                case SPACE:
                {
                    std::string sConstraint;
                    if (getConstraint(++iter, pNode->end(), sConstraint))
                        outputText("distance", sConstraint.c_str(), nOption++);
                    continue;
                }
                case SQUARE:
                    outputText(NULL, "square", nOption);
                    ++iter;
                    continue;
                case CENTERS:
                    outputText("from_center", NULL, nOption++);
                    ++iter;
                    continue;
                case INSIDE_OF_LAYER:
                {

                    std::string sLayerName;
                    std::list<rcsToken_T>::iterator iterTmp = iter;
                    if (getLayerRefName(++iterTmp, pNode->end(), sLayerName))
                    {
                       if(string::npos != sLayerName.find("pvrs_tmp_layer_"))
                       {
                           iter++;
                           if (iter->eType == IDENTIFIER_NAME)
                           {
                               outputText("inside_of_layer", NULL, nOption++);
                               parseTmpLayerForOperation(iter->sValue);
                           }
                           iter++;
                       }
                       else
                       {
                           iter = iterTmp;
                           outputText("inside_of_layer", sLayerName.c_str(), nOption++);
                       }
                    }

                    continue;
                }
                case CONNECTED:
                    outputText("same_net", NULL, nOption++);
                    ++iter;
                    continue;
                case NOT_CONNECTED:
                    outputText("diff_net", NULL, nOption++);
                    ++iter;
                    continue;
                default:
                    break;
            }
        }
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
                else if (iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    outputText(NULL, sConstraint.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomSelectByCoinEdgeCmd(rcsSynLayerOperationNode_T *pNode)
{
     switch(pNode->getOperationType())
     {
         case WITH_EDGE:
             m_sCurCommandString = "WITH EDGE";
             break;
         case NOT_WITH_EDGE:
             m_sCurCommandString = "NOT WITH EDGE";
             break;
         default:
             break;
     }
    hvUInt32 nOption = 0;
    switch(pNode->getOperationType())
    {
        case WITH_EDGE:
            (*m_oPVRSStream) << "geom_with_edge ( ";
            break;
        case NOT_WITH_EDGE:
            (*m_oPVRSStream) << "~geom_with_edge ( ";
            break;
        default:
            return;
    }

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
                else if (iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    outputText(NULL, sConstraint.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convGeomSelectTextedCmd(rcsSynLayerOperationNode_T *pNode)
{
     switch(pNode->getOperationType())
     {
         case WITH_TEXT:
             m_sCurCommandString = "WITH TEXT";
             break;
         case NOT_WITH_TEXT:
             m_sCurCommandString = "NOT WITH TEXT";
             break;
         default:
             break;
     }
    hvUInt32 nOption = 0;
    if (pNode->getOperationType() == WITH_TEXT)
        (*m_oPVRSStream) << "geom_with_label ( ";
    else if ((pNode->getOperationType() == NOT_WITH_TEXT))
        (*m_oPVRSStream) << "~geom_with_label ( ";
    else
        return;

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if (PRIMARY == iter->eKeyType)
                {
                    if (++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType && ONLY == iter->eKeyType)
                    {
                        outputText("top_cell", NULL, nOption++);
                        break;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                }
                else if (CASE == iter->eKeyType)
                {
                    if (++iter != pNode->end() && SECONDARY_KEYWORD == iter->eType && SENSITIVE == iter->eKeyType)
                    {
                        outputText("case_sensitive", NULL, nOption++);
                        break;
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText(NULL, sLayerName.c_str(), nOption++);
                }
                continue;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convCheckWidthCmd(rcsSynLayerOperationNode_T *pNode)
{
     switch(pNode->getOperationType())
     {
         case WITH_WIDTH:
             m_sCurCommandString = "WITH WIDTH";
             break;
         case NOT_WITH_WIDTH:
             m_sCurCommandString = "NOT WITH WIDTH";
             break;
         default:
             break;
     }
    hvUInt32 nOption = 0;
    if (pNode->getOperationType() == WITH_WIDTH)
        (*m_oPVRSStream) << "geom_with_width ( ";
    else
        (*m_oPVRSStream) << "~geom_with_width ( ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   continue;
                }
                else if (iter->eType == OPERATOR)
                {
                    std::string sConstraint;
                    getConstraint(iter, pNode->end(), sConstraint);
                    outputText(NULL, sConstraint.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

static bool
_isOpcbiasKeyword(const rcsToken_T &token)
{
    if (token.eType == SECONDARY_KEYWORD)
    {
        switch(token.eKeyType)
        {
            case V1:
            case V2:
            case IGNORE:
            case MINBIASLENGTH:
            case CLOSEREDGE:
            case CLOSERSYMMETRIC:
            case GRID:
            case IMPLICITBIAS:
            case OPTION:
            case SPACE:
            case SPACELAYER:
            case ANGLED:
                return true;
            default:
                return false;
        }
    }
    else if (token.eType == LAYER_OPERATION_KEYWORD)
    {
        return true;
    }
    return false;
}

void
rcsSynPvrsConvertor::convOpcbiasRule(std::list<rcsToken_T>::iterator &iter,
                                          std::list<rcsToken_T>::iterator end,
                                          hvUInt32 &nOption)
{
    bool hasSpaceMetric   = false;
    bool hasWidthMetric   = false;
    bool hasWidth2Metric  = false;
    bool hasLength1Metric = false;
    bool hasLength2Metric = false;
    bool hasSpaceLayer    = false;
    while(iter != end)
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                switch(iter->eKeyType)
                {
                    case MOVE:
                    {
                        ++iter;
                        std::string sValue;
                        if (getNumberExp(iter, end, sValue))
                            outputText("bias", sValue.c_str(), nOption++);
                        continue;
                    }
                    case SPACE:
                    {
                        if (hasSpaceMetric)
                            return;

                        hasSpaceMetric = true;
                        ++iter;
                        std::string sCons;
                        if (getConstraint(iter, end, sCons))
                            outputText("with_space", sCons.c_str(), nOption++);
                        continue;
                    }
                    case WIDTH:
                    {
                        if (hasWidthMetric)
                            return;

                        hasWidthMetric = true;
                        ++iter;
                        std::string sCons;
                        if (getConstraint(iter, end, sCons))
                            outputText("with_width1", sCons.c_str(), nOption++);
                        continue;
                    }
                    case WIDTH2:
                    {
                        if (hasWidth2Metric)
                            return;

                        hasWidth2Metric = true;
                        ++iter;
                        std::string sCons;
                        if (getConstraint(iter, end, sCons))
                            outputText("with_width2", sCons.c_str(), nOption++);
                        continue;
                    }
                    case LENGTH1:
                    {
                        if (hasLength1Metric)
                            return;

                        hasLength1Metric = true;
                        ++iter;
                        std::string sCons;
                        if (getConstraint(iter, end, sCons))
                            outputText("with_length1", sCons.c_str(), nOption++);
                        continue;
                    }
                    case LENGTH2:
                    {
                        if (hasLength2Metric)
                            return;

                        hasLength2Metric = true;
                        ++iter;
                        std::string sCons;
                        if (getConstraint(iter, end, sCons))
                            outputText("with_length2", sCons.c_str(), nOption++);
                        continue;
                    }
                    case SPACELAYER:
                    {
                        if(hasSpaceLayer)
                            return;

                        hasSpaceLayer = true;
                        ++iter;
                        outputText("edge_layer", NULL, nOption++);
                        if (iter->eType == IDENTIFIER_NAME)
                        {
                            if(parseTmpLayerForOperation(iter->sValue))
                            {
                                break;
                            }
                        }
                        std::string sLayerName;
                        if (getLayerRefName(iter, end, sLayerName))
                            outputText(NULL, sLayerName.c_str(), nOption++);
                        continue;
                    }
                    case OPPOSITE:
                    {
                        outputText(NULL, "opposite", nOption++);
                        ++iter;
                        continue;
                    }
                    case OPPOSITE_SYMMETRIC:
                    {
                        outputText(NULL, "opposite_symmetric", nOption++);
                        ++iter;
                        continue;
                    }
                    case OPPOSITE_FSYMMETRIC:
                    {
                        outputText(NULL, "opposite_fsymmetric", nOption++);
                        ++iter;
                        continue;
                    }
                    case OPPOSITE_EXTENDED:
                    {
                        outputText(NULL, "opposite_extended", nOption++);
                        ++iter;
                        std::string sValue;
                        if (getNumberExp(iter, end, sValue))
                            outputText(NULL, sValue.c_str(), nOption++);
                        continue;
                    }
                    case OPPOSITE_EXTENDED_SYMMETRIC:
                    {
                        outputText(NULL, "opposite_extended_symmetric", nOption++);
                        ++iter;
                        std::string sValue;
                        if (getNumberExp(iter, end, sValue))
                            outputText(NULL, sValue.c_str(), nOption++);
                        continue;
                    }
                    case OPPOSITE_EXTENDED_FSYMMETRIC:
                    {
                        outputText(NULL, "opposite_extended_fsymmetric", nOption++);
                        ++iter;
                        std::string sValue;
                        if (getNumberExp(iter, end, sValue))
                            outputText(NULL, sValue.c_str(), nOption++);
                        continue;
                    }
                    default:
                        return;
                }
            }
            default:
                return;
        }
        ++iter;
    }
}


void
rcsSynPvrsConvertor::convOpcBiasCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "OPC BIAS";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "opc_bias ( ";

    hvUInt32 iLayer = 0;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case V1:
                {
                    outputCon2Error(pNode,*iter);
                    ++iter;
                    continue;
                }
                case V2:
                {
                    outputCon2Error(pNode,*iter);
                    ++iter;
                    continue;
                }
                case IGNORE:
                {
                    outputCon2Error(pNode,*iter);
                    ++iter;
                    std::string sValue;
                    if (getNumberExp(iter, pNode->end(), sValue))
                    {
                        ;
                    }
                    continue;
                }
                case MINBIASLENGTH:
                {
                    ++iter;
                    std::string sValue;
                    if (getNumberExp(iter, pNode->end(), sValue))
                        outputText("limit", sValue.c_str(), nOption++);

                    if (iter != pNode->end() && !_isOpcbiasKeyword(*iter))
                    {
                        std::string sValue;
                        if (getNumberExp(iter, pNode->end(), sValue))
                            outputText("convex_limit", sValue.c_str(), nOption++);

                        if (iter != pNode->end() && !_isOpcbiasKeyword(*iter))
                        {
                            std::string sValue;
                            if (getNumberExp(iter, pNode->end(), sValue))
                                outputText("concave_limit", sValue.c_str(), nOption++);
                        }
                    }

                    if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD && iter->eKeyType == ANGLED)
                    {
                        ++iter;
                        std::string sValue;
                        if (getNumberExp(iter, pNode->end(), sValue))
                            outputText("slant_limit", sValue.c_str(), nOption++);
                        if (iter != pNode->end() && !_isOpcbiasKeyword(*iter))
                        {
                            std::string sValue;
                            if (getNumberExp(iter, pNode->end(), sValue))
                                outputText("slant_convex_limit", sValue.c_str(), nOption++);

                            if (iter != pNode->end() && !_isOpcbiasKeyword(*iter))
                            {
                                std::string sValue;
                                if (getNumberExp(iter, pNode->end(), sValue))
                                    outputText("slant_concave_limit", sValue.c_str(), nOption++);
                            }
                        }
                    }
                    continue;
                }
                case CLOSEREDGE:
                {
                    outputCon2Error(pNode,*iter);
                    ++iter;
                    if (iter != pNode->end())
                    {
                        if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == V1)
                        {
                            
                            ++iter;
                            continue;
                        }
                        else if (iter->eType == SECONDARY_KEYWORD && iter->eKeyType == V1)
                        {
                            
                            ++iter;
                            continue;
                        }
                    }
                    else
                    {
                        ;
                    }
                    continue;
                }
                case CLOSERSYMMETRIC:
                {
                    outputCon2Error(pNode,*iter);
                    ++iter;
                    continue;
                }
                case GRID:
                {
                    ++iter;
                    std::string sValue;
                    if (getNumberExp(iter, pNode->end(), sValue))
                        outputText("diagonal", sValue.c_str(), nOption++);
                    continue;
                }
                case IMPLICITBIAS:
                {
                    outputCon2Error(pNode,*iter);
                    ++iter;
                    continue;
                }
                case OPTION:
                {
                    outputCon2Error(pNode,*iter);
                    ++iter;
                    if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD)
                    {
                        std::string sOption;
                        switch(iter->eKeyType)
                        {
                            case ACUTE_ANGLE_ABUT:
                                sOption = "acute_also";
                                break;
                            case IMPLICIT_METRIC:
                                sOption = "neighbor_metric";
                                break;
                            case FAST_CLASSIFY:
                                sOption = "alternate_classify";
                                break;
                            case SPIKE_CLEANUP:
                                sOption = "repair_skew";
                                break;
                            case CAREFUL_SKEW:
                                sOption = "fill_gap";
                                break;
                            case CORNER_FILL:
                                sOption = "fill_corner";
                                break;
                            case PROJECTING_ONLY:
                                sOption = "project_classify";
                                break;
                            case CAREFUL_MEASURE:
                                sOption = "careful_hier";
                                break;
                            default:
                                break;
                        }

                        ++iter;
                        if (iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                          (iter->eKeyType == YES || iter->eKeyType == NO))
                        {
                            ++iter;
                            continue;
                        }
                        else
                        {
                            --iter;
                        }
                    }
                    --iter;
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP11,
                                        iter->nLineNo, iter->sValue);
                }
                case SPACE:
                case SPACELAYER:
                    convOpcbiasRule(iter, pNode->end(), nOption);
                    continue;
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                if (iLayer < 3)
                {
                    ++iLayer;
                    std::string sLayerName;
                    if (getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText(NULL, sLayerName.c_str(), nOption++);
                    }
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

static bool
isPathChkKeyword(const std::list<rcsToken_T>::iterator iter)
{
    if (iter->eType == SECONDARY_KEYWORD)
    {
        switch(iter->eKeyType)
        {
            case PRINT:
            case BY_LAYER:
            case BY_CELL:
            case FLAT:
            case PORTS_ALSO:
            case NOFLOAT:
            case EXCLUDE_UNUSED:
            case EXCLUDE_SUPPLY:
            case POWERNAME:
            case GROUNDNAME:
            case LABELNAME:
            case BREAKNAME:
                return true;
            default:
                return false;
        }
    }
    else if (iter->eType == LAYER_OPERATION_KEYWORD)
    {
        return true;
    }
    return false;
}


void
rcsSynPvrsConvertor::parsePathChkName(std::list<rcsToken_T>::iterator &iter,
                                        std::list<rcsToken_T>::iterator end,
                                        std::string &sValues)
{
    ++iter;
    while(iter != end)
    {
        std::string sValue;
        if (!isPathChkKeyword(iter) && getStringName(iter, end, sValue))
        {
            sValues += sValue;
            sValues += " ";
        }
        else
            break;
        ++iter;
    }

    if (sValues.empty())
    {
        --iter;
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo,
                            iter->sValue);
    }
}

void
rcsSynPvrsConvertor::parseBreakName(std::list<rcsToken_T>::iterator &iter,
                                      std::list<rcsToken_T>::iterator end,
                                      std::string &sValues)
{
    std::list<rcsToken_T>::iterator begin = iter;
    ++iter;
    while(iter != end)
    {
        if (!isPathChkKeyword(iter) && getStringName(iter, end, sValues))
        {
            if (++iter != end && iter->eType == SEPARATOR && iter->sValue == "(")
            {
                sValues += " ( ";
                ++iter;
                while(!(iter->eType == SEPARATOR && iter->sValue == ")"))
                {
                    std::string sNetName;
                    if (!isPathChkKeyword(iter) && getStringName(iter, end, sNetName))
                        sValues += sNetName;
                    else
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                            begin->nLineNo, begin->sValue);
                    sValues += " ";
                    ++iter;
                }
                sValues += ")";
                ++iter;
            }
        }
        else
            break;
    }
}

void
rcsSynPvrsConvertor::convPathchkCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "PATHCHK";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "path_check ( ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();

    while(iter != pNode->end())
    {
        if (iter->eKeyType == PATHCHK)
        {
            ++iter;
            continue;
        }

        if (iter->eType == SECONDARY_KEYWORD)
        {
            switch(iter->eKeyType)
            {
                case LABELED:
                {
                    outputText(NULL, "labeled", nOption++);
                    ++iter;
                    continue;
                }
                case POWER:
                {
                    outputText(NULL, "powered", nOption++);
                    ++iter;
                    continue;
                }
                case GROUND:
                {
                    outputText(NULL, "grounded", nOption++);
                    ++iter;
                    continue;
                }
                case PRINT:
                {
                    if (++iter != pNode->end() && iter->eType == SECONDARY_KEYWORD &&
                       (iter->eKeyType == NETS || iter->eKeyType == POLYGONS))
                    {
                        std::string sOption;
                        if (iter->eKeyType == NETS)
                        {
                            sOption = "output_net";
                        }
                        else
                        {
                            sOption = "output_polygon";
                        }

                        if (++iter != pNode->end())
                        {
                            std::string sFileName;
                            if (getStringName(iter, pNode->end(), sFileName))
                                outputText(sOption.c_str(), sFileName.c_str(), nOption++);
                            ++iter;
                        }
                    }
                    else
                    {
                        --iter;
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP7,
                                            iter->nLineNo, iter->sValue);
                    }
                    continue;
                }
                case BY_LAYER:
                {
                    outputText(NULL, "by_layer", nOption++);
                    ++iter;
                    continue;
                }
                case BY_CELL:
                {
                    outputText(NULL, "by_cell", nOption++);
                    ++iter;
                    continue;
                }
                case FLAT:
                {
                    outputText(NULL, "flatten", nOption++);
                    ++iter;
                    continue;
                }
                case PORTS_ALSO:
                {
                    outputText("with_port", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case NOFLOAT:
                {
                    outputText("ignore_floatting", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case EXCLUDE_UNUSED:
                {
                    outputText("only_used", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case EXCLUDE_SUPPLY:
                {
                    outputText("exclude_power_ground", NULL, nOption++);
                    ++iter;
                    continue;
                }
                case POWERNAME:
                {
                    std::string sPowerName;
                    parsePathChkName(iter, pNode->end(), sPowerName);
                    outputText("power", sPowerName.c_str(), nOption++);
                    continue;
                }
                case GROUNDNAME:
                {
                    std::string sGroundName;
                    parsePathChkName(iter, pNode->end(), sGroundName);
                    outputText("ground", sGroundName.c_str(), nOption++);
                    continue;
                }
                case LABELNAME:
                {
                    std::string sLabelName;
                    parsePathChkName(iter, pNode->end(), sLabelName);
                    outputText("label", sLabelName.c_str(), nOption++);
                    continue;
                }
                case BREAKNAME:
                {
                    std::string sBreakName;
                    parseBreakName(iter, pNode->end(), sBreakName);
                    if (!sBreakName.empty())
                        outputText("stop", sBreakName.c_str(), nOption++);
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                    outputText(NULL, sLayerName.c_str(), nOption++);
                continue;
            }
            case OPERATOR:
            {
                if (iter->sValue == "+" || iter->sValue == "-")
                {
                    std::string sLayerName;
                    if (getLayerRefName(iter, pNode->end(), sLayerName))
                        outputText(NULL, sLayerName.c_str(), nOption++);
                    continue;
                }
                else if ("!" == iter->sValue)
                {
                    ++iter;
                    if(iter == pNode->end())
                        continue;

                    if(iter != pNode->end() && iter->eType == SECONDARY_KEYWORD)
                    {
                        if (iter->eKeyType == LABELED)
                        {
                            outputText(NULL, "!labeled", nOption++);
                        }
                        else if (iter->eKeyType == POWER)
                        {
                            outputText(NULL, "!powered", nOption++);
                        }
                        else if (iter->eKeyType == GROUND)
                        {
                            outputText(NULL, "!grounded", nOption++);
                        }
                        else
                        {
                            --iter;
                            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo,
                                                iter->sValue);
                        }
                    }
                    break;
                }
                else if ( "&&" == iter->sValue)
                {
                    outputText(NULL, "&&", nOption++);
                    break;
                }
                else if ( "||" == iter->sValue)
                {
                    outputText(NULL, "||", nOption++);
                    break;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo,
                                    iter->sValue);
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convNetAreaRatioRDBCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "NET AREA RATIO PRINT";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "print_log ( nar ";

    bool hasInLayer = false;
    bool hasFile    = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                if (!isValidSvrfName(iter->sValue))
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo,
                                        iter->sValue);
                }
            }
            case STRING_CONSTANTS:
            {
                if (!hasInLayer)
                {
                    std::string sLayerName;
                    if (getLayerRefName(iter, pNode->end(), sLayerName))
                        outputText(NULL, sLayerName.c_str(), nOption++);
                    hasInLayer = true;
                    continue;
                }
                else if (!hasFile)
                {
                    hasFile = true;
                    std::string sFileName;
                    if (getStringName(iter, pNode->end(), sFileName))
                    {
                        outputText(NULL, sFileName.c_str(), nOption++);
                    }
                }
                else
                {
                    throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo,
                                        iter->sValue);
                }
                break;
            }
            case NUMBER:
            case OPERATOR:
            {
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                    outputText(NULL, sLayerName.c_str(), nOption++);
                hasInLayer = true;
                continue;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo,
                                    iter->sValue);
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") " << std::endl;
}

void
rcsSynPvrsConvertor::convDisconnectCmd(rcsSynLayerOperationNode_T *pNode)
{
    (*m_oPVRSStream) << "remove_connect ";

    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convTableDRCSpaceRule(std::list<rcsToken_T>::iterator &iter,
                                           std::list<rcsToken_T>::iterator end)
{
    bool hasSpaceCons = false;
    while(iter != end)
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                switch(iter->eKeyType)
                {
                    case SPACE:
                    {
                        if (hasSpaceCons)
                            return;

                        hasSpaceCons = true;
                        std::string sCons;
                        if (getConstraint(++iter ,end, sCons))
                            outputText("check_space", sCons.c_str());
                        continue;
                    }
                    case EXCLUDE_SHIELDED:
                    {
                        if (++iter != end)
                        {
                            if(((SECONDARY_KEYWORD == iter->eType || IDENTIFIER_NAME == iter->eType) &&
                               !isValidSvrfName(iter->sValue)) || (SECONDARY_KEYWORD == iter->eType &&
                               (WIDTH == iter->eKeyType || SPACE == iter->eKeyType ||
                                REGION == iter->eKeyType || BY_POLYGON == iter->eKeyType ||
                                MEASURE_ALL == iter->eKeyType || PROJECTING == iter->eKeyType ||
                                OPPOSITE == iter->eKeyType || OPPOSITE_EXTENDED == iter->eKeyType ||
                                WIDTH1 == iter->eKeyType || WIDTH2 == iter->eKeyType ||
                                WIDTH == iter->eKeyType)))
                            {
                                outputText("shielded_level", "4");
                                continue;
                            }
                            std::string sNumExp;
                            if (getNumberExp(iter, end, sNumExp))
                            {
                                outputText("shielded_level", sNumExp.c_str());
                            }
                            else
                            {
                                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, iter->nLineNo,
                                                    iter->sValue);
                            }
                        }
                        else
                        {
                            outputText("shielded_level", "4");
                        }
                        continue;
                    }
                    case OPPOSITE_EXTENDED:
                    {
                        outputText(NULL, "opposite_extended");

                        std::string sNumExp;
                        if (getNumberExp(++iter, end, sNumExp))
                            outputText(NULL, sNumExp.c_str());
                        continue;
                    }
                    case OPPOSITE:
                        outputText(NULL, "opposite");
                        ++iter;
                        continue;
                    case SQUARE:
                        outputText(NULL, "square");
                        ++iter;
                        continue;
                    case PROJECTING:
                    {
                        std::string sCons;
                        if (getConstraint(++iter, end, sCons))
                        {
                            outputText("apposition", sCons.c_str());
                        }
                        continue;
                    }
                    case WIDTH1:
                    {
                        std::string sCons;
                        if (getConstraint(++iter ,end, sCons))
                            outputText("with_width1", sCons.c_str());
                        continue;
                    }
                    case WIDTH2:
                    {
                        std::string sCons;
                        if (getConstraint(++iter ,end, sCons))
                            outputText("with_width2", sCons.c_str());
                        continue;
                    }
                    default:
                        return;
                }
            }
            default:
                return;
        }
        ++iter;
    }
}

void
rcsSynPvrsConvertor::convTableDRCWidthRule(std::list<rcsToken_T>::iterator &iter,
                                           std::list<rcsToken_T>::iterator end)
{
    bool hasWidthCons = false;
    while(iter != end)
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                switch(iter->eKeyType)
                {
                    case WIDTH:
                    {
                        if (hasWidthCons)
                            return;

                        hasWidthCons = true;
                        std::string sCons;
                        if (getConstraint(++iter ,end, sCons))
                            outputText("check_width", sCons.c_str());
                        continue;
                    }
                    case OPPOSITE_EXTENDED:
                    {
                        outputText(NULL, "opposite_extended");
                        std::string sNumExp;
                        if (getNumberExp(++iter, end, sNumExp))
                            outputText(NULL, sNumExp.c_str());
                        continue;
                    }
                    case OPPOSITE:
                        outputText(NULL, "opposite");
                        ++iter;
                        continue;
                    case SQUARE:
                        outputText(NULL, "square");
                        ++iter;
                        continue;
                    case PROJECTING:
                    {
                        std::string sCons;
                        if (getConstraint(++iter, end, sCons))
                        {
                            outputText("apposition", sCons.c_str());
                        }
                        continue;
                    }
                    default:
                        return;
                }
            }
            default:
                return;
        }
        ++iter;
    }
}

void
rcsSynPvrsConvertor::convCheckTableDRCCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "TDDRC";
    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "table_drc ( ";

    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        if (SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case REGION:
                    outputText(NULL, "region", nOption++);
                    ++iter;
                    continue;
                case BY_POLYGON:
                    outputText(NULL, "by_polygon", nOption++);
                    ++iter;
                    continue;
                case MEASURE_ALL:
                    outputText(NULL, "no_shielded_edge", nOption++);
                    ++iter;
                    continue;
                case SPACE:
                    convTableDRCSpaceRule(iter, pNode->end());
                    continue;
                case WIDTH:
                    convTableDRCWidthRule(iter, pNode->end());
                    continue;
                default:
                    break;
            }
        }
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    outputText(NULL, sLayerName.c_str(), nOption++);
                }
                continue;
            }
            case BUILT_IN_LANG:
            {
#if 0
                if(!outputBuiltInLangInTrs(*iter, nOption))
                {
                    std::string sValue = iter->sValue;
                    sValue.erase(0, 1);
                    sValue.erase(sValue.size() - 1);
                    trim(sValue);
                    if(sValue[0] == '\"' || sValue[0] == '\'')
                    {
                        trim(sValue, "\"\'");
                        sValue.insert(0, "[\"");
                        sValue.insert(sValue.size(), "\"]");
                    }
                    else
                    {
                        sValue.insert(0, "[");
                        sValue.insert(sValue.size(), "]");
                    }
                    outputText(NULL, sValue.c_str(), nOption++);
                }
#else
                outputBuiltInLang(pNode->getOperationType(), *iter, nOption);
#endif
                break;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convSconnectCmd(rcsSynLayerOperationNode_T *pNode)
{
    m_sCurCommandString = "SCONNECT";
    if (_hasOnlyDirectKeyword(pNode))
    {
        outputCon1Error(pNode);
        return;
    }

    hvUInt32 nOption = 0;
    (*m_oPVRSStream) << "uniconnect ( ";

    bool hasUpperLayer = false;
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                break;
            case SECONDARY_KEYWORD:
            {
                if (MASK == iter->eKeyType)
                {
                    outputCon2Error(pNode, *iter);
                    break;
                }
                else if (DIRECT == iter->eKeyType)
                {
                    outputCon2Error(pNode, *iter);
                    break;
                }
                else if (BY == iter->eKeyType)
                {
                    ++iter;
                    outputText("by", NULL, nOption++);
                    if (iter->eType == IDENTIFIER_NAME)
                    {
                        if(parseTmpLayerForOperation(iter->sValue))
                        {
                            break;
                        }
                    }
                    std::string sLayerName;
                    if (getLayerRefName(iter, pNode->end(), sLayerName))
                    {
                        outputText(NULL, sLayerName.c_str(), nOption++);
                        continue;
                    }
                }
                else if (LINK == iter->eKeyType)
                {
                    ++iter;
                    outputText("floating_polygon_name", iter->sValue.c_str(), nOption++);
                    break;
                }
                else if (ABUT_ALSO == iter->eKeyType)
                {
                    outputText("adjacent", NULL, nOption++);
                    break;
                }
            }
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                    if (hasUpperLayer)
                    {
                        outputText(NULL, sLayerName.c_str(), nOption++);
                    }
                    else
                    {
                        outputText(NULL, sLayerName.c_str(), nOption++);
                        hasUpperLayer = true;
                    }
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
        }
        ++iter;
    }
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void rcsSynPvrsConvertor::convNetInteract(rcsSynLayerOperationNode_T *pNode)
{

    m_sCurCommandString = "NET INTERACT";



    int layers = 0;
    int nOption = 0;
    (*m_oPVRSStream) << "geom_net_interact ( ";
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    std::list<rcsToken_T>::iterator oper = pNode->begin();
    while(iter != pNode->end())
    {
        switch(iter->eType)
        {
            case LAYER_OPERATION_KEYWORD:
                oper = iter;
                break;
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            {
                if (iter->eType == IDENTIFIER_NAME)
                {
                    if(parseTmpLayerForOperation(iter->sValue))
                    {
                        break;
                    }
                }
                std::string sLayerName;
                if (getLayerRefName(iter, pNode->end(), sLayerName))
                {
                   outputText(NULL, sLayerName.c_str(), nOption++);
                   ++layers;
                   continue;
                }
            }
            case OPERATOR:
            {
                std::string sConstraint;
                if(getConstraint(iter, pNode->end(), sConstraint))
                {
                    outputText(NULL, sConstraint.c_str(), nOption++);
                    continue;
                }
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
        }
        ++iter;
    }

    if (layers != 2)
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP38, oper->nLineNo, oper->sValue);
    }

    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void rcsSynPvrsConvertor::convLayoutCloneRotatedPlacements(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    std::list<rcsToken_T>::iterator it = pNode->begin();
    ++it;
    if (it != pNode->end() && (it->eKeyType == NO || it->eKeyType == YES))
    {
        (*m_oPVRSStream) << (_isTvf2Trs ? "layout_clone rotated_placement " : "layout_clone ( rotated_placement " ) << it->sValue
                << (_isTvf2Trs ? " " : " ) ")<< "\n";
    }
}

void rcsSynPvrsConvertor::convLayoutCloneTransformedPlacements(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    std::list<rcsToken_T>::iterator it = pNode->begin();
    ++it;
    if (it != pNode->end() && (it->eKeyType == NO || it->eKeyType == YES))
    {
        (*m_oPVRSStream) << ( _isTvf2Trs ? "layout_clone transformed_placement " : "layout_clone ( transformed_placement " )
                << it->sValue << (_isTvf2Trs ? " " : " ) ") << "\n";
    }
}

void rcsSynPvrsConvertor::convLayoutPathWidthMultiple(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    std::list<rcsToken_T>::iterator it = pNode->begin();
    (*m_oPVRSStream) << ( _isTvf2Trs ? "layout_path_width_multiple " : "layout_path_width_multiple ( " );

    while (++it != pNode->end())
    {
        (*m_oPVRSStream) << it->sValue << " ";
    }

    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;

}

void rcsSynPvrsConvertor::convDFMDefaultsRDB(rcsSynSpecificationNode_T *pNode)
{
    bool _isTvf2Trs = rcsManager_T::getInstance()->isTvf2Trs();
    std::list<rcsToken_T>::iterator it = pNode->begin();
    (*m_oPVRSStream) << ( _isTvf2Trs ? "dfm_default_result " : "dfm_default_result ( " );

    hvUInt32 nOption = 0;
    ++it;
    while (it != pNode->end())
    {
        switch (it->eKeyType)
        {
            case FILE_SECOND:
            {
                outputText("file", NULL, nOption++);
                ++it;
                continue;
            }
            case NOFILE:
            {
                outputText("nofile", NULL, nOption++);
                ++it;
                continue;
            }
            case NODAL:
            {
                outputText("node", NULL, nOption++);
                if (++it != pNode->end())
                {
                    if (it->eKeyType == ALL)
                    {
                        outputText("all", NULL, nOption++);
                        ++it;
                        continue;
                    }
                    else if (it->eKeyType == OTHER)
                    {
                        outputText("other", NULL, nOption++);
                        ++it;
                        continue;
                    }
                    else if (it->eKeyType == NULL_SECOND)
                    {
                        outputText("unset", NULL, nOption++);
                        ++it;
                        continue;
                    }
                }
                continue;
            }
            case NOPSEUDO:
            {
                outputText("flatten_injection", NULL, nOption++);
                if (++it != pNode->end())
                {
                    if (it->eKeyType == NULL_SECOND)
                    {
                        outputText("unset", NULL, nOption++);
                        ++it;
                        continue;
                    }
                }
                continue;
            }
            case NOEMPTY:
            {
                outputText("ignore_empty", NULL, nOption++);
                if (++it != pNode->end())
                {
                    if (it->eKeyType == NULL_SECOND)
                    {
                        outputText("unset", NULL, nOption++);
                        ++it;
                        continue;
                    }
                }
                continue;
            }
            case OUTPUT:
            {
                outputText("output_boundary", NULL, nOption++);
                if (++it != pNode->end())
                {
                    if (it->eKeyType == NULL_SECOND)
                    {
                        outputText("unset", NULL, nOption++);
                        ++it;
                        continue;
                    }
                }
                continue;
            }
            case ALL_CELLS:
            {
                outputText("all_cell", NULL, nOption++);
                if (++it != pNode->end())
                {
                    if (it->eKeyType == NULL_SECOND)
                    {
                        outputText("unset", NULL, nOption++);
                        ++it;
                        continue;
                    }
                }
                continue;
            }
            case CELL_SPACE:
            {
                outputText("cell_coordinate", NULL, nOption++);
                if (++it != pNode->end())
                {
                    if (it->eKeyType == NULL_SECOND)
                    {
                        outputText("unset", NULL, nOption++);
                        ++it;
                        continue;
                    }
                }
                continue;
            }
            case JOINED:
            {
                outputText("joined", NULL, nOption++);
                if (++it != pNode->end())
                {
                    if (it->eKeyType == NULL_SECOND)
                    {
                        outputText("unset", NULL, nOption++);
                        ++it;
                        continue;
                    }
                }
                continue;
            }
            case RESULT_CELLS:
            {
                outputText("result_cells", NULL, nOption++);
                if (++it != pNode->end())
                {
                    if (it->eKeyType == NULL_SECOND)
                    {
                        outputText("unset", NULL, nOption++);
                        ++it;
                        continue;
                    }
                }
                continue;
            }
            case CHECKNAME:
            {
                outputText("rule_name", NULL, nOption++);
                if (++it != pNode->end())
                {
                    if (it->eKeyType == NULL_SECOND)
                    {
                        outputText("unset", NULL, nOption++);
                        ++it;
                        continue;
                    }
                }
                continue;
            }
            case MAXIMUM:
            {
                outputText("max_result", NULL, nOption++);
                if (++it != pNode->end())
                {
                    if (it->eKeyType == NULL_SECOND)
                    {
                        outputText("unset", NULL, nOption++);
                        ++it;
                        continue;
                    }
                    else if (it->eKeyType == ALL)
                    {
                        outputText("all", NULL, nOption++);
                        ++it;
                        continue;
                    }
                }
                continue;
            }
            default:
            {
                outputOriginString(it);
                ++it;
            }
        }
    }

    if (!_isTvf2Trs)
    {
        (*m_oPVRSStream) << " ) ";
    }
    if (!m_isInLayerDefinition)
        (*m_oPVRSStream) << std::endl;
}


void rcsSynPvrsConvertor::convDFMFill(rcsSynLayerOperationNode_T *pNode)
{
    std::list<rcsToken_T>::iterator it = pNode->begin();
    (*m_oPVRSStream) << "smart_fill ( ";

    int nOption = 0;
    while (++it != pNode->end())
    {
        switch (it->eKeyType)
        {
            case COLOR:
                outputText("pattern", NULL, nOption++);
                break;
            case HLAYER:
                outputText("hier_layer", NULL, nOption++);
                break;
            case COMPRESS:
                outputText("compact", NULL, nOption++);
                break;
            case ENVELOPE:
                outputText("enve_extent", NULL, nOption++);
                break;
            default:

                outputOriginString(it);
                break;
        }
        (*m_oPVRSStream) << " ";
    }

    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convRetNMDPCCmd(rcsSynLayerOperationNode_T *pNode)
{
    std::list<rcsToken_T>::iterator it = pNode->begin();
    (*m_oPVRSStream) << "dfm_color ( ";

    typedef std::map<std::string, rcsLithoFileSpec> LithoMap;
    const LithoMap &lithoMap = rcsManager_T::getInstance()->getLithoFileSpec();
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    LithoMap::const_iterator lithoIt = lithoMap.end();
    std::vector<std::string> _layerNames;
    std::string _strMapType;
    std::list<rcsToken_T>::iterator beginIter = iter;
    ++iter;
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case FILE_SECOND:
                {
                    if (lithoIt != lithoMap.end())
                    {
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, KEY1,
                                            iter->nLineNo, iter->sValue);
                    }
                    ++iter;
                    string lithoFileName = iter->sValue;
                    lithoIt = lithoMap.find(lithoFileName);
                    ++iter;
                    continue;
                }
                case MAP:
                {
                    ++iter;
                    _strMapType = iter->sValue;
                    ++iter;
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                _layerNames.push_back(iter->sValue);
                break;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    std::swap(_layerNames[0], _layerNames[1]);
    const rcsLithoFileSpec &lithoSpec = lithoIt->second;
    if(strcasecmp(_strMapType.c_str(), "anchor_conflict") == 0)
        (*m_oPVRSStream) << "TAG_CONFLICT ";
    else if(strcasecmp(_strMapType.c_str(), "anchor_path") == 0)
        (*m_oPVRSStream) << "TAG_PATH ";
    else if(strcasecmp(_strMapType.c_str(), "anchor_self_conflict") == 0)
        (*m_oPVRSStream) << "TAG_SELF_CONFLICT ";
    else if(strcasecmp(_strMapType.c_str(), "conflict") == 0)
        (*m_oPVRSStream) << "CONFLICT ";
    else if(strcasecmp(_strMapType.c_str(), "loop") == 0)
        (*m_oPVRSStream) << "LOOP ";
    else if(strcasecmp(_strMapType.c_str(), "mask0") == 0)
        (*m_oPVRSStream) << "MASK0 ";
    else if(strcasecmp(_strMapType.c_str(), "mask1") == 0)
        (*m_oPVRSStream) << "MASK1 ";
    else if(strcasecmp(_strMapType.c_str(), "ring") == 0)
        (*m_oPVRSStream) << "RING ";
    else if(strcasecmp(_strMapType.c_str(), "self_conflict") == 0)
        (*m_oPVRSStream) << "SELF_CONFLICT ";
    else if(strcasecmp(_strMapType.c_str(), "warning") == 0)
        (*m_oPVRSStream) << "WARNING ";
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, USER1,
                            beginIter->nLineNo, _strMapType + " cannot be used here");
    }
    (*m_oPVRSStream) << _layerNames[0] << " ";
    const std::vector<rcsLithoFileAction> &acts = lithoSpec.m_stAction;
    for (size_t i=0; i!=acts.size(); ++i)
    {
        const rcsLithoFileAction& action = acts[i];
        if (false == action.m_hasStitchMode)
        {
            (*m_oPVRSStream) << action.m_actionLayer << " ";
            (*m_oPVRSStream) << "( ";
            if(action.m_bSame)
                (*m_oPVRSStream) << "SAME ";
            else
                (*m_oPVRSStream) << "OPPOSITE ";
            (*m_oPVRSStream) << action.m_iPriority << " ";
            (*m_oPVRSStream) << ") ";
        }
        else
        {
            (*m_oPVRSStream) << action.m_actionLayer << " ";
            (*m_oPVRSStream) << "(PATCH ";
            (*m_oPVRSStream) << "MAXIMIZE ";
            (*m_oPVRSStream) << action.m_iPriority;
            (*m_oPVRSStream) << ") ";
        }
    }

    const std::vector<rcsLithoFileAnchor> &stAnchor = lithoSpec.m_stAnchor;
    for (hvUInt32 i=0; i<2 && i<stAnchor.size(); ++i)
    {
        const rcsLithoFileAnchor &anchorLayer = stAnchor[i];
        (*m_oPVRSStream) << anchorLayer.m_anchorLayer << " ";
        (*m_oPVRSStream) << "(TAG ";
        if(anchorLayer.m_isMask0)
            (*m_oPVRSStream) << "MASK0 ";
        else
            (*m_oPVRSStream) << "MASK1 ";
        (*m_oPVRSStream) << anchorLayer.m_iPriority << " ";
        (*m_oPVRSStream) << ") ";
    }

    (*m_oPVRSStream) << "UNSREENED ";
    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convRetTPCmd(rcsSynLayerOperationNode_T *pNode)
{
    std::list<rcsToken_T>::iterator it = pNode->begin();
    (*m_oPVRSStream) << "dfm_color ( 3 ";

    typedef std::map<std::string, rcsLithoFileSpec> LithoMap;
    const LithoMap &lithoMap = rcsManager_T::getInstance()->getLithoFileSpec();
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    LithoMap::const_iterator lithoIt = lithoMap.end();
    std::vector<std::string> _layerNames;
    std::string _strMapType;
    std::list<rcsToken_T>::iterator beginIter = iter;
    ++iter;
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case FILE_SECOND:
                {
                    if (lithoIt != lithoMap.end())
                    {
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, KEY1,
                                            iter->nLineNo, iter->sValue);
                    }
                    ++iter;
                    string lithoFileName = iter->sValue;
                    lithoIt = lithoMap.find(lithoFileName);
                    ++iter;
                    continue;
                }
                case MAP:
                {
                    ++iter;
                    _strMapType = iter->sValue;
                    ++iter;
                    continue;
                }
                case CLUSTER:
                {
                    _strMapType += " ";
                    _strMapType += iter->sValue;
                    ++iter;
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                _layerNames.push_back(iter->sValue);
                break;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    const rcsLithoFileSpec &lithoSpec = lithoIt->second;
    if(strcasecmp(_strMapType.c_str(), "MASK1") == 0)
        (*m_oPVRSStream) << "MASK1 ";
    else if(strcasecmp(_strMapType.c_str(), "MASK2") == 0)
        (*m_oPVRSStream) << "MASK2 ";
    else if(strcasecmp(_strMapType.c_str(), "MASK3") == 0)
        (*m_oPVRSStream) << "MASK3 ";
    else if(strcasecmp(_strMapType.c_str(), "ANCHOR_SELF_CONFLICT") == 0)
        (*m_oPVRSStream) << "TAG_SELF_CONFLICT_POLYGON ";
    else if(strcasecmp(_strMapType.c_str(), "CONFLICT") == 0)
        (*m_oPVRSStream) << "CONFLICT ";
    else if(strcasecmp(_strMapType.c_str(), "DIRECT_ANCHOR_CONFLICT CLUSTER") == 0)
        (*m_oPVRSStream) << "DIRECT_TAG_CONFLICT_SPACER ";
    else if(strcasecmp(_strMapType.c_str(), "EQUIV_LOOP") == 0)
        (*m_oPVRSStream) << "EQV_CYCLE_GROUP ";
    else if(strcasecmp(_strMapType.c_str(), "EQUIV_LOOP_POLYGONS") == 0)
        (*m_oPVRSStream) << "EQV_CYCLE_POLYGON ";
    else if(strcasecmp(_strMapType.c_str(), "EQUIV_LOOP_EQUIV") == 0)
        (*m_oPVRSStream) << "EQV_CYCLE_EQV_POLYGON ";
    else if(strcasecmp(_strMapType.c_str(), "EQUIV_LOOP_SEP CLUSTER") == 0)
        (*m_oPVRSStream) << "EQV_CYCLE_SPACER ";
    else if(strcasecmp(_strMapType.c_str(), "EQUIV_LOOP_VIOLATION_SEP CLUSTER") == 0)
        (*m_oPVRSStream) << "EQV_CYCLE_VIOLATION_SPACER ";
    else if(strcasecmp(_strMapType.c_str(), "OPPOSITE_ANCHOR_EQUIV") == 0)
        (*m_oPVRSStream) << "OPPOSITE_TAG_EQV_POLYGON ";
    else if(strcasecmp(_strMapType.c_str(), "OPPOSITE_ANCHOR_PATH") == 0)
        (*m_oPVRSStream) << "OPPOSITE_TAG_GROUP ";
    else if(strcasecmp(_strMapType.c_str(), "OPPOSITE_ANCHOR_POLYGONS") == 0)
        (*m_oPVRSStream) << "OPPOSITE_TAG_POLYGON ";
    else if(strcasecmp(_strMapType.c_str(), "OPPOSITE_ANCHOR_SEP CLUSTER") == 0)
        (*m_oPVRSStream) << "OPPOSITE_TAG_SPACER ";
    else if(strcasecmp(_strMapType.c_str(), "REMAINING_VIOLATIONS") == 0)
        (*m_oPVRSStream) << "LASTING_VIOLATION_GROUP ";
    else if(strcasecmp(_strMapType.c_str(), "REMAINING_VIOLATIONS_POLYGONS") == 0)
        (*m_oPVRSStream) << "LASTING_VIOLATION_POLYGON ";
    else if(strcasecmp(_strMapType.c_str(), "REMAINING_VIOLATIONS_SEP CLUSTER") == 0)
        (*m_oPVRSStream) << "LASTING_VIOLATION_SPACER ";
    else if(strcasecmp(_strMapType.c_str(), "SAME_ANCHOR_EQUIV") == 0)
        (*m_oPVRSStream) << "SAME_TAG_EQV_POLYGON ";
    else if(strcasecmp(_strMapType.c_str(), "SAME_ANCHOR_PATH") == 0)
        (*m_oPVRSStream) << "SAME_TAG_GROUP ";
    else if(strcasecmp(_strMapType.c_str(), "SAME_ANCHOR_POLYGONS") == 0)
        (*m_oPVRSStream) << "SAME_TAG_POLYGON ";
    else if(strcasecmp(_strMapType.c_str(), "SAME_ANCHOR_SEP CLUSTER") == 0)
        (*m_oPVRSStream) << "SAME_TAG_SPACER ";
    else if(strcasecmp(_strMapType.c_str(), "SAME_ANCHOR_VIOLATION_SEP CLUSTER") == 0)
        (*m_oPVRSStream) << "SAME_TAG_VIOLATION_SPACER ";
    else if(strcasecmp(_strMapType.c_str(), "SELF_CONFLICT_SEP CLUSTER") == 0)
        (*m_oPVRSStream) << "SELF_CONFLICT_SPACER ";
    else if(strcasecmp(_strMapType.c_str(), "SELF_CONTAINED_POLYGONS") == 0)
        (*m_oPVRSStream) << "SELF_REJECTED_VIOLATION_POLYGON ";
    else if(strcasecmp(_strMapType.c_str(), "SELF_CONTAINED_SEP CLUSTER") == 0)
        (*m_oPVRSStream) << "SELF_REJECTED_VIOLATION_SPACER ";
    else if(strcasecmp(_strMapType.c_str(), "SELF_CONTAINED_VIOLATION") == 0)
        (*m_oPVRSStream) << "SELF_REJECTED_VIOLATION_GROUP ";
    else if(strcasecmp(_strMapType.c_str(), "VIOLATION_SEPARATORS CLUSTER") == 0)
        (*m_oPVRSStream) << "VIOLATED_HT_SPACER ";
    else if(strcasecmp(_strMapType.c_str(), "REDUCED_TARGET") == 0)
        (*m_oPVRSStream) << "MERGE_POLYGON ";
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, USER1,
                            beginIter->nLineNo, _strMapType + " cannot be used here");
    }
    (*m_oPVRSStream) << _layerNames[0] << " ";

    {
        const std::vector<std::string> &sepLayers = lithoSpec.m_stSeparatorLayers;
        for (size_t i=0; i<sepLayers.size(); ++i)
        {
            const std::string &lithoLayer = sepLayers[i];
            (*m_oPVRSStream) << lithoLayer << " ";
        }

        for (hvUInt32 i=0; i<4; ++i)
        {
            const std::string &lithoLayer = lithoSpec.m_anchorForMP[i];
            if (lithoLayer.empty())
                continue;
            (*m_oPVRSStream) << lithoLayer << " ";
            (*m_oPVRSStream) << "(TAG ";
            switch(i)
            {
            case 0:
                (*m_oPVRSStream) << "MASK1 ";
                break;
            case 1:
                (*m_oPVRSStream) << "MASK2 ";
                break;
            case 2:
                (*m_oPVRSStream) << "MASK3 ";
                break;
            case 3:
                (*m_oPVRSStream) << "MASK4 ";
                break;
            }
            (*m_oPVRSStream) << ") ";
        }
    }

    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::convRetQPCmd(rcsSynLayerOperationNode_T *pNode)
{
    std::list<rcsToken_T>::iterator it = pNode->begin();
    (*m_oPVRSStream) << "dfm_color ( 4 ";

    typedef std::map<std::string, rcsLithoFileSpec> LithoMap;
    const LithoMap &lithoMap = rcsManager_T::getInstance()->getLithoFileSpec();
    std::list<rcsToken_T>::iterator iter = pNode->begin();
    LithoMap::const_iterator lithoIt = lithoMap.end();
    std::vector<std::string> _layerNames;
    std::string _strMapType;
    std::list<rcsToken_T>::iterator beginIter = iter;
    ++iter;
    while(iter != pNode->end())
    {
        if(SECONDARY_KEYWORD == iter->eType)
        {
            switch(iter->eKeyType)
            {
                case FILE_SECOND:
                {
                    if (lithoIt != lithoMap.end())
                    {
                        throw rcErrorNode_T(rcErrorNode_T::ERROR, KEY1,
                                            iter->nLineNo, iter->sValue);
                    }
                    ++iter;
                    string lithoFileName = iter->sValue;
                    lithoIt = lithoMap.find(lithoFileName);
                    ++iter;
                    continue;
                }
                case MAP:
                {
                    ++iter;
                    _strMapType = iter->sValue;
                    ++iter;
                    continue;
                }
                case CLUSTER:
                {
                    _strMapType += " ";
                    _strMapType += iter->sValue;
                    ++iter;
                    continue;
                }
                default:
                    break;
            }
        }

        switch(iter->eType)
        {
            case IDENTIFIER_NAME:
            case STRING_CONSTANTS:
            case NUMBER:
            case OPERATOR:
            {
                _layerNames.push_back(iter->sValue);
                break;
            }
            default:
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
                break;
        }
        ++iter;
    }
    const rcsLithoFileSpec &lithoSpec = lithoIt->second;
    if(strcasecmp(_strMapType.c_str(), "MASK1") == 0)
        (*m_oPVRSStream) << "MASK1 ";
    else if(strcasecmp(_strMapType.c_str(), "MASK2") == 0)
        (*m_oPVRSStream) << "MASK2 ";
    else if(strcasecmp(_strMapType.c_str(), "MASK3") == 0)
        (*m_oPVRSStream) << "MASK3 ";
    else if(strcasecmp(_strMapType.c_str(), "MASK4") == 0)
        (*m_oPVRSStream) << "MASK4 ";
    else if(strcasecmp(_strMapType.c_str(), "ANCHOR_SELF_CONFLICT") == 0)
        (*m_oPVRSStream) << "TAG_SELF_CONFLICT_POLYGON ";
    else if(strcasecmp(_strMapType.c_str(), "CONFLICT") == 0)
        (*m_oPVRSStream) << "CONFLICT ";
    else if(strcasecmp(_strMapType.c_str(), "DIRECT_ANCHOR_CONFLICT CLUSTER") == 0)
        (*m_oPVRSStream) << "DIRECT_TAG_CONFLICT_SPACER ";
    else if(strcasecmp(_strMapType.c_str(), "REMAINING_VIOLATIONS") == 0)
        (*m_oPVRSStream) << "LASTING_VIOLATION_GROUP ";
    else if(strcasecmp(_strMapType.c_str(), "REMAINING_VIOLATIONS_POLYGONS") == 0)
        (*m_oPVRSStream) << "LASTING_VIOLATION_POLYGON ";
    else if(strcasecmp(_strMapType.c_str(), "REMAINING_VIOLATIONS_SEP CLUSTER") == 0)
        (*m_oPVRSStream) << "LASTING_VIOLATION_SPACER ";
    else if(strcasecmp(_strMapType.c_str(), "SELF_CONFLICT_SEP CLUSTER") == 0)
        (*m_oPVRSStream) << "SELF_CONFLICT_SPACER ";
    else if(strcasecmp(_strMapType.c_str(), "SELF_CONTAINED_POLYGONS") == 0)
        (*m_oPVRSStream) << "SELF_REJECTED_VIOLATION_POLYGON ";
    else if(strcasecmp(_strMapType.c_str(), "SELF_CONTAINED_SEP CLUSTER") == 0)
        (*m_oPVRSStream) << "SELF_REJECTED_VIOLATION_SPACER ";
    else if(strcasecmp(_strMapType.c_str(), "SELF_CONTAINED_VIOLATION") == 0)
        (*m_oPVRSStream) << "SELF_REJECTED_VIOLATION_GROUP ";
    else if(strcasecmp(_strMapType.c_str(), "VIOLATION_SEPARATORS CLUSTER") == 0)
        (*m_oPVRSStream) << "VIOLATED_HT_SPACER ";
    else if(strcasecmp(_strMapType.c_str(), "REDUCED_TARGET") == 0)
        (*m_oPVRSStream) << "MERGE_POLYGON ";
    else
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, USER1,
                            beginIter->nLineNo, _strMapType + " cannot be used here");
    }
    (*m_oPVRSStream) << _layerNames[0] << " ";

    {
        const std::vector<std::string> &sepLayers = lithoSpec.m_stSeparatorLayers;
        for (size_t i=0; i<sepLayers.size(); ++i)
        {
            const std::string &lithoLayer = sepLayers[i];
            (*m_oPVRSStream) << lithoLayer << " ";
        }

        for (hvUInt32 i=0; i<4; ++i)
        {
            const std::string &lithoLayer = lithoSpec.m_anchorForMP[i];
            if (lithoLayer.empty())
                continue;
            (*m_oPVRSStream) << lithoLayer << " ";
            (*m_oPVRSStream) << "(TAG ";
            switch(i)
            {
            case 0:
                (*m_oPVRSStream) << "MASK1 ";
                break;
            case 1:
                (*m_oPVRSStream) << "MASK2 ";
                break;
            case 2:
                (*m_oPVRSStream) << "MASK3 ";
                break;
            case 3:
                (*m_oPVRSStream) << "MASK4 ";
                break;
            }
            (*m_oPVRSStream) << ") ";
        }
    }

    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;
}

void
rcsSynPvrsConvertor::outputTrsPrefix(rcsSynSpecificationNode_T *pNode)
{
    switch(pNode->getSpecificationType())
    {
        case PORT_LAYER_MERGED_POLYGON:
        case LVS_NETLIST_CONNECT_PORT_NAMES:
        case LAYOUT_MERGE_ON_INPUT:
        case LAYOUT_PROPERTY_TEXT:
        case DFM_SELECT_CHECK_NODAL:
        case DFM_SUMMARY_REPORT:
        case LVS_RECOGNIZE_GATES_TOLERANCE:
        case LAYOUT_PROPERTY_AUDIT:
        case DRC_BOOLEAN_NOSNAP45:
        case DFM_FUNCTION:
        case DFM_DATABASE:
        case DFM_DATABASE_DIRECTORY:
        case DFM_DEFAULTS_RDB:
        case DFM_SPEC_SPATIAL_SAMPLE:
        case DFM_SPEC_VIA_REDUNDANCY:
        case DFM_SPEC_VIA:
        case DFM_SPEC_RANDOM_SPOT_FAULT:
        case DFM_ASSERT:
        case DFM_SPEC_FILL:
        case DFM_YS_AUTOSTART:
        case LVS_PIN_NAME_PROPERTY:
        case LVS_COMPONENT_SUBTYPE_PROPERTY:
        case LVS_COMPONENT_TYPE_PROPERTY:
        case LVS_AUTO_EXPAND_HCELL:
        case LVS_CENTER_DEVICE_PINS:
        case LVS_MOS_SWAPPABLE_PROPERTIES:
        case LVS_ANNOTATE_DEVICES:
        case LAYOUT_TEXT_FILE:
        case LVS_CONFIGURE_UNUSED_FILTER:
        case LVS_APPLY_UNUSED_FILTER:
        case DFM_DEFAULTS:
        case LAYOUT_PROPERTY_TEXT_OASIS:
        case PEX_CMD:
        case CAPACITANCE_CMD:
        case RESISTANCE_CMD:
        case DRC_PRINT_EXTENT:
        case NON_SUPPORT_SPECIFICATION:
            break;
        default:
            (*m_oPVRSStream) << "trs::";
            break;
    }
}

void
rcsSynPvrsConvertor::outputTrsPrefix(rcsSynLayerOperationNode_T *pNode)
{
    switch(pNode->getOperationType())
    {
#if 0
        case DFM_RDB_GDS:
        case DFM_RDB_OASIS:
#endif
        case POLYNET:
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
        case DFM_SHIFT:
        case DFM_SHIFT_EDGE:
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
        case TVF:
        case AND:
        case OR:
        case XOR:
        case NOT:
        case SPACE:
        case COPY:
            break;
        default:
            (*m_oPVRSStream) << "trs::";
            break;
    }
}

void
rcsSynPvrsConvertor::outputBlankLinesBefore(hvUInt32 nCurLine, bool isReset)
{
    std::map<hvUInt32, std::pair<hvUInt32, bool> >::iterator iter = m_blankLinesBefore.find(nCurLine);
    if (iter != m_blankLinesBefore.end())
    {
        if (iter->second.second)
        {
            const hvUInt32 nBlankLines = iter->second.first;
            for (hvUInt32 i = 0; i != nBlankLines; i++)
            {
                (*m_oPVRSStream) << "\n";
            }
        }
        iter->second.second = isReset;
    }
}

void rcsSynPvrsConvertor::initSimplePVRSmap()
{
    // dfm fill
    {
        rcs_SvrfRuleInfo &info = addSvrfKey(DFM_FILL, "smart_fill");
        addSvrfOption(info, COLOR, "pattern");
        addSvrfOption(info, HLAYER, "hier_layer");
        addSvrfOption(info, COMPRESS, "compact");
        addSvrfOption(info, ENVELOPE, "enve_extent");
        addSvrfOption(info, VERSION_SECOND, "version");
    }

    // dfm fill region
    {
        rcs_SvrfRuleInfo &info = addSvrfKey(DFM_FILL_REGION, "smart_fill_region");
        addSvrfOption(info, DEBUG_SPACE, "debug space");
        addSvrfOption(info, ORTHO, "orthogonal");
    }

    // dfm fill wrap
    {
        rcs_SvrfRuleInfo &info = addSvrfKey(DFM_FILL_WRAP, "smart_fill_wrap");
        addSvrfOption(info, OUTPUT, "output");
        addSvrfOption(info, MINLENGTH, "wrap_min_length");
    }

    // dfm compress
    {
        rcs_SvrfRuleInfo &info = addSvrfKey(DFM_COMPRESS, "smart_compact");
        addSvrfOption(info, FILL_SPEC, "fill_spec");
        addSvrfOption(info, HLAYER, "hier_layer");
        addSvrfOption(info, OUTPUT, "output");
        addSvrfOption(info, PREFIX, "with_prefix");
        addSvrfOption(info, COLOR, "pattern");
    }

    // dfm spec fill shape
    {
        rcs_SvrfRuleInfo &info = addSvrfKey(DFM_SPEC_FILL_SHAPE, "smart_fill_spec_shape");
        addSvrfOption(info, RECTFILL, "rect_shape");
        addSvrfOption(info, POLYFILL, "poly_shape");
        addSvrfOption(info, STRETCHFILL, "strech_shape");
        addSvrfOption(info, STRIPE, "stripe_shape");
//        addSvrfOption(info, HORIZONTAL, "horizontal");
//        addSvrfOption(info, VERTICAL, "vertical");
//        addSvrfOption(info, CENTERED, "centered");
//        addSvrfOption(info, TRIM, "trim");
//        addSvrfOption(info, SLIP, "slip");
        addSvrfOption(info, MINLENGTH, "stripe_minlength");
        addSvrfOption(info, MAXLENGTH, "stripe_maxlength");
//        addSvrfOption(info, GAP, "gap");
        addSvrfOption(info, SPACE, "dis");
        addSvrfOption(info, SPACEX, "dis_x");
        addSvrfOption(info, SPACEY, "dis_y");
        addSvrfOption(info, INTERIOR, "inner");
        addSvrfOption(info, SPACE_EDGE, "dis_edge");
        addSvrfOption(info, INSIDE_BY, "inside_by");
        addSvrfOption(info, OUTSIDE_BY, "outside_by");
        addSvrfOption(info, SPACEXY, "dis_xy");
        addSvrfOption(info, EXTEND_BY, "extend_by");
        addSvrfOption(info, SHAPESPACE, "shape_dis");
        addSvrfOption(info, BY_EXTENT, "by_extent");
        addSvrfOption(info, SETBACK, "diagonal_dis");
        addSvrfOption(info, ALLOW, "min_dis");
        addSvrfOption(info, ALLOWX, "min_xdis");
        addSvrfOption(info, ALLOWY, "min_ydis");
        addSvrfOption(info, ON_SECOND, "on");
        addSvrfOption(info, OPTIMIZER, "optimizer_name");
        addSvrfOption(info, OPPOSITE_EXTENDED, "rect_expand");
        addSvrfOption(info, COLOR_SCHEME, "patterning");
        addSvrfOption(info, CHECKER, "checker_2c");
        addSvrfOption(info, CHECKER3, "checker_3c");
        addSvrfOption(info, QUAD, "checker_4c");
        addSvrfOption(info, ALTERNATE, "rotary");
        addSvrfOption(info, ALTERNATEX, "rotary_x");
        addSvrfOption(info, ALTERNATEY, "rotary_y");
    }

    // dfm spec space
    {
        rcs_SvrfRuleInfo &info = addSvrfKey(DFM_SPEC_SPACE, "smart_fill_space_spec");
        addSvrfOption(info, TYPE, "space_type");
        addSvrfOption(info, WORK_ZONE, "fill_area");
        addSvrfOption(info, EXCLUSION_ZONES, "nofill_area");
        addSvrfOption(info, WRAP_PATTERN, "wrap_pattern");
        addSvrfOption(info, SAME_COLOR, "same_color");
        addSvrfOption(info, LOCAL_CONSTRAINTS, "normal_constraints");
        addSvrfOption(info, SPACE, "dis");
        addSvrfOption(info, SPACEX, "dis_x");
        addSvrfOption(info, SPACEY, "dis_y");
        addSvrfOption(info, SPACEXY, "dis_xy");
        addSvrfOption(info, EXTEND_BY, "extend_by");
        addSvrfOption(info, WIDTH, "wrap_width");
        addSvrfOption(info, HWIDTH, "wrap_x_width");
        addSvrfOption(info, VWIDTH, "wrap_y_width");
        addSvrfOption(info, INTERIOR, "inner");
        addSvrfOption(info, SPACE_EDGE, "dis_edge");
        addSvrfOption(info, INSIDE_BY, "inside_by");
        addSvrfOption(info, OUTSIDE_BY, "outside_by");
        addSvrfOption(info, SPACE_EDGE_ALIGN, "dis_edge_align");
        addSvrfOption(info, MAJOR, "major_layer");
        addSvrfOption(info, C2C, "diagonal_dis");
        addSvrfOption(info, EDGE, "edge");
    }

    // dfm spec fill guide
    {
        rcs_SvrfRuleInfo &info = addSvrfKey(DFM_SPEC_FILL_GUIDE, "smart_fill_spec_control");
        addSvrfOption(info, GUIDELAYER, "control_space_spec");
        addSvrfOption(info, MINLENGTH, "wrap_min_length");
        addSvrfOption(info, SPACE, "space_spec");
        addSvrfOption(info, COLOR, "pattern");
        addSvrfOption(info, WRAP_COLOR, "wrap_pattern");
    }

    // dfm spec fill optimizer
    {
        rcs_SvrfRuleInfo &info = addSvrfKey(DFM_SPEC_FILL_OPTIMIZER, "smart_fill_spec_optimizer");
        addSvrfOption(info, NOEMPTY, "no_empty");
        addSvrfOption(info, TOMAX, "fillmax");
        addSvrfOption(info, WINDOW, "window");
        addSvrfOption(info, STEP, "step");
        addSvrfOption(info, BACKUP, "backup");
        addSvrfOption(info, INSIDE_OF_LAYER, "inside_of_layer");
        addSvrfOption(info, BY_POLYGON, "by_polygon");
        addSvrfOption(info, MAGNITUDE, "den_mag");
        addSvrfOption(info, ABSOLUTE, "absolute");
        addSvrfOption(info, RELATIVE, "relative");
        addSvrfOption(info, GRADIENT, "den_gradient");
        addSvrfOption(info, CORNER, "corner");
    }

    // dfm spec fill optimizer
    {
        rcs_SvrfRuleInfo &info = addSvrfKey(DFM_SPEC_FILL_REGION, "smart_fill_spec_fillregion");
        addSvrfOption(info, SPACE, "space_spec");
    }

    // dfm spec fill
    {
        rcs_SvrfRuleInfo &info = addSvrfKey(DFM_SPEC_FILL, "smart_fill_spec");
        addSvrfOption(info, OPTIMIZER, "optimizer_name");
        addSvrfOption(info, RDB, "rdb");
        addSvrfOption(info, MISSEDONLY, "missedonly");
        addSvrfOption(info, FILLEDONLY, "filledonly");
        addSvrfOption(info, WINDOW, "window");
        addSvrfOption(info, STEP, "step");
        addSvrfOption(info, BACKUP, "backup");
        addSvrfOption(info, GWINDOW, "gwindow");
        addSvrfOption(info, WRAP, "wrap");
        addSvrfOption(info, INSIDE_OF_EXTENT, "inside_of_boundary");
        addSvrfOption(info, INSIDE_OF, "inside_of");
        addSvrfOption(info, INSIDE_OF_LAYER, "inside_of_layer");
        addSvrfOption(info, BY_POLYGON, "by_polygon");
        addSvrfOption(info, INITIAL_SECOND, "fill_initial");
        addSvrfOption(info, INITIAL_FILLREGION, "fill_initial_fillregion");
        addSvrfOption(info, INITIAL_INSIDE_OF_LAYER, "fill_initial_inside_of_layer");
        addSvrfOption(info, INITIAL_POINT, "fill_initial_point");
        addSvrfOption(info, COLOR_SCHEME, "patterning");
        addSvrfOption(info, CHECKER, "checker_2c");
        addSvrfOption(info, CHECKER3, "checker_3c");
        addSvrfOption(info, QUAD, "checker_4c");
        addSvrfOption(info, ALTERNATE, "rotary");
        addSvrfOption(info, ALTERNATEX, "rotary_x");
        addSvrfOption(info, ALTERNATEY, "rotary_y");
        addSvrfOption(info, DEBUG_SPACE, "debug space");
        addSvrfOption(info, PRIORITY, "priority");
        addSvrfOption(info, NOOVERLAP, "nooverlap");
        addSvrfOption(info, DISPLACE, "fill_shift");
        addSvrfOption(info, OFFSET, "offset");
        addSvrfOption(info, SETBACK, "diagonal_dis");
        addSvrfOption(info, ALLOW, "min_dis");
        addSvrfOption(info, ALLOWX, "min_xdis");
        addSvrfOption(info, ALLOWY, "min_ydis");
        addSvrfOption(info, AUTOROTATE, "rotate_dir");
        addSvrfOption(info, FILLMIN, "minfill");
        addSvrfOption(info, FILLMAX, "maxfill");
        addSvrfOption(info, REPEAT, "opt_number");
        addSvrfOption(info, SHAPESPACE, "shape_dis");
        addSvrfOption(info, BY_EXTENT, "by_extent");
        addSvrfOption(info, EFFORT, "opt_tradeoff");
        addSvrfOption(info, EFFORT_PITCH, "opt_tradeoff pitch");
        addSvrfOption(info, SPACE, "dis");
        addSvrfOption(info, SPACEX, "dis_x");
        addSvrfOption(info, SPACEY, "dis_y");
        addSvrfOption(info, INTERIOR, "inner");
        addSvrfOption(info, SPACE_EDGE, "dis_edge");
        addSvrfOption(info, INSIDE_BY, "inside_by");
        addSvrfOption(info, OUTSIDE_BY, "outside_by");
        addSvrfOption(info, SPACEXY, "dis_xy");
        addSvrfOption(info, EXTEND_BY, "extend_by");
        addSvrfOption(info, EDGE, "edge");
        addSvrfOption(info, SPACE_WRAP, "dis_wrap");
        addSvrfOption(info, OUTPUT, "output");
        addSvrfOption(info, FILLSHAPE, "fill_shape");
        addSvrfOption(info, RECTFILL, "rect_shape");
        addSvrfOption(info, POLYFILL, "poly_shape");
        addSvrfOption(info, STRETCHFILL, "stretch_shape");
        addSvrfOption(info, STRIPE, "stripe_shape");
        addSvrfOption(info, TRIM, "trim");
        addSvrfOption(info, SLIP, "slip");
        addSvrfOption(info, MINLENGTH, "stripe_minlength");
        addSvrfOption(info, MAXLENGTH, "stripe_maxlength");
        addSvrfOption(info, GAP, "gap");
        addSvrfOption(info, FILLSTACK, "stack_shape");
        addSvrfOption(info, MINSTACK, "min_stack_num");
        addSvrfOption(info, MAXSTACK, "max_stack_num");
        addSvrfOption(info, VIA, "via_layer");
        addSvrfOption(info, FLEXSHAPE, "flex_shape");
        addSvrfOption(info, MEGACELL, "bigcell");
        addSvrfOption(info, NUMBER_SECOND, "flex_number");
        addSvrfOption(info, EVEN, "even");
        addSvrfOption(info, ODD, "odd");
        addSvrfOption(info, MAXCORES, "max_core");
        addSvrfOption(info, MAX_SECOND, "max");
        addSvrfOption(info, SPLIT, "break_up");
        addSvrfOption(info, UNBOUNDED, "boundless");
        addSvrfOption(info, ADJ, "nearby");
        addSvrfOption(info, NONADJ, "non_nearby");
        addSvrfOption(info, TOP, "top");
        addSvrfOption(info, BOTTOM, "bottom");
        addSvrfOption(info, LEFT, "left");
        addSvrfOption(info, RIGHT, "right");
        addSvrfOption(info, UR, "ur");
        addSvrfOption(info, UL, "ul");
        addSvrfOption(info, LR, "lr");
        addSvrfOption(info, LL, "ll");
        addSvrfOption(info, HORIZONTAL, "horizontal");
        addSvrfOption(info, VERTICAL, "vertical");
        addSvrfOption(info, NOTHORIZONTAL, "nothorizontal");
        addSvrfOption(info, NOTVERTICAL, "notvertical");
        addSvrfOption(info, FLEX_SECOND, "flex");
        addSvrfOption(info, CAP, "hat");
//        addSvrfOption(info, PRIORITY, "priority");
        addSvrfOption(info, CLOSETO, "close_to");
        addSvrfOption(info, HPERIODIC, "hperiodic");
        addSvrfOption(info, VPERIODIC, "vperiodic");
        addSvrfOption(info, PERIOD, "period");
        addSvrfOption(info, KEEP, "keep");
        addSvrfOption(info, ADJOFF, "nearby_off");
        addSvrfOption(info, FLIPX, "mirror_x");
        addSvrfOption(info, ROTATION, "rolling");
        addSvrfOption(info, PARTITION, "partition");
//        addSvrfOption(info, GAP, "gap");
        addSvrfOption(info, RESTRICT, "restrict_to_main");
        addSvrfOption(info, MIN_SECOND, "min");
        addSvrfOption(info, TOP_LEFT, "top left");
        addSvrfOption(info, TOP_RIGHT, "top right");
        addSvrfOption(info, BOTTOM_LEFT, "bottom left");
        addSvrfOption(info, BOTTOM_RIGHT, "bottom right");
        addSvrfOption(info, FILLCORNER, "fill_corner");
        addSvrfOption(info, FIELD, "field");
        addSvrfOption(info, VALIGNCUT, "valigncut");
        addSvrfOption(info, OPPOSITE_EXTENDED, "rect_expand");
    }
    // dfm spec fill wap
    {
        rcs_SvrfRuleInfo &info = addSvrfKey(DFM_SPEC_FILL_WRAP, "smart_fill_wrap_spec");
        addSvrfOption(info, VSHAPE, "shape_ydir");
        addSvrfOption(info, STEP, "step");
        addSvrfOption(info, HSHAPE, "shape_xdir");
        addSvrfOption(info, EDGE_ALIGN, "align_edge");
        addSvrfOption(info, INSIDE_BY, "inside_by");
        addSvrfOption(info, EXTEND_BY, "extend_by");
        addSvrfOption(info, MAJOR, "major_color");
        addSvrfOption(info, C2C, "diagonal_dis");
        addSvrfOption(info, SAME_COLOR, "same_color");
        addSvrfOption(info, WRAPS, "wrap_num");
        addSvrfOption(info, LONGSHAPE, "wrap_longest");
        addSvrfOption(info, WIDESHAPE, "wrap_widest");
        addSvrfOption(info, UNLIMITED, "limitless");
        addSvrfOption(info, FLEXSTEP, "flexstep");
        addSvrfOption(info, ALLOW, "min_dis");
        addSvrfOption(info, ALLOWX, "min_xdis");
        addSvrfOption(info, ALLOWY, "min_ydis");
        addSvrfOption(info, SHAPE, "shape");
    }

    // dfm mat
    {
        rcs_SvrfRuleInfo &info = addSvrfKey(DFM_MAT, "smart_fillregion_smoother");
        addSvrfOption(info, JOGX, "stair_x");
        addSvrfOption(info, JOGY, "stair_y");
        addSvrfOption(info, BUMPX, "bugle_x");
        addSvrfOption(info, BUMPY, "bugle_y");
        addSvrfOption(info, NOTCHX, "cove_x");
        addSvrfOption(info, NOTCHY, "cove_y");
        addSvrfOption(info, NARROWTRACKX, "narrowpath_x");
        addSvrfOption(info, NARROWTRACKY, "narrowpath_Y");
        addSvrfOption(info, VPROXIMITY, "v_approach");
        addSvrfOption(info, HPROXIMITY, "h_approach");
        addSvrfOption(info, GAPX, "gap_x");
        addSvrfOption(info, GAPY, "gap_y");
        addSvrfOption(info, EXTEND_BY, "extend_by");
        addSvrfOption(info, PITCHX, "pitch_x");
        addSvrfOption(info, PITCHY, "pitch_y");
        addSvrfOption(info, ALIGNBY, "align_by");
        addSvrfOption(info, ALIGNSLIPR, "align_slip_r");
        addSvrfOption(info, ALIGNSLIPL, "align_slip_l");
        addSvrfOption(info, ALIGNSLIPT, "align_slip_t");
        addSvrfOption(info, ALIGNSLIPB, "align_slip_b");
        addSvrfOption(info, DISPLACE, "fill_shift");
        addSvrfOption(info, HDISPLACE, "h_fill_shift");
        addSvrfOption(info, HALIGNBY, "h_align_by");
        addSvrfOption(info, ADJACENT, "adjacent");
        addSvrfOption(info, JOGS, "stairs");
        addSvrfOption(info, BUMPS, "bugles");
        addSvrfOption(info, NOTCHES, "coves");
        addSvrfOption(info, MAXPULLBACK, "maxpullback");
        addSvrfOption(info, PRIORITY, "priority");
        addSvrfOption(info, HORIZONTAL, "horizontal");
        addSvrfOption(info, VERTICAL, "vertical");
    }

    // dfm rdb {GDS OASIS}
    {
        rcs_SvrfRuleInfo &info = addSvrfKey(DFM_RDB, "dfm_result");
        addSvrfOption(info, NODAL, "node");
        addSvrfOption(info, ALL, "all");
        addSvrfOption(info, OTHER, "other");
        addSvrfOption(info, NOPSEUDO, "flatten_injection"); // also "flatten_injection" "not_keep_injected_cell"
        addSvrfOption(info, NOEMPTY, "ignore_empty");
        addSvrfOption(info, ALL_CELLS, "all_cell");
        addSvrfOption(info, OUTPUT, "output_boundary");
        addSvrfOption(info, MAXIMUM, "max_result");
        addSvrfOption(info, RESULT_CELLS, "result_cells");
        addSvrfOption(info, CHECKNAME, "rule_name");
        addSvrfOption(info, COMMENT_SECOND, "comment");
        addSvrfOption(info, NULL_SECOND, "unset");
        addSvrfOption(info, GDSII, "gds");
        addSvrfOption(info, OASIS, "oasis");
        addSvrfOption(info, DEFAULT, "default");
        addSvrfOption(info, USERMERGED, "merged");
        addSvrfOption(info, PSEUDO, "keep_injected");
        addSvrfOption(info, USER, "user_cell");
        addSvrfOption(info, COMBINE, "group");
        addSvrfOption(info, TOP, "top_cell");
        addSvrfOption(info, KEEPEMPTY, "keep_empty");
        addSvrfOption(info, PREFIX, "with_prefix");
        addSvrfOption(info, FILLONLY, "fill_only");
        addSvrfOption(info, APPEND, "with_append");
        addSvrfOption(info, AUTOREF, "auto_inject_array");
        addSvrfOption(info, PROPERTIES, "property");
        addSvrfOption(info, NOEMPTYDB, "ignore_empty_db");
    }

    // dfm defaults fill
    {
        rcs_SvrfRuleInfo &info = addSvrfKey(DFM_DEFAULTS_FILL, "dfm_default_fill");
        addSvrfOption(info, COMPRESS_PREFIX, "compact_with_prefix");
        addSvrfOption(info, COMPRESS_VERSION, "compact_version_number");
    }

    // dfm dp
    {
        rcs_SvrfRuleInfo &info = addSvrfKey(DFM_DP, "dfm_color");
        addSvrfOption(info, SAME, "same");
        addSvrfOption(info, OPPOSITE, "opposite");
        addSvrfOption(info, ANCHOR, "tag");
        addSvrfOption(info, MASK0, "mask0");
        addSvrfOption(info, MASK1, "mask1");
        addSvrfOption(info, STITCH, "patch");
        addSvrfOption(info, REQUIRED, "required");
        addSvrfOption(info, USER, "user");
        addSvrfOption(info, MINIMIZE, "minimize");
        addSvrfOption(info, MAXIMIZE, "maximize");
        addSvrfOption(info, PREFER, "prefer");
        addSvrfOption(info, UNFILTERED, "unsreened");
        addSvrfOption(info, WAIVER, "ignore");

        addSvrfOption(info, ALL_ANCHOR_POLYGONS, "all_tag_polygon");
        addSvrfOption(info, ALL_ANCHOR_SEPARATORS, "all_tag_spacer");
        addSvrfOption(info, ALL_LOOP_POLYGONS, "all_loop_polygon ");
        addSvrfOption(info, ALL_LOOP_SEPARATORS, "all_loop_spacer");
        addSvrfOption(info, ANCHOR_SELF_CONFLICT_POLYGONS, "tag_self_conflict_polygon");
        addSvrfOption(info, CONFLICT, "conflict");
        addSvrfOption(info, CONFLICT_MARKER, "conflict_marker");
        addSvrfOption(info, DIRECT_ANCHOR_CONFLICT_SEPARATORS, "direct_tag_conflict_spacer");
        addSvrfOption(info, INVALID_ANCHOR0, "inactive_tag0");
        addSvrfOption(info, INVALID_ANCHOR1, "inactive_tag1");
        addSvrfOption(info, INVALID_SEPARATORS, "inactive_spacer");
        addSvrfOption(info, INVALID_STITCHES, "inactive_patch");

        //addSvrfOption(info, RING, "ring");
        addSvrfOption(info, SELF_CONFLICT_POLYGONS, "self_conflict_polygon");
        addSvrfOption(info, SELF_CONFLICT_SEPARATORS, "self_conflict_spacer");
        addSvrfOption(info, SHORTEST_ANCHOR_LINE, "shortest_tag_path");
        addSvrfOption(info, SHORTEST_ANCHOR_POLYGONS, "shortest_tag_polygon");
        addSvrfOption(info, SHORTEST_ANCHOR_SEPARATORS, "shortest_tag_spacer");
        addSvrfOption(info, SHORTEST_LOOP_LINE, "shortest_loop_path");
        addSvrfOption(info, SHORTEST_LOOP_LINE_UNMERGED, "shortest_loop_path_unmerged");
        addSvrfOption(info, SHORTEST_LOOP_POLYGONS, "shortest_loop_polygon");
        addSvrfOption(info, SHORTEST_LOOP_SEPARATORS, "shortest_loop_spacer");
        addSvrfOption(info, VIOLATED_OPTIONAL_SEPARATORS, "violated_lr_spacer");
        addSvrfOption(info, VIOLATED_OPTIONAL_STITCHES, "violated_lr_patch");
        addSvrfOption(info, VIOLATED_SEPARATORS, "violated_ht_spacer");
        addSvrfOption(info, VIOLATED_STITCHES, "violated_ht_patch");
        addSvrfOption(info, WARNING_LOOP_LINE, "warning_loop_path");
        addSvrfOption(info, ANCHOR_CONFLICT, "tag_conflict");
        addSvrfOption(info, ANCHOR_PATH, "tag_path");
        addSvrfOption(info, ANCHOR_SELF_CONFLICT, "tag_self_conflict");
        addSvrfOption(info, ANCHOR_TREE, "tag_tree");
        //addSvrfOption(info, CONFLICT, "conflict");
        addSvrfOption(info, LOOP, "loop");
        addSvrfOption(info, RING, "ring");
        addSvrfOption(info, SELF_CONFLICT, "self_conflict");
        addSvrfOption(info, WARNING_SECOND, "warning");
    }

    // dfm mp
    {
        rcs_SvrfRuleInfo &info = addSvrfKey(DFM_MP, "dfm_color");
        addSvrfOption(info, SAME, "same");
        addSvrfOption(info, OPPOSITE, "opposite");
        addSvrfOption(info, ANCHOR, "tag");
        addSvrfOption(info, MASK1, "mask1");
        addSvrfOption(info, MASK2, "mask2");
        addSvrfOption(info, MASK3, "mask3");
        addSvrfOption(info, MASK4, "mask4");
        addSvrfOption(info, STITCH, "patch");
        addSvrfOption(info, REQUIRED, "required");
        addSvrfOption(info, USER, "user");
        addSvrfOption(info, MINIMIZE, "minimize");
        addSvrfOption(info, MAXIMIZE, "maximize");

        addSvrfOption(info, ANCHOR_SELF_CONFLICT_POLYGONS, "tag_self_conflict_polygon");
        addSvrfOption(info, CONFLICT, "conflict");
        addSvrfOption(info, DIRECT_ANCHOR_CONFLICT_SEPARATORS, "direct_tag_conflict_spacer");
        addSvrfOption(info, EQUIV_LOOP_COMBINED, "eqv_cycle_group");
        addSvrfOption(info, EQUIV_LOOP_POLYGONS, "eqv_cycle_polygon");
        addSvrfOption(info, EQUIV_LOOP_EQUIV_POLYGONS, "eqv_cycle_eqv_polygon");
        addSvrfOption(info, EQUIV_LOOP_SEPARATORS, "eqv_cycle_spacer");
        addSvrfOption(info, EQUIV_LOOP_VIOLATION_SEPARATORS, "eqv_cycle_violation_spacer");
        addSvrfOption(info, INVALID_ANCHOR0, "inactive_tag0");
        addSvrfOption(info, INVALID_ANCHOR1, "inactive_tag1");
        addSvrfOption(info, INVALID_ANCHOR2, "inactive_tag2");
        addSvrfOption(info, INVALID_ANCHOR3, "inactive_tag3");
        addSvrfOption(info, INVALID_ANCHOR4, "inactive_tag4");
        addSvrfOption(info, INVALID_SEPARATORS, "inactive_spacer");
        addSvrfOption(info, INVALID_STITCHES, "inactive_patch");
        addSvrfOption(info, OPPOSITE_ANCHOR_EQUIV_POLYGONS, "opposite_tag_eqv_polygon");
        addSvrfOption(info, OPPOSITE_ANCHOR_COMBINED, "opposite_tag_group");
        addSvrfOption(info, OPPOSITE_ANCHOR_POLYGONS, "opposite_tag_polygon");
        addSvrfOption(info, OPPOSITE_ANCHOR_SEPARATORS, "opposite_tag_spacer");
        addSvrfOption(info, REDUCED_SEPARATORS, "merge_spacer");
        addSvrfOption(info, REDUCED_TARGET, "merge_polygon");
        addSvrfOption(info, REDUCED_TARGET_POLYGONS, "merge_polygon");
        addSvrfOption(info, REDUCED_TARGET_SEPARATORS, "merge_spacer");
        addSvrfOption(info, REMAINING_VIOLATIONS_COMBINED , "lasting_violation_group");
        addSvrfOption(info, REMAINING_VIOLATIONS_POLYGONS, "lasting_violation_polygon");
        addSvrfOption(info, REMAINING_VIOLATIONS_SEPARATORS, "lasting_violation_spacer");
        addSvrfOption(info, SAME_ANCHOR_COMBINED, "same_tag_group");
        addSvrfOption(info, SAME_ANCHOR_EQUIV_POLYGONS, "same_tag_eqv_polygon");
        addSvrfOption(info, SAME_ANCHOR_POLYGONS, "same_tag_polygon");
        addSvrfOption(info, SAME_ANCHOR_SEPARATORS, "same_tag_spacer");
        addSvrfOption(info, SAME_ANCHOR_VIOLATION_SEPARATORS, "same_tag_violation_spacer");
        addSvrfOption(info, SELF_CONFLICT_SEPARATORS, "self_conflict_spacer");
        addSvrfOption(info, SELF_CONTAINED_VIOLATION_POLYGONS, "self_rejected_violation_polygon");
        addSvrfOption(info, SELF_CONTAINED_VIOLATION_SEPARATORS, "self_rejected_violation_spacer");
        addSvrfOption(info, SELF_CONTAINED_VIOLATION_COMBINED, "self_rejected_violation_group");
        addSvrfOption(info, VIOLATED_OPTIONAL_SEPARATORS, "violated_lr_spacer");
        addSvrfOption(info, VIOLATED_OPTIONAL_STITCHES, "violated_lr_patch");
        addSvrfOption(info, VIOLATED_SEPARATORS, "violated_ht_spacer");
        addSvrfOption(info, VIOLATED_STITCHES, "violated_ht_patch");
    }

    // lvs pathchk maximum results
    {
        rcs_SvrfRuleInfo &info = addSvrfKey(LVS_PATHCHK_MAXIMUM_RESULTS, "path_check_max_results");
    }

    // dfm read
    {
        rcs_SvrfRuleInfo &info = addSvrfKey(DFM_READ, "dfm_read");
    }

    // dfm property singleton
    {
        rcs_SvrfRuleInfo &info = addSvrfKey(DFM_PROPERTY_SINGLETON, "dfm_build_property_singleton");
        addSvrfOption(info, CORNER, "corner");
        addSvrfOption(info, INSIDE_OF, "inside_of");
        addSvrfOption(info, GLOBALXY, "globalxy");
        //addSvrfOption(info, DBU, "dbu");
    }

    // dfm expand edge
    {
        rcs_SvrfRuleInfo &info = addSvrfKey(DFM_EXPAND_EDGE, "dfm_edge_to_rect");
        addSvrfOption(info, OUTSIDE_BY, "outside_by");
        addSvrfOption(info, INSIDE_BY, "inside_by");
        addSvrfOption(info, EXTEND_BY, "extend_by");
        addSvrfOption(info, OFFSET, "offset");
        addSvrfOption(info, CORNER_FILL, "fill_corner");
        addSvrfOption(info, UNMERGED, "keep_unmerged");
    }

    //add erctcl check
    {
        rcs_SvrfRuleInfo &info = addSvrfKey(ERC_TVF, "erc_trs");
    }
}

rcsSynPvrsConvertor::rcs_SvrfRuleInfo &rcsSynPvrsConvertor::addSvrfKey(KEY_TYPE t, const char *pvrs)
{
    Utassert(m_simplePVRSmap.find(t) == m_simplePVRSmap.end());
    rcsSynPvrsConvertor::rcs_SvrfRuleInfo &ret = m_simplePVRSmap[t];
    ret.m_key = t;
    ret.m_pvrsKey = pvrs;
    return ret;
}

void rcsSynPvrsConvertor::addSvrfOption(rcsSynPvrsConvertor::rcs_SvrfRuleInfo &info, KEY_TYPE t, const char *pvrs)
{
    Utassert(info.m_optMap.find(t) == info.m_optMap.end());
    if (info.m_optMap.find(t) != info.m_optMap.end())
    {
        std::cerr << "duplcate pvrs map key: " << pvrs <<std::endl;
    }
    info.m_optMap[t] = pvrs;
}

bool rcsSynPvrsConvertor::convCommonNode(KEY_TYPE type, rcsSynNode_T *pNode)
{
    const rcs_KeyMap &KM = m_simplePVRSmap;
    rcs_KeyMap::const_iterator kIt = KM.find(type);

    if (kIt == KM.end())
    {
        s_errManager.addError(rcErrorNode_T(rcErrorNode_T::WARNING, CON1,
                              pNode->getNodeBeginLineNo(), pNode->begin()->sValue));
        return false;
    }

    const rcs_SvrfRuleInfo &SvrfInfo = kIt->second;
    const rcs_OptionMap &optMap = SvrfInfo.m_optMap;

    (*m_oPVRSStream) <<  SvrfInfo.m_pvrsKey << " ( ";

    std::list<rcsToken_T>::iterator it = pNode->begin();
    int nOption = 0;
    while (++it != pNode->end())
    {
        rcs_OptionMap::const_iterator opIt = optMap.find(it->eKeyType);
        if (opIt != optMap.end())
        {
            const std::string &opStr = opIt->second;
            outputText(opStr.c_str(), NULL, nOption++);
        }
        else
        {
            if (it->eType == IDENTIFIER_NAME)
            {
                if(parseTmpLayerForOperation(it->sValue))
                {
                    continue;
                }
            }

            outputOriginString(it);
        }
    }

    (*m_oPVRSStream) << ") ";
    if( (!m_isInLayerDefinition) && (m_nParsingTmpLayers == 0) )
        (*m_oPVRSStream) << std::endl;

    return true;
}

static bool
isLithoFileKeyWord(std::list<rcsToken_T>::iterator &iter)
{
    switch(iter->eKeyType)
    {
        case ACTION:
        case ANCHOR:
        case COLLECTOR:
        case LAYERMODE:
        case LITHO_RESOLVE:
        case SEED:
        case SETLAYER:
        case TILEMICRONS:
        case DPFACECLASS:
        case DPFACESET:
        case DPMINSTITCHWIDTH:
        case DPSPACING:
        case DPSTITCHCANDIDATES:
        case DPSTITCHSIZE:
        case MINCONVEXCORNERSPACING:
        case INTERSECTIONSTITCH:
        case MINCONCAVECORNEROVERLAP:
        case MININTERSECTIONARMLENGTH:
        case DPSTITCHTOVIASPACING:
        case MINCONCAVECORNERSPACING:
        case VERSION:
            return true;
        default:
            return false;
    }
}

static rcsLithoFileAction
_parseActionOption(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end)
{
    std::list<rcsToken_T>::iterator begin = iter;
    rcsLithoFileAction tmpAction;
    ++iter;
    switch(iter->eType)
    {
        case IDENTIFIER_NAME:
        {
            if(!isValidSvrfName(iter->sValue))
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
        }
        case STRING_CONSTANTS:
        {
            tmpAction.m_actionLayer = iter->sValue;
            break;
        }
        default:
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
            break;
    }

    bool bFirstNum = true;
    while(iter != end && !isLithoFileKeyWord(iter))
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                switch(iter->eKeyType)
                {
                    case LITHO_STITCH_MODE:
                        tmpAction.m_hasStitchMode = true;
                        break;
                    case OPPOSITE:
                        tmpAction.m_bSame = false;
                        break;
                    case SAME:
                        tmpAction.m_bSame = true;
                        break;
                    default:
                        break;
                }
                break;
            }
            case NUMBER:
            {
                if(bFirstNum)
                {
                    std::string sNicety;
                    rcsExpressionParser_T::parseNumber(iter, end, sNicety);
                    hvUInt32 iNicety  = ::atoi(sNicety.c_str());
                    tmpAction.m_iPriority = iNicety;
                    bFirstNum = false;
                    continue;
                }
                else
                {
                    std::string sSame;
                    rcsExpressionParser_T::parseNumber(iter, end, sSame);
                    hvUInt32 iSame  = ::atoi(sSame.c_str());
                    tmpAction.m_bSame = iSame == 0 ? true : false;
                    continue;
                }
                break;
            }
            default:
                break;
        }
        iter++;
    }
    return tmpAction;
}

static rcsLithoFileAnchor
_parseAnchorOption(std::list<rcsToken_T>::iterator &iter, std::list<rcsToken_T>::iterator end)
{
    std::list<rcsToken_T>::iterator begin = iter;
    rcsLithoFileAnchor tmpAnchor;
    ++iter;
    switch(iter->eType)
    {
        case IDENTIFIER_NAME:
        {
            if(!isValidSvrfName(iter->sValue))
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
        }
        case STRING_CONSTANTS:
        {
            tmpAnchor.m_anchorLayer = iter->sValue;
            break;
        }
        default:
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
            break;
    }

    bool bFirstNum = true;
    while(iter != end && !isLithoFileKeyWord(iter))
    {
        switch(iter->eType)
        {
            case SECONDARY_KEYWORD:
            {
                switch(iter->eKeyType)
                {
                    case MASK0:
                        tmpAnchor.m_isMask0 = true;
                        break;
                    case MASK1:
                        tmpAnchor.m_isMask0 = false;
                        break;
                    default:
                        break;
                }
                break;
            }
            case NUMBER:
            {
                if(bFirstNum)
                {
                    std::string sNicety;
                    rcsExpressionParser_T::parseNumber(iter, end, sNicety);
                    hvUInt32 iNicety  = ::atoi(sNicety.c_str());
                    tmpAnchor.m_iPriority = iNicety;
                    bFirstNum = false;
                }
                else
                {
                    std::string sMaskId;
                    rcsExpressionParser_T::parseNumber(iter, end, sMaskId);
                    hvUInt32 iMaskId  = ::atoi(sMaskId.c_str());
                    tmpAnchor.m_isMask0 = iMaskId == 0 ? true : false;
                }
                break;
            }
            default:
                break;
        }
        iter++;
    }
    return tmpAnchor;
}

std::vector<string>
splitLithoLine(const string &lithoStr)
{
    std::vector<string> vec(1, "");

    for (size_t i = 0; i < lithoStr.size(); ++i)
    {
        char c = lithoStr[i];
        if ( c != ';' )
            vec.back().push_back(c);
        else
            vec.resize(vec.size() + 1);
    }

    return vec;
}

static void
parseLithoFileLang(std::list<rcsToken_T> &tokens, const string &s)
{
    std::set<hvUInt32> _vUnstoredCommentLineNumbers;
    rcsLexParser_T lexer(tokens, _vUnstoredCommentLineNumbers);
    lexer.parse(NULL, s.c_str());
}

static void
parseNewLithoLine(rcsLithoFileSpec &newLithoFile, const string &lithoLine, hvUInt32 &nOldLineNo)
{
    std::list<rcsToken_T> lithoTokens;
    parseLithoFileLang(lithoTokens, lithoLine);

    std::list<rcsToken_T>::iterator iter = lithoTokens.begin();
    std::list<rcsToken_T>::iterator lithoEnd = lithoTokens.end();

    if (iter == lithoEnd)
        return;

    hvUInt32 nLineDiff = nOldLineNo - 1;
    while(iter != lithoEnd)
    {
        iter->nLineNo += nLineDiff;
        nOldLineNo = iter->nLineNo;
        ++iter;
    }

    iter = lithoTokens.begin();
    int nAnchorIndex = 0;
    if(iter->eType == SECONDARY_KEYWORD)
    {
        switch(iter->eKeyType)
        {
            case LITHO_RESOLVE:
                newLithoFile.m_isResolve = true;
                break;
            case LITHO_VERBOSE:
                newLithoFile.m_isVerbose = true;
                break;
            case LITHO_OUTPUT_ALL_CONFLICTS:
                newLithoFile.m_isOutputAllConflict = true;
                break;
            case ACTION:
            {
                rcsLithoFileAction tmpAction = _parseActionOption(iter, lithoEnd);
                newLithoFile.m_stAction.push_back(tmpAction);
                break;
            }
            case ANCHOR:
            {
                rcsLithoFileAnchor tmpAnchor = _parseAnchorOption(iter, lithoEnd);
                newLithoFile.m_stAnchor.push_back(tmpAnchor);
                break;
            }
            case ANCHOR1:
                nAnchorIndex = 0;
                ++iter;
                newLithoFile.m_anchorForMP[nAnchorIndex] = iter->sValue;
                break;
            case ANCHOR2:
                nAnchorIndex = 1;
                ++iter;
                newLithoFile.m_anchorForMP[nAnchorIndex] = iter->sValue;
                break;
            case ANCHOR3:
                nAnchorIndex = 2;
                ++iter;
                newLithoFile.m_anchorForMP[nAnchorIndex] = iter->sValue;
                break;
            case ANCHOR4:
                nAnchorIndex = 3;
                ++iter;
                newLithoFile.m_anchorForMP[nAnchorIndex] = iter->sValue;
                break;
            case TARGET:
            {
                ++iter;
                newLithoFile.m_stTargetLayers.push_back(iter->sValue);
                break;
            }
            case DFMSEPARATOR:
            {
                ++iter;
                newLithoFile.m_stSeparatorLayers.push_back(iter->sValue);
                break;
            }
            default:
                break;
        }
    }
}

static bool
isLithoCommentLine(const string &line)
{
    for (size_t i=0; i<line.size(); ++i)
    {
        const char &c =line[i];
        if (std::isspace(c))
            continue;

        if (c == '#')
            return true;
        else
            return false;
    }

    return true;
}

static void
parseNewLithoFile(rcsLithoFileSpec &newLithoFile, const string &lithoStr, hvUInt32 nOldLineNo)
{
    std::string __lithoStr = lithoStr;
    std::size_t _nPos = __lithoStr.find(";", 0);
    while( (_nPos != std::string::npos) &&
           (_nPos < __lithoStr.size() - 1) )
    {
        std::size_t _nBackPos = _nPos;
        if('\"'==__lithoStr[_nPos + 1])
        {
            while(_nBackPos >= 0)
            {
                if(__lithoStr[_nBackPos] != '\"')
                    _nBackPos--;
                else
                    break;
            }
            if(_nBackPos >= 0)
            {
                __lithoStr.erase(_nPos + 1, 1);
                __lithoStr.erase(_nBackPos, 1);
            }
        }
        _nPos = __lithoStr.find(";", _nPos);
    }
    vector<string> vec = splitLithoLine(__lithoStr);
    for (size_t i=0; i<vec.size(); ++i)
    {
        if (isLithoCommentLine(vec[i]))
            continue;
        parseNewLithoLine(newLithoFile, vec[i], nOldLineNo);
    }
}

void rcsSynPvrsConvertor::parseLithoFileSpec(rcsSynSpecificationNode_T *pNode)
{
    std::map<std::string, rcsLithoFileSpec>& val = rcsManager_T::getInstance()->getLithoFileSpec();
    std::list<rcsToken_T>::iterator iter  = pNode->begin();
    ++iter;

    rcsLithoFileSpec newFill;
    rcsLithoFileSpec newLithoFile;
    switch(iter->eType)
    {
        case IDENTIFIER_NAME:
        {
            if(!isValidSvrfName(iter->sValue))
            {
                throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                    iter->nLineNo, iter->sValue);
            }
        }
        case STRING_CONSTANTS:
        {
            newLithoFile.m_sLithoFileName = iter->sValue;
            break;
        }
        default:
            throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                                iter->nLineNo, iter->sValue);
    }
    ++iter;
    if(iter == pNode->end() || iter->eType != BUILT_IN_LANG)
    {

        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1,
                             iter->nLineNo, iter->sValue);
    }
    std::list<rcsToken_T>::iterator testNext = iter;
    ++testNext;
    if (testNext != pNode->end())
    {
        throw rcErrorNode_T(rcErrorNode_T::ERROR, INP1, pNode->begin()->nLineNo, pNode->begin()->sValue);
    }
    hvUInt32 nOldLineNo = iter->nLineNo;
    string lithoStr = iter->sValue;
    lithoStr.erase(lithoStr.size()-1, 1);
    lithoStr.erase(0, 1);
    parseNewLithoFile(newLithoFile, lithoStr, nOldLineNo);
    val.insert(std::make_pair(newLithoFile.m_sLithoFileName, newLithoFile));

}

bool rcsSynPvrsConvertor::parseTmpLayerForOperation(std::string& tmpLayer)
{
    std::map<std::string, rcsSynLayerDefinitionNode_T*>& _tmpLayers = rcsManager_T::getInstance()->getTmpLayers();
    std::map<std::string, rcsSynLayerDefinitionNode_T*>::iterator _iter = _tmpLayers.find(tmpLayer);
    if( _iter == _tmpLayers.end() )
    {
        return false;
    }
    if( NULL == _iter->second)
    {
        return false;
    }
    m_nParsingTmpLayers++;
    rcsSynLayerOperationNode_T * _op = _iter->second->getLayerOperationNode();
    if(_op != NULL)
    {
        if(m_debugCommandSet.find(m_sCurCommandString) == m_debugCommandSet.end())
        {
            m_debugCommandSet.insert(m_sCurCommandString);
        }
        if(m_bTvfConvertor)
        {
            (*m_oPVRSStream) << " ( ";
        }
        this->beginVisitLayerOperationNode(_iter->second->getLayerOperationNode());

        if(m_bTvfConvertor)
        {
            (*m_oPVRSStream) << " ) ";
        }
    }
    m_nParsingTmpLayers--;
    return true;
}

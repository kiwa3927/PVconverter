#include "rcscommentutil.h"
#include <algorithm>
#include <stack>
#include <sstream>
#include "lexical/FlexLexer.h"

template <class Iter, class Cmp>
Iter rfind_if(Iter beg, Iter end, Cmp cmp)
{
    Iter it = end;
    while (it != beg)
    {
        --it;
        if (cmp(*it)) return it;
    }
    return end;
}

bool rcsCommentUtil::has_braces_match(const std::string &str)
{
    const char left = '{';
    const char right = '}';

    int braces = 0;

    std::stack<bool> stack;
    for (size_t i=0; i<str.size(); ++i)
    {
        const char &c = str[i];
        if (c == left)
        {
            ++braces;
            stack.push(true);
        }
        else if (c == right)
        {
            if (stack.empty())
                return false;

            stack.pop();
        }
    }

    return braces!=0 && stack.empty();
}


int rcsCommentUtil::count_line(const std::string &str)
{
    return std::count(str.begin(), str.end(), '\n');
}


bool rcsCommentUtil::maybe_rulecheck(const std::string &svrf)
{
    if (count_line(svrf) < 3)
        return false;

    if (!has_braces_match(svrf))
        return false;

    return true;
}



std::string rcsCommentUtil::replaceKeyword(const std::vector<rcsToken_T> &tokens)
{
    std::ostringstream writer;
    for (std::vector<rcsToken_T>::const_iterator iter = tokens.begin();
         iter != tokens.end(); ++iter)
    {
        switch (iter->eKeyType)
        {
        case COMMENT:
        {
            if (maybe_rulecheck_comment(tokens, iter))
            {
                std::string str = iter->sValue;
                str[0] = ';';
                writer << str;
            }
            else
                writer << iter->sValue;
        } break;
        
        case CMACRO: writer << "call_fun"; break;
        case DMACRO: writer << "def_fun"; break;

            
            
        case AND:   { writer << "GEOM_AND"; }    break;
        case ANGLE:   { writer << "EDGE_ANGLE"; }    break;
        case AREA:   { writer << "GEOM_AREA"; }    break;
        case ATTACH:   { writer << "ATTACH"; }    break;
        case COINCIDENT_EDGE: writer << "edge_coincident both_side"; break;
        case COINCIDENT_INSIDE_EDGE: writer << "edge_coincident inside"; break;
        case COINCIDENT_OUTSIDE_EDGE: writer << "edge_coincident outside"; break;
        case CONNECT:   { writer << "CONNECT"; }    break;
        case CONVEX_EDGE: writer << "edge_convex_point"; break;
        case COPY:   { writer << "COPY"; }    break;
        case CUT:   { writer << "GEOM_CUT"; }    break;
        case DEANGLE: writer << "geom_orthogonalize_angle"; break;
        case DENSITY:   { writer << "CHECK_DENSITY"; }    break;
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
        case ENCLOSE:   { writer << "geom_enclose"; }    break;
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
        case EXTENT:   { writer << "geom_get_boundary"; }    break;
        case EXTENTS:   { writer << "geom_get_boundaries"; }    break;
        case EXTENT_CELL: writer << "geom_get_cell_boundary"; break;
        case EXTENT_DRAWN: writer << "geom_get_layout_boundary"; break;
        case EXTERNAL:   { writer << "ext"; }    break;
        case FLAG_NONSIMPLE: writer << "print_log self_intersecting_polygon"; break;
        case FLAG_NONSIMPLE_PATH: writer << "print_log self_intersecting_path "; break;
        case FLATTEN:   { writer << "geom_flatten"; }    break;
        case FLATTEN_CELL: writer << "flatten cell"; break;
        case FLATTEN_INSIDE_CELL: writer << "flatten in_cell"; break;
        case FLATTEN_VERY_SMALL_CELLS: writer << "flatten very_small_cell"; break;
        case GROUP:   { writer << "GROUP_RULE"; }    break;
        case GROW:   { writer << "GEOM_ORTH_SIZE"; }    break;
        case HCELL:   { writer << "hcell"; }    break;
        case HOLES:   { writer << "geom_holes"; }    break;
        case INSIDE:   { writer << "GEOM_INSIDE"; }    break;
        case INSIDE_CELL: writer << "geom_inside_cell"; break;
        case INSIDE_EDGE: writer << "edge_inside"; break;
        case INTERACT:   { writer << "geom_interact"; }    break;
        case INTERNAL:   { writer << "int"; }    break;
        case LABEL_ORDER: writer << "attach_order"; break;
        case LAYER:   { writer << "LAYER"; }    break;
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
        case LENGTH:   { writer << "EDGE_LENGTH"; }    break;
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
        case MAGNIFY:   { writer << "geom_magnify"; }    break;
        case MASK_SVDB_DIRECTORY: writer << "lvs_db"; break;
        case MERGE:   { writer << "geom_merge"; }    break;
        case NET:   { writer << "GEOM_NET BY_LAYER"; }    break;
        case NET_AREA: writer << "geom_net by_area"; break;
        case NET_AREA_RATIO: writer << "check_nar"; break;
        case NET_AREA_RATIO_ACCUMULATE: writer << "check_incremental_nar"; break;
        case NET_AREA_RATIO_PRINT: writer << "print_log nar"; break;
        case NOT:   { writer << "NOT"; }    break;
        case NOT_COINCIDENT_EDGE: writer << "not_edge_coincident both_side"; break;
        case NOT_COINCIDENT_INSIDE_EDGE: writer << "not_edge_coincident inside"; break;
        case NOT_COINCIDENT_OUTSIDE_EDGE: writer << "not_edge_coincident outside"; break;
        case NOT_NET: writer << "not_geom_net by_layer"; break;
        case NOT_TOUCH_EDGE: writer << "not_edge_adjacent both_side"; break;
        case NOT_TOUCH_INSIDE_EDGE: writer << "not_edge_adjacent inside"; break;
        case NOT_TOUCH_OUTSIDE_EDGE: writer << "not_edge_adjacent outside"; break;
        case OFFGRID: writer << "check_offgrid"; break;
        case OPCBIAS: writer << "opc_bias"; break;
        case OR:   { writer << "GEOM_OR"; }    break;
        case ORNET: writer << "geom_or_net"; break;
        case OR_EDGE: writer << "edge_or"; break;
        case OUTSIDE:   { writer << "GEOM_OUTSIDE"; }    break;
        case OUTSIDE_EDGE: writer << "edge_outside"; break;
        case PATHCHK: writer << "path_check"; break;
        case PATH_LENGTH: writer << "edge_path_length"; break;
        case PERIMETER: writer << "geom_perimeter"; break;
        case POLYGON:   { writer << "ADD_POLYGON"; }    break;
        case PORT_DEPTH: writer << "label_level port"; break;
        case PORT_LAYER_POLYGON: writer << "label_layer port_polygon"; break;
        case PORT_LAYER_TEXT: writer << "label_layer port"; break;
        case PRECISION:   { writer << "PRECISION"; }    break;
        case PUSH: writer << "geom_push"; break;
        case PUSH_CELL: writer << "push_cell_lowest"; break;
        case RECTANGLE:   { writer << "geom_rectangle"; }    break;
        case RECTANGLES:   { writer << "geom_fill_rectangles"; }    break;
        case RECTANGLE_ENCLOSURE: writer << "enc_rect"; break;
        case RESOLUTION:   { writer << "resolution db"; }    break;
        case ROTATE:   { writer << "geom_rotate"; }    break;
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
        case TEXT:   { writer << "ADD_TEXT"; }    break;
        case TEXT_DEPTH: writer << "label_level text"; break;
        case TEXT_LAYER: writer << "label_layer text"; break;
        case TEXT_PRINT_MAXIMUM: writer << "print_log text_max"; break;
        case TITLE:   { writer << "head_name"; }    break;
        case TOUCH:   { writer << "geom_adjacent"; }    break;
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
        case VARIABLE:   { writer << "VAR"; }    break;
        case VERTEX:   { writer << "GEOM_VERTEX"; }    break;
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
        case SIZE:   { writer << "GEOM_SIZE"; }    break;
        case VIRTUAL_CONNECT_INCREMENTAL: writer << "virtual_connect incremental_connect"; break;

            
        case NETLIST:
        {
            std::vector<rcsToken_T>::const_iterator it = iter;
            ++it;

            while (it != tokens.end() && it->eType == BLANK_SPACE)
            {
                ++it;
            }

            if (it != tokens.end())
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

    return writer.str();
}


std::vector<rcsToken_T> rcsCommentUtil::getTokens(const std::string &svrf, bool *hasBadToken)
{
    std::vector<rcsToken_T> tokens;
    std::istringstream reader(svrf);
    FlexLexer *pLexer = new yyFlexLexer(&reader);
    pLexer->setReturnBadCharacter();
    hvUInt32 eType = pLexer->yylex();
    while (eType)
    {
        if (eType == BAD_CHARACTER)
            *hasBadToken = true;

        if(eType >= PRO_DEFINE && eType <= PRO_INCLUDE)
        {
            tokens.push_back(rcsToken_T(PRO_KEYWORD,
                                            pLexer->lineno(), pLexer->YYText()));
        }
        else if((eType >= AND) && (eType < DEVICE))
        {
            tokens.push_back(rcsToken_T(LAYER_OPERATION_KEYWORD,
                                            pLexer->lineno(), pLexer->YYText()));
        }
        else if((eType >= DEVICE) && (eType <= NON_SUPPORT_SPECIFICATION))
        {
            tokens.push_back(rcsToken_T(SPECIFICATION_KEYWORD,
                                            pLexer->lineno(), pLexer->YYText()));
        }
        else if(eType >= ABUT_ALSO)
        {
            tokens.push_back(rcsToken_T(SECONDARY_KEYWORD,
                                            pLexer->lineno(), pLexer->YYText()));
        }
        else
        {
            if (eType >= INT_NUMBER && eType <= SCIENTIFIC_NUMBER)
            {
                tokens.push_back(rcsToken_T(NUMBER,
                                                pLexer->lineno(), pLexer->YYText()));
            }
            else
            {
                tokens.push_back(rcsToken_T(static_cast<TOKEN_TYPE>(eType),
                                                pLexer->lineno(), pLexer->YYText()));
            }
        }
        tokens.back().eKeyType = static_cast<KEY_TYPE>(eType);

        eType = pLexer->yylex();
    }
    delete pLexer;
#if 0
    for (std::vector<rcsToken_T>::const_iterator it=tokens.begin();
         it!=tokens.end(); ++it)
    {
        const rcsToken_T &token = *it;
        std::cout << token.eType << std::endl;
        std::cout << token.sValue << std::endl;
    }
#endif
    return tokens;
}

bool rcsCommentUtil::maybe_in_buildin(const std::vector<rcsToken_T> &tokens,
                                      std::vector<rcsToken_T>::const_iterator iter)
{
    if (rfind_if(tokens.begin(), iter, token_cmp(SEPARATOR, "[")) != iter
            && std::find_if(iter, tokens.end(), token_cmp(SEPARATOR, "]")) != tokens.end())
    {
        return true;
    }
    return false;
}


bool rcsCommentUtil::maybe_rulecheck_comment(const std::vector<rcsToken_T> &tokens,
                                             std::vector<rcsToken_T>::const_iterator iter)
{
    const std::string &str = iter->sValue;
    Utassert(!str.empty());
    if (str[0] != '@')
        return false;


    std::vector<rcsToken_T>::const_iterator left = rfind_if(tokens.begin(), iter, token_cmp(SEPARATOR, "{"));
    if (left == iter)
        return false;

    std::vector<rcsToken_T>::const_iterator rulecheckName = rfind_if(tokens.begin(), left, token_type_cmp(IDENTIFIER_NAME));
    if (rulecheckName == left || std::distance(rulecheckName, left) > 2)
        return false;


    if (std::find_if(iter, tokens.end(), token_cmp(SEPARATOR, "}")) == tokens.end())
        return false;

    return true;
}


std::vector<std::pair<std::vector<rcsToken_T>, bool> > rcsCommentUtil::split_rule(const std::vector<rcsToken_T> &tokens, bool *hasCheck)
{
    std::vector<std::pair<std::vector<rcsToken_T>, bool> > vec;
    std::vector<rcsToken_T>::const_iterator iter =tokens.begin();
    while (iter != tokens.end())
    {
        if (split_next_rule(iter, tokens.end(), vec))
            *hasCheck = true;
    }
    return vec;
}


bool rcsCommentUtil::split_next_rule(std::vector<rcsToken_T>::const_iterator &iter,
                                     const std::vector<rcsToken_T>::const_iterator &end,
                                     std::vector<std::pair<std::vector<rcsToken_T>, bool> > &vec)
{
    if (iter == end)
        return false;

    std::vector<rcsToken_T>::const_iterator left = std::find_if(iter, end, token_cmp(SEPARATOR, "{"));

    if (left == end)
    {
        vec.push_back(std::make_pair(std::vector<rcsToken_T>(iter, left), false));
        iter = left;
        return false;
    }

    std::vector<rcsToken_T>::const_iterator ruleName = rfind_if(iter, left, token_type_cmp(IDENTIFIER_NAME));

    if (ruleName == left)
    {
        ++left;
        vec.push_back(std::make_pair(std::vector<rcsToken_T>(iter, left), false));
        iter = left;
        return false;
    }

    std::vector<rcsToken_T>::const_iterator right = std::find_if(left, end, token_cmp(SEPARATOR, "}"));
    if (right == end)
    {
        vec.push_back(std::make_pair(std::vector<rcsToken_T>(iter, right), false));
        iter = right;
        return false;
    }

    if (iter != ruleName)
        vec.push_back(std::make_pair(std::vector<rcsToken_T>(iter, ruleName), false));

    ++right;
    vec.push_back(std::make_pair(std::vector<rcsToken_T>(ruleName, right), false));
    iter = right;
    return true;
}


bool rcsCommentUtil::getCommentContent(const std::string &comment, std::string::size_type &start, std::string::size_type &end)
{
    Utassert( comment.size() >=2 );
    start = 0;
    end = comment.size();

    if (comment[0] == '/' && comment[start + 1] == '*')
    {
        Utassert(end >= 4);
        Utassert(comment[end - 2] == '*' && comment[end - 1] == '/');
        start += 2;
        end -= 2;

        while (start != end && comment[start] == '*')
            ++start;

        while (end != start && comment[end - 1] == '*')
            --end;

        return true;
    }
    if (comment[start] == '/' && comment[start + 1] == '/')
    {
        start += 2;
        while (start != end && comment[start] == '/')
            ++start;

        return false;
    }
    // -Werror=return type
    Utassert(false);
    return false;
}

bool rcsCommentUtil::replaceBlockComment(const std::string &comment, std::string &newStr)
{
    if (!maybe_rulecheck(comment))
        return false;

    bool hasBad = false;
    std::vector<rcsToken_T> tokens = getTokens(comment, &hasBad);
    if (hasBad)
        return false;

    bool hasCheck = false;
    std::vector<std::pair<std::vector<rcsToken_T>, bool > > checks = split_rule(tokens, &hasCheck);

    if (!hasCheck)
        return false;

    std::string str;
    typedef std::vector<std::pair<std::vector<rcsToken_T>, bool > >::const_iterator PCIter;
    for (PCIter iter=checks.begin(); iter!=checks.end(); ++iter)
    {
        const std::pair<std::vector<rcsToken_T>, bool > &p = *iter;
        str += replaceKeyword(p.first);
    }
    newStr = str;
    return true;
}

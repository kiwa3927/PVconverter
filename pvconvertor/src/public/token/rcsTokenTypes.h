



#ifndef TOKENTYPES_H_
#define TOKENTYPES_H_

enum TOKEN_TYPE
{
    LAYER_OPERATION_KEYWORD,
    SPECIFICATION_KEYWORD,
    SECONDARY_KEYWORD,
    NUMBER,
    STRING_CONSTANTS,
    IDENTIFIER_NAME,
    OPERATOR,
    SEPARATOR,

    BUILT_IN_LANG,

    CMACRO,
    DMACRO,

    INCOMPLETE_KEYWORD,

    COMMENT,

    COMMENT_TEXT,

    BLANK_SPACE,

    PRO_KEYWORD,

    BAD_CHARACTER,              
};

enum NUMBER_TYPE
{
    INT_NUMBER = 20,
    FLOAT_NUMBER,
    HEX_NUMBER,
    SCIENTIFIC_NUMBER
};

enum KEY_TYPE
{
    
    
    
    INVALID_KEY = 50,

    PRO_DEFINE,
    PRO_REDEFINE,
    PRO_UNDEFINE,
    PRO_REUNDEFINE,
    PRO_IFDEF,
    PRO_IFNDEF,
    PRO_ELSE,
    PRO_ENDIF,
    PRO_PRAGMA_ENV,
    PRO_CRYPT,
    PRO_INCLUDE,

    AND,
    OR,
    XOR,
    NOT,
    COPY,
    EXTENT,
    AREA,
    NOT_AREA,
    NET_AREA,
    CUT,
    DEANGLE,
    DENSITY,
    DENSITY_CONVOLVE,
    DEVICE_LAYER,
    DONUT,
    ENCLOSE,
    ENCLOSE_RECTANGLE,
    EXTENT_CELL,
    EXTENT_DRAWN,
    EXPAND_TEXT,
    EXTENTS,
    HOLES,
    INSIDE,
    INSIDE_CELL,
    INTERACT,
    MAGNIFY,
    MERGE,

    ORNET,
    OUTSIDE,
    PERIMETER,
    PUSH,
    PINS,
    PORTS,
    PATHCHK,
    RECTANGLE,
    RECTANGLE_ENCLOSURE,
    RECTANGLES,
    EXPAND_EDGE,
    ROTATE,
    GROW,
    SHIFT,
    SHRINK,
    SIZE,
    SIZE_XY,
    SNAP,
    STAMP,
    WITH_EDGE,
    WITH_NEIGHBOR,
    WITH_TEXT,
    WITH_WIDTH,
    TOUCH,
    TOPEX,
    VERTEX,
    TVF,

    NET,
    NOT_NET,
    NET_AREA_RATIO,
    NET_AREA_RATIO_ACCUMULATE,
    NET_AREA_RATIO_PRINT,
    NET_INTERACT,
    NOT_CUT,
    NOT_DONUT,
    NOT_ENCLOSE,
    NOT_ENCLOSE_RECTANGLE,
    NOT_INSIDE,
    NOT_INSIDE_CELL,
    NOT_INTERACT,
    NOT_OUTSIDE,
    NOT_RECTANGLE,
    NOT_TOUCH,
    NOT_WITH_EDGE,
    NOT_WITH_NEIGHBOR,
    NOT_WITH_TEXT,
    NOT_WITH_WIDTH,

    ANGLE,
    COINCIDENT_EDGE,
    COINCIDENT_INSIDE_EDGE,
    COINCIDENT_OUTSIDE_EDGE,
    CONVEX_EDGE,
    INSIDE_EDGE,
    LENGTH,
    NOT_ANGLE,
    NOT_COINCIDENT_EDGE,
    NOT_COINCIDENT_INSIDE_EDGE,
    NOT_COINCIDENT_OUTSIDE_EDGE,

    NOT_INSIDE_EDGE,
    NOT_LENGTH,
    NOT_OUTSIDE_EDGE,
    NOT_TOUCH_EDGE,
    NOT_TOUCH_INSIDE_EDGE,
    NOT_TOUCH_OUTSIDE_EDGE,

    OR_EDGE,
    OUTSIDE_EDGE,
    TOUCH_EDGE,
    TOUCH_INSIDE_EDGE,
    TOUCH_OUTSIDE_EDGE,
    PATH_LENGTH,


    
    DRAWN_ACUTE,
    DRAWN_ANGLED,
    DRAWN_OFFGRID,
    DRAWN_SKEW,
    OFFGRID,

    
    ENCLOSURE,
    EXTERNAL,
    FLATTEN,
    INTERNAL,
    TDDRC,

    POLYNET,

    DFM_ANALYZE,
    DFM_BALANCE,
    DFM_CAF,
    DFM_CLASSIFY,
    DFM_COMPRESS,
    DFM_CONNECTIVITY_REDUNDANT,
    DFM_CONNECTIVITY_NOT_REDUNDANT,
    DFM_COPY,
    DFM_CREATE_LAYER,
    DFM_CREATE_LAYER_POLYGONS,
    DFM_CRITICAL_AREA,
    DFM_DV,
    DFM_EXPAND_EDGE,
    DFM_EXPAND_ENCLOSURE,
    DFM_FILL,
    DFM_FILL_REGION,
    DFM_FILL_WRAP,
    DFM_MAT,
    DFM_HISTOGRAM,
    DFM_GCA,
    DFM_GROW,
    DFM_MEASURE,
    DFM_NARAC,
    DFM_OR_EDGE,
    DFM_PROPERTY,
    DFM_PROPERTY_SELECT_SECONDARY,
    DFM_PROPERTY_MERGE,
    DFM_PROPERTY_NET,
    DFM_PROPERTY_SINGLETON,
    DFM_RDB,
#if 0
    DFM_RDB_GDS,
    DFM_RDB_OASIS,
#endif
    DFM_READ,
    DFM_REDUNDANT_VIAS,
    DFM_NON_REDUNDANT_VIAS,
    DFM_SHIFT_EDGE,
    DFM_SIZE,
    DFM_SHIFT,
    DFM_SPACE,
    DFM_STAMP,
    DFM_TEXT,
    DFM_TRANSFORM,
    DFM_TRANSITION,
    DFM_DP,
    DFM_MP,

    LITHO_OPC,
    LITHO_DENSEOPC,
    LITHO_OPCVERIFY,
    LITHO_ORC,
    LITHO_NMBIAS,
    LITHO_PSMGATE,
    LITHO_PRINTIMAGE,

    RET_DPSTITCH,
    RET_NMDPC,
    RET_TP,
    RET_QP,

    PEX_CMD,
    OPCBIAS,
    OPCSBAR,
    NON_SUPPORT_OPERTION,

    ATTACH,
    CONNECT,
    LABEL_ORDER,
    SCONNECT,
    DISCONNECT,

    DEVICE,
    DEVICE_GROUP,

    DRC_CELL_NAME,
    DRC_CELL_TEXT,
    DRC_CHECK_MAP,
    DRC_CHECK_TEXT,
    DRC_BOOLEAN_NOSNAP45,
    DRC_EXCLUDE_FALSE_NOTCH,
    DRC_INCREMENTAL_CONNECT,
    DRC_INCREMENTAL_CONNECT_WARNING,
    DRC_ICSTATION,
    DRC_KEEP_EMPTY,
    DRC_MAGNIFY_RESULTS,
    DRC_MAGNIFY_DENSITY,
    DRC_MAGNIFY_NAR,
    DRC_MAP_TEXT,
    DRC_MAP_TEXT_DEPTH,
    DRC_MAP_TEXT_LAYER,
    DRC_MAXIMUM_CELL_NAME_LENGTH,
    DRC_MAXIMUM_UNATTACHED_LABEL_WARNINGS,
    DRC_MAXIMUM_RESULTS_NAR,
    DRC_MAXIMUM_RESULTS,
    DRC_MAXIMUM_VERTEX,
    DRC_PRINT_AREA,
    DRC_PRINT_PERIMETER,
    DRC_PRINT_EXTENT,
    DRC_RESULTS_DATABASE,
    DRC_RESULTS_DATABASE_LIBNAME,
    DRC_RESULTS_DATABASE_PRECISION,
    DRC_SELECT_CHECK,
    DRC_SELECT_CHECK_BY_LAYER,
    DRC_SUMMARY_REPORT,
    DRC_TOLERANCE_FACTOR,
    DRC_TOLERANCE_FACTOR_NAR,
    DRC_TOLERANCE_ANGLE,
    DRC_UNSELECT_CHECK,
    DRC_UNSELECT_CHECK_BY_LAYER,

    DRC_ERROR_WAIVE_CELL,
    DRC_ERROR_WAIVE_LAYER,

    EXCLUDE_ACUTE,
    EXCLUDE_ANGLED,
    EXCLUDE_OFFGRID,
    EXCLUDE_SKEW,
    EXCLUDE_CELL,
    EXPAND_CELL,
    DRC_IP_CELL,
    EXPAND_CELL_TEXT,

    FLAG_ACUTE,
    FLAG_ANGLED,
    FLAG_NONSIMPLE,
    FLAG_NONSIMPLE_PATH,
    FLAG_OFFGRID,
    FLAG_SKEW,
    FLATTEN_CELL,
    FLATTEN_INSIDE_CELL,
    FLATTEN_VERY_SMALL_CELLS,

    GROUP,

    HCELL,

    LAYER,
    LAYER_DIRECTORY,
    LAYER_MAP,
    LAYER_RESOLUTION,
    LAYOUT_ALLOW_DUPLICATE_CELL,
    LAYOUT_BASE_CELL,
    LAYOUT_BASE_LAYER,
    LAYOUT_BUMP2,
    LAYOUT_CASE,
    LAYOUT_CELL_LIST,
    LAYOUT_CELL_MATCH_RULE,
    LAYOUT_CLONE_BY_LAYER,
    LAYOUT_CLONE_ROTATED_PLACEMENTS,
    LAYOUT_CLONE_TRANSFORMED_PLACEMENTS,
    LAYOUT_DEPTH,
    LAYOUT_ERROR_ON_INPUT,
    LAYOUT_IGNORE_TEXT,
    LAYOUT_INPUT_EXCEPTION_RDB,
    LAYOUT_INPUT_EXCEPTION_SEVERITY,
    LAYOUT_MAGNIFY,
    LAYOUT_MERGE_ON_INPUT,
    LAYOUT_NET_LIST,
    LAYOUT_PATH,
    LAYOUT_ULTRA_FLEX,
    LAYOUT_PATH2,
    LAYOUT_PATH_WIDTH_MULTIPLE,
    LAYOUT_VIEW,
    LAYOUT_VIEW2,
    LAYOUT_PLACE_CELL,
    LAYOUT_POLYGON,
    LAYOUT_PRECISION,
    LAYOUT_PRESERVE_CASE,
    LAYOUT_PRESERVE_NET_CASE,
    LAYOUT_PRESERVE_PROPERTY_CASE,
    LAYOUT_PRESERVE_CELL_LIST,
    LAYOUT_PRIMARY,
    LAYOUT_PRIMARY2,
    LAYOUT_OA_ELLIPSE_SEGMENTS,
    LAYOUT_OA_DONUT_SEGMENTS,
    LAYOUT_OA_LAYER_MAP_FILE,
    LAYOUT_OA_TECH_FILE,
    LAYOUT_PROCESS_BOX_RECORD,
    LAYOUT_PROCESS_NODE_RECORD,
    LAYOUT_PROPERTY_AUDIT,
    LAYOUT_PROPERTY_TEXT,
    LAYOUT_PROPERTY_TEXT_OASIS,
    LAYOUT_RENAME_CELL,
    LAYOUT_RENAME_ICV,
    LAYOUT_RENAME_TEXT,
    LAYOUT_SYSTEM,
    LAYOUT_SYSTEM2,
    LAYOUT_TEXT,
    LAYOUT_TEXT_FILE,
    LAYOUT_TOP_LAYER,
    LAYOUT_USE_DATABASE_PRECISION,
    LAYOUT_WINDEL,
    LAYOUT_WINDEL_CELL,
    LAYOUT_WINDEL_LAYER,
    LAYOUT_WINDOW,
    LAYOUT_WINDOW_CELL,
    LAYOUT_WINDOW_CLIP,
    LAYOUT_WINDOW_LAYER,

    LITHO_FILE,

    LVS_ABORT_ON_ERC_ERROR,
    LVS_ABORT_ON_SOFTCHK,
    LVS_ABORT_ON_SUPPLY_ERROR,
    LVS_ALL_CAPACITOR_PINS_SWAPPABLE,
    LVS_AUTO_EXPAND_HCELL,
    LVS_ANNOTATE_DEVICES,
    LVS_BOX,
    LVS_BOX_CUT_EXTENT,
    LVS_BOX_PEEK_LAYER,
    LVS_BLACK_BOX_PORT,
    LVS_BLACK_BOX_PORT_DEPTH,
    LVS_BUILTIN_DEVICE_PIN_SWAP,
    LVS_CELL_LIST,
    LVS_CELL_SUPPLY,
    LVS_CENTER_DEVICE_LOCATION,
    LVS_CENTER_DEVICE_PINS,
    LVS_CHECK_PORT_NAMES,
    LVS_COMPARE_CASE,
    LVS_COMPONENT_SUBTYPE_PROPERTY,
    LVS_COMPONENT_TYPE_PROPERTY,
    LVS_CONFIGURE_UNUSED_FILTER,
    LVS_APPLY_UNUSED_FILTER,
    LVS_CPOINT,
    LVS_DB_LAYER,
    LVS_DB_CONNECTIVITY_LAYER,
    LVS_DEVICE_TYPE,
    LVS_DISCARD_PINS_BY_DEVICE,
    LVS_DOWNCASE_DEVICE,
    LVS_EXACT_SUBTYPES,
    LVS_EXCLUDE_HCELL,
    LVS_EXECUTE_ERC,
    LVS_EXPAND_SEED_PROMOTIONS,
    LVS_EXPAND_UNBALANCED_CELLS,
    LVS_FILTER,
    LVS_FILTER_UNUSED_BIPOLAR,
    LVS_FILTER_UNUSED_CAPACITORS,
    LVS_FILTER_UNUSED_DIODES,
    LVS_FILTER_UNUSED_MOS,
    LVS_FILTER_UNUSED_OPTION,
    LVS_FILTER_UNUSED_RESISTORS,
    LVS_FLATTEN_INSIDE_CELL,
    LVS_GLOBAL_LAYOUT_NAME,
    LVS_GLOBALS_ARE_PORTS,
    LVS_GRAY_BOX_NOT_PORT,
    LVS_GROUND_NAME,
    LVS_HEAP_DIRECTORY,
    LVS_IGNORE_PORTS,
    LVS_IGNORE_TRIVIAL_NAMED_PORTS,
    LVS_INJECT_LOGIC,
    LVS_ISOLATE_OPENS,
    LVS_ISOLATE_SHORTS,
    LVS_MAXIMUM_ISOLATE_SHORTS,
    LVS_MAXIMUM_ISOLATE_OPENS,
    LVS_MAXIMUM_NETS_PER_ISOLATE_OPEN,
    LVS_IGNORE_DEVICE_MODEL,
    LVS_ISOLATE_OPENS_NETS,
    LVS_ISOLATE_OPENS_IGNORE_NETS,
    LVS_ISOLATE_SHORTS_IGNORE_NETS,

    LVS_MAP_DEVICE,
    LVS_MOS_SWAPPABLE_PROPERTIES,
    LVS_NETLIST_ALL_TEXTED_PINS,
    LVS_NETLIST_ALLOW_INCONSISTENT_MODEL,
    LVS_NETLIST_BOX_CONTENTS,
    LVS_NETLIST_COMMENT_CODED_SUBSTRATE,
    LVS_NETLIST_COMMENT_CODED_PROPERTIES,
    LVS_NETLIST_CONNECT_PORT_NAMES,
    LVS_NETLIST_UNNAMED_BOX_PINS,
    LVS_NETLIST_UNNAMED_ISOLATED_NETS,
    LVS_NETLIST_PROPERTY,
    LVS_NL_PIN_LOCATIONS,
    LVS_NON_USER_NAME_INSTANCE,
    LVS_NON_USER_NAME_NET,
    LVS_NON_USER_NAME_PORT,
    LVS_OUT_OF_RANGE_EXCLUDE_ZERO,
    LVS_PATHCHK_MAXIMUM_RESULTS,
    LVS_PERC_LOAD,
    LVS_PERC_NETLIST,
    LVS_PERC_PROPERTY,
    LVS_PERC_REPORT,
    LVS_PIN_NAME_PROPERTY,
    LVS_POWER_NAME,
    LVS_PRECISE_INTERACTION,
    LVS_PRESERVE_BOX_CELLS,
    LVS_PRESERVE_BOX_PORTS,
    LVS_PRESERVE_FLOATING_TOP_NETS,
    LVS_PRESERVE_PARAMETERIZED_CELLS,
    LVS_PROPERTY_INITIALIZE,
    LVS_PROPERTY_MAP,
    LVS_PROPERTY_RESOLUTION_MAXIMUM,
    LVS_PUSH_DEVICES,
    LVS_PUSH_DEVICES_SEPARATE_PROPERTIES,
    LVS_RECOGNIZE_GATES,
    LVS_RECOGNIZE_GATES_TOLERANCE,
    LVS_REDUCE,
    LVS_REDUCE_PARALLEL_BIPOLAR,
    LVS_REDUCE_PARALLEL_CAPACITORS,
    LVS_REDUCE_PARALLEL_DIODES,
    LVS_REDUCE_PARALLEL_MOS,
    LVS_REDUCE_PARALLEL_RESISTORS,
    LVS_REDUCE_SEMI_SERIES_MOS,
    LVS_REDUCE_SERIES_CAPACITORS,
    LVS_REDUCE_SERIES_MOS,
    LVS_REDUCE_SERIES_RESISTORS,
    LVS_REDUCE_SPLIT_GATES,
    LVS_SHORT_EQUIVALENT_NODES,
    LVS_REDUCTION_PRIORITY,
    LVS_REPORT,
    LVS_REPORT_MAXIMUM,
    LVS_REPORT_OPTION,
    LVS_REPORT_UNITS,
    LVS_REPORT_WARNINGS_HCELL_ONLY,
    LVS_REPORT_WARNINGS_TOP_ONLY,
    LVS_REVERSE_WL,
    LVS_SHOW_SEED_PROMOTIONS,
    LVS_SHOW_SEED_PROMOTIONS_MAXIMUM,
    LVS_SIGNATURE_MAXIMUM,
    LVS_SOFT_SUBSTRATE_PINS,
    LVS_SOFTCHK,
    LVS_SOFTCHK_MAXIMUM_RESULTS,
    LVS_SPICE_ALLOW_DUPLICATE_SUBCIRCUIT_NAMES,
    LVS_SPICE_ALLOW_FLOATING_PINS,
    LVS_SPICE_ALLOW_INLINE_PARAMETERS,
    LVS_SPICE_ALLOW_UNQUOTED_STRINGS,
    LVS_SPICE_CONDITIONAL_LDD,
    LVS_SPICE_CULL_PRIMITIVE_SUBCIRCUITS,
    LVS_SPICE_IMPLIED_MOS_AREA,
    LVS_SPICE_MULTIPLIER_NAME,
    LVS_SPICE_OPTION,
    LVS_SPICE_OPTION_F,
    LVS_SPICE_OPTION_S,
    LVS_SPICE_OPTION_X,
    LVS_SPICE_OVERRIDE_GLOBALS,
    LVS_SPICE_PREFER_PINS,
    LVS_SPICE_REDEFINE_PARAM,
    LVS_SPICE_RENAME_PARAMETER,
    LVS_SPICE_REPLICATE_DEVICES,
    LVS_SPICE_SCALE_X_PARAMETERS,
    LVS_SPICE_SLASH_IS_SPACE,
    LVS_SPICE_STRICT_WL,
    LVS_SPLIT_GATE_RATIO,
    LVS_STRICT_SUBTYPES,
    LVS_WRITE_INJECTED_LAYOUT_NETLIST,
    LVS_WRITE_INJECTED_SOURCE_NETLIST,
    LVS_WRITE_LAYOUT_NETLIST,
    LVS_WRITE_SOURCE_NETLIST,

    MASK_RESULTS_DATABASE,
    MASK_SVDB_DIRECTORY,

    PEX_REDUCE_RONLY,

    POLYGON,
    PORT_DEPTH,
    PORT_LAYER_POLYGON,
    PORT_LAYER_MERGED_POLYGON,
    PORT_LAYER_TEXT,

    PRECISION,
    PUSH_CELL,

    RESOLUTION,
    SNAP_OFFGRID,
    SOURCE_CASE,
    SOURCE_PATH,
    SOURCE_PRIMARY,
    SOURCE_SYSTEM,

    SVRF_ERROR,
    SVRF_MESSAGE,
    SVRF_PROPERTY,
    SVRF_VERSION,

    TEXT,
    TEXT_DEPTH,
    TEXT_LAYER,
    TEXT_PRINT_MAXIMUM,
    TITLE,
    TRACE_PROPERTY,

    UNIT_CAPACITANCE,
    UNIT_LENGTH,
    UNIT_INDUCTANCE,
    UNIT_RESISTANCE,
    UNIT_TIME,

    VARIABLE,
    VIRTUAL_CONNECT_BOX_COLON,
    VIRTUAL_CONNECT_BOX_NAME,
    VIRTUAL_CONNECT_COLON,
    VIRTUAL_CONNECT_DEPTH,
    VIRTUAL_CONNECT_INCREMENTAL,
    VIRTUAL_CONNECT_NAME,
    VIRTUAL_CONNECT_REPORT,
    VIRTUAL_CONNECT_REPORT_MAXIMUM,
    VIRTUAL_CONNECT_SEMICOLON_AS_COLON,

    TVF_FUNCTION,

    ERC_CELL_NAME,
    ERC_CHECK_TEXT,
    ERC_KEEP_EMPTY,
    ERC_MAXIMUM_RESULTS,
    ERC_MAXIMUM_VERTEX,
    ERC_MAXIMUM_WARNINGS,
    ERC_PATH_ALSO,
    ERC_PATHCHK,
    ERC_RESULTS_DATABASE,
    ERC_SELECT_CHECK,
    ERC_SELECT_CHECK_ABORT_LVS,
    ERC_SUMMARY_REPORT,
    ERC_UNSELECT_CHECK,
    ERC_TVF,

    DFM_ASSERT,
    DFM_DATABASE,
    DFM_DATABASE_DIRECTORY,
    DFM_DEFAULTS,
    DFM_DEFAULTS_RDB,
    DFM_DEFAULTS_FILL,
    DFM_FUNCTION,
    DFM_SELECT_CHECK,
    DFM_SELECT_CHECK_NODAL,
    DFM_UNSELECT_CHECK,
    DFM_SPEC_FILL,
    DFM_SPEC_FILL_OPTIMIZER,
    DFM_SPEC_FILL_SHAPE,
    DFM_SPEC_FILL_GUIDE,
    DFM_SPEC_FILL_REGION,
    DFM_SPEC_FILL_WRAP,
    DFM_SPEC_RANDOM_SPOT_FAULT,
    DFM_SPEC_SPATIAL_SAMPLE,
    DFM_SPEC_SPACE,
    DFM_SPEC_VIA_REDUNDANCY,
    DFM_SPEC_BALANCE,
    DFM_SPEC_VIA,
    DFM_SUMMARY_REPORT,
    DFM_YS_AUTOSTART,

    CAPACITANCE_CMD,
    RESISTANCE_CMD,

    NON_SUPPORT_SPECIFICATION,

    
    ABUT_ALSO,
    ALL,
    EXACT,
    CENTERED,
    CORNER_FILL,
    EXTEND_BY,
    EXTEND_PRODUCED_BY,
    EXTEND_BY_FACTOR,
    INSIDE_BY,
    INSIDE_BY_FACTOR,
    INSIDE_OF,
    INSIDE_OF_EXTENT,
    INSIDE_OF_LAYER,
    OUTSIDE_OF_LAYER,
    OUTSIDE_BY,
    OUTSIDE_OF,
    OUTSIDE_BY_FACTOR,
    BY,
    BY_FACTOR,
    BY_NET,
    BY_EXTENT,
    BY_POLYGON,
    BY_RECTANGLE,
    BY_LAYER,
    BY_CELL,
    BY_EXCEPTION,
    BY_SHAPE,
    RIGHT,
    TOP,
    TOP_LEFT,
    TOP_RIGHT,
    LEFT,
    BOTTOM,
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
    SEQUENTIAL,
    NETLIST,
    TEXT_MODEL_LAYER,
    TEXT_PROPERTY_LAYER,
    SIGNATURE,
    TEMPLATE,

    DIRECT,
    MASK,
    MASK_DIRECT,

    WITH,
    WITH_LENGTH,
    WITH_DEFAULT_VOLTAGES,
    DVPARAMS,
    ANGLE1,
    ANGLE2,
    LENGTH1,
    LENGTH2,

    SKEW,
    ORTHOGONAL,
    ONLY,
    FIXED_SECOND,

    UNSATISFIED,

    REPORT,
    PRESET,
    
    COMBINE,
    GRADIENT,
    RELATIVE,
    ABSOLUTE,
    CORNER,
    GLOBALXY,
    MAGNITUDE,
    CENTERS,
    PRINT,
    RDB,
    MAG,
    TRUNCATE,
    BACKUP,
    IGNORE,
    WRAP,
    WRAPS,
    STEP,
    WINDOW,

    XFORM,

    ACTION,
    ANCHOR1,
    ANCHOR2,
    ANCHOR3,
    ANCHOR4,
    COLLECTOR,
    TARGET,
    LAYERMODE,
    LITHO_RESOLVE,
    LITHO_OUTPUT_ALL_CONFLICTS,
    LITHO_VERBOSE,
    LITHO_STITCH_MODE,
    DFMSEPARATOR,
    SEED,
    SETLAYER,
    DPFACECLASS,
    DPFACESET,
    DPMINSTITCHWIDTH,
    DPSPACING,
    DPSTITCHCANDIDATES,
    DPSTITCHSIZE,
    MINCONVEXCORNERSPACING,
    INTERSECTIONSTITCH,
    MINCONCAVECORNEROVERLAP,
    MININTERSECTIONARMLENGTH,
    DPSTITCHTOVIASPACING,
    MINCONCAVECORNERSPACING,
    VERSION,
    MAP,
    
    GDSII,
    OASIS,
    ASCII,
    SPICE,
    PIPE,
    LEFDEF,
    MILKYWAY,
    OPENACCESS,
    CNET,
    BINARY,
    MAXIMUM,
    RESULTS,
    VERTICES,
    TEXTTAG,
    USER,
    MERGED,
    PSEUDO,
    AREF,
    SUBSTITUTE,
    AUTOREF,
    INDEX,
    NOVIEW,
    PROPERTIES,

    COMMENTS,
    RFI,
    NONE,

    PLACE,

    PRIMARY,

    PREFIX,
    CBLOCK,
    STRICT,

    REPLACE,
    APPEND,
    HIER,

    SELECT,

    SQUARE,
    SQUARE_ORTHOGONAL,
    OPPOSITE,
    OPPOSITE1,
    OPPOSITE2,
    OPPOSITE_EXTENDED,
    OPPOSITE_EXTENDED1,
    OPPOSITE_EXTENDED2,
    OPPOSITE_SYMMETRIC,
    OPPOSITE_EXTENDED_SYMMETRIC,
    OPPOSITE_FSYMMETRIC,
    OPPOSITE_EXTENDED_FSYMMETRIC,

    NOTCH,
    SPACE,
    SPACEX,
    SPACEY,
    SPACEXY,
    SPACE_WRAP,

    MEASURE_ALL,
    MEASURE_COINCIDENT,

    CONNECTED,
    NOT_CONNECTED,

    ACUTE_ALSO,
    ACUTE_ONLY,
    NOT_ACUTE,

    PARALLEL_ALSO,
    PARALLEL_ONLY,
    NOT_PARALLEL,

    NOT_PERPENDICULAR,
    PERPENDICULAR_ONLY,
    PERPENDICULAR_ALSO,

    NOT_OBTUSE,
    OBTUSE_ONLY,
    OBTUSE_ALSO,

    PROJECTING,
    NOT_PROJECTING,

    ANGLED,

    CORNER_TO_CORNER,
    CORNER_TO_EDGE,
    NOT_CORNER,

    ABUT,
    OVERLAP,
    SINGULAR,
    INTERSECTING_ONLY,

    INSIDE_ALSO,
    OUTSIDE_ALSO,

    EXCLUDE_FALSE,
    EXCLUDE_FALSE_NOTCH,

    REGION,
    REGION_EXTENTS,
    REGION_CENTERLINE,

    CASE,
    SENSITIVE,

    ORIGINAL,
    COLLAPSE,
    MAPPED,
    OCCUPIED,
    WITH_MATCH,

    SQUARES,

    WARNINGS,

    INNER,
    EMPTY,

    WITH_LAYER,

    EVEN,
    ODD,
    SINGULAR_ALSO,
    SINGULAR_ONLY,

    DATATYPE,
    TEXTTYPE,

    DATABASE,
    RULES,
    TO,

    EXIT,
    NONEMPTY,

    AUTO,

    PREC,
    STDIN,
    BUMP,

    SCALE,
    COUNT,
    OVER,
    ACCUMULATE,

    PERIMETER_ONLY,

    ASPECT,
    MEASURE_EXTENTS,

    EDGE,
    OFFSET,

    ERROR,

    LOCATE,
    LIGHT,
    MEDIUM,

    MAINTAIN,
    SPACING,

    LINK,

    OVERLAP_ONLY,
    OVERUNDER,
    UNDEROVER,
    BEVEL,

    YES,
    NO,
    STRING,
    CELL,
    LIST,

    OCTAGONAL,
    WIDTH,
    WIDTH1,
    WIDTH2,

    POLYGONS,
    FLAT,
    NETS,
    PORTS_ALSO,
    NOFLOAT,
    EXCLUDE_UNUSED,
    PROPERTY,
    EXCLUDE_SUPPLY,
    EXCLUDE_SHIELDED,
    POWERNAME,
    GROUNDNAME,
    LABELNAME,
    BREAKNAME,
    LABELED,
    POWER,
    GROUND,

    ENVIRONMENT,

    GOOD,
    BAD,

    
    SOURCE,
    LAYOUT,
    OPEN,
    SHORT,
    BLACK,
    GRAY,
    ENABLE,
    DISABLE,

    MOS,

    EXCEPTION_BEGIN,
    ARRAY_PITCH_ZERO_EXCEPT,
    AREF_ANGLED_EXCEPT,
    AREF_PLACEMENT_EXCEPT,
    DUPLICATE_CELL_EXCEPT,
    EXCLUDE_CELL_NAME_EXCEPT,
    EXTENT_CELL_EXCEPT,
    INSIDE_CELL_EXCEPT,
    MISSING_REFERENCE_EXCEPT,
    TOP_CELL_CHOSEN_EXCEPT,
    TOP_CELL_EXCLUDED_EXCEPT,
    TOP_CELL_MISSING_EXCEPT,
    FILE_OPEN_EXCEPT,
    FILE_READ_EXCEPT,
    LAYER_DIRECTORY_EXCEPT,
    DATATYPE_MAP_SOURCE_EXCEPT,
    DATATYPE_MAP_TARGET_EXCEPT,
    EMPTY_BASE_LAYER_EXCEPT,
    EMPTY_LAYER_EXCEPT,
    LAYER_OVER_BUMP_EXCEPT,
    UNUSED_DATA_EXCEPT,
    CIRCLE_RADIUS_ZERO_EXCEPT,
    CIRCLE_EXCEPT,
    CTRAPEZOID_AREA_ZERO_EXCEPT,
    CTRAPEZOID_DEGENERATE_EXCEPT,
    TRAPEZOID_AREA_ZERO_EXCEPT,
    TRAPEZOID_DEGENERATE_EXCEPT,
    XGEOMETRY_UNSUPPORTED_EXCEPT,
    PATH_ACUTE_EXCEPT,
    PATH_ANGLED_EXCEPT,
    PATH_BIG_EXCEPT,
    PATH_CIRCULAR_EXCEPT,
    PATH_COINCIDENT_EXCEPT,
    PATH_DEGENERATE_EXCEPT,
    PATH_ENDSEGMENT_SHORT_EXCEPT,
    PATH_EXTENSION_EXCEPT,
    PATH_NEGATIVE_EXCEPT,
    PATH_NONORIENTABLE_EXCEPT,
    PATH_NONSIMPLE_EXCEPT,
    PATH_SPIKE_EXCEPT,
    PATH_VARIABLE_EXCEPT,
    PATH_WIDTH_ABSOLUTE_EXCEPT,
    PATH_WIDTH_MULTIPLE_EXCEPT,
    PATH_WIDTH_ODD_EXCEPT,
    PATH_WIDTH_ZERO_EXCEPT,
    PLACEMENT_ANGLE_ABSOLUTE_EXCEPT,
    PLACEMENT_MAGNIFICATION_ABSOLUTE_EXCEPT,
    PLACEMENT_MAGNIFICATION_NONPOSITIVE_EXCEPT,
    PLACEMENT_MAGNIFIED_EXCEPT,
    LAYOUT_POLYGON_EXCEPT,
    POLYGON_ACUTE_EXCEPT,
    POLYGON_AREA_ZERO_EXCEPT,
    POLYGON_DEGENERATE_EXCEPT,
    POLYGON_IS_RECTANGLE_EXCEPT,
    POLYGON_NONORIENTABLE_EXCEPT,
    POLYGON_NONSIMPLE_EXCEPT,
    POLYGON_NOT_CLOSED_EXCEPT,
    POLYGON_SPIKE_EXCEPT,
    RECTANGLE_SIDE_ZERO_EXCEPT,
    BOX_RECORD_EXCEPT,
    CELLNAME_NSTRING_EXCEPT,
    NODE_RECORD_EXCEPT,
    PROPNAME_NSTRING_EXCEPT,
    TEXTSTRING_ASTRING_EXCEPT,
    XELEMENT_UNSUPPORTED_EXCEPT,
    XNAME_UNSUPPORTED_EXCEPT,
    LAYOUT_TEXT_EXCEPT,
    TEXT_BIG_EXCEPT,
    TEXT_DEGENERATE_EXCEPT,
    METRIC_INPUT_FILE_EXCEPT,
    METRIC_RULE_FILE_EXCEPT,
    PRECISION_INPUT_FILE_EXCEPT,
    PRECISION_LAYOUT_EXCEPT,
    PRECISION_RULE_FILE_EXCEPT,
    EXCEPTION_END,

    BIPOLAR,
    CAPACITOR,
    DIODE,

    HINT,

    ENDPOINT,
    ALSO,

    MULTI,
    NOMULTI,
    NODAL,
    SPLIT,
    NOHIER,
    NOPSEUDO,
    NOPUSH,

    UNMERGED,
    CENTERLINE,
    CENTERLINE_CLUSTER,
    TIE,
    TIE_CENTER,
    TIE_CLUSTER,
    TIE_CENTER_CLUSTER,
    CENTER,
    CLUSTER,
    LAYERID,

    MASK0,
    MASK1,
    MASK2,
    MASK3,
    MASK4,
    STITCH,
    REQUIRED,
    MINIMIZE,
    MAXIMIZE,
    PREFER,
    UNFILTERED,
    WAIVER,

    TARGETAREA,
    MINWIDTH,
    ITERATIONS,

    ROTATION,
    FLIPX,
    FLIPY,

    SINGLE,
    FLIP,

    GRID,
    BY_EXT,
    BY_ENC,
    BY_INT,
    HORIZONTAL,
    VERTICAL,
    NOTHORIZONTAL,
    NOTVERTICAL,

    NOEMPTY,
    NOEMPTYDB,

    SPACE1,
    SPACE2,
    SPACE3,
    ENC1,
    ENC2,
    SIZE3,
    START,
    OUTPUT1,
    OUTPUT2,
    OUTPUT3,

    SIDE,
    LINE,
    END,
    LINEENDMAX,
    SIDEMAX,

    VECTOR,
    VSTRING,
    NUMBER_SECOND,

    ORDERED,

    CONNECTIVITY,

    FILL,
    FILLONLY,
    SPEC,
    OUTPUT,
    HLAYER,
    COLOR,
    VERSION_SECOND,

    OVERWRITE,
    REVISIONS,
    ANNOTATE,
    UNIDIRECTIONAL,
    NONTRIVIAL_ONLY,

    DEVICES,
    PINLOC,
    ANALYZE,
    MEASURE,
    NOCONNECT,

    FILE_SECOND,
    NOFILE,
    OTHER,
    NULL_SECOND,
    ALL_CELLS,
    CHECKNAME,
    JOINED,
    RESULT_CELLS,
    CELL_SPACE,
    TOP_SPACE,

    COMPRESS,
    COMPRESS_PREFIX,
    COMPRESS_VERSION,
    ENVELOPE,

    MIN_SECOND,
    MAX_SECOND,
    BINS,
    AVERAGE,
    COMMENT_SECOND,

    NUMWIN,
    RINDEX,
    CONVERGENCE,
    COVERAGE,

    CSG,
    SAME,
    TRANSITION,
    USERMERGED,
    KEEPEMPTY,
    DEFAULT,

    PITCH,

    AGF,
    LDE,
    LPI,

    HIERARCHICAL,
    QUANTIFY,
    RGID,
    RGFS,
    RGLS,

    DV,
    DX,
    COORD,
    ALLDEPTH,

    SCOPE,
    LAYERS,
    LOCAL,
    GLOBAL,

    EXCLUDE_EDGE,
    POLYGONAL,
    OPENWIRE,
    OPENUWIRE,
    OPENVIA,
    SPATIAL,
    SAMPLE,
    METRIC,
    REGION_OCTAGONAL,

    EXCLUDE_FAULTS_BETWEEN,
    PORTSLAYER,
    GROUPED,
    EXCLUDING,
    WITHOUT_TOPCELL,

    OPTIMIZER,
    MISSEDONLY,
    FILLEDONLY,
    GWINDOW,
    INITIAL_SECOND,
    INITIAL_FILLREGION,
    INITIAL_INSIDE_OF_LAYER,
    INITIAL_POINT,
    POINT,
    FILLREGION,
    PARTITION,
    FILLSHAPE,
    ALLOW,
    ALLOWX,
    ALLOWY,
    ON_SECOND,
    AUTOROTATE,
    FILLMIN,
    FILLMAX,
    REPEAT,
    SHAPESPACE,
    EFFORT,
    EFFORT_PITCH,
    SPACE_EDGE,
    SPACE_INTERIOR,
    ELLIPTICAL,
    EUCLIDEAN,
    RECTFILL,
    POLYFILL,
    STRETCHFILL,
    STRIPE,
    TRIM,
    SLIP,
    MINLENGTH,
    MAXLENGTH,
    GAP,
    GAPX,
    GAPY,
    INTERIOR,
    SETBACK,
    COLOR_SCHEME,
    CHECKER,
    CHECKER3,
    QUAD,
    ALTERNATE,
    ALTERNATEX,
    ALTERNATEY,

    TYPE,
    WORK_ZONE,
    EXCLUSION_ZONES,
    WRAP_PATTERN,
    SAME_COLOR,
    LOCAL_CONSTRAINTS,
    HWIDTH,
    VWIDTH,
    SPACE_EDGE_ALIGN,
    MAJOR,
    C2C,
    GUIDELAYER,
    WRAP_COLOR,
    TOMAX,
    DEBUG_SPACE,
    ORTHO,
    PRIORITY,
    NOOVERLAP,
    DISPLACE,
    HDISPLACE,
    FILLSTACK,
    UP,
    DOWN,
    MAXSTACK,
    MINSTACK,
    VIA,
    FLEXSHAPE,
    MEGACELL,
    MAXCORES,
    UNBOUNDED,
    ADJ,
    NONADJ,
    UR,
    UL,
    LR,
    LL,
    FLEX_SECOND,
    CAP,
    CLOSETO,
    HPERIODIC,
    VPERIODIC,
    PERIOD,
    KEEP,
    ADJOFF,
    RESTRICT,
    FILLCORNER,
    FIELD,
    VALIGNCUT,
    VSHAPE,
    HSHAPE,
    EDGE_ALIGN,
    LONGSHAPE,
    WIDESHAPE,
    UNLIMITED,
    FLEXSTEP,
    SHAPE,
    JOGX,
    JOGY,
    BUMPX,
    BUMPY,
    NOTCHX,
    NOTCHY,
    NARROWTRACKX,
    NARROWTRACKY,
    VPROXIMITY,
    HPROXIMITY,
    PITCHX,
    PITCHY,
    ALIGNBY,
    HALIGNBY,
    ALIGNSLIPR,
    ALIGNSLIPL,
    ALIGNSLIPT,
    ALIGNSLIPB,
    ADJACENT,
    JOGS,
    BUMPS,
    NOTCHES,
    MAXPULLBACK,
    FILL_SPEC,
    ANCHOR,
    ALL_ANCHOR_POLYGONS,
    ALL_ANCHOR_SEPARATORS,
    ALL_LOOP_POLYGONS,
    ALL_LOOP_SEPARATORS,
    ANCHOR_CONFLICT,
    ANCHOR_PATH,
    ANCHOR_SELF_CONFLICT,
    ANCHOR_SELF_CONFLICT_POLYGONS,
    ANCHOR_TREE,
    CONFLICT,
    CONFLICT_MARKER,
    DIRECT_ANCHOR_CONFLICT_SEPARATORS,
    DIRECT_ANCHOR_CONFLICT,
    EQUIV_LOOP_COMBINED,
    EQUIV_LOOP_POLYGONS,
    EQUIV_LOOP_EQUIV_POLYGONS,
    EQUIV_LOOP_SEPARATORS,
    EQUIV_LOOP_VIOLATION_SEPARATORS,
    INVALID_ANCHOR0,
    INVALID_ANCHOR1,
    INVALID_ANCHOR2,
    INVALID_ANCHOR3,
    INVALID_ANCHOR4,
    INVALID_SEPARATORS,
    INVALID_STITCHES,
    LOOP,
    OPPOSITE_ANCHOR_EQUIV_POLYGONS,
    OPPOSITE_ANCHOR_COMBINED,
    OPPOSITE_ANCHOR_POLYGONS,
    OPPOSITE_ANCHOR_SEPARATORS,
    REDUCED_SEPARATORS,
    REDUCED_TARGET,
    REDUCED_TARGET_POLYGONS,
    REDUCED_TARGET_SEPARATORS,
    REMAINING_VIOLATIONS_COMBINED,
    REMAINING_VIOLATIONS_POLYGONS,
    REMAINING_VIOLATIONS_SEPARATORS,
    REDUCED_SEPARATORS_LDR,
    RING,
    RING_TIE,
    SAME_ANCHOR_COMBINED,
    SAME_ANCHOR_EQUIV_POLYGONS,
    SAME_ANCHOR_POLYGONS,
    SAME_ANCHOR_SEPARATORS,
    SAME_ANCHOR_VIOLATION_SEPARATORS,
    SELF_CONTAINED_VIOLATION_POLYGONS,
    SELF_CONTAINED_VIOLATION_SEPARATORS,
    SELF_CONTAINED_VIOLATION_COMBINED,
    SELF_CONFLICT,
    SELF_CONFLICT_POLYGONS,
    SELF_CONFLICT_SEPARATORS,
    SHORTEST_ANCHOR_LINE,
    SHORTEST_ANCHOR_POLYGONS,
    SHORTEST_ANCHOR_SEPARATORS,
    SHORTEST_LOOP_LINE,
    SHORTEST_LOOP_LINE_UNMERGED,
    SHORTEST_LOOP_POLYGONS,
    SHORTEST_LOOP_SEPARATORS,
    VIOLATED_OPTIONAL_SEPARATORS,
    VIOLATED_OPTIONAL_STITCHES,
    VIOLATED_SEPARATORS,
    VIOLATED_STITCHES,
    WARNING_SECOND,
    WARNING_LOOP_LINE,

    CONFLICTS,
    BOTH,

    NAMES,
    TYPES,
    SUBTYPES,
    VALUES,

    MIX,
    SIMPLE,
    XALSO,
    WITHIN,
    TOLERANCE,

    MODEL,
    ELEMENT,

    LOWER,
    CONTACT,
    UPPER,

    QUERY,
    XRC,
    CCI,
    SI,
    IXF,
    NXF,
    PHDB,
    NOPINLOC,
    XDB,
    SLPH,
    NOFLAT,
    GATE,
    OMN,
    XCALIBRE,
    XACT,

    NAME,
    CONTACTS,
    IGNORE_NAME,

    V1,
    V2,
    MINBIASLENGTH,
    CLOSEREDGE,
    CLOSERSYMMETRIC,
    IMPLICITBIAS,
    OPTION,
    ACUTE_ANGLE_ABUT,
    IMPLICIT_METRIC,
    FAST_CLASSIFY,
    SPIKE_CLEANUP,
    CAREFUL_SKEW,
    PROJECTING_ONLY,
    CAREFUL_MEASURE,
    SPACELAYER,
    MOVE,

    SBLAYER,
    CLEANSBLAYER,
    OFFSETLAYER,
    VERIFICATION_MODE,
    TILEMICRONS,
    MINEDGELENGTH,
    MINSBSPACE,
    MINSBOFFSET,
    LINEENDOFFSET,
    LINEENDSPACE,
    LINEENDTOLONGSPACE,
    MINSBWIDTH,
    MAXSBWIDTH,
    MINSBLENGTH,
    MAXSBLENGTH,
    WITHWIDTH,
    NEGATIVE,
    INTERSECTION,
    STRICTCENTER,
    ANGLEEDGE,
    OPPOSITEEXTENDED,
    OPENT,
    MINJOGWIDTH,
    JOG,
    PRIORITIZE_BY_LAYER,
    CENTERMERGE,
    LINEENDMERGE,
    EXTENSION,
    BEFORECLEANUP,
    COLINEARIZE,
    SMOOTH,
    CAREFUL_CLEANUP,
    MINSBSPACE_CLEANUP_TIEBREAKER,
    USE_OLD_NEG_CLEANUP,
    PROCESSING_MODE,
    INCLUSIVE_JOG_LENGTH,
    FAST_MRC,
    PRIORITIZE_SPACE,
    NOCLEANUP,
    PRIORITYCENTER,
    SBWIDTH,
    SBOFFSET,
    SBPITCH,
    CENTERPITCH,
    VIALAYER,

    SEMI,
    ORDER,
    SP,

    PARALLEL,
    SERIES,

    GOLDEN,
    RULE,
    WITH_PROPERTY,
    NOT_WITH_PROPERTY,

    MAX_PER_OPEN,

    TEXTED,
};

enum TVF_COMMAND_TYPE
{
    TVF_COMMENT = 2000,
    TVF_RULE_CHECK_COMMENT,
    TVF_BLOCK_TRANSCRIPT,
    TVF_UNBLOCK_TRANSCRIPT,
    TVF_IS_TRANSCRIPT_BLOCKED,
    TVF_DELETE_LAYER,
    TVF_ECHO_SVRF,
    TVF_ECHO_TO_SVRF_FILE,
    TVF_OUTPUT_TO_SVRF,
    TVF_EPILOG,
    TVF_SET_GLOBAL_VARIABLE,
    TVF_GET_GLOBAL_VARIABLE,
    TVF_UNSET_GLOBAL_VARIABLE,
    TVF_EXISTS_GLOBAL_VARIABLE,
    TVF_GET_CALIBRE_VERSION,
    TVF_GET_TVF_ARG,
    TVF_GET_LAYER_ARGS,
    TVF_GET_LAYER_EXTENT,
    TVF_GET_LAYER_NAME,
    TVF_IS_LAYER_EMPTY,
    TVF_PRINT_CALL_STACK,
    TVF_RULE_CHECK,
    TVF_SET_DISCLAIMER,
    TVF_SET_RULE_CHECK_INDENTATION,
    TVF_SET_TRACEBACK_LINE,
    TVF_SETLAYER,
    TVF_OUTLAYER,
    TVF_SVRF_VAR,
    TVF_SYS_VAR,
    TVF_VERBATIM,

    TVF_SVRF_COMMAND,

    //add erc func by gaor
    ERC_SETUP_PARALLEL_DEVICE_PARAMS,
    ERC_EXECUTE_PARALLEL_DEVICE_PARAMS,
};

#endif 

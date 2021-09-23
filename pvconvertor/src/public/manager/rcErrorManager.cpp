





#include <iostream>
#include <sstream>

#include "public/rcsTypes.h"
#include "public/manager/rcsManager.h"
#include "rcErrorManager.h"


static const
rcErrorMsg_T vSvrfErrorMsg[] =
{
    {"OPEN1", "problem with access, file type, or file open of file: "},

    {"DEF1", "this preprocessor directive requires an operand: "},
    {"DEF2", "too many operands for this preprocessor directive: "},
    {"DEF3", "improper specification of #ELSE preprocessor directive."},
    {"DEF4", "improper specification of #ENDIF preprocessor directive."},
    {"DEF5", "missing #ENDIF preprocessor directive."},

    {"INCL1", "problem with access, file type, or file open of this include file: "},
    {"INCL2", "this file name is included recursively: "},
    {"INCL3", "an include statement requires an operand."},
    {"INCL4", "too many operands for an include statement."},

    {"LOC1", "a RuleCheck comment must be inside of a RuleCheck statement."},
    {"LOC2", "this unassigned layer operation must be inside of a RuleCheck statement: "},
    {"LOC3", "this statement or operation may not be inside of a RuleCheck statement: "},
    {"LOC4", "this statement or operation must be inside of a RuleCheck statement: "},

    {"SYN1", "unrecognized operation."},
    {"SYN2", "unpaired left bracket ( [ ) or right bracket ( ] )."},
    {"SYN3", "missing name, or invalid name, preceding this statement designator: "},
    {"SYN4", "nested, unpaired, or unterminated /* ... */ style comment."},
    {"SYN5", "invalid or incomplete numeric expression near: "},
    {"SYN6", "unpaired left parenthesis or right parenthesis."},
    {"SYN7", "names beginning with \"tmp<\" are reserved."},
    {"SYN8", "unpaired left brace ( { ) or right brace ( } )."},

    {"MAC1", "missing or invalid DMACRO definition name."},
    {"MAC2", "missing left brace ( { ) for this DMACRO definition: "},
    {"MAC3", "duplicate DMACRO definition name: "},
    {"MAC4", "invalid DMACRO definition argument (name required): "},
    {"MAC5", "duplicate DMACRO definition argument: "},
    {"MAC6", "missing or invalid CMACRO name."},
    {"MAC7", "invalid CMACRO argument (name or numeric constant required): "},
    {"MAC8", "this CMACRO has no corresponding DMACRO definition: "},
    {"MAC9", "this CMACRO has too few arguments: "},
    {"MAC10","recursive call of this DMACRO definition: "},

    {"ENV1", "undefined or empty environment variable: "},
    {"ENV2", "cannot convert the value of this environment variable to a numeric quantity: "},

    {"LAY1", "this layer definition has a circular definition: "},
    {"LAY2", "invalid operation assigned to this layer definition name: "},
    {"LAY3", "no operation is assigned to this layer definition name: "},
    {"LAY4", "this layer definition name is also defined as an original layer: "},
    {"LAY5", "duplicate layer definition name: "},
    {"LAY6", "invalid implicit layer definition near: "},
    {"LAY7", "invalid use of implicit layer definition - must be an operation parameter."},

    {"OLS1", "no layer name, or invalid layer name, specified in an original layer specification statement."},
    {"OLS2", "this original layer specification is empty or references no layers: "},
    {"OLS3", "duplicate original layer specification statement name: "},
    {"OLS4", "invalid, out of range, or unresolvable operand within an original layer specification statement: "},

    {"OUT1", "this RuleCheck statement will never generate output: "},
    {"OUT2", "duplicate RuleCheck statement name: "},

    {"SPC1", "superfluous specification statement: "},
    {"SPC2", "missing parameter for this specification statement: "},
    {"SPC4", "a layer is not allowed in a TEXT specification statement when the text has not been specified."},
    {"SPC5", "this DRC/ERC SELECT CHECK name is not the name of a RuleCheck statement or RuleCheck group: "},
    {"SPC7", "invalid geometric data or missing layer in a POLYGON or LAYOUT POLYGON specification statement."},
    {"SPC9", "invalid geometric data in a LAYOUT WINDOW/WINDEL specification statement."},
    {"SPC10", "superfluous LAYER RESOLUTION specification for this layer: "},
    {"SPC11", "this layer cannot be used as a specification statement parameter: "},
    {"SPC12", "LVS SOFTCHK parameter must be a lower layer in an SCONNECT operation in the same connectivity mode: "},
    {"SPC13", "superfluous LAYOUT RENAME CELL specification statement: "},
    {"SPC14", "invalid LAYOUT RENAME TEXT argument: "},
    {"SPC15", "superfluous LAYOUT INPUT EXCEPTION SEVERITY specification statement: "},
    {"SPC16", "LAYOUT INPUT EXCEPTION SEVERITY severity value invalid for this exception name: "},
    {"SPC17", "PLACE may not be specified in LAYOUT MAGNIFY if any per-database magnification is specified in LAYOUT PATH(2)."},
    {"SPC18", "APPEND or PREFIX may only be specified in DRC RESULTS DATABASE if the DRC results database is GDSII- or OASIS-type."},
    {"SPC19", "CBLOCK or STRICT may only be specified in DRC RESULTS DATABASE if the DRC results database is not OASIS-type."},

    {"VAR1", "no variable name, or invalid variable name, specified in a VARIABLE specification statement."},
    {"VAR2", "no variable value, or invalid variable value, specified for this variable name: "},
    {"VAR3", "duplicate VARIABLE specification statement name: "},

    {"RES1", "undefined variable name parameter: "},
    {"RES2", "undefined layer name parameter: "},
    {"RES3", "undefined original layer name parameter: "},
    {"RES4", "invalid layer number parameter: "},
    {"RES5", "numeric type expected for this variable name parameter: "},
    {"RES8", "string variable may not have multiple values: "},

    
    {"#PRAGMA Error 1", " Unsupported PRAGMA: "},
    {"#PRAGMA Error 2", " Missing define for PRAGMA ENV."},
    {"#PRAGMA Error 3", " Multi defined PRAGMA: ENV "},

    {"INP1", "superfluous or invalid input object: "},
    {"INP2", "missing input layer for this operation: "},
    {"INP3", "this operation requires a BY keyword: "},
    {"INP4", "missing net name, text name, cell name, or file name operand for this operation or keyword: "},
    {"INP5", "identical input layers for this operation: "},
    {"INP6", "superfluous edge-directed output specification."},
    {"INP7", "incomplete keyword specification: "},
    {"INP8", "improperly specified edge-directed output specification."},
    {"INP9", "both polygon- and edge-directed output specified for this dimensional check operation: "},
    {"INP10", "this keyword must be followed by an input layer: "},
    {"INP11", "missing numeric operand for this keyword or operation: "},
    {"INP12", "this keyword must be followed by a constraint: "},
    {"INP13", "missing expansion specification for EXPAND EDGE operation."},
    {"INP14", "this input layer may not be derived from an ORNET operation: "},
    {"INP15", "this keyword must be followed by a name: "},
    {"INP16", "invalid orthogonal rectangle following this keyword: "},
    {"INP17", "missing, incomplete, over-specified, or invalid endpoint specification for CONVEX EDGE operation."},
    {"INP18", "this keyword must be followed by a name or string constant: "},
    {"INP20", "missing growth or shrink specification for GROW or SHRINK operation."},
    {"INP21", "missing GOOD/BAD specification for RECTANGLE ENCLOSURE operation."},
    {"INP22", "missing SPACE secondary keyword for WITH NEIGHBOR operation."},
    {"INP23", "missing SPACE or WIDTH secondary keyword for TDDRC operation."},
    {"INP24", "the input layers of the OR EDGE operation must have the same layer of origin."},
    {"INP28", "unidirectional metric may only be specified if the dimensional check operation has two input layers. "},
    {"INP32", "EDGE may not be specified in OFFGRID if CENTERS is specified and the input layer is polygon-type."},
    {"INP33", "REGION may not be specified in OFFGRID if the input layer is edge-type."},
    {"INP34", "OFFSET keyword for the OFFGRID operation must be followed by INSIDE BY, OUTSIDE BY, or two numeric values."},
    {"INP35", "OFFSET keyword for the OFFGRID operation may not have INSIDE BY or OUTSIDE BY if the input layer is polygon-type."},
    {"INP36", "this keyword must be followed by an input layer of this operation: "},
    {"INP37", "Unexpected end of this operation (maybe missing some Required Arguments): "},
    {"INP38", "net interact must be use two layers: "},

    {"NUM1", "improper numeric value (must be non-negative): "},
    {"NUM2", "improper numeric value (must be non-zero): "},
    {"NUM3", "improper numeric value (must be positive): "},
    {"NUM4", "improper numeric value (must be an integer and >= 0): "},
    {"NUM5", "improper numeric value (must be an integer and >= 1): "},
    {"NUM6", "improper numeric value (must be an integer and >= 4): "},
    {"NUM7", "division by 0 specified in a numeric expression."},
    {"NUM8", "improper numeric value (must be an integer when multiplied by the precision): "},
    {"NUM11", "improper numeric value (must be >= 0 and < 1): "},
    {"NUM12", "improper numeric value (must be an integer): "},
    {"NUM13", "improper numeric value (the MAX number must larger than the MIN number): "},

    {"KEY1", "superfluous secondary keyword: "},

    {"CNS1", "improperly specified constraint."},
    {"CNS2", "interval constraint has a >= b."},
    {"CNS3", "improper constraint range for this operation or keyword: "},
    {"CNS4", "ABUT == 0 or ABUT <= 0 may not be specified in a one-layer dimensional check operation."},
    {"CNS5", "missing constraint for this operation: "},
    {"CNS6", "superfluous constraint specification for this operation: "},
    {"CNS7", "improper constraint range for this operation or keyword (must be \"<\" or \"<=\"): "},
    {"CNS8", "improper constraint range for this operation or keyword (must be \">\" or \">=\"): "},

    {"GRP1", "no group name, or invalid group name, specified in a RuleCheck group specification statement."},
    {"GRP2", "this RuleCheck group specification is empty or references no RuleCheck statements: "},
    {"GRP3", "duplicate RuleCheck group name: "},
    {"GRP4", "invalid operand within RuleCheck group specification statement: "},
    {"GRP5", "RuleCheck group name conflicts with RuleCheck statement name: "},

    {"DCM1", "no RuleCheck name, or invalid RuleCheck name, specified in a DRC CHECK MAP specification statement."},
    {"DCM2", "invalid layer number specified for this DRC CHECK MAP specification statement: "},
    {"DCM3", "invalid data type specified for this DRC CHECK MAP specification statement: "},
    {"DCM4", "this DRC CHECK MAP name is not the name of a RuleCheck statement or RuleCheck group: "},
    {"DCM5", "duplicate DRC CHECK MAP specification statement RuleCheck name: "},
    {"DCM6", "duplicate or conflicting DRC CHECK MAP AREF cell name: "},
    {"DCM7", "inconsistent results database type in this DRC CHECK MAP specification statement: "},
    {"DCM8", "type of unspecified results database must match global type in this DRC CHECK MAP specification statement: "},
    {"DCM9", "missing or invalid parameter for MAXIMUM RESULTS keyword in this DRC CHECK MAP specification statement: "},
    {"DCM10", "inconsistent DRC CHECK MAP MAXIMUM RESULTS specification for this RuleCheck name: "},
    {"DCM11", "missing or invalid parameter for MAXIMUM VERTICES keyword in this DRC CHECK MAP specification statement: "},
    {"DCM12", "inconsistent DRC CHECK MAP MAXIMUM VERTICES specification for this RuleCheck name: "},
    {"DCM13", "invalid geometric data for SUBSTITUTE polygon in DRC CHECK MAP AREF cell name: "},
    {"DCM14", "inconsistent results database PSEUDO/USER specification in this DRC CHECK MAP specification statement: "},
    {"DCM15", "AREF may only be specified in DRC CHECK MAP if the applicable DRC results database is GDSII- or OASIS-type."},
    {"DCM16", "AUTOREF may only be specified in DRC CHECK MAP if the applicable DRC results database is GDSII-type."},
    {"DCM17", "inconsistent results database AUTOREF prefix specification in this DRC CHECK MAP specification statement: "},
    {"DCM18", "APPEND, PREFIX, or TEXTTAG may only be specified in DRC CHECK MAP if the applicable DRC results database is GDSII- or OASIS-type."},
    {"DCM19", "inconsistent results database APPEND/PREFIX specification in this DRC CHECK MAP specification statement: "},

    {"TYP1", "incorrect input layer type (must be an original layer): "},
    {"TYP2", "incorrect input layer type (must be a derived edge layer): "},
    {"TYP3", "incorrect input layer type (must not be a derived edge layer): "},
    {"TYP4", "incorrect input layer type (must not be a derived error layer): "},
    {"TYP5", "incorrect input layer type (must be a derived polygon layer): "},
    {"TYP6", "incorrect input layer type (must be a derived error layer): "},
    {"TYP7", "incorrect input layer type (must not be a derived polygon layer): "},

    {"NAR1", "the ACCUMULATE layer in the NET AREA RATIO operation must be geometrically equivalent to the first denominator layer."},
    {"NAR2", "this layer must be the output of a NET AREA RATIO [...] ACCUMULATE operation: "},
    {"NAR3", "superfluous NET AREA RATIO [ ACCUMULATE ] expression specification."},
    {"NAR4", "this NET AREA RATIO secondary keyword is invalid when an expression ([...]) is specified: "},
    {"NAR5", "syntax error in NET AREA RATIO [ ACCUMULATE ] expression near: "},
    {"NAR6", "layer parameter in NET AREA RATIO [ ACCUMULATE ] expression is not an input layer to the operation: "},
    {"NAR7", "RDB layer parameter in NET AREA RATIO is not an input layer to the operation: "},
    {"NAR8", "duplicate NET AREA RATIO RDB layer parameter: "},
    {"NAR9", "BY LAYER may not be specified in NET AREA RATIO if there are no RDB layer parameters."},
    {"NAR10", "OVER may not be specified in NET AREA RATIO if there is one input layer."},
    {"NAR11", "an expression must be specified in NET AREA RATIO if there is one input layer."},
    {"NAR12", "the layer parameters in the NET AREA RATIO ACCUMULATE operation must be geometrically equivalent."},
    {"NAR13", "an expression must be specified in the NET AREA RATIO ACCUMULATE operation."},
    {"NAR14", "INSIDE OF LAYER may not be specified in NET AREA RATIO with either ACCUMULATE or RDB."},

    {"EXT1", "there may only be one LABEL ORDER operation per connectivity type."},
    {"EXT2", "cannot establish connectivity of this layer in the DIRECT connectivity set: "},
    {"EXT3", "cannot establish connectivity of this layer in the MASK connectivity set: "},
    {"EXT4", "this CONNECT or SCONNECT layer is derived from an operation requiring or generating connectivity: "},
    {"EXT5", "this connectivity extraction layer is within a connectivity extraction layer set: "},
    {"EXT6", "this connectivity extraction layer set overlaps a connectivity extraction layer set: "},
    {"EXT7", "more than 32 input layers for a CONNECT or SCONNECT operation."},
    {"EXT8", "this layer parameter has an illegal foward reference across incremental CONNECT zones: "},
    {"EXT9", "MASK mode SCONNECT not allowed when DRC INCREMENTAL CONNECT YES is specified."},
    {"EXT10", "SCONNECT upper layer must be a CONNECT parameter or a lower layer in an SCONNECT operation: "},
    {"EXT11", "SCONNECT lower layer must not be a CONNECT parameter or a contact layer in an SCONNECT operation: "},
    {"EXT12", "MASK mode LABEL ORDER not allowed when DRC INCREMENTAL CONNECT YES is specified. "},
    {"EXT13", "invalid chain of SCONNECT operations at layer: "},
    {"EXT14", "this layer parameter must be a CONNECT or SCONNECT layer: "},
    {"EXT15", "this operation requires connectivity; no CONNECT operations present: "},
    {"EXT16", "Invalid connectivity in this operation: "},
    {"EXT17", "At least two layers must be specified in the layer connectivity definition: "},

    {"LCL1", "invalid LAYOUT CELL LIST name: "},
    {"LCL2", "duplicate LAYOUT CELL LIST name: "},
    {"LCL3", "unknown LAYOUT CELL LIST name: "},
    {"LCL4", "invalid LAYOUT CELL LIST argument: "},
    {"LCL5", "circular LAYOUT CELL LIST reference: "},
    {"LCL6", "LAYOUT CELL LIST cannot begin with an exclusive pattern:  "},

    {"DVL1", "missing or invalid element name for a DEVICE LAYER operation: "},
    {"DVL2", "improperly specified model name for this DEVICE LAYER operation: "},
    {"DVL3", "no corresponding device definition for DEVICE LAYER operation: "},
    {"DVL4", "multiple corresponding device definitions for DEVICE LAYER PROPERTY operation: "},

    {"CEL1", "missing or invalid cell list name for an LVS CELL LIST specification statement."},
    {"CEL2", "duplicate LVS CELL LIST specification statement: "},
    {"CEL3", "missing cell name for this LVS CELL LIST specification statement: "},
    {"CEL4", "invalid cell name in an LVS CELL LIST specification statement: "},
    {"CEL5", "duplicate cell names in LVS CELL LIST specification statement(s): "},
    {"CEL6", "cell list name not defined in any LVS CELL LIST specification statement: "},

    {"DTP1", "empty pin mapping list found: "},
    {"DTP2", "multiple pin mapping lists found: "},
    {"DTP3", ""},
    {"DTP4", "duplicate pin name in pin mapping list of LVS DEVICE TYPE statement: "},
    {"DTP5", "duplicate device name found in LVS DEVICE TYPE statement: "},
    {"DTP6", "invalid pin mapping list found: "},
    {"DTP7", "invalid pin name for found in LVS DEVICE TYPE statement: "},
    {"DTP8", "DEVICE specified in LVS DEVICE TYPE statement missing required pin: "},
    {"DTP9", "DEVICE used in LVS DEVICE TYPE statement does not exist: "},
    {"FIL1", "missing or invalid element name for an LVS FILTER specification statement."},
    {"FIL2", "improperly specified model name for this LVS FILTER specification statement: "},
    {"FIL3", "improperly specified model name for this LVS FILTER specification statement: "},
    {"FIL4", "improperly specified filter constraint for this LVS FILTER specification statement: "},
    {"FIL5", "both a property spice value and a string constraint are specified in this LVS FILTER specification statement: "},
    {"FIL6", "SHORT or OPEN must be specified in this LVS FILTER specification statement: "},
    {"FIL7", ""},
    {"FIL8", ""},
    {"FIL9", ""},
    {"FIL10", ""},
    {"FIL11", "the SHORT pin designation in this LVS FILTER specification statement is inconsistent withother LVS FILTER statements."},
    {"FIL12", ""},

    {"MDV1", "missing original element name in an LVS MAP DEVICE specification statement."},
    {"MDV2", "invalid original element name in an LVS MAP DEVICE specification statement: "},
    {"MDV3", ""},
    {"MDV4", "invalid original model name in an LVS MAP DEVICE specification statement: "},
    {"MDV5", "improperly specified original model name in an LVS MAP DEVICE specification statement."},
    {"MDV6", "missing new element name in an LVS MAP DEVICE specification statement."},
    {"MDV7", "invalid new element name in an LVS MAP DEVICE specification statement: "},
    {"MDV8", ""},
    {"MDV9", "invalid new model name in an LVS MAP DEVICE specification statement: "},
    {"MDV10", "improperly specified new model name in an LVS MAP DEVICE specification statement."},
    {"MDV11", "identical original and new model names in an LVS MAP DEVICE specification statement."},
    {"MDV12", "duplicate LVS MAP DEVICE specification statement."},
    {"MDV13", "different original and new element names in an LVS MAP DEVICE specification statement."},
    {"LPI1", "missing or invalid type name for an LVS PROPERTY INIT type(subtype) statement: "},
    {"LPI2", "premature end of LVS PROPERTY INIT specification statement here (expected [...] user program section): "},
    {"LPI3", "duplicate LVS PROPERTY INIT type(subtype) statement: "},
    {"LPI4", "incomplete [...] user program section on LVS PROPERTY INIT specification statement: ]."},
    {"LPI5", ""},
    {"LPI6", "this was found where a complete statement or right bracket (]) was expected:"},
    {"LPI7", "improperly specified subtype name for an LVS PROPERTY INIT type(subtype) statement: "},

    {"LPM1", " "},
    {"LPM2", "missing or invalid input property for this LVS PROPERTY MAP specification statement: "},
    {"LPM3", "missing or invalid target property for this LVS PROPERTY MAP specification statement: "},
    {"LPM4", "invalid or improperly specified spice value for this LVS PROPERTY MAP property: "},
    {"LPM5", "LAYOUT or SOURCE must be specified in this LVS PROPERTY MAP specification statement: "},
    {"LPM6", "duplicate LVS PROPERTY MAP specification statement: "},
    {"LPM7", "conflicting LVS PROPERTY MAP specification statement (different input properties for onetarget property): "},
    {"LPM8", "conflicting LVS PROPERTY MAP specification statement (different target properties for one input property): "},
    {"LPM9", ""},
    {"LPM10", ""},
    {"LPM11", ""},
    {"LPM12", ""},

    {"RGT1", "missing or invalid element name for an LVS RECOGNIZE GATES TOLERANCE specification statement."},
    {"RGT2", "improperly specified model name for this LVS RECOGNIZE GATES TOLERANCE specification statement: "},
    {"RGT3", "missing or invalid property for this LVS RECOGNIZE GATES TOLERANCE specification statement: "},
    {"RGT4", "invalid or improperly specified spice value for this LVS RECOGNIZE GATES TOLERANCE property: "},
    {"RGT5", "missing or invalid tolerance for this LVS RECOGNIZE GATES TOLERANCE specification statement: "},
    {"RGT6", "duplicate LVS RECOGNIZE GATES TOLERANCE specification statement: "},
    {"RGT7", ""},
    {"RGT8", ""},
    {"RGT9", ""},
    {"RGT10", ""},

    {"LRD1", ""},
    {"LRD2", ""},
    {"LRD3", ""},
    {"LRD4", ""},
    {"LRD5", "this was found where a numeric scalar expression was expected: "},
    {"LRD6", ""},
    {"LRD7", ""},
    {"LRD8", "missing or invalid type name for an LVS REDUCE type(subtype) statement."},
    {"LRD9", "improperly specified subtype name for an LVS REDUCE type(subtype) statement: "},
    {"LRD10", "in LVS REDUCE type(subtype) statement, found this where PARALLEL or SERIES keyword was expected: "},
    {"LRD11", "missing or invalid SERIES pin names for an LVS REDUCE type(subtype) statement near: "},
    {"LRD12", "optional square bracket property section \"[...]\" is not allowed with the optional NO keyword."},
    {"LRD13", "duplicate LVS REDUCE type(subtype) statement."},
    {"LRD14", ""},
    {"LRD15", ""},
    {"LRD16", "duplicate SERIES pin names for an LVS REDUCE type(subtype) statement: "},
    {"LRD17", "invalid builtin SERIES pin name for an LVS REDUCE type(subtype) statement: "},
    {"LRD18", "LVS REDUCE type(subtype) statement matches a DEVICE statement which does not contain pinname: "},
    {"LRD19", ""},
    {"LRD20", ""},
    {"LRD21", ""},
    {"LRD22", ""},
    {"LRD23", ""},
    {"LRD24", ""},
    {"LRD25", ""},
    {"LRD26", ""},
    {"LRD27", "duplicate TOLERANCE or TOLERANCE STRING property name: "},
    {"LRD28", ""},
    {"LRD29", ""},
    {"LRD30", ""},

    {"SGR1", "missing or invalid element name for an LVS SPLIT GATE RATIO specification statement."},
    {"SGR2", ""},
    {"SGR3", "missing or invalid property for this LVS SPLIT GATE RATIO specification statement: "},
    {"SGR4", "invalid or improperly specified spice value for this LVS SPLIT GATE RATIO property: "},
    {"SGR5", "missing or invalid tolerance for this LVS SPLIT GATE RATIO specification statement: "},
    {"SGR6", "duplicate LVS SPLIT GATE RATIO specification statement: "},

    {"SPI1", "improperly specified subtype name for an LVS SPICE RENAME PARAMETER type(subtype) statement: "},
    {"SPI2", "improperly specified subtype name for an LVS SPICE RENAME PARAMETER type(subtype) statement with more than one type."},
    {"SPI3", "new and old parameter names must both be names, not keywords or numbers: "},
    {"SPI4", "LVS SPICE RENAME PARAMETER rule conflicts with previous LVS SPICE RENAME PARAMETER rule for parameter "},
    {"SPI5", "element name type missing or invalid for the LVS SPICE RENAME PARAMETER statement: "},

    {"TRP1", "missing or invalid element name for a TRACE PROPERTY specification statement."},
    {"TRP2", "improperly specified model name for this TRACE PROPERTY specification statement: "},
    {"TRP3", "missing or invalid source property for this TRACE PROPERTY specification statement: "},
    {"TRP4", "missing or invalid layout property for this TRACE PROPERTY specification statement: "},
    {"TRP5", "invalid or improperly specified spice value for this TRACE PROPERTY property: "},
    {"TRP6", "both a property spice value and STRING are specified in this TRACE PROPERTY specification statement: "},
    {"TRP7", "duplicate MASK mode numeric TRACE PROPERTY specification statement: "},
    {"TRP8", ""},
    {"TRP9", ""},
    {"TRP10", ""},
    {"TRP11", ""},
    {"TRP12", ""},
    {"TRP13", "no corresponding device definition for this MASK mode TRACE PROPERTY specification statement: "},
    {"TRP14", ""},
    {"TRP15", ""},
    {"TRP16", "improperly specified tolerance property name for this TRACE PROPERTY specification statement:"},
    {"TRP17", "this was found where an name declared in the PROPERTY statement (above) was expected: "},
    {"TRP18", "cannot assign directly to a name declared in the PROPERTY statement: "},
    {"TRP19", ""},
    {"TRP20", ""},
    {"TRP21", ""},
    {"TRP22", "duplicate user-language TRACE PROPERTY specification statement: "},
    {"TRP23", ""},

    {"DPR1", ""},
    {"DPR2", ""},
    {"DPR3", ""},
    {"DPR4", ""},
    {"DPR5", ""},
    {"DPR6", ""},
    {"DPR7", ""},
    {"DPR8", "this was found where a left parenthesis, \"(\" was expected: "},
    {"DPR9", "superfluous input parameter in this function: "},
    {"DPR10", ""},
    {"DPR11", ""},
    {"DPR12", "this was found where a comma was expected: "},
    {"DPR13", "this was found where the keyword PROPERTY was expected: "},
    {"DPR14", ""},
    {"DPR15", "this property identifier is declared twice:"},
    {"DPR16", ""},
    {"DPR17", "this was found where a layer or pin name was expected: "},
    {"DPR18", ""},
    {"DPR19", "this was found where an expression representing a numeric value was expected: "},
    {"DPR20", "this process variable does not have a numeric value: "},
    {"DPR21", "this variable was used on the right before it was unconditionally initialized: "},
    {"DPR22", ""},
    {"DPR23", ""},
    {"DPR24", ""},
    {"DPR25", ""},
    {"DPR26", ""},
    {"DPR27", ""},
    {"DPR28", ""},
    {"DPR29", ""},
    {"DPR30", ""},
    {"DPR31", ""},
    {"DPR32", "this was found where a constant or process variable was expected: "},
    {"DPR33", ""},
    {"DPR34", ""},
    {"DPR35", "this property was never assigned a value: "},
    {"DPR36", "the property was not assigned a value in all cases of the builtin program: "},
    {"DPR37", "this function cannot have identical arguments: "},
    {"DPR38", "both arguments of this function cannot be associated with the same layer: "},
    {"DPR39", ""},
    {"DPR40", ""},
    {"DPR41", ""},
    {"DPR42", ""},
    {"DPR43", ""},
    {"DPR44", ""},
    {"DPR45", ""},
    {"DPR46", ""},
    {"DPR47", ""},
    {"DPR48", ""},
    {"DPR49", ""},
    {"DPR50", ""},
    {"DPR51", ""},
    {"DPR52", ""},
    {"DPR53", "this layer is not allowed in this context: "},
    {"DPR54", ""},
    {"DPR55", ""},
    {"DPR56", "this was found where a text property layer name was expected: "},
    {"DPR57", "this was found where a string constant is required: "},
    {"DPR58", ""},
    {"DPR59", "this layer argument must occur only once in the device layer definitions, try a pin name: "},
    {"DPR60", "complex properties can only be accessed inside TVF functions: "},
    {"DPR61", ""},
    {"DPR62", ""},
    {"DPR63", ""},

    {"ERC1","EXCLUDE SUPPLY requires that either POWER or GROUND be specifed but neither is: "},

    {"DEV1", "missing element name for a device definition."},
    {"DEV2", "missing device layer for a device definition."},
    {"DEV3", "improperly specified device definition model name, pin name, or pin swap group."},
    {"DEV4", "pin name specification in device definition improperly follows this device layer: "},
    {"DEV5", "too few pin layers or no pin layers specified for this device definition: "},
    {"DEV6", "a device definition pin swap group must contain more than just this one pin name: "},
    {"DEV7", "a pin name must be attached to this device definition pin layer: "},
    {"DEV8", "this pin name is not valid for the device definition element name and pin layer: "},
    {"DEV9", "this pin name is duplicated within the device definition: "},
    {"DEV10", "improperly specified auxiliary layer specification."},
    {"DEV11", "this device definition pin swap group name is not the pin name for any pin layer:"},
    {"DEV12", "this pin name within a device definition appears more than once in pin swap groups:"},
    {"DEV13", "Deprecated."},
    {"DEV14", "this device definition pin layer does not have consistent pin swap group membership: "},
    {"DEV15", "this device layer appears as more than one pin layer in the same device definition: "},
    {"DEV16", "identical pin/auxiliary layer sets in multiple device definitions for this device layer: "},
    {"DEV17", "different pin name sets in multiple device definitions with this element name: "},
    {"DEV18", "DEV16"},
    {"DEV19", "this auxiliary layer is also the device layer: "},
    {"DEV20", "this auxiliary layer is duplicated within the same device definition: "},
    {"DEV21", "this auxiliary layer is also a pin layer in a device definition with the same device layer: "},
    {"DEV22", "superfluous device property specification."},
    {"DEV23", "conflicting pin recognition algorithms in multiple device definitions for this device layer: "},
    {"DEV24", ""},
    {"DEV25", "conflicting model text layer for this device layer: "},
    {"DEV26", "this context layer is duplicated within the same device definition: "},
    {"DEV27", ""},
    {"DEV28", ""},
    {"DEV29", ""},
    {"DEV30", "signature string specified does not decode into a valid signature: Unknown version number of encoded signature."},
    {"DEV31", "signature keyword requires a reference layer following the signature string "},
    {"DEV32", "signature keyword requires at least 1 context layer following the reference layer "},
    {"DEV33", "a reference layer must be associated with only one ordered context layer list across all devices, mp(pmos) has a different list."},
    {"DEV34", "the reference layer must not be repeated in the context layer list: "},
    {"DEV35", "it not supported currently for this component type: "},
    {"DEV36", "it not supported currently for this component subtype: "},
    {"DEV37", "invalid component type or component subtype params for devices: "},
    {"USER1", ""},
    {"ARG1",  "unrecognized builtin program section or a expression was expected: "},

    {"TRT1", "missing TVF FUNCTION name for TVF operation."},
    {"TRT2", "no TVF FUNCTION corresponding to this TVF operation function name: "},
    {"TRT5", "At least one argument is required for a DEVICE TVF_NUMERIC_FUNCTION: "},

    {"RDBG1", "invalid layer number specified for this operation: "},
    {"RDBG2", "invalid data type specified for this operation: "},
    {"RDBG3", "the values of all secondary keywords must match for all DFM RDB GDS operations having the same output filename: "},
    {"RDBG4", "invalid input layer specified for this operation: "},

    {"CON1", "this operation or specification does not have corresponding command in pvrs: "},
    {"CON2", "this option does not have corresponding option in the pvrs: "},
    {"CON3", "the encrypted lines can not be converted to pvrs."},

    {"ERCTVF1", "missing or invalid library name for this operation: "},
    {"ERCTVF2", "missing or invalid procedure names' list for this operation: "},
    {"ERCTVF3", "duplicate check name for this operation: "},
    {"ERCTVF4", "missing or invalid check name for this operation: "},
};

const rcErrorMsg_T* rcErrManager_T::m_pvErrMsg = NULL;

rcErrManager_T&
rcErrManager_T::Singleton()
{
	static rcErrManager_T aInstance;
	aInstance.initial();
	return aInstance;
}

rcErrManager_T::rcErrManager_T()
{
}


void
rcErrManager_T::initial()
{
    m_pvErrMsg = vSvrfErrorMsg;
}

void
rcErrManager_T::addError(const rcErrorNode_T& error, std::ostream &out)
{
	m_vErrorList.push_back(error);

	if((rcErrorNode_T::FATAL == error.m_eErrLevel || rcErrorNode_T::ERROR == error.m_eErrLevel) && error.m_nType != INCL1)
	{
	    if(rcsManager_T::getInstance()->isGui())
	        throw false;
	    else
	        Report(out);
	}
}

void
rcErrManager_T::MapLineIndex(const hvUInt32 &nGlobalLineNo, const hvUInt32 &nLocalLineNo, const string &sFileName)
{
	m_mapLineIndex.insert(std::make_pair(nGlobalLineNo, std::make_pair(sFileName, nLocalLineNo)));
}

void
rcErrManager_T::reportError(const rcErrorNode_T& error, std::ostream &out)
{
    if( rcErrorNode_T::FATAL == error.m_eErrLevel || rcErrorNode_T::ERROR == error.m_eErrLevel )
    {
        out << "Compile-Error " << vSvrfErrorMsg[error.m_nType].pType;
    }
    else
    {
        out << "Compile-Warning " << vSvrfErrorMsg[error.m_nType].pType;
    }

    if(0 != error.m_nLineNo)
    {
        out << " on line ";
        const rcErrManager_T::LINE_INDEX_MAP::iterator iter =
                m_mapLineIndex.find(error.m_nLineNo);
        if(iter != m_mapLineIndex.end())
        {
            out << (iter->second.second) << " of " << '\"' <<(iter->second.first) << '\"';
        }
    }

    out << " - " << vSvrfErrorMsg[error.m_nType].pMsg;

    if(!error.m_eKeyValue.empty())
    {
        out << error.m_eKeyValue << ".";
    }

    out << std::endl;
    out.flush();
}

void
rcErrManager_T::clear()
{
    m_mapLineIndex.clear();
    m_vErrorList.clear();
}

void
rcErrManager_T::Report(std::ostream &out)
{
    Utassert(m_pvErrMsg != NULL);

    bool needExit = false;
	for(std::vector<rcErrorNode_T>::iterator iter = m_vErrorList.begin();
		iter != m_vErrorList.end(); ++iter)
	{
	    if(rcErrorNode_T::FATAL == iter->m_eErrLevel || rcErrorNode_T::ERROR == iter->m_eErrLevel)
	        needExit = true;
	    
        reportError(*iter, out);
        reportError(*iter, rcsManager_T::getInstance()->getPVRSSumStream());
	}

	if(needExit)
	{
	    rcsManager_T::getInstance()->endWrite();
	    if(!rcsManager_T::getInstance()->isGui())
	        exit(1);
	}
}

bool
rcErrManager_T::getErrors(std::vector<std::string> &vErrMsgs)
{
    bool bHasError = !m_vErrorList.empty();
    for(std::vector<rcErrorNode_T>::iterator iter = m_vErrorList.begin();
        iter != m_vErrorList.end(); ++iter)
    {
        std::string sErrMsg;
        std::ostringstream o(sErrMsg);
        reportError(*iter, o);
        vErrMsgs.push_back(o.str());
    }

    return bHasError;
}

#ifdef DEBUG
void
rcErrManager_T::printMap()
{
	rcErrManager_T::LINE_INDEX_MAP::iterator iter;
	std::cout<<"********************************************"<<std::endl;
	for(iter = m_mapLineIndex.begin(); iter != m_mapLineIndex.end(); ++iter)
	{
		std::cout << (*iter).first << " = <" << (*iter).second.first << " , " << (*iter).second.second << " >" << std::endl;
	}
	std::cout<<"********************************************"<<std::endl;
}
#endif

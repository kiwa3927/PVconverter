%{
/*flex -+ -i -orcstvfflex.cpp rcstvflexrule.l*/

#include "public/token/rcsTokenTypes.h"

#undef YY_BUF_SIZE
#define YY_BUF_SIZE 32 * 1024
#undef YY_READ_BUF_SIZE
#define YY_READ_BUF_SIZE 16 * 1024
#include <libio.h>
%}

%option prefix="tvf"
%option noyywrap
%option yylineno

STRING          ((\"((\\[^\n\r]?)|[^"\\\r\n])*["])|(\'((\\[^\r\n]?)|[^'\\\r\n])*[']))

%%

(tvf\:\:)?ATTACH |
(tvf\:\:)?FLATTEN |
(tvf\:\:)?MDP |
(tvf\:\:)?RESOLUTION |
(tvf\:\:)?CAPACITANCE |
(tvf\:\:)?FRACTURE |
(tvf\:\:)?MDPMERGE | 
(tvf\:\:)?SCONNECT |
(tvf\:\:)?CONNECT |
(tvf\:\:)?GROUP |
(tvf\:\:)?MDPSTAT |
(tvf\:\:)?SNAP |
(tvf\:\:)?COPY |
(tvf\:\:)?HCELL |
(tvf\:\:)?MDPVERIFY |
(tvf\:\:)?SOURCE |
(tvf\:\:)?DBCLASSIFY |
(tvf\:\:)?INCLUDE |
(tvf\:\:)?PARASITIC |
(tvf\:\:)?SVRF |
(tvf\:\:)?DEVICE |
(tvf\:\:)?INDUCTANCE |
(tvf\:\:)?PERC |
(tvf\:\:)?TEXT |
(tvf\:\:)?DFM |
(tvf\:\:)?LABEL |
(tvf\:\:)?PEX |
(tvf\:\:)?TITLE |
(tvf\:\:)?DISCONNECT |
(tvf\:\:)?LAYER |
(tvf\:\:)?POLYGON |
(tvf\:\:)?TRACE |
(tvf\:\:)?DRC |
(tvf\:\:)?LAYOUT |
(tvf\:\:)?PORT |
(tvf\:\:)?TVF |
(tvf\:\:)?ERC |
(tvf\:\:)?LITHO |
(tvf\:\:)?PRECISION |
(tvf\:\:)?UNIT |
(tvf\:\:)?EXCLUDE |
(tvf\:\:)?LVS |
(tvf\:\:)?PUSH |
(tvf\:\:)?VARIABLE |
(tvf\:\:)?EXPAND |
(tvf\:\:)?MASK |
(tvf\:\:)?RESISTANCE |
(tvf\:\:)?VIRTUAL |
(tvf\:\:)?FLAG 							   {return TVF_SVRF_COMMAND;}

(tvf\:\:)?\/\/    				           {return TVF_COMMENT;}
(tvf\:\:)?COMMENT				           {return TVF_RULE_CHECK_COMMENT;}
(tvf\:\:)?@ 	  				           {return TVF_RULE_CHECK_COMMENT;}
(tvf\:\:)?epilog				           {return TVF_EPILOG;}
(tvf\:\:)?print_call_stack		           {return TVF_PRINT_CALL_STACK;}
(tvf\:\:)?VERBATIM  			           {return TVF_VERBATIM;}
(tvf\:\:)?SETLAYER  			           {return TVF_SETLAYER;}
(tvf\:\:)?RULECHECK 			           {return TVF_RULE_CHECK;}
(tvf\:\:)?OUTLAYER  			           {return TVF_OUTLAYER;}
(tvf\:\:)?echo_svrf 			           {return TVF_ECHO_SVRF;}
(tvf\:\:)?block_transcript 		           {return TVF_BLOCK_TRANSCRIPT;}
(tvf\:\:)?is_transcript_blocked            {return TVF_IS_TRANSCRIPT_BLOCKED;}
(tvf\:\:)?unblock_transcript               {return TVF_UNBLOCK_TRANSCRIPT;}
(tvf\:\:)?get_tvf_arg			           {return TVF_GET_TVF_ARG;}
(tvf\:\:)?echo_to_svrf_file		           {return TVF_ECHO_TO_SVRF_FILE;}
(tvf\:\:)?output_to_svrf    	           {return TVF_OUTPUT_TO_SVRF;}
(tvf\:\:)?get_calibre_version              {return TVF_GET_CALIBRE_VERSION;} 
(tvf\:\:)?exists_global_variable           {return TVF_EXISTS_GLOBAL_VARIABLE;}
(tvf\:\:)?set_global_variable              {return TVF_SET_GLOBAL_VARIABLE;}
(tvf\:\:)?get_global_variable              {return TVF_GET_GLOBAL_VARIABLE;}
(tvf\:\:)?unset_global_variable            {return TVF_UNSET_GLOBAL_VARIABLE;}
(tvf\:\:)?DELETE_LAYER		               {return TVF_DELETE_LAYER;}
(tvf\:\:)?GET_LAYER_ARGS		           {return TVF_GET_LAYER_ARGS;}
(tvf\:\:)?GET_LAYER_EXTENT                 {return TVF_GET_LAYER_EXTENT;}
(tvf\:\:)?GET_LAYER_NAME		           {return TVF_GET_LAYER_NAME;}
(tvf\:\:)?IS_LAYER_EMPTY 		           {return TVF_IS_LAYER_EMPTY;}
(tvf\:\:)?svrf_var				           {return TVF_SVRF_VAR;}
(tvf\:\:)?sys_var				           {return TVF_SYS_VAR;}
(tvf\:\:)?set_disclaimer                   {return TVF_SET_DISCLAIMER;}
(tvf\:\:)?set_rule_check_indentation       {return TVF_SET_RULE_CHECK_INDENTATION;}
(tvf\:\:)?set_traceback_line               {return TVF_SET_TRACEBACK_LINE;}
(erc\:\:)?setup_parallel_device_parameters             {return ERC_SETUP_PARALLEL_DEVICE_PARAMS;}
(erc\:\:)?execute_parallel_device_parameters             {return ERC_EXECUTE_PARALLEL_DEVICE_PARAMS;}

[[:digit:]]+(\.)?(0)*                               {return INT_NUMBER;}
[[:digit:]]*\.[[:digit:]]+                          {return FLOAT_NUMBER;}
0[xX][[:xdigit:]]+                                  {return HEX_NUMBER;}
[[:digit:]]*\.[[:digit:]]+([eE][+-]?[[:digit:]]+)   {return SCIENTIFIC_NUMBER;}
[[:digit:]]+(\.)?(0)*([eE][+-]?[[:digit:]]+)        {return SCIENTIFIC_NUMBER;}

{STRING}                  		 {return STRING_CONSTANTS;}
[ \t]*[\n\r]*                    {return BLANK_SPACE;}

("&&")|("||")|("::")|([><!=][=])|[><!~\+\-\*\/\%\^\$\?\&\|\:] {return OPERATOR;}
[=\{\}\[\]\(\),\[\]]                                          {return SEPARATOR;}

[_[:alnum:]\.\;\\\#\`]+    { return IDENTIFIER_NAME;}

%%

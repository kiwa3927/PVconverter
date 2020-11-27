%{
/*flex -+ -i -orcspreflex.cpp rcsprerule.l*/

#undef YY_BUF_SIZE
#define YY_BUF_SIZE 32768
#include <libio.h>
%}

%option prefix="pp"
%option noyywrap
%option yylineno

%%

"@".*[\n\r] {return 1;}

((\"((\\[^\n\r]?)|[^"\\\n\r])*[\r\n])|(\'((\\[^\r\n]?)|[^'\\\r\n])*[\r\n])|(\"((\\[^\n\r]?)|[^"\\\r\n])*["])|(\'((\\[^\r\n]?)|[^'\\\r\n])*['])) {return 2;}

.         {}

[ \t]*[\n\r]*   {}
%%

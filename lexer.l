/*
* Author: Martin Brugnara #157791 <martin.brugnara@studenti.unitn.it>
*/

%{
    #include <stdlib.h>
    #include <string.h>
    #include "interpreter.h"
    #include "y.tab.h"
%}

%%

">="            return GTE; // Start of Boolean exp
"<="            return LTE;
"=="            return DEQ;
"!="            return NE;

"||"            return OR; // Boolean expression evaluation
"and"           return AND;
"not"           return NOT;

"while"         return WHILE; // Start of Flow Control
"if"            return IF;
"else"          return ELSE;
"print"         return PRINT;
"for"           return FOR;
"to"            return TO;

"int"           return INT; // Start of Type
"real"          return REAL;
"bool"          return BOOL;

"main"          return MAIN; // Main entry point

"true"  {
            yylval.bVal = 1;
            return BOOLEAN;
        }

"false" {
            yylval.bVal = 0;
            return BOOLEAN;
        }


[a-zA-Z][a-zA-Z0-9_]*  { // Get or create variable
                        // var name + \0
                        yylval.varName = (char *)xmalloc(strlen(yytext) + 1);
                        // strcpy also copy \0
                        strcpy(yylval.varName, yytext);
                        return VARIABLE_NAME;
                    }

0               {
                    yylval.iVal = atoi(yytext);
                    return INTEGER;
                }

(0,|[1-9][0-9]*,[0-9]+)    { // Parse the whole string and replace ',' the atof
                            char * f = (char *) xmalloc(strlen(yytext) + 1);
                            strcpy(f, yytext);

                            // We are sure ',' is in there.
                            char * c = f;
                            for(; *c != ','; c++);
                            *c = '.';

                            yylval.rVal = atof(f);

                            free(f);
                            return REALNUM;
                        }

[1-9][0-9]*     {
                    yylval.iVal = atoi(yytext);
                    return INTEGER;
                }

"-"   return MIN;
"+"   return PLUS;
"<"   return LT;
">"   return GT;
"="   return EQ;
"*"   return MUL;
"/"   return DIV;

[ \t\n]         ;/* skip whitespace */
\;              return SEMICOLON;
\,              return COMMA;
\(              return LP;
\)              return RP;
\{              return LCURLY;
\}              return RCURLY;
.               {ECHO;
                yyerror("Previous char is unknown to lexer");}
%%

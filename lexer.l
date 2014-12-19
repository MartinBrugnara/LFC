/*
* Author: Martin Brugnara #157791 <martin.brugnara@studenti.unitn.it>
*/

%{
    #include <stdlib.h>
    #include <string.h>
    #include "y.tab.h"
    #include "interpreter.h"
%}

%%

">="            return GTE; // Start of Boolean exp
"<="            return LTE;
"=="            return DEQ;
"!="            return NE;

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


[a-zA-Z][a-zA-Z_]*  { // Get or create variable
                        symrec * s = getsym(yytext);
                        if (s == 0){ // Does not exists (yet)
                            s = (symrec *) xmalloc (sizeof (symrec));
                            // var name + \0
                            s->name = (char *) xmalloc(strlen(yytext) + 1);
                            // strcpy also copy \0
                            strcpy (s->name,yytext);
                        }
                        // yylval = s
                        *((symrec**)&yylval) = s;
                        return VARIABLE;
                    }

0               {
                    yylval.iValue = atoi(yytext);
                    return INTEGER;
                }

([1-9][0-9]*,[0-9]+)    { // Parse the whole string and replace ',' the atof
                            char * f = (char *) xmalloc(strlen(yytext) + 1);
                            strcpy(f, yytext);

                            // We are sure ',' is in there.
                            int i;
                            for(i=0; f[i] != ','; i++);
                            f[i] = '.';

                            yylval.fVal = atof(f);

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

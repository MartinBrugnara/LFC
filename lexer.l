/*
* Author: Martin Brugnara #157791 <martin.brugnara@studenti.unitn.it>
*/

%{
    #include <stdlib.h>
    #include "y.tab.h"
    #include "interpreter.h"
%}

%%

">="            return GE; // Start of Boolean exp
"<="            return LE;
"=="            return EQ;
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
                            s = (symrec *) malloc (sizeof (symrec));

                            // var name + \0
                            s->name = (char *) malloc(strlen(yytext) + 1);

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
                            char * f = (char *) malloc(strlen(yytext) + 1);
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

[-+<>=*/.]   { return *yytext; /*TODO: remove .? */ }

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

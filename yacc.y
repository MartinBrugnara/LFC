/* Author: Martin Brugnara #157791 <martin.brugnara@studenti.unitn.it> */

%{
    #include <stdlib.h>
    #include <stdio.h>
    #include "interpreter.h"

    symrec * symTable = NULL;

    // ext references (dark magic)
    int yylex();
    extern conNodeType * ex(nodeType *);
%}
%error-verbose

%union { // yylval_t
    int     iVal;
    float   rVal;
    int     bVal;

    //struct symrec   * sRec; // was used from the shit in lexer
    nodeType * nPtr;

    char * varName;

    // varTypeEnum type; // was used by  B -> ...
};

/*
    Define association between llval &
    llval_type (return argv of lex matching)
    format:
        %token <field_name of union> TOKEN
*/
%token <iVal> INTEGER
%token <rVal> REALNUM
%token <bVal> BOOLEAN
%token <varName> VARIABLE_NAME

%token WHILE IF PRINT FOR TO INT REAL BOOL AND OR NOT PRINTINT PRINTREAL PRINTBOOL
%nonassoc IFX
%nonassoc ELSE

%left EQ
%left AND OR
%left DEQ NE GT LT GTE LTE
%left PLUS MIN
%left MUL DIV
%right NOT
%nonassoc UMINUS RCURLY LCURLY LP RP COMMA SEMICOLON INTEGER REALNUM BOOLEAN MAIN

%type <nPtr> stmt dec expr opt_stmt_list stmt_list opt_dec_list variable scope

%%

program: opt_dec_list
        MAIN
        scope       {
                            /* since I do not give a fuck about func
                                I deliberately skip scope creation */
                            ex($1); // Populate Global VARS
                            ex($3); // Exec Main
                            exit(0); // exit success
                        }
        ;

opt_dec_list: /* empty */       {$$ = NULL;}
            | opt_dec_list dec  {$$ = opr(SEMICOLON, 2, $1, $2);}
            ;


dec: INT  VARIABLE_NAME SEMICOLON    { $$ = dic($2, INTTYPE); }
   | REAL VARIABLE_NAME SEMICOLON    { $$ = dic($2, REALTYPE); }
   | BOOL VARIABLE_NAME SEMICOLON    { $$ = dic($2, BOOLTYPE); }
   ;

variable: VARIABLE_NAME             { $$ = id($1);}
        ;

scope: LCURLY opt_stmt_list RCURLY                       {$$ = block($2);}
    ;

opt_stmt_list: /* empty */ {$$ = NULL;}
             | stmt_list   {$$ = $1;}
             ;

stmt_list: stmt
         | stmt_list stmt   {$$ = opr(SEMICOLON, 2, $1, $2);}
         ;

stmt: SEMICOLON                                     {$$ = opr(SEMICOLON, 2, NULL, NULL);}
    | dec
    | expr SEMICOLON
    | PRINT expr SEMICOLON                          {$$ = opr(PRINT,1,$2);}
    | PRINTINT expr SEMICOLON                       {$$ = opr(PRINTINT,1,$2);}
    | PRINTREAL expr SEMICOLON                      {$$ = opr(PRINTREAL,1,$2);}
    | PRINTBOOL expr SEMICOLON                      {$$ = opr(PRINTBOOL,1,$2);}
    | variable EQ expr SEMICOLON                    {$$ = opr(EQ,2,$1,$3);}
    | WHILE LP expr RP stmt                         {$$ = opr(WHILE,2,$3,$5);}
    | IF LP expr RP stmt %prec IFX                  {$$ = opr(IF,2,$3,$5);}
    | IF LP expr RP stmt ELSE stmt                  {$$ = opr(IF,3,$3,$5,$7);}
    | FOR LP variable EQ expr TO expr RP stmt       {$$ = opr(FOR,4,$3,$5,$7,$9);}
    | scope                                         {$$ = $1;}
    ;

expr : INTEGER      {$$ = con(&$1, INTTYPE);}
    | REALNUM       {$$ = con(&$1, REALTYPE);}
    | BOOLEAN       {$$ = con(&$1, BOOLTYPE);}
    | variable      /*manage variables - namely an IDENTIFIER*/
    | LP expr RP    {$$ = $2;}

    | expr PLUS expr  {$$ = opr(PLUS,2,$1,$3);}
    | expr MIN expr   {$$ = opr(MIN,2,$1,$3);}
    | expr MUL expr   {$$ = opr(MUL,2,$1,$3);}
    | expr DIV expr   {$$ = opr(DIV,2,$1,$3);}
    | MIN expr %prec UMINUS    {$$ = opr(UMINUS,1,$2);}

    | expr LT expr          {$$ = opr(LT,2,$1,$3);}
    | expr GT expr          {$$ = opr(GT,2,$1,$3);}
    | expr LTE expr         {$$ = opr(LTE,2,$1,$3);}
    | expr GTE expr         {$$ = opr(GTE,2,$1,$3);}
    | expr NE expr          {$$ = opr(NE,2,$1,$3);}
    | expr DEQ expr         {$$ = opr(DEQ,2,$1,$3);}
    | expr AND expr         {$$ = opr(AND,2,$1,$3);}
    | expr OR expr          {$$ = opr(OR,2,$1,$3);}
    | NOT expr              {$$ = opr(NOT,1,$2);}
    ;

%%

int main(){
    yyparse();
    return 0;
}

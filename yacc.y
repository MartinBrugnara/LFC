/* Author: Martin Brugnara #157791 <martin.brugnara@studenti.unitn.it> */

%{
    // TODO: include
%}
%error-verbose

%union { // yylval_t
    int     iValue;
    float   rValue;
    int     bValue;

    symrec   * sRec;
    nodeType * nPtr;

    varTypeEnum varType;
    parameter *parameter;
    routine * routine;
    linkedList *lista;
};

/*
    Define associetion between llval &
    llval_type (return argv of lex matching)
    format:
        %token <field_name of union> TOKEN
*/
%token <iValue> INTEGER
%token <rValue> REALNUM
%token <bValue> BOOLEAN
%token <sRec>   VARIABLE

/* TODO: go ahed  with token definition and rules */

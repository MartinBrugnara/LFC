/* Author: Brugnara Martin #157791 */

#ifndef INTERPRETER
#define INTERPRETER

#include <sys/types.h>

void * xmalloc(size_t size);

// Symbol Table (list)
// Type for symrec
// !! LEAVE in this order: type checking is max(t1, t2);
typedef enum{BOOLTYPE, INTTYPE, REALTYPE} varTypeEnum; // old: basicType
typedef struct symrec {
	char * name; // Comes from lexer
    float value;
    varTypeEnum type;

	struct symrec *next;
} symrec;

typedef struct {
    char * name;
    varTypeEnum type;
} dicNodeType;

/* Code node tree */
/* constants */
typedef struct {
    float value;
    varTypeEnum type;
} conNodeType;

/* identifiers */
typedef struct {char * name;} idNodeType;

/* operators */
typedef struct {
    int  oper;              /* operator */
    int  nops;              /* number of operands */
    struct nodeType **op;	/* operands */
} oprNodeType;

/* used in the struct nodeType to define the type of node*/
typedef enum {nodeDic, nodeCon, nodeId, nodeOpr, nodeBlock} nodeEnum;
typedef struct nodeType{
    nodeEnum type;              /* type of node */

    union {
        conNodeType con;        /* constants */
        idNodeType id;          /* identifiers */
        oprNodeType opr;        /* operators */
        dicNodeType dic;        /* declararion  */
        struct nodeType * blk;         /* scooping and procedure */
    };
} nodeType;

nodeType * block(nodeType * next);
nodeType * con(float, varTypeEnum);
nodeType * id(const char * const);

symrec * getsym(const char * const identifier);
int isdefsym(const char * const identifier, const symrec * const EBP);
symrec * putsym(char const * identifier, varTypeEnum type);

nodeType * dic(char *, varTypeEnum);
nodeType * opr(int oper, int nops, ...);

void yyerror(const char *);

/* GLOBAL VARS */
extern symrec * symTable; /* declared in yacc.y */
#endif

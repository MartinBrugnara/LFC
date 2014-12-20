#ifndef INTERPRETER
#define INTERPRETER

#include <sys/types.h>

void * xmalloc(size_t size);

// Symbol Table (list)
// Type for symrec
// !! LEAVE in this order: coercion could be done via max(t1, t2);
typedef enum{BOOLTYPE, INTTYPE, REALTYPE} varTypeEnum; // old: basicType
typedef struct symrec {
	char * name; // Comes from lexer
    union { // Comes from yacc & run time
        int     i;
        float   r;
        int     b;
    };
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
    union {
        int i;
        float r;
        int b;
    };
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
// TODO: delete me
// typedef enum {typeCon, typeId, typeOpr} nodeEnum;
typedef enum {nodeDic, nodeCon, nodeId, nodeOpr} nodeEnum;
typedef struct nodeType{
    nodeEnum type;              /* type of node */

    union {
        conNodeType con;        /* constants */
        idNodeType id;          /* identifiers */
        oprNodeType opr;        /* operators */
        dicNodeType dic;        /* declararion  */
    };
} nodeType;


nodeType * con(void *value, varTypeEnum type);
nodeType * id(symrec * ide);


symrec * getsym(const char * const identifier);
symrec * putsym(char const * identifier, varTypeEnum type);

nodeType * dic(char *, varTypeEnum);
nodeType * opr(int oper, int nops, ...);

void yyerror(const char *);

/* GLOBAL VARS */
extern symrec * symTable; /* declared in yacc.y */
#endif

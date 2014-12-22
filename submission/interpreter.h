/* Author: Brugnara Martin #157791 */

#ifndef INTERPRETER
#define INTERPRETER

/* ============================  Symbol Table  ============================== */

// DO NOT CHANGE THE ORDER. (-> see how type promotion is implemented (max))
typedef enum{BOOLTYPE, INTTYPE, REALTYPE} varTypeEnum; // old: basicType
typedef struct symrec {
	char * name;
    float value;
    varTypeEnum type;

	struct symrec *next;
} symrec;

/* ============================ Sym Table func ============================== */
symrec * getsym(const char * const identifier);
int isdefsym(const char * const identifier, const symrec * const EBP);
symrec * putsym(char const * identifier, varTypeEnum type);

/* ============================ Code node tree ============================== */
typedef struct {
    char * name;
    varTypeEnum type;
} dicNodeType;

typedef struct {
    float value;
    varTypeEnum type;
} conNodeType;

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
        struct nodeType * blk;  /* scooping and procedure */
    };
} nodeType;

/* ============================ Node tree func ============================== */
nodeType * block(nodeType * next);
nodeType * con(float, varTypeEnum);
nodeType * id(const char * const);
nodeType * dic(char *, varTypeEnum);
nodeType * opr(int oper, int nops, ...);

/* ============================  GLOBAL VARS   ============================== */
extern symrec * symTable; /* declared in yacc.y */

#endif

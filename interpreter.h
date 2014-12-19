
// Symbol Table (list)
// Type for symrec
typedef enum{INTTYPE, REALTYPE, BOOLTYPE} varTypeEnum; // old: basicType
typedef struct symrec {
	char * name; // Comes from lexer
    union { // Comes from yacc & run time
        int     iVal;
        float   fVal;
        int     bVal;
    };
    varTypeEnum varType;

	struct symrec *next;
} symrec;

/* Code node tree */

/* constants */
typedef struct {
    union {
        int i;
        float f;
        int b;
    };
} conNodeType;

/* identifiers */
typedef struct {char * name;} idNodeType;

/* operators */
typedef struct {
    char oper;              /* operator */
    int  nops;              /* number of operands */
    struct nodeType **op;	/* operands */
} oprNodeType;

/* used in the struct nodeType to define the type of node*/
// TODO: delete me
// typedef enum {typeCon, typeId, typeOpr} nodeEnum;
typedef enum {nodeCon, nodeId, nodeOpr} nodeEnum;
typedef struct nodeType{
    nodeEnum type;              /* type of node */

    union {
        conNodeType con;        /* constants */
        idNodeType id;          /* identifiers */
        oprNodeType opr;        /* operators */
    };
} nodeType;


nodeType *con(void *value, varTypeEnum type);
nodeType * id(symrec * ide);


symrec * getsym(const char * const identifier);
symrec * putsym(char const * identifier);


nodeType *opr(int oper, int nops, ...);

void yyerror(const char *);

/* GLOBAL VARS */
extern symrec * symTable; /* declared in yacc.y */

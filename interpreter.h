
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

/* used in the struct nodeType to define the type of node*/
typedef enum {nodeCon, nodeId, nodeOpr} nodeEnum;
typedef struct {
    nodeEnum type;              /* type of node */

    union {
        conNodeType con;        /* constants */
        idNodeType id;          /* identifiers */
        oprNodeType opr;        /* operators */
    };
} nodeType;

/* constants */
typedef struct {
    union {
        int i;
        float f;
        int b;
    }
} conNodeType;

/* identifiers */
typedef struct {char * name;} idNodeType;

/* operators */
typedef struct {
    char oper;                   /* operator */
    int  nops;                   /* number of operands */
    struct nodeType **op;	/* operands */
} oprNodeType;


void yyerror(const char *);

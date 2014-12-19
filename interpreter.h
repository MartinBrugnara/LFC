
// Type for symrec
typedef enum{INTTYPE, REALTYPE, BOOLTYPE} varTypeEnum; // old: basicType

typedef struct symrec
{
	char * name; // Comes from lexer
    union { // Comes from yacc & run time
        int     iVal;
        float   fVal;
        int     bVal;
    };
    varTypeEnum varType;

	struct symrec *next;
} symrec;

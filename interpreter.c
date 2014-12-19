#include "interpreter.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/* http://www.gnu.org/software/bison/manual/html_node/Error-Reporting.html
 * """ After yyerror returns to yyparse, the latter will attempt error recovery
 * if you have written suitable error recovery grammar rules (see Error Recovery).
 * If recovery is impossible, yyparse will immediately return 1."""
 */
void yyerror(const char * msg) {
    fprintf (stderr, "%s\n", msg);
}

/* Malloc with out of memory error */
void * xmalloc(size_t size) {
    void * p = malloc(size);
    if (p == NULL) {
        yyerror("Out of memory");
    }
    return p;
}

/* Create a node containing a constant value */
nodeType *con(void *value, varTypeEnum type){
    nodeType *p = (nodeType*)xmalloc(sizeof(nodeType));

    /* copy information */
    p->type = nodeCon;
    switch (type) {
        case INTTYPE:
            p->con.i = *(int*)value;
            break;
        case REALTYPE:
            p->con.r = *(float*)value;
            break;
        case BOOLTYPE:
            p->con.b = *(int*)value;
            break;
        default:
            yyerror("Error handling constant type");
    }
    return p;
}

/* Get or Create a Variable in Symbol Table. Key = id. */
nodeType * id(symrec * ide){
    nodeType * p = (nodeType*)xmalloc(sizeof(nodeType));
    p->type = nodeId;
    p->id.name = (char*)xmalloc(sizeof(strlen(ide->name)) + 1);
    strcpy(p->id.name, ide->name);

    if (getsym(ide->name) == NULL) { // Symbol not yet in SymT
        putsym(p->id.name);
    }
    return p;
}

// Search in Symbol Table (that is a fucking list) O(n)
symrec * getsym(const char * const identifier) {
    symrec *ptr;
    for (ptr = symTable; ptr != (symrec *) 0; ptr = (symrec *)ptr->next){
        if (strcmp (ptr->name, identifier) == 0)
            return ptr; // found
    }
    return NULL; // not found
}

symrec * putsym(char const * identifier) {
    symrec *ptr = (symrec *)xmalloc(sizeof (symrec));
    ptr->name = (char *) malloc (strlen (identifier) + 1);
    strcpy (ptr->name,identifier);
    ptr->next = (struct symrec *)symTable;
    symTable = ptr;
    return ptr;
}

/* Create a operand node
 * oper: operand
 * nops: #operands
 */
nodeType *opr(int oper, int nops, ...){
    /* (ap = argument pointer) va_list is used to declare a variable
            which, from time to time, is referring to an argument*/
    va_list ap;

    nodeType *p;

    p = (nodeType*)xmalloc(sizeof(nodeType));
    p->opr.op = (struct nodeType**)xmalloc(nops*sizeof(nodeType));

    p->type     = nodeOpr;
    p->opr.oper = oper;
    p->opr.nops = nops;

    // *args (list) -> p->opr.op (array)
    va_start(ap, nops);
    for(int i=0; i<nops; i++)
        p->opr.op[i]=va_arg(ap,nodeType*);
    va_end(ap);

    return p;
}

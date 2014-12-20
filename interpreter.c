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

/* Create a declaration node */
nodeType * dic(char * name, varTypeEnum type) {
    printf("XXX dic\n");
    /*
    if (getsym(name) != NULL) {
        // Yet in SymT
        fprintf(stderr, "This variable was previously declared %s\n", name);
        exit(1);
    }*/

    printf("XXX dic after getsym\n");

    nodeType *p = (nodeType*)xmalloc(sizeof(nodeType));

    p->type = nodeDic;
    p->dic.name = (char*)xmalloc(sizeof(strlen(name) + 1));
    strcpy(p->dic.name, name);
    p->dic.type = type;
    printf("XXX dic pre return\n");
    return p;
}

/* Create a node containing a constant value */
nodeType * con(void *value, varTypeEnum type){
    printf("XXX con\n");

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

/* Wrap symrec ref into a id node
 * Also checks that the var has been yet declared.
 */
nodeType * id(const char * const name){
    printf("XXX id\n");


    printf("XXX retrieving var %s\n", name);
    /*
    if (getsym(name) == NULL) {
        // Symbol not yet in SymT
        fprintf(stderr, "Undeclared variable %s\n", name);
        exit(1);
    }*/

    printf("XXX post for id\n");

    nodeType * p = (nodeType*)xmalloc(sizeof(nodeType));
    p->type      = nodeId;
    p->id.name   = (char*)xmalloc(sizeof(strlen(name)) + 1);
    strcpy(p->id.name, name);

    printf("XXX exit id\n");
    return p;
}

// Search in Symbol Table (that is a fucking list) O(n)
symrec * getsym(const char * const name) {
    printf("getting %s, symtable is %d\n", name, symTable);
    for (symrec *ptr = symTable; ptr != NULL; ptr=(symrec *)ptr->next){
        printf("len: %s | %s", ptr->name, name);
        if (!strcmp(ptr->name, name)) {
            return ptr; // found
        }
    }
    return NULL; // not found
}

symrec * putsym(char const * identifier, varTypeEnum type) {
    printf("registering %s\n", identifier);
    symrec *ptr = (symrec *)xmalloc(sizeof (symrec));
    ptr->name = (char *) xmalloc (strlen (identifier) + 1);
    strcpy (ptr->name,identifier);
    ptr->type = type;
    ptr->next = symTable; // add in head O(1)
    ptr->b=ptr->r=ptr->i=0;
    symTable = ptr;
    return ptr;
}

/* Create a operand node
 * oper: operator
 * nops: #operands
 * ... : operands *xargs of *nodeType
 */
nodeType *opr(int oper, int nops, ...){
    printf("XXX opr\n");

    nodeType *p = (nodeType*)xmalloc(sizeof(nodeType));
    p->type     = nodeOpr;

    p->opr.op = (nodeType**)xmalloc(nops*sizeof(nodeType));

    p->opr.oper = oper;
    p->opr.nops = nops;

    /* (ap = argument pointer) va_list is used to declare a variable
            which, from time to time, is referring to an argument*/
    va_list ap;
    va_start(ap, nops);
    for(int i=0; i<nops; i++)
        p->opr.op[i] = va_arg(ap, nodeType*);
    va_end(ap);

    return p;
}

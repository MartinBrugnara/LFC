#include "interpreter.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

void yyerror(const char * msg) {
    fprintf (stderr, "[ERROR] %s\n", msg);
    exit(1);
}

/* Malloc with out of memory error */
void * xmalloc(size_t size) {
    void * p = malloc(size);
    if (p == NULL) {
        yyerror("Out of memory.");
    }
    return p;
}

nodeType * block(nodeType * code) {
    nodeType *p = (nodeType*)xmalloc(sizeof(nodeType));
    p->type = nodeBlock;
    p->blk = code;
    return p;
}


/* Create a declaration node */
nodeType * dic(char * name, varTypeEnum type) {
    nodeType *p = (nodeType*)xmalloc(sizeof(nodeType));

    p->type = nodeDic;
    // NOT SURE IF COPY IS NEEDED (probably not)
    p->dic.name = (char*)xmalloc(sizeof(strlen(name) + 1));
    strcpy(p->dic.name, name);
    p->dic.type = type;

    return p;
}

/* Create a node containing a constant value */
nodeType * con(float value, varTypeEnum type){
    nodeType *p = (nodeType*)xmalloc(sizeof(nodeType));

    /* copy information */
    p->type = nodeCon;
    p->con.type = type;
    switch (type) {
        case INTTYPE:
            p->con.value = (int)value;
            break;
        case BOOLTYPE:
            p->con.value = value != 0;
            break;
        case REALTYPE:
            p->con.value = value;
            break;
        default:
            yyerror("Error handling constant type.");
    }
    return p;
}

/* Wrap symrec ref into a id node
 * Also checks that the var has been yet declared.
 */
nodeType * id(const char * const name){
    nodeType * p = (nodeType*)xmalloc(sizeof(nodeType));
    p->type      = nodeId;
    p->id.name   = (char*)xmalloc(sizeof(strlen(name)) + 1);
    strcpy(p->id.name, name);
    return p;
}

/* Check if a sym is defined in current scope.
 * Return boolean (0,1)
 */
int isdefsym(const char * const name, const symrec * const EBP) {
    for (symrec *ptr = symTable; ptr != NULL && ptr != EBP; ptr=(symrec *)ptr->next)
        if (!strcmp(ptr->name, name))
            return 1; // found
    return 0;
}

// Search in Symbol Table (that is a fucking list) O(n)
symrec * getsym(const char * const name) {
    for (symrec *ptr = symTable; ptr != NULL; ptr=(symrec *)ptr->next){
        if (!strcmp(ptr->name, name)) {
            return ptr; // found
        }
    }
    return (symrec*)0; // not found
}

symrec * putsym(char const * identifier, varTypeEnum type) {
    symrec *ptr = (symrec *)xmalloc(sizeof (symrec));
    ptr->name = (char *) xmalloc (strlen (identifier) + 1);
    strcpy (ptr->name,identifier);
    ptr->type = type;
    ptr->next = symTable; // add in head O(1)
    ptr->value=0;
    symTable = ptr;
    return ptr;
}

/* Create a operand node
 * oper: operator
 * nops: #operands
 * ... : operands *xargs of *nodeType
 */
nodeType *opr(int oper, int nops, ...){
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

/* Author: Brugnara Martin #157791 */

/* This file include function to create tree nodes.
 * It's used by yacc and by the ex() function.
 *
 * In some part is similar to es5.2/auxiliaryFunctions.c
 * but it's completely rewrote.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "interpreter.h"
#include "helper.h"

/* Create a block / scope node */
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

/* Create an id node */
nodeType * id(const char * const name){
    nodeType * p = (nodeType*)xmalloc(sizeof(nodeType));
    p->type      = nodeId;
    p->id.name   = (char*)xmalloc(sizeof(strlen(name)) + 1);
    strcpy(p->id.name, name);
    return p;
}

/* Check if a sym is defined in current scope.
 * Return 1 (yes) 0 (no)
 */
int isdefsym(const char * const name, const symrec * const EBP) {
    for (symrec *ptr = symTable; ptr != NULL && ptr != EBP; ptr=(symrec *)ptr->next)
        if (!strcmp(ptr->name, name))
            return 1; // found
    return 0;
}

/* Search in Symbol Table ('Table' ... Stack ) O(n) */
symrec * getsym(const char * const name) {
    for (symrec *ptr = symTable; ptr != NULL; ptr=(symrec *)ptr->next){
        if (!strcmp(ptr->name, name)) {
            return ptr; // found
        }
    }
    return (symrec*)0; // not found
}


/* Register a var in Symbol Table ('Table' ... Stack ) O(1) */
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

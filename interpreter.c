#include "interpreter.h"
#include <stdlib.h>
#include <stdio.h>

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

nodeType *con(int value){
    nodeType *p;
    /* allocate node space in memory */
    if((p=malloc(sizeof(nodeType))) == NULL){
        yyerror("out of memory");
    }
    /* copy information */
    p->type = typeCon;
    p->con.value = value;
    return p;
}

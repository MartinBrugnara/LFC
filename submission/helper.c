/* Author: Brugnara Martin #157791 */

/* This file contains general purpose functions
 * used project wide.
 */

#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include "helper.h"

void yyerror(const char * msg) {
    fprintf (stderr, "[ERROR] %s\n", msg);
    exit(1);
}

/* malloc with out of memory error checking */
void * xmalloc(size_t size) {
    void * p = malloc(size);
    if (p == NULL) {
        yyerror("Out of memory.");
    }
    return p;
}

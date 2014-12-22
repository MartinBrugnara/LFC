/* Author: Brugnara Martin #157791 */

/* This file implement coercion (that is used for type checking too)
 */

#include "interpreter.h"
#include "helper.h"

// [dst][src]
char coercion_table[3][3] = {
    // B, I, R
      {1, 1, 0}, // B
      {0, 1, 0}, // I
      {0, 1, 1}, // R
};

conNodeType * coercion(conNodeType * a, varTypeEnum req) {
    if (!a) return 0;
    if (a->type == req) return a; // easy case
    if (!coercion_table[req][a->type]) yyerror("Type error.");

    a->type = req;
    if (req == REALTYPE) a->value = (float)a->value;
    else if (req == BOOLTYPE) a->value = a->value != 0;

    return a;
}

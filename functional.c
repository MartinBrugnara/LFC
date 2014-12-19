#include "interpreter.h"
#include "functional.h"

float ass(float a, float b) {return a;}
float neg(float a, float b) {return -a;}

float sum(float a, float b) {return a + b;}
float min(float a, float b) {return a - b;}
float mul(float a, float b) {return a * b;}
float dvi(float a, float b) {return a / b;} // DIV was yet taken

float gt(float a, float b) {return a > b;}
float lt(float a, float b) {return a < b;}
float gte(float a, float b) {return a >= b;}
float lte(float a, float b) {return a <= b;}
float deq(float a, float b) {return a == b;}
float neq(float a, float b) {return a != b;}


varTypeEnum max(varTypeEnum a, varTypeEnum b) {return a>b?a:b;}

conNodeType * apply(mappable f, conNodeType * a, conNodeType * b, varTypeEnum dstType) {
    conNodeType * res = xmalloc(sizeof(conNodeType*));

    float fst;
    switch (a->type) {
        case INTTYPE:   fst = (float)(a->i);break;
        case REALTYPE:  fst = (float)(a->r);break;
        case BOOLTYPE:  fst = (float)(a->b);break;
        default: yyerror("Unrecognized type.");
    }

    float snd;
    if (b) // OPTIONAL
        switch (a->type) {
            case INTTYPE:   snd = (float)(b->i);break;
            case REALTYPE:  snd = (float)(b->r);break;
            case BOOLTYPE:  snd = (float)(b->b);break;
            default: yyerror("Unrecognized type.");
        }

    res->type = dstType;
    switch (res->type) {
        case INTTYPE:   res->i = (int)(*f)(fst, snd); break;
        case REALTYPE:  res->r = (float)(*f)(fst, snd); break;
        case BOOLTYPE:  res->b = (int)(*f)(fst, snd) != 0; break;
        default: yyerror("Unrecognized type.");
    }

    return res;
}

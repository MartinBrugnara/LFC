#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "interpreter.h"
#include "functional.h"
#include "y.tab.h"

// Hold ex() return value.
// NULL if no return value (eg declaration, assignment).
typedef conNodeType ret;

// used for ++/-- operations
nodeType * __ONE = 0;
nodeType * ONE() {
    if (!__ONE) {
        printf("init one");
        int uno = 1;
        __ONE = con(&uno, INTTYPE);
    }
        printf("ret one\n");

    return __ONE;
}

// [dst][src]
char coercion_table[3][3] = {
    // I, R, B
    {1,0,0}, // I
    {1,1,0}, // R
    {1,0,1}, // B
};

// Cast if we can
conNodeType * coercion(conNodeType * a, varTypeEnum req) {
    if (!a) return NULL;

    if (a->type == req) return a; // easy case

    if (!coercion_table[req][a->type]) yyerror("Incompatible type (coercion failed).");

    conNodeType * n = a;
    switch (req) {
        case INTTYPE:
            switch (n->type) {
                case INTTYPE: break;
                case REALTYPE: n->i = (int)n->r; break;
                case BOOLTYPE: n->i = (int)n->b; break;
            }
            break;
        case REALTYPE:
            switch (n->type) {
                case INTTYPE:  n->r = (float)n->i; break;
                case REALTYPE: break;
                case BOOLTYPE: n->r = (float)n->b; break;
            }
            break;

        case BOOLTYPE:
            switch (n->type) {
                case INTTYPE:  n->b = n->i != 0; break;
                case REALTYPE: n->b = n->r != 0; break;
                case BOOLTYPE: break;
            }
            break;
    }
    a->type = req;
    return a;
}


/* Execute a subtree.
 * Return node evaluation.
 */
ret * ex(nodeType *p) {
    // Early exit on EOTree.
    if (!p){
        printf("[DEBUG] p is null\n");
        return 0;
    }

    printf("[DEBUG]\t\tp->type: %d\n", p->type);
    switch(p->type) {
        case nodeDic:
            if (getsym(p->dic.name)) {
                fprintf(stderr, "Variable %s was previously declared.", p->dic.name);
                exit(1);
            }
            putsym(p->dic.name, p->dic.type);
            return 0;

        case nodeCon:
            {
                ret * r = xmalloc(sizeof(ret));
                memcpy(r, &(p->con), sizeof(ret));
                return r;
            }

        case nodeId:
            {
                symrec * s = getsym(p->id.name);
                if(s == NULL){
                    fprintf(stderr, "There is not such '%s' variable in the symtable\n", p->id.name);
                    exit(1);
                }

                ret * r = xmalloc(sizeof(ret));
                r->type = s->type;
                switch(s->type){
                    case INTTYPE:
                        r->i = s->i;
                        break;
                    case REALTYPE:
                        r->r = s->r;
                        break;
                    case BOOLTYPE:
                        r->b = s->b;
                        break;
                    default:
                        yyerror("Unrecognized type.");
                }

                return r;
            }

        case nodeOpr:
            printf("[DEBUG] operator %d\n", p->opr.oper);
            {
                // Used for expr
                mappable f = NULL;
                ret * a = NULL , * b = NULL;
                int mod = 0;

                switch(p->opr.oper) {
                    case WHILE:
                        {
                            while(coercion(ex(p->opr.op[0]), BOOLTYPE)->b)
                                ex(p->opr.op[1]);
                            return 0;
                        }

                    case FOR:
                        {
                            /*
                            * 0: var
                            * 1: initial value
                            * 2: upper boundary
                            * 3: body
                            */

                            ret * c;

                            ex(opr(EQ, 2, p->opr.op[0], p->opr.op[1]));
                            // iterator < boundary
                            while(coercion(ex(opr(LT, 2, p->opr.op[0], p->opr.op[2])), BOOLTYPE)->b) {
                                // exec
                                ex(p->opr.op[3]);

                                // Please use singleton for con too
                                ex(opr(EQ, 2,
                                    p->opr.op[0],
                                    con(&(*ex(opr(PLUS, 2,
                                        p->opr.op[0],
                                        ONE()))).i,
                                        INTTYPE
                                    )
                                ));
                            }

                            return 0;
                        }

                    case IF:
                        {
                            if(coercion(ex(p->opr.op[0]), BOOLTYPE)->b)
                                ex(p->opr.op[1]); // IF
                            else if (p->opr.nops > 2)
                                ex(p->opr.op[2]); // ELSE (if any)
                            return 0;
                        }

                    case PRINTINT:
                    case PRINTREAL:
                    case PRINTBOOL:

                    case PRINT: // HERE NO COERCION !
                        {
                            int cmd = p->opr.oper;
                            ret * to_print = ex(p->opr.op[0]);

                            switch(to_print->type){
                                case INTTYPE:
                                    if (cmd != PRINT && cmd != PRINTINT) yyerror("Type error.");
                                    printf("%d\n", to_print->i);
                                    break;
                                case REALTYPE:
                                    if (cmd != PRINT && cmd != PRINTREAL) yyerror("Type error.");
                                    {
                                        char * fstr = (char*)xmalloc(46 + 1); // len(print(FLT_MAX);
                                        sprintf(fstr, "%f", to_print->r);

                                        // substitute comma with dot
                                        char * c = fstr;
                                        for(; *c != '.'; c++);
                                        *c = ',';
                                        printf("%s\n", fstr);

                                        free(fstr);
                                    }
                                        break;

                                case BOOLTYPE:
                                    if (cmd != PRINT && cmd != PRINTBOOL) yyerror("Type error.");
                                    if (to_print->b)
                                        printf("true\n");
                                    else
                                        printf("false\n");
                                    break;
                                default:
                                    yyerror("Unrecognized type.");
                            }
                            return 0;
                        }

                    case SEMICOLON:
                        ex(p->opr.op[0]);
                        return ex(p->opr.op[1]);

                    case EQ:
                        {
                            symrec * s = getsym(p->opr.op[0]->id.name);
                            if(s == NULL){
                                fprintf(stderr, "There is not such '%s' varibale in the symtable\n", p->opr.op[0]->id.name);
                                exit(1);
                            }

                            ret * n = ex(p->opr.op[1]);

                            if (!coercion_table[s->type][n->type])
                                yyerror("Incompatible type assignment.");
                            switch (s->type) {
                                case INTTYPE:
                                    switch (n->type) {
                                        case INTTYPE: break;
                                        case REALTYPE: n->i = (int)n->r; break;
                                        case BOOLTYPE: n->i = (int)n->b; break;
                                    }
                                    break;
                                case REALTYPE:
                                    switch (n->type) {
                                        case INTTYPE:  n->r = (float)n->i; break;
                                        case REALTYPE: break;
                                        case BOOLTYPE: n->r = (float)n->b; break;
                                    }
                                    break;

                                case BOOLTYPE:
                                    switch (n->type) {
                                        case INTTYPE:  n->b = n->i != 0; break;
                                        case REALTYPE: n->b = n->r != 0; break;
                                        case BOOLTYPE: break;
                                    }
                                    break;
                            }
                            n->type = s->type;
                            return n;
                        }

                    case UMINUS:
                        {
                            ret * a = ex(p->opr.op[0]);
                            coercion(a, min(a->type, INTTYPE));
                            return apply(&neg, a, NULL, a->type);
                        }

                    case PLUS: f = f != NULL ? f : &sum;
                    case MIN:  f = f != NULL ? f : &min;
                    case MUL:  f = f != NULL ? f : &mul;
                    case DIV:  f = f != NULL ? f : &dvi;
                        mod = 1; // number -> number

                    case LT:   f = f != NULL ? f : &lt;
                    case GT:   f = f != NULL ? f : &gt;
                    case GTE:  f = f != NULL ? f : &gte;
                    case LTE:  f = f != NULL ? f : &lte;
                        mod = mod ? mod : 2; // number -> bool
                    case AND: f = f != NULL ? f : &and;
                    case OR:  f = f != NULL ? f : &or;
                    case NOT: f = f != NULL ? f : &not;
                        mod = mod ? mod : 3; // bool -> bool
                    case NE:  f = f != NULL ? f : &neq;
                    case DEQ: f = f != NULL ? f : &deq;
                        a = ex(p->opr.op[0]);
                        b = ex(p->opr.op[1]);

                        int error = 0;
                        varTypeEnum dstType = BOOLTYPE;

                        switch (mod) {
                            case 1: // number -> number
                                dstType = max(a->type, b->type);
                            case 2: // number -> bool
                                error = a->type == BOOLTYPE;
                                break;
                            case 3: // bool -> bool
                                error = a->type != BOOLTYPE;
                        }
                        if (error || a->type != b->type)
                                yyerror("Incompatible type");

                        coercion(a, dstType);
                        coercion(b, dstType);
                        return apply(f, a, b, dstType);

                    default:
                        yyerror("Operator not matched.");
                }
                break;
            }
        default:
            yyerror("Node was not matched\n");
    }

    yyerror("[WTF] This should be DEAD CODE.");
    return 0;
}

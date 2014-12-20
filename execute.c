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

            switch(p->opr.oper) {
                case WHILE:
                    while((*ex(p->opr.op[0])).b)
                        ex(p->opr.op[1]);
                    return 0;

                case FOR:
                    {
                        /*
                         * 0: var
                         * 1: initial value
                         * 2: upper boundary
                         * 3: body
                         */
                        ex(opr(EQ, 2, p->opr.op[0], p->opr.op[1]));
                        // iterator < boundary
                        while((*ex(opr(LT, 2, p->opr.op[0], p->opr.op[2]))).b) {
                            // exec
                            ex(p->opr.op[3]);

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
                    if ((*ex(p->opr.op[0])).b)
                        ex(p->opr.op[1]); // IF
                    else if (p->opr.nops > 2)
                        ex(p->opr.op[2]); // ELSE (if any)
                    return 0;

                case PRINT:
                    {
                        ret * to_print = ex(p->opr.op[0]);

                        switch(to_print->type){
                            case INTTYPE:
                                printf("%d\n", to_print->i);
                                break;
                            case REALTYPE:
                                printf("%f\n", to_print->r);
                                break;
                            case BOOLTYPE:
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

                        ret * val = ex(p->opr.op[1]);

                        // TODO: INTRODUCE HERE coercion & type checking

                        printf("[DEBUG]\t\t\tseitch oever type\n");
                        s->type = val->type;
                        switch(val->type){
                            case INTTYPE:
                                s->i = val->i;
                                break;
                            case REALTYPE:
                                s->r = val->r;
                                break;
                            case BOOLTYPE:
                                s->b = val->b;
                                break;
                            default:
                                yyerror("Unrecognized type.");
                        }

                        return val;
                    }

                // TODO: introduce here coercion
                case UMINUS:
                    {
                        ret * a = ex(p->opr.op[0]);
                        return apply(&neg, a, NULL, a->type);
                    }

                case PLUS:
                    {
                        ret * a = ex(p->opr.op[0]);
                        ret * b = ex(p->opr.op[1]);

                        // TODO: add here type checking
                        varTypeEnum dstType = max(a->type, b->type);

                        return apply(&sum, a, b, dstType);
                    }

                case MIN:
                    {
                        ret * a = ex(p->opr.op[0]);
                        ret * b = ex(p->opr.op[1]);

                        // TODO: add here type checking
                        varTypeEnum dstType = max(a->type, b->type);

                        return apply(&min, a, b, dstType);
                    }

                case MUL:
                    {
                        ret * a = ex(p->opr.op[0]);
                        ret * b = ex(p->opr.op[1]);

                        // TODO: add here type checking
                        varTypeEnum dstType = max(a->type, b->type);

                        return apply(&mul, a, b, dstType);
                    }

                case DIV:
                    {
                        ret * a = ex(p->opr.op[0]);
                        ret * b = ex(p->opr.op[1]);

                        // TODO: add here type checking
                        varTypeEnum dstType = max(a->type, b->type);

                        return apply(&dvi, a, b, dstType);
                    }

                case LT:
                    {
                        ret * a = ex(p->opr.op[0]);
                        ret * b = ex(p->opr.op[1]);

                        // TODO: add here type checking

                        return apply(&lt, a, b, BOOLTYPE);
                    }

                case GT:
                    {
                        ret * a = ex(p->opr.op[0]);
                        ret * b = ex(p->opr.op[1]);

                        // TODO: add here type checking

                        return apply(&gt, a, b, BOOLTYPE);
                    }

                case GTE:
                    {
                        ret * a = ex(p->opr.op[0]);
                        ret * b = ex(p->opr.op[1]);

                        // TODO: add here type checking

                        return apply(&gte, a, b, BOOLTYPE);
                    }

                case LTE:
                    {
                        ret * a = ex(p->opr.op[0]);
                        ret * b = ex(p->opr.op[1]);

                        // TODO: add here type checking

                        return apply(&lte, a, b, BOOLTYPE);
                    }

                case NE:
                    {
                        ret * a = ex(p->opr.op[0]);
                        ret * b = ex(p->opr.op[1]);

                        // TODO: add here type checking
                        varTypeEnum dstType = max(a->type, b->type);

                        return apply(&neq, a, b, BOOLTYPE);
                    }

                case DEQ:
                    {
                        ret * a = ex(p->opr.op[0]);
                        ret * b = ex(p->opr.op[1]);

                        // TODO: add here type checking

                        return apply(&deq, a, b, BOOLTYPE);
                    }

                 case AND:{
                        ret* a = ex(p->opr.op[0]);
                        ret* b = ex(p->opr.op[1]);

                        // TODO: add here type checking

                        return apply(&and, a, b, BOOLTYPE);
                 }

                 case OR:{
                        ret* a = ex(p->opr.op[0]);
                        ret* b = ex(p->opr.op[1]);

                        // TODO: add here type checking

                        return apply(&or, a, b, BOOLTYPE);
                 }

                 case NOT:{
                        ret* a = ex(p->opr.op[0]);

                        // TODO: add here type checking

                        return apply(&not, a, NULL, BOOLTYPE);
                 }

                default:
                    fprintf(stderr, "Operator not matched\n");
                    exit(1);
            }
            break;

        default:
            fprintf(stderr, "Node was not matched\n");
            exit(1);
    }

    yyerror("[WTF] This should be DEAD CODE.");
    return 0;
}

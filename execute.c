#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "interpreter.h"
#include "functional.h"
#include "y.tab.h"

// Hold ex() return value.
// NULL if no return value (eg declaration, assignment).
typedef conNodeType ret;

nodeType * _ONE = NULL;
nodeType * ONE() {
    if (_ONE != NULL) return _ONE;
    _ONE = (nodeType*)xmalloc(sizeof(nodeType));

    _ONE->type = nodeCon;
    _ONE->con = *(conNodeType *)xmalloc(sizeof(conNodeType));
    _ONE->con.i = 1;
    _ONE->con.r = 1.0;
    _ONE->con.b = 1;

    return _ONE;
}


/* Execute a subtree.
 * Return node evaluation.
 */
ret * ex(nodeType *p) {
    // Early exit on EOTree.
    printf("[DEBUG] p is null\n");
    if (!p) return 0;

    printf("[DEBUG] p->type: %d\n", p->type);
    switch(p->type) {
        case nodeDic:
            printf("[DEBUG]\tEval dic for: %s\n", p->dic.name);
            putsym(p->dic.name, p->dic.type);
            return 0;

        case nodeCon:
            printf("[DEBUG]\tEval node con\n");
            {
                ret * r = xmalloc(sizeof(ret));
                memcpy(r, &(p->con), sizeof(ret));
                return r;
            }

        case nodeId:
            printf("[DEBUG]\tEval node id\n");
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
                    while(ex(p->opr.op[0]))
                        ex(p->opr.op[1]);
                    return NULL;

                case FOR:
                    {
                        /* 0: var
                         * 1: initial value
                         * 2: upper boundary
                         * 3: body
                         */
                        // var = exp;
                        // TODO: test me
                        // symrec * s = getsym(p->opr.op[0]->id.name);
                        //ex(opr(EQ, 2, id(s), p->opr.op[1]));
                        ex(opr(EQ, 2, p->opr.op[0], p->opr.op[1]));

                        // iterator < boundary
                        while(ex(opr(LTE, 2, p->opr.op[0], p->opr.op[2]))){
                            // exec
                            ex(p->opr.op[3]);
                            // inc
                            ex(opr(PLUS, 2, p->opr.op[0], ONE()));
                        }
                        return 0;
                    }

                case IF:
                    if (ex(p->opr.op[0]))
                        ex(p->opr.op[1]); // IF
                    else if (p->opr.nops > 2)
                        ex(p->opr.op[2]); // ELSE (if any)
                    return 0;


                case PRINT:
                    printf("[DEBUG] printf");
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
                    printf("[DEBUG]\t\tsemicolon\n");
                    ex(p->opr.op[0]);
                    return ex(p->opr.op[1]);

                case EQ:
                    {
                        symrec * s = getsym(p->opr.op[0]->id.name);
                        if(s == NULL){
                            fprintf(stderr, "There is not such '%s' varibale in the symtable\n", p->opr.op[0]->id.name);
                            exit(0);
                        }

                        ret * val = ex(p->opr.op[1]);

                        // TODO: INTRODUCE HERE coercion & type checking

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
                        varTypeEnum dstType = max(a->type, b->type);

                        return apply(&lt, a, b, dstType);
                    }

                case GT:
                    {
                        ret * a = ex(p->opr.op[0]);
                        ret * b = ex(p->opr.op[1]);

                        // TODO: add here type checking
                        varTypeEnum dstType = max(a->type, b->type);

                        return apply(&gt, a, b, dstType);
                    }

                case GTE:
                    {
                        ret * a = ex(p->opr.op[0]);
                        ret * b = ex(p->opr.op[1]);

                        // TODO: add here type checking
                        varTypeEnum dstType = max(a->type, b->type);

                        return apply(&gte, a, b, dstType);
                    }

                case LTE:
                    {
                        ret * a = ex(p->opr.op[0]);
                        ret * b = ex(p->opr.op[1]);

                        // TODO: add here type checking
                        varTypeEnum dstType = max(a->type, b->type);

                        return apply(&lte, a, b, dstType);
                    }

                case NE:
                    {
                        ret * a = ex(p->opr.op[0]);
                        ret * b = ex(p->opr.op[1]);

                        // TODO: add here type checking
                        varTypeEnum dstType = max(a->type, b->type);

                        return apply(&neq, a, b, dstType);
                    }

                case DEQ:
                    {
                        ret * a = ex(p->opr.op[0]);
                        ret * b = ex(p->opr.op[1]);

                        // TODO: add here type checking
                        varTypeEnum dstType = max(a->type, b->type);

                        return apply(&deq, a, b, dstType);
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

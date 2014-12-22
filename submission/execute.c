/* Author: Brugnara Martin #157791 */

/* This file contains the function that really execute the code: ex()
 *
 * This file could be considered similar to 5.2/calcInterpreter.c
 * But this is completely rewrote.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "interpreter.h"
#include "helper.h"
#include "functional.h"
#include "coercion.h"
#include "y.tab.h"

// Hold ex() return value.
// Like in func return value are 'constant' (we do not have pointer);
// NULL if no return value (eg declaration, assignment).
typedef conNodeType ret;

// used for ++/-- operations
// Singleton | cache
nodeType * __ONE = 0;
nodeType * ONE() {
    if (!__ONE) {
        __ONE = con(1, INTTYPE);
    }
    return __ONE;
}


/* Execute a tree.
 * return conNodeType */
symrec * EBP = NULL;
ret * ex(nodeType *p) {
    // Early exit on EOTree.
    if (!p) return 0;

    switch(p->type) {
        case nodeBlock:
            // This is support for scoping.
            /* NOTE: if function implementation is needed
            // just store dic_list and run it after saved EBP
            */
            {
                symrec * bk_EBP = EBP; // Backup EBP
                EBP = symTable; // new EBP from ESP

                // IMPROVEMENT: if func: ex(dec_list)
                ex(p->blk);

                symTable = EBP; // Reset ESP
                EBP = bk_EBP; // Reset EBP
                return 0;
            }
        case nodeDic:
            if (isdefsym(p->dic.name, EBP)) {
                fprintf(stderr, "[ERROR] Variable %s was previously declared.\n", p->dic.name);
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
                if(!s) {
                    fprintf(stderr, "[ERROR] There is not such '%s' variable in the symtable\n", p->id.name);
                    exit(1);
                }
                return ex(con(s->value, s->type));
            }

        case nodeOpr:
            {
                // Used for expr
                mappable f = NULL;
                ret * a = NULL , * b = NULL;
                int flag = 0;

                switch(p->opr.oper) {
                    case WHILE:
                        while(coercion(ex(p->opr.op[0]), BOOLTYPE)->value)
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

                            ret * c;

                            symrec * s = getsym(p->opr.op[0]->id.name);

                            // DO first assign via opr in order to ensure
                            // type checking and coercion.
                            ex(opr(EQ, 2, p->opr.op[0], p->opr.op[1]));

                            // iterator < boundary
                            while(coercion(ex(opr(LT, 2, p->opr.op[0], p->opr.op[2])), BOOLTYPE)->value) {
                                // exec
                                ex(p->opr.op[3]);

                                // Speed up (no need for other checks,
                                // respect ev. scope;
                                s->value += 1;
                            }

                            return 0;
                        }

                    case IF:
                        {
                            if(coercion(ex(p->opr.op[0]), BOOLTYPE)->value)
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
                                    printf("%d\n", (int)(to_print->value));
                                    break;
                                case REALTYPE:
                                    if (cmd != PRINT && cmd != PRINTREAL) yyerror("Type error.");
                                    {
                                        char * fstr = (char*)xmalloc(46 + 1); // len(print(FLT_MAX);
                                        sprintf(fstr, "%f", to_print->value);

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
                                    if (to_print->value)
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
                                fprintf(stderr, "[ERROR] There is not such '%s' varibale in the symtable\n", p->opr.op[0]->id.name);
                                exit(1);
                            }

                            ret * val = coercion(ex(p->opr.op[1]), s->type);
                            s->value = val->value;
                            return 0;
                        }


                    case UMINUS: f = f != NULL ? f : &neg;
                    case PLUS: f = f != NULL ? f : &sum;
                    case MIN:  f = f != NULL ? f : &mni;
                    case MUL:  f = f != NULL ? f : &mul;
                    case DIV:  f = f != NULL ? f : &dvi;
                        flag = 3;

                    case LT:   f = f != NULL ? f : &lt;
                    case GT:   f = f != NULL ? f : &gt;
                    case GTE:  f = f != NULL ? f : &gte;
                    case LTE:  f = f != NULL ? f : &lte;
                        flag = max(flag, 2);

                    case NE:  f = f != NULL ? f : &neq;
                    case DEQ: f = f != NULL ? f : &deq;
                        flag = max(flag, 1);

                    case AND: f = f != NULL ? f : &and;
                    case OR:  f = f != NULL ? f : &or;
                    case NOT: f = f != NULL ? f : &not;
                        flag = max(flag, 0);
                        {
                            varTypeEnum
                                retType = BOOLTYPE,
                                valType = BOOLTYPE;

                            a = ex(p->opr.op[0]);
                            b = p->opr.nops == 2 ? ex(p->opr.op[1]) : NULL;

                            switch (flag) {
                                case 3:
                                    valType = retType = max(b ? max(a->type, b->type) : a->type, INTTYPE);
                                    break;
                                case 2:
                                    valType = max(b ? max(a->type, b->type) : a->type, INTTYPE);
                                    break;
                                case 1:
                                    valType = b ? max(a->type, b->type) : a->type;
                                    break;
                            }

                            return ex(con((*f)(
                                coercion(a, valType)->value,
                                b ? coercion(b, valType)->value : 0), retType));
                        }
                    default:
                        yyerror("Operator not matched.");
                }
                break;
            }
        default:
            yyerror("Node was not matched.");
    }

    yyerror("WTF! This should be DEAD CODE.");
    return 0;
}

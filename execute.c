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
        int uno = 1;
        __ONE = con(&uno, INTTYPE);
    }
    return __ONE;
}

// [dst][src]
char coercion_table[3][3] = {
    // B, I, R
      {1, 1, 0}, // B
      {0, 1, 0}, // I
      {0, 1, 1}, // R
};

// Cast if we can
conNodeType * coercion(conNodeType * a, varTypeEnum req) {
    if (!a) return NULL;

    if (a->type == req) return a; // easy case

    if (!coercion_table[req][a->type]) yyerror("Incompatible type.");

    switch (req) {
        case INTTYPE:
            {
                int tmp;
                switch (a->type) {
                    case INTTYPE:   tmp = (int)a->i; break;
                    case REALTYPE:  tmp = (int)a->r; break;
                    case BOOLTYPE:  tmp = (int)a->b; break;
                }
                a->i = tmp;
                break;
            }
        case REALTYPE:
            {
                float tmp;
                switch (a->type) {
                    case INTTYPE:  tmp = (float)a->i; break;
                    case REALTYPE: tmp = (float)a->r; break;
                    case BOOLTYPE: tmp = (float)a->b; break;
                }
                a->r = tmp;
                break;
            }
        case BOOLTYPE:
            {
                int tmp;
                switch (a->type) {
                    case INTTYPE:  tmp = a->i != 0; break;
                    case REALTYPE: tmp = a->r != 0; break;
                    case BOOLTYPE: tmp = a->b != 0; break;
                }
                a->b = tmp;
                break;
            }
    }
    a->type = req;
    return a;
}


/* Execute a subtree.
 * Return node evaluation.
 */
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
            { // Language Semantic Course rocks!
                symrec * bk_EBP = EBP; // Backup EBP
                EBP = symTable; // new EBP from ESP
                // IMPROVEMENT: if func: ex(dec_list)
                ex(p->blk);
                symTable = EBP; // Reset ESP
                EBP = bk_EBP; // Reset EBP
            }
        case nodeDic:
            if (isdefsym(p->dic.name, EBP)) {
                fprintf(stderr, "[ERROR] Variable %s was previously declared.", p->dic.name);
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
                    fprintf(stderr, "[ERROR] There is not such '%s' variable in the symtable\n", p->id.name);
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
            {
                // Used for expr
                mappable f = NULL;
                ret * a = NULL , * b = NULL;
                int flag = 0;

                switch(p->opr.oper) {
                    case WHILE:
                        while(coercion(ex(p->opr.op[0]), BOOLTYPE)->b)
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
                                fprintf(stderr, "[ERROR] There is not such '%s' varibale in the symtable\n", p->opr.op[0]->id.name);
                                exit(1);
                            }

                            ret * n = ex(p->opr.op[1]);

                            // real = int
                            if (!coercion_table[s->type][n->type])
                                yyerror("Incompatible type assignment.");

                            switch (s->type) {
                                case INTTYPE:
                                    switch (n->type) {
                                        case INTTYPE:  s->i = n->i; break;
                                        case REALTYPE: s->i = (int)n->r; break;
                                        case BOOLTYPE: s->i = (int)n->b; break;
                                    }
                                    break;
                                case REALTYPE:
                                    switch (n->type) {
                                        case INTTYPE:  s->r = (float)n->i; break;
                                        case REALTYPE: s->r = n->r; break;
                                        case BOOLTYPE: s->r = (float)n->b; break;
                                    }
                                    break;

                                case BOOLTYPE:
                                    switch (n->type) {
                                        case INTTYPE:  s->b = n->i != 0; break;
                                        case REALTYPE: s->b = n->r != 0; break;
                                        case BOOLTYPE: s->b = n->b; break;
                                    }
                                    break;
                            }
                            return n;
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

                            return apply(
                                    f,
                                    coercion(a, valType),
                                    coercion(b, valType),
                                    retType
                                    );
                        }
                    default:
                        yyerror("Operator not matched.");
                }
                break;
            }
        default:
            yyerror("Node was not matched.");
    }

    yyerror("[WTF] This should be DEAD CODE.");
    return 0;
}

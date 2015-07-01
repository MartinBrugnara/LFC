# Author: Brugnara Martin #157791

LEX = lex
YACC = yacc -d

CC = clang -std=c99

interpreter: y.tab.o lex.yy.o coercion.o functional.o helper.o interpreter.o execute.o
	$(CC) -o interpreter helper.o y.tab.o lex.yy.o coercion.o functional.o interpreter.o execute.o -ll -lm

coercion.o: coercion.c
functional.o: functional.c
helper.o: helper.c
interpreter.o: interpreter.c
execute.o: execute.c

lex.yy.o: lex.yy.c y.tab.h

y.tab.c y.tab.h: yacc.y
	$(YACC) -v yacc.y

lex.yy.c: lexer.l
	$(LEX) lexer.l

clean:
	-rm -f *.o lex.yy.c *.tab.*  interpreter *.output

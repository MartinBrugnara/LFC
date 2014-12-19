
LEX = lex
YACC = yacc -d


CC = cc -std=c99

interpreter: y.tab.o lex.yy.o interpreter.o execute.o functional.o
	$(CC) -o interpreter y.tab.o lex.yy.o interpreter.o execute.o functional.o -ll -lm

interpreter.o: interpreter.c

execute.o: execute.c

functional.o: functional.c

lex.yy.o: lex.yy.c y.tab.h

y.tab.c y.tab.h: yacc.y
	$(YACC) -v yacc.y

lex.yy.c: lexer.l
	$(LEX) lexer.l

clean:
	-rm -f *.o lex.yy.c *.tab.*  interpreter *.output

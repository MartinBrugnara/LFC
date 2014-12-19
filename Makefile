
LEX = lex
YACC = yacc -d


CC = cc -std=c99

interpreter: y.tab.o lex.yy.o interpreter.o
	$(CC) -o interpreter y.tab.o lex.yy.o interpreter.o -ll -lm

interpreter.o: interpreter.c


lex.yy.o: lex.yy.c y.tab.h

y.tab.c y.tab.h: calc.y
	$(YACC) -v yacc.y

lex.yy.c: calc.l
	$(LEX) lexer.l

clean:
	-rm -f *.o lex.yy.c *.tab.*  interpreter *.output

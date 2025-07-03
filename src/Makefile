main: lex.yy.c bison.tab.c
	gcc lex.yy.c bison.tab.c src/tree.c -o main -w

lex.yy.c: bison.tab.c
	lex -l src/lex.l

bison.tab.c:
	bison -d src/bison.y --debug

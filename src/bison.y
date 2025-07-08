%{ 
    #include <stdio.h>
    #include "tree.h"
    #include <stdbool.h>
    #include <stdarg.h>
	#include <stdlib.h>

    int yylex(void);
    extern int yyparse();
    extern FILE *yyin;
    extern char* yytext;

    int yyerror(char *s);
%}

%union {
    long num;
    bool boollit;
    char *op;
    char *id;
    ast_node *ast;
}

%token <num> num
%token <boollit> boollit
%token <id> ident
%token <op> OP2 OP3 OP4
%token LP RP ASGN SC IF ELSE THEN
%token BEGN END WHILE DO PROGRAM VAR AS INT BOOL WRITEINT READINT WHITESPACE
%token DECL STMT_SEQ PARENTHESES

%type <ast> program declarations statement statementsequence
%type <ast> assignment ifstatement writeint
%type <ast> elseclause whilestatement expression
%type <ast> simpleexpression term factor type

%start program

%%

program: 
    PROGRAM declarations BEGN statementsequence END 
    { 
        $$ = node(PROGRAM, 3, $2, $4); 
        gen_code($$);
    }
;

declarations:
    VAR ident AS type SC declarations 
    { $$ = node(DECL, 3, id($2), $4, $6); }
    | 
    { $$ = NULL; }
;

type:
    INT 
    { $$ = node(INT, 0); }
    | BOOL 
    { $$ = node(BOOL, 0); }
;

statementsequence:
    statement SC statementsequence 
    { $$ = node(STMT_SEQ, 2, $1, $3); }
    | 
    { $$ = NULL; }
;

statement:
    assignment // left blank, aka $$ = $1
    | ifstatement 
    | whilestatement
    | writeint
;

assignment:
    ident ASGN expression 
    { $$ = node(ASGN, 2, id($1), $3); }
    | ident ASGN READINT 
    { $$ = node(ASGN, 2, id($1), node(READINT, 0)); }
;

ifstatement:
    IF expression THEN statementsequence elseclause END
    { $$ = node(IF, 3, $2, $4, $5); }
;

elseclause:
    ELSE statementsequence 
    { $$ = node(ELSE, 1, $2); }
    | 
    { $$ = NULL; }
;

whilestatement:
    WHILE expression DO statementsequence END
    { $$ = node(WHILE, 2, $2, $4); }
;

writeint:
    WRITEINT expression 
    { $$ = node(WRITEINT, 1, $2); }
;

expression:
    simpleexpression
    | simpleexpression OP4 simpleexpression
    { $$ = node(OP4, 3, $1, opr($2), $3); }
;

simpleexpression:
    term OP3 term 
    { $$ = node(OP3, 3, $1, opr($2), $3); }
    | term
;

term:
    factor OP2 factor
    { $$ = node(OP2, 2, $1, opr($2), $3); }
    | factor 
;

factor:
    ident 
    { $$ = id($1); }
    | num { 
        if ($1 < 0 || $1 > 2147483647) {
            yyerror("Error: num out of range");
            exit(1);
        }
        $$ = constant($1); 
    }
    | boollit {
        if ($1 != false || $1 != true) {
            yyerror("Error: boollit out of range");
            exit(1);
        } 
        $$ = constant($1); }
    | LP expression RP { $$ = node(PARENTHESES, 1, $2); }
;


%%

int yyerror(char *s) {
		printf("yyerror : %s\n",s);
}

int main(int argc, char** argv) {

		if (argc != 2) {
				printf("Please specify an input file\n");
				exit(1);
		}
		FILE *myfile = fopen(argv[1], "r");

		if (!myfile) return -1;

		// Set Flex to read from the file instead of defaulting to STDIN
		yyin = myfile;

		// Parse through the input
		if (!yyparse()) return 0;

		return 1;

}

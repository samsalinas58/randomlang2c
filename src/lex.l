%{
    #include "tree.h"
    #include "bison.tab.h"
%}

%option noyywrap

num         [1-9][0-9]*|0
boollit     false|true
ident       [A-Z][A-Z0-9]*
LP          "("
RP          ")"
ASGN        ":="
SC          ";"
OP2         "*"|div|mod
OP3         "+"|"-"
OP4         "="|"!="|"<"|">"|"<="|">="
IF          if
ELSE        else
THEN        then
BEGN        begin
END         end
WHILE       while
DO          do
PROGRAM     program
VAR         var
AS          as
INT         int
BOOL        bool
WRITEINT    writeInt
READINT     readInt
WHITESPACE  " "

%%
{num}   {
        char *str = yytext;
        char *ptr;
        long ret = strtol(str, &ptr, 10);
        yylval.num = ret;
        return(num);
        }

{boollit}   {
        if (strcmp(yytext, "false") == 0) yylval.boollit = false;
        else if (strcmp(yytext, "true") == 0) yylval.boollit = true;
        return(boollit);
    }

{ident} {
        if ((yylval.id = strdup(yytext)) == NULL) perror("failed strdup()");
        return(ident);
    }

{LP} {return(LP);}

{RP} {return(RP);}

{ASGN} {return(ASGN);}

{SC} {return(SC);}

{OP2} {
    if ((yylval.op = strdup(yytext)) == NULL) perror("failed strdup()");
    return(OP2);
    }

{OP3} {
    if ((yylval.op = strdup(yytext)) == NULL) perror("failed strdup()");
    return(OP3);
    }

{OP4} {
    if ((yylval.op = strdup(yytext)) == NULL) perror(NULL);
    return(OP4);
    }

{IF} {return(IF);}

{THEN} {return(THEN);}

{ELSE} {return(ELSE);}

{BEGN} {return(BEGN);}

{END} {return(END);}

{WHILE} {return(WHILE);}

{DO} {return(DO);}

{PROGRAM} {return(PROGRAM);}

{VAR} {return(VAR);}

{AS} {return(AS);}

{INT} {return(INT);}

{BOOL} {return(BOOL);}

{WRITEINT} {return(WRITEINT);}

{READINT} {return(READINT);}

{WHITESPACE}|.|\n { /* empty */ }
%%

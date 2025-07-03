#!/bin/bash

bison  -d bison.y --debug &&
lex -l lex.l &&
gcc lex.yy.c bison.tab.c tree.c -o $1 -w

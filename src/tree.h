#include <stdbool.h>

#ifndef TREE_H
#define TREE_H

typedef struct ast_node ast_node;
typedef struct ast_node_ptr ast_node_ptr;
typedef struct op_node op_node;
typedef struct con_node con_node;
typedef struct id_node id_node;

typedef enum nodeEnum { typeCon, typeId, typeOp, typeNode } nodeEnum;

typedef struct con_node {
	int value;
} con_node;

typedef struct id_node {
	char *id;
} id_node;

typedef struct op_node {
    char *op;
} op_node;

typedef struct ast_node_ptr {
    int token;
    int num_c;
    ast_node **ch;
} ast_node_ptr;

typedef struct ast_node {
	nodeEnum type;
    union {
        con_node con;
        id_node id;
        op_node opr;
        ast_node_ptr a; 
    };
} ast_node;

ast_node *node(int oper, int nops, ...);
ast_node *id(char *id);
ast_node *opr(char* op);
ast_node *constant(int val);
void gen_code(ast_node *prgrm);
void _header();
void prgrm(ast_node *root);
void _declarations(ast_node *decl);
void _statement_sequence(ast_node *decl);
void _statement(ast_node *stmt);
void _if(ast_node *if_stmt);
void _else(ast_node *else_cl);
void _while(ast_node *while_stmt);
void _writeint(ast_node *writeint);
char* _assignment(ast_node *asgn);
char* _expression(ast_node *expr);
char* _simple_expression(ast_node *simple_expr);
char* _term(ast_node *term);
char* _factor(ast_node *factor);
void _insert(char* s);
void _print_code_buffer();


#endif
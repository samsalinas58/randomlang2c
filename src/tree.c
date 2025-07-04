
#include "tree.h"
#include "bison.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

extern int yyerror(char* s);

size_t sz = 0;
size_t cap = 1;
char **code;
int depth = 0;

ast_node *node(int token, int num_c, ...) {
	va_list ap;
	ast_node *p;
	size_t size;
    size_t c_size;

	size = sizeof(ast_node);
    c_size = num_c * sizeof(ast_node*);
	if ((p = malloc(size)) == NULL) yyerror("out of memory");
    if ((p->a.ch = malloc(c_size)) == NULL) yyerror("out of memory");

	p->type = typeNode;
	p->a.token = token;
	p->a.num_c = num_c;

	va_start(ap, num_c);
	int i;
	for (i = 0; i < num_c; i++) p->a.ch[i] = va_arg(ap, ast_node*);
	va_end(ap);

	return p;
}

ast_node *id(char *id){
    ast_node *p;
    size_t ast_node_size = sizeof(ast_node);

    if ((p = malloc(ast_node_size)) == NULL) yyerror("out of memory");
    if ((p->id.id = strdup(id)) == NULL) yyerror("out of memory");

    p->type = typeId;

    return p;
}

ast_node *opr(char *op){
    ast_node *p;
    size_t ast_node_size = sizeof(ast_node);
    size_t id_size = strlen(op) + 1;

    if ((p = malloc(ast_node_size)) == NULL) yyerror("out of memory");
    if ((p->opr.op = strdup(op)) == NULL) yyerror("failed to call strdup() in opr"); 

    p->type = typeOp;

    return p;
}

ast_node *constant(int val){
    ast_node *p;

    if ((p = malloc(sizeof(ast_node))) == NULL) yyerror("out of memory");

    p->type = typeCon;
    p->con.value = val;

    return p;
}
void gen_code(struct ast_node *root){
    _header();
    prgrm(root);
    _insert("}");
    _print_code_buffer();
}

void _header(){
    char *stdlib = "#include <stdlib.h>";
    char *stdio = "#include <stdio.h>";
    char *main = "int main() {";

    code = malloc(sizeof(char*) * cap);
    _insert(stdlib);
    _insert(stdio);
    _insert(main);
}

void prgrm(ast_node *root){
    if (root == NULL) return;

    ast_node *curr = root;
    switch(curr->type) {
        case typeNode: 
            switch (curr->a.token) {
                case PROGRAM:
                    depth += 1; 
                    _declarations(curr->a.ch[0]);
                    _statement_sequence(curr->a.ch[1]);
                    depth -= 1;
                    break;
            }
            break;
        
        default: break;
    }

}


void _declarations(ast_node *decl) {
    if (decl == NULL) return;
    char *id = decl->a.ch[0]->id.id;
    int token = decl->a.ch[1]->a.token;

    char *s;
    switch (token){
        case INT:
            if (asprintf(&s, "int %s;", id) < 0) yyerror("Failed to create string");
            _insert(s);
            break;
        case BOOL: 
            if (asprintf(&s, "bool %s;", id) < 0) yyerror("Failed to create string");
            _insert(s);
            break;
    }
    _declarations(decl->a.ch[2]);
}

void _statement_sequence(ast_node *stmt_seq){
    if (stmt_seq == NULL) return;
    _statement(stmt_seq->a.ch[0]);
    _statement_sequence(stmt_seq->a.ch[1]);
}

void _statement(ast_node *stmt){
    //guaranteed to be typeNode
    if (stmt == NULL) return;
    int token = stmt->a.token;
    switch (token){
        char *s;
        case ASGN:
            s = _assignment(stmt);
            _insert(s);
            break;
        case IF:
            _if(stmt);
            break;
        case WHILE: 
            _while(stmt);
            break;
        case WRITEINT:
            _writeint(stmt);
            break;
        default: break;
    }
    return;
}

void _if(ast_node *if_stmt){
    //only one result possible
    char *s;
    char *expr = _expression(if_stmt->a.ch[0]);
    if (asprintf(&s, "if (%s) {", expr) < 0)
        yyerror("failed asprintf in _if");
    _insert(s);
    depth += 1;
    _statement_sequence(if_stmt->a.ch[1]);
    depth -= 1;
    _insert("}");
    _else(if_stmt->a.ch[2]);
}

void _else(ast_node* else_cl){
    if (else_cl == NULL) return;
    _insert("else {");
    depth += 1;
    _statement_sequence(else_cl->a.ch[0]);
    depth -= 1;
    _insert("}");
}

void _while(ast_node *while_stmt){
    if (while_stmt == NULL) return;
    char *s;
    char *expr = _expression(while_stmt->a.ch[0]);
    if (asprintf(&s, "while (%s) {", expr) < 0)
        yyerror("failed asprintf in _while");
    _insert(s);
    depth += 1;
    _statement_sequence(while_stmt->a.ch[1]);
    depth -= 1;
    _insert("}");
}

void _writeint(ast_node *writeint){
    if (writeint == NULL) return;
    char *s;
    char *expr = _expression(writeint->a.ch[0]);
    char *str = "\"%d\"";
    if (asprintf(&s, "printf(%s, %s);", str, expr) < 0)
        yyerror("Failed asprintf in _writeint");
    _insert(s);
}

char* _assignment(ast_node *asgn){
    if (asgn == NULL) return NULL;
    char *id = asgn->a.ch[0]->id.id;
    switch(asgn->a.ch[1]->a.token){
        char *s, *expr, *str;
        case READINT:
            str = "\"%d\"";
            if (asprintf(&s, "scanf(%s, %s);", str, id) < 0)
                yyerror("Failed readint in _assignment");
            return s;
        default:
            expr = _expression(asgn->a.ch[1]);
            if (asprintf(&s, "%s = %s;", id, expr) < 0) 
                yyerror("Failed expression in _assignment");
            return s;
    }
}

char* _expression(ast_node *expr){
    switch (expr->type){
        case typeNode:
            switch (expr->a.token){
                char *s, *op, *s1, *s2;
                case OP4: 
                    s1 = _simple_expression(expr->a.ch[0]);
                    op = expr->a.ch[1]->opr.op;
                    s2 = _simple_expression(expr->a.ch[2]);
                    if (asprintf(&s, "%s %s %s", s1, op, s2) < 0) 
                        yyerror("Failed asprintf in _expression");
                    return s;
                    break;

                default:
                    return _simple_expression(expr);
            }
            break; 
        default:
            return _simple_expression(expr);
    }
}

char* _simple_expression(ast_node *simple_expr){
    switch (simple_expr->type){
        case typeNode:
            switch (simple_expr->a.token){
                char *s, *op, *s1, *s2;
                case OP3:
                    s1 = _term(simple_expr->a.ch[0]);
                    op = simple_expr->a.ch[1]->opr.op;
                    s2 = _term(simple_expr->a.ch[2]);
                    if (asprintf(&s, "%s %s %s", s1, op, s2) < 0) 
                        yyerror("Failed asprintf in _simple_expression");
                    return s;
                default:
                    return _term(simple_expr);
            }
            break;
        default:
            return _term(simple_expr);
    }
}

char* _term(ast_node *term){
    switch (term->type) {
        case typeNode:
            switch(term->a.token){
                char *s, *op, *s1, *s2;
                case OP2:
                    s1 = _factor(term->a.ch[0]);
                    op = term->a.ch[1]->opr.op;
                    if (strcmp(op, "div") == 0) op = "/";
                    else if (strcmp(op, "mod") == 0) op = "%";
                    s2 = _factor(term->a.ch[2]);
                    if (asprintf(&s, "%s %s %s", s1, op, s2) < 0)
                        yyerror("Failed to create string in _term");
                    return s;
                default:
                    return _factor(term);
            }
            break;
        default:
            return _factor(term);
    }
}

char* _factor(ast_node *factor){
    if (factor == NULL) return NULL;
    switch (factor->type){
        char *s;
        case typeId:
            return factor->id.id;

        case typeCon:
            if (asprintf(&s, "%d", factor->con.value) < 0) 
                yyerror("Failed asprintf in _factor");
            return s;

        case typeNode:

            switch(factor->a.token){
                char *s, *expr;
                case PARENTHESES:
                    expr = _expression(factor->a.ch[0]);
                    if (asprintf(&s, "( %s )", expr) < 0)
                        yyerror("Failed asprintf in _factor");
                    return s;
                default: break;
            }

            break;

        default: 
            break;
    }
}

char* _leftPadTabs(char *s, int size) {
    if (size == 0) return s;
    size_t len = strlen(s);
    int sz = size + len;
    char *temp = realloc(s, sizeof(char) * sz);
    if (temp != NULL) s = temp;
    else printf("Error when reallocating space.\n");

    printf("strlen(s) = %d, sz = %d\n", strlen(s), sz);
    int i;
    len--;
    for (i = sz - 1; len >= 0; i--)
        s[i] = s[len--];
    for (i = 0; i < size; i++) s[i] = '\t';
    printf("s = %s\n", s);
    return s;
}

void _insert(char *s){
    if (++sz >= cap) cap *= 2;
    char **temp = realloc(code, sizeof(char*) * cap);
    if (temp == NULL) yyerror("failed to realloc()");
    else code = temp;
    s = _leftPadTabs(s, depth);
    code[sz - 1] = s;
}

void _print_code_buffer(){
    int i = 0;
    while (code[i] != NULL) printf("%s\n", code[i++]);
}


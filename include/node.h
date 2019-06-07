#ifndef AS3_NODE_H
#define AS3_NODE_H

#include "symbol.h"
#include "token.h"

typedef struct ast_tree ast_tree_t;

struct ast_tree {
  ast_tree_t* left;
  ast_tree_t* right;
  ast_tree_t* condition;            // tree for control_stmt, default is NULL

  ast_tree_t* arglist;              // argument list
  ast_tree_t* stmtlist;             // tree for statement
  ast_tree_t* next;                 // linked list for arglist

  symbol_t* localvar;               // local symbol table

  token_data_t token;
};

ast_tree_t* mknode(token_data_t, ast_tree_t *, ast_tree_t *);
ast_tree_t* mknode_control(token_data_t, ast_tree_t *, ast_tree_t *, ast_tree_t *);
ast_tree_t* mknode_pcall(token_data_t, ast_tree_t *);
ast_tree_t* append_args(ast_tree_t *, ast_tree_t *);
ast_tree_t* append_stmts(ast_tree_t *, ast_tree_t *);
ast_tree_t* mkleaf(token_data_t);

#endif
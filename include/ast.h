#ifndef AS3_AST_H
#define AS3_AST_H

typedef struct ast_btree ast_btree_t;
typedef struct token_data token_data_t;

typedef enum token_type {
  TT_ID = 1,
  TT_INT,
  TT_FLOAT
} token_type_t;

struct token_data {
  char* value;
  token_type_t type;
};

struct ast_btree {
  ast_btree_t* left;
  ast_btree_t* right;
  token_data_t* token;
};

#endif
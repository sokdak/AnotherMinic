#include <stdlib.h>
#include "include/node.h"

ast_tree_t* mknode(token_data_t token, ast_tree_t* left, ast_tree_t* right) {
  ast_tree_t* newnode;

  newnode = (ast_tree_t*)malloc(sizeof(ast_tree_t));
  newnode->token = token;
  newnode->left = left;
  newnode->right = right;
  newnode->arglist = NULL;
  newnode->next = NULL;
  newnode->condition = NULL;

  return newnode;
}

ast_tree_t* mknode_control(token_data_t token, ast_tree_t* left,
  ast_tree_t* right, ast_tree_t* condition)
{
  ast_tree_t* newnode;

  newnode = (ast_tree_t*)malloc(sizeof(ast_tree_t));
  newnode->token = token;
  newnode->left = left;
  newnode->right = right;
  newnode->arglist = NULL;
  newnode->next = NULL;
  newnode->condition = condition;

  return newnode;
}

ast_tree_t* mknode_pcall(token_data_t token, ast_tree_t* arglist) {
  ast_tree_t* newnode;

  newnode = (ast_tree_t*)malloc(sizeof(ast_tree_t));
  newnode->token = token;
  newnode->arglist = arglist;

  return newnode;
}

// input: tree->arg
ast_tree_t* append_args(ast_tree_t* arg, ast_tree_t* org) {
  ast_tree_t* ret_tree = org;
  ast_tree_t* list = ret_tree->arglist;
  
  while (list != NULL) // NULL arg가 나올때 까지 반복
    list = list->next;

  list = arg;

  return ret_tree;
}

ast_tree_t* append_stmts(ast_tree_t* arg, ast_tree_t* org) {
  ast_tree_t* ret_tree = org;
  ast_tree_t* list = ret_tree->stmtlist;
  
  while (list != NULL) // NULL arg가 나올때 까지 반복
    list = list->next;

  list = arg;

  return ret_tree;
}

ast_tree_t* mkleaf(token_data_t token) {
  ast_tree_t* newleaf;

  newleaf = (ast_tree_t*)malloc(sizeof(ast_tree_t));
  newleaf->token = token;
  newleaf->left = NULL;
  newleaf->right = NULL;
  newleaf->arglist = NULL;
  newleaf->next = NULL;
  newleaf->condition = NULL;

  return newleaf;
}

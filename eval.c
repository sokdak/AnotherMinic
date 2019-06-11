#include <stdio.h>
#include <stdlib.h>
#include "include/eval.h"
#include "include/symbol.h"
#include "include/opt.h"

function_t *functions;
symbol_t *sym_global, *sym_local, *sym_local_new;

ast_tree_t *main_procedure;

void runtime_error(char* msg) {
  fprintf(stderr, "runtime error: %s\n", msg);
  exit(0);
}

void eval(program_t* program) {
  if (program->function)
    functions = program->function; // local symbol은 evaluation할 때 함수 진입시 할당

  main_procedure = program->statement;

  if (EVAL_DEBUG)
    fprintf(stderr, "main procedure: start\n");
  
  while (main_procedure != NULL) {
    evaluation(main_procedure);
    main_procedure = main_procedure->stmtlist;
  }

  if (EVAL_DEBUG)
    fprintf(stderr, "main procedure: end\n");
}

rvalue_t evaluation(ast_tree_t *node) {
  rvalue_t retval;

  if (node->token.type == 258) {
    rvalue_t nodeend = { .type = RVAL_INT, .value.ival = 0 };
    return nodeend;
  }

  token_data_t token_data = node->token;
  
  if (EVAL_DEBUG)
    fprintf(stderr, "\n[eval::main] processing token %d, left %p, right %p\n",
      token_data.type, node->left, node->right);

  if (token_data.type == TT_INT) {
    if (EVAL_DEBUG)
      fprintf(stderr, "[eval::int] init\n");

    retval.type = RVAL_INT;
    retval.value.ival = token_data.value.intval;
  }
  else if (token_data.type == TT_FLOAT) {
    if (EVAL_DEBUG)
      fprintf(stderr, "[eval::float] init\n");

    retval.type = RVAL_REAL;
    retval.value.dval = token_data.value.doubleval;
  }
  else if (token_data.type == TT_ARITH_PLUS) {
    if (EVAL_DEBUG)
      fprintf(stderr,"[eval::plus] init\n");

    rvalue_t lval = evaluation(node->left);
    rvalue_t rval = evaluation(node->right);

    if (lval.type == RVAL_INT && rval.type == RVAL_REAL) {
      retval.type = RVAL_REAL;
      retval.value.dval = (double)lval.value.ival + rval.value.dval;
    }
    else if (lval.type == RVAL_INT && rval.type == RVAL_INT) {
      retval.type = RVAL_INT;
      retval.value.ival = lval.value.ival + rval.value.ival;
    }
    else if (lval.type == RVAL_REAL && rval.type == RVAL_REAL) {
      retval.type = RVAL_REAL;
      retval.value.dval = lval.value.dval + rval.value.dval;
    }
    else if (lval.type == RVAL_REAL && rval.type == RVAL_INT) {
      retval.type = RVAL_REAL;
      retval.value.dval = lval.value.dval + (double)rval.value.ival;
    }
    else runtime_error("undefined return type value");
  }
  else if (token_data.type == TT_ARITH_MINUS) {
    if (EVAL_DEBUG)
      fprintf(stderr,"[eval::minus] init\n");

    rvalue_t lval = evaluation(node->left);
    rvalue_t rval = evaluation(node->right);
    
    if (lval.type == RVAL_INT && rval.type == RVAL_REAL) {
      retval.type = RVAL_REAL;
      retval.value.dval = (double)lval.value.ival - rval.value.dval;
    }
    else if (lval.type == RVAL_INT && rval.type == RVAL_INT) {
      retval.type = RVAL_INT;
      retval.value.ival = lval.value.ival - rval.value.ival;
    }
    else if (lval.type == RVAL_REAL && rval.type == RVAL_REAL) {
      retval.type = RVAL_REAL;
      retval.value.dval = lval.value.dval - rval.value.dval;
    }
    else if (lval.type == RVAL_REAL && rval.type == RVAL_INT) {
      retval.type = RVAL_REAL;
      retval.value.dval = lval.value.dval - (double)rval.value.ival;
    }
    else runtime_error("undefined return type value");
  }
  else if (token_data.type == TT_ARITH_MULTI) {
    if (EVAL_DEBUG)
      fprintf(stderr,"[eval::multi] init\n");

    rvalue_t lval = evaluation(node->left);
    rvalue_t rval = evaluation(node->right);
    
    if (lval.type == RVAL_INT && rval.type == RVAL_REAL) {
      retval.type = RVAL_REAL;
      retval.value.dval = (double)lval.value.ival * rval.value.dval;
    }
    else if (lval.type == RVAL_INT && rval.type == RVAL_INT) {
      retval.type = RVAL_INT;
      retval.value.ival = lval.value.ival * rval.value.ival;
    }
    else if (lval.type == RVAL_REAL && rval.type == RVAL_REAL) {
      retval.type = RVAL_REAL;
      retval.value.dval = lval.value.dval * rval.value.dval;
    }
    else if (lval.type == RVAL_REAL && rval.type == RVAL_INT) {
      retval.type = RVAL_REAL;
      retval.value.dval = lval.value.dval * (double)rval.value.ival;
    }
    else runtime_error("undefined return type value");
  }
  else if (token_data.type == TT_ARITH_DIVIDE) {
    if (EVAL_DEBUG)
      fprintf(stderr,"[eval::divide] init\n");

    rvalue_t lval = evaluation(node->left);
    rvalue_t rval = evaluation(node->right);

    if (rval.type == RVAL_INT) {                // divide by zero
      if (rval.value.ival == 0)
        runtime_error("divide by zero");
    }
    else {
      if (rval.value.dval == 0)
        runtime_error("divide by zero");
    }
    
    if (lval.type == RVAL_INT && rval.type == RVAL_REAL) {
      retval.type = RVAL_REAL;
      retval.value.dval = (double)lval.value.ival / rval.value.dval;
    }
    else if (lval.type == RVAL_INT && rval.type == RVAL_INT) {
      if (lval.value.ival % rval.value.ival != 0) { // 소수점이 발생하는지 비교, 발생시 double로 변경
        retval.type = RVAL_REAL;
        retval.value.dval = (double)lval.value.ival / (double)rval.value.ival;  
      }
      else { // 발생하지 않으면 integer
        retval.type = RVAL_INT;
        retval.value.ival = lval.value.ival / rval.value.ival;
      }
    }
    else if (lval.type == RVAL_REAL && rval.type == RVAL_REAL) {
      retval.type = RVAL_REAL;
      retval.value.dval = lval.value.dval / rval.value.dval;
    }
    else if (lval.type == RVAL_REAL && rval.type == RVAL_INT) {
      retval.type = RVAL_REAL;
      retval.value.dval = lval.value.dval / (double)rval.value.ival;
    }
    else runtime_error("undefined return type value");
  }
  else if (token_data.type == TT_ARITH_UMINUS) {
    if (EVAL_DEBUG)
      fprintf(stderr, "[eval::uminus] init\n");

    rvalue_t val = evaluation(node->left);

    if (val.type == RVAL_INT) {
      retval.type = RVAL_INT;
      retval.value.ival = -val.value.ival;
    }
    else {
      retval.type = RVAL_REAL;
      retval.value.dval = -val.value.dval;
    }
  }
  else if (token_data.type == TT_ASSIGN) {
    if (EVAL_DEBUG)
      fprintf(stderr, "[eval:assign] init\n");

    char* lval_symval = node->left->token.value.symval;

    rvalue_t rval = evaluation(node->right);

    if (stack_isempty()) { // 스택이 비어있으면 (main procedure)
      if (EVAL_DEBUG)
        fprintf(stderr, "[eval::assign] stack is empty\n");

      symbol_t* sym = get_symbol_by_name(lval_symval, sym_global);

      int idx;

      if (sym == NULL) {
        idx = insert_symbol(lval_symval, &sym_global);
        sym = get_symbol(idx, sym_global);
      }

      if (EVAL_DEBUG)
        fprintf(stderr, "[eval::assign] symid: %d, sym: %p\n", idx, sym);

      token_data_t newtoken;

      if (rval.type == RVAL_REAL) {
        sym->type = RVAL_REAL;
        sym->value.real_constant = rval.value.dval;

        if (EVAL_DEBUG)
          fprintf(stderr, "[eval::assign] value %f on symbol %s, global %p, idx %d\n",
            newtoken.value.doubleval, lval_symval, sym_global, idx);

        retval.type = RVAL_REAL;
        retval.value.dval = rval.value.dval;
      }
      else if (rval.type == RVAL_INT) {
        sym->type = RVAL_INT;
        sym->value.integer_constant = rval.value.ival;

        if (EVAL_DEBUG)
          fprintf(stderr, "[eval::assign] value %d on symbol %s, global %p, idx %d\n",
            newtoken.value.intval, lval_symval, sym_global, idx);

        retval.type = RVAL_INT;
        retval.value.ival = rval.value.ival;
      }

      if (EVAL_DEBUG)
        fprintf(stderr, "[eval::assign] global assigning, after symtableTypeDef: %d\n", sym->type);

      sym->assigned = true;
    }
    else { // subroutine 내부일 경우
      if (EVAL_DEBUG)
        fprintf(stderr, "[eval::assign] local assigning\n");

      if (!symbol_exists_on_table_by_name(lval_symval, sym_local)) { // 심볼id가 null이면 (할당 안됐으면)
        if (EVAL_DEBUG)
          fprintf(stderr, "[eval::assign] no such symbol at local, continuing with global\n");

        symbol_t* sym = get_symbol_by_name(lval_symval, sym_global); // 글로벌에 삽입

        int idx;

        if (sym == NULL) {
          idx = insert_symbol(lval_symval, &sym_global);
          sym = get_symbol(idx, sym_global);
        }

        token_data_t newtoken;

        if (rval.type == RVAL_REAL) {
          sym->type = RVAL_INT;
          sym->value.integer_constant = rval.value.ival;

          if (EVAL_DEBUG)
            fprintf(stderr, "[eval::assign] value %f on symbol %s, global %p, idx %d\n",
              newtoken.value.doubleval, lval_symval, sym_global, idx);

          retval.type = RVAL_REAL;
          retval.value.dval = rval.value.dval;
        }
        else if (rval.type == RVAL_INT) {
          sym->type = RVAL_INT;
          sym->value.integer_constant = rval.value.ival;

          if (EVAL_DEBUG)
            fprintf(stderr, "[eval::assign] value %d on symbol %s, global %p, idx %d\n",
              newtoken.value.intval, lval_symval, sym_global, idx);

          retval.type = RVAL_INT;
          retval.value.ival = rval.value.ival;
        }

        sym->assigned = true;
      }
      else { // 로컬에 할당
        if (EVAL_DEBUG)
          fprintf(stderr, "[eval::assign] found symbol at local\n");
        
        symbol_t* sym = get_symbol_by_name(lval_symval, sym_local);

        int idx;

        if (sym == NULL) {
          idx = insert_symbol(lval_symval, &sym_local);
          sym = get_symbol(idx, sym_local);
        }

        sym->assigned = true;
        
        token_data_t newtoken;

        if (rval.type == RVAL_REAL) {
          sym->type = RVAL_INT;
          sym->value.integer_constant = rval.value.ival;

          if (EVAL_DEBUG)
            fprintf(stderr, "[eval::assign] value %f on symbol %s, local %p, idx %d\n",
              newtoken.value.doubleval, lval_symval, sym_local, idx);

          retval.type = RVAL_REAL;
          retval.value.dval = rval.value.dval;
        }
        else if (rval.type == RVAL_INT) {
          sym->type = RVAL_INT;
          sym->value.integer_constant = rval.value.ival;

          if (EVAL_DEBUG)
            fprintf(stderr, "[eval::assign] value %d on symbol %s, local %p, idx %d\n",
              newtoken.value.intval, lval_symval, sym_local, idx);

          retval.type = RVAL_INT;
          retval.value.ival = rval.value.ival;
        }
      }
    }
  }
  else if (token_data.type == TT_COMP_LGT) {
    if (EVAL_DEBUG)
      fprintf(stderr, "[eval::comp_lgt] init\n");

    rvalue_t lval = evaluation(node->left);
    rvalue_t rval = evaluation(node->right);
    
    retval.type = RVAL_INT;

    if (lval.type == RVAL_INT && rval.type == RVAL_REAL)
      retval.value.ival = (double)lval.value.ival > rval.value.dval;
    else if (lval.type == RVAL_INT && rval.type == RVAL_INT)
      retval.value.ival = lval.value.ival > rval.value.ival;
    else if (lval.type == RVAL_REAL && rval.type == RVAL_REAL)
      retval.value.ival = lval.value.dval > rval.value.dval;
    else if (lval.type == RVAL_REAL && rval.type == RVAL_INT)
      retval.value.ival = lval.value.dval > (double)rval.value.ival;
    else runtime_error("undefined return type value");

    if (EVAL_DEBUG)
      fprintf(stderr, "[eval::comp_lgt] result: %d\n", retval.value.ival);
  }
  else if (token_data.type == TT_COMP_LEGT) {
    if (EVAL_DEBUG)
      fprintf(stderr, "[eval::comp_legt] init\n");

    rvalue_t lval = evaluation(node->left);
    rvalue_t rval = evaluation(node->right);
    
    retval.type = RVAL_INT;

    if (lval.type == RVAL_INT && rval.type == RVAL_REAL)
      retval.value.ival = lval.value.ival >= rval.value.dval;
    else if (lval.type == RVAL_INT && rval.type == RVAL_INT)
      retval.value.ival = lval.value.ival >= rval.value.ival;
    else if (lval.type == RVAL_REAL && rval.type == RVAL_REAL)
      retval.value.ival = lval.value.dval >= rval.value.dval;
    else if (lval.type == RVAL_REAL && rval.type == RVAL_INT)
      retval.value.ival = lval.value.dval >= rval.value.ival;
    else runtime_error("undefined return type value");
  }
  else if (token_data.type == TT_COMP_RGT) {
    if (EVAL_DEBUG)
      fprintf(stderr, "[eval::comp_rgt] init\n");

    rvalue_t lval = evaluation(node->left);
    rvalue_t rval = evaluation(node->right);
    
    retval.type = RVAL_INT;

    if (lval.type == RVAL_INT && rval.type == RVAL_REAL)
      retval.value.ival = lval.value.ival < rval.value.dval;
    else if (lval.type == RVAL_INT && rval.type == RVAL_INT)
      retval.value.ival = lval.value.ival < rval.value.ival;
    else if (lval.type == RVAL_REAL && rval.type == RVAL_REAL)
      retval.value.ival = lval.value.dval < rval.value.dval;
    else if (lval.type == RVAL_REAL && rval.type == RVAL_INT)
      retval.value.ival = lval.value.dval < rval.value.ival;
    else runtime_error("undefined return type value");
  }
  else if (token_data.type == TT_COMP_REGT) {
    if (EVAL_DEBUG)
      fprintf(stderr, "[eval::comp_regt] init\n");

    rvalue_t lval = evaluation(node->left);
    rvalue_t rval = evaluation(node->right);
    
    retval.type = RVAL_INT;

    if (lval.type == RVAL_INT && rval.type == RVAL_REAL)
      retval.value.ival = lval.value.ival <= rval.value.dval;
    else if (lval.type == RVAL_INT && rval.type == RVAL_INT)
      retval.value.ival = lval.value.ival <= rval.value.ival;
    else if (lval.type == RVAL_REAL && rval.type == RVAL_REAL)
      retval.value.ival = lval.value.dval <= rval.value.dval;
    else if (lval.type == RVAL_REAL && rval.type == RVAL_INT)
      retval.value.ival = lval.value.dval <= rval.value.ival;
    else runtime_error("undefined return type value");
  }
  else if (token_data.type == TT_COMP_EQUALS) {
    if (EVAL_DEBUG)
      fprintf(stderr, "[eval::comp_equals] init\n");

    rvalue_t lval = evaluation(node->left);
    rvalue_t rval = evaluation(node->right);
    
    retval.type = RVAL_INT;

    if (lval.type == RVAL_INT && rval.type == RVAL_REAL)
      retval.value.ival = lval.value.ival == rval.value.dval;
    else if (lval.type == RVAL_INT && rval.type == RVAL_INT)
      retval.value.ival = lval.value.ival == rval.value.ival;
    else if (lval.type == RVAL_REAL && rval.type == RVAL_REAL)
      retval.value.ival = lval.value.dval == rval.value.dval;
    else if (lval.type == RVAL_REAL && rval.type == RVAL_INT)
      retval.value.ival = lval.value.dval == rval.value.ival;
    else runtime_error("undefined return type value");
  }
  else if (token_data.type == TT_COMP_NOTEQUALS) {
    if (EVAL_DEBUG)
      fprintf(stderr, "[eval::comp_notequals] init\n");

    rvalue_t lval = evaluation(node->left);
    rvalue_t rval = evaluation(node->right);
    
    retval.type = RVAL_INT;

    if (lval.type == RVAL_INT && rval.type == RVAL_REAL)
      retval.value.ival = lval.value.ival != rval.value.dval;
    else if (lval.type == RVAL_INT && rval.type == RVAL_INT)
      retval.value.ival = lval.value.ival != rval.value.ival;
    else if (lval.type == RVAL_REAL && rval.type == RVAL_REAL)
      retval.value.ival = lval.value.dval != rval.value.dval;
    else if (lval.type == RVAL_REAL && rval.type == RVAL_INT)
      retval.value.ival = lval.value.dval != rval.value.ival;
    else runtime_error("undefined return type value");
  }
  else if (token_data.type == TT_PROCEDURE_CALL) {
    if (EVAL_DEBUG)
      fprintf(stderr, "[eval::pcall] init\n");

    function_t *func;
    ast_tree_t *arglist, *req_arglist;

    func = get_function(token_data.value.funstr, functions);
    
    if (func == NULL) runtime_error("tried to call non-exist function");
    
    if (EVAL_DEBUG)
      fprintf(stderr, "[eval::pcall] functionstr: %s\n", token_data.value.funstr);

    sym_local_new = hardcopy_symbollist(func->ast->localvar); // 새 symbol list
    req_arglist = func->arguments;
    arglist = node->arglist;

    if (EVAL_DEBUG) {
      fprintf(stderr, "[eval::pcall] func %p, localarg %p, arglist %p, sym_local_new %p\n", func, req_arglist, arglist, sym_local_new);
      print_symboltable(sym_local);
    }

    if (func->defined) {
      if (EVAL_DEBUG)
        fprintf(stderr, "[eval::pcall] function %s defined at %p\n", func->name, func);

      // arguments에 있는 변수명 순서에 따라 들어가는 symbol을 해당 이름으로 로컬에 카피함
      while (arglist != NULL && req_arglist != NULL) {
        if (EVAL_DEBUG)
          fprintf(stderr, "[eval::pcall] on arglist\n");

        int sidx;
        rvalue_t val;
        symbol_t* symt;

        fprintf(stderr, "[eval::pcall] calling symins\n");
        sidx = insert_symbol(req_arglist->token.value.argval, &sym_local_new); // 로컬에 심볼 자리 만들고
        fprintf(stderr, "[eval::pcall] called symins\n");
        symt = get_symbol(sidx, sym_local_new);

        // 해당 expression 값을 가져와서 symbol_local_new에 넣기 (스택 비었으면 글로벌)
        val = evaluation(arglist); // 현재 args를 evaluation함

        if (val.type == RVAL_INT) {
          symt->type = TYPE_VARIABLE_INT;
          symt->value.integer_constant = val.value.ival;

          if (EVAL_DEBUG)
            fprintf(stderr, "[eval::pcall] loaded arg %s, argval: %d\n", req_arglist->token.value.argval, val.value.ival);
        }
        else {
          symt->type = TYPE_VARIABLE_DOUBLE;
          symt->value.real_constant = val.value.dval;

          if (EVAL_DEBUG)
            fprintf(stderr, "[eval::pcall] loaded arg %s, argval: %f\n", req_arglist->token.value.argval, val.value.dval);
        }

        symt->assigned = true;

        if (EVAL_DEBUG)
          print_symboltable(sym_local_new);

        req_arglist = req_arglist->arglist;
        arglist = arglist->arglist;
      }

      if (EVAL_DEBUG)
        fprintf(stderr, "[eval::pcall] arguments loaded into symbol_local_new at %p\n", sym_local_new);

      // 여기서 기존 symboltable push
      // 기존 로컬 심볼테이블을 stack에 넣기 (루틴 개선해야함, 첫번째 NULL일 경우)
      push_symboltable(sym_local);
      sym_local = sym_local_new;
      sym_local_new = NULL;

      rvalue_t fval;
      ast_tree_t* local_stmt = func->ast;

      while (local_stmt != NULL) { // 문제: 중간에 return이 발생하면 stmt loop를 중단할 수 없음
        fval = evaluation(local_stmt);        // run subroutine
        local_stmt = local_stmt->stmtlist;
      }

      if (fval.type == TYPE_VARIABLE_INT)
        fprintf(stderr, "[eval::pcall] received return value %d\n", fval.value.ival);
      else fprintf(stderr, "[eval::pcall] received return value %f\n", fval.value.dval);

      if (fval.type == RVAL_INT) {
        retval.type = RVAL_INT;
        retval.value.ival = fval.value.ival;
      }
      else {
        retval.type = RVAL_REAL;
        retval.value.dval = fval.value.dval;
      }
    }
    else runtime_error("cannot use undefined function"); // 정의되지 않은 함수 사용

    if (arglist != NULL || req_arglist != NULL)
      runtime_error("argument does not match");

    fprintf(stderr, "pcall out\n");

    sym_local = pop_symboltable(); // 스택이 차있다면 1-level 내려오기
  }
  else if (token_data.type == TT_PRINT) {
    if (EVAL_DEBUG)
      fprintf(stderr, "[eval::print] init\n");

    rvalue_t val = evaluation(node->left);

    if (val.type == RVAL_INT)
      printf("%d\n", val.value.ival);
    else
      printf("%f\n", val.value.dval);
  }
  else if (token_data.type == TT_IF || token_data.type == TT_IFELSE) {
    if (EVAL_DEBUG)
      fprintf(stderr, "[eval::branch] init\n");

    // condition을 추출해서 if로 돌려버리기 (1 = true, 0 = false)
    ast_tree_t* condition = node->condition;
    ast_tree_t* if_true = node->left;
    ast_tree_t* if_false = node->right;

    if (EVAL_DEBUG)
      fprintf(stderr, "[eval::branch] cond %p, true %p, false %p\n", condition, if_true, if_false);

    int cond_result = evaluation(condition).value.ival;

    // if들어가면 node->left실행
    if (cond_result > 0 || cond_result < 0) {
      if (EVAL_DEBUG)
        fprintf(stderr, "[eval::branch] condition:%d, goto %p\n", cond_result, if_true);

      retval = evaluation(if_true); // left만 실행하고 나와야함
    }
    else { // 안들어가면 node->right 실행
      if (if_false != NULL) { // node->right가 null이면 if만 있는걸로 처리
        if (EVAL_DEBUG)
          fprintf(stderr, "[eval::branch] condition:%d, goto %p\n", cond_result, if_false);
        
        retval = evaluation(if_false);
      }
      else {
        if (EVAL_DEBUG)
          fprintf(stderr, "[eval::branch] condition:%d, no else\n", cond_result);
      }
    }

    if (EVAL_DEBUG)
      fprintf(stderr, "[eval::branch] out\n");
  }
  else if (token_data.type == TT_WHILE) {
    if (EVAL_DEBUG)
      fprintf(stderr, "[eval::while] init\n");

    ast_tree_t* condition = node->condition;
    ast_tree_t* body = node->left;

    if (EVAL_DEBUG)
      fprintf(stderr, "[eval::loop] cond %p, body %p\n", condition, body);
    
    while (true) {
      int cond_result = evaluation(condition).value.ival;

      if (EVAL_DEBUG)
        fprintf(stderr, "[eval::loop] condition result: %d\n", cond_result);

      if (cond_result > 0 || cond_result < 0)
        evaluation(body);
      else break;
    }

    if (EVAL_DEBUG)
      fprintf(stderr, "[eval::loop] out");
  }
  else if (token_data.type == TT_RETURN) {
    if (EVAL_DEBUG)
      fprintf(stderr, "[eval::return] init\n");

    rvalue_t val = evaluation(node->left);

    if (val.type == RVAL_INT) {
      retval.type = RVAL_INT;
      retval.value.ival = val.value.ival;

      if (EVAL_DEBUG)
        fprintf(stderr, "[eval::return] returning value %d\n", val.value.ival);
    }
    else {
      retval.type = RVAL_REAL;
      retval.value.dval = val.value.dval;

      if (EVAL_DEBUG)
        fprintf(stderr, "[eval::return] returning value %f\n", val.value.dval);
    }
  }
  else if (token_data.type == TT_ID) {
    if (EVAL_DEBUG)
      fprintf(stderr, "[eval::id] init\n");

    char* symval = token_data.value.symval;

    // local stack이 차있으면 로컬에서 찾고
    if (!stack_isempty()) {
      if (EVAL_DEBUG)
        fprintf(stderr, "[eval::id] stack is not empty, symval: %p, sym_local: %p\n",
          symval, sym_local);

      if (symbol_exists_on_table_by_name(symval, sym_local)) { // 로컬에 심볼이 있음
        if (EVAL_DEBUG)
          fprintf(stderr, "[eval::id] symbol on local, symval: %p, sym_local: %p\n",
            symval, sym_local);

        symbol_t* sym = get_symbol_by_name(symval, sym_local);

        if (sym->type == TYPE_VARIABLE_INT) {
          retval.type = RVAL_INT;
          retval.value.ival = sym->value.integer_constant;
        }
        else {
          retval.type = RVAL_REAL;
          retval.value.dval = sym->value.real_constant;
        }
      }
      else { // 로컬에 심볼이 없음: 글로벌에서 찾기
        symbol_t* sym = get_symbol_by_name(symval, sym_global);

        if (sym->type == TYPE_VARIABLE_INT) {
          retval.type = RVAL_INT;
          retval.value.ival = sym->value.integer_constant;
        }
        else {
          retval.type = RVAL_REAL;
          retval.value.dval = sym->value.real_constant;
        }
      }
    }
    else { // stack이 비어있으면 글로벌에서 찾기
      if (EVAL_DEBUG)
        fprintf(stderr, "[eval::id] global var\n");

      symbol_t* sym = get_symbol_by_name(symval, sym_global);

      if (sym->type == TYPE_VARIABLE_INT) {
        retval.type = RVAL_INT;
        retval.value.ival = sym->value.integer_constant;
      }
      else {
        retval.type = RVAL_REAL;
        retval.value.dval = sym->value.real_constant;
      }
    }
  }
  else runtime_error("unknown data type, failed to evaluation");

  return retval;
}
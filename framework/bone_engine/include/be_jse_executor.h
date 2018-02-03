/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*
 * be_jse_executor.h
 *
 *  Ecmascript bytecode executor.
 *
 *
 */

#ifndef BE_JSE_EXECUTOR_H_
#define BE_JSE_EXECUTOR_H_

#include "be_jse_types.h"
#include "be_jse_symbol.h"
#include "be_jse_lex.h"

#ifdef __cplusplus
extern "C" {
#endif

/*关系操作表达式*/
#define MEET_RELATIONAL_OP (vm.lex->tk==BE_TOKEN_OP_EQUAL ||\
                            vm.lex->tk==BE_TOKEN_OP_NEQUAL || \
                            vm.lex->tk==BE_TOKEN_OP_TYPE_EQUAL|| \
                            vm.lex->tk==BE_TOKEN_OP_TYPE_NEQUAL ||\
                            vm.lex->tk==BE_TOKEN_OP_LESS_EQUAL || \
                            vm.lex->tk==BE_TOKEN_OP_MORE_EQUAL ||\
                            vm.lex->tk=='<' || vm.lex->tk=='>')

/**/
#define MEET_POSTFIX_OP              (vm.lex->tk==BE_TOKEN_OP_PLUS_PLUS || vm.lex->tk==BE_TOKEN_OP_MINUS_MINUS)
#define MEET_SHIFT_OP                (vm.lex->tk==BE_TOKEN_OP_LEFT_SHIFT || vm.lex->tk==BE_TOKEN_OP_RIGHT_SHIFT || vm.lex->tk==BE_TOKEN_OP_RIGHT_SHIFT_UNSIGN)
#define MEET_BIT_ARITHMATICAL_OP    (vm.lex->tk=='&' || vm.lex->tk=='|' || vm.lex->tk=='^')
#define MEET_LOGICAL_OP             (vm.lex->tk==BE_TOKEN_OP_LOGIC_AND || vm.lex->tk==BE_TOKEN_OP_LOGIC_OR)
#define MEET_ASSIGNMENT_OP          (vm.lex->tk=='=' || vm.lex->tk==BE_TOKEN_OP_PLUS_EQUAL || vm.lex->tk==BE_TOKEN_OP_MINUS_EQUAL)



/* 宏定义可以减少重复编码,相对于函数，可以减少函数调用和返回的堆栈压栈和出栈*/

//匹配需要的词法，并搜索下一个char,该宏带返回值，并带异常处理
#define LEXER_MATCH_WITH_CLEAN_AND_RETURN(EXPECTED_TOKEN, CLEANUP_CODE, RETURN_VAL) { if (!lexer_token_cmp(vm.lex,(EXPECTED_TOKEN))) { be_jse_execute_error(); CLEANUP_CODE; return RETURN_VAL; } }

//匹配需要的词法，并搜索下一个char,该宏带返回值
#define LEXER_MATCH_WITH_RETURN(EXPECTED_TOKEN, RETURN_VAL) LEXER_MATCH_WITH_CLEAN_AND_RETURN(EXPECTED_TOKEN, , RETURN_VAL)

//匹配需要的词法，并搜索下一个char
#define LEXER_MATCH(EXPECTED_TOKEN) LEXER_MATCH_WITH_CLEAN_AND_RETURN(EXPECTED_TOKEN, , 0)

#define BE_VM_PARSE_SHOULD_EXECUTE (((vm.execute)&EXEC_RUN_MASK)==EXEC_NORMAL)

#define BE_SAVE_VM_EXE_STATE() be_execflag_e oldExecute = vm.execute
#define BE_RESTORE_VM_EXE_STATE() vm.execute = (vm.execute&(be_execflag_e)(~EXEC_NORMAL)) | (oldExecute&EXEC_NORMAL);

void be_jse_executor_init(be_jse_executor_ctx_t *executor);
void be_jse_executor_deinit(be_jse_executor_ctx_t *executor);

//添加Native函数和本地对象
bool be_jse_add_native_func(be_jse_executor_ctx_t *executor, const char *funcDesc, be_jse_callback callbackPtr);

// 运行JS代码
be_jse_symbol_t *be_jse_eval_string(be_jse_executor_ctx_t *executor, const char *str);
bool be_jse_execute_func(be_jse_executor_ctx_t *executor, be_jse_symbol_t *func, int argCount, be_jse_symbol_t **argPtr);

bool be_jse_handle_function(be_jse_parse_skip_flag_e skipName, be_jse_symbol_t **a, be_jse_symbol_t **b, be_jse_symbol_t **c, be_jse_symbol_t **d);
bool be_jse_is_none_arg_function();
be_jse_symbol_t *be_jse_handle_single_arg_function();

void be_jse_execute_error();

#ifdef __cplusplus
}
#endif

#endif /* BE_JSE_EXECUTOR_H_ */

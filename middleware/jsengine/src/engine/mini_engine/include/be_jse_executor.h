/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/*
 * be_jse_executor.h
 *
 *  Ecmascript bytecode executor.
 *
 *
 */

#ifndef BE_JSE_EXECUTOR_H
#define BE_JSE_EXECUTOR_H

#include "be_jse_lex.h"
#include "be_jse_symbol.h"
#include "be_jse_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* relational operation expression */
#define MEET_RELATIONAL_OP                                                  \
    (vm.lex->tk == BE_TOKEN_OP_EQUAL || vm.lex->tk == BE_TOKEN_OP_NEQUAL || \
     vm.lex->tk == BE_TOKEN_OP_STRICT_EQUAL ||                              \
     vm.lex->tk == BE_TOKEN_OP_STRICT_NEQUAL ||                             \
     vm.lex->tk == BE_TOKEN_OP_LESS_EQUAL ||                                \
     vm.lex->tk == BE_TOKEN_OP_MORE_EQUAL || vm.lex->tk == '<' ||           \
     vm.lex->tk == '>')

#define MEET_POSTFIX_OP                     \
    (vm.lex->tk == BE_TOKEN_OP_PLUS_PLUS || \
     vm.lex->tk == BE_TOKEN_OP_MINUS_MINUS)
#define MEET_SHIFT_OP                         \
    (vm.lex->tk == BE_TOKEN_OP_LEFT_SHIFT ||  \
     vm.lex->tk == BE_TOKEN_OP_RIGHT_SHIFT || \
     vm.lex->tk == BE_TOKEN_OP_RIGHT_SHIFT_UNSIGN)
#define MEET_BIT_ARITHMATICAL_OP \
    (vm.lex->tk == '&' || vm.lex->tk == '|' || vm.lex->tk == '^')
#define MEET_LOGICAL_OP \
    (vm.lex->tk == BE_TOKEN_OP_LOGIC_AND || vm.lex->tk == BE_TOKEN_OP_LOGIC_OR)
#define MEET_ASSIGNMENT_OP                                        \
    (vm.lex->tk == '=' || vm.lex->tk == BE_TOKEN_OP_PLUS_EQUAL || \
     vm.lex->tk == BE_TOKEN_OP_MINUS_EQUAL ||                     \
     vm.lex->tk == BE_TOKEN_OP_MUL_EQUAL ||                       \
     vm.lex->tk == BE_TOKEN_OP_DIV_EQUAL)

/* match lexer and search next char, with return, with exception handle */
#define LEXER_MATCH_WITH_CLEAN_AND_RETURN(EXPECTED_TOKEN, CLEANUP_CODE, \
                                          RETURN_VAL)                   \
    {                                                                   \
        if (!lexer_token_cmp(vm.lex, (EXPECTED_TOKEN))) {               \
            be_jse_execute_error();                                     \
            CLEANUP_CODE;                                               \
            return RETURN_VAL;                                          \
        }                                                               \
    }

/* match lexer and search next char, with return */
#define LEXER_MATCH_WITH_RETURN(EXPECTED_TOKEN, RETURN_VAL) \
    LEXER_MATCH_WITH_CLEAN_AND_RETURN(EXPECTED_TOKEN, , RETURN_VAL)

/* match lexer and search next char */
#define LEXER_MATCH(EXPECTED_TOKEN) \
    LEXER_MATCH_WITH_CLEAN_AND_RETURN(EXPECTED_TOKEN, , 0)

#define BE_VM_PARSE_SHOULD_EXECUTE \
    (((vm.execute) & EXEC_RUN_MASK) == EXEC_NORMAL)

#define BE_SAVE_VM_EXE_STATE() be_execflag_e oldExecute = vm.execute
#define BE_RESTORE_VM_EXE_STATE()                               \
    vm.execute = (vm.execute & (be_execflag_e)(~EXEC_NORMAL)) | \
                 (oldExecute & EXEC_NORMAL);

void be_jse_executor_init(be_jse_executor_ctx_t *executor);
void be_jse_executor_deinit(be_jse_executor_ctx_t *executor);

/* add native function */
bool be_jse_add_native_func(be_jse_executor_ctx_t *executor,
                            const char *funcDesc, be_jse_callback callbackPtr);

/* execute js code */
be_jse_symbol_t *be_jse_eval_string(be_jse_executor_ctx_t *executor,
                                    const char *str);

typedef struct _async {
    be_jse_symbol_t *func;
    be_jse_symbol_t **params;
    int param_count;
} BE_ASYNC_S;

void be_jse_async_event_cb(void *arg);
bool be_jse_execute_func(be_jse_executor_ctx_t *executor, be_jse_symbol_t *func,
                         int argCount, be_jse_symbol_t **argPtr);

bool be_jse_handle_function(be_jse_parse_skip_flag_e skipName,
                            be_jse_symbol_t **a, be_jse_symbol_t **b,
                            be_jse_symbol_t **c, be_jse_symbol_t **d);
bool be_jse_is_none_arg_function(void);
be_jse_symbol_t *be_jse_handle_single_arg_function(void);

void be_jse_execute_error(void);

be_jse_symbol_t *jspEvaluateModule(be_jse_executor_ctx_t *executor,
                                   const char *str);
be_jse_symbol_t *be_jse_require(be_jse_executor_ctx_t *executor,
                                const char *moduleName);
be_jse_symbol_t *be_jse_add_module(be_jse_executor_ctx_t *executor,
                                   be_jse_symbol_t *id, be_jse_symbol_t *code);
void be_jse_remove_modules(be_jse_executor_ctx_t *executor);

/**
 *   convert a symbol array to a malloc buffer ,need free by caller
 *   @return a buffer pointor
 */
char *be_array_symbol_to_buffer(be_jse_symbol_t *arg);

#ifdef __cplusplus
}
#endif

#endif /* BE_JSE_EXECUTOR_H */

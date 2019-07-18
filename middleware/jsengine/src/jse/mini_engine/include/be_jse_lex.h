/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/*
 * be_jse_lex.h
 *
 * JS词法分析器
 * 当前支持的单词类型 be_lexer_token_type_e
 *
 *
 */

#ifndef BE_JSE_LEX_H
#define BE_JSE_LEX_H

#include "be_jse.h"
#include "be_jse_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* lexer 操作 */
void be_jse_lexer_init(be_jse_lexer_ctx_t *lex, const char *src, int startPos,
                       int endPos);
/* 用于for/while循环中分析条件变量 */
void be_jse_lexer_init2(be_jse_lexer_ctx_t *lex, be_jse_lexer_ctx_t *initFrom,
                        int startPos);
void be_jse_lexer_deinit(be_jse_lexer_ctx_t *lex);
void be_jse_lexer_reset(be_jse_lexer_ctx_t *lex);

/* Lexer对token/字符元素操作 */
bool lexer_token_cmp(be_jse_lexer_ctx_t *lex, int expected_token);
char *lexer_get_token(be_jse_lexer_ctx_t *lex);
void lexer_seekto_char(be_jse_lexer_ctx_t *lex, int seekToChar);
void lexer_get_next_char(be_jse_lexer_ctx_t *lex);

/* 调试使用
   把token转成string, 方便调试 */
void lexer_token_to_str(int token, char *str, size_t len);
void lexer_dump_line_and_col(be_jse_lexer_ctx_t *lex, int charPos, int *line,
                             int *col);

#ifdef __cplusplus
}
#endif

#endif /* BE_JSE_LEX_H */

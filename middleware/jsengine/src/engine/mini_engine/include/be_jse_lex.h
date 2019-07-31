/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/*
 * be_jse_lex.h
 *
 * JS lexical analyzer
 * supported lexer in be_lexer_token_type_e
 *
 */

#ifndef BE_JSE_LEX_H
#define BE_JSE_LEX_H

#include "be_jse.h"
#include "be_jse_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* lexer initialization */
void be_jse_lexer_init(be_jse_lexer_ctx_t *lex, const char *src, int startPos,
                       int endPos);

/* for analysis conditional variable in for/while */
void be_jse_lexer_init2(be_jse_lexer_ctx_t *lex, be_jse_lexer_ctx_t *initFrom,
                        int startPos);
void be_jse_lexer_deinit(be_jse_lexer_ctx_t *lex);
void be_jse_lexer_reset(be_jse_lexer_ctx_t *lex);

/* char operation */
bool lexer_token_cmp(be_jse_lexer_ctx_t *lex, int expected_token);
char *lexer_get_token(be_jse_lexer_ctx_t *lex);
void lexer_seekto_char(be_jse_lexer_ctx_t *lex, int seekToChar);
void lexer_get_next_char(be_jse_lexer_ctx_t *lex);

/* convert token to string, for debugging! */
void lexer_token_to_str(int token, char *str, size_t len);
void lexer_dump_line_and_col(be_jse_lexer_ctx_t *lex, int charPos, int *line,
                             int *col);

#ifdef __cplusplus
}
#endif

#endif /* BE_JSE_LEX_H */

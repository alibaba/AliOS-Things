/*
 * Copyright (C) 2018-2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CSS_SYNTAX_CONSUME_H
#define LEXBOR_CSS_SYNTAX_CONSUME_H


#ifdef __cplusplus
extern "C" {
#endif

#include "syntax/base.h"
#include "syntax/tokenizer.h"


const lxb_char_t *
lxb_css_syntax_consume_string(lxb_css_syntax_tokenizer_t *tkz,
                              const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_consume_before_numeric(lxb_css_syntax_tokenizer_t *tkz,
                                      const lxb_char_t *data,
                                      const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_consume_numeric(lxb_css_syntax_tokenizer_t *tkz,
                               const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_consume_numeric_decimal(lxb_css_syntax_tokenizer_t *tkz,
                                       const lxb_char_t *data,
                                       const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_consume_ident_like(lxb_css_syntax_tokenizer_t *tkz,
                                  const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_consume_ident_like_not_url(lxb_css_syntax_tokenizer_t *tkz,
                                          const lxb_char_t *data,
                                          const lxb_char_t *end);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_CSS_SYNTAX_CONSUME_H */

/*
 * Copyright (C) 2018-2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CSS_SYNTAX_STATE_H
#define LEXBOR_CSS_SYNTAX_STATE_H

#define lxb_css_syntax_state_set(tkz, func)                                    \
    do {                                                                       \
        tkz->state = func;                                                     \
    }                                                                          \
    while (0)

#define _lxb_css_syntax_state_token_done_m(tkz, v_end)                         \
    tkz->token = tkz->cb_token_done(tkz, tkz->token, tkz->cb_token_ctx);       \
    if (tkz->token == NULL) {                                                  \
        if (tkz->status == LXB_STATUS_OK) {                                    \
            tkz->status = LXB_STATUS_ERROR;                                    \
        }                                                                      \
        return v_end;                                                          \
    }

#define lxb_css_syntax_state_token_done_m(tkz, v_end)                          \
    do {                                                                       \
        _lxb_css_syntax_state_token_done_m(tkz, v_end)                         \
        lxb_css_syntax_token_clean(tkz->token);                                \
    }                                                                          \
    while (0)

#ifdef __cplusplus
extern "C" {
#endif

#include "syntax/base.h"
#include "syntax/tokenizer.h"

#define LEXBOR_STR_RES_MAP_HEX
#include "core/str_res.h"


const lxb_char_t *
lxb_css_syntax_state_data(lxb_css_syntax_tokenizer_t *tkz,
                          const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_delim(lxb_css_syntax_tokenizer_t *tkz,
                           const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_eof(lxb_css_syntax_tokenizer_t *tkz,
                         const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_comment_begin(lxb_css_syntax_tokenizer_t *tkz,
                                   const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_whitespace(lxb_css_syntax_tokenizer_t *tkz,
                                const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_string(lxb_css_syntax_tokenizer_t *tkz,
                            const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_hash(lxb_css_syntax_tokenizer_t *tkz,
                          const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_lparenthesis(lxb_css_syntax_tokenizer_t *tkz,
                                  const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_rparenthesis(lxb_css_syntax_tokenizer_t *tkz,
                                  const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_plus(lxb_css_syntax_tokenizer_t *tkz,
                          const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_comma(lxb_css_syntax_tokenizer_t *tkz,
                           const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_minus(lxb_css_syntax_tokenizer_t *tkz,
                           const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_full_stop(lxb_css_syntax_tokenizer_t *tkz,
                               const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_colon(lxb_css_syntax_tokenizer_t *tkz,
                           const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_semicolon(lxb_css_syntax_tokenizer_t *tkz,
                               const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_less_sign(lxb_css_syntax_tokenizer_t *tkz,
                               const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_at(lxb_css_syntax_tokenizer_t *tkz,
                        const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_ls_bracket(lxb_css_syntax_tokenizer_t *tkz,
                                const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_rsolidus(lxb_css_syntax_tokenizer_t *tkz,
                              const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_rs_bracket(lxb_css_syntax_tokenizer_t *tkz,
                                const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_lc_bracket(lxb_css_syntax_tokenizer_t *tkz,
                                const lxb_char_t *data, const lxb_char_t *end);

const lxb_char_t *
lxb_css_syntax_state_rc_bracket(lxb_css_syntax_tokenizer_t *tkz,
                                const lxb_char_t *data, const lxb_char_t *end);


/*
 * Inline functions
 */
lxb_inline const lxb_char_t *
lxb_css_syntax_state_check_newline(lxb_css_syntax_tokenizer_t *tkz,
                                   const lxb_char_t *data,
                                   const lxb_char_t *end)
{
    tkz->state = tkz->return_state;

    if (tkz->is_eof) {
        return data;
    }

    if (*data == 0x0A) {
        data += 1;
    }

    return data;
}

lxb_inline const lxb_char_t *
lxb_css_syntax_state_check_escaped_loop(lxb_css_syntax_tokenizer_t *tkz,
                                        const lxb_char_t *data,
                                        const lxb_char_t *end)
{
    if (tkz->is_eof) {
        tkz->state = tkz->return_state;

        return data;
    }

    for (; tkz->count < 6; tkz->count++, data++) {
        if (data == end) {
            return data;
        }

        if (lexbor_str_res_map_hex[*data] == 0xFF) {
            /*
             * U+0009 CHARACTER TABULATION
             * U+0020 SPACE
             * U+000A LINE FEED (LF)
             * U+000C FORM FEED (FF)
             * U+000D CARRIAGE RETURN (CR)
             */
            switch (*data) {
                case 0x0D:
                    lxb_css_syntax_token_cr_set(tkz);

                    data += 1;

                    if (data == end) {
                        tkz->state = lxb_css_syntax_state_check_newline;

                        return data;
                    }

                    if (*data == 0x0A) {
                        data += 1;
                    }

                    break;

                case 0x0C:
                    lxb_css_syntax_token_ff_set(tkz);
                    /* fall through */

                case 0x09:
                case 0x20:
                case 0x0A:
                    data += 1;
                    break;
            }

            break;
        }
    }

    tkz->state = tkz->return_state;

    return data;
}

lxb_inline const lxb_char_t *
lxb_css_syntax_state_check_escaped(lxb_css_syntax_tokenizer_t *tkz,
                                   const lxb_char_t *data,
                                   const lxb_char_t *end,
                                   lxb_css_syntax_tokenizer_state_f ret_state)
{
    for (tkz->count = 0; tkz->count < 6; tkz->count++, data++) {
        if (data == end) {
            tkz->state = lxb_css_syntax_state_check_escaped_loop;
            tkz->return_state = ret_state;

            return data;
        }

        if (lexbor_str_res_map_hex[*data] == 0xFF) {
            if (tkz->count == 0) {
                data += 1;
                if (data == end) {
                    tkz->state = ret_state;
                }

                break;
            }

            switch (*data) {
                case 0x0D:
                    lxb_css_syntax_token_cr_set(tkz);

                    data += 1;
                    if (data == end) {
                        tkz->state = lxb_css_syntax_state_check_newline;
                        tkz->return_state = ret_state;

                        return data;
                    }

                    if (*data == 0x0A) {
                        data += 1;
                        if (data == end) {
                            tkz->state = ret_state;

                            return data;
                        }
                    }

                    break;

                case 0x0C:
                    lxb_css_syntax_token_ff_set(tkz);
                    /* fall through */

                case 0x09:
                case 0x20:
                case 0x0A:
                    data += 1;
                    if (data == end) {
                        tkz->state = ret_state;

                        return data;
                    }

                    break;
            }

            break;
        }
    }

    return data;
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_CSS_SYNTAX_STATE_H */

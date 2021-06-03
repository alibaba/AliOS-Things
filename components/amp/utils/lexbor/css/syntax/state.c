/*
 * Copyright (C) 2018-2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include <string.h>

#include "core/utils.h"

#include "syntax/state.h"
#include "syntax/consume.h"
#include "syntax/state_res.h"
#include "syntax/tokenizer/error.h"


#define LXB_CSS_SYNTAX_RES_NAME_MAP
#include "syntax/res.h"


static const lxb_char_t *
lxb_css_syntax_state_comment_slash(lxb_css_syntax_tokenizer_t *tkz,
                                   const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_comment(lxb_css_syntax_tokenizer_t *tkz,
                             const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_comment_end(lxb_css_syntax_tokenizer_t *tkz,
                                 const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_hash_name(lxb_css_syntax_tokenizer_t *tkz,
                               const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_hash_escape(lxb_css_syntax_tokenizer_t *tkz,
                                 const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_hash_consume_name(lxb_css_syntax_tokenizer_t *tkz,
                                       const lxb_char_t *data,
                                       const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_hash_consume_name_escape(lxb_css_syntax_tokenizer_t *tkz,
                                              const lxb_char_t *data,
                                              const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_plus_check(lxb_css_syntax_tokenizer_t *tkz,
                                const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_plus_check_digit(lxb_css_syntax_tokenizer_t *tkz,
                                      const lxb_char_t *data,
                                      const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_minus_check(lxb_css_syntax_tokenizer_t *tkz,
                                 const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_minus_check_digit(lxb_css_syntax_tokenizer_t *tkz,
                                       const lxb_char_t *data,
                                       const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_minus_check_cdc(lxb_css_syntax_tokenizer_t *tkz,
                                     const lxb_char_t *data,
                                     const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_minus_check_solidus(lxb_css_syntax_tokenizer_t *tkz,
                                         const lxb_char_t *data,
                                         const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_full_stop_num(lxb_css_syntax_tokenizer_t *tkz,
                                   const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_less_sign_check_exmark(lxb_css_syntax_tokenizer_t *tkz,
                                            const lxb_char_t *data,
                                            const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_less_sign_check_fminus(lxb_css_syntax_tokenizer_t *tkz,
                                            const lxb_char_t *data,
                                            const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_less_sign_check_tminus(lxb_css_syntax_tokenizer_t *tkz,
                                            const lxb_char_t *data,
                                            const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_at_begin(lxb_css_syntax_tokenizer_t *tkz,
                              const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_at_minus(lxb_css_syntax_tokenizer_t *tkz,
                              const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_at_escape(lxb_css_syntax_tokenizer_t *tkz,
                               const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_at_name(lxb_css_syntax_tokenizer_t *tkz,
                             const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_at_name_escape(lxb_css_syntax_tokenizer_t *tkz,
                                    const lxb_char_t *data,
                                    const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_state_rsolidus_check(lxb_css_syntax_tokenizer_t *tkz,
                                    const lxb_char_t *data,
                                    const lxb_char_t *end);


const lxb_char_t *
lxb_css_syntax_state_data(lxb_css_syntax_tokenizer_t *tkz,
                          const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_res_map[*data]);

    return tkz->state(tkz, data, end);
}

const lxb_char_t *
lxb_css_syntax_state_delim(lxb_css_syntax_tokenizer_t *tkz,
                           const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_DELIM;
    lxb_css_syntax_token_delim(tkz->token)->character = (lxb_codepoint_t) *data;

    lxb_css_syntax_token_delim(tkz->token)->begin = data;
    data += 1;
    lxb_css_syntax_token_delim(tkz->token)->end = data;

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
    lxb_css_syntax_state_token_done_m(tkz, end);

    return data;
}

/*
 * END-OF-FILE
 */
const lxb_char_t *
lxb_css_syntax_state_eof(lxb_css_syntax_tokenizer_t *tkz,
                         const lxb_char_t *data, const lxb_char_t *end)
{
    if (tkz->is_eof) {
        return end;
    }

    lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_IDENT;
    lxb_css_syntax_token_have_null_set(tkz);

    lxb_css_syntax_token_ident(tkz->token)->begin = data;
    data += 1;
    lxb_css_syntax_token_ident(tkz->token)->end = data;

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
    lxb_css_syntax_state_token_done_m(tkz, end);

    return data;
}

/*
 * Comment
 */
const lxb_char_t *
lxb_css_syntax_state_comment_begin(lxb_css_syntax_tokenizer_t *tkz,
                                   const lxb_char_t *data, const lxb_char_t *end)
{
    /* Skip forward slash (/) */
    data += 1;

    if (data == end) {
        tkz->begin = data - 1;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_comment_slash);

        return data;
    }

    /* U+002A ASTERISK (*) */
    if (*data == 0x2A) {
        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_COMMENT;
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_comment);

        return (data + 1);
    }

    return lxb_css_syntax_state_delim(tkz, (data - 1), end);
}

static const lxb_char_t *
lxb_css_syntax_state_comment_slash(lxb_css_syntax_tokenizer_t *tkz,
                                   const lxb_char_t *data, const lxb_char_t *end)
{
    /* U+002A ASTERISK (*) */
    if (*data == 0x2A) {
        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_COMMENT;
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_comment);

        return (data + 1);
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_delim);

    return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->begin);
}

static const lxb_char_t *
lxb_css_syntax_state_comment(lxb_css_syntax_tokenizer_t *tkz,
                             const lxb_char_t *data, const lxb_char_t *end)
{
    /* EOF */
    if (tkz->is_eof) {
        lxb_css_syntax_tokenizer_error_add(tkz->parse_errors,
                                           tkz->incoming_node->end,
                                           LXB_CSS_SYNTAX_TOKENIZER_ERROR_EOINCO);

        lxb_css_syntax_token_comment(tkz->token)->end = tkz->incoming_node->end;
        lxb_css_syntax_state_token_done_m(tkz, end);

        return end;
    }

    if (lxb_css_syntax_token_comment(tkz->token)->begin == NULL) {
        lxb_css_syntax_token_comment(tkz->token)->begin = data;
    }

    for (; data != end; data++) {
        switch (*data) {
            case 0x00:
                lxb_css_syntax_token_have_null_set(tkz);
                /* fall through */

            case 0x0D:
                lxb_css_syntax_token_cr_set(tkz);
                /* fall through */

            case 0x0C:
                lxb_css_syntax_token_ff_set(tkz);
                break;

            /* U+002A ASTERISK (*) */
            case 0x2A:
                data += 1;

                if (data == end) {
                    lxb_css_syntax_token_comment(tkz->token)->end = data - 1;

                    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_comment_end);

                    return data;
                }

                /* U+002F Forward slash (/) */
                if (*data == 0x2F) {
                    lxb_css_syntax_token_comment(tkz->token)->end = data - 1;

                    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
                    lxb_css_syntax_state_token_done_m(tkz, end);

                    return (data + 1);
                }

                data -= 1;

                break;
        }
    }

    return data;
}

static const lxb_char_t *
lxb_css_syntax_state_comment_end(lxb_css_syntax_tokenizer_t *tkz,
                                 const lxb_char_t *data, const lxb_char_t *end)
{
    /* EOF */
    if (tkz->is_eof) {
        lxb_css_syntax_tokenizer_error_add(tkz->parse_errors,
                                           tkz->incoming_node->end,
                                           LXB_CSS_SYNTAX_TOKENIZER_ERROR_EOINCO);

        lxb_css_syntax_token_comment(tkz->token)->end = tkz->incoming_node->end;
        lxb_css_syntax_state_token_done_m(tkz, end);

        return end;
    }

    /* U+002F Forward slash (/) */
    if (*data == 0x2F) {
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
        lxb_css_syntax_state_token_done_m(tkz, end);

        return (data + 1);
    }

    tkz->state = lxb_css_syntax_state_comment;

    return data;
}

/*
 * Whitespace
 */
const lxb_char_t *
lxb_css_syntax_state_whitespace(lxb_css_syntax_tokenizer_t *tkz,
                                const lxb_char_t *data, const lxb_char_t *end)
{
    if (lxb_css_syntax_token_whitespace(tkz->token)->begin == NULL) {
        lxb_css_syntax_token_whitespace(tkz->token)->begin = data;
        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_WHITESPACE;
    }
    else if (tkz->is_eof) {
        lxb_css_syntax_token_whitespace(tkz->token)->end = tkz->incoming_node->end;
        lxb_css_syntax_state_token_done_m(tkz, end);

        return end;
    }

    for (; data != end; data++) {
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
                break;

            case 0x0C:
                lxb_css_syntax_token_ff_set(tkz);
                /* fall through */

            case 0x09:
            case 0x20:
            case 0x0A:
                break;

            default:
                lxb_css_syntax_token_whitespace(tkz->token)->end = data;

                lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
                lxb_css_syntax_state_token_done_m(tkz, end);

                return data;
        }
    }

    return data;
}

/*
 * String token for U+0022 Quotation Mark (") and U+0027 Apostrophe (')
 */
const lxb_char_t *
lxb_css_syntax_state_string(lxb_css_syntax_tokenizer_t *tkz,
                            const lxb_char_t *data, const lxb_char_t *end)
{
    tkz->str_ending = *data;

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_string);

    return (data + 1);
}

/*
 * U+0023 NUMBER SIGN (#)
 */
const lxb_char_t *
lxb_css_syntax_state_hash(lxb_css_syntax_tokenizer_t *tkz,
                          const lxb_char_t *data, const lxb_char_t *end)
{
    /* Skip Number Sign (#) char */
    data += 1;

    if (data == end) {
        /*
         * Save position for the Number Sign (#) for the delim token
         * if some tests will be failed.
         */
        tkz->begin = data - 1;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_hash_name);

        return data;
    }

    lxb_css_syntax_token_hash(tkz->token)->begin = data;

    if (lxb_css_syntax_res_name_map[*data] == 0x00) {
        /* U+005C REVERSE SOLIDUS (\) */
        if (*data == 0x5C) {
            data += 1;

            if (data == end) {
                tkz->begin = data - 2;
                lxb_css_syntax_token_hash(tkz->token)->begin = data - 1;

                lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_hash_escape);

                return data;
            }

            /*
             * U+000A LINE FEED (LF)
             * U+000C FORM FEED (FF)
             * U+000D CARRIAGE RETURN (CR)
             */
            if (*data == 0x0A || *data == 0x0C || *data == 0x0D) {
                return lxb_css_syntax_state_delim(tkz, (data - 2), end);
            }
            else if (*data == 0x00) {
                lxb_css_syntax_token_have_null_set(tkz);
            }

            lxb_css_syntax_token_escaped_set(tkz);

            data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                        lxb_css_syntax_state_hash_consume_name);
            if (data == end) {
                lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_HASH;

                return data;
            }
        }
        else if (*data == 0x00) {
            lxb_css_syntax_token_have_null_set(tkz);
        }
        else {
            return lxb_css_syntax_state_delim(tkz, (data - 1), end);
        }
    }

    lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_HASH;

    for (; data < end; data++) {
        if (lxb_css_syntax_res_name_map[*data] == 0x00) {
            /* U+005C REVERSE SOLIDUS (\) */
            if (*data == 0x5C) {
                data += 1;

                if (data == end) {
                    tkz->end = data - 1;
                    lxb_css_syntax_token_hash(tkz->token)->end = tkz->end;

                    lxb_css_syntax_state_set(tkz,
                                 lxb_css_syntax_state_hash_consume_name_escape);
                    return data;
                }

                /*
                 * U+000A LINE FEED (LF)
                 * U+000C FORM FEED (FF)
                 * U+000D CARRIAGE RETURN (CR)
                 */
                if (*data == 0x0A || *data == 0x0C || *data == 0x0D) {
                    data -= 1;

                    lxb_css_syntax_token_hash(tkz->token)->end = data;

                    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
                    lxb_css_syntax_state_token_done_m(tkz, end);

                    return data;
                }
                else if (*data == 0x00) {
                    lxb_css_syntax_token_have_null_set(tkz);
                }

                lxb_css_syntax_token_escaped_set(tkz);

                data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                        lxb_css_syntax_state_hash_consume_name);
                if (data == end) {
                    return data;
                }

                data -= 1;
            }
            else if (*data == 0x00) {
                lxb_css_syntax_token_have_null_set(tkz);
            }
            else {
                lxb_css_syntax_token_hash(tkz->token)->end = data;

                lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
                lxb_css_syntax_state_token_done_m(tkz, end);

                return data;
            }
        }
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_hash_consume_name);

    return data;
}

static const lxb_char_t *
lxb_css_syntax_state_hash_name(lxb_css_syntax_tokenizer_t *tkz,
                               const lxb_char_t *data, const lxb_char_t *end)
{
    if (tkz->is_eof) {
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_delim);

        return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->begin);
    }

    if (lxb_css_syntax_res_name_map[*data] == 0x00) {
        /* U+005C REVERSE SOLIDUS (\) */
        if (*data == 0x5C) {
            data += 1;

            if (data == end) {
                lxb_css_syntax_token_hash(tkz->token)->begin = data - 1;
                lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_hash_escape);

                return data;
            }

            /*
             * U+000A LINE FEED (LF)
             * U+000C FORM FEED (FF)
             * U+000D CARRIAGE RETURN (CR)
             */
            if (*data == 0x0A || *data == 0x0C || *data == 0x0D) {
                lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_delim);

                return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->begin);
            }
            else if (*data == 0x00) {
                lxb_css_syntax_token_have_null_set(tkz);
            }

            lxb_css_syntax_token_escaped_set(tkz);

            lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_HASH;
            lxb_css_syntax_token_hash(tkz->token)->begin = data - 1;

            data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                        lxb_css_syntax_state_hash_consume_name);
            if (data == end) {
                return data;
            }

            lxb_css_syntax_state_set(tkz,
                                     lxb_css_syntax_state_hash_consume_name);
            return data;
        }
        else if (*data == 0x00) {
            lxb_css_syntax_token_have_null_set(tkz);
        }
        else {
            lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_delim);

            return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->begin);
        }
    }

    lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_HASH;
    lxb_css_syntax_token_hash(tkz->token)->begin = data;

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_hash_consume_name);

    return (data + 1);
}

static const lxb_char_t *
lxb_css_syntax_state_hash_escape(lxb_css_syntax_tokenizer_t *tkz,
                                 const lxb_char_t *data, const lxb_char_t *end)
{
    /*
     * U+000A LINE FEED (LF)
     * U+000C FORM FEED (FF)
     * U+000D CARRIAGE RETURN (CR)
     */
    if ((*data == 0x0A || *data == 0x0C || *data == 0x0D) || tkz->is_eof) {
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_delim);

        return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->begin);
    }
    else if (*data == 0x00) {
        lxb_css_syntax_token_have_null_set(tkz);
    }

    lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_HASH;
    lxb_css_syntax_token_escaped_set(tkz);

    data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                        lxb_css_syntax_state_hash_consume_name);
    if (data == end) {
        return data;
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_hash_consume_name);

    return data;
}

static const lxb_char_t *
lxb_css_syntax_state_hash_consume_name(lxb_css_syntax_tokenizer_t *tkz,
                                       const lxb_char_t *data,
                                       const lxb_char_t *end)
{
    if (tkz->is_eof) {
        lxb_css_syntax_token_hash(tkz->token)->end = tkz->incoming_node->end;

        lxb_css_syntax_state_token_done_m(tkz, end);

        return end;
    }

    for (; data < end; data++) {
        if (lxb_css_syntax_res_name_map[*data] == 0x00) {
            /* U+005C REVERSE SOLIDUS (\) */
            if (*data == 0x5C) {
                data += 1;

                if (data == end) {
                    tkz->end = data - 1;
                    lxb_css_syntax_token_hash(tkz->token)->end = tkz->end;

                    lxb_css_syntax_state_set(tkz,
                                 lxb_css_syntax_state_hash_consume_name_escape);
                    return data;
                }

                /*
                 * U+000A LINE FEED (LF)
                 * U+000C FORM FEED (FF)
                 * U+000D CARRIAGE RETURN (CR)
                 */
                if (*data == 0x0A || *data == 0x0C || *data == 0x0D) {
                    data -= 1;

                    lxb_css_syntax_token_hash(tkz->token)->end = data;

                    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
                    lxb_css_syntax_state_token_done_m(tkz, end);

                    return data;
                }
                else if (*data == 0x00) {
                    lxb_css_syntax_token_have_null_set(tkz);
                }

                lxb_css_syntax_token_escaped_set(tkz);

                data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                        lxb_css_syntax_state_hash_consume_name);
                if (data == end) {
                    return data;
                }

                data -= 1;
            }
            else if (*data == 0x00) {
                lxb_css_syntax_token_have_null_set(tkz);
            }
            else {
                lxb_css_syntax_token_hash(tkz->token)->end = data;

                lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
                lxb_css_syntax_state_token_done_m(tkz, end);

                return data;
            }
        }
    }

    return data;
}

static const lxb_char_t *
lxb_css_syntax_state_hash_consume_name_escape(lxb_css_syntax_tokenizer_t *tkz,
                                              const lxb_char_t *data,
                                              const lxb_char_t *end)
{
    /*
     * U+000A LINE FEED (LF)
     * U+000C FORM FEED (FF)
     * U+000D CARRIAGE RETURN (CR)
     */
    if (*data == 0x0A || *data == 0x0C || *data == 0x0D || tkz->is_eof) {
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
        lxb_css_syntax_state_token_done_m(tkz, end);

        return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->end);
    }
    else if (*data == 0x00) {
        lxb_css_syntax_token_have_null_set(tkz);
    }

    lxb_css_syntax_token_escaped_set(tkz);

    data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                        lxb_css_syntax_state_hash_consume_name);
    if (data == end) {
        return data;
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_hash_consume_name);

    return data;
}

/*
 * U+0028 LEFT PARENTHESIS (()
 */
const lxb_char_t *
lxb_css_syntax_state_lparenthesis(lxb_css_syntax_tokenizer_t *tkz,
                                  const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_L_PARENTHESIS;

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
    lxb_css_syntax_state_token_done_m(tkz, end);

    return (data + 1);
}

/*
 * U+0029 RIGHT PARENTHESIS ())
 */
const lxb_char_t *
lxb_css_syntax_state_rparenthesis(lxb_css_syntax_tokenizer_t *tkz,
                                  const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS;

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
    lxb_css_syntax_state_token_done_m(tkz, end);

    return (data + 1);
}

/*
 * U+002B PLUS SIGN (+)
 */
const lxb_char_t *
lxb_css_syntax_state_plus(lxb_css_syntax_tokenizer_t *tkz,
                          const lxb_char_t *data, const lxb_char_t *end)
{
    /* Skip + */
    data += 1;

    if (data == end) {
        tkz->begin = data - 1;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_plus_check);

        return data;
    }

    /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
    if (*data >= 0x30 && *data <= 0x39) {
        tkz->numeric.is_negative = false;
        tkz->numeric.buf = tkz->numeric.data;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric);

        return data;
    }

    /* U+002E FULL STOP (.) */
    if (*data == 0x2E) {
        data += 1;

        if (data == end) {
            tkz->begin = data - 2;

            lxb_css_syntax_state_set(tkz,
                                     lxb_css_syntax_state_plus_check_digit);
            return data;
        }

        /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
        if (*data >= 0x30 && *data <= 0x39) {
            tkz->numeric.exponent = 0;
            tkz->numeric.is_negative = false;
            tkz->numeric.buf = tkz->numeric.data;

            lxb_css_syntax_state_set(tkz,
                                     lxb_css_syntax_consume_numeric_decimal);
            return data;
        }

        data -= 1;
    }

    return lxb_css_syntax_state_delim(tkz, (data - 1), end);
}

static const lxb_char_t *
lxb_css_syntax_state_plus_check(lxb_css_syntax_tokenizer_t *tkz,
                                const lxb_char_t *data, const lxb_char_t *end)
{
    if (tkz->is_eof) {
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_delim);

        return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->begin);
    }

    /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
    if (*data >= 0x30 && *data <= 0x39) {
        tkz->numeric.is_negative = false;
        tkz->numeric.buf = tkz->numeric.data;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric);

        return data;
    }

    /* U+002E FULL STOP (.) */
    if (*data == 0x2E) {
        data += 1;

        if (data == end) {
            lxb_css_syntax_state_set(tkz,
                                     lxb_css_syntax_state_plus_check_digit);
            return data;
        }

        /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
        if (*data >= 0x30 && *data <= 0x39) {
            tkz->numeric.exponent = 0;
            tkz->numeric.is_negative = false;
            tkz->numeric.buf = tkz->numeric.data;

            lxb_css_syntax_state_set(tkz,
                                     lxb_css_syntax_consume_numeric_decimal);
            return data;
        }
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_delim);

    return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->begin);
}

static const lxb_char_t *
lxb_css_syntax_state_plus_check_digit(lxb_css_syntax_tokenizer_t *tkz,
                                      const lxb_char_t *data,
                                      const lxb_char_t *end)
{
    if (tkz->is_eof) {
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_delim);

        return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->begin);
    }

    /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
    if (*data >= 0x30 && *data <= 0x39) {
        tkz->numeric.exponent = 0;
        tkz->numeric.is_negative = false;
        tkz->numeric.buf = tkz->numeric.data;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric_decimal);

        return data;
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_delim);

    return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->begin);
}

/*
 * U+002C COMMA (,)
 */
const lxb_char_t *
lxb_css_syntax_state_comma(lxb_css_syntax_tokenizer_t *tkz,
                           const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_COMMA;

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
    lxb_css_syntax_state_token_done_m(tkz, end);

    return (data + 1);
}

/*
 * U+002D HYPHEN-MINUS (-)
 */
const lxb_char_t *
lxb_css_syntax_state_minus(lxb_css_syntax_tokenizer_t *tkz,
                           const lxb_char_t *data, const lxb_char_t *end)
{
    /* Skip minus (-) */
    data += 1;

    /* Check for <number-token> */

    if (data == end) {
        tkz->begin = data - 1;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_minus_check);

        return data;
    }

    /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
    if (*data >= 0x30 && *data <= 0x39) {
        tkz->numeric.is_negative = true;
        tkz->numeric.buf = tkz->numeric.data;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric);

        return data;
    }

    /* U+002E FULL STOP (.) */
    if (*data == 0x2E) {
        data += 1;

        if (data == end) {
            tkz->begin = data - 2;

            lxb_css_syntax_state_set(tkz,
                                     lxb_css_syntax_state_minus_check_digit);
            return data;
        }

        /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
        if (*data >= 0x30 && *data <= 0x39) {
            tkz->numeric.exponent = 0;
            tkz->numeric.is_negative = true;
            tkz->numeric.buf = tkz->numeric.data;

            lxb_css_syntax_state_set(tkz,
                                     lxb_css_syntax_consume_numeric_decimal);
            return data;
        }

        data -= 1;
    }
    /* U+002D HYPHEN-MINUS (-) */
    else if (*data == 0x2D) {
        data += 1;

        /* Check for <CDC-token> */

        if (data == end) {
            tkz->begin = data - 2;

            lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_minus_check_cdc);

            return data;
        }

        /* U+003E GREATER-THAN SIGN (>) */
        if (*data == 0x3E) {
            lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_CDC;

            lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
            lxb_css_syntax_state_token_done_m(tkz, end);

            return (data + 1);
        }

        lxb_css_syntax_token_ident(tkz->token)->begin = data - 2;

        lxb_css_syntax_state_set(tkz,
                                 lxb_css_syntax_consume_ident_like_not_url);
        return data;
    }

    /* Check for <ident-token> */

    if (lxb_css_syntax_res_name_map[*data] == LXB_CSS_SYNTAX_RES_NAME_START) {
        lxb_css_syntax_token_ident(tkz->token)->begin = data - 1;

        lxb_css_syntax_state_set(tkz,
                                 lxb_css_syntax_consume_ident_like_not_url);
        return (data + 1);
    }
    /* U+005C REVERSE SOLIDUS (\) */
    else if (*data == 0x5C) {
        data += 1;

        if (data == end) {
            tkz->begin = data - 2;

            lxb_css_syntax_state_set(tkz,
                                     lxb_css_syntax_state_minus_check_solidus);
            return data;
        }

        if (*data != 0x0A && *data != 0x0C && *data != 0x0D) {
            if (*data == 0x00) {
                lxb_css_syntax_token_have_null_set(tkz);
            }

            lxb_css_syntax_token_escaped_set(tkz);
            lxb_css_syntax_token_ident(tkz->token)->begin = data - 2;

            data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                        lxb_css_syntax_state_hash_consume_name);
            if (data == end) {
                return data;
            }

            lxb_css_syntax_state_set(tkz,
                                     lxb_css_syntax_consume_ident_like_not_url);
            return data;
        }

        data -= 1;
    }
    /* U+0000 NULL (\0) */
    else if (*data == 0x00) {
        lxb_css_syntax_token_have_null_set(tkz);
        lxb_css_syntax_token_ident(tkz->token)->begin = data - 1;

        lxb_css_syntax_state_set(tkz,
                                 lxb_css_syntax_consume_ident_like_not_url);
        return (data + 1);
    }

    return lxb_css_syntax_state_delim(tkz, (data - 1), end);
}

static const lxb_char_t *
lxb_css_syntax_state_minus_check(lxb_css_syntax_tokenizer_t *tkz,
                                 const lxb_char_t *data, const lxb_char_t *end)
{
    if (tkz->is_eof) {
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_delim);

        return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->begin);
    }

    /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
    if (*data >= 0x30 && *data <= 0x39) {
        tkz->numeric.is_negative = true;
        tkz->numeric.buf = tkz->numeric.data;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric);

        return data;
    }

    /* U+002E FULL STOP (.) */
    if (*data == 0x2E) {
        data += 1;

        if (data == end) {
            lxb_css_syntax_state_set(tkz,
                                     lxb_css_syntax_state_minus_check_digit);
            return data;
        }

        /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
        if (*data >= 0x30 && *data <= 0x39) {
            tkz->numeric.exponent = 0;
            tkz->numeric.is_negative = true;
            tkz->numeric.buf = tkz->numeric.data;

            lxb_css_syntax_state_set(tkz,
                                     lxb_css_syntax_consume_numeric_decimal);
            return data;
        }

        data -= 1;
    }
    /* U+002D HYPHEN-MINUS (-) */
    else if (*data == 0x2D) {
        data += 1;

        /* Check for <CDC-token> */

        if (data == end) {
            lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_minus_check_cdc);

            return data;
        }

        /* U+003E GREATER-THAN SIGN (>) */
        if (*data == 0x3E) {
            lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_CDC;

            lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
            lxb_css_syntax_state_token_done_m(tkz, end);

            return (data + 1);
        }

        lxb_css_syntax_token_ident(tkz->token)->begin = tkz->begin;

        lxb_css_syntax_state_set(tkz,
                                 lxb_css_syntax_consume_ident_like_not_url);
        return data;
    }

    /* Check for <ident-token> */

    if (lxb_css_syntax_res_name_map[*data] == LXB_CSS_SYNTAX_RES_NAME_START) {
        lxb_css_syntax_token_ident(tkz->token)->begin = tkz->begin;

        lxb_css_syntax_state_set(tkz,
                                 lxb_css_syntax_consume_ident_like_not_url);
        return (data + 1);
    }
    /* U+005C REVERSE SOLIDUS (\) */
    else if (*data == 0x5C) {
        data += 1;

        if (data == end) {
            lxb_css_syntax_state_set(tkz,
                                     lxb_css_syntax_state_minus_check_solidus);
            return data;
        }

        if (*data != 0x0A && *data != 0x0C && *data != 0x0D) {
            if (*data == 0x00) {
                lxb_css_syntax_token_have_null_set(tkz);
            }

            lxb_css_syntax_token_escaped_set(tkz);
            lxb_css_syntax_token_ident(tkz->token)->begin = tkz->begin;

            data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                        lxb_css_syntax_state_hash_consume_name);
            if (data == end) {
                return data;
            }

            lxb_css_syntax_state_set(tkz,
                                     lxb_css_syntax_consume_ident_like_not_url);
            return data;
        }

        data -= 1;
    }
    /* U+0000 NULL (\0) */
    else if (*data == 0x00) {
        lxb_css_syntax_token_have_null_set(tkz);
        lxb_css_syntax_token_ident(tkz->token)->begin = tkz->begin;

        lxb_css_syntax_state_set(tkz,
                                 lxb_css_syntax_consume_ident_like_not_url);
        return (data + 1);
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_delim);

    return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->begin);
}

static const lxb_char_t *
lxb_css_syntax_state_minus_check_digit(lxb_css_syntax_tokenizer_t *tkz,
                                       const lxb_char_t *data,
                                       const lxb_char_t *end)
{
    if (tkz->is_eof) {
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_delim);

        return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->begin);
    }

    /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
    if (*data >= 0x30 && *data <= 0x39) {
        tkz->numeric.exponent = 0;
        tkz->numeric.is_negative = true;
        tkz->numeric.buf = tkz->numeric.data;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric_decimal);

        return data;
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_delim);

    return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->begin);
}

static const lxb_char_t *
lxb_css_syntax_state_minus_check_cdc(lxb_css_syntax_tokenizer_t *tkz,
                                     const lxb_char_t *data,
                                     const lxb_char_t *end)
{
    if (tkz->is_eof) {
        lxb_css_syntax_state_set(tkz,
                                 lxb_css_syntax_consume_ident_like_not_url);

        return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->begin);
    }

    /* U+003E GREATER-THAN SIGN (>) */
    if (*data == 0x3E) {
        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_CDC;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
        lxb_css_syntax_state_token_done_m(tkz, end);

        return (data + 1);
    }

    lxb_css_syntax_token_ident(tkz->token)->begin = tkz->begin;
    lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_ident_like_not_url);

    return data;
}

static const lxb_char_t *
lxb_css_syntax_state_minus_check_solidus(lxb_css_syntax_tokenizer_t *tkz,
                                         const lxb_char_t *data,
                                         const lxb_char_t *end)
{
    if (*data == 0x0A || *data == 0x0C || *data == 0x0D || tkz->is_eof) {
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_delim);

        return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->begin);
    }
    else if (*data == 0x00) {
        lxb_css_syntax_token_have_null_set(tkz);
    }

    lxb_css_syntax_token_escaped_set(tkz);
    lxb_css_syntax_token_ident(tkz->token)->begin = tkz->begin;

    data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                        lxb_css_syntax_state_hash_consume_name);
    if (data == end) {
        return data;
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_ident_like_not_url);

    return data;
}

/*
 * U+002E FULL STOP (.)
 */
const lxb_char_t *
lxb_css_syntax_state_full_stop(lxb_css_syntax_tokenizer_t *tkz,
                               const lxb_char_t *data, const lxb_char_t *end)
{
    data += 1;

    if (data == end) {
        tkz->end = data - 1;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_full_stop_num);

        return end;
    }

    /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
    if (*data >= 0x30 && *data <= 0x39) {
        tkz->numeric.exponent = 0;
        tkz->numeric.buf = tkz->numeric.data;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric_decimal);

        return data;
    }

    return lxb_css_syntax_state_delim(tkz, (data - 1), end);
}

static const lxb_char_t *
lxb_css_syntax_state_full_stop_num(lxb_css_syntax_tokenizer_t *tkz,
                                   const lxb_char_t *data, const lxb_char_t *end)
{
    if (tkz->is_eof) {
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_delim);

        return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->end);
    }

    /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
    if (*data >= 0x30 && *data <= 0x39) {
        tkz->numeric.exponent = 0;
        tkz->numeric.buf = tkz->numeric.data;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric_decimal);

        return data;
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_delim);

    return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->end);
}

/*
 * U+003A COLON (:)
 */
const lxb_char_t *
lxb_css_syntax_state_colon(lxb_css_syntax_tokenizer_t *tkz,
                           const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_COLON;

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
    lxb_css_syntax_state_token_done_m(tkz, end);

    return (data + 1);
}

/*
 * U+003B SEMICOLON (;)
 */
const lxb_char_t *
lxb_css_syntax_state_semicolon(lxb_css_syntax_tokenizer_t *tkz,
                               const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_SEMICOLON;

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
    lxb_css_syntax_state_token_done_m(tkz, end);

    return (data + 1);
}

/*
 * U+003C LESS-THAN SIGN (<)
 */
const lxb_char_t *
lxb_css_syntax_state_less_sign(lxb_css_syntax_tokenizer_t *tkz,
                               const lxb_char_t *data, const lxb_char_t *end)
{
    data += 1;

    if ((end - data) > 2) {
        if (memcmp(data, "!--", sizeof(lxb_char_t) * 3) == 0) {
            lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_CDO;

            lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
            lxb_css_syntax_state_token_done_m(tkz, end);

            return (data + 3);
        }

        return lxb_css_syntax_state_delim(tkz, (data - 1), end);
    }

    if (data == end) {
        tkz->end = data - 1;

        lxb_css_syntax_state_set(tkz,
                                 lxb_css_syntax_state_less_sign_check_exmark);
        return data;
    }

    /* U+0021 EXCLAMATION MARK */
    if (*data != 0x21) {
        return lxb_css_syntax_state_delim(tkz, (data - 1), end);
    }

    data += 1;

    if (data == end) {
        tkz->end = data - 2;

        lxb_css_syntax_state_set(tkz,
                                 lxb_css_syntax_state_less_sign_check_fminus);
        return data;
    }

    /* U+002D HYPHEN-MINUS */
    if (*data != 0x2D) {
        return lxb_css_syntax_state_delim(tkz, (data - 2), end);
    }

    tkz->end = data - 2;

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_less_sign_check_tminus);

    return (data + 1);
}

static const lxb_char_t *
lxb_css_syntax_state_less_sign_check_exmark(lxb_css_syntax_tokenizer_t *tkz,
                                            const lxb_char_t *data,
                                            const lxb_char_t *end)
{
    /* U+0021 EXCLAMATION MARK */
    if (tkz->is_eof || *data != 0x21) {
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_delim);

        return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->end);
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_less_sign_check_fminus);

    return (data + 1);
}

static const lxb_char_t *
lxb_css_syntax_state_less_sign_check_fminus(lxb_css_syntax_tokenizer_t *tkz,
                                            const lxb_char_t *data,
                                            const lxb_char_t *end)
{
    /* U+002D HYPHEN-MINUS */
    if (tkz->is_eof || *data != 0x2D) {
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_delim);

        return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->end);
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_less_sign_check_tminus);

    return (data + 1);
}

static const lxb_char_t *
lxb_css_syntax_state_less_sign_check_tminus(lxb_css_syntax_tokenizer_t *tkz,
                                            const lxb_char_t *data,
                                            const lxb_char_t *end)
{
    /* U+002D HYPHEN-MINUS */
    if (tkz->is_eof || *data != 0x2D) {
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_delim);

        return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->end);
    }

    lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_CDO;

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
    lxb_css_syntax_state_token_done_m(tkz, end);

    return (data + 1);
}

/*
 * U+0040 COMMERCIAL AT (@)
 */
const lxb_char_t *
lxb_css_syntax_state_at(lxb_css_syntax_tokenizer_t *tkz,
                        const lxb_char_t *data, const lxb_char_t *end)
{
    data += 1;

    if (data == end) {
        tkz->end = data - 1;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_at_begin);

        return data;
    }

    lxb_css_syntax_token_at_keyword(tkz->token)->begin = data;

    if (lxb_css_syntax_res_name_map[*data] != LXB_CSS_SYNTAX_RES_NAME_START) {
        /* U+002D HYPHEN-MINUS */
        if (*data == 0x2D) {
            data += 1;

            if (data == end) {
                tkz->end = data - 2;

                lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_at_minus);

                return data;
            }

            if (lxb_css_syntax_res_name_map[*data] != LXB_CSS_SYNTAX_RES_NAME_START) {
                /* U+005C REVERSE SOLIDUS (\) */
                if (*data == 0x5C) {
                    data += 1;

                    if (data == end) {
                        tkz->end = data - 3;

                        lxb_css_syntax_state_set(tkz,
                                                 lxb_css_syntax_state_at_escape);
                        return data;
                    }

                    /*
                     * U+000A LINE FEED (LF)
                     * U+000C FORM FEED (FF)
                     * U+000D CARRIAGE RETURN (CR)
                     */
                    if (*data == 0x0A || *data == 0x0C || *data == 0x0D) {
                        return lxb_css_syntax_state_delim(tkz, (data - 3), end);
                    }
                    else if (*data == 0x00) {
                        lxb_css_syntax_token_have_null_set(tkz);
                    }

                    lxb_css_syntax_token_escaped_set(tkz);

                    data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                                  lxb_css_syntax_state_at_name);
                    if (data == end) {
                        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_AT_KEYWORD;

                        return data;
                    }
                }
                else if (*data == 0x00) {
                    lxb_css_syntax_token_have_null_set(tkz);
                }
                else if (*data != 0x2D) {
                    return lxb_css_syntax_state_delim(tkz, (data - 2), end);
                }
            }
        }
        /* U+005C REVERSE SOLIDUS (\) */
        else if (*data == 0x5C) {
            data += 1;

            if (data == end) {
                tkz->end = data - 2;

                lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_at_escape);

                return data;
            }

            if (*data == 0x0A || *data == 0x0C || *data == 0x0D) {
                return lxb_css_syntax_state_delim(tkz, (data - 2), end);
            }
            else if (*data == 0x00) {
                lxb_css_syntax_token_have_null_set(tkz);
            }

            lxb_css_syntax_token_escaped_set(tkz);

            data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                                  lxb_css_syntax_state_at_name);
            if (data == end) {
                lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_AT_KEYWORD;

                return data;
            }
        }
        else if (*data == 0x00) {
            lxb_css_syntax_token_have_null_set(tkz);
        }
        else {
            return lxb_css_syntax_state_delim(tkz, (data - 1), end);
        }
    }

    lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_AT_KEYWORD;

    for (; data < end; data++) {
        if (lxb_css_syntax_res_name_map[*data] == 0x00) {
            /* U+005C REVERSE SOLIDUS (\) */
            if (*data == 0x5C) {
                data += 1;

                if (data == end) {
                    tkz->end = data - 1;
                    lxb_css_syntax_token_at_keyword(tkz->token)->end = tkz->end;

                    lxb_css_syntax_state_set(tkz,
                                             lxb_css_syntax_state_at_name_escape);
                    return data;
                }

                /*
                 * U+000A LINE FEED (LF)
                 * U+000C FORM FEED (FF)
                 * U+000D CARRIAGE RETURN (CR)
                 */
                if (*data == 0x0A || *data == 0x0C || *data == 0x0D) {
                    data -= 1;

                    lxb_css_syntax_token_at_keyword(tkz->token)->end = data;

                    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
                    lxb_css_syntax_state_token_done_m(tkz, end);

                    return data;
                }
                else if (*data == 0x00) {
                    lxb_css_syntax_token_have_null_set(tkz);
                }

                lxb_css_syntax_token_escaped_set(tkz);

                data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                                  lxb_css_syntax_state_at_name);
                if (data == end) {
                    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_at_name);

                    return data;
                }

                data -= 1;

                continue;
            }
            else if (*data == 0x00) {
                lxb_css_syntax_token_have_null_set(tkz);
            }
            else {
                lxb_css_syntax_token_at_keyword(tkz->token)->end = data;

                lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
                lxb_css_syntax_state_token_done_m(tkz, end);

                return data;
            }
        }
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_at_name);

    return data;
}

static const lxb_char_t *
lxb_css_syntax_state_at_begin(lxb_css_syntax_tokenizer_t *tkz,
                              const lxb_char_t *data, const lxb_char_t *end)
{
    if (tkz->is_eof) {
        goto consume_delim;
    }

    lxb_css_syntax_token_at_keyword(tkz->token)->begin = data;

    if (lxb_css_syntax_res_name_map[*data] != LXB_CSS_SYNTAX_RES_NAME_START) {
        /* U+002D HYPHEN-MINUS */
        if (*data == 0x2D) {
            data += 1;

            if (data == end) {
                lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_at_minus);

                return data;
            }

            if (lxb_css_syntax_res_name_map[*data] != LXB_CSS_SYNTAX_RES_NAME_START) {
                /* U+005C REVERSE SOLIDUS (\) */
                if (*data == 0x5C) {
                    data += 1;

                    if (data == end) {
                        lxb_css_syntax_state_set(tkz,
                                                 lxb_css_syntax_state_at_escape);
                        return data;
                    }

                    /*
                     * U+000A LINE FEED (LF)
                     * U+000C FORM FEED (FF)
                     * U+000D CARRIAGE RETURN (CR)
                     */
                    if (*data == 0x0A || *data == 0x0C || *data == 0x0D) {
                        goto consume_delim;
                    }
                    else if (*data == 0x00) {
                        lxb_css_syntax_token_have_null_set(tkz);
                    }

                    lxb_css_syntax_token_escaped_set(tkz);

                    data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                                  lxb_css_syntax_state_at_name);
                    if (data == end) {
                        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_AT_KEYWORD;

                        return data;
                    }

                    data -= 1;
                }
                else if (*data == 0x00) {
                    lxb_css_syntax_token_have_null_set(tkz);
                }
                else if (*data != 0x2D) {
                    goto consume_delim;
                }
            }
        }
        /* U+005C REVERSE SOLIDUS (\) */
        else if (*data == 0x5C) {
            data += 1;

            if (data == end) {
                lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_at_escape);

                return data;
            }

            if (*data == 0x0A || *data == 0x0C || *data == 0x0D) {
                goto consume_delim;
            }
            else if (*data == 0x00) {
                lxb_css_syntax_token_have_null_set(tkz);
            }

            lxb_css_syntax_token_escaped_set(tkz);

            data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                                  lxb_css_syntax_state_at_name);
            if (data == end) {
                lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_AT_KEYWORD;

                return data;
            }

            data -= 1;
        }
        else if (*data == 0x00) {
            lxb_css_syntax_token_have_null_set(tkz);
        }
        else {
            goto consume_delim;
        }
    }

    lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_AT_KEYWORD;
    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_at_name);

    return (data + 1);


consume_delim:

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_delim);

    return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->end);
}

static const lxb_char_t *
lxb_css_syntax_state_at_minus(lxb_css_syntax_tokenizer_t *tkz,
                              const lxb_char_t *data, const lxb_char_t *end)
{
    if (tkz->is_eof) {
        goto consume_delim;
    }

    if (lxb_css_syntax_res_name_map[*data] != LXB_CSS_SYNTAX_RES_NAME_START) {
        /* U+005C REVERSE SOLIDUS (\) */
        if (*data == 0x5C) {
            data += 1;

            if (data == end) {
                lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_at_escape);

                return data;
            }

            /*
             * U+000A LINE FEED (LF)
             * U+000C FORM FEED (FF)
             * U+000D CARRIAGE RETURN (CR)
             */
            if (*data == 0x0A || *data == 0x0C || *data == 0x0D) {
                goto consume_delim;
            }
            else if (*data == 0x00) {
                lxb_css_syntax_token_have_null_set(tkz);
            }

            lxb_css_syntax_token_escaped_set(tkz);

            data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                                  lxb_css_syntax_state_at_name);
            if (data == end) {
                lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_AT_KEYWORD;

                return data;
            }

            data -= 1;
        }
        else if (*data == 0x00) {
            lxb_css_syntax_token_have_null_set(tkz);
        }
        /* U+002D HYPHEN-MINUS */
        else if (*data != 0x2D) {
            goto consume_delim;
        }
    }

    lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_AT_KEYWORD;
    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_at_name);

    return (data + 1);


consume_delim:

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_delim);

    return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->end);
}

static const lxb_char_t *
lxb_css_syntax_state_at_escape(lxb_css_syntax_tokenizer_t *tkz,
                               const lxb_char_t *data, const lxb_char_t *end)
{
    /*
     * U+000A LINE FEED (LF)
     * U+000C FORM FEED (FF)
     * U+000D CARRIAGE RETURN (CR)
     */
    if (*data == 0x0A || *data == 0x0C || *data == 0x0D || tkz->is_eof) {
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_delim);

        return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->end);
    }
    else if (*data == 0x00) {
        lxb_css_syntax_token_have_null_set(tkz);
    }

    lxb_css_syntax_token_escaped_set(tkz);

    data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                              lxb_css_syntax_state_at_name);
    if (data == end) {
        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_AT_KEYWORD;

        return data;
    }

    lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_AT_KEYWORD;
    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_at_name);

    return data;
}

static const lxb_char_t *
lxb_css_syntax_state_at_name(lxb_css_syntax_tokenizer_t *tkz,
                             const lxb_char_t *data, const lxb_char_t *end)
{
    if (tkz->is_eof) {
        lxb_css_syntax_token_at_keyword(tkz->token)->end = tkz->incoming_node->end;
        lxb_css_syntax_state_token_done_m(tkz, end);

        return end;
    }

    for (; data < end; data++) {
        if (lxb_css_syntax_res_name_map[*data] == 0x00) {
            /* U+005C REVERSE SOLIDUS (\) */
            if (*data == 0x5C) {
                data += 1;

                if (data == end) {
                    tkz->end = data - 1;
                    lxb_css_syntax_token_at_keyword(tkz->token)->end = tkz->end;

                    lxb_css_syntax_state_set(tkz,
                                             lxb_css_syntax_state_at_name_escape);
                    return data;
                }

                /*
                 * U+000A LINE FEED (LF)
                 * U+000C FORM FEED (FF)
                 * U+000D CARRIAGE RETURN (CR)
                 */
                if (*data == 0x0A || *data == 0x0C || *data == 0x0D) {
                    data -= 1;

                    lxb_css_syntax_token_at_keyword(tkz->token)->end = data;

                    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
                    lxb_css_syntax_state_token_done_m(tkz, end);

                    return data;
                }
                else if (*data == 0x00) {
                    lxb_css_syntax_token_have_null_set(tkz);
                }

                lxb_css_syntax_token_escaped_set(tkz);

                data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                                  lxb_css_syntax_state_at_name);
                if (data == end) {
                    return data;
                }

                data -= 1;
            }
            else if (*data == 0x00) {
                lxb_css_syntax_token_have_null_set(tkz);
            }
            else {
                lxb_css_syntax_token_at_keyword(tkz->token)->end = data;

                lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
                lxb_css_syntax_state_token_done_m(tkz, end);

                return data;
            }
        }
    }

    return data;
}

static const lxb_char_t *
lxb_css_syntax_state_at_name_escape(lxb_css_syntax_tokenizer_t *tkz,
                                    const lxb_char_t *data,
                                    const lxb_char_t *end)
{
    /*
     * U+000A LINE FEED (LF)
     * U+000C FORM FEED (FF)
     * U+000D CARRIAGE RETURN (CR)
     */
    if (*data == 0x0A || *data == 0x0C || *data == 0x0D || tkz->is_eof) {
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
        lxb_css_syntax_state_token_done_m(tkz, end);

        return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->end);
    }
    else if (*data == 0x00) {
        lxb_css_syntax_token_have_null_set(tkz);
    }

    lxb_css_syntax_token_escaped_set(tkz);

    data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                              lxb_css_syntax_state_at_name);
    if (data == end) {
        return data;
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_at_name);

    return (data + 1);
}

/*
 * U+005B LEFT SQUARE BRACKET ([)
 */
const lxb_char_t *
lxb_css_syntax_state_ls_bracket(lxb_css_syntax_tokenizer_t *tkz,
                                const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_LS_BRACKET;

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
    lxb_css_syntax_state_token_done_m(tkz, end);

    return (data + 1);
}

/*
 * U+005C REVERSE SOLIDUS (\)
 */
const lxb_char_t *
lxb_css_syntax_state_rsolidus(lxb_css_syntax_tokenizer_t *tkz,
                              const lxb_char_t *data, const lxb_char_t *end)
{
    data += 1;

    if (data == end) {
        tkz->end = data - 1;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_rsolidus_check);

        return data;
    }

    if (*data == 0x0A || *data == 0x0C || *data == 0x0D) {
        return lxb_css_syntax_state_delim(tkz, (data - 1), end);
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_ident_like);

    return (data - 1);
}

static const lxb_char_t *
lxb_css_syntax_state_rsolidus_check(lxb_css_syntax_tokenizer_t *tkz,
                                    const lxb_char_t *data,
                                    const lxb_char_t *end)
{
    /*
     * U+000A LINE FEED (LF)
     * U+000C FORM FEED (FF)
     * U+000D CARRIAGE RETURN (CR)
     */
    if ((*data == 0x0A || *data == 0x0C || *data == 0x0D) || tkz->is_eof) {
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_delim);

        return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->end);
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_ident_like);

    return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->end);
}

/*
 * U+005D RIGHT SQUARE BRACKET (])
 */
const lxb_char_t *
lxb_css_syntax_state_rs_bracket(lxb_css_syntax_tokenizer_t *tkz,
                                const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_RS_BRACKET;

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
    lxb_css_syntax_state_token_done_m(tkz, end);

    return (data + 1);
}

/*
 * U+007B LEFT CURLY BRACKET ({)
 */
const lxb_char_t *
lxb_css_syntax_state_lc_bracket(lxb_css_syntax_tokenizer_t *tkz,
                                const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_LC_BRACKET;

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
    lxb_css_syntax_state_token_done_m(tkz, end);

    return (data + 1);
}

/*
 * U+007D RIGHT CURLY BRACKET (})
 */
const lxb_char_t *
lxb_css_syntax_state_rc_bracket(lxb_css_syntax_tokenizer_t *tkz,
                                const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_RC_BRACKET;

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
    lxb_css_syntax_state_token_done_m(tkz, end);

    return (data + 1);
}


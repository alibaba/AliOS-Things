/*
 * Copyright (C) 2018-2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "syntax/consume.h"
#include "syntax/state.h"
#include "syntax/tokenizer/error.h"

#define LXB_CSS_SYNTAX_RES_NAME_MAP
#include "syntax/res.h"

#define LEXBOR_STR_RES_MAP_HEX
#define LEXBOR_STR_RES_MAP_LOWERCASE
#include "core/str_res.h"
#include "core/strtod.h"

#include "core/utils.h"


static const lxb_char_t lxb_css_syntax_consume_url_ch[] = "url";
static const lxb_char_t *lxb_css_syntax_consume_url_ch_end =
    lxb_css_syntax_consume_url_ch + sizeof(lxb_css_syntax_consume_url_ch) - 1;

static const lxb_char_t *
lxb_css_syntax_consume_string_solidus(lxb_css_syntax_tokenizer_t *tkz,
                                      const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_consume_string_solidus_n(lxb_css_syntax_tokenizer_t *tkz,
                                        const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_consume_numeric_e(lxb_css_syntax_tokenizer_t *tkz,
                                 const lxb_char_t *data,
                                 const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_consume_numeric_e_digit(lxb_css_syntax_tokenizer_t *tkz,
                                       const lxb_char_t *data,
                                       const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_consume_numeric_e_digits(lxb_css_syntax_tokenizer_t *tkz,
                                        const lxb_char_t *data,
                                        const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_consume_numeric_name_start(lxb_css_syntax_tokenizer_t *tkz,
                                          const lxb_char_t *data,
                                          const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_consume_numeric_name_start_minus(lxb_css_syntax_tokenizer_t *tkz,
                                                const lxb_char_t *data,
                                                const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_consume_numeric_before_name_escape(lxb_css_syntax_tokenizer_t *tkz,
                                                  const lxb_char_t *data,
                                                  const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_consume_numeric_name(lxb_css_syntax_tokenizer_t *tkz,
                                    const lxb_char_t *data,
                                    const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_consume_numeric_name_escape(lxb_css_syntax_tokenizer_t *tkz,
                                           const lxb_char_t *data,
                                           const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_consume_ident_like_name(lxb_css_syntax_tokenizer_t *tkz,
                                       const lxb_char_t *data,
                                       const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_consume_ident_like_escape(lxb_css_syntax_tokenizer_t *tkz,
                                         const lxb_char_t *data,
                                         const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_consume_ident_like_solidus_data(lxb_css_syntax_tokenizer_t *tkz,
                                               const lxb_char_t *data,
                                               const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_consume_ident_like_is_function(lxb_css_syntax_tokenizer_t *tkz,
                                              const lxb_char_t *data,
                                              const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_consume_ident_like_before_check_url(lxb_css_syntax_tokenizer_t *tkz,
                                                   const lxb_char_t *data,
                                                   const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_consume_ident_like_check_url(lxb_css_syntax_tokenizer_t *tkz,
                                            const lxb_char_t *data,
                                            const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_consume_url(lxb_css_syntax_tokenizer_t *tkz,
                           const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_consume_url_escape(lxb_css_syntax_tokenizer_t *tkz,
                                  const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_consume_url_end(lxb_css_syntax_tokenizer_t *tkz,
                               const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_consume_bad_url(lxb_css_syntax_tokenizer_t *tkz,
                               const lxb_char_t *data, const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_consume_ident_like_not_url_escape(lxb_css_syntax_tokenizer_t *tkz,
                                                 const lxb_char_t *data,
                                                 const lxb_char_t *end);


/*
 * String
 */
const lxb_char_t *
lxb_css_syntax_consume_string(lxb_css_syntax_tokenizer_t *tkz,
                              const lxb_char_t *data, const lxb_char_t *end)
{
    if (tkz->is_eof) {
        lxb_css_syntax_tokenizer_error_add(tkz->parse_errors,
                                           tkz->incoming_node->end,
                                         LXB_CSS_SYNTAX_TOKENIZER_ERROR_EOINST);

        if (lxb_css_syntax_token_string(tkz->token)->begin == NULL) {
            lxb_css_syntax_token_string(tkz->token)->begin = tkz->incoming_node->end;
        }

        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_STRING;
        lxb_css_syntax_token_string(tkz->token)->end = tkz->incoming_node->end;

        lxb_css_syntax_state_token_done_m(tkz, end);

        return end;
    }

    if (lxb_css_syntax_token_string(tkz->token)->begin == NULL) {
        lxb_css_syntax_token_string(tkz->token)->begin = data;
    }

    for (; data != end; data++) {
        switch (*data) {
            case 0x00:
                lxb_css_syntax_token_have_null_set(tkz);

                break;

            /*
             * U+000A LINE FEED
             * U+000D CARRIAGE RETURN
             * U+000C FORM FEED
             */
            case 0x0A:
            case 0x0D:
            case 0x0C:
                lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_CSS_SYNTAX_TOKENIZER_ERROR_NEINST);

                lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_BAD_STRING;
                lxb_css_syntax_token_string(tkz->token)->end = data;

                lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
                lxb_css_syntax_state_token_done_m(tkz, end);

                return data;

            /* U+005C REVERSE SOLIDUS (\) */
            case 0x5C:
                data += 1;

                if (data == end) {
                    lxb_css_syntax_state_set(tkz,
                                         lxb_css_syntax_consume_string_solidus);
                    return data;
                }

                lxb_css_syntax_token_escaped_set(tkz);

                /* U+000D CARRIAGE RETURN */
                if (*data == 0x0D) {
                    data += 1;

                    lxb_css_syntax_token_cr_set(tkz);

                    if (data == end) {
                        lxb_css_syntax_state_set(tkz,
                                       lxb_css_syntax_consume_string_solidus_n);
                        return data;
                    }

                    /* U+000A LINE FEED */
                    if (*data != 0x0A) {
                        data -= 1;
                    }

                    break;
                }
                else if (*data == 0x00) {
                    lxb_css_syntax_token_have_null_set(tkz);
                }

                data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                                 lxb_css_syntax_consume_string);
                if (data == end) {
                    return data;
                }

                data -= 1;

                break;

            default:
                /* '"' or ''' */
                if (*data == tkz->str_ending) {
                    lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_STRING;
                    lxb_css_syntax_token_string(tkz->token)->end = data;

                    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
                    lxb_css_syntax_state_token_done_m(tkz, end);

                    return (data + 1);
                }

                break;
        }
    }

    return data;
}

static const lxb_char_t *
lxb_css_syntax_consume_string_solidus(lxb_css_syntax_tokenizer_t *tkz,
                                      const lxb_char_t *data, const lxb_char_t *end)
{
    if (tkz->is_eof) {
        /* Do nothing */
        return end;
    }

    /* U+000D CARRIAGE RETURN */
    if (*data == 0x0D) {
        data += 1;

        lxb_css_syntax_token_cr_set(tkz);

        if (data == end) {
            lxb_css_syntax_state_set(tkz,
                                     lxb_css_syntax_consume_string_solidus_n);
            return data;
        }

        /* U+000A LINE FEED */
        if (*data == 0x0A) {
            return (data + 1);
        }

        return data;
    }
    else if (*data == 0x00) {
        lxb_css_syntax_token_have_null_set(tkz);
    }

    lxb_css_syntax_token_escaped_set(tkz);

    data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                              lxb_css_syntax_consume_string);
    if (data == end) {
        return data;
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_string);

    return data;
}

static const lxb_char_t *
lxb_css_syntax_consume_string_solidus_n(lxb_css_syntax_tokenizer_t *tkz,
                                        const lxb_char_t *data,
                                        const lxb_char_t *end)
{
    lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_string);

    if (tkz->is_eof) {
        lxb_css_syntax_tokenizer_error_add(tkz->parse_errors,
                                           tkz->incoming_node->end,
                                         LXB_CSS_SYNTAX_TOKENIZER_ERROR_EOINST);

        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_STRING;
        lxb_css_syntax_token_string(tkz->token)->end = tkz->incoming_node->end;

        lxb_css_syntax_state_token_done_m(tkz, end);

        return end;
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_string);

    /* U+000A LINE FEED */
    if (*data == 0x0A) {
        return (data + 1);
    }

    return data;
}

/*
 * Numeric
 */
lxb_inline void
lxb_css_syntax_consume_numeric_set_int(lxb_css_syntax_tokenizer_t *tkz)
{
    double num = lexbor_strtod_internal(tkz->numeric.data,
                                        (tkz->numeric.buf - tkz->numeric.data),
                                        tkz->numeric.exponent);
    if (tkz->numeric.is_negative) {
        num = -num;
    }

    lxb_css_syntax_token_number(tkz->token)->is_float = false;
    lxb_css_syntax_token_number(tkz->token)->num = num;

    lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_NUMBER;
}

lxb_inline void
lxb_css_syntax_consume_numeric_set_float(lxb_css_syntax_tokenizer_t *tkz)
{
    if (tkz->numeric.e_is_negative) {
        tkz->numeric.exponent -= tkz->numeric.e_digit;
    }
    else {
        tkz->numeric.exponent += tkz->numeric.e_digit;
    }

    double num = lexbor_strtod_internal(tkz->numeric.data,
                                        (tkz->numeric.buf - tkz->numeric.data),
                                        tkz->numeric.exponent);
    if (tkz->numeric.is_negative) {
        num = -num;
    }

    lxb_css_syntax_token_number(tkz->token)->num = num;

    lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_NUMBER;
}

const lxb_char_t *
lxb_css_syntax_consume_before_numeric(lxb_css_syntax_tokenizer_t *tkz,
                                      const lxb_char_t *data,
                                      const lxb_char_t *end)
{
    tkz->numeric.buf = tkz->numeric.data;
    tkz->numeric.is_negative = false;

    return lxb_css_syntax_consume_numeric(tkz, data, end);
}

const lxb_char_t *
lxb_css_syntax_consume_numeric(lxb_css_syntax_tokenizer_t *tkz,
                               const lxb_char_t *data, const lxb_char_t *end)
{
    const lxb_char_t *begin;
    lxb_css_syntax_token_number_t *token;
    lxb_css_syntax_tokenizer_numeric_t *numeric;

    if (tkz->is_eof) {
        lxb_css_syntax_consume_numeric_set_int(tkz);

        lxb_css_syntax_state_token_done_m(tkz, end);

        return end;
    }

    token = (lxb_css_syntax_token_number_t *) tkz->token;
    numeric = &tkz->numeric;

    do {
        /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
        if (*data < 0x30 || *data > 0x39) {
            break;
        }

        if (numeric->buf != numeric->end) {
            *numeric->buf++ = *data;
        }

        data += 1;
        if (data == end) {
            lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric);

            return data;
        }
    }
    while (1);

    /* U+002E FULL STOP (.) */
    if (*data != 0x2E) {
        lxb_css_syntax_consume_numeric_set_int(tkz);

        lxb_css_syntax_token_dimension(tkz->token)->begin = data;

        return lxb_css_syntax_consume_numeric_name_start(tkz, data, end);
    }

    /* Decimal */
    data += 1;
    begin = data;

    numeric->exponent = 0;

    /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
    for (; data < end && (*data >= 0x30 && *data <= 0x39); data++) {
        if (numeric->buf != numeric->end) {
            *numeric->buf++ = *data;
            numeric->exponent -= 1;
        }
    }

    if (data == end) {
        tkz->begin = begin - 1;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric_decimal);

        return data;
    }

    if (numeric->exponent == 0) {
        lxb_css_syntax_consume_numeric_set_int(tkz);

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
        lxb_css_syntax_state_token_done_m(tkz, end);

        return (begin - 1);
    }

    token->is_float = true;
    numeric->e_digit = 0;

    /* U+0045 Latin Capital Letter (E) or U+0065 Latin Small Letter (e) */
    if (*data != 0x45 && *data != 0x65) {
        lxb_css_syntax_consume_numeric_set_float(tkz);

        lxb_css_syntax_token_dimension(tkz->token)->begin = data;

        return lxb_css_syntax_consume_numeric_name_start(tkz, data, end);
    }

    data += 1;
    numeric->e_is_negative = false;

    if (data == end) {
        lxb_css_syntax_token_dimension(tkz->token)->begin = data - 1;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric_e);

        return data;
    }

    switch (*data) {
        /* U+002D HYPHEN-MINUS (-) */
        case 0x2D:
            numeric->e_is_negative = true;
            /* fall through */

        /* U+002B PLUS SIGN (+) */
        case 0x2B:
            data += 1;

            if (data == end) {
                lxb_css_syntax_token_dimension(tkz->token)->begin = data - 2;

                lxb_css_syntax_state_set(tkz,
                                        lxb_css_syntax_consume_numeric_e_digit);
                return data;
            }

            /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
            if (*data < 0x30 || *data > 0x39) {
                data -= 2;

                lxb_css_syntax_consume_numeric_set_float(tkz);

                lxb_css_syntax_token_dimension(tkz->token)->begin = data;
                lxb_css_syntax_state_set(tkz,
                                         lxb_css_syntax_consume_numeric_name);
                return data;
            }

            break;

        default:
            /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
            if (*data < 0x30 || *data > 0x39) {
                data -= 1;

                lxb_css_syntax_consume_numeric_set_float(tkz);

                lxb_css_syntax_token_dimension(tkz->token)->begin = data;
                lxb_css_syntax_state_set(tkz,
                                         lxb_css_syntax_consume_numeric_name);
                return data;
            }
    }

    /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
    for (; data < end && (*data >= 0x30 && *data <= 0x39); data++) {
        numeric->e_digit = (*data - 0x30) + numeric->e_digit * 0x0A;
    }

    if (data == end) {
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric_e_digits);

        return data;
    }

    lxb_css_syntax_consume_numeric_set_float(tkz);

    lxb_css_syntax_token_dimension(tkz->token)->begin = data;
    lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric_name_start);

    return data;
}

const lxb_char_t *
lxb_css_syntax_consume_numeric_decimal(lxb_css_syntax_tokenizer_t *tkz,
                                       const lxb_char_t *data,
                                       const lxb_char_t *end)
{
    lxb_css_syntax_token_number_t *token;
    lxb_css_syntax_tokenizer_numeric_t *numeric;

    token = (lxb_css_syntax_token_number_t *) tkz->token;
    numeric = &tkz->numeric;

    if (tkz->is_eof) {
        if (numeric->exponent == 0) {
            goto data_state;
        }

        token->is_float = true;

        lxb_css_syntax_consume_numeric_set_float(tkz);

        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_NUMBER;
        lxb_css_syntax_state_token_done_m(tkz, end);

        return end;
    }

    do {
        if (*data < 0x30 || *data > 0x39) {
            break;
        }

        if (numeric->buf != numeric->end) {
            *numeric->buf++ = *data;
            numeric->exponent -= 1;
        }

        data += 1;
        if (data == end) {
            return data;
        }
    }
    while (1);

    if (numeric->exponent == 0) {
        goto data_state;
    }

    token->is_float = true;
    numeric->e_digit = 0;

    lxb_css_syntax_token_dimension(tkz->token)->begin = data;

    /* U+0045 Latin Capital Letter (E) or U+0065 Latin Small Letter (e) */
    if (*data == 0x45 || *data == 0x65) {
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric_e);

        return (data + 1);
    }

    lxb_css_syntax_consume_numeric_set_float(tkz);

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric_name_start);

    return data;


data_state:

    lxb_css_syntax_consume_numeric_set_int(tkz);

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
    lxb_css_syntax_state_token_done_m(tkz, end);

    return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->begin);
}

static const lxb_char_t *
lxb_css_syntax_consume_numeric_e(lxb_css_syntax_tokenizer_t *tkz,
                                 const lxb_char_t *data,
                                 const lxb_char_t *end)
{
    /* U+002B PLUS SIGN (+) or U+002D HYPHEN-MINUS (-) */
    if (*data == 0x2B) {
        tkz->numeric.e_is_negative = false;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric_e_digit);

        return (data + 1);
    }
    else if (*data == 0x2D) {
        tkz->numeric.e_is_negative = true;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric_e_digit);

        return (data + 1);
    }
    /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
    else if (*data >= 0x30 && *data <= 0x39) {
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric_e_digits);

        return data;
    }

    lxb_css_syntax_consume_numeric_set_float(tkz);

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric_name_start);

    return lxb_css_syntax_tokenizer_change_incoming(tkz,
                             lxb_css_syntax_token_dimension(tkz->token)->begin);
}

static const lxb_char_t *
lxb_css_syntax_consume_numeric_e_digit(lxb_css_syntax_tokenizer_t *tkz,
                                       const lxb_char_t *data,
                                       const lxb_char_t *end)
{
    /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
    if (*data >= 0x30 && *data <= 0x39) {
        tkz->numeric.e_digit = 0;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric_e_digits);

        return data;
    }

    lxb_css_syntax_consume_numeric_set_float(tkz);

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric_name_start);

    return lxb_css_syntax_tokenizer_change_incoming(tkz,
                             lxb_css_syntax_token_dimension(tkz->token)->begin);
}

static const lxb_char_t *
lxb_css_syntax_consume_numeric_e_digits(lxb_css_syntax_tokenizer_t *tkz,
                                        const lxb_char_t *data,
                                        const lxb_char_t *end)
{
    if (tkz->is_eof) {
        lxb_css_syntax_consume_numeric_set_float(tkz);

        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_NUMBER;
        lxb_css_syntax_state_token_done_m(tkz, end);

        return end;
    }

    do {
        /* U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) */
        if (*data < 0x30 || *data > 0x39) {
            break;
        }

        tkz->numeric.e_digit = (*data - 0x30) + tkz->numeric.e_digit * 0x0A;

        data += 1;
        if (data == end) {
            return data;
        }
    }
    while (1);

    lxb_css_syntax_consume_numeric_set_float(tkz);

    lxb_css_syntax_token_dimension(tkz->token)->begin = data;
    lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric_name_start);

    return data;
}

static const lxb_char_t *
lxb_css_syntax_consume_numeric_name_start(lxb_css_syntax_tokenizer_t *tkz,
                                          const lxb_char_t *data,
                                          const lxb_char_t *end)
{
    if (tkz->is_eof) {
        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_NUMBER;
        lxb_css_syntax_state_token_done_m(tkz, end);

        return end;
    }

    if (lxb_css_syntax_res_name_map[*data] == LXB_CSS_SYNTAX_RES_NAME_START) {
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric_name);

        return (data + 1);
    }
    else if (*data == 0x00) {
        lxb_css_syntax_token_have_null_set(tkz);
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric_name);

        return (data + 1);
    }

    /* U+0025 PERCENTAGE SIGN (%) */
    if (*data == 0x25) {
        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_PERCENTAGE;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
        lxb_css_syntax_state_token_done_m(tkz, end);

        return (data + 1);
    }

    /* U+002D HYPHEN-MINUS */
    if (*data == 0x2D) {
        lxb_css_syntax_state_set(tkz,
                               lxb_css_syntax_consume_numeric_name_start_minus);
        return (data + 1);
    }

    /* U+005C REVERSE SOLIDUS (\) */
    if (*data == 0x5C) {
        lxb_css_syntax_state_set(tkz,
                             lxb_css_syntax_consume_numeric_before_name_escape);
        return (data + 1);
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
    lxb_css_syntax_state_token_done_m(tkz, end);

    return data;
}

static const lxb_char_t *
lxb_css_syntax_consume_numeric_name_start_minus(lxb_css_syntax_tokenizer_t *tkz,
                                                const lxb_char_t *data,
                                                const lxb_char_t *end)
{
    if (lxb_css_syntax_res_name_map[*data] == LXB_CSS_SYNTAX_RES_NAME_START) {
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric_name);

        return (data + 1);
    }
    else if (*data == 0x00) {
        lxb_css_syntax_token_have_null_set(tkz);
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric_name);

        return (data + 1);
    }

    /* U+002D HYPHEN-MINUS */
    if (*data == 0x2D) {
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric_name);

        return (data + 1);
    }

    /* U+005C REVERSE SOLIDUS (\) */
    if (*data == 0x5C) {
        lxb_css_syntax_state_set(tkz,
                             lxb_css_syntax_consume_numeric_before_name_escape);
        return (data + 1);
    }

    tkz->begin = lxb_css_syntax_token_dimension(tkz->token)->begin;

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
    lxb_css_syntax_state_token_done_m(tkz, end);

    return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->begin);
}

static const lxb_char_t *
lxb_css_syntax_consume_numeric_before_name_escape(lxb_css_syntax_tokenizer_t *tkz,
                                                  const lxb_char_t *data,
                                                  const lxb_char_t *end)
{
    /*
     * U+000A LINE FEED (LF)
     * U+000C FORM FEED (FF)
     * U+000D CARRIAGE RETURN (CR)
     */
    if ((*data == 0x0A || *data == 0x0C || *data == 0x0D) || tkz->is_eof) {
        tkz->begin = lxb_css_syntax_token_dimension(tkz->token)->begin;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
        lxb_css_syntax_state_token_done_m(tkz, end);

        return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->begin);
    }
    else if (*data == 0x00) {
        lxb_css_syntax_token_have_null_set(tkz);
    }

    lxb_css_syntax_token_escaped_set(tkz);

    data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                           lxb_css_syntax_consume_numeric_name);
    if (data == end) {
        return data;
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric_name);

    return data;
}

static const lxb_char_t *
lxb_css_syntax_consume_numeric_name(lxb_css_syntax_tokenizer_t *tkz,
                                    const lxb_char_t *data,
                                    const lxb_char_t *end)
{
    if (tkz->is_eof) {
        lxb_css_syntax_token_dimension(tkz->token)->end = tkz->incoming_node->end;
        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_DIMENSION;

        lxb_css_syntax_state_token_done_m(tkz, end);

        return end;
    }

    for (; data < end; data++) {
        if (lxb_css_syntax_res_name_map[*data] == 0x00) {
            /* U+0000 NULL (\0) */
            if (*data == 0x00) {
                lxb_css_syntax_token_have_null_set(tkz);

                continue;
            }
            /* U+005C REVERSE SOLIDUS (\) */
            else if (*data != 0x5C) {
                lxb_css_syntax_token_dimension(tkz->token)->end = data;
                lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_DIMENSION;

                lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
                lxb_css_syntax_state_token_done_m(tkz, end);

                return data;
            }

            data += 1;

            if (data == end) {
                lxb_css_syntax_token_dimension(tkz->token)->end = data - 1;

                lxb_css_syntax_state_set(tkz,
                                         lxb_css_syntax_consume_numeric_name_escape);
                return data;
            }

            /*
             * U+000A LINE FEED (LF)
             * U+000C FORM FEED (FF)
             * U+000D CARRIAGE RETURN (CR)
             */
            if (*data == 0x0A || *data == 0x0C || *data == 0x0D) {
                data -= 1;

                lxb_css_syntax_token_dimension(tkz->token)->end = data;
                lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_DIMENSION;

                lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
                lxb_css_syntax_state_token_done_m(tkz, end);

                return data;
            }
            else if (*data == 0x00) {
                lxb_css_syntax_token_have_null_set(tkz);
            }

            lxb_css_syntax_token_escaped_set(tkz);

            data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                           lxb_css_syntax_consume_numeric_name);
            if (data == end) {
                return data;
            }

            data -= 1;
        }
    }

    return data;
}

static const lxb_char_t *
lxb_css_syntax_consume_numeric_name_escape(lxb_css_syntax_tokenizer_t *tkz,
                                           const lxb_char_t *data,
                                           const lxb_char_t *end)
{
    /*
     * U+000A LINE FEED (LF)
     * U+000C FORM FEED (FF)
     * U+000D CARRIAGE RETURN (CR)
     */
    if ((*data == 0x0A || *data == 0x0C || *data == 0x0D) || tkz->is_eof) {
        tkz->end = lxb_css_syntax_token_dimension(tkz->token)->end;

        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_DIMENSION;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
        lxb_css_syntax_state_token_done_m(tkz, end);

        return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->end);
    }
    /* U+0000 NULL (\0) */
    else if (*data == 0x00) {
        lxb_css_syntax_token_have_null_set(tkz);
    }

    lxb_css_syntax_token_escaped_set(tkz);

    data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                           lxb_css_syntax_consume_numeric_name);
    if (data == end) {
        return data;
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_numeric_name);

    return data;
}

/*
 * Ident-like
 */
const lxb_char_t *
lxb_css_syntax_consume_ident_like(lxb_css_syntax_tokenizer_t *tkz,
                                  const lxb_char_t *data, const lxb_char_t *end)
{
    tkz->end = lxb_css_syntax_consume_url_ch;

    lxb_css_syntax_token_ident(tkz->token)->begin = data;

    return lxb_css_syntax_consume_ident_like_name(tkz, data, end);
}

static const lxb_char_t *
lxb_css_syntax_consume_ident_like_name(lxb_css_syntax_tokenizer_t *tkz,
                                       const lxb_char_t *data,
                                       const lxb_char_t *end)
{
    /* Having come here, we have at least one char (not escaped). */

    if (tkz->is_eof) {
        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_IDENT;
        lxb_css_syntax_token_ident(tkz->token)->end = tkz->incoming_node->end;

        lxb_css_syntax_state_token_done_m(tkz, end);

        return end;
    }

    /* Check for "url(" */

    do {
        /* U+005C REVERSE SOLIDUS (\) */
        if (*data == 0x5C) {
            data += 1;

            if (data == end) {
                tkz->begin = data - 1;

                lxb_css_syntax_state_set(tkz,
                                      lxb_css_syntax_consume_ident_like_escape);
                return data;
            }

            /*
             * U+000A LINE FEED (LF)
             * U+000C FORM FEED (FF)
             * U+000D CARRIAGE RETURN (CR)
             */
            if (*data == 0x0A || *data == 0x0C || *data == 0x0D) {
                data -= 1;

                lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_IDENT;
                lxb_css_syntax_token_ident(tkz->token)->end = data;

                lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
                lxb_css_syntax_state_token_done_m(tkz, end);

                return data;
            }

            /* U+0000 NULL (\0) */
            if (*data == 0x00) {
                lxb_css_syntax_token_have_null_set(tkz);
            }

            lxb_css_syntax_token_escaped_set(tkz);

            tkz->num = 0;

            for (tkz->count = 0; tkz->count < 6; tkz->count++, data++) {
                if (data == end) {
                    lxb_css_syntax_state_set(tkz,
                                lxb_css_syntax_consume_ident_like_solidus_data);
                    return data;
                }

                if (lexbor_str_res_map_hex[*data] == 0xFF) {
                    if (tkz->count == 0) {
                        tkz->num = *data;

                        break;
                    }

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
                                goto url_escape_newline;
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

                tkz->num <<= 4;
                tkz->num |= lexbor_str_res_map_hex[*data];
            }

            if (tkz->num == 0x00 || tkz->num > 0x80) {
                break;
            }

            if (*tkz->end != lexbor_str_res_map_lowercase[ (lxb_char_t) tkz->num ]) {
                break;
            }
        }
        else if (*tkz->end == lexbor_str_res_map_lowercase[*data]) {
            data += 1;
        }
        else {
            break;
        }

        tkz->end += 1;

        if (tkz->end == lxb_css_syntax_consume_url_ch_end) {
            if (data == end) {
                lxb_css_syntax_state_set(tkz,
                                 lxb_css_syntax_consume_ident_like_is_function);
                return data;
            }

            /* U+0028 LEFT PARENTHESIS (() */
            if (*data == 0x28) {
                lxb_css_syntax_token_function(tkz->token)->end = data;

                data += 1;
                tkz->end = NULL;

                if (data == end) {
                    lxb_css_syntax_state_set(tkz,
                            lxb_css_syntax_consume_ident_like_before_check_url);

                    return data;
                }

                tkz->begin = data;

                lxb_css_syntax_state_set(tkz,
                                   lxb_css_syntax_consume_ident_like_check_url);
                return data;
            }
        }

        if (data == end) {
            lxb_css_syntax_state_set(tkz,
                                lxb_css_syntax_consume_ident_like_name);
            return data;
        }
    }
    while (1);

    for (; data != end; data++) {
        if (lxb_css_syntax_res_name_map[*data] == 0x00) {
            if (*data == 0x00) {
                lxb_css_syntax_token_have_null_set(tkz);

                continue;
            }
            /* U+0028 LEFT PARENTHESIS (() */
            if (*data == 0x28) {
                lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_FUNCTION;
                lxb_css_syntax_token_function(tkz->token)->end = data;

                lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
                lxb_css_syntax_state_token_done_m(tkz, end);

                return (data + 1);
            }
            /* U+005C REVERSE SOLIDUS (\) */
            else if (*data == 0x5C) {
                data += 1;

                if (data == end) {
                    tkz->end = data - 1;

                    lxb_css_syntax_state_set(tkz,
                              lxb_css_syntax_consume_ident_like_not_url_escape);

                    return data;
                }

                if (*data != 0x0A && *data != 0x0C && *data != 0x0D) {
                    if (*data == 0x00) {
                        lxb_css_syntax_token_have_null_set(tkz);
                    }

                    lxb_css_syntax_token_escaped_set(tkz);

                    data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                     lxb_css_syntax_consume_ident_like_not_url);
                    if (data == end) {
                        return data;
                    }

                    data -= 1;

                    continue;
                }

                data -= 1;
            }

            lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_IDENT;
            lxb_css_syntax_token_ident(tkz->token)->end = data;

            lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
            lxb_css_syntax_state_token_done_m(tkz, end);

            return data;
        }
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_ident_like_not_url);

    return data;


url_escape_newline:

    tkz->state = lxb_css_syntax_state_check_newline;
    tkz->return_state = lxb_css_syntax_consume_ident_like_not_url;

    return data;
}

static const lxb_char_t *
lxb_css_syntax_consume_ident_like_escape(lxb_css_syntax_tokenizer_t *tkz,
                                          const lxb_char_t *data,
                                          const lxb_char_t *end)
{
    if ((*data == 0x0A || *data == 0x0C || *data == 0x0D) || tkz->is_eof) {
        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_IDENT;
        lxb_css_syntax_token_ident(tkz->token)->end = tkz->begin;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
        lxb_css_syntax_state_token_done_m(tkz, end);

        return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->end);
    }
    else if (*data == 0x00) {
        lxb_css_syntax_token_have_null_set(tkz);
    }

    lxb_css_syntax_token_escaped_set(tkz);

    tkz->num = 0;
    tkz->count = 0;

    lxb_css_syntax_state_set(tkz,
                             lxb_css_syntax_consume_ident_like_solidus_data);
    return data;
}

static const lxb_char_t *
lxb_css_syntax_consume_ident_like_solidus_data(lxb_css_syntax_tokenizer_t *tkz,
                                               const lxb_char_t *data,
                                               const lxb_char_t *end)
{
    if (tkz->is_eof) {
        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_IDENT;

        lxb_css_syntax_token_ident(tkz->token)->end = tkz->incoming_node->end;
        lxb_css_syntax_state_token_done_m(tkz, end);

        return end;
    }

    for (; tkz->count < 6; tkz->count++, data++) {
        if (data == end) {
            return data;
        }

        if (lexbor_str_res_map_hex[*data] == 0xFF) {
            if (tkz->count == 0) {
                tkz->num = *data;

                break;
            }

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
                        goto url_escape_newline;
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

        tkz->num <<= 4;
        tkz->num |= lexbor_str_res_map_hex[*data];
    }

    /*
     * Zero, or is for a surrogate, or is greater than
     * the maximum allowed code point (tkz->num > 0x10FFFF).
     */
    if (tkz->num == 0x00 || tkz->num > 0x80) {
        lxb_css_syntax_state_set(tkz,
                                 lxb_css_syntax_consume_ident_like_not_url);
        return data;
    }

    if (*tkz->end != lexbor_str_res_map_lowercase[ (lxb_char_t) tkz->num ]) {
        lxb_css_syntax_state_set(tkz,
                                 lxb_css_syntax_consume_ident_like_not_url);
        return data;
    }

    tkz->end += 1;

    if (tkz->end == lxb_css_syntax_consume_url_ch_end) {
        lxb_css_syntax_state_set(tkz,
                                 lxb_css_syntax_consume_ident_like_is_function);
        return data;
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_ident_like_name);

    return data;

url_escape_newline:

    tkz->state = lxb_css_syntax_state_check_newline;

    tkz->return_state = lxb_css_syntax_consume_ident_like_not_url;

    return data;
}

static const lxb_char_t *
lxb_css_syntax_consume_ident_like_is_function(lxb_css_syntax_tokenizer_t *tkz,
                                              const lxb_char_t *data,
                                              const lxb_char_t *end)
{
    if (tkz->is_eof) {
        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_IDENT;
        lxb_css_syntax_token_ident(tkz->token)->end = tkz->incoming_node->end;

        lxb_css_syntax_state_token_done_m(tkz, end);

        return end;
    }

    /* U+0028 LEFT PARENTHESIS (() */
    if (*data == 0x28) {
        lxb_css_syntax_token_ident(tkz->token)->end = data;

        data += 1;
        tkz->end = NULL;

        if (data == end) {
            lxb_css_syntax_state_set(tkz,
                            lxb_css_syntax_consume_ident_like_before_check_url);
            return data;
        }

        tkz->begin = data;

        lxb_css_syntax_state_set(tkz,
                                 lxb_css_syntax_consume_ident_like_check_url);
        return data;
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_ident_like_not_url);

    return data;
}

static const lxb_char_t *
lxb_css_syntax_consume_ident_like_before_check_url(lxb_css_syntax_tokenizer_t *tkz,
                                                   const lxb_char_t *data,
                                                   const lxb_char_t *end)
{
    if (tkz->is_eof) {
        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_FUNCTION;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
        lxb_css_syntax_state_token_done_m(tkz, end);

        return end;
    }

    tkz->begin = data;

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_ident_like_check_url);

    return data;
}

static const lxb_char_t *
lxb_css_syntax_consume_ident_like_check_url(lxb_css_syntax_tokenizer_t *tkz,
                                            const lxb_char_t *data,
                                            const lxb_char_t *end)
{
    if (tkz->is_eof) {
        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_FUNCTION;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
        lxb_css_syntax_state_token_done_m(tkz, end);

        return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->begin);
    }

    if (lexbor_utils_whitespace(*data, !=, &&)) {
        /* U+0022 QUOTATION MARK (") or U+0027 APOSTROPHE (') */
        if (*data == 0x22 || *data == 0x27) {
            lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_FUNCTION;

            lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
            lxb_css_syntax_state_token_done_m(tkz, end);

            if (tkz->end != NULL) {
                return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->end);
            }

            return data;
        }

        lxb_css_syntax_token_url(tkz->token)->begin = data;
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_url);

        return data;
    }

    data += 1;

    for (; data != end; data++) {
        if (lexbor_utils_whitespace(*data, !=, &&)) {
            /* U+0022 QUOTATION MARK (") or U+0027 APOSTROPHE (') */
            if (*data == 0x22 || *data == 0x27) {
                lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_FUNCTION;

                lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
                lxb_css_syntax_state_token_done_m(tkz, end);

                return (data - 1);
            }

            lxb_css_syntax_token_url(tkz->token)->begin = data;
            lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_url);

            return data;
        }
    }

    tkz->end = data - 1;

    return data;
}

/*
 * URL
 */
static const lxb_char_t *
lxb_css_syntax_consume_url(lxb_css_syntax_tokenizer_t *tkz,
                           const lxb_char_t *data, const lxb_char_t *end)
{
    if (tkz->is_eof) {
        lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_CSS_SYNTAX_TOKENIZER_ERROR_EOINUR);

        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_URL;
        lxb_css_syntax_token_url(tkz->token)->end = tkz->incoming_node->end;

        lxb_css_syntax_state_token_done_m(tkz, end);

        return end;
    }

    for (; data != end; data++) {
        switch (*data) {
            /* U+0000 NULL (\0) */
            case 0x00:
                lxb_css_syntax_token_have_null_set(tkz);

                break;

            /* U+0029 RIGHT PARENTHESIS ()) */
            case 0x29:
                lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_URL;
                lxb_css_syntax_token_url(tkz->token)->end = data;

                lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
                lxb_css_syntax_state_token_done_m(tkz, end);

                return (data + 1);

            /*
             * U+0022 QUOTATION MARK (")
             * U+0027 APOSTROPHE (')
             * U+0028 LEFT PARENTHESIS (()
             * U+000B LINE TABULATION
             * U+007F DELETE
             */
            case 0x22:
            case 0x27:
            case 0x28:
            case 0x0B:
            case 0x7F:
                lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_CSS_SYNTAX_TOKENIZER_ERROR_QOINUR);

                lxb_css_syntax_token_url(tkz->token)->end = data;
                lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_bad_url);

                return (data + 1);

            /* U+005C REVERSE SOLIDUS (\) */
            case 0x5C:
                data += 1;

                if (data == end) {
                    lxb_css_syntax_token_url(tkz->token)->end = data;

                    lxb_css_syntax_state_set(tkz,
                                             lxb_css_syntax_consume_url_escape);
                    return data;
                }

                /*
                 * U+000A LINE FEED (LF)
                 * U+000C FORM FEED (FF)
                 * U+000D CARRIAGE RETURN (CR)
                 */
                if (*data == 0x0A || *data == 0x0C || *data == 0x0D) {
                    lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, data,
                                       LXB_CSS_SYNTAX_TOKENIZER_ERROR_WRESINUR);

                    lxb_css_syntax_token_url(tkz->token)->end = data;
                    lxb_css_syntax_state_set(tkz,
                                             lxb_css_syntax_consume_bad_url);
                    return data;
                }
                else if (*data == 0x00) {
                    lxb_css_syntax_token_have_null_set(tkz);
                }

                lxb_css_syntax_token_escaped_set(tkz);

                data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                                    lxb_css_syntax_consume_url);
                if (data == end) {
                    return data;
                }

                data -= 1;

                break;
            /*
             * U+0009 CHARACTER TABULATION (tab)
             * U+000A LINE FEED (LF)
             * U+000C FORM FEED (FF)
             * U+000D CARRIAGE RETURN (CR)
             * U+0020 SPACE
             */
            case 0x09:
            case 0x0A:
            case 0x0C:
            case 0x0D:
            case 0x20:
                lxb_css_syntax_token_url(tkz->token)->end = data;

                data += 1;

                for (; data != end; data++) {
                    if (lexbor_utils_whitespace(*data, !=, &&)) {
                        /* U+0029 RIGHT PARENTHESIS ()) */
                        if (*data == 0x29) {
                            lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_URL;

                            lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
                            lxb_css_syntax_state_token_done_m(tkz, end);

                            return (data + 1);
                        }

                        lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_bad_url);

                        return data;
                    }
                }

                lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_url_end);

                return data;

            default:
                /*
                 * Inclusive:
                 * U+0000 NULL and U+0008 BACKSPACE or
                 * U+000E SHIFT OUT and U+001F INFORMATION SEPARATOR ONE
                 */
                if ((*data >= 0x00 && *data <= 0x08)
                    || (*data >= 0x0E && *data <= 0x1F))
                {
                    lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_CSS_SYNTAX_TOKENIZER_ERROR_QOINUR);

                    lxb_css_syntax_token_url(tkz->token)->end = data;
                    lxb_css_syntax_state_set(tkz,
                                             lxb_css_syntax_consume_bad_url);
                    return (data + 1);
                }

                break;
        }
    }

    return data;
}

static const lxb_char_t *
lxb_css_syntax_consume_url_escape(lxb_css_syntax_tokenizer_t *tkz,
                                  const lxb_char_t *data, const lxb_char_t *end)
{
    if (tkz->is_eof) {
        lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, data,
                                       LXB_CSS_SYNTAX_TOKENIZER_ERROR_WRESINUR);

        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_BAD_URL;
        lxb_css_syntax_state_token_done_m(tkz, end);

        return end;
    }

    /*
     * U+000A LINE FEED (LF)
     * U+000C FORM FEED (FF)
     * U+000D CARRIAGE RETURN (CR)
     */
    if (*data == 0x0A || *data == 0x0C || *data == 0x0D) {
        lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_bad_url);

        return (data + 1);
    }
    else if (*data == 0x00) {
        lxb_css_syntax_token_have_null_set(tkz);
    }

    lxb_css_syntax_token_escaped_set(tkz);

    data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                              lxb_css_syntax_consume_url);
    if (data == end) {
        return data;
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_url);

    return (data + 1);
}

static const lxb_char_t *
lxb_css_syntax_consume_url_end(lxb_css_syntax_tokenizer_t *tkz,
                               const lxb_char_t *data, const lxb_char_t *end)
{
    if (tkz->is_eof) {
        lxb_css_syntax_tokenizer_error_add(tkz->parse_errors, data,
                                         LXB_CSS_SYNTAX_TOKENIZER_ERROR_EOINUR);

        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_URL;
        lxb_css_syntax_state_token_done_m(tkz, end);

        return end;
    }

    for (; data != end; data++) {
        if (lexbor_utils_whitespace(*data, !=, &&)) {
            /* U+0029 RIGHT PARENTHESIS ()) */
            if (*data == 0x29) {
                lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_URL;

                lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
                lxb_css_syntax_state_token_done_m(tkz, end);

                return (data + 1);
            }

            lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_bad_url);

            return data;
        }
    }

    return data;
}

/*
 * Bad URL
 */
static const lxb_char_t *
lxb_css_syntax_consume_bad_url(lxb_css_syntax_tokenizer_t *tkz,
                               const lxb_char_t *data, const lxb_char_t *end)
{
    if (tkz->is_eof) {
        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_BAD_URL;
        lxb_css_syntax_state_token_done_m(tkz, end);

        return end;
    }

    for (; data != end; data++) {
        /* U+0029 RIGHT PARENTHESIS ()) */
        if (*data == 0x29) {
            lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_BAD_URL;

            lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
            lxb_css_syntax_state_token_done_m(tkz, end);

            return (data + 1);
        }
        /* U+005C REVERSE SOLIDUS (\) */
        else if (*data == 0x5C) {
            data += 1;
        }
    }

    return data;
}

const lxb_char_t *
lxb_css_syntax_consume_ident_like_not_url(lxb_css_syntax_tokenizer_t *tkz,
                                          const lxb_char_t *data,
                                          const lxb_char_t *end)
{
    if (tkz->is_eof) {
        if (lxb_css_syntax_token_ident(tkz->token)->begin == NULL) {
            lxb_css_syntax_token_ident(tkz->token)->begin = tkz->incoming_node->end;
        }

        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_IDENT;
        lxb_css_syntax_token_ident(tkz->token)->end = tkz->incoming_node->end;

        lxb_css_syntax_state_token_done_m(tkz, end);

        return end;
    }

    if (lxb_css_syntax_token_ident(tkz->token)->begin == NULL) {
        lxb_css_syntax_token_ident(tkz->token)->begin = data;
    }

    for (; data != end; data++) {
        if (lxb_css_syntax_res_name_map[*data] == 0x00) {
            /* U+0028 LEFT PARENTHESIS (() */
            if (*data == 0x28) {
                lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_FUNCTION;
                lxb_css_syntax_token_function(tkz->token)->end = data;

                lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
                lxb_css_syntax_state_token_done_m(tkz, end);

                return (data + 1);
            }
            /* U+005C REVERSE SOLIDUS (\) */
            else if (*data == 0x5C) {
                data += 1;

                if (data == end) {
                    tkz->end = data - 1;

                    lxb_css_syntax_state_set(tkz,
                              lxb_css_syntax_consume_ident_like_not_url_escape);

                    return data;
                }

                if (*data != 0x0A && *data != 0x0C && *data != 0x0D) {
                    if (*data == 0x00) {
                        lxb_css_syntax_token_have_null_set(tkz);
                    }

                    lxb_css_syntax_token_escaped_set(tkz);

                    data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                     lxb_css_syntax_consume_ident_like_not_url);
                    if (data == end) {
                        return data;
                    }

                    data -= 1;

                    continue;
                }

                data -= 1;
            }
            else if (*data == 0x00) {
                lxb_css_syntax_token_have_null_set(tkz);

                continue;
            }

            lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_IDENT;
            lxb_css_syntax_token_ident(tkz->token)->end = data;

            lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
            lxb_css_syntax_state_token_done_m(tkz, end);

            return data;
        }
    }

    return data;
}

static const lxb_char_t *
lxb_css_syntax_consume_ident_like_not_url_escape(lxb_css_syntax_tokenizer_t *tkz,
                                                 const lxb_char_t *data,
                                                 const lxb_char_t *end)
{
    if ((*data == 0x0A || *data == 0x0C || *data == 0x0D) || tkz->is_eof) {
        lxb_css_syntax_token_string(tkz->token)->end = tkz->end;
        lxb_css_syntax_token(tkz)->type = LXB_CSS_SYNTAX_TOKEN_IDENT;

        lxb_css_syntax_state_set(tkz, lxb_css_syntax_state_data);
        lxb_css_syntax_state_token_done_m(tkz, end);

        return lxb_css_syntax_tokenizer_change_incoming(tkz, tkz->end);
    }
    else if (*data == 0x00) {
        lxb_css_syntax_token_have_null_set(tkz);
    }

    lxb_css_syntax_token_escaped_set(tkz);

    data = lxb_css_syntax_state_check_escaped(tkz, data, end,
                                     lxb_css_syntax_consume_ident_like_not_url);
    if (data == end) {
        return data;
    }

    lxb_css_syntax_state_set(tkz, lxb_css_syntax_consume_ident_like_not_url);

    return data;
}

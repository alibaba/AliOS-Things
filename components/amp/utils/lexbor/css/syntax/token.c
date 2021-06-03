/*
 * Copyright (C) 2018-2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "core/shs.h"
#include "core/conv.h"

#include "token.h"

#define LXB_CSS_SYNTAX_TOKEN_RES_NAME_SHS_MAP
#include "syntax/token_res.h"

#define LEXBOR_STR_RES_MAP_HEX
#define LEXBOR_STR_RES_ANSI_REPLACEMENT_CHARACTER
#include "core/str_res.h"


//typedef lxb_status_t (*lxb_css_syntax_token_cb_f)(lxb_css_syntax_token_type_t type, const lxb_char_t *data, size_t len, void *ctx);


typedef struct {
    lexbor_str_t  *str;
    lexbor_mraw_t *mraw;
}
lxb_css_syntax_token_ctx_t;


static lxb_status_t
lxb_css_syntax_token_make_data_hard(lexbor_in_node_t *in, lexbor_mraw_t *mraw,
                                    lxb_css_syntax_token_data_t *td,
                                    lexbor_str_t *str, const lxb_char_t *begin,
                                    const lxb_char_t *end);

static lxb_status_t
lxb_css_syntax_token_make_data_simple(lexbor_in_node_t *in, lexbor_mraw_t *mraw,
                                      lxb_css_syntax_token_data_t *td,
                                      lexbor_str_t *str, const lxb_char_t *begin,
                                      const lxb_char_t *end);

static const lxb_char_t *
lxb_css_syntax_token_make_data_conv(const lxb_char_t *begin, const lxb_char_t *end,
                                    lexbor_str_t *str, lexbor_mraw_t *mraw,
                                    lxb_css_syntax_token_data_t *td);

static const lxb_char_t *
lxb_css_syntax_token_make_data_conv_num(const lxb_char_t *begin, const lxb_char_t *end,
                                        lexbor_str_t *str, lexbor_mraw_t *mraw,
                                        lxb_css_syntax_token_data_t *td);

static const lxb_char_t *
lxb_css_syntax_token_make_data_conv_cr(const lxb_char_t *begin, const lxb_char_t *end,
                                       lexbor_str_t *str, lexbor_mraw_t *mraw,
                                       lxb_css_syntax_token_data_t *td);

static lxb_status_t
lxb_css_syntax_token_codepoint_to_ascii(lxb_codepoint_t cp, lexbor_str_t *str,
                                        lexbor_mraw_t *mraw);

static lxb_status_t
lxb_css_syntax_token_str_cb(const lxb_char_t *data, size_t len, void *ctx);


const lxb_char_t *
lxb_css_syntax_token_type_name_by_id(lxb_css_syntax_token_type_t type)
{
    switch (type) {
        case LXB_CSS_SYNTAX_TOKEN_IDENT:
            return (lxb_char_t *) "ident";
        case LXB_CSS_SYNTAX_TOKEN_FUNCTION:
            return (lxb_char_t *) "function";
        case LXB_CSS_SYNTAX_TOKEN_AT_KEYWORD:
            return (lxb_char_t *) "at-keyword";
        case LXB_CSS_SYNTAX_TOKEN_HASH:
            return (lxb_char_t *) "hash";
        case LXB_CSS_SYNTAX_TOKEN_STRING:
            return (lxb_char_t *) "string";
        case LXB_CSS_SYNTAX_TOKEN_BAD_STRING:
            return (lxb_char_t *) "bad-string";
        case LXB_CSS_SYNTAX_TOKEN_URL:
            return (lxb_char_t *) "url";
        case LXB_CSS_SYNTAX_TOKEN_BAD_URL:
            return (lxb_char_t *) "bad-url";
        case LXB_CSS_SYNTAX_TOKEN_DELIM:
            return (lxb_char_t *) "delim";
        case LXB_CSS_SYNTAX_TOKEN_NUMBER:
            return (lxb_char_t *) "number";
        case LXB_CSS_SYNTAX_TOKEN_PERCENTAGE:
            return (lxb_char_t *) "percentage";
        case LXB_CSS_SYNTAX_TOKEN_DIMENSION:
            return (lxb_char_t *) "dimension";
        case LXB_CSS_SYNTAX_TOKEN_WHITESPACE:
            return (lxb_char_t *) "whitespace";
        case LXB_CSS_SYNTAX_TOKEN_CDO:
            return (lxb_char_t *) "CDO";
        case LXB_CSS_SYNTAX_TOKEN_CDC:
            return (lxb_char_t *) "CDC";
        case LXB_CSS_SYNTAX_TOKEN_COLON:
            return (lxb_char_t *) "colon";
        case LXB_CSS_SYNTAX_TOKEN_SEMICOLON:
            return (lxb_char_t *) "semicolon";
        case LXB_CSS_SYNTAX_TOKEN_COMMA:
            return (lxb_char_t *) "comma";
        case LXB_CSS_SYNTAX_TOKEN_LS_BRACKET:
            return (lxb_char_t *) "left-square-bracket";
        case LXB_CSS_SYNTAX_TOKEN_RS_BRACKET:
            return (lxb_char_t *) "right-square-bracket";
        case LXB_CSS_SYNTAX_TOKEN_L_PARENTHESIS:
            return (lxb_char_t *) "left-parenthesis";
        case LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS:
            return (lxb_char_t *) "right-parenthesis";
        case LXB_CSS_SYNTAX_TOKEN_LC_BRACKET:
            return (lxb_char_t *) "left-curly-bracket";
        case LXB_CSS_SYNTAX_TOKEN_RC_BRACKET:
            return (lxb_char_t *) "right-curly-bracket";
        case LXB_CSS_SYNTAX_TOKEN_COMMENT:
            return (lxb_char_t *) "comment";
        case LXB_CSS_SYNTAX_TOKEN__EOF:
            return (lxb_char_t *) "end-of-file";
    }

    return (lxb_char_t *) "undefined";
}

lxb_css_syntax_token_type_t
lxb_css_syntax_token_type_id_by_name(const lxb_char_t *type_name, size_t len)
{
    const lexbor_shs_entry_t *entry;

    entry = lexbor_shs_entry_get_lower_static(lxb_css_syntax_token_res_name_shs_map,
                                              type_name, len);
    if (entry == NULL) {
        return LXB_CSS_SYNTAX_TOKEN_UNDEF;
    }

    return (lxb_css_syntax_token_type_t) (uintptr_t) entry->value;
}

lxb_status_t
lxb_css_syntax_token_make_data(lxb_css_syntax_token_t *token, lexbor_in_node_t *in,
                               lexbor_mraw_t *mraw, lxb_css_syntax_token_data_t *td)
{
    switch (token->types.base.type) {
        /* All this types inherit from lxb_css_syntax_token_string_t. */
        case LXB_CSS_SYNTAX_TOKEN_IDENT:
        case LXB_CSS_SYNTAX_TOKEN_FUNCTION:
        case LXB_CSS_SYNTAX_TOKEN_AT_KEYWORD:
        case LXB_CSS_SYNTAX_TOKEN_HASH:
        case LXB_CSS_SYNTAX_TOKEN_STRING:
        case LXB_CSS_SYNTAX_TOKEN_BAD_STRING:
        case LXB_CSS_SYNTAX_TOKEN_URL:
        case LXB_CSS_SYNTAX_TOKEN_BAD_URL:
        case LXB_CSS_SYNTAX_TOKEN_COMMENT:
        case LXB_CSS_SYNTAX_TOKEN_WHITESPACE:
            if (token->types.base.data_type == LXB_CSS_SYNTAX_TOKEN_DATA_SIMPLE) {
                return lxb_css_syntax_token_make_data_simple(in, mraw, td,
                                                     &token->types.string.data,
                                                     token->types.string.begin,
                                                     token->types.string.end);
            }

            td->cb = lxb_css_syntax_token_make_data_conv;

            return lxb_css_syntax_token_make_data_hard(in, mraw, td,
                                                       &token->types.string.data,
                                                       token->types.string.begin,
                                                       token->types.string.end);

        case LXB_CSS_SYNTAX_TOKEN_DIMENSION:
            if (token->types.base.data_type == LXB_CSS_SYNTAX_TOKEN_DATA_SIMPLE) {
                return lxb_css_syntax_token_make_data_simple(in, mraw, td,
                                                   &token->types.dimension.data,
                                                   token->types.dimension.begin,
                                                   token->types.dimension.end);
            }

            td->cb = lxb_css_syntax_token_make_data_conv;

            return lxb_css_syntax_token_make_data_hard(in, mraw, td,
                                                       &token->types.dimension.data,
                                                       token->types.dimension.begin,
                                                       token->types.dimension.end);
    }

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_css_syntax_token_make_data_hard(lexbor_in_node_t *in, lexbor_mraw_t *mraw,
                                    lxb_css_syntax_token_data_t *td,
                                    lexbor_str_t *str, const lxb_char_t *begin,
                                    const lxb_char_t *end)
{
    size_t len = 0;
    const lxb_char_t *ptr = end;

    td->is_last = false;
    td->status = LXB_STATUS_OK;

    in = lexbor_in_node_find(in, end);

    do {
        if (lexbor_in_segment(in, begin)) {
            len += ptr - begin;

            break;
        }

        len += ptr - in->begin;

        if (in->prev == NULL) {
            return LXB_STATUS_ERROR;
        }

        in = in->prev;
        ptr = in->end;
    }
    while (1);

    if (str->data == NULL) {
        lexbor_str_init(str, mraw, len);
        if (str->data == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }
    else if (lexbor_str_size(str) <= len) {
        ptr = lexbor_str_realloc(str, mraw, (len + 1));
        if (ptr == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    while (lexbor_in_segment(in, end) == false) {
        while (begin < in->end) {
            begin = td->cb(begin, in->end, str, mraw, td);
        }

        if (td->status != LXB_STATUS_OK) {
            return td->status;
        }

        in = in->next;
        begin = in->begin;
    }

    td->node_done = in;
    td->is_last = true;

    do {
        begin = td->cb(begin, end, str, mraw, td);
    }
    while (begin < end);

    return td->status;
}

static lxb_status_t
lxb_css_syntax_token_make_data_simple(lexbor_in_node_t *in, lexbor_mraw_t *mraw,
                                      lxb_css_syntax_token_data_t *td,
                                      lexbor_str_t *str, const lxb_char_t *begin,
                                      const lxb_char_t *end)
{
    size_t len = 0;
    const lxb_char_t *ptr = end;

    in = lexbor_in_node_find(in, end);

    do {
        if (lexbor_in_segment(in, begin)) {
            len += ptr - begin;

            break;
        }

        len += ptr - in->begin;

        if (in->prev == NULL) {
            return LXB_STATUS_ERROR;
        }

        in = in->prev;
        ptr = in->end;
    }
    while (1);

    if (str->data == NULL) {
        lexbor_str_init(str, mraw, len);
        if (str->data == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }
    else if (lexbor_str_size(str) <= len) {
        ptr = lexbor_str_realloc(str, mraw, (len + 1));
        if (ptr == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    while (lexbor_in_segment(in, end) == false) {
        memcpy(&str->data[str->length], begin, (in->end - begin));
        str->length += (in->end - begin);

        in = in->next;
        begin = in->begin;
    }

    memcpy(&str->data[str->length], begin, (end - begin));
    str->length += (end - begin);

    str->data[str->length] = 0x00;

    td->node_done = in;

    return LXB_STATUS_OK;
}

static const lxb_char_t *
lxb_css_syntax_token_make_data_conv(const lxb_char_t *begin, const lxb_char_t *end,
                                    lexbor_str_t *str, lexbor_mraw_t *mraw,
                                    lxb_css_syntax_token_data_t *td)
{
    const lxb_char_t *ptr = begin;

    while (begin < end) {
        switch (*begin) {
            /* U+005C REVERSE SOLIDUS (\) */
            case 0x5C:
                memcpy(&str->data[str->length], ptr, (begin - ptr));
                str->length += begin - ptr;

                begin += 1;

                td->num = 0;
                td->count = 0;

                for (; td->count < 6; td->count++, begin++) {
                    if (begin == end) {
                        if (td->is_last == false) {
                            td->cb = lxb_css_syntax_token_make_data_conv_num;

                            return begin;
                        }

                        break;
                    }

                    if (lexbor_str_res_map_hex[*begin] == 0xFF) {
                        if (td->count == 0) {
                            if (*begin == 0x0A || *begin == 0x0D
                                || *begin == 0x0C)
                            {
                                goto ws_processing;
                            }

                            td->num = *begin;

                            begin += 1;
                        }

                        break;
                    }

                    td->num <<= 4;
                    td->num |= lexbor_str_res_map_hex[*begin];
                }

                td->status = lxb_css_syntax_token_codepoint_to_ascii(td->num,
                                                                     str, mraw);
                if (td->status != LXB_STATUS_OK) {
                    return end;
                }

ws_processing:

                if (begin == end) {
                    return end;
                }

                /*
                 * U+0009 CHARACTER TABULATION
                 * U+0020 SPACE
                 * U+000A LINE FEED (LF)
                 * U+000C FORM FEED (FF)
                 * U+000D CARRIAGE RETURN (CR)
                 */
                switch (*begin) {
                    case 0x0D:
                        begin += 1;

                        if (begin == end) {
                            td->cb = lxb_css_syntax_token_make_data_conv_cr;

                            return begin;
                        }

                        if (*begin == 0x0A) {
                            begin += 1;
                        }

                        break;

                    case 0x09:
                    case 0x20:
                    case 0x0A:
                    case 0x0C:
                        begin += 1;

                        break;
                }

                ptr = begin;

                continue;

            /* U+000C FORM FEED (FF) */
            case 0x0C:
                memcpy(&str->data[str->length], ptr, (begin - ptr));
                str->length += begin - ptr;

                begin += 1;

                str->data[str->length] = 0x0A;
                str->length++;

                ptr = begin;

                continue;

            /* U+000D CARRIAGE RETURN (CR) */
            case 0x0D:
                memcpy(&str->data[str->length], ptr, (begin - ptr));
                str->length += begin - ptr;

                begin += 1;

                str->data[str->length] = 0x0A;
                str->length++;

                if (begin == end) {
                    if (td->is_last == false) {
                        td->cb = lxb_css_syntax_token_make_data_conv_cr;

                        return begin;
                    }

                    return begin;
                }

                /* U+000A LINE FEED (LF) */
                if (*begin == 0x0A) {
                    begin += 1;
                }

                ptr = begin;

                continue;

            /* U+0000 NULL (\0) */
            case 0x00:
                memcpy(&str->data[str->length], ptr, (begin - ptr));
                str->length += begin - ptr;

                ptr = lexbor_str_realloc(str, mraw, lexbor_str_size(str) + 2);
                if (ptr == NULL) {
                    td->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

                    return end;
                }

                memcpy(&str->data[str->length],
                       lexbor_str_res_ansi_replacement_character, 3);

                str->length += 3;

                ptr = begin + 1;
        }

        begin += 1;
    }

    if (ptr < begin) {
        memcpy(&str->data[str->length], ptr, (begin - ptr));
        str->length += begin - ptr;
    }

    if (td->is_last) {
        str->data[str->length] = 0x00;
    }

    return begin;
}

static const lxb_char_t *
lxb_css_syntax_token_make_data_conv_num(const lxb_char_t *begin, const lxb_char_t *end,
                                        lexbor_str_t *str, lexbor_mraw_t *mraw,
                                        lxb_css_syntax_token_data_t *td)
{
    for (; td->count < 6; td->count++, begin++) {
        if (begin == end) {
            if (td->is_last == false) {
                return begin;
            }

            break;
        }

        if (lexbor_str_res_map_hex[*begin] == 0xFF) {
            if (td->count == 0) {
                if (*begin == 0x0A || *begin == 0x0D || *begin == 0x0C) {
                    td->cb = lxb_css_syntax_token_make_data_conv;

                    goto ws_processing;
                }

                td->num = *begin;

                begin += 1;
            }

            break;
        }

        td->num <<= 4;
        td->num |= lexbor_str_res_map_hex[*begin];
    }

    td->cb = lxb_css_syntax_token_make_data_conv;

    td->status = lxb_css_syntax_token_codepoint_to_ascii(td->num, str, mraw);
    if (td->status != LXB_STATUS_OK) {
        return end;
    }

    if (td->is_last) {
        str->data[str->length] = 0x00;
    }

ws_processing:

    if (begin == end) {
        return end;
    }

    /*
     * U+0009 CHARACTER TABULATION
     * U+0020 SPACE
     * U+000A LINE FEED (LF)
     * U+000C FORM FEED (FF)
     * U+000D CARRIAGE RETURN (CR)
     */
    switch (*begin) {
        case 0x0D:
            begin += 1;

            if (begin == end) {
                td->cb = lxb_css_syntax_token_make_data_conv_cr;

                return begin;
            }

            if (*begin == 0x0A) {
                begin += 1;
            }

            break;

        case 0x09:
        case 0x20:
        case 0x0A:
        case 0x0C:
            begin += 1;

            break;
    }

    return begin;
}

static const lxb_char_t *
lxb_css_syntax_token_make_data_conv_cr(const lxb_char_t *begin, const lxb_char_t *end,
                                       lexbor_str_t *str, lexbor_mraw_t *mraw,
                                       lxb_css_syntax_token_data_t *td)
{
    td->cb = lxb_css_syntax_token_make_data_conv;

    /* U+000A LINE FEED (LF) */
    if (*begin == 0x0A) {
        return (begin + 1);
    }

    return begin;
}

static lxb_status_t
lxb_css_syntax_token_codepoint_to_ascii(lxb_codepoint_t cp, lexbor_str_t *str,
                                        lexbor_mraw_t *mraw)
{
    /*
     * Zero, or is for a surrogate, or is greater than
     * the maximum allowed code point (tkz->num > 0x10FFFF).
     */
    if (cp == 0) {
        lxb_char_t *ptr;

        ptr = lexbor_str_realloc(str, mraw, lexbor_str_size(str) + 1);
        if (ptr == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }

        memcpy(&str->data[str->length],
               lexbor_str_res_ansi_replacement_character, 3);

        str->length += 3;

        return LXB_STATUS_OK;
    }
    else if (cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF)) {
        memcpy(&str->data[str->length],
               lexbor_str_res_ansi_replacement_character, 3);

        str->length += 3;

        return LXB_STATUS_OK;
    }

    lxb_char_t *data = &str->data[str->length];

    if (cp <= 0x0000007F) {
        /* 0xxxxxxx */
        data[0] = (lxb_char_t) cp;

        str->length += 1;
    }
    else if (cp <= 0x000007FF) {
        /* 110xxxxx 10xxxxxx */
        data[0] = (char)(0xC0 | (cp >> 6  ));
        data[1] = (char)(0x80 | (cp & 0x3F));

        str->length += 2;
    }
    else if (cp <= 0x0000FFFF) {
        /* 1110xxxx 10xxxxxx 10xxxxxx */
        data[0] = (char)(0xE0 | ((cp >> 12)));
        data[1] = (char)(0x80 | ((cp >> 6 ) & 0x3F));
        data[2] = (char)(0x80 | ( cp & 0x3F));

        str->length += 3;
    }
    else if (cp <= 0x001FFFFF) {
        /* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
        data[0] = (char)(0xF0 | ( cp >> 18));
        data[1] = (char)(0x80 | ((cp >> 12) & 0x3F));
        data[2] = (char)(0x80 | ((cp >> 6 ) & 0x3F));
        data[3] = (char)(0x80 | ( cp & 0x3F));

        str->length += 4;
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_syntax_token_serialize_cb(lxb_css_syntax_token_t *token,
                                  lxb_css_syntax_token_cb_f cb, void *ctx)
{
    size_t len;
    lxb_status_t status;
    lxb_char_t buf[128];

    switch (token->types.base.type) {
        case LXB_CSS_SYNTAX_TOKEN_DELIM:
            return cb(token->types.base.type, &token->types.delim.character, 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_NUMBER:
            len = lexbor_conv_float_to_data(token->types.number.num,
                                            buf, (sizeof(buf) - 1));

            return cb(token->types.base.type, buf, len, ctx);

        case LXB_CSS_SYNTAX_TOKEN_PERCENTAGE:
            len = lexbor_conv_float_to_data(token->types.number.num,
                                            buf, (sizeof(buf) - 1));

            status = cb(token->types.base.type, buf, len, ctx);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            return cb(token->types.base.type, (lxb_char_t *) "%", 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_CDO:
            return cb(token->types.base.type, (lxb_char_t *) "<!--", 4, ctx);

        case LXB_CSS_SYNTAX_TOKEN_CDC:
            return cb(token->types.base.type, (lxb_char_t *) "-->", 3, ctx);

        case LXB_CSS_SYNTAX_TOKEN_COLON:
            return cb(token->types.base.type, (lxb_char_t *) ":", 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_SEMICOLON:
            return cb(token->types.base.type, (lxb_char_t *) ";", 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_COMMA:
            return cb(token->types.base.type, (lxb_char_t *) ",", 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_LS_BRACKET:
            return cb(token->types.base.type, (lxb_char_t *) "[", 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_RS_BRACKET:
            return cb(token->types.base.type, (lxb_char_t *) "]", 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_L_PARENTHESIS:
            return cb(token->types.base.type, (lxb_char_t *) "(", 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS:
            return cb(token->types.base.type, (lxb_char_t *) ")", 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_LC_BRACKET:
            return cb(token->types.base.type, (lxb_char_t *) "{", 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_RC_BRACKET:
            return cb(token->types.base.type, (lxb_char_t *) "}", 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_HASH:
#if 1
            status = cb(token->types.base.type, (lxb_char_t *) "#", 1, ctx);
            if (status != LXB_STATUS_OK) {
                return status;
            }
#endif

            return cb(token->types.base.type, token->types.string.data.data,
                      token->types.string.data.length, ctx);

        case LXB_CSS_SYNTAX_TOKEN_AT_KEYWORD:
            status = cb(token->types.base.type, (lxb_char_t *) "@", 1, ctx);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            return cb(token->types.base.type, token->types.string.data.data,
                      token->types.string.data.length, ctx);

        case LXB_CSS_SYNTAX_TOKEN_IDENT:
            return cb(token->types.base.type, token->types.string.data.data,
                      token->types.string.data.length, ctx);

        case LXB_CSS_SYNTAX_TOKEN_FUNCTION:
            status = cb(token->types.base.type, token->types.string.data.data,
                        token->types.string.data.length, ctx);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            return cb(token->types.base.type, (lxb_char_t *) "(", 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_STRING:
        case LXB_CSS_SYNTAX_TOKEN_BAD_STRING: {
           
#if 0
            status = cb((lxb_char_t *) "\"", 1, ctx);
            if (status != LXB_STATUS_OK) {
                return status;
            }
#endif

            const lxb_char_t *begin = token->types.string.data.data;
            const lxb_char_t *end = token->types.string.data.data
                                    + token->types.string.data.length;

            const lxb_char_t *ptr = begin;

            for (; begin < end; begin++) {
                /* 0x5C; '\'; Inverse/backward slash */
                if (*begin == 0x5C) {
                    begin += 1;

                    status = cb(token->types.base.type, ptr, (begin - ptr), ctx);
                    if (status != LXB_STATUS_OK) {
                        return status;
                    }

                    if (begin == end) {
                        status = cb(token->types.base.type, (const lxb_char_t *) "\\", 1, ctx);
                        if (status != LXB_STATUS_OK) {
                            return status;
                        }

                        break;
                    }

                    begin -= 1;
                    ptr = begin;
                }
                /* 0x22; '"'; Only quotes above */
                else if (*begin == 0x22) {
                    if (ptr != begin) {
                        status = cb(token->types.base.type, ptr, (begin - ptr), ctx);
                        if (status != LXB_STATUS_OK) {
                            return status;
                        }
                    }

                    status = cb(token->types.base.type, (const lxb_char_t *) "\\", 1, ctx);
                    if (status != LXB_STATUS_OK) {
                        return status;
                    }

                    ptr = begin;
                }
            }

            if (ptr != begin) {
                status = cb(token->types.base.type, ptr, (begin - ptr), ctx);
                if (status != LXB_STATUS_OK) {
                    return status;
                }
            }

            //return cb(token->types.base.type, (const lxb_char_t *) "\"", 1, ctx);
        }

        case LXB_CSS_SYNTAX_TOKEN_URL:
        case LXB_CSS_SYNTAX_TOKEN_BAD_URL:
            status = cb(token->types.base.type, (lxb_char_t *) "url(", 4, ctx);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            status = cb(token->types.base.type, token->types.string.data.data,
                        token->types.string.data.length, ctx);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            return cb(token->types.base.type, (lxb_char_t *) ")", 1, ctx);

        case LXB_CSS_SYNTAX_TOKEN_COMMENT:
            status = cb(token->types.base.type, (lxb_char_t *) "/*", 2, ctx);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            status = cb(token->types.base.type, token->types.string.data.data,
                        token->types.string.data.length, ctx);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            return cb(token->types.base.type, (lxb_char_t *) "*/", 2, ctx);

        case LXB_CSS_SYNTAX_TOKEN_WHITESPACE:
            return cb(token->types.base.type, token->types.whitespace.data.data,
                      token->types.whitespace.data.length, ctx);

        case LXB_CSS_SYNTAX_TOKEN_DIMENSION:
            len = lexbor_conv_float_to_data(token->types.number.num,
                                            buf, (sizeof(buf) - 1));

            status = cb(token->types.base.type, buf, len, ctx);
            if (status != LXB_STATUS_OK) {
                return status;
            }
#if 0
            return cb(token->types.base.type, token->types.dimension.data.data,
                      token->types.dimension.data.length, ctx);
#endif
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_syntax_token_serialize_str(lxb_css_syntax_token_t *token,
                                   lexbor_str_t *str, lexbor_mraw_t *mraw)
{
    lxb_css_syntax_token_ctx_t ctx;

    ctx.str = str;
    ctx.mraw = mraw;

    if (str->data == NULL) {
        lexbor_str_init(str, mraw, 1);
        if (str->data == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    return lxb_css_syntax_token_serialize_cb(token,
                                             lxb_css_syntax_token_str_cb, &ctx);
}

static lxb_status_t
lxb_css_syntax_token_str_cb(const lxb_char_t *data, size_t len, void *cb_ctx)
{
    lxb_char_t *ptr;
    lxb_css_syntax_token_ctx_t *ctx = (lxb_css_syntax_token_ctx_t *) cb_ctx;

    ptr = lexbor_str_append(ctx->str, ctx->mraw, data, len);
    if (ptr == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    return LXB_STATUS_OK;
}

/*
 * No inline functions for ABI.
 */
lxb_css_syntax_token_t *
lxb_css_syntax_token_create_noi(lexbor_dobject_t *dobj)
{
    return lxb_css_syntax_token_create(dobj);
}

void
lxb_css_syntax_token_clean_noi(lxb_css_syntax_token_t *token)
{
    lxb_css_syntax_token_clean(token);
}

lxb_css_syntax_token_t *
lxb_css_syntax_token_destroy_noi(lxb_css_syntax_token_t *token,
                                 lexbor_dobject_t *dobj)
{
    return lxb_css_syntax_token_destroy(token, dobj);
}

const lxb_char_t *
lxb_css_syntax_token_type_name_noi(lxb_css_syntax_token_t *token)
{
    return lxb_css_syntax_token_type_name(token);
}

lxb_css_syntax_token_type_t
lxb_css_syntax_token_type_noi(lxb_css_syntax_token_t *token)
{
    return lxb_css_syntax_token_type(token);
}

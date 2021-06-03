/*
 * Copyright (C) 2018-2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CSS_SYNTAX_TOKEN_H
#define LEXBOR_CSS_SYNTAX_TOKEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "core/in.h"
#include "core/str.h"

#include "syntax/base.h"


#define lxb_css_syntax_token_data_type_set(tkz, dtype)                         \
    do {                                                                       \
        if ((lxb_css_syntax_token(tkz)->data_type & dtype) == 0) {             \
                lxb_css_syntax_token(tkz)->data_type |= dtype;                 \
        }                                                                      \
    }                                                                          \
    while (0)

#define lxb_css_syntax_token_escaped_set(tkz)                                  \
    lxb_css_syntax_token_data_type_set(tkz, LXB_CSS_SYNTAX_TOKEN_DATA_ESCAPED)

#define lxb_css_syntax_token_cr_set(tkz)                                       \
    lxb_css_syntax_token_data_type_set(tkz, LXB_CSS_SYNTAX_TOKEN_DATA_CR)

#define lxb_css_syntax_token_ff_set(tkz)                                       \
    lxb_css_syntax_token_data_type_set(tkz, LXB_CSS_SYNTAX_TOKEN_DATA_FF)

#define lxb_css_syntax_token_have_null_set(tkz)                                \
    lxb_css_syntax_token_data_type_set(tkz, LXB_CSS_SYNTAX_TOKEN_DATA_HAVE_NULL)


#define lxb_css_syntax_token(tkz) ((lxb_css_syntax_token_base_t *) tkz->token)

#define lxb_css_syntax_token_base(token) ((lxb_css_syntax_token_base_t *) token)
#define lxb_css_syntax_token_ident(token) ((lxb_css_syntax_token_ident_t *) token)
#define lxb_css_syntax_token_function(token) ((lxb_css_syntax_token_function_t *) token)
#define lxb_css_syntax_token_at_keyword(token) ((lxb_css_syntax_token_at_keyword_t *) token)
#define lxb_css_syntax_token_hash(token) ((lxb_css_syntax_token_hash_t *) token)
#define lxb_css_syntax_token_string(token) ((lxb_css_syntax_token_string_t *) token)
#define lxb_css_syntax_token_bad_string(token) ((lxb_css_syntax_token_bad_string_t *) token)
#define lxb_css_syntax_token_url(token) ((lxb_css_syntax_token_url_t *) token)
#define lxb_css_syntax_token_bad_url(token) ((lxb_css_syntax_token_bad_url_t *) token)
#define lxb_css_syntax_token_delim(token) ((lxb_css_syntax_token_delim_t *) token)
#define lxb_css_syntax_token_number(token) ((lxb_css_syntax_token_number_t *) token)
#define lxb_css_syntax_token_percentage(token) ((lxb_css_syntax_token_percentage_t *) token)
#define lxb_css_syntax_token_dimension(token) ((lxb_css_syntax_token_dimension_t *) token)
#define lxb_css_syntax_token_whitespace(token) ((lxb_css_syntax_token_whitespace_t *) token)
#define lxb_css_syntax_token_cdo(token) ((lxb_css_syntax_token_cdo_t *) token)
#define lxb_css_syntax_token_cdc(token) ((lxb_css_syntax_token_cdc_t *) token)
#define lxb_css_syntax_token_colon(token) ((lxb_css_syntax_token_colon_t *) token)
#define lxb_css_syntax_token_semicolon(token) ((lxb_css_syntax_token_semicolon_t *) token)
#define lxb_css_syntax_token_comma(token) ((lxb_css_syntax_token_comma_t *) token)
#define lxb_css_syntax_token_ls_bracket(token) ((lxb_css_syntax_token_ls_bracket_t *) token)
#define lxb_css_syntax_token_rs_bracket(token) ((lxb_css_syntax_token_rs_bracket_t *) token)
#define lxb_css_syntax_token_l_parenthesis(token) ((lxb_css_syntax_token_l_parenthesis_t *) token)
#define lxb_css_syntax_token_r_parenthesis(token) ((lxb_css_syntax_token_r_parenthesis_t *) token)
#define lxb_css_syntax_token_lc_bracket(token) ((lxb_css_syntax_token_lc_bracket_t *) token)
#define lxb_css_syntax_token_rc_bracket(token) ((lxb_css_syntax_token_rc_bracket_t *) token)
#define lxb_css_syntax_token_comment(token) ((lxb_css_syntax_token_comment_t *) token)


typedef struct lxb_css_syntax_token_data lxb_css_syntax_token_data_t;

typedef const lxb_char_t *
(*lxb_css_syntax_token_data_cb_f)(const lxb_char_t *begin, const lxb_char_t *end,
                                  lexbor_str_t *str, lexbor_mraw_t *mraw,
                                  lxb_css_syntax_token_data_t *td);



struct lxb_css_syntax_token_data {
    lxb_css_syntax_token_data_cb_f cb;
    lexbor_in_node_t               *node_done;
    lxb_status_t                   status;
    int                            count;
    uint32_t                       num;
    bool                           is_last;
};


typedef unsigned int lxb_css_syntax_token_type_t;
typedef unsigned int lxb_css_syntax_token_data_type_t;
typedef lxb_status_t (*lxb_css_syntax_token_cb_f)(lxb_css_syntax_token_type_t type, const lxb_char_t *data, size_t len, void *ctx);

enum lxb_css_syntax_token_type {
    LXB_CSS_SYNTAX_TOKEN_UNDEF = 0x00,
    LXB_CSS_SYNTAX_TOKEN_IDENT,
    LXB_CSS_SYNTAX_TOKEN_FUNCTION,
    LXB_CSS_SYNTAX_TOKEN_AT_KEYWORD,
    LXB_CSS_SYNTAX_TOKEN_HASH,
    LXB_CSS_SYNTAX_TOKEN_STRING,
    LXB_CSS_SYNTAX_TOKEN_BAD_STRING,
    LXB_CSS_SYNTAX_TOKEN_URL,
    LXB_CSS_SYNTAX_TOKEN_BAD_URL,
    LXB_CSS_SYNTAX_TOKEN_DELIM,
    LXB_CSS_SYNTAX_TOKEN_NUMBER,
    LXB_CSS_SYNTAX_TOKEN_PERCENTAGE,
    LXB_CSS_SYNTAX_TOKEN_DIMENSION,
    LXB_CSS_SYNTAX_TOKEN_WHITESPACE,
    LXB_CSS_SYNTAX_TOKEN_CDO,
    LXB_CSS_SYNTAX_TOKEN_CDC,
    LXB_CSS_SYNTAX_TOKEN_COLON,
    LXB_CSS_SYNTAX_TOKEN_SEMICOLON,
    LXB_CSS_SYNTAX_TOKEN_COMMA,
    LXB_CSS_SYNTAX_TOKEN_LS_BRACKET,   /* U+005B LEFT SQUARE BRACKET ([) */
    LXB_CSS_SYNTAX_TOKEN_RS_BRACKET,  /* U+005D RIGHT SQUARE BRACKET (]) */
    LXB_CSS_SYNTAX_TOKEN_L_PARENTHESIS,   /* U+0028 LEFT PARENTHESIS (() */
    LXB_CSS_SYNTAX_TOKEN_R_PARENTHESIS,  /* U+0029 RIGHT PARENTHESIS ()) */
    LXB_CSS_SYNTAX_TOKEN_LC_BRACKET,    /* U+007B LEFT CURLY BRACKET ({) */
    LXB_CSS_SYNTAX_TOKEN_RC_BRACKET,   /* U+007D RIGHT CURLY BRACKET (}) */
    LXB_CSS_SYNTAX_TOKEN_COMMENT,                /* not in specification */
    LXB_CSS_SYNTAX_TOKEN__EOF,
    LXB_CSS_SYNTAX_TOKEN__LAST_ENTRY
};

enum lxb_css_syntax_token_data_type {
    LXB_CSS_SYNTAX_TOKEN_DATA_SIMPLE    = 0x00,
    LXB_CSS_SYNTAX_TOKEN_DATA_CR        = 0x01,
    LXB_CSS_SYNTAX_TOKEN_DATA_FF        = 0x02,
    LXB_CSS_SYNTAX_TOKEN_DATA_ESCAPED   = 0x04,
    LXB_CSS_SYNTAX_TOKEN_DATA_HAVE_NULL = 0x08
};

typedef struct lxb_css_syntax_token_base {
    lxb_css_syntax_token_type_t      type;
    lxb_css_syntax_token_data_type_t data_type;
}
lxb_css_syntax_token_base_t;

typedef struct lxb_css_syntax_token_number {
    lxb_css_syntax_token_base_t base;

    double                      num;
    bool                        is_float;
}
lxb_css_syntax_token_number_t;

typedef struct lxb_css_syntax_token_dimension {
    /* Do not change it. */
    lxb_css_syntax_token_number_t num;

    lexbor_str_t                  data;

    /* Ident */
    const lxb_char_t              *begin;
    const lxb_char_t              *end;
}
lxb_css_syntax_token_dimension_t;

typedef struct lxb_css_syntax_token_string {
    lxb_css_syntax_token_base_t base;

    lexbor_str_t                data;

    const lxb_char_t            *begin;
    const lxb_char_t            *end;
}
lxb_css_syntax_token_string_t;

typedef struct lxb_css_syntax_token_delim {
    lxb_css_syntax_token_base_t base;

    lxb_char_t                  character;

    const lxb_char_t            *begin;
    const lxb_char_t            *end;
}
lxb_css_syntax_token_delim_t;

typedef lxb_css_syntax_token_string_t lxb_css_syntax_token_ident_t;
typedef lxb_css_syntax_token_string_t lxb_css_syntax_token_function_t;
typedef lxb_css_syntax_token_string_t lxb_css_syntax_token_at_keyword_t;
typedef lxb_css_syntax_token_string_t lxb_css_syntax_token_hash_t;
typedef lxb_css_syntax_token_string_t lxb_css_syntax_token_bad_string_t;
typedef lxb_css_syntax_token_string_t lxb_css_syntax_token_url_t;
typedef lxb_css_syntax_token_string_t lxb_css_syntax_token_bad_url_t;
typedef lxb_css_syntax_token_number_t lxb_css_syntax_token_percentage_t;
typedef lxb_css_syntax_token_string_t lxb_css_syntax_token_whitespace_t;
typedef lxb_css_syntax_token_base_t   lxb_css_syntax_token_cdo_t;
typedef lxb_css_syntax_token_base_t   lxb_css_syntax_token_cdc_t;
typedef lxb_css_syntax_token_base_t   lxb_css_syntax_token_colon_t;
typedef lxb_css_syntax_token_base_t   lxb_css_syntax_token_semicolon_t;
typedef lxb_css_syntax_token_base_t   lxb_css_syntax_token_comma_t;
typedef lxb_css_syntax_token_base_t   lxb_css_syntax_token_ls_bracket_t;
typedef lxb_css_syntax_token_base_t   lxb_css_syntax_token_rs_bracket_t;
typedef lxb_css_syntax_token_base_t   lxb_css_syntax_token_l_parenthesis_t;
typedef lxb_css_syntax_token_base_t   lxb_css_syntax_token_r_parenthesis_t;
typedef lxb_css_syntax_token_base_t   lxb_css_syntax_token_lc_bracket_t;
typedef lxb_css_syntax_token_base_t   lxb_css_syntax_token_rc_bracket_t;
typedef lxb_css_syntax_token_string_t lxb_css_syntax_token_comment_t;

typedef struct lxb_css_syntax_token {
    union lxb_css_syntax_token_u {
        lxb_css_syntax_token_base_t          base;
        lxb_css_syntax_token_comment_t       comment;
        lxb_css_syntax_token_number_t        number;
        lxb_css_syntax_token_dimension_t     dimension;
        lxb_css_syntax_token_percentage_t    percentage;
        lxb_css_syntax_token_hash_t          hash;
        lxb_css_syntax_token_string_t        string;
        lxb_css_syntax_token_bad_string_t    bad_string;
        lxb_css_syntax_token_delim_t         delim;
        lxb_css_syntax_token_l_parenthesis_t lparenthesis;
        lxb_css_syntax_token_r_parenthesis_t rparenthesis;
        lxb_css_syntax_token_cdc_t           cdc;
        lxb_css_syntax_token_function_t      function;
        lxb_css_syntax_token_ident_t         ident;
        lxb_css_syntax_token_url_t           url;
        lxb_css_syntax_token_bad_url_t       bad_url;
        lxb_css_syntax_token_at_keyword_t    at_keyword;
        lxb_css_syntax_token_whitespace_t    whitespace;
    }
    types;
}
lxb_css_syntax_token_t;


LXB_API const lxb_char_t *
lxb_css_syntax_token_type_name_by_id(lxb_css_syntax_token_type_t type);

LXB_API lxb_css_syntax_token_type_t
lxb_css_syntax_token_type_id_by_name(const lxb_char_t *type_name, size_t len);

LXB_API lxb_status_t
lxb_css_syntax_token_make_data(lxb_css_syntax_token_t *token, lexbor_in_node_t *in,
                               lexbor_mraw_t *mraw, lxb_css_syntax_token_data_t *td);

LXB_API lxb_status_t
lxb_css_syntax_token_serialize_cb(lxb_css_syntax_token_t *token,
                                  lxb_css_syntax_token_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_css_syntax_token_serialize_str(lxb_css_syntax_token_t *token,
                                   lexbor_str_t *str, lexbor_mraw_t *mraw);


/*
 * Inline functions
 */
lxb_inline lxb_css_syntax_token_t *
lxb_css_syntax_token_create(lexbor_dobject_t *dobj)
{
    return (lxb_css_syntax_token_t *) lexbor_dobject_calloc(dobj);
}

lxb_inline void
lxb_css_syntax_token_clean(lxb_css_syntax_token_t *token)
{
    memset(token, 0, sizeof(lxb_css_syntax_token_t));
}

lxb_inline lxb_css_syntax_token_t *
lxb_css_syntax_token_destroy(lxb_css_syntax_token_t *token,
                             lexbor_dobject_t *dobj)
{
    return (lxb_css_syntax_token_t *) lexbor_dobject_free(dobj, token);
}

lxb_inline const lxb_char_t *
lxb_css_syntax_token_type_name(lxb_css_syntax_token_t *token)
{
    return lxb_css_syntax_token_type_name_by_id(token->types.base.type);
}

lxb_inline lxb_css_syntax_token_type_t
lxb_css_syntax_token_type(lxb_css_syntax_token_t *token)
{
    return token->types.base.type;
}

/*
 * No inline functions for ABI.
 */
lxb_css_syntax_token_t *
lxb_css_syntax_token_create_noi(lexbor_dobject_t *dobj);

void
lxb_css_syntax_token_clean_noi(lxb_css_syntax_token_t *token);

lxb_css_syntax_token_t *
lxb_css_syntax_token_destroy_noi(lxb_css_syntax_token_t *token,
                                 lexbor_dobject_t *dobj);

const lxb_char_t *
lxb_css_syntax_token_type_name_noi(lxb_css_syntax_token_t *token);

lxb_css_syntax_token_type_t
lxb_css_syntax_token_type_noi(lxb_css_syntax_token_t *token);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_CSS_SYNTAX_TOKEN_H */

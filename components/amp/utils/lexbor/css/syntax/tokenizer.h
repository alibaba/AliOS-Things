/*
 * Copyright (C) 2018-2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CSS_SYNTAX_TOKENIZER_H
#define LEXBOR_CSS_SYNTAX_TOKENIZER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "core/in.h"
#include "core/array_obj.h"

#include "syntax/base.h"
#include "syntax/token.h"


typedef struct lxb_css_syntax_tokenizer lxb_css_syntax_tokenizer_t;

/* State */
typedef const lxb_char_t *
(*lxb_css_syntax_tokenizer_state_f)(lxb_css_syntax_tokenizer_t *tkz,
                                 const lxb_char_t *data, const lxb_char_t *end);

typedef lxb_css_syntax_token_t *
(*lxb_css_syntax_tokenizer_cb_f)(lxb_css_syntax_tokenizer_t *tkz,
                                 lxb_css_syntax_token_t *token, void *ctx);


enum lxb_css_syntax_tokenizer_opt {
    LXB_CSS_SYNTAX_TOKENIZER_OPT_UNDEF   = 0x00,
    LXB_CSS_SYNTAX_TOKENIZER_OPT_WO_COPY = 0x01
};

typedef enum {
    LXB_CSS_SYNTAX_TOKENIZER_BEGIN   = 0x00,
    LXB_CSS_SYNTAX_TOKENIZER_PROCESS = 0x01,
    LXB_CSS_SYNTAX_TOKENIZER_END     = 0x02
}
lxb_css_syntax_process_state_t;


typedef struct lxb_css_syntax_tokenizer_numeric {
    lxb_char_t data[128];
    lxb_char_t *buf;
    lxb_char_t *end;

    int        exponent;
    int        e_digit;
    bool       is_negative;
    bool       e_is_negative;
}
lxb_css_syntax_tokenizer_numeric_t;

struct lxb_css_syntax_tokenizer {
    lxb_css_syntax_tokenizer_state_f   state;
    lxb_css_syntax_tokenizer_state_f   return_state;

    lxb_css_syntax_tokenizer_cb_f      cb_token_done;
    void                               *cb_token_ctx;

    /* Current process token */
    lxb_css_syntax_token_t             *token;

    /* Memory for tokens */
    lexbor_dobject_t                   *dobj_token;
    lexbor_mraw_t                      *mraw;

    /* Incoming Buffer and current process buffer */
    lexbor_in_t                        *incoming;
    lexbor_in_node_t                   *incoming_first;
    lexbor_in_node_t                   *incoming_node;
    lexbor_in_node_t                   *incoming_done;

    lexbor_array_obj_t                 *parse_errors;

    /* Temp */
    int                                count;
    size_t                             num;
    const lxb_char_t                   *begin;
    const lxb_char_t                   *end;
    lxb_char_t                         str_ending;
    lxb_css_syntax_tokenizer_numeric_t numeric;
    lxb_css_syntax_token_data_t        token_data;

    /* Process */
    unsigned int                       opt;             /* bitmap */
    lxb_css_syntax_process_state_t     process_state;
    lxb_status_t                       status;
    bool                               is_eof;
    bool                               reuse;
};


LXB_API lxb_css_syntax_tokenizer_t *
lxb_css_syntax_tokenizer_create(void);

LXB_API lxb_status_t
lxb_css_syntax_tokenizer_init(lxb_css_syntax_tokenizer_t *tkz);

LXB_API void
lxb_css_syntax_tokenizer_clean(lxb_css_syntax_tokenizer_t *tkz);

LXB_API lxb_css_syntax_tokenizer_t *
lxb_css_syntax_tokenizer_destroy(lxb_css_syntax_tokenizer_t *tkz);


LXB_API lxb_status_t
lxb_css_syntax_tokenizer_parse(lxb_css_syntax_tokenizer_t *tkz,
                               const lxb_char_t *data, size_t size);

LXB_API lxb_status_t
lxb_css_syntax_tokenizer_begin(lxb_css_syntax_tokenizer_t *tkz);

LXB_API lxb_status_t
lxb_css_syntax_tokenizer_chunk(lxb_css_syntax_tokenizer_t *tkz,
                               const lxb_char_t *data, size_t size);

LXB_API lxb_status_t
lxb_css_syntax_tokenizer_end(lxb_css_syntax_tokenizer_t *tkz);

LXB_API const lxb_char_t *
lxb_css_syntax_tokenizer_change_incoming(lxb_css_syntax_tokenizer_t *tkz,
                                         const lxb_char_t *pos);


/*
 * Inline functions
 */
lxb_inline void
lxb_css_syntax_tokenizer_token_cb_set(lxb_css_syntax_tokenizer_t *tkz,
                                      lxb_css_syntax_tokenizer_cb_f cb_done,
                                      void *ctx)
{
    tkz->cb_token_done = cb_done;
    tkz->cb_token_ctx = ctx;
}

lxb_inline void
lxb_css_syntax_tokenizer_last_needed_in(lxb_css_syntax_tokenizer_t *tkz,
                                        lexbor_in_node_t *in)
{
    tkz->incoming_done = in;
}

lxb_inline lxb_status_t
lxb_css_syntax_tokenizer_make_data(lxb_css_syntax_tokenizer_t *tkz,
                                   lxb_css_syntax_token_t *token)
{
    lxb_status_t status;

    status = lxb_css_syntax_token_make_data(token, tkz->incoming_node,
                                            tkz->mraw, &tkz->token_data);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    lxb_css_syntax_tokenizer_last_needed_in(tkz, tkz->token_data.node_done);

    return LXB_STATUS_OK;
}

lxb_inline lxb_status_t
lxb_css_syntax_tokenizer_status(lxb_css_syntax_tokenizer_t *tkz)
{
    return tkz->status;
}

/*
 * No inline functions for ABI.
 */
void
lxb_css_syntax_tokenizer_token_cb_set_noi(lxb_css_syntax_tokenizer_t *tkz,
                                          lxb_css_syntax_tokenizer_cb_f cb_done,
                                          void *ctx);

void
lxb_css_syntax_tokenizer_last_needed_in_noi(lxb_css_syntax_tokenizer_t *tkz,
                                            lexbor_in_node_t *in);

lxb_status_t
lxb_css_syntax_tokenizer_make_data_noi(lxb_css_syntax_tokenizer_t *tkz,
                                       lxb_css_syntax_token_t *token);

lxb_status_t
lxb_css_syntax_tokenizer_status_noi(lxb_css_syntax_tokenizer_t *tkz);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_CSS_SYNTAX_TOKENIZER_H */

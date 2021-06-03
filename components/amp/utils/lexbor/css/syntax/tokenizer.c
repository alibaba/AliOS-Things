/*
 * Copyright (C) 2018-2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "syntax/tokenizer.h"
#include "syntax/tokenizer/error.h"
#include "syntax/state.h"

#define  CSS_PARSE_CHUNK_LEN    1024

const lxb_char_t *lxb_css_syntax_tokenizer_eof = (const lxb_char_t *) "\x00";


static void
lxb_css_syntax_tokenizer_erase_incoming(lxb_css_syntax_tokenizer_t *tkz);

static lxb_css_syntax_token_t *
lxb_css_syntax_tokenizer_cb_done(lxb_css_syntax_tokenizer_t *tkz,
                                 lxb_css_syntax_token_t *token, void *ctx);

static void
lxb_css_syntax_tokenizer_process(lxb_css_syntax_tokenizer_t *tkz,
                                 const lxb_char_t *data, size_t size);

static const lxb_char_t *
lxb_css_syntax_tokenizer_change_incoming_eof(lxb_css_syntax_tokenizer_t *tkz,
                                             const lxb_char_t *pos);


lxb_css_syntax_tokenizer_t *
lxb_css_syntax_tokenizer_create(void)
{
    return lexbor_calloc(1, sizeof(lxb_css_syntax_tokenizer_t));
}

lxb_status_t
lxb_css_syntax_tokenizer_init(lxb_css_syntax_tokenizer_t *tkz)
{
    if (tkz == NULL) {
        return LXB_STATUS_ERROR_OBJECT_IS_NULL;
    }

    lxb_status_t status;

    /* Init Token */
    tkz->token = NULL;

    tkz->dobj_token = lexbor_dobject_create();
    status = lexbor_dobject_init(tkz->dobj_token,
                                 1024, sizeof(lxb_css_syntax_token_t));
    if (status != LXB_STATUS_OK) {
        return status;
    }

    /* Incoming */
    tkz->incoming = lexbor_in_create();
    status = lexbor_in_init(tkz->incoming, 32);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    tkz->incoming_first = NULL;
    tkz->incoming_node = NULL;
    tkz->incoming_done = NULL;

    /* mraw */
    tkz->mraw = lexbor_mraw_create();
    status = lexbor_mraw_init(tkz->mraw, 1024);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    /* Parse errors */
    tkz->parse_errors = lexbor_array_obj_create();
    status = lexbor_array_obj_init(tkz->parse_errors, 16,
                                   sizeof(lxb_css_syntax_tokenizer_error_t));
    if (status != LXB_STATUS_OK) {
        return status;
    }

    tkz->cb_token_done = lxb_css_syntax_tokenizer_cb_done;
    tkz->cb_token_ctx = NULL;

    tkz->is_eof = false;
    tkz->status = LXB_STATUS_OK;

    tkz->opt = LXB_CSS_SYNTAX_TOKENIZER_OPT_UNDEF;
    tkz->process_state = LXB_CSS_SYNTAX_TOKENIZER_BEGIN;

    tkz->numeric.end = tkz->numeric.data
        + sizeof(tkz->numeric.data) / sizeof(lxb_char_t);

    return LXB_STATUS_OK;
}

void
lxb_css_syntax_tokenizer_clean(lxb_css_syntax_tokenizer_t *tkz)
{
    lxb_css_syntax_tokenizer_erase_incoming(tkz);

    lexbor_in_clean(tkz->incoming);

    lexbor_mraw_clean(tkz->mraw);
    lexbor_dobject_clean(tkz->dobj_token);
    lexbor_array_obj_clean(tkz->parse_errors);

    tkz->status = LXB_STATUS_OK;
    tkz->process_state = LXB_CSS_SYNTAX_TOKENIZER_BEGIN;
}

lxb_css_syntax_tokenizer_t *
lxb_css_syntax_tokenizer_destroy(lxb_css_syntax_tokenizer_t *tkz)
{
    if (tkz == NULL) {
        return NULL;
    }

    lxb_css_syntax_tokenizer_erase_incoming(tkz);

    tkz->incoming = lexbor_in_destroy(tkz->incoming, true);

    tkz->mraw = lexbor_mraw_destroy(tkz->mraw, true);
    tkz->dobj_token = lexbor_dobject_destroy(tkz->dobj_token, true);
    tkz->parse_errors = lexbor_array_obj_destroy(tkz->parse_errors, true);

    return lexbor_free(tkz);
}

static void
lxb_css_syntax_tokenizer_erase_incoming(lxb_css_syntax_tokenizer_t *tkz)
{
    lexbor_in_node_t *next_node;

    while (tkz->incoming_first != NULL)
    {
        if (tkz->incoming_first->opt & LEXBOR_IN_OPT_ALLOC) {
            lexbor_free((lxb_char_t *) tkz->incoming_first->begin);
        }

        next_node = tkz->incoming_first->next;

        lexbor_in_node_destroy(tkz->incoming, tkz->incoming_first, true);

        tkz->incoming_first = next_node;
    }

    tkz->incoming_done = NULL;
}

static lxb_css_syntax_token_t *
lxb_css_syntax_tokenizer_cb_done(lxb_css_syntax_tokenizer_t *tkz,
                                 lxb_css_syntax_token_t *token, void *ctx)
{
    return token;
}

lxb_status_t
lxb_css_syntax_tokenizer_begin(lxb_css_syntax_tokenizer_t *tkz)
{
    if (tkz->process_state == LXB_CSS_SYNTAX_TOKENIZER_PROCESS) {
        return LXB_STATUS_ERROR_WRONG_STAGE;
    }

    tkz->is_eof = false;
    tkz->status = LXB_STATUS_OK;
    tkz->state = lxb_css_syntax_state_data;
    tkz->opt = LXB_CSS_SYNTAX_TOKENIZER_OPT_UNDEF;

    if (tkz->token == NULL) {
        tkz->token = lxb_css_syntax_token_create(tkz->dobj_token);
        if (tkz->token == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    tkz->process_state = LXB_CSS_SYNTAX_TOKENIZER_PROCESS;

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_css_syntax_tokenizer_chunk(lxb_css_syntax_tokenizer_t *tkz,
                               const lxb_char_t *data, size_t size)
{
    if (tkz->process_state != LXB_CSS_SYNTAX_TOKENIZER_PROCESS) {
        tkz->status = LXB_STATUS_ERROR_WRONG_STAGE;

        return tkz->status;
    }

    lxb_char_t *copied;
    lexbor_in_node_t *next_node;

    if (tkz->opt & LXB_CSS_SYNTAX_TOKENIZER_OPT_WO_COPY) {
        tkz->incoming_node = lexbor_in_node_make(tkz->incoming, tkz->incoming_node,
                                                 data, size);
        if (tkz->incoming_node == NULL) {
            lxb_css_syntax_tokenizer_erase_incoming(tkz);

            tkz->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

            return tkz->status;
        }

        if (tkz->incoming_first == NULL) {
            tkz->incoming_first = tkz->incoming_node;
        }

        lxb_css_syntax_tokenizer_process(tkz, data, size);

        goto done;
    }

    copied = lexbor_malloc(sizeof(lxb_char_t) * size);
    if (copied == NULL) {
        lxb_css_syntax_tokenizer_erase_incoming(tkz);

        tkz->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return tkz->status;
    }

    memcpy(copied, data, sizeof(lxb_char_t) * size);

    tkz->incoming_node = lexbor_in_node_make(tkz->incoming, tkz->incoming_node,
                                             copied, size);
    if (tkz->incoming_node == NULL) {
        lexbor_free(copied);
        lxb_css_syntax_tokenizer_erase_incoming(tkz);

        tkz->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;

        return tkz->status;
    }

    if (tkz->incoming_first == NULL) {
        tkz->incoming_first = tkz->incoming_node;
    }

    tkz->incoming_node->opt = LEXBOR_IN_OPT_ALLOC;

    lxb_css_syntax_tokenizer_process(tkz, copied, size);

done:

    if (tkz->status != LXB_STATUS_OK) {
        lxb_css_syntax_tokenizer_erase_incoming(tkz);

        return tkz->status;
    }

    if (tkz->incoming_done != NULL) {
        while (tkz->incoming_first != tkz->incoming_done) {
            if (tkz->incoming_first->opt & LEXBOR_IN_OPT_ALLOC) {
                lexbor_free((lxb_char_t *) tkz->incoming_first->begin);
            }

            next_node = tkz->incoming_first->next;

            lexbor_in_node_destroy(tkz->incoming, tkz->incoming_first, true);

            tkz->incoming_first = next_node;
            next_node->prev = NULL;
        }
    }

    return tkz->status;
}

static void
lxb_css_syntax_tokenizer_process(lxb_css_syntax_tokenizer_t *tkz,
                                 const lxb_char_t *data, size_t size)
{
    lexbor_in_node_t *in_node;
    const lxb_char_t *end = data + size;

    while (data < end) {
        data = tkz->state(tkz, data, end);
    }

    if (tkz->incoming_node->next != NULL) {

reuse:

        in_node = tkz->incoming_node;
        data = in_node->use;

        for (;;) {
            while (data < in_node->end) {
                data = tkz->state(tkz, data, in_node->end);
            }

            if (in_node != tkz->incoming_node) {
                goto reuse;
            }

            in_node->use = in_node->end;

            if (in_node->next == NULL) {
                break;
            }

            in_node = in_node->next;
            tkz->incoming_node = in_node;

            data = in_node->begin;
        }
    }

    tkz->incoming_node->use = end;
}

lxb_status_t
lxb_css_syntax_tokenizer_end(lxb_css_syntax_tokenizer_t *tkz)
{
    if (tkz->process_state != LXB_CSS_SYNTAX_TOKENIZER_PROCESS) {
        tkz->status = LXB_STATUS_ERROR_WRONG_STAGE;

        return tkz->status;
    }

    const lxb_char_t *data, *end;

    /*
     * Send a fake EOF data (not added in to incoming buffer chain)
     * If some state change incoming buffer,
     * then we need parse again all buffers after current position
     * and try again send fake EOF.
     */
    do {
        data = lxb_css_syntax_tokenizer_eof;
        end = lxb_css_syntax_tokenizer_eof + 1UL;

        tkz->is_eof = true;

        while (tkz->state(tkz, data, end) < end) {
            /* empty loop */
        }

        if (tkz->reuse) {
            tkz->is_eof = false;
            data = tkz->incoming_node->use;

            for (;;) {
                while (data < tkz->incoming_node->end) {
                    data = tkz->state(tkz, data, tkz->incoming_node->end);
                }

                if (tkz->incoming_node->next == NULL) {
                    break;
                }

                tkz->incoming_node->use = tkz->incoming_node->end;
                tkz->incoming_node = tkz->incoming_node->next;

                data = tkz->incoming_node->begin;
            }

            tkz->reuse = false;
        }
        else {
            break;
        }
    }
    while (1);

    if (tkz->status != LXB_STATUS_OK) {
        return tkz->status;
    }

    tkz->is_eof = false;

    /* Emit token: END OF FILE */
    lxb_css_syntax_token_clean(tkz->token);

    tkz->token->types.base.type = LXB_CSS_SYNTAX_TOKEN__EOF;

    tkz->token = tkz->cb_token_done(tkz, tkz->token, tkz->cb_token_ctx);

    if (tkz->token == NULL && tkz->status == LXB_STATUS_OK) {
        tkz->status = LXB_STATUS_ERROR;
    }

    tkz->process_state = LXB_CSS_SYNTAX_TOKENIZER_END;

    return tkz->status;
}

lxb_status_t
lxb_css_syntax_tokenizer_parse(lxb_css_syntax_tokenizer_t *tkz,
                               const lxb_char_t *data, size_t size)
{
    uint32_t len = 0;
    uint32_t size_chunk = 0;
    unsigned int old_opt = tkz->opt;

    tkz->opt |= LXB_CSS_SYNTAX_TOKENIZER_OPT_WO_COPY;

    lxb_css_syntax_tokenizer_begin(tkz);
    if (tkz->status != LXB_STATUS_OK) {
        goto done;
    }

    len = CSS_PARSE_CHUNK_LEN < size? CSS_PARSE_CHUNK_LEN : size;

    for (size_chunk = 0; size_chunk < size; ) {
        lxb_css_syntax_tokenizer_chunk(tkz, data + size_chunk, len );

        size_chunk += len;
        len = (size - size_chunk) >= CSS_PARSE_CHUNK_LEN ? CSS_PARSE_CHUNK_LEN : (size - size_chunk);
    }

    if (tkz->status != LXB_STATUS_OK) {
        goto done;
    }

    lxb_css_syntax_tokenizer_end(tkz);

done:

    tkz->opt = old_opt;

    return tkz->status;
}

const lxb_char_t *
lxb_css_syntax_tokenizer_change_incoming(lxb_css_syntax_tokenizer_t *tkz,
                                         const lxb_char_t *pos)
{
    if (tkz->is_eof) {
        return lxb_css_syntax_tokenizer_change_incoming_eof(tkz, pos);
    }

    if (lexbor_in_segment(tkz->incoming_node, pos)) {
        tkz->incoming_node->use = pos;

        return pos;
    }

    lexbor_in_node_t *node = tkz->incoming_node;
    tkz->incoming_node = lexbor_in_node_find(tkz->incoming_node, pos);

    if (tkz->incoming_node == NULL) {
        tkz->status = LXB_STATUS_ERROR;
        tkz->incoming_node = node;

        return tkz->incoming_node->end;
    }

    tkz->incoming_node->use = pos;

    return node->end;
}

static const lxb_char_t *
lxb_css_syntax_tokenizer_change_incoming_eof(lxb_css_syntax_tokenizer_t *tkz,
                                             const lxb_char_t *pos)
{
    if (pos == lxb_css_syntax_tokenizer_eof) {
        return pos;
    }

    tkz->reuse = true;

    if (lexbor_in_segment(tkz->incoming_node, pos)) {
        tkz->incoming_node->use = pos;

        return lxb_css_syntax_tokenizer_eof + 1;
    }

    lexbor_in_node_t *node = tkz->incoming_node;
    tkz->incoming_node = lexbor_in_node_find(tkz->incoming_node, pos);

    if (tkz->incoming_node == NULL) {
        tkz->reuse = false;

        tkz->status = LXB_STATUS_ERROR;
        tkz->incoming_node = node;

        return lxb_css_syntax_tokenizer_eof + 1;
    }

    tkz->incoming_node->use = pos;

    return lxb_css_syntax_tokenizer_eof + 1;
}

/*
 * No inline functions for ABI.
 */
void
lxb_css_syntax_tokenizer_token_cb_set_noi(lxb_css_syntax_tokenizer_t *tkz,
                                          lxb_css_syntax_tokenizer_cb_f cb_done,
                                          void *ctx)
{
    lxb_css_syntax_tokenizer_token_cb_set(tkz, cb_done, ctx);
}

void
lxb_css_syntax_tokenizer_last_needed_in_noi(lxb_css_syntax_tokenizer_t *tkz,
                                            lexbor_in_node_t *in)
{
    lxb_css_syntax_tokenizer_last_needed_in(tkz, in);
}

lxb_status_t
lxb_css_syntax_tokenizer_make_data_noi(lxb_css_syntax_tokenizer_t *tkz,
                                       lxb_css_syntax_token_t *token)
{
    return lxb_css_syntax_tokenizer_make_data(tkz, token);
}

lxb_status_t
lxb_css_syntax_tokenizer_status_noi(lxb_css_syntax_tokenizer_t *tkz)
{
    return lxb_css_syntax_tokenizer_status(tkz);
}

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "aos/list.h"
#include "packet.h"

typedef struct {
    dlist_t next;
    int message_type;
    message_handler_t hdl;
    void *ctx;
} message_handle_node_t;

static dlist_t handlers = AOS_DLIST_INIT(handlers);

int dda_register_message_handler(int message_type, message_handler_t hdl, void *ctx)
{
    message_handle_node_t *node = malloc(sizeof(*node));
    node->message_type = message_type;
    node->hdl = hdl;
    node->ctx = ctx;

    dlist_add_tail(&node->next, &handlers);
    return 0;
}

int dda_unregister_message_handler(int message_type, message_handler_t hdl, void *ctx)
{
    message_handle_node_t *node;
    dlist_for_each_entry(&handlers, node, message_handle_node_t, next) {
        if (node->hdl == hdl && node->ctx == ctx) {
            dlist_del(&node->next);
            free(node);
            return 0;
        }
    }

    return -1;
}

void dda_handle_message(ipc_msg_t *msg)
{
    message_handle_node_t *node;
    dlist_for_each_entry(&handlers, node, message_handle_node_t, next) {
        if (msg->type != node->message_type) continue;

        node->hdl(msg->cmd, msg+1, msg->payload_len, node->ctx);
    }
}


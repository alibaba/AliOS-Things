/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <sys/time.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "aos/yloop.h"

#include <network/network.h>

#include "loop_hal.h"
#include "yloop.h"

#define TAG "yloop"

typedef struct yloop_timeout_s {
    dlist_t          next;
    long long        timeout_ms;
    void            *private_data;
    aos_call_t       cb;
    int              ms;
} yloop_timeout_t;

yloop_ctx_t    *g_main_ctx = NULL;
static unsigned int g_loop_key;

extern int aos_poll(struct pollfd *fds, int nfds, int timeout);
extern int aos_fcntl(int fd, int cmd, int val);

static inline void _set_context(yloop_ctx_t *ctx)
{
    hal_loop_task_setspecific(g_loop_key, ctx);
}

static inline yloop_ctx_t *_get_context(void)
{
    return hal_loop_task_getspecific(g_loop_key);
}

static inline yloop_ctx_t *get_context(void)
{
    yloop_ctx_t *ctx = _get_context();
    if (!ctx) {
        _set_context(g_main_ctx);
        return g_main_ctx;
    }
    return ctx;
}

void aos_loop_set_eventfd(int fd)
{
    yloop_ctx_t *ctx = get_context();
    ctx->eventfd = fd;
}

int aos_loop_get_eventfd(void *loop)
{
    yloop_ctx_t *ctx = loop ? loop : get_context();
    return ctx->eventfd;
}

aos_loop_t aos_current_loop(void)
{
    return get_context();
}

aos_loop_t aos_loop_init(void)
{
    yloop_ctx_t *ctx = _get_context();

    if (!g_main_ctx) {
        hal_loop_task_key_create(&g_loop_key);
    } else if (ctx) {
        LOGE(TAG, "yloop already inited");
        return ctx;
    }

    ctx = aos_zalloc(sizeof(*g_main_ctx));
    if (ctx == NULL) {
        return ctx;
    }

    if (!g_main_ctx) {
        g_main_ctx = ctx;
    }

    dlist_init(&ctx->timeouts);
    ctx->eventfd = -1;
    _set_context(ctx);

    aos_event_service_init();

    return ctx;
}

int aos_poll_read_fd(int sock, aos_poll_call_t cb, void *private_data)
{
    yloop_ctx_t *ctx = get_context();
    if (sock  < 0) {
        return -EINVAL;
    }

    yloop_sock_t *new_sock;
    struct pollfd *new_loop_pollfds;
    int cnt = ctx->reader_count + 1;

    new_sock = aos_malloc(cnt * sizeof(yloop_sock_t));
    new_loop_pollfds = aos_malloc(cnt * sizeof(struct pollfd));

    if (new_sock == NULL || new_loop_pollfds == NULL) {
        LOGE(TAG, "out of memory");
        aos_free(new_sock);
        aos_free(new_loop_pollfds);
        return -ENOMEM;
    }

    int status = aos_fcntl(sock, F_GETFL, 0);
    aos_fcntl(sock, F_SETFL, status | O_NONBLOCK);

    ctx->reader_count++;

    memcpy(new_sock, ctx->readers, (cnt - 1) * sizeof(yloop_sock_t));
    aos_free(ctx->readers);
    ctx->readers = new_sock;

    memcpy(new_loop_pollfds, ctx->pollfds, (cnt - 1) * sizeof(struct pollfd));
    aos_free(ctx->pollfds);
    ctx->pollfds = new_loop_pollfds;

    new_sock += cnt - 1;
    new_sock->sock = sock;
    new_sock->private_data = private_data;
    new_sock->cb = cb;

    if (sock > ctx->max_sock) {
        ctx->max_sock = sock;
    }

    return 0;
}

void aos_cancel_poll_read_fd(int sock, aos_poll_call_t action, void *param)
{
    yloop_ctx_t *ctx = get_context();
    if (ctx->readers == NULL || ctx->reader_count == 0) {
        return;
    }

    int i;
    for (i = 0; i < ctx->reader_count; i++) {
        if (ctx->readers[i].sock == sock) {
            break;
        }
    }

    if (i == ctx->reader_count) {
        return;
    }

    if (i != ctx->reader_count - 1) {
        memmove(&ctx->readers[i], &ctx->readers[i + 1],
                (ctx->reader_count - i - 1) *
                sizeof(yloop_sock_t));
    }

    ctx->reader_count--;
}

int aos_post_delayed_action(int ms, aos_call_t action, void *param)
{
    if (action == NULL) {
        return -EINVAL;
    }

    yloop_ctx_t *ctx = get_context();
    yloop_timeout_t *timeout = aos_malloc(sizeof(*timeout));
    if (timeout == NULL) {
        return -ENOMEM;
    }

    timeout->timeout_ms = aos_now_ms() + ms;
    timeout->private_data = param;
    timeout->cb = action;
    timeout->ms = ms;

    yloop_timeout_t *tmp;

    dlist_for_each_entry(&ctx->timeouts, tmp, yloop_timeout_t, next) {
        if (timeout->timeout_ms < tmp->timeout_ms) {
            break;
        }
    }

    dlist_add_tail(&timeout->next, &tmp->next);

    return 0;
}

void aos_cancel_delayed_action(int ms, aos_call_t cb, void *private_data)
{
    yloop_ctx_t *ctx = get_context();
    yloop_timeout_t *tmp;

    dlist_for_each_entry(&ctx->timeouts, tmp, yloop_timeout_t, next) {
        if (ms != -1 && tmp->ms != ms) {
            continue;
        }

        if (tmp->cb != cb) {
            continue;
        }

        if (tmp->private_data != private_data) {
            continue;
        }

        dlist_del(&tmp->next);
        aos_free(tmp);
        return;
    }
}

void aos_loop_run(void)
{
    yloop_ctx_t *ctx = get_context();

    while (!ctx->terminate &&
           (!dlist_empty(&ctx->timeouts) || ctx->reader_count > 0)) {
        int delayed_ms = -1;
        int readers = ctx->reader_count;
        int i;

        if (!dlist_empty(&ctx->timeouts)) {
            yloop_timeout_t *tmo = dlist_first_entry(&ctx->timeouts, yloop_timeout_t, next);
            long long now = aos_now_ms();

            if (now < tmo->timeout_ms) {
                delayed_ms = tmo->timeout_ms - now;
            } else {
                delayed_ms = 0;
            }
        }

        for (i = 0; i < readers; i++) {
            ctx->pollfds[i].fd = ctx->readers[i].sock;
            ctx->pollfds[i].events = POLLIN;
        }

        int res = aos_poll(ctx->pollfds, readers, delayed_ms);

        if (res < 0 && res != EINTR) {
            LOGE(TAG, "aos_poll");
            return;
        }

        /* check if some registered timeouts have occurred */
        if (!dlist_empty(&ctx->timeouts)) {
            yloop_timeout_t *tmo = dlist_first_entry(&ctx->timeouts, yloop_timeout_t, next);
            long long now = aos_now_ms();

            if (now >= tmo->timeout_ms) {
                dlist_del(&tmo->next);
                tmo->cb(tmo->private_data);
                aos_free(tmo);
            }
        }

        if (res <= 0) {
            continue;
        }

        for (i = 0; i < readers; i++) {
            if (ctx->pollfds[i].revents & POLLIN) {
                ctx->readers[i].cb(
                    ctx->readers[i].sock,
                    ctx->readers[i].private_data);
            }
        }
    }

    ctx->terminate = 0;
}

void aos_loop_exit(void)
{
    yloop_ctx_t *ctx = get_context();
    ctx->terminate = 1;
}

void aos_loop_destroy(void)
{
    yloop_ctx_t *ctx = _get_context();

    if (ctx == NULL) {
        return;
    }

    aos_event_service_deinit(ctx->eventfd);

    while (!dlist_empty(&ctx->timeouts)) {
        yloop_timeout_t *timeout = dlist_first_entry(&ctx->timeouts, yloop_timeout_t,
                                                     next);
        dlist_del(&timeout->next);
        aos_free(timeout);
    }

    aos_free(ctx->readers);
    aos_free(ctx->pollfds);

    _set_context(NULL);
    if (ctx == g_main_ctx) {
        g_main_ctx = NULL;
    }
    aos_free(ctx);
}


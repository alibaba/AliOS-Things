/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifdef RHINO_CONFIG_IPC

#ifdef CONFIG_NET_LWIP
    #include <lwip/sockets.h>
    #include <lwip/un.h>
#else
    #include <sys/socket.h>
    #include <sys/un.h>
#endif

#include <sys/select.h>
#include <stdatomic.h>

#include "k_api.h"
#include "debug_api.h"

#include "ipc.h"
#include "kipc_utils.h"

#ifdef __cplusplus
extern "C" {
#endif
#ifndef CONFIG_NET_LWIP
#define FD_UDS_SOCKET_OFFSET  0
#define FD_UDS_SOCKET_NUM     4
#endif

#define UDS_SOCKET_FD_OFFSET      (FD_UDS_SOCKET_OFFSET)
#define UDS_SOCKET_MAX_COUNT      (FD_UDS_SOCKET_NUM)
#define UDS_SOCKET_MASK           (UDS_SOCKET_MAX_COUNT - 1)
#define UDS_SOCKET_NAME_MAX_LEN   (108)
#define UDS_SOCKET_BUF_MAX_LEN    (4096)
#define UDS_SOCKET_CLIENT_LIMITED (UDS_SOCKET_MAX_COUNT - 1)
#define UDS_SOCKET_HANDLE_FREE    (0)
#define UDS_SOCKET_HANDLE_INVALID (-1)
#define UDS_SOCKET_PEER_SIZE      (sizeof(uint16_t))
#define UDS_FD_SANITY_CHECK(fd, ret)   \
    if ((fd) < UDS_SOCKET_FD_OFFSET ||\
        (fd) > UDS_SOCKET_FD_OFFSET + UDS_SOCKET_MASK) {\
        return ret; \
    }

struct uds_desc_t {
    char name[UDS_SOCKET_NAME_MAX_LEN + 1];
    uint8_t limited;   /* max count of client */
    uint8_t as_server;  /* work as server */
};

struct uds_socket_t {
    int type;
    int ipc_handle;
    int peer;
    int psock;
    struct uds_desc_t *desc;
};

struct uds_server_sock_t {
    uint8_t ccount;    /* the count of client */
    uint16_t sock;
    klist_t node;
};

struct uds_ctx_t {
    struct uds_socket_t *sockets;
    uint16_t allocator;  /* allocate for Unix domain socket */
    uint16_t count;      /* current count of UDS */
    klist_t slst;        /* socket server list */
    klist_t select_lst;  /* select list for select operation */
    kmutex_t mtx;        /* object sync mutex */
};

struct uds_select_ctx_t {
    blk_obj_t blk_obj;
    int nfds;
    fd_set *readfds;
    fd_set *writefds;
    fd_set *exceptfds;
    klist_t node;
};

enum UDS_OP_T {
    UDS_RD = 0,
    UDS_WR,
    UDS_DEL
};

static struct uds_ctx_t *uds_ctx_ptr = NULL;

static kstat_t __uds_select_add(int nfds, fd_set *readfds,
                                fd_set *writefds, fd_set *exceptfds,
                                struct timeval *timeout)
{
    struct uds_ctx_t *ctx = uds_ctx_ptr;
    struct uds_select_ctx_t *select;
    uint8_t cur_cpu_num;
    sys_time_t time = 0;
    ktask_t *task;
    kstat_t res;

    CPSR_ALLOC();

    select = (struct uds_select_ctx_t *)krhino_mm_alloc(sizeof(*select));
    if (select == NULL) {
        return -RHINO_NO_MEM;
    }
    memset(select, 0, sizeof(*select));

    klist_init(&select->blk_obj.blk_list);
    select->blk_obj.blk_policy = BLK_POLICY_FIFO;
#if (RHINO_CONFIG_TASK_DEL > 0)
    select->blk_obj.cancel = 1u;
#endif
    klist_init(&select->node);
    select->nfds = nfds;
    select->readfds = readfds;
    select->writefds = writefds;
    select->exceptfds = exceptfds;

    if (timeout) {
        time += timeout->tv_sec * 1000;
        time += timeout->tv_usec / 1000;
    }

    res = krhino_mutex_lock(&ctx->mtx, krhino_ms_to_ticks(time));
    if (res != RHINO_SUCCESS && res != RHINO_MUTEX_OWNER_NESTED) {
        return -res;
    }
    klist_insert(&ctx->select_lst, &select->node);
    krhino_mutex_unlock(&ctx->mtx);

    RHINO_CRITICAL_ENTER();
    cur_cpu_num = cpu_cur_get();
    task = g_active_task[cur_cpu_num];
    pend_to_blk_obj(&select->blk_obj, task, krhino_ms_to_ticks(time));
    RHINO_CRITICAL_EXIT_SCHED();

    RHINO_CPU_INTRPT_DISABLE();
    res = pend_state_end_proc(task, &select->blk_obj);
    RHINO_CPU_INTRPT_ENABLE();

    krhino_mm_free(select);

    return res != RHINO_SUCCESS ? -res : RHINO_SUCCESS;
}

static kstat_t __uds_select_check(int sock, int op)
{
    ktask_t *task;
    klist_t *head, *iter;
    struct uds_ctx_t *ctx = uds_ctx_ptr;
    struct uds_select_ctx_t *select = NULL;

    uint16_t csock = sock - UDS_SOCKET_FD_OFFSET;

    char update = 0;
    kstat_t res;

    CPSR_ALLOC();

    res = krhino_mutex_lock(&ctx->mtx, RHINO_WAIT_FOREVER);
    if (res != RHINO_SUCCESS && res != RHINO_MUTEX_OWNER_NESTED) {
        return -res;
    }
    head = &ctx->select_lst;
    iter = head->next;

    while (iter != head) {
        select = krhino_list_entry(iter, struct uds_select_ctx_t, node);
        iter = iter->next;
        if (select->readfds && op == UDS_WR &&
            FD_ISSET(ctx->sockets[csock].psock + UDS_SOCKET_FD_OFFSET, select->readfds) &&
            aos_ipc_handle_readable(ctx->sockets[csock].peer) == RHINO_SUCCESS) {
            FD_ZERO(select->readfds);
            FD_SET(ctx->sockets[csock].psock + UDS_SOCKET_FD_OFFSET, select->readfds);
            update = 1;
            break;
        }

        if (select->writefds && op == UDS_RD &&
            FD_ISSET(ctx->sockets[csock].psock + UDS_SOCKET_FD_OFFSET, select->writefds) &&
            aos_ipc_handle_writable(ctx->sockets[csock].peer) == RHINO_SUCCESS) {
            FD_ZERO(select->writefds);
            FD_SET(ctx->sockets[csock].psock + UDS_SOCKET_FD_OFFSET, select->writefds);
            update = 1;
            break;
        }

        /* exceptfds is not support now */
    }

    if (update == 0) {
        krhino_mutex_unlock(&ctx->mtx);
        return RHINO_SUCCESS;
    }

    klist_rm(&select->node);
    head = &select->blk_obj.blk_list;
    task = krhino_list_entry(head->next, ktask_t, task_list);
    krhino_mutex_unlock(&ctx->mtx);

    //krhino_mm_free(select);

    RHINO_CRITICAL_ENTER();
    pend_task_wakeup(task);
    RHINO_CRITICAL_EXIT_SCHED();

    return RHINO_SUCCESS;
}

static int __uds_ctx_init()
{
    int len;
    struct uds_ctx_t *ctx = NULL;

    CPSR_ALLOC();

    if (__atomic_compare_exchange(&uds_ctx_ptr, &ctx, &uds_ctx_ptr,
                                  0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
        RHINO_CRITICAL_ENTER();
        if (uds_ctx_ptr == NULL) {
            /* slow path */
            uds_ctx_ptr = (struct uds_ctx_t *)krhino_mm_alloc(sizeof(*uds_ctx_ptr));
            if (uds_ctx_ptr == NULL) {
                RHINO_CRITICAL_EXIT();
                return -RHINO_NO_MEM;
            }
            memset(uds_ctx_ptr, 0, sizeof(*uds_ctx_ptr));
            len = sizeof(struct uds_socket_t) * UDS_SOCKET_MAX_COUNT;
            uds_ctx_ptr->sockets = (struct uds_socket_t *)krhino_mm_alloc(len);
            if (uds_ctx_ptr->sockets == NULL) {
                krhino_mm_free(uds_ctx_ptr);
                uds_ctx_ptr = NULL;
                RHINO_CRITICAL_EXIT();
                return -RHINO_NO_MEM;
            }
            memset(uds_ctx_ptr->sockets, 0, len);
            klist_init(&uds_ctx_ptr->slst);
            klist_init(&uds_ctx_ptr->select_lst);
            krhino_mutex_create(&uds_ctx_ptr->mtx, "uds_ctx_mtx");
        }
        RHINO_CRITICAL_EXIT();
    }

    return RHINO_SUCCESS;
}

static void *__uds_server_search(int sock, const char *name)
{
    klist_t *head, *iter;
    struct uds_ctx_t *ctx = uds_ctx_ptr;
    struct uds_server_sock_t *server, *s = NULL;

    head = &ctx->slst;
    iter = head->next;

    while (iter != head) {
        server = krhino_list_entry(iter, struct uds_server_sock_t, node);
        iter = iter->next;
        if (sock != UDS_SOCKET_HANDLE_INVALID && server->sock == sock) {
            s = server;
            break;
        }
        if (name != NULL && aos_ipc_strlen(name) > 0 &&
            ctx->sockets[server->sock].desc != NULL &&
            aos_ipc_strlen(ctx->sockets[server->sock].desc->name) > 0 &&
            aos_ipc_strcmp(ctx->sockets[server->sock].desc->name, name) == 0) {
            s = server;
            break;
        }
    }

    return s;
}

static int __uds_socket_alloc()
{
    struct uds_ctx_t *ctx = uds_ctx_ptr;

    do {
        if (ctx->allocator >= UDS_SOCKET_MAX_COUNT) {
            ctx->allocator = 0;
        }

        if (ctx->sockets[ctx->allocator ++].ipc_handle == UDS_SOCKET_HANDLE_FREE) {
            break;
        }
    } while (1);

    return ctx->allocator - 1;
}

int uds_socket(int domain, int type, int protocol)
{
    int sock;
    kstat_t res;
    struct uds_ctx_t *ctx;

    if (__uds_ctx_init() != RHINO_SUCCESS) {
        return -RHINO_NO_MEM;
    }
    __atomic_load(&uds_ctx_ptr, &ctx, __ATOMIC_RELAXED);

    res = krhino_mutex_lock(&ctx->mtx, RHINO_WAIT_FOREVER);
    if (res != RHINO_SUCCESS && res != RHINO_MUTEX_OWNER_NESTED) {
        return -res;
    }

    if (ctx->count >= UDS_SOCKET_MAX_COUNT) {
        krhino_mutex_unlock(&ctx->mtx);
        return -RHINO_UDS_FULL;
    }

    sock = __uds_socket_alloc();
    memset(ctx->sockets + sock, 0, sizeof(ctx->sockets[0]));
    res = aos_ipc_create_stream(NULL, UDS_SOCKET_BUF_MAX_LEN,
            &ctx->sockets[sock].ipc_handle);
    if (res == RHINO_SUCCESS) {
        ctx->count ++;
    }
    krhino_mutex_unlock(&ctx->mtx);

    return res == RHINO_SUCCESS ? sock + UDS_SOCKET_FD_OFFSET : res;
}

int uds_close(int sockfd)
{
    int res;
    struct uds_ctx_t *ctx;
    struct uds_socket_t *uds_sock;
    struct uds_server_sock_t *server;

    UDS_FD_SANITY_CHECK(sockfd, -RHINO_UDS_NOT_EXIST);

    __atomic_load(&uds_ctx_ptr, &ctx, __ATOMIC_RELAXED);

    if (ctx == NULL) {
        return -RHINO_UDS_NOT_INIT;
    }

    res = krhino_mutex_lock(&ctx->mtx, RHINO_WAIT_FOREVER);
    if (res != RHINO_SUCCESS && res != RHINO_MUTEX_OWNER_NESTED) {
        return -res;
    }

    sockfd -= UDS_SOCKET_FD_OFFSET;
    server = __uds_server_search(sockfd, NULL);
    if (server != NULL) {
        klist_rm(&server->node);
        krhino_mm_free(server);
    }

    uds_sock = &ctx->sockets[sockfd];
    if (uds_sock->ipc_handle != UDS_SOCKET_HANDLE_FREE) {
        aos_ipc_del(uds_sock->ipc_handle);
        if (uds_sock->desc) {
            krhino_mm_free(uds_sock->desc);
        }
        memset(uds_sock, 0, sizeof(*uds_sock));
        ctx->count --;
        res = RHINO_SUCCESS;
    } else {
        res = -RHINO_UDS_NOT_EXIST;
    }

    krhino_mutex_unlock(&ctx->mtx);

    return res;
}

int uds_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    kstat_t res;
    struct uds_ctx_t *ctx;
    struct uds_socket_t *uds_sock;
    struct sockaddr_un *uds_addr = (struct sockaddr_un *)addr;

    UDS_FD_SANITY_CHECK(sockfd, -RHINO_UDS_NOT_EXIST);

    if (addrlen != sizeof(*uds_addr) || uds_addr->sun_family != AF_UNIX) {
        return -RHINO_UDS_NOT_SUPPORT;
    }

    __atomic_load(&uds_ctx_ptr, &ctx, __ATOMIC_RELAXED);
    if (ctx == NULL) {
        return -RHINO_UDS_NOT_INIT;
    }

    res = krhino_mutex_lock(&ctx->mtx, RHINO_WAIT_FOREVER);
    if (res != RHINO_SUCCESS && res != RHINO_MUTEX_OWNER_NESTED) {
        return -res;
    }

    sockfd -= UDS_SOCKET_FD_OFFSET;
    uds_sock = &ctx->sockets[sockfd];
    if (uds_sock->ipc_handle == UDS_SOCKET_HANDLE_FREE) {
        krhino_mutex_unlock(&ctx->mtx);
        return -RHINO_UDS_NOT_EXIST;
    }

    if (uds_sock->desc == NULL) {
        uds_sock->desc = (struct uds_desc_t *)krhino_mm_alloc(
                                     sizeof(struct uds_desc_t));
        if (uds_sock->desc == NULL) {
            krhino_mutex_unlock(&ctx->mtx);
            return -RHINO_NO_MEM;
        }
        memset(uds_sock->desc, 0, sizeof(struct uds_desc_t));
        uds_sock->desc->limited = UDS_SOCKET_CLIENT_LIMITED;
    }
    aos_ipc_memcpy(uds_sock->desc->name, uds_addr->sun_path, UDS_SOCKET_NAME_MAX_LEN);

    krhino_mutex_unlock(&ctx->mtx);

    return RHINO_SUCCESS;
}

int uds_listen(int sockfd, int backlog)
{
    kstat_t res;
    struct uds_ctx_t *ctx;
    struct uds_socket_t *uds_sock;

    UDS_FD_SANITY_CHECK(sockfd, -RHINO_UDS_NOT_EXIST);

    if (backlog > UDS_SOCKET_CLIENT_LIMITED) {
        return -RHINO_UDS_NOT_SUPPORT;
    }

    __atomic_load(&uds_ctx_ptr, &ctx, __ATOMIC_RELAXED);
    if (ctx == NULL) {
        return -RHINO_UDS_NOT_INIT;
    }

    res = krhino_mutex_lock(&ctx->mtx, RHINO_WAIT_FOREVER);
    if (res != RHINO_SUCCESS && res != RHINO_MUTEX_OWNER_NESTED) {
        return -res;
    }

    sockfd -= UDS_SOCKET_FD_OFFSET;
    uds_sock = &ctx->sockets[sockfd];
    if (uds_sock->ipc_handle == UDS_SOCKET_HANDLE_FREE) {
        krhino_mutex_unlock(&ctx->mtx);
        return -RHINO_UDS_NOT_EXIST;
    }

    if (uds_sock->desc == NULL) {
        uds_sock->desc = (struct uds_desc_t *)krhino_mm_alloc(
                                     sizeof(struct uds_desc_t));
        if (uds_sock->desc == NULL) {
            krhino_mutex_unlock(&ctx->mtx);
            return -RHINO_NO_MEM;
        }
        memset(uds_sock->desc, 0, sizeof(struct uds_desc_t));
    }

    uds_sock->desc->limited = backlog;

    krhino_mutex_unlock(&ctx->mtx);

    return RHINO_SUCCESS;
}

int uds_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int ipc;
    int sock;
    uint16_t peer;
    size_t sock_len = sizeof(peer);

    kstat_t res;
    int ipc_handle;
    struct uds_ctx_t *ctx;
    struct uds_server_sock_t *server;
    struct uds_socket_t *csock, *ssock;

    struct sockaddr_un *uds_addr = (struct sockaddr_un *)addr;

    UDS_FD_SANITY_CHECK(sockfd, -RHINO_UDS_NOT_EXIST);

    __atomic_load(&uds_ctx_ptr, &ctx, __ATOMIC_RELAXED);
    if (ctx == NULL) {
        return -RHINO_UDS_NOT_INIT;
    }

    res = krhino_mutex_lock(&ctx->mtx, RHINO_WAIT_FOREVER);
    if (res != RHINO_SUCCESS && res != RHINO_MUTEX_OWNER_NESTED) {
        return -res;
    }

    sockfd -= UDS_SOCKET_FD_OFFSET;
    ssock = &ctx->sockets[sockfd];
    if (ssock->ipc_handle == UDS_SOCKET_HANDLE_FREE) {
        krhino_mutex_unlock(&ctx->mtx);
        return -RHINO_UDS_NOT_EXIST;
    }

    if (ssock->desc == NULL || aos_ipc_strlen(ssock->desc->name) == 0) {
        krhino_mutex_unlock(&ctx->mtx);
        return -RHINO_UDS_OP_ILLEGAL;
    }

    server = __uds_server_search(sockfd, NULL);
    if (server == NULL) {
        server = (struct uds_server_sock_t *)krhino_mm_alloc(sizeof(*server));
        if (server == NULL) {
            krhino_mutex_unlock(&ctx->mtx);
            return -RHINO_NO_MEM;
        }
        server->ccount = 0;
        server->sock = sockfd;
        if (aos_ipc_create_fifo(NULL, UDS_SOCKET_PEER_SIZE,
                    UDS_SOCKET_CLIENT_LIMITED, &ipc) != RHINO_SUCCESS) {
            krhino_mm_free(server);
            krhino_mutex_unlock(&ctx->mtx);
            return -RHINO_NO_MEM;
        }
        aos_ipc_del(ssock->ipc_handle);
        ssock->ipc_handle = ipc;
        klist_init(&server->node);
        klist_add(&ctx->slst, &server->node);
        ssock->desc->as_server = 1;
    }

    printk("server->ccount:%d, limite:%d, ctx->count:%d, MAX:%d\r\n",
           server->ccount, ssock->desc->limited, ctx->count, UDS_SOCKET_MAX_COUNT);
    if (server->ccount >= ssock->desc->limited ||
        ctx->count >= UDS_SOCKET_MAX_COUNT) {
        krhino_mutex_unlock(&ctx->mtx);
        return -RHINO_UDS_FULL;
    }

    sock = __uds_socket_alloc();
    csock = &ctx->sockets[sock];

    res = aos_ipc_create_stream(NULL, UDS_SOCKET_BUF_MAX_LEN, &csock->ipc_handle);
    if (res != RHINO_SUCCESS) {
        krhino_mutex_unlock(&ctx->mtx);
        return -RHINO_NO_MEM;
    }
    ctx->count ++;

    do {
        if (res == RHINO_SUCCESS || res == RHINO_MUTEX_OWNER_NESTED) {
            krhino_mutex_unlock(&ctx->mtx);
        }

        res = aos_ipc_recv(ssock->ipc_handle, &peer, &sock_len, RHINO_WAIT_FOREVER);
        if (res != RHINO_SUCCESS) {
            aos_ipc_del(csock->ipc_handle);
            csock->ipc_handle = UDS_SOCKET_HANDLE_FREE;
            ctx->count --;
            return res;
        }

        res = krhino_mutex_lock(&ctx->mtx, RHINO_WAIT_FOREVER);

        if (ctx->sockets[peer].ipc_handle != UDS_SOCKET_HANDLE_FREE) {
            break;
        }
    } while (1);

    server->ccount ++;
    csock->psock = peer;
    csock->peer = ctx->sockets[peer].ipc_handle;
    ctx->sockets[peer].peer = csock->ipc_handle;
    ctx->sockets[peer].psock = sock;
    printk("%s, csock:%d, ipc_handle:%x, peer_handle:%x\r\n", __func__, sock, csock->ipc_handle, csock->peer);
    printk("%s, peer:%d, ipc_handle:%x, peer_handle:%x\r\n", __func__, peer, ctx->sockets[peer].ipc_handle,
           ctx->sockets[peer].peer);

    if (uds_addr) {
        if (addrlen) {
            *addrlen = sizeof(*uds_addr);
        }
        uds_addr->sun_family = AF_UNIX;
        memset(uds_addr->sun_path, 0, sizeof(uds_addr->sun_path));
        if (ctx->sockets[peer].desc) {
            aos_ipc_memcpy(uds_addr->sun_path, ctx->sockets[peer].desc->name,
                           UDS_SOCKET_NAME_MAX_LEN);
        }
    }

    peer = sock;
    ipc_handle = csock->peer;

    if (res == RHINO_SUCCESS || res == RHINO_MUTEX_OWNER_NESTED) {
        krhino_mutex_unlock(&ctx->mtx);
    }

    aos_ipc_sendto(ipc_handle, &peer, UDS_SOCKET_PEER_SIZE, RHINO_WAIT_FOREVER);

    return sock + UDS_SOCKET_FD_OFFSET;
}

int uds_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    kstat_t res;
    uint16_t sock;
    int ipc_handle;
    struct uds_ctx_t *ctx;
    struct uds_socket_t *uds_sock;
    struct uds_server_sock_t *server;
    struct sockaddr_un *uds_addr = (struct sockaddr_un *)addr;

    UDS_FD_SANITY_CHECK(sockfd, -RHINO_UDS_NOT_EXIST);

    if (addr == NULL || addrlen != sizeof(*uds_addr)) {
        return -RHINO_INV_PARAM;
    }

    __atomic_load(&uds_ctx_ptr, &ctx, __ATOMIC_RELAXED);
    if (ctx == NULL) {
        return -RHINO_UDS_NOT_INIT;
    }

    res = krhino_mutex_lock(&ctx->mtx, RHINO_WAIT_FOREVER);
    if (res != RHINO_SUCCESS && res != RHINO_MUTEX_OWNER_NESTED) {
        return -res;
    }

    sockfd -= UDS_SOCKET_FD_OFFSET;
    uds_sock = &ctx->sockets[sockfd];
    if (uds_sock->ipc_handle == UDS_SOCKET_HANDLE_FREE) {
        krhino_mutex_unlock(&ctx->mtx);
        return -RHINO_UDS_NOT_EXIST;
    }

    server = __uds_server_search(UDS_SOCKET_HANDLE_INVALID, (const char *)uds_addr->sun_path);
    if (server == NULL || server->ccount >= ctx->sockets[server->sock].desc->limited ||
        ctx->count >= UDS_SOCKET_MAX_COUNT) {
        krhino_mutex_unlock(&ctx->mtx);
        return server == NULL ? -RHINO_UDS_NOT_EXIST : -RHINO_UDS_FULL;
    }

    sock = sockfd;
    ipc_handle = ctx->sockets[server->sock].ipc_handle;
    krhino_mutex_unlock(&ctx->mtx);

    return aos_ipc_sendto(ipc_handle, &sock, UDS_SOCKET_PEER_SIZE, RHINO_WAIT_FOREVER);
}

ssize_t uds_send(int sockfd, const void *buf, size_t len, int flags)
{
    kstat_t res;
    uint16_t peer;
    size_t sock_len;
    struct uds_ctx_t *ctx;
    struct uds_socket_t *uds_sock;

    UDS_FD_SANITY_CHECK(sockfd, 0);

    __atomic_load(&uds_ctx_ptr, &ctx, __ATOMIC_RELAXED);
    if (uds_ctx_ptr == NULL) {
        return 0;
    }

    sockfd -= UDS_SOCKET_FD_OFFSET;
    uds_sock = &ctx->sockets[sockfd];
    if (uds_sock->ipc_handle == UDS_SOCKET_HANDLE_FREE) {
        return 0;
    }

    if (uds_sock->peer == UDS_SOCKET_HANDLE_FREE) {
        sock_len = sizeof(peer);
        res = aos_ipc_recv(uds_sock->ipc_handle, &peer, &sock_len,
                (flags & MSG_DONTWAIT) == 0 ? RHINO_WAIT_FOREVER : RHINO_NO_WAIT);
        if (res != RHINO_SUCCESS ||
            uds_sock->peer != ctx->sockets[peer].ipc_handle) {
            printk("error occures for connect\r\n");
            return 0;
        }
    }

    res = aos_ipc_sendto(uds_sock->peer, (void *)buf, len,
            (flags & MSG_DONTWAIT) == 0 ? RHINO_WAIT_FOREVER : RHINO_NO_WAIT);

    if (RHINO_SUCCESS != res) {
        return 0;
    }

    __uds_select_check(sockfd + UDS_SOCKET_FD_OFFSET, UDS_WR);

    return len;
}

ssize_t uds_sendto(int sockfd, const void *buf, size_t len, int flags,
                   const struct sockaddr *dest_addr, socklen_t addrlen)
{
    struct uds_ctx_t *ctx;
    struct uds_socket_t *uds_sock;

    UDS_FD_SANITY_CHECK(sockfd, 0);

    __atomic_load(&uds_ctx_ptr, &ctx, __ATOMIC_RELAXED);
    if (ctx == NULL) {
        return 0;
    }

    sockfd -= UDS_SOCKET_FD_OFFSET;
    uds_sock = &ctx->sockets[sockfd];
    if (uds_sock->ipc_handle == UDS_SOCKET_HANDLE_FREE) {
        return 0;
    }

    if (uds_sock->peer != UDS_SOCKET_HANDLE_FREE) {
        return uds_send(sockfd + UDS_SOCKET_FD_OFFSET, buf, len, flags);
    }

    return 0;
}

ssize_t uds_recv(int sockfd, void *buf, size_t len, int flags)
{
    kstat_t res;
    struct uds_ctx_t *ctx;
    struct uds_socket_t *uds_sock;

    UDS_FD_SANITY_CHECK(sockfd, 0);

    __atomic_load(&uds_ctx_ptr, &ctx, __ATOMIC_RELAXED);
    if (ctx == NULL) {
        return -RHINO_UDS_NOT_INIT;
    }

    sockfd -= UDS_SOCKET_FD_OFFSET;
    uds_sock = &ctx->sockets[sockfd];
    if (uds_sock->ipc_handle == UDS_SOCKET_HANDLE_FREE) {
        return 0;
    }

    res = aos_ipc_recv(uds_sock->ipc_handle, buf, &len,
            (flags & MSG_DONTWAIT) == 0 ? RHINO_WAIT_FOREVER : RHINO_NO_WAIT);
    if (RHINO_SUCCESS != res) {
        return 0;
    }

    __uds_select_check(sockfd + UDS_SOCKET_FD_OFFSET, UDS_RD);

    return len;
}

ssize_t uds_recvfrom(int sockfd, void *buf, size_t len, int flags,
                     struct sockaddr *src_addr, socklen_t *addrlen)
{
#if 0
    kstat_t res;
    struct uds_ctx_t *ctx;
    struct uds_socket_t *uds_sock;
    struct uds_server_sock_t *server;

    UDS_FD_SANITY_CHECK(sockfd, 0);

    __atomic_load(&uds_ctx_ptr, &ctx, __ATOMIC_RELAXED);
    if (ctx == NULL) {
        return -RHINO_UDS_NOT_INIT;
    }

    sockfd -= UDS_SOCKET_FD_OFFSET;
    uds_sock = &ctx->sockets[sockfd];
    if (uds_sock->ipc_handle == UDS_SOCKET_HANDLE_FREE) {
        return 0;
    }

    res = krhino_mutex_lock(&ctx->mtx, RHINO_WAIT_FOREVER);
    if (res != RHINO_SUCCESS && res != RHINO_MUTEX_OWNER_NESTED) {
        return -res;
    }

    krhino_mutex_unlock(&ctx->mtx);
#endif
    return -RHINO_UDS_NOT_SUPPORT;
}

int uds_select(int nfds, fd_set *readfds, fd_set *writefds,
               fd_set *exceptfds, struct timeval *timeout)
{
    int sock, idx;
    struct uds_ctx_t *ctx;

    sys_time_t time = 0;

    fd_set read, write;
    char avail = 0;
    kstat_t res;

    UDS_FD_SANITY_CHECK(nfds - 1, -RHINO_UDS_NOT_EXIST);

    if (readfds == NULL && writefds == NULL) {
        return -RHINO_INV_PARAM;
    }

    __atomic_load(&uds_ctx_ptr, &ctx, __ATOMIC_RELAXED);
    if (ctx == NULL) {
        return -RHINO_UDS_NOT_INIT;
    }

    FD_ZERO(&read);
    FD_ZERO(&write);

    if ((writefds == NULL && (readfds && memcmp(&read, readfds, sizeof(read)) == 0)) ||
        (readfds == NULL && (writefds && memcmp(&write, writefds, sizeof(write)) == 0))) {
        return RHINO_SUCCESS;
    }
    if (writefds && memcmp(writefds, &write, sizeof(write)) == 0 &&
        readfds && memcmp(readfds, &read, sizeof(read)) == 0) {
        return RHINO_SUCCESS;
    }

    if (timeout) {
        time += timeout->tv_sec * 1000;
        time += timeout->tv_usec / 1000;
    }

    res = krhino_mutex_lock(&ctx->mtx, krhino_ms_to_ticks(time));
    if (res != RHINO_SUCCESS && res != RHINO_MUTEX_OWNER_NESTED) {
        return -res;
    }

    for (sock = UDS_SOCKET_FD_OFFSET; sock < nfds; sock ++) {
        idx = sock - UDS_SOCKET_FD_OFFSET;
        if (readfds && FD_ISSET(sock, readfds) &&
            aos_ipc_handle_readable(ctx->sockets[idx].ipc_handle) == RHINO_SUCCESS) {
            FD_SET(sock, &read);
            avail = 1;
        }
        if (writefds && FD_ISSET(sock, writefds) &&
            aos_ipc_handle_writable(ctx->sockets[idx].peer) == RHINO_SUCCESS) {
            FD_SET(sock, &write);
            avail = 1;
        }
    }
    krhino_mutex_unlock(&ctx->mtx);

    if (avail == 0) {
        return __uds_select_add(nfds, readfds, writefds, exceptfds, timeout);
    }

    if (readfds) {
        aos_ipc_memcpy(readfds, &read, sizeof(read));
    }
    if (writefds) {
        aos_ipc_memcpy(writefds, &write, sizeof(write));
    }

    return avail == 1 ? RHINO_SUCCESS : -1;
}

#ifdef __cplusplus
}
#endif

#endif

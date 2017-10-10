/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <syscall_uapi.h>
#include <k_api.h>
#include <aos/aos.h>
#include <hal/hal.h>
#ifdef CONFIG_AOS_MESH
#include <umesh.h>
#endif

#ifdef MBEDTLS_IN_KERNEL
#include <ali_crypto.h>
#endif

#define SYSCALL(nr, func)

#include <syscall_tbl.h>

#ifdef WITH_LWIP
#include <aos/network.h>
#endif

/* --------------------Rhino-------------------- */

void aos_reboot(void)
{
    SYS_CALL0(SYS_REBOOT, void);
}

int aos_get_hz(void)
{
    return RHINO_CONFIG_TICKS_PER_SECOND;
}

const char *aos_version_get(void)
{
    return SYS_CALL0(SYS_VERSION_GET, const char *);
}

int aos_task_new(const char *name, void (*fn)(void *), void *arg,
                 int stack_size)
{
    return SYS_CALL4(SYS_TASK_NEW, int, const char *, name,
                     void (*)(void *), fn, void *, arg, int, stack_size);
}

int aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *), void *arg,
                     int stack_size, int prio)
{
    return SYS_CALL6(SYS_TASK_NEW_EXT, int, aos_task_t *, task, const char *, name,
                     void (*)(void *), fn, void *, arg, int, stack_size, int, prio);
}

void aos_task_exit(int code)
{
    SYS_CALL1(SYS_TASK_EXIT, void, int, code);
}

const char *aos_task_name(void)
{
    return SYS_CALL0(SYS_TASK_NAME, const char *);
}

int aos_task_key_create(aos_task_key_t *key)
{
    return SYS_CALL1(SYS_TASK_KEY_CREATE, int, aos_task_key_t *, key);
}

void aos_task_key_delete(aos_task_key_t key)
{
    SYS_CALL1(SYS_TASK_KEY_DELETE, void, aos_task_key_t, key);
}

int aos_task_setspecific(aos_task_key_t key, void *vp)
{
    return SYS_CALL2(SYS_TASK_SETSPECIFIC, int, aos_task_key_t, key, void *, vp);
}

void *aos_task_getspecific(aos_task_key_t key)
{
    return SYS_CALL1(SYS_TASK_GETSPECIFIC, void *, aos_task_key_t, key);
}

int aos_mutex_new(aos_mutex_t *mutex)
{
    return SYS_CALL1(SYS_MUTEX_NEW, int, aos_mutex_t *, mutex);
}

void aos_mutex_free(aos_mutex_t *mutex)
{
    SYS_CALL1(SYS_MUTEX_FREE, void, aos_mutex_t *, mutex);
}

int aos_mutex_lock(aos_mutex_t *mutex, unsigned int timeout)
{
    return SYS_CALL2(SYS_MUTEX_LOCK, int, aos_mutex_t *, mutex, unsigned int, timeout);
}

int aos_mutex_unlock(aos_mutex_t *mutex)
{
    return SYS_CALL1(SYS_MUTEX_UNLOCK, int, aos_mutex_t *, mutex);
}

int aos_sem_new(aos_sem_t *sem, int count)
{
    return SYS_CALL2(SYS_SEM_NEW, int, aos_sem_t *, sem, int, count);
}

void aos_sem_free(aos_sem_t *sem)
{
    SYS_CALL1(SYS_SEM_FREE, void, aos_sem_t *, sem);
}

int aos_sem_wait(aos_sem_t *sem, unsigned int timeout)
{
    return SYS_CALL2(SYS_SEM_WAIT, int, aos_sem_t *, sem, unsigned int, timeout);
}

void aos_sem_signal(aos_sem_t *sem)
{
    SYS_CALL1(SYS_SEM_SIGNAL, void, aos_sem_t *, sem);
}

int aos_queue_new(aos_queue_t *queue, void *buf, unsigned int size, int max_msg)
{
    return SYS_CALL4(SYS_QUEUE_NEW, int, aos_queue_t *, queue, void *, buf,
                     unsigned int, size, int, max_msg);
}

void aos_queue_free(aos_queue_t *queue)
{
    SYS_CALL1(SYS_QUEUE_FREE, void, aos_queue_t *, queue);
}

int aos_queue_send(aos_queue_t *queue, void *msg, unsigned int size)
{
    return SYS_CALL3(SYS_QUEUE_SEND, int, aos_queue_t *, queue, void *, msg,
                     unsigned int, size);
}

int aos_queue_recv(aos_queue_t *queue, unsigned int ms, void *msg,
                   unsigned int *size)
{
    return SYS_CALL4(SYS_QUEUE_RECV, int, aos_queue_t *, queue,
                     unsigned int, ms, void *, msg, unsigned int *, size);
}

int aos_timer_new(aos_timer_t *timer, void (*fn)(void *, void *), void *arg, int ms,
                  int repeat)
{
    return SYS_CALL5(SYS_TIMER_NEW, int, aos_timer_t *, timer,
                     void (*)(void *, void *), fn, void *, arg, int, ms, int, repeat);
}

void aos_timer_free(aos_timer_t *timer)
{
    SYS_CALL1(SYS_TIMER_FREE, void, aos_timer_t *, timer);
}

int aos_timer_start(aos_timer_t *timer)
{
    return SYS_CALL1(SYS_TIMER_START, int, aos_timer_t *, timer);
}

int aos_timer_stop(aos_timer_t *timer)
{
    return SYS_CALL1(SYS_TIMER_STOP, int, aos_timer_t *, timer);
}

int aos_timer_change(aos_timer_t *timer, int ms)
{
    return SYS_CALL2(SYS_TIMER_CHANGE, int, aos_timer_t *, timer, int, ms);
}

int aos_workqueue_create(aos_workqueue_t *workqueue, int pri, int stack_size)
{
    return SYS_CALL3(SYS_WORKQUEUE_CREATE, int, aos_workqueue_t *, workqueue,
                     int, pri, int, stack_size);
}

void aos_workqueue_del(aos_workqueue_t *workqueue)
{
    SYS_CALL1(SYS_WORKQUEUE_DEL, void, aos_workqueue_t *, workqueue);
}

int aos_work_init(aos_work_t *work, void (*fn)(void *), void *arg, int dly)
{
    return SYS_CALL4(SYS_WORK_INIT, int, aos_work_t *, work,
                     void (*)(void *), fn, void *, arg, int, dly);
}

void aos_work_destroy(aos_work_t *work)
{
    SYS_CALL1(SYS_WORK_DESTROY, int, aos_work_t *, work);
}

int aos_work_run(aos_workqueue_t *workqueue, aos_work_t *work)
{
    return SYS_CALL2(SYS_WORK_RUN, int, aos_workqueue_t *, workqueue,
                     aos_work_t *, work);
}

int aos_work_sched(aos_work_t *work)
{
    return SYS_CALL1(SYS_WORK_SCHED, int, aos_work_t *, work);
}

int aos_work_cancel(aos_work_t *work)
{
    return SYS_CALL1(SYS_WORK_CANCEL, int, aos_work_t *, work);
}

void *aos_malloc(unsigned int size)
{
#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
    if ((size & AOS_UNSIGNED_INT_MSB) == 0) {
        return SYS_CALL2(SYS_MALLOC, void *, unsigned int, size, size_t, (size_t)__builtin_return_address(0));
    } else {
        return SYS_CALL2(SYS_MALLOC, void *, unsigned int, size, size_t, 0);
    }
#else
    return SYS_CALL1(SYS_MALLOC, void *, unsigned int, size);
#endif
}

void *aos_realloc(void *mem, unsigned int size)
{
#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
    if ((size & AOS_UNSIGNED_INT_MSB) == 0) {
        return SYS_CALL3(SYS_REALLOC, void *, void *, mem, unsigned int, size,
                         size_t, (size_t)__builtin_return_address(0));
    } else {
        return SYS_CALL3(SYS_REALLOC, void *, void *, mem, unsigned int, size, size_t, 0);
    }
#else
    return SYS_CALL2(SYS_REALLOC, void *, void *, mem, unsigned int, size);
#endif
}

void *aos_zalloc(unsigned int size)
{
#if (RHINO_CONFIG_MM_DEBUG > 0u && RHINO_CONFIG_GCC_RETADDR > 0u)
    if ((size & AOS_UNSIGNED_INT_MSB) == 0) {
        return SYS_CALL2(SYS_ZALLOC, void *, unsigned int, size, size_t, (size_t)__builtin_return_address(0));
    } else {
        return SYS_CALL2(SYS_ZALLOC, void *, unsigned int, size, size_t, 0);
    }
#else
    return SYS_CALL1(SYS_ZALLOC, void *, unsigned int, size);
#endif
}

void aos_alloc_trace(void *addr, size_t allocator)
{
    return SYS_CALL2(SYS_ALLOC_TRACE, void, void *, addr, size_t, allocator);
}

void aos_free(void *mem)
{
    SYS_CALL1(SYS_FREE, void, void *, mem);
}

long long aos_now(void)
{
    return SYS_CALL0(SYS_NOW, long long);
}

long long aos_now_ms(void)
{
    return SYS_CALL0(SYS_NOW_MS, long long);
}

void aos_msleep(int ms)
{
    SYS_CALL1(SYS_MSLEEP, void, int, ms);
}

/* --------------------Framework-------------------- */

typedef void (*aos_event_cb)(input_event_t *event, void *private_data);
typedef void (*aos_call_t)(void *arg);
typedef void (*aos_poll_call_t)(int fd, void *arg);

int aos_register_event_filter(uint16_t type, aos_event_cb cb, void *priv)
{
    return SYS_CALL3(SYS_REGISTER_EVENT_FILTER, int, uint16_t, type,
                     aos_event_cb, cb, void *, priv);
}

int aos_unregister_event_filter(uint16_t type, aos_event_cb cb, void *priv)
{
    return SYS_CALL3(SYS_UNREGISTER_EVENT_FILTER, int, uint16_t, type,
                     aos_event_cb, cb, void *, priv);
}

int aos_post_event(uint16_t type, uint16_t code, unsigned long value)
{
    return SYS_CALL3(SYS_POST_EVENT, int, uint16_t, type, uint16_t, code,
                     unsigned long, value);
}

int aos_poll_read_fd(int fd, aos_poll_call_t action, void *param)
{
    return SYS_CALL3(SYS_POLL_READ_FD, int, int, fd, aos_poll_call_t, action,
                     void *, param);
}

void aos_cancel_poll_read_fd(int fd, aos_poll_call_t action, void *param)
{
    return SYS_CALL3(SYS_CANCEL_POLL_READ_FD, void, int, fd,
                     aos_poll_call_t, action, void *, param);
}

int aos_post_delayed_action(int ms, aos_call_t action, void *arg)
{
    return SYS_CALL3(SYS_POST_DELAYED_ACTION, int, int, ms, aos_call_t, action,
                     void *, arg);
}

void aos_cancel_delayed_action(int ms, aos_call_t action, void *arg)
{
    return SYS_CALL3(SYS_CANCEL_DELAYED_ACTION, void, int, ms,
                     aos_call_t, action, void *, arg);
}

int aos_schedule_call(aos_call_t action, void *arg)
{
    return SYS_CALL2(SYS_SCHEDULE_CALL, int, aos_call_t, action, void *, arg);
}

typedef void *aos_loop_t;

aos_loop_t aos_loop_init(void)
{
    return SYS_CALL0(SYS_LOOP_INIT, aos_loop_t);
}

aos_loop_t aos_current_loop(void)
{
    return SYS_CALL0(SYS_CURRENT_LOOP, aos_loop_t);
}

void aos_loop_run(void)
{
    return SYS_CALL0(SYS_LOOP_RUN, void);
}

void aos_loop_exit(void)
{
    return SYS_CALL0(SYS_LOOP_EXIT, void);
}

void aos_loop_destroy(void)
{
    return SYS_CALL0(SYS_LOOP_DESTROY, void);
}

int aos_loop_schedule_call(aos_loop_t *loop, aos_call_t action, void *arg)
{
    return SYS_CALL3(SYS_LOOP_SCHEDULE_CALL, int, aos_loop_t *, loop,
                     aos_call_t, action, void *, arg);
}

void *aos_loop_schedule_work(int ms, aos_call_t action, void *arg1,
                             aos_call_t fini_cb, void *arg2)
{
    return SYS_CALL5(SYS_LOOP_SCHEDULE_WORK, void *, int, ms,
                     aos_call_t, action, void *, arg1, aos_call_t, fini_cb,
                     void *, arg2);
}

void aos_cancel_work(void *work, aos_call_t action, void *arg1)
{
    return SYS_CALL3(SYS_CANCEL_WORK, void, void *, work, aos_call_t, action,
                     void *, arg1);
}

int aos_kv_set(const char *key, const void *value, int len, int sync)
{
    return SYS_CALL4(SYS_KV_SET, int, const char *, key, const void *, value,
                     int, len, int, sync);
}

int aos_kv_get(const char *key, void *buffer, int *buffer_len)
{
    return SYS_CALL3(SYS_KV_GET, int, const char *, key, void *, buffer,
                     int *, buffer_len);
}

int aos_kv_del(const char *key)
{
    return SYS_CALL1(SYS_KV_DEL, int, const char *, key);
}

int aos_open(const char *path, int flags)
{
    return SYS_CALL2(SYS_OPEN, int, const char *, path, int, flags);
}

int aos_close(int fd)
{
    return SYS_CALL1(SYS_CLOSE, int, int, fd);
}

ssize_t aos_read(int fd, void *buf, size_t nbytes)
{
    return SYS_CALL3(SYS_READ, ssize_t, int, fd, void *, buf, size_t, nbytes);
}

ssize_t aos_write(int fd, const void *buf, size_t nbytes)
{
    return SYS_CALL3(SYS_WRITE, ssize_t, int, fd, const void *, buf, size_t, nbytes);
}

int aos_ioctl(int fd, int cmd, unsigned long arg)
{
    return SYS_CALL3(SYS_IOCTL, int, int, fd, int, cmd, unsigned long, arg);
}

int aos_poll(struct pollfd *fds, int nfds, int timeout)
{
    return SYS_CALL3(SYS_POLL, int, struct pollfd *, fds, int, nfds, int, timeout);
}

int aos_fcntl(int fd, int cmd, int val)
{
    return SYS_CALL3(SYS_FCNTL, int, int, fd, int, cmd, int, val);
}

off_t aos_lseek(int fd, off_t offset, int whence)
{
    return SYS_CALL3(SYS_LSEEK, off_t, int, fd, off_t, offset, int, whence);
}

int aos_sync(int fd)
{
    return SYS_CALL1(SYS_SYNC, int, int, fd);
}

int aos_stat(const char *path, struct stat *st)
{
    return SYS_CALL2(SYS_STAT, int, const char *, path, struct stat *, st);
}

int aos_unlink(const char *path)
{
    return SYS_CALL1(SYS_UNLINK, int, const char *, path);
}

int aos_rename(const char *oldpath, const char *newpath)
{
    return SYS_CALL2(SYS_RENAME, int, const char *, oldpath, const char *, newpath);
}

aos_dir_t *aos_opendir(const char *path)
{
    return SYS_CALL1(SYS_OPENDIR, aos_dir_t *, const char *, path);
}

int aos_closedir(aos_dir_t *dir)
{
    return SYS_CALL1(SYS_CLOSEDIR, int, aos_dir_t *, dir);
}

aos_dirent_t *aos_readdir(aos_dir_t *dir)
{
    return SYS_CALL1(SYS_READDIR, aos_dirent_t *, aos_dir_t *, dir);
}

int aos_mkdir(const char *path)
{
    return SYS_CALL1(SYS_MKDIR, int, const char *, path);
}

/* --------------------MBEDTLS----------------- */

#ifdef MBEDTLS_IN_KERNEL

void *mbedtls_ssl_connect(void *tcp_fd, const char *ca_cert, int ca_cert_len)
{
    return SYS_CALL3(SYS_MBEDTLS_CONNECT, void *, void *, tcp_fd, const char *, ca_cert,
                     int, ca_cert_len);
}

int mbedtls_ssl_send(void *ssl, const char *buffer, int length)
{
    return SYS_CALL3(SYS_MBEDTLS_SEND, int, void *, ssl, const char *, buffer,
                     int, length);
}

int mbedtls_ssl_recv(void *ssl, char *buffer, int length)
{
    return SYS_CALL3(SYS_MBEDTLS_RECV, int, void *, ssl, char *, buffer,
                     int, length);
}

int mbedtls_ssl_close(void *ssl)
{
    return SYS_CALL1(SYS_MBEDTLS_CLOSE, int, void *, ssl);
}

ali_crypto_result ali_aes_init(aes_type_t type, bool is_enc,
                               const uint8_t *key1, const uint8_t *key2,
                               size_t keybytes, const uint8_t *iv, void *context)
{
    return SYS_CALL7(SYS_CRYPTO_AES_INIT, ali_crypto_result, aes_type_t, type,
                     bool, is_enc, const uint8_t *, key1, const uint8_t *, key2,
                     size_t, keybytes, const uint8_t *, iv, void *, context);
}

ali_crypto_result ali_aes_finish(const uint8_t *src, size_t src_size,
                                 uint8_t *dst, size_t *dst_size,
                                 sym_padding_t padding, void *context)
{
    return SYS_CALL6(SYS_CRYPTO_AES_FINISH, ali_crypto_result, const uint8_t *, src,
                     size_t, src_size, uint8_t *, dst, size_t *, dst_size, sym_padding_t,
                     padding, void *, context);
}

ali_crypto_result ali_aes_get_ctx_size(aes_type_t type, size_t *size)
{
    return SYS_CALL2(SYS_CRYPTO_AES_GET_CTX_SIZE, ali_crypto_result, aes_type_t, type, size_t *, size);
}

#endif /* MBEDTLS_IN_KERNEL */

/* --------------------LWIP-------------------- */

#ifdef WITH_LWIP

int lwip_accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
    return SYS_CALL3(SYS_LWIP_ACCEPT, int, int, s, struct sockaddr *, addr,
                     socklen_t *, addrlen);
}

int lwip_bind(int s, const struct sockaddr *name, socklen_t namelen)
{
    return SYS_CALL3(SYS_LWIP_BIND, int, int, s, const struct sockaddr *, name,
                     socklen_t, namelen);
}

int lwip_shutdown(int s, int how)
{
    return SYS_CALL2(SYS_LWIP_SHUTDOWN, int, int, s, int, how);
}

int lwip_getpeername (int s, struct sockaddr *name, socklen_t *namelen)
{
    return SYS_CALL3(SYS_LWIP_GETPEERNAME, int, int, s, struct sockaddr *, name,
                     socklen_t *, namelen);
}

int lwip_getsockname (int s, struct sockaddr *name, socklen_t *namelen)
{
    return SYS_CALL3(SYS_LWIP_GETSOCKNAME, int, int, s, struct sockaddr *, name,
                     socklen_t *, namelen);
}

int lwip_getsockopt (int s, int level, int optname, void *optval, socklen_t *optlen)
{
    return SYS_CALL5(SYS_LWIP_GETSOCKOPT, int, int, s, int, level, int, optname,
                     void *, optval, socklen_t *, optlen);
}

int lwip_setsockopt (int s, int level, int optname, const void *optval, socklen_t optlen)
{
    return SYS_CALL5(SYS_LWIP_SETSOCKOPT, int, int, s, int, level, int, optname,
                     const void *, optval, socklen_t, optlen);
}

int lwip_close(int s)
{
    return SYS_CALL1(SYS_LWIP_CLOSE, int, int, s);
}

int lwip_connect(int s, const struct sockaddr *name, socklen_t namelen)
{
    return SYS_CALL3(SYS_LWIP_CONNECT, int, int, s,
                     const struct sockaddr *, name, socklen_t, namelen);
}

int lwip_listen(int s, int backlog)
{
    return SYS_CALL2(SYS_LWIP_LISTEN, int, int, s, int, backlog);
}

int lwip_recv(int s, void *mem, size_t len, int flags)
{
    return SYS_CALL4(SYS_LWIP_RECV, int, int, s, void *, mem, size_t, len, int, flags);
}

int lwip_read(int s, void *mem, size_t len)
{
    return SYS_CALL3(SYS_LWIP_READ, int, int, s, void *, mem, size_t, len);
}

int lwip_recvfrom(int s, void *mem, size_t len, int flags,
                  struct sockaddr *from, socklen_t *fromlen)
{
    return SYS_CALL6(SYS_LWIP_RECVFROM, int, int, s, void *, mem, size_t, len,
                     int, flags, struct sockaddr *, from, socklen_t *, fromlen);
}

int lwip_send(int s, const void *dataptr, size_t size, int flags)
{
    return SYS_CALL4(SYS_LWIP_SEND, int, int, s, const void *, dataptr,
                     size_t, size, int, flags);
}

int lwip_sendmsg(int s, const struct msghdr *message, int flags)
{
    return SYS_CALL3(SYS_LWIP_SENDMSG, int, int, s,
                     const struct msghdr *, message, int, flags);
}

int lwip_sendto(int s, const void *dataptr, size_t size, int flags,
                const struct sockaddr *to, socklen_t tolen)
{
    return SYS_CALL6(SYS_LWIP_SENDTO, int, int, s, const void *, dataptr,
                     size_t, size, int, flags, const struct sockaddr *, to,
                     socklen_t, tolen);
}

int lwip_socket(int domain, int type, int protocol)
{
    return SYS_CALL3(SYS_LWIP_SOCKET, int, int, domain, int, type, int, protocol);
}

int lwip_write(int s, const void *dataptr, size_t size)
{
    return SYS_CALL3(SYS_LWIP_WRITE, int, int, s, const void *, dataptr, size_t, size);
}

int lwip_writev(int s, const struct iovec *iov, int iovcnt)
{
    return SYS_CALL3(SYS_LWIP_WRITEV, int, int, s, const struct iovec *, iov, int, iovcnt);
}

int lwip_select(int maxfdp1, fd_set *readset, fd_set *writeset,
                fd_set *exceptset, struct timeval *timeout)
{
    return SYS_CALL5(SYS_LWIP_SELECT, int, int, maxfdp1, fd_set *, readset,
                     fd_set *, writeset, fd_set *, exceptset, struct timeval *, timeout);
}

int lwip_ioctl(int s, long cmd, void *argp)
{
    return SYS_CALL3(SYS_LWIP_IOCTL, int, int, s, long, cmd, void *, argp);
}

int lwip_fcntl(int s, int cmd, int val)
{
    return SYS_CALL3(SYS_LWIP_FCNTL, int, int, s, int, cmd, int, val);
}

int lwip_eventfd(unsigned int initval, int flags)
{
    return SYS_CALL2(SYS_LWIP_EVENTFD, int, unsigned int, initval, int, flags);
}

u32_t lwip_htonl(u32_t x)
{
    return SYS_CALL1(SYS_LWIP_HTONL, u32_t, u32_t, x);
}

u16_t lwip_htons(u16_t x)
{
    return SYS_CALL1(SYS_LWIP_HTONS, u16_t, u16_t, x);
}

struct hostent *lwip_gethostbyname(const char *name)
{
    return SYS_CALL1(SYS_LWIP_GETHOSTBYNAME, struct hostent *, const char *, name);
}

int ip4addr_aton(const char *cp, ip4_addr_t *addr)
{
    return SYS_CALL2(SYS_LWIP_IP4ADDR_ATON, int, const char *, cp, ip4_addr_t *, addr);
}

char *ip4addr_ntoa(const ip4_addr_t *addr)
{
    return SYS_CALL1(SYS_LWIP_IP4ADDR_NTOA, char *, const ip4_addr_t *, addr);
}

u32_t ipaddr_addr(const char *cp)
{
    return SYS_CALL1(SYS_LWIP_IPADDR_ADDR, u32_t, const char *, cp);
}

void lwip_freeaddrinfo(struct addrinfo *ai)
{
    return SYS_CALL1(SYS_LWIP_FREEADDRINFO, void, struct addrinfo *, ai);
}

int lwip_getaddrinfo(const char *nodename, const char *servname,
                     const struct addrinfo *hints, struct addrinfo **res)
{
    return SYS_CALL4(SYS_LWIP_GETADDRINFO, int, const char *, nodename,
                     const char *, servname, const struct addrinfo *, hints,
                     struct addrinfo **, res);
}

#endif /* WITH_LWIP */

/* --------------------WIFI-------------------- */

hal_wifi_module_t *hal_wifi_get_default_module(void)
{
    return SYS_CALL0(SYS_WIFI_GET_DEFAULT_MODULE, hal_wifi_module_t *);
}

void hal_wifi_register_module(hal_wifi_module_t *m)
{
    return SYS_CALL1(SYS_WIFI_REGISTER_MODULE, void, hal_wifi_module_t *, m);
}

int  hal_wifi_init(void)
{
    return SYS_CALL0(SYS_WIFI_INIT, int);
}

void hal_wifi_get_mac_addr(hal_wifi_module_t *m, uint8_t *mac)
{
    return SYS_CALL2(SYS_WIFI_GET_MAC_ADDR, void, hal_wifi_module_t *, m, uint8_t *, mac);
}

int  hal_wifi_start(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para)
{
    return SYS_CALL2(SYS_WIFI_START, int, hal_wifi_module_t *, m, hal_wifi_init_type_t *, init_para);
}

int  hal_wifi_start_adv(hal_wifi_module_t *m,
                        hal_wifi_init_type_adv_t *init_para_adv)
{
    return SYS_CALL2(SYS_WIFI_START_ADV, int, hal_wifi_module_t *, m,
                     hal_wifi_init_type_adv_t *, init_para_adv);
}

int  hal_wifi_get_ip_stat(hal_wifi_module_t *m,
                          hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type)
{
    return SYS_CALL3(SYS_WIFI_GET_IP_STAT, int, hal_wifi_module_t *, m,
                     hal_wifi_ip_stat_t *, out_net_para, hal_wifi_type_t, wifi_type);
}

int  hal_wifi_get_link_stat(hal_wifi_module_t *m,
                            hal_wifi_link_stat_t *out_stat)
{
    return SYS_CALL2(SYS_WIFI_GET_LINK_STAT, int, hal_wifi_module_t *, m,
                     hal_wifi_link_stat_t *, out_stat);
}

void hal_wifi_start_scan(hal_wifi_module_t *m)
{
    return SYS_CALL1(SYS_WIFI_START_SCAN, void, hal_wifi_module_t *, m);
}

void hal_wifi_start_scan_adv(hal_wifi_module_t *m)
{
    return SYS_CALL1(SYS_WIFI_START_SCAN_ADV, void, hal_wifi_module_t *, m);
}

int  hal_wifi_power_off(hal_wifi_module_t *m)
{
    return SYS_CALL1(SYS_WIFI_POWER_OFF, int, hal_wifi_module_t *, m);
}

int  hal_wifi_power_on(hal_wifi_module_t *m)
{
    return SYS_CALL1(SYS_WIFI_POWER_ON, int, hal_wifi_module_t *, m);
}

int  hal_wifi_suspend(hal_wifi_module_t *m)
{
    return SYS_CALL1(SYS_WIFI_SUSPEND, int, hal_wifi_module_t *, m);
}

int  hal_wifi_suspend_station(hal_wifi_module_t *m)
{
    return SYS_CALL1(SYS_WIFI_SUSPEND_STATION, int, hal_wifi_module_t *, m);
}

int  hal_wifi_suspend_soft_ap(hal_wifi_module_t *m)
{
    return SYS_CALL1(SYS_WIFI_SUSPEND_SOFT_AP, int, hal_wifi_module_t *, m);
}

int  hal_wifi_set_channel(hal_wifi_module_t *m, int ch)
{
    return SYS_CALL2(SYS_WIFI_SET_CHANNEL, int, hal_wifi_module_t *, m, int, ch);
}

void hal_wifi_start_wifi_monitor(hal_wifi_module_t *m)
{
    return SYS_CALL1(SYS_WIFI_START_WIFI_MONITOR, void, hal_wifi_module_t *, m);
}

void hal_wifi_stop_wifi_monitor(hal_wifi_module_t *m)
{
    return SYS_CALL1(SYS_WIFI_STOP_WIFI_MONITOR, void, hal_wifi_module_t *, m);
}

void hal_wifi_register_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    return SYS_CALL2(SYS_WIFI_REGISTER_MONITOR_CB, void, hal_wifi_module_t *, m,
                     monitor_data_cb_t, fn);
}

void hal_wifi_install_event(hal_wifi_module_t *m, const hal_wifi_event_cb_t *cb)
{
    return SYS_CALL2(SYS_WIFI_INSTALL_EVENT, void, hal_wifi_module_t *, m,
                     const hal_wifi_event_cb_t *, cb);
}

void hal_wlan_register_mgnt_monitor_cb(hal_wifi_module_t *m, monitor_data_cb_t fn)
{
    return SYS_CALL2(SYS_HAL_WLAN_REG_MGNT_MONITOR_CB, void, hal_wifi_module_t *, m,
                     monitor_data_cb_t, fn);
}

int hal_wlan_send_80211_raw_frame(hal_wifi_module_t *m, uint8_t *buf, int len)
{
    return SYS_CALL3(SYS_HAL_WLAN_SEND_80211_RAW_FRAME, int, hal_wifi_module_t *,
                     m, uint8_t *, buf, int, len);
}

#ifdef CONFIG_AOS_MESH
ur_error_t umesh_init(node_mode_t mode)
{
    return SYS_CALL1(SYS_UR_MESH_INIT, ur_error_t, node_mode_t, mode);
}

ur_error_t umesh_start(void)
{
    return SYS_CALL0(SYS_UR_MESH_START, ur_error_t);
}

ur_error_t umesh_stop(void)
{
    return SYS_CALL0(SYS_UR_MESH_STOP, ur_error_t);
}

ur_error_t umesh_set_mode(uint8_t mode)
{
    return SYS_CALL1(SYS_UR_MESH_SET_MODE, ur_error_t, uint8_t, mode);
}


uint8_t umesh_get_mode(void)
{
    return SYS_CALL0(SYS_UR_MESH_GET_MODE, uint8_t);
}

const ur_netif_ip6_address_t *umesh_get_mcast_addr(void)
{
    return SYS_CALL0(SYS_UR_MESH_GET_MCAST_ADDR, ur_netif_ip6_address_t *);
}

const ur_netif_ip6_address_t *umesh_get_ucast_addr(void)
{
    return SYS_CALL0(SYS_UR_MESH_GET_UCAST_ADDR, ur_netif_ip6_address_t *);
}

const mac_address_t *umesh_get_mac_address(media_type_t type)
{
    return SYS_CALL1(SYS_UR_MESH_NET_GET_MAC_ADDRESS, mac_address_t *, media_type_t, type);
}

uint8_t umesh_get_device_state(void)
{
    return SYS_CALL0(SYS_UR_MESH_GET_DEVICE_STATE, uint8_t);
}
#endif

/* --------------------OTA-------------------- */

void hal_ota_register_module(hal_ota_module_t *module)
{
    return SYS_CALL1(SYS_OTA_REGISTER_MODULE, void, hal_ota_module_t *, module);
}

hal_stat_t hal_ota_init(void *something)
{
    return SYS_CALL1(SYS_OTA_INIT, hal_stat_t, void *, something);
}

hal_stat_t hal_ota_write(hal_ota_module_t *m, volatile uint32_t *off_set,
                         uint8_t *in_buf , uint32_t in_buf_len)
{
    return SYS_CALL4(SYS_OTA_WRITE, hal_stat_t, hal_ota_module_t *, m,
                     volatile uint32_t *, off_set, uint8_t *, in_buf , uint32_t, in_buf_len);
}

hal_stat_t hal_ota_read(hal_ota_module_t *m, volatile uint32_t *off_set,
                        uint8_t *out_buf, uint32_t out_buf_len)
{
    return SYS_CALL4(SYS_OTA_READ, hal_stat_t, hal_ota_module_t *, m,
                     volatile uint32_t *, off_set, uint8_t *, out_buf, uint32_t, out_buf_len);
}

hal_stat_t hal_ota_set_boot(hal_ota_module_t *m, void *something)
{
    return SYS_CALL2(SYS_OTA_SET_BOOT, hal_stat_t, hal_ota_module_t *, m, void *, something);
}

hal_ota_module_t *hal_ota_get_default_module(void)
{
    return SYS_CALL0(SYS_OTA_GET_DEFAULT_MODULE, hal_ota_module_t *);
}

/* --------------------CLI-------------------- */

int aos_cli_register_command(const struct cli_command *cmd)
{
    return SYS_CALL1(SYS_CLI_REG_CMD, int, const struct cli_command *, cmd);
}

int aos_cli_unregister_command(const struct cli_command *cmd)
{
    return SYS_CALL1(SYS_CLI_UNREG_CMD, int, const struct cli_command *, cmd);
}

int aos_cli_register_commands(const struct cli_command *cmds, int num_cmds)
{
    return SYS_CALL2(SYS_CLI_REG_CMDS, int, const struct cli_command *, cmds, int, num_cmds);
}

int aos_cli_unregister_commands(const struct cli_command *cmds, int num_cmds)
{
    return SYS_CALL2(SYS_CLI_UNREG_CMDS, int, const struct cli_command *, cmds, int, num_cmds);
}

int aos_cli_init(void)
{
    return SYS_CALL0(SYS_CLI_INIT, int);
}

int aos_cli_stop(void)
{
    return SYS_CALL0(SYS_CLI_STOP, int);
}

/* --------------------OTHERS-------------------- */

int32_t aos_uart_send(void *data, uint32_t size, uint32_t timeout)
{
    return SYS_CALL3(SYS_UART_SEND, int32_t, void *, data, uint32_t, size, uint32_t, timeout);
}

int get_errno(void)
{
    return SYS_CALL0(SYS_GET_ERRNO, int);
}

void set_errno(int err)
{
    return SYS_CALL1(SYS_SET_ERRNO, void, int, err);
}



#ifdef SOCKETS_MT

#ifndef LWIP_SYNC_MT_SLEEP_MS
	#define LWIP_SYNC_MT_SLEEP_MS 10
#endif

#ifndef SOCK_MT_DEBUG_LEVEL
#define SOCK_MT_DEBUG_LEVEL 255
#endif

typedef int (*lwip_io_mt_fn)(int , int );

enum sock_mt_stat{
    SOCK_MT_STATE_NONE = 0,
    SOCK_MT_STATE_BIND,
    SOCK_MT_STATE_LISTEN,
    SOCK_MT_STATE_ACCEPT,
    SOCK_MT_STATE_CONNECT,
    SOCK_MT_STATE_SEND,
};

enum sock_mt_shutdown {
    SOCK_MT_SHUTDOWN_NONE = 0,
    SOCK_MT_SHUTDOWN_OK
};

enum sock_mt_module {
    SOCK_MT_STATE,
    SOCK_MT_RECV,
    SOCK_MT_IOCTL,
    SOCK_MT_SELECT,

    SOCK_MT_SHUTDOWN,

    SOCK_MT_CLOSE,

    SOCK_MT_MODULE_MAX,
};

enum sock_mt_lock {
    SOCK_MT_STATE_LOCK,
    SOCK_MT_RECV_LOCK,
    SOCK_MT_IOCTRL_LOCK,

    SOCK_MT_LOCK_MAX,
};

struct _sock_mt {

    enum sock_mt_shutdown shutdown;

    enum sock_mt_stat state;

    int sel;

    sys_mutex_t lock[SOCK_MT_LOCK_MAX];
};
typedef struct _sock_mt sock_mt_t;

#if (SOCK_MT_DEBUG_LEVEL < 16)
    #define SOCK_MT_DEBUG(level, ...)                                       \
        if (level >= SOCK_MT_DEBUG_LEVEL)                                   \
            os_printf(__VA_ARGS__);
#else
    #define SOCK_MT_DEBUG(level, ...)
#endif

#define SOCK_MT_LOCK(s, l)                                                  \
{                                                                           \
    SOCK_MT_DEBUG(1, "s %d l %d enter ", s, l);                             \
    sys_mutex_lock(&sockets_mt[s].lock[l]);                                 \
    SOCK_MT_DEBUG(1, "OK\n");                                               \
}

#define SOCK_MT_TRYLOCK(s, l, r)                                            \
{                                                                           \
    SOCK_MT_DEBUG(1, "s %d l %d try enter ", s, l);                         \
    r = sys_mutex_trylock(&sockets_mt[s].lock[l]);                          \
    SOCK_MT_DEBUG(1, "ret %d OK\n", r);                                     \
}

#define SOCK_MT_UNLOCK(s, l)                                                \
{                                                                           \
    SOCK_MT_DEBUG(1, "s %d l %d exit ", s, l);                              \
    sys_mutex_unlock(&sockets_mt[s].lock[l]);                               \
    SOCK_MT_DEBUG(1, "OK\n");                                               \
}


#define SOCK_MT_SET_STATE(s, stat)                                          \
{                                                                           \
    SYS_ARCH_DECL_PROTECT(lev);                                             \
    SYS_ARCH_PROTECT(lev);                                                  \
    sockets_mt[s].state = stat;                                             \
    SYS_ARCH_UNPROTECT(lev);                                                \
}

#define SOCK_MT_GET_STATE(s)                                                \
    sockets_mt[s].state

#define SOCK_MT_SET_SHUTDOWN(s, d)                                          \
{                                                                           \
    SYS_ARCH_DECL_PROTECT(lev);                                             \
    SYS_ARCH_PROTECT(lev);                                                  \
    sockets_mt[s].shutdown = d;                                             \
    SYS_ARCH_UNPROTECT(lev);                                                \
}

#define SOCK_MT_GET_SHUTDOWN(s)                                             \
    sockets_mt[s].shutdown

#define SOCK_MT_SET_WRITE_SEL(s)                                            \
{                                                                           \
    SYS_ARCH_DECL_PROTECT(lev);                                             \
    SYS_ARCH_PROTECT(lev);                                                  \
    sockets_mt[s].sel |= 0x1;                                               \
    SYS_ARCH_UNPROTECT(lev);                                                \
}

#define SOCK_MT_RESET_WRITE_SEL(s)                                          \
{                                                                           \
    SYS_ARCH_DECL_PROTECT(lev);                                             \
    SYS_ARCH_PROTECT(lev);                                                  \
    sockets_mt[s].sel &= ~0x1;                                              \
    SYS_ARCH_UNPROTECT(lev);                                                \
}

#define SOCK_MT_GET_WRITE_SEL(s)                                            \
    (sockets_mt[s].sel & 0x01)

#define SOCK_MT_SET_READ_SEL(s)                                             \
{                                                                           \
    SYS_ARCH_DECL_PROTECT(lev);                                             \
    SYS_ARCH_PROTECT(lev);                                                  \
    sockets_mt[s].sel |= 0x2;                                               \
    SYS_ARCH_UNPROTECT(lev);                                                \
}

#define SOCK_MT_RESET_READ_SEL(s)                                           \
{                                                                           \
    SYS_ARCH_DECL_PROTECT(lev);                                             \
    SYS_ARCH_PROTECT(lev);                                                  \
    sockets_mt[s].sel &= ~0x2;                                              \
    SYS_ARCH_UNPROTECT(lev);                                                \
}

#define SOCK_MT_GET_READ_SEL(s)                                             \
    (sockets_mt[s].sel & 0x02)

#define SOCK_MT_GET_SEL(s)                                                  \
    sockets_mt[s].sel

#define SOCK_MT_SET_LINGER(sock)                                            \
{                                                                           \
    SYS_ARCH_DECL_PROTECT(lev);                                             \
    SYS_ARCH_PROTECT(lev);                                                  \
    sock->conn->linger = 1;                                                 \
    SYS_ARCH_UNPROTECT(lev);                                                \
}

#define LWIP_ENTER_MT(s, m, p)                                              \
{                                                                           \
    int r;                                                                  \
    SOCK_MT_DEBUG(1, "enter s %d module %d args %d\n", s, m, p);            \
	r = lwip_enter_mt_table[m](s, p);                                       \
    SOCK_MT_DEBUG(1, "enter s %d ret %d\n", s, r);                          \
    if (r)                                                                  \
        return -1;                                                          \
}

#define LWIP_EXIT_MT(s, m, p)                                               \
{                                                                           \
    int r;                                                                  \
    SOCK_MT_DEBUG(1, "exit s %d module %d args %d\n", s, m, p);             \
	r = lwip_exit_mt_table[m](s, p);                                        \
    SOCK_MT_DEBUG(1, "exit s %d ret %d\n", s, r);                           \
    if (r)                                                                  \
        return -1;                                                          \
}


LOCAL sock_mt_t sockets_mt[NUM_SOCKETS];

LOCAL int lwip_enter_mt_state(int s, int arg)
{
    if(tryget_socket(s) == NULL ||
       SOCK_MT_GET_STATE(s) != SOCK_MT_STATE_NONE ||
       SOCK_MT_GET_WRITE_SEL(s))
        return -1;

    SOCK_MT_LOCK(s, SOCK_MT_STATE_LOCK);
    if (SOCK_MT_GET_SHUTDOWN(s) != SOCK_MT_SHUTDOWN_NONE) {
        goto failed;
    }
    SOCK_MT_SET_STATE(s, arg);
    
    return 0;

failed:
    SOCK_MT_UNLOCK(s, SOCK_MT_STATE_LOCK);
    return -1;
}

LOCAL int lwip_enter_mt_recv(int s, int arg)
{
    if(tryget_socket(s) == NULL ||
       SOCK_MT_GET_READ_SEL(s))
        return -1;

	SOCK_MT_LOCK(s, SOCK_MT_RECV_LOCK);
	if (SOCK_MT_GET_SHUTDOWN(s) != SOCK_MT_SHUTDOWN_NONE) {
		goto failed;
	}
    
	return 0;

failed:
	SOCK_MT_UNLOCK(s, SOCK_MT_RECV_LOCK);
	return -1;
}

LOCAL int lwip_enter_mt_shutdown(int s, int arg)
{
    if(tryget_socket(s) == NULL)
        return -1;

	SOCK_MT_SET_SHUTDOWN(s, SOCK_MT_SHUTDOWN_OK);
    
	return 0;
}

LOCAL int lwip_enter_mt_close(int s, int arg)
{
    if(tryget_socket(s) == NULL)
        return -1;

	SOCK_MT_SET_SHUTDOWN(s, SOCK_MT_SHUTDOWN_OK);

	return 0;
}


LOCAL int lwip_enter_mt_select(int s, int arg)
{
	int i;
	int *fdset = (int *)arg;
	fd_set *read_set = (fd_set *)fdset[0];
	fd_set *write_set = (fd_set *)fdset[1];

	if (s > NUM_SOCKETS || s < 0)
		return -1;

	for (i = 0; i < s; i++) {
        if(FD_ISSET(i, read_set) || FD_ISSET(i, write_set))
            if (tryget_socket(i) == NULL)
                goto failed1;
        
		if (FD_ISSET(i, read_set)) {
			SOCK_MT_SET_READ_SEL(i);
			SOCK_MT_LOCK(i, SOCK_MT_RECV_LOCK);
			if (SOCK_MT_GET_SHUTDOWN(i) != SOCK_MT_SHUTDOWN_NONE) {
				goto failed2;
			}
		}
		
		if (FD_ISSET(i, write_set)) {
			SOCK_MT_SET_WRITE_SEL(i);
			SOCK_MT_LOCK(i, SOCK_MT_STATE_LOCK);
			if (SOCK_MT_GET_SHUTDOWN(i) != SOCK_MT_SHUTDOWN_NONE) {
				goto failed3;
			}
		}	
	}

	return 0;

failed3:
	SOCK_MT_UNLOCK(i, SOCK_MT_STATE_LOCK);
	SOCK_MT_RESET_WRITE_SEL(i);
failed2:
	if (FD_ISSET(i, read_set)) {
		SOCK_MT_UNLOCK(i, SOCK_MT_RECV_LOCK);
		SOCK_MT_RESET_READ_SEL(i);
	}
failed1:
	for (i--; i >=0; i--) {
		if (FD_ISSET(i, read_set) ){
			SOCK_MT_UNLOCK(i, SOCK_MT_RECV_LOCK);
			SOCK_MT_RESET_READ_SEL(i);
		}

		if (FD_ISSET(i, write_set)) {
			SOCK_MT_UNLOCK(i, SOCK_MT_STATE_LOCK);
			SOCK_MT_RESET_WRITE_SEL(i);
		}
	}
	
	return -1;
}

LOCAL int lwip_enter_mt_ioctrl(int s, int arg)
{
    if(tryget_socket(s) == NULL)
        return -1;

	SOCK_MT_LOCK(s, SOCK_MT_IOCTRL_LOCK);
	if (SOCK_MT_GET_SHUTDOWN(s) != SOCK_MT_SHUTDOWN_NONE) {
		goto failed;
	}
    
	return 0;

failed:
	SOCK_MT_UNLOCK(s, SOCK_MT_IOCTRL_LOCK);
	return -1;
}

LOCAL int lwip_exit_mt_state(int s, int arg)
{
	SOCK_MT_SET_STATE(s, SOCK_MT_STATE_NONE);
	SOCK_MT_UNLOCK(s, SOCK_MT_STATE_LOCK);

	if (SOCK_MT_GET_SHUTDOWN(s) != SOCK_MT_SHUTDOWN_NONE) {
		return -1;
	}
	
	return 0;
}

LOCAL int lwip_exit_mt_recv(int s, int arg)
{
	SOCK_MT_UNLOCK(s, SOCK_MT_RECV_LOCK);

	if (SOCK_MT_GET_SHUTDOWN(s) != SOCK_MT_SHUTDOWN_NONE) {
		return -1;
	}
	
	return 0;
}

LOCAL int lwip_exit_mt_shutdown(int s, int arg)
{
	SOCK_MT_SET_SHUTDOWN(s, SOCK_MT_STATE_NONE);
	return 0;
} 

LOCAL int lwip_exit_mt_close(int s, int arg)
{
	return 0;
} 

LOCAL int lwip_exit_mt_select(int s, int arg)
{
	int i;
	int *fdset = (int *)arg;
	fd_set *read_set = (fd_set *)fdset[0];
	fd_set *write_set = (fd_set *)fdset[1];

	for (i = 0; i < s; i++) {
		if (FD_ISSET(i, read_set)) {
			SOCK_MT_UNLOCK(i, SOCK_MT_RECV_LOCK);
            SOCK_MT_RESET_READ_SEL(i);
		}

		if (FD_ISSET(i, write_set)) {
			SOCK_MT_UNLOCK(i, SOCK_MT_STATE_LOCK);
            SOCK_MT_RESET_WRITE_SEL(i);
		}
	}

	for (i = 0; i < s; i++) {
	    if ((FD_ISSET(i, read_set) || FD_ISSET(i, write_set)) \
	            && SOCK_MT_GET_SHUTDOWN(i) != SOCK_MT_SHUTDOWN_NONE) {
			return -1;
		}	
	}
	
	return 0;
}

LOCAL int lwip_exit_mt_ioctrl(int s, int arg)
{
	SOCK_MT_UNLOCK(s, SOCK_MT_IOCTRL_LOCK);

	if (SOCK_MT_GET_SHUTDOWN(s) != SOCK_MT_SHUTDOWN_NONE) {
		return -1;
	}
	
	return 0;
}

LOCAL const ICACHE_RODATA_ATTR STORE_ATTR lwip_io_mt_fn lwip_enter_mt_table[] = {
    lwip_enter_mt_state,
    lwip_enter_mt_recv,
    lwip_enter_mt_ioctrl,
    lwip_enter_mt_select,
    lwip_enter_mt_shutdown,
    lwip_enter_mt_close
};

LOCAL const ICACHE_RODATA_ATTR STORE_ATTR lwip_io_mt_fn lwip_exit_mt_table[] = {
    lwip_exit_mt_state,
    lwip_exit_mt_recv,
    lwip_exit_mt_ioctrl,
    lwip_exit_mt_select,
    lwip_exit_mt_shutdown,
    lwip_exit_mt_close
};

LOCAL void lwip_sync_state_mt(struct lwip_sock *sock , int state)
{
	SOCK_MT_DEBUG(1, "sync state %d\n", state);

	switch (state) {
		case SOCK_MT_STATE_ACCEPT :
            if (sys_mbox_valid(&sock->conn->acceptmbox))
			    sys_mbox_trypost(&sock->conn->acceptmbox, NULL);
			break;
		case SOCK_MT_STATE_SEND :
		{
			SYS_ARCH_DECL_PROTECT(lev);
			SYS_ARCH_PROTECT(lev);
			if (sock->conn->current_msg) {
				sock->conn->current_msg->err = ERR_OK;
	        	sock->conn->current_msg = NULL;
			}
	        sock->conn->state = NETCONN_NONE;
			SYS_ARCH_UNPROTECT(lev);
            if (sys_sem_valid(&(sock->conn->snd_op_completed)))
			    sys_sem_signal(&(sock->conn->snd_op_completed));
			break;
		}	
		case SOCK_MT_STATE_CONNECT :
            if (sys_sem_valid(&(sock->conn->op_completed)))
			    sys_sem_signal(&(sock->conn->op_completed));
			break;
		default :
			break;
	}
}

LOCAL void lwip_sync_recv_mt(struct lwip_sock *sock)
{
	SOCK_MT_DEBUG(1, "sync recv %d\n", sock->conn->socket);
    if (sys_mbox_valid(&sock->conn->recvmbox))
	    sys_mbox_trypost(&sock->conn->recvmbox, NULL);
}

LOCAL void lwip_sync_select_mt(struct lwip_sock *sock)
{
	SOCK_MT_DEBUG(1, "sync select %d\n", sock->conn->socket);
	event_callback(sock->conn, NETCONN_EVT_ERROR, 0);
}


LOCAL void lwip_sync_mt(int s)
{
	int module = 0;
    int ret;

	while (module < SOCK_MT_SELECT) {
		extern void sys_arch_msleep(int ms);
		int ret;
		struct lwip_sock *sock;

		SOCK_MT_TRYLOCK(s, module, ret);
		if (ret == ERR_OK) {
			SOCK_MT_UNLOCK(s, module);
			module++;
			continue;
		}

		sock = get_socket(s);
#if LWIP_SO_LINGER
		if (SOCK_MT_GET_STATE(s) == SOCK_MT_STATE_SEND) {
			SOCK_MT_SET_LINGER(sock);
		}
#endif
		if (!SOCK_MT_GET_SEL(s)) {
			switch (module) {
				case SOCK_MT_STATE:
					lwip_sync_state_mt(sock, SOCK_MT_GET_STATE(s));
					break;
				case SOCK_MT_RECV:
					lwip_sync_recv_mt(sock);
					break;
				default :
					break;
			}
		} else {
			lwip_sync_select_mt(sock);
		}

		sys_arch_msleep(LWIP_SYNC_MT_SLEEP_MS);
	}
}

int lwip_socket_mt(int domain, int type, int protocol)
{
    int s;
    int i;

    s = lwip_socket(domain, type, protocol);
    if (s < 0)
        return -1;

    for (i = 0; i < SOCK_MT_LOCK_MAX; i++) {
        if (sys_mutex_new(&sockets_mt[s].lock[i]) != ERR_OK)
            break;
    }
    if (i < SOCK_MT_LOCK_MAX) {
        for (i-- ; i >= 0; i--) {
            sys_mutex_free(&sockets_mt[s].lock[i]);
            sockets_mt[s].lock[i] = ESPOS_OBJ_NONE;
        }

        lwip_close(s);
        s = -1;
    }

    if (s >= 0)
        SOCK_MT_SET_SHUTDOWN(s, SOCK_MT_SHUTDOWN_NONE);

    return s;
}

int lwip_bind_mt(int s, const struct sockaddr *name, socklen_t namelen)
{
    int ret;

    LWIP_ENTER_MT(s, SOCK_MT_STATE, SOCK_MT_STATE_BIND);

    ret = lwip_bind(s, name, namelen);

    LWIP_EXIT_MT(s, SOCK_MT_STATE, SOCK_MT_STATE_BIND);


    return ret;
}

int lwip_connect_mt(int s, const struct sockaddr *name, socklen_t namelen)
{
    int ret;

    LWIP_ENTER_MT(s, SOCK_MT_STATE, SOCK_MT_STATE_CONNECT);

    ret = lwip_connect(s, name, namelen);

    LWIP_EXIT_MT(s, SOCK_MT_STATE, SOCK_MT_STATE_CONNECT);

    return ret;
}

int lwip_listen_mt(int s, int backlog)
{
    int ret;

    LWIP_ENTER_MT(s, SOCK_MT_STATE, SOCK_MT_STATE_LISTEN);

    ret = lwip_listen(s, backlog);

    LWIP_EXIT_MT(s, SOCK_MT_STATE, SOCK_MT_STATE_LISTEN);

    return ret;
}

int lwip_accept_mt(int s, struct sockaddr *addr, socklen_t *addrlen)
{
    int i;
    int ret;

    LWIP_ENTER_MT(s, SOCK_MT_STATE, SOCK_MT_STATE_ACCEPT)

    ret = lwip_accept(s, addr, addrlen);
	
    LWIP_EXIT_MT(s, SOCK_MT_STATE, SOCK_MT_STATE_ACCEPT);

    if (ret < 0)
        return -1;

    for (i = 0; i < SOCK_MT_LOCK_MAX; i++) {
        if (sys_mutex_new(&sockets_mt[ret].lock[i]) != ERR_OK)
            break;
    }
    if (i < SOCK_MT_LOCK_MAX) {
        for (i-- ; i >= 0; i--) {
            sys_mutex_free(&sockets_mt[ret].lock[i]);
            sockets_mt[ret].lock[i] = ESPOS_OBJ_NONE;
        }

        lwip_close(ret);
        ret = -1;
    }

    if (ret >= 0)
        SOCK_MT_SET_SHUTDOWN(ret, SOCK_MT_SHUTDOWN_NONE);

    return ret;
}

int lwip_getpeername_mt(int s, struct sockaddr *name, socklen_t *namelen)
{
    int ret;

    LWIP_ENTER_MT(s, SOCK_MT_IOCTL, 0);

    ret = lwip_getpeername(s, name, namelen);

    LWIP_EXIT_MT(s, SOCK_MT_IOCTL, 0);

    return ret;
}

int lwip_getsockname_mt(int s, struct sockaddr *name, socklen_t *namelen)
{
    int ret;

    LWIP_ENTER_MT(s, SOCK_MT_IOCTL, 0);

    ret = lwip_getsockname(s, name, namelen);

    LWIP_EXIT_MT(s, SOCK_MT_IOCTL, 0);

    return ret;
}

int lwip_setsockopt_mt(int s, int level, int optname, const void *optval, socklen_t optlen)
{
    int ret;

    LWIP_ENTER_MT(s, SOCK_MT_IOCTL, 0);

    ret = lwip_setsockopt(s, level, optname, optval, optlen);

    LWIP_EXIT_MT(s, SOCK_MT_IOCTL, 0);

    return ret;
}

int lwip_getsockopt_mt(int s, int level, int optname, void *optval, socklen_t *optlen)
{
    int ret;

	if (optname == SO_ERROR ) {
		int retval = 0;
		
		if (SOCK_MT_GET_SHUTDOWN(s) != SOCK_MT_SHUTDOWN_NONE)
			retval = ENOTCONN;

		if (retval) {
			*(int *)optval = retval;
			return 0;
		}
	} 

    LWIP_ENTER_MT(s, SOCK_MT_IOCTL, 0);

    ret = lwip_getsockopt(s, level, optname, optval, optlen);

    LWIP_EXIT_MT(s, SOCK_MT_IOCTL, 0);

    return ret;
}

int lwip_ioctl_mt(int s, long cmd, void *argp)
{
    int ret;

    LWIP_ENTER_MT(s, SOCK_MT_IOCTL, 0)

    ret = lwip_ioctl(s, cmd, argp);

    LWIP_EXIT_MT(s, SOCK_MT_IOCTL, 0);

    return ret;
}

int lwip_sendto_mt(int s, const void *data, size_t size, int flags,
            const struct sockaddr *to, socklen_t tolen)
{
    int ret;
	
    LWIP_ENTER_MT(s, SOCK_MT_STATE, SOCK_MT_STATE_SEND);

    ret = lwip_sendto(s, data, size, flags, to, tolen);

    LWIP_EXIT_MT(s, SOCK_MT_STATE, SOCK_MT_STATE_SEND);

    return ret;
}

int lwip_send_mt(int s, const void *data, size_t size, int flags)
{
    int ret;

    LWIP_ENTER_MT(s, SOCK_MT_STATE, SOCK_MT_STATE_SEND);

    ret = lwip_send(s, data, size, flags);

    LWIP_EXIT_MT(s, SOCK_MT_STATE, SOCK_MT_STATE_SEND);

    return ret;
}

int lwip_recvfrom_mt(int s, void *mem, size_t len, int flags,
            struct sockaddr *from, socklen_t *fromlen)
{
    int ret;

    LWIP_ENTER_MT(s, SOCK_MT_RECV, 0);

    ret = lwip_recvfrom(s, mem, len, flags, from, fromlen);

    LWIP_EXIT_MT(s, SOCK_MT_RECV, 0);

    return ret;
}

int lwip_recv_mt(int s, void *mem, size_t len, int flags)
{
    return lwip_recvfrom_mt(s, mem, len, flags, NULL, NULL);
}

int lwip_read_mt(int s, void *mem, size_t len)
{
    return lwip_recvfrom_mt(s, mem, len, 0, NULL, NULL);
}

int lwip_write_mt(int s, const void *data, size_t size)
{
    return lwip_send_mt(s, data, size, 0);
}

int lwip_fcntl_mt(int s, int cmd, int val)
{
	int ret;

    LWIP_ENTER_MT(s, SOCK_MT_IOCTL, 0);
    
    ret = lwip_fcntl(s, cmd, val);

    LWIP_EXIT_MT(s, SOCK_MT_IOCTL, 0);

    return ret;
}

int lwip_shutdown_mt(int s, int how)
{
    int ret;

    LWIP_ENTER_MT(s, SOCK_MT_SHUTDOWN, 0);

    lwip_sync_mt(s);

    ret = lwip_shutdown(s, how);

    LWIP_EXIT_MT(s, SOCK_MT_SHUTDOWN, 0);

    return ret;
}

int lwip_close_mt(int s)
{
    int ret;
    int i;

	lwip_shutdown_mt(s, SHUT_RDWR);

    LWIP_ENTER_MT(s, SOCK_MT_CLOSE, 0);	

    ret = lwip_close(s);

    LWIP_EXIT_MT(s, SOCK_MT_CLOSE, 0);

    for (i = 0 ; i < SOCK_MT_LOCK_MAX; i++) {
        sys_mutex_free(&sockets_mt[s].lock[i]);
        sockets_mt[s].lock[i] = ESPOS_OBJ_NONE;
    }    

    return ret;
}

int lwip_select_mt(int maxfdp1, fd_set *readset, fd_set *writeset,
        fd_set *exceptset, struct timeval *timeout)
{
    int ret;
	fd_set read_set, write_set;
	int pset[2] = {(int)&read_set, (int)&write_set};

	FD_ZERO(&read_set);
  	FD_ZERO(&write_set);

	if (readset)
		MEMCPY(&read_set, readset, sizeof(fd_set));

	if (writeset)
		MEMCPY(&write_set, writeset, sizeof(fd_set));

    LWIP_ENTER_MT(maxfdp1, SOCK_MT_SELECT, (int)pset);

    ret = lwip_select(maxfdp1, readset, writeset, exceptset, timeout);

    LWIP_EXIT_MT(maxfdp1, SOCK_MT_SELECT, (int)pset);

    return ret;
}

#ifdef SOCKETS_TCP_TRACE

int lwip_trace_tcp(int s, int cmd, void *arg)
{
    struct lwip_sock *sock;
    u32_t *pbuf = (u32_t *)arg;

    if (!(sock = tryget_socket(s)))
        return -1;

    ADD_TCP_SEND_BYTES_GET(sock->conn, &pbuf[0]);
    ADD_TCP_SEND_BYTES_OK_GET(sock->conn, &pbuf[2]);
    ADD_TCP_SEND_BYTES_NOMEM_GET(sock->conn, &pbuf[4]);
    
    ADD_TCP_RECV_BYTES_GET(sock->conn, &pbuf[6]);
    ADD_TCP_RECV_BYTES_ERR_GET(sock->conn, &pbuf[8]);

    return 0;
}
#endif

#endif

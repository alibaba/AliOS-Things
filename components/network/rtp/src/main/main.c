/**
 * @file main.c  Main polling routine
 *
 * Copyright (C) 2010 Creytiv.com
 */
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <sys/types.h>
#undef _STRICT_ANSI
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef WIN32
#include <winsock.h>
#endif
#ifdef HAVE_SIGNAL
#include <signal.h>
#endif
#ifdef HAVE_POLL
#include <poll.h>
#endif
#ifdef HAVE_EPOLL
#include <sys/epoll.h>
#endif
#ifdef HAVE_KQUEUE
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#undef LIST_INIT
#undef LIST_FOREACH
#endif
#include <re_types.h>
#include <re_fmt.h>
#include <re_mem.h>
#include <re_mbuf.h>
#include <re_list.h>
#include <re_tmr.h>
#include <re_main.h>
//#include "main.h"
#ifdef HAVE_PTHREAD
#define __USE_GNU 1
#include <stdlib.h>
#include <pthread.h>
#endif
#include <network/network.h>

#define DEBUG_MODULE "main"
#define DEBUG_LEVEL 5
#include <re_dbg.h>

/*
  epoll() has been tested successfully on the following kernels:

  - Linux 2.6.16.29-xen   (Debian 4.0 etch)
  - Linux 2.6.18-4-amd64  (Debian 4.0 etch)


  TODO clean this up

  - The polling method is selectable both in compile-time and run-time
  - The polling method can be changed in run time. this is cool!
  - Maximum number of fds can be set from application, but only once!
  - Look at howto optimise main loop
 */

#if !defined (RELEASE) && !defined (MAIN_DEBUG)
#define MAIN_DEBUG 1  /**< Enable main loop debugging */
#endif


/** Main loop values */
enum {
	MAX_BLOCKING = 100,    /**< Maximum time spent in handler in [ms] */
#if defined (FD_SETSIZE)
	DEFAULT_MAXFDS = FD_SETSIZE
#else
	DEFAULT_MAXFDS = 128
#endif
};


/** Polling loop data */
struct re {
	/** File descriptor handler set */
	struct {
		int flags;           /**< Polling flags (Read, Write, etc.) */
		fd_h *fh;            /**< Event handler                     */
		void *arg;           /**< Handler argument                  */
	} *fhs;
	int maxfds;                  /**< Maximum number of polling fds     */
	int nfds;                    /**< Number of active file descriptors */
	enum poll_method method;     /**< The current polling method        */
	bool update;                 /**< File descriptor set need updating */
	bool polling;                /**< Is polling flag                   */
	int sig;                     /**< Last caught signal                */
	struct list tmrl;            /**< List of timers                    */

#ifdef HAVE_POLL
	struct pollfd *fds;          /**< Event set for poll()              */
#endif

#ifdef HAVE_EPOLL
	struct epoll_event *events;  /**< Event set for epoll()             */
	int epfd;                    /**< epoll control file descriptor     */
#endif

#ifdef HAVE_KQUEUE
	struct kevent *evlist;
	int kqfd;
#endif

#ifdef HAVE_PTHREAD
	pthread_mutex_t mutex;       /**< Mutex for thread synchronization  */
	pthread_mutex_t *mutexp;     /**< Pointer to active mutex           */
#endif
};

static struct re global_re = {
	NULL,
	0,
	0,
	METHOD_NULL,
	false,
	false,
	0,
	LIST_INIT,
#ifdef HAVE_POLL
	NULL,
#endif
#ifdef HAVE_EPOLL
	NULL,
	-1,
#endif
#ifdef HAVE_KQUEUE
	NULL,
	-1,
#endif
#ifdef HAVE_PTHREAD
#if MAIN_DEBUG && defined (PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP)
	PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP,
#else
	PTHREAD_MUTEX_INITIALIZER,
#endif
	&global_re.mutex,
#endif
};


#ifdef HAVE_PTHREAD

static void poll_close(struct re *re);

static pthread_once_t pt_once = PTHREAD_ONCE_INIT;
static pthread_key_t  pt_key;


static void thread_destructor(void *arg)
{
	poll_close(arg);
	free(arg);
}


static void re_once(void)
{
	pthread_key_create(&pt_key, thread_destructor);
}


static struct re *re_get(void)
{
	struct re *re;

	pthread_once(&pt_once, re_once);

	re = pthread_getspecific(pt_key);
	if (!re) {
		re = &global_re;
	}

	return re;
}


static inline void re_lock(struct re *re)
{
	int err;

	err = pthread_mutex_lock(re->mutexp);
	if (err) {
		DEBUG_WARNING("re_lock: %m\n", err);
	}
}


static inline void re_unlock(struct re *re)
{
	int err;

	err = pthread_mutex_unlock(re->mutexp);
	if (err) {
		DEBUG_WARNING("re_unlock: %m\n", err);
	}
}


#else

static struct re *re_get(void)
{
	return &global_re;
}

#define re_lock(x)    /**< Stub */
#define re_unlock(x)  /**< Stub */

#endif


#if MAIN_DEBUG
/**
 * Call the application event handler
 *
 * @param re     Poll state
 * @param fd     File descriptor
 * @param flags  Event flags
 */
static void fd_handler(struct re *re, int fd, int flags)
{
	const uint64_t tick = tmr_jiffies();
	uint32_t diff;

	DEBUG_INFO("event on fd=%d (flags=0x%02x)...\n", fd, flags);

	re->fhs[fd].fh(flags, re->fhs[fd].arg);

	diff = (uint32_t)(tmr_jiffies() - tick);

	if (diff > MAX_BLOCKING) {
		DEBUG_WARNING("long async blocking: %u>%u ms (h=%p arg=%p)\n",
			      diff, MAX_BLOCKING,
			      re->fhs[fd].fh, re->fhs[fd].arg);
	}
}
#endif


#ifdef HAVE_POLL
static int set_poll_fds(struct re *re, int fd, int flags)
{
	if (!re->fds)
		return 0;

	if (flags)
		re->fds[fd].fd = fd;
	else
		re->fds[fd].fd = -1;

	re->fds[fd].events = 0;
	if (flags & FD_READ)
		re->fds[fd].events |= POLLIN;
	if (flags & FD_WRITE)
		re->fds[fd].events |= POLLOUT;
	if (flags & FD_EXCEPT)
		re->fds[fd].events |= POLLERR;

	return 0;
}
#endif


#ifdef HAVE_EPOLL
static int set_epoll_fds(struct re *re, int fd, int flags)
{
	struct epoll_event event;
	int err = 0;

	if (re->epfd < 0)
		return EBADFD;

	memset(&event, 0, sizeof(event));

	DEBUG_INFO("set_epoll_fds: fd=%d flags=0x%02x\n", fd, flags);

	if (flags) {
		event.data.fd = fd;

		if (flags & FD_READ)
			event.events |= EPOLLIN;
		if (flags & FD_WRITE)
			event.events |= EPOLLOUT;
		if (flags & FD_EXCEPT)
			event.events |= EPOLLERR;

		/* Try to add it first */
		if (-1 == epoll_ctl(re->epfd, EPOLL_CTL_ADD, fd, &event)) {

			/* If already exist then modify it */
			if (EEXIST == errno) {

				if (-1 == epoll_ctl(re->epfd, EPOLL_CTL_MOD,
						    fd, &event)) {
					err = errno;
					DEBUG_WARNING("epoll_ctl:"
						      " EPOLL_CTL_MOD:"
						      " fd=%d (%m)\n",
						      fd, err);
				}
			}
			else {
				err = errno;
				DEBUG_WARNING("epoll_ctl: EPOLL_CTL_ADD:"
					      " fd=%d (%m)\n",
					      fd, err);
			}
		}
	}
	else {
		if (-1 == epoll_ctl(re->epfd, EPOLL_CTL_DEL, fd, &event)) {
			err = errno;
			DEBUG_INFO("epoll_ctl: EPOLL_CTL_DEL: fd=%d (%m)\n",
				   fd, err);
		}
	}

	return err;
}
#endif


#ifdef HAVE_KQUEUE
static int set_kqueue_fds(struct re *re, int fd, int flags)
{
	struct kevent kev[2];
	int r, n = 0;

	memset(kev, 0, sizeof(kev));

	/* always delete the events */
	EV_SET(&kev[0], fd, EVFILT_READ,  EV_DELETE, 0, 0, 0);
	EV_SET(&kev[1], fd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
	kevent(re->kqfd, kev, 2, NULL, 0, NULL);

	memset(kev, 0, sizeof(kev));

	if (flags & FD_WRITE) {
		EV_SET(&kev[n], fd, EVFILT_WRITE, EV_ADD, 0, 0, 0);
		++n;
	}
	if (flags & FD_READ) {
		EV_SET(&kev[n], fd, EVFILT_READ, EV_ADD, 0, 0, 0);
		++n;
	}

	if (n) {
		r = kevent(re->kqfd, kev, n, NULL, 0, NULL);
		if (r < 0) {
			int err = errno;

			DEBUG_WARNING("set: [fd=%d, flags=%x] kevent: %m\n",
				      fd, flags, err);
			return err;
		}
	}

	return 0;
}
#endif


/**
 * Rebuild the file descriptor mapping table. This must be done whenever
 * the polling method is changed.
 */
static int rebuild_fds(struct re *re)
{
	int i, err = 0;

	DEBUG_INFO("rebuilding fds (nfds=%d)\n", re->nfds);

	/* Update fd sets */
	for (i=0; i<re->nfds; i++) {
		if (!re->fhs[i].fh)
			continue;

		switch (re->method) {

#ifdef HAVE_POLL
		case METHOD_POLL:
			err = set_poll_fds(re, i, re->fhs[i].flags);
			break;
#endif
#ifdef HAVE_EPOLL
		case METHOD_EPOLL:
			err = set_epoll_fds(re, i, re->fhs[i].flags);
			break;
#endif

#ifdef HAVE_KQUEUE
		case METHOD_KQUEUE:
			err = set_kqueue_fds(re, i, re->fhs[i].flags);
			break;
#endif

		default:
			break;
		}

		if (err)
			break;
	}

	return err;
}


static int poll_init(struct re *re)
{
	DEBUG_INFO("poll init (maxfds=%d)\n", re->maxfds);

	if (!re->maxfds) {
		DEBUG_WARNING("poll init: maxfds is 0\n");
		return EINVAL;
	}

	switch (re->method) {

#ifdef HAVE_POLL
	case METHOD_POLL:
		if (!re->fds) {
			re->fds = mem_zalloc(re->maxfds * sizeof(*re->fds),
					    NULL);
			if (!re->fds)
				return ENOMEM;
		}
		break;
#endif
#ifdef HAVE_EPOLL
	case METHOD_EPOLL:
		if (!re->events) {
			DEBUG_INFO("allocate %u bytes for epoll set\n",
				   re->maxfds * sizeof(*re->events));
			re->events = mem_zalloc(re->maxfds*sizeof(*re->events),
					      NULL);
			if (!re->events)
				return ENOMEM;
		}

		if (re->epfd < 0
		    && -1 == (re->epfd = epoll_create(re->maxfds))) {

			int err = errno;

			DEBUG_WARNING("epoll_create: %m (maxfds=%d)\n",
				      err, re->maxfds);
			return err;
		}
		DEBUG_INFO("init: epoll_create() epfd=%d\n", re->epfd);
		break;
#endif

#ifdef HAVE_KQUEUE
	case METHOD_KQUEUE:

		if (!re->evlist) {
			size_t sz = re->maxfds * sizeof(*re->evlist);
			re->evlist = mem_zalloc(sz, NULL);
			if (!re->evlist)
				return ENOMEM;
		}

		if (re->kqfd < 0) {
			re->kqfd = kqueue();
			if (re->kqfd < 0)
				return errno;
			DEBUG_INFO("kqueue: fd=%d\n", re->kqfd);
		}

		break;
#endif

	default:
		break;
	}
	return 0;
}


/** Free all resources */
static void poll_close(struct re *re)
{
	DEBUG_INFO("poll close\n");

	re->fhs = mem_deref(re->fhs);
	re->maxfds = 0;

#ifdef HAVE_POLL
	re->fds = mem_deref(re->fds);
#endif
#ifdef HAVE_EPOLL
	DEBUG_INFO("poll_close: epfd=%d\n", re->epfd);

	if (re->epfd >= 0) {
		(void)close(re->epfd);
		re->epfd = -1;
	}

	re->events = mem_deref(re->events);
#endif

#ifdef HAVE_KQUEUE
	if (re->kqfd >= 0) {
		close(re->kqfd);
		re->kqfd = -1;
	}

	re->evlist = mem_deref(re->evlist);
#endif
}


static int poll_setup(struct re *re)
{
	int err;

	err = fd_setsize(DEFAULT_MAXFDS);
	if (err)
		goto out;

	if (METHOD_NULL == re->method) {
		err = poll_method_set(poll_method_best());
		if (err)
			goto out;

		DEBUG_INFO("poll setup: poll method not set - set to `%s'\n",
			   poll_method_name(re->method));
	}

	err = poll_init(re);

 out:
	if (err)
		poll_close(re);

	return err;
}


/**
 * Listen for events on a file descriptor
 *
 * @param fd     File descriptor
 * @param flags  Wanted event flags
 * @param fh     Event handler
 * @param arg    Handler argument
 *
 * @return 0 if success, otherwise errorcode
 */
int fd_listen(int fd, int flags, fd_h *fh, void *arg)
{
	struct re *re = re_get();
	int err = 0;

	DEBUG_INFO("fd_listen: fd=%d flags=0x%02x\n", fd, flags);

	if (fd < 0) {
		DEBUG_WARNING("fd_listen: corrupt fd %d\n", fd);
		return EBADF;
	}

	if (flags || fh) {
		err = poll_setup(re);
		if (err)
			return err;
	}

	if (fd >= re->maxfds) {
		if (flags) {
			DEBUG_WARNING("fd_listen: fd=%d flags=0x%02x"
				      " - Max %d fds\n",
				      fd, flags, re->maxfds);
		}
		return EMFILE;
	}

	/* Update fh set */
	if (re->fhs) {
		re->fhs[fd].flags = flags;
		re->fhs[fd].fh    = fh;
		re->fhs[fd].arg   = arg;
	}

	re->nfds = max(re->nfds, fd+1);

	switch (re->method) {
#ifdef HAVE_POLL
	case METHOD_POLL:
		err = set_poll_fds(re, fd, flags);
		break;
#endif

#ifdef HAVE_EPOLL
	case METHOD_EPOLL:
		if (re->epfd < 0)
			return EBADFD;
		err = set_epoll_fds(re, fd, flags);
		break;
#endif

#ifdef HAVE_KQUEUE
	case METHOD_KQUEUE:
		err = set_kqueue_fds(re, fd, flags);
		break;
#endif

	default:
		break;
	}

	if (err) {
		if (flags && fh) {
			fd_close(fd);
			DEBUG_WARNING("fd_listen: fd=%d flags=0x%02x (%m)\n",
				      fd, flags, err);
		}
	}

	return err;
}


/**
 * Stop listening for events on a file descriptor
 *
 * @param fd     File descriptor
 */
void fd_close(int fd)
{
	(void)fd_listen(fd, 0, NULL, NULL);
}


/**
 * Polling loop
 *
 * @param re Poll state.
 *
 * @return 0 if success, otherwise errorcode
 */
static int fd_poll(struct re *re)
{
	const uint64_t to = tmr_next_timeout(&re->tmrl);
	int i, n;
#ifdef HAVE_SELECT
	fd_set rfds, wfds, efds;
#endif

	DEBUG_INFO("next timer: %llu ms\n", to);

	/* Wait for I/O */
	switch (re->method) {

#ifdef HAVE_POLL
	case METHOD_POLL:
		re_unlock(re);
		n = poll(re->fds, re->nfds, to ? (int)to : -1);
		re_lock(re);
		break;
#endif
#ifdef HAVE_SELECT
	case METHOD_SELECT: {
		struct timeval tv;

		/* Clear and update fd sets */
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_ZERO(&efds);

		for (i=0; i<re->nfds; i++) {
			if (!re->fhs[i].fh)
				continue;

			if (re->fhs[i].flags & FD_READ)
				FD_SET(i, &rfds);
			if (re->fhs[i].flags & FD_WRITE)
				FD_SET(i, &wfds);
			if (re->fhs[i].flags & FD_EXCEPT)
				FD_SET(i, &efds);
		}

#ifdef WIN32
		tv.tv_sec  = (long) to / 1000;
#else
		tv.tv_sec  = (time_t) to / 1000;
#endif
		tv.tv_usec = (uint32_t) (to % 1000) * 1000;
		re_unlock(re);
		n = select(re->nfds, &rfds, &wfds, &efds, to ? &tv : NULL);
		re_lock(re);
	}
		break;
#endif
#ifdef HAVE_EPOLL
	case METHOD_EPOLL:
		re_unlock(re);
		n = epoll_wait(re->epfd, re->events, re->maxfds,
			       to ? (int)to : -1);
		re_lock(re);
		break;
#endif

#ifdef HAVE_KQUEUE
	case METHOD_KQUEUE: {
		struct timespec timeout;

		timeout.tv_sec = (time_t) (to / 1000);
		timeout.tv_nsec = (to % 1000) * 1000000;

		re_unlock(re);
		n = kevent(re->kqfd, NULL, 0, re->evlist, re->maxfds,
			   to ? &timeout : NULL);
		re_lock(re);
		}
		break;
#endif

	default:
		(void)to;
		DEBUG_WARNING("no polling method set\n");
		return EINVAL;
	}

	if (n < 0)
		return errno;

	/* Check for events */
	for (i=0; (n > 0) && (i < re->nfds); i++) {
		int fd, flags = 0;

		switch (re->method) {

#ifdef HAVE_POLL
		case METHOD_POLL:
			fd = i;
			if (re->fds[fd].revents & POLLIN)
				flags |= FD_READ;
			if (re->fds[fd].revents & POLLOUT)
				flags |= FD_WRITE;
			if (re->fds[fd].revents & (POLLERR|POLLHUP|POLLNVAL))
				flags |= FD_EXCEPT;
			if (re->fds[fd].revents & POLLNVAL) {
				DEBUG_WARNING("event: fd=%d POLLNVAL"
					      " (fds.fd=%d,"
					      " fds.events=0x%02x)\n",
					      fd, re->fds[fd].fd,
					      re->fds[fd].events);
			}
			/* Clear events */
			re->fds[fd].revents = 0;
			break;
#endif
#ifdef HAVE_SELECT
		case METHOD_SELECT:
			fd = i;
			if (FD_ISSET(fd, &rfds))
				flags |= FD_READ;
			if (FD_ISSET(fd, &wfds))
				flags |= FD_WRITE;
			if (FD_ISSET(fd, &efds))
				flags |= FD_EXCEPT;
			break;
#endif
#ifdef HAVE_EPOLL
		case METHOD_EPOLL:
			fd = re->events[i].data.fd;

			if (re->events[i].events & EPOLLIN)
				flags |= FD_READ;
			if (re->events[i].events & EPOLLOUT)
				flags |= FD_WRITE;
			if (re->events[i].events & (EPOLLERR|EPOLLHUP))
				flags |= FD_EXCEPT;

			if (!flags) {
				DEBUG_WARNING("epoll: no flags fd=%d\n", fd);
			}

			break;
#endif

#ifdef HAVE_KQUEUE
		case METHOD_KQUEUE: {

			struct kevent *kev = &re->evlist[i];

			fd = (int)kev->ident;

			if (fd >= re->maxfds) {
				DEBUG_WARNING("large fd=%d\n", fd);
				break;
			}

			if (kev->filter == EVFILT_READ)
				flags |= FD_READ;
			else if (kev->filter == EVFILT_WRITE)
				flags |= FD_WRITE;
			else {
				DEBUG_WARNING("kqueue: unhandled "
					      "filter %x\n",
					      kev->filter);
			}

			if (kev->flags & EV_EOF) {
				flags |= FD_EXCEPT;
			}
			if (kev->flags & EV_ERROR) {
				DEBUG_WARNING("kqueue: EV_ERROR on fd %d\n",
					      fd);
			}

			if (!flags) {
				DEBUG_WARNING("kqueue: no flags fd=%d\n", fd);
			}
		}
			break;
#endif

		default:
			return EINVAL;
		}

		if (!flags)
			continue;

		if (re->fhs[fd].fh) {
#if MAIN_DEBUG
			fd_handler(re, fd, flags);
#else
			re->fhs[fd].fh(flags, re->fhs[fd].arg);
#endif
		}

		/* Check if polling method was changed */
		if (re->update) {
			re->update = false;
			return 0;
		}

		--n;
	}

	return 0;
}


/**
 * Set the maximum number of file descriptors
 *
 * @param maxfds Max FDs. 0 to free.
 *
 * @return 0 if success, otherwise errorcode
 */
int fd_setsize(int maxfds)
{
	struct re *re = re_get();

	if (!maxfds) {
		fd_debug();
		poll_close(re);
		return 0;
	}

	if (!re->maxfds)
		re->maxfds = maxfds;

	if (!re->fhs) {
		DEBUG_INFO("fd_setsize: maxfds=%d, allocating %u bytes\n",
			   re->maxfds, re->maxfds * sizeof(*re->fhs));

		re->fhs = mem_zalloc(re->maxfds * sizeof(*re->fhs), NULL);
		if (!re->fhs)
			return ENOMEM;
	}

	return 0;
}


/**
 * Print all file descriptors in-use
 */
void fd_debug(void)
{
	const struct re *re = re_get();
	int i;

	if (!re->fhs)
		return;

	for (i=0; i<re->nfds; i++) {

		if (!re->fhs[i].flags)
			continue;

		(void)re_fprintf(stderr,
				 "fd %d in use: flags=%x fh=%p arg=%p\n",
				 i, re->fhs[i].flags, re->fhs[i].fh,
				 re->fhs[i].arg);
	}
}


#ifdef HAVE_SIGNAL
/* Thread-safe signal handling */
static void signal_handler(int sig)
{
	(void)signal(sig, signal_handler);
	re_get()->sig = sig;
}
#endif


/**
 * Main polling loop for async I/O events. This function will only return when
 * re_cancel() is called or an error occured.
 *
 * @param signalh Optional Signal handler
 *
 * @return 0 if success, otherwise errorcode
 */
int re_main(re_signal_h *signalh)
{
	struct re *re = re_get();
	int err;

#ifdef HAVE_SIGNAL
	if (signalh) {
		(void)signal(SIGINT, signal_handler);
		(void)signal(SIGALRM, signal_handler);
		(void)signal(SIGTERM, signal_handler);
	}
#endif

	if (re->polling) {
		DEBUG_WARNING("main loop already polling\n");
		return EALREADY;
	}

	err = poll_setup(re);
	if (err)
		goto out;

	DEBUG_INFO("Using async I/O polling method: `%s'\n",
		   poll_method_name(re->method));

	re->polling = true;

	re_lock(re);
	for (;;) {

		if (re->sig) {
			if (signalh)
				signalh(re->sig);

			re->sig = 0;
		}

		if (!re->polling) {
			err = 0;
			break;
		}

		err = fd_poll(re);
		if (err) {
			if (EINTR == err)
				continue;

#ifdef DARWIN
			/* NOTE: workaround for Darwin */
			if (EBADF == err)
				continue;
#endif

			break;
		}

		tmr_poll(&re->tmrl);
	}
	re_unlock(re);

 out:
	re->polling = false;

	return err;
}


/**
 * Cancel the main polling loop
 */
void re_cancel(void)
{
	struct re *re = re_get();

	re->polling = false;
}


/**
 * Debug the main polling loop
 *
 * @param pf     Print handler where debug output is printed to
 * @param unused Unused parameter
 *
 * @return 0 if success, otherwise errorcode
 */
int re_debug(struct re_printf *pf, void *unused)
{
	struct re *re = re_get();
	int err = 0;

	(void)unused;

	err |= re_hprintf(pf, "re main loop:\n");
	err |= re_hprintf(pf, "  maxfds:  %d\n", re->maxfds);
	err |= re_hprintf(pf, "  nfds:    %d\n", re->nfds);
	err |= re_hprintf(pf, "  method:  %d (%s)\n", re->method,
			  poll_method_name(re->method));

	return err;
}


/**
 * Set async I/O polling method. This function can also be called while the
 * program is running.
 *
 * @param method New polling method
 *
 * @return 0 if success, otherwise errorcode
 */
int poll_method_set(enum poll_method method)
{
	struct re *re = re_get();
	int err;

	err = fd_setsize(DEFAULT_MAXFDS);
	if (err)
		return err;

	switch (method) {

#ifdef HAVE_POLL
	case METHOD_POLL:
		break;
#endif
#ifdef HAVE_SELECT
	case METHOD_SELECT:
		if (re->maxfds > (int)FD_SETSIZE) {
			DEBUG_WARNING("SELECT: maxfds > FD_SETSIZE\n");
			return EMFILE;
		}
		break;
#endif
#ifdef HAVE_EPOLL
	case METHOD_EPOLL:
		if (!epoll_check())
			return EINVAL;
		break;
#endif
#ifdef HAVE_KQUEUE
	case METHOD_KQUEUE:
		break;
#endif
	default:
		DEBUG_WARNING("poll method not supported: '%s'\n",
			      poll_method_name(method));
		return EINVAL;
	}

	re->method = method;
	re->update = true;

	DEBUG_INFO("Setting async I/O polling method to `%s'\n",
		   poll_method_name(re->method));

	err = poll_init(re);
	if (err)
		return err;

	return rebuild_fds(re);
}


/**
 * Add a worker thread for this thread
 *
 * @return 0 if success, otherwise errorcode
 */
int re_thread_init(void)
{
#ifdef HAVE_PTHREAD
	struct re *re;

	pthread_once(&pt_once, re_once);

	re = pthread_getspecific(pt_key);
	if (re) {
		DEBUG_WARNING("thread_init: already added for thread %d\n",
			      pthread_self());
		return EALREADY;
	}

	re = malloc(sizeof(*re));
	if (!re)
		return ENOMEM;

	memset(re, 0, sizeof(*re));
	pthread_mutex_init(&re->mutex, NULL);
	re->mutexp = &re->mutex;

#ifdef HAVE_EPOLL
	re->epfd = -1;
#endif

#ifdef HAVE_KQUEUE
	re->kqfd = -1;
#endif

	pthread_setspecific(pt_key, re);
	return 0;
#else
	return ENOSYS;
#endif
}


/**
 * Remove the worker thread for this thread
 */
void re_thread_close(void)
{
#ifdef HAVE_PTHREAD
	struct re *re;

	pthread_once(&pt_once, re_once);

	re = pthread_getspecific(pt_key);
	if (re) {
		poll_close(re);
		free(re);
		pthread_setspecific(pt_key, NULL);
	}
#endif
}


/**
 * Enter an 're' thread
 *
 * @note Must only be called from a non-re thread
 */
void re_thread_enter(void)
{
	re_lock(re_get());
}


/**
 * Leave an 're' thread
 *
 * @note Must only be called from a non-re thread
 */
void re_thread_leave(void)
{
	re_unlock(re_get());
}


/**
 * Set an external mutex for this thread
 *
 * @param mutexp Pointer to external mutex, NULL to use internal
 */
void re_set_mutex(void *mutexp)
{
#ifdef HAVE_PTHREAD
	struct re *re = re_get();

	re->mutexp = mutexp ? mutexp : &re->mutex;
#else
	(void)mutexp;
#endif
}


/**
 * Get the timer-list for this thread
 *
 * @return Timer list
 *
 * @note only used by tmr module
 */
struct list *tmrl_get(void);
struct list *tmrl_get(void)
{
	return &re_get()->tmrl;
}

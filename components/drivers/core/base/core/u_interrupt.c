/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <pthread.h>

#include "aos/list.h"

#include <drivers/bug.h>
#include <drivers/compat.h>
#include <drivers/char/u_device.h>
#include <drivers/u_interrupt.h>

//TODO: use rbtree instead
#if U_IRQ_DESC_LIST_RBTREE
#else
dlist_t g_u_irq_thread_head;
dlist_t g_u_irq_desc_head;

u_irq_lock_t g_u_irq_lock;

u_irq_desc_t g_dummy_irq = {.name = "dummy",
							.irq_id = 0xFEDC,
							.irq_handler = NULL,
							.thread_handler = NULL,
							.irq_cnt = 0,
							.flags = 0,
							.data = &g_dummy_irq,
							.p_irq_th = NULL,
							};
#endif

#if U_IRQ_LOCK_SPINLOCK

static inline int u_irq_lock_init(u_irq_lock_t *lock) {
	int ret = -1;

	ret = pthread_spin_init(lock, PTHREAD_PROCESS_PRIVATE);

	if (ret) {
		ddkc_err("pthread_spin_init failed, ret:%d\r\n", ret);
	} else {
		ddkc_dbg("pthread_spin_init success\r\n");
	}

	return ret;
}

static inline int u_irq_lock_lock(u_irq_lock_t *lock) {
	int ret = -1;

	ret = pthread_spin_lock(lock);

	if (ret) {
		ddkc_err("pthread_spin_lock failed, ret:%d\r\n", ret);
	} else {
		ddkc_dbg("pthread_spin_lock success\r\n");
	}

	return ret;
}

static inline int u_irq_lock_unlock(u_irq_lock_t *lock) {
	int ret = -1;

	ret = pthread_spin_unlock(lock);

	if (ret) {
		ddkc_err("pthread_spin_unlock failed, ret:%d\r\n", ret);
	} else {
		ddkc_dbg("pthread_spin_unlock success\r\n");
	}

	return ret;
}


static inline int u_irq_lock_destroy(u_irq_lock_t *lock) {
	int ret = -1;

	ret = pthread_spin_destroy(lock);

	if (ret) {
		ddkc_err("pthread_spin_destroy failed, ret:%d\r\n", ret);
	} else {
		ddkc_dbg("pthread_spin_destroy success\r\n");
	}

	return ret;
}
#else
static inline int u_irq_lock_init(u_irq_lock_t *lock) {
	int ret = -1;

	ret = pthread_mutex_init(lock, NULL);

	if (ret) {
		ddkc_err("pthread_mutex_init failed, ret:%d\r\n", ret);
	} else {
		ddkc_loud("pthread_mutex_init success\r\n");
	}

	return ret;
}

static inline int u_irq_lock_lock(u_irq_lock_t *lock) {
	int ret = -1;

	ret = pthread_mutex_lock(lock);

	if (ret) {
		ddkc_err("pthread_mutex_lock failed, ret:%d\r\n", ret);
	} else {
		ddkc_loud("pthread_mutex_lock success\r\n");
	}

	return ret;
}

static inline int u_irq_lock_unlock(u_irq_lock_t *lock) {
	int ret = -1;

	ret = pthread_mutex_unlock(lock);

	if (ret) {
		ddkc_err("pthread_mutex_unlock failed, ret:%d\r\n", ret);
	} else {
		ddkc_loud("pthread_mutex_unlock success\r\n");
	}

	return ret;
}


static inline int u_irq_lock_destroy(u_irq_lock_t *lock) {
	int ret = -1;

	ret = pthread_mutex_destroy(lock);

	if (ret) {
		ddkc_err("pthread_mutex_destroy failed, ret:%d\r\n", ret);
	} else {
		ddkc_loud("pthread_mutex_destroy success\r\n");
	}

	return ret;
}


#endif

u_irqreturn_t u_irq_handler(int irq_id, void *data) {

    u_irq_desc_t *tmp = NULL;
	u_irq_desc_t *desc = (u_irq_desc_t *)data;
	u_irq_thread_t *p_irq_th = NULL;
	u_irq_msg_t *p_irq_msg = NULL;

#if 0
	p_irq_th = desc->p_irq_th;

	p_irq_msg = (u_irq_msg_t *)malloc(sizeof(*p_irq_msg));
	if (p_irq_msg) {
		p_irq_msg->u_irq = desc;
		u_irq_lock_lock(&p_irq_th->lock);
		dlist_add_tail(&p_irq_msg->node, &p_irq_th->msg_head);
		u_irq_lock_unlock(&p_irq_th->lock);

		pthread_cond_signal(&p_irq_th->cond);
	} else {
		ddkc_err("malloc for irq_msg failed, %d missed\n", irq_id);
	}
#else
	// search irq_id's thread
    dlist_for_each_entry_safe(&g_u_irq_desc_head, tmp, desc, u_irq_desc_t, irq_desc_node) {
		if (desc->irq_id != irq_id)
			continue;

		p_irq_th = desc->p_irq_th;
		// TODO: malloc is not allowed in IRQ context
		// add irq_id into p_irq_th's message list
		p_irq_msg = (u_irq_msg_t *)malloc(sizeof(*p_irq_msg));
		if (p_irq_msg) {
			p_irq_msg->u_irq = desc;
			u_irq_lock_lock(&p_irq_th->lock);
			dlist_add_tail(&p_irq_msg->node, &p_irq_th->msg_head);
			u_irq_lock_unlock(&p_irq_th->lock);

			pthread_cond_signal(&p_irq_th->cond);
		} else {
			ddkc_err("malloc for irq_msg failed, %d missed\r\n", irq_id);
		}

	}
#endif
	return IRQ_HANDLED;
}

void* u_irq_thread_fn(void *arg) {
	unsigned int r = -1;
	u_irq_thread_t *p_irq_th = (u_irq_thread_t *)arg;
    u_irq_msg_t *tmp = NULL;
	u_irq_msg_t *p_irq_msg = NULL, *next = NULL;
	u_irq_desc_t *desc = NULL;

	BUG_ON(!p_irq_th);

	do {
        dlist_for_each_entry_safe(&p_irq_th->msg_head, tmp, p_irq_msg, u_irq_msg_t, node) {
			u_irq_msg_t *m = p_irq_msg;

			//TODO: NOTICE performance issue!!!
			/** shall we add another lock to protect this critical section from operation on desc in u_free_irq?
			 */
			u_irq_lock_lock(&p_irq_th->lock);
			dlist_del(p_irq_msg);
			desc = m->u_irq;
			if (desc->thread_handler)
				(desc->thread_handler)(desc->irq_id, desc->data);
			else if (desc->irq_handler)
				(desc->irq_handler)(desc->irq_id, desc->data);
			desc->irq_cnt++;
			u_irq_lock_unlock(&p_irq_th->lock);

			free(p_irq_msg);
			//TODO: shall add monitor on irq_cnt in <x> ms, disable the irq for flood irq case
		}

		r = pthread_cond_wait(&p_irq_th->cond, &p_irq_th->mutex);
		if (r) {
			ddkc_err("%s wait for cond failed, r:%d\r\n", p_irq_th->name, r);
		}
	} while (!p_irq_th->should_stop);

	//TODO: clear IRQ related resources
	p_irq_th->stopped = 1;
	ddkc_info("thread %p stopped\r\n", p_irq_th);

	return NULL;
}

u_irq_thread_t *u_irq_thread_create(u_irq_desc_t *u_irq, int prio) {
	int r = -1;
	u_irq_thread_t *p_irq_th = NULL;
	pthread_attr_t attr;

	if (!u_irq) {
		ddkc_err("invalid p_irq_th or u_irq\r\n");
		return NULL;
	}

	p_irq_th = (u_irq_thread_t *)malloc(sizeof(*p_irq_th));
	if (!p_irq_th) {
		ddkc_err("malloc u_irq_thread_t for irq-%d failed\r\n", u_irq->irq_id);
		return NULL;
	}
	p_irq_th->should_stop = 0;
	p_irq_th->stopped = 0;

	dlist_init(&p_irq_th->irq_desc_head);
	dlist_init(&p_irq_th->irq_thread_node);
	dlist_init(&p_irq_th->msg_head);

	pthread_condattr_init(&p_irq_th->condattr);
	pthread_condattr_setclock(&p_irq_th->condattr, CLOCK_MONOTONIC);
	pthread_cond_init(&p_irq_th->cond, &p_irq_th->condattr);
	pthread_mutex_init(&p_irq_th->mutex, NULL);
	u_irq_lock_init(&p_irq_th->lock);

	pthread_attr_init(&attr);

#if 0 //TODO: need to adjust thread's priority!!, change 0 and 120 to marco
	struct sched_param sched;
	if (prio <= 0 && prio > 120)
		sched.sched_priority = 1;
	else
		sched.sched_priority = prio;

	pthread_attr_setschedparam(&attr,&sched);
	pthread_attr_setstacksize(&attr,4096);
#endif
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	r = pthread_create(&(p_irq_th->thread), &attr, u_irq_thread_fn, (void*)(p_irq_th));
	pthread_attr_destroy(&attr);
	if(!r) {
		memset(p_irq_th->name, 0, sizeof(p_irq_th->name));

		if (u_irq != &g_dummy_irq)
			snprintf(p_irq_th->name, sizeof(p_irq_th->name), "irq-%d", u_irq->irq_id);
		else
			snprintf(p_irq_th->name, sizeof(p_irq_th->name), "irq-m-%d", getpid());

		// assume this operation won't fail
		pthread_setname_np(p_irq_th->thread, p_irq_th->name);
	} else {
		ddkc_err("pthread_create for irq-%d failed, r:%d\r\n", u_irq->irq_id, r);
		free(p_irq_th);
		return NULL;
	}
	ddkc_info("pthread create for irq-%d success, pid:%p, name:%s\r\n",
	         u_irq->irq_id, p_irq_th->thread, p_irq_th->name);
	// add irq's thread_node into thread's irq_desc_head
	u_irq_lock_lock(&p_irq_th->lock);
	dlist_add_tail(&u_irq->thread_node, &p_irq_th->irq_desc_head);
	u_irq_lock_unlock(&p_irq_th->lock);

	u_irq_lock_lock(&g_u_irq_lock);
	dlist_add_tail(&p_irq_th->irq_thread_node, &g_u_irq_thread_head);
	u_irq_lock_unlock(&g_u_irq_lock);

	return p_irq_th;
}

int u_irq_thread_delete(u_irq_thread_t *t) {
	t->should_stop = 1;
	pthread_cond_signal(&t->cond);

	/* wait for thread to quit and free the thread if necessary */
	while(!t->stopped) {
		// TODO: ethan - should add sychnronized mechanism
		//msleep(50);
		usleep(50 * 1000);
	}

	u_irq_lock_lock(&g_u_irq_lock);
	dlist_del(&t->irq_thread_node);
	u_irq_lock_unlock(&g_u_irq_lock);

	WARN_ON(!dlist_empty(&t->irq_desc_head));

	pthread_cond_destroy(&t->cond);
	pthread_mutex_destroy(&t->mutex);
	u_irq_lock_destroy(&t->lock);
	pthread_condattr_destroy(&t->condattr);

	free(t);

	return 0;
}

//TODO:- must be called before any other IRQ system APIs called
int u_irq_system_init(void) {
	int r = 0;

	u_irq_thread_t *g_u_irq_th = NULL;

	dlist_init(&g_u_irq_thread_head);
	dlist_init(&g_u_irq_desc_head);
	u_irq_lock_init(&g_u_irq_lock);

	g_u_irq_th = u_irq_thread_create(&g_dummy_irq, -1);

	if (g_u_irq_th) {
		ddkc_info("u_irq system init success\r\n");
		r = 0;
	} else {
        ddkc_err("u_irq_thread_create for g_dummy_irq failed\r\n");
        return -ENOMEM;
	}

	g_dummy_irq.p_irq_th = g_u_irq_th;

	u_irq_lock_lock(&g_u_irq_lock);
	dlist_add_tail(&g_dummy_irq.irq_desc_node, &g_u_irq_desc_head);
	u_irq_lock_unlock(&g_u_irq_lock);

	return r;
}

int u_irq_system_deinit(void) {
	u_irq_desc_t *desc = NULL;
	u_irq_desc_t *n = NULL;

	// search irq_id's thread
	dlist_for_each_entry_safe(&g_u_irq_desc_head, n, desc, u_irq_desc_t, irq_desc_node) {

		if (desc == &g_dummy_irq)
			continue;

		// deattch irq desc from g_u_irq_desc_head
		u_irq_lock_lock(&g_u_irq_lock);
		dlist_del(&desc->irq_desc_node);
		u_irq_lock_unlock(&g_u_irq_lock);

		// remove irq desc
		u_irq_remove(desc);

		free(desc);
	}

	// remove irq desc
	u_irq_remove(desc);

	return 0;
}

int u_irq_setup(u_irq_desc_t *u_irq) {
	if (!u_irq)
		return -EINVAL;

	BUG_ON_MSG((g_dummy_irq.irq_id == u_irq->irq_id),
	           "u_irq->irq_id equals with dummy irq_id, should NEVER happen\n");

	if (u_irq->thread_handler) {
		u_irq_thread_t *u_irq_th = u_irq_thread_create(u_irq, -1);
		if (!u_irq_th)
			return -ENOMEM;

		u_irq->p_irq_th = u_irq_th;
	} else if (u_irq->irq_handler) {
		u_irq->p_irq_th = g_dummy_irq.p_irq_th;
	} else {
		ddkc_err("!!!!This should never happen\r\n");
		return -EINVAL;
	}
	// add irq_desc_node to global u_irq_desc list
	u_irq_lock_lock(&g_u_irq_lock);
	dlist_add_tail(&u_irq->irq_desc_node, &g_u_irq_desc_head);
	u_irq_lock_unlock(&g_u_irq_lock);
	/** TODO: call system API to register IRQ
	 * Passthrough mode: register u_irq->irq_handler directly to low level system
	 * Agent mode: register agent irq handler (u_irq_handler) to system
	 */


	return 0;
}

int u_irq_remove(u_irq_desc_t *u_irq) {
	int ret = 0;
	u_irq_thread_t *p_irq_th = NULL;

	if (!u_irq) {
		ddkc_err("invalid u_irq:%p\r\n", u_irq);
		return -EINVAL;
	}

	if (!u_irq->p_irq_th || !u_irq->thread_handler) {
		ddkc_dbg("irq:%d pthread_irq_th:%p, thread_handler:%p, no need to stop irq thread\r\n",
		         u_irq->irq_id, u_irq->p_irq_th, u_irq->thread_handler);
		return 0;
	}
	ddkc_dbg("start to clear resource for irq:%d\r\n", u_irq->irq_id);

	p_irq_th = u_irq->p_irq_th;
	if ((p_irq_th == g_dummy_irq.p_irq_th) &&
		(u_irq->irq_id != g_dummy_irq.irq_id)) {
		ddkc_info("irq[%d] shares common irq thread, ignore\r\n", u_irq->irq_id);
		return 0;
	}

	ret = u_irq_thread_delete(p_irq_th);

	ddkc_dbg("thread stopped for irq:%d\r\n", u_irq->irq_id);

	return 0;
}

int u_request_threaded_irq(
	unsigned int irq, u_irq_handler_t handler, u_irq_handler_t thread_fn,
	unsigned long flags, const char *name, void *data) {
	int r = -1;
	u_irq_desc_t *u_irq = NULL;

	if (!handler && !thread_fn) {
		ddkc_err("both handler and thread_fn is NULL\r\n", irq);
		return -EINVAL;
	}

	u_irq = (u_irq_desc_t *)malloc (sizeof(u_irq_desc_t));
	if (!u_irq) {
		ddkc_err("malloc u_irq_desc_t failed for irq:%d\r\n", irq);
		return -ENOMEM;
	}
#if U_IRQ_DESC_LIST_RBTREE
#else
	dlist_init(&u_irq->irq_desc_node);
#endif
	dlist_init(&u_irq->thread_node);

	u_irq->name = name;
	u_irq->data = data;
	u_irq->flags = flags;
	u_irq->irq_cnt = 0;
	u_irq->irq_id = irq;
	u_irq->irq_handler = handler;
	u_irq->thread_handler = thread_fn;
	u_irq->p_irq_th = NULL;

	/* setup new thread if needed */
	r = u_irq_setup(u_irq);
	if (r)
		goto irq_setup_fail;

	return 0;

irq_setup_fail:
	u_irq_remove(u_irq);

	free(u_irq);

	return r;
}

void free_u_irq(unsigned int irq_id) {
	u_irq_desc_t *n = NULL;
	u_irq_desc_t *u_irq = NULL;
	u_irq_thread_t *p_irq_th = NULL;
	u_irq_msg_t *p_irq_msg = NULL, *next = NULL;

	// search irq_id's thread
	dlist_for_each_entry_safe(&g_u_irq_desc_head, n, u_irq, u_irq_desc_t, irq_desc_node) {
		if (u_irq->irq_id != irq_id)
			continue;

		p_irq_th = u_irq->p_irq_th;

		dlist_for_each_entry_safe(&p_irq_th->msg_head, next, p_irq_msg, u_irq_msg_t, node) {

			// clear all irq message of target irq
			if (p_irq_msg->u_irq == u_irq) {
				u_irq_lock_lock(&p_irq_th->lock);
				dlist_del(p_irq_msg);
				u_irq_lock_unlock(&p_irq_th->lock);
				free(p_irq_msg);
			}
		}

		// deattach u_irq from thread's irq list and global irq desc list
		u_irq_lock_lock(&p_irq_th->lock);
		dlist_del(&u_irq->thread_node);
		u_irq_lock_unlock(&p_irq_th->lock);

		u_irq_lock_lock(&g_u_irq_lock);
		dlist_del(&u_irq->irq_desc_node);
		u_irq_lock_unlock(&g_u_irq_lock);

		// try to clean irq's thread resource
		u_irq_remove(u_irq);
		free(u_irq);
	}

	return 0;
}
void disable_u_irq_nosync(unsigned int irq) {
}

void disable_u_irq(unsigned int irq) {
}

void enable_u_irq(unsigned int irq) {
}

__weak void free_irq(unsigned int irq, void *dev_id) {
}

void __wrap_free_irq(unsigned int irq, void *dev_id) {
    __real_free_irq(irq, dev_id);
}


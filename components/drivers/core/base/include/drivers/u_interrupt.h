/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _U_INTERRUPT_H_
#define _U_INTERRUPT_H_

#include "aos/list.h"

#include <drivers/atomic.h>

#define U_IRQ_PASS_THROUGH 0
#define U_IRQ_LOCK_SPINLOCK 0

//TODO: irq description rbtree control marco
#define U_IRQ_DESC_LIST_RBTREE 0

#if U_IRQ_DESC_LIST_RBTREE
#include "k_rbtree.h"
#endif


/**
 * enum irqreturn
 * @IRQ_NONE		interrupt was not from this device or was not handled
 * @IRQ_HANDLED		interrupt was handled by this device
 * @IRQ_WAKE_THREAD	handler requests to wake the handler thread
 */
typedef enum u_irqreturn {
	IRQ_NONE		= (0 << 0),
	IRQ_HANDLED		= (1 << 0),
	IRQ_WAKE_THREAD		= (1 << 1),
} u_irqreturn_t;

typedef u_irqreturn_t (*u_irq_handler_t)(int, void *);

int u_request_threaded_irq(
	unsigned int irq, u_irq_handler_t handler, u_irq_handler_t thread_fn,
	unsigned long flags, const char *name, void *data);

static inline int u_request_irq(
	unsigned int irq, u_irq_handler_t handler,
	unsigned long flags, const char *name, void *data) {
	return u_request_threaded_irq(irq, handler, NULL, flags, name, data);
}

void disable_u_irq_nosync(unsigned int irq);
void disable_u_irq(unsigned int irq);
void enable_u_irq(unsigned int irq);

#if U_IRQ_DESC_LIST_RBTREE
typedef struct u_irq_node {
	struct k_rbtree_node_t rbt_node;  /* rbttree node */
	void *data;       /* data */
} u_irq_node_t;
#endif

#if U_IRQ_LOCK_SPINLOCK
typedef pthread_spinlock_t u_irq_lock_t;
#else
typedef pthread_mutex_t u_irq_lock_t;
#endif

typedef struct u_irq_thread {
	unsigned int should_stop;
	unsigned int stopped;
	char name[16];
	dlist_t irq_desc_head;
	dlist_t irq_thread_node;
	pthread_t thread;
	pthread_cond_t cond;
	pthread_condattr_t condattr;
	pthread_mutex_t mutex;
	u_irq_lock_t lock;
	dlist_t msg_head;
} u_irq_thread_t;

typedef struct u_irq_desc {
	const char *name;
	unsigned int irq_id;
	u_irq_handler_t irq_handler;
	u_irq_handler_t thread_handler;
	unsigned int irq_cnt;
	unsigned int flags;
	void *data;
	u_irq_thread_t *p_irq_th;
#if U_IRQ_DESC_LIST_RBTREE
	u_irq_node_t node;
#else
	dlist_t irq_desc_node; // pointer to next u_irq_desc
	dlist_t thread_node;   // node in irq thread list
#endif
} u_irq_desc_t;

typedef struct u_irq_msg {
	dlist_t node;
	u_irq_desc_t *u_irq;
} u_irq_msg_t;

int u_irq_system_init(void);
int u_irq_system_deinit(void);

#endif //_U_INTERRUPT_H_

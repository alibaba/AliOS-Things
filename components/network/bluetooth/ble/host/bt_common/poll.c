/*
 * Copyright (c) 2017 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 *
 * @brief Kernel asynchronous event polling interface.
 *
 * This polling mechanism allows waiting on multiple events concurrently,
 * either events triggered directly, or from kernel objects or other kernel
 * constructs.
 */

#include <stdio.h>

#include <zephyr.h>
#include <zephyr/types.h>
#include <misc/slist.h>
#include <misc/dlist.h>
#include <misc/__assert.h>

struct event_cb {
    sys_dlist_t next;
    struct k_poll_event *events;
    int num_events;
    struct k_sem sem;
};

static sys_dlist_t event_cb_list = SYS_DLIST_STATIC_INIT(&event_cb_list);
static volatile int event_cb_counter = 0;

void k_poll_event_init(struct k_poll_event *event, u32_t type, int mode,
                       void *obj)
{
    __ASSERT(mode == K_POLL_MODE_NOTIFY_ONLY,
             "only NOTIFY_ONLY mode is supported\n");
    __ASSERT(type < (1 << _POLL_NUM_TYPES), "invalid type\n");
    __ASSERT(obj, "must provide an object\n");

    event->poller = NULL;
    /* event->tag is left uninitialized: the user will set it if needed */
    event->type   = type;
    event->state  = K_POLL_STATE_NOT_READY;
    event->mode   = mode;
    event->unused = 0;
    event->obj    = obj;
}

static inline void set_event_state(struct k_poll_event *event, u32_t state)
{
    event->poller = NULL;
    event->state |= state;
}

extern int has_tx_sem(struct k_poll_event *event);
extern void event_callback(uint8_t event_type);
static int _signal_poll_event(struct k_poll_event *event, u32_t state, int *must_reschedule)
{
    *must_reschedule = 0;
    if (event->type != K_POLL_TYPE_DATA_AVAILABLE || has_tx_sem(event)) {
        set_event_state(event, state);
        event_callback(K_POLL_TYPE_FIFO_DATA_AVAILABLE);
    }

    return 0;
}

void _handle_obj_poll_events(sys_dlist_t *events, u32_t state)
{
    struct k_poll_event *poll_event;
    int must_reschedule;

    poll_event = (struct k_poll_event *)sys_dlist_get(events);
    if (poll_event) {
        (void)_signal_poll_event(poll_event, state, &must_reschedule);
    }
}

/* must be called with interrupts locked */
static inline int is_condition_met(struct k_poll_event *event, u32_t *state)
{
    switch (event->type) {
        case K_POLL_TYPE_DATA_AVAILABLE:
            if (has_tx_sem(event) == 0) {
                return 0;
            }
            if (!k_queue_is_empty(event->queue)) {
                *state = K_POLL_STATE_FIFO_DATA_AVAILABLE;
                return 1;
            }
            break;
        case K_POLL_TYPE_SIGNAL:
            if (event->signal->signaled) {
                *state = K_POLL_STATE_SIGNALED;
                return 1;
            }
            break;
        case K_POLL_TYPE_DATA_RECV:
            if (event->signal->signaled) {
                *state = K_POLL_STATE_DATA_RECV;
                event->signal->signaled = 0;
                return 1;
            }
            break;
        default:
            __ASSERT(0, "invalid event type (0x%x)\n", event->type);
            break;
    }

    return 0;
}

static inline void add_event(sys_dlist_t *events, struct k_poll_event *event,
                             struct _poller *poller)
{
    sys_dlist_append(events, &event->_node);
}

/* must be called with interrupts locked */
static inline int register_event(struct k_poll_event *event,
                                 struct _poller *     poller)
{
    switch (event->type) {
        case K_POLL_TYPE_DATA_AVAILABLE:
            __ASSERT(event->queue, "invalid queue\n");
            add_event(&event->queue->poll_events, event, poller);
            break;
        case K_POLL_TYPE_SIGNAL:
            __ASSERT(event->signal, "invalid poll signal\n");
            add_event(&event->signal->poll_events, event, poller);
            break;
        case K_POLL_TYPE_DATA_RECV:
            __ASSERT(event->queue, "invalid queue\n");
            add_event(&event->signal->poll_events, event, poller);
            break;
        default:
            __ASSERT(0, "invalid event type\n");
            break;
    }

    event->poller = poller;

    return 0;
}

/* must be called with interrupts locked */
static inline void clear_event_registration(struct k_poll_event *event)
{
    event->poller = NULL;

    switch (event->type) {
        case K_POLL_TYPE_DATA_AVAILABLE:
            __ASSERT(event->queue, "invalid queue\n");
            sys_dlist_remove(&event->_node);
            break;
        case K_POLL_TYPE_SIGNAL:
            __ASSERT(event->signal, "invalid poll signal\n");
            sys_dlist_remove(&event->_node);
            break;
        case K_POLL_TYPE_DATA_RECV:
            __ASSERT(event->queue, "invalid queue\n");
            sys_dlist_remove(&event->_node);
            break;
            __ASSERT(0, "invalid event type\n");
            break;
    }
}

/* must be called with interrupts locked */
static inline void clear_event_registrations(struct k_poll_event *events,
                                             int          last_registered,
                                             unsigned int key)
{
    for (; last_registered >= 0; last_registered--) {
        clear_event_registration(&events[last_registered]);
        irq_unlock(key);
        key = irq_lock();
    }
}

static bool polling_events(struct k_poll_event *events, int num_events,
                           s32_t timeout, int *last_registered)
{
    bool polling = true;
    unsigned int key;

    for (int ii = 0; ii < num_events; ii++) {
        u32_t state;
        key = irq_lock();
        if (is_condition_met(&events[ii], &state)) {
            set_event_state(&events[ii], state);
            polling = false;
        } else if (timeout != K_NO_WAIT && polling) {
            register_event(&events[ii], NULL);
            ++(*last_registered);
        }
        irq_unlock(key);
    }
    return polling;
}

void event_callback(uint8_t event_type)
{
    sys_dnode_t *event_next;
    sys_dnode_t *event_next_save;
    struct k_poll_event *events;
    unsigned int key;

    key = irq_lock();
    SYS_DLIST_FOR_EACH_NODE_SAFE(&event_cb_list, event_next, event_next_save) {
        for (int i = 0; i < ((struct event_cb *)event_next)->num_events; i++) {
            if (((struct event_cb *)event_next)->events[i].type == event_type ||
                event_type == K_POLL_TYPE_EARLIER_WORK) {
                k_sem_give(&((struct event_cb *)event_next)->sem);
                break;
            }
        }
    }
    irq_unlock(key);
}

int k_poll(struct k_poll_event *events, int num_events, s32_t timeout)
{
    int last_registered = -1;
    unsigned int key;
    bool polling = true;
    struct event_cb eventcb;

    /* find events whose condition is already fulfilled */
#if 0
    polling = polling_events(events, num_events, timeout, &last_registered);

    if (polling == false) {
        goto exit;
    }
#endif

    eventcb.events = events;
    eventcb.num_events = num_events;

    k_sem_init(&eventcb.sem, 0, 1);

    sys_dlist_append(&event_cb_list, (sys_dnode_t *)&eventcb);
    event_cb_counter++;

    polling = polling_events(events, num_events, timeout, &last_registered);

    if (polling == false) {
        goto exit;
    }

    k_sem_take(&eventcb.sem, timeout);

    last_registered = -1;
    polling_events(events, num_events, K_NO_WAIT, &last_registered);
exit:
    sys_dlist_remove((sys_dnode_t *)&eventcb);
    k_sem_delete(&eventcb.sem);

    key = irq_lock();
    clear_event_registrations(events, last_registered, key);
    irq_unlock(key);
    return 0;
}

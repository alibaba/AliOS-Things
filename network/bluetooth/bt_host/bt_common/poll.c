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

struct k_sem g_poll_sem;

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
static int _signal_poll_event(struct k_poll_event *event, u32_t state,
                              int *must_reschedule, uint8_t sync)
{
    *must_reschedule = 0;
    if (event->type != K_POLL_TYPE_DATA_AVAILABLE || has_tx_sem(event)) {
        set_event_state(event, state);
    }

    if (sync == 0) {
        k_sem_give(&g_poll_sem);
    }
    return 0;
}

void _handle_obj_poll_events(sys_dlist_t *events, u32_t state, uint8_t sync)
{
    struct k_poll_event *poll_event;
    int must_reschedule;

    poll_event = (struct k_poll_event *)sys_dlist_get(events);
    if (poll_event) {
        (void)_signal_poll_event(poll_event, state, &must_reschedule, sync);
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
        default:
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
    int rc;
    bool polling = true;
    unsigned int key;

    for (int ii = 0; ii < num_events; ii++) {
        u32_t state;
        key = irq_lock();
        if (is_condition_met(&events[ii], &state)) {
            set_event_state(&events[ii], state);
            polling = false;
        } else if (timeout != K_NO_WAIT && polling) {
            rc = register_event(&events[ii], NULL);
            if (rc == 0) {
                ++(*last_registered);
            } else {
                __ASSERT(0, "unexpected return code\n");
            }
        }
        irq_unlock(key);
    }
    return polling;
}

int k_poll(struct k_poll_event *events, int num_events, s32_t timeout)
{
    int last_registered = -1;
    unsigned int key;
    bool polling = true;

    /* find events whose condition is already fulfilled */
    polling = polling_events(events, num_events, timeout, &last_registered);

    if (polling == false) {
        goto exit;
    }

    k_sem_take(&g_poll_sem, timeout);

    last_registered = -1;
    polling_events(events, num_events, timeout, &last_registered);
exit:
    key = irq_lock();
    clear_event_registrations(events, last_registered, key);
    irq_unlock(key);
    return 0;
}

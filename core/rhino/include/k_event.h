/**
 * @file k_event.h
 *
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef K_EVENT_H
#define K_EVENT_H

/** @addtogroup aos_rhino event
 *  Event can be used for tasks synchronize.
 *
 *  @{
 */

/**
 * event object
 */
typedef struct {
    /**<
     *  Manage blocked tasks
     *  List head is this event, list node is task, which are blocked in this event
     */
    blk_obj_t blk_obj;
    uint32_t  flags;            /**< 32bit mapping 32 synchronization events */
#if (RHINO_CONFIG_KOBJ_LIST > 0)
    klist_t   event_item;       /**< kobj list for statistics */
#endif
    uint8_t   mm_alloc_flag;    /**< buffer from internal malloc or caller input */
} kevent_t;

/**
 * event operation bit1 - AND / OR
 * AND, Synchronization success only for all flag bits matching
 * OR,  Synchronization success only for 1 flag bits matching
 */
#define RHINO_FLAGS_AND_MASK    0x2u
/**
 * event operation bit0 - clear / not clear
 */
#define RHINO_FLAGS_CLEAR_MASK  0x1u

/**
 * event operation, explanation in APIs
 */
#define RHINO_AND               0x02u
#define RHINO_AND_CLEAR         0x03u
#define RHINO_OR                0x00u
#define RHINO_OR_CLEAR          0x01u

/**
 * Create a event.
 *
 * @param[in]  event  pointer to the event (the space is provided outside, by user)
 * @param[in]  name   name of the event
 * @param[in]  flags  flags to be init
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_event_create(kevent_t *event, const name_t *name, uint32_t flags);

/**
 * Delete a event.
 *
 * @param[in]  event  pointer to a event
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_event_del(kevent_t *event);

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
/**
 * Malloc and create a event.
 *
 * @param[out]  event  pointer to the event (the space is provided inside, from heap)
 * @param[in]   name   name of the semaphore
 * @param[in]   flags  flags to be init
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_event_dyn_create(kevent_t **event, const name_t *name, uint32_t flags);

/**
 * Delete and free a event.
 *
 * @param[in]  event  pointer to a event
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_event_dyn_del(kevent_t *event);
#endif

/**
 * Get event, task may be blocked.
 * opt = RHINO_AND: Waiting for 'event->flags' & 'flags' == 'flags', do not clear 'event->flags' after success.
 * opt = RHINO_AND_CLEAR: Waiting for 'event->flags' & 'flags' == 'flags', clear 'event->flags' after success.
 * opt = RHINO_OR: Waiting for 'event->flags' & 'flags' != 0, do not clear 'event->flags' after success.
 * opt = RHINO_OR_CLEAR: Waiting for 'event->flags' & 'flags' != 0, clear 'event->flags' after success.
 *
 * @param[in]   event       pointer to the event
 * @param[in]   flags       which is provided by users
 * @param[in]   opt         could be RHINO_AND, RHINO_AND_CLEAR, RHINO_OR, RHINO_OR_CLEAR
 * @param[out]  actl_flags  the actually flag where flags is satisfied
 * @param[in]   ticks       ticks to wait before getting success
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_event_get(kevent_t *event, uint32_t flags, uint8_t opt,
                         uint32_t *actl_flags, tick_t ticks);

/**
 * Set a event
 * opt = RHINO_AND: Clear some bit in 'event->flags', 'event->flags' &= 'flags'.
 * opt = RHINO_OR: Set some bit in 'event->flags', 'event->flags' |= 'flags', may invoke other waiting task
 *
 * @param[in]  event  pointer to a event
 * @param[in]  flags  which users want to be set
 * @param[in]  opt    could be RHINO_AND, RHINO_OR
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_event_set(kevent_t *event, uint32_t flags, uint8_t opt);

/** @} */

#endif /* K_EVENT_H */


/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _BLE_OS_PORT_H
#define _BLE_OS_PORT_H

#ifndef ASSERT
#define ASSERT __ASSERT
#endif
#include <k_api.h>
#include <misc/dlist.h>
#include <misc/slist.h>

#define _STRINGIFY(x) #x

#define ___in_section(a, b, c) \
        __attribute__((section("." _STRINGIFY(a)            \
                               "." _STRINGIFY(b)           \
                               "." _STRINGIFY(c))))

//#define __in_section(a, b, c) ___in_section(a, b, c)
#define __in_section(a, b, c)

#define __in_section_unique(seg) ___in_section(seg, _FILE_PATH_HASH, \
            __COUNTER__)

#define __in_btstack_section() __in_section(btstack, static, func)

#ifndef ARG_UNUSED
#define ARG_UNUSED(x) (void)(x)
#endif

#ifndef __packed
#define __packed              __attribute__((__packed__))
#endif

#ifndef __printf_like
#define __printf_like(f, a)   __attribute__((format (printf, f, a)))
#endif
#define  STACK_ALIGN 4
#define __stack __aligned(STACK_ALIGN)

#define K_FOREVER -1
#define K_NO_WAIT 0

    /* Unaligned access */
#define UNALIGNED_GET(p)                        \
        __extension__ ({                            \
            struct  __attribute__((__packed__)) {               \
                __typeof__(*(p)) __v;                   \
            } *__p = (__typeof__(__p)) (p);                 \
            __p->__v;                           \
        })

#ifndef UNUSED
#define UNUSED(x) (void)x
#endif

    enum _poll_types_bits {
        _POLL_TYPE_IGNORE,
        _POLL_TYPE_SIGNAL,
        _POLL_TYPE_SEM_AVAILABLE,
        _POLL_TYPE_DATA_AVAILABLE,
        _POLL_NUM_TYPES
    };

#define _POLL_TYPE_BIT(type) (1 << ((type)-1))

    enum _poll_states_bits {
        _POLL_STATE_NOT_READY,
        _POLL_STATE_SIGNALED,
        _POLL_STATE_SEM_AVAILABLE,
        _POLL_STATE_DATA_AVAILABLE,
        _POLL_NUM_STATES
    };

#define _POLL_STATE_BIT(state) (1 << ((state)-1))

#define _POLL_EVENT_NUM_UNUSED_BITS                             \
        (32 - (0 + 8                                    /* tag */   \
               + _POLL_NUM_TYPES + _POLL_NUM_STATES + 1 /* modes */ \
              ))

#define K_POLL_SIGNAL_INITIALIZER(obj)                                         \
        {                                                                          \
            .poll_events = SYS_DLIST_STATIC_INIT(&obj.poll_events), .signaled = 0, \
                           .result = 0,                                                           \
        }

    struct k_poll_event {
        sys_dnode_t     _node;
        struct _poller *poller;
        uint32_t           tag : 8;
    uint32_t           type :
        _POLL_NUM_TYPES;
    uint32_t           state :
        _POLL_NUM_STATES;
        uint32_t           mode : 1;
    uint32_t           unused :
        _POLL_EVENT_NUM_UNUSED_BITS;
        union {
            void                 *obj;
            struct k_poll_signal *signal;
            struct k_sem         *sem;
            struct kfifo        *fifo;
            struct k_queue       *queue;
        };
    };

    struct k_poll_signal {
        sys_dlist_t  poll_events;
        unsigned int signaled;
        int          result;
    };

#define K_POLL_STATE_NOT_READY 0
#define K_POLL_STATE_EADDRINUSE 1
#define K_POLL_STATE_SIGNALED 2
#define K_POLL_STATE_SEM_AVAILABLE 3
#define K_POLL_STATE_DATA_AVAILABLE 4
#define K_POLL_STATE_FIFO_DATA_AVAILABLE K_POLL_STATE_DATA_AVAILABLE
#define K_POLL_STATE_DATA_RECV 5
#define K_POLL_STATE_TX_SYNC_DONE 6


#define K_POLL_TYPE_IGNORE 0
#define K_POLL_TYPE_SIGNAL 1
#define K_POLL_TYPE_SEM_AVAILABLE 2
#define K_POLL_TYPE_DATA_AVAILABLE 3
#define K_POLL_TYPE_FIFO_DATA_AVAILABLE K_POLL_TYPE_DATA_AVAILABLE
#define K_POLL_TYPE_DATA_RECV 5
#define K_POLL_TYPE_EARLIER_WORK 6

#define K_POLL_EVENT_STATIC_INITIALIZER(event_type, event_mode, event_obj,     \
                                            event_tag)                             \
    {                                                                          \
        .type = event_type, .tag = event_tag, .state = K_POLL_STATE_NOT_READY, \
                                   .mode = event_mode, .unused = 0, { .obj = event_obj },                 \
    }

extern void k_poll_signal_raise(struct k_poll_signal *signal, int result);

#if 0  //YULONG
extern int  k_poll(struct k_poll_event *events, int num_events,
                   int32_t timeout);
extern void k_poll_event_init(struct k_poll_event *event, uint32_t type,
                              int mode, void *obj);
extern void _handle_obj_poll_events(sys_dlist_t *events, uint32_t state);
#endif

/* public - polling modes */
enum k_poll_modes {
    /* polling thread does not take ownership of objects when available */
    K_POLL_MODE_NOTIFY_ONLY = 0,

    K_POLL_NUM_MODES
};

#define BT_STACK(name, size) \
        k_thread_stack_t name[(size) / sizeof(k_thread_stack_t)];

#define BT_STACK_NOINIT(name, size) \
        k_thread_stack_t name[(size) / sizeof(k_thread_stack_t)];

    static inline  void k_call_stacks_analyze()
    {
        return;
    }

    static inline char *K_THREAD_STACK_BUFFER(uint32_t *sym)
    {
        return NULL;
    }

#define k_oops()    while(1)

    void *k_current_get(void);
    uint32_t k_get_tick(void);
    uint32_t k_tick2ms(uint32_t tick);
    void k_sleep(int32_t ms);

    uint32_t k_uptime_get_32();


#define _DO_CONCAT(x, y) x ## y
#define _CONCAT(x, y) _DO_CONCAT(x, y)

#ifndef BUILD_ASSERT
    /* compile-time assertion that makes the build fail */
#define BUILD_ASSERT(EXPR) \
        enum _CONCAT(__build_assert_enum, __COUNTER__) { \
            _CONCAT(__build_assert, __COUNTER__) = 1 / !!(EXPR) \
        }
#endif
#ifndef BUILD_ASSERT_MSG
    /* build assertion with message -- common implementation swallows message. */
#define BUILD_ASSERT_MSG(EXPR, MSG) BUILD_ASSERT(EXPR)
#endif

#ifndef MSEC_PER_SEC
#define MSEC_PER_SEC 1000
#endif
    /**
     * @brief Generate timeout delay from milliseconds.
     *
     * This macro generates a timeout delay that that instructs a kernel API
     * to wait up to @a ms milliseconds to perform the requested operation.
     *
     * @param ms Duration in milliseconds.
     *
     * @return Timeout delay value.
     */
#define K_MSEC(ms)     (ms)

    /**
     * @brief Generate timeout delay from seconds.
     *
     * This macro generates a timeout delay that that instructs a kernel API
     * to wait up to @a s seconds to perform the requested operation.
     *
     * @param s Duration in seconds.
     *
     * @return Timeout delay value.
     */
#define K_SECONDS(s)   K_MSEC((s) * MSEC_PER_SEC)

    /**
     * @brief Generate timeout delay from minutes.
     *
     * This macro generates a timeout delay that that instructs a kernel API
     * to wait up to @a m minutes to perform the requested operation.
     *
     * @param m Duration in minutes.
     *
     * @return Timeout delay value.
     */
#define K_MINUTES(m)   K_SECONDS((m) * 60)

    /**
     * @brief Generate timeout delay from hours.
     *
     * This macro generates a timeout delay that that instructs a kernel API
     * to wait up to @a h hours to perform the requested operation.
     *
     * @param h Duration in hours.
     *
     * @return Timeout delay value.
     */
#define K_HOURS(h)     K_MINUTES((h) * 60)

#define popcount(x) __builtin_popcount(x)

static inline unsigned int find_msb_set(uint32_t op)
{
    if (op == 0) {
        return 0;
    }

    return 32 - __builtin_clz(op);
}

static inline unsigned int find_lsb_set(uint32_t op)
{
    return __builtin_ffs(op);
}

#define k_thread_foreach(...)

#define k_free free

#define snprintk snprintf

#ifndef __aligned
#define __aligned(x)          __attribute__((__aligned__(x)))
#endif

#ifndef BIT
#define BIT(nr)                 (1UL << (nr))
#endif

#define __noinit

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) \
    ((unsigned long) (sizeof(array) / sizeof((array)[0])))
#endif

typedef kbuf_queue_t _queue_t;
typedef ksem_t       _sem_t;
typedef ktask_t      _task_t;
typedef cpu_stack_t  _stack_element_t;
typedef kmutex_t     _mutex_t;

/* Log define*/
enum {
    LOG_LEVEL_NONE = 0,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_MAX_BIT
};

#define BT_TAG "AOSBT"

#define BT_LOG_DBG 1
#if BT_LOG_DBG
#define SYS_LOG_DBG(fmt,...) LOGD(BT_TAG, "%s:"fmt, __FUNCTION__, ##__VA_ARGS__)
#define SYS_LOG_INF(fmt,...) LOGI(BT_TAG, "%s:"fmt, __FUNCTION__, ##__VA_ARGS__)
#define SYS_LOG_WRN(fmt,...) LOGW(BT_TAG, "%s:"fmt, __FUNCTION__, ##__VA_ARGS__)
#define SYS_LOG_ERR(fmt,...) LOGE(BT_TAG, "%s:"fmt, __FUNCTION__, ##__VA_ARGS__)
#else
#define SYS_LOG_DBG(fmt,...) LOGD(BT_TAG, fmt, ##__VA_ARGS__)
#define SYS_LOG_INF(fmt,...) LOGI(BT_TAG, fmt, ##__VA_ARGS__)
#define SYS_LOG_WRN(fmt,...) LOGW(BT_TAG, fmt, ##__VA_ARGS__)
#define SYS_LOG_ERR(fmt,...) LOGE(BT_TAG, fmt, ##__VA_ARGS__)
#endif

struct k_queue {
    _sem_t sem;
    sys_slist_t queue_list;
    sys_dlist_t poll_events;
};

extern void  k_queue_init(struct k_queue *queue);
extern void  k_queue_uninit(struct k_queue *queue);
extern void  k_queue_cancel_wait(struct k_queue *queue);
extern void  k_queue_append(struct k_queue *queue, void *data);
extern void  k_queue_prepend(struct k_queue *queue, void *data);
extern void  k_queue_insert(struct k_queue *queue, void *prev, void *data);
extern void  k_queue_append_list(struct k_queue *queue, void *head, void *tail);
extern void *k_queue_get(struct k_queue *queue, int32_t timeout);
extern int   k_queue_is_empty(struct k_queue *queue);
extern int   k_queue_count(struct k_queue *queue);

static inline bool k_queue_remove(struct k_queue *queue, void *data)
{
    return sys_slist_find_and_remove(&queue->queue_list, (sys_snode_t *)data);
}

static inline void *k_queue_first_entry(struct k_queue *queue)
{
    return sys_slist_peek_head(&queue->queue_list);
}

/* lifo define*/
struct k_lifo {
    struct k_queue _queue;
};

#define k_lifo_init(lifo) k_queue_init((struct k_queue *)lifo)

#define k_lifo_put(lifo, data) k_queue_prepend((struct k_queue *)lifo, data)

#define k_lifo_get(lifo, timeout) k_queue_get((struct k_queue *)lifo, timeout)

#define k_lifo_num_get(lifo) k_queue_count((struct k_queue *)lifo)

struct kfifo {
    struct k_queue _queue;
};

#define k_fifo_init(fifo) k_queue_init((struct k_queue *)fifo)

#define k_fifo_cancel_wait(fifo) k_queue_cancel_wait((struct k_queue *)fifo)

#define k_fifo_put(fifo, data) k_queue_append((struct k_queue *)fifo, data)

#define k_fifo_put_list(fifo, head, tail) \
    k_queue_append_list((struct k_queue *)fifo, head, tail)

#define k_fifo_get(fifo, timeout) k_queue_get((struct k_queue *)fifo, timeout)
#define k_fifo_num_get(fifo) k_queue_count((struct k_queue *)fifo)

/* sem define*/
struct k_sem {
    _sem_t      sem;
    sys_dlist_t poll_events;
};

/**
 * @brief Initialize a semaphore.
 *
 * This routine initializes a semaphore object, prior to its first use.
 *
 * @param sem Address of the semaphore.
 * @param initial_count Initial semaphore count.
 * @param limit Maximum permitted semaphore count.
 *
 * @return 0 Creat a semaphore succcess
 */
int k_sem_init(struct k_sem *sem, unsigned int initial_count,
               unsigned int limit);

/**
 * @brief Take a semaphore.
 *
 * This routine takes @a sem.
 *
 * @note Can be called by ISRs, but @a timeout must be set to K_NO_WAIT.
 *
 * @param sem Address of the semaphore.
 * @param timeout Waiting period to take the semaphore (in milliseconds),
 *                or one of the special values K_NO_WAIT and K_FOREVER.
 *
 * @note When porting code from the nanokernel legacy API to the new API, be
 * careful with the return value of this function. The return value is the
 * reverse of the one of nano_sem_take family of APIs: 0 means success, and
 * non-zero means failure, while the nano_sem_take family returns 1 for success
 * and 0 for failure.
 *
 * @retval 0 Semaphore taken.
 * @retval -EBUSY Returned without waiting.
 * @retval -EAGAIN Waiting period timed out.
 */
int k_sem_take(struct k_sem *sem, uint32_t timeout);

/**
 * @brief Give a semaphore.
 *
 * This routine gives @a sem, unless the semaphore is already at its maximum
 * permitted count.
 *
 * @note Can be called by ISRs.
 *
 * @param sem Address of the semaphore.
 *
 * @return 0 Give semaphore success
 */
int k_sem_give(struct k_sem *sem);

/**
 * @brief Delete a semaphore.
 *
 * This routine delete @a sem,
 *
 * @note Can be called by ISRs.
 *
 * @param sem Address of the semaphore.
 *
 * @return 0 delete semaphore success
 */
int k_sem_delete(struct k_sem *sem);

/**
 * @brief Get a semaphore's count.
 *
 * This routine returns the current count of @a sem.
 *
 * @param sem Address of the semaphore.
 *
 * @return Current semaphore count.
 */
unsigned int k_sem_count_get(struct k_sem *sem);

/* mutex define*/
struct k_mutex {
    _mutex_t    mutex;
    sys_dlist_t poll_events;
};

typedef void (*k_timer_handler_t)(void *timer, void *args);
typedef struct k_timer {
    ktimer_t          timer;
    k_timer_handler_t handler;
    void             *args;
    uint32_t          timeout;
    uint32_t          start_ms;
} k_timer_t;

/**
 * @brief Initialize a timer.
 *
 * This routine initializes a timer, prior to its first use.
 *
 * @param timer     Address of timer.
 * @param handle Function to invoke each time the timer expires.
 * @param args   Arguments sent to handle.
 *
 * @return N/A
 */
void k_timer_init(k_timer_t *timer, k_timer_handler_t handle, void *args);

/**
 * @brief Start a timer.
 *
 * @param timer    Address of timer.
 * @param timeout  time before timeout happen(in milliseconds).
 *
 * @return N/A
 */
void k_timer_start(k_timer_t *timer, uint32_t timeout);

/**
 * @brief Stop a timer.
 *
 * @param timer     Address of timer.
 *
 * @return N/A
 */
void k_timer_stop(k_timer_t *timer);

/**
 * @brief Get time now.
 *
 * @return time(in milliseconds)
 */
int64_t k_uptime_get(void);

/**
 * @brief Get time now.
 *
 * @return time(in milliseconds)
 */
uint32_t k_uptime_get_32(void);

/**
 * @brief Judge if a timer started.
 *
 * @param timer     Address of timer.
 *
 * @return N/A
 */
bool k_timer_is_started(k_timer_t *timer);

void k_sleep(int32_t duration);

void k_mutex_init(struct k_mutex *mutex);
int k_mutex_lock(struct k_mutex *mutex, bt_s32_t timeout);
void k_mutex_unlock(struct k_mutex *mutex);

const char *log_strdup(const char *str);

#define __DEPRECATED_MACRO

#endif /* KPORT_H */

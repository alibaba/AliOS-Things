/****************************************************************************
**
**  Name        gki.h
**
**  Function    This file contains GKI public definitions
**
*****************************************************************************/
#ifndef GKI_H
#define GKI_H

/* Include target-specific build configuration header */
#ifdef BUILDCFG
#if ((defined(NFC_HAL_TARGET) && (NFC_HAL_TARGET == TRUE)) && !defined(NFC_HAL_SHARED_GKI))
/* Building NFC HAL w/ non-shared GKI resources */
#include "buildcfg_hal.h"
#else
/* Default build configuration file */
#include "buildcfg.h"
#endif
#endif /* BUILDCFG */

/* Include target-specific GKI configuration */
#if ((defined(NFC_STANDALONE) && (NFC_STANDALONE == TRUE)) || defined(NFC_HAL_SHARED_GKI))
/* Building NFC stack, or NFC HAL w/ shared GKI resources (use shared NFC/HAL target files) */
#include "gki_target.h"
#include "nfc_target.h"
#include "nfc_types.h"
#elif (defined(NFC_HAL_TARGET) && (NFC_HAL_TARGET == TRUE))
/* Building NFC HAL w/ non-shared GKI resources (use HAL-specific target file) */
#include "gki_hal_target.h"
#include "nfc_types.h"
#else
/* Default: building all other targets */
#include "bt_target.h"
#include "bt_types.h"
#endif

/* Error codes */
#define GKI_SUCCESS         0x00
#define GKI_FAILURE         0x01
#define GKI_INVALID_TASK    0xF0
#define GKI_INVALID_POOL    0xFF


/************************************************************************
** Mailbox definitions. Each task has 4 mailboxes that are used to
** send buffers to the task.
*/
#define TASK_MBOX_0    0
#define TASK_MBOX_1    1
#define TASK_MBOX_2    2
#define TASK_MBOX_3    3

#ifndef NUM_TASK_MBOX
#define NUM_TASK_MBOX  4
#endif


/************************************************************************
** Event definitions.
**
** There are 4 reserved events used to signal messages rcvd in task mailboxes.
** There are 4 reserved events used to signal timeout events.
** There are 8 general purpose events available for applications.
*/
#define MAX_EVENTS              16

#define TASK_MBOX_0_EVT_MASK   0x0001
#define TASK_MBOX_1_EVT_MASK   0x0002
#define TASK_MBOX_2_EVT_MASK   0x0004
#define TASK_MBOX_3_EVT_MASK   0x0008


#define TIMER_0             0
#define TIMER_1             1
#define TIMER_2             2
#define TIMER_3             3

#define TIMER_0_EVT_MASK    0x0010
#define TIMER_1_EVT_MASK    0x0020
#define TIMER_2_EVT_MASK    0x0040
#define TIMER_3_EVT_MASK    0x0080

#define APPL_EVT_0          8
#define APPL_EVT_1          9
#define APPL_EVT_2          10
#define APPL_EVT_3          11
#define APPL_EVT_4          12
#define APPL_EVT_5          13
#define APPL_EVT_6          14
#define APPL_EVT_7          15

#define EVENT_MASK(evt)       ((UINT16)(0x0001 << (evt)))

/************************************************************************
**  Max Time Queue
**/
#ifndef GKI_MAX_TIMER_QUEUES
#define GKI_MAX_TIMER_QUEUES    3
#endif

/************************************************************************
**  Macro to determine the pool buffer size based on the GKI POOL ID at compile time.
**  Pool IDs index from 0 to GKI_NUM_FIXED_BUF_POOLS - 1
*/
#if (defined(GKI_DYNAMIC_POOL_CFG) && (GKI_DYNAMIC_POOL_CFG == TRUE))
#define GKI_POOL_ID_0                0
#define GKI_POOL_ID_1                1
#define GKI_POOL_ID_2                2
#define GKI_POOL_ID_3                3
#define GKI_POOL_ID_4                4
#define GKI_POOL_ID_5                5
#define GKI_POOL_ID_6                6
#define GKI_POOL_ID_7                7
#define GKI_POOL_ID_8                8
#define GKI_POOL_ID_9                9
#define GKI_POOL_ID_10               10
#define GKI_POOL_ID_11               11
#define GKI_POOL_ID_12               12
#define GKI_POOL_ID_13               13
#define GKI_POOL_ID_14               14
#define GKI_POOL_ID_15               15
#else /* GKI_DYNAMIC_POOL_CFG */

#if (GKI_NUM_FIXED_BUF_POOLS < 1)

#ifndef GKI_POOL_ID_0
#define GKI_POOL_ID_0                0
#endif /* ifndef GKI_POOL_ID_0 */

#ifndef GKI_BUF0_SIZE
#define GKI_BUF0_SIZE                0
#endif /* ifndef GKI_BUF0_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 1 */


#if (GKI_NUM_FIXED_BUF_POOLS < 2)

#ifndef GKI_POOL_ID_1
#define GKI_POOL_ID_1                0
#endif /* ifndef GKI_POOL_ID_1 */

#ifndef GKI_BUF1_SIZE
#define GKI_BUF1_SIZE                0
#endif /* ifndef GKI_BUF1_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 2 */


#if (GKI_NUM_FIXED_BUF_POOLS < 3)

#ifndef GKI_POOL_ID_2
#define GKI_POOL_ID_2                0
#endif /* ifndef GKI_POOL_ID_2 */

#ifndef GKI_BUF2_SIZE
#define GKI_BUF2_SIZE                0
#endif /* ifndef GKI_BUF2_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 3 */


#if (GKI_NUM_FIXED_BUF_POOLS < 4)

#ifndef GKI_POOL_ID_3
#define GKI_POOL_ID_3                0
#endif /* ifndef GKI_POOL_ID_4 */

#ifndef GKI_BUF3_SIZE
#define GKI_BUF3_SIZE                0
#endif /* ifndef GKI_BUF3_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 4 */


#if (GKI_NUM_FIXED_BUF_POOLS < 5)

#ifndef GKI_POOL_ID_4
#define GKI_POOL_ID_4                0
#endif /* ifndef GKI_POOL_ID_4 */

#ifndef GKI_BUF4_SIZE
#define GKI_BUF4_SIZE                0
#endif /* ifndef GKI_BUF4_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 5 */


#if (GKI_NUM_FIXED_BUF_POOLS < 6)

#ifndef GKI_POOL_ID_5
#define GKI_POOL_ID_5                0
#endif /* ifndef GKI_POOL_ID_5 */

#ifndef GKI_BUF5_SIZE
#define GKI_BUF5_SIZE                0
#endif /* ifndef GKI_BUF5_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 6 */


#if (GKI_NUM_FIXED_BUF_POOLS < 7)

#ifndef GKI_POOL_ID_6
#define GKI_POOL_ID_6                0
#endif /* ifndef GKI_POOL_ID_6 */

#ifndef GKI_BUF6_SIZE
#define GKI_BUF6_SIZE                0
#endif /* ifndef GKI_BUF6_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 7 */


#if (GKI_NUM_FIXED_BUF_POOLS < 8)

#ifndef GKI_POOL_ID_7
#define GKI_POOL_ID_7                0
#endif /* ifndef GKI_POOL_ID_7 */

#ifndef GKI_BUF7_SIZE
#define GKI_BUF7_SIZE                0
#endif /* ifndef GKI_BUF7_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 8 */


#if (GKI_NUM_FIXED_BUF_POOLS < 9)

#ifndef GKI_POOL_ID_8
#define GKI_POOL_ID_8                0
#endif /* ifndef GKI_POOL_ID_8 */

#ifndef GKI_BUF8_SIZE
#define GKI_BUF8_SIZE                0
#endif /* ifndef GKI_BUF8_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 9 */


#if (GKI_NUM_FIXED_BUF_POOLS < 10)

#ifndef GKI_POOL_ID_9
#define GKI_POOL_ID_9                0
#endif /* ifndef GKI_POOL_ID_9 */

#ifndef GKI_BUF9_SIZE
#define GKI_BUF9_SIZE                0
#endif /* ifndef GKI_BUF9_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 10 */


#if (GKI_NUM_FIXED_BUF_POOLS < 11)

#ifndef GKI_POOL_ID_10
#define GKI_POOL_ID_10                0
#endif /* ifndef GKI_POOL_ID_10 */

#ifndef GKI_BUF10_SIZE
#define GKI_BUF10_SIZE                0
#endif /* ifndef GKI_BUF10_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 11 */


#if (GKI_NUM_FIXED_BUF_POOLS < 12)

#ifndef GKI_POOL_ID_11
#define GKI_POOL_ID_11                0
#endif /* ifndef GKI_POOL_ID_11 */

#ifndef GKI_BUF11_SIZE
#define GKI_BUF11_SIZE                0
#endif /* ifndef GKI_BUF11_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 12 */


#if (GKI_NUM_FIXED_BUF_POOLS < 13)

#ifndef GKI_POOL_ID_12
#define GKI_POOL_ID_12                0
#endif /* ifndef GKI_POOL_ID_12 */

#ifndef GKI_BUF12_SIZE
#define GKI_BUF12_SIZE                0
#endif /* ifndef GKI_BUF12_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 13 */


#if (GKI_NUM_FIXED_BUF_POOLS < 14)

#ifndef GKI_POOL_ID_13
#define GKI_POOL_ID_13                0
#endif /* ifndef GKI_POOL_ID_13 */

#ifndef GKI_BUF13_SIZE
#define GKI_BUF13_SIZE                0
#endif /* ifndef GKI_BUF13_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 14 */


#if (GKI_NUM_FIXED_BUF_POOLS < 15)

#ifndef GKI_POOL_ID_14
#define GKI_POOL_ID_14                0
#endif /* ifndef GKI_POOL_ID_14 */

#ifndef GKI_BUF14_SIZE
#define GKI_BUF14_SIZE                0
#endif /* ifndef GKI_BUF14_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 15 */


#if (GKI_NUM_FIXED_BUF_POOLS < 16)

#ifndef GKI_POOL_ID_15
#define GKI_POOL_ID_15                0
#endif /* ifndef GKI_POOL_ID_15 */

#ifndef GKI_BUF15_SIZE
#define GKI_BUF15_SIZE                0
#endif /* ifndef GKI_BUF15_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 16 */
#endif /*GKI_DYNAMIC_POOL_CFG */

/* Timer list entry callback type
*/
//typedef void (TIMER_CBACK)(void *p_tle);

/* Define a timer list entry
*/
typedef struct _tle {
    struct _tle  *p_next;
    struct _tle  *p_prev;
    TIMER_CBACK  *p_cback;
    INT32         ticks;
    TIMER_PARAM_TYPE   param;
    UINT16        event;
    UINT8         in_use;
} TIMER_LIST_ENT;

/* Define a timer list queue
*/
typedef struct {
    TIMER_LIST_ENT   *p_first;
    TIMER_LIST_ENT   *p_last;
    INT32             last_ticks;
} TIMER_LIST_Q;


/***********************************************************************
** This queue is a general purpose buffer queue, for application use.
*/
typedef struct {
    void    *p_first;
    void    *p_last;
    UINT16   count;
} BUFFER_Q;

#define GKI_IS_QUEUE_EMPTY(p_q) ((p_q)->count == 0)

/* Task constants
*/
#ifndef TASKPTR
typedef void (*TASKPTR)(UINT32);
#endif


#define GKI_PUBLIC_POOL         0       /* General pool accessible to GKI_getbuf() */
#define GKI_RESTRICTED_POOL     1       /* Inaccessible pool to GKI_getbuf() */

/***********************************************************************
** Mutex defintions
*/
typedef struct {
    void *p_platform_mutex;
} GKI_MUTEX;

/***********************************************************************
** Function prototypes
*/

#ifdef __cplusplus
extern "C" {
#endif

/* Task management
*/
GKI_API extern UINT8   GKI_create_task (TASKPTR, UINT8, INT8 *, UINT16 *, UINT16);
GKI_API extern void    GKI_exit_task(UINT8);
GKI_API extern UINT8   GKI_get_taskid(void);
GKI_API extern void    GKI_init(void);
GKI_API extern INT8   *GKI_map_taskname(UINT8);
GKI_API extern UINT8   GKI_resume_task(UINT8);
GKI_API extern void    GKI_run(void *);
GKI_API extern void    GKI_stop(void);
GKI_API extern UINT8   GKI_suspend_task(UINT8);
GKI_API extern UINT8   GKI_is_task_running(UINT8);

/* memory management
*/
GKI_API extern void GKI_shiftdown (UINT8 *p_mem, UINT32 len, UINT32 shift_amount);
GKI_API extern void GKI_shiftup (UINT8 *p_dest, UINT8 *p_src, UINT32 len);

/* To send buffers and events between tasks
*/
GKI_API extern UINT8   GKI_isend_event (UINT8, UINT16);
GKI_API extern void    GKI_isend_msg (UINT8, UINT8, void *);
GKI_API extern void   *GKI_read_mbox  (UINT8);
GKI_API extern void    GKI_send_msg   (UINT8, UINT8, void *);
GKI_API extern UINT8   GKI_send_event (UINT8, UINT16);


/* To get and release buffers, change owner and get size
*/
GKI_API extern void    GKI_change_buf_owner (void *, UINT8);
GKI_API extern UINT8   GKI_create_pool (UINT16, UINT16, UINT8, void *);
GKI_API extern void    GKI_delete_pool (UINT8);
GKI_API extern void   *GKI_find_buf_start (void *);
GKI_API extern void    GKI_freebuf (void *);
GKI_API extern void   *GKI_getbuf (UINT16);
GKI_API extern UINT16  GKI_get_buf_size (void *);
GKI_API extern void   *GKI_getpoolbuf (UINT8);
GKI_API extern UINT16  GKI_poolcount (UINT8);
GKI_API extern UINT16  GKI_poolfreecount (UINT8);
GKI_API extern UINT16  GKI_poolutilization (UINT8);
GKI_API extern void    GKI_register_mempool (void *p_mem);
GKI_API extern UINT8   GKI_set_pool_permission(UINT8, UINT8);


/* User buffer queue management
*/
GKI_API extern void   *GKI_dequeue  (BUFFER_Q *);
GKI_API extern void    GKI_enqueue (BUFFER_Q *, void *);
GKI_API extern void    GKI_enqueue_head (BUFFER_Q *, void *);
GKI_API extern void   *GKI_getfirst (BUFFER_Q *);
GKI_API extern void   *GKI_getlast (BUFFER_Q *);
GKI_API extern void   *GKI_getnext (void *);
GKI_API extern void    GKI_init_q (BUFFER_Q *);
GKI_API extern BOOLEAN GKI_queue_is_empty(BUFFER_Q *);
GKI_API extern void   *GKI_remove_from_queue (BUFFER_Q *, void *);
GKI_API extern UINT16  GKI_get_pool_bufsize (UINT8);

/* Timer management
*/
GKI_API extern void    GKI_add_to_timer_list (TIMER_LIST_Q *, TIMER_LIST_ENT *);
GKI_API extern void    GKI_delay(UINT32);
GKI_API extern UINT32  GKI_get_tick_count(void);
GKI_API extern INT8   *GKI_get_time_stamp(INT8 *);
GKI_API extern void    GKI_init_timer_list (TIMER_LIST_Q *);
GKI_API extern void    GKI_init_timer_list_entry (TIMER_LIST_ENT *);
GKI_API extern INT32   GKI_ready_to_sleep (void);
GKI_API extern void    GKI_remove_from_timer_list (TIMER_LIST_Q *, TIMER_LIST_ENT *);
GKI_API extern void    GKI_start_timer(UINT8, INT32, BOOLEAN);
GKI_API extern void    GKI_stop_timer (UINT8);
GKI_API extern void    GKI_timer_update(INT32);
GKI_API extern UINT16  GKI_update_timer_list (TIMER_LIST_Q *, INT32);
GKI_API extern UINT32  GKI_get_remaining_ticks (TIMER_LIST_Q *, TIMER_LIST_ENT *);
GKI_API extern UINT16  GKI_wait(UINT16, UINT32);

/* Start and Stop system time tick callback
 * true for start system tick if time queue is not empty
 * false to stop system tick if time queue is empty
*/
typedef void (SYSTEM_TICK_CBACK)(BOOLEAN);

/* Time queue management for system ticks
*/
GKI_API extern BOOLEAN GKI_timer_queue_empty (void);
GKI_API extern void    GKI_timer_queue_register_callback(SYSTEM_TICK_CBACK *);

/* Disable Interrupts, Enable Interrupts
*/
GKI_API extern void    GKI_enable(void);
GKI_API extern void    GKI_disable(void);
GKI_API extern void    GKI_sched_lock(void);
GKI_API extern void    GKI_sched_unlock(void);

/* Allocate (Free) memory from an OS
*/
GKI_API extern void     *GKI_os_malloc (UINT32);
GKI_API extern void      GKI_os_free (void *);

/* os timer operation */
GKI_API extern UINT32 GKI_get_os_tick_count(void);

/* Exception handling
*/
GKI_API extern void    GKI_exception (UINT16, char *);

#if GKI_DEBUG == TRUE
GKI_API extern void    GKI_PrintBufferUsage(UINT8 *p_num_pools, UINT16 *p_cur_used);
GKI_API extern void    GKI_PrintBuffer(void);
GKI_API extern void    GKI_print_task(void);
#else
#undef GKI_PrintBufferUsage
#define GKI_PrintBuffer() NULL
#endif


/* Mutex APIs
*/
GKI_API extern BOOLEAN GKI_mutex_create(GKI_MUTEX *p_mutex);
GKI_API extern void GKI_mutex_delete(GKI_MUTEX *p_mutex);
GKI_API extern BOOLEAN GKI_mutex_lock(GKI_MUTEX *p_mutex);
GKI_API extern BOOLEAN GKI_mutex_unlock(GKI_MUTEX *p_mutex);

#ifdef __cplusplus
}
#endif


#endif


/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef __AOS_OSDEP_SERVICE_H_
#define __AOS_OSDEP_SERVICE_H_

#include "dlist.h"
#include <aos/aos.h>
#include <k_api.h>
// --------------------------------------------
//	Platform dependent include file
// --------------------------------------------
#if defined(CONFIG_PLATFORM_8195A)
#include "platform/platform_stdlib.h"
extern VOID RtlUdelayOS(u32 us);
#elif defined(CONFIG_PLATFORM_8711B)
#include "platform/platform_stdlib.h"
#else
// other MCU may use standard library 
#include <string.h>
#endif


#define configTICK_RATE_HZ				( ( uint32_t ) 1000 )

#define DBG_ERR(fmt, args...)		DBG_8195A("\n\r[%s] " fmt, __FUNCTION__, ## args)
#if WLAN_INTF_DBG
#define DBG_TRACE(fmt, args...)		DBG_8195A("\n\r[%s] " fmt, __FUNCTION__, ## args)
#define DBG_INFO(fmt, args...)		DBG_8195A("\n\r[%s] " fmt, __FUNCTION__, ## args)
#else
#define DBG_TRACE(fmt, args...)
#define DBG_INFO(fmt, args...)
#endif

#define pvPortMalloc   _aos_zmalloc
#define vPortFree           _aos_mfree

/*
 * atomic_read - read atomic variable
 * @v: pointer of type atomic_t
 *
 * Atomically reads the value of @v.  Note that the guaranteed
 * useful range of an atomic_t is only 24 bits.
 */
#undef atomic_read
#define atomic_read(v)  ((v)->counter)

/*
 * atomic_set - set atomic variable
 * @v: pointer of type atomic_t
 * @i: required value
 *
 * Atomically sets the value of @v to @i.  Note that the guaranteed
 * useful range of an atomic_t is only 24 bits.
 */
#undef atomic_set
#define atomic_set(v,i) ((v)->counter = (i))


#define HALT()				do { cli(); for(;;);} while(0)
#undef ASSERT
#define ASSERT(x)			do { \
						if((x) == 0){\
							DBG_8195A("\n\rAssert(" #x ") failed on line %d in file %s", __LINE__, __FILE__); \
						HALT();}\
					} while(0)

#undef DBG_ASSERT
#define DBG_ASSERT(x, msg)		do { \
						if((x) == 0) \
							DBG_8195A("\n\r%s, Assert(" #x ") failed on line %d in file %s", msg, __LINE__, __FILE__); \
					} while(0)



// os types
typedef char			osdepCHAR;
typedef float			osdepFLOAT;
typedef double			osdepDOUBLE;
typedef long			osdepLONG;
typedef short			osdepSHORT;
typedef unsigned long	osdepSTACK_TYPE;
typedef long			osdepBASE_TYPE;
typedef unsigned long	osdepTickType;



struct timerHandle{
    aos_timer_t	timer;
    char* q_buf;
};

typedef struct timerHandle* _timerHandle;
typedef aos_sem_t	_sema;
typedef aos_mutex_t	_mutex;
typedef aos_mutex_t	_lock;
typedef aos_queue_t	_queueHandle;
typedef kbuf_queue_t	_xqueue;
#if 0
typedef struct{
    aos_queue_t	queue;
    char* q_buf;
}_xqueue;
#endif
typedef struct timer_list	_timer;

typedef	struct sk_buff		_pkt;
typedef unsigned char		_buffer;

struct	__queue	{
	struct	list_head	queue;
	_lock			lock;
};

typedef struct	__queue		_queue;
typedef struct	list_head	_list;
typedef u32		_irqL;

#define ATOMIC_T atomic_t

#define DBG_ERR(fmt, args...)		DBG_8195A("\n\r[%s] " fmt, __FUNCTION__, ## args)

#if CONFIG_PLATFORM_8711B
extern u32 random_seed;
#endif
#endif



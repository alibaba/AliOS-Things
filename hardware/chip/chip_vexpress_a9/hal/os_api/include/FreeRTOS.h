#ifndef AOS_FREERTOS_H
#define AOS_FREERTOS_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include "k_api.h"
#include "freertos_to_aos.h"

#ifndef configUSE_QUEUE_SETS
	#define configUSE_QUEUE_SETS 0
#endif

#ifndef configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES
    #define configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES 0
#endif

/* See the comments above the struct xSTATIC_LIST_ITEM definition. */
struct xSTATIC_MINI_LIST_ITEM
{
#if( configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES == 1 )
    TickType_t xDummy1;
#endif
    TickType_t xDummy2;
    void *pvDummy3[ 2 ];
};
typedef struct xSTATIC_MINI_LIST_ITEM StaticMiniListItem_t;

/* See the comments above the struct xSTATIC_LIST_ITEM definition. */
typedef struct xSTATIC_LIST
{
#if( configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES == 1 )
    TickType_t xDummy1;
#endif
    UBaseType_t uxDummy2;
    void *pvDummy3;
    StaticMiniListItem_t xDummy4;
#if( configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES == 1 )
    TickType_t xDummy5;
#endif
} StaticList_t;

typedef struct xSTATIC_QUEUE
{
    void *pvDummy1[ 3 ];

    union
    {
        void *pvDummy2;
        UBaseType_t uxDummy2;
    } u;

    StaticList_t xDummy3[ 2 ];
    UBaseType_t uxDummy4[ 3 ];
    uint8_t ucDummy5[ 2 ];

#if( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
    uint8_t ucDummy6;
#endif

#if ( configUSE_QUEUE_SETS == 1 )
    void *pvDummy7;
#endif

#if ( configUSE_TRACE_FACILITY == 1 )
    UBaseType_t uxDummy8;
    uint8_t ucDummy9;
#endif

} StaticQueue_t;
typedef StaticQueue_t StaticSemaphore_t;

typedef struct xSTATIC_EVENT_GROUP
{
    TickType_t xDummy1;
    StaticList_t xDummy2;

#if( configUSE_TRACE_FACILITY == 1 )
    UBaseType_t uxDummy3;
#endif

#if( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
    uint8_t ucDummy4;
#endif

} StaticEventGroup_t;


#endif


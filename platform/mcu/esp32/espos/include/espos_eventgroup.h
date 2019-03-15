#ifndef _ESPOS_EVENTGROUP_H_
#define _ESPOS_EVENTGROUP_H_

#include "espos_types.h"
#include "espos_errno.h"
#include "espos_time.h"

#ifdef __cplusplus
extern "C"
{
#endif


typedef void * EventGroupHandle_t;
typedef uint32_t EventBits_t;


EventGroupHandle_t espos_event_group_create();
void espos_event_group_delete(EventGroupHandle_t xEventGroup);
EventBits_t espos_event_group_wait_bits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToWaitFor, const uint32_t xClearOnExit, const uint32_t xWaitForAllBits, uint32_t xTicksToWait);
EventBits_t espos_event_group_set_bits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet);
EventBits_t espos_event_group_clear_bits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear);


#ifdef __cplusplus
}
#endif

#endif

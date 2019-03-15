#include "k_api.h"
#include "espos_eventgroup.h"

EventGroupHandle_t espos_event_group_create()
{
    kevent_t *event_obj;

	(void)krhino_event_dyn_create(&event_obj, "espos_event", 0);

    return (EventGroupHandle_t)event_obj;
}

void espos_event_group_delete(EventGroupHandle_t xEventGroup)
{
    krhino_event_del(xEventGroup);
}

EventBits_t espos_event_group_wait_bits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToWaitFor, const uint32_t xClearOnExit, const uint32_t xWaitForAllBits, uint32_t xTicksToWait)
{
	uint32_t actl_flags;
	krhino_event_get(xEventGroup, uxBitsToWaitFor, (xWaitForAllBits << 1u) | xClearOnExit, &actl_flags, xTicksToWait);
	return (EventBits_t)actl_flags;
}

EventBits_t espos_event_group_set_bits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet)
{
	krhino_event_set(xEventGroup, uxBitsToSet, RHINO_OR);

	return ((kevent_t *)xEventGroup)->flags;

}
EventBits_t espos_event_group_clear_bits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear)
{
	EventBits_t val = ((kevent_t *)xEventGroup)->flags;
	krhino_event_set(xEventGroup, ~( uxBitsToClear), RHINO_AND);
	
	return val;
}


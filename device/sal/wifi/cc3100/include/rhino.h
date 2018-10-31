#ifndef INC_RHINO_H
#define INC_RHINO_H

typedef uint64_t  portTickType;
//#define eTaskStateGet eTaskGetState
//#define portTickType rhino_sys_time_t
//#define xTaskHandle TaskHandle_t
//#define xQueueHandle QueueHandle_t
//#define xSemaphoreHandle SemaphoreHandle_t
//#define xQueueSetHandle QueueSetHandle_t
//#define xQueueSetMemberHandle QueueSetMemberHandle_t
//#define xTimeOutType TimeOut_t
//#define xMemoryRegion MemoryRegion_t
//#define xTaskParameters TaskParameters_t
//#define xTaskStatusType TaskStatus_t
//#define xTimerHandle TimerHandle_t
//#define xCoRoutineHandle CoRoutineHandle_t
//#define pdTASK_HOOK_CODE TaskHookFunction_t
#define portTICK_RATE_MS (1000/RHINO_CONFIG_TICKS_PER_SECOND)
//#define pcTaskGetTaskName pcTaskGetName
//#define pcTimerGetTimerName pcTimerGetName
//#define pcQueueGetQueueName pcQueueGetName
//#define vTaskGetTaskInfo vTaskGetInfo

/* Backward compatibility within the scheduler code only - these definitions
are not really required but are included for completeness. */
//#define tmrTIMER_CALLBACK TimerCallbackFunction_t
//#define pdTASK_CODE TaskFunction_t
//#define xListItem ListItem_t
//#define xList List_t

#endif

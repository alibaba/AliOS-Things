#ifndef _WRAPPERS_OS_H_
#define _WRAPPERS_OS_H_
#include <stdarg.h>
#include "linkkit/infra/infra_types.h"
#include "linkkit/infra/infra_defs.h"


#define HAL_Printf printf

void *HAL_Malloc(uint32_t size);

void HAL_Free(void *ptr);

uint64_t HAL_UptimeMs(void);

void HAL_SleepMs(uint32_t ms);

void HAL_Srandom(uint32_t seed);

uint32_t HAL_Random(uint32_t region);

int HAL_Snprintf(char *str, const int len, const char *fmt, ...);

int HAL_Vsnprintf(char *str, const int len, const char *format, va_list ap);

void *HAL_SemaphoreCreate(void);

void HAL_SemaphoreDestroy(void *sem);

void HAL_SemaphorePost(void *sem);

int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms);

int HAL_ThreadCreate(
            void **thread_handle,
            void *(*work_routine)(void *),
            void *arg,
            hal_os_thread_param_t *hal_os_thread_param,
            int *stack_used);

void *HAL_MutexCreate(void);

void HAL_MutexDestroy(void *mutex);

void HAL_MutexLock(void *mutex);

void HAL_MutexUnlock(void *mutex);

void *HAL_Timer_Create(const char *name, void (*func)(void *), void *user_data);

void HAL_Reboot(void);

int HAL_Timer_Start(void *timer, int ms);

int HAL_Timer_Stop(void *timer);

int HAL_Timer_Delete(void *timer);

int HAL_SetProductKey(char *product_key);

int HAL_SetDeviceName(char *device_name);

int HAL_SetProductSecret(char *product_secret);

int HAL_SetDeviceSecret(char *device_secret);

int HAL_GetProductKey(char product_key[IOTX_PRODUCT_KEY_LEN + 1]);

int HAL_GetProductSecret(char product_secret[IOTX_PRODUCT_SECRET_LEN + 1]);

int HAL_GetDeviceName(char device_name[IOTX_DEVICE_NAME_LEN + 1]);

int HAL_GetDeviceSecret(char device_secret[IOTX_DEVICE_SECRET_LEN + 1]);

int HAL_GetFirmwareVersion(char *version);

void HAL_Firmware_Persistence_Start(void);

int HAL_Firmware_Persistence_Write(char *buffer, uint32_t length);

int HAL_Firmware_Persistence_Stop(void);

#endif


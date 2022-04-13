#ifndef __AIOT_DAS_API_H__
#define __AIOT_DAS_API_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

#define STATE_DAS_BASE                                             (-0x0100)
#define STATE_DAS_INIT_ERROR                                       (STATE_DAS_BASE-1)
#define STATE_DAS_SUB_TOPIC_ERROR                                  (STATE_DAS_BASE-2)
#define STATE_DAS_SET_MQTT_HANDLE_ERROR                            (STATE_DAS_BASE-3)
#define STATE_DAS_STEPPING_ERROR                                   (STATE_DAS_BASE-4)
#define STATE_DAS_SET_FIRVER_ERROR                                 (STATE_DAS_BASE-5)

typedef enum {
    AIOT_DASOPT_MQTT_HANDLE,
    AIOT_DASOPT_SET_FIRMWARE_VERSION,
    AIOT_DASOPT_MAX,
} aiot_das_option_t;

void *aiot_das_init(void);

int32_t aiot_das_deinit(void **handle);

int32_t aiot_das_setopt(void *handle, aiot_das_option_t option, void *data);

int32_t aiot_das_stepping(void *handle);

#if defined(__cplusplus)
}
#endif

#endif  /* __AIOT_DAS_API_H__ */


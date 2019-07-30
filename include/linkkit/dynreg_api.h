#ifndef _DYNREG_API_H_
#define _DYNREG_API_H_

#include "linkkit/infra/infra_types.h"
#include "linkkit/infra/infra_defs.h"

#define MQTT_DYNREG_TIMEOUT_MS (15 * 1000)

int32_t IOT_Dynamic_Register(iotx_http_region_types_t region, iotx_dev_meta_info_t *meta);

#endif


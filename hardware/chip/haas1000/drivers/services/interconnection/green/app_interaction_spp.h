#ifndef __APP_INTERACTION_SPP_H__
#define __APP_INTERACTION_SPP_H__

#include "spp_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*app_spp_tx_done_t)(void);

//add new function
void green_spp_server_open(void);
void app_interaction_send_cmd_via_spp(uint8_t* ptrData, uint32_t length);

void green_spp_register_tx_done(app_spp_tx_done_t callback);


#ifdef __cplusplus
}
#endif

#endif


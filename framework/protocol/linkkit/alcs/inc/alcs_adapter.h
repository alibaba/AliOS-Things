#ifndef __ALCS_ADAPTER_H__
#define __ALCS_ADAPTER_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include "iot_export_mqtt.h"
#include "CoAPExport.h"

typedef struct iotx_alcs_send_msg_st{
	uint8_t token_len;
	uint8_t *token;
	char *uri;
} iotx_alcs_send_msg_t, *iotx_alcs_send_msg_pt;

typedef void (*iotx_alcs_auth_timer_fnuc_t)(CoAPContext *);

typedef struct iotx_alcs_adapter_st{
	CoAPContext *coap_ctx;
	uint8_t role;
	iotx_alcs_auth_timer_fnuc_t alcs_client_auth_timer_func;
	iotx_alcs_auth_timer_fnuc_t alcs_server_auth_timer_func;
	iotx_alcs_event_handle_t *alcs_event_handle;
	linked_list_t *alcs_send_list;
	char local_ip[NETWORK_ADDR_LEN];
	uint16_t local_port;
} iotx_alcs_adapter_t, *iotx_alcs_adapter_pt;

void iotx_alcs_send_list_handle(void* list_node, va_list* params);
int iotx_alcs_adapter_list_init(iotx_alcs_adapter_t *adapter);

#if defined(__cplusplus)
}
#endif
#endif

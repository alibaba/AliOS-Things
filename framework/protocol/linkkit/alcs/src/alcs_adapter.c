#include <stddef.h>
#include <string.h>

#include "iot_import.h"
#include "iot_export.h"
#include "sdk-impl_internal.h"
#include "linked_list.h"
#include "iot_export_alcs.h"
#include "alcs_api.h"
#include "alcs_coap.h"
#include "alcs_mqtt.h"
#include "CoAPInternal.h"
#include "CoAPExport.h"
#include "CoAPServer.h"
#include "iot_export_alcs.h"
#include "alcs_adapter.h"

static iotx_alcs_adapter_t g_alcs_adapter;

extern void on_client_auth_timer(CoAPContext *);
extern void on_svr_auth_timer(CoAPContext *);

#if  defined(COAP_WITH_YLOOP) 
static int alcs_inited=0;
static void alcs_repeat_action(void *handle);
#endif
static void alcs_heartbeat(void *handle);
static iotx_alcs_adapter_t* __iotx_alcs_get_ctx(void)
{
	return &g_alcs_adapter;
}

int iotx_alcs_coap_ack_send(CoAPContext *context, NetworkAddr *remote, unsigned short msgid)
{   int ret   = COAP_SUCCESS;
    CoAPMessage message;
    CoAPIntContext *ctx =  (CoAPIntContext *)context;

    CoAPMessage_init(&message);
    CoAPMessageId_set(&message, msgid);
    log_info("Send Ack Response Message: %d",msgid);
    ret = CoAPMessage_send(ctx, remote, &message);
    CoAPMessage_destory(&message);
    return ret;
}

void iotx_alcs_coap_adapter_send_msg_handle(CoAPContext *context,
											CoAPReqResult result,
											void *userdata,
											NetworkAddr *remote,
											CoAPMessage *message)
{
	iotx_alcs_adapter_t *adapter = (iotx_alcs_adapter_t *)userdata;
	iotx_alcs_event_msg_t event;
	memset(&event,0,sizeof(iotx_alcs_event_msg_t));
		
	switch (result)
	{
		case COAP_REQUEST_SUCCESS:
			{
				iotx_alcs_transfer_msg_t transfer_msg;
				iotx_alcs_send_msg_t *send_msg = NULL;
				
				memset(&transfer_msg,0,sizeof(iotx_alcs_transfer_msg_t));
				
				transfer_msg.ip = (char *)remote->addr;
				transfer_msg.port = remote->port;
				linked_list_iterator(adapter->alcs_send_list,
										iotx_alcs_send_list_handle,
										&send_msg,
										message->token);
				if (send_msg == NULL)
					return;

				transfer_msg.uri = send_msg->uri;
				transfer_msg.token_len = send_msg->token_len;
				transfer_msg.token = send_msg->token;
				transfer_msg.payload_len = message->payloadlen;
				transfer_msg.payload = message->payload;
				
				event.event_type = IOTX_ALCS_EVENT_MSG_SEND_MESSAGE_SUCCESS;
				event.msg = &transfer_msg;

				adapter->alcs_event_handle->h_fp(adapter->alcs_event_handle->pcontext,(void *)adapter,&event);

				linked_list_remove(adapter->alcs_send_list,send_msg);
				LITE_free(send_msg->token);
				LITE_free(send_msg->uri);
				LITE_free(send_msg);
			}
			break;
		case COAP_RECV_RESP_TIMEOUT:
			{
				iotx_alcs_transfer_msg_t transfer_msg;
				iotx_alcs_send_msg_t *send_msg = NULL;
				
				memset(&transfer_msg,0,sizeof(iotx_alcs_transfer_msg_t));

				transfer_msg.ip = (char *)remote->addr;
				transfer_msg.port = remote->port;
				linked_list_iterator(adapter->alcs_send_list,
										iotx_alcs_send_list_handle,
										&send_msg,
										message->token);

				if (send_msg == NULL)
					return;
				
				transfer_msg.uri = send_msg->uri;
				transfer_msg.token_len = send_msg->token_len;
				transfer_msg.token = send_msg->token;
				transfer_msg.payload_len = 0;
				transfer_msg.payload = NULL;
				
				event.event_type = IOTX_ALCS_EVENT_MSG_SEND_MESSAGE_RESP_TIMEOUT;
				event.msg = &transfer_msg;

				adapter->alcs_event_handle->h_fp(adapter->alcs_event_handle->pcontext,(void *)adapter,&event);

				linked_list_remove(adapter->alcs_send_list,send_msg);
				LITE_free(send_msg->token);
				LITE_free(send_msg->uri);
				LITE_free(send_msg);
			}
			break;
		default:
			log_warning("Unknown Coap Request Result: %d",result);
			break;
	}
}

void iotx_alcs_coap_adapter_recv_msg_handle(CoAPContext *context, const char *paths, NetworkAddr *remote, CoAPMessage *message)
{
	iotx_alcs_adapter_t *adapter = __iotx_alcs_get_ctx();
	iotx_alcs_transfer_msg_t transfer_msg;
	iotx_alcs_event_msg_t event;

	memset(&transfer_msg,0,sizeof(iotx_alcs_transfer_msg_t));
	memset(&event,0,sizeof(iotx_alcs_event_msg_t));
	
	transfer_msg.ip = (char *)remote->addr;
	transfer_msg.port = remote->port;
	transfer_msg.uri = (char *)paths;
	transfer_msg.token_len = message->header.tokenlen;
	transfer_msg.token = message->token;
	transfer_msg.payload_len = message->payloadlen;
	transfer_msg.payload = message->payload;

	event.event_type = IOTX_ALCS_EVENT_MSG_RECV_MESSAGE;
	event.msg = (void *)&transfer_msg;

	iotx_alcs_coap_ack_send(context,remote,message->header.msgid);
		
	adapter->alcs_event_handle->h_fp(adapter->alcs_event_handle->pcontext,(void *)adapter,&event);
}

void iotx_alcs_coap_adapter_event_notifier(unsigned int event, NetworkAddr *remote, void *message)
{
	log_info("ALCS Coap Event: %d, Remote Device Address: %s, Remote Device Port: %d",
				event,remote->addr,remote->port);	
}

void iotx_alcs_send_list_handle(void* list_node, va_list* params)
{
	iotx_alcs_send_msg_t *send_msg = (iotx_alcs_send_msg_t *)list_node;

	iotx_alcs_send_msg_t **match_msg = NULL;
    unsigned char *token = NULL;

	match_msg = va_arg(*params,iotx_alcs_send_msg_t **);
    token = va_arg(*params, unsigned char *);

    if (memcmp(send_msg->token, token, send_msg->token_len) == 0) {
		*match_msg = send_msg;
	}
}

int iotx_alcs_adapter_list_init(iotx_alcs_adapter_t *adapter)
{
	//initialze send list
	adapter->alcs_send_list = linked_list_create("alcs send list",1);

	return adapter->alcs_send_list ? SUCCESS_RETURN : FAIL_RETURN;
}

int iotx_alcs_adapter_deinit(void)
{
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};
	iotx_alcs_adapter_t *adapter = __iotx_alcs_get_ctx();

    HAL_GetProductKey(product_key);
    HAL_GetDeviceName(device_name);

	if (adapter->alcs_send_list) linked_list_destroy(adapter->alcs_send_list);

    if (adapter->alcs_event_handle) LITE_free(adapter->alcs_event_handle);

    alcs_mqtt_deinit(adapter->coap_ctx, product_key, device_name);

	//if (adapter->coap_ctx) CoAPContext_free(adapter->coap_ctx);
#if  defined(COAP_WITH_YLOOP) 
    alcs_inited=0;
#endif
	alcs_context_deinit();
	alcs_deinit();
	alcs_auth_deinit();

	return SUCCESS_RETURN;
}

int iotx_alcs_adapter_init(iotx_alcs_adapter_t *adapter, iotx_alcs_param_t *param)
{
    log_info ("iotx_alcs_adapter_init");

	int res;
	CoAPInitParam coap_param;
	CoAPContext *coap_ctx = NULL;
	char product_key[PRODUCT_KEY_MAXLEN] = {0};
	char device_name[DEVICE_NAME_MAXLEN] = {0};
	
	memset(&coap_param,0,sizeof(CoAPInitParam));

	coap_param.send_maxcount = param->send_maxcount;
	coap_param.obs_maxcount = param->obs_maxcount;
	coap_param.port = param->port;
	coap_param.group = param->group;
	coap_param.waittime = param->waittime;
	coap_param.res_maxcount = param->res_maxcount;
	coap_param.appdata = NULL;
	coap_param.notifier = iotx_alcs_coap_adapter_event_notifier;

	coap_ctx = alcs_context_init(&coap_param);
	if (coap_ctx == NULL) {
		log_err("Coap Context Init Failed");
		return FAIL_RETURN;
	}
	adapter->coap_ctx = coap_ctx;

	res = HAL_GetProductKey(product_key);
	if (res <= 0 || res > PRODUCT_KEY_MAXLEN-1) {
		iotx_alcs_adapter_deinit();
		log_err("Get Product Key Failed");
		return FAIL_RETURN;
	}

	res = HAL_GetDeviceName(device_name);
	if (res <= 0 || res > DEVICE_NAME_MAXLEN-1)	{
		iotx_alcs_adapter_deinit();
		log_err("Get Device Name Failed");
		return FAIL_RETURN;
	}

	alcs_init();
	
	res = alcs_auth_init(coap_ctx,product_key,device_name,param->role);
	if (res != COAP_SUCCESS) {
		iotx_alcs_adapter_deinit();
		log_err("ALCS Auth Init Failed");
		return FAIL_RETURN;
	}
	adapter->role = param->role;
	
	if(adapter->role & IOTX_ALCS_ROLE_SERVER)
		adapter->alcs_server_auth_timer_func = on_svr_auth_timer;

	if (adapter->role & IOTX_ALCS_ROLE_CLIENT)
		adapter->alcs_client_auth_timer_func = on_client_auth_timer;

	adapter->alcs_event_handle = (iotx_alcs_event_handle_t *)LITE_malloc(sizeof(iotx_alcs_event_handle_t));
	if (adapter->alcs_event_handle == NULL) {
		iotx_alcs_adapter_deinit();
		log_err("ALCS Event Handle Init Failed");
		return FAIL_RETURN;
	}
	memcpy(adapter->alcs_event_handle,param->handle_event,sizeof(iotx_alcs_event_handle_t));

	if (iotx_alcs_adapter_list_init(adapter) != SUCCESS_RETURN) {
		iotx_alcs_adapter_deinit();
		log_err("ALCS Linked List Init Failed");
		return FAIL_RETURN;
	}
	if (alcs_mqtt_init(adapter->coap_ctx,product_key,device_name) != ALCS_MQTT_STATUS_SUCCESS) {
		iotx_alcs_adapter_deinit();
		log_err("ALCS MQTT Init Failed");
		return FAIL_RETURN;
	}
#if  defined(COAP_WITH_YLOOP) /*&& defined(CM_SUPPORT_MULTI_THREAD)*/
    alcs_inited=1;
    aos_schedule_call(alcs_repeat_action, (void *)adapter);
#endif

	return SUCCESS_RETURN;
}

void* IOT_ALCS_Construct(iotx_alcs_param_t *params)
{
    log_info ("IOT_ALCS_Construct enter");

	int res = 0;
	iotx_alcs_adapter_t *adapter = __iotx_alcs_get_ctx();

	POINTER_SANITY_CHECK(params, NULL);
	STRING_PTR_SANITY_CHECK(params->group, NULL);
	
	memset(adapter,0,sizeof(iotx_alcs_adapter_t));
	
	res = iotx_alcs_adapter_init(adapter, params);
	if (res != SUCCESS_RETURN) {
		log_err("Adapter Init Failed");
		return NULL;
	}
	
	return (void *)adapter;
}

int IOT_ALCS_Destroy(void **phandle)
{
	POINTER_SANITY_CHECK(phandle, NULL_VALUE_ERROR);
	POINTER_SANITY_CHECK(*phandle, NULL_VALUE_ERROR);

	iotx_alcs_adapter_deinit();
	
	return SUCCESS_RETURN;
}

#if  defined(COAP_WITH_YLOOP)
static void alcs_repeat_action(void *handle)
{
	if(handle == NULL||alcs_inited == 0){
		return;
	}
    alcs_heartbeat(handle);
    aos_post_delayed_action(1000, alcs_repeat_action, handle);
}
#endif

static void alcs_heartbeat(void *handle)
{
    iotx_alcs_adapter_t *adapter = (iotx_alcs_adapter_t *)handle;

	if (adapter->role & IOTX_ALCS_ROLE_SERVER && adapter->alcs_server_auth_timer_func != NULL)
		adapter->alcs_server_auth_timer_func(adapter->coap_ctx);

	if (adapter->role & IOTX_ALCS_ROLE_CLIENT && adapter->alcs_client_auth_timer_func != NULL)
		adapter->alcs_client_auth_timer_func(adapter->coap_ctx);
}
int IOT_ALCS_Yield(void *handle)
{
	int res = 0;
	iotx_alcs_adapter_t *adapter = (iotx_alcs_adapter_t *)handle;
	
	POINTER_SANITY_CHECK(adapter, NULL_VALUE_ERROR);
	POINTER_SANITY_CHECK(adapter->coap_ctx, NULL_VALUE_ERROR);
#ifndef COAP_WITH_YLOOP 
	res = (CoAPMessage_cycle(adapter->coap_ctx) != COAP_SUCCESS) ? (FAIL_RETURN) : (SUCCESS_RETURN);
#endif  
	alcs_heartbeat(handle);

	return res;
}

int IOT_ALCS_Send(void *handle, iotx_alcs_msg_t *msg)
{
	int res = 0;
	iotx_alcs_adapter_t *adapter = (iotx_alcs_adapter_t *)handle;
	CoAPMessage coap_msg;
	CoAPLenString coap_payload;
	NetworkAddr network_addr;
	
	POINTER_SANITY_CHECK(adapter, NULL_VALUE_ERROR);
	POINTER_SANITY_CHECK(adapter->coap_ctx, NULL_VALUE_ERROR);
	POINTER_SANITY_CHECK(msg, NULL_VALUE_ERROR);
	POINTER_SANITY_CHECK(msg->payload,NULL_VALUE_ERROR);
	
	STRING_PTR_SANITY_CHECK(msg->ip, FAIL_RETURN);
	STRING_PTR_SANITY_CHECK(msg->uri,FAIL_RETURN);
	

	if (strlen(msg->ip) > NETWORK_ADDR_LEN) {
		log_err("Invalid Ip Address Length");
		return FAIL_RETURN;
	}
	
	memset(&coap_msg,0,sizeof(CoAPMessage));
	memset(&coap_payload,0,sizeof(CoAPLenString));

	coap_payload.len = msg->payload_len;
	coap_payload.data = msg->payload;
	
	alcs_msg_init(adapter->coap_ctx,&coap_msg,msg->msg_code, msg->msg_type,0,&coap_payload,(void *)adapter);
	
    res = alcs_msg_setAddr(&coap_msg, msg->uri, NULL);
	if (res != COAP_SUCCESS) {
		log_err("ALCS Message Set URI Failed");
		return FAIL_RETURN;
	}

	memset(&network_addr,0,sizeof(NetworkAddr));
	memcpy(&network_addr,msg->ip,strlen(msg->ip));
	network_addr.port = msg->port;

    res = alcs_sendmsg_secure(adapter->coap_ctx,&network_addr,&coap_msg,2,iotx_alcs_coap_adapter_send_msg_handle);

	alcs_msg_deinit(&coap_msg);
	if (res != COAP_SUCCESS) {
		log_err("ALCS Message Send Message Failed");
		return FAIL_RETURN;
	}

	iotx_alcs_send_msg_t *alcs_send_msg = 
		(iotx_alcs_send_msg_t *)LITE_malloc(sizeof(iotx_alcs_send_msg_t));
	if (alcs_send_msg == NULL)
	{
		log_warning("Not Enough Memory");
		return FAIL_RETURN;
	}
	memset(alcs_send_msg,0,sizeof(iotx_alcs_send_msg_t));

	alcs_send_msg->token = (uint8_t *)LITE_malloc(coap_msg.header.tokenlen + 1);
	if (alcs_send_msg->token == NULL)
	{
		LITE_free(alcs_send_msg);
		log_warning("Not Enough Memory");
		return FAIL_RETURN;
	}
	alcs_send_msg->token_len = coap_msg.header.tokenlen;
	
	memset(alcs_send_msg->token,0,alcs_send_msg->token_len + 1);
	memcpy(alcs_send_msg->token,coap_msg.token,alcs_send_msg->token_len);

	alcs_send_msg->uri = (char *)LITE_malloc(strlen(msg->uri)+1);
	if (alcs_send_msg->uri == NULL)
	{	
		LITE_free(alcs_send_msg->token);
		LITE_free(alcs_send_msg);
		log_warning("ALCS Message Buffer Failed");
		return FAIL_RETURN;
	}
	memset(alcs_send_msg->uri,0,strlen(msg->uri)+1);
	memcpy(alcs_send_msg->uri,msg->uri,strlen(msg->uri));

	linked_list_insert(adapter->alcs_send_list,(void *)alcs_send_msg);
	
	return SUCCESS_RETURN;
}

int IOT_ALCS_Send_Response(void *handle, iotx_alcs_msg_t *msg, uint8_t token_len, uint8_t *token)
{
	int res = 0;
	iotx_alcs_adapter_t *adapter = (iotx_alcs_adapter_t *)handle;
	CoAPMessage coap_msg;
	CoAPLenString coap_payload;
	CoAPLenString token_payload;
	NetworkAddr network_addr;
	
	POINTER_SANITY_CHECK(adapter, NULL_VALUE_ERROR);
	POINTER_SANITY_CHECK(adapter->coap_ctx, NULL_VALUE_ERROR);
	POINTER_SANITY_CHECK(msg, NULL_VALUE_ERROR);
	POINTER_SANITY_CHECK(msg->payload,NULL_VALUE_ERROR);
	
	STRING_PTR_SANITY_CHECK(msg->ip, FAIL_RETURN);
	STRING_PTR_SANITY_CHECK(msg->uri,FAIL_RETURN);
	STRING_PTR_SANITY_CHECK((const char *)token,FAIL_RETURN);
	

	if (strlen(msg->ip) > NETWORK_ADDR_LEN) {
		log_err("Invalid Ip Address Length");
		return FAIL_RETURN;
	}

	memset(&coap_msg,0,sizeof(CoAPMessage));
	memset(&coap_payload,0,sizeof(CoAPLenString));
	memset(&token_payload,0,sizeof(CoAPLenString));

	coap_payload.len = msg->payload_len;
	coap_payload.data = msg->payload;

	alcs_msg_init(adapter->coap_ctx,&coap_msg,msg->msg_code, msg->msg_type,0,&coap_payload,(void *)adapter);
	
    res = alcs_msg_setAddr(&coap_msg, msg->uri, NULL);
	if (res != COAP_SUCCESS)
	{
		log_err("ALCS Message Set URI Failed");
		return FAIL_RETURN;
	}

	memset(&network_addr,0,sizeof(NetworkAddr));
	memcpy(&network_addr,msg->ip,strlen(msg->ip));
	network_addr.port = msg->port;

	token_payload.len = token_len;
	token_payload.data = token;

    if (alcs_resource_need_auth(adapter->coap_ctx, msg->uri)) {
        res = alcs_sendrsp_secure(adapter->coap_ctx,&network_addr,&coap_msg,2,0,&token_payload);
    } else {
        res = alcs_sendrsp(adapter->coap_ctx,&network_addr,&coap_msg,2,0,&token_payload);
    }

	alcs_msg_deinit(&coap_msg);
	if (res != COAP_SUCCESS)
	{
		log_err("ALCS Message Send Message Failed");
		return FAIL_RETURN;
	}
	
	return SUCCESS_RETURN;
}

int IOT_ALCS_Register_Resource(void *handle, iotx_alcs_res_t *resource)
{
	int res = 0;
	iotx_alcs_adapter_t *adapter = (iotx_alcs_adapter_t *)handle;
	
	POINTER_SANITY_CHECK(adapter, NULL_VALUE_ERROR);
	POINTER_SANITY_CHECK(adapter->coap_ctx, NULL_VALUE_ERROR);

	STRING_PTR_SANITY_CHECK(resource->uri, FAIL_RETURN);

    log_info ("alcs register resource, uri:%s", resource->uri);	

    int needAuth = resource->need_auth; // strcmp (resource->uri, "/dev/core/service/dev");
//    int needAuth = strcmp (resource->uri, "/dev/core/service/dev");

	res = alcs_resource_register(adapter->coap_ctx,
									resource->uri,
									resource->msg_perm,
									resource->msg_ct,
									resource->maxage,
									needAuth,
									iotx_alcs_coap_adapter_recv_msg_handle);
	
	if (res != COAP_SUCCESS) {
		log_err("ALCS Register Resource Failed, Code: %d",res);
		return FAIL_RETURN;
	}
	
	return SUCCESS_RETURN;
}

int IOT_ALCS_Observe_Notify(void *handle, const char *uri, uint32_t payload_len, uint8_t *payload)
{
	int res = 0;
	iotx_alcs_adapter_t *adapter = (iotx_alcs_adapter_t *)handle;
	CoAPLenString coap_payload;

	coap_payload.len = (int32_t)payload_len;
	coap_payload.data = payload;

	res = alcs_observe_notify(adapter->coap_ctx,uri,&coap_payload);
	if (res != COAP_SUCCESS) {
		log_err("ALCS Observe Notify Failed, Code: %d",res);
		return FAIL_RETURN;
	}
	
	return SUCCESS_RETURN;
}

int IOT_ALCS_Unregister_Resource(void *handle,char *uri)
{
	return SUCCESS_RETURN;
}


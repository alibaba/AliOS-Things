#ifndef _IOT_DM_API_H_
#define _IOT_DM_API_H_

#include "lite-cjson.h"
#include "iot_export_dm.h"

typedef struct {
	void *mutex;
	void *cloud_connectivity;
	void *local_connectivity;
	iotx_dm_event_callback event_callback;
}iotx_dapi_ctx_t;

typedef struct {
	void *mutex;
	int devid;
	lite_cjson_item_t *lite;
}iotx_dapi_property_t;

#endif

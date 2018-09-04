#ifndef _STREAM_H
#define _STREAM_H
#include "cmsis_os.h"
#include "mmf_common.h"

typedef struct _media_sink_module{
	void* (*create)(void);
	void  (*destroy)(void*);
	int   (*set_param)(void*, int, int);
	int   (*handle)(void*, void*);
}msink_module_t;

typedef struct _media_sink_context{
	xQueueHandle input_qid;
	xQueueHandle output_qid;
	int state;
	xTaskHandle	hdl_task;
	
	void* drv_priv;		// private data for module
	msink_module_t *sink;
}msink_context;

void 			mmf_sink_close(msink_context* ctx);
msink_context* 	mmf_sink_open(msink_module_t *sink);
int 			mmf_sink_ctrl(msink_context* ctx, int cmd, int arg);
int 			mmf_sink_put_frame(msink_context* ctx, exch_buf_t* exbuf);

// must be here
#include "mmf_sink_modules/mmf_sink_list.h"
#endif

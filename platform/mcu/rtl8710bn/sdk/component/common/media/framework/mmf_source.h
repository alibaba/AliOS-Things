#ifndef _MEDIA_H
#define _MEDIA_H
#include "cmsis_os.h"
#include "mmf_common.h"

typedef struct _media_source_module{
	void* (*create)(void);
	void  (*destroy)(void*);
	int   (*set_param)(void*, int, int);
	int   (*handle)(void*, void*);		// input output will cast to exch_buf_t
}msrc_module_t;



typedef struct _media_source_context{
	xQueueHandle input_qid;
	xQueueHandle output_qid;
	int state;
	xTaskHandle	hdl_task;
	
	void* drv_priv;
	msrc_module_t* source;
}msrc_context;

void 			mmf_source_close(msrc_context* ctx);
msrc_context* 	mmf_source_open(msrc_module_t* source);
int 			mmf_source_ctrl(msrc_context* ctx, int cmd, int arg);
int 			mmf_source_get_frame(msrc_context* ctx, exch_buf_t *exbuf);

// must be here
#include "mmf_source_modules/mmf_source_list.h"
#endif

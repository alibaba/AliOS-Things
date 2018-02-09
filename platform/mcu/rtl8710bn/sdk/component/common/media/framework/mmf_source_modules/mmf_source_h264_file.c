#include "mmf_source.h"


#include "sample_h264.h"

void* h264f_mod_open(void)
{
	return (void*)1;
}

void h264f_mod_close(void* ctx)
{
	(void)ctx;
	return;
}

int h264f_mod_set_param(void* ctx, int cmd, int arg)
{
	(void)ctx;
	(void)cmd;
	(void)arg;
	return 0;
}

static unsigned char* seek_to_next(unsigned char* ptr, unsigned char* ptr_end)
{
	ptr+=3;
	
	while( ptr < ptr_end ){
		if(ptr[0]==0 && ptr[1]==0){
			if(ptr[2]==0 && ptr[3]==1 )
				return ptr;
			else if(ptr[2]==1)
				return ptr;
		}
		ptr++;
	}
	
	return NULL;
}

static unsigned char *sample = (unsigned char*)h264_sample;
static unsigned char *next_sample = (unsigned char *)h264_sample;
u8 *ptr = NULL;
int h264f_mod_handle(void* ctx, void* b)
{
	int ret = 0;
	
	(void)ctx;
	
	exch_buf_t *exbuf = (exch_buf_t*)b;
	
	/*get uvc buffer for new data*/
	if(exbuf->state!=STAT_READY){	
		sample = next_sample;
		ptr = sample;        
		next_sample = seek_to_next(sample, (unsigned char*)h264_sample+h264_sample_len);
		if(next_sample == NULL)
		{
			//replay
			exbuf->len = h264_sample + h264_sample_len - sample;
			next_sample = (unsigned char*)h264_sample; 
		}else{
			exbuf->len = next_sample - sample;
		}               
		
		exbuf->index = 0;
		exbuf->data = sample;
		exbuf->timestamp = 0;
		//vTaskDelay(500);
		exbuf->state = STAT_READY;
	}
	
	return 0;

}

msrc_module_t h264f_module =
{
	.create = h264f_mod_open,
	.destroy = h264f_mod_close,
	.set_param = h264f_mod_set_param,
	.handle = h264f_mod_handle,
};
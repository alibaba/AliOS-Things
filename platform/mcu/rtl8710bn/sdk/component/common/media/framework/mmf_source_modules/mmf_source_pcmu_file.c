#include "mmf_source.h"
#include "sample_pcmu.h"
static unsigned int base_ts;

void* pcmuf_mod_open(void)
{
	return (void*)1;
}

void pcmuf_mod_close(void* ctx)
{
	(void)ctx;
	return;
}

int pcmuf_mod_set_param(void* ctx, int cmd, int arg)
{
	(void)ctx;
	(void)cmd;
	(void)arg;
	return 0;
}

#define G711F_SIZE 160
static unsigned char *sample=(unsigned char *)pcmu_sample;
static unsigned char *next_sample=(unsigned char *)pcmu_sample;
int pcmuf_mod_handle(void* ctx, void* b)
{
	int ret = 0;
	
	(void)ctx;
	
	exch_buf_t *exbuf = (exch_buf_t*)b;
	
	/*get uvc buffer for new data*/
	if(exbuf->state!=STAT_READY){	
		
		
		unsigned char * source=(unsigned char*)pcmu_sample;
		sample=next_sample;
		if(sample-source >= pcmu_sample_size){//replay
			sample = source; 
			next_sample = source+G711F_SIZE;
		}
		else{
			next_sample+=G711F_SIZE;
			if(next_sample - source >= pcmu_sample_size){
				next_sample = source + pcmu_sample_size;
			}
		}
		exbuf->len =  next_sample-sample;	       
		
		exbuf->index = 0;
		exbuf->data = sample;
		//exbuf->timestamp = 0;
                base_ts += 160;
                exbuf->timestamp = base_ts;
		//vTaskDelay(500);
		exbuf->state = STAT_READY;
	}
	
	return 0;

}

msrc_module_t pcmuf_module =
{
	.create = pcmuf_mod_open,
	.destroy = pcmuf_mod_close,
	.set_param = pcmuf_mod_set_param,
	.handle = pcmuf_mod_handle,
};
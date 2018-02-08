#include "mmf_source.h"
#include "sample_aac.h"

static unsigned int base_ts;
void* aacf_mod_open(void)
{
        base_ts = xTaskGetTickCount();
	return (void*)1;
}

void aacf_mod_close(void* ctx)
{
	(void)ctx;
	return;
}

int aacf_mod_set_param(void* ctx, int cmd, int arg)
{
	(void)ctx;
	(void)cmd;
	(void)arg;
	return 0;
}

static unsigned char* seek_to_next(unsigned char* ptr, unsigned char* ptr_end)
{
	//ptr+=7;
	while(ptr < ptr_end){
			if(ptr[0]==0xff && (ptr[1]>>4)==0x0f)
				break;
			ptr++;
	}
	if(ptr>=ptr_end)	return NULL;
	unsigned char * temp = ptr+3;
	u32 ausize = ((*temp&0x03)<<11)|(*(temp+1)<<3)|((*(temp+2)&0xe0)>>5);
	ptr+=ausize;
	if(ptr>=ptr_end)
		return NULL;
	else{
		while(ptr < ptr_end){
			if(ptr[0]==0xff && (ptr[1]>>4)==0x0f)
				return ptr;
			ptr++;
		}
		return NULL;
	}
		
}

static unsigned char *sample = (unsigned char*)aac_sample;
static unsigned char *next_sample = (unsigned char *)aac_sample;
static u8 *ptr = NULL;
int aacf_mod_handle(void* ctx, void* b)
{
	int ret = 0;
	
	(void)ctx;
	
	exch_buf_t *exbuf = (exch_buf_t*)b;
	
	/*get uvc buffer for new data*/
	if(exbuf->state!=STAT_READY){	
		
		
		/*we will carry single AU for each rtp packet*/
		sample = next_sample;
		next_sample = seek_to_next(sample, (unsigned char*)aac_sample+aac_sample_size);
		if(next_sample == NULL)
		{
			//replay
			exbuf->len = aac_sample + aac_sample_size - sample;
			next_sample = (unsigned char*)aac_sample; 
		}else{
			exbuf->len = next_sample - sample;
		}               
		
		exbuf->index = 0;
		exbuf->data = sample;
		//exbuf->timestamp = 0;
                base_ts += 1024;
                exbuf->timestamp = base_ts;
		//vTaskDelay(500);
		exbuf->state = STAT_READY;
	}
	
	return 0;

}

msrc_module_t aacf_module =
{
	.create = aacf_mod_open,
	.destroy = aacf_mod_close,
	.set_param = aacf_mod_set_param,
	.handle = aacf_mod_handle,
};
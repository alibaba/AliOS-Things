#include "autoconf.h"
#if !defined(CONFIG_PLATFORM_8195A) && !defined(CONFIG_PLATFORM_8711B)
#include <flash/stm32_flash.h>
#if defined(STM32F2XX)
#include <stm32f2xx_flash.h>
#elif defined(STM32F4XX)	 
#include <stm32f4xx_flash.h>
#elif defined(STM32f1xx)	 
#include <stm32f10x_flash.h>
#endif
#include "cloud_updater.h"
#else
#include "flash_api.h"
#include "device_lock.h"
#endif

#define BUF_LEN 512
#define CONFIG_MD5_USE_SSL

#ifdef	CONFIG_MD5_USE_SSL
#include "md5.h"
#define	file_md5_context						md5_context
#define	file_md5_init(ctx)						md5_init(ctx)
#define	file_md5_starts(ctx)					md5_starts(ctx)
#define file_md5_update(ctx, input, len)		md5_update(ctx, input, len)
#define file_md5_finish(ctx, output)			md5_finish(ctx, output)
#define file_md5_free(ctx)						md5_free(ctx)
#else
#include "rom_md5.h"
#define	file_md5_context						md5_ctx
#define	file_md5_init(ctx)						rt_md5_init(ctx)
#define file_md5_starts(ctx)
#define file_md5_update(ctx, input, len)		rt_md5_append(ctx, input, len)
#define file_md5_finish(ctx, output)			rt_md5_final(output, ctx)
#define file_md5_free(ctx)
#endif

int file_check_sum(uint32_t addr, uint32_t image_len, u8* check_sum)
{
	int ret = -1 ;
	flash_t	flash;	
	file_md5_context md5;
	unsigned char buf[BUF_LEN];
	uint32_t read_addr = addr;
	int len = 0;
       
	file_md5_init(&md5);
	file_md5_starts(&md5);
	
	while(len < image_len){
		if ((image_len-len) >= BUF_LEN){
			device_mutex_lock(RT_DEV_LOCK_FLASH);
			flash_stream_read(&flash, read_addr, BUF_LEN, buf);		
			device_mutex_unlock(RT_DEV_LOCK_FLASH);
			file_md5_update(&md5, buf, BUF_LEN);
		}else{
			device_mutex_lock(RT_DEV_LOCK_FLASH);
			flash_stream_read(&flash, read_addr, (image_len-len), buf);
			device_mutex_unlock(RT_DEV_LOCK_FLASH);
			file_md5_update(&md5, buf, (image_len-len)); 				
		}
		len += BUF_LEN;
		read_addr = read_addr + BUF_LEN;
	}
	
	file_md5_finish(&md5, check_sum);
	file_md5_free(&md5);
}

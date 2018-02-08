#ifndef MMF_SOURCE_I2S_FILE_H
#define MMF_SOURCE_I2S_FILE_H

#include "mmf_source.h"
#include "FreeRTOS.h"
#include "task.h"
//#include "example_rtsp_server_i2s.h"
#include "rtsp/rtsp_api.h"
#include "sockets.h"
#include "lwip/netif.h"

#include "i2s_api.h"
#include "alc5651.h"        
#include "g711\g711_codec.h"
#include "gpio_api.h"   // mbed
#include "gpio_irq_api.h"

#include <platform/platform_stdlib.h>
#include "platform_opts.h"
#include "dlist.h"
#include "basic_types.h"
#include "osdep_service.h"

#define I2S_DMA_PAGE_SIZE	320   // 2 ~ 4096
#define I2S_DMA_PAGE_NUM    4   // Vaild number is 2~4
#define RECV_PAGE_NUM 	I2S_DMA_PAGE_NUM

#define G711_FSIZE 			160
#define I2S_MODE_SWITCH			PE_5 

#define I2S_SCLK_PIN            PC_1
#define I2S_WS_PIN              PC_0
#define I2S_SD_PIN              PC_2

#endif /* MMF_SOURCE_I2S_FILE_H */
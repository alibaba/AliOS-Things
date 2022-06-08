/*
 * This file is part of the OpenMV project.
 * Copyright (c) 2013/2014 Ibrahim Abdelkader <i.abdalkader@gmail.com>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * OV2640 driver.
 *
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "pin.h"
#include "pin_static_af.h"
#include "mpu.h"
#include "systick.h"

#include "lib/oofatfs/ff.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"

#include "py/objstr.h"
#include "py/objlist.h"
#include "py/stream.h"

#include "i2c.h"


#if MICROPY_HW_OV2640

#include "global.h" 
#include "ov2640.h"
#include "ov2640_regs.h"

#if MICROPY_ENABLE_TFTLCD || MICROPY_HW_LTDC_LCD

#if MICROPY_HW_LTDC_LCD
#include "ltdc.h"
#endif

#include "modtftlcd.h"

#if MICROPY_HW_LCD43M
#include "lcd43m.h"
#endif

#endif

//#define OV2640_DEBUG 1

uint8_t disp_dir = 0;
#ifdef OV2640_DEBUG
#define ov_printf(...) mp_printf(&mp_plat_print, __VA_ARGS__)
#else
#define ov_printf(...)
#endif

#define OV2640_ADDR			(0x30)
#define I2C_TIMEOUT_MS		(100)

#define VGA_WIDTH			(640)
#define VGA_HEIGHT			(480)

#define SVGA_WIDTH			(800)
#define SVGA_HEIGHT			(600)

#define UXGA_WIDTH			(1600)
#define UXGA_HEIGHT			(1200)

#define ACK					0
#define NACK				1
#define STOP				1
#define NSTOP				0

#if MICROPY_HW_BOARD_COLUMBUS
#define SCCB_I2C						I2C1
#define SCCB_I2C_SDA					MICROPY_HW_I2C1_SDA
#define SCCB_I2C_SCL					MICROPY_HW_I2C1_SCL
#define DCMI_DMA_STREAM					DMA2_Stream1
#define DCMI_DMA_STREAM_IRQ				DMA2_Stream1_IRQn
#elif MICROPY_HW_BOARD_MAGELLAM
#define SCCB_I2C						I2C2
#define SCCB_I2C_SDA					MICROPY_HW_I2C2_SDA
#define SCCB_I2C_SCL					MICROPY_HW_I2C2_SCL
#if defined(STM32F4) || defined(STM32F7)
#define DCMI_DMA_STREAM					DMA2_Stream1
#define DCMI_DMA_STREAM_IRQ				DMA2_Stream1_IRQn
#elif defined(STM32H7)
#define DCMI_DMA_STREAM					DMA1_Stream1
#define DCMI_DMA_STREAM_IRQ				DMA1_Stream1_IRQn
#endif
#else
	#error "no define iic"
#endif
//===================================================================================================================
typedef struct _sensor_ov2640_obj_t {
    mp_obj_base_t base;
		uint16_t sensor_id;
} ov2640_obj_t;

STATIC uint16_t ov2640_id = 0;

//=====================================================================================================================

FIL ov2640_fp;//文件读写对象

#define 	jpeg_dma_bufsize	5*1024		//定义JPEG DMA接收时数据缓存jpeg_buf0/1的大小(*4字节)
volatile 	uint32_t jpeg_data_len=0; 	//buf中的JPEG有效数据长度
volatile 	uint8_t jpeg_data_ok=0;			//JPEG数据采集完成标志 
#if MICROPY_HW_LTDC_LCD
uint16_t curline = 0;							//摄像头输出数据,当前行编号
#endif
#if defined(STM32H7)
uint32_t jpeg_buf0[jpeg_dma_bufsize];						
uint32_t jpeg_buf1[jpeg_dma_bufsize];
#elif defined(STM32F4) || defined(STM32F7)
uint32_t *jpeg_buf0;						
uint32_t *jpeg_buf1;
#endif

uint32_t *jpeg_data_buf;

uint8_t ov2640_mode=0; 		 //工作模式:0,RGB565模式;1,JPEG模式

STATIC uint16_t display_w = 0;       // display width
STATIC uint16_t display_h = 0;       // display height

STATIC uint16_t display_sx = 0;       // display x
STATIC uint16_t display_sy = 0;       // display y

framesize_t framesize;

bool is_display = false; //目前在显示图像
const int resolution[][2] = {
    {0,    0   },
    {80,   60  },    /* QQQVGA    */
    {160,  120 },    /* QQVGA     */
    {320,  240 },    /* QVGA      */
    {640,  480 },    /* VGA       */
    {720,  480 },    /* WVGA      */
    {1024, 768 },    /* XGA       */
    {1600, 1200},    /* UXGA      */
    {0,    0   },
};

//=====================================================================================================================
void dcmi_dma_init(uint32_t mem0addr,uint32_t mem1addr,uint16_t memsize,uint32_t memblen,uint32_t meminc);
uint8_t ov2640_outsize_set(uint16_t width,uint16_t height);
void dcmi_init(void);
void dcmi_set_window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height);
void DCMI_Start(void);
void sw_sdcard_mode(void);
//===================================================================================================================
DCMI_HandleTypeDef  DCMI_Handler;           //DCMI句柄
DMA_HandleTypeDef   DMADMCI_Handler;        //DMA句柄
//===================================================================================================================

void (*dcmi_rx_callback)(void);
 
STATIC uint8_t fps = 0; // 帧率
//DCMI中断服务函数
void DCMI_IRQHandler(void)
{
    HAL_DCMI_IRQHandler(&DCMI_Handler);
}

//捕获到一帧图像处理函数
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
	jpeg_data_process();
	__HAL_DCMI_CLEAR_FLAG(&DCMI_Handler,DCMI_FLAG_FRAMERI);
	fps++;
	#if MICROPY_HW_LTDC_LCD
	curline = 0;							//摄像头输出数据,当前行编号
	#endif
	#if MICROPY_HW_LCD43M
	if(is_display){
		if(lcddev.type == LCD43M){
			uint16_t sx = (lcddev.width - display_w)>>1;
			uint16_t sy = (lcddev.height - display_h)>>1;
			if(lcddev.type == LCD43M){
				LCD_SetCursor(sx,sy);  
				LCD43M_REG=WRAMCMD;
			}
		}
	}
	#endif
	
	__HAL_DCMI_ENABLE_IT(&DCMI_Handler,DCMI_IT_FRAME);	
}

//jpeg数据接收回调函数
void jpeg_dcmi_rx_callback(void)
{ 
	uint16_t i;
	uint32_t *pbuf;
	pbuf=jpeg_data_buf+jpeg_data_len;
	if(DCMI_DMA_STREAM->CR&(1<<19))
	{ 
		for(i=0;i<jpeg_dma_bufsize;i++)pbuf[i]=jpeg_buf0[i];
		jpeg_data_len+=jpeg_dma_bufsize;
	}else 
	{
		for(i=0;i<jpeg_dma_bufsize;i++)pbuf[i]=jpeg_buf1[i];
		jpeg_data_len+=jpeg_dma_bufsize;
	} 	
}
#if MICROPY_HW_LTDC_LCD
//RGB屏数据接收回调函数
void ltdc_dcmi_rx_callback(void)
{  
	uint16_t *pbuf;
	if(DCMI_DMA_STREAM->CR&(1<<19)){ 
		pbuf=(uint16_t*)jpeg_buf0; 
	}else{
		pbuf=(uint16_t*)jpeg_buf1; 
	} 	
	
	//printf("ltdc_dcmi_rx_callback\r\n");
	#if MICROPY_HW_LTDC_LCD
	#if MICROPY_HW_LCD43R
	if(lcddev.type == LCD43R){
		lcd43g_full_cam(display_sx, display_sy+curline, display_w, 1,pbuf);
	}
	#endif
	
	#if MICROPY_HW_LCD7R
	if(lcddev.type == LCD7R){
		lcd7r_full_cam(display_sx, display_sy+curline, display_w, 1,pbuf);
	}
	#endif
	
	#endif
	if(curline<display_h)curline +=1;
}
#endif

//====================================================================================
void DCMI_Start(void)
{  
	#if MICROPY_HW_LCD43M
	if(lcddev.type == LCD43M){
		LCD43M_REG=WRAMCMD;	
	}
	#endif
	__HAL_DMA_ENABLE(&DMADMCI_Handler); 
	DCMI->CR|=DCMI_CR_CAPTURE;    			
}

void DCMI_Stop(void)
{
	DCMI->CR&=~(DCMI_CR_CAPTURE);  		
	while(DCMI->CR&0X01);					
	__HAL_DMA_DISABLE(&DMADMCI_Handler);
} 
//==============================================================================================

int sccb_write_reg(uint8_t reg, uint8_t dat)
{
	int ret = 0;
	uint8_t data[2] = {reg, dat}; //
	ret = i2c_writeto(SCCB_I2C, OV2640_ADDR, data, 2, true);
	return ret;
}

int sccb_read_reg(uint8_t reg, uint8_t *dat)
{
	int ret = 0;

	ret = i2c_writeto(SCCB_I2C, OV2640_ADDR, &reg, 1, false);
	ret |= i2c_readfrom(SCCB_I2C, OV2640_ADDR, dat, 1, true);
	return ret;
}
//=========================================================================================
void sccb_deinit()
{
	DCMI_Stop();

	__HAL_RCC_DCMI_CLK_DISABLE();
	HAL_NVIC_DisableIRQ(DCMI_IRQn);
	HAL_DMA_DeInit(&DMADMCI_Handler);
	HAL_DCMI_Init(&DCMI_Handler); 
	__HAL_DCMI_DISABLE_IT(&DCMI_Handler,DCMI_IT_FRAME);	
	__HAL_DCMI_DISABLE(&DCMI_Handler);
	HAL_NVIC_DisableIRQ(DCMI_DMA_STREAM_IRQ);
	mp_hal_pin_high(MICROPY_HW_DCMI_PWDN); //
	mp_hal_pin_low(MICROPY_HW_DCMI_RESE); //

#if MICROPY_HW_BOARD_COLUMBUS
	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_11);
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);
	HAL_GPIO_DeInit(GPIOE, GPIO_PIN_6|GPIO_PIN_5);
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6|GPIO_PIN_4);
#elif MICROPY_HW_BOARD_MAGELLAM
	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_11);
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9);
	HAL_GPIO_DeInit(GPIOE, GPIO_PIN_6|GPIO_PIN_5);
	HAL_GPIO_DeInit(GPIOD, GPIO_PIN_3);
	HAL_GPIO_DeInit(GPIOI, GPIO_PIN_8);
	HAL_GPIO_DeInit(GPIOG, GPIO_PIN_9);
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6|GPIO_PIN_4|GPIO_PIN_8);
#endif
	sw_sdcard_mode();
}
//===================================================================================================================

void ov2640_init(void)
{
	int ret;
	#if defined(STM32F4) || defined(STM32F7)
	//HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_PLLCLK, RCC_MCODIV_4);//42M
	HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_PLLCLK, RCC_MCODIV_5);//33M
	#if MICROPY_HW_BOARD_COLUMBUS
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_DCMI_PIXCK, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_DCMI_PIXCK);
	#elif MICROPY_HW_BOARD_MAGELLAM
	
	#if MICROPY_HW_LTDC_LCD
	if(lcddev.type == LCD43R || lcddev.type == LCD7R){
		HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1);//12M
	}
	#endif
	
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_DCMI_PIXCK, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_DCMI_PIXCLK);
	#endif
	
	#elif defined(STM32H7)
	HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_PLL1QCLK, RCC_MCODIV_4);//100M/4
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_DCMI_PIXCK, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_DCMI_PIXCLK);
	#endif
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_DCMI_HSYNC, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_DCMI_HSYNC);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_DCMI_VSYNC, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_DCMI_VSYNC);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_DCMI_D0, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_DCMI_D0);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_DCMI_D1, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_DCMI_D1);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_DCMI_D2, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_DCMI_D2);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_DCMI_D3, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_DCMI_D3);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_DCMI_D4, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_DCMI_D4);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_DCMI_D5, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_DCMI_D5);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_DCMI_D6, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_DCMI_D6);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_DCMI_D7, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_DCMI_D7);

	mp_hal_pin_config(MICROPY_HW_DCMI_PWDN, MP_HAL_PIN_MODE_OUTPUT, MP_HAL_PIN_PULL_UP, 0);
	mp_hal_pin_config(MICROPY_HW_DCMI_RESE, MP_HAL_PIN_MODE_OUTPUT, MP_HAL_PIN_PULL_UP, 0);

	mp_hal_pin_high(MICROPY_HW_DCMI_PWDN); //
	mp_hal_delay_ms(10);
	mp_hal_pin_low(MICROPY_HW_DCMI_PWDN); // turn off AVDD
	mp_hal_delay_ms(10);
	mp_hal_pin_low(MICROPY_HW_DCMI_RESE); //
	mp_hal_delay_ms(10);
	mp_hal_pin_high(MICROPY_HW_DCMI_RESE); //
	mp_hal_delay_ms(10);

  // start the I2C bus in master mode
	i2c_init(SCCB_I2C, SCCB_I2C_SCL, SCCB_I2C_SDA, 400000, I2C_TIMEOUT_MS);

	uint8_t data[2] = {BANK_SEL, BANK_SEL_SENSOR}; //
	uint16_t radta = 0;
	i2c_writeto(SCCB_I2C, OV2640_ADDR, data, 2, true);
	
	data[0] = COM7; data[1] = COM7_SRST;
	i2c_writeto(SCCB_I2C, OV2640_ADDR, data, 2, true);
	mp_hal_delay_ms(50);
	
	data[0] = MIDH; data[1] = MIDL;
	i2c_writeto(SCCB_I2C, OV2640_ADDR, data, 1, false);
	i2c_readfrom(SCCB_I2C, OV2640_ADDR, data, 1, true);
	radta = (uint16_t)(data[0]<<8);
	data[0] = MIDL;
	i2c_writeto(SCCB_I2C, OV2640_ADDR, data, 1, false);
	i2c_readfrom(SCCB_I2C, OV2640_ADDR, data, 1, true);
	radta |= data[0];
	ov_printf("MID:%x\n",radta);

	data[0] = REG_PID; 
	i2c_writeto(SCCB_I2C, OV2640_ADDR, data, 1, false);
	i2c_readfrom(SCCB_I2C, OV2640_ADDR, data, 1, true);
	radta = (uint16_t)(data[0]<<8);
	data[0] = REG_VER;
	i2c_writeto(SCCB_I2C, OV2640_ADDR, data, 1, false);
	i2c_readfrom(SCCB_I2C, OV2640_ADDR, data, 1, true);
	radta |= data[0];
	
	ov_printf("PID:%x\n",radta);
	ov2640_id = radta;

	for (int i = 0; i < 4; i++) {
		ret = i2c_writeto(SCCB_I2C, OV2640_ADDR, NULL, 0, true);
		if (ret == 0) {
			break;
		}
	}
	if (ret != 0) {
		mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("ov2640 not found"));
	}

	for(uint16_t i=0;i<sizeof(default_regs)/2;i++)
	{
		data[0] = default_regs[i][0]; //
		data[1] = default_regs[i][1]; //
		i2c_writeto(SCCB_I2C, OV2640_ADDR, data, 2, true);
	} 
		
    __HAL_RCC_DCMI_CLK_ENABLE();                

    HAL_NVIC_SetPriority(DCMI_IRQn,1,2);        
    HAL_NVIC_EnableIRQ(DCMI_IRQn);              
    
	mp_hal_delay_ms(100);

}

//DCMI初始化
void dcmi_init(void)
{
	DCMI_Handler.Instance=DCMI;
	DCMI_Handler.Init.SynchroMode=DCMI_SYNCHRO_HARDWARE;    
	DCMI_Handler.Init.PCKPolarity=DCMI_PCKPOLARITY_RISING; 
	DCMI_Handler.Init.VSPolarity=DCMI_VSPOLARITY_LOW;      
	DCMI_Handler.Init.HSPolarity=DCMI_HSPOLARITY_LOW;      
	DCMI_Handler.Init.CaptureRate=DCMI_CR_ALL_FRAME;        
	DCMI_Handler.Init.ExtendedDataMode=DCMI_EXTEND_DATA_8B; 
	HAL_DCMI_Init(&DCMI_Handler);                           
	DCMI->IER=0x0;										
	__HAL_DCMI_ENABLE_IT(&DCMI_Handler,DCMI_IT_FRAME);
	__HAL_DCMI_ENABLE(&DCMI_Handler);						//使能DCMI
	
}
//DCMI DMA配置
void dcmi_dma_init(uint32_t mem0addr,uint32_t mem1addr,uint16_t memsize,uint32_t memblen,uint32_t meminc)
{
	#if defined(STM32F4) || defined(STM32F7)
	__HAL_RCC_DMA2_CLK_ENABLE();  
	DMADMCI_Handler.Init.Channel=DMA_CHANNEL_1;
	#elif defined(STM32H7)
	__HAL_RCC_DMA1_CLK_ENABLE();
	__HAL_DMA_DISABLE_IT(&DMADMCI_Handler,DMA_IT_TC);
	DMADMCI_Handler.Init.Request=DMA_REQUEST_DCMI;					//DCMI的DMA请求
	#endif

    DMADMCI_Handler.Instance=DCMI_DMA_STREAM;
    DMADMCI_Handler.Init.Direction=DMA_PERIPH_TO_MEMORY;         
    DMADMCI_Handler.Init.PeriphInc=DMA_PINC_DISABLE;               
    DMADMCI_Handler.Init.MemInc=meminc;    

    DMADMCI_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_WORD;  
    DMADMCI_Handler.Init.MemDataAlignment=memblen;               
    DMADMCI_Handler.Init.Mode=DMA_CIRCULAR;                    
    DMADMCI_Handler.Init.Priority=DMA_PRIORITY_HIGH;//=DMA_PRIORITY_LOW;
    DMADMCI_Handler.Init.FIFOMode=DMA_FIFOMODE_ENABLE; 
    DMADMCI_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_HALFFULL;
    DMADMCI_Handler.Init.MemBurst=DMA_MBURST_SINGLE;            
    DMADMCI_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;         
	__HAL_LINKDMA(&DCMI_Handler,DMA_Handle,DMADMCI_Handler);      

  	HAL_DMA_DeInit(&DMADMCI_Handler);                            
    HAL_DMA_Init(&DMADMCI_Handler);	                             
    
    __HAL_UNLOCK(&DMADMCI_Handler);
    if(mem1addr==0)    
    {
        HAL_DMA_Start(&DMADMCI_Handler,(uint32_t)&DCMI->DR,mem0addr,memsize);
    }
    else            
    {
		HAL_DMAEx_MultiBufferStart(&DMADMCI_Handler,(uint32_t)&DCMI->DR,mem0addr,mem1addr,memsize);
		__HAL_DMA_ENABLE_IT(&DMADMCI_Handler,DMA_IT_TC); 
		HAL_NVIC_SetPriority(DCMI_DMA_STREAM_IRQ,1,2);       
		HAL_NVIC_EnableIRQ(DCMI_DMA_STREAM_IRQ);
    }
}


//DCMI设置显示窗口
void dcmi_set_window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{
	DCMI_Stop();
	#if MICROPY_HW_LCD43M
	if(lcddev.type == LCD43M){
		LCD_Clear(BLACK);
		LCD_Set_Window(sx,sy,width,height);
		LCD_SetCursor(sx,sy);
	}
	#endif
	ov2640_outsize_set(width,height);	
}

void sw_ov2640_mode(void)
{
	mp_hal_pin_low(MICROPY_HW_DCMI_PWDN); //
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_DCMI_D2, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_DCMI_D2);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_DCMI_D3, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_DCMI_D3);
	mp_hal_pin_config_alt_static_speed(MICROPY_HW_DCMI_D4, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_DCMI_D4);
} 

void sw_sdcard_mode(void)
{
	GPIO_InitTypeDef GPIO_Initure;

	mp_hal_pin_high(MICROPY_HW_DCMI_PWDN);

	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_11);

	GPIO_Initure.Pin=GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_11;  
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;      
	GPIO_Initure.Pull=GPIO_PULLUP;       
	GPIO_Initure.Alternate=GPIO_AF12_SDMMC1;  
	HAL_GPIO_Init(GPIOC,&GPIO_Initure); 
}

void ov2640_rgb565_mode(void)
{
	uint16_t i=0;
	int res;

	for(i=0;i<(sizeof(rgb565_regs)/2);i++)
	{
		res = sccb_write_reg(rgb565_regs[i][0],rgb565_regs[i][1]); 
		if(res != 2){
			ov_printf("config rgb565 err:%d\n",i);
		}
	}
}

void ov2640_jpeg_mode(void)
{
	uint16_t i=0;

	for(i=0;i<(sizeof(yuv422_regs)/2);i++)
	{
		sccb_write_reg(yuv422_regs[i][0],yuv422_regs[i][1]); 
	}

	for(i=0;i<(sizeof(jpeg_regs)/2);i++)
	{
		sccb_write_reg(jpeg_regs[i][0],jpeg_regs[i][1]);  
	}
}

//设置图像输出大小
uint8_t ov2640_outsize_set(uint16_t width,uint16_t height)
{
	uint16_t outh;
	uint16_t outw;
	uint8_t temp; 
	if(width%4)return 1;
	if(height%4)return 2;
	outw=width/4;
	outh=height/4; 
	sccb_write_reg(0XFF,0X00);	
	sccb_write_reg(0XE0,0X04);			
	sccb_write_reg(0X5A,outw&0XFF);	
	sccb_write_reg(0X5B,outh&0XFF);		
	temp=(outw>>8)&0X03;
	temp|=(outh>>6)&0X04;
	sccb_write_reg(0X5C,temp);			
	sccb_write_reg(0XE0,0X00);	
	return 0;
}
//设置图像尺寸大小,输出分辨率
uint8_t ov2640_imagesize_set(uint16_t width,uint16_t height)
{ 
	uint8_t temp; 
	sccb_write_reg(0XFF,0X00);			
	sccb_write_reg(0XE0,0X04);			
	sccb_write_reg(0XC0,(width)>>3&0XFF);		//设置HSIZE的10:3位
	sccb_write_reg(0XC1,(height)>>3&0XFF);		//设置VSIZE的10:3位
	temp=(width&0X07)<<3;
	temp|=height&0X07;
	temp|=(width>>4)&0X80; 
	sccb_write_reg(0X8C,temp);	
	sccb_write_reg(0XE0,0X00);				 
	return 0;
}
//设置图像开窗大小
uint8_t ov2640_imagewin_set(uint16_t offx,uint16_t offy,uint16_t width,uint16_t height)
{
	uint16_t hsize;
	uint16_t vsize;
	uint8_t temp; 
	if(width%4)return 1;
	if(height%4)return 2;
	hsize=width/4;
	vsize=height/4;
	sccb_write_reg(0XFF,0X00);	
	sccb_write_reg(0XE0,0X04);					
	sccb_write_reg(0X51,hsize&0XFF);	
	sccb_write_reg(0X52,vsize&0XFF);	
	sccb_write_reg(0X53,offx&0XFF);	
	sccb_write_reg(0X54,offy&0XFF);	
	temp=(vsize>>1)&0X80;
	temp|=(offy>>4)&0X70;
	temp|=(hsize>>5)&0X08;
	temp|=(offx>>8)&0X07; 
	sccb_write_reg(0X55,temp);				
	sccb_write_reg(0X57,(hsize>>2)&0X80);	
	sccb_write_reg(0XE0,0X00);	
	return 0;
} 
//-----------------------------------------------------------------------
STATIC void setframesize(void)
{
	sw_ov2640_mode();	
	mp_hal_delay_ms(10);	

	uint16_t w = resolution[framesize][0];
	uint16_t h = resolution[framesize][1];
	uint8_t data[2] = {0};

	if ((w <= VGA_WIDTH) && (h <= VGA_HEIGHT)) {
	#if defined(STM32F4)
	//HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_PLLCLK, RCC_MCODIV_4);//42M
	#endif
		for(uint16_t i=0;i<sizeof(svga_regs)/2;i++)
		{
			data[0] = svga_regs[i][0]; //
			data[1] = svga_regs[i][1]; //
			i2c_writeto(SCCB_I2C, OV2640_ADDR, data, 2, true);
		}
	} else {
		#if defined(STM32F4)
		//HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_PLLCLK, RCC_MCODIV_5);//33M
		#endif
		for(uint16_t i=0;i<sizeof(default_regs)/2;i++)
		{
			data[0] = default_regs[i][0]; //
			data[1] = default_regs[i][1]; //
			i2c_writeto(SCCB_I2C, OV2640_ADDR, data, 2, true);
		}
	}
			
}
//-----------------------------------------------------------------------
STATIC uint8_t ov2640_jpg_photo(const TCHAR *pname)
{
	uint16_t i;
	uint8_t res=0;
	UINT bwr;
	uint8_t *pbuf;
	uint16_t sensor_w = 0;
	uint16_t sensor_h = 0;
	ov2640_mode = 1;

	DCMI_Stop(); 	
	sw_ov2640_mode(); 	

	uint16_t w = resolution[framesize][0];
	uint16_t h = resolution[framesize][1];

	if ((w <= VGA_WIDTH) && (h <= VGA_HEIGHT)) {
		sensor_w = SVGA_WIDTH;
		sensor_h = SVGA_HEIGHT;
	} else {
		sensor_w = UXGA_WIDTH;
		sensor_h = UXGA_HEIGHT;
	}

	dcmi_rx_callback=jpeg_dcmi_rx_callback;
	dcmi_init();	
	dcmi_dma_init((uint32_t)jpeg_buf0,(uint32_t)jpeg_buf1,jpeg_dma_bufsize,DMA_MDATAALIGN_WORD,DMA_MINC_ENABLE);
	ov2640_jpeg_mode();	
	ov2640_imagewin_set(0,0,sensor_w,sensor_h);
	ov2640_outsize_set(w,h);

	DCMI_Start(); 	
	for(uint8_t j=0; j<10 ; j++ ){
		while(jpeg_data_ok!=1);	
		jpeg_data_ok=2;	
	}
	
	while(jpeg_data_ok!=1);	
	DCMI_Stop(); 		
	
	ov2640_mode = 0;
	sw_sdcard_mode();

	const char *file_path = mp_obj_str_get_str(get_path(pname ,&res));

	mp_vfs_mount_t *vfs = MP_STATE_VM(vfs_mount_table);
	if(res == 1){
		vfs = vfs->next;
	}
	fs_user_mount_t *vfs_fat = MP_OBJ_TO_PTR(vfs->obj);
	
	//printf("jpeg data size:%d\r\n",jpeg_data_len*4);

	res=f_open(&vfs_fat->fatfs,&ov2640_fp,file_path,FA_WRITE|FA_CREATE_ALWAYS);	 
	if(res==0){
		pbuf=(uint8_t*)jpeg_data_buf;
		for(i=0;i<jpeg_data_len*4;i++){
			if((pbuf[i]==0XFF)&&(pbuf[i+1]==0XD8))break;
		}
		if(i==jpeg_data_len*4){
			ov_printf("no find jpg head \n");
			res=0XFD;
		}
		else{
			pbuf+=i;
			res=f_write(&ov2640_fp,pbuf,jpeg_data_len*4-i,&bwr);
			if(bwr!=(jpeg_data_len*4-i))res=0XFE; 
		}
	}else {
		ov_printf("open file err\n");
	}
	jpeg_data_len=0;
	f_close(&ov2640_fp); 
	
	if(is_display){
		if(w > lcddev.width) 	display_w = lcddev.width;
		if(h > lcddev.height) display_h = lcddev.height;
		uint16_t sx = (lcddev.width - display_w)>>1;
		uint16_t sy = (lcddev.height - display_h)>>1;
		sw_ov2640_mode();
		ov2640_rgb565_mode(); 
		dcmi_init();			

		#if MICROPY_HW_LCD43M
		if(lcddev.type == LCD43M){
			dcmi_dma_init((uint32_t)&LCD43M_RAM,0,1,DMA_MDATAALIGN_HALFWORD,DMA_MINC_DISABLE);//DCMI DMA配置 
		}
		#endif
		#if MICROPY_HW_LTDC_LCD
		if(lcddev.type == LCD7R || lcddev.type == LCD43R){
			dcmi_rx_callback=ltdc_dcmi_rx_callback;
			dcmi_dma_init((uint32_t)jpeg_buf0,(uint32_t)jpeg_buf1,display_w>>1,DMA_MDATAALIGN_HALFWORD,DMA_MINC_ENABLE);//DCMI DMA配置  
		}
		#endif

		ov2640_imagewin_set(0,0,sensor_w,sensor_h);//1:1真实尺寸
		
		dcmi_set_window(sx,sy,display_w,display_h); 
	
		DCMI_Start(); 		
	}
	return res;
}
//-----------------------------------------------------------------------
//处理JPEG数据
//当采集完一帧JPEG数据后,调用此函数,切换JPEG BUF.开始下一帧采集.
void jpeg_data_process(void)
{
	uint16_t i;
	uint16_t rlen;
	uint32_t *pbuf;
	if(ov2640_mode)
	{
		if(jpeg_data_ok==0)	{
			__HAL_DMA_DISABLE(&DMADMCI_Handler);		
			while(DCMI_DMA_STREAM->CR&0X01);	   
			rlen=jpeg_dma_bufsize-__HAL_DMA_GET_COUNTER(&DMADMCI_Handler);	
			pbuf=jpeg_data_buf+jpeg_data_len;
			if(DCMI_DMA_STREAM->CR&(1<<19))for(i=0;i<rlen;i++)pbuf[i]=jpeg_buf1[i];
			else for(i=0;i<rlen;i++)pbuf[i]=jpeg_buf0[i];
			jpeg_data_len+=rlen;		
			jpeg_data_ok=1; 			
		}
		if(jpeg_data_ok==2)	{ 
			__HAL_DMA_SET_COUNTER(&DMADMCI_Handler,jpeg_dma_bufsize);
			__HAL_DMA_ENABLE(&DMADMCI_Handler); 
			jpeg_data_ok=0;					
			jpeg_data_len=0;			
		}
	}
}

//=======================================================================================================
STATIC mp_obj_t sensor_ov2640_reset(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
	uint8_t data[2] = {BANK_SEL, BANK_SEL_SENSOR}; //
	int res = i2c_writeto(SCCB_I2C, OV2640_ADDR, data, 2, true);
	data[0] = COM7; data[1] = COM7_SRST;
	res |= i2c_writeto(SCCB_I2C, OV2640_ADDR, data, 2, true);
	for(uint16_t i=0;i<sizeof(default_regs)/2;i++)
	{
		data[0] = default_regs[i][0]; //
		data[1] = default_regs[i][1]; //
		i2c_writeto(SCCB_I2C, OV2640_ADDR, data, 2, true);
	}
			
	mp_hal_delay_ms(300);

	return mp_obj_new_int(res);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(sensor_ov2640_reset_obj, 0, sensor_ov2640_reset);
//=======================================================================================================
STATIC mp_obj_t sensor_ov2640_snapshot(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

  STATIC const mp_arg_t snapshot_args[] = {
    { MP_QSTR_filepath,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
  };

  mp_arg_val_t args[MP_ARRAY_SIZE(snapshot_args)];
  mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(snapshot_args), snapshot_args, args);

    //MP_OBJ_NULL
  if(args[0].u_obj !=MP_OBJ_NULL) {
    mp_buffer_info_t bufinfo;
    if (mp_obj_is_int(args[0].u_obj)) {
      mp_raise_ValueError(MP_ERROR_TEXT("snapshot text parameter error"));
    } else {
		mp_get_buffer_raise(args[0].u_obj, &bufinfo, MP_BUFFER_READ);
		char *str = bufinfo.buf;
		ov2640_jpg_photo(str);
		return mp_obj_new_str(str, strlen(str));
    }
  }else{
     mp_raise_ValueError(MP_ERROR_TEXT("snapshot text parameter is empty"));
  }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(sensor_ov2640_snapshot_obj, 1, sensor_ov2640_snapshot);
//=====================================================================================================
STATIC mp_obj_t sensor_ov2640_setframesize(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
	STATIC const mp_arg_t ov2640_args[] = {
	{ MP_QSTR_framesize,       MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = FRAMESIZE_QQQVGA} },
  };

  mp_arg_val_t args[MP_ARRAY_SIZE(ov2640_args)];
  mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(ov2640_args), ov2640_args, args);

	if(args[0].u_int > FRAMESIZE_INVALID && args[0].u_int < FRAMESIZE_END){
		framesize = args[0].u_int;
		setframesize();
	}else{
		mp_raise_ValueError(MP_ERROR_TEXT("set framesize error"));
	}

	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(sensor_ov2640_setframesize_obj, 1, sensor_ov2640_setframesize);

//----------------------------------------------------------------------------------
#if (MICROPY_ENABLE_TFTLCD || MICROPY_HW_LTDC_LCD)
STATIC mp_obj_t sensor_ov2640_display(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

	static const mp_arg_t ov_args[] = {
		{ MP_QSTR_lcd, MP_ARG_INT, {.u_int = LCD43M} },
	};
	
	mp_arg_val_t args[MP_ARRAY_SIZE(ov_args)];
	mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(ov_args), ov_args, args);

	disp_dir = lcddev.dir;
	
	switch(lcddev.type)
	{
		case LCD43M:
		#if MICROPY_HW_LCD43M
		#if defined(STM32H7)
		HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_PLL1QCLK, RCC_MCODIV_2);//100M/2
		#endif
		LCD_Display_Dir(2);
		LCD_Clear(BLACK);
		#endif
		break;
		case LCD43R:
		#if MICROPY_HW_LCD43R
		set_lcd_dir(3);
		ltdc_clear(BLACK);
		#endif
		break;
		case LCD7R:
		#if MICROPY_HW_LCD7R
		set_lcd_dir(1);
		ltdc_clear(BLACK);
		#endif
		break;
		default :
		break;
	}
	
	display_w = resolution[framesize][0];
	display_h = resolution[framesize][1];

	uint16_t sensor_w = 0;
	uint16_t sensor_h = 0;

	if ((display_w <= VGA_WIDTH) && (display_h <= VGA_HEIGHT)) {
		sensor_w = SVGA_WIDTH;
		sensor_h = SVGA_HEIGHT;
	}else {
		sensor_w = UXGA_WIDTH;
		sensor_h = UXGA_HEIGHT;
	}

	DCMI_Stop();
	if(display_w > lcddev.width) 	display_w = lcddev.width;
	if(display_h > lcddev.height) display_h = lcddev.height;
	display_sx = (lcddev.width - display_w)>>1;
	display_sy = (lcddev.height - display_h)>>1;

	ov2640_rgb565_mode();
	dcmi_init();	//无关项	
	
	#if MICROPY_HW_LCD43M
	if(lcddev.type == LCD43M){
		dcmi_dma_init((uint32_t)&LCD43M_RAM,0,1,DMA_MDATAALIGN_HALFWORD,DMA_MINC_DISABLE);//DCMI DMA配置 h7
	}
	#endif
	
	#if MICROPY_HW_LTDC_LCD
	if(lcddev.type == LCD7R || lcddev.type == LCD43R){
		dcmi_rx_callback=ltdc_dcmi_rx_callback;
		dcmi_dma_init((uint32_t)jpeg_buf0,(uint32_t)jpeg_buf1,display_w>>1,DMA_MDATAALIGN_HALFWORD,DMA_MINC_ENABLE);//DCMI DMA配置  
	}
	#endif

	ov2640_imagewin_set(0,0,sensor_w,sensor_h);
	dcmi_set_window(display_sx,display_sy,display_w,display_h); 
	DCMI_Start(); 		//启动传输
	is_display = true;

	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(sensor_ov2640_display_obj,0, sensor_ov2640_display);
//----------------------------------------------------------------------------------
STATIC mp_obj_t sensor_ov2640_display_stop(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
	
	DCMI_Stop(); 		
	
	ov2640_mode = 0;

	is_display = false;
	lcddev.dir = disp_dir;
	#if MICROPY_HW_LCD43M
	if(lcddev.type == LCD43M){
		LCD_Display_Dir(lcddev.dir);
		LCD_Clear(BLACK);
		#if defined(STM32H7)
		HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_PLL1QCLK, RCC_MCODIV_4);//100M/4
		#endif
	}
	#endif
	
	#if MICROPY_HW_LTDC_LCD
	if(lcddev.type == LCD43R || lcddev.type == LCD7R){
		set_lcd_dir(lcddev.dir);
		ltdc_clear(BLACK);
	}
	#endif
	
	//sw_sdcard_mode();		//切换为SD卡模式

	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(sensor_ov2640_display_stop_obj,0, sensor_ov2640_display_stop);
#endif
//----------------------------------------------------------------------------------
STATIC mp_obj_t sensor_ov2640_hmirror(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
	static const mp_arg_t hmirror_args[] = {
			{ MP_QSTR_value,MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
	};
	mp_arg_val_t args[MP_ARRAY_SIZE(hmirror_args)];
	mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(hmirror_args), hmirror_args, args);

	DCMI_Stop(); 		
	uint8_t get_set = args[0].u_int;

	uint8_t reg;
	int ret = sccb_write_reg( BANK_SEL, BANK_SEL_SENSOR);
	ret |= sccb_read_reg(REG04, &reg);
	if (!get_set) { // 0
		reg &= ~(REG04_VFLIP_IMG | REG04_VREF_EN);
	} else { //1
		reg |= REG04_VFLIP_IMG | REG04_VREF_EN;
	}
	sccb_write_reg( REG04, reg);
	DCMI_Start();
	return mp_obj_new_int(get_set);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(sensor_ov2640_hmirror_obj,0, sensor_ov2640_hmirror);

//----------------------------------------------------------------------------------------------------------
STATIC mp_obj_t sensor_ov2640_vflip(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
	static const mp_arg_t vfilp_args[] = {
			{ MP_QSTR_value,MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
	};
	mp_arg_val_t args[MP_ARRAY_SIZE(vfilp_args)];
	mp_arg_parse_all(n_args-1, pos_args+1, kw_args, MP_ARRAY_SIZE(vfilp_args), vfilp_args, args);

	DCMI_Stop(); 		
	uint8_t get_set = args[0].u_int;

	uint8_t reg;
	int ret = sccb_write_reg(BANK_SEL, BANK_SEL_SENSOR);
	ret |= sccb_read_reg(REG04, &reg);
	
	if (!get_set) { // Already mirrored.
		reg |= REG04_HFLIP_IMG;
	} else {
		reg &= ~REG04_HFLIP_IMG;
	}
	sccb_write_reg( REG04, reg);

	DCMI_Start();
	return mp_obj_new_int(get_set);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(sensor_ov2640_vflip_obj,0, sensor_ov2640_vflip);

//---------------------------------------------------------------------------------
STATIC mp_obj_t sensor_ov2640_fps(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
	uint8_t getfps = fps;
	fps = 0;
	return mp_obj_new_int(getfps);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(sensor_ov2640_fps_obj,0, sensor_ov2640_fps);

//----------------------------------------------------------------------------------
STATIC mp_obj_t sensor_ov2640_deinit(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

	sccb_deinit();
	sw_sdcard_mode();
	#if defined(STM32F4) || defined(STM32F7)
	m_free(jpeg_buf0);
	m_free(jpeg_buf1);
	#endif
	m_free(jpeg_data_buf);
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(sensor_ov2640_deinit_obj,0, sensor_ov2640_deinit);

//==============================================================================
STATIC void sensor_ov2640_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
		ov2640_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<ov2640 ID:%x>", self->sensor_id);
}
//----------------------------------------------------------------------------------
STATIC mp_obj_t sensor_ov2640_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {

	// check arguments
	mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);

	ov2640_init();
	framesize = FRAMESIZE_QQQVGA;
	jpeg_data_buf = m_malloc(300*1024);
	if(jpeg_data_buf == NULL){
		mp_raise_ValueError(MP_ERROR_TEXT("jpeg data malloc error"));
	}
	
	#if defined(STM32F4) || defined(STM32F7)
	jpeg_buf0 = m_malloc(jpeg_dma_bufsize*4);
	jpeg_buf1 = m_malloc(jpeg_dma_bufsize*4);
	if(jpeg_buf0 == NULL || jpeg_buf1 == NULL){
		mp_raise_ValueError(MP_ERROR_TEXT("jpeg malloc error"));
	}
	#endif
	
	ov2640_obj_t *ov2640_obj;
	ov2640_obj = m_new_obj(ov2640_obj_t);
	ov2640_obj->base.type = &sensor_ov2640_type;
	ov2640_obj->sensor_id = ov2640_id;
	return MP_OBJ_FROM_PTR(ov2640_obj);
}

/******************************************************************************/
STATIC const mp_rom_map_elem_t sensor_ov2640_locals_dict_table[] = {
	{ MP_ROM_QSTR(MP_QSTR__name__), MP_ROM_QSTR(MP_QSTR_ov2640) },
	{ MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&sensor_ov2640_deinit_obj) },
	{ MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&sensor_ov2640_reset_obj) },
	{ MP_ROM_QSTR(MP_QSTR_set_framesize), MP_ROM_PTR(&sensor_ov2640_setframesize_obj) },
	{ MP_ROM_QSTR(MP_QSTR_snapshot), MP_ROM_PTR(&sensor_ov2640_snapshot_obj) },
	{ MP_ROM_QSTR(MP_QSTR_fps), MP_ROM_PTR(&sensor_ov2640_fps_obj) },

	{ MP_ROM_QSTR(MP_QSTR_set_hmirror), MP_ROM_PTR(&sensor_ov2640_hmirror_obj) },
	{ MP_ROM_QSTR(MP_QSTR_set_vfilp), MP_ROM_PTR(&sensor_ov2640_vflip_obj) },

	#if (MICROPY_ENABLE_TFTLCD || MICROPY_HW_LTDC_LCD)
	{ MP_ROM_QSTR(MP_QSTR_display), MP_ROM_PTR(&sensor_ov2640_display_obj) },
	{ MP_ROM_QSTR(MP_QSTR_display_stop), MP_ROM_PTR(&sensor_ov2640_display_stop_obj) },
	#endif
};
STATIC MP_DEFINE_CONST_DICT(sensor_ov2640_locals_dict,sensor_ov2640_locals_dict_table);
const mp_obj_type_t sensor_ov2640_type = {
    { &mp_type_type },
    .name = MP_QSTR_OV2640,
    .print = sensor_ov2640_print,
    .make_new = sensor_ov2640_make_new,
    .locals_dict = (mp_obj_dict_t *)&sensor_ov2640_locals_dict,
};

#endif


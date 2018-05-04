#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32l4xx_hal.h"
#include "gc0329.h"

#define ST7789_LCD_PIXEL_WIDTH ((uint16_t)240)
#define ST7789_LCD_PIXEL_HEIGHT ((uint16_t)240)

CAMERA_DrvTypeDef *camera_drv;
DCMI_HandleTypeDef *phdcmi;
uint8_t camera_dis_on = 0;
uint16_t pBuffer[ST7789_LCD_PIXEL_WIDTH * ST7789_LCD_PIXEL_HEIGHT];
extern void camera_dispaly(uint16_t *data, uint32_t pixel_num);
extern void BSP_LCD_Clear(uint16_t Color);

/**
* @brief This function handles DMA2 channel6 global interrupt.
*/
void DMA2_Channel6_IRQHandler(void)
{
  krhino_intrpt_enter();
  HAL_DMA_IRQHandler(hdcmi.DMA_Handle);
  krhino_intrpt_exit();
}

/**
* @brief This function handles DCMI global interrupt.
*/
void DCMI_IRQHandler(void)
{
  krhino_intrpt_enter();
  HAL_DCMI_IRQHandler(&hdcmi);
  krhino_intrpt_exit();
}

void CAMERA_Init(uint32_t Resolution)
{
	camera_drv = &gc0329_drv;
	phdcmi = &hdcmi;
	
	 /* Camera Module Initialization via I2C to the wanted 'Resolution' */
    if (Resolution == CAMERA_R640x480)
    {
      /* For 240x240 resolution, the OV9655 sensor is set to QVGA resolution
       * as OV9655 doesn't supports 240x240  resolution,
       * then DCMI is configured to output a 240x240 cropped window */
      camera_drv->Init(GC0329_I2CADDR, CAMERA_R640x480);


      HAL_DCMI_ConfigCROP(phdcmi,
                          150,                 /* Crop in the middle of the VGA picture */
                          120,                 /* Same height (same number of lines: no need to crop vertically) */
                          (240 * 2) - 1,     /* 2 pixels clock needed to capture one pixel */
                          (240 * 1) - 1);    /* All 240 lines are captured */
      HAL_DCMI_EnableCROP(phdcmi);
    }
}

void CameraDEMO_Main(void)
{
	uint8_t  sensor_id;
	HAL_StatusTypeDef hal_status = HAL_OK;
	
	printf("CameraDEMO_Main\n");
	gc0329_power_onoff(1);
	sensor_id = gc0329_ReadID();
	printf("sensor_id = 0x%x\n", sensor_id);
	//gc0329_power_onoff(0);
	
	if(sensor_id == GC0329_ID){
		CAMERA_Init(CAMERA_R640x480);

		/* Wait 1s to let auto-loops in the camera module converge and lead to correct exposure */
 		// HAL_Delay(1000);
  		 krhino_task_sleep(krhino_ms_to_ticks(1000));
		  /*##-4- Camera Continuous capture start in QVGA resolution ############################*/
		  /* Disable unwanted HSYNC (IT_LINE)/VSYNC interrupts */
		  __HAL_DCMI_DISABLE_IT(phdcmi, DCMI_IT_LINE | DCMI_IT_VSYNC);

		  /* LCD size is 240 x 240 and format is RGB565 i.e. 16 bpp or 2 bytes/pixel. 
		     The LCD frame size is therefore 240 * 240 half-words of (240*240)/2 32-bit long words . 
		     Since the DMA associated to DCMI IP is configured in  BSP_CAMERA_MspInit() of stm32l496g_discovery_camera.c file
		     with words alignment, the last parameter of HAL_DCMI_Start_DMA is set to:
		      (ST7789H2_LCD_PIXEL_WIDTH*ST7789H2_LCD_PIXEL_HEIGHT)/2, that is 240 * 240 / 2
		   */   
		  hal_status = HAL_DCMI_Start_DMA(phdcmi, DCMI_MODE_CONTINUOUS,  (uint32_t)pBuffer , (ST7789_LCD_PIXEL_WIDTH* ST7789_LCD_PIXEL_HEIGHT)/2 );
		 // OnError_Handler(hal_status != HAL_OK); 
	}	
}

void GC0329_CAMERA_FrameEventCallback(void)
{
	if(camera_dis_on)
		camera_dispaly(pBuffer, (ST7789_LCD_PIXEL_WIDTH* ST7789_LCD_PIXEL_HEIGHT));
}

/**
  * @brief  Frame event callback
  * @param  hdcmi: pointer to the DCMI handle
  * @retval None
  */
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
  GC0329_CAMERA_FrameEventCallback();
}



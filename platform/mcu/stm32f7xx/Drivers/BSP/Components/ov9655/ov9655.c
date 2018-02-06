/**
  ******************************************************************************
  * @file    ov9655.c
  * @author  MCD Application Team
  * @brief   This file provides the OV9655 camera driver
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "ov9655.h"

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup Components
  * @{
  */ 
  
/** @addtogroup OV9655
  * @brief     This file provides a set of functions needed to drive the 
  *            OV9655 Camera module.
  * @{
  */

/** @defgroup OV9655_Private_TypesDefinitions
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup OV9655_Private_Defines
  * @{
  */

/**
  * @}
  */ 
  
/** @defgroup OV9655_Private_Macros
  * @{
  */
     
/**
  * @}
  */  
  
/** @defgroup OV9655_Private_FunctionPrototypes
  * @{
  */
static uint64_t ov9655_ConvertValue(uint32_t feature, uint32_t value);
/**
  * @}
  */ 
  
/** @defgroup OV9655_Private_Variables
  * @{
  */        

CAMERA_DrvTypeDef   ov9655_drv = 
{
  ov9655_Init,
  ov9655_ReadID,  
  ov9655_Config,
};

/* Initialization sequence for VGA resolution (640x480)*/
const unsigned char OV9655_VGA[][2]=
{
  {0x00, 0x00},
  {0x01, 0x80},   
  {0x02, 0x80},   
  {0xb5, 0x00},   
  {0x35, 0x00},   
  {0xa8, 0xc1},   
  {0x3a, 0xcc},   
  {0x3d, 0x99},   
  {0x77, 0x02},   
  {0x13, 0xe7},   
  {0x26, 0x72},   
  {0x27, 0x08},   
  {0x28, 0x08},   
  {0x2c, 0x08},   
  {0xab, 0x04},   
  {0x6e, 0x00},   
  {0x6d, 0x55},   
  {0x00, 0x11},   
  {0x10, 0x7b},   
  {0xbb, 0xae},   
  {0x11, 0x03},   
  {0x72, 0x00},   
  {0x3e, 0x0c},   
  {0x74, 0x3a},   
  {0x76, 0x01},   
  {0x75, 0x35},   
  {0x73, 0x00},   
  {0xc7, 0x80},   
  {0x62, 0x00},   
  {0x63, 0x00},   
  {0x64, 0x02},   
  {0x65, 0x20},   
  {0x66, 0x01},   
  {0xc3, 0x4e},   
  {0x33, 0x00},   
  {0xa4, 0x50},   
  {0xaa, 0x92},   
  {0xc2, 0x01},   
  {0xc1, 0xc8},   
  {0x1e, 0x04},   
  {0xa9, 0xef},   
  {0x0e, 0x61},   
  {0x39, 0x57},   
  {0x0f, 0x48},
  {0x24, 0x3c},   
  {0x25, 0x36},   
  {0x12, 0x63},   
  {0x03, 0x12},   
  {0x32, 0xff},   
  {0x17, 0x16},   
  {0x18, 0x02},   
  {0x19, 0x01},   
  {0x1a, 0x3d},   
  {0x36, 0xfa},   
  {0x69, 0x0a},   
  {0x8c, 0x8d},   
  {0xc0, 0xaa},   
  {0x40, 0xd0},   
  {0x43, 0x14},   
  {0x44, 0xf0},   
  {0x45, 0x46},   
  {0x46, 0x62},   
  {0x47, 0x2a},   
  {0x48, 0x3c},   
  {0x59, 0x85},   
  {0x5a, 0xa9},   
  {0x5b, 0x64},   
  {0x5c, 0x84},   
  {0x5d, 0x53},   
  {0x5e, 0x0e},   
  {0x6c, 0x0c},   
  {0xc6, 0x85},   
  {0xcb, 0xf0},   
  {0xcc, 0xd8},   
  {0x71, 0x78},   
  {0xa5, 0x68},   
  {0x6f, 0x9e},   
  {0x42, 0xc0},   
  {0x3f, 0x82},   
  {0x8a, 0x23},   
  {0x14, 0x3a},   
  {0x3b, 0xcc},   
  {0x34, 0x3d},   
  {0x41, 0x40},   
  {0xc9, 0xe0},   
  {0xca, 0xe8},   
  {0xcd, 0x93},   
  {0x7a, 0x20},   
  {0x7b, 0x1c},   
  {0x7c, 0x28},   
  {0x7d, 0x3c},   
  {0x7e, 0x5a},   
  {0x7f, 0x68},   
  {0x80, 0x76},   
  {0x81, 0x80},   
  {0x82, 0x88},   
  {0x83, 0x8f},   
  {0x84, 0x96},   
  {0x85, 0xa3},   
  {0x86, 0xaf},   
  {0x87, 0xc4},   
  {0x88, 0xd7},   
  {0x89, 0xe8},   
  {0x4f, 0x98},   
  {0x50, 0x98},   
  {0x51, 0x00},   
  {0x52, 0x28},   
  {0x53, 0x70},   
  {0x54, 0x98},   
  {0x58, 0x1a},   
  {0x6b, 0x5a},   
  {0x90, 0x92},   
  {0x91, 0x92},   
  {0x9f, 0x90},   
  {0xa0, 0x90},   
  {0x16, 0x24},   
  {0x2a, 0x00},   
  {0x2b, 0x00},   
  {0xac, 0x80},   
  {0xad, 0x80},   
  {0xae, 0x80},   
  {0xaf, 0x80},   
  {0xb2, 0xf2},   
  {0xb3, 0x20},   
  {0xb4, 0x20},   
  {0xb6, 0xaf},   
  {0x29, 0x15},   
  {0x9d, 0x02},   
  {0x9e, 0x02},   
  {0x9e, 0x02},   
  {0x04, 0x03},   
  {0x05, 0x2e},   
  {0x06, 0x2e},   
  {0x07, 0x2e},   
  {0x08, 0x2e},   
  {0x2f, 0x2e},   
  {0x4a, 0xe9},   
  {0x4b, 0xdd},   
  {0x4c, 0xdd},   
  {0x4d, 0xdd},   
  {0x4e, 0xdd},   
  {0x70, 0x06},   
  {0xa6, 0x40},   
  {0xbc, 0x02},   
  {0xbd, 0x01},   
  {0xbe, 0x02},   
  {0xbf, 0x01},
};

/* Initialization sequence for QVGA resolution (320x240) */
const unsigned char OV9655_QVGA[][2]=
{
  {0x00, 0x00},
  {0x01, 0x80},
  {0x02, 0x80},
  {0x03, 0x02},
  {0x04, 0x03},
  {0x09, 0x01},
  {0x0b, 0x57},
  {0x0e, 0x61},
  {0x0f, 0x40},
  {0x11, 0x01},
  {0x12, 0x62},
  {0x13, 0xc7},
  {0x14, 0x3a},
  {0x16, 0x24},
  {0x17, 0x18},
  {0x18, 0x04},
  {0x19, 0x01},
  {0x1a, 0x81},
  {0x1e, 0x00},
  {0x24, 0x3c},
  {0x25, 0x36},
  {0x26, 0x72},
  {0x27, 0x08},
  {0x28, 0x08},
  {0x29, 0x15},
  {0x2a, 0x00},
  {0x2b, 0x00},
  {0x2c, 0x08},
  {0x32, 0x12},
  {0x33, 0x00},
  {0x34, 0x3f},
  {0x35, 0x00},
  {0x36, 0x3a},
  {0x38, 0x72},
  {0x39, 0x57},
  {0x3a, 0xcc},
  {0x3b, 0x04},
  {0x3d, 0x99},
  {0x3e, 0x02},
  {0x3f, 0xc1},
  {0x40, 0xc0},
  {0x41, 0x41},
  {0x42, 0xc0},
  {0x43, 0x0a},
  {0x44, 0xf0},
  {0x45, 0x46},
  {0x46, 0x62},
  {0x47, 0x2a},
  {0x48, 0x3c},
  {0x4a, 0xfc},
  {0x4b, 0xfc},
  {0x4c, 0x7f},
  {0x4d, 0x7f},
  {0x4e, 0x7f},
  {0x4f, 0x98},
  {0x50, 0x98},
  {0x51, 0x00},
  {0x52, 0x28},
  {0x53, 0x70},
  {0x54, 0x98},
  {0x58, 0x1a},
  {0x59, 0x85},
  {0x5a, 0xa9},
  {0x5b, 0x64},
  {0x5c, 0x84},
  {0x5d, 0x53},
  {0x5e, 0x0e},
  {0x5f, 0xf0},
  {0x60, 0xf0},
  {0x61, 0xf0},
  {0x62, 0x00},
  {0x63, 0x00},
  {0x64, 0x02},
  {0x65, 0x20},
  {0x66, 0x00},
  {0x69, 0x0a},
  {0x6b, 0x5a},
  {0x6c, 0x04},
  {0x6d, 0x55},
  {0x6e, 0x00},
  {0x6f, 0x9d},
  {0x70, 0x21},
  {0x71, 0x78},
  {0x72, 0x11},
  {0x73, 0x01},
  {0x74, 0x10},
  {0x75, 0x10},
  {0x76, 0x01},
  {0x77, 0x02},
  {0x7A, 0x12},
  {0x7B, 0x08},
  {0x7C, 0x16},
  {0x7D, 0x30},
  {0x7E, 0x5e},
  {0x7F, 0x72},
  {0x80, 0x82},
  {0x81, 0x8e},
  {0x82, 0x9a},
  {0x83, 0xa4},
  {0x84, 0xac},
  {0x85, 0xb8},
  {0x86, 0xc3},
  {0x87, 0xd6},
  {0x88, 0xe6},
  {0x89, 0xf2},
  {0x8a, 0x24},
  {0x8c, 0x80},
  {0x90, 0x7d},
  {0x91, 0x7b},
  {0x9d, 0x02},
  {0x9e, 0x02},
  {0x9f, 0x7a},
  {0xa0, 0x79},
  {0xa1, 0x40},
  {0xa4, 0x50},
  {0xa5, 0x68},
  {0xa6, 0x4a},
  {0xa8, 0xc1},
  {0xa9, 0xef},
  {0xaa, 0x92},
  {0xab, 0x04},
  {0xac, 0x80},
  {0xad, 0x80},
  {0xae, 0x80},
  {0xaf, 0x80},
  {0xb2, 0xf2},
  {0xb3, 0x20},
  {0xb4, 0x20},
  {0xb5, 0x00},
  {0xb6, 0xaf},
  {0xb6, 0xaf},
  {0xbb, 0xae},
  {0xbc, 0x7f},
  {0xbd, 0x7f},
  {0xbe, 0x7f},
  {0xbf, 0x7f},
  {0xbf, 0x7f},
  {0xc0, 0xaa},
  {0xc1, 0xc0},
  {0xc2, 0x01},
  {0xc3, 0x4e},
  {0xc6, 0x05},
  {0xc7, 0x81},
  {0xc9, 0xe0},
  {0xca, 0xe8},
  {0xcb, 0xf0},
  {0xcc, 0xd8},
  {0xcd, 0x93},
  {0x12, 0x63},
  {0x40, 0x10},
};

/* Initialization sequence for QQVGA resolution (160x120) */
const char OV9655_QQVGA[][2]=
{
  {0x00, 0x00},
  {0x01, 0x80},
  {0x02, 0x80},
  {0x03, 0x02},
  {0x04, 0x03},
  {0x09, 0x01},
  {0x0b, 0x57},
  {0x0e, 0x61},
  {0x0f, 0x40},
  {0x11, 0x01},
  {0x12, 0x62},
  {0x13, 0xc7},
  {0x14, 0x3a},
  {0x16, 0x24},
  {0x17, 0x18},
  {0x18, 0x04},
  {0x19, 0x01},
  {0x1a, 0x81},
  {0x1e, 0x00},
  {0x24, 0x3c},
  {0x25, 0x36},
  {0x26, 0x72},
  {0x27, 0x08},
  {0x28, 0x08},
  {0x29, 0x15},
  {0x2a, 0x00},
  {0x2b, 0x00},
  {0x2c, 0x08},
  {0x32, 0xa4},
  {0x33, 0x00},
  {0x34, 0x3f},
  {0x35, 0x00},
  {0x36, 0x3a},
  {0x38, 0x72},
  {0x39, 0x57},
  {0x3a, 0xcc},
  {0x3b, 0x04},
  {0x3d, 0x99},
  {0x3e, 0x0e},
  {0x3f, 0xc1},
  {0x40, 0xc0},
  {0x41, 0x41},
  {0x42, 0xc0},
  {0x43, 0x0a},
  {0x44, 0xf0},
  {0x45, 0x46},
  {0x46, 0x62},
  {0x47, 0x2a},
  {0x48, 0x3c},
  {0x4a, 0xfc},
  {0x4b, 0xfc},
  {0x4c, 0x7f},
  {0x4d, 0x7f},
  {0x4e, 0x7f},
  {0x4f, 0x98},
  {0x50, 0x98},
  {0x51, 0x00},
  {0x52, 0x28},
  {0x53, 0x70},
  {0x54, 0x98},
  {0x58, 0x1a},
  {0x59, 0x85},
  {0x5a, 0xa9},
  {0x5b, 0x64},
  {0x5c, 0x84},
  {0x5d, 0x53},
  {0x5e, 0x0e},
  {0x5f, 0xf0},
  {0x60, 0xf0},
  {0x61, 0xf0},
  {0x62, 0x00},
  {0x63, 0x00},
  {0x64, 0x02},
  {0x65, 0x20},
  {0x66, 0x00},
  {0x69, 0x0a},
  {0x6b, 0x5a},
  {0x6c, 0x04},
  {0x6d, 0x55},
  {0x6e, 0x00},
  {0x6f, 0x9d},
  {0x70, 0x21},
  {0x71, 0x78},
  {0x72, 0x22},
  {0x73, 0x02},
  {0x74, 0x10},
  {0x75, 0x10},
  {0x76, 0x01},
  {0x77, 0x02},
  {0x7A, 0x12},
  {0x7B, 0x08},
  {0x7C, 0x16},
  {0x7D, 0x30},
  {0x7E, 0x5e},
  {0x7F, 0x72},
  {0x80, 0x82},
  {0x81, 0x8e},
  {0x82, 0x9a},
  {0x83, 0xa4},
  {0x84, 0xac},
  {0x85, 0xb8},
  {0x86, 0xc3},
  {0x87, 0xd6},
  {0x88, 0xe6},
  {0x89, 0xf2},
  {0x8a, 0x24},
  {0x8c, 0x80},
  {0x90, 0x7d},
  {0x91, 0x7b},
  {0x9d, 0x02},
  {0x9e, 0x02},
  {0x9f, 0x7a},
  {0xa0, 0x79},
  {0xa1, 0x40},
  {0xa4, 0x50},
  {0xa5, 0x68},
  {0xa6, 0x4a},
  {0xa8, 0xc1},
  {0xa9, 0xef},
  {0xaa, 0x92},
  {0xab, 0x04},
  {0xac, 0x80},
  {0xad, 0x80},
  {0xae, 0x80},
  {0xaf, 0x80},
  {0xb2, 0xf2},
  {0xb3, 0x20},
  {0xb4, 0x20},
  {0xb5, 0x00},
  {0xb6, 0xaf},
  {0xb6, 0xaf},
  {0xbb, 0xae},
  {0xbc, 0x7f},
  {0xbd, 0x7f},
  {0xbe, 0x7f},
  {0xbf, 0x7f},
  {0xbf, 0x7f},
  {0xc0, 0xaa},
  {0xc1, 0xc0},
  {0xc2, 0x01},
  {0xc3, 0x4e},
  {0xc6, 0x05},
  {0xc7, 0x82},
  {0xc9, 0xe0},
  {0xca, 0xe8},
  {0xcb, 0xf0},
  {0xcc, 0xd8},
  {0xcd, 0x93},
  {0x12, 0x63},
  {0x40, 0x10},
};

/**
  * @}
  */
  
/** @defgroup OV9655_Private_Functions
  * @{
  */ 
  
/**
  * @brief  Initializes the OV9655 CAMERA component.
  * @param  DeviceAddr: Device address on communication Bus.
  * @param  resolution: Camera resolution
  * @retval None
  */
void ov9655_Init(uint16_t DeviceAddr, uint32_t resolution)
{
  uint32_t index;
  
  /* Initialize I2C */
  CAMERA_IO_Init();    
  
  /* Prepare the camera to be configured by resetting all its registers */
  CAMERA_IO_Write(DeviceAddr, OV9655_SENSOR_COM7, 0x80);
  CAMERA_Delay(200);
  
  /* Initialize OV9655 */
  switch (resolution)
  {
  case CAMERA_R160x120:
    {
      for(index=0; index<(sizeof(OV9655_QQVGA)/2); index++)
      {
        CAMERA_IO_Write(DeviceAddr, OV9655_QQVGA[index][0], OV9655_QQVGA[index][1]);
        CAMERA_Delay(2);
      } 
      break;
    }    
  case CAMERA_R320x240:
    {
      for(index=0; index<(sizeof(OV9655_QVGA)/2); index++)
      {
        CAMERA_IO_Write(DeviceAddr, OV9655_QVGA[index][0], OV9655_QVGA[index][1]);
        CAMERA_Delay(2);
      } 
      break;
    }
  case CAMERA_R480x272:
    {
      /* Not supported resolution */
      break;
    }
  case CAMERA_R640x480:
    {
      for(index=0; index<(sizeof(OV9655_VGA)/2); index++)
      {
        CAMERA_IO_Write(DeviceAddr, OV9655_VGA[index][0], OV9655_VGA[index][1]);
        CAMERA_Delay(2);
      }
      break;
    }    
  default:
    {
      break;
    }
  }
}

/**
  * @brief  Configures the OV9655 camera feature.
  * @param  DeviceAddr: Device address on communication Bus.
  * @param  feature: Camera feature to be configured
  * @param  value: Value to be configured
  * @param  brightness_value: Brightness value to be configured
  * @retval None
  */
void ov9655_Config(uint16_t DeviceAddr, uint32_t feature, uint32_t value, uint32_t brightness_value)
{
  uint8_t tslb, mtx1, mtx2, mtx3, mtx4, mtx5, mtx6;
  uint64_t value_tmp;
  uint32_t br_value;
  
  /* Convert the input value into ov9655 parameters */
  value_tmp = ov9655_ConvertValue(feature, value); 
  br_value = (uint32_t)ov9655_ConvertValue(CAMERA_CONTRAST_BRIGHTNESS, brightness_value);
    
  switch(feature)
  {
  case CAMERA_CONTRAST_BRIGHTNESS:
    {
      CAMERA_IO_Write(DeviceAddr, OV9655_SENSOR_BRTN, br_value);
      CAMERA_IO_Write(DeviceAddr, OV9655_SENSOR_CNST1, value_tmp);
      break;
    }
  case CAMERA_BLACK_WHITE:
  case CAMERA_COLOR_EFFECT:
    {     
      tslb = (uint8_t)(value_tmp >> 48);
      mtx1 = (uint8_t)(value_tmp >> 40);
      mtx2 = (uint8_t)(value_tmp >> 32);
      mtx3 = (uint8_t)(value_tmp >> 24);
      mtx4 = (uint8_t)(value_tmp >> 16);
      mtx5 = (uint8_t)(value_tmp >> 8);
      mtx6 = (uint8_t)(value_tmp);
      CAMERA_IO_Write(DeviceAddr, OV9655_SENSOR_TSLB, tslb);
      CAMERA_IO_Write(DeviceAddr, OV9655_SENSOR_MTX1, mtx1);
      CAMERA_IO_Write(DeviceAddr, OV9655_SENSOR_MTX2, mtx2);
      CAMERA_IO_Write(DeviceAddr, OV9655_SENSOR_MTX3, mtx3);
      CAMERA_IO_Write(DeviceAddr, OV9655_SENSOR_MTX4, mtx4);
      CAMERA_IO_Write(DeviceAddr, OV9655_SENSOR_MTX5, mtx5);
      CAMERA_IO_Write(DeviceAddr, OV9655_SENSOR_MTX6, mtx6);
      break;
    }     
  default:
    {
      break;
    }
  }
}

/**
  * @brief  Read the OV9655 Camera identity.
  * @param  DeviceAddr: Device address on communication Bus.
  * @retval the OV9655 ID
  */
uint16_t ov9655_ReadID(uint16_t DeviceAddr)
{
  /* Initialize I2C */
  CAMERA_IO_Init();
  
  /* Get the camera ID */
  return (CAMERA_IO_Read(DeviceAddr, OV9655_SENSOR_PIDH));
}

/******************************************************************************
                            Static Functions
*******************************************************************************/
/**
  * @brief  Convert input values into ov9655 parameters.
  * @param  feature: Camera feature to be configured
  * @param  value: Value to be configured
  * @retval The converted value
  */
static uint64_t ov9655_ConvertValue(uint32_t feature, uint32_t value)
{
  uint64_t ret = 0;
  
  switch(feature)
  {
  case CAMERA_BLACK_WHITE:
    {
      switch(value)
      {
      case CAMERA_BLACK_WHITE_BW:
        {
          ret =  OV9655_BLACK_WHITE_BW;
          break;
        }
      case CAMERA_BLACK_WHITE_NEGATIVE:
        {
          ret =  OV9655_BLACK_WHITE_NEGATIVE;
          break;
        }
      case CAMERA_BLACK_WHITE_BW_NEGATIVE:
        {
          ret =  OV9655_BLACK_WHITE_BW_NEGATIVE;
          break;
        }
      case CAMERA_BLACK_WHITE_NORMAL:
        {
          ret =  OV9655_BLACK_WHITE_NORMAL;
          break;
        }
      default:
        {
          ret =  OV9655_BLACK_WHITE_NORMAL;
          break;
        }
      }
      break;
    }
  case CAMERA_CONTRAST_BRIGHTNESS:
    {
      switch(value)
      {
      case CAMERA_BRIGHTNESS_LEVEL0:
        {
          ret =  OV9655_BRIGHTNESS_LEVEL0;
          break;
        }
      case CAMERA_BRIGHTNESS_LEVEL1:
        {
          ret =  OV9655_BRIGHTNESS_LEVEL1;
          break;
        }
      case CAMERA_BRIGHTNESS_LEVEL2:
        {
          ret =  OV9655_BRIGHTNESS_LEVEL2;
          break;
        }
      case CAMERA_BRIGHTNESS_LEVEL3:
        {
          ret =  OV9655_BRIGHTNESS_LEVEL3;
          break;
        }
      case CAMERA_BRIGHTNESS_LEVEL4:
        {
          ret =  OV9655_BRIGHTNESS_LEVEL4;
          break;
        }        
      case CAMERA_CONTRAST_LEVEL0:
        {
          ret =  OV9655_CONTRAST_LEVEL0;
          break;
        }
      case CAMERA_CONTRAST_LEVEL1:
        {
          ret =  OV9655_CONTRAST_LEVEL1;
          break;
        }
      case CAMERA_CONTRAST_LEVEL2:
        {
          ret =  OV9655_CONTRAST_LEVEL2;
          break;
        }
      case CAMERA_CONTRAST_LEVEL3:
        {
          ret =  OV9655_CONTRAST_LEVEL3;
          break;
        }
      case CAMERA_CONTRAST_LEVEL4:
        {
          ret =  OV9655_CONTRAST_LEVEL4;
          break;
        }
      default:
        {
          ret =  OV9655_CONTRAST_LEVEL0;
          break;
        }
      }
      break;
    }
  case CAMERA_COLOR_EFFECT:
    {
      switch(value)
      {
      case CAMERA_COLOR_EFFECT_ANTIQUE:
        {
          ret =  OV9655_COLOR_EFFECT_ANTIQUE;
          break;
        }
      case CAMERA_COLOR_EFFECT_BLUE:
        {
          ret =  OV9655_COLOR_EFFECT_BLUE;
          break;
        }
      case CAMERA_COLOR_EFFECT_GREEN:
        {
          ret =  OV9655_COLOR_EFFECT_GREEN;
          break;
        }
      case CAMERA_COLOR_EFFECT_RED:
        {
          ret =  OV9655_COLOR_EFFECT_RED;
          break;
        }
      case CAMERA_COLOR_EFFECT_NONE:
      default:
        {
          ret =  OV9655_COLOR_EFFECT_NONE;
          break;
        }
      }
      break;
    default:
      {
        ret = 0;
        break;
      }    
    }
  }
  
  return ret;
}
         
/**
  * @}
  */ 
  
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/**
  * @}
  */  

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

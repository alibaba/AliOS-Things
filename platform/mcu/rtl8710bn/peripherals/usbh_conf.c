/**
  ******************************************************************************
  * @file    FatFs/FatFs_USBDisk_RTOS/Src/usbh_conf.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-March-2014
  * @brief   USB Host configuration file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

#ifdef USE_USB_FS

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_hal.h"
#include "usbh_core.h"
#include "platform_common_config.h"


HCD_HandleTypeDef hhcd;

/*******************************************************************************
                       HCD BSP Routines
*******************************************************************************/
/**
  * @brief  Initializes the HCD MSP.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_MspInit(HCD_HandleTypeDef *hhcd)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  if(hhcd->Instance == USB_OTG_FS)
  {
    /* Configure USB FS GPIOs */
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);

    /* Configure SOF VBUS ID DM DP Pins */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_OTG1_FS) ;
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_OTG1_FS) ;
    
    /* Enable USB FS Clocks */ 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, ENABLE) ;
    
    /* Enable USB FS Clocks */ 
    
    /* Set USBFS Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;//1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* Enable USBFS Interrupt */
  } 
  
  else if(hhcd->Instance == USB_OTG_HS)
  {

  }   
}

/**
  * @brief  DeInitializes the HCD MSP.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_MspDeInit(HCD_HandleTypeDef *hhcd)
{
  if(hhcd->Instance == USB_OTG_FS)
  {  
    /* Disable USB FS Clocks */ 
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, ENABLE) ;
//    __USB_OTG_FS_CLK_DISABLE();
  }
  else if(hhcd->Instance == USB_OTG_HS)
  {  

  }  
}

/*******************************************************************************
                       LL Driver Callbacks (HCD -> USB Host Library)
*******************************************************************************/

/**
  * @brief  SOF callback.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_SOF_Callback(HCD_HandleTypeDef *hhcd)
{
  USBH_LL_IncTimer (hhcd->pData);
}

/**
  * @brief  Connect callback.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_Connect_Callback(HCD_HandleTypeDef *hhcd)
{
  USBH_LL_Connect(hhcd->pData);
}

/**
  * @brief  Disconnect callback.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_Disconnect_Callback(HCD_HandleTypeDef *hhcd)
{
  USBH_LL_Disconnect(hhcd->pData);
} 

/**
  * @brief  Notify URB state change callback.
  * @param  hhcd: HCD handle
  * @param  chnum:
  * @param  urb_state:
  * @retval None
  */
void HAL_HCD_HC_NotifyURBChange_Callback(HCD_HandleTypeDef *hhcd, uint8_t chnum, HCD_URBStateTypeDef urb_state)
{
#if (USBH_USE_OS == 1)   
  USBH_LL_NotifyURBChange(hhcd->pData);
#endif  
}

/*******************************************************************************
                       LL Driver Interface (USB Host Library --> HCD)
*******************************************************************************/

/**
  * @brief  Initializes the Low Level portion of the Host driver.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_Init(USBH_HandleTypeDef *phost)
{  
#ifdef USE_USB_FS  
  /* Set the LL driver parameters */
  hhcd.Instance = USB_OTG_FS;
  hhcd.Init.Host_channels = 11; 
  hhcd.Init.dma_enable = 0;
  hhcd.Init.low_power_enable = 0;
  hhcd.Init.phy_itface = HCD_PHY_EMBEDDED; 
  hhcd.Init.Sof_enable = 0;
  hhcd.Init.speed = HCD_SPEED_FULL;
  hhcd.Init.vbus_sensing_enable = 0;
  /* Link the driver to the stack */
  hhcd.pData = phost;
  phost->pData = &hhcd;
  /* Initialize the LL Driver */
  HAL_HCD_Init(&hhcd);
#endif 
#ifdef USE_USB_HS  
  /* Set the LL driver parameters */
  hhcd.Instance = USB_OTG_HS;
  hhcd.Init.Host_channels = 11; 
  hhcd.Init.dma_enable = 1;
  hhcd.Init.low_power_enable = 0;
  hhcd.Init.phy_itface = HCD_PHY_ULPI; 
  hhcd.Init.Sof_enable = 0;
  hhcd.Init.speed = HCD_SPEED_HIGH;
  hhcd.Init.vbus_sensing_enable = 0;
  hhcd.Init.use_external_vbus = 1;  
  /* Link the driver to the stack */
  hhcd.pData = phost;
  phost->pData = &hhcd;
  /* Initialize the LL driver */
  HAL_HCD_Init(&hhcd);
#endif 
 
  USBH_LL_SetTimer(phost, HAL_HCD_GetCurrentFrame(&hhcd));
  
  return USBH_OK;
}

/**
  * @brief  De-Initializes the Low Level portion of the Host driver.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_DeInit(USBH_HandleTypeDef *phost)
{
  HAL_HCD_DeInit(phost->pData);
  return USBH_OK; 
}

/**
  * @brief  Starts the Low Level portion of the Host driver.   
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_Start(USBH_HandleTypeDef *phost)
{
  HAL_HCD_Start(phost->pData);
  return USBH_OK; 
}

/**
  * @brief  Stops the Low Level portion of the Host driver.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_Stop(USBH_HandleTypeDef *phost)
{
  HAL_HCD_Stop(phost->pData);
  return USBH_OK; 
}

/**
  * @brief  Returns the USB Host Speed from the Low Level Driver.
  * @param  phost: Host handle
  * @retval USBH Speeds
  */
USBH_SpeedTypeDef USBH_LL_GetSpeed(USBH_HandleTypeDef *phost)
{
  USBH_SpeedTypeDef speed = USBH_SPEED_FULL;
    
  switch (HAL_HCD_GetCurrentSpeed(phost->pData))
  {
  case 0: 
    speed = USBH_SPEED_HIGH;
    break;

  case 1: 
    speed = USBH_SPEED_FULL;    
    break;
    
  case 2: 
    speed = USBH_SPEED_LOW;   
    break;
    
   default:  
    speed = USBH_SPEED_FULL;    
    break;    
  }
  return speed;
}

/**
  * @brief  Resets the Host Port of the Low Level Driver.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_ResetPort (USBH_HandleTypeDef *phost) 
{
  HAL_HCD_ResetPort(phost->pData);
  return USBH_OK; 
}

/**
  * @brief  Returns the last transfered packet size.
  * @param  phost: Host handle
  * @param  pipe: Pipe index   
  * @retval Packet Size
  */
uint32_t USBH_LL_GetLastXferSize(USBH_HandleTypeDef *phost, uint8_t pipe)  
{
  return HAL_HCD_HC_GetXferCount(phost->pData, pipe);
}

/**
  * @brief  Opens a pipe of the Low Level Driver.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  * @param  epnum: Endpoint Number
  * @param  dev_address: Device USB address
  * @param  speed: Device Speed 
  * @param  ep_type: Endpoint Type
  * @param  mps: Endpoint Max Packet Size                 
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_OpenPipe(USBH_HandleTypeDef *phost, 
                                    uint8_t pipe,
                                    uint8_t epnum,                                      
                                    uint8_t dev_address,
                                    uint8_t speed,
                                    uint8_t ep_type,
                                    uint16_t mps)
{
  HAL_HCD_HC_Init(phost->pData,
                  pipe,
                  epnum,
                  dev_address,
                  speed,
                  ep_type,
                  mps);
  return USBH_OK; 
}

/**
  * @brief  Closes a pipe of the Low Level Driver.
  * @param  phost: Host handle
  * @param  pipe: Pipe index               
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_ClosePipe(USBH_HandleTypeDef *phost, uint8_t pipe)   
{
  HAL_HCD_HC_Halt(phost->pData, pipe);
  return USBH_OK;
}

/**
  * @brief  Submits a new URB to the low level driver.
  * @param  phost: Host handle
  * @param  pipe: Pipe index    
  *          This parameter can be a value from 1 to 15
  * @param  direction: Channel number
  *          This parameter can be one of these values:
  *           0: Output 
  *           1: Input
  * @param  ep_type: Endpoint Type
  *          This parameter can be one of these values:
  *            @arg EP_TYPE_CTRL: Control type
  *            @arg EP_TYPE_ISOC: Isochrounous type
  *            @arg EP_TYPE_BULK: Bulk type
  *            @arg EP_TYPE_INTR: Interrupt type
  * @param  token: Endpoint Type
  *          This parameter can be one of these values:
  *            @arg 0: PID_SETUP
  *            @arg 1: PID_DATA
  * @param  pbuff: pointer to URB data
  * @param  length: length of URB data
  * @param  do_ping: activate do ping protocol (for high speed only)
  *          This parameter can be one of these values:
  *           0: do ping inactive 
  *           1: do ping active 
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_SubmitURB(USBH_HandleTypeDef *phost, 
                                     uint8_t pipe, 
                                     uint8_t direction,
                                     uint8_t ep_type,  
                                     uint8_t token, 
                                     uint8_t* pbuff, 
                                     uint16_t length,
                                     uint8_t do_ping) 
{
  HAL_HCD_HC_SubmitRequest(phost->pData,
                           pipe, 
                           direction,
                           ep_type,  
                           token, 
                           pbuff, 
                           length,
                           do_ping);
  return USBH_OK;   
}

/**
  * @brief  Gets a URB state from the low level driver.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  *          This parameter can be a value from 1 to 15
  * @retval URB state
  *          This parameter can be one of these values:
  *            @arg URB_IDLE
  *            @arg URB_DONE
  *            @arg URB_NOTREADY
  *            @arg URB_NYET 
  *            @arg URB_ERROR  
  *            @arg URB_STALL      
  */
USBH_URBStateTypeDef USBH_LL_GetURBState(USBH_HandleTypeDef *phost, uint8_t pipe) 
{
  return (USBH_URBStateTypeDef)HAL_HCD_HC_GetURBState (phost->pData, pipe);
}

/**
  * @brief  Drives VBUS.
  * @param  phost: Host handle
  * @param  state: VBUS state
  *          This parameter can be one of these values:
  *           0: VBUS Active 
  *           1: VBUS Inactive
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_DriverVBUS(USBH_HandleTypeDef *phost, uint8_t state)
{
//  if(state == 0)
//  {
//    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_5, GPIO_PIN_SET);
//  }
//  else
//  {
//    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_5, GPIO_PIN_RESET);
//  }
  
  HAL_Delay(200);
  return USBH_OK;  
}

/**
  * @brief  Sets toggle for a pipe.
  * @param  phost: Host handle
  * @param  pipe: Pipe index   
  * @param  toggle: toggle (0/1)
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_SetToggle(USBH_HandleTypeDef *phost, uint8_t pipe, uint8_t toggle)   
{
  if(hhcd.hc[pipe].ep_is_in)
  {
    hhcd.hc[pipe].toggle_in = toggle;
  }
  else
  {
    hhcd.hc[pipe].toggle_out = toggle;
  }
  return USBH_OK; 
}

/**
  * @brief  Returns the current toggle of a pipe.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  * @retval toggle (0/1)
  */
uint8_t USBH_LL_GetToggle(USBH_HandleTypeDef *phost, uint8_t pipe)   
{
  uint8_t toggle = 0;
  
  if(hhcd.hc[pipe].ep_is_in)
  {
    toggle = hhcd.hc[pipe].toggle_in;
  }
  else
  {
    toggle = hhcd.hc[pipe].toggle_out;
  }
  return toggle; 
}

/**
  * @brief  Delay routine for the USB Host Library
  * @param  Delay: Delay in ms
  * @retval None
  */
void USBH_Delay(uint32_t Delay)
{
  HAL_Delay(Delay);  
}

#ifdef USE_USB_FS
void OTG_FS_IRQHandler(void)
{
  HAL_HCD_IRQHandler(&hhcd);
}
#endif

#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

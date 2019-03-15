/**
  ******************************************************************************
  * @file    stm32f1xx_ll_usb.h
  * @author  MCD Application Team
  * @brief   Header file of USB Low Layer HAL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F1xx_LL_USB_H
#define __STM32F1xx_LL_USB_H

#ifdef __cplusplus
 extern "C" {
#endif

#if defined(STM32F102x6) || defined(STM32F102xB) || \
    defined(STM32F103x6) || defined(STM32F103xB) || \
    defined(STM32F103xE) || defined(STM32F103xG) || \
    defined(STM32F105xC) || defined(STM32F107xC)

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal_def.h"

/** @addtogroup STM32F1xx_HAL_Driver
  * @{
  */

/** @addtogroup USB_LL
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup USB_LL_Exported_Types USB Low Layer Exported Types
  * @{
  */
/**
  * @brief  USB Mode definition
  */
typedef enum
{
   USB_DEVICE_MODE  = 0,
   USB_HOST_MODE    = 1,
   USB_DRD_MODE     = 2
}USB_ModeTypeDef;

#if defined (USB_OTG_FS)
/**
  * @brief  URB States definition
  */
typedef enum {
  URB_IDLE = 0,
  URB_DONE,
  URB_NOTREADY,
  URB_NYET,
  URB_ERROR,
  URB_STALL
}USB_OTG_URBStateTypeDef;

/**
  * @brief  Host channel States  definition
  */
typedef enum {
  HC_IDLE = 0,
  HC_XFRC,
  HC_HALTED,
  HC_NAK,
  HC_NYET,
  HC_STALL,
  HC_XACTERR,
  HC_BBLERR,
  HC_DATATGLERR
}USB_OTG_HCStateTypeDef;

/**
  * @brief  USB OTG Initialization Structure definition
  */
typedef struct
{
  uint32_t dev_endpoints;        /*!< Device Endpoints number.
                                      This parameter depends on the used USB core.
                                      This parameter must be a number between Min_Data = 1 and Max_Data = 15 */

  uint32_t Host_channels;        /*!< Host Channels number.
                                      This parameter Depends on the used USB core.
                                      This parameter must be a number between Min_Data = 1 and Max_Data = 15 */

  uint32_t speed;                /*!< USB Core speed.
                                      This parameter can be any value of @ref USB_Core_Speed_                */

  uint32_t ep0_mps;              /*!< Set the Endpoint 0 Max Packet size.
                                      This parameter can be any value of @ref USB_EP0_MPS_                   */

  uint32_t Sof_enable;           /*!< Enable or disable the output of the SOF signal.                        */

  uint32_t low_power_enable;     /*!< Enable or disable the low power mode.                                  */

  uint32_t vbus_sensing_enable;  /*!< Enable or disable the VBUS Sensing feature.                            */

  uint32_t use_external_vbus;    /*!< Enable or disable the use of the external VBUS.                        */
}USB_OTG_CfgTypeDef;

typedef struct
{
  uint8_t   num;            /*!< Endpoint number
                                This parameter must be a number between Min_Data = 1 and Max_Data = 15    */

  uint8_t   is_in;          /*!< Endpoint direction
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1     */

  uint8_t   is_stall;       /*!< Endpoint stall condition
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1     */

  uint8_t   type;           /*!< Endpoint type
                                 This parameter can be any value of @ref USB_EP_Type_                     */

  uint8_t   data_pid_start; /*!< Initial data PID
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1     */

  uint8_t   even_odd_frame; /*!< IFrame parity
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 1    */

  uint16_t  tx_fifo_num;    /*!< Transmission FIFO number
                                 This parameter must be a number between Min_Data = 1 and Max_Data = 15   */

  uint32_t  maxpacket;      /*!< Endpoint Max packet size
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 64KB */

  uint8_t   *xfer_buff;     /*!< Pointer to transfer buffer                                               */

  uint32_t  dma_addr;       /*!< 32 bits aligned transfer buffer address                                  */

  uint32_t  xfer_len;       /*!< Current transfer length                                                  */

  uint32_t  xfer_count;     /*!< Partial transfer length in case of multi packet transfer                 */
}USB_OTG_EPTypeDef;

typedef struct
{
  uint8_t   dev_addr ;     /*!< USB device address.
                                This parameter must be a number between Min_Data = 1 and Max_Data = 255    */

  uint8_t   ch_num;        /*!< Host channel number.
                                This parameter must be a number between Min_Data = 1 and Max_Data = 15     */

  uint8_t   ep_num;        /*!< Endpoint number.
                                This parameter must be a number between Min_Data = 1 and Max_Data = 15     */

  uint8_t   ep_is_in;      /*!< Endpoint direction
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1      */

  uint8_t   speed;         /*!< USB Host speed.
                                This parameter can be any value of @ref USB_Core_Speed_                    */

  uint8_t   do_ping;       /*!< Enable or disable the use of the PING protocol for HS mode.                */

  uint8_t   process_ping;  /*!< Execute the PING protocol for HS mode.                                     */

  uint8_t   ep_type;       /*!< Endpoint Type.
                                This parameter can be any value of @ref USB_EP_Type_                       */

  uint16_t  max_packet;    /*!< Endpoint Max packet size.
                                This parameter must be a number between Min_Data = 0 and Max_Data = 64KB   */

  uint8_t   data_pid;      /*!< Initial data PID.
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1      */

  uint8_t   *xfer_buff;    /*!< Pointer to transfer buffer.                                                */

  uint32_t  xfer_len;      /*!< Current transfer length.                                                   */

  uint32_t  xfer_count;    /*!< Partial transfer length in case of multi packet transfer.                  */

  uint8_t   toggle_in;     /*!< IN transfer current toggle flag.
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1      */

  uint8_t   toggle_out;    /*!< OUT transfer current toggle flag
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1      */

  uint32_t  dma_addr;      /*!< 32 bits aligned transfer buffer address.                                   */

  uint32_t  ErrCnt;        /*!< Host channel error count.*/

  USB_OTG_URBStateTypeDef  urb_state;  /*!< URB state.
                                           This parameter can be any value of @ref USB_OTG_URBStateTypeDef */

  USB_OTG_HCStateTypeDef   state;     /*!< Host Channel state.
                                           This parameter can be any value of @ref USB_OTG_HCStateTypeDef  */
}USB_OTG_HCTypeDef;
#endif /* USB_OTG_FS */

#if defined (USB)
/**
  * @brief  USB Initialization Structure definition
  */
typedef struct
{
  uint32_t dev_endpoints;        /*!< Device Endpoints number.
                                      This parameter depends on the used USB core.
                                      This parameter must be a number between Min_Data = 1 and Max_Data = 15 */

  uint32_t speed;                /*!< USB Core speed.
                                      This parameter can be any value of @ref USB_Core_Speed                 */

  uint32_t ep0_mps;              /*!< Set the Endpoint 0 Max Packet size.
                                      This parameter can be any value of @ref USB_EP0_MPS                    */

  uint32_t phy_itface;           /*!< Select the used PHY interface.
                                      This parameter can be any value of @ref USB_Core_PHY                   */

  uint32_t Sof_enable;           /*!< Enable or disable the output of the SOF signal.                        */

  uint32_t low_power_enable;       /*!< Enable or disable Low Power mode                                      */

  uint32_t lpm_enable;             /*!< Enable or disable Battery charging.                                  */

  uint32_t battery_charging_enable; /*!< Enable or disable Battery charging.                                  */
} USB_CfgTypeDef;

typedef struct
{
  uint8_t   num;            /*!< Endpoint number
                                This parameter must be a number between Min_Data = 1 and Max_Data = 15    */

  uint8_t   is_in;          /*!< Endpoint direction
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1     */

  uint8_t   is_stall;       /*!< Endpoint stall condition
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1     */

  uint8_t   type;           /*!< Endpoint type
                                 This parameter can be any value of @ref USB_EP_Type                      */

  uint16_t  pmaadress;      /*!< PMA Address
                                 This parameter can be any value between Min_addr = 0 and Max_addr = 1K   */

  uint16_t  pmaaddr0;       /*!< PMA Address0
                                 This parameter can be any value between Min_addr = 0 and Max_addr = 1K   */

  uint16_t  pmaaddr1;        /*!< PMA Address1
                                 This parameter can be any value between Min_addr = 0 and Max_addr = 1K   */

  uint8_t   doublebuffer;    /*!< Double buffer enable
                                 This parameter can be 0 or 1                                             */

  uint16_t  tx_fifo_num;    /*!< This parameter is not required by USB Device FS peripheral, it is used
                                 only by USB OTG FS peripheral
                                 This parameter is added to ensure compatibility across USB peripherals   */

  uint32_t  maxpacket;      /*!< Endpoint Max packet size
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 64KB */

  uint8_t   *xfer_buff;     /*!< Pointer to transfer buffer                                               */

  uint32_t  xfer_len;       /*!< Current transfer length                                                  */

  uint32_t  xfer_count;     /*!< Partial transfer length in case of multi packet transfer                 */

} USB_EPTypeDef;
#endif /* USB */
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup USB_LL_Exported_Constants USB Low Layer Exported Constants
  * @{
  */
#if defined (USB_OTG_FS)
/** @defgroup USB_LL_Core_Mode USB Low Layer Core Mode
  * @{
  */
#define USB_OTG_MODE_DEVICE                    0
#define USB_OTG_MODE_HOST                      1
#define USB_OTG_MODE_DRD                       2
/**
  * @}
  */

/** @defgroup USB_LL_Core_Speed USB Low Layer Core Speed
  * @{
  */
#define USB_OTG_SPEED_LOW                      2
#define USB_OTG_SPEED_FULL                     3

/**
  * @}
  */

/** @defgroup USB_LL_Core_PHY USB Low Layer Core PHY
  * @{
  */
#define USB_OTG_ULPI_PHY                       1
#define USB_OTG_EMBEDDED_PHY                   2
/**
  * @}
  */

/** @defgroup USB_LL_Core_MPS USB Low Layer Core MPS
  * @{
  */
#define USB_OTG_FS_MAX_PACKET_SIZE             64
#define USB_OTG_MAX_EP0_SIZE                   64
/**
  * @}
  */

/** @defgroup USB_LL_Core_PHY_Frequency USB Low Layer Core PHY Frequency
  * @{
  */
#define DSTS_ENUMSPD_FS_PHY_30MHZ_OR_60MHZ     (1 << 1)
#define DSTS_ENUMSPD_LS_PHY_6MHZ               (2 << 1)
#define DSTS_ENUMSPD_FS_PHY_48MHZ              (3 << 1)
/**
  * @}
  */

/** @defgroup USB_LL_CORE_Frame_Interval USB Low Layer Core Frame Interval
  * @{
  */
#define DCFG_FRAME_INTERVAL_80                 0
#define DCFG_FRAME_INTERVAL_85                 1
#define DCFG_FRAME_INTERVAL_90                 2
#define DCFG_FRAME_INTERVAL_95                 3
/**
  * @}
  */

/** @defgroup USB_LL_EP0_MPS USB Low Layer EP0 MPS
  * @{
  */
#define DEP0CTL_MPS_64                         0
#define DEP0CTL_MPS_32                         1
#define DEP0CTL_MPS_16                         2
#define DEP0CTL_MPS_8                          3
/**
  * @}
  */

/** @defgroup USB_LL_EP_Speed USB Low Layer EP Speed
  * @{
  */
#define EP_SPEED_LOW                           0
#define EP_SPEED_FULL                          1
#define EP_SPEED_HIGH                          2
/**
  * @}
  */

/** @defgroup USB_LL_EP_Type USB Low Layer EP Type
  * @{
  */
#define EP_TYPE_CTRL                           0
#define EP_TYPE_ISOC                           1
#define EP_TYPE_BULK                           2
#define EP_TYPE_INTR                           3
#define EP_TYPE_MSK                            3
/**
  * @}
  */

/** @defgroup USB_LL_STS_Defines USB Low Layer STS Defines
  * @{
  */
#define STS_GOUT_NAK                           1
#define STS_DATA_UPDT                          2
#define STS_XFER_COMP                          3
#define STS_SETUP_COMP                         4
#define STS_SETUP_UPDT                         6
/**
  * @}
  */

/** @defgroup USB_LL_HCFG_SPEED_Defines USB Low Layer HCFG Speed Defines
  * @{
  */
#define HCFG_30_60_MHZ                         0
#define HCFG_48_MHZ                            1
#define HCFG_6_MHZ                             2
/**
  * @}
  */

/** @defgroup USB_LL_HPRT0_PRTSPD_SPEED_Defines USB Low Layer HPRT0 PRTSPD Speed Defines
  * @{
  */
#define HPRT0_PRTSPD_HIGH_SPEED                0
#define HPRT0_PRTSPD_FULL_SPEED                1
#define HPRT0_PRTSPD_LOW_SPEED                 2
/**
  * @}
  */

#define HCCHAR_CTRL                            0
#define HCCHAR_ISOC                            1
#define HCCHAR_BULK                            2
#define HCCHAR_INTR                            3

#define HC_PID_DATA0                           0
#define HC_PID_DATA2                           1
#define HC_PID_DATA1                           2
#define HC_PID_SETUP                           3

#define GRXSTS_PKTSTS_IN                       2
#define GRXSTS_PKTSTS_IN_XFER_COMP             3
#define GRXSTS_PKTSTS_DATA_TOGGLE_ERR          5
#define GRXSTS_PKTSTS_CH_HALTED                7

#define USBx_PCGCCTL                           *(__IO uint32_t *)((uint32_t)USBx + USB_OTG_PCGCCTL_BASE)
#define USBx_HPRT0                             *(__IO uint32_t *)((uint32_t)USBx + USB_OTG_HOST_PORT_BASE)

#define USBx_DEVICE                            ((USB_OTG_DeviceTypeDef *)((uint32_t )USBx + USB_OTG_DEVICE_BASE))
#define USBx_INEP(i)                           ((USB_OTG_INEndpointTypeDef *)((uint32_t)USBx + USB_OTG_IN_ENDPOINT_BASE + (i)*USB_OTG_EP_REG_SIZE))
#define USBx_OUTEP(i)                          ((USB_OTG_OUTEndpointTypeDef *)((uint32_t)USBx + USB_OTG_OUT_ENDPOINT_BASE + (i)*USB_OTG_EP_REG_SIZE))
#define USBx_DFIFO(i)                          *(__IO uint32_t *)((uint32_t)USBx + USB_OTG_FIFO_BASE + (i) * USB_OTG_FIFO_SIZE)

#define USBx_HOST                              ((USB_OTG_HostTypeDef *)((uint32_t )USBx + USB_OTG_HOST_BASE))
#define USBx_HC(i)                             ((USB_OTG_HostChannelTypeDef *)((uint32_t)USBx + USB_OTG_HOST_CHANNEL_BASE + (i)*USB_OTG_HOST_CHANNEL_SIZE))
#endif /* USB_OTG_FS */

#if defined (USB)
/** @defgroup USB_LL_EP0_MPS USB Low Layer EP0 MPS
  * @{
  */
#define DEP0CTL_MPS_64                         0
#define DEP0CTL_MPS_32                         1
#define DEP0CTL_MPS_16                         2
#define DEP0CTL_MPS_8                          3
/**
  * @}
  */

/** @defgroup USB_LL_EP_Type USB Low Layer EP Type
  * @{
  */
#define EP_TYPE_CTRL                           0
#define EP_TYPE_ISOC                           1
#define EP_TYPE_BULK                           2
#define EP_TYPE_INTR                           3
#define EP_TYPE_MSK                            3
/**
  * @}
  */

#define BTABLE_ADDRESS                         (0x000)
#endif /* USB */
/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
/** @defgroup USB_LL_Exported_Macros USB Low Layer Exported Macros
  * @{
  */
#if defined (USB_OTG_FS)
#define USB_MASK_INTERRUPT(__INSTANCE__, __INTERRUPT__)     ((__INSTANCE__)->GINTMSK &= ~(__INTERRUPT__))
#define USB_UNMASK_INTERRUPT(__INSTANCE__, __INTERRUPT__)   ((__INSTANCE__)->GINTMSK |= (__INTERRUPT__))

#define CLEAR_IN_EP_INTR(__EPNUM__, __INTERRUPT__)          (USBx_INEP(__EPNUM__)->DIEPINT = (__INTERRUPT__))
#define CLEAR_OUT_EP_INTR(__EPNUM__, __INTERRUPT__)         (USBx_OUTEP(__EPNUM__)->DOEPINT = (__INTERRUPT__))
#endif /* USB_OTG_FS */
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup USB_LL_Exported_Functions USB Low Layer Exported Functions
  * @{
  */
/** @addtogroup USB_LL_Exported_Functions_Group1 Peripheral Control functions
  * @{
  */
#if defined (USB_OTG_FS)
HAL_StatusTypeDef USB_CoreInit(USB_OTG_GlobalTypeDef *USBx, USB_OTG_CfgTypeDef Init);
HAL_StatusTypeDef USB_DevInit(USB_OTG_GlobalTypeDef *USBx, USB_OTG_CfgTypeDef Init);
HAL_StatusTypeDef USB_EnableGlobalInt(USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_DisableGlobalInt(USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_SetCurrentMode(USB_OTG_GlobalTypeDef *USBx , USB_ModeTypeDef mode);
HAL_StatusTypeDef USB_SetDevSpeed(USB_OTG_GlobalTypeDef *USBx , uint8_t speed);
HAL_StatusTypeDef USB_FlushRxFifo (USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_FlushTxFifo (USB_OTG_GlobalTypeDef *USBx, uint32_t num );
HAL_StatusTypeDef USB_ActivateEndpoint(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep);
HAL_StatusTypeDef USB_DeactivateEndpoint(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep);
HAL_StatusTypeDef USB_EPStartXfer(USB_OTG_GlobalTypeDef *USBx , USB_OTG_EPTypeDef *ep);
HAL_StatusTypeDef USB_EP0StartXfer(USB_OTG_GlobalTypeDef *USBx , USB_OTG_EPTypeDef *ep);
HAL_StatusTypeDef USB_WritePacket(USB_OTG_GlobalTypeDef *USBx, uint8_t *src, uint8_t ch_ep_num, uint16_t len);
void *            USB_ReadPacket(USB_OTG_GlobalTypeDef *USBx, uint8_t *dest, uint16_t len);
HAL_StatusTypeDef USB_EPSetStall(USB_OTG_GlobalTypeDef *USBx , USB_OTG_EPTypeDef *ep);
HAL_StatusTypeDef USB_EPClearStall(USB_OTG_GlobalTypeDef *USBx , USB_OTG_EPTypeDef *ep);
HAL_StatusTypeDef USB_SetDevAddress (USB_OTG_GlobalTypeDef *USBx, uint8_t address);
HAL_StatusTypeDef USB_DevConnect (USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_DevDisconnect (USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_StopDevice(USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_ActivateSetup (USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_EP0_OutStart(USB_OTG_GlobalTypeDef *USBx, uint8_t *psetup);
uint8_t           USB_GetDevSpeed(USB_OTG_GlobalTypeDef *USBx);
uint32_t          USB_GetMode(USB_OTG_GlobalTypeDef *USBx);
uint32_t          USB_ReadInterrupts (USB_OTG_GlobalTypeDef *USBx);
uint32_t          USB_ReadDevAllOutEpInterrupt (USB_OTG_GlobalTypeDef *USBx);
uint32_t          USB_ReadDevOutEPInterrupt (USB_OTG_GlobalTypeDef *USBx , uint8_t epnum);
uint32_t          USB_ReadDevAllInEpInterrupt (USB_OTG_GlobalTypeDef *USBx);
uint32_t          USB_ReadDevInEPInterrupt (USB_OTG_GlobalTypeDef *USBx , uint8_t epnum);
void              USB_ClearInterrupts (USB_OTG_GlobalTypeDef *USBx, uint32_t interrupt);

HAL_StatusTypeDef USB_HostInit (USB_OTG_GlobalTypeDef *USBx, USB_OTG_CfgTypeDef cfg);
HAL_StatusTypeDef USB_InitFSLSPClkSel(USB_OTG_GlobalTypeDef *USBx , uint8_t freq);
HAL_StatusTypeDef USB_ResetPort(USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_DriveVbus (USB_OTG_GlobalTypeDef *USBx, uint8_t state);
uint32_t          USB_GetHostSpeed (USB_OTG_GlobalTypeDef *USBx);
uint32_t          USB_GetCurrentFrame (USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_HC_Init(USB_OTG_GlobalTypeDef *USBx,
                              uint8_t ch_num,
                              uint8_t epnum,
                              uint8_t dev_address,
                              uint8_t speed,
                              uint8_t ep_type,
                              uint16_t mps);
HAL_StatusTypeDef USB_HC_StartXfer(USB_OTG_GlobalTypeDef *USBx, USB_OTG_HCTypeDef *hc);
uint32_t          USB_HC_ReadInterrupt (USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_HC_Halt(USB_OTG_GlobalTypeDef *USBx , uint8_t hc_num);
HAL_StatusTypeDef USB_DoPing(USB_OTG_GlobalTypeDef *USBx , uint8_t ch_num);
HAL_StatusTypeDef USB_StopHost(USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_ActivateRemoteWakeup(USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_DeActivateRemoteWakeup(USB_OTG_GlobalTypeDef *USBx);
#endif /* USB_OTG_FS */

#if defined (USB)
HAL_StatusTypeDef USB_CoreInit(USB_TypeDef *USBx, USB_CfgTypeDef Init);
HAL_StatusTypeDef USB_DevInit(USB_TypeDef *USBx, USB_CfgTypeDef Init);
HAL_StatusTypeDef USB_EnableGlobalInt(USB_TypeDef *USBx);
HAL_StatusTypeDef USB_DisableGlobalInt(USB_TypeDef *USBx);
HAL_StatusTypeDef USB_SetCurrentMode(USB_TypeDef *USBx , USB_ModeTypeDef mode);
HAL_StatusTypeDef USB_SetDevSpeed(USB_TypeDef *USBx , uint8_t speed);
HAL_StatusTypeDef USB_FlushRxFifo (USB_TypeDef *USBx);
HAL_StatusTypeDef USB_FlushTxFifo (USB_TypeDef *USBx, uint32_t num );
HAL_StatusTypeDef USB_ActivateEndpoint(USB_TypeDef *USBx, USB_EPTypeDef *ep);
HAL_StatusTypeDef USB_DeactivateEndpoint(USB_TypeDef *USBx, USB_EPTypeDef *ep);
HAL_StatusTypeDef USB_EPStartXfer(USB_TypeDef *USBx , USB_EPTypeDef *ep);
HAL_StatusTypeDef USB_WritePacket(USB_TypeDef *USBx, uint8_t *src, uint8_t ch_ep_num, uint16_t len);
void *            USB_ReadPacket(USB_TypeDef *USBx, uint8_t *dest, uint16_t len);
HAL_StatusTypeDef USB_EPSetStall(USB_TypeDef *USBx , USB_EPTypeDef *ep);
HAL_StatusTypeDef USB_EPClearStall(USB_TypeDef *USBx , USB_EPTypeDef *ep);
HAL_StatusTypeDef USB_SetDevAddress (USB_TypeDef *USBx, uint8_t address);
HAL_StatusTypeDef USB_DevConnect (USB_TypeDef *USBx);
HAL_StatusTypeDef USB_DevDisconnect (USB_TypeDef *USBx);
HAL_StatusTypeDef USB_StopDevice(USB_TypeDef *USBx);
HAL_StatusTypeDef USB_EP0_OutStart(USB_TypeDef *USBx, uint8_t *psetup);
uint32_t          USB_ReadInterrupts (USB_TypeDef *USBx);
uint32_t          USB_ReadDevAllOutEpInterrupt (USB_TypeDef *USBx);
uint32_t          USB_ReadDevOutEPInterrupt (USB_TypeDef *USBx , uint8_t epnum);
uint32_t          USB_ReadDevAllInEpInterrupt (USB_TypeDef *USBx);
uint32_t          USB_ReadDevInEPInterrupt (USB_TypeDef *USBx , uint8_t epnum);
void              USB_ClearInterrupts (USB_TypeDef *USBx, uint32_t interrupt);

HAL_StatusTypeDef USB_ActivateRemoteWakeup(USB_TypeDef *USBx);
HAL_StatusTypeDef USB_DeActivateRemoteWakeup(USB_TypeDef *USBx);
void USB_WritePMA(USB_TypeDef  *USBx, uint8_t *pbUsrBuf, uint16_t wPMABufAddr, uint16_t wNBytes);
void USB_ReadPMA(USB_TypeDef  *USBx, uint8_t *pbUsrBuf, uint16_t wPMABufAddr, uint16_t wNBytes);
#endif /* USB */
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

#endif /* STM32F102x6 || STM32F102xB || */
       /* STM32F103x6 || STM32F103xB || */
       /* STM32F103xE || STM32F103xG || */
       /* STM32F105xC || STM32F107xC    */

#ifdef __cplusplus
}
#endif


#endif /* __STM32F1xx_LL_USB_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

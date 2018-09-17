#ifndef __ehci_h__
#define __ehci_h__ 1
/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
* All Rights Reserved
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: ehci.h$
* $Version : 3.4.4.0$
* $Date    : Sep-7-2009$
*
* Comments:
*
*   This file contains the defines, externs and
*   data structure definitions required by the VUSB_HS Device
*   driver.
*
*END************************************************************************/
#include "types.h"

#include "ehci_cache.h"

/* VUSBHS specific defines */
#define  VUSBHS_MAX_PORTS                          (8)
#define  EHCI_CAP_LEN_MASK                         (0x000000FF)
#define  EHCI_DATA_STRUCTURE_BASE_ADDRESS          (0)

/* Command Register Bit Masks */
#define  EHCI_CMD_RUN_STOP                         USBHS_USBCMD_RS_MASK
#define  EHCI_CMD_CTRL_RESET                       USBHS_USBCMD_RST_MASK
#define  EHCI_CMD_SETUP_TRIPWIRE_SET               (0x00002000)
#define  EHCI_CMD_SETUP_TRIPWIRE_CLEAR             ~EHCI_CMD_SETUP_TRIPWIRE_SET

#define  EHCI_CMD_ATDTW_TRIPWIRE_SET               (0x00001000)
#define  EHCI_CMD_ATDTW_TRIPWIRE_CLEAR             ~EHCI_CMD_SETUP_TRIPWIRE_CLEAR

  /*bit 15,3,2 are for frame list size */
  #define  EHCI_CMD_FRAME_SIZE_1024                  (0x00000000) /* 000 */
  #define  EHCI_CMD_FRAME_SIZE_512                   (0x00000004) /* 001 */
  #define  EHCI_CMD_FRAME_SIZE_256                   (0x00000008) /* 010 */
  #define  EHCI_CMD_FRAME_SIZE_128                   (0x0000000C) /* 011 */
  #define  EHCI_CMD_FRAME_SIZE_64                    (0x00008000) /* 100 */
  #define  EHCI_CMD_FRAME_SIZE_32                    (0x00008004) /* 101 */
  #define  EHCI_CMD_FRAME_SIZE_16                    (0x00008008) /* 110 */
  #define  EHCI_CMD_FRAME_SIZE_8                     (0x0000800C) /* 111 */

                                             
/* Mode Register Bit Masks */
#define  VUSBHS_MODE_CTRL_MODE_IDLE                (0x00000000)
#define  VUSBHS_MODE_CTRL_MODE_DEV                 (0x00000002)
#define  VUSBHS_MODE_CTRL_MODE_HOST                (0x00000003)
#define  VUSBHS_MODE_BIG_ENDIAN                    (0x00000004)
#define  VUSBHS_MODE_SETUP_LOCK_DISABLE            (0x00000008)
                                                
/* Interrupt Enable Register Bit Masks */
#define  EHCI_INTR_INT_EN                          (0x00000001)
#define  EHCI_INTR_ERR_INT_EN                      (0x00000002)
#define  EHCI_INTR_PORT_CHANGE_DETECT_EN           (0x00000004)

#define  EHCI_INTR_ASYNC_ADV_AAE                   (0x00000020)
         #define  EHCI_INTR_ASYNC_ADV_AAE_ENABLE   (0x00000020) /* | with this to enable */
         #define  EHCI_INTR_ASYNC_ADV_AAE_DISABLE  (0xFFFFFFDF) /* & with this to disable */

#define  EHCI_INTR_UAI_EN                          (0x00040000)

#define  EHCI_INTR_RESET_EN                        (0x00000040)
#define  EHCI_INTR_SOF_UFRAME_EN                   (0x00000080)
#define  EHCI_INTR_DEVICE_SUSPEND                  (0x00000100)
#define  EHCI_INTR_TIMER0                          (0x01000000)
#define  EHCI_INTR_TIMER1                          (0x02000000)

/* Interrupt Status Register Masks */
#define  EHCI_STS_SOF                              (0x00000080)
#define  EHCI_STS_RESET                            (0x00000040)
#define  EHCI_STS_PORT_CHANGE                      (0x00000004)
#define  EHCI_STS_ERR                              (0x00000002)
#define  EHCI_STS_INT                              (0x00000001)
#define  EHCI_STS_SUSPEND                          (0x00000100)
#define  EHCI_STS_HC_HALTED                        (0x00001000)
#define  EHCI_STS_TIMER0                           (0x01000000)
#define  EHCI_STS_TIMER1                           (0x02000000)

/* Endpoint Queue Head Bit Masks */
#define  VUSB_EP_QUEUE_HEAD_IOS                    (0x00008000)
#define  VUSB_EP_QUEUE_HEAD_IOC                    (0x00008000)
#define  VUSB_EP_QUEUE_HEAD_INT                    (0x00000100)
#define  VUSB_EP_QUEUE_HEAD_NEXT_TERMINATE         (0x00000001)
#define  VUSB_EP_QUEUE_HEAD_MAX_PKT_LEN_POS        (16)
#define  VUSB_EP_QUEUE_HEAD_ZERO_LEN_TER_SEL       (0x20000000)
#define  VUSB_EP_QUEUE_HEAD_MULT_POS               (30)
#define  VUSB_EP_MAX_LENGTH_TRANSFER               (0x4000)

#define  VUSB_EP_QUEUE_HEAD_STATUS_ACTIVE          (0x00000080)

#define  VUSBHS_TD_NEXT_TERMINATE                  (0x00000001)
#define  VUSBHS_TD_IOC                             (0x00008000)
#define  VUSBHS_TD_STATUS_ACTIVE                   (0x00000080)
#define  VUSBHS_TD_STATUS_HALTED                   (0x00000040)
#define  VUSBHS_TD_RESERVED_FIELDS                 (0x00007F00)
#define  VUSBHS_TD_ERROR_MASK                      (0x68)
#define  VUSBHS_TD_ADDR_MASK                       (0xFFFFFFE0)
#define  VUSBHS_TD_LENGTH_BIT_POS                  (16)

#define  EHCI_EPCTRL_TX_DATA_TOGGLE_RST            (0x00400000)
#define  EHCI_EPCTRL_TX_EP_STALL                   (0x00010000)
#define  EHCI_EPCTRL_RX_EP_STALL                   (0x00000001)
#define  EHCI_EPCTRL_RX_DATA_TOGGLE_RST            (0x00000040)
#define  EHCI_EPCTRL_RX_ENABLE                     (0x00000080)
#define  EHCI_EPCTRL_TX_ENABLE                     (0x00800000)
#define  EHCI_EPCTRL_CONTROL                       (0x00000000)
#define  EHCI_EPCTRL_ISOCHRONOUS                   (0x00040000)
#define  EHCI_EPCTRL_BULK                          (0x00080000)
#define  EHCI_EPCTRL_INT                           (0x000C0000)
#define  EHCI_EPCTRL_TX_TYPE                       (0x000C0000)
#define  EHCI_EPCTRL_RX_TYPE                       (0x0000000C)
#define  EHCI_EPCTRL_DATA_TOGGLE_INHIBIT           (0x00000020)
#define  EHCI_EPCTRL_TX_EP_TYPE_SHIFT              (18)
#define  EHCI_EPCTRL_RX_EP_TYPE_SHIFT              (2)

#define  EHCI_PORTSCX_PORT_POWER                   (0x00001000)
#define  EHCI_PORTSCX_LINE_STATUS_BITS             (0x00000C00)
#define  EHCI_PORTSCX_LINE_STATUS_SE0              (0x00000000)
#define  EHCI_PORTSCX_LINE_STATUS_KSTATE           (0x00000400)
#define  EHCI_PORTSCX_LINE_STATUS_JSTATE           (0x00000800)
#define  EHCI_PORTSCX_PORT_HIGH_SPEED              (0x00000200)
#define  EHCI_PORTSCX_PORT_RESET                   (0x00000100)
#define  EHCI_PORTSCX_PORT_SUSPEND                 (0x00000080)
#define  EHCI_PORTSCX_PORT_FORCE_RESUME            (0x00000040)
#define  EHCI_PORTSCX_PORT_EN_DIS_CHANGE           (0x00000008)
#define  EHCI_PORTSCX_PORT_ENABLE                  (0x00000004)
#define  EHCI_PORTSCX_CONNECT_STATUS_CHANGE        (0x00000002)
#define  EHCI_PORTSCX_CURRENT_CONNECT_STATUS       (0x00000001)

#define  VUSBHS_PORTSCX_PORT_SPEED_FULL            (0x00000000)
#define  VUSBHS_PORTSCX_PORT_SPEED_LOW             (0x04000000)
#define  VUSBHS_PORTSCX_PORT_SPEED_HIGH            (0x08000000)
#define  VUSBHS_SPEED_MASK                         (0x0C000000)
#define  VUSBHS_SPEED_BIT_POS                      (26)

#define  EHCI_PORTSCX_W1C_BITS                     (0x2A)

#define  VUSB_EP_TR_PACKET_SIZE                    (0x7FFF0000)

#define  VUSBHS_FRINDEX_MS_MASK                    (0xFFFFFFF8)
#define  VUSBHS_ADDRESS_BIT_SHIFT                  (25)

#define  VUSB20_MAX_ENDPTS_SUPPORTED               (0x1F)
#define  EHCI_HCC_PARAMS_64_BIT_ADDR_CAP           (0x01)
#define  EHCI_HCC_PARAMS_PGM_FRM_LIST_FLAG         (0x02)
#define  EHCI_HCC_PARAMS_ASYNC_PARK_CAP            (0x04)
#define  EHCI_HCC_PARAMS_ISOCH_SCHED_THRESHOLD     (0xF0)
#define  EHCI_HCC_PARAMS_ISOCH_FRAME_CACHED        (0x80)

#define  VUSB20_HCS_PARAMS_PORT_POWER_CONTROL_FLAG (0x10)

#define  VUSB20_HOST_INTR_EN_BITS                  (0x37)

#define  VUSB20_DEFAULT_PERIODIC_FRAME_LIST_SIZE   (1024)
#define  VUSB20_NEW_PERIODIC_FRAME_LIST_BITS       (2)
#define  EHCI_FRAME_LIST_ELEMENT_POINTER_T_BIT     (0x01)
#define  EHCI_ITD_T_BIT                            (0x01)
#define  EHCI_SITD_T_BIT                           (0x01)
#define  EHCI_QUEUE_HEAD_POINTER_T_BIT             (0x01)


/************************************************************
Split transatcions specific defines
************************************************************/
#define  EHCI_START_SPLIT_MAX_BUDGET               188

#define  EHCI_ELEMENT_TYPE_ITD                     (0x00)
#define  EHCI_ELEMENT_TYPE_QH                      (0x02)
#define  EHCI_ELEMENT_TYPE_SITD                    (0x04)
#define  EHCI_ELEMENT_TYPE_FSTN                    (0x06)
#define  EHCI_ELEMENT_TYPE_MASK                    (0x06)

#define  EHCI_FRAME_LIST_ELEMENT_TYPE_ITD          (0x00)
#define  EHCI_FRAME_LIST_ELEMENT_TYPE_QH           (0x01)
#define  EHCI_FRAME_LIST_ELEMENT_TYPE_SITD         (0x02)
#define  EHCI_FRAME_LIST_ELEMENT_TYPE_FSTN         (0x03)
#define  EHCI_FRAME_LIST_ELEMENT_TYPE_BIT_POS      (1)


#define  EHCI_QH_ELEMENT_TYPE_ITD                  (0x00)
#define  EHCI_QH_ELEMENT_TYPE_QH                   (0x01)
#define  EHCI_QH_ELEMENT_TYPE_SITD                 (0x02)
#define  EHCI_QH_ELEMENT_TYPE_FSTN                 (0x03)

#define  EHCI_QH_ELEMENT_TYPE_BIT_POS              (1)

#define  EHCI_QTD_PID_OUT_TOKEN                    (0x000)
#define  EHCI_QTD_PID_IN_TOKEN                     (0x100)
#define  EHCI_QTD_PID_SETUP_TOKEN                  (0x200)
#define  EHCI_QTD_IOC                              (0x8000)
#define  EHCI_QTD_STATUS_ACTIVE                    (0x0080)
#define  EHCI_QTD_STATUS_HALTED                    (0x0040)
#define  EHCI_QTD_PID_SETUP                        (0x0200)
#define  EHCI_QTD_PID_IN                           (0x0100)
#define  EHCI_QTD_PID_OUT                          (0x0000)
#define  EHCI_QTD_LENGTH_BIT_POS                   (16)
#define  EHCI_QTD_DATA_TOGGLE                      (0x80000000)
#define  EHCI_QTD_DATA_TOGGLE_BIT_POS              (31)
#define  EHCI_QTD_LENGTH_BIT_MASK                  (0x7FFF0000)
#define  EHCI_QTD_ERROR_BITS_MASK                  (0x0000003E)
#define  EHCI_QTD_DEFAULT_CERR_VALUE               (0xC00)

#define  EHCI_SETUP_TOKEN                          (2)
#define  EHCI_OUT_TOKEN                            (0)
#define  EHCI_IN_TOKEN                             (1)

#define  EHCI_QTD_T_BIT                            (0x01)

#define  EHCI_QH_ENDPOINT_SPEED_FULL               (0x00)
#define  EHCI_QH_ENDPOINT_SPEED_LOW                (0x01)
#define  EHCI_QH_ENDPOINT_SPEED_HIGH               (0x02)
#define  EHCI_QH_ENDPOINT_SPEED_RESERVED           (0x03)

#define  EHCI_ITD_LENGTH_BIT_POS                   (16)
#define  EHCI_ITD_IOC_BIT                          (0x00008000)
#define  EHCI_ITD_ACTIVE_BIT                       (0x80000000)
#define  EHCI_ITD_PG_SELECT_BIT_POS                (12)
#define  EHCI_ITD_DIRECTION_BIT_POS                (11)
#define  EHCI_ITD_EP_BIT_POS                       (8)
#define  EHCI_ITD_STATUS                           (0xF0000000)
        #define  EHCI_ITD_STATUS_ACTIVE            (0x80000000) /*bit 4 = 1000*/
        #define  EHCI_ITD_STATUS_DATA_BUFFER_ERR   (0x40000000) /*bit 3 = 0100*/
        #define  EHCI_ITD_STATUS_BABBLE_ERROR      (0x20000000) /*bit 2 = 0010*/
        #define  EHCI_ITD_STATUS_TRANSACTION_ERR   (0x10000000) /*bit 4 = 0001*/

#define  EHCI_ITD_LENGTH_TRANSMITTED               (0x0FFF0000)
#define  EHCI_ITD_BUFFER_OFFSET                    (0x00000FFF)
#define  EHCI_ITD_PAGE_NUMBER                      (0x00007000)
#define  EHCI_ITD_BUFFER_POINTER                   (0xFFFFF000)
#define  EHCI_ITD_MULTI_TRANSACTION_BITS           (0x00000003)

          

/* SITD position bits */
#define  EHCI_SITD_DIRECTION_BIT_POS                (31)
#define  EHCI_SITD_PORT_NUMBER_BIT_POS              (24)
#define  EHCI_SITD_HUB_ADDR_BIT_POS                 (16)
#define  EHCI_SITD_EP_ADDR_BIT_POS                  (8)

#define  EHCI_SITD_COMPLETE_SPLIT_MASK_BIT_POS      (8)

#define  EHCI_SITD_IOC_BIT_SET                      (0x80000000)
#define  EHCI_SITD_PAGE_SELECT_BIT_POS              (30)
#define  EHCI_SITD_TRANSFER_LENGTH_BIT_POS          (16)
#define  EHCI_SITD_STATUS_ACTIVE                    (0x80)

#define  EHCI_SITD_STATUS                           (0xFF)
#define  EHCI_SITD_LENGTH_TRANSMITTED               (0x03FF0000)
#define  EHCI_SITD_BUFFER_OFFSET                    (0x00000FFF)
#define  EHCI_SITD_PAGE_NUMBER                      (0x40000000)
#define  EHCI_SITD_BUFFER_POINTER                   (0xFFFFF000)



#define  EHCI_SITD_BUFFER_PTR_BIT_POS                 (12)
#define  EHCI_SITD_TP_BIT_POS                         (3)
    #define  EHCI_SITD_TP_ALL                         (0)
    #define  EHCI_SITD_TP_BEGIN                       (1)
    #define  EHCI_SITD_TP_MID                         (2)
    #define  EHCI_SITD_TP_END                         (3)



/* Interrupt enable bit masks */
#define  EHCI_IER_ASYNCH_ADVANCE                   (0x00000020)
#define  EHCI_IER_HOST_SYS_ERROR                   (0x00000010)
#define  EHCI_IER_FRAME_LIST_ROLLOVER              (0x00000008)
#define  EHCI_IER_PORT_CHANGE                      (0x00000004)
#define  EHCI_IER_USB_ERROR                        (0x00000002)
#define  EHCI_IER_USB_INTERRUPT                    (0x00000001)

/* Interrupt status bit masks */
#define  EHCI_STS_NAK                              (0x00010000)
#define  EHCI_STS_RECLAIMATION                     (0x00002000)
#define  EHCI_STS_SOF_COUNT                        (0x00000080)
#define  EHCI_STS_ASYNCH_ADVANCE                   (0x00000020)
#define  EHCI_STS_HOST_SYS_ERROR                   (0x00000010)
#define  EHCI_STS_FRAME_LIST_ROLLOVER              (0x00000008)
#define  EHCI_STS_PORT_CHANGE                      (0x00000004)
#define  EHCI_STS_USB_ERROR                        (0x00000002)
#define  EHCI_STS_USB_INTERRUPT                    (0x00000001)

/* Status bit masks */
#define  EHCI_STS_ASYNCH_SCHEDULE                  (0x00008000)
#define  EHCI_STS_PERIODIC_SCHEDULE                (0x00004000)
#define  EHCI_STS_RECLAMATION                      (0x00002000)
#define  EHCI_STS_HC_HALTED                        (0x00001000)

/* USB command bit masks */
#define  EHCI_USBCMD_ASYNC_SCHED_ENABLE            (0x00000020)
#define  EHCI_USBCMD_PERIODIC_SCHED_ENABLE         (0x00000010)

#define  EHCI_HCS_PARAMS_N_PORTS                   (0x0F)

#define  EHCI_PIPE_TIMEOUT                         (30)

#define  VUSB_HS_DELAY                             (3500)

#define  EHCI_QH_EP_NUM_MASK                       (0x0F00)
#define  EHCI_QH_EP_NUM_BITS_POS                   (8)
#define  EHCI_QH_DEVICE_ADDRESS_MASK               (0x7F)
#define  EHCI_QH_SPEED_BITS_POS                    (12)
#define  EHCI_QH_MAX_PKT_SIZE_BITS_POS             (16)
#define  EHCI_QH_NAK_COUNT_RL_BITS_POS             (28)
#define  EHCI_QH_EP_CTRL_FLAG_BIT_POS              (27)
#define  EHCI_QH_HEAD_RECLAMATION_BIT_POS          (15)
#define  EHCI_QH_DTC_BIT_POS                       (14)
#define  EHCI_QH_HIGH_BW_MULT_BIT_POS              (30)
#define  EHCI_QH_HUB_PORT_NUM_BITS_POS             (23)
#define  EHCI_QH_HUB_ADDR_BITS_POS                 (16)
#define  EHCI_QH_SPLIT_COMPLETION_MASK_BITS_POS    (8)
#define  EHCI_QH_SPLIT_COMPLETION_MASK             (0xFF00)
#define  EHCI_QH_INTR_SCHED_MASK                   (0xFF)
#define  EHCI_QH_INACTIVATE_NEXT_TR_BIT_POS        (7)
#define  EHCI_QH_HORIZ_PHY_ADDRESS_MASK            (0xFFFFFFE0)
#define  EHCI_QH_TR_OVERLAY_DT_BIT                 (0x80000000)

#define  EHCI_SITD_SPLIT_COMPLETION_MASK_BITS_POS  (8)

#define  EHCI_INTR_NO_THRESHOLD_IMMEDIATE          (0x00010000)
#define  EHCI_NEW_PERIODIC_FRAME_LIST_SIZE         (1024)
#define  EHCI_FRAME_LIST_SIZE_BITS_POS             (2)
#define  EHCI_HORIZ_PHY_ADDRESS_MASK              (0xFFFFFFE0)

/* OTG Status and control register bit masks */

/* OTG interrupt enable bit masks */
#define  VUSBHS_OTGSC_INTERRUPT_ENABLE_BITS_MASK   (0x5F000000)
#define  VUSBHS_OTGSC_DPIE                         (0x40000000)   /* Data-line pulsing IE */
#define  VUSBHS_OTGSC_1MSIE                        (0x20000000)
#define  VUSBHS_OTGSC_BSEIE                        (0x10000000)   /* B-session end IE */
#define  VUSBHS_OTGSC_BSVIE                        (0x08000000)   /* B-session valid IE */
#define  VUSBHS_OTGSC_ASVIE                        (0x04000000)   /* A-session valid IE */
#define  VUSBHS_OTGSC_AVVIE                        (0x02000000)   /* A-V-bus valid IE */
#define  VUSBHS_OTGSC_IDIE                         (0x01000000)   /* OTG ID IE */

/* OTG interrupt status bit masks */
#define  VUSBHS_OTGSC_INTERRUPT_STATUS_BITS_MASK   (0x005F0000)
#define  VUSBHS_OTGSC_DPIS                         (0x00400000)   /* Data-line pulsing IS */
#define  VUSBHS_OTGSC_1MSIS                        (0x00200000)
#define  VUSBHS_OTGSC_BSEIS                        (0x00100000)   /* B-session end IS */
#define  VUSBHS_OTGSC_BSVIS                        (0x00080000)   /* B-session valid IS */
#define  VUSBHS_OTGSC_ASVIS                        (0x00040000)   /* A-session valid IS */
#define  VUSBHS_OTGSC_AVVIS                        (0x00020000)   /* A-Vbus valid IS */
#define  VUSBHS_OTGSC_IDIS                         (0x00010000)   /* OTG ID IS */

/* OTG status bit masks */
#define  VUSBHS_OTGSC_DPS                          (0x00004000)
#define  VUSBHS_OTGSC_BSE                          (0x00001000)   /* B-session end */
#define  VUSBHS_OTGSC_BSV                          (0x00000800)   /* B-session valid */
#define  VUSBHS_OTGSC_ASV                          (0x00000400)   /* A-session valid */
#define  VUSBHS_OTGSC_AVV                          (0x00000200)   /* A-Vbus Valid */
#define  VUSBHS_OTGSC_ID                           (0x00000100)   /* OTG ID */

/* OTG control bit masks */
#define  VUSBHS_OTGSC_CTL_BITS                     (0x2F)
#define  VUSBHS_OTGSC_B_HOST_EN                    (0x00000020)   /* B_host_enable */
#define  VUSBHS_OTGSC_DP                           (0x00000010)   /* Data-pulsing */
#define  VUSBHS_OTGSC_OT                           (0x00000008)   /* OTG termination */
#define  VUSBHS_OTGSC_VO                           (0x00000004)   /* Vbus on */
#define  VUSBHS_OTGSC_VC                           (0x00000002)   /* Vbus charge */
#define  VUSBHS_OTGSC_VD                           (0x00000001)   /* Vbus discharge */

typedef  uint_32  USB_REGISTER, _PTR_ USB_REGISTER_PTR;


#if USBCFG_REGISTER_ENDIANNESS==MQX_BIG_ENDIAN
   #define ehci_reg_read(r,n)           LONG_BE_TO_HOST((uint_32)r)
   #define ehci_reg_write(r,v,n)       {r=HOST_TO_BE_LONG(v);_PSP_SYNC();}
#else
   #define ehci_reg_read(r,n)           LONG_LE_TO_HOST((uint_32)r)
   #define ehci_reg_write(r,v,n)       {r=HOST_TO_LE_LONG(v);_PSP_SYNC();}
#endif

#if USBCFG_MEMORY_ENDIANNESS==MQX_BIG_ENDIAN
   #define ehci_mem_read(r,n)           LONG_BE_TO_HOST((uint_32)r)
   #define ehci_mem_write(r,v,n)       {r=HOST_TO_BE_LONG(v);_PSP_SYNC();}
#else
   #define ehci_mem_read(r,n)           LONG_LE_TO_HOST((uint_32)r)
   #define ehci_mem_write(r,v,n)       {r=HOST_TO_LE_LONG(v);_PSP_SYNC();}
#endif

#define ehci_reg_clear_bits(r,v,n)  ehci_reg_write(r, ehci_reg_read(r,n) & ~v,n)
#define ehci_reg_set_bits(r,v,n)    ehci_reg_write(r, ehci_reg_read(r,n) | v,n)
#define ehci_mem_clear_bits(r,v,n)  ehci_mem_write(r, ehci_mem_read(r,n) & ~v,n)
#define ehci_mem_set_bits(r,v,n)    ehci_mem_write(r, ehci_mem_read(r,n) | v,n)

#define EHCI_REG_READ(r)            ehci_reg_read(r,#r)
#define EHCI_REG_WRITE(r,v)         ehci_reg_write(r,v,#r)
#define EHCI_REG_CLEAR_BITS(r,v)    ehci_reg_clear_bits(r,v,#r)
#define EHCI_REG_SET_BITS(r,v)      ehci_reg_set_bits(r,v,#r)

#define EHCI_MEM_READ(r)            ehci_mem_read(r,#r)
#define EHCI_MEM_WRITE(r,v)         ehci_mem_write(r,v,#r)
#define EHCI_MEM_CLEAR_BITS(r,v)    ehci_mem_clear_bits(r,v,#r)
#define EHCI_MEM_SET_BITS(r,v)      ehci_mem_set_bits(r,v,#r)


/* The VUSB register structure */
typedef USB_Uncached struct {
   union  {
      struct  {
         USB_REGISTER   CAPLENGTH_HCIVER;
         USB_REGISTER   HCS_PARAMS;       /* HC structural parameters */
         USB_REGISTER   HCC_PARAMS;       /* HC Capability Parameters*/
         USB_REGISTER   RESERVED1[5];
         USB_REGISTER   DCI_VERSION;      /* DC version number and reserved 16 bits */
         USB_REGISTER   DCC_PARAMS;       /* DC Capability Parameters */
      } CAPABILITY_REGISTERS;

      struct  {
         USB_REGISTER   GPTIMER0LD;
         USB_REGISTER   GPTIMER0CTL;
         USB_REGISTER   GPTIMER1LD;
         USB_REGISTER   GPTIMER1CTL;
      } TIMER_REGISTERS;
      
      struct  {
         USB_REGISTER   USB_CMD;                   /* Command register */
         USB_REGISTER   USB_STS;                   /* Status register */
         USB_REGISTER   USB_INTR;                  /* Interrupt enable */
         USB_REGISTER   USB_FRINDEX;               /* Frame index */
         USB_REGISTER   CTRLDSSEGMENT;             /* 4G segment selector */
         USB_REGISTER   DEVICE_ADDR;               /* Device Address */
         USB_REGISTER   EP_LIST_ADDR;              /* Endpoint List Address */
         USB_REGISTER   RESERVED0[9];
         USB_REGISTER   CONFIG_FLAG;               /* Configured Flag register */
         USB_REGISTER   PORTSCX[VUSBHS_MAX_PORTS]; /* Port Status/Control x, x = 1..8 */
         USB_REGISTER   OTGSC;
         USB_REGISTER   USB_MODE;                  /* USB Host/Device mode */
         USB_REGISTER   ENDPT_SETUP_STAT;          /* Endpoint Setup Status */
         USB_REGISTER   ENDPTPRIME;                /* Endpoint Initialize */
         USB_REGISTER   ENDPTFLUSH;                /* Endpoint De-initialize */
         USB_REGISTER   ENDPTSTATUS;               /* Endpoint Status */
         USB_REGISTER   ENDPTCOMPLETE;             /* Endpoint Interrupt On Complete */
         USB_REGISTER   ENDPTCTRLX[16];            /* Endpoint Control, where x = 0.. 15 */
      } OPERATIONAL_DEVICE_REGISTERS;
      
      struct  {
         USB_REGISTER   USB_CMD;                   /* Command register */
         USB_REGISTER   USB_STS;                   /* Status register */
         USB_REGISTER   USB_INTR;                  /* Interrupt enable */
         USB_REGISTER   USB_FRINDEX;               /* Frame index */
         USB_REGISTER   CTRLDSSEGMENT;             /* 4G segment selector */
         USB_REGISTER   PERIODIC_LIST_BASE_ADDR;   /* Periodic schedule list */
         USB_REGISTER   CURR_ASYNC_LIST_ADDR;      /* Current Asynch schedule list */
         USB_REGISTER   ASYNCTTSTS;                /* Async buffer in embedded TT control */
#if BSP_TWRMPC5125         
         USB_REGISTER   RESERVED0[4];
         USB_REGISTER   ULPIVIEWPORT;
         USB_REGISTER   RESERVED1[3];
#else
         USB_REGISTER   RESERVED0[8];
#endif        
         USB_REGISTER   CONFIG_FLAG;               /* Configured Flag register */
         USB_REGISTER   PORTSCX[VUSBHS_MAX_PORTS]; /* Port Status/Control x, x = 1..8 */
         USB_REGISTER   OTGSC;                     /* OTG status and control register */
         USB_REGISTER   USB_MODE;                  /* USB Host/Device mode */
      } OPERATIONAL_HOST_REGISTERS;
   } REGISTERS;
} VUSB20_REG_STRUCT, _PTR_ VUSB20_REG_STRUCT_PTR;

#if 0
typedef USB_Uncached struct {
   uint_32   MAX_PKT_LENGTH;      /* Bits 16..26 Bit 15 is Interrupt 
                                          ** On Setup 
                                          */
   uint_32   CURR_DTD_PTR;        /* Current dTD Pointer */
   uint_32   NEXT_DTD_PTR;        /* Next dTD Pointer */
   uint_32   SIZE_IOC_INT_STS;    /* Total bytes (16..30), IOC (15), 
                                          ** INT (8), STS (0-7) 
                                          */
   uint_32   BUFF_PTR0;           /* Buffer pointer Page 0 (12-31) */
   uint_32   BUFF_PTR1;           /* Buffer pointer Page 1 (12-31) */
   uint_32   BUFF_PTR2;           /* Buffer pointer Page 2 (12-31) */
   uint_32   BUFF_PTR3;           /* Buffer pointer Page 3 (12-31) */
   uint_32   BUFF_PTR4;           /* Buffer pointer Page 4 (12-31) */
   uint_32   RESERVED1;
   uchar     SETUP_BUFFER[8];     /* 8 bytes of setup data that follows 
                                  ** the Setup PID 
                                  */
   uint_32   RESERVED2[4];
} VUSB20_EP_QUEUE_HEAD_STRUCT, _PTR_ VUSB20_EP_QUEUE_HEAD_STRUCT_PTR;

typedef USB_Uncached struct ep_tr_struct {
   uint_32      NEXT_TR_ELEM_PTR; /* Memory address of next 
                                          ** dTD to be processed (5-31)
                                          ** and the T (bit 0) indicating 
                                          ** pointer validity
                                          */
   uint_32      SIZE_IOC_STS;     /* total bytes (16-30), 
                                          ** IOC (15), Status (0-7) 
                                          */
   uint_32      BUFF_PTR0;        /* Buffer pointer Page 0 */
   uint_32      BUFF_PTR1;        /* Buffer pointer Page 1 */
   uint_32      BUFF_PTR2;        /* Buffer pointer Page 2 */
   uint_32      BUFF_PTR3;        /* Buffer pointer Page 3 */
   uint_32      BUFF_PTR4;        /* Buffer pointer Page 4 */
   pointer      SCRATCH_PTR;
} VUSB20_EP_TR_STRUCT, _PTR_ VUSB20_EP_TR_STRUCT_PTR;
#endif

typedef USB_Uncached struct {
   // EHCI Defined
   // These should be accessed via macros                                           
   uint_32      NEXT_LINK_PTR;    /* (5-31) Memory address of 
                                          ** next schedule data structure 
                                          ** item Type (1..2 ) and the 
                                          ** T (bit 0) indicating pointer 
                                          ** validity
                                          */
   uint_32      TR_STATUS_CTL_LIST[8];  /* bits 31-28: Status,
                                                ** bits 27-16: Tr X length
                                                ** bit 15: Int on complete
                                                ** bits 14-12: Page Select
                                                ** bits 11-0: Tr X offset
                                                */
   uint_32      BUFFER_PAGE_PTR_LIST[7];  /* bits 31-12 4K aligned pointer 
                                                ** to physical memory 
                                                ** bits 11-8 endpoint no.
                                                ** bit 7: reserved
                                                ** bits 6-0 device address*/
                                                
   // USB Host Stack specific extensions (not read by controller)  
   // These can be accessed directly                                           
   pointer      SCRATCH_PTR;
   pointer      PIPE_DESCR_FOR_THIS_ITD;
   pointer      PIPE_TR_DESCR_FOR_THIS_ITD;
   uint_32_ptr  frame_list_ptr;
   uint_32      number_of_transactions;
   /* 32-byte aligned structures */
   uint_32        RESERVED[11];
} EHCI_ITD_STRUCT, _PTR_ EHCI_ITD_STRUCT_PTR;

typedef USB_Uncached struct {
   // EHCI Defined
   // These should be accessed via macros                                           
   uint_32      NEXT_LINK_PTR;    /* (5-31) Memory address of 
                                          ** next schedule data structure 
                                          ** item Type (1..2 ) and the 
                                          ** T (bit 0) indicating pointer 
                                          ** validity
                                          */
   uint_32      EP_CAPAB_CHARAC;  /* bits 31: Direction (I/O),
                                          ** bits 30-24: Port number
                                          ** bit 23: reserved
                                          ** bits 22-16: Hub address
                                          ** bits 15-12: Reserved
                                          ** bits 11-8: Endpoint number
                                          ** bit 7: reserved
                                          ** bits 6-0: device address
                                          */
   uint_32      UFRAME_SCHED_CTL; /* bits 31-16: reserved 
                                          ** bits 15-8: Split completion mask
                                          ** bits 7-0: Split start mask
                                          */
   uint_32      TRANSFER_STATE;   /* bit 31: int on complete
                                          ** bit 30: Page Select
                                          ** bits 29-26: Reserved
                                          ** bits 25-16: total bytes to 
                                          ** transfer
                                          ** bits 15-8: uframe 
                                          ** complete-split progress mask
                                          ** bits 7-0: status
                                          */
   uint_32      BUFFER_PTR_0;     /* bits 31-12: 4K aligned pointer 
                                          ** to physical memory 
                                          ** bits 11-0: Current offset
                                          */
   uint_32      BUFFER_PTR_1;     /* bits 31-12: 4K aligned pointer 
                                          ** to physical memory 
                                          ** bits 11-5 reserved
                                          ** bits 4-3 tr position
                                          ** bits 2-0 tr count
                                          */
   uint_32      BACK_LINK_PTR;    /* bits 31-5 back pointer points to sITD 
                                          ** bits 4-1: reserved
                                          ** bit 0: terminate
                                          */
   // USB Host Stack specific extensions (not read by controller)  
   // These can be accessed directly                                           
    pointer      SCRATCH_PTR;
   pointer      PIPE_DESCR_FOR_THIS_SITD;
   pointer      PIPE_TR_DESCR_FOR_THIS_SITD;
   uint_32_ptr  frame_list_ptr;

   /* align to 16 word boundry */
   uint_32      RESERVED[5];

} EHCI_SITD_STRUCT, _PTR_ EHCI_SITD_STRUCT_PTR;

typedef USB_Uncached struct {
   // EHCI Defined
   // These should be accessed via macros                                           
   uint_32      NEXT_QTD_PTR;     /* (5-31) Memory address of 
                                          ** next qTD to be processed
                                          ** (4..1) reserved 
                                          ** T (bit 0) indicating pointer 
                                          ** validity
                                          */
   uint_32      ALT_NEXT_QTD_PTR; /* bits 31-5: alternate next 
                                          ** qTD if the above one encounters 
                                          ** a short packet
                                          ** (4..1) reserved 
                                          ** T (bit 0) indicating pointer 
                                          ** validity
                                          */
   uint_32      TOKEN;            /* bits 31: data toggle 
                                          ** bits 30-16: Total bytes to transfer
                                          ** bit 15: Interrupt on Complete
                                          ** bits 14-12: Current page
                                          ** bits 11-10: Error Counter
                                          ** bits 9-8: PID code
                                          ** bits 7-0: status
                                          */
   uint_32      BUFFER_PTR_0;     /* bit 31-12: 4K-page aligned 
                                          ** physical memory address
                                          ** bit 11-0: Current Offset
                                          */
   uint_32      BUFFER_PTR_1;     /* bit 31-12: 4K-page aligned 
                                          ** physical memory address
                                          ** bit 11-0: reserved
                                          */
   uint_32      BUFFER_PTR_2;     /* bit 31-12: 4K-page aligned 
                                          ** physical memory address
                                          ** bit 11-0: reserved
                                          */
   uint_32      BUFFER_PTR_3;     /* bit 31-12: 4K-page aligned 
                                          ** physical memory address
                                          ** bit 11-0: reserved
                                          */
   uint_32      BUFFER_PTR_4;     /* bit 31-12: 4K-page aligned 
                                          ** physical memory address
                                          ** bit 11-0: reserved
                                          */
   // USB Host Stack specific extensions (not read by controller)  
   // These can be accessed directly                                           
    pointer     SCRATCH_PTR;
   pointer     PIPE_DESCR_FOR_THIS_QTD;
   pointer     TR_FOR_THIS_QTD;
   uint_32     RESERVED[5];
} EHCI_QTD_STRUCT, _PTR_ EHCI_QTD_STRUCT_PTR;

typedef USB_Uncached struct {
   // EHCI Defined
   // These should be accessed via macros                                           
   uint_32      HORIZ_LINK_PTR;   /* (5-31) Memory address of 
                                          ** next data object to be processed
                                          ** (4..3) reserved 
                                          ** (2..1) type of the item
                                          ** T (bit 0) indicating pointer 
                                          ** validity
                                          */
   uint_32      EP_CAPAB_CHARAC1; /* bits 31-28: NAK count reload,
                                          ** bit 27: Control endpoint flag
                                          ** bit 26-16: Maximum packet length
                                          ** bit 15: Head of reclamation 
                                          ** list flag
                                          ** bit 14: data toggle control
                                          ** bits 13-12: endpoint speed
                                          ** bit 11-8: endpoint number
                                          ** bits 7: Inactivate on next tr
                                          ** bits 6-0: Device address
                                          */
   uint_32      EP_CAPAB_CHARAC2; /* bits 31-30: High-BW pipe 
                                          ** Multiplier, 
                                          ** bit 29-23: Port number
                                          ** bit 22-16: Hub address
                                          ** bit 15-8: Split completion mask
                                          ** bit 7-0: Interrupt schedule mask
                                          */
   uint_32      CURR_QTD_LINK_PTR;/* bits 31-5: physical memory address
                                          ** of the current xaction processed
                                          */
   uint_32      NEXT_QTD_LINK_PTR;/* bits 31-5: physical memory address
                                          ** of the current xaction processed
                                          ** bit 0: Terminate bit
                                          */
   uint_32      ALT_NEXT_QTD_LINK_PTR;  /* bits 31-5: physical memory address
                                                ** of the current xaction processed
                                                ** bits 4-1: NAK counter
                                                ** bit 0: Terminate bit
                                                */
   uint_32      STATUS;           /* bit 31: data-toggle
                                          ** bits 30-16: total bytes to transfer
                                          ** bit 15: Interrupt on complete
                                          ** bits 11-10: Error counter
                                          ** bit 0: Ping state/Err
                                          ** physical memory address
                                          ** bit 11-0: reserved
                                          */
   uint_32      BUFFER_PTR_0;     /* bit 31-12: 4K-page aligned 
                                          ** physical memory address
                                          ** bit 11-0: reserved
                                          */
   uint_32      BUFFER_PTR_1;     /* bit 31-12: 4K-page aligned 
                                          ** physical memory address
                                          ** bit 7-0: Split-transaction, 
                                          ** complete-split progress
                                          */
   uint_32      BUFFER_PTR_2;     /* bits 31-12: 4K-page aligned 
                                          ** physical memory address
                                          ** bits 11-5: S-bytes
                                          ** bits 4-0: Split-transaction 
                                          ** frame tag
                                          */
   uint_32      BUFFER_PTR_3;     /* bit 31-12: 4K-page aligned 
                                          ** physical memory address
                                          ** bit 11-0: reserved
                                          */
   uint_32      BUFFER_PTR_4;     /* bit 31-12: 4K-page aligned 
                                          ** physical memory address
                                          ** bit 11-0: reserved
                                          */
   // USB Host Stack specific extensions (not read by controller)  
   // These can be accessed directly                                           
    pointer     SCRATCH_PTR;
   pointer     PIPE_DESCR_FOR_THIS_QH;
   uint_32     RESERVED[18];
} EHCI_QH_STRUCT, _PTR_ EHCI_QH_STRUCT_PTR;

typedef USB_Uncached struct {
   // EHCI Defined
   // These should be accessed via macros                                           
   uint_32      NORMAL_PATH_LINK_PTR;   /* (5-31) Memory address of 
                                                ** next data object to be processed
                                                ** in the periodic list
                                                ** bits 4-3: reserved 
                                                ** (2..1) type of the item
                                                ** T (bit 0) indicating pointer 
                                                ** validity
                                                */
   uint_32      BACK_PATH_LINK_PTR;     /* bits 31-5: Memory address of 
                                                ** the queue head,
                                                ** bit 4-3: reserved
                                                ** (2..1) type of the item
                                                ** T (bit 0) indicating pointer 
                                                ** validity
                                                */
   // USB Host Stack specific extensions (not read by controller)  
   // These can be accessed directly                                           
    pointer      SCRATCH_PTR;
   /* 32-bytes aligned */
   uint_32              RESERVED[6];
} EHCI_FSTN_STRUCT, _PTR_ EHCI_FSTN_STRUCT_PTR;

typedef uint_32   EHCI_FRAME_LIST_ELEMENT_POINTER;

typedef struct {
   // EHCI Defined
   // This should be accessed via macros                                           
   uint_32  ALLOCATED_BW;  /* Allocated bandwidth for this frame/uframe */
} FRAME_BW_STRUCT, _PTR_ FRAME_BW_STRUCT_PTR;


#endif // __ehci_h__

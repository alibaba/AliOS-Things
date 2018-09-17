#ifndef __usb_host_hub_prv_h__
#define __usb_host_hub_prv_h__
/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
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
* $FileName: usb_host_hub_prv.h$
* $Version : 3.0.3.0$
* $Date    : Jun-17-2009$
*
* Comments:
*
*   This file defines a structure(s) for hub class driver.
*
*END************************************************************************/

/* System Include files */
#include "usb.h"
#include "hostapi.h"
#include "host_ch9.h"
#include "host_dev_list.h"
#include "host_common.h"
#include "usbprv.h"

#define  C_HUB_LOCAL_POWER          (0)
#define  C_HUB_OVER_CURRENT         (1)
#define  PORT_CONNECTION            (0)
#define  PORT_ENABLE                (1)
#define  PORT_SUSPEND               (2)
#define  PORT_OVER_CURRENT          (3)
#define  PORT_RESET                 (4)
#define  PORT_POWER                 (8)
#define  PORT_LOW_SPEED             (9)
#define  PORT_HIGH_SPEED            (10)
#define  C_PORT_CONNECTION          (16)
#define  C_PORT_ENABLE              (17)
#define  C_PORT_SUSPEND             (18)
#define  C_PORT_OVER_CURRENT        (19)
#define  C_PORT_RESET               (20)
#define  C_PORT_POWER               (24)
#define  PORT_TEST                  (21)
#define  PORT_INDICATOR             (22)

/* structure for HUB class descriptor */
typedef struct _HUB_DESCRIPTOR_STRUCT {
    uint_8     BLENGTH;
    uint_8     BDESCRIPTORTYPE;
    uint_8     BNRPORTS;
    uint_8     WHUBCHARACTERISTICS[2];
    uint_8     BPWRON2PWRGOOD;
    uint_8     BHUBCONTRCURRENT;
    uint_8     DEVICEREMOVABLE[];
/* not used: */
/*    uint_8     PORTPOWERCTRLMASK; */
} HUB_DESCRIPTOR_STRUCT, _PTR_ HUB_DESCRIPTOR_STRUCT_PTR;

typedef struct {
   CLASS_CALL_STRUCT_PTR   CLASS_PTR;
   tr_callback             CALLBACK_FN;
   pointer                 CALLBACK_PARAM;
} HUB_COMMAND, _PTR_ HUB_COMMAND_PTR;

/* Class specific functions exported by hub class driver */
#ifdef __cplusplus
extern "C" {
#endif

extern USB_STATUS usb_class_hub_cntrl_common(HUB_COMMAND_PTR, uint_8, uint_8, uint_16, uint_16, uint_16, uchar_ptr);
extern void usb_class_hub_cntrl_callback(pointer, pointer, uchar_ptr, uint_32, USB_STATUS);
extern USB_STATUS usb_class_hub_get_descriptor(HUB_COMMAND_PTR, uchar_ptr, uchar);
extern USB_STATUS usb_class_hub_set_port_feature(HUB_COMMAND_PTR, uint_8, uint_8);
extern USB_STATUS usb_class_hub_clear_port_feature(HUB_COMMAND_PTR, uint_8, uint_8);
extern USB_STATUS usb_class_hub_get_port_status(HUB_COMMAND_PTR, uint_8, uchar_ptr, uchar);

#ifdef __cplusplus
}
#endif

#endif

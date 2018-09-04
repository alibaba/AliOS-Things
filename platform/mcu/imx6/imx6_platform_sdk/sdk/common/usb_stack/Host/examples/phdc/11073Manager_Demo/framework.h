#ifndef __framework_h__
#define __framework_h__

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
* $FileName: framework.h$
* $Version : 1.0.0.0$
* $Date    : Jan-29-2010$
*
* Comments:
*
*   This file contains keyboard-application types and definitions.
*
*END************************************************************************/


#include "types.h"
#include "usb_host_phdc.h"
#include "usb_host_hub_sm.h"
#include "usbevent.h"
#include "usb_bsp.h"
#include "host_common.h" 
#include "host_driver.h"
#include "hidef.h"        /* for EnableInterrupts macro */
#include "derivative.h"   /* include peripheral declarations */
#include "usb.h"
#include "khci.h"
#include "rtc.h"
#include "sci.h"

#define fw_event_set(event, eventname)  _usb_event_set((event), (eventname))

typedef USB_EVENT_STRUCT FW_EVENT_STRUCT;


extern void ApplicationInit(void);
extern void ApplicationTask(void);


#endif /* __framework_h__ */

/* EOF */
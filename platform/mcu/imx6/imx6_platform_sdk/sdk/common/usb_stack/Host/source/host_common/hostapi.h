#ifndef __hostapi_h__
#define __hostapi_h__ 1

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
* $FileName: hostapi.h$
* $Version : 3.0.5.0$
* $Date    : Aug-12-2009$
*
* Comments:
*
*   This file contains the USB Host API specific data structures and
*   function definitions.
*
*END************************************************************************/

#include "psptypes.h"
#include "usb.h"

#include "host_ch9.h"
#include "host_close.h"
#include "host_cnl.h"

#include "host_common.h"
#include "host_dev_list.h"
#include "host_main.h"
#include "host_rcv.h"
#include "host_shut.h"
#include "host_snd.h"


#define  USB_INTERFACE_DESCRIPTOR_TYPE       (0x04)

/* Available service types */
#define  USB_SERVICE_HOST_RESUME             (0x01)
#define  USB_SERVICE_SYSTEM_ERROR            (0x02)
#define  USB_SERVICE_SPEED                   (0x03)
#define  USB_SERVICE_ATTACH                  (0x04)
#define  USB_SERVICE_DETACH                  (0x05)
#define  USB_SERVICE_STALL_PACKET            (0x06)
#define  USB_SERVICE_ENUMERATION_DONE        (0x07)
#define  USB_SERVICE_1MS_TIMER               (0x08)

/* Used to manage services and service callbacks */
typedef struct host_service_struct
{
   struct host_service_struct _PTR_ NEXT;
   uint_32                          TYPE;
   void                 (_CODE_PTR_ SERVICE)(pointer, uint_32);
} USB_SERVICE_STRUCT, _PTR_ USB_SERVICE_STRUCT_PTR;

/* Prototypes */

#endif

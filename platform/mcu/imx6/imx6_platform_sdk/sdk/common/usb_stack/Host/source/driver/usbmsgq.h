#ifndef __usb_msq_h__
#define __usb_msq_h__ 
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
* $FileName: usbmsgq.h$
* $Version : 3.0.8.0$
* $Date    : Apr-14-2009$
*
* Comments:
*
*   This file containts definitions for use with light weight message queues
*
*END************************************************************************/
#include "psptypes.h"
#include "user_config.h"
#if HIGH_SPEED_DEVICE
#include "ehci_host.h"
#else
#include "khci.h"
#endif

/*--------------------------------------------------------------------------*/
/*
**                            MACRO DEFINITIONS
*/

/* control bits for lwmsgq_send */

/* Error messages */
#define USB_MSGQ_OK                           0x00
#define USB_MSGQ_SEND_ERROR                   0x01
#define USB_MSGQ_RECEIVE_ERROR                0x02
#define USB_MSGQ_INIT_ERROR                   0x03

/* QUEUE status */
#define USB_MSGQ_VALID                        0x00
#define USB_MSGQ_FULL                         0x01
#define USB_MSGQ_EMPTY                        0x02
#define USB_MSGQ_TIMEOUT                      0x03

/*--------------------------------------------------------------------------*/
/*
**                            DATATYPE DECLARATIONS
*/

/*---------------------------------------------------------------------
**
** LWMSGQ STRUCTURE
**
** This structure used to store a circular long word queue.
** The structure must be the LAST if it is included into another
** data structure, as the queue falls off of the end of
** this structure.
*/

typedef struct usb_msgq_element
{
  TR_MSG_STRUCT        MESSAGE_DATA;
  struct usb_msgq_element * NEXT;
  struct usb_msgq_element * PREV;
}USB_MSGQ_ELEMENT, _PTR_ USB_MSGQ_ELEMENT_PTR;
typedef struct usb_msgq_struct
{
   /* A Queue of task descriptors waiting to write */
   USB_MSGQ_ELEMENT_PTR    HEAD;

   /* A Queue of task descriptors waiting to read */
   USB_MSGQ_ELEMENT_PTR    TAIL;

   /* The validity check field */
   uint_32    QUEUE_STATUS;

   /* The size of the message chunk in the queue in _mqx_max_type's */
   uint_32    SIZE;

   /* The maximum number of msgs for the queue, as specified in
    * initialization of the queue.
    */
   uint_32     MAX_SIZE;

   /* The current number of messages in the queue. */
   uint_32    CURRENT_SIZE;

} USBMSGQ_STRUCT, _PTR_ USBMSGQ_STRUCT_PTR;

/*--------------------------------------------------------------------------*/
/*
** FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

uint_32 _usb_msgq_init(uint_32, uint_32);
uint_32 _usb_msgq_send(int*);
uint_32 _usb_msgq_receive(int*);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */

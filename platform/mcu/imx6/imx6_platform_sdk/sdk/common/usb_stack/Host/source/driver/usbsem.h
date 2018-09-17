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
* $FileName: usbevent.h$
* $Version : 0.0.0.1$
* $Date    : Nov-18-2009$
*
* Comments:
*
*   This file containts definitions for use with light weight message queues
*
*END************************************************************************/
#ifndef __usb_sem_h__
#define __usb_sem_h__ 
#include "psptypes.h"
#include "khci.h"

/*--------------------------------------------------------------------------*/
/*
**                            MACRO DEFINITIONS
*/

#define USB_SEM_OK                       0x00
#define USB_SEM_ERROR                    0x01
#define USB_SEM_FREE                     0x02
#define USB_SEM_VALID                    0x04
#define USB_SEM_INVALID                  0x05

/*--------------------------------------------------------------------------*/
/*
**                            DATATYPE DECLARATIONS
*/

typedef struct usb_sem 
{
boolean VALID;
uint_32 VALUE;
} USB_SEM_STRUCT, _PTR_ USB_SEM_STRUCT_PTR;
/*---------------------------------------------------------------------
**
** 
*/

/*--------------------------------------------------------------------------*/
/*
** FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

extern uint_16 _usb_sem_create(USB_SEM_STRUCT_PTR, int_32);
extern uint_16 _usb_sem_post(USB_SEM_STRUCT_PTR);
extern uint_16 _usb_sem_wait(USB_SEM_STRUCT_PTR);
extern uint_16 _usb_sem_wait_ticks(USB_SEM_STRUCT_PTR, uint_32);
extern uint_16 _usb_sem_destroy(USB_SEM_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */

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
* $FileName: host_snd.c$
* $Version : 3.4.10.0$
* $Date    : Sep-10-2009$
*
* Comments:
*
*   This file contains the USB Host API specific functions to send data.
*
*END************************************************************************/
#include "hostapi.h"
#include "user_config.h"
#if HIGH_SPEED_DEVICE
#include "ehci_host.h"
#include "ehci_usbprv_host.h"
#else
#include "khci.h"
#include "usbprv_host.h"
#endif
#include "host_snd.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_get_frame_number
*  Returned Value : frame number
*  Comments       :
* This function can be used to inquire the current frame number.
*
*END*-----------------------------------------------------------------*/
uint_32 _usb_host_get_frame_number
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle  hci_handle
   )
{ /* Body */
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr;
   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)hci_handle;
   
   /* Call the low-level send routine */      
   return((USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR)->HOST_EHCI_GET_FRAME_NUM(hci_handle);
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_get_micro_frame_number
*  Returned Value : frame number
*  Comments       :
* This function can be used to inquire the current micro frame number.
*
*END*-----------------------------------------------------------------*/
uint_32 _usb_host_get_micro_frame_number
   (
      /* [IN] USB Host controller handle */
      _usb_host_handle  hci_handle
   )
{ /* Body */
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr;
   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)hci_handle;
   
    /* Call the low-level send routine */      
    return((USB_HOST_CALLBACK_FUNCTIONS_STRUCT_PTR)usb_host_ptr->CALLBACK_STRUCT_PTR)->HOST_EHCI_GET_MICRO_FRAME_NUM(hci_handle);
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_send_data
*  Returned Value : error or status of the transfer
*  Comments       :
* The Send Data routine is non-blocking routine that causes a block of data 
* to be made available for transmission to the USB host.
*
*END*-----------------------------------------------------------------*/
uint_32 _usb_host_send_data
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle           hci_handle, 

      /* [IN] the pipe handle */
      _usb_pipe_handle           pipe_handle,

      /* [IN] transfer parameters */
      TR_INIT_PARAM_STRUCT_PTR   tr_params_ptr
   )
{ /* Body */
   PIPE_DESCRIPTOR_STRUCT_PTR pipe_descr_ptr;
   USB_HOST_STATE_STRUCT_PTR  usb_host_ptr;
   uint_32  return_code;
   
   PIPE_TR_STRUCT_PTR         pipe_tr_ptr;


   USB_lock();
   return_code = _usb_host_set_up_TR(pipe_handle, tr_params_ptr, &pipe_tr_ptr);
   if (return_code != USB_STATUS_TRANSFER_QUEUED) {
      USB_unlock();
       return return_code;
   } /* Endif */
      
   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)hci_handle;
   pipe_descr_ptr = (PIPE_DESCRIPTOR_STRUCT_PTR)pipe_handle;

   /*Must Flush and Invalidate the buffer before sending
   /receiving the data in it */
   USB_dcache_flush_mlines((void *)pipe_tr_ptr->TX_BUFFER, pipe_tr_ptr->TX_LENGTH);

   /* We have obtained the current TR on the Pipe's TR list 
   ** from _usb_host_set_up_TR
   */
      /* Call the low-level send routine */      
   return_code = _usb_host_send_call_interface (hci_handle, pipe_descr_ptr, pipe_tr_ptr);

   USB_unlock();
   
   if(return_code == USB_OK)
   {
      return USB_STATUS_TRANSFER_QUEUED;
   }
   else
   {
      return return_code;
   }

} /* Endbody */

/* EOF */


   


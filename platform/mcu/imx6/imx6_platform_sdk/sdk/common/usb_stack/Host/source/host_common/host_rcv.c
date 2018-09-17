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
* $FileName: host_rcv.c$
* $Version : 3.4.7.0$
* $Date    : Sep-10-2009$
*
* Comments:
*
*   This file contains the USB Host API specific functions to receive data.
*
*END************************************************************************/
#include "hostapi.h"
#include "user_config.h"
#if HIGH_SPEED_DEVICE
#include "ehci_usbprv_host.h"
#else
#include "usbprv_host.h"
#endif
#include "host_rcv.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_recv_data
*  Returned Value : error or status of the transfer
*  Comments       :
* The Receive Data routine is non-blocking routine that causes a buffer 
* to be made available for data received from the 
* USB host. It takes the buffer and passes it down to lower
level hardware driver. 
*END*-----------------------------------------------------------------*/
uint_32 _usb_host_recv_data
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
   USB_HOST_STATE_STRUCT_PTR usb_host_ptr;
   USB_STATUS status;



/*****************************************************************
CODE FOR ARC FULL SPEED CORE
*****************************************************************/

   PIPE_TR_STRUCT_PTR   pipe_tr_ptr;
   USB_STATUS  return_code;   

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
   USB_dcache_invalidate_mlines((void *)pipe_tr_ptr->RX_BUFFER, pipe_tr_ptr->RX_LENGTH);  

   /* We have obtained the current TR on the Pipe's TR list 
   ** from _usb_host_set_up_TR
   */

   /* Call the low-level routine to receive data */
   status = _usb_host_recv_data_call_interface (hci_handle, pipe_descr_ptr, pipe_tr_ptr);
         
   USB_unlock();
   
   if(status == USB_OK) 
   {
      return USB_STATUS_TRANSFER_QUEUED;
   }
   else
   {
      return status;
   }

   
} /* Endbody */

/* EOF */


   


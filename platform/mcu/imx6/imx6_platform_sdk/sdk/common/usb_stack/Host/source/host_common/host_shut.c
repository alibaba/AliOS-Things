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
* $FileName: host_shut.c$
* $Version : 3.4.7.0$
* $Date    : Sep-11-2009$
*
* Comments:
*
*   This file contains the USB Host API specific function to shutdown 
*   the host
*
*END************************************************************************/
#include "hostapi.h"
#include "user_config.h"
#if HIGH_SPEED_DEVICE
#include "ehci_usbprv_host.h"
#else
#include "usbprv_host.h"
#endif
#include "host_shut.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_shutdown
*  Returned Value : USB_OK or error code
*  Comments       :
*        Shutdown an initialized USB Host
*
*END*-----------------------------------------------------------------*/
void _usb_host_shutdown
   (
      /* [IN] the USB_host handle */
      _usb_host_handle         hci_handle
   )
{ /* Body */
   USB_STATUS                 status;
   USB_HOST_STATE_STRUCT_PTR  usb_host_ptr;
   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)hci_handle;
   /* De-initialize and disconnect the host hardware from the bus */
   status = _usb_host_shutdown_call_interface (hci_handle);
   
   /* Free all Pipe Descriptors */
   free((pointer)usb_host_ptr->PIPE_DESCRIPTOR_BASE_PTR);

   /* Free the USB state structure */
   free(hci_handle);
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_host_bus_control
*  Returned Value : None
*  Comments       :
* _usb_host_bus_control controls the bus operations such as asserting and 
* deasserting the bus reset, asserting and deasserting resume, 
* suspending and resuming the SOF generation
*
*END*-----------------------------------------------------------------*/

void _usb_host_bus_control
   (
      /* [IN] the USB Host state structure */
      _usb_host_handle           hci_handle,
      
      /* The operation to be performed on the bus */
      uint_8                     bus_control
   )
{ /* Body */

   USB_STATUS status;
   USB_HOST_STATE_STRUCT_PTR         usb_host_ptr;
   
   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)hci_handle;

   status = _usb_host_bus_control_data_call_interface (hci_handle, bus_control);
    
} /* Endbody */

/* EOF */


   


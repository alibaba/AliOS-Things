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
* $FileName: usb_host_hub.c$
* $Version : 3.0.5.0$
* $Date    : Jun-17-2009$
*
* Comments:
*
*   This file contains the implementation of class driver for hub devices.
*
*END************************************************************************/

#include "usb_host_hub.h"
#include "usb_host_hub_prv.h"

static USB_HUB_CLASS_INTF_STRUCT_PTR hub_anchor = NULL;

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hub_init
* Returned Value : None
* Comments       :
*     This function is called by common class to initialize the class driver. It
*     is called in response to a select interface call by application
*
*END*--------------------------------------------------------------------*/

void usb_class_hub_init
   (
      /* [IN] structure with USB pipe information on the interface */
      PIPE_BUNDLE_STRUCT_PTR      pbs_ptr,

      /* [IN] hub call struct pointer */
      CLASS_CALL_STRUCT_PTR       ccs_ptr
   )
{ /* Body */
   USB_STATUS                    status;
   USB_HUB_CLASS_INTF_STRUCT_PTR if_ptr = ccs_ptr->class_intf_handle;
   
   /* Make sure the device is still attached */

   status = usb_host_class_intf_init(pbs_ptr, if_ptr, &hub_anchor);
   if (status == USB_OK) {
      /*
      ** We generate a code_key based on the attached device. This is used to
      ** verify that the device has not been detached and replaced with another.
      */
      ccs_ptr->code_key = 0;
      ccs_ptr->code_key = usb_host_class_intf_validate(ccs_ptr);

      if_ptr->P_CONTROL = usb_hostdev_get_pipe_handle(pbs_ptr, USB_CONTROL_PIPE,
         0);
      if_ptr->P_INT_IN = usb_hostdev_get_pipe_handle(pbs_ptr,
         USB_INTERRUPT_PIPE, USB_RECV);
   } /* Endif */

   /* Signal that an error has occured by setting the "code_key" to 0 */
   if (status || !if_ptr->P_INT_IN || !if_ptr->P_CONTROL) {
      ccs_ptr->code_key = 0;
   } /* Endif */
   
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hub_cntrl_callback
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     This is the callback used when hub information is sent or received
*
*END*--------------------------------------------------------------------*/

void usb_class_hub_cntrl_callback
   (
      /* [IN] Unused */
      pointer     pipe,
      /* [IN] Pointer to the class interface instance */
      pointer     param,
      /* [IN] Data buffer */
      uchar_ptr   buffer,
      /* [IN] Length of buffer */
      uint_32     len,
      /* [IN] Error code (if any) */
      USB_STATUS  status
   )
{ /* Body */
   USB_HUB_CLASS_INTF_STRUCT_PTR      if_ptr;

   /*
   ** There is need for USB_lock in the callback function, and there is also no
   ** need to check if the device is still attached (otherwise this callback
   ** would never have been called!
   */

   /* Get class interface handle, reset IN_SETUP and call callback */
   if_ptr = (USB_HUB_CLASS_INTF_STRUCT_PTR)param;

   if_ptr->IN_SETUP = FALSE;

   if (if_ptr->USER_CALLBACK) {
      if_ptr->USER_CALLBACK(pipe, if_ptr->USER_PARAM, buffer, len, status);
   } /* Endif */

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hub_cntrl_common
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     This function is used to send a control request
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_hub_cntrl_common
   (
      /* [IN] The communication device common command structure */
      HUB_COMMAND_PTR         com_ptr,
      /* [IN] Bitmask of the request type */
      uint_8                  bmrequesttype,
      /* [IN] Request code */
      uint_8                  brequest,
      /* [IN] Value to copy into WVALUE field of the REQUEST */
      uint_16                 wvalue,
      /* [IN] Length of the data associated with REQUEST */
      uint_16                 windex,
      /* [IN] Index field of CTRL packet */
      uint_16                 wlength,
      /* [IN] Pointer to data buffer used to send/recv */
      uchar_ptr               data
   )
{ /* Body */
   USB_HUB_CLASS_INTF_STRUCT_PTR    if_ptr;
   USB_SETUP                        req;
   USB_STATUS                       status = USBERR_NO_INTERFACE;

   USB_lock();
   /* Validity checking */
   if (usb_host_class_intf_validate(com_ptr->CLASS_PTR)) {
      if_ptr =
         (USB_HUB_CLASS_INTF_STRUCT_PTR)com_ptr->CLASS_PTR->class_intf_handle;
      status = usb_hostdev_validate(if_ptr->G.dev_handle);
   } /* Endif */

   if (!status && if_ptr->IN_SETUP) {
      status = USBERR_TRANSFER_IN_PROGRESS;
   } /* Endif */

   if (!status) {
      /* Save the higher level callback and ID */
      if_ptr->USER_CALLBACK = com_ptr->CALLBACK_FN;
      if_ptr->USER_PARAM = com_ptr->CALLBACK_PARAM;

      /* Setup the request */
      req.BMREQUESTTYPE = bmrequesttype;
      req.BREQUEST = brequest;
      htou16(req.WVALUE, wvalue);
      htou16(req.WINDEX, windex);
      htou16(req.WLENGTH, wlength);
      status = _usb_hostdev_cntrl_request(if_ptr->G.dev_handle, &req, data,
         usb_class_hub_cntrl_callback, if_ptr);
      if (status == USB_STATUS_TRANSFER_QUEUED) {
         if_ptr->IN_SETUP = TRUE;
      } /* Endif */
   } /* Endif */
   USB_unlock();

   return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hub_get_descriptor
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     This function is called by the application to read the descriptor
*     of hub device
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_hub_get_descriptor
   (
      /* [IN] The communication device common command structure */
      HUB_COMMAND_PTR         com_ptr,
      /* [IN] descriptor buffer */
      uchar_ptr               buffer,
      /* [IN] buffer length (or, better said, how many bytes to read) */
      uchar                   len
   )
{ /* Body */
   USB_STATUS error = USB_OK;

   USB_lock();

   error = usb_class_hub_cntrl_common(
      com_ptr,
      REQ_TYPE_DEVICE | REQ_TYPE_IN | REQ_TYPE_CLASS,
      REQ_GET_DESCRIPTOR,
      0,
      0,
      len,
      buffer
   );

   USB_unlock();

   
   return error;

} /* EndBody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hub_set_port_feature
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     Sets feature of specified hub port
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_hub_set_port_feature
   (
      /* [IN] Class Interface structure pointer */
      HUB_COMMAND_PTR            com_ptr,
      /* [IN] port number */
      uint_8                     port_nr,
      /* [IN] feature ID */
      uint_8                     feature
   )
{ /* Body */

   return usb_class_hub_cntrl_common(com_ptr,
      REQ_TYPE_OUT | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE | REQ_TYPE_ENDPOINT,
      REQ_SET_FEATURE,
      feature,
      port_nr,
      0,
      NULL);

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hub_clear_port_feature
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     Clears feature of selected hub port
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_hub_clear_port_feature
   (
      /* [IN] Class Interface structure pointer */
      HUB_COMMAND_PTR            com_ptr,
      /* [IN] port number */
      uint_8                     port_nr,
      /* [IN] feature ID */
      uint_8                     feature
   )
{ /* Body */

   return usb_class_hub_cntrl_common(com_ptr,
      REQ_TYPE_OUT | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE | REQ_TYPE_ENDPOINT,
      REQ_CLEAR_FEATURE,
      feature,
      port_nr,
      0,
      NULL);

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_hub_get_port_status
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     Gets the status of specified port
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_hub_get_port_status
   (
      /* [IN] Class Interface structure pointer */
      HUB_COMMAND_PTR            com_ptr,
      /* [IN] port number */
      uint_8                     port_nr,
      /* [IN] status buffer */
      uchar_ptr                  buffer,
      /* [IN] buffer length (or, better said, how many bytes to read) */
      uchar                      len
   )
{ /* Body */

   return usb_class_hub_cntrl_common(com_ptr,
      REQ_TYPE_IN | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE | REQ_TYPE_ENDPOINT,
      REQ_GET_STATUS,
      0,
      port_nr,
      len,
      buffer);

} /* Endbody */


/* EOF */

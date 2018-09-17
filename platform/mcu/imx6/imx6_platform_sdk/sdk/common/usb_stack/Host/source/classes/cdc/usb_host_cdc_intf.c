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
* $FileName: usb_host_cdc_intf.c$
* $Version : 3.4.4.0$
* $Date    : Sep-10-2009$
*
* Comments:
*
*   This file Contains the implementation of class driver for CDC devices.
*
*END************************************************************************/
//#include <usbprv_host.h>
#include "hostapi.h"
//#include <usb_mqx.h>
#include "usb_host_cdc.h"
#include "mem_util.h"
#if HIGH_SPEED_DEVICE
#include "ehci_usbprv_host.h"
#endif /* HIGH_SPEED_DEVICE */

static CLASS_CALL_STRUCT_PTR           ctrl_anchor = NULL;
static CLASS_CALL_STRUCT_PTR           data_anchor = NULL;

static pointer                         acm_anchor_abstract; /* used for lower abstract layer */
static pointer                         data_anchor_abstract; /* used for lower abstract layer */

static void usb_class_cdc_bind_data_interface(CLASS_CALL_STRUCT_PTR, pointer);
static void usb_class_cdc_bind_ctrl_interface(CLASS_CALL_STRUCT_PTR, pointer);
static void usb_class_cdc_register_interface(CLASS_CALL_STRUCT_PTR, CLASS_CALL_STRUCT_PTR _PTR_);
static void usb_class_cdc_unregister_interface(CLASS_CALL_STRUCT_PTR, CLASS_CALL_STRUCT_PTR _PTR_);

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_cdc_acm_init
* Returned Value : None
* Comments       :
*     This function is called by common class to initialize the class driver
*     for AbstractClassControl. It is called in response to a select
*     interface call by application.
*
*END*--------------------------------------------------------------------*/

void usb_class_cdc_acm_init
   (
      /* [IN]  structure with USB pipe information on the interface */
      PIPE_BUNDLE_STRUCT_PTR      pbs_ptr,

      /* [IN] acm call struct pointer */
      CLASS_CALL_STRUCT_PTR       ccs_ptr
   )
{ /* Body */
    USB_ACM_CLASS_INTF_STRUCT_PTR acm_anchor = NULL;
    USB_ACM_CLASS_INTF_STRUCT_PTR if_ptr = ccs_ptr->class_intf_handle;
    USB_STATUS                    status;
 
    /* Make sure the device is still attached */
    USB_lock();
    status = usb_host_class_intf_init(pbs_ptr, if_ptr, &acm_anchor_abstract);
    if (status == USB_OK) {
        /*
        ** We generate a code_key based on the attached device. This is used to
        ** verify that the device has not been detached and replaced with another.
        */
        ccs_ptr->code_key = 0;
        ccs_ptr->code_key = usb_host_class_intf_validate(ccs_ptr);
  
        if_ptr->CDC_G.IFNUM = ((INTERFACE_DESCRIPTOR_PTR)if_ptr->CDC_G.G.intf_handle)->bInterfaceNumber;
        if_ptr->interrupt_pipe = usb_hostdev_get_pipe_handle(pbs_ptr, USB_INTERRUPT_PIPE, 0);
  
        if (USB_OK != (status = _usb_event_init(&if_ptr->acm_event))) {
           status = USBERR_INIT_FAILED;
        }
        else {
            /* prepare events to be auto or manual */
            //_lwevent_set_auto_clear(&if_ptr->acm_event, USB_ACM_CTRL_PIPE_FREE | USB_ACM_INT_PIPE_FREE);
            /* pre-set events */
            _usb_event_set(&if_ptr->acm_event, USB_ACM_CTRL_PIPE_FREE | USB_ACM_INT_PIPE_FREE);
        }
    } /* Endif */
 
    /* Signal that an error has occured by setting the "code_key" */
    if (status) {
        ccs_ptr->code_key = 0;
    } /* Endif */
 
    USB_unlock();
    
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_cdc_data_init
* Returned Value : None
* Comments       :
*     This function is called by common class to initialize the class driver
*     for AbstractClassControl. It is called in response to a select
*     interface call by application.
*
*END*--------------------------------------------------------------------*/
void usb_class_cdc_data_init
   (
      /* [IN]  structure with USB pipe information on the interface */
      PIPE_BUNDLE_STRUCT_PTR      pbs_ptr,

      /* [IN] acm call struct pointer */
      CLASS_CALL_STRUCT_PTR       ccs_ptr
   )
{ /* Body */
    USB_DATA_CLASS_INTF_STRUCT_PTR if_ptr = ccs_ptr->class_intf_handle;
    USB_STATUS                     status;
 
    /* Make sure the device is still attached */
    USB_lock();
    status = usb_host_class_intf_init(pbs_ptr, if_ptr, &data_anchor_abstract);
    if (status == USB_OK) {
        /*
        ** We generate a code_key based on the attached device. This is used to
        ** verify that the device has not been detached and replaced with another.
        */
        ccs_ptr->code_key = 0;
        ccs_ptr->code_key = usb_host_class_intf_validate(ccs_ptr);
  
        if_ptr->CDC_G.IFNUM = ((INTERFACE_DESCRIPTOR_PTR)if_ptr->CDC_G.G.intf_handle)->bInterfaceNumber;
        if_ptr->in_pipe = usb_hostdev_get_pipe_handle(pbs_ptr, USB_BULK_PIPE, USB_RECV);
        if_ptr->out_pipe = usb_hostdev_get_pipe_handle(pbs_ptr, USB_BULK_PIPE, USB_SEND);

        if ((if_ptr->in_pipe == NULL) && (if_ptr->out_pipe == NULL))
            status = USBERR_OPEN_PIPE_FAILED;
        else if (USB_OK != (status = _usb_event_init(&if_ptr->data_event)))
                status = USBERR_INIT_FAILED;
        else {
            /* prepare events to be auto or manual */
            //_lwevent_set_auto_clear(&if_ptr->data_event, USB_DATA_READ_PIPE_FREE | USB_DATA_SEND_PIPE_FREE);
            /* pre-set events */
            _usb_event_set(&if_ptr->data_event, USB_DATA_READ_PIPE_FREE | USB_DATA_SEND_PIPE_FREE);

            if (if_ptr->out_pipe) {
                /* Don't use host - predefined constant for NAK_COUNT...
                ** NOTE!!!
                ** This hack is not very clean. We need to maximize number of retries to minimize the time of
                ** transaction (minimize task's time while waiting for 1 transaction to be done (with or without data))
                ** The time depends on user expecatation of the read() latency, on the delay between 2 NAKs and on number
                ** of NAKs to be performed.
                ** The workaround is to limit amount of retries for the pipe maximally to 3.
                ** Number 3 is hard-coded here for now.
                */
                if (((PIPE_DESCRIPTOR_STRUCT_PTR) if_ptr->in_pipe)->NAK_COUNT > 3)
                    ((PIPE_DESCRIPTOR_STRUCT_PTR) if_ptr->in_pipe)->NAK_COUNT = 3; /* don't use host - predefined constant */
            }
            if (if_ptr->in_pipe) {
                /* The same as for OUT pipe applies here */
                if (((PIPE_DESCRIPTOR_STRUCT_PTR) if_ptr->out_pipe)->NAK_COUNT > 3)
                    ((PIPE_DESCRIPTOR_STRUCT_PTR) if_ptr->out_pipe)->NAK_COUNT = 3; /* don't use host - predefined constant */
            
                /* initialize buffer */
                /* size of buffer equals to the size of endpoint data size */
                if_ptr->RX_BUFFER_SIZE = ((PIPE_INIT_PARAM_STRUCT_PTR) (if_ptr->in_pipe))->MAX_PACKET_SIZE;
                if (NULL == (if_ptr->RX_BUFFER = USB_mem_alloc_zero(if_ptr->RX_BUFFER_SIZE))) {
                    status = USBERR_ALLOC;
                }
                else {
                    /* initialize members */
                    if_ptr->RX_BUFFER_APP = if_ptr->RX_BUFFER_DRV = if_ptr->RX_BUFFER;
                }
            }
        }
    } /* Endif */

    /* Signal that an error has occured by setting the "code_key" */
    if (status) {
        ccs_ptr->code_key = 0;
    } /* Endif */
 
    USB_unlock();
       
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_cdc_bind_data_interface
* Returned Value :
* Comments       :
*     This function is used to bind data interface with control interface.
*     It must be run with interrupts disabled to have interfaces validated.
*END*--------------------------------------------------------------------*/

static void usb_class_cdc_bind_data_interface
   (
      /* [IN] pointer to control interface call struct */
      CLASS_CALL_STRUCT_PTR                control_class_ptr,
      
      /* [IN] pointer to (data) interface desriptor to be controlled */
      pointer                              data_descriptor_ptr
   )
{
    CLASS_CALL_STRUCT_PTR          ccs_data_interface = data_anchor;
    USB_DATA_CLASS_INTF_STRUCT_PTR data_interface;
 
    /* Search for all registered data interfaces */
    while (ccs_data_interface != NULL) {
        if (usb_host_class_intf_validate(ccs_data_interface)) {
            data_interface = ccs_data_interface->class_intf_handle;
            /* test if interface descriptor matches */
            if (data_interface->CDC_G.G.intf_handle == data_descriptor_ptr) {
                data_interface->BOUND_CONTROL_INTERFACE = control_class_ptr;
                break;
            }
        }
        ccs_data_interface = ccs_data_interface->next;
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_cdc_bind_ctrl_interface
* Returned Value :
* Comments       :
*     This function is used to bind control interface with data interface.
*     It must be run with interrupts disabled to have interfaces validated.
*END*--------------------------------------------------------------------*/

static void usb_class_cdc_bind_ctrl_interface
   (
      /* [IN] pointer to data interface */
      CLASS_CALL_STRUCT_PTR                data_class_ptr,
      
      /* [IN] pointer to (control) interface desriptor to be used to control */
      pointer                              control_descriptor_ptr
   )
{
    CLASS_CALL_STRUCT_PTR     ccs_control_interface = ctrl_anchor;
    USB_CDC_GENERAL_CLASS_PTR control_interface;
 
    /* Search for all registered control interfaces */
    while (ccs_control_interface != NULL) {
        if (usb_host_class_intf_validate(ccs_control_interface)) {
            control_interface = ccs_control_interface->class_intf_handle;
            /* test if interface descriptor matches */
            if (control_interface->G.intf_handle == control_descriptor_ptr) {
                ((USB_DATA_CLASS_INTF_STRUCT_PTR) data_class_ptr->class_intf_handle)->BOUND_CONTROL_INTERFACE = ccs_control_interface;
                break;
            }
        }
        ccs_control_interface = ccs_control_interface->next;
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_cdc_register_interface
* Returned Value :
* Comments       :
*     This function is used to register interface in the chain.
*     It must be run with interrupts disabled to have interfaces validated.
*END*--------------------------------------------------------------------*/

static void usb_class_cdc_register_interface
   (
      /* [IN] pointer to registered interface */
      CLASS_CALL_STRUCT_PTR  class_ptr,
      
      CLASS_CALL_STRUCT_PTR _PTR_ anchor_ptr
   )
{
    CLASS_CALL_STRUCT_PTR     interface_parser;

    if (*anchor_ptr == NULL) {
        *anchor_ptr = class_ptr;
        (*anchor_ptr)->next = NULL;
    }
    else {
        interface_parser = *anchor_ptr;
        /* add new class to the list */
        while (interface_parser->next != NULL)
            interface_parser = interface_parser->next;
        interface_parser->next = class_ptr;
        class_ptr->next = NULL;
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_cdc_unregister_interface
* Returned Value :
* Comments       :
*     This function is used to unregister interface from the chain.
*     It must be run with interrupts disabled to have interfaces validated.
*END*--------------------------------------------------------------------*/

static void usb_class_cdc_unregister_interface
   (
      /* [IN] pointer to control interface */
      CLASS_CALL_STRUCT_PTR       class_ptr,
      
      CLASS_CALL_STRUCT_PTR _PTR_ anchor_ptr
   )
{
    CLASS_CALL_STRUCT_PTR     interface_parser;
   
    if (*anchor_ptr == NULL)
        return;
   
    if (*anchor_ptr == class_ptr)
        *anchor_ptr = class_ptr->next;
    else {
        interface_parser = *anchor_ptr;
        /* add new control class to the list */
        while (interface_parser->next != NULL)
            if (interface_parser->next == class_ptr)  {
      	        interface_parser->next = class_ptr->next;
      	    break;	
        }
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_cdc_get_ctrl_interface
* Returned Value : control interface instance
* Comments       :
*     This function is used to find registered control interface in the chain.
*     It must be run with interrupts disabled to have interfaces validated.
*END*--------------------------------------------------------------------*/

CLASS_CALL_STRUCT_PTR usb_class_cdc_get_ctrl_interface
   (
      /* [IN] pointer to interface handle */
      pointer           intf_handle
   )
{
    CLASS_CALL_STRUCT_PTR   control_parser;

    /* find this acm interface in the list with interface descriptor */
    for (control_parser = ctrl_anchor; control_parser != NULL; control_parser = control_parser->next)
        if (usb_host_class_intf_validate(control_parser))
            if (((USB_ACM_CLASS_INTF_STRUCT_PTR) control_parser->class_intf_handle)->CDC_G.G.intf_handle == intf_handle)
                break;
       
   return control_parser;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_cdc_get_data_interface
* Returned Value : data interface instance
* Comments       :
*     This function is used to find registered data interface in the chain.
*     It must be run with interrupts disabled to have interfaces validated.
*END*--------------------------------------------------------------------*/

CLASS_CALL_STRUCT_PTR usb_class_cdc_get_data_interface
   (
      /* [IN] pointer to interface handle */
      pointer           intf_handle
   )
{
    CLASS_CALL_STRUCT_PTR   data_parser;

    /* find this acm interface in the list with interface descriptor */
    for (data_parser = data_anchor; data_parser != NULL; data_parser = data_parser->next)
        if (usb_host_class_intf_validate(data_parser))
            if (((USB_ACM_CLASS_INTF_STRUCT_PTR) data_parser->class_intf_handle)->CDC_G.G.intf_handle == intf_handle)
                break;
  
    return data_parser;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_cdc_get_acm_descriptors
* Returned Value : USB_OK
* Comments       :
*     This function is hunting for descriptors in the device configuration
*     and fills back fields if the descriptor was found.
*     Must be run in locked state and validated USB device or
*     directly from attach event.
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_cdc_get_acm_descriptors
(
      /* [IN] pointer to device instance */
      _usb_device_instance_handle      dev_handle,

      /* [IN] pointer to interface descriptor */
      _usb_interface_descriptor_handle intf_handle,

      USB_CDC_DESC_ACM_PTR _PTR_       acm_desc,
      USB_CDC_DESC_CM_PTR _PTR_        cm_desc,
      USB_CDC_DESC_HEADER_PTR _PTR_    header_desc,
      USB_CDC_DESC_UNION_PTR _PTR_     union_desc
) 
{
    INTERFACE_DESCRIPTOR_PTR   intf_ptr =
        (INTERFACE_DESCRIPTOR_PTR)intf_handle;
    USB_STATUS                 status;
    int_32 i;
    USB_CDC_FUNC_DESC_PTR      fd;

    status = USB_OK;
    /* collect all interface functional descriptors */
    for (i = 0; ; i++) {
        if (USB_OK != _usb_hostdev_get_descriptor(
            dev_handle,
            intf_handle,
            USB_DESC_TYPE_CS_INTERFACE,  /* Functional descriptor for interface */
            (uint_8)i,                           /* Index of descriptor */
            intf_ptr->bAlternateSetting, /* Index of interface alternate */
            (pointer _PTR_)&fd))
        {
            /* Here we can check capabilities from functional descriptors */
            /* But for now, nothing is checked */
            if (union_desc == NULL)
                status = USBERR_INIT_FAILED;
            break;
        }
  
        switch (fd->header.bDescriptorSubtype) {
            case USB_DESC_SUBTYPE_CS_HEADER:
                *header_desc = &fd->header;
                if (utoh16((*header_desc)->bcdCDC) > 0x0110)
                    status = USBERR_INIT_FAILED;
                break;
            case USB_DESC_SUBTYPE_CS_UNION:
                /* Check if this union descriptor describes master for this interface */
                if (fd->uni.bMasterInterface == intf_ptr->bInterfaceNumber) {
                    /* Check if another union descriptor has not been already assigned */
                    if (*union_desc == NULL)
                        *union_desc = &fd->uni;
                    else
                        status = USBERR_INIT_FAILED;
              }
              break;
            case USB_DESC_SUBTYPE_CS_ACM:
                *acm_desc = &fd->acm;
                break;
            case USB_DESC_SUBTYPE_CS_CM:
                *cm_desc = &fd->cm;
                break;
        }
        if (status != USB_OK)
            break;
    }

    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_cdc_set_acm_descriptors
* Returned Value : USB_OK if validation passed
* Comments       :
*     This function is used to set descriptors for ACM interface
*     Descriptors can be used afterwards by application or by driver
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_cdc_set_acm_descriptors
   (
      /* [IN] acm call struct pointer */
      CLASS_CALL_STRUCT_PTR            ccs_ptr,
      
      /* [IN] acm functional descriptor pointer */
      USB_CDC_DESC_ACM_PTR             acm_desc,

      /* [IN] acm functional descriptor pointer */
      USB_CDC_DESC_CM_PTR              cm_desc,

      /* [IN] header functional descriptor pointer */
      USB_CDC_DESC_HEADER_PTR          header_desc,

      /* [IN] union functional descriptor pointer */
      USB_CDC_DESC_UNION_PTR           union_desc
   )
{ /* Body */
    USB_ACM_CLASS_INTF_STRUCT_PTR if_ptr;
    USB_STATUS                    status = USBERR_NO_INTERFACE;
  
  	UNUSED(cm_desc)
    /* Make sure the device is still attached */
    USB_lock();
    
    /* Validity checking */
    if (usb_host_class_intf_validate(ccs_ptr)) {
        if_ptr = (USB_ACM_CLASS_INTF_STRUCT_PTR)ccs_ptr->class_intf_handle;
    
        if_ptr->acm_desc = acm_desc;
        if_ptr->header_desc = header_desc;
        if_ptr->union_desc = union_desc;
 
    } /* Endif */
    
    USB_unlock();

    
    return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_cdc_get_ctrl_descriptor
* Returned Value : USB_OK if found
* Comments       :
*     This function is hunting for descriptor of control interface, which
*     controls data interface identified by descriptor (intf_handle)
*     The found control interface descriptor is written to if_desc_ptr
*     Must be run in locked state and validated USB device or
*     directly from attach event.
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_cdc_get_ctrl_descriptor
    (
      /* [IN] pointer to device instance */
      _usb_device_instance_handle      dev_handle,

      /* [IN] pointer to interface descriptor */
      _usb_interface_descriptor_handle intf_handle,
      
      INTERFACE_DESCRIPTOR_PTR _PTR_   if_desc_ptr
    )
{
    USB_CDC_FUNC_DESC_PTR      fd_desc;
    INTERFACE_DESCRIPTOR_PTR   if_desc;
    INTERFACE_DESCRIPTOR_PTR   intf_ptr =
       (INTERFACE_DESCRIPTOR_PTR)intf_handle;
    USB_STATUS                 status;
    int_32 i, j;
    int_32 ctrl_intf = -1; /* assume not found */

    status = USBERR_INIT_FAILED; 
    *if_desc_ptr = NULL; /* presume not found */

    /* Find union descriptor containing this data descriptor */
    for (i = 0; ; i++) {
        if (USB_OK == _usb_hostdev_get_descriptor(
            dev_handle,
            NULL,
            USB_DESC_TYPE_CS_INTERFACE,  /* Get any functional descriptor */
            (uint_8)i,                           /* Index of descriptor */
            intf_ptr->bAlternateSetting, /* Index of interface alternate */
            (pointer _PTR_) &fd_desc))
        {
            if (fd_desc->header.bDescriptorSubtype != USB_DESC_SUBTYPE_CS_UNION)
                continue;
            /* Here we get only when union descriptor found */
            /* Find if there is this data interface in the found union descriptor */
            for (j = 0; j < fd_desc->uni.bFunctionLength - 3; j++) {
                if (intf_ptr->bInterfaceNumber == fd_desc->uni.bSlaveInterface[j]) {
                    /* Union descriptor containing our data interface found */
                    ctrl_intf = fd_desc->uni.bMasterInterface; /* copy control interface number */
                    break;
                }
            }
            if (ctrl_intf != -1) /* when control interface was already found */
                break; /* finish any next search for union descriptors */
        }
        else
            break;
    }
    
    if (ctrl_intf != -1) {
        /* find interface descriptor from control interface number */
        for (i = 0; ; i++) {
            if (USB_OK == _usb_hostdev_get_descriptor(
                dev_handle,
                NULL,                        /* Search within device scope */
                USB_DESC_TYPE_IF,            /* Get any interface descriptor */
                (uint_8)i,                           /* Index of descriptor */
                intf_ptr->bAlternateSetting, /* Index of interface alternate */
                (pointer _PTR_) &if_desc))
            {
                if (if_desc->bInterfaceNumber == ctrl_intf) {
                    /* Interface descriptor of control interface found */
                    *if_desc_ptr = if_desc;
                    status = USB_OK;
                    break;
                }
            }
        }
    }

    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_cdc_bind_data_interfaces
* Returned Value : USB_OK if found
* Comments       :
*     All data interfaces belonging to ACM control instance
*     (specified by ccs_ptr) will be bound to this interface.
*     Union functional descriptor describes which data interfaces
*     should be bound.
*     Must be run in locked state and validated USB device or
*     directly from attach event.
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_cdc_bind_data_interfaces
   (
      /* [IN] pointer to device instance */
      _usb_device_instance_handle      dev_handle,

      CLASS_CALL_STRUCT_PTR            ccs_ptr
   ) 
{
    USB_CDC_DESC_UNION_PTR            union_desc;
    INTERFACE_DESCRIPTOR_PTR          if_desc;
    int_32 i, j;
 
    union_desc = ((USB_ACM_CLASS_INTF_STRUCT_PTR) ccs_ptr->class_intf_handle)->union_desc;

    /* Now link all already initialized interfaces
        that should be controlled by this ACM */
    for (i = 0; ; i++) {
        if (USB_OK != _usb_hostdev_get_descriptor(
            dev_handle,
            NULL,
            USB_DESC_TYPE_IF,  /* search for all interfaces */
            (uint_8)i,                 /* Index of descriptor */
            0,                 /* Index of interface alternate */
            (pointer _PTR_) &if_desc))
                break;
        for (j = 0; j < union_desc->bFunctionLength - 3; j++)
            if (if_desc->bInterfaceNumber == union_desc->bSlaveInterface[j]) {
                /* the found (data) interface if_desc should be controlled by this ACM */
                usb_class_cdc_bind_data_interface((pointer)ccs_ptr, if_desc);
                break;
            }
    }
    /* Store control interface for data interfaces included later */
    usb_class_cdc_register_interface(ccs_ptr, &ctrl_anchor);
    
    return USB_OK;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_cdc_unbind_data_interfaces
* Returned Value : USB_OK if found
* Comments       :
*     All data interfaces bound to ACM control instance
*     will be unbound from this interface.
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_cdc_unbind_data_interfaces
   (
      CLASS_CALL_STRUCT_PTR            ccs_ptr
   ) 
{
    CLASS_CALL_STRUCT_PTR    data_parser;

    for (data_parser = data_anchor; data_parser != NULL; data_parser = data_parser->next) 
    {
        if (usb_host_class_intf_validate(data_parser))
            if (((USB_DATA_CLASS_INTF_STRUCT_PTR) (data_parser->class_intf_handle))->BOUND_CONTROL_INTERFACE == ccs_ptr)
                ((USB_DATA_CLASS_INTF_STRUCT_PTR) (data_parser->class_intf_handle))->BOUND_CONTROL_INTERFACE = NULL;
    }
    /* Delete control interface for data interfaces included later */
    usb_class_cdc_unregister_interface(ccs_ptr, &ctrl_anchor);

    return USB_OK;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_cdc_bind_acm_interface
* Returned Value : USB_OK
* Comments       :
*     Data interface (specified by ccs_ptr) will be bound to appropriate
*     control interface.
*     Must be run in locked state and validated USB device or
*     directly from attach event.
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_cdc_bind_acm_interface
   (
      CLASS_CALL_STRUCT_PTR            ccs_ptr,
      
      INTERFACE_DESCRIPTOR_PTR         if_desc
   )
{
    
    usb_class_cdc_bind_ctrl_interface(ccs_ptr, if_desc);
    usb_class_cdc_register_interface(ccs_ptr, &data_anchor);

    return USB_OK;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_cdc_unbind_acm_interface
* Returned Value : USB_OK
* Comments       :
*     Data interface (specified by ccs_ptr) will be unbound from
*     appropriate control interface.
*     Must be run in locked state and validated USB device.
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_cdc_unbind_acm_interface
   (
      CLASS_CALL_STRUCT_PTR            ccs_ptr
   )
{
    USB_DATA_CLASS_INTF_STRUCT_PTR       if_ptr;

    if (usb_host_class_intf_validate(ccs_ptr)) {
        if_ptr = (USB_DATA_CLASS_INTF_STRUCT_PTR) ccs_ptr->class_intf_handle;
        usb_class_cdc_unregister_interface(ccs_ptr, &data_anchor);
        if_ptr->BOUND_CONTROL_INTERFACE = NULL;
    }

    return USB_OK;
}

/* EOF */

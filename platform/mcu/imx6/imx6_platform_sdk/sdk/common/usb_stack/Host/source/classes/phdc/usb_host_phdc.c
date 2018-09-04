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
* $FileName: usb_host_phdc.c$
* $Version : 1.0.0.0$
* $Date    : Jan-18-2010$
*
* Comments:
*
*   This file contains  the USB PHDC class driver implementation
*
*END************************************************************************/


#include "usb_host_phdc.h"
#include "usbprv_host.h"

#include "host_ch9.h"
#include <string.h>
#include "mem_util.h"

/*--------------------------------------------------------------**
** This anchor points to the first class/interface in a linked  **
**   chain of structs, one for each functional phdc interface. **
**   Items are added by calling "select interface" and removed  **
**   by "delete interface".  Typically an application will call **
**   these select in its attach callback routine.  It may later **
**   call delete, or the call may be automatic during detach.   **
**--------------------------------------------------------------*/
USB_PHDC_CLASS_INTF_STRUCT  	*phdc_intf_anchor  = NULL;

/* String to give the metadata signature */
static char const msg_preamble_signature[] = "PhdcQoSSignature";

/* Private functions for use within this file only */
static void    usb_class_phdc_uninit_interface(CLASS_CALL_STRUCT_PTR   phdc_call_ptr);
static void    usb_class_phdc_free_qos_metadata_list(USB_PHDC_CLASS_INTF_STRUCT *phdc_intf);
static boolean usb_class_phdc_pipe_transfers_pending(_usb_pipe_handle pipe_handle);
static boolean usb_class_phdc_interface_transfers_pending(USB_PHDC_CLASS_INTF_STRUCT  *phdc_intf);
static void    usb_class_phdc_send_data_callback(pointer pipe_handle, pointer call_param_ptr, uchar_ptr data_ptr, uint_32 data_size, uint_32 usb_status);
static void    usb_class_phdc_send_control_callback(pointer pipeHandle, pointer callbackParam, uchar_ptr bBuff, uint_32 length, uint_32 usbStatus);
static void    usb_class_phdc_recv_data_callback(_usb_pipe_handle pipe, USB_PHDC_PARAM* callback_param_ptr, uint_8* buff_ptr, uint_32 buff_size, USB_STATUS status);
static USB_PHDC_DESC_QOS_METADATA_LIST*  
               usb_class_phdc_search_endpoint_type(USB_PHDC_DESC_QOS_METADATA_LIST* phdcQosMetaList_ptr, uint_8 ep_type, uint_8 ep_dir);

/* Private macro definitions  */
#define   USB_PHDC_LATECNY_RELIABILITY_MASK   0x3F

/* PUBLIC FUNCTION*--------------------------------------------------------
* 
* Function Name  : usb_class_phdc_init
* Returned Value : none
* Comments       :
*     Initialize phdc-class interface info struct.
* 
*END*--------------------------------------------------------------------*/

void usb_class_phdc_init
   (
      /* [IN] device/descriptor/pipe handles */
      PIPE_BUNDLE_STRUCT_PTR  pbs_ptr,      
      /* [IN] class-interface data pointer + key */
      CLASS_CALL_STRUCT_PTR   phdc_call_ptr
   )
{ /* Body */
   USB_STATUS                  status;
   USB_PHDC_CLASS_INTF_STRUCT  *phdc_intf;
   USB_PHDC_CLASS_TYPE         *phdcClassDesc;

   /* Pointer validity-checking, clear memory, init header */
   phdc_intf = phdc_call_ptr->class_intf_handle;
   if (USB_OK != usb_host_class_intf_init(pbs_ptr, (void*)phdc_intf, (void*)&phdc_intf_anchor))
   {
      return;
   }

   USB_lock();
   /* Generate the code_key based on the attached device. This is used to
    * verify that the device has not been detached and replaced with another.
    */
   phdc_call_ptr->code_key = 0;
   phdc_call_ptr->code_key = usb_host_class_intf_validate (phdc_call_ptr);
   if (phdc_call_ptr->code_key == 0)
   {
	  usb_class_phdc_uninit_interface(phdc_call_ptr);
	  USB_unlock ();
	  return;
   }
   
   /* Initialize the pipes from the pipe bundle passed at function call */	
   /* Control Pipe */
   phdc_intf->controlPipe = usb_hostdev_get_pipe_handle (pbs_ptr, USB_CONTROL_PIPE, 0);
   /* Bulk In Pipe */
   phdc_intf->bulkInPipe = usb_hostdev_get_pipe_handle (pbs_ptr, USB_BULK_PIPE, USB_RECV);
   /* Bulk Out Pipe */
   phdc_intf->bulkOutPipe = usb_hostdev_get_pipe_handle (pbs_ptr, USB_BULK_PIPE, USB_SEND);
   /* Interrupt In Pipe (optional) */
   phdc_intf->intInPipe = usb_hostdev_get_pipe_handle (pbs_ptr, USB_INTERRUPT_PIPE, USB_RECV);
   /* Test for the mandatory pipes */
   if( (phdc_intf->controlPipe == NULL) ||
	   (phdc_intf->bulkInPipe == NULL)  ||
	   (phdc_intf->bulkOutPipe == NULL)
	 )
	 {
	     usb_class_phdc_uninit_interface(phdc_call_ptr);
		 USB_unlock ();
	     return;
	 }
   
   /* Initialize the Function Extension Descriptor */
   status = _usb_hostdev_get_descriptor(phdc_intf->generalClass.dev_handle, 
										phdc_intf->generalClass.intf_handle,
										PHDC_11073PHD_FUNCTION_DESCRIPTOR,
										0,
										0,
										(void**)&(phdc_intf->fcnExtDesc)
									   );
   if(status != USB_OK)
   {
	    usb_class_phdc_uninit_interface(phdc_call_ptr);
		USB_unlock ();
	    return;
   }
   if((phdc_intf->fcnExtDesc == NULL) ||      
      (phdc_intf->fcnExtDesc->bLength != (4 +  2 * phdc_intf->fcnExtDesc->bNumDevSpecs))
	 )
   {
	    usb_class_phdc_uninit_interface(phdc_call_ptr);		  
		USB_unlock ();
	    return;
   }
   
   /* Initialize the Function Extension Descriptor */
   status = _usb_hostdev_get_descriptor(phdc_intf->generalClass.dev_handle, 
										phdc_intf->generalClass.intf_handle,
										USB_PHDC_CLASSFUNCTION_DESCRIPTOR,
										0,
										0,
										(void**)&phdcClassDesc
									   );
   
   if((status != USB_OK) || (phdcClassDesc == NULL))
   {
	    usb_class_phdc_uninit_interface(phdc_call_ptr);
		USB_unlock ();
	    return;
   }
   
   if(phdcClassDesc->bmCapability == 0x01)
   {
    phdc_intf->preambleCapability = TRUE;
   }
   else
   {
    phdc_intf->preambleCapability = FALSE;
   }
   
   phdc_intf->phdcDataCode = phdcClassDesc->bPHDCDataCode;
   
   /* Initialize the QoSMetadata List of descriptors */
   {
    uint_8 endpointCount = 0;
	USB_PHDC_DESC_QOS_METADATA_LIST *qosMetadata, *qosMetadataListTail;
	boolean noMoreEndpoints = FALSE;
	
	do
	{
	  /* Allocate memory for the structure member */	  
	  //status = _usb_hostdev_get_buffer(phdc_intf->generalClass.dev_handle, sizeof(USB_PHDC_DESC_QOS_METADATA_LIST), (void**)&qosMetadata);
	  
	  qosMetadata = (USB_PHDC_DESC_QOS_METADATA_LIST *)USB_mem_alloc_uncached_zero(sizeof(USB_PHDC_DESC_QOS_METADATA_LIST));
	  
	  if(qosMetadata == NULL)
	  {
	    usb_class_phdc_uninit_interface(phdc_call_ptr);		  
		USB_unlock ();
	    return;
	  }
	  
	  /* Initialize the fields in the current list member */
	  qosMetadata->pNext = NULL;
	  status =  _usb_hostdev_get_descriptor(phdc_intf->generalClass.dev_handle, 
				 						    phdc_intf->generalClass.intf_handle,
				 						    USB_ENDPOINT_DESCRIPTOR,
                        (uint_8)(endpointCount+1),  /* Index starts with 1 for endpoint descriptors (Endpoint 0 not includded in the count) */
										    0,
										    (void**)&(qosMetadata->pEndpointDesc)
									       );
									       
      status |= _usb_hostdev_get_descriptor(phdc_intf->generalClass.dev_handle, 
				 						    phdc_intf->generalClass.intf_handle,
				 						    USB_PHDC_QOS_DESCRIPTOR,
                        (uint_8)endpointCount,
										    0,
										    (void**)&qosMetadata->pQosDesc
									       );
									       
	  if((status != USB_OK) || (qosMetadata->pEndpointDesc == NULL) || (qosMetadata->pQosDesc == NULL))	  
	  {
	   
	   /* dealocate the memory for the qosMetadata pointer */
	   USB_mem_free(qosMetadata);
	   
	   /* PHDC is expecting minimum 2 endpoint descriptors (Bulk In and Bulk Out) */
     if(endpointCount < 2)
	   {
	    usb_class_phdc_uninit_interface(phdc_call_ptr);	
		USB_unlock ();
	    return;
	   }
	   
	   /* End the loop */
	   noMoreEndpoints = TRUE;
	  }
      
	  /* First element in the list?	*/
    if(endpointCount == 0)
	  {
	    /* Save the reference in the interface struct */
	    phdc_intf->qosMetadataList = qosMetadata;
	    qosMetadataListTail = phdc_intf->qosMetadataList; /* Initialize the tail, pointing to the only element in the list */
	  }
	  else
	  {
	   if(!noMoreEndpoints)
	   {
	    /* Link it on the last element in the list */
	    qosMetadataListTail->pNext = qosMetadata;
	    /* Increment the tail */
	    qosMetadataListTail = qosMetadataListTail->pNext;
	   }
	  }
	  
	  if(!noMoreEndpoints)
	  {		  
	   /* Check the Qos descriptor fields */
	   if(qosMetadata->pQosDesc != NULL)
	   {
	    if(qosMetadata->pQosDesc->bQoSEncodingVersion != 0x01)
	    {
	     /* The encoding of the descriptor latancy reliability is not supported, Ignore this descriptor */
 		 qosMetadata->pQosDesc = NULL;
	    }
	    else
	    {
	     if(qosMetadata->pEndpointDesc != NULL)
	 	 {
		  switch(qosMetadata->pEndpointDesc->bmAttributes & 0x03)
		  {
		   case 0x02: /* Bulk endpoint */
		  		     if(qosMetadata->pQosDesc->bmLatencyReliability & 0x01)
					 {
					   qosMetadata->pQosDesc = NULL; /* Ignore this descriptor */
					 }
					 else
					 {
					  if(phdc_intf->preambleCapability == FALSE)
					  {
					   /* This interface does not support metadata. Only one of the b1-b7 bits can be set */
					   if(qosMetadata->pQosDesc->bmLatencyReliability & (qosMetadata->pQosDesc->bmLatencyReliability - 1))
					   {
					    qosMetadata->pQosDesc = NULL; /* Ignore this descriptor */
					   }
					  }
					 }
					 break; 
		   case 0x03: /* Interrupt endpoint */
				     if(qosMetadata->pQosDesc->bmLatencyReliability != 0x01) /* for this endpoint only the b0 should be set in the QoS bin.*/
					 {
					  qosMetadata->pQosDesc = NULL; /* Ignore this descriptor */
					 }
					 break;
		   default:  /* The endpoint is not supported */
					 qosMetadata->pQosDesc = NULL; /* Ignore this descriptor */	  
					 break;		  
		 }
		}
	   }
	  }
	  
  
	   /* Initialize the optional metadata descriptor */
	   status = _usb_hostdev_get_descriptor(phdc_intf->generalClass.dev_handle, 
	  			 						   phdc_intf->generalClass.intf_handle,
				 						   PHDC_METADATA_DESCRIPTOR,
                                           endpointCount,
										   0,
										   (void**)&(qosMetadata->pMetadataDesc)
									      );
       endpointCount++;
	 } /* if(!noMoreEndpoints) */
     									      	  
    }while(!noMoreEndpoints);
   
   }/* End block: Initialize the QoSMetadata List of descriptors */

   USB_unlock();
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_phdc_send_data
* Returned Value : error code
* Comments       :
*     Send data to a phdc device through a bulk-out pipe.
* 
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_phdc_send_data
   (
      /* [IN] phdc specific generic parameter structure pointer */
      USB_PHDC_PARAM* call_param_ptr
   )
{ /* Body */
  TR_INIT_PARAM_STRUCT             tr;
  USB_STATUS                       error;
  USB_PHDC_CLASS_INTF_STRUCT*      class_interface_handle;
  USB_PHDC_DESC_QOS_METADATA_LIST* qosListParser = NULL;
  uint_8                           numTransfers;
  uint_8                           latencyReliability;

  if(NULL == call_param_ptr)
  {
    error = USBERR_ERROR;
    return error;
  }

  if( (NULL == call_param_ptr->ccs_ptr) || (NULL == call_param_ptr->buff_ptr) )
  {
    error = USBERR_ERROR;
    return error;
  }

  if( 0 == call_param_ptr->buff_size )
  {
    error = USBERR_ERROR;
    return error;
  }

  USB_lock ();

  if( 0 == usb_host_class_intf_validate(call_param_ptr->ccs_ptr) )
  {
    USB_unlock ();
    error = USBERR_NO_INTERFACE;
    return error;
  }

  class_interface_handle = (USB_PHDC_CLASS_INTF_STRUCT *)call_param_ptr->ccs_ptr->class_intf_handle;
  error = usb_hostdev_validate (class_interface_handle->generalClass.dev_handle);
  if(USB_OK != error)
  {
    USB_unlock ();
    return error;
  }

  /* check for pending transfers */
  if(TRUE == class_interface_handle->setClearRequestPending)
  {
    USB_unlock ();
    error = USBERR_TRANSFER_IN_PROGRESS;
    return error;
  }

  /* check for meta-data capability */
  if(class_interface_handle->preambleCapability && class_interface_handle->deviceFeatureSet)
  {
    /* meta-data and QOS checking */
    if(TRUE == call_param_ptr->metadata)
    {
      latencyReliability = ((USB_PHDC_METADATA_PREAMBLE *)call_param_ptr->buff_ptr)->bmLatencyReliability;
      if( ((latencyReliability & latencyReliability) - 1) || (0 == latencyReliability) || (latencyReliability & 0xC0) )
      {
        USB_unlock ();
        error = USBERR_INVALID_BMREQ_TYPE;
        return error;
      }
      
      /* Check if 'qos' bin is supported by the Bulk OUT endpoint's PHDC QoS Descriptor */
      /* Search in the USB_PHDC_DESC_QOS_METADATA_LIST the endpoint number corresponding to the Bulk OUT */
      qosListParser = usb_class_phdc_search_endpoint_type(class_interface_handle -> qosMetadataList, ENDPOINT_BULK, ENDPOINT_DIR_OUT);
         
      /* Compare the qos bin with 'bmLatencyReliability' field from the corresponding QoS Descriptor */
      if(qosListParser == NULL)
      {
        USB_unlock ();
        error = USBERR_ERROR;
        return error;
      }

      /* Check if the requested QoS is supported */
      if(0 == (qosListParser -> pQosDesc -> bmLatencyReliability & latencyReliability))
      {
        USB_unlock ();
        error = USBERR_ERROR;
        return error;
      }

      numTransfers = ((USB_PHDC_METADATA_PREAMBLE *)call_param_ptr->buff_ptr)->bNumTransfers;
      if( 0 == numTransfers )
      {
        USB_unlock ();
        error = USBERR_ERROR;
        return error;
      }
      class_interface_handle->numTransfBulkOut = numTransfers;
    }
    else /* if not a meta-data send request */
    {
      if( 0 == class_interface_handle->numTransfBulkOut )
      {
        USB_unlock ();
        error = USBERR_ERROR;
        return error;
      }
    }
  }

  /* prepare the transfer */
  usb_hostdev_tr_init (&tr, usb_class_phdc_send_data_callback, call_param_ptr);
  tr.TX_BUFFER = call_param_ptr->buff_ptr;
  tr.TX_LENGTH = call_param_ptr->buff_size;
  call_param_ptr->tr_index = tr.TR_INDEX;
  call_param_ptr->tr_pipe_handle = class_interface_handle->bulkOutPipe;

  error = _usb_host_send_data(
                                class_interface_handle->generalClass.host_handle,
                                class_interface_handle->bulkOutPipe,
                                &tr
                              );

  /* decrement the numTransfers for bulkOut pipe */
  if((class_interface_handle->numTransfBulkOut != 0) && (FALSE == call_param_ptr->metadata))
  {
    class_interface_handle->numTransfBulkOut--;
  }

  USB_unlock ();

  return error;   

} /* Endbody */

/* PUBLIC FUNCTION*--------------------------------------------------------
* 
* Function Name  : usb_class_phdc_set_callbacks
* Returned Value : none
* Comments       :
*     Initializes the application callbacks for the current PHDC interface
* 
*END*--------------------------------------------------------------------*/
USB_STATUS usb_class_phdc_set_callbacks
(
    /* [IN] phdc call struct pointer */
    CLASS_CALL_STRUCT_PTR       ccs_ptr,
	/* [IN] phdc application-registered send Callback */
	phdc_callback  sendCallback,
	/* [IN] phdc application-registered receive Callback */
	phdc_callback  recvCallback,
	/* [IN] phdc application-registered control Callback */
	phdc_callback  ctrlCallback
)
{
   USB_STATUS returnStatus = USBERR_NO_INTERFACE;
   
   USB_lock();
   
   if(usb_host_class_intf_validate(ccs_ptr))
   {
     /* Ok, the interface is valid */
     USB_PHDC_CLASS_INTF_STRUCT  *phdc_intf = (USB_PHDC_CLASS_INTF_STRUCT  *)ccs_ptr->class_intf_handle;
     
     if(phdc_intf != NULL)
     {    	
       /* Check the pending transfers on the pipes */    
       returnStatus = USBERR_TRANSFER_IN_PROGRESS;     

       if(!usb_class_phdc_interface_transfers_pending(phdc_intf))
       {
           /* No transfers are pending in the interface. Set the callbacks */
           phdc_intf->sendCallback = sendCallback;
           phdc_intf->recvCallback = recvCallback;
           phdc_intf->ctrlCallback = ctrlCallback;           
           
           returnStatus = USB_OK;
       }
     }
   }
   
   USB_unlock();
   
   return returnStatus;   
}


/* PUBLIC FUNCTION*--------------------------------------------------------
* 
* Function Name  : usb_class_phdc_uninit
* Returned Value : none
* Comments       :
*     Uninitialize phdc-class interface info struct.
* 
*END*--------------------------------------------------------------------*/
void  usb_class_phdc_uninit(
     /* [IN] pointer to device instance */
    _usb_device_instance_handle dev_handle,
    /* [IN] pointer to interface descriptor */
    _usb_interface_descriptor_handle intf_handle
   )
{
 USB_PHDC_CLASS_INTF_STRUCT *phdcParser = phdc_intf_anchor;
 boolean interfaceFound = FALSE;
 
 USB_lock();
 /* Search the interface descriptor handle (intf_handle) and device descriptor handle (intf_handle) in the interface list */
 if(phdc_intf_anchor != NULL)
 {
  do
  {
   if( (phdcParser->generalClass.dev_handle == dev_handle) &&
       (phdcParser->generalClass.intf_handle == intf_handle)
     )
   {
    interfaceFound = TRUE;
   }
   else
   {
    /* Go to the next element in the list */
    phdcParser = (USB_PHDC_CLASS_INTF_STRUCT *)phdcParser->generalClass.next;
   }
  }
  while((interfaceFound == FALSE) && (phdcParser != NULL));
 }
 
 if(interfaceFound)
 {
   if(phdcParser->bulkInPipe)
   {   	
    _usb_host_close_pipe(phdcParser->generalClass.host_handle, phdcParser->bulkInPipe);
   }
   if(phdcParser->bulkOutPipe)
   {   	
    _usb_host_close_pipe(phdcParser->generalClass.host_handle, phdcParser->bulkOutPipe);
   }
   if(phdcParser->intInPipe)
   {   	
    _usb_host_close_pipe(phdcParser->generalClass.host_handle, phdcParser->intInPipe);
   }
   
   usb_class_phdc_free_qos_metadata_list(phdcParser);
 }
 
 USB_unlock();
}


/* PUBLIC FUNCTION*--------------------------------------------------------
* 
* Function Name  : usb_class_phdc_cancel_transfer
* Returned Value : none
* Comments       :
*     Attempts to cancel the indicated transfer (Send or Receive)
* 
*END*--------------------------------------------------------------------*/
USB_STATUS usb_class_phdc_cancel_transfer(CLASS_CALL_STRUCT_PTR ccs_ptr,
                                          uint_32               tr_index,
                                          _usb_pipe_handle      tr_pipe_handle
                                         )
{
   USB_STATUS returnStatus = USBERR_NO_INTERFACE;
   
   USB_lock();
   
   if(usb_host_class_intf_validate(ccs_ptr))
   {
     /* Ok, the interface is valid */
     USB_PHDC_CLASS_INTF_STRUCT  *phdc_intf = (USB_PHDC_CLASS_INTF_STRUCT *)ccs_ptr->class_intf_handle;
     
     if(phdc_intf != NULL)
     {
      returnStatus = _usb_host_cancel_transfer(phdc_intf->generalClass.host_handle, tr_pipe_handle, tr_index);
     }
   }

   USB_unlock();
   return returnStatus;     
}



/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_class_phdc_recv_data
* Returned Value : none
* Comments       :
*     Receive printer data from either a bulk-in or an interrupt pipe.
* 
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_phdc_recv_data(USB_PHDC_PARAM* call_param_ptr) 
{ /* Body */

  _usb_pipe_handle             pipe;
  TR_INIT_PARAM_STRUCT         tr;
  USB_STATUS                   usb_status			= USBERR_NO_INTERFACE; 
  USB_PHDC_CLASS_INTF_STRUCT*  phdc_cis_ptr 	= (USB_PHDC_CLASS_INTF_STRUCT*)(call_param_ptr ->ccs_ptr ->class_intf_handle);
  DEV_INSTANCE_PTR             dev 				= (DEV_INSTANCE_PTR)(phdc_cis_ptr ->generalClass.dev_handle);	
  
  /* used to search a specific QoS descriptor in the linked list of Endpoint-QoS-Metadata descriptors */
  USB_PHDC_DESC_QOS_METADATA_LIST* phdcQosMetadataListParser = NULL;
  
   
  /* Function's input parameters checkings */
  if((NULL == call_param_ptr) || (NULL == call_param_ptr ->ccs_ptr) || (NULL == call_param_ptr ->buff_ptr) || !(call_param_ptr ->buff_size)) 
  {
    usb_status = USBERR_ERROR;
      
    return usb_status;
  }
      
  /* Checks if QoS 'points' to the interrupt pipe and this pipe exists OR 'qos' contains valid value */
  if(call_param_ptr ->qos & 0x01) 
  {
     if((call_param_ptr ->qos & 0xFE) || (NULL == phdc_cis_ptr ->intInPipe))
     {
       usb_status = USBERR_ERROR; 
        
       return usb_status; 
     }      
  }
    
  /* Performs a class validity check in order to detect if device is still attached or it wasn't replaced by another */   
   
  USB_lock ();
  if(usb_host_class_intf_validate(call_param_ptr ->ccs_ptr)) 
  {           
     usb_status = usb_hostdev_validate (dev);
     
     if(USB_OK != usb_status) 
     {       
       USB_unlock ();        
       return usb_status;
     } 
     else
     {
     }
  }
      
               
  /* Get the pipe handle used for receive data (parameter passed in _usb_host_recv_data()), depending on the qos bin */ 
  pipe = (call_param_ptr ->qos & 0x01) ? (phdc_cis_ptr ->intInPipe) : (phdc_cis_ptr ->bulkInPipe);
   
  /* Checks if pipe selection is Bulk IN */
  if(pipe == phdc_cis_ptr ->bulkInPipe) 
  {
    /* Check if 'qos' bin is supported by the Bulk IN endpoint's PHDC QoS Descriptor */
    /* Search in the USB_PHDC_DESC_QOS_METADATA_LIST the endpoint number corresponding to the Bulk IN */
    phdcQosMetadataListParser = usb_class_phdc_search_endpoint_type(phdc_cis_ptr ->qosMetadataList, ENDPOINT_BULK, ENDPOINT_DIR_IN);
         
    /* Compare the qos bin with 'bmLatencyReliability' field from the corresponding QoS Descriptor */
    if((phdcQosMetadataListParser == NULL) || (!(call_param_ptr ->qos & phdcQosMetadataListParser ->pQosDesc ->bmLatencyReliability)))
    {
      usb_status = USBERR_ERROR;
      
      USB_unlock ();        
      return usb_status;    
    }    
     
    /* Checks if any SET_FEATURE/CLEAR_FEATURE class command is pending; if it is, refuses the current transaction */
    if(TRUE == phdc_cis_ptr ->setClearRequestPending) 
    {
      usb_status = USBERR_TRANSFER_IN_PROGRESS;
       
      USB_unlock ();         
      return usb_status;  
    }   
  }
    
  /* Populate the transfer structure */   
  usb_hostdev_tr_init (&tr, (tr_callback)usb_class_phdc_recv_data_callback, call_param_ptr); 
   
  tr.RX_BUFFER       = call_param_ptr ->buff_ptr;
  tr.RX_LENGTH       = call_param_ptr ->buff_size;   
  
  /* Save the transfer index and pipe handle */
  call_param_ptr ->tr_index = tr.TR_INDEX;
  call_param_ptr ->tr_pipe_handle = pipe;     
     
  /* Require Host API to post transaction */
  usb_status = _usb_host_recv_data(phdc_cis_ptr ->generalClass.host_handle, pipe, &tr); 

  USB_unlock ();
   
  return usb_status;               
} /* Endbody */                  



/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : phdc_receive_data_callback
* Returned Value : none
* Comments       :
*     Internal PHDC callback function when a posted receive transfer has been finished.
* 
*END*--------------------------------------------------------------------*/
static void  usb_class_phdc_recv_data_callback
   (
      /* [IN] pipe handle */
      _usb_pipe_handle          pipe,
      
      /* [IN] parameter specified by higher level */ 
      USB_PHDC_PARAM*  callback_param_ptr, 
      
      /* [IN] pointer to buffer containing data (Rx) */
      uint_8*                  buff_ptr, 
      
      /* [IN] length of data transferred */
      uint_32                   buff_size, 
      
      /* [IN] status, preferably USB_OK or USB_DONE */
      USB_STATUS                usb_status
   ) 
{ /* Body */
  
  USB_PHDC_CLASS_INTF_STRUCT* 	  	phdc_cis_ptr 		          = (USB_PHDC_CLASS_INTF_STRUCT*)(callback_param_ptr ->ccs_ptr ->class_intf_handle); 
  DEV_INSTANCE_PTR             		  dev 				              = (DEV_INSTANCE_PTR)(phdc_cis_ptr ->generalClass.dev_handle);
  PIPE_DESCRIPTOR_STRUCT_PTR        pipe_desc_ptr	  	        = (PIPE_DESCRIPTOR_STRUCT_PTR)(phdc_cis_ptr ->bulkInPipe); 
 
  USB_PHDC_METADATA_PREAMBLE*   		 pMetaDataPreamble 	      = NULL;  
  USB_PHDC_DESC_QOS_METADATA_LIST*  phdcQosMetadataListParser = NULL;
  uint_8 						                metadata_packet;  
  USB_STATUS                        status;
    
    
  USB_lock ();
 
  if(usb_status != USB_STATUS_IDLE)
  {
    /* ERROR: error returned USB status  */
    callback_param_ptr ->usb_phdc_status = USB_PHDC_ERR;	              
  }
  else
  {
    if(usb_host_class_intf_validate(callback_param_ptr ->ccs_ptr)) 
    { 
   	  /* Check the type of pipe */
  	  if(pipe == phdc_cis_ptr ->intInPipe)
  	  { /* Pipe is Interrupt IN */      
  	      
  	    /* Packet received is assumed to be data (no metadata preamble check) */
  	    callback_param_ptr ->usb_phdc_status = USB_PHDC_RX_OK;  
  	  }
  	  else
  	  { /* Pipe is Bulk IN */
  	      
  	    /* Checks for the preamble capability feature and activation */
  	    if((phdc_cis_ptr ->preambleCapability == TRUE) && (phdc_cis_ptr ->deviceFeatureSet == TRUE))
  	    { /* Preamble feature supported and also enabled */
  	        
  	      /* Checks if packet expected is metadata preamble or not */
  	      if(phdc_cis_ptr ->numTransfBulkIn)
  	      { /* Packet expected: message data */
  	        
  	        /* Packet received is assumed to be data (no metadata preamble check) */
  	        callback_param_ptr ->usb_phdc_status = USB_PHDC_RX_OK;
  	        phdc_cis_ptr ->numTransfBulkIn--;   /* Decrements the number of data transfer left until a new metadata is expected */   
  	      }  
  	      else
  	      { /* Packet expected: metadata */
  	          
  	        /* 
  	          Compare the first 16 bytes of the received data with the preamble signature 
  	          in order to decide if metadata preamble has been received; if metadata_pachet = 0 
  	          the incoming packet is a metadata.                    
  	        */  
  	        metadata_packet = (uint_8) strncmp((const char *)buff_ptr, msg_preamble_signature, METADATA_PREAMBLE_SIGNATURE_SIZE);         
  	               
  	        /* Checks the incoming packet */
  	        if(!metadata_packet)
  	        { /* Metadata preamble received */
  	       
  	          /* Metadata expected and also received */
  	          /* Parse the buffer as it would be a metadata preamble */ 
  	          /* Checks for correctness of the rest of fields in preamble metadata (bNumTransfers, bQosEncodingVersion and bmLatencyReliability) */
  	          pMetaDataPreamble = (USB_PHDC_METADATA_PREAMBLE*)buff_ptr;
  	            
  	          if(
  	             !(pMetaDataPreamble ->bNumTransfers)               ||   /* bNumTrensfer should not be 0 */
  	              (pMetaDataPreamble ->bQoSEncodingVersion != 0x01) ||   /* if 'bQoSEncodingVersion' isn't 0x01 it would be considered that 'bmLatencyReliability' is not valid */
  	              (pMetaDataPreamble ->bmLatencyReliability & 0x01) ||   /* 'bmLatencyReliability' bitmap shouldn't contains a '1' in the bit 0 position */
  	             !(pMetaDataPreamble ->bmLatencyReliability)        ||   /* 'bmLatencyReliability' should not be 0 */                             
  	              ((pMetaDataPreamble ->bmLatencyReliability & pMetaDataPreamble ->bmLatencyReliability) - 1)  /* 'bmLatencyReliability' bitmap shall contains only one bit set */
  	            )
  	          {
  	            /* ERROR: metadata preamble expected and received BUT invalid 'bNumTransfer' or 'bmLatencyReliability' values */
  	            callback_param_ptr ->usb_phdc_status = USB_PHDC_RX_ERR_METADATA_EXPECTED;
  	             
  	            /* Performs a SetFeature(Endpoint_Halt) command  followed by a ClearFeature(Endpoint_Halt) */
  	            /* Prototype:  USB_STATUS  _usb_host_ch9_set/clear_feature(dev_handle, req_type, intf_endpt,feature) where: */
  	            /* REQ_TYPE_ENDPOINT = 0x02, ENDPOINT_HALT = 0x01 defined in "host_ch9.h" */               
  	            status = _usb_host_ch9_set_feature(dev, REQ_TYPE_ENDPOINT, (uint_8)(pipe_desc_ptr ->ENDPOINT_NUMBER | pipe_desc_ptr ->DIRECTION), ENDPOINT_HALT);
  	          
  	            if((USB_STATUS_TRANSFER_QUEUED != status) && (USB_OK != status))
  	            {
  	              callback_param_ptr ->usb_phdc_status = USB_PHDC_ERR_ENDP_CLEAR_STALL;  
  	            }
  	            else
  	            {	          	
  	              status = _usb_host_ch9_clear_feature(dev, REQ_TYPE_ENDPOINT, (uint_8)(pipe_desc_ptr ->ENDPOINT_NUMBER | pipe_desc_ptr ->DIRECTION), ENDPOINT_HALT);	          	
  	          	  if((USB_STATUS_TRANSFER_QUEUED != status) && (USB_OK != status)) 
  	              {
  	            	  callback_param_ptr ->usb_phdc_status = USB_PHDC_ERR_ENDP_CLEAR_STALL; 
  	              }
  	              else
  	              {
  	            	  /* Reset the 'numTransfBulkIn' parameter in order to expect a new metadata preamble */
  	             	  phdc_cis_ptr ->numTransfBulkIn = 0;              
  	              }
  	            } /* End checking the status of SetFeature/Clear Feature(Endpoint_Halt) operations */                
  	          }
  	          else
  	          { /* Metadata preamble packet is correct */
  	          
  	            /* Compare the 'bmLatencyReliability' of the incoming metadata preamble with the QoS Descriptor attached to device's Bulk IN endpoint */
  	            /* Search in the USB_PHDC_DESC_QOS_METADATA_LIST the endpoint number corresponding to the Bulk IN */
                phdcQosMetadataListParser = usb_class_phdc_search_endpoint_type(phdc_cis_ptr ->qosMetadataList, ENDPOINT_BULK, ENDPOINT_DIR_IN);
  	          	                                                     
  	            if(phdcQosMetadataListParser != NULL)
  	            { /* Descriptor was found */
  	                
  	              /* Compare the qos bin with 'bmLatencyReliability' field from the corresponding QoS Descriptor */
  	              if(!(pMetaDataPreamble ->bmLatencyReliability & phdcQosMetadataListParser ->pQosDesc ->bmLatencyReliability))
  	              {
  	                /* ERROR: Metadata preamble's 'bmLatencyReliability' is not supported by the Bulk IN PHDC QoS Descriptor */
  	                callback_param_ptr ->usb_phdc_status = USB_PHDC_ERR;   
  	              }
  	              else
  	              {
  	                /* Metadata preamble received correctly */
  	                callback_param_ptr ->usb_phdc_status = USB_PHDC_RX_OK; 
  	                 
  	                /* Reloads the 'numTransfBulkIn' parameter with the value stored in received metadata preamble */
  	                phdc_cis_ptr ->numTransfBulkIn = pMetaDataPreamble ->bNumTransfers;              
  	              }
  	            }
  	            else
  	            { /* Descriptor not found */                              
  	              callback_param_ptr ->usb_phdc_status = USB_PHDC_ERR;                
  	            }                    
  	          }            
  	        }
  	        else
  	        { /* Message data received */
  	          
  	          /* ERROR: metadata preamble expected but message data received */
  	          callback_param_ptr ->usb_phdc_status = USB_PHDC_RX_ERR_METADATA_EXPECTED;
  	                       
  	          /* Performs a  SetFeature(Endpoint_Halt) command  followed by a ClearFeature(Endpoint_Halt) */
  	          status = _usb_host_ch9_set_feature(dev, REQ_TYPE_ENDPOINT, (uint_8)(pipe_desc_ptr ->ENDPOINT_NUMBER | pipe_desc_ptr ->DIRECTION), ENDPOINT_HALT);
  	          if((USB_STATUS_TRANSFER_QUEUED != status) && (USB_OK != status))
  	          {
  	            callback_param_ptr ->usb_phdc_status = USB_PHDC_ERR_ENDP_CLEAR_STALL;  
  	          }
  	          else
  	          {
  	            status = _usb_host_ch9_clear_feature(dev, REQ_TYPE_ENDPOINT, (uint_8)(pipe_desc_ptr ->ENDPOINT_NUMBER | pipe_desc_ptr ->DIRECTION), ENDPOINT_HALT);
  	            if((USB_STATUS_TRANSFER_QUEUED != status) && (USB_OK != status))
  	            {
  	              callback_param_ptr ->usb_phdc_status = USB_PHDC_ERR_ENDP_CLEAR_STALL; 
  	            }
  	            else
  	            {
  	       	 	    /* Reset the 'numTransfBulkIn' parameter in order to expect a new metadata preamble */
  	          	  phdc_cis_ptr ->numTransfBulkIn = 0;              
  	            }
  	          } /* End checking the status of SetFeature/Clear Feature(Endpoint_Halt) operations */                          
  	        } /* End checking the incoming packet */
  	      } /* End checking the expectation of metadata */
  	    }
  	    else
  	    {
  	      if((phdc_cis_ptr ->preambleCapability == FALSE) && (phdc_cis_ptr ->deviceFeatureSet == FALSE))
  	      {  /* Neither preamble capability supported nor enabled */
  	         
  	        /* Packet received is assumed to be data (no metadata preamble check) */
  	        callback_param_ptr ->usb_phdc_status = USB_PHDC_RX_OK;
  	      }
  	    } /* End checking the preamble capability */   
  	  } /* End checking the type of pipe */ 
    } /* End checking the class validation */
  } /* End checking the returned USB status */
	 
	/* Populates the 'usb_status and buff_size' in the USB_PHDC_PARAM */
	callback_param_ptr ->usb_status 	 = usb_status;
	callback_param_ptr ->buff_size     = buff_size;
	

	/* Call the application callback function unless the callback pointer is NULL */
	if(NULL != phdc_cis_ptr ->recvCallback) 
	{
	  phdc_cis_ptr ->recvCallback(callback_param_ptr);
	}
 
 
  USB_unlock (); 
} /* EndBody */



/* PUBLIC FUNCTION*--------------------------------------------------------
* 
* Function Name  : usb_class_phdc_search_endpoint_type
* Returned Value : USB_STATUS
* Comments: This function search in the USB_PHDC_DESC_QOS_METADATA_LIST the endpoint
*           corresponding to the type given as argument 
* 
*END*--------------------------------------------------------------------*/
static USB_PHDC_DESC_QOS_METADATA_LIST*  usb_class_phdc_search_endpoint_type
  (
    USB_PHDC_DESC_QOS_METADATA_LIST* phdcQosMetaList_ptr, 
    uint_8 ep_type, 
    uint_8 ep_dir
  )
{ /* Body */
	          
  /* used to search a specific QoS descriptor in the linked list of Endpoint-QoS-Metadata descriptors */
  USB_PHDC_DESC_QOS_METADATA_LIST* 	phdcQosMetadataListParser = phdcQosMetaList_ptr;
            
  while(phdcQosMetadataListParser -> pNext != NULL) 
  {
    /* Check if the endpoint is Bulk IN */
    if(
       (((uint_8)(phdcQosMetadataListParser ->pEndpointDesc ->bmAttributes) & ENDPOINT_TRANSFER_TYPE) == ep_type) &&        /* Endpoint type */
       (((uint_8)(phdcQosMetadataListParser ->pEndpointDesc ->bEndpointAddress) & ENDPOINT_DIRECTION) == ep_dir)            /* Endpoint direction */
      )
    {
  		return phdcQosMetadataListParser; 
    }

    /* Points to the next element in the list */
    phdcQosMetadataListParser = phdcQosMetadataListParser -> pNext;             	                 
  }	
  
  return NULL;
} /* Endbody */


/* PUBLIC FUNCTION*--------------------------------------------------------
* 
* Function Name  : usb_class_phdc_send_control_request
* Returned Value : USB_STATUS
* Comments: This function implements class specific request: PHDC_GET_STATUS_BREQ, PHDC_SET_FEATURE_BREQ and      :
* PHDC_CLEAR_FEATURE_BREQ    
* 
*END*--------------------------------------------------------------------*/
USB_STATUS usb_class_phdc_send_control_request(USB_PHDC_PARAM* call_param_ptr)
{ /* Body */
  USB_STATUS                       status = USBERR_NO_INTERFACE;
  USB_SETUP                        request;
  USB_PHDC_CLASS_INTF_STRUCT*      intf_ptr;
  uint_16                          targetInterface;                        
  uint_8                           reqType; 

  /* parameter checking*/
  if(call_param_ptr == NULL)
  {
    return USBERR_ERROR;
  }
     /*make sure that no USB event occurs until the function ends*/
  USB_lock();
  intf_ptr = (USB_PHDC_CLASS_INTF_STRUCT*)call_param_ptr->ccs_ptr->class_intf_handle;
  /* make sure that the interface is a valid one*/
  if (!usb_host_class_intf_validate(call_param_ptr->ccs_ptr)) 
  {
    USB_unlock();
    return status;
  } /* Endif */
  reqType =  call_param_ptr->classRequestType;
  /* read interface number from the interface descriptor*/
  targetInterface = ((INTERFACE_DESCRIPTOR_PTR)intf_ptr->generalClass.intf_handle)->bInterfaceNumber;
  switch(reqType)
  {
    case PHDC_GET_STATUS_BREQ:
      /* status bytes are to be received from the device, so, make sure that buff_ptr isn't NULL */
     	if(call_param_ptr->buff_ptr == NULL)
     	{
     	  USB_unlock();
     	  return USBERR_ERROR;
     	}
     	/* populate the USB_SETUP structure for the status request */
     	request.BMREQUESTTYPE = PHDC_GET_STATUS_BMREQTYPE;   	
     	request.BREQUEST = PHDC_GET_STATUS_BREQ;
     	htou16(request.WVALUE, 0); 
     	htou16(request.WINDEX,targetInterface);  
     	htou16(request.WLENGTH, 2);
      break;

    case PHDC_SET_FEATURE_BREQ:
    case PHDC_CLEAR_FEATURE_BREQ:
      /* make sure that the device supports metadata message preambul feature */
     	if(intf_ptr->preambleCapability == 0)
     	{
     	  USB_unlock();
     	  return USBERR_INVALID_REQ_TYPE;
     	}
     	/* make sure that no other set/clear feature request is in progress*/
     	if(intf_ptr->setClearRequestPending) 
      {
        USB_unlock();
     	  return USBERR_TRANSFER_IN_PROGRESS;
      }
      /* To avoid synchronization issues with preamble */ 
     	/* make sure that no send or receive transactions are pending  */
     	if(usb_class_phdc_interface_transfers_pending(intf_ptr))
     	{
     	  USB_unlock();
     	  return USBERR_TRANSFER_IN_PROGRESS;	
     	}
     	/* populate the USB_SETUP structure for the set/clear feature request */
     	if(reqType == PHDC_SET_FEATURE_BREQ)
     	{
     	  request.BMREQUESTTYPE = PHDC_SET_FEATURE_BMREQTYPE;
     	  request.BREQUEST = PHDC_SET_FEATURE_BREQ;	
     	}
     	else
     	{
     	  request.BMREQUESTTYPE = PHDC_CLEAR_FEATURE_BMREQTYPE;
     	  request.BREQUEST = PHDC_CLEAR_FEATURE_BREQ;	
     	}

     	request.WVALUE[0] = PHDC_FEATURE_METADATA;
     	request.WVALUE[1] = PHDC_QoS_ENCODING_VERSION;
     	htou16(request.WINDEX,targetInterface);
     	htou16(request.WLENGTH, 0);
     	/* set the setClearRequestPending flag to indicate that  */
     	/* a set/clear feature request is in progress.This flag  */
     	/* will be cleared in the phdc send control request callback */
     	/* or later in this function if _usb_hostdev_cntrl_request returns error*/
     	intf_ptr->setClearRequestPending = TRUE;
     	break;

    default:
      USB_unlock(); 
     	return USBERR_INVALID_REQ_TYPE;   
  }
     /* call _usb_hostdev_cntrl_request function to send the control request over the USB */
  status = _usb_hostdev_cntrl_request (intf_ptr->generalClass.dev_handle,
        &request, call_param_ptr->buff_ptr, usb_class_phdc_send_control_callback, call_param_ptr);
        
  if((status != USB_STATUS_TRANSFER_QUEUED) && (status != USB_OK))
  {
    if((reqType == PHDC_SET_FEATURE_BREQ) || (reqType == PHDC_CLEAR_FEATURE_BREQ))
    {
      intf_ptr->setClearRequestPending = FALSE;	
    }
  }
  USB_unlock();

  return status;
} /* Endbody */

/* PRIVATE FUNCTIONS *----------------------------------------------------*/

/* usb_class_phdc_send_data_callback
 * this function is called by USB host API
 * Parameters: pointer   pipe_handle
               pointer   call_param_ptr
               uchar_ptr data_ptr
               uint_32   data_size
               uint_32   usb_status
 * Returns: void
 */
static void usb_class_phdc_send_data_callback
                  (
                      pointer   pipe_handle,
                      pointer   call_param_ptr,
                      uchar_ptr data_ptr,
                      uint_32   data_size,
                      uint_32   usb_status
                  )
{
  USB_PHDC_CLASS_INTF_STRUCT* class_interface_handle;
  USB_PHDC_PARAM*             param_ptr;
  USB_STATUS                  status;
  
  UNUSED(data_ptr)
  UNUSED(data_size)

  /* exit if input pointer is NULL */
  if(NULL == call_param_ptr)
  {
    return;
  }
  param_ptr = call_param_ptr;

  /* make sure that no USB event occurs until the function end */
  USB_lock ();

  /* validate the interface */
  if( usb_host_class_intf_validate(param_ptr->ccs_ptr) )
  { /* populate the USB_PHDC_PARAM structure using the data provided by the host API */
    param_ptr->usb_status  = usb_status;
    class_interface_handle = (USB_PHDC_CLASS_INTF_STRUCT *)param_ptr->ccs_ptr->class_intf_handle;

    /* verify the USB error code and generate the PHDC specific error code */
    if(USB_OK == usb_status)
    {
      param_ptr->usb_phdc_status = USB_PHDC_TX_OK;
    }
    else
    {
      param_ptr->usb_phdc_status = USB_PHDC_ERR;
      /* if deivce's endpoint is stalled , try to clear this status */
      if(USBERR_ENDPOINT_STALLED == usb_status)
      { /* if the 'clear feature' operation fails, exit with error code */
        status = _usb_host_ch9_clear_feature
                 ( class_interface_handle->generalClass.dev_handle,
                   REQ_TYPE_ENDPOINT,
                   (uint_8)(((PIPE_DESCRIPTOR_STRUCT_PTR)pipe_handle)->ENDPOINT_NUMBER | REQ_TYPE_OUT),
                   ENDPOINT_HALT );
        if( (status != USB_OK) && (status != USB_STATUS_TRANSFER_QUEUED))
        {
          param_ptr->usb_phdc_status = USB_PHDC_ERR_ENDP_CLEAR_STALL;
        }
      }
    }

    /* verify the callback pointer and if != NULL, launch the application's callback function */
    if(NULL != class_interface_handle->sendCallback)
    {
      class_interface_handle->sendCallback(param_ptr);
    }
  }
  USB_unlock();
}

/*------------------------------------------------------------------------*/

/* usb_class_phdc_interface_transfers_pending
 * this function checks if there are active transfers at the PHDC interface level
 * it checks all the pipes for transfers using the usb_class_phdc_pipe_transfers_pending function
 * WARNING: This function presumes that USB interrupts are disabled
 * Returns: TRUE  = the provided interface has active transfers
 *          FALSE = the provided interface does not have active transfers
 */
static boolean usb_class_phdc_interface_transfers_pending(USB_PHDC_CLASS_INTF_STRUCT  *phdc_intf)
{
 boolean returnValue = FALSE;
  
 if(phdc_intf != NULL)
 {
    returnValue = usb_class_phdc_pipe_transfers_pending(phdc_intf->controlPipe);
    if(returnValue == FALSE)
    {
      returnValue = usb_class_phdc_pipe_transfers_pending(phdc_intf->bulkInPipe);
      if(returnValue == FALSE)
      {
        returnValue = usb_class_phdc_pipe_transfers_pending(phdc_intf->bulkOutPipe);
        if(returnValue == FALSE)
        {
          returnValue = usb_class_phdc_pipe_transfers_pending(phdc_intf->intInPipe);
        }
      }
    }
 }

 return returnValue;
}
/*------------------------------------------------------------------------*/

/* usb_class_phdc_pipe_transfers_pending
 * this function checks if there are active transfers on a pipe
 * WARNING: This function presumes that USB interrupts are disabled
 * Returns: TRUE = the provided pipe has active transfers
 *          FALSE = the provided pipe does not have active transfers
 */
static boolean usb_class_phdc_pipe_transfers_pending(_usb_pipe_handle pipe_handle)
{
 PIPE_DESCRIPTOR_STRUCT    *pipe_descriptor_ptr = (PIPE_DESCRIPTOR_STRUCT *)pipe_handle;
 boolean returnValue = FALSE;
 

 if(pipe_descriptor_ptr != NULL)
 {
  /* Search the pipe active transfer list */
  PIPE_TR_STRUCT  *tr_ptr = pipe_descriptor_ptr->tr_list_ptr;

  while((tr_ptr != NULL) && (tr_ptr->TR_INDEX == 0))
  {
   if(pipe_descriptor_ptr->tr_list_ptr != tr_ptr->next)
   {
    tr_ptr = tr_ptr->next;
   }
   else
   {
    tr_ptr = NULL; /* End of the list reached */
   }
  }
  
  if(tr_ptr != NULL)
  {
   /* TR_INDEX found to be != 0. This pipe has queued transfers */
   returnValue = TRUE;
  }

 }

 return returnValue;
}

/*------------------------------------------------------------------------*/

static void usb_class_phdc_uninit_interface(CLASS_CALL_STRUCT_PTR   phdc_call_ptr)
{
 USB_PHDC_CLASS_INTF_STRUCT  *phdc_intf = phdc_call_ptr->class_intf_handle;
 
 usb_class_phdc_free_qos_metadata_list(phdc_intf);
 
 /* Invalidate the code_key */

 phdc_call_ptr->class_intf_handle = NULL;
 phdc_call_ptr->code_key = 0;
}

/*------------------------------------------------------------------------*/

static void usb_class_phdc_free_qos_metadata_list(USB_PHDC_CLASS_INTF_STRUCT *phdc_intf)
{
 /* Check for allocated memory inside the interface structure */
 USB_PHDC_DESC_QOS_METADATA_LIST *qosMetadataTail = phdc_intf->qosMetadataList; 
 
 while(qosMetadataTail != NULL)
 {
  /* Ok we got a location to be freed. Save this pointer location */
  USB_PHDC_DESC_QOS_METADATA_LIST *qosMetadataLast = qosMetadataTail;
  
  /* Increment the tail for the next iteration */
  qosMetadataTail = qosMetadataTail->pNext;
  
  /* Free the saved location */
  USB_mem_free(qosMetadataLast);
 }
 
 /* Clear the interface allocated memory */
 USB_mem_zero (phdc_intf, sizeof(USB_PHDC_CLASS_INTF_STRUCT)); 
}

/*------------------------------------------------------------------------*/
/* usb_class_phdc_send_control_callback
 * this function will be called when a phdc control request completes
 * 
 * 
 */
/*-------------------------------------------------------------**
** Transaction callback parameters:                            **
**    pointer     pipe handle                                  **
**    pointer     parameter specified by higher level          **
**    uchar_ptr   pointer to buffer containing data (TX or RX) **
**    uint_32     length of data transferred                   **
**    USB_STATUS  status, preferably USB_OK or USB_DONE etc.   **
**-------------------------------------------------------------*/

static void usb_class_phdc_send_control_callback(pointer pipeHandle, pointer callbackParam, uchar_ptr bBuff, uint_32 length, uint_32 usbStatus)
{
  USB_PHDC_CLASS_INTF_STRUCT*      intf_ptr;
  USB_PHDC_PARAM *                 call_param_ptr;
  uint_8                           reqType; 
  uint_8                           phdcStatus = USB_PHDC_ERR;
  USB_STATUS                       status;
  
  UNUSED(bBuff)
  /* parameter checking*/
  if(callbackParam != NULL)
  {
    call_param_ptr = (USB_PHDC_PARAM *)callbackParam;
    /*make sure that no USB event occurs until the function ends*/
    USB_lock();
    intf_ptr = (USB_PHDC_CLASS_INTF_STRUCT*)call_param_ptr->ccs_ptr->class_intf_handle;
    /* make sure that the interface is a valid one*/
    if(usb_host_class_intf_validate(call_param_ptr->ccs_ptr))
    {
      reqType = call_param_ptr->classRequestType;
      if((reqType == PHDC_SET_FEATURE_BREQ) || (reqType == PHDC_CLEAR_FEATURE_BREQ))
      {
        /*clear the setClearRequestPending to enable another set/clear feature requests*/
        intf_ptr->setClearRequestPending = FALSE; 	
      }
      /* update the status of the deviceFeatureSet flag and the phdc status */
      /* if the _usb_hostdev_cntrl_request completes successfully */
      if(usbStatus == USB_OK)
      {
        phdcStatus = USB_PHDC_CTRL_OK;
        if(reqType == PHDC_SET_FEATURE_BREQ )
        {
          intf_ptr->deviceFeatureSet = TRUE;	
        }
        else if(reqType == PHDC_CLEAR_FEATURE_BREQ )
        {
          intf_ptr->deviceFeatureSet = FALSE;	
        }
      }
      /* clear the stall condition if the control endpoint is stalled  */
      else if(usbStatus == USBERR_ENDPOINT_STALLED)	
      {
        status =  _usb_host_ch9_clear_feature(intf_ptr->generalClass.dev_handle,
                    REQ_TYPE_ENDPOINT, ((PIPE_DESCRIPTOR_STRUCT_PTR)pipeHandle)->ENDPOINT_NUMBER , ENDPOINT_HALT);
     	  if((status != USB_STATUS_TRANSFER_QUEUED ) && (status != USB_OK ))
     	  {
     	    phdcStatus = USB_PHDC_ERR_ENDP_CLEAR_STALL;
     	  }
      }
      /* if a user control request callback was installed update the USB_PHDC_PARAM */
      /* structure and launch the callback*/
      if(*intf_ptr->ctrlCallback != NULL)
      {
        call_param_ptr->usb_phdc_status =  phdcStatus;
        call_param_ptr->usb_status =   usbStatus;
        if(usbStatus == USB_OK)
        {
          call_param_ptr->buff_size =   length;	
        }
        (*intf_ptr->ctrlCallback)(call_param_ptr) ;	
      }
    }
    USB_unlock();
  }
}  /* Endbody */

/*------------------------------------------------------------------------*/

/* EOF */

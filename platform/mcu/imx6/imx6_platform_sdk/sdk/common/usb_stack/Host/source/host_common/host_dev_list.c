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
* $FileName: host_dev_list.c$
* $Version : 3.4.12.0$
* $Date    : Sep-11-2009$
*
* Comments:
*
*   This file contains the device-tree root for USB class drivers
*
*END************************************************************************/
#include "host_dev_list.h"
#include "psptypes.h"
#include "user_config.h"
#if HIGH_SPEED_DEVICE
#include "ehci_usbprv_host.h"
#else
#include "usbprv_host.h"
#endif
#include "mem_util.h"

static const PIPE_INIT_PARAM_STRUCT pipe_init_params_prototype =
{
   NULL,                   /* device instance pointer */
   0,                      /* polling interval */
   64,                     /* max. packet size */
   USBCFG_DEFAULT_MAX_NAK_COUNT,  /* max. NAKs */
   0, 0,                   /* 1st frame/uframe */
   0,                      /* zero-length last packet */
   0,                      /* device address on a particular USB */
   0,                      /* endpoint # */
   0,                      /* direction */
   0,                      /* pipe type */
   0,                      /* speed */
   1                       /* tr's per uframe */   
};

/* Unpublished functions, not intended for application use */
static void  usb_dev_list_close_pipe
   (_usb_host_handle,
   PIPE_DESCRIPTOR_STRUCT_PTR
   );

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_dev_list_free_memory
* Returned Value : USB_OK if valid device, else error code
* Comments       :
*     Close pipes & free memory on device's linked list, whose
*        anchor is device.memlist--> 
*     
*END*--------------------------------------------------------------------*/
USB_STATUS usb_dev_list_free_memory
   (
      /* [IN] Handle for the USB device */
      _usb_device_instance_handle   dev_handle
   )
{ /* Body */
   DEV_INSTANCE_PTR  dev;
   DEV_MEMORY_PTR    mem_ptr, list_ptr;
   USB_STATUS        error;
   uint_8            intf_no;

   error = usb_hostdev_validate(dev_handle);
   /* EAI - missing {} so failed if debug on */
   if (error != USB_OK) {
      return error;
   }
   dev = (DEV_INSTANCE_PTR)dev_handle;

   /* Deleting interfaces closes their pipes */
   for (intf_no = 0; intf_no < dev->num_of_interfaces; intf_no++)
      usb_hostdev_delete_interface(dev, dev->intf_descriptor[intf_no]);

   /* Free memory blocks on this device's list */
   USB_lock();
   mem_ptr = dev->memlist;
   list_ptr = mem_ptr->next;

   do {
      free(mem_ptr);
      mem_ptr = list_ptr;
      list_ptr = mem_ptr->next;
   } while (mem_ptr != NULL);

   dev->memlist = NULL;
   USB_unlock();

   return USB_OK;

} /* Endbody */
  

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_dev_list_get_memory
* Returned Value : USB_OK if memory allocated, else error code
* Comments       :
*     Memory is added at the end of a linked list, whose
*        anchor is device.memlist--> 
*     
*END*--------------------------------------------------------------------*/
USB_STATUS usb_dev_list_get_memory
   (
      /* [IN] Pointer to the USB device */
      DEV_INSTANCE_PTR        dev_ptr,

      /* [IN] Size of memory payload required */
      uint_32                 mem_size,

      /* [IN] Type of memory payload required */
      memory_type             mem_type,

      /* [OUT] Pointer to memory block's header */
      pointer         _PTR_   header_ptr
   )
{ /* Body */

   DEV_MEMORY_PTR  mem_ptr, list_ptr;
   USB_STATUS      error;
   
   USB_lock();
   error = usb_hostdev_validate((pointer)dev_ptr);
   USB_unlock();

   if (error != USB_OK) {
      return error;
   }
   
   if ((mem_type <= USB_MEMTYPE_MIN_VALUE) ||
      (mem_type >= USB_MEMTYPE_MAX_VALUE))
   {
      return USBERR_INVALID_MEM_TYPE;
   } /* Endif */
          
   /* get memory for header + payload, rounded up to 4-byte */
   mem_ptr = (DEV_MEMORY_PTR) USB_mem_alloc_word_aligned((mem_size+MEM_HEADER_LEN+3)&0xFFFC);
   
   if (mem_ptr == NULL) {
      return USBERR_GET_MEMORY_FAILED;
   }
   
   if (dev_ptr->memlist == NULL) {
      dev_ptr->memlist = mem_ptr;
   } else {
      list_ptr = dev_ptr->memlist;

      while (list_ptr->next != NULL)
         list_ptr = list_ptr->next;

      list_ptr->next = mem_ptr;   
   } /* EndIf */

   mem_ptr->next = NULL;
   mem_ptr->blktype = mem_type;
   mem_ptr->blksize = mem_size;
   *header_ptr = (pointer)mem_ptr;

   return USB_OK;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_dev_list_attach_device
* Returned Value : 
* Comments       :
*     This function will be called when attach interrupt happens, to
*       add onto the device list and do common initialization.     
* 
*END*--------------------------------------------------------------------*/
USB_STATUS usb_dev_list_attach_device
   (
      _usb_host_handle  handle,
      uint_8            speed,
      uint_8            hub_no,
      uint_8            port_no
   )
{ /* Body */
   USB_STATUS                 status;
   DEV_INSTANCE_PTR           dev_instance_ptr;
   PIPE_INIT_PARAM_STRUCT     ctrl_pipe_init_params;
   USB_HOST_STATE_STRUCT_PTR  usb_host_ptr;
   DEV_INSTANCE_PTR           device_root = NULL;
   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;

   /* Allocate new device instance */
   dev_instance_ptr = (DEV_INSTANCE_PTR) USB_mem_alloc_word_aligned(sizeof(DEV_INSTANCE));

   if (dev_instance_ptr == NULL) {
      return USBERR_GET_MEMORY_FAILED;
   } /* EndIf */

   memset(dev_instance_ptr, 0, sizeof(DEV_INSTANCE));

   dev_instance_ptr->host = handle;
   dev_instance_ptr->speed = speed;
   dev_instance_ptr->hub_no = hub_no;
   dev_instance_ptr->port_no = port_no;
   dev_instance_ptr->cfg_value = 0; /* We don't know yet what the 
                                    ** device's default configuration is 
                                    */

   USB_lock();
   /* Insert instance in pushdown list of devices */
   dev_instance_ptr->next = (DEV_INSTANCE_PTR)usb_host_ptr->DEVICE_LIST_PTR;     /* existing list */
   usb_host_ptr->DEVICE_LIST_PTR = (pointer)dev_instance_ptr;  /* root = new device */
   device_root = dev_instance_ptr;

   /* Find unused address from 1 - 127 for this host */
   device_root->address = 1;
   do {
      dev_instance_ptr = device_root->next; /* 2nd device, if any */
      while (dev_instance_ptr != NULL) 
      {    /* step through list  */
         if (device_root->address == dev_instance_ptr->address) {
            device_root->address++;
            break;
         } /* EndIf */
         dev_instance_ptr = dev_instance_ptr->next;
      } /* EndWhile */
   } while ((dev_instance_ptr != NULL) && (device_root->address <= 127));

   dev_instance_ptr = device_root;   

   /* If all 127 addresses used up, delete instance & bail out */
   if (dev_instance_ptr->address > 127) 
   {
      usb_host_ptr->DEVICE_LIST_PTR = (pointer)\
         dev_instance_ptr->next; /* pop off list */
      USB_unlock();
      free((pointer)dev_instance_ptr);
      return USBERR_ADDRESS_ALLOC_FAILED;
   } /* EndIf */
      
   USB_unlock();

   /*-----------------------------------------------------------**
   ** Open control pipe, get first 8 bytes of device descriptor **
   ** The host_ch9 routine internally sets the callback to      **
   **    usb_host_cntrl_transaction_done (in usb_host_ch9.c)    **
   **    where the action resumes on completion of the get.     **
   **-----------------------------------------------------------*/
   
   ctrl_pipe_init_params = pipe_init_params_prototype;
   ctrl_pipe_init_params.DEV_INSTANCE = dev_instance_ptr;
   ctrl_pipe_init_params.PIPETYPE = USB_CONTROL_PIPE;   
   ctrl_pipe_init_params.SPEED = dev_instance_ptr->speed;

   if (USB_OK != _usb_host_open_pipe(dev_instance_ptr->host, 
      &ctrl_pipe_init_params, 
      &dev_instance_ptr->control_pipe))
   {
      free((pointer)dev_instance_ptr);
      return USBERR_PIPE_OPENED_FAILED;
   } /* Endif */

   /* Set state to enter after transaction completion */
   dev_instance_ptr->state = DEVSTATE_DEVDESC8;

   status = _usb_host_ch9_get_descriptor((pointer)dev_instance_ptr,
      USB_DESC_TYPE_DEV << 8, 0, 8, 
      (uchar_ptr)&dev_instance_ptr->dev_descriptor);
      
   if (status != USB_STATUS_TRANSFER_QUEUED)
   {
      dev_instance_ptr->state = DEVSTATE_INITIAL;
      return USBERR_NO_DESCRIPTOR;
   }
   return USB_OK;

} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_dev_list_close_pipe
* Returned Value : 
* Comments       :
*     Cancel pending TR's and close a pipe.
* 
*END*--------------------------------------------------------------------*/
static void  usb_dev_list_close_pipe
   (
      _usb_host_handle              handle,
      PIPE_DESCRIPTOR_STRUCT_PTR    pipe_ptr
   )
{ /* Body */
   USB_STATUS                       status;
   USB_HOST_STATE_STRUCT_PTR        usb_host_ptr;
   PIPE_TR_STRUCT_PTR               tr_ptr;   

   if (pipe_ptr == NULL)
   {
      return;
   }

   usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR)handle;
      
   USB_lock();

   /* Cancel all active TR's in the pipe */
   tr_ptr = pipe_ptr->tr_list_ptr;
   if (tr_ptr) {
      while (tr_ptr->TR_INDEX != 0) {
         tr_ptr->CALLBACK = NULL;   /* no surprises */

         /* Cancel the transfer at low-level */
         status = _usb_host_cancel_call_interface (handle, pipe_ptr, tr_ptr);
         
         if (status != USB_OK)
         {
         }

         /* Indicate that the TR is idle */
         tr_ptr->TR_INDEX = 0;
         tr_ptr->status = USB_STATUS_IDLE;
         tr_ptr = tr_ptr->next;
      } /* EndWhile */
   } /* Endif */

   /* zero the pipe descriptor, except keep TR list */
   tr_ptr = pipe_ptr->tr_list_ptr;
   _usb_host_close_pipe(handle, (pointer)pipe_ptr);
   pipe_ptr->tr_list_ptr = tr_ptr;   

   USB_unlock();

} /* EndBody */


/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_dev_list_close_pipe_bundle
* Returned Value : 
* Comments       :
*    Close all pipes in an interface's bundle.
* 
*END*--------------------------------------------------------------------*/
void  usb_dev_list_close_pipe_bundle
   (
      PIPE_BUNDLE_STRUCT_PTR    pbs_ptr
   )
{ /* Body */

   _usb_host_handle  handle;
   uint_8            index, index_max;
   handle = ((DEV_INSTANCE_PTR)pbs_ptr->dev_handle)->host;
   index_max = ((INTERFACE_DESCRIPTOR_PTR)
      pbs_ptr->intf_handle)->bNumEndpoints;

   for (index = 0; index < index_max; index++)
      usb_dev_list_close_pipe(handle, (PIPE_DESCRIPTOR_STRUCT_PTR)pbs_ptr->pipe_handle[index]); 

} /* EndBody */
    

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_dev_list_detach_device
* Returned Value : 
* Comments       :
*     This function will be called when detach interrupt happens.
* 
*END*--------------------------------------------------------------------*/
void  usb_dev_list_detach_device
   (
      _usb_host_handle  handle,
      uint_8            hub_no,
      uint_8            port_no
   )
{ /* Body */
   USB_HOST_STATE_STRUCT_PTR  usb_host_ptr = 
      (USB_HOST_STATE_STRUCT_PTR)handle;
   DEV_INSTANCE_PTR  dev_instance_ptr;
   DEV_INSTANCE_PTR  _PTR_ device_root = 
      (DEV_INSTANCE_PTR _PTR_)&usb_host_ptr->DEVICE_LIST_PTR;
   /* search device list for the one being detached */
   USB_lock();
   for (dev_instance_ptr = *device_root;   
      dev_instance_ptr != NULL;
      dev_instance_ptr = dev_instance_ptr->next)   
   {
      if (dev_instance_ptr->port_no != port_no)
         continue;
      if (dev_instance_ptr->hub_no != hub_no)
         continue;
      if (dev_instance_ptr->host != handle)
         continue;
      break;
   } /* EndFor */

   USB_unlock();

   if (dev_instance_ptr == NULL) {
      return;  /* No match, abandon ship! */
   } /* Endif */

   /* SGarg 09/23/2003 . Sometimes  a detach could come even before a 
   attach is finished. This means that we should make sure that device
   memory is not NULL before we try to clean it up. For example this happens
   with OPT tester test 4.10.*/
   
   if(dev_instance_ptr->memlist != NULL)
   {
      /* Notify the application of unavailable interfaces */
      usb_hostdev_attach_detach(dev_instance_ptr, USB_DETACH_EVENT);
      
   }

   /* Unlink device from the instance list */
   dev_instance_ptr->control_callback = NULL; /* no surprises */

   /* Close control pipe */
   usb_dev_list_close_pipe(handle, (PIPE_DESCRIPTOR_STRUCT_PTR)dev_instance_ptr->control_pipe);
   
   /* SGarg 09/24/2003 . Sometimes  a detach could come even before a 
   attach is finished. This means that we should make sure that device
   memory is not NULL before we try to clean it up. For example this happens
   with OPT tester test 4.10.*/
   if(dev_instance_ptr->memlist != NULL)
   {
      usb_dev_list_free_memory((pointer)dev_instance_ptr);
   }

   USB_lock();

   /* Find the address of the "pointer to the detached device" */
   while (*device_root != dev_instance_ptr) {
      device_root = &(*device_root)->next;
   } /* Endwhile */

   /* Remove the device */
   *device_root = dev_instance_ptr->next;

   free(dev_instance_ptr);

   USB_unlock();
} /* EndBody */

/* EOF */
 

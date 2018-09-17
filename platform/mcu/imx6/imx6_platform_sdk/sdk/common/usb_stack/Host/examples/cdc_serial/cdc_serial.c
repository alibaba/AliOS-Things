/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989-2010 ARC International;
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
* $FileName: cdc_serial.c$
* $Version : 3.4.14.0$
* $Date    : Sep-15-2009$
*
* Comments:
*
*   This file is an example of device drivers for the CDC class. This example
*   demonstrates the virtual serial port capability with abstract control model.
*   Redirects the communication from CDC device, which is connected to the board,
*   to the serial port ttyB.
*
*END************************************************************************/

#include "derivative.h" /* include peripheral declarations */
#include "hidef.h" /* for EnableInterrupts macro */
#include "usb_host_cdc.h"
#include "cdc_serial.h"
#include "usb_host_hub_sm.h"
#include "usbevent.h"
#include "usb_bsp.h"
#include "sci.h"
#include "rtc.h"
#include "poll.h"
#include "io.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
#include "exceptions.h"
#endif
#include "mem_util.h"

/***************************************
**
** Globals
*/

USB_EVENT_STRUCT               device_registered;
_usb_device_instance_handle      reg_device = 0;
const CDC_SERIAL_INIT   usb_open_param = {
	USB_UART_NO_BLOCKING | USB_UART_HW_FLOW
};


FILE_CDC_PTR             f_usb;
char                 uart2usb[BUFF_SIZE];
char                 usb2uart[BUFF_SIZE];
char_ptr           device_name = "tty0:";
volatile int_32    uart2usb_num = 0;
volatile int_32    usb2uart_num = 0;
uchar * main_buffer;
volatile int_32    num_done;
extern char buff[BUFF_SIZE];
extern int_32 buff_index;
extern USB_CDC_UART_CODING uart_coding;

void CDC_Task();
extern void _usb_khci_task(void);

static const USB_HOST_DRIVER_INFO DriverInfoTable[] = {
   {
      {0x00,0x00},                  /* Vendor ID per USB-IF             */
      {0x00,0x00},                  /* Product ID per manufacturer      */
      USB_CLASS_COMMUNICATION,      /* Class code                       */
      USB_SUBCLASS_COM_ABSTRACT,    /* Sub-Class code                   */
      0xFF,                         /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_cdc_acm_event        /* Application call back function   */
   },
   {
      {0x00,0x00},                  /* Vendor ID per USB-IF             */
      {0x00,0x00},                  /* Product ID per manufacturer      */
      USB_CLASS_DATA,               /* Class code                       */
      0xFF,                         /* Sub-Class code                   */
      0xFF,                         /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_cdc_data_event           /* Application call back function   */
   },
   /* USB 1.1 hub */
   {

      {0x00,0x00},                  /* Vendor ID per USB-IF             */
      {0x00,0x00},                  /* Product ID per manufacturer      */
      USB_CLASS_HUB,                /* Class code                       */
      USB_SUBCLASS_HUB_NONE,        /* Sub-Class code                   */
      USB_PROTOCOL_HUB_LS,          /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_hub_device_event     /* Application call back function   */
   },
   {
      {0x00,0x00},                  /* All-zero entry terminates        */
      {0x00,0x00},                  /* driver info list.                */
      0,
      0,
      0,
      0,
      NULL
   }
};

    static uint_32       check_open = 0;
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : Main
* Returned Value : none
* Comments       :
*     
*
*END*--------------------------------------------------------------------*/
#ifdef __GNUC__
int main(void) 
#else
void main(void) 
#endif
{
   USB_STATUS           status = USB_OK;
   _usb_host_handle     host_handle;   
   
   /* Initialize the current platform. Call for the _bsp_platform_init which is specific to each processor family */
   _bsp_platform_init();
#ifdef MCU_MK70F12
  sci2_init();
#else
  sci1_init();
#endif
   TimerInit();
   
   /* Init polling global variable */
   POLL_init(); 

   DisableInterrupts;
   #if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
   usb_int_dis();
   #endif	   
   /*
   ** It means that we are going to act like host, so we initialize the
   ** host stack. This call will allow USB system to allocate memory for
   ** data structures, it uses later (e.g pipes etc.).
   */
   status = _usb_host_init (
         HOST_CONTROLLER_NUMBER,   /* Use value in header file */
         MAX_FRAME_SIZE,            /* Frame size per USB spec  */
         &host_handle);             /* Returned pointer */
   if (status != USB_OK) 
   {
      printf("\nUSB Host Initialization failed. STATUS: %x",(unsigned int) status);
      fflush(stdout);
      exit(3);
   }
   status = _usb_host_driver_info_register (
                                    host_handle,
                                    (void *)DriverInfoTable
                                    );
   if (status != USB_OK) 
   {
      printf("\nDriver Registration failed. STATUS: %x", (unsigned int)status);
      fflush(stdout);
      exit(4);
   }    
  
  EnableInterrupts;
  #if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
  usb_int_en();
  #endif	  

  printf("\fInitialization passed. Plug-in CDC device to USB port.\nUse ttyb: as the in/out port for CDC device data.\n");
  
  _usb_event_init(&device_registered);
  uart2usb_num = usb2uart_num = 0; /* reset number of bytes in buffers */
    
  f_usb = (FILE_CDC_PTR)malloc(sizeof(FILE_CDC));
  memset(f_usb, 0, sizeof(FILE_CDC));
    
  f_usb->DEV_PTR = (IO_DEVICE_STRUCT_PTR)malloc(sizeof(IO_DEVICE_STRUCT));
  
  for(;;) 
  {
    Poll(); 
    CDC_Task();
    __RESET_WATCHDOG(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
#ifdef __GNUC__
  return 0;
#endif
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : CDC_Task
* Returned Value : none
* Comments       :
*     Execution starts here
*
*END*--------------------------------------------------------------------*/
void CDC_Task ()
{ /* Body */
    USB_STATUS           status = USB_OK;
    uint_32              i = 0;    
    static uint_32       char_to_recv = 0;

    /* due to the fact that uart driver blocks task, we will check if char is available and then we read it */
    /* write them to USB */
    if(USB_EVENT_NOT_SET == _usb_event_wait_ticks(&device_registered, 0x01, TRUE, 0))
      return;

    if (0 == check_open) 
    {
    	_io_cdc_serial_open(f_usb, device_name, (char *)&usb_open_param);
    	check_open = 1;
    } 
    else 
    {        
#ifdef MCU_mcf51jf128
    	/* todo AI: change this for MCU_mcf51jf128 */
    	char temp;
    	
    	temp = TERMIO_GetChar();
    	if(temp)
    	{
    		buff_index = 1;
    		buff[0] = temp;
    	}
#endif /* MCU_mcf51jf128 */
    	
		/* Read data from UART */
		 DisableInterrupts;
		 uart2usb_num = buff_index;
		 for (i = 0; i < buff_index; i++)
			 uart2usb[i] = buff[i];
		 buff_index = 0;
		 EnableInterrupts;
		 
		 /* Write data to USB */
		 if (uart2usb_num) 
		 {
			num_done = _io_cdc_serial_write(f_usb, uart2usb, (int_32)(sizeof(uart2usb[0]) * uart2usb_num));
			if(num_done > 0)
			{
				for (i = (uint_32)num_done; i < uart2usb_num; i++) /* move buffer data, remove the written ones */
					uart2usb[i - num_done] = uart2usb[i];
				uart2usb_num -= num_done; 
				char_to_recv += num_done;
			}
		 }
        
		 /* Read data from USB */    
		 if(char_to_recv > 0)
		 {       
			 num_done = _io_cdc_serial_read(f_usb, usb2uart + usb2uart_num, (int_32)(sizeof(uart2usb) / sizeof(uart2usb[0]) - usb2uart_num));
			 if(num_done > 0)
			 {
				 usb2uart_num += num_done;	
				 if(char_to_recv >= num_done)
				 {
					 char_to_recv -= num_done;	
				 }
				 else
				 {
					 char_to_recv  = 0;
				 }
			 }
			 else
			 {
				 char_to_recv  = 0;
			 }      
		 }
		 
		 /* write them to UART */
		 if (usb2uart_num > 0) 
		 {
			 for (i = 0; i < usb2uart_num; i++) 
			 {
#ifdef MCU_mcf51jf128
				 printf(" --received--> %c\n\r",usb2uart[i]);
#else
				 sci2_PutChar(usb2uart[i]);        
#endif
			 }
			 usb2uart_num = 0;      
		 }
    }       
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_cdc_acm_event
* Returned Value : None
* Comments       :
*     Called when acm interface has been attached, detached, etc.
*END*--------------------------------------------------------------------*/

void usb_host_cdc_acm_event
   (
      /* [IN] pointer to device instance */
      _usb_device_instance_handle      dev_handle,

      /* [IN] pointer to interface descriptor */
      _usb_interface_descriptor_handle intf_handle,

      /* [IN] code number for event causing callback */
      uint_32                          event_code
   )
{ /* Body */
   INTERFACE_DESCRIPTOR_PTR   intf_ptr = (INTERFACE_DESCRIPTOR_PTR)intf_handle;
   ACM_DEVICE_STRUCT_PTR      acm_device;

   fflush(stdout);
   switch (event_code) 
   {
      case USB_CONFIG_EVENT:
         /* Drop through into attach, same processing */
      case USB_ATTACH_EVENT: 
      {
         USB_CDC_DESC_ACM_PTR     	acm_desc = NULL; 
         USB_CDC_DESC_CM_PTR      	cm_desc = NULL;
         USB_CDC_DESC_HEADER_PTR 	header_desc = NULL;
         USB_CDC_DESC_UNION_PTR 	union_desc = NULL;
         int_32                     external_data = 0;

         /* finds all the descriptors in the configuration */
         if (USB_OK != usb_class_cdc_get_acm_descriptors(dev_handle,
             intf_handle,
             &acm_desc, 
             &cm_desc, 
             &header_desc, 
             &union_desc)) 
            break;
         /* we can allocate new acm device */
         if (NULL == (acm_device = USB_mem_alloc_zero(sizeof(ACM_DEVICE_STRUCT))))
            break;
         /* initialize new interface members and select this interface */
         if (USB_OK != _usb_hostdev_select_interface(dev_handle,
            intf_handle, (pointer)&acm_device->CLASS_INTF))
         {
            free(acm_device);
            break;
         }
         /* set all info got from descriptors to the class interface struct */
         usb_class_cdc_set_acm_descriptors((pointer)&acm_device->CLASS_INTF,
            acm_desc, cm_desc, header_desc, union_desc);

         /* link all already registered data interfaces to this ACM control, if needed */
         if (USB_OK != usb_class_cdc_bind_data_interfaces(dev_handle, (pointer)&acm_device->CLASS_INTF)) 
         {
            free(acm_device);
            break;
         }
         
         printf("----- CDC control interface attach Event -----\n");
         fflush(stdout);
         printf("State = attached");
         printf("  Class = %d", intf_ptr->bInterfaceClass);
         printf("  SubClass = %d", intf_ptr->bInterfaceSubClass);
         printf("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
         fflush(stdout);
          check_open = 0;
         break;
      }
      case USB_INTF_EVENT: 
      {
         CLASS_CALL_STRUCT_PTR   acm_parser;
         USB_STATUS              status;
         
         if (NULL == (acm_parser = usb_class_cdc_get_ctrl_interface(intf_handle)))
             break;
         status = usb_class_cdc_init_ipipe(acm_parser);
         if ((status != USB_OK) && (status != USBERR_OPEN_PIPE_FAILED))
             break;
         
         printf("----- CDC control interface selected -----\n");

         break;
      }
         
      case USB_DETACH_EVENT:
      {
         CLASS_CALL_STRUCT_PTR acm_parser;
         USB_ACM_CLASS_INTF_STRUCT_PTR if_ptr;
         
         if (NULL == (acm_parser = usb_class_cdc_get_ctrl_interface(intf_handle)))
             break;
         if_ptr = (USB_ACM_CLASS_INTF_STRUCT_PTR) acm_parser->class_intf_handle;
         
         _usb_event_set(&if_ptr->acm_event, USB_ACM_DETACH); /* mark we are not using input pipe */

         /* Allow tasks waiting for acm to be finished...
         ** This does have sense only if this task will not be active
         ** or scheduler switches to another task.
         ** For simplification, we dont use any semaphore to indicate that
         ** all tasks have finished its job with device. Instead, we have just
         ** informed them that device is detached and we rely on USB stack layer
         ** that it checking if the device is available returns false.
         ** The code that would synchronize tasks to be finished would look like:
         **
         **  _lwsem_wait(if_ptr->device_using_tasks);
         */

         usb_class_cdc_unbind_data_interfaces(acm_parser);

         USB_unlock();
         
         free(acm_parser);
         /* Use only the interface with desired protocol */
         printf("----- CDC control interface detach event -----\n");
         fflush(stdout);
         printf("State = detached");
         printf("  Class = %d", intf_ptr->bInterfaceClass);
         printf("  SubClass = %d", intf_ptr->bInterfaceSubClass);
         printf("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
         fflush(stdout);
        
         break;
      }

         break;
      default:
         printf("CDC device: unknown control event\n");
         fflush(stdout);
         break;
   } /* EndSwitch */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_cdc_data_event
* Returned Value : None
* Comments       :
*     Called when data interface has been attached, detached, etc.
*END*--------------------------------------------------------------------*/

void usb_host_cdc_data_event
   (
      /* [IN] pointer to device instance */
      _usb_device_instance_handle      dev_handle,

      /* [IN] pointer to interface descriptor */
      _usb_interface_descriptor_handle intf_handle,

      /* [IN] code number for event causing callback */
      uint_32                          event_code
   )
{ /* Body */
   INTERFACE_DESCRIPTOR_PTR   intf_ptr =
      (INTERFACE_DESCRIPTOR_PTR)intf_handle;
   DATA_DEVICE_STRUCT_PTR     data_device;

   fflush(stdout);
   switch (event_code) 
   {
      case USB_CONFIG_EVENT:
         /* Drop through into attach, same processing */
      case USB_ATTACH_EVENT: 
      {
         /* This data interface could be controlled by some control interface,
          * which could be already initialized (or not). We have to find 
          * that interface. Then we need to bind this interface with
          * found control interface. */
         INTERFACE_DESCRIPTOR_PTR   if_desc;
         
         if (USB_OK != usb_class_cdc_get_ctrl_descriptor(dev_handle,
            intf_handle,
            &if_desc))
            break;
         /* interface descriptor found, so we can allocate new data device */
         if (NULL == (data_device = USB_mem_alloc_zero(sizeof(DATA_DEVICE_STRUCT))))
            break;
         /* initializes interface members and selects it */
         if (USB_OK != _usb_hostdev_select_interface(dev_handle,
            intf_handle, (pointer)&data_device->CLASS_INTF))
         {
         	free(data_device);
         	break;
         }
         /* binds this data interface with its control interface, if possible */
         if (USB_OK != usb_class_cdc_bind_acm_interface((pointer)&data_device->CLASS_INTF,
            if_desc))
         {
            free(data_device);
         	break;
         }
         
         printf("----- CDC data interface attach event -----\n");
         fflush(stdout);
         printf("State = attached");
         printf("  Class = %d", intf_ptr->bInterfaceClass);
         printf("  SubClass = %d", intf_ptr->bInterfaceSubClass);
         printf("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
         fflush(stdout);
         check_open = 0;

         break;
      }   
      case USB_INTF_EVENT: 
      {
         CLASS_CALL_STRUCT_PTR data_parser;
         //USB_CDC_UART_CODING uart_coding;
         
         fflush(stdout);
         
         if (NULL == (data_parser = usb_class_cdc_get_data_interface(intf_handle)))
         	break;
         if (USB_OK == usb_class_cdc_install_driver(data_parser, device_name)) 
         {
             if (((USB_DATA_CLASS_INTF_STRUCT_PTR) (data_parser->class_intf_handle))->BOUND_CONTROL_INTERFACE != NULL) 
             {
                 if (reg_device == 0) 
                 {
                     reg_device = dev_handle;
                     _usb_event_set(&device_registered, 0x01);
                 }
             }
             printf("----- Device installed -----\n");
         }
         printf("----- CDC data interface selected -----\n");
         break;
      }
         
      case USB_DETACH_EVENT: 
      {
         CLASS_CALL_STRUCT_PTR data_parser;
         USB_DATA_CLASS_INTF_STRUCT_PTR if_ptr;
                          
         if (NULL == (data_parser = usb_class_cdc_get_data_interface(intf_handle)))
             break;
         if_ptr = (USB_DATA_CLASS_INTF_STRUCT_PTR) data_parser->class_intf_handle;

         if (if_ptr->in_pipe != NULL)
             _usb_event_set(&if_ptr->data_event, USB_DATA_DETACH); /* mark we are not using input pipe */

         /* Allow tasks waiting for data to be finished...
         ** This does have sense only if this task will not be active
         ** or scheduler switches to another task.
         ** For simplification, we dont use any semaphore to indicate that
         ** all tasks have finished its job with device. Instead, we have just
         ** informed them that device is detached and we rely on USB stack layer
         ** that it checking if the device is available returns false.
         ** The code that would synchronize tasks to be finished would look like:
         **
         **  _lwsem_wait(if_ptr->device_using_tasks);
         */

         /* unbind data interface */
         if (USB_OK != usb_class_cdc_unbind_acm_interface(data_parser))
             break;
         //if (USB_OK != usb_class_cdc_uninstall_driver(data_parser))
         //    break;
         
         if (if_ptr->in_pipe != NULL) 
         {
             free(if_ptr->RX_BUFFER);
         }
         
         if (reg_device == dev_handle) 
         {
             reg_device = 0;
             _usb_event_clear(&device_registered, 0x01);
         }

         printf("----- CDC data interface detach Event -----\n");
         fflush(stdout);
         printf("State = detached");
         printf("  Class = %d", intf_ptr->bInterfaceClass);
         printf("  SubClass = %d", intf_ptr->bInterfaceSubClass);
         printf("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
         fflush(stdout);
         
         break;
      }

      default:
         printf("CDC device: unknown data event\n");
         fflush(stdout);
         break;
   } /* EndSwitch */
} /* Endbody */

/* EOF */



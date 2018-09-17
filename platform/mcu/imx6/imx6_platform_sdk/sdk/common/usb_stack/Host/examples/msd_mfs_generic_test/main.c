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
* $FileName: main.c$
* $Version : 
* $Date    : 
*
* Comments:
*
*   This file contains device driver for mass storage class. This code tests
*   the FAT functionalities.
*
*END************************************************************************/

/**************************************************************************
Include the OS and BSP dependent files that define IO functions and
basic types. You may like to change these files for your board and RTOS 
**************************************************************************/
/**************************************************************************
Include the USB stack header files.
**************************************************************************/
#include "derivative.h"
#include "hidef.h"
#include "usb_bsp.h"
#include "sci.h"
#include "rtc.h"
#include "usb_host_hub_sm.h"
#include "diskio.h"
#if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
#include "exceptions.h"
#endif
#include "poll.h"

 /* USB device states */
#define  USB_DEVICE_IDLE                   (0)
#define  USB_DEVICE_ATTACHED               (1)
#define  USB_DEVICE_CONFIGURED             (2)
#define  USB_DEVICE_SET_INTERFACE_STARTED  (3)
#define  USB_DEVICE_INTERFACED             (4)
#define  USB_DEVICE_DETACHED               (5)
#define  USB_DEVICE_OTHER                  (6)



/**************************************************************************
   Global variables
**************************************************************************/
extern volatile DEVICE_STRUCT       mass_device;   /* mass storage device struct */
void Main_Task(void);
extern int fattest(void);                
void   usb_host_mass_device_event (_usb_device_instance_handle,_usb_interface_descriptor_handle,uint_32);

const USB_HOST_DRIVER_INFO DriverInfoTable[] = {
   /* Floppy drive */
   {
      {0x00,0x00},                  /* Vendor ID per USB-IF             */
      {0x00,0x00},                  /* Product ID per manufacturer      */
      USB_CLASS_MASS_STORAGE,       /* Class code                       */
      USB_SUBCLASS_MASS_UFI,        /* Sub-Class code                   */
      USB_PROTOCOL_MASS_BULK,       /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_mass_device_event    /* Application call back function   */
   },

   /* USB 2.0 hard drive */
   {

      {0x49,0x0D},                  /* Vendor ID per USB-IF             */
      {0x00,0x30},                  /* Product ID per manufacturer      */
      USB_CLASS_MASS_STORAGE,       /* Class code                       */
      USB_SUBCLASS_MASS_SCSI,       /* Sub-Class code                   */
      USB_PROTOCOL_MASS_BULK,       /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_mass_device_event    /* Application call back function   */
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
      {0x00,0x00},                  /*    driver info list.             */
      0,
      0,
      0,
      0,
      NULL
   }

};
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : main
* Returned Value : none
* Comments       :
*     Execution starts here
*
*END*--------------------------------------------------------------------*/
#ifdef __GNUC__
int main()
#else
void main() 
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
	status = _usb_host_init
	(HOST_CONTROLLER_NUMBER,    /* Use value in header file */
			MAX_FRAME_SIZE,            /* Frame size per USB spec  */
			&host_handle);             /* Returned pointer */

	if (status != USB_OK) 
	{
		printf("\n\rUSB Host Initialization failed! STATUS: 0x%x", (unsigned int)status);    
	} /* Endif */

	/*
	 ** Since we are going to act as the host driver, register
	 ** the driver information for wanted class/subclass/protocols
	 */
	status = _usb_host_driver_info_register(host_handle, (void*)DriverInfoTable);

	if (status != USB_OK) 
	{
		printf("\n\rDriver Registration failed! STATUS: 0x%x", (unsigned int)status);      
	}

	EnableInterrupts;
#if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
	usb_int_en();
#endif

	printf("\n\rFAT demo\n\rWaiting for USB mass storage to be attached...\n\r");

	for(;;) 
	{
		Main_Task();
		Poll();
		__RESET_WATCHDOG(); /* feeds the dog */
	} /* loop forever */
	/* please make sure that you never leave main */
	
#ifdef __GNUC__
    return 0;
#endif
}
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : Main_Task
* Returned Value : none
* Comments       :
*     
*
*END*--------------------------------------------------------------------*/

void Main_Task ( void )
{ 
	/* Body */
	USB_STATUS           status = USB_OK;
	static uint_8 fat_task_flag = 0;

	/*----------------------------------------------------**
	 ** Infinite loop, waiting for events requiring action **
	 **----------------------------------------------------*/
	switch ( mass_device.dev_state ) 
	{
		case USB_DEVICE_IDLE:
			break;
			
		case USB_DEVICE_ATTACHED:
			printf( "Mass Storage Device Attached\n\r" );
			mass_device.dev_state = USB_DEVICE_SET_INTERFACE_STARTED;
			status = _usb_hostdev_select_interface(mass_device.dev_handle,
					mass_device.intf_handle, (pointer)&mass_device.class_intf);
			/* Can run fat task */
			fat_task_flag = 1;    
			break;
			
		case USB_DEVICE_SET_INTERFACE_STARTED:
			break;
			
		case USB_DEVICE_INTERFACED:
			if(1 == fat_task_flag)
			{	
				fattest();
			}
			/* Disable flag to run FAT task */
			fat_task_flag = 0;
			break;
			
		case USB_DEVICE_DETACHED:
			printf ( "\n\rMass Storage Device Detached\n\r" );
			mass_device.dev_state = USB_DEVICE_IDLE;
			break;
			
		case USB_DEVICE_OTHER:
			break;
			
		default:
			printf( "Unknown Mass Storage Device State = %d\n\r",\
				(int)mass_device.dev_state );
			break;
	} /* Endswitch */
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_mass_device_event
* Returned Value : None
* Comments       : Called when mass storage device has been attached, detached, etc.
*
*END*--------------------------------------------------------------------*/
void usb_host_mass_device_event
   (
      /* [IN] pointer to device instance */
      _usb_device_instance_handle      dev_handle,

      /* [IN] pointer to interface descriptor */
      _usb_interface_descriptor_handle intf_handle,

      /* [IN] code number for event causing callback */
      uint_32           event_code
   )
{ /* Body */
   INTERFACE_DESCRIPTOR_PTR   intf_ptr =
      (INTERFACE_DESCRIPTOR_PTR)intf_handle;

   switch (event_code) 
   {
      case USB_CONFIG_EVENT:
         /* Drop through into attach, same processing */
      case USB_ATTACH_EVENT:
         if (mass_device.dev_state == USB_DEVICE_IDLE) 
         {
            mass_device.dev_handle = dev_handle;
            mass_device.intf_handle = intf_handle;
            mass_device.dev_state = USB_DEVICE_ATTACHED;
         } 
         else 
         {
            printf("Mass Storage Device Is Already Attached\n\r");
         } /* EndIf */
         break;
         
      case USB_INTF_EVENT:
         mass_device.dev_state = USB_DEVICE_INTERFACED;
         break;
         
      case USB_DETACH_EVENT:
         if (mass_device.dev_state != USB_DEVICE_IDLE) 
         {
            mass_device.dev_handle = NULL;
            mass_device.intf_handle = NULL;
            mass_device.dev_state = USB_DEVICE_DETACHED;
         } 
         else 
         {
            printf("Mass Storage Device Is Not Attached\n\r");
         } /* EndIf */
         break;
         
      default:
         mass_device.dev_state = USB_DEVICE_IDLE;
         break;
   } /* EndSwitch */
} /* Endbody */


#include "derivative.h" /* include peripheral declarations */
#include "hidef.h" /* for EnableInterrupts macro */
#include "host_common.h"
#include "usb.h"
#include "hidkeyboard.h"
#include "hidmouse.h"
#include "khci.h"
#include "usb_host_hub_sm.h"
#include "usb_host_hid.h"
#include "usbevent.h"
#include "usb_bsp.h"
#include "sci.h"
#include "rtc.h"
#include "poll.h"

#if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
#include "exceptions.h"
#endif

static const USB_HOST_DRIVER_INFO DriverInfoTable[] =
{
    {
      {0x00,0x00},                  /* Vendor ID per USB-IF             */
      {0x00,0x00},                  /* Product ID per manufacturer      */
      USB_CLASS_HID,                /* Class code                       */
      USB_SUBCLASS_HID_BOOT,        /* Sub-Class code                   */
      USB_PROTOCOL_HID_KEYBOARD,    /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_hid_keyboard_event   /* Application call back function   */
   },
   
   {
      {0x00,0x00},                  /* Vendor ID per USB-IF             */
      {0x00,0x00},                  /* Product ID per manufacturer      */
      USB_CLASS_HID,                /* Class code                       */
      USB_SUBCLASS_HID_BOOT,        /* Sub-Class code                   */
      USB_PROTOCOL_HID_MOUSE,       /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_hid_mouse_event      /* Application call back function   */
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

_usb_host_handle        host_handle; 
extern USB_EVENT_STRUCT USB_Mouse_Event; 
extern USB_EVENT_STRUCT USB_Keyboard_Event; 

extern volatile DEVICE_STRUCT keyboard_hid_device;
extern volatile DEVICE_STRUCT mouse_hid_device;

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : main
* Returned Value : none
* Comments       :
*     Execution starts here
*
*END*--------------------------------------------------------------------*/
void main(void) {
  
  static USB_STATUS status = USB_OK;
  
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
  
  //status = _usb_host_driver_install(0, (pointer)&_bsp_usb_host_callback_table);
  status = _usb_host_init(HOST_CONTROLLER_NUMBER, /* Use value in header file */
        MAX_FRAME_SIZE,         /* Frame size per USB spec  */
       &host_handle);          /* Returned pointer */
  if(status != USB_OK) 
  {
      printf("\nUSB Host Initialization failed. STATUS: 0x%x", status);
      fflush(stdout);
      exit(1);
  }
  
  /*
   ** since we are going to act as the host driver, register the driver
   ** information for wanted class/subclass/protocols
   */
  status = _usb_host_driver_info_register(host_handle, (void *)DriverInfoTable);
  if(status != USB_OK) 
  {         
      printf("\nUSB Initialization driver info failed. STATUS: 0x%x", status);
      fflush(stdout);
	  exit(1);
  }
  _usb_event_init(&USB_Mouse_Event);
  _usb_event_init(&USB_Keyboard_Event);  
  EnableInterrupts;  
  #if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
  usb_int_en();
  #endif    
  
  printf("\nUSB HID Keyboard + Mouse\nWaiting for USB Keyboard or mouse to be attached...\n");
  fflush(stdout);
  
  for(;;) {
    Poll();
    if(USB_DEVICE_IDLE == keyboard_hid_device.DEV_STATE)
      Mouse_Task();
    if(USB_DEVICE_IDLE == mouse_hid_device.DEV_STATE)
      Keyboard_Task();
      
    __RESET_WATCHDOG(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}



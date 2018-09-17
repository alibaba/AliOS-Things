/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved
*
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
* $FileName: audio.c$
* $Version : 
* $Date    : 
*
* Comments:
*
*   This file is an example of device drivers for the Audio host class. This example
*   demonstrates the audio transfer capability of audio host class with audio devices.
*
*END************************************************************************/


#include "derivative.h" /* include peripheral declarations */
#include "hidef.h" /* for EnableInterrupts macro */
#include "audio.h"
#include "usb_host_audio.h"
#include "rtc.h"
#include "usb_host_hub_sm.h"
#include "usbprv_host.h"

#include "usbevent.h"
#include "usb_bsp.h"
#ifdef __MCF52xxx_H__
#include "Int_Ctl_cfv2.h"
#include "PIT0_cfv2.h"
#endif
#if (defined _MK_xxx_H_)
#include "PIT0_kinetis.h"
#endif
#include "audio_pwm.h"
#include "cmt.h"
#include "sci.h"

#include "poll.h"

#if (defined(__MCF52259_H__) || (defined (__MCF52221_H__)))
#include "kbi_cfv2.h"
#endif

#if (defined(_MCF51MM256_H) || (defined _MCF51JE256_H))
#include "kbi.h"
#endif
#if (defined MCU_mcf51jf128)
#include "audio_mtim_cfv1_plus.h"
#endif
#if defined (_MK_xxx_H_)
#include "kbi_kinetis.h"
#endif
/***************************************
**
** Global variables
****************************************/

USB_EVENT_STRUCT USB_Event;

/* Table of driver capabilities this application wants to use */
#if defined (__IAR_SYSTEMS_ICC__)
static const USB_HOST_DRIVER_INFO DriverInfoTable[] =
#else
static USB_HOST_DRIVER_INFO DriverInfoTable[] =
#endif
{
   {
      {0x00,0x00},                  /* Vendor ID per USB-IF             */
      {0x00,0x00},                  /* Product ID per manufacturer      */
      USB_CLASS_AUDIO,      	    /* Class code                       */
      USB_SUBCLASS_AUD_CONTROL,     /* Sub-Class code                   */
      0xFF,                         /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_audio_control_event  /* Application call back function   */
   },
   {
      {0x00,0x00},                  /* Vendor ID per USB-IF             */
      {0x00,0x00},                  /* Product ID per manufacturer      */
      USB_CLASS_AUDIO,              /* Class code                       */
      USB_SUBCLASS_AUD_STREAMING,   /* Sub-Class code                   */
      0xFF,                         /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_audio_stream_event   /* Application call back function   */
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

/* Input Terminal types */
#if defined (__IAR_SYSTEMS_ICC__)
const uchar_ptr it_type_string[7] = 
#else
uchar_ptr it_type_string[7] = 
#endif
	{
		"Input Undefined",
		"Microphone",
		"Desktop Microphone",
		"Personal Microphone",
		"Omni directional Microphone",
		"Microphone array",
		"Processing Microphone array",
	};

/* Output Terminal types */
#if defined (__IAR_SYSTEMS_ICC__)
const uchar_ptr ot_type_string[8] = 
#else
uchar_ptr ot_type_string[8] =
#endif
	{
		"Output Undefined",
		"Speaker",
		"Headphones",
		"Head Mounted Display Audio",
		"Desktop Speaker",
		"Room Speaker",
		"Communication Speaker",
		"Low Frequency Effects Speaker",
	};

volatile AUDIO_CONTROL_DEVICE_STRUCT 	audio_control = { 0 };
volatile AUDIO_STREAM_DEVICE_STRUCT 	audio_stream  = { 0 };

uchar_ptr 								device_string;
uchar 									device_direction = UNDEFINE_DEVICE;

uint_8 									wav_buff[MAX_ISO_PACKET_SIZE];
uint_8 									wav_recv_buff[MAX_ISO_PACKET_SIZE];
uint_8 									wav_recv_buff_tmp[MAX_ISO_PACKET_SIZE];
uint_32	 								packet_size=8;
uint_8									resolution_size;

static USB_AUDIO_CONTROL_STATUS audio_status_interrupt;

extern const uchar 						wav_data[];
extern const uint_32 					wav_size;

/****************************************************************************
**
** Global functions
*****************************************************************************/
void Audio_Task();
extern void _usb_khci_task(void);

/****************************************************************************
**
** Local variables
*****************************************************************************/
static AUDIO_COMMAND audio_com;
static uint_32 audio_position = 0;

static uint_8 g_cur_mute[1] ={0x01};

boolean play = FALSE;
uint_8 sample_step;
uint_8 sample_out;
uint_8 mute_flag = FALSE;
#if (defined MCU_mcf51jf128)
extern void GPIO_Init(void);
#endif
/****************************************************************************
**
** Local functions
*****************************************************************************/
static void USB_Prepare_Data(void);
static USB_STATUS check_device_type(USB_AUDIO_CTRL_DESC_IT_PTR ,USB_AUDIO_CTRL_DESC_OT_PTR ,uchar_ptr _PTR_ ,uchar_ptr);

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
   boolean send = TRUE;
   
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
    
#if (defined(_MCF51MM256_H) || (defined _MCF51JE256_H))
   cmt_init();
   Mcu_Init();
   Kbi_Init();
   pwm_init();
#endif

#if (defined(__MCF52259_H__) || (defined (__MCF52221_H__)) ||(defined (_MK_xxx_H_)) )
   	GPIO_Init();
   	pit0_init();
   	pwm_init();
#endif

#if (defined MCU_mcf51jf128)
   	GPIO_Init();
   	mtim_init();
   	pwm_init();
#endif
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
	   printf("\nUSB Host Initialization failed! STATUS: 0x%x", (unsigned int) status);
	   fflush(stdout);
	   exit(3);
   }
   status = _usb_host_driver_info_register (
                                    host_handle,
                                    (pointer)DriverInfoTable
                                    );
	if (status != USB_OK) 
	{
		printf("\nDriver Registration failed! STATUS: 0x%x",(unsigned int) status);
		fflush(stdout);
		exit(4);
   }    
  
  EnableInterrupts;
  #if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
  usb_int_en();
  #endif	  

  printf("USB Host Audio Demo\r\nWaitting for USB Audio Device to be attached...\r\n");
 
 _usb_event_init(&USB_Event);
 
	for(;;)
	{
    Poll(); 
    Audio_Task();
    __RESET_WATCHDOG(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
#ifdef __GNUC__
return 0;
#endif
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : USB_Prepare_Data
* Returned Value : None
* Comments       :
*     Called to prepare data to send.
*
*END*--------------------------------------------------------------------*/
static void USB_Prepare_Data(void)
{
    uint_8 k;
    /* copy data to buffer */
  	for(k=1; k<packet_size+1; k++, audio_position += sample_step)
  	{
  		wav_buff[resolution_size*k - 1]	= wav_data[audio_position];
  	}
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : USB_Audio_Get_Packet_Size
* Returned Value : None
* Comments       :
*     This function gets the frequency supported by the attached device.
*
*END*--------------------------------------------------------------------*/
static uint_32 USB_Audio_Get_Packet_Size
	(
		/* [IN] Point to format type descriptor */
		USB_AUDIO_STREAM_DESC_FORMAT_TYPE_PTR format_type_desc
	)
{
	uint_32 packet_size_tmp;
    /* copy data to buffer */
    packet_size_tmp = (uint_32)(format_type_desc->tSamFreq[2] << 16) |
    				  (format_type_desc->tSamFreq[1] << 8)  |
    				  (format_type_desc->tSamFreq[0] << 0);
    				  
	return (packet_size_tmp/1000);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : check_device_type
* Returned Value : None
* Comments       :
*    This function check whether the attached device is out-device or in-device. 
*
*END*--------------------------------------------------------------------*/
USB_STATUS check_device_type
	(
	/* [IN] Input terminal descriptor */
	USB_AUDIO_CTRL_DESC_IT_PTR 		it_desc,
	
	/* [IN] Output terminal descriptor */
	USB_AUDIO_CTRL_DESC_OT_PTR 		ot_desc,
	
	/* [OUT] Terminal type name */
	uchar_ptr _PTR_   				device_type,
	
	/* [OUT] device direction */
	uchar_ptr						direction
	)
{
	uchar it_type_high, it_type_low, ot_type_high, ot_type_low;
	
	it_type_high = it_desc->wTerminalType[0];
	it_type_low  = it_desc->wTerminalType[1];
	ot_type_high = ot_desc->wTerminalType[0];
	ot_type_low  = ot_desc->wTerminalType[1];
	
	/* Input terminal associates with audio streaming */
	if (USB_TERMINAL_TYPE == it_type_low)
	{
		*direction = IN_DEVICE;
	}
	/* Input terminal type */
	else if (INPUT_TERMINAL_TYPE == it_type_low)
	{
		/* get type device name */
		*device_type = it_type_string[it_type_high];
	}
	else
	{
		return USBERR_ERROR;
	}
	
	/* Output terminal associates with audio streaming */
	if (USB_TERMINAL_TYPE == ot_type_low)
	{
		if (IN_DEVICE == (*direction))
		{
			*direction = UNDEFINE_DEVICE;
			return USBERR_ERROR;
		}
		else
		{
			*direction = OUT_DEVICE;
		}
	}
	/* Output terminal type */
	else if (OUTPUT_TERMINAL_TYPE == ot_type_low)
	{
		/* get type device name */
		*device_type = ot_type_string[it_type_high];
	}
	else
	{
		return USBERR_ERROR;
	}
	return USB_OK;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_audio_tr_callback
* Returned Value : None
* Comments       :
*     Called when a isochronous pipe transfer is completed.
*
*END*--------------------------------------------------------------------*/
void usb_host_audio_tr_callback
	(
    /* [IN] pointer to pipe */
    _usb_pipe_handle pipe_handle,
    
    /* [IN] user-defined parameter */
    pointer user_parm,
    
    /* [IN] buffer address */
    uchar_ptr buffer,
    
    /* [IN] length of data transferred */
    uint_32 buflen,
    
    /* [IN] status, hopefully USB_OK or USB_DONE */
    uint_32 status
    )
{
	UNUSED(pipe_handle)
	UNUSED(user_parm)
	UNUSED(buffer)
	UNUSED(buflen)
	UNUSED(status)
	
	if (IN_DEVICE == device_direction)
	{
	  if(audio_position > wav_size)
		{
			audio_position = 0;
		}
		USB_Prepare_Data();		
	}
	else
	{
		uint_8 k;
		for(k=0; k < packet_size; k++)
		{
			wav_recv_buff_tmp[k] = wav_recv_buff[resolution_size -1 + resolution_size*k];
		}
		_usb_event_set(&USB_Event, USB_EVENT_RECEIVED_DATA);
	}
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_audio_mute_ctrl_callback
* Returned Value : None
* Comments       :
*     Called when a mute request is sent successfully.
*
*END*--------------------------------------------------------------------*/
void usb_host_audio_mute_ctrl_callback
   (
      /* [IN] pointer to pipe */
      _usb_pipe_handle  pipe_handle,

      /* [IN] user-defined parameter */
      pointer           user_parm,

      /* [IN] buffer address */
      uchar_ptr         buffer,

      /* [IN] length of data transferred */
      uint_32           buflen,

      /* [IN] status, hopefully USB_OK or USB_DONE */
      uint_32           status
   )
{ /* Body */
	
	UNUSED(pipe_handle)
	UNUSED(user_parm)
	UNUSED(buffer)
  UNUSED(buflen)
  UNUSED(status)	
	
	
	printf("Set Mute successfully\r\n");
	fflush(stdout);
	if(TRUE == play) 
    	EnableTimer1Interrupt();
			    
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_audio_interrupt_callback
* Returned Value : None
* Comments       :
*     Called when a interrupt pipe transfer is completed.
*
*END*--------------------------------------------------------------------*/
void usb_host_audio_interrupt_callback
   (
      /* [IN] pointer to pipe */
      _usb_pipe_handle  pipe_handle,

      /* [IN] user-defined parameter */
      pointer           user_parm,

      /* [IN] buffer address */
      uchar_ptr         buffer,

      /* [IN] length of data transferred */
      uint_32           buflen,

      /* [IN] status, hopefully USB_OK or USB_DONE */
      uint_32           status
   )
{ /* Body */

	UNUSED(pipe_handle)
	UNUSED(user_parm)
	UNUSED(buflen)
	
	if(!status)
	{
		audio_status_interrupt.Status=buffer[0];
		audio_status_interrupt.Originator=buffer[1];
		
		printf("\r\nStatus Interrupt:");
		printf("   Status: %d",audio_status_interrupt.Status);	
		printf("   Originator: %d",audio_status_interrupt.Originator);
	}
	fflush(stdout);
	
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_audio_control_event
* Returned Value : None
* Comments       :
*     Called when control interface has been attached, detached, etc.
*END*--------------------------------------------------------------------*/
void usb_host_audio_control_event
   (
      /* [IN] pointer to device instance */
      _usb_device_instance_handle      dev_handle,

      /* [IN] pointer to interface descriptor */
      _usb_interface_descriptor_handle intf_handle,

      /* [IN] code number for event causing callback */
      uint_32                          event_code
   )
{
   INTERFACE_DESCRIPTOR_PTR   intf_ptr =
      (INTERFACE_DESCRIPTOR_PTR)intf_handle;
	
   fflush(stdout);
   switch (event_code) {
      case USB_CONFIG_EVENT:
      /* Drop through into attach, same processing */
      case USB_ATTACH_EVENT: 
      {
         USB_AUDIO_CTRL_DESC_HEADER_PTR     header_desc = NULL;
         USB_AUDIO_CTRL_DESC_IT_PTR			it_desc 	= NULL;
         USB_AUDIO_CTRL_DESC_OT_PTR  		ot_desc 	= NULL;
         USB_AUDIO_CTRL_DESC_FU_PTR  		fu_desc 	= NULL;
        
        if((audio_stream.DEV_STATE == USB_DEVICE_IDLE) || (audio_stream.DEV_STATE == USB_DEVICE_DETACHED)) {
            audio_control.DEV_HANDLE  = dev_handle;
            audio_control.INTF_HANDLE = intf_handle;
            audio_control.DEV_STATE   = USB_DEVICE_ATTACHED;
        }
        else {
            printf("Audio device already attached\r\n");
            fflush(stdout);
        }
        
         /* finds all the descriptors in the configuration */
         if (USB_OK != usb_class_audio_control_get_descriptors(dev_handle,
             intf_handle,
             &header_desc,
             &it_desc,
             &ot_desc,
             &fu_desc))
         {
         	break;
         };
         
		 if(USB_OK != check_device_type(it_desc, ot_desc, &device_string, &device_direction))
		 {
		 	break;
		 }
           /* initialize new interface members and select this interface */
         if (USB_OK != _usb_hostdev_select_interface(dev_handle,
            intf_handle, (pointer)&audio_control.CLASS_INTF))
         {
            break;
         }
         
         /* set all info got from descriptors to the class interface struct */
         usb_class_audio_control_set_descriptors((pointer)&audio_control.CLASS_INTF,
            header_desc, it_desc, ot_desc, fu_desc);
            
         printf("----- Audio control interface: attach event -----\r\n");
         fflush(stdout);
         printf("State = attached");
         printf("  Class = %d", intf_ptr->bInterfaceClass);
         printf("  SubClass = %d", intf_ptr->bInterfaceSubClass);
         printf("  Protocol = %d\r\n", intf_ptr->bInterfaceProtocol);
         break;

      }
      case USB_INTF_EVENT: 
      {
         USB_STATUS status;
         
         status = usb_class_audio_init_ipipe((CLASS_CALL_STRUCT_PTR)&audio_control.CLASS_INTF,
         usb_host_audio_interrupt_callback,NULL);
          
         if ((status != USB_OK) && (status != USBERR_OPEN_PIPE_FAILED))
             break;
         
         printf("----- Audio control interface: interface event -----\r\n");
         
         audio_control.DEV_STATE = USB_DEVICE_INTERFACED;

         break;
      }
         
      case USB_DETACH_EVENT:
         printf("----- Audio control interface: detach event -----\r\n");
         fflush(stdout);
         printf("State = detached");
         printf("  Class = %d", intf_ptr->bInterfaceClass);
         printf("  SubClass = %d", intf_ptr->bInterfaceSubClass);
         printf("  Protocol = %d\r\n", intf_ptr->bInterfaceProtocol);
         fflush(stdout);
         audio_control.DEV_STATE = USB_DEVICE_DETACHED;
         break;
      default:
         printf("Audio Device: unknown control event\r\n");
         fflush(stdout);
         break;
   } /* EndSwitch */
   fflush(stdout);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_audio_stream_event
* Returned Value : None
* Comments       :
*     Called when stream interface has been attached, detached, etc.
*END*--------------------------------------------------------------------*/
void usb_host_audio_stream_event
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
   
   /* Check audio stream interface alternating 0 */
   if (intf_ptr->bNumEndpoints == 0)
   		return;
   
   switch (event_code) {
      case USB_CONFIG_EVENT:
      /* Drop through into attach, same processing */
      case USB_ATTACH_EVENT: 
      {
         USB_AUDIO_STREAM_DESC_SPEPIFIC_AS_IF_PTR     	as_itf_desc 		= NULL;
         USB_AUDIO_STREAM_DESC_FORMAT_TYPE_PTR			frm_type_desc 		= NULL;
         USB_AUDIO_STREAM_DESC_SPECIFIC_ISO_ENDP_PTR  	iso_endp_spec_desc 	= NULL;
                
        if(audio_stream.DEV_STATE == USB_DEVICE_IDLE) {
            audio_stream.DEV_HANDLE  = dev_handle;
            audio_stream.INTF_HANDLE = intf_handle;
            audio_stream.DEV_STATE 	 = USB_DEVICE_ATTACHED;
        }
        else {
            printf("Audio device already attached\r\n");
            fflush(stdout);
        }

         /* finds all the descriptors in the configuration */
         if (USB_OK != usb_class_audio_stream_get_descriptors(dev_handle,
             intf_handle,
             &as_itf_desc,
             &frm_type_desc,
             &iso_endp_spec_desc))
         {
         	break;
         };
             
           /* initialize new interface members and select this interface */
         if (USB_OK != _usb_hostdev_select_interface(dev_handle,
            intf_handle, (pointer)&audio_stream.CLASS_INTF))
         {
              break;
         }
         
         packet_size = USB_Audio_Get_Packet_Size(frm_type_desc);
         resolution_size = (uint_8)(frm_type_desc->bBitResolution / 8);
         
         if (OUT_DEVICE == device_direction)
         {
#if (defined(__MCF52259_H__) || (defined (__MCF52221_H__)))
         MCF_PIT0_PMR = (TIMER_MODULAR/8); 	
#endif
 
#if ((defined _MCF51MM256_H) || (defined _MCF51JE256_H))
         CMTCMD12 = TIMER_MODULAR / TIMER_DIV;
#endif         	
#if (defined _MK_xxx_H_)
#ifdef MCU_MKL25Z4
         PIT_LDVAL0 = 24000 / 8;
#else
         PIT_LDVAL0 = 48000 / 8;
#endif
#endif      
 
#if (defined MCU_mcf51jf128)
     	MTIM0_MODH = 0x1;	/* 1/8 ms Interrupt Generation */
     	MTIM0_MODL = 0x77;
#endif     
         sample_out = (uint_8)(packet_size / 8);
         
         }
         else
         {
#if (defined(__MCF52259_H__) || (defined (__MCF52221_H__)))
         	MCF_PIT0_PMR = (TIMER_MODULAR); 	
#endif
 
#if ((defined _MCF51MM256_H) || (defined _MCF51JE256_H))
        	 CMTCMD12 = TIMER_MODULAR ;
#endif      

#if (defined _MK_xxx_H_)
#ifdef MCU_MKL25Z4
         PIT_LDVAL0 = 24000;
#else
         PIT_LDVAL0 = 48000;
#endif
#endif   
#if (defined MCU_mcf51jf128)
     	MTIM0_MODH = 0xB;	/* 1 ms Interrupt Generation */
     	MTIM0_MODL = 0xB8;
#endif           
			sample_step = (uint_8)(48/packet_size);
		}
		     
         /* set all info got from descriptors to the class interface struct */
         usb_class_audio_stream_set_descriptors((pointer)&audio_stream.CLASS_INTF,
            as_itf_desc, frm_type_desc, iso_endp_spec_desc);
            
         printf("----- Audio stream interface: attach event -----\r\n");
         fflush(stdout);
         printf("State = attached");
         printf("  Class = %d", intf_ptr->bInterfaceClass);
         printf("  SubClass = %d", intf_ptr->bInterfaceSubClass);
         printf("  Protocol = %d\r\n", intf_ptr->bInterfaceProtocol);
         fflush(stdout);

         break;
      }   
      case USB_INTF_EVENT: 
      {
		 audio_stream.DEV_STATE = USB_DEVICE_INTERFACED;
      	 printf("----- Audio stream interface: inteface event-----\r\n");
         printf("Audio device information:\r\n");
         printf("   - Device type    : %s\r\n", device_string);
         printf("   - Frequency      : %d KHz\r\n",(int) packet_size);
         printf("   - Bit resolution : %d bit\r\n", resolution_size * 8); 
         fflush(stdout);
      	break;
      }
         
      case USB_DETACH_EVENT: 
      {
      	 audio_stream.DEV_STATE = USB_DEVICE_DETACHED;
         printf("----- Audio stream interface: detach event-----\r\n");
         fflush(stdout);
         audio_stream.DEV_STATE = USB_DEVICE_DETACHED;
         play = FALSE;
         DisableTimer1Interrupt();
      	 break;
      }

      default:
         printf("Audio device: unknown data event\r\n");
         fflush(stdout);
         break;
   } /* EndSwitch */
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : Audio_Task
* Returned Value : None
* Comments       :
*     Execution starts here
*END*--------------------------------------------------------------------*/
void Audio_Task()
{

	if(USB_DEVICE_INTERFACED == audio_control.DEV_STATE)
   {
  		   	 
   	if(mute_flag == 1)
   		{
   		mute_flag = 0;
   		g_cur_mute[0] = (boolean)(1 - g_cur_mute[0]);
   		if (FALSE == g_cur_mute[0])
   		{
   			printf("Set Mute OFF\n");
   		}
   		else
   		{
   			printf("Set Mute ON\n");
   		}
   		
   		if(1 == play) 
   			DisableTimer1Interrupt();
   		
   		/* Send set mute request */
   		audio_com.CLASS_PTR = (CLASS_CALL_STRUCT_PTR)&audio_control.CLASS_INTF;
   		audio_com.CALLBACK_FN = usb_host_audio_mute_ctrl_callback;
   		audio_com.CALLBACK_PARAM = 0;
   		usb_class_audio_set_cur_mute(&audio_com,(pointer)g_cur_mute);
   	}	
 }
 fflush(stdout);
}
/* EOF */

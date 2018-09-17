/******************************************************************************
 *
 * Freescale Semiconductor Inc.
 * (c) Copyright 2004-2010 Freescale Semiconductor, Inc.
 * ALL RIGHTS RESERVED.
 *
 ******************************************************************************
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
 **************************************************************************//*!
 *
 * @file PIT0_kinetis.c
 *
 * @author
 *
 * @version
 *
 * @date    
 *
 * @brief   
 *          
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <string.h>
#include "derivative.h"     /* include peripheral declarations */
#include "types.h"          /* Contains User Defined Data Types */
#include "audio.h"
#include "usb_bsp.h"
#include "audio_pwm.h"

#define FEEDBACK_SIZE 3

/******************************************************************************
 * Global functions
 *****************************************************************************/
void pit0_init(void);
void EnableTimer1Interrupt(void);
void DisableTimer1Interrupt(void);
void PIT0_ISR(void);
extern void usb_host_audio_tr_callback(_usb_pipe_handle ,pointer ,uchar_ptr ,uint_32 ,uint_32 );

/******************************************************************************
 * Global variable
 *****************************************************************************/
extern volatile AUDIO_CONTROL_DEVICE_STRUCT audio_control;
extern volatile AUDIO_STREAM_DEVICE_STRUCT audio_stream;

extern uint_8 wav_buff[MAX_ISO_PACKET_SIZE];
extern uint_8 wav_recv_buff[MAX_ISO_PACKET_SIZE];
extern uint_8 wav_recv_buff_tmp[MAX_ISO_PACKET_SIZE];

extern uint_8 device_direction;
extern uint_32 packet_size;
#ifndef MCU_MKL25Z4
extern volatile unsigned char *duty;
#endif
extern uint_8 resolution_size;
extern uint_8 sample_out;

extern USB_EVENT_STRUCT USB_Event;

uint_8 audio_sample = 0;

/******************************************************************************
*   @name        pit0_init
*
*   @brief       This function init PIT0
*
*   @return      None
*
*   @comment	 
*    
*******************************************************************************/
void pit0_init(void) 
{ 
	/* Enable PIT Module Clock */
	SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;

	/* Configure PIT */
	PIT_MCR = ~(PIT_MCR_FRZ_MASK |PIT_MCR_MDIS_MASK);

	/* Timer 0.1ms */
#ifndef MCU_MK70F12
		#if (defined MCU_MK20D7) || (defined MCU_MK40D7)
			#ifdef MCGOUTCLK_72_MHZ
				PIT_LDVAL0 = 3600;
			#else
				PIT_LDVAL0 = 4800;
			#endif
		#else
            #ifdef MCU_MKL25Z4
                PIT_LDVAL0 = 2400;
            #else
			PIT_LDVAL0 = 4800;
		#endif
        #endif
#else
	PIT_LDVAL0 = 6000;
#endif

	/* Mask PIT interrupt flag */
	PIT_TFLG0 = PIT_TFLG_TIF_MASK;

#ifdef MCU_MK20D5
	NVICICPR0|=(1<<30);   /* Clear any pending interrupts on PIT0 */
	NVICISER0|=(1<<30);   /* Enable interrupts from PIT0 module */
#elif (defined MCU_MK21D5)
	NVICICPR1 |= (1<<16);   /* Clear any pending interrupts on PIT0 */
	NVICISER1 |= (1<<16);   /* Enable interrupts from PIT0 module */
#elif (defined MCU_MKL25Z4)
	NVIC_ICPR |= (1<<22);   /* Clear any pending interrupts on PIT1 */
	NVIC_ISER |= (1<<22);   /* Enable interrupts from PIT1 module */
#else
	NVICICPR2|=(1<<4);   /* Clear any pending interrupts on PIT0 */
	NVICISER2|=(1<<4);   /* Enable interrupts from PIT0 module */
#endif
    DisableTimer1Interrupt();
}

/******************************************************************************
 *   @name        EnablePIT0Interrupt
 *
 *   @brief       This routine enables Timer Interrupt
 *
 *   @return      None
 *
 ******************************************************************************
 * Enables RTC Timer Interrupt
 *****************************************************************************/
void EnableTimer1Interrupt(void)
{
	/* Enable Timer Interrupt */
		PIT_TCTRL0 |= PIT_TCTRL_TEN_MASK;
		PIT_TCTRL0 |= PIT_TCTRL_TIE_MASK;
		return;
}

/******************************************************************************
 *   @name        DisablePIT0Interrupt
 *
 *   @brief       This routine disables Timer Interrupt
 *
 *   @return      None
 *
 ******************************************************************************
 * Disables RTC Timer Interrupt
 *****************************************************************************/
void DisableTimer1Interrupt(void)
{
	/* Disable Timer Interrupt */
		PIT_TCTRL0 &= ~PIT_TCTRL_TEN_MASK;
		PIT_TCTRL0 &= ~PIT_TCTRL_TIE_MASK;
		return;
}

/******************************************************************************
 *   @name        PIT0_ISR
 *
 *   @brief       This routine services RTC Interrupt
 *
 *	 @param       None
 *
 *   @return      None
 *
 ******************************************************************************
 * Services Programmable Interrupt Timer 0. If a Timer Object expires, then  
 * removes the object from Timer Queue and Calls the callback function 
 * (if registered)
 *****************************************************************************/
void PIT0_ISR(void)
{
	/* Clear RTC Interrupt */
	PIT_TFLG0 |= PIT_TFLG_TIF_MASK;

	if (USB_DEVICE_INTERFACED == audio_stream.DEV_STATE)
	{
		/* Check device type */
		if(IN_DEVICE == device_direction)
		{
			/* For speaker */
			/* Send data */
			usb_audio_send_data((CLASS_CALL_STRUCT_PTR)&audio_control.CLASS_INTF,
			(CLASS_CALL_STRUCT_PTR)&audio_stream.CLASS_INTF, usb_host_audio_tr_callback,
			NULL, packet_size, (uchar_ptr)wav_buff);

			// read feedback data
			usb_audio_recv_data((CLASS_CALL_STRUCT_PTR)&audio_control.CLASS_INTF,
							(CLASS_CALL_STRUCT_PTR)&audio_stream.CLASS_INTF, usb_host_audio_tr_callback,
							NULL, FEEDBACK_SIZE, (uchar_ptr)wav_recv_buff);
		}
			
		else
		{
			/* For microphone */
			if((packet_size-1) < audio_sample)
			{	
				audio_sample = 0;
				_usb_event_clear(&USB_Event, USB_EVENT_RECEIVED_DATA);
				/* Recv data */
				usb_audio_recv_data((CLASS_CALL_STRUCT_PTR)&audio_control.CLASS_INTF,
				(CLASS_CALL_STRUCT_PTR)&audio_stream.CLASS_INTF, usb_host_audio_tr_callback,
				NULL, packet_size*resolution_size, (uchar_ptr)wav_recv_buff);
			}
			else
			{
				/* Check packet is sent completely */
		    	if (USB_EVENT_SET == _usb_event_wait_ticks(&USB_Event, USB_EVENT_RECEIVED_DATA,0,0))
		    	{
				    /* update duty */
#ifdef MCU_MKL25Z4
                    duty = wav_recv_buff_tmp[audio_sample];
#else
                    *duty = wav_recv_buff_tmp[audio_sample];
#endif
		    	}
			}
			audio_sample+= sample_out;		
		}
	}
	PIT_TCTRL0 |= PIT_TCTRL_TIE_MASK;
}

/* EOF */


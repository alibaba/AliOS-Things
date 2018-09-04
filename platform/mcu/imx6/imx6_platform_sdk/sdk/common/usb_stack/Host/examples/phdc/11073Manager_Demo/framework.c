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
* $FileName: framework.c$
* $Version : 1.0.0.0$
* $Date    : Jan-29-2010$
*
* Comments:
*
*   This file is an example of device drivers for the PHDC class which intends
*   to demonstrate the IEEE-11073 Manager functionality.
*   the file is adapted for Bare Metal (BM) infrastructure; the original file runs on MXQ
*
*END************************************************************************/


#include "framework.h"
#include "phdcmng.h"
#include "poll.h"

#if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
#include "exceptions.h"
#endif

extern FW_EVENT_STRUCT USB_Event;

void Main_App_Task(void);
#ifdef __GNUC__
int main(void)
#else
void main(void)
#endif
{       
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

    /* event for USB callback signaling */
    _usb_event_init(&USB_Event);   
  
    DisableInterrupts; 
    #if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
    usb_int_dis();
    #endif	
    ApplicationInit();
	EnableInterrupts;
    #if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
    usb_int_en();
    #endif		
    
    fflush(stdout);
	
	/*
     ** Infinite loop, waiting for events requiring action
     */
    for(;;) 
    {
      Poll();
      Main_App_Task();
      __RESET_WATCHDOG(); /* feeds the dog */
    }  
#ifdef __GNUC__
    return 0;
#endif
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : Main_App_Task
* Returned Value : None
* Comments       :
*     Application function 
*END*--------------------------------------------------------------------*/
void Main_App_Task(void)
{
  // Wait for insertion or removal event
  if(_usb_event_wait_ticks(&USB_Event, USB_EVENT_CTRL | USB_EVENT_DATA, FALSE, 0) == USB_EVENT_SET)
  {
    _usb_event_clear(&USB_Event, USB_EVENT_CTRL);
    ApplicationTask();
  }
}

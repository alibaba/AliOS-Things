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
* Comments:
*
*
*END************************************************************************/
#include "psptypes.h"
#include "derivative.h"
#include "rtc.h"
#include "audio.h"
#include "kbi.h"
#include "cmt.h"

void Kbi_Init(void);
void Mcu_Init(void);
void interrupt VectorNumber_Vkbi2 Kbi_ISR(void);
void interrupt VectorNumber_Virq IRQ_ISR(void);

extern uint_8 mute_flag;
extern boolean play;
extern volatile AUDIO_STREAM_DEVICE_STRUCT 	audio_stream;
extern USB_EVENT_STRUCT USB_Event;
 /******************************************************************************
 * @name        Kbi_Init
 *
 * @brief       This routine is the initialization function
 *
 * @param       None
 *
 * @return      None
 *
 ******************************************************************************
 * This function initializes the keyboard interrupt module
 *****************************************************************************/
 void Kbi_Init(void)
{
    PTCPE |= 0x40;
    PTCD &= 0xBF;
    KBI2SC_KB2ACK = 1;   
    KBI2SC_KBI2MOD = 0;
    KBI2ES = 0x00;
    KBI2PE = 0x02;
    KBI2SC_KB2IE = 1;
    
    //IRQSC = IRQSC_IRQIE_MASK | IRQSC_IRQPE_MASK; /*enable the IRQ interrupt for SW4 */    
}

/*****************************************************************************
 * @name     MCU_Init
 *
 * @brief:   Initialization of the MCU.
 *
 * @param  : None
 *
 * @return : None
 *****************************************************************************
 * It will configure the MCU to disable STOP and COP Modules.
 * It also set the MCG configuration and bus clock frequency.
 ****************************************************************************/
void Mcu_Init(void)
{
#if ((defined _MCF51MM256_H) || (defined _MCF51JE256_H))
  /* Disable watchdog. */
    SOPT1 = 0x23; /* disable COP; enable bkgd, stop and wait mode */
 
    SOPT2 = 0x06; /* set right endianess for USB module */
    SOPT3 = SOPT3_SCI1PS_MASK;/* route TX1,RX1 to PTD6,PTD7 */

  /* Configure LED io pins to be outputs. 
   * EVB51JM128: LED to port mapping: 
   *  LED1  LED2 LED3 LED4 LED5 LED6 LED7 LED8
   *   |     |    |    |    |    |    |    |
   *   ^     ^    ^    ^    ^    ^    ^    ^
   *  PTE2  PTE3 PTF0 PTF1 PTF5 PTD2 PTC4 PTC6
   * DEMO51JM128: LED to port mapping: 
   *  LED1  LED2 LED3 LED4 LED5 LED6 LED7 LED8
   *   |     |    |    |    |    |    |    |
   *   ^     ^    ^    ^    ^    ^    ^    ^
   *  PTE2  PTE3 PTF0 PTF1 PTC2 PTC4 PTF5 PTD2
   */
  PTEDD= PTED_PTED2_MASK | PTED_PTED3_MASK;
  PTFDD= PTFD_PTFD0_MASK | PTFD_PTFD1_MASK | PTFD_PTFD5_MASK;
  PTCDD= PTCD_PTCD4_MASK 
 #ifdef EVB
          | PTCD_PTCD6_MASK
 #else
          | PTCD_PTCD2_MASK          
 #endif          
          ;
          
  PTDDD= PTDD_PTDD2_MASK;
  
  /* Enable internal pull-ups on port E pins to get switches working.
   * EVB51JM128: Button-to-port mapping:
   *  SW1   SW2   SW3   SW4
   *   |     |     |     |
   *   ^     ^     ^     ^
   *  PTG1  PTG2  PTG3  PTD5
   * DEMO51JM128: Button-to-port mapping:
   *  SW1   SW2   SW3   SW4
   *   |     |     |     |
   *   ^     ^     ^     ^
   *  PTG0  PTG1  PTG2  PTG3
   */
  PTGPE= PTGPE_PTGPE1_MASK | PTGPE_PTGPE2_MASK | PTGPE_PTGPE3_MASK
  #ifndef EVB
    | PTGPE_PTGPE0_MASK  
  #endif
   ;
  PTDPE= PTDPE_PTDPE5_MASK ;
  
  IRQSC = IRQSC_IRQIE_MASK | IRQSC_IRQPE_MASK; /*enable the IRQ interrupt for SW4 */    

#endif
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : Kbi_ISR 
* Returned Value   :
* Comments         : KBI interrupt service routine
*    
*
*END*----------------------------------------------------------------------*/
void interrupt VectorNumber_Vkbi2 Kbi_ISR(void)
{  
 /* Acknowledge the interrupt */
   
    KBI2SC_KB2ACK = 1;     /* clear KBI interrupt */  
    
    /* Change Mute state */
    mute_flag=1;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : IRQ_ISR 
* Returned Value   :
* Comments         : IRQ interrupt service routine
*    
*
*END*----------------------------------------------------------------------*/
void interrupt VectorNumber_Virq IRQ_ISR(void)
{
		 	if(USB_DEVICE_INTERFACED == audio_stream.DEV_STATE)
		 	{
				play = (boolean)(1 - play);
				/* play */
		 		if (TRUE == play)
		 		{
		 			printf("Playing ...\r\n");
		 			_usb_event_set(&USB_Event, USB_EVENT_RECEIVED_DATA);
		 			EnableTimer1Interrupt();
		 		}
		 		/* stop */
		 		else
		 		{
		 			printf("\nPaused.\n");
		 			DisableTimer1Interrupt();
		 		}
		 	}
		 		 	
    IRQSC_IRQACK = 1; /* acknoledge IRQ */
    
}

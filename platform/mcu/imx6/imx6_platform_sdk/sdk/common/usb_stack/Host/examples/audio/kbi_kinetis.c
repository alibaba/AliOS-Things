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
 * @file kbi_kinetis.c
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
 
#include <string.h>
#include "derivative.h"     /* include peripheral declarations */
#include "types.h"          /* Contains User Defined Data Types */
#include "audio.h"
#include "PIT0_kinetis.h"
#include "kbi_kinetis.h"



/***************************************
**
** Global variables
****************************************/
extern volatile AUDIO_CONTROL_DEVICE_STRUCT 	audio_stream;
extern boolean play;
extern uint_8 mute_flag;
extern USB_EVENT_STRUCT USB_Event;

/***************************************
**
** Global functions
****************************************/
/******************************************************************************
*   @name        GPIO_Init
*
*   @brief       This function init GPIO
*
*   @return      None
*
*   @comment	 
*    
*******************************************************************************/
void GPIO_Init(void)
{
#if (defined MCU_MK20D5) || (defined MCU_MK20D7) || (defined MCU_MK40D7)
	/* Enable clock gating to PORTC */
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
	/* Set input PORTC1 */
	PORTC_PCR1 = PORT_PCR_MUX(1);
	GPIOC_PDDR &= ~((uint_32)1 << 1);
	/* Pull up */
	PORTC_PCR1 |= PORT_PCR_PE_MASK|PORT_PCR_PS_MASK;
	/* GPIO_INT_EDGE_HIGH */
	PORTC_PCR1 |= PORT_PCR_IRQC(9);	
	/* Set input PORTC2 */
	PORTC_PCR2 =  PORT_PCR_MUX(1);
	GPIOC_PDDR &= ~((uint_32)1 << 2);
	/* Pull up */
	PORTC_PCR2 |= PORT_PCR_PE_MASK|PORT_PCR_PS_MASK;
	/* GPIO_INT_EDGE_HIGH */
	PORTC_PCR2 |= PORT_PCR_IRQC(9);
	/* Enable interrupt */
	PORTC_ISFR |=(1<<1);
	PORTC_ISFR |=(1<<2);
	#ifdef MCU_MK20D5
		NVICICPR1 = 1 << ((42)%32);
		NVICISER1 = 1 << ((42)%32);	
	#else
		NVICICPR2 = 1 << ((89)%32);
		NVICISER2 = 1 << ((89)%32);	
	#endif	
#endif
#if (defined MCU_MK40N512VMD100)||(defined MCU_MK53N512CMD100) 
	/* Enable clock gating to PORTC */
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
	/* set in put PORTC5*/
	PORTC_PCR5 =  PORT_PCR_MUX(1);
	GPIOC_PDDR &= ~((uint_32)1 << 5);
	/* pull up*/
	PORTC_PCR5 |= PORT_PCR_PE_MASK|PORT_PCR_PS_MASK;
	/* GPIO_INT_EDGE_HIGH */
	PORTC_PCR5 |= PORT_PCR_IRQC(9);	
	/* set in put PORTC13*/
	PORTC_PCR13 =  PORT_PCR_MUX(1);
	GPIOC_PDDR &= ~((uint_32)1 << 13);
	/* pull up*/
	PORTC_PCR13 |= PORT_PCR_PE_MASK|PORT_PCR_PS_MASK;
	/* GPIO_INT_EDGE_HIGH */
	PORTC_PCR13 |= PORT_PCR_IRQC(9);
	/* enable interrupt */
	PORTC_ISFR |=(1<<5);
	PORTC_ISFR |=(1<<13);
	NVICICPR2 = 1 << ((89)%32);
	NVICISER2 = 1 << ((89)%32);
#endif	
#if defined(MCU_MK60N512VMD100)  
	/* Enable clock gating to PORTA and PORTE */
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK |SIM_SCGC5_PORTE_MASK;
	
	/* set in put PORTA pin 19 */
	PORTA_PCR19 =  PORT_PCR_MUX(1);
	GPIOA_PDDR &= ~((uint_32)1 << 19);
	
	/* pull up*/
	PORTA_PCR19 |= PORT_PCR_PE_MASK|PORT_PCR_PS_MASK;
	
	/* GPIO_INT_EDGE_HIGH */
	PORTA_PCR19 |= PORT_PCR_IRQC(9);	
	
	/* set in put PORTE pin 26 */
	PORTE_PCR26 =  PORT_PCR_MUX(1);
	GPIOE_PDDR &= ~((uint_32)1 << 26);
	
	/* pull up*/
	PORTE_PCR26 |= PORT_PCR_PE_MASK|PORT_PCR_PS_MASK;
	
	/* GPIO_INT_EDGE_HIGH */
	PORTE_PCR26 |= PORT_PCR_IRQC(9);
	
	/* Clear interrupt flag */
	PORTA_ISFR |=(1<<19);
	PORTE_ISFR |=(1<<26);
	
	/* enable interrupt port A */
	NVICICPR2 = 1 << ((87)%32);
	NVICISER2 = 1 << ((87)%32);
	
	/* enable interrupt port E */
	NVICICPR2 = 1 << ((91)%32);
	NVICISER2 = 1 << ((91)%32);
#endif
#ifdef MCU_MK70F12
	/* Enable clock gating to PORTA and PORTE */
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK|SIM_SCGC5_PORTE_MASK;

	/* set in put PORTA pin 19 */
	PORTD_PCR0 =  PORT_PCR_MUX(1);
	GPIOD_PDDR &= ~((uint_32)1 << 0);

	/* pull up*/
	PORTD_PCR0 |= PORT_PCR_PE_MASK|PORT_PCR_PS_MASK;

	/* GPIO_INT_EDGE_HIGH */
	PORTD_PCR0 |= PORT_PCR_IRQC(9);	

	/* set in put PORTE pin 26 */
	PORTE_PCR26 =  PORT_PCR_MUX(1);
	GPIOE_PDDR &= ~((uint_32)1 << 26);

	/* pull up*/
	PORTE_PCR26 |= PORT_PCR_PE_MASK|PORT_PCR_PS_MASK;

	/* GPIO_INT_EDGE_HIGH */
	PORTE_PCR26 |= PORT_PCR_IRQC(9);

	/* Clear interrupt flag */
	PORTD_ISFR |=(1<<0);
	PORTE_ISFR |=(1<<26);

	/* enable interrupt port D */
	NVICICPR2 = 1 << ((90)%32);
	NVICISER2 = 1 << ((90)%32);

	/* enable interrupt port E */
	NVICICPR2 = 1 << ((91)%32);
	NVICISER2 = 1 << ((91)%32);
#endif
#ifdef MCU_MK21D5
	/* Enable clock gating to PORTC */
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
	
	PORTC_PCR6 =  PORT_PCR_MUX(1);
	GPIOC_PDDR &= ~((uint_32)1 << 6);	
	/* Pull up enabled */
	PORTC_PCR6 |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;	
	/* GPIO_INT_EDGE_HIGH */
	PORTC_PCR6 |= PORT_PCR_IRQC(9);	
	
	/* Set input on PORTC pin 7 */
	PORTC_PCR7 =  PORT_PCR_MUX(1);
	GPIOC_PDDR &= ~((uint_32)1 << 7);	
	/* Pull up */
	PORTC_PCR7 |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;	
	/* GPIO_INT_EDGE_HIGH */
	PORTC_PCR7 |= PORT_PCR_IRQC(9);
	
	/* enable interrupt port C */
	NVICICPR1 = 1 << ((61)%32);
	NVICISER1 = 1 << ((61)%32);
#endif
#if defined(MCU_MKL25Z4)  
    /* Enable clock gating to PORTA, PORTB, PORTC, PORTD and PORTE */
    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK 
              | SIM_SCGC5_PORTB_MASK 
              | SIM_SCGC5_PORTC_MASK 
              | SIM_SCGC5_PORTD_MASK 
              | SIM_SCGC5_PORTE_MASK);

    /* LEDs settings */
    PORTA_PCR5  =  PORT_PCR_MUX(1);
    PORTA_PCR16 =  PORT_PCR_MUX(1);
    PORTA_PCR17 =  PORT_PCR_MUX(1);
    PORTB_PCR8  =  PORT_PCR_MUX(1);

    GPIOA_PDDR |= (1<<5) | (1<<16) | (1<<17);
    GPIOB_PDDR |= (1<<8);

    /* Switch buttons settings */
    /* Set input on PORTC pin 3 */
    PORTC_PCR3 =  PORT_PCR_MUX(1);
    GPIOC_PDDR &= ~((uint_32)1 << 3);
    /* Pull up enabled */
    PORTC_PCR3 |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    /* GPIO_INT_EDGE_HIGH */
    PORTC_PCR3 |= PORT_PCR_IRQC(9);

    /* Set input on PORTA pin 4 */
    PORTA_PCR4 =  PORT_PCR_MUX(1);
    GPIOA_PDDR &= ~((uint_32)1 << 4);
    /* Pull up */
    PORTA_PCR4 |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    /* GPIO_INT_EDGE_HIGH */
    PORTA_PCR4 |= PORT_PCR_IRQC(9);

    /* Clear interrupt flag */
    PORTC_ISFR |= (1<<3);
    PORTA_ISFR |= (1<<4);

    /* Enable interrupt port A */
    NVIC_ICPR = 1 << 30;
    NVIC_ISER = 1 << 30;
#endif
}


/******************************************************************************
*   @name        IRQ_ISR_PTC
*
*   @brief       Service interrupt routine of IRQ
*
*   @return      None
*
*   @comment	 
*    
*******************************************************************************/
void IRQ_ISR_PORTC(void)
{
	#if(defined MCU_MK20D5) || (defined MCU_MK20D7) || (defined MCU_MK40D7)
		if(PORTC_ISFR & (1<<1))
	#elif (defined MCU_MK21D5)
		if(PORTC_ISFR & (1<<6))	
    #else
		if(PORTC_ISFR & (1<<5))
	#endif
	 	{
		 	if(USB_DEVICE_INTERFACED == audio_stream.DEV_STATE)
		 	{
				play = (boolean)(1 - play);
				/* play */
		 		if (TRUE == play)
		 		{
		 			printf("Playing ...\n");
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
		 	/* Clear the bit by writing a 1 to it */
		 	#if(defined MCU_MK20D5) || (defined MCU_MK20D7) || (defined MCU_MK40D7)
				PORTC_ISFR |= (1<<1);
            #elif (defined MCU_MK21D5)
				PORTC_ISFR |= (1<<6);
            #else
				PORTC_ISFR |= (1<<5);
			#endif
	 	}
	 
	#if(defined MCU_MK20D5) || (defined MCU_MK20D7) || (defined MCU_MK40D7)
		if(PORTC_ISFR & (1<<2))
    #elif (defined MCU_MK21D5)
	    if(PORTC_ISFR & (1<<7))	
    #else		
	 	if(PORTC_ISFR & (1<<13))
	#endif
	 	{
	 		mute_flag=1;
	 		/* Clear the bit by writing a 1 to it */
			#if(defined MCU_MK20D5) || (defined MCU_MK20D7) || (defined MCU_MK40D7)
				PORTC_ISFR |= (1<<2);
            #elif (defined MCU_MK21D5)
				PORTC_ISFR |= (1<<7);
            #else
				PORTC_ISFR |= (1<<13);
			#endif
	 	}	 	
}

/******************************************************************************
*   @name        IRQ_ISR_PORTA
*
*   @brief       Service interrupt routine of IRQ
*
*   @return      None
*
*   @comment	 
*    
*******************************************************************************/
void IRQ_ISR_PORTA(void)
{
#if defined MCU_MKL25Z4
    if(PORTA_ISFR & (1<<4))
#else    	
	if(PORTA_ISFR & (1<<19))
#endif
	{
		if(USB_DEVICE_INTERFACED == audio_stream.DEV_STATE)
		{
			play = (boolean)(1 - play);
			/* play */
			if (TRUE == play)
			{
				printf("Playing ...\n");
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
#if defined MCU_MKL25Z4
        PORTA_ISFR |=(1<<4);
#else
		PORTA_ISFR |=(1<<19);			/* Clear the bit by writing a 1 to it */
#endif
	}
}

/******************************************************************************
*   @name        IRQ_ISR_PORTD
*
*   @brief       Service interrupt routine of IRQ
*
*   @return      None
*
*   @comment	 
*    
*******************************************************************************/
void IRQ_ISR_PORTD(void)
{
	if(PORTD_ISFR & (1<<0))
	{
		if(USB_DEVICE_INTERFACED == audio_stream.DEV_STATE)
		{
			play = (boolean)(1 - play);
			/* play */
			if (TRUE == play)
			{
				printf("Playing ...\n");
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
		PORTD_ISFR |= (1<<0);			/* Clear the bit by writing a 1 to it */
	}
}

/******************************************************************************
*   @name        IRQ_ISR_PORTE
*
*   @brief       Service interrupt routine of IRQ
*
*   @return      None
*
*   @comment	 
*    
*******************************************************************************/
// andrei
volatile uint_8 dbg_continue = FALSE;
void IRQ_ISR_PORTE(void)
{
	if(PORTE_ISFR & (1<<26))
	{
		dbg_continue++;
		mute_flag = 1;
		PORTE_ISFR |= (1<<26);			/* Clear the bit by writing a 1 to it */
	}
}
/* EOF */

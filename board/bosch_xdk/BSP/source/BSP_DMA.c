/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Bosch Connected Devices and Solutions GmbH. 
 * All Rights Reserved. Confidential.
 *
 * Distribution only to people who need to know this information in
 * order to do their job.(Need-to-know principle).
 * Distribution to persons outside the company, only if these persons
 * signed a non-disclosure agreement.
 * Electronic transmission, e.g. via electronic mail, must be made in
 * encrypted form.
 */
/*----------------------------------------------------------------------------*/
/**
 * @file
 * @brief  Implementation of DMA BSP functions
 */

#include "BCDS_HAL.h"

#if BCDS_FEATURE_DMA

#include "BCDS_Basics.h"

#include "BSP_DMA.h"
#include "BCDS_MCU_DMA_Handle.h"

#include "BSP_BoardType.h"
#include "BSP_BoardSettings.h"
#include "BSP_BoardShared.h"


Retcode_T BSP_DMA_Initialize(void  * DmaControlBlock)
{
	Retcode_T retcode = RETCODE_OK;
    
	/*ensure right alignment of control block structure */
	if (!((uint32_t)DmaControlBlock & (0x100-1)))
	{
	
		/* Enable DMA Clock */
		CMU_ClockEnable(cmuClock_DMA, true);
		DMA_Reset();

		/* bus error interrupt */
	    DMA->IEN = DMA_IEN_ERR;

	    /*assign control block base address */
	    DMA->CTRLBASE = (uint32_t)DmaControlBlock;

	    /* Disable DMA Clock */
	    CMU_ClockEnable(cmuClock_DMA, false);

	    retcode = RETCODE_OK;
	}
	else
	{
		/*assign control block base address */
		DMA->CTRLBASE =  0;
		retcode = RETCODE(RETCODE_SEVERITY_FATAL, RETCODE_INVALID_PARAM );
	}
	return retcode;

}

Retcode_T BSP_DMA_Enable()
{
	Retcode_T retcode = RETCODE_OK;

	/* Clear/enable DMA interrupts */
	NVIC_ClearPendingIRQ(DMA_IRQn);
	NVIC_SetPriority(DMA_IRQn, DMA_INTERRUPT_PRIO);
	NVIC_EnableIRQ(DMA_IRQn);
	/* Enable DMA Clock */
	CMU_ClockEnable(cmuClock_DMA, true);
	/* Configure and enable the DMA controller */
	DMA->CONFIG = ((uint32_t) (0 << _DMA_CONFIG_CHPROT_SHIFT) | DMA_CONFIG_EN);

	return retcode;

}

Retcode_T BSP_DMA_Disable(void)
{
	Retcode_T retcode = RETCODE_OK;

	/* Clear/disable DMA interrupts */
	NVIC_DisableIRQ(DMA_IRQn);
	NVIC_ClearPendingIRQ(DMA_IRQn);

	/* Configure and enable the DMA controller */
	DMA->CONFIG = _DMA_CONFIG_RESETVALUE;

	/* Enable DMA Clock */
	CMU_ClockEnable(cmuClock_DMA, false);


	return retcode;

}

#endif /* BCDS_FEATURE_DMA */

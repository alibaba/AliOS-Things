#ifndef _DRV_LDMA_H_
#define _DRV_LDMA_H_

#include <stdint.h>
#include <stdio.h>
#include <nds32_intrinsic.h>


/*
 * Copyright (c) 2012-2015 Andes Technology Corporation
 * All rights reserved.
 *
 */

#define MAX(a,b)         (((a) > (b)) ? (a) : (b))

#define CFG_LMDMA

#ifdef CFG_LMDMA
#define TRY_TO_USE_LMDMA (1)
#else
#define TRY_TO_USE_LMDMA (0)
#endif

#define LM_ILM 0
#define LM_DLM 1

#define EXT_TO_DLM (0)		// EXT -> DLM
#define DLM_TO_EXT (1)		// DLM -> EXT
#define DLM_ADDR_MASK(addr) (addr&0x000FFFFF)	//remove  0x10-0000 base address!!


#if 0
#define IS_ULM()            (__nds32__mfsr(NDS32_SR_MSC_CFG) & (1 << 25))
#define ULMMAP_OK(a,b)      (((a) & MAX(g_ilm_size, g_dlm_size)) == ((b) & MAX(g_ilm_size, g_dlm_size)) ? 0 : 1)
#endif
#define IS_LMDMA_V2()       (((__nds32__mfsr(NDS32_SR_DMA_CFG) >> 24) & 0xff) >= 0x2)

#define DMA_SETUP_offLM     0	/* Local Memory Selection */
#define DMA_SETUP_offTDIR   1	/* Transfer Direction */
#define DMA_SETUP_offTES    2	/* Transfer Element Size */
#define DMA_SETUP_offESTR   4	/* External memory transfer Stride */
#define DMA_SETUP_offCIE    16	/* Interrupt Enable on Completion */
#define DMA_SETUP_offSIE    17	/* Interrupt Enable on explicit Stop */
#define DMA_SETUP_offEIE    18	/* Interrupt Enable on Error */
#define DMA_SETUP_offUE     19	/* Enable the Un-aligned External Address */
#define DMA_SETUP_off2DE    20	/* Enable the 2-D External Transfer */
#define DMA_SETUP_offCOA    21	/* Transfer Coalescable */
/* bit 22:31 reserved */

#define DMA_SETUP_mskLM     ( 0x1UL << DMA_SETUP_offLM )
#define DMA_SETUP_mskTDIR   ( 0x1UL << DMA_SETUP_offTDIR )
#define DMA_SETUP_mskTES    ( 0x3UL << DMA_SETUP_offTES )
#define DMA_SETUP_mskESTR   ( 0xFFFUL << DMA_SETUP_offESTR )
#define DMA_SETUP_mskCIE    ( 0x1UL << DMA_SETUP_offCIE )
#define DMA_SETUP_mskSIE    ( 0x1UL << DMA_SETUP_offSIE )
#define DMA_SETUP_mskEIE    ( 0x1UL << DMA_SETUP_offEIE )
#define DMA_SETUP_mskUE     ( 0x1UL << DMA_SETUP_offUE )
#define DMA_SETUP_msk2DE    ( 0x1UL << DMA_SETUP_off2DE )
#define DMA_SETUP_mskCOA    ( 0x1UL << DMA_SETUP_offCOA )

void dma_memcpy_dlm2pbuf(uint32_t des_addr,uint32_t src_addr,int size);
void dma_memcpy_pbuf2dlm(uint32_t des_addr,uint32_t src_addr,int size);

#endif	//_DRV_LDMA_H_
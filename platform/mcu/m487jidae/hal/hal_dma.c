/* Microcontroller Library
 * Copyright (c) 2015-2016 Nuvoton
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "objects.h"
#include "nu_modutil.h"
#include "nu_bitutil.h"
#include "hal_status.h"
#include <k_api.h>
#include "hal_dma.h"

#define NU_PDMA_CH_MAX      PDMA_CH_MAX     /* Specify maximum channels of PDMA */
#define NU_PDMA_CH_Pos      0               /* Specify first channel number of PDMA */
#define NU_PDMA_CH_Msk      (((1 << NU_PDMA_CH_MAX) - 1) << NU_PDMA_CH_Pos)

struct nu_dma_chn_s {
    void        (*handler)(uint32_t, uint32_t);
    uint32_t    id;
    uint32_t    event;
};

static int dma_inited = 0;
static uint32_t dma_chn_mask = 0;
static struct nu_dma_chn_s dma_chn_arr[NU_PDMA_CH_MAX];

//static void pdma_vec(void);
static const struct nu_modinit_s dma_modinit = {DMA_0, PDMA_MODULE, 0, 0, PDMA_RST, PDMA_IRQn, (void *) dma_chn_arr};


typedef struct {
	uint32_t m_u32Peripheral;
	uint32_t m_u32SrcCtl;
	uint32_t m_u32DstCtl;
} S_PDMA_Peripheral_Ctl;	

const S_PDMA_Peripheral_Ctl sPDMAPeripheralCtl[ ] = {
		{ PDMA_MEM, PDMA_SAR_INC, PDMA_DAR_INC },
		// M2P
		{ PDMA_USB_TX, PDMA_SAR_INC, PDMA_DAR_FIX },
		{ PDMA_UART0_TX, PDMA_SAR_INC, PDMA_DAR_FIX },
		{ PDMA_UART1_TX, PDMA_SAR_INC, PDMA_DAR_FIX },
		{ PDMA_UART2_TX, PDMA_SAR_INC, PDMA_DAR_FIX },
		{ PDMA_UART3_TX, PDMA_SAR_INC, PDMA_DAR_FIX },
		{ PDMA_UART4_TX, PDMA_SAR_INC, PDMA_DAR_FIX },
		{ PDMA_UART5_TX, PDMA_SAR_INC, PDMA_DAR_FIX },
		{ PDMA_USCI0_TX, PDMA_SAR_INC, PDMA_DAR_FIX },
		{ PDMA_USCI1_TX, PDMA_SAR_INC, PDMA_DAR_FIX },
		{ PDMA_QSPI0_TX, PDMA_SAR_INC, PDMA_DAR_FIX },
		{ PDMA_SPI0_TX, PDMA_SAR_INC, PDMA_DAR_FIX },
		{ PDMA_SPI1_TX, PDMA_SAR_INC, PDMA_DAR_FIX },
		{ PDMA_SPI2_TX, PDMA_SAR_INC, PDMA_DAR_FIX },
		{ PDMA_SPI3_TX, PDMA_SAR_INC, PDMA_DAR_FIX },
		{ PDMA_I2C0_TX, PDMA_SAR_INC, PDMA_DAR_FIX },
		{ PDMA_I2C1_TX, PDMA_SAR_INC, PDMA_DAR_FIX },
		{ PDMA_I2C2_TX, PDMA_SAR_INC, PDMA_DAR_FIX },
		{ PDMA_I2S0_TX, PDMA_SAR_INC, PDMA_DAR_FIX },
		{ PDMA_DAC0_TX, PDMA_SAR_INC, PDMA_DAR_FIX },
		{ PDMA_DAC1_TX, PDMA_SAR_INC, PDMA_DAR_FIX },
		
		// P2M
		{ PDMA_USB_RX, PDMA_SAR_FIX, PDMA_DAR_INC },
		{ PDMA_UART0_RX, PDMA_SAR_FIX, PDMA_DAR_INC },
		{ PDMA_UART1_RX, PDMA_SAR_FIX, PDMA_DAR_INC },
		{ PDMA_UART2_RX, PDMA_SAR_FIX, PDMA_DAR_INC },
		{ PDMA_UART3_RX, PDMA_SAR_FIX, PDMA_DAR_INC },
		{ PDMA_UART4_RX, PDMA_SAR_FIX, PDMA_DAR_INC },
		{ PDMA_UART5_RX, PDMA_SAR_FIX, PDMA_DAR_INC },
		{ PDMA_USCI0_RX, PDMA_SAR_FIX, PDMA_DAR_INC },
		{ PDMA_USCI1_RX, PDMA_SAR_FIX, PDMA_DAR_INC },
		{ PDMA_QSPI0_RX, PDMA_SAR_FIX, PDMA_DAR_INC },
		{ PDMA_SPI0_RX, PDMA_SAR_FIX, PDMA_DAR_INC },
		{ PDMA_SPI1_RX, PDMA_SAR_FIX, PDMA_DAR_INC },
		{ PDMA_SPI2_RX, PDMA_SAR_FIX, PDMA_DAR_INC },
		{ PDMA_SPI3_RX, PDMA_SAR_FIX, PDMA_DAR_INC },
		{ PDMA_EPWM0_P1_RX, PDMA_SAR_FIX, PDMA_DAR_INC },
		{ PDMA_EPWM0_P2_RX, PDMA_SAR_FIX, PDMA_DAR_INC },
		{ PDMA_EPWM0_P3_RX, PDMA_SAR_FIX, PDMA_DAR_INC },
		{ PDMA_EPWM1_P1_RX, PDMA_SAR_FIX, PDMA_DAR_INC },
		{ PDMA_EPWM1_P2_RX, PDMA_SAR_FIX, PDMA_DAR_INC },
		{ PDMA_EPWM1_P3_RX, PDMA_SAR_FIX, PDMA_DAR_INC },
		{ PDMA_I2C0_RX, PDMA_SAR_FIX, PDMA_DAR_INC },
		{ PDMA_I2C1_RX, PDMA_SAR_FIX, PDMA_DAR_INC },
		{ PDMA_I2C2_RX, PDMA_SAR_FIX, PDMA_DAR_INC },
		{ PDMA_I2S0_RX, PDMA_SAR_FIX, PDMA_DAR_INC },
		{ PDMA_ADC_RX, PDMA_SAR_FIX, PDMA_DAR_INC },		
		{ 0 }
};
	
static PDMA_T *hal_dma_modbase(void)
{
    return (PDMA_T *) NU_MODBASE(dma_modinit.modname);
}

static void hal_dma_init(void)
{
		PDMA_T * pdma = NULL;	
    if (dma_inited) {
        return;
    }
		
		/* Unlock register lock protect */
		SYS_UnlockReg();                   

		pdma = hal_dma_modbase();

    dma_chn_mask = ~NU_PDMA_CH_Msk;
    memset(dma_chn_arr, 0x00, sizeof (dma_chn_arr));

    // Reset this module
    SYS_ResetModule(dma_modinit.rsetidx);

    // Enable IP clock
    CLK_EnableModuleClock(dma_modinit.clkidx);

		PDMA_Open(pdma, 0);
		
    //NVIC_SetVector(dma_modinit.irq_n, (uint32_t) dma_modinit.var);
    NVIC_EnableIRQ(dma_modinit.irq_n);

		dma_inited = 1;
}

int hal_dma_channel_allocate(uint32_t capabilities)
{		
		PDMA_T * pdma = NULL;	
		hal_dma_init();
   	
		pdma = hal_dma_modbase();
		
    int i = nu_cto(dma_chn_mask);
    if (i != 32) {
        dma_chn_mask |= 1 << i;
        memset(dma_chn_arr + i - NU_PDMA_CH_Pos, 0x00, sizeof (struct nu_dma_chn_s));
        return i;
    }

    // No channel available
		return HAL_ERROR;
}

int hal_dma_channel_free(int channelid)
{
    if (! dma_inited) {
        return 0;
    }

    if ( channelid != HAL_ERROR ) {
        dma_chn_mask &= ~(1 << channelid);
    }
		
    return 0;
}

int hal_dma_set_handler(int channelid, uint32_t handler, uint32_t id, uint32_t event)
{
		PDMA_T * pdma;
	
		if ( !(dma_chn_mask & (1 << channelid)) )
			goto exit_hal_dma_set_handler;
		
		pdma = hal_dma_modbase();
		
    dma_chn_arr[channelid - NU_PDMA_CH_Pos].handler = (void (*)(uint32_t, uint32_t)) handler;
    dma_chn_arr[channelid - NU_PDMA_CH_Pos].id = id;
    dma_chn_arr[channelid - NU_PDMA_CH_Pos].event = event;

    // Set interrupt vector if someone has removed it.
    // NVIC_SetVector(dma_modinit.irq_n, (uint32_t) dma_modinit.var);
    //NVIC_EnableIRQ(dma_modinit.irq_n);

		return HAL_OK;

exit_hal_dma_set_handler:
	
		return HAL_ERROR;
}

static void hal_dma_enable_channel (PDMA_T * pdma, int channelid )
{
    pdma->CHCTL |=  (1 << channelid); 
}

static void hal_dma_disable_channel (PDMA_T * pdma, int channelid )
{
    pdma->CHCTL &= ~(1 << channelid); 
}	

static int hal_dma_peripheral_control ( uint32_t u32Peripheral, uint32_t* u32SrcCtl, uint32_t* u32DstCtl  )
{
	int idx=0;

	while ( idx < (sizeof(sPDMAPeripheralCtl) / sizeof(sPDMAPeripheralCtl[0])) )
	{
		if ( sPDMAPeripheralCtl[idx].m_u32Peripheral == u32Peripheral )
		{                                   
			*u32SrcCtl = sPDMAPeripheralCtl[idx].m_u32SrcCtl; 
			*u32DstCtl = sPDMAPeripheralCtl[idx].m_u32DstCtl; 
			return 1;
		}
		idx++;
	}
	return 0;
}

int hal_dma_transferred_bytecount(int channelid)
{
		PDMA_T * pdma;
		int i32BitWidth=0;
	
		if ( !(dma_chn_mask & (1 << channelid)) )
			return 0;
		
		pdma = hal_dma_modbase();

		i32BitWidth = pdma->DSCT[channelid].CTL & PDMA_DSCT_CTL_TXWIDTH_Msk;
		i32BitWidth = (i32BitWidth==PDMA_WIDTH_8)?1:(i32BitWidth==PDMA_WIDTH_16)?2:(i32BitWidth==PDMA_WIDTH_32)?4:0;
		
		return i32BitWidth * (((pdma->DSCT[channelid].CTL & PDMA_DSCT_CTL_TXCNT_Msk)>>PDMA_DSCT_CTL_TXCNT_Pos));
}

int hal_dma_fill_description(int channelid, uint32_t u32Peripheral, uint32_t data_width, uint32_t addr_src, uint32_t addr_dst, int32_t length, uint32_t timeout )
{
		PDMA_T * pdma;
	
		uint32_t u32SrcCtrl, u32DstCtrl;
		uint32_t u32PdmaIntMask=0;
		if ( !(dma_chn_mask & (1 << channelid))  )
			goto exit_hal_dma_fill_description;

		if ( !hal_dma_peripheral_control ( u32Peripheral, &u32SrcCtrl, &u32DstCtrl ) )
			goto exit_hal_dma_fill_description;

		pdma = hal_dma_modbase();
	
		hal_dma_disable_channel ( pdma, channelid );
		PDMA_DisableTimeout ( pdma,	1<<channelid );

		hal_dma_enable_channel ( pdma, channelid );
		
		// void PDMA_SetTransferMode(PDMA_T * pdma,uint32_t u32Ch, uint32_t u32Peripheral, uint32_t u32ScatterEn, uint32_t u32DescAddr)
		PDMA_SetTransferMode ( 	pdma,	
														channelid,
														u32Peripheral, 
														0, 
														0);
		//	void PDMA_SetTransferCnt(PDMA_T * pdma,uint32_t u32Ch, uint32_t u32Width, uint32_t u32TransCount)
		PDMA_SetTransferCnt ( pdma, 
													channelid, 
													(data_width == 8) ? PDMA_WIDTH_8 : (data_width == 16) ? PDMA_WIDTH_16 : PDMA_WIDTH_32,
													length );
				
		// void PDMA_SetTransferAddr(PDMA_T * pdma,uint32_t u32Ch, uint32_t u32SrcAddr, uint32_t u32SrcCtrl, uint32_t u32DstAddr, uint32_t u32DstCtrl)
    PDMA_SetTransferAddr ( pdma,
													 channelid,
                           addr_src,  // NOTE:
                           // NUC472: End of source address
                           // M451: Start of source address
                           // M480: Start of source address
                           u32SrcCtrl,//hal_dma_check_fixed_addr(u32Peripheral)?PDMA_SAR_FIX:PDMA_SAR_INC,   	// check source address is incremental or fixed
                           addr_dst,  			// Destination address
                           u32DstCtrl);//hal_dma_check_fixed_addr(u32Peripheral)?PDMA_DAR_FIX:PDMA_DAR_INC);  	// check destination address is incremental or fixed

		//	void PDMA_SetBurstType(PDMA_T * pdma,uint32_t u32Ch, uint32_t u32BurstType, uint32_t u32BurstSize)
    PDMA_SetBurstType ( pdma,
												channelid,
                        PDMA_REQ_SINGLE,    // Single mode
                        0); // Burst size

    PDMA_EnableInt (  pdma,	channelid, PDMA_INT_TRANS_DONE); // Interrupt type		

		if ( timeout && channelid <= 1 ) // M480 limit
		{
			PDMA_SetTimeOut ( pdma,	channelid, 0, 0x3b00 ); //20ms in 192MHz
			PDMA_EnableTimeout ( pdma,	1<<channelid );
	    PDMA_EnableInt (  pdma, channelid, PDMA_INT_TIMEOUT); // Interrupt type		
		}
		else
		{
	    PDMA_DisableInt (  pdma, channelid, PDMA_INT_TIMEOUT); // Interrupt type		
			PDMA_DisableTimeout ( pdma,	1<<channelid );
		}
	
		return HAL_OK;

exit_hal_dma_fill_description:
	
		return HAL_ERROR;
}

//static void hal_pdma_vec(void)
void PDMA_IRQHandler(void)
{
		krhino_intrpt_enter();

		PDMA_T * pdma = hal_dma_modbase();
    uint32_t intsts = PDMA_GET_INT_STATUS ( pdma );

    // Abort
    if (intsts & PDMA_INTSTS_ABTIF_Msk) {
        uint32_t abtsts = PDMA_GET_ABORT_STS( pdma );
        // Clear all Abort flags
        PDMA_CLR_ABORT_FLAG(pdma, abtsts);

        while (abtsts) {
            int chn_id = nu_ctz(abtsts) - PDMA_ABTSTS_ABTIF0_Pos + NU_PDMA_CH_Pos;
            if (dma_chn_mask & (1 << chn_id)) {
                struct nu_dma_chn_s *dma_chn = dma_chn_arr + chn_id - NU_PDMA_CH_Pos;
                if (dma_chn->handler && (dma_chn->event & DMA_EVENT_ABORT)) {
                    dma_chn->handler(dma_chn->id, DMA_EVENT_ABORT);
                }
            }
            abtsts &= ~(1 << (chn_id - NU_PDMA_CH_Pos + PDMA_ABTSTS_ABTIF0_Pos));
        }
    }

    // Transfer done
    if (intsts & PDMA_INTSTS_TDIF_Msk) {
        uint32_t tdsts = PDMA_GET_TD_STS( pdma );
        // Clear all transfer done flags
        PDMA_CLR_TD_FLAG(pdma, tdsts);

        while (tdsts) {
            int chn_id = nu_ctz(tdsts) - PDMA_TDSTS_TDIF0_Pos + NU_PDMA_CH_Pos;
            if (dma_chn_mask & (1 << chn_id)) {
                struct nu_dma_chn_s *dma_chn = dma_chn_arr + chn_id - NU_PDMA_CH_Pos;
                if (dma_chn->handler && (dma_chn->event & DMA_EVENT_TRANSFER_DONE)) {
                    dma_chn->handler(dma_chn->id, DMA_EVENT_TRANSFER_DONE);
                }
            }
            tdsts &= ~(1 << (chn_id - NU_PDMA_CH_Pos + PDMA_TDSTS_TDIF0_Pos));
        }
    } 

		// Timeout
		uint32_t reqto = intsts & (PDMA_INTSTS_REQTOF0_Msk | PDMA_INTSTS_REQTOF1_Msk);
		if (reqto) {
				// Clear all Timeout flags
				pdma->INTSTS = reqto;
			
				while (reqto) {
						int chn_id = nu_ctz(reqto) - PDMA_INTSTS_REQTOF0_Pos + NU_PDMA_CH_Pos;
						if (dma_chn_mask & (1 << chn_id)) {
								struct nu_dma_chn_s *dma_chn = dma_chn_arr + chn_id - NU_PDMA_CH_Pos;
								if (dma_chn->handler && (dma_chn->event & DMA_EVENT_TIMEOUT)) {
										dma_chn->handler(dma_chn->id, DMA_EVENT_TIMEOUT);
								}
						}
						reqto &= ~(1 << (chn_id - NU_PDMA_CH_Pos + PDMA_INTSTS_REQTOF0_Pos));
				}
		}

		krhino_intrpt_exit();		
}

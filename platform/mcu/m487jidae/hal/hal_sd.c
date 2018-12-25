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

#include "aos/hal/sd.h"
#include "board.h"


struct nu_sdh_var {
    uint32_t    ref_cnt;                // Reference count of the H/W module
    struct sdh_s *  obj;
};

static struct nu_sdh_var sdh0_var = {
    .ref_cnt            =   0,
    .obj                =   NULL,
};

static struct nu_sdh_var sdh1_var = {
    .ref_cnt            =   0,
    .obj                =   NULL,
};

/*  LINK TO SDH DRIVER */
extern int SDH_ok;
extern SDH_INFO_T SD0, SD1;

static const struct nu_modinit_s sdh_modinit_tab[] = {
    {SDH_0, SDH0_MODULE, CLK_CLKSEL0_SDH0SEL_HCLK, CLK_CLKDIV0_SDH0(2), SDH0_RST, SDH0_IRQn, &sdh0_var},
    {SDH_1, SDH1_MODULE, CLK_CLKSEL0_SDH1SEL_HCLK, CLK_CLKDIV3_SDH1(2), SDH1_RST, SDH1_IRQn, &sdh1_var},
    {NC, 0, 0, 0, 0, (IRQn_Type) 0, NULL}
};

static uint32_t sdh_modinit_mask = 0;

void sdhx_irqhander (struct nu_sdh_var* psNuSdhVar)
{
		struct sdh_s *obj = psNuSdhVar->obj;	
    SDH_T *sdh_base = (SDH_T *) NU_MODBASE(obj->sdh);
	
    // FMI data abort interrupt
    if (sdh_base->GINTSTS & SDH_GINTSTS_DTAIF_Msk) {
        //sdh_base->GINTSTS = SDH_GINTSTS_DTAIF_Msk;
        /* ResetAllEngine() */
        sdh_base->GCTL |= SDH_GCTL_GCTLRST_Msk;
    }

    //----- SD interrupt status
    if (sdh_base->INTSTS & SDH_INTSTS_BLKDIF_Msk) {
        // block down
        extern uint8_t volatile g_u8SDDataReadyFlag;
        g_u8SDDataReadyFlag = TRUE;
        sdh_base->INTSTS = SDH_INTSTS_BLKDIF_Msk;
    }
    
    // NOTE: On M487, there are two SDH instances which each support port 0 and don't support port 1.
    //       Port 0 (support): INTEN.CDIEN0, INTEN.CDSRC0, INTSTS.CDIF0, INTSTS.CDSTS0
    //       Port 1 (no support): INTEN.CDIEN1, INTEN.CDSRC1, INTSTS.CDIF1, INTSTS.CDSTS1
    if (sdh_base->INTSTS & SDH_INTSTS_CDIF_Msk) { // port 0 card detect
        sdh_base->INTSTS = SDH_INTSTS_CDIF_Msk;
        // TBD: Support PnP
    }

    // CRC error interrupt
    if (sdh_base->INTSTS & SDH_INTSTS_CRCIF_Msk) {
        sdh_base->INTSTS = SDH_INTSTS_CRCIF_Msk;      // clear interrupt flag
    }

    if (sdh_base->INTSTS & SDH_INTSTS_DITOIF_Msk) {
        sdh_base->INTSTS = SDH_INTSTS_DITOIF_Msk;
    }

    // Response in timeout interrupt
    if (sdh_base->INTSTS & SDH_INTSTS_RTOIF_Msk) {
        sdh_base->INTSTS |= SDH_INTSTS_RTOIF_Msk;
    }
}

void SDH0_IRQHandler (void) { sdhx_irqhander(&sdh0_var); }

void SDH1_IRQHandler (void) { sdhx_irqhander(&sdh1_var); }

static uint32_t platform_sdh_get_sector_num ( SDHName s_SdhName )
{
  switch (NU_MODINDEX(s_SdhName)) 
	{
    case 0:
        return SD0.totalSectorN;
    case 1:
        return SD1.totalSectorN;
   }
	return 0;
}

static uint32_t platform_sdh_get_sector_size ( SDHName s_SdhName )
{
  switch (NU_MODINDEX(s_SdhName)) 
	{
    case 0:
        return SD0.sectorSize;
    case 1:
        return SD1.sectorSize;
   }
	return 0;
}

static uint32_t platform_sdh_get_isinitialized( SDHName s_SdhName )
{
  switch (NU_MODINDEX(s_SdhName)) 
	{
		case 0:
      return (SDH_ok && (SD0.CardType != SDH_TYPE_UNKNOWN));
    case 1:
			return (SDH_ok && (SD1.CardType != SDH_TYPE_UNKNOWN));
  }
	return 0;
}

static struct sdh_s* hal_get_sdh_s ( sd_dev_t *sd )
{
	if ( !(sd) || (sd->port >= i32BoardMaxSDHNum) )
		goto exit_hal_get_sdh_s;

	return (struct sdh_s*)&board_sdh[sd->port];

exit_hal_get_sdh_s:
	return NULL;
}

/**
 * Initialises a sd interface
 *
 * @param[in]  sd  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_sd_init(sd_dev_t *sd)
{
		struct nu_modinit_s *modinit;
		struct sdh_s* psdh_s;
		struct nu_sdh_var *var;
		SDHName s_SdhName;
		SDH_T* pSdh;
		uint32_t	is_initialized = 0 ;
	
		if ( !(psdh_s=hal_get_sdh_s ( sd )) )
			goto exit_hal_sd_init;
	
	   // Check if all pins belong to the same SD module
    // Merge SD DAT0/1/2/3
    uint32_t sd_dat0_mod = pinmap_peripheral(psdh_s->pin_dat0, PinMap_SDH_DAT0);
    uint32_t sd_dat1_mod = pinmap_peripheral(psdh_s->pin_dat1, PinMap_SDH_DAT1);
    uint32_t sd_dat2_mod = pinmap_peripheral(psdh_s->pin_dat2, PinMap_SDH_DAT2);
    uint32_t sd_dat3_mod = pinmap_peripheral(psdh_s->pin_dat3, PinMap_SDH_DAT3);
    uint32_t sd_dat01_mod = (SDHName) pinmap_merge(sd_dat0_mod, sd_dat1_mod);
    uint32_t sd_dat23_mod = (SDHName) pinmap_merge(sd_dat2_mod, sd_dat3_mod);
    uint32_t sd_dat0123_mod = (SDHName) pinmap_merge(sd_dat01_mod, sd_dat23_mod);
    // Merge SD CMD/CLK/CDn
    uint32_t sd_cmd_mod = pinmap_peripheral(psdh_s->pin_cmd, PinMap_SDH_CMD);
    uint32_t sd_clk_mod = pinmap_peripheral(psdh_s->pin_clk, PinMap_SDH_CLK);
    uint32_t sd_cdn_mod = pinmap_peripheral(psdh_s->pin_cdn, PinMap_SDH_CD);
    uint32_t sd_cmdclk_mod = (SDHName) pinmap_merge(sd_cmd_mod, sd_clk_mod);
    uint32_t sd_cmdclkcdn_mod = (SDHName) pinmap_merge(sd_cmdclk_mod, sd_cdn_mod);
    // Merge SD DAT0/1/2/3 and SD CMD/CLK/CDn
    s_SdhName = (SDHName) pinmap_merge(sd_dat0123_mod, sd_cmdclkcdn_mod);
    
    if (s_SdhName == (uint32_t) NC) 
			goto exit_hal_sd_init;
    
    	// Find entry by SDName
		if ( !(modinit = get_modinit(s_SdhName, sdh_modinit_tab)) ) 
			goto exit_hal_sd_init;
		
		var = (struct nu_sdh_var *) modinit->var;
		if (! var->ref_cnt) {
    
			   do {
            // Reset this module
            SYS_ResetModule(modinit->rsetidx);

            // Select IP clock source
            CLK_SetModuleClock(modinit->clkidx, modinit->clksrc, modinit->clkdiv);
            
            // Enable IP clock
            CLK_EnableModuleClock(modinit->clkidx);

						// Configure SD multi-function pins
						pinmap_pinout ( psdh_s->pin_dat0, 	PinMap_SDH_DAT0 );
						pinmap_pinout ( psdh_s->pin_dat1, 	PinMap_SDH_DAT1 );
						pinmap_pinout ( psdh_s->pin_dat2, 	PinMap_SDH_DAT2 );
						pinmap_pinout ( psdh_s->pin_dat3, 	PinMap_SDH_DAT3 );
						pinmap_pinout ( psdh_s->pin_cmd, 	PinMap_SDH_CMD );
						pinmap_pinout ( psdh_s->pin_clk, 	PinMap_SDH_CLK );
						pinmap_pinout ( psdh_s->pin_cdn, 	PinMap_SDH_CD );

        } while (0);
    
				/* Link parent and children. */
				var->obj = psdh_s;
				
        NVIC_EnableIRQ(modinit->irq_n);

				// Get SDH base address
				pSdh = (SDH_T *) NU_MODBASE(s_SdhName);				
				SDH_Open(pSdh, CardDetect_From_GPIO);
        SDH_Probe(pSdh);

        is_initialized = platform_sdh_get_isinitialized(s_SdhName);				
				if ( !is_initialized )
					goto exit_hal_sd_init;

    } //if (! var->ref_cnt)
   
    var->ref_cnt ++;
    
    if (var->ref_cnt) {
        // Mark this module to be inited.
        int i = modinit - sdh_modinit_tab;
        sdh_modinit_mask |= 1 << i;
    }

		return HAL_OK;

exit_hal_sd_init:

	return HAL_ERROR;
}

/**
 * Read sd blocks
 *
 * @param[in]   sd        the interface which should be initialised
 * @param[out]  data      pointer to the buffer which will store incoming data
 * @param[in]   blk_addr  sd blk addr
 * @param[in]   blks      sd blks
 * @param[in]   timeout   timeout in milisecond
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_sd_blks_read(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr,
                         uint32_t blks, uint32_t timeout)
{
	struct nu_modinit_s *modinit;
	struct sdh_s* psdh_s;
	struct nu_sdh_var *var;
	SDH_T* pSdh;
	uint32_t	is_initialized = 0 ;
	
	if ( !(psdh_s=hal_get_sdh_s ( sd )) || !data )
		goto exit_hal_sd_blks_read;
	
	modinit = get_modinit(psdh_s->sdh, sdh_modinit_tab);
	/* Valid? */
	if ( !modinit ) goto exit_hal_sd_blks_read;
	
	var = modinit->var;	
	/* Initialized? */
	if ( !var->ref_cnt ) goto exit_hal_sd_blks_read;

	psdh_s = var->obj;
	if ( !psdh_s ) goto exit_hal_sd_blks_read;
	
	if ( !platform_sdh_get_isinitialized(psdh_s->sdh) )
		goto exit_hal_sd_blks_read;
	
  if ( SDH_Read(pSdh, data, blk_addr, blks) != 0 )
		goto exit_hal_sd_blks_read;
	
	return HAL_OK;

exit_hal_sd_blks_read:
	return HAL_ERROR;
	
}


/**
 * Write sd blocks
 *
 * @param[in]  sd        the interface which should be initialised
 * @param[in]  data      pointer to the buffer which will store incoming data
 * @param[in]  blk_addr  sd blk addr
 * @param[in]  blks      sd blks
 * @param[in]  timeout   timeout in milisecond
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_sd_blks_write(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr,
                          uint32_t blks, uint32_t timeout)
{
	struct nu_modinit_s *modinit;
	struct sdh_s* psdh_s;
	struct nu_sdh_var *var;
	SDH_T* pSdh;
	uint32_t	is_initialized = 0 ;
	
	if ( !(psdh_s=hal_get_sdh_s ( sd )) || !data )
		goto exit_hal_sd_blks_write;
	
	modinit = get_modinit(psdh_s->sdh, sdh_modinit_tab);
	/* Valid? */
	if ( !modinit ) goto exit_hal_sd_blks_write;
	
	var = modinit->var;	
	/* Initialized? */
	if ( !var->ref_cnt ) goto exit_hal_sd_blks_write;

	psdh_s = var->obj;
	if ( !psdh_s ) goto exit_hal_sd_blks_write;
	
	if ( !platform_sdh_get_isinitialized(psdh_s->sdh) )
		goto exit_hal_sd_blks_write;
	
  if ( SDH_Write(pSdh, data, blk_addr, blks) != 0 )
		goto exit_hal_sd_blks_write;

	return HAL_OK;

exit_hal_sd_blks_write:
	return HAL_ERROR;
}

/**
 * Erase sd blocks
 *
 * @param[in]  sd              the interface which should be initialised
 * @param[in]  blk_start_addr  sd blocks start addr
 * @param[in]  blk_end_addr    sd blocks end addr
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_sd_erase(sd_dev_t *sd, uint32_t blk_start_addr, uint32_t blk_end_addr)
{
	return HAL_OK;
}

/**
 * Get sd state
 *
 * @param[in]   sd    the interface which should be initialised
 * @param[out]  stat  pointer to the buffer which will store incoming data
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_sd_stat_get(sd_dev_t *sd, hal_sd_stat *stat)
{
	return HAL_OK;
}

/**
 * Get sd info
 *
 * @param[in]   sd    the interface which should be initialised
 * @param[out]  stat  pointer to the buffer which will store incoming data
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_sd_info_get(sd_dev_t *sd, hal_sd_info_t *info)
{
	struct nu_modinit_s *modinit;
	struct sdh_s* psdh_s;
	struct nu_sdh_var *var;
	SDH_T* pSdh;
	uint32_t	is_initialized = 0 ;
	
	if ( !(psdh_s=hal_get_sdh_s ( sd )) || !info )
		goto exit_hal_sd_info_get;
	
	modinit = get_modinit(psdh_s->sdh, sdh_modinit_tab);
	/* Valid? */
	if ( !modinit ) goto exit_hal_sd_info_get;
	
	var = modinit->var;	
	/* Initialized? */
	if ( !var->ref_cnt ) goto exit_hal_sd_info_get;

	psdh_s = var->obj;
	if ( !psdh_s ) goto exit_hal_sd_info_get;
	
	if ( !platform_sdh_get_isinitialized(psdh_s->sdh) )
		goto exit_hal_sd_info_get;
	
	info->blk_nums = platform_sdh_get_sector_num ( psdh_s->sdh );
	info->blk_size = platform_sdh_get_sector_size ( psdh_s->sdh );
	
	return HAL_OK;
	
exit_hal_sd_info_get:
	return HAL_ERROR;
}

/**
 * Deinitialises a sd interface
 *
 * @param[in]  sd  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_sd_finalize(sd_dev_t *sd)
{
	struct sdh_s* psdh_s;
	struct nu_modinit_s *modinit;
	struct nu_sdh_var *var;
	SDH_T* pSdh;

  if ( !sd )
		goto exit_hal_sd_finalize;

	psdh_s = hal_get_sdh_s ( sd );
	if ( !psdh_s )
		goto exit_hal_sd_finalize;

	modinit = get_modinit(psdh_s->sdh, sdh_modinit_tab);
	/* Valid? */
	if ( !modinit ) goto exit_hal_sd_finalize;

	var = (struct nu_sdh_var *)modinit->var;
	/* Initialized? */
	if ( !var->ref_cnt ) goto exit_hal_sd_finalize;

	pSdh = (SDH_T *) NU_MODBASE(psdh_s->sdh);
	
	var->ref_cnt --;
	
	if (! var->ref_cnt) {
		
		do {

					SDH_DISABLE_INT(pSdh, (UART_INTEN_RDAIEN_Msk | UART_INTEN_THREIEN_Msk | UART_INTEN_RXTOIEN_Msk));
					NVIC_DisableIRQ(modinit->irq_n);

					// Disable IP clock
					CLK_DisableModuleClock(modinit->clkidx);
		} while (0);
	
		/* Unlink parent and children. */
		var->obj = NULL ;
	}

	if (! var->ref_cnt) {
		// Mark this module to be deinited.
		int i = modinit - sdh_modinit_tab;
		sdh_modinit_mask &= ~(1 << i);
	}

	return HAL_OK;
    
exit_hal_sd_finalize:

	return HAL_ERROR;
}


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
#include "aos/hal/spi.h"
#include "board.h"

#define NU_SPI_FRAME_MIN    8
#define NU_SPI_FRAME_MAX    32

struct nu_spi_var {
		struct spi_s* obj;
		uint32_t			ref_cnt;
		kmutex_t 			port_mutex;
	
#if DEVICE_SPI_ASYNCH
    uint8_t     pdma_perp_tx;
    uint8_t     pdma_perp_rx;
#endif
};

static struct nu_spi_var spi0_var = {
#if DEVICE_SPI_ASYNCH
    .pdma_perp_tx       =   PDMA_SPI0_TX,
    .pdma_perp_rx       =   PDMA_SPI0_RX
#endif
};
static struct nu_spi_var spi1_var = {
#if DEVICE_SPI_ASYNCH
    .pdma_perp_tx       =   PDMA_SPI1_TX,
    .pdma_perp_rx       =   PDMA_SPI1_RX
#endif
};
static struct nu_spi_var spi2_var = {
#if DEVICE_SPI_ASYNCH
    .pdma_perp_tx       =   PDMA_SPI2_TX,
    .pdma_perp_rx       =   PDMA_SPI2_RX
#endif
};
static struct nu_spi_var spi3_var = {
#if DEVICE_SPI_ASYNCH
    .pdma_perp_tx       =   PDMA_SPI3_TX,
    .pdma_perp_rx       =   PDMA_SPI3_RX
#endif
};

static uint32_t spi_modinit_mask = 0;

static const struct nu_modinit_s spi_modinit_tab[] = {
    {SPI_0, SPI0_MODULE, CLK_CLKSEL2_SPI0SEL_PCLK1, MODULE_NoMsk, SPI0_RST, SPI0_IRQn, &spi0_var},
    {SPI_1, SPI1_MODULE, CLK_CLKSEL2_SPI1SEL_PCLK0, MODULE_NoMsk, SPI1_RST, SPI1_IRQn, &spi1_var},
    {SPI_2, SPI2_MODULE, CLK_CLKSEL2_SPI2SEL_PCLK1, MODULE_NoMsk, SPI2_RST, SPI2_IRQn, &spi2_var},
    {SPI_3, SPI3_MODULE, CLK_CLKSEL2_SPI3SEL_PCLK0, MODULE_NoMsk, SPI3_RST, SPI3_IRQn, &spi3_var},
		
    {NC, 0, 0, 0, 0, (IRQn_Type) 0, NULL}
};

/* Synchronous version of SPI_ENABLE()/SPI_DISABLE() macros
 *
 * The SPI peripheral clock is asynchronous with the system clock. In order to make sure the SPI
 * control logic is enabled/disabled, this bit indicates the real status of SPI controller.
 *
 * NOTE: All configurations shall be ready before calling SPI_ENABLE_SYNC().
 * NOTE: Before changing the configurations of SPIx_CTL, SPIx_CLKDIV, SPIx_SSCTL and SPIx_FIFOCTL registers,
 *       user shall clear the SPIEN (SPIx_CTL[0]) and confirm the SPIENSTS (SPIx_STATUS[15]) is 0
 *       (by SPI_DISABLE_SYNC here).
 */
__STATIC_INLINE void SPI_ENABLE_SYNC(SPI_T *spi_base)
{
    if (! (spi_base->CTL & SPI_CTL_SPIEN_Msk)) {
        SPI_ENABLE(spi_base);
    }
    while (! (spi_base->STATUS & SPI_STATUS_SPIENSTS_Msk));
}
__STATIC_INLINE void SPI_DISABLE_SYNC(SPI_T *spi_base)
{
    if (spi_base->CTL & SPI_CTL_SPIEN_Msk) {
        // NOTE: SPI H/W may get out of state without the busy check.
        while (SPI_IS_BUSY(spi_base));
    
        SPI_DISABLE(spi_base);
    }
    while (spi_base->STATUS & SPI_STATUS_SPIENSTS_Msk);
}

static int spi_writeable(SPI_T *spi_base)
{
    // Receive FIFO must not be full to avoid receive FIFO overflow on next transmit/receive
    return ! SPI_GET_TX_FIFO_FULL_FLAG(spi_base);
}

static int spi_readable(SPI_T *spi_base)
{
    return ! SPI_GET_RX_FIFO_EMPTY_FLAG(spi_base);
}

static int platform_spi_slave_receive(struct spi_s *obj)
{
    SPI_T *spi_base = (SPI_T *) NU_MODBASE(obj->spi);

    SPI_ENABLE_SYNC(spi_base);

    return spi_readable(obj);
};

static int platform_spi_slave_read(struct spi_s *obj)
{
    SPI_T *spi_base = (SPI_T *) NU_MODBASE(obj->spi);

    SPI_ENABLE_SYNC(spi_base);

    // Wait for rx buffer avaiable
    while (! spi_readable(obj));
    int value = SPI_READ_RX(spi_base);
    return value;
}

static void platform_spi_slave_write(struct spi_s *obj, int value)
{
    SPI_T *spi_base = (SPI_T *) NU_MODBASE(obj->spi);

    SPI_ENABLE_SYNC(spi_base);

    // Wait for tx buffer avaiable
    while(! spi_writeable(obj));
    SPI_WRITE_TX(spi_base, value);
}

static void platform_spi_master_sync(struct spi_s *obj, int* tx_value, int* rx_value)
{
    SPI_T *spi_base = (SPI_T *) NU_MODBASE(obj->spi);

    // NOTE: Data in receive FIFO can be read out via ICE.
    SPI_ENABLE_SYNC(spi_base);

		if ( tx_value )
		{
			// Wait for tx buffer empty
			while(! spi_writeable(spi_base));
			SPI_WRITE_TX(spi_base, *tx_value);
		}
		
		if ( rx_value )
		{
			// Wait for rx buffer full
			while (! spi_readable(spi_base));
			*rx_value = SPI_READ_RX(spi_base);
		}
}

static int platform_spi_master_block_write(struct spi_s *obj, const char *tx_buffer, int tx_length,
                           char *rx_buffer, int rx_length, char write_fill) 
{
		int32_t tx, rx;
    int total = (tx_length > rx_length) ? tx_length : rx_length;

    for (int i = 0; i < total; i++) {
				if ( tx_buffer && i < tx_length )
					tx = tx_buffer[i];
				else
					tx = write_fill;
				
				platform_spi_master_sync(obj, &tx, &rx);
			
				if ( rx_buffer && i < rx_length )
            rx_buffer[i] = (char)rx;				
    }

    return total;
}


static struct serial_s* hal_get_spi_s ( spi_dev_t *spi )
{
	if ( !(spi) || (spi->port >= i32BoardMaxSPINum) )
		goto exit_hal_get_spi_s;

	return (struct spi_s*)&board_spi[spi->port];

exit_hal_get_spi_s:
	return NULL;
}

static void platform_spi_frequency ( struct spi_s *obj, int hz )
{
    SPI_T *spi_base = (SPI_T *) NU_MODBASE(obj->spi);

    SPI_DISABLE_SYNC(spi_base);

    SPI_SetBusClock((SPI_T *) NU_MODBASE(obj->spi), hz);
}


static void platform_spi_format ( struct spi_s *obj, int bits, int mode, int slave )
{
  SPI_T *spi_base = (SPI_T *) NU_MODBASE(obj->spi);

  SPI_DISABLE_SYNC(spi_base);

  SPI_Open(		spi_base,
							slave ? SPI_SLAVE : SPI_MASTER,
							(mode == 0) ? SPI_MODE_0 : (mode == 1) ? SPI_MODE_1 : (mode == 2) ? SPI_MODE_2 : SPI_MODE_3,
							bits,
							SPI_GetBusClock(spi_base));
	
  // NOTE: Hardcode to be MSB first.
  SPI_SET_MSB_FIRST(spi_base);

  if (! slave) {
      // Master
      if (obj->pin_ssel != NC) {
         // Configure SS as low active.
         SPI_EnableAutoSS(spi_base, SPI_SS, SPI_SS_ACTIVE_LOW);
      } else {
         SPI_DisableAutoSS(spi_base);
      }
  } else {
      // Slave
      // Configure SS as low active.
      spi_base->SSCTL &= ~SPI_SSCTL_SSACTPOL_Msk;
  }
	
exit_platform_spi_format:
	return;
}

/**
 * Initialises the SPI interface for a given SPI device
 *
 * @param[in]  spi  the spi device
 *
 * @return  0 : on success, EIO : if the SPI device could not be initialised
 */
int32_t hal_spi_init(spi_dev_t *spi)
{
	struct nu_modinit_s *modinit;
  struct nu_spi_var *var;	
	struct spi_s* pspi_s = NULL;
	SPIName s_SpiName;
	SPI_T* pSpi;

	if ( ! (pspi_s = hal_get_spi_s ( spi )) )
		goto exit_hal_spi_init;

	if ( !((spi->config.mode == HAL_SPI_MODE_MASTER)  || (spi->config.mode == HAL_SPI_MODE_SLAVE))  )
		goto exit_hal_spi_init;

	// Determine which SPI_x the pins are used for
  uint32_t spi_mosi = pinmap_peripheral(pspi_s->pin_mosi, PinMap_SPI_MOSI);
  uint32_t spi_miso = pinmap_peripheral(pspi_s->pin_miso, PinMap_SPI_MISO);
  uint32_t spi_sclk = pinmap_peripheral(pspi_s->pin_sclk, PinMap_SPI_SCLK);
	uint32_t spi_ssel = pinmap_peripheral(pspi_s->pin_ssel, PinMap_SPI_SSEL);
  uint32_t spi_data = pinmap_merge(spi_mosi, spi_miso);
  uint32_t spi_cntl = pinmap_merge(spi_sclk, spi_ssel);
  s_SpiName = (SPIName) pinmap_merge(spi_data, spi_cntl);
  if ( (int) s_SpiName == NC )
		goto exit_hal_spi_init;
	
	// Find entry by UARTNAME
  if ( !(modinit = get_modinit(s_SpiName, spi_modinit_tab)) ) 
  	goto exit_hal_spi_init;

	var = (struct nu_spi_var *) modinit->var;

	if (! var->ref_cnt) {

        do {
            // Reset this module
            SYS_ResetModule(modinit->rsetidx);

            // Select IP clock source
            CLK_SetModuleClock(modinit->clkidx, modinit->clksrc, modinit->clkdiv);
            
            // Enable IP clock
            CLK_EnableModuleClock(modinit->clkidx);

						pinmap_pinout(pspi_s->pin_mosi, PinMap_SPI_MOSI);
						pinmap_pinout(pspi_s->pin_miso, PinMap_SPI_MISO);
						pinmap_pinout(pspi_s->pin_sclk, PinMap_SPI_SCLK);
						pinmap_pinout(pspi_s->pin_ssel, PinMap_SPI_SSEL);

        } while (0);

				if ( krhino_mutex_create(&var->port_mutex, "spiMutex") != RHINO_SUCCESS )
					goto exit_hal_spi_init;
								
				platform_spi_frequency ( pspi_s, spi->config.freq );			
				platform_spi_format ( pspi_s, 
															8, 	//Todo: FIXME. AOS SPI is without the paramter.
															0,	//Todo: FIXME. AOS SPI is without the paramter.
															(spi->config.mode==HAL_SPI_MODE_SLAVE)? 1: 0 );	//Todo: FIXME. AOS SPI is without the paramter.
								
				/* Link parent and children. */
				var->obj = pspi_s;

    } //if (! var->ref_cnt)
   
    var->ref_cnt ++;
    
    if (var->ref_cnt) {
        // Mark this module to be inited.
        int i = modinit - spi_modinit_tab;
        spi_modinit_mask |= 1 << i;
    }

	return HAL_OK;
	
exit_hal_spi_init:
	return HAL_ERROR;
}

/**
 * Spi send
 *
 * @param[in]  spi      the spi device
 * @param[in]  data     spi send data
 * @param[in]  size     spi send data size
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0 : on success, EIO : if the SPI device could not be initialised
 */
int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
	struct nu_modinit_s *modinit;
  struct nu_spi_var *var;	
	struct spi_s* pspi_s = NULL;
	SPI_T* pSpi;
	kstat_t stat = RHINO_SUCCESS;

	if ( ! (pspi_s = hal_get_spi_s ( spi )) )
		goto exit_hal_spi_send;

	modinit = get_modinit(pspi_s->spi, spi_modinit_tab);
	/* Valid? */
	if ( !modinit ) goto exit_hal_spi_send;
	
	var = modinit->var;	
	/* Initialized? */
	if ( !var->ref_cnt ) goto exit_hal_spi_send;

  if (timeout == 0)
      timeout = 0xffffffff;

	pspi_s = var->obj;
	pSpi = (SPI_T *) NU_MODBASE(pspi_s->spi);
		
  /* Wait for Lock */
  stat = krhino_mutex_lock(&var->port_mutex, timeout);
  if (stat != RHINO_SUCCESS)
    goto exit_hal_spi_send;	

//	if ( spi->config.mode==HAL_SPI_MODE_MASTER )
		platform_spi_master_block_write ( pspi_s, data, size, NULL, 0, 0xFF );
//	else	
//			;
	
	/* Unlock before exiting. */
  stat = krhino_mutex_unlock(&var->port_mutex);
  if (stat != RHINO_SUCCESS)
     goto exit_hal_spi_send;	
	
	
	return HAL_OK;

exit_hal_spi_send:

	return HAL_ERROR;
}

/**
 * spi_recv
 *
 * @param[in]   spi      the spi device
 * @param[out]  data     spi recv data
 * @param[in]   size     spi recv data size
 * @param[in]  timeout   timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0 : on success, EIO : if the SPI device could not be initialised
 */
int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
{
	struct nu_modinit_s *modinit;
  struct nu_spi_var *var;	
	struct spi_s* pspi_s = NULL;
	SPI_T* pSpi;
	kstat_t stat = RHINO_SUCCESS;

	if ( ! (pspi_s = hal_get_spi_s ( spi )) )
		goto exit_hal_spi_recv;

	modinit = get_modinit(pspi_s->spi, spi_modinit_tab);
	/* Valid? */
	if ( !modinit ) goto exit_hal_spi_recv;
	
	var = modinit->var;	
	/* Initialized? */
	if ( !var->ref_cnt ) goto exit_hal_spi_recv;

  if (timeout == 0)
      timeout = 0xffffffff;

	pspi_s = var->obj;
	pSpi = (SPI_T *) NU_MODBASE(pspi_s->spi);
		
  /* Wait for Lock */
  stat = krhino_mutex_lock(&var->port_mutex, timeout);
  if (stat != RHINO_SUCCESS)
    goto exit_hal_spi_recv;	

//	if ( spi->config.mode==HAL_SPI_MODE_MASTER )
		platform_spi_master_block_write ( pspi_s, NULL, 0, data, size, 0xFF );
//	else	
//			;

	
	/* Unlock before exiting. */
  stat = krhino_mutex_unlock(&var->port_mutex);
  if (stat != RHINO_SUCCESS)
     goto exit_hal_spi_recv;	
	
	
	return HAL_OK;

exit_hal_spi_recv:

	return HAL_ERROR;
}

/**
 * spi send data and recv
 *
 * @param[in]  spi      the spi device
 * @param[in]  tx_data  spi send data
 * @param[in]  rx_data  spi recv data
 * @param[in]  size     spi data to be sent and recived
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0, on success;  EIO : if the SPI device could not be initialised
 */
int32_t hal_spi_send_recv(spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data,
                          uint16_t size, uint32_t timeout)
{
	struct nu_modinit_s *modinit;
  struct nu_spi_var *var;	
	struct spi_s* pspi_s = NULL;
	SPI_T* pSpi;
	kstat_t stat = RHINO_SUCCESS;

	if ( ! (pspi_s = hal_get_spi_s ( spi )) )
		goto exit_hal_spi_send_recv;

	modinit = get_modinit(pspi_s->spi, spi_modinit_tab);
	/* Valid? */
	if ( !modinit ) goto exit_hal_spi_send_recv;
	
	var = modinit->var;	
	/* Initialized? */
	if ( !var->ref_cnt ) goto exit_hal_spi_send_recv;

  if (timeout == 0)
      timeout = 0xffffffff;

	pspi_s = var->obj;
	pSpi = (SPI_T *) NU_MODBASE(pspi_s->spi);
		
  /* Wait for Lock */
  stat = krhino_mutex_lock(&var->port_mutex, timeout);
  if (stat != RHINO_SUCCESS)
    goto exit_hal_spi_send_recv;	

//	if ( spi->config.mode==HAL_SPI_MODE_MASTER )
		platform_spi_master_block_write ( pspi_s, tx_data, size, rx_data, size, 0xFF );
//	else	
//			;
	
	/* Unlock before exiting. */
  stat = krhino_mutex_unlock(&var->port_mutex);
  if (stat != RHINO_SUCCESS)
     goto exit_hal_spi_send_recv;	
	
	
	return HAL_OK;

exit_hal_spi_send_recv:

	return HAL_ERROR;
}

/**
 * De-initialises a SPI interface
 *
 *
 * @param[in]  spi the SPI device to be de-initialised
 *
 * @return  0 : on success, EIO : if an error occurred
 */
int32_t hal_spi_finalize(spi_dev_t *spi)
{
	struct spi_s* pspi_s;
	struct nu_modinit_s *modinit;
	struct nu_spi_var *var;
	SPI_T* pSpi;

  if ( !spi )
		goto exit_hal_spi_finalize;

	pspi_s = hal_get_spi_s ( spi );
	if ( !pspi_s )
		goto exit_hal_spi_finalize;

	modinit = get_modinit(pspi_s->spi, spi_modinit_tab);
	/* Valid? */
	if ( !modinit ) goto exit_hal_spi_finalize;

	var = (struct nu_spi_var *)modinit->var;
	/* Initialized? */
	if ( !var->ref_cnt ) goto exit_hal_spi_finalize;

	pSpi = (SPI_T *) NU_MODBASE(pspi_s->spi);
	
	var->ref_cnt --;
	
	if (! var->ref_cnt) {
		
		do {
					SPI_Close(pSpi);

					//NVIC_DisableIRQ(modinit->irq_n);

					// Disable IP clock
					CLK_DisableModuleClock(modinit->clkidx);
		} while (0);
		
		/* Unlink parent and children. */
		var->obj = NULL ;
	}

	if (! var->ref_cnt) {
		// Mark this module to be deinited.
		int i = modinit - spi_modinit_tab;
		spi_modinit_mask &= ~(1 << i);
	}

	return HAL_OK;
    
exit_hal_spi_finalize:

	return HAL_ERROR;
}

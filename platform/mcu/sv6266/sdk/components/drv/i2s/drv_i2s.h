#ifndef _DRV_I2S_H_
#define _DRV_I2S_H_
#include "stdint.h"
#include "hal_i2s.h"
#include "osal.h"

typedef enum {
    DRV_I2S_STATUS_INVALID_PARAMETER  = -2,  
    DRV_I2S_STATUS_ERROR              = -1,  
    DRV_I2S_STATUS_OK                 =  0   
} i2s_status_t;

typedef enum {  
    DRV_I2S_PCM              = 0,  
    DRV_I2S_I2S              = 1   
} i2s_data_format_t;

typedef enum {  
    DRV_I2S_PING              = 0,  
    DRV_I2S_PONG              = 1   
} i2s_buffer_t;

typedef struct  _i2s_data_config{
  bool ping_buf_full;
  bool pong_buf_full;
  bool dma_done;
  uint8_t *ping_buffer;
  uint8_t *pong_buffer;
  uint8_t *conv_buffer;
  uint32_t size;
  uint32_t conv_data_length;
  uint32_t ping_data_length;
  uint32_t pong_data_length;
  OsBufQ bufferQ_len;
  OsBufQ bufferQ;
  OsEvent bufferQ_event;
  OsEvent dma_int_event;
  i2s_buffer_t buffQ_inf;
  i2s_buffer_t dma_inf;
} i2s_data_config;

typedef struct _i2s_setup_config{
    i2s_transfer_mode_t         transfer_mode;
    i2s_mode_t                  i2s_mode;
    i2s_data_format_t           data_format;
    bit_resolution_t            resolution;
    sample_rate_t               sample_rate;
} i2s_setup_config;


/**
 * @brief This function will initialize I2S config setting
 *
 * @param  data_format         	- DRV_I2S_PCM, DRV_I2S_I2S
 * @param  transfer_mode     	- mode_tx, mode_rx, mode_trx
 * @param  mode                 - mode_master, mode_slave
 * @param  sample_rate        	- SampleRate_08kHZ, SampleRate_32kHZ, SampleRate_44.1kHZ, SampleRate_48kHZ, SampleRate_96kHZ
 * @param  resolution           - Resolution_16bit, Resolution_24bit, Resolution_32bit
 *
 * @return
 *     - DRV_I2S_STATUS_INVALID_PARAMETER
 *     - DRV_I2S_STATUS_OK
 */
i2s_status_t drv_i2s_init(i2s_data_format_t data_format, i2s_transfer_mode_t transfer_mode, i2s_mode_t mode, sample_rate_t sample_rate, bit_resolution_t resolution);


/**
 * @brief  Enables I2S driver and create the task(TX, RX, TRX)
 *
 * @param  NONE
 *
 * @return
 *     - DRV_I2S_STATUS_ERROR
 *     - DRV_I2S_STATUS_OK
 */
i2s_status_t drv_i2s_enable(void);


/**
 * @brief  Disables I2S driver and destroy the task(TX, RX, TRX)
 *
 * @param  NONE
 *
 * @return
 *     - DRV_I2S_STATUS_ERROR
 *     - DRV_I2S_STATUS_OK
 */
i2s_status_t drv_i2s_disable(void);


/**
 * @brief  Setting DMA transmit buffer size
 *
 * @param  byte_size	transfer buffer size(TX, RX, TRX)
 * 
 * @note must setting ample size to DMA transfer for task delay(1 - 1.5ms)
 *	must 4 byte alignment & mach L & R channel
 *	will cost 4 time SRAM size for one mode(TX, RX, TRX)
 *
 * @return
 *     - DRV_I2S_STATUS_INVALID_PARAMETER
 *     - DRV_I2S_STATUS_OK
 */
i2s_status_t drv_i2s_set_transfer_size(uint32_t byte_size);


/**
 * @brief  Write data to I2S DMA transmit buffer.
 *
 * @param  data_length	    Write data length
 * @param  tx_data	        Source address to write from
 * @param  tick_to_wait     Wait timeout in RTOS ticks.
 *
 * @return
 *     - DRV_I2S_STATUS_ERROR
 *     - SUCCES_LENGTH
 */
int32_t drv_i2s_tx_write_data(uint32_t data_length, uint8_t *tx_data, uint8_t tick_to_wait)__attribute__((section(".prog_in_sram")));


/**
 * @brief Read data from I2S DMA receive buffer.
 *
 * @param  data_length	    Read data length
 * @param  rx_data	        Destination address to read into
 * @param  tick_to_wait 	Wait timeout in RTOS ticks.
 *
 * @return
 *     - DRV_I2S_STATUS_ERROR
 *     - DRV_I2S_STATUS_OK
 */
int32_t drv_i2s_rx_read_data(uint32_t data_length, uint8_t *rx_data, uint8_t tick_to_wait)__attribute__((section(".prog_in_sram")));

#endif

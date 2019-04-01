#ifndef _HAL_I2S_H_
#define _HAL_I2S_H_

#include "sys/intc.h"   // components/

typedef void (*I2S_ISR) (void);

typedef enum {
    I2S_INTR_RXDA = 0,
    I2S_INTR_RXFO = 1,    
    I2S_INTR_RXFE = 4,
    I2S_INTR_TXFO = 5
} i2s_int_status_t;


typedef enum{
	SampleRate_08kHZ = 8,
	SampleRate_32kHZ = 32,
	SampleRate_44kHZ = 44,
	SampleRate_48kHZ = 48,
	SampleRate_96kHZ = 96
} sample_rate_t;

typedef enum{
    Resolution_16bit = 16,
	Resolution_24bit = 24,
	Resolution_32bit = 32
} bit_resolution_t;

typedef enum{
	mode_master = 0,
	mode_slave	= 1
} i2s_mode_t;

typedef enum{
    mode_tx     = 0,
    mode_rx     = 1,
    mode_trx    = 2
} i2s_transfer_mode_t;


void hal_i2s_enable(void);
void hal_i2s_rx_enable(void);
void hal_i2s_tx_enable(void);
void hal_i2s_disable(void);
void hal_i2s_rx_disable(void);
void hal_i2s_tx_disable(void);
void hal_i2s_clk_enable(void);
void hal_i2s_clk_disable(void);
void hal_i2s_mst_set_clk_divider(uint16_t clk_divider);
void hal_i2s_set_intr_mask(void);
void hal_i2s_set_word_select_width(bit_resolution_t word_select_width);
void hal_i2s_set_rx_fifo_threshold(uint8_t rx_threshold);
void hal_i2s_set_tx_fifo_threshold(uint8_t tx_threshold);
void hal_i2s_dma_tx(uint8_t *tx_data, uint32_t byte_size, I2S_ISR tx_done_isr)__attribute__((section(".prog_in_sram")));
void hal_i2s_dma_rx(uint8_t *rx_data, uint32_t byte_size, I2S_ISR rx_done_isr)__attribute__((section(".prog_in_sram")));

#endif  // #ifndef _HAL_ADC_H_

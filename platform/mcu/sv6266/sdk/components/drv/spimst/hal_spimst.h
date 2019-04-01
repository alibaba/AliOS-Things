#ifndef HAL_SPIMST
#define HAL_SPIMST

#define SPI_MST_DATA_SIZE 1024

#define SPIMST_ERROR_PRINTF (0)
#define SPIMST_DEBUG_PRINTF (0)

typedef void (*SPI_MST_ISR) (void);


typedef enum {
    SPI_MST_TXE_INT         = 0,
    SPI_MST_TXO_INT         = 1,
    SPI_MST_RXU_INT         = 2,
    SPI_MST_RXO_INT         = 3,
    SPI_MST_RXF_INT         = 4
} ENUM_SPI_MST_INT_STATUS_T;

typedef enum {
    SPI_MST_TRX_BUSY        = 0,
    SPI_MST_TX_NOT_FULL     = 1,
    SPI_MST_TX_EMPTY        = 2,
    SPI_MST_RX_NOT_EMPTY    = 3,
    SPI_MST_RX_FULL         = 4,
    SPI_MST_TX_ERROR        = 5
} ENUM_SPI_MST_STATUS_T;

typedef struct _spi_data_inf {
	uint8_t *tx_data;
    uint8_t *rx_data;
	uint32_t length;
    uint32_t tx_cnt;
    uint32_t rx_cnt;
} spi_data_inf;


int8_t hal_spi_mst_set_trx_mode(uint8_t trx_mode);
int8_t hal_spi_mst_set_cpha_cpol(uint8_t cpha, uint8_t cpol);
int8_t hal_spi_mst_set_clk_divider(uint32_t clk_Hz);
int8_t hal_spi_mst_enable(void);
int8_t hal_spi_mst_disable(void);
int8_t hal_spi_mst_init(uint32_t clk_Hz, uint8_t CPHA, uint8_t CPOL);
int8_t hal_spi_mst_deinit(void);
int8_t hal_spi_mst_get_interrupt_status(uint8_t bit);
int8_t hal_spi_mst_get_status(uint8_t bit);
int8_t hal_spi_mst_transfer(uint8_t *tx_data, uint8_t *rx_data, uint32_t data_length, int8_t csn);
int8_t hal_spi_mst_dma_trx(uint8_t *tx_data, uint8_t *rx_data, uint32_t data_length, SPI_MST_ISR trx_done_isr) __attribute__((section(".prog_in_sram")));

#endif /* end of include guard */

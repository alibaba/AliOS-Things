#ifndef _LEGA_SPI_H_
#define _LEGA_SPI_H_

#define SPI0                0x0
#define SPI1                0x1
#define SPI2                0x2

#define SPIx_BASE(i)        (0x40014000 + ((i) * 0x1000))

#define SPIx_CTRL0(i)        SPIx_BASE(i) + 0x0
#define SPI_DFS16_MASK        0xF
#define SPI_FR_SHIFT        4
#define SPI_FR_MOTOROLA        0x0
#define SPI_FR_TI            0x1
#define SPI_FR_NS            0x2
#define SPI_CPHA_SHIFT        6
#define SPI_CPHA            0x1
#define SPI_CPOL_SHIFT        7
#define SPI_CPOL            0x1
#define SPI_TMOD_SHIFT        8
#define SPI_TMOD_TR            0x0
#define SPI_TMOD_T            0x1
#define SPI_TMOD_R            0x2
#define SPI_TMODE_EEPROM    0x3
#define SPI_SLV_OE_SHIFT    10
#define SPI_SRL_SHIFT        11
#define SPI_CFS_SHIFT        12
#define SPI_DFS32_MASK        0xF
#define SPI_DFS32_SHIFT        16
#define SPI_FRF_SHIFT        21
#define SPI_FRF_STANDARD    0x0
#define SPI_FRF_DUAL        0x1
#define SPI_FRF_QUAD        0x2
//
#define SPIx_CTRL1(i)        SPIx_BASE(i) + 0x4
#define SPI_NDF_MASK        0xFFFF

#define SPIx_SSIEN(i)        SPIx_BASE(i) + 0x8
#define SPI_EN                0x1
#define SPI_DISABLE            0x0

#define SPIx_MWC(i)            SPIx_BASE(i) + 0xc
#define SPI_MW_MOD            (1 << 0)
#define SPI_MW_MDD            (1 << 1)
#define SPI_MW_MHS            (1 << 2)

#define SPIx_SE(i)            SPIx_BASE(i) + 0x10
#define SPI_SLAVES_MASK        0xFFFF
//SPI_SLAVES_MASK dependend on hardware's setting
#define SPIx_BAUD(i)        SPIx_BASE(i) + 0x14
#define SPI_BAUD_MASK        0xFFFF

#define SPIx_TXFTL(i)        SPIx_BASE(i) + 0x18
#define SPI_TFT_MASK        0x7
//dependend on TX_ABW
#define SPIx_RXFTL(i)        SPIx_BASE(i) + 0x1c
#define SPI_RFT_MASK        0x7
//dependend on RX_ABW
#define SPIx_TXFL(i)        SPIx_BASE(i) + 0x20
#define SPI_TXFL_MASK        0x8
//dependend on TX_ABW
#define SPIx_RXFL(i)        SPIx_BASE(i) + 0x24
#define SPI_RXFL_MASK        0x8
//dependend on RX_ABW
#define SPIx_ST(i)            SPIx_BASE(i) + 0x28
#define SPI_ST_BUSY            (1 << 0)
#define SPI_ST_TFNF            (1 << 1)
#define SPI_ST_TFE            (1 << 2)
#define SPI_ST_RFNE            (1 << 3)
#define SPI_ST_RFF            (1 << 4)
#define SPI_ST_TXE            (1 << 5)
#define SPI_ST_DCOL            (1 << 6)

#define SPIx_IM(i)            SPIx_BASE(i) + 0x2c
#define SPIx_IS(i)            SPIx_BASE(i) + 0x30
#define SPI_IS_TX_EMPTY        (1 << 0)
#define SPI_IS_TX_OF        (1 << 1)
#define SPI_IS_RX_UF        (1 << 2)
#define SPI_IS_RX_OF        (1 << 3)
#define SPI_IS_RX_FULL        (1 << 4)
#define SPI_IS_MST            (1 << 5)
#define SPI_IS_MSK            (0x3F)

#define SPIx_RIS(i)            SPIx_BASE(i) + 0x34
#define SPIx_TXOIC(i)        SPIx_BASE(i) + 0x38
#define SPIx_RXOIC(i)        SPIx_BASE(i) + 0x3c
#define SPIx_RXUIC(i)        SPIx_BASE(i) + 0x40
#define SPIx_MSTIC(i)        SPIx_BASE(i) + 0x44
#define SPIx_IC(i)            SPIx_BASE(i) + 0x48
#define SPIx_DMAC(i)        SPIx_BASE(i) + 0x4c
#define SPI_DMA_TX_EN        (1 << 1)
#define SPI_DMA_RX_EN        (1 << 0)

#define SPIx_DMATDL(i)        SPIx_BASE(i) + 0x50
#define SPI_DMA_TDL_MASK    0x1F
//dependend on TX_ABW
#define SPIx_DMARDL(i)        SPIx_BASE(i) + 0x54
#define SPI_DMA_RDL_MASK    0x1F
//dependend on RX_ABW
#define SPIx_ID(i)            SPIx_BASE(i) + 0x58
#define SPIx_COMP_VER(i)    SPIx_BASE(i) + 0x5c
#define SPIx_DR(i, j)        SPIx_BASE(i) + 0x60 + (j) * 0x4
#define SPIx_RX_SAMPLE_DLY(i)    SPIx_BASE(i) + 0xf0
#define SPIx_SPI_CTRL        SPIx_BASE(i) + 0xf4
#define SPIx_RSVD1(i)        SPIx_BASE(i) + 0xf8
#define SPIx_RSVD2(i)        SPIx_BASE(i) + 0xfc

// DRAMC Registers
#define DMA_BASE                     0x40004000
// ch0
#define DMA_SAR0_L                   *((volatile int *)(DMA_BASE+0x000))
#define DMA_SAR0_H                   *((volatile int *)(DMA_BASE+0x004))
#define DMA_DAR0_L                   *((volatile int *)(DMA_BASE+0x008))
#define DMA_DAR0_H                   *((volatile int *)(DMA_BASE+0x00C))

#define DMA_LLP0_L                   *((volatile int *)(DMA_BASE+0x010))
#define DMA_LLP0_H                   *((volatile int *)(DMA_BASE+0x014))
#define DMA_CTL0_L                   *((volatile int *)(DMA_BASE+0x018))
#define DMA_CTL0_H                   *((volatile int *)(DMA_BASE+0x01C))

#define DMA_SSTAT0_L                 *((volatile int *)(DMA_BASE+0x020))
#define DMA_SSTAT0_H                 *((volatile int *)(DMA_BASE+0x024))
#define DMA_DSTAT0_L                 *((volatile int *)(DMA_BASE+0x028))
#define DMA_DSTAT0_H                 *((volatile int *)(DMA_BASE+0x02C))

#define DMA_SSTATAR0_L               *((volatile int *)(DMA_BASE+0x030))
#define DMA_SSTATAR0_H               *((volatile int *)(DMA_BASE+0x034))
#define DMA_DSTATAR0_L               *((volatile int *)(DMA_BASE+0x038))
#define DMA_DSTATAR0_H               *((volatile int *)(DMA_BASE+0x03C))

#define DMA_CFG0_L                   *((volatile int *)(DMA_BASE+0x040))
#define DMA_CFG0_H                   *((volatile int *)(DMA_BASE+0x044))
#define DMA_SGR0_L                   *((volatile int *)(DMA_BASE+0x048))
#define DMA_SGR0_H                   *((volatile int *)(DMA_BASE+0x04C))
#define DMA_DSR0_L                   *((volatile int *)(DMA_BASE+0x050))
#define DMA_DSR0_H                   *((volatile int *)(DMA_BASE+0x054))
// ch1
#define DMA_SAR1_L                   *((volatile int *)(DMA_BASE+0x058))
#define DMA_SAR1_H                   *((volatile int *)(DMA_BASE+0x05C))
#define DMA_DAR1_L                   *((volatile int *)(DMA_BASE+0x060))
#define DMA_DAR1_H                   *((volatile int *)(DMA_BASE+0x064))

#define DMA_LLP1_L                   *((volatile int *)(DMA_BASE+0x068))
#define DMA_LLP1_H                   *((volatile int *)(DMA_BASE+0x06C))
#define DMA_CTL1_L                   *((volatile int *)(DMA_BASE+0x070))
#define DMA_CTL1_H                   *((volatile int *)(DMA_BASE+0x074))

#define DMA_SSTAT1_L                 *((volatile int *)(DMA_BASE+0x078))
#define DMA_SSTAT1_H                 *((volatile int *)(DMA_BASE+0x07C))
#define DMA_DSTAT1_L                 *((volatile int *)(DMA_BASE+0x080))
#define DMA_DSTAT1_H                 *((volatile int *)(DMA_BASE+0x084))

#define DMA_SSTATAR1_L               *((volatile int *)(DMA_BASE+0x088))
#define DMA_SSTATAR1_H               *((volatile int *)(DMA_BASE+0x08C))
#define DMA_DSTATAR1_L               *((volatile int *)(DMA_BASE+0x090))
#define DMA_DSTATAR1_H               *((volatile int *)(DMA_BASE+0x094))

#define DMA_CFG1_L                   *((volatile int *)(DMA_BASE+0x098))
#define DMA_CFG1_H                   *((volatile int *)(DMA_BASE+0x09C))
#define DMA_SGR1_L                   *((volatile int *)(DMA_BASE+0x0A0))
#define DMA_SGR1_H                   *((volatile int *)(DMA_BASE+0x0A4))
#define DMA_DSR1_L                   *((volatile int *)(DMA_BASE+0x0A8))
#define DMA_DSR1_H                   *((volatile int *)(DMA_BASE+0x0AC))

#define DMA_STATUSTFR                     *((volatile int *)(DMA_BASE+0x2e8))
#define DMA_STATUSBLOCK                 *((volatile int *)(DMA_BASE+0x2f0))
#define DMA_STATUSSRCTRAN                 *((volatile int *)(DMA_BASE+0x2f8))
#define DMA_STATUSDSTTRAN                 *((volatile int *)(DMA_BASE+0x300))
#define DMA_STATUSERR                     *((volatile int *)(DMA_BASE+0x308))

#define DMA_MASKTFR                     *((volatile int *)(DMA_BASE+0x310))
#define DMA_MASKBLOCK                 *((volatile int *)(DMA_BASE+0x318))
#define DMA_MASKSRCTRAN                 *((volatile int *)(DMA_BASE+0x320))
#define DMA_MASKDSTTRAN                 *((volatile int *)(DMA_BASE+0x328))
#define DMA_MASKERR                     *((volatile int *)(DMA_BASE+0x330))

#define DMA_CLEARTFR                     *((volatile int *)(DMA_BASE+0x338))
#define DMA_CLEARBLOCK                 *((volatile int *)(DMA_BASE+0x340))
#define DMA_CLEARSRCTRAN                 *((volatile int *)(DMA_BASE+0x348))
#define DMA_CLEARDSTTRAN                 *((volatile int *)(DMA_BASE+0x350))
#define DMA_CLEARERR                     *((volatile int *)(DMA_BASE+0x358))

#define DMACFGREG_L                  *((volatile int *)(DMA_BASE+0x398))
#define DMACFGREG_H                  *((volatile int *)(DMA_BASE+0x39C))
#define CHENREG_L                    *((volatile int *)(DMA_BASE+0x3A0))
#define CHENREG_H                    *((volatile int *)(DMA_BASE+0x3A4))
#define DMA_COMP_PARAMS_3_L          *((volatile int *)(DMA_BASE+0x3E0))
#define DMA_COMP_PARAMS_3_H          *((volatile int *)(DMA_BASE+0x3E4))
#define DMA_COMP_PARAMS_2_L          *((volatile int *)(DMA_BASE+0x3E8))
#define DMA_COMP_PARAMS_2_H          *((volatile int *)(DMA_BASE+0x3EC))
#define DMA_COMP_PARAMS_1_L          *((volatile int *)(DMA_BASE+0x3F0))
#define DMA_COMP_PARAMS_1_H          *((volatile int *)(DMA_BASE+0x3F4))

typedef struct spi_controller {
    char *name;
    volatile uint32_t left_tx_size;
    volatile uint32_t left_rx_size;
    void *pvtx;
    void *pvrx;
    void (*tx_func) (int port);
    void (*rx_func) (int port);
    uint8_t bits;
    uint8_t lsb;
    uint8_t is_loopback;
} lega_spi_controller_t;

typedef enum spi_trans_mode {
    SPI_TRANS_PIO_MODE,
    SPI_TRANS_DMA_MODE,
} lega_spi_trans_mode_t;

void tx_8bit(int port);
void rx_8bit(int port);
void tx_16bit(int port);
void rx_16bit(int port);
void tx_32bit(int port);
void rx_32bit(int port);

int spi_pio_trans_data(int port);
int spi_dma_trans_data(int port);

#define LEGA_SPI_MODE_MASTER 1  /* spi communication is master mode */
#define LEGA_SPI_MODE_SLAVE  2  /* spi communication is slave mode */

typedef struct {
    uint32_t mode;        /* spi communication mode */
    uint32_t freq;        /* communication frequency Hz */
} lega_spi_config_t;

typedef struct {
    uint8_t      port;    /* spi port */
    lega_spi_config_t config;  /* spi config */
    void        *priv;    /* priv data */
} lega_spi_dev_t;

/**
 * Initialises the SPI interface for a given SPI device
 *
 * @param[in]  spi  the spi device
 *
 * @return  0 : on success, EIO : if the SPI device could not be initialised
 */
int32_t lega_spi_init(lega_spi_dev_t *spi);

/**
 * Spi send
 *
 * @param[in]  spi      the spi device
 * @param[in]  data     spi send data
 * @param[in]  size     spi send data size
 * @param[in]  timeout  timeout in ms
 *
 * @return  0 : on success, EIO : if the SPI device could not be initialised
 */
int32_t lega_spi_send(lega_spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout);

/**
 * spi_recv
 *
 * @param[in]   spi      the spi device
 * @param[out]  data     spi recv data
 * @param[in]   size     spi recv data size
 * @param[in]   timeout  timeout in ms
 *
 * @return  0 : on success, EIO : if the SPI device could not be initialised
 */
int32_t lega_spi_recv(lega_spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout);

/**
 * spi send data and recv
 *
 * @param[in]  spi      the spi device
 * @param[in]  tx_data  spi send data
 * @param[in]  rx_data  spi recv data
 * @param[in]  size  spi data to be sent and recived
 * @param[in]  timeout  timeout in ms
 *
 * @return  0, on success;  EIO : if the SPI device could not be initialised
 */
int32_t lega_spi_send_recv(lega_spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data,
                          uint16_t size, uint32_t timeout);

/**
 * De-initialises a SPI interface
 *
 *
 * @param[in]  spi the SPI device to be de-initialised
 *
 * @return  0 : on success, EIO : if an error occurred
 */
int32_t lega_spi_finalize(lega_spi_dev_t *spi);


#endif//_LEGA_SPI_H_

#ifndef _RDA_DMA_H_
#define _RDA_DMA_H_

#define RDA_HW_ACCELERATE_ENABLE   1

typedef enum _dma_mode {
    NORMAL_MODE  = 0,
    AES_ENC_MODE = 1,
    AES_DEC_MODE = 5,
    TX_CRC_MODE  = 2,
    RX_CRC_MODE  = 6
}dma_mode;

typedef enum _dma_int_out_dscp {
    AHB_DMA_DONE          = (1<<0),
    PRNG_ALERT            = (1<<1),
    TRNG_ON_FLY_TEST_FAIL = (1<<2),
    TRNG_START_TEST_FAIL  = (1<<3),
    TRNG_DATA_READY       = (1<<4),
    CIOS_DONE             = (1<<5)
}dma_int_out_dscp;

typedef enum _dma_ctrl_hsizem{
    DMACtrlHsizemByte     = 0U,      /*!< DMA ctrl hsizem 8 bits */
    DMACtrlHsizemHalfword = 1U,      /*!< DMA ctrl hsizem 16 bits */
    DMACtrlHsizemWord     = 2U       /*!< DMA ctrl hsizem 32 bits */
}dma_ctrl_hsizem;

typedef enum _dma_ctrl_fix_src{
    DMACtrlSrcAddrIncrease      = 0U,      /*!< DMA ctrl fix src off */
    DMACtrlFixSrcAddr           = 1U      /*!< DMA ctrl fix src on */
}dma_ctrl_fix_src;

typedef enum _dma_ctrl_fix_dst{
    DMACtrlDstAddrIncrease      = 0U,      /*!< DMA ctrl fix dst off */
    DMACtrlFixDstAddr           = 1U      /*!< DMA ctrl fix dst on */
}dma_ctrl_fix_dst;

#ifndef RDA_SCU_CLK_GATE0_ENABLE
#define RDA_SCU_CLK_GATE0_ENABLE()           do { \
                                                   RDA_SCU->CLKGATE0 |= (1 << 18); \
                                                   for (int j=0; j<4; j++) ;       \
                                               } while(0)
#endif

#ifndef RDA_SCU_CLK_GATE0_DISABLE
#define RDA_SCU_CLK_GATE0_DISABLE()          do { \
                                                   RDA_SCU->CLKGATE0 &= ~(1 << 18); \
                                               } while(0)
#endif

/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                                    \
{                                                               \
    (b)[(i) + 3] = (unsigned char) ( ( (n)       ) & 0xFF );    \
    (b)[(i) + 2] = (unsigned char) ( ( (n) >>  8 ) & 0xFF );    \
    (b)[(i) + 1] = (unsigned char) ( ( (n) >> 16 ) & 0xFF );    \
    (b)[(i) + 0] = (unsigned char) ( ( (n) >> 24 ) & 0xFF );    \
}
#endif /* PUT_UINT32_BE */
#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)                            \
{                                                       \
    (n) = ( (unsigned int) (b)[(i) + 3]       )             \
        | ( (unsigned int) (b)[(i) + 2] <<  8 )             \
        | ( (unsigned int) (b)[(i) + 1] << 16 )             \
        | ( (unsigned int) (b)[(i) + 0] << 24 );            \
}
#endif /* GET_UINT32_BE */

#endif /* _RDA_DMA_H_ */

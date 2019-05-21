#ifndef __GDMA_PUB_H__
#define __GDMA_PUB_H__

#if CFG_GENERAL_DMA
#define GDMA_FAILURE                (1)
#define GDMA_SUCCESS                (0)

#define GDMA_DEV_NAME                "generaldma"

#define GDMA_CMD_MAGIC              (0x0e809000)

enum
{
    CMD_GDMA_SET_DMA_ENABLE = GDMA_CMD_MAGIC + 1,
    CMD_GDMA_CFG_FIN_INT_ENABLE,
    CMD_GDMA_CFG_HFIN_INT_ENABLE,
    CMD_GDMA_CFG_WORK_MODE,    
    CMD_GDMA_CFG_SRCDATA_WIDTH,
    CMD_GDMA_CFG_DSTDATA_WIDTH,
    CMD_GDMA_CFG_SRCADDR_INCREASE,
    CMD_GDMA_CFG_DSTADDR_INCREASE,
    CMD_GDMA_CFG_SRCADDR_LOOP,
    CMD_GDMA_CFG_DSTADDR_LOOP,
    CMD_GDMA_SET_CHNL_PRIO,
    CMD_GDMA_SET_TRANS_LENGTH,
    CMD_GDMA_SET_DST_START_ADDR,
    CMD_GDMA_SET_SRC_START_ADDR,
    CMD_GDMA_SET_DST_LOOP_ENDADDR,
    CMD_GDMA_SET_DST_LOOP_STARTADDR,
    CMD_GDMA_SET_SRC_LOOP_ENDADDR,
    CMD_GDMA_SET_SRC_LOOP_STARTADDR,
    CMD_GDMA_SET_DST_REQ_MUX,
    CMD_GDMA_SET_SRC_REQ_MUX,
    CMD_GDMA_SET_DTCM_WRITE_WORD,
    CMD_GDMA_GET_REMAIN_LENGTH,
    CMD_GDMA_SET_FIN_CNT,
    CMD_GDMA_SET_HFIN_CNT,
    CMD_GDMA_SET_PRIO_MODE,
    CMD_GDMA_GET_FIN_INT_STATUS,
    CMD_GDMA_CLR_FIN_INT_STATUS,
    CMD_GDMA_GET_HFIN_INT_STATUS,
    CMD_GDMA_CLR_HFIN_INT_STATUS,
    CMD_GDMA_CFG_TYPE0,
    CMD_GDMA_CFG_TYPE1,
    CMD_GDMA_CFG_TYPE2,
    CMD_GDMA_CFG_TYPE3,
    CMD_GDMA_CFG_TYPE4,
    CMD_GDMA_CFG_TYPE5,
    CMD_GDMA_CFG_TYPE6,     
    CMD_GDMA_ENABLE,
    CMD_GDMA_GET_LEFT_LEN,
    CMD_GDMA_SET_SRC_PAUSE_ADDR,
    CMD_GDMA_GET_SRC_PAUSE_ADDR, 
    CMD_GDMA_SET_DST_PAUSE_ADDR,
    CMD_GDMA_GET_DST_PAUSE_ADDR, 
    CMD_GDMA_GET_SRC_READ_ADDR,
    CMD_GDMA_GET_DST_WRITE_ADDR, 
};

#define GDMA_X_DST_DTCM_WR_REQ            (0x0)
#define GDMA_X_DST_HSSPI_TX_REQ           (0x1)
#define GDMA_X_DST_AUDIO_RX_REQ           (0x2)
#define GDMA_X_DST_SDIO_TX_REQ            (0x3)
#define GDMA_X_DST_UART1_TX_REQ           (0x4)
#define GDMA_X_DST_UART2_TX_REQ           (0x5)
#define GDMA_X_DST_I2S_TX_REQ             (0x6)
#define GDMA_X_DST_GSPI_TX_REQ            (0x7)
#define GDMA_X_DST_PSRAM_V_WR_REQ         (0x9)
#define GDMA_X_DST_PSRAM_A_WR_REQ         (0xA)
#define GDMA_X_SRC_DTCM_RD_REQ            (0x0)
#define GDMA_X_SRC_HSSPI_RX_REQ           (0x1)
#define GDMA_X_SRC_AUDIO_TX_REQ           (0x2)
#define GDMA_X_SRC_SDIO_RX_REQ            (0x3)
#define GDMA_X_SRC_UART1_RX_REQ           (0x4)
#define GDMA_X_SRC_UART2_RX_REQ           (0x5)
#define GDMA_X_SRC_I2S_RX_REQ             (0x6)
#define GDMA_X_SRC_GSPI_RX_REQ            (0x7)
#define GDMA_X_SRC_JPEG_WR_REQ            (0x8)
#define GDMA_X_SRC_PSRAM_V_RD_REQ         (0x9)
#define GDMA_X_SRC_PSRAM_A_RD_REQ         (0xA)
#define GDMA_X_SRC_DST_RESERVE            (0xB)

enum
{
    GDMA_CHANNEL_0  = 0,
    GDMA_CHANNEL_1,
    GDMA_CHANNEL_2,
    GDMA_CHANNEL_3,
#if (CFG_SOC_NAME == SOC_BK7231)
    GDMA_CHANNEL_MAX,
#else
    GDMA_CHANNEL_4,
    GDMA_CHANNEL_5,
    GDMA_CHANNEL_MAX
#endif // (CFG_SOC_NAME == SOC_BK7231)
};

#define GDMA_TYPE_0         (0U)  // no loop src, no loop dst, no register
#define GDMA_TYPE_1         (1U)  // loop src,    no loop dst, no register
#define GDMA_TYPE_2         (2U)  // no loop src, loop dst, no register
#define GDMA_TYPE_3         (3U)  // loop src,    loop dst, no register

#define GDMA_TYPE_4         (4U)  // loop src,    no loop dst, register
#define GDMA_TYPE_5         (5U)  // no loop src, loop dst, register
#define GDMA_TYPE_6         (6U)  // loop src,    loop dst, register

typedef struct gdmacfg_types_st
{
    UINT32 channel;
    UINT32 prio;
    UINT8 dstptr_incr;
    UINT8 srcptr_incr;
    UINT8 dstdat_width;
    UINT8 srcdat_width;

    void *src_start_addr;
    void *dst_start_addr;

    UINT8 src_module;
    UINT8 dst_module;
    void (*fin_handler)(UINT32);
    void (*half_fin_handler)(UINT32);

    union
    {
        struct
        {
            void *src_loop_start_addr;
            void *src_loop_end_addr;
        } type1; // loop src,    no loop dst,    no register

        struct
        {
            void *dst_loop_start_addr;
            void *dst_loop_end_addr;
        } type2; // no loop src, loop dst,    no register

        struct
        {
            void *src_loop_start_addr;
            void *src_loop_end_addr;
            void *dst_loop_start_addr;
            void *dst_loop_end_addr;
        } type3; // loop src,    loop dst,    no register

        struct
        {
            void *src_loop_start_addr;
            void *src_loop_end_addr;
        } type4; // loop src,    no loop dst,    register

        struct
        {
            void *dst_loop_start_addr;
            void *dst_loop_end_addr;
        } type5; // no loop src, loop dst,    register

        struct
        {
            void *src_loop_start_addr;
            void *src_loop_end_addr;
            void *dst_loop_start_addr;
            void *dst_loop_end_addr;
        } type6; // loop src,    loop dst,    register
    } u;
} GDMACFG_TPYES_ST, *GDMACFG_TPYES_PTR;

typedef struct gdma_do_st
{
    UINT32 channel;
    void *src_addr;
    void *dst_addr;
    UINT32 length;
} GDMA_DO_ST, *GDMA_DO_PTR;


typedef struct generdam_cfg_st
{
    UINT32 param;
    UINT32 channel;
} GDMA_CFG_ST, *GDMA_CFG_PTR;

void gdma_init(void);
void gdma_exit(void);
void *gdma_memcpy(void *out, const void *in, UINT32 n);

#endif  // CFG_GENERAL_DMA

#endif

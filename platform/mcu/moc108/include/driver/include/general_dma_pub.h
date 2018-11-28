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
    CMD_GDMA_GET_REMAIN_LENGTH,
    CMD_GDMA_GET_FIN_INT_STATUS,
    CMD_GDMA_CLR_FIN_INT_STATUS,
    CMD_GDMA_GET_HFIN_INT_STATUS,
    CMD_GDMA_CLR_HFIN_INT_STATUS,
    CMD_GDMA_CFG_TYPE0,
    CMD_GDMA_CFG_TYPE1,
    CMD_GDMA_CFG_TYPE2,
    CMD_GDMA_CFG_TYPE3,
    CMD_GDMA_ENABLE
};


#define GDMA_CHANNEL_0      (0U)
#define GDMA_CHANNEL_1      (1U)
#define GDMA_CHANNEL_2      (2U)
#define GDMA_CHANNEL_3      (3U)

#define GDMA_TYPE_0         (0U)  // no loop src, no loop dst
#define GDMA_TYPE_1         (1U)  // loop src,    no loop dst
#define GDMA_TYPE_2         (2U)  // no loop src, loop dst
#define GDMA_TYPE_3         (3U)  // loop src,    loop dst

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

    union
    {
        struct
        {
            void *src_loop_start_addr;
            void *src_loop_end_addr;
        } type1; // loop src,    no loop dst

        struct
        {
            void *dst_loop_start_addr;
            void *dst_loop_end_addr;
        } type2; // no loop src, loop dst

        struct
        {
            void *src_loop_start_addr;
            void *src_loop_end_addr;
            void *dst_loop_start_addr;
            void *dst_loop_end_addr;
        } type3; // loop src,    loop dst
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

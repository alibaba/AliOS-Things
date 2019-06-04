#include "include.h"
#include "arm_arch.h"

#if CFG_GENERAL_DMA
#include "general_dma_pub.h"
#include "general_dma.h"

#include "intc_pub.h"
#include "icu_pub.h"

#include "drv_model_pub.h"
#include "mem_pub.h"

static SDD_OPERATIONS gdma_op =
{
    gdma_ctrl
};

#if (CFG_SOC_NAME != SOC_BK7231)
void (*p_dma_fin_handler[GDMA_CHANNEL_MAX])(UINT32);
void (*p_dma_hfin_handler[GDMA_CHANNEL_MAX])(UINT32);
#endif

static void gdma_isr(void);

static void gdma_set_dma_en(UINT32 channel, UINT32 enable)
{
    UINT32 reg_addr = GENER_DMA0_REG0_CONF + (channel * 8 * 4);
    UINT32 reg_val = REG_READ(reg_addr);
    if(enable)
        reg_val |= GDMA_X_DMA_EN;
    else
        reg_val &= ~GDMA_X_DMA_EN;
    REG_WRITE(reg_addr, reg_val);
}

static UINT32 gdma_get_dma_en(UINT32 channel)
{
    UINT32 reg_addr = GENER_DMA0_REG0_CONF + (channel * 8 * 4);
    
    return REG_READ(reg_addr) & GDMA_X_DMA_EN;
}

static void gdma_cfg_finish_inten(UINT32 channel, UINT32 enable)
{
    UINT32 reg_addr = GENER_DMA0_REG0_CONF + (channel * 8 * 4);
    UINT32 reg_val = REG_READ(reg_addr);
    if(enable)
        reg_val |= GDMA_X_FIN_INTEN;
    else
        reg_val &= ~GDMA_X_FIN_INTEN;
    REG_WRITE(reg_addr, reg_val);
}

static void gdma_cfg_half_fin_inten(UINT32 channel, UINT32 enable)
{
    UINT32 reg_addr = GENER_DMA0_REG0_CONF + (channel * 8 * 4);
    UINT32 reg_val = REG_READ(reg_addr);
    if(enable)
        reg_val |= GDMA_X_HFIN_INTEN;
    else
        reg_val &= ~GDMA_X_HFIN_INTEN;
    REG_WRITE(reg_addr, reg_val);
}

#if (CFG_SOC_NAME != SOC_BK7231)
static void gdma_cfg_work_mode(UINT32 channel, UINT32 repeat)
{
    UINT32 reg_addr = GENER_DMA0_REG0_CONF + (channel * 8 * 4);
    UINT32 reg_val = REG_READ(reg_addr);
    if(repeat)
        reg_val |= GDMA_X_REPEAT_MODE;
    else
        reg_val &= ~GDMA_X_REPEAT_MODE;
    REG_WRITE(reg_addr, reg_val);
}
#endif // (CFG_SOC_NAME != SOC_BK7231)

static void gdma_cfg_srcdata_width(UINT32 channel, UINT32 bitwidth)
{
    UINT32 reg_addr = GENER_DMA0_REG0_CONF + (channel * 8 * 4);
    UINT32 reg_val = REG_READ(reg_addr);
    reg_val &= ~(GDMA_X_SRCDATA_WIDTH_MASK << GDMA_X_SRCDATA_WIDTH_POSI);

    switch(bitwidth)
    {
    case 8:
        reg_val |= (GDMA_DATA_WIDTH_8BIT << GDMA_X_SRCDATA_WIDTH_POSI);
        break;
    case 16:
        reg_val |= (GDMA_DATA_WIDTH_16BIT << GDMA_X_SRCDATA_WIDTH_POSI);
        break;
    case 32:
        reg_val |= (GDMA_DATA_WIDTH_32BIT << GDMA_X_SRCDATA_WIDTH_POSI);
        break;
    default:
        reg_val |= (GDMA_DATA_WIDTH_32BIT << GDMA_X_SRCDATA_WIDTH_POSI);
        break;
    }
    REG_WRITE(reg_addr, reg_val);
}

static void gdma_cfg_dstdata_width(UINT32 channel, UINT32 bitwidth)
{
    UINT32 reg_addr = GENER_DMA0_REG0_CONF + (channel * 8 * 4);
    UINT32 reg_val = REG_READ(reg_addr);
    reg_val &= ~(GDMA_X_DSTDATA_WIDTH_MASK << GDMA_X_DSTDATA_WIDTH_POSI);

    switch(bitwidth)
    {
    case 8:
        reg_val |= (GDMA_DATA_WIDTH_8BIT << GDMA_X_DSTDATA_WIDTH_POSI);
        break;
    case 16:
        reg_val |= (GDMA_DATA_WIDTH_16BIT << GDMA_X_DSTDATA_WIDTH_POSI);
        break;
    case 32:
        reg_val |= (GDMA_DATA_WIDTH_32BIT << GDMA_X_DSTDATA_WIDTH_POSI);
        break;
    default:
        reg_val |= (GDMA_DATA_WIDTH_32BIT << GDMA_X_DSTDATA_WIDTH_POSI);
        break;
    }
    REG_WRITE(reg_addr, reg_val);
}

static void gdma_cfg_srcaddr_increase(UINT32 channel, UINT32 incr)
{
    UINT32 reg_addr = GENER_DMA0_REG0_CONF + (channel * 8 * 4);
    UINT32 reg_val = REG_READ(reg_addr);
    if(incr)
        reg_val |= GDMA_X_SRCADDR_INC;
    else
        reg_val &= ~GDMA_X_SRCADDR_INC;
    REG_WRITE(reg_addr, reg_val);
}

static void gdma_cfg_dstaddr_increase(UINT32 channel, UINT32 incr)
{
    UINT32 reg_addr = GENER_DMA0_REG0_CONF + (channel * 8 * 4);
    UINT32 reg_val = REG_READ(reg_addr);
    if(incr)
        reg_val |= GDMA_X_DSTADDR_INC;
    else
        reg_val &= ~GDMA_X_DSTADDR_INC;
    REG_WRITE(reg_addr, reg_val);
}

static void gdma_cfg_srcaddr_loop(UINT32 channel, UINT32 loop)
{
    UINT32 reg_addr = GENER_DMA0_REG0_CONF + (channel * 8 * 4);
    UINT32 reg_val = REG_READ(reg_addr);
    if(loop)
        reg_val |= GDMA_X_SRCADDR_LOOP;
    else
        reg_val &= ~GDMA_X_SRCADDR_LOOP;
    REG_WRITE(reg_addr, reg_val);
}

static void gdma_cfg_dstaddr_loop(UINT32 channel, UINT32 loop)
{
    UINT32 reg_addr = GENER_DMA0_REG0_CONF + (channel * 8 * 4);
    UINT32 reg_val = REG_READ(reg_addr);
    if(loop)
        reg_val |= GDMA_X_DSTADDR_LOOP;
    else
        reg_val &= ~GDMA_X_DSTADDR_LOOP;
    REG_WRITE(reg_addr, reg_val);
}

static void gdma_set_channel_prioprity(UINT32 channel, UINT32 prio)
{
    UINT32 reg_addr = GENER_DMA0_REG0_CONF + (channel * 8 * 4);
    UINT32 reg_val = REG_READ(reg_addr);

    reg_val &= ~(GDMA_X_CHNL_PRIO_MASK << GDMA_X_CHNL_PRIO_POSI);
    reg_val |= ((prio & GDMA_X_CHNL_PRIO_MASK) << GDMA_X_CHNL_PRIO_POSI);

    REG_WRITE(reg_addr, reg_val);
}

static void gdma_set_transfer_length(UINT32 channel, UINT32 len)
{
    UINT32 reg_addr = GENER_DMA0_REG0_CONF + (channel * 8 * 4);
    UINT32 reg_val = REG_READ(reg_addr);

    reg_val &= ~(GDMA_X_TRANS_LEN_MASK << GDMA_X_TRANS_LEN_POSI);
    reg_val |= (((len - 1) & GDMA_X_TRANS_LEN_MASK) << GDMA_X_TRANS_LEN_POSI);

    REG_WRITE(reg_addr, reg_val);
}

static UINT32 gdma_get_transfer_length(UINT32 channel)
{
    UINT32 reg_addr = GENER_DMA0_REG0_CONF + (channel * 8 * 4);
    UINT32 reg_val = REG_READ(reg_addr);

    return ((reg_val >> GDMA_X_TRANS_LEN_POSI) & GDMA_X_TRANS_LEN_MASK) + 1;
}

static void gdma_set_dst_start_addr(UINT32 channel, void *dstptr)
{
    UINT32 reg_addr = GENER_DMA0_REG1_DST_START_ADDR + (channel * 8 * 4);
    REG_WRITE(reg_addr, (UINT32)dstptr);
}

static void gdma_set_src_start_addr(UINT32 channel, void *srcptr)
{
    UINT32 reg_addr = GENER_DMA0_REG2_SRC_START_ADDR + (channel * 8 * 4);
    REG_WRITE(reg_addr, (UINT32)srcptr);
}

static void gdma_set_dstloop_endaddr(UINT32 channel, void *dendptr)
{
    UINT32 reg_addr = GENER_DMA0_REG3_DSTLOOP_END_ADDR + (channel * 8 * 4);
    REG_WRITE(reg_addr, (UINT32)dendptr);
}

static void gdma_set_dstloop_startaddr(UINT32 channel, void *dstartptr)
{
    UINT32 reg_addr = GENER_DMA0_REG4_DSTLOOP_START_ADDR + (channel * 8 * 4);
    REG_WRITE(reg_addr, (UINT32)dstartptr);
}

static void gdma_set_srcloop_endaddr(UINT32 channel, void *sendptr)
{
    UINT32 reg_addr = GENER_DMA0_REG5_SRCLOOP_END_ADDR + (channel * 8 * 4);
    REG_WRITE(reg_addr, (UINT32)sendptr);
}

static void gdma_set_srcloop_startaddr(UINT32 channel, void *sstartptr)
{
    UINT32 reg_addr = GENER_DMA0_REG6_SRCLOOP_START_ADDR + (channel * 8 * 4);
    REG_WRITE(reg_addr, (UINT32)sstartptr);
}

#if (CFG_SOC_NAME == SOC_BK7231)
static UINT32 gdma_get_remain_len(UINT32 channel)
{
    UINT32 reg_addr = GENER_DMA0_REG7_REMAIN_LEN + (channel * 8 * 4);
    return REG_READ(reg_addr) & GDMA_REMAIN_LEN_MASK;
}
#else
static UINT32 gdma_get_remain_len(UINT32 channel)
{
    UINT32 reg_addr = GENER_DMA0_REG30_INT_CNTS + (channel * 4);
    UINT32 reg_val = REG_READ(reg_addr);
    
    return ((reg_val >> GDMA_X_REMAIN_LEN_POSI) & GDMA_X_REMAIN_LEN_MASK);
}

static void gdma_set_src_reqmux(UINT32 channel, UINT32 reqmux)
{
    UINT32 reg_addr = GENER_DMA0_REG7_MUX_REQS + (channel * 8 * 4);
    UINT32 reg_val = REG_READ(reg_addr);

    reg_val &= ~(GDMA_X_SRC_REQ_MASK << GDMA_X_SRC_REQ_POSI);

    reg_val |= ((reqmux & GDMA_X_SRC_REQ_MASK) << GDMA_X_SRC_REQ_POSI);
    
    REG_WRITE(reg_addr, reg_val);
}

static void gdma_set_dst_reqmux(UINT32 channel, UINT32 reqmux)
{
    UINT32 reg_addr = GENER_DMA0_REG7_MUX_REQS + (channel * 8 * 4);
    UINT32 reg_val = REG_READ(reg_addr);

    reg_val &= ~(GDMA_X_DST_REQ_MASK << GDMA_X_DST_REQ_POSI);

    reg_val |= ((reqmux & GDMA_X_DST_REQ_MASK) << GDMA_X_DST_REQ_POSI);
    
    REG_WRITE(reg_addr, reg_val);
}

static void gdma_set_dctm_write_wd(UINT32 channel, UINT32 read4byte)
{
    UINT32 reg_addr = GENER_DMA0_REG7_MUX_REQS + (channel * 8 * 4);
    UINT32 reg_val = REG_READ(reg_addr);

    if(read4byte)
        reg_val |= GDMA_X_DTCM_WR_WAIT_WORD;
    else
        reg_val &= ~GDMA_X_DTCM_WR_WAIT_WORD;
    REG_WRITE(reg_addr, reg_val);
}

static UINT32 gdma_get_finish_cnt(UINT32 channel)
{
    UINT32 reg_addr = GENER_DMA0_REG30_INT_CNTS + (channel * 4);
    UINT32 reg_val = REG_READ(reg_addr);
    
    return ((reg_val >> GDMA_X_FIN_CNT_POSI) & GDMA_X_FIN_CNT_MASK);
}

static UINT32 gdma_get_half_finish_cnt(UINT32 channel)
{
    UINT32 reg_addr = GENER_DMA0_REG30_INT_CNTS + (channel * 4);
    UINT32 reg_val = REG_READ(reg_addr);
    
    return ((reg_val >> GDMA_X_HFIN_CNT_POSI) & GDMA_X_HFIN_CNT_MASK);
}

static void gdma_set_priority(UINT32 fixed)
{
    UINT32 reg_val = REG_READ(GENER_DMA_REG37_PRIO_MODE);

    if(fixed)
        reg_val |= GDMA_PRIO_FIXED;
    else
        reg_val &= ~GDMA_PRIO_FIXED;
    REG_WRITE(GENER_DMA_REG37_PRIO_MODE, reg_val);
}

#endif // (CFG_SOC_NAME == SOC_BK7231)

static UINT32 gdma_get_finish_interrupt_bit(UINT32 channel)
{
    #if (CFG_SOC_NAME == SOC_BK7231)
    UINT32 reg_val = REG_READ(GENER_DMA_REG20_DMA_INT_STATUS);
    #else
    UINT32 reg_val = REG_READ(GENER_DMA_REG38_DMA_INT_STATUS);
    #endif // (CFG_SOC_NAME == SOC_BK7231)

    reg_val &= GENER_DMA_FIN_INT_STATUS_MASK;
    reg_val &= (0x1 << channel);
    reg_val = reg_val >> channel;;
    return reg_val;
}

static void gdma_clr_finish_interrupt_bit(UINT32 channel)
{
    UINT32 reg_val;

    reg_val = ( 0x1 << channel);
    
    #if (CFG_SOC_NAME == SOC_BK7231)
    REG_WRITE(GENER_DMA_REG20_DMA_INT_STATUS, reg_val);
    #else
    REG_WRITE(GENER_DMA_REG38_DMA_INT_STATUS, reg_val);
    #endif // (CFG_SOC_NAME == SOC_BK7231)
}

static UINT32 gdma_get_half_finish_interrupt_bit(UINT32 channel)
{
    #if (CFG_SOC_NAME == SOC_BK7231)
    UINT32 reg_val = REG_READ(GENER_DMA_REG20_DMA_INT_STATUS);
    #else
    UINT32 reg_val = REG_READ(GENER_DMA_REG38_DMA_INT_STATUS);
    #endif // (CFG_SOC_NAME == SOC_BK7231)

    reg_val &= (GENER_DMA_HFIN_INT_STATUS_MASK << GENER_DMA_HFIN_INT_STATUS_POSI);
    reg_val &= (0x1 << (channel + GENER_DMA_HFIN_INT_STATUS_POSI));
    reg_val = (reg_val >> (channel + GENER_DMA_HFIN_INT_STATUS_POSI));
    return reg_val;
}

static void gdma_clr_half_finish_interrupt_bit(UINT32 channel)
{
    UINT32 reg_val;

    reg_val = ( 0x1 << (channel + GENER_DMA_HFIN_INT_STATUS_POSI));

    #if (CFG_SOC_NAME == SOC_BK7231)
    REG_WRITE(GENER_DMA_REG20_DMA_INT_STATUS, reg_val);
    #else
    REG_WRITE(GENER_DMA_REG38_DMA_INT_STATUS, reg_val);
    #endif // (CFG_SOC_NAME == SOC_BK7231)
}

static void gdma_enable_interrupt(void)
{
    UINT32 param;
    param = (IRQ_GDMA_BIT);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);
}

static void gdma_disable_interrupt(void)
{
    UINT32 param;
    param = (IRQ_GDMA_BIT);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param);
}

#if (CFG_SOC_NAME == SOC_BK7221U)
static void gdma_set_src_pause_addr(UINT32 channel, UINT32 addr)
{
    UINT32 param = addr;
    UINT32 reg_addr = GENER_DMA0_REG40_SRC_PAUSE_ADDR + (channel * 4);

    //os_printf("set src pause:%d, %x\r\n", channel, addr);
    REG_WRITE(reg_addr, addr);
}

static UINT32 gdma_get_src_pause_addr(UINT32 channel)
{
    UINT32 reg_addr = GENER_DMA0_REG40_SRC_PAUSE_ADDR + (channel * 4);
    UINT32 reg_val = REG_READ(reg_addr);

    return reg_val;
}

static void gdma_set_dest_pause_addr(UINT32 channel, UINT32 addr)
{
    UINT32 param = addr;
    UINT32 reg_addr = GENER_DMA0_REG48_DST_PAUSE_ADDR + (channel * 4);

    REG_WRITE(reg_addr, addr);
}

static UINT32 gdma_get_dest_pause_addr(UINT32 channel)
{
    UINT32 reg_addr = GENER_DMA0_REG48_DST_PAUSE_ADDR + (channel * 4);
    UINT32 reg_val = REG_READ(reg_addr);

    return reg_val;
}

static UINT32 gdma_get_src_read_addr(UINT32 channel)
{
    UINT32 reg_addr = GENER_DMA0_REG50_SRC_RD_ADDR + (channel * 4);
    UINT32 reg_val = REG_READ(reg_addr);

    //os_printf("get src read:%d, %x\r\n", channel, reg_val);
    return reg_val;
}

static UINT32 gdma_get_dest_write_addr(UINT32 channel)
{
    UINT32 reg_addr = GENER_DMA0_REG58_DST_WR_ADDR + (channel * 4);
    UINT32 reg_val = REG_READ(reg_addr);

    return reg_val;
}
#endif // (CFG_SOC_NAME == SOC_BK7221U)

/*---------------------------------------------------------------------------*/
static int gdma_get_received_len(UINT32 channel)
{
    if(channel >= GDMA_CHANNEL_MAX) {
        GENER_DMA_WPRT("gdma channel err\r\n");
        return 0;
    }
    
    return (int)gdma_get_remain_len(channel); 
}

// no loop src, no loop dst
static void gdma_congfig_type0(GDMACFG_TPYES_PTR cfg)
{
    UINT32 reg_addr;
    if(cfg->channel >= GDMA_CHANNEL_MAX)
    {
        GENER_DMA_WPRT("gdma channel err\r\n");
        return;
    }
    reg_addr = GENER_DMA0_REG0_CONF + (cfg->channel * 8 * 4);
    REG_WRITE(reg_addr, 0);

    gdma_cfg_srcaddr_increase(cfg->channel, cfg->srcptr_incr);
    gdma_cfg_dstaddr_increase(cfg->channel, cfg->dstptr_incr);
    gdma_cfg_srcdata_width(cfg->channel, cfg->srcdat_width);
    gdma_cfg_dstdata_width(cfg->channel, cfg->dstdat_width);
    gdma_set_channel_prioprity(cfg->channel, cfg->prio);
    //gdma_cfg_finish_inten(cfg->channel, 1);

    gdma_set_dst_start_addr(cfg->channel, cfg->dst_start_addr);
    gdma_set_src_start_addr(cfg->channel, cfg->src_start_addr);
}

// loop src, no loop dst
static void gdma_congfig_type1(GDMACFG_TPYES_PTR cfg)
{
    UINT32 reg_addr;
    if(cfg->channel >= GDMA_CHANNEL_MAX)
    {
        GENER_DMA_WPRT("gdma channel err\r\n");
        return;
    }
    reg_addr = GENER_DMA0_REG0_CONF + (cfg->channel * 8 * 4);
    REG_WRITE(reg_addr, 0);

    gdma_cfg_srcaddr_loop(cfg->channel, 1);
    gdma_cfg_srcaddr_increase(cfg->channel, cfg->srcptr_incr);
    gdma_cfg_dstaddr_increase(cfg->channel, cfg->dstptr_incr);
    gdma_cfg_srcdata_width(cfg->channel, cfg->srcdat_width);
    gdma_cfg_dstdata_width(cfg->channel, cfg->dstdat_width);
    gdma_set_channel_prioprity(cfg->channel, cfg->prio);
    //gdma_cfg_finish_inten(cfg->channel, 1);

    gdma_set_dst_start_addr(cfg->channel, cfg->dst_start_addr);
    gdma_set_src_start_addr(cfg->channel, cfg->src_start_addr);

    gdma_set_srcloop_startaddr(cfg->channel, cfg->u.type1.src_loop_start_addr);
    gdma_set_srcloop_endaddr(cfg->channel, cfg->u.type1.src_loop_end_addr);
}

// no loop src, loop dst
static void gdma_congfig_type2(GDMACFG_TPYES_PTR cfg)
{
    UINT32 reg_addr;
    if(cfg->channel >= GDMA_CHANNEL_MAX)
    {
        GENER_DMA_WPRT("gdma channel err\r\n");
        return;
    }
    reg_addr = GENER_DMA0_REG0_CONF + (cfg->channel * 8 * 4);
    REG_WRITE(reg_addr, 0);

    gdma_cfg_dstaddr_loop(cfg->channel, 1);
    gdma_cfg_srcaddr_increase(cfg->channel, cfg->srcptr_incr);
    gdma_cfg_dstaddr_increase(cfg->channel, cfg->dstptr_incr);
    gdma_cfg_srcdata_width(cfg->channel, cfg->srcdat_width);
    gdma_cfg_dstdata_width(cfg->channel, cfg->dstdat_width);
    gdma_set_channel_prioprity(cfg->channel, cfg->prio);
    //gdma_cfg_finish_inten(cfg->channel, 1);

    gdma_set_dst_start_addr(cfg->channel, cfg->dst_start_addr);
    gdma_set_src_start_addr(cfg->channel, cfg->src_start_addr);

    gdma_set_dstloop_startaddr(cfg->channel, cfg->u.type2.dst_loop_start_addr);
    gdma_set_dstloop_endaddr(cfg->channel, cfg->u.type2.dst_loop_end_addr);
}

// loop src, loop dst
static void gdma_congfig_type3(GDMACFG_TPYES_PTR cfg)
{
    UINT32 reg_addr;
    if(cfg->channel >= GDMA_CHANNEL_MAX)
    {
        GENER_DMA_WPRT("gdma channel err\r\n");
        return;
    }
    reg_addr = GENER_DMA0_REG0_CONF + (cfg->channel * 8 * 4);
    REG_WRITE(reg_addr, 0);

    gdma_cfg_srcaddr_loop(cfg->channel, 1);
    gdma_cfg_dstaddr_loop(cfg->channel, 1);

    gdma_cfg_srcaddr_increase(cfg->channel, cfg->srcptr_incr);
    gdma_cfg_dstaddr_increase(cfg->channel, cfg->dstptr_incr);
    gdma_cfg_srcdata_width(cfg->channel, cfg->srcdat_width);
    gdma_cfg_dstdata_width(cfg->channel, cfg->dstdat_width);
    gdma_set_channel_prioprity(cfg->channel, cfg->prio);
    //gdma_cfg_finish_inten(cfg->channel, 1);

    gdma_set_dst_start_addr(cfg->channel, cfg->dst_start_addr);
    gdma_set_src_start_addr(cfg->channel, cfg->src_start_addr);

    gdma_set_srcloop_startaddr(cfg->channel, cfg->u.type3.src_loop_start_addr);
    gdma_set_srcloop_endaddr(cfg->channel, cfg->u.type3.src_loop_end_addr);
    gdma_set_dstloop_startaddr(cfg->channel, cfg->u.type3.dst_loop_start_addr);
    gdma_set_dstloop_endaddr(cfg->channel, cfg->u.type3.dst_loop_end_addr);
}


#if(CFG_SOC_NAME != SOC_BK7231)
// memery to module fifo to , so dest is register -- src is memery and loop
static void gdma_congfig_type4(GDMACFG_TPYES_PTR cfg)
{
    UINT32 reg_addr;
    if(cfg->channel >= GDMA_CHANNEL_MAX)
    {
        GENER_DMA_WPRT("gdma channel err\r\n");
        return;
    }
    reg_addr = GENER_DMA0_REG0_CONF + (cfg->channel * 8 * 4);
    REG_WRITE(reg_addr, 0);

    gdma_set_src_reqmux(cfg->channel, cfg->src_module);
    gdma_set_dst_reqmux(cfg->channel, cfg->dst_module);
    gdma_cfg_work_mode(cfg->channel, 1);  // repeat mode

    gdma_cfg_srcaddr_loop(cfg->channel, 1); 

    gdma_cfg_dstaddr_increase(cfg->channel, 0);
    gdma_cfg_srcaddr_increase(cfg->channel, cfg->srcptr_incr);
    
    gdma_cfg_srcdata_width(cfg->channel, cfg->srcdat_width);
    gdma_cfg_dstdata_width(cfg->channel, cfg->dstdat_width);
    gdma_set_channel_prioprity(cfg->channel, cfg->prio);

    gdma_set_dst_start_addr(cfg->channel, cfg->dst_start_addr);
    gdma_set_src_start_addr(cfg->channel, cfg->src_start_addr);

    gdma_set_srcloop_startaddr(cfg->channel, cfg->u.type4.src_loop_start_addr);
    gdma_set_srcloop_endaddr(cfg->channel, cfg->u.type4.src_loop_end_addr);

    if(cfg->fin_handler) {
        gdma_cfg_finish_inten(cfg->channel, 1);
        p_dma_fin_handler[cfg->channel] = cfg->fin_handler;
    }
    
    if(cfg->half_fin_handler) {
        gdma_cfg_half_fin_inten(cfg->channel, 1);
        p_dma_hfin_handler[cfg->channel] = cfg->half_fin_handler;
    }
}

// module fifo to memery, so src is register -- dst is memery and loop
static void gdma_congfig_type5(GDMACFG_TPYES_PTR cfg)
{
    UINT32 reg_addr;
    if(cfg->channel >= GDMA_CHANNEL_MAX)
    {
        GENER_DMA_WPRT("gdma channel err\r\n");
        return;
    }
    reg_addr = GENER_DMA0_REG0_CONF + (cfg->channel * 8 * 4);
    REG_WRITE(reg_addr, 0);

    gdma_set_src_reqmux(cfg->channel, cfg->src_module);
    gdma_set_dst_reqmux(cfg->channel, cfg->dst_module);
    gdma_cfg_work_mode(cfg->channel, 1);  // repeat mode

    gdma_cfg_dstaddr_loop(cfg->channel, 1); 

    gdma_cfg_srcaddr_increase(cfg->channel, 0);
    gdma_cfg_dstaddr_increase(cfg->channel, cfg->dstptr_incr);
    
    gdma_cfg_srcdata_width(cfg->channel, cfg->srcdat_width);
    gdma_cfg_dstdata_width(cfg->channel, cfg->dstdat_width);
    gdma_set_channel_prioprity(cfg->channel, cfg->prio);

    gdma_set_dst_start_addr(cfg->channel, cfg->dst_start_addr);
    gdma_set_src_start_addr(cfg->channel, cfg->src_start_addr);

    gdma_set_dstloop_startaddr(cfg->channel, cfg->u.type5.dst_loop_start_addr);
    gdma_set_dstloop_endaddr(cfg->channel, cfg->u.type5.dst_loop_end_addr);

    if(cfg->fin_handler) {
        gdma_cfg_finish_inten(cfg->channel, 1);
        p_dma_fin_handler[cfg->channel] = cfg->fin_handler;
    }
    
    if(cfg->half_fin_handler) {
        gdma_cfg_half_fin_inten(cfg->channel, 1);
        p_dma_hfin_handler[cfg->channel] = cfg->half_fin_handler;
    }
}

static void gdma_congfig_type6(GDMACFG_TPYES_PTR cfg)
{
    GENER_DMA_WPRT("no gdma_congfig_type6\r\n");
    return;
}
#endif

static UINT32 gdma_enable( GDMA_DO_PTR do_st )
{
    GLOBAL_INT_DECLARATION();
    if(!do_st->length)
        return 0;

    GLOBAL_INT_DISABLE();
    gdma_set_dst_start_addr(do_st->channel, do_st->dst_addr);
    gdma_set_src_start_addr(do_st->channel, do_st->src_addr);
    gdma_set_transfer_length(do_st->channel, do_st->length);

    GENER_DMA_PRT("%p,%p,%d\r\n", do_st->dst_addr, do_st->src_addr, do_st->length);

    gdma_set_dma_en(do_st->channel, 1);
    #if (CFG_SOC_NAME == SOC_BK7231)
    while(!gdma_get_finish_interrupt_bit(do_st->channel));
    gdma_clr_finish_interrupt_bit(do_st->channel);
    #else
    while(gdma_get_dma_en(do_st->channel));
    //gdma_clr_finish_interrupt_bit(do_st->channel);
    #endif // (CFG_SOC_NAME == SOC_BK7231)
    GLOBAL_INT_RESTORE();

    return do_st->length;
}

/*---------------------------------------------------------------------------*/
void gdma_flush(void)
{
	UINT32 status;

	gdma_set_dma_en(GDMA_CHANNEL_0, 0);
	gdma_set_dma_en(GDMA_CHANNEL_1, 0);
	gdma_set_dma_en(GDMA_CHANNEL_2, 0);
	gdma_set_dma_en(GDMA_CHANNEL_3, 0);
	
	#if (CFG_SOC_NAME != SOC_BK7231)
	gdma_set_dma_en(GDMA_CHANNEL_4, 0);
	gdma_set_dma_en(GDMA_CHANNEL_5, 0);
	#endif
	
    status = REG_READ(GENER_DMA_REG38_DMA_INT_STATUS);
	REG_WRITE(GENER_DMA_REG38_DMA_INT_STATUS, status);
}

void gdma_init(void)
{
    GDMACFG_TPYES_ST cfg;

    intc_service_register(IRQ_GENERDMA, PRI_IRQ_GENERDMA, gdma_isr);
    sddev_register_dev(GDMA_DEV_NAME, &gdma_op);

    #if (CFG_SOC_NAME != SOC_BK7231)
    for(int i=0; i<GDMA_CHANNEL_MAX; i++) {
        p_dma_fin_handler[i] = NULL;
        p_dma_hfin_handler[i] = NULL;
    }
    #endif // (CFG_SOC_NAME != SOC_BK7231)
    
    os_memset(&cfg, 0, sizeof(GDMACFG_TPYES_ST));
	gdma_flush();

    cfg.dstdat_width = 32;
    cfg.srcdat_width = 32;
    cfg.dstptr_incr = 1;
    cfg.srcptr_incr = 1;
    cfg.src_start_addr = NULL;
    cfg.dst_start_addr = NULL;

    cfg.channel = GDMA_CHANNEL_0;
    cfg.prio = 1;
    gdma_congfig_type0(&cfg);

    cfg.channel = GDMA_CHANNEL_1;
    cfg.prio = 0;
    cfg.u.type1.src_loop_start_addr = NULL;
    cfg.u.type1.src_loop_end_addr = NULL;
    gdma_congfig_type1(&cfg);

    cfg.channel = GDMA_CHANNEL_2;
    cfg.prio = 2;
    cfg.u.type2.dst_loop_start_addr = NULL;
    cfg.u.type2.dst_loop_end_addr = NULL;
    gdma_congfig_type2(&cfg);
    
    cfg.channel = GDMA_CHANNEL_3;
    cfg.prio = 2;
    cfg.u.type3.src_loop_start_addr = NULL;
    cfg.u.type3.src_loop_end_addr = NULL;
    cfg.u.type3.dst_loop_start_addr = NULL;
    cfg.u.type3.dst_loop_end_addr = NULL;
    gdma_congfig_type3(&cfg);
    
#if (CFG_SOC_NAME != SOC_BK7231)
    cfg.channel = GDMA_CHANNEL_4;
    cfg.prio = 0;
    cfg.u.type1.src_loop_start_addr = NULL;
    cfg.u.type1.src_loop_end_addr = NULL;

    cfg.half_fin_handler = NULL;
    cfg.src_module = GDMA_X_SRC_DST_RESERVE;
    cfg.dst_module = GDMA_X_SRC_DST_RESERVE;
    gdma_congfig_type1(&cfg);   

    cfg.channel = GDMA_CHANNEL_5;
    cfg.prio = 0;
    cfg.u.type2.dst_loop_start_addr = NULL;
    cfg.u.type2.dst_loop_end_addr = NULL;

    cfg.half_fin_handler = NULL;
    cfg.src_module = GDMA_X_SRC_DST_RESERVE;
    cfg.dst_module = GDMA_X_SRC_DST_RESERVE;
    gdma_congfig_type2(&cfg); 

    gdma_set_priority(0);  // round-robin mode, all dma priority are same
    gdma_enable_interrupt();
#endif // (CFG_SOC_NAME != SOC_BK7231)
}

void gdma_exit(void)
{
    gdma_disable_interrupt();
    
    REG_WRITE(GENER_DMA0_REG0_CONF, 0);
    REG_WRITE(GENER_DMA1_REG8_CONF, 0);
    REG_WRITE(GENER_DMA2_REG10_CONF, 0);
    REG_WRITE(GENER_DMA3_REG18_CONF, 0);

    #if (CFG_SOC_NAME == SOC_BK7231)
    REG_WRITE(GENER_DMA_REG20_DMA_INT_STATUS,
    REG_READ(GENER_DMA_REG20_DMA_INT_STATUS));
    #else
    REG_WRITE(GENER_DMA4_REG20_CONF, 0);
    REG_WRITE(GENER_DMA5_REG28_CONF, 0);
    
    REG_WRITE(GENER_DMA_REG38_DMA_INT_STATUS,
    REG_READ(GENER_DMA_REG38_DMA_INT_STATUS));
    #endif // (CFG_SOC_NAME == SOC_BK7231)

    sddev_unregister_dev(GDMA_DEV_NAME);
}

UINT32 gdma_ctrl(UINT32 cmd, void *param)
{
    UINT32 ret;
    GDMA_CFG_PTR dma_cfg;
    ret = ICU_SUCCESS;

    ASSERT(param);

    dma_cfg = (GDMA_CFG_PTR)param;

    switch(cmd)
    {
    case CMD_GDMA_SET_DMA_ENABLE:
        gdma_set_dma_en(dma_cfg->channel, dma_cfg->param);
        break;
    case CMD_GDMA_CFG_FIN_INT_ENABLE:
        gdma_cfg_finish_inten(dma_cfg->channel, dma_cfg->param);
        break;
    case CMD_GDMA_CFG_HFIN_INT_ENABLE:
        gdma_cfg_half_fin_inten(dma_cfg->channel, dma_cfg->param);
        break;
    #if (CFG_SOC_NAME != SOC_BK7231)    
    case CMD_GDMA_CFG_WORK_MODE:
        gdma_cfg_work_mode(dma_cfg->channel, dma_cfg->param);
        break;
    #endif // (CFG_SOC_NAME != SOC_BK7231)      
    case CMD_GDMA_CFG_SRCDATA_WIDTH:
        gdma_cfg_srcdata_width(dma_cfg->channel, dma_cfg->param);
        break;
    case CMD_GDMA_CFG_DSTDATA_WIDTH:
        gdma_cfg_dstdata_width(dma_cfg->channel, dma_cfg->param);
        break;
    case CMD_GDMA_CFG_SRCADDR_INCREASE:
        gdma_cfg_srcaddr_increase(dma_cfg->channel, dma_cfg->param);
        break;
    case CMD_GDMA_CFG_DSTADDR_INCREASE:
        gdma_cfg_dstaddr_increase(dma_cfg->channel, dma_cfg->param);
        break;
    case CMD_GDMA_CFG_SRCADDR_LOOP:
        gdma_cfg_srcaddr_loop(dma_cfg->channel, dma_cfg->param);
        break;
    case CMD_GDMA_CFG_DSTADDR_LOOP:
        gdma_cfg_dstaddr_loop(dma_cfg->channel, dma_cfg->param);
        break;
    case CMD_GDMA_SET_CHNL_PRIO:
        gdma_set_channel_prioprity(dma_cfg->channel, dma_cfg->param);
        break;
    case CMD_GDMA_SET_TRANS_LENGTH:
        gdma_set_transfer_length(dma_cfg->channel, dma_cfg->param);
        break;
    case CMD_GDMA_SET_DST_START_ADDR:
        gdma_set_dst_start_addr(dma_cfg->channel, (void *)dma_cfg->param);
        break;
    case CMD_GDMA_SET_SRC_START_ADDR:
        gdma_set_src_start_addr(dma_cfg->channel, (void *)dma_cfg->param);
        break;
    case CMD_GDMA_SET_DST_LOOP_ENDADDR:
        gdma_set_dstloop_endaddr(dma_cfg->channel, (void *)dma_cfg->param);
        break;
    case CMD_GDMA_SET_DST_LOOP_STARTADDR:
        gdma_set_dstloop_startaddr(dma_cfg->channel, (void *)dma_cfg->param);
        break;
    case CMD_GDMA_SET_SRC_LOOP_ENDADDR:
        gdma_set_srcloop_endaddr(dma_cfg->channel, (void *)dma_cfg->param);
        break;
    case CMD_GDMA_SET_SRC_LOOP_STARTADDR:
        gdma_set_srcloop_startaddr(dma_cfg->channel, (void *)dma_cfg->param);
        break;
    case CMD_GDMA_GET_REMAIN_LENGTH:
        ret = gdma_get_remain_len(dma_cfg->channel);
        break;  
    #if (CFG_SOC_NAME != SOC_BK7231)
    case CMD_GDMA_SET_DST_REQ_MUX:
        gdma_set_dst_reqmux(dma_cfg->channel, dma_cfg->param);
        break;
    case CMD_GDMA_SET_SRC_REQ_MUX:
        gdma_set_src_reqmux(dma_cfg->channel, dma_cfg->param);
        break;  
    case CMD_GDMA_SET_DTCM_WRITE_WORD:
        gdma_set_dctm_write_wd(dma_cfg->channel, dma_cfg->param);
        break;        
    case CMD_GDMA_SET_FIN_CNT:
        ret = gdma_get_finish_cnt((UINT32)param);
        break;
    case CMD_GDMA_SET_HFIN_CNT:
        ret = gdma_get_half_finish_cnt(dma_cfg->channel);
        break;
    case CMD_GDMA_SET_PRIO_MODE:
        gdma_set_priority(dma_cfg->param);
        break;
    #endif // (CFG_SOC_NAME != SOC_BK7231)   
    case CMD_GDMA_GET_FIN_INT_STATUS:
        ret = gdma_get_finish_interrupt_bit(dma_cfg->channel);
        break;
    case CMD_GDMA_CLR_FIN_INT_STATUS:
        gdma_clr_finish_interrupt_bit(dma_cfg->channel);
        break;
    case CMD_GDMA_GET_HFIN_INT_STATUS:
        ret = gdma_get_half_finish_interrupt_bit(dma_cfg->channel);
        break;
    case CMD_GDMA_CLR_HFIN_INT_STATUS:
        gdma_clr_half_finish_interrupt_bit(dma_cfg->channel);
        break;
    case CMD_GDMA_CFG_TYPE0:
        gdma_congfig_type0((GDMACFG_TPYES_PTR)param);
        break;
    case CMD_GDMA_CFG_TYPE1:
        gdma_congfig_type1((GDMACFG_TPYES_PTR)param);
        break;
    case CMD_GDMA_CFG_TYPE2:
        gdma_congfig_type2((GDMACFG_TPYES_PTR)param);
        break;
    case CMD_GDMA_CFG_TYPE3:
        gdma_congfig_type3((GDMACFG_TPYES_PTR)param);
        break;
#if (CFG_SOC_NAME != SOC_BK7231)        
    case CMD_GDMA_CFG_TYPE4:
        gdma_congfig_type4((GDMACFG_TPYES_PTR)param);
        break;
    case CMD_GDMA_CFG_TYPE5:
        gdma_congfig_type5((GDMACFG_TPYES_PTR)param);
        break;
    case CMD_GDMA_CFG_TYPE6:
        gdma_congfig_type6((GDMACFG_TPYES_PTR)param);
        break;
#endif        
    case CMD_GDMA_ENABLE:
        gdma_enable((GDMA_DO_PTR)param);
        break;
    #if (CFG_SOC_NAME != SOC_BK7231)
    case CMD_GDMA_GET_LEFT_LEN:
        ret = gdma_get_received_len((UINT32)param);
        break;
    #endif // (CFG_SOC_NAME != SOC_BK7231)
    #if (CFG_SOC_NAME == SOC_BK7221U)
    case CMD_GDMA_SET_SRC_PAUSE_ADDR:
        gdma_set_src_pause_addr(dma_cfg->channel, dma_cfg->param);
        break;
    case CMD_GDMA_GET_SRC_PAUSE_ADDR:
        ret = gdma_get_src_pause_addr(dma_cfg->channel);
        break;     
    case CMD_GDMA_SET_DST_PAUSE_ADDR:
        gdma_set_dest_pause_addr(dma_cfg->channel, dma_cfg->param);
        break;    
    case CMD_GDMA_GET_DST_PAUSE_ADDR:
        ret = gdma_get_src_pause_addr(dma_cfg->channel);
        break;
    case CMD_GDMA_GET_SRC_READ_ADDR:
        ret = gdma_get_src_read_addr(dma_cfg->channel);
        break;
    case CMD_GDMA_GET_DST_WRITE_ADDR:
        ret = gdma_get_dest_write_addr(dma_cfg->channel);
        break;         
    #endif // (CFG_SOC_NAME == SOC_BK7221U)
    default:
        break;
    }

    return ret;
}

/*Before using this fuction, please config general dma moudule first*/
void *gdma_memcpy(void *out, const void *in, UINT32 n)
{
    GLOBAL_INT_DECLARATION();
    GDMA_DO_ST do_st;
    
    do_st.channel = GDMA_CHANNEL_0;
    do_st.src_addr = (void*)in;
    do_st.length = n;
    do_st.dst_addr = out;
    GLOBAL_INT_DISABLE();
    gdma_enable(&do_st);
    GLOBAL_INT_RESTORE();    

    return out;
}

static void gdma_isr(void)
{
    #if (CFG_SOC_NAME == SOC_BK7231)
    GENER_DMA_PRT("gdma_isr is null\r\n");
    #else
    UINT32 i, status, rev_len, cmp_bit;

    status = REG_READ(GENER_DMA_REG38_DMA_INT_STATUS);

    for(i = 0; i < GDMA_CHANNEL_MAX; i++)
    {
        rev_len = gdma_get_transfer_length(i) - gdma_get_remain_len(i); 
        rev_len = rev_len;
        
        cmp_bit = (1 << (i+GENER_DMA_HFIN_INT_STATUS_POSI));
        if(status & cmp_bit)
        {
            if(p_dma_hfin_handler[i]){
                p_dma_hfin_handler[i](0);
                REG_WRITE(GENER_DMA_REG38_DMA_INT_STATUS, cmp_bit);
            }
        }

        cmp_bit = (1 << (i+GENER_DMA_FIN_INT_STATUS_POSI));
        if(status & cmp_bit) 
        {
            if(p_dma_fin_handler[i]){
                p_dma_fin_handler[i](1);
                REG_WRITE(GENER_DMA_REG38_DMA_INT_STATUS, cmp_bit);
            }
        }
    }
    #endif //  (CFG_SOC_NAME == SOC_BK7231)
}

#endif  // CFG_GENERAL_DMA


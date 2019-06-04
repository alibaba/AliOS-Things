#include "include.h"
#include "ring_buffer_dma_read.h"
#include "arch.h"
#include "mem_pub.h"
#include "drv_model_pub.h"
#include "uart_pub.h"

#if CFG_GENERAL_DMA
#include "general_dma_pub.h"

#define RWP_SAFE_INTERVAL           (4)

#define RB_DMA_RD_MEMCPY                   os_memcpy
#define RB_DMA_RD_INT_DECLARATION()        
#define RB_DMA_RD_INT_DISABLE()            
#define RB_DMA_RD_INT_RESTORE()
#define RB_DMA_RD_PRT                      //os_printf                   

void rb_init_dma_read(RB_DMA_RD_PTR rb, UINT8 *addr, UINT32 capacity, UINT32 ch)
{
    GDMA_CFG_ST en_cfg;
    RB_DMA_RD_INT_DECLARATION();

    RB_DMA_RD_INT_DISABLE();
    rb->address  = addr;
    rb->capacity = capacity;
    rb->wp       = 0;
    rb->rp       = 0;
    rb->dma_ch   = ch;
    RB_DMA_RD_INT_RESTORE();

    en_cfg.channel = rb->dma_ch;
    en_cfg.param = (UINT32)(rb->address + rb->wp);
    RB_DMA_RD_PRT("init set src:%x\r\n", en_cfg.param);
    sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_SRC_PAUSE_ADDR, &en_cfg);
}

void rb_clear_dma_read(RB_DMA_RD_PTR rb)
{
    GDMA_CFG_ST en_cfg;
    RB_DMA_RD_INT_DECLARATION();    
 
    RB_DMA_RD_INT_DISABLE();   
    rb->wp    = 0;
    rb->rp    = 0;
    RB_DMA_RD_INT_RESTORE();

    en_cfg.channel = rb->dma_ch;
    en_cfg.param = (UINT32)(rb->address + rb->wp);
    RB_DMA_RD_PRT("clear set src:%x\r\n", en_cfg.param);
    sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_SRC_PAUSE_ADDR, &en_cfg);
}


UINT32 rb_write_dma_read(RB_DMA_RD_PTR rb, UINT8 *buffer, UINT32 size, UINT32 count)
{
    UINT32 remain_bytes;
    UINT32 write_bytes = size * count;
    UINT32 rp;
    GDMA_CFG_ST en_cfg;
    
    RB_DMA_RD_INT_DECLARATION();
    
    if(write_bytes == 0) 
        return 0;

    en_cfg.channel = rb->dma_ch;
    en_cfg.param = 0;
    rp = sddev_control(GDMA_DEV_NAME, CMD_GDMA_GET_SRC_READ_ADDR, &en_cfg);
    RB_DMA_RD_PRT("write get src_rd:%x\r\n", rp);
    rp -= (UINT32)rb->address;
    rb->rp = rp;

    if(rb->wp >= rp)
    {
        remain_bytes = rb->capacity - rb->wp + rp;

        if(remain_bytes >= write_bytes + RWP_SAFE_INTERVAL)
        {
            remain_bytes = rb->capacity - rb->wp;

            if(remain_bytes >= write_bytes)
            {
                RB_DMA_RD_MEMCPY(&rb->address[rb->wp], buffer, write_bytes);
                RB_DMA_RD_INT_DISABLE();
                rb->wp += write_bytes;
                RB_DMA_RD_INT_RESTORE();
            }
            else
            {
                RB_DMA_RD_MEMCPY(&rb->address[rb->wp], buffer, remain_bytes);
                RB_DMA_RD_INT_DISABLE();
                rb->wp = write_bytes - remain_bytes;
                RB_DMA_RD_INT_RESTORE();
                RB_DMA_RD_MEMCPY(&rb->address[0], &buffer[remain_bytes], rb->wp);
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        remain_bytes = rp - rb->wp;

        if(remain_bytes >= write_bytes + RWP_SAFE_INTERVAL)
        {
            RB_DMA_RD_MEMCPY(&rb->address[rb->wp], buffer, write_bytes);
            RB_DMA_RD_INT_DISABLE();
            rb->wp += write_bytes;
            RB_DMA_RD_INT_RESTORE();
        }
        else
        {
            return 0;
        }
    }

    if(rb->wp >= rb->capacity && rb->rp)
    {
        RB_DMA_RD_INT_DISABLE();
        rb->wp = 0;
        RB_DMA_RD_INT_RESTORE();
    }

    en_cfg.channel = rb->dma_ch;
    en_cfg.param = (UINT32)(rb->address + rb->wp);
    RB_DMA_RD_PRT("write set src:%x\r\n", en_cfg.param);
    sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_SRC_PAUSE_ADDR, &en_cfg);

    return write_bytes;
}

UINT32 rb_get_fill_size_dma_read(RB_DMA_RD_PTR rb)
{
    UINT32 fill_size, rp;
    GDMA_CFG_ST en_cfg;

    en_cfg.channel = rb->dma_ch;
    rp = sddev_control(GDMA_DEV_NAME, CMD_GDMA_GET_SRC_READ_ADDR, &en_cfg);
    RB_DMA_RD_PRT("fillsize get src_rd:%x\r\n", rp);
    rp -= (UINT32)rb->address;
    rb->rp = rp;

    fill_size = rb->wp >= rb->rp ? rb->wp - rb->rp : rb->capacity - rb->rp + rb->wp;

    return fill_size;
}

UINT32 rb_get_free_size_dma_read(RB_DMA_RD_PTR rb)
{
    UINT32 free_size, rp;
    GDMA_CFG_ST en_cfg;

    en_cfg.channel = rb->dma_ch;
    rp = sddev_control(GDMA_DEV_NAME, CMD_GDMA_GET_SRC_READ_ADDR, &en_cfg);
    RB_DMA_RD_PRT("freesize get src_rd:%x\r\n", rp);
    rp -= (UINT32)rb->address;
    rb->rp = rp;

    free_size = rb->wp >= rb->rp ? rb->capacity - rb->wp + rb->rp : rb->rp - rb->wp;

    return free_size > RWP_SAFE_INTERVAL ? free_size - RWP_SAFE_INTERVAL : 0;
}

#endif // CFG_GENERAL_DMA
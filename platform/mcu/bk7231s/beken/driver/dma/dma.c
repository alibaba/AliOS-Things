/**
 ****************************************************************************************
 *
 * @file dma.c
 *
 * @brief IPC DMA utility functions
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
// for NULL
#include <stddef.h>
#include <string.h>

#include "dma.h"
#include "reg_dma.h"
#include "co_utils.h"
#include "ke_env.h"
#include "rxl_cntrl.h"
#include "ke_event.h"

#include "include.h"
#include "arm_arch.h"
#include "mem_pub.h"
#include "uart_pub.h"

#if CFG_GENERAL_DMA
#include "general_dma_pub.h"
#endif

struct dma_env_tag dma_env;

void dma_init(void)
{
    memset(dma_env.last_dma, 0, sizeof(dma_env.last_dma));

    // configure the channel priorities
    dma_channel_priority_set(0x0C);

    // configure the interrupts (enable all interrupts)
    dma_int_unmask_set(DMA_LLI_IRQ_MASK);
}

uint32_t dma_push(struct dma_desc *first, struct dma_desc *last, int channel_idx)
{
    uint32_t push_len = 0;
    evt_field_t event = 0;
    uint32_t go_on_flag = 1;
    UINT32 access_category = 0;
    struct dma_desc *ddesc;

    ddesc = first;
    while(go_on_flag && ddesc)
    {
        ASSERT(ddesc->length < 2300);
#if  CFG_GENERAL_DMA
        gdma_memcpy((void *)ddesc->dest, (void *)ddesc->src, ddesc->length);
#else
        os_memcpy((void *)ddesc->dest, (void *)ddesc->src, ddesc->length);
#endif
        push_len += ddesc->length;

        if(ddesc == last)
        {
            go_on_flag = 0;
        }

        ddesc = (struct dma_desc *)ddesc->next;
    }

    switch(channel_idx)
    {
    case IPC_DMA_CHANNEL_CTRL_RX:
        break;

    case IPC_DMA_CHANNEL_DATA_RX:
        rxl_dma_int_handler();
        break;

    case IPC_DMA_CHANNEL_CTRL_TX:
        break;

    case IPC_DMA_CHANNEL_DATA_TX:
        access_category = AC_VI;
        event = (ke_get_ac_payload_bit(access_category));
        break;

    default:
        break;
    }

    if(event)
    {
        ke_evt_set(event);
    }

    return push_len;
}
// eof


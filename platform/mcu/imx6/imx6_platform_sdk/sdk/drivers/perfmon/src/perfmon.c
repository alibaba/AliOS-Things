/*
 * Copyright (c) 2012, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "sdk.h"
#include "perfmon/perfmon.h"

static perfmon_res_p last_stats[PERFMON_INST];

/*!
 * Configure the PerfMon trap mode.
 *
 * @param   id perfmon instance PERFMON_IDx.
 * @param   addr_low lower address of the monitored range.
 * @param   addr_high higher address of the monitored range.
 * @param   range_mode monitor access inside or outside
 *                       the range (IN_RANGE/OUT_RANGE).
 */
void perfmon_set_trap_mode(perfmon_id_e id, uint32_t addr_low,
                           uint32_t addr_high, uint8_t range_mode)
{
    uint32_t base = perfmon_list[id].base;

    /* set the trap low address */
    reg32_write(base + HW_PERFMON_TRAP_ADDR_LOW, addr_low);
    /* set the trap high address */
    reg32_write(base + HW_PERFMON_TRAP_ADDR_HIGH, addr_high);
    /* set the trap mode : event occurs inside OR outside the defined
     * address range.
     */
    reg32_write(base + HW_PERFMON_CTRL_SET, range_mode);

    /* clear status flag */
    reg32_write(base + HW_PERFMON_CTRL_CLR, BM_PERFMON_CTRL_TRAP_IRQ);

    /* enable the trap mode and its interrupt */
    reg32_write(base + HW_PERFMON_CTRL_SET,
                BM_PERFMON_CTRL_TRAP_ENABLE | BM_PERFMON_CTRL_TRAP_IRQ_EN);

    return;
}

/*!
 * Configure the PerfMon latency mode.
 *
 * @param   id perfmon instance PERFMON_IDx.
 * @param   lat_threshold latency threshold to generate an interrupt.
 */
void perfmon_set_latency_mode(perfmon_id_e id, uint32_t lat_threshold)
{
    uint32_t base = perfmon_list[id].base;

    /* set the latency threshold register */
    reg32_write(base + HW_PERFMON_LAT_THRESHOLD, lat_threshold & 0xFFF);

    /* clear status flag */
    reg32_write(base + HW_PERFMON_CTRL_CLR, BM_PERFMON_CTRL_LATENCY_IRQ);

    /* enable the latency mode with IRQ */
    reg32_write(base + HW_PERFMON_CTRL_SET,
                BM_PERFMON_CTRL_LATENCY_ENABLE | BM_PERFMON_CTRL_LATENCY_IRQ_EN);

    return;
}

/*!
 * Start monitoring the performance.
 *
 * @param   id perfmon instance PERFMON_IDx.
 * @param   mid monitored masters PERFMON_MIDx.
 * @param   trans monitored direction : WRITE_TRANS / READ_TRANS.
 */
void perfmon_start(perfmon_id_e id, perfmon_mid_e mid, perfmon_trans_e trans)
{
    uint32_t base = perfmon_list[id].base;

    /* clear all counters */
    reg32_write(base + HW_PERFMON_CTRL_SET, BM_PERFMON_CTRL_CLR);
    while (reg32_read(base + HW_PERFMON_CTRL) & BM_PERFMON_CTRL_CLR);

    /* clear all status flags */
    reg32_write(base + HW_PERFMON_CTRL_CLR, BM_PERFMON_CTRL_LATENCY_IRQ |
                                            BM_PERFMON_CTRL_TRAP_IRQ);

    /* set the monitored direction */
    if (trans == READ_TRANS)
        reg32_write(base + HW_PERFMON_CTRL_SET, BM_PERFMON_CTRL_READ_EN);
    else
        reg32_write(base + HW_PERFMON_CTRL_CLR, BM_PERFMON_CTRL_READ_EN);

    /* set MID to profile */
    reg32_write(base + HW_PERFMON_MASTER_EN, mid & 0xFFFF);

    /* clear the variable used to store the statistics */
    memset(last_stats[id], 0, sizeof(perfmon_res_t));

    /* start to monitor */
    reg32_write(base + HW_PERFMON_CTRL_SET, BM_PERFMON_CTRL_RUN);

    return;
}

/*!
 * Perform a snapshot of the monitored performances.
 *
 * @param   id perfmon instance PERFMON_IDx.
 */
void perfmon_do_snapshot(perfmon_id_e id)
{
    uint32_t base = perfmon_list[id].base;

    reg32_write(base + HW_PERFMON_CTRL_SET, BM_PERFMON_CTRL_SNAP);
    while (reg32_read(base + HW_PERFMON_CTRL) & BM_PERFMON_CTRL_SNAP) ;

    last_stats[id]->total_active_cycle = reg32_read(base + HW_PERFMON_ACTIVE_CYCLE);
    last_stats[id]->total_trans = reg32_read(base + HW_PERFMON_TRANSFER_COUNT);
    last_stats[id]->total_data = reg32_read(base + HW_PERFMON_DATA_COUNT);
    last_stats[id]->max_latency.u = reg32_read(base + HW_PERFMON_MAX_LATENCY);
    last_stats[id]->total_lat_cnt = reg32_read(base + HW_PERFMON_TOTAL_LATENCY);
    /* Clear the status. If this snapshot is triggered by an interrupt,
     * the status is updated in the interrupt routine.
     */
    last_stats[id]->status = 0;

    return;
}

/*!
 * Get the result of the monitored performance from a fresh snapshot,
 * or from the last one possibly issued after the interrupt of
 * an event (trap, latency). It optionally clears the counters.
 *
 * @param   id perfmon instance PERFMON_IDx.
 * @param   res monitoring results.
 * @param   snapshot perform a fresh snapshot TRUE/FALSE.
 * @param   clear_stat clear the statistics TRUE/FALSE.
 */
void perfmon_get_performance(perfmon_id_e id, perfmon_res_p res,
                             uint8_t snapshot, uint8_t clear_stat)
{
    uint32_t base = perfmon_list[id].base;

    /* perform a fresh snapshot, otherwise returns the last snapshot */
    if(snapshot == TRUE)
        perfmon_do_snapshot(id);

    /* copy the result to the application */
    memcpy(res, last_stats[id], sizeof(perfmon_res_t));

    /* clear all counters */
    if(clear_stat == TRUE)
    {
        reg32_write(base + HW_PERFMON_CTRL_SET, BM_PERFMON_CTRL_CLR);
        while (reg32_read(base + HW_PERFMON_CTRL) & BM_PERFMON_CTRL_CLR);
    }

    return;
}

/*!
 * Stop monitoring the performance.
 *
 * @param   id perfmon instance PERFMON_IDx.
 * @param   res monitoring results.
 */
void perfmon_stop(perfmon_id_e id, perfmon_res_p res)
{
    uint32_t base = perfmon_list[id].base;

    perfmon_do_snapshot(id);

    /* copy the result to the application */
    memcpy(res, last_stats[id], sizeof(perfmon_res_t));

    /* stop the monitoring */
    reg32_write(base + HW_PERFMON_CTRL_CLR, BM_PERFMON_CTRL_RUN);

    return;
}

/*!
 * Performance monitor interrupt routine.
 */
void perfmon_interrupt_routine(void)
{
    uint32_t i, base, status, status_mask = 0;

    for(i=0;i<PERFMON_INST;i++)
    {
        if(g_vectNum[CPU_0] == perfmon_list[i].irq_id)
        {
            /* get the base address of the perfmon that generated the interrupt */
            base= perfmon_list[i].base;

            status = reg32_read(base + HW_PERFMON_CTRL);

            /* if the interrupt for latency is enabled */
            if(status & BM_PERFMON_CTRL_LATENCY_IRQ_EN)
            {
                /* if the flag for latency is set */
                if(status & BM_PERFMON_CTRL_LATENCY_IRQ)
                    status_mask |= BM_PERFMON_CTRL_LATENCY_IRQ |
                                   BM_PERFMON_CTRL_LATENCY_IRQ_EN;
            }
            /* if the interrupt for trap mode is enabled */
            if(status & BM_PERFMON_CTRL_TRAP_IRQ_EN)
            {
                /* if the flag for trap mode is set */
                if(status & BM_PERFMON_CTRL_TRAP_IRQ)
                    status_mask |= BM_PERFMON_CTRL_TRAP_IRQ |
                                   BM_PERFMON_CTRL_TRAP_IRQ_EN;
            }

            /* clear flags and disable interrupts */
            reg32_write(base + HW_PERFMON_CTRL_CLR, status_mask);
            /* do a snapshot of the counters */
            perfmon_do_snapshot(i);
            /* update the status */
            last_stats[i]->status = status_mask;
        }
    }
}

/*!
 * Setup PerfMon interrupt. It enables or disables the related HW module
 * interrupt, and attached the related sub-routine into the vector table.
 *
 * @param   id perfmon instance PERFMON_IDx.
 * @param   state ENABLE or DISABLE the interrupt.
 */
void perfmon_setup_interrupt(perfmon_id_e id, uint8_t state)
{
    if (state == TRUE) {
        /* register the IRQ sub-routine */
        register_interrupt_routine(perfmon_list[id].irq_id, &perfmon_interrupt_routine);
        /* enable the IRQ */
        enable_interrupt(perfmon_list[id].irq_id, CPU_0, 0);
    } else
        /* disable the IRQ */
        disable_interrupt(perfmon_list[id].irq_id, CPU_0);
}

/*!
 * Initialize a performance monitor.
 *
 * @param   id perfmon instance PERFMON_IDx.
 */
void perfmon_open(perfmon_id_e id)
{
    uint32_t base = perfmon_list[id].base;

    /* switch on the clock
     * (for i.MX6, IOMUXC GPR11 has a bit to enable clocks to perfmon)
     */
    clock_gating_config(base, CLOCK_ON);

    /* get the PerfMon out of reset */
    reg32_write(base + HW_PERFMON_CTRL_CLR,
                BM_PERFMON_CTRL_SFTRST | BM_PERFMON_CTRL_CLKGATE);

    /* clear the whole control register */
    reg32_write(base + HW_PERFMON_CTRL, 0x0);

    /* allocate some space to store the latest stats of perfmon 'id' */
    last_stats[id] = malloc(sizeof(perfmon_res_t));

    /* configure the interrupt */
    perfmon_setup_interrupt(id, TRUE);

    return;
}

/*!
 * Properly disable a performance monitor.
 *
 * @param   id perfmon instance PERFMON_IDx.
 */
void perfmon_close(perfmon_id_e id)
{
    uint32_t base = perfmon_list[id].base;

    /* maintain the PerfMon into reset */
    reg32_write(base + HW_PERFMON_CTRL_SET,
                BM_PERFMON_CTRL_SFTRST | BM_PERFMON_CTRL_CLKGATE);

    perfmon_setup_interrupt(id, FALSE);

    /* switch off the clock */
    clock_gating_config(base, CLOCK_OFF);

    /* free the allocated space used to store the latest stats of perfmon 'id' */
    free(last_stats[id]);

    return;
}

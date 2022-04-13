/*
 * Copyright (c) 2016, ARM Limited and Contributors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of ARM nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __MCUCFG_H__
#define __MCUCFG_H__

#include <stdint.h>
#include <mt_reg_base.h>

#define MCUCFG_BASE		MCUSYS_CFGREG_BASE

#define MP2_MISC_CONFIG_BOOT_ADDR_L(cpu)	(MCUCFG_BASE + 0x2290 + ((cpu) * 8))
#define MP2_MISC_CONFIG_BOOT_ADDR_H(cpu)	(MCUCFG_BASE + 0x2294 + ((cpu) * 8))

#define MP2_CPUCFG			(MCUCFG_BASE + 0x2208)

#define MP2_CPU0_STANDBYWFE		(1<<4)
#define MP2_CPU1_STANDBYWFE		(1<<5)

#define MP0_CPUTOP_SPMC_CTL		(MCUCFG_BASE + 0x788)
#define MP1_CPUTOP_SPMC_CTL		(MCUCFG_BASE + 0x78C)
#define MP1_CPUTOP_SPMC_SRAM_CTL	(MCUCFG_BASE + 0x790)

#define sw_spark_en			(1<<0)
#define sw_no_wait_for_q_channel	(1<<1)
#define sw_fsm_override			(1<<2)
#define sw_logic_pre1_pdb		(1<<3)
#define sw_logic_pre2_pdb		(1<<4)
#define sw_logic_pdb			(1<<5)
#define sw_iso				(1<<6)
#define sw_sram_sleepb			(0x3F<<7)
#define sw_sram_isointb			(1<<13)
#define sw_clk_dis			(1<<14)
#define sw_ckiso			(1<<15)
#define sw_pd				(0x3F<<16)
#define sw_hot_plug_reset		(1<<22)
#define sw_pwr_on_override_en		(1<<23)
#define sw_pwr_on			(1<<24)
#define sw_coq_dis			(1<<25)
#define logic_pdbo_all_off_ack		(1<<26)
#define logic_pdbo_all_on_ack		(1<<27)
#define logic_pre2_pdbo_all_on_ack	(1<<28)
#define logic_pre1_pdbo_all_on_ack	(1<<29)


#define CPUSYSx_CPUx_SPMC_CTL(cluster, cpu)     (MCUCFG_BASE + 0x1c30 + cluster * 0x2000 + cpu * 4)

#define CPUSYS0_CPU0_SPMC_CTL	(MCUCFG_BASE + 0x1c30)
#define CPUSYS0_CPU1_SPMC_CTL	(MCUCFG_BASE + 0x1c34)
#define CPUSYS0_CPU2_SPMC_CTL	(MCUCFG_BASE + 0x1c38)
#define CPUSYS0_CPU3_SPMC_CTL	(MCUCFG_BASE + 0x1c3C)

#define CPUSYS1_CPU0_SPMC_CTL	(MCUCFG_BASE + 0x3c30)
#define CPUSYS1_CPU1_SPMC_CTL	(MCUCFG_BASE + 0x3c34)
#define CPUSYS1_CPU2_SPMC_CTL	(MCUCFG_BASE + 0x3c38)
#define CPUSYS1_CPU3_SPMC_CTL	(MCUCFG_BASE + 0x3c3C)

#define cpu_sw_spark_en			(1<<0)
#define cpu_sw_no_wait_for_q_channel	(1<<1)
#define cpu_sw_fsm_override		(1<<2)
#define cpu_sw_logic_pre1_pdb		(1<<3)
#define cpu_sw_logic_pre2_pdb		(1<<4)
#define cpu_sw_logic_pdb		(1<<5)
#define cpu_sw_iso			(1<<6)
#define cpu_sw_sram_sleepb		(1<<7)
#define cpu_sw_sram_isointb		(1<<8)
#define cpu_sw_clk_dis			(1<<9)
#define cpu_sw_ckiso			(1<<10)
#define cpu_sw_pd			(0x1F<<11)
#define cpu_sw_hot_plug_reset		(1<<16)
#define cpu_sw_powr_on_override_en	(1<<17)
#define cpu_sw_pwr_on			(1<<18)
#define cpu_spark2ldo_allswoff		(1<<19)
#define cpu_pdbo_all_on_ack		(1<<20)
#define cpu_pre2_pdbo_allon_ack		(1<<21)
#define cpu_pre1_pdbo_allon_ack		(1<<22)

/* CPC related registers */
#define CPC_MCUSYS_CPC_OFF_THRES	(MCUCFG_BASE + 0xa714)
#define CPC_MCUSYS_PWR_CTRL		(MCUCFG_BASE + 0xa804)
#define CPC_MCUSYS_CPC_FLOW_CTRL_CFG	(MCUCFG_BASE + 0xa814)
#define CPC_MCUSYS_LAST_CORE_REQ	(MCUCFG_BASE + 0xa818)
#define CPC_MCUSYS_MP_LAST_CORE_RESP	(MCUCFG_BASE + 0xa81c)
#define CPC_MCUSYS_LAST_CORE_RESP	(MCUCFG_BASE + 0xa824)
#define CPC_MCUSYS_PWR_ON_MASK		(MCUCFG_BASE + 0xa828)
#define CPC_MCUSYS_CPU_ON_SW_HINT_SET	(MCUCFG_BASE + 0xa8a8)
#define CPC_MCUSYS_CPC_DBG_SETTING	(MCUCFG_BASE + 0xab00)
#define CPC_MCUSYS_TRACE_SEL		(MCUCFG_BASE + 0xab14)
#define CPC_MCUSYS_TRACE_DATA		(MCUCFG_BASE + 0xab20)
#define CPC_MCUSYS_CLUSTER_COUNTER	(MCUCFG_BASE + 0xab70)
#define CPC_MCUSYS_CLUSTER_COUNTER_CLR	(MCUCFG_BASE + 0xab74)

#define SPARK2LDO			(MCUCFG_BASE + 0x2700)
/* APB Module mcucfg */
#define MP0_CA7_CACHE_CONFIG		(MCUCFG_BASE + 0x000)
#define MP0_AXI_CONFIG			(MCUCFG_BASE + 0x02C)
#define MP0_MISC_CONFIG0		(MCUCFG_BASE + 0x030)
#define MP0_MISC_CONFIG1		(MCUCFG_BASE + 0x034)
#define MP0_MISC_CONFIG2		(MCUCFG_BASE + 0x038)
#define MP0_MISC_CONFIG_BOOT_ADDR(cpu)	(MCUCFG_BASE + 0x038 + ((cpu) * 8))
#define MP0_MISC_CONFIG3		(MCUCFG_BASE + 0x03C)
#define MP0_MISC_CONFIG9		(MCUCFG_BASE + 0x054)
#define MP0_CA7_MISC_CONFIG		(MCUCFG_BASE + 0x064)

#define MP0_RW_RSVD0			(MCUCFG_BASE + 0x06C)


#define MP1_CA7_CACHE_CONFIG		(MCUCFG_BASE + 0x200)
#define MP1_AXI_CONFIG			(MCUCFG_BASE + 0x22C)
#define MP1_MISC_CONFIG0		(MCUCFG_BASE + 0x230)
#define MP1_MISC_CONFIG1		(MCUCFG_BASE + 0x234)
#define MP1_MISC_CONFIG2		(MCUCFG_BASE + 0x238)
#define MP1_MISC_CONFIG_BOOT_ADDR(cpu)	(MCUCFG_BASE + 0x238 + ((cpu) * 8))
#define MP1_MISC_CONFIG3		(MCUCFG_BASE + 0x23C)
#define MP1_MISC_CONFIG9		(MCUCFG_BASE + 0x254)
#define MP1_CA7_MISC_CONFIG		(MCUCFG_BASE + 0x264)

#define CCI_ADB400_DCM_CONFIG		(MCUCFG_BASE + 0x740)
#define SYNC_DCM_CONFIG			(MCUCFG_BASE + 0x744)

#define MP0_CLUSTER_CFG0		(MCUCFG_BASE + 0xC8D0)

#define MP0_SPMC			(MCUCFG_BASE + 0x788)
#define MP1_SPMC			(MCUCFG_BASE + 0x78C)
#define L2C_CFG_MP0			(MCUCFG_BASE + 0x7f0)
#define MP2_AXI_CONFIG			(MCUCFG_BASE + 0x220C)
#define MP2_AXI_CONFIG_ACINACTM		(1<<0)
#define MP2_AXI_CONFIG_AINACTS		(1<<4)

#define MPx_AXI_CONFIG_ACINACTM		(1<<4)
#define MPx_AXI_CONFIG_AINACTS		(1<<5)

#define MPx_CA7_MISC_CONFIG_standbywfil2	(1<<28)

#define MP0_CPU0_STANDBYWFE		(1<<20)
#define MP0_CPU1_STANDBYWFE		(1<<21)
#define MP0_CPU2_STANDBYWFE		(1<<22)
#define MP0_CPU3_STANDBYWFE		(1<<23)

#define MP1_CPU0_STANDBYWFE		(1<<20)
#define MP1_CPU1_STANDBYWFE		(1<<21)
#define MP1_CPU2_STANDBYWFE		(1<<22)
#define MP1_CPU3_STANDBYWFE		(1<<23)

#define CPUSYS0_SPARKVRETCNTRL		(MCUCFG_BASE+0x1c00)
#define CPUSYS0_SPARKEN			(MCUCFG_BASE+0x1c04)
#define CPUSYS0_AMUXSEL			(MCUCFG_BASE+0x1c08)
#define CPUSYS1_SPARKVRETCNTRL		(MCUCFG_BASE+0x3c00)
#define CPUSYS1_SPARKEN			(MCUCFG_BASE+0x3c04)
#define CPUSYS1_AMUXSEL			(MCUCFG_BASE+0x3c08)

#define MP2_PWR_RST_CTL			(MCUCFG_BASE + 0x2008)
#define MP2_PTP3_CPUTOP_SPMC0		(MCUCFG_BASE + 0x22A0)
#define MP2_PTP3_CPUTOP_SPMC1		(MCUCFG_BASE + 0x22A4)

#define MP2_COQ				(MCUCFG_BASE + 0x22BC)
#define MP2_COQ_SW_DIS			(1<<0)

#define MP2_CA15M_MON_SEL		(MCUCFG_BASE + 0x2400)
#define MP2_CA15M_MON_L			(MCUCFG_BASE + 0x2404)

#define CPUSYS2_CPU0_SPMC_CTL		(MCUCFG_BASE + 0x2430)
#define CPUSYS2_CPU1_SPMC_CTL		(MCUCFG_BASE + 0x2438)
#define CPUSYS2_CPU0_SPMC_STA		(MCUCFG_BASE + 0x2434)
#define CPUSYS2_CPU1_SPMC_STA		(MCUCFG_BASE + 0x243C)

#define MP0_CA7L_DBG_PWR_CTRL		(MCUCFG_BASE + 0x068)
#define MP1_CA7L_DBG_PWR_CTRL		(MCUCFG_BASE + 0x268)
#define BIG_DBG_PWR_CTRL		(MCUCFG_BASE + 0x75C)

#define MP2_SW_RST_B			(1<<0)
#define MP2_TOPAON_APB_MASK		(1<<1)

#define B_SW_HOT_PLUG_RESET		(1<<30)

#define B_SW_PD_OFFSET			(18)
#define B_SW_PD				(0x3f<<B_SW_PD_OFFSET)

#define B_SW_SRAM_SLEEPB_OFFSET		(12)
#define B_SW_SRAM_SLEEPB		(0x3f<<B_SW_SRAM_SLEEPB_OFFSET)

#define B_SW_SRAM_ISOINTB		(1<<9)
#define B_SW_ISO			(1<<8)
#define B_SW_LOGIC_PDB			(1<<7)
#define B_SW_LOGIC_PRE2_PDB		(1<<6)
#define B_SW_LOGIC_PRE1_PDB		(1<<5)
#define B_SW_FSM_OVERRIDE		(1<<4)
#define B_SW_PWR_ON			(1<<3)
#define B_SW_PWR_ON_OVERRIDE_EN		(1<<2)



#define B_FSM_STATE_OUT_OFFSET		(6)
#define B_FSM_STATE_OUT_MASK		(0x1f<<B_FSM_STATE_OUT_OFFSET)
#define B_SW_LOGIC_PDBO_ALL_OFF_ACK	(1<<5)
#define B_SW_LOGIC_PDBO_ALL_ON_ACK	(1<<4)
#define B_SW_LOGIC_PRE2_PDBO_ALL_ON_ACK	(1<<3)
#define B_SW_LOGIC_PRE1_PDBO_ALL_ON_ACK	(1<<2)


#define B_FSM_OFF				(0<<B_FSM_STATE_OUT_OFFSET)
#define B_FSM_ON				(1<<B_FSM_STATE_OUT_OFFSET)
#define B_FSM_RET				(2<<B_FSM_STATE_OUT_OFFSET)

/* cpu boot mode */
enum {
	MP0_CPUCFG_64BIT_SHIFT = 12,
	MP1_CPUCFG_64BIT_SHIFT = 28,
	MP0_CPUCFG_64BIT = 0xf << MP0_CPUCFG_64BIT_SHIFT,
	MP1_CPUCFG_64BIT = 0xf << MP1_CPUCFG_64BIT_SHIFT
};

enum {
	MP1_DIS_RGU0_WAIT_PD_CPUS_L1_ACK_SHIFT = 0,
	MP1_DIS_RGU1_WAIT_PD_CPUS_L1_ACK_SHIFT = 4,
	MP1_DIS_RGU2_WAIT_PD_CPUS_L1_ACK_SHIFT = 8,
	MP1_DIS_RGU3_WAIT_PD_CPUS_L1_ACK_SHIFT = 12,
	MP1_DIS_RGU_NOCPU_WAIT_PD_CPUS_L1_ACK_SHIFT = 16,

	MP1_DIS_RGU0_WAIT_PD_CPUS_L1_ACK =
		0xf << MP1_DIS_RGU0_WAIT_PD_CPUS_L1_ACK_SHIFT,
	MP1_DIS_RGU1_WAIT_PD_CPUS_L1_ACK =
		0xf << MP1_DIS_RGU1_WAIT_PD_CPUS_L1_ACK_SHIFT,
	MP1_DIS_RGU2_WAIT_PD_CPUS_L1_ACK =
		0xf << MP1_DIS_RGU2_WAIT_PD_CPUS_L1_ACK_SHIFT,
	MP1_DIS_RGU3_WAIT_PD_CPUS_L1_ACK =
		0xf << MP1_DIS_RGU3_WAIT_PD_CPUS_L1_ACK_SHIFT,
	MP1_DIS_RGU_NOCPU_WAIT_PD_CPUS_L1_ACK =
		0xf << MP1_DIS_RGU_NOCPU_WAIT_PD_CPUS_L1_ACK_SHIFT
};

enum {
	MP1_AINACTS_SHIFT = 4,
	MP1_AINACTS = 1 << MP1_AINACTS_SHIFT
};

enum {
	MP1_SW_CG_GEN_SHIFT = 12,
	MP1_SW_CG_GEN = 1 << MP1_SW_CG_GEN_SHIFT
};

enum {
	MP1_L2RSTDISABLE_SHIFT = 14,
	MP1_L2RSTDISABLE = 1 << MP1_L2RSTDISABLE_SHIFT
};

#endif  /* __MCUCFG_H__ */

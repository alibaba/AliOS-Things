/*
 * Copyright (c) 2015,  ARM Limited and Contributors. All rights reserved.
 *
 * Redistribution and use in source and binary forms,  with or without
 * modification,  are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice,  this
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
 * AND ANY EXPRESS OR IMPLIED WARRANTIES,  INCLUDING,  BUT NOT LIMITED TO,  THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR
 * CONSEQUENTIAL DAMAGES (INCLUDING,  BUT NOT LIMITED TO,  PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  DATA,  OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,  WHETHER IN
 * CONTRACT,  STRICT LIABILITY,  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,  EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <assert.h>
#include <aos/cli.h>
#include <mcucfg.h>
#include <mtspmc.h>
#include <mtspmc_private.h>

#include <mt_reg_base.h>

unsigned int cpu_bitmask;

#define POWERON_CONFIG_EN              (SCPSYS_BASE + 0x000)
#define CPU_IDLE_STA                   (SCPSYS_BASE + 0x174)
#define MP0_CPUTOP_PWR_CON             (SCPSYS_BASE + 0x204)
#define MP0_CPU0_PWR_CON               (SCPSYS_BASE + 0x208)
#define MP0_CPU1_PWR_CON               (SCPSYS_BASE + 0x20C)
#define BYPASS_SPMC                    (SCPSYS_BASE + 0x2B4)

#define SPM_PROJECT_CODE    0xb16

#define SRAM_BOOTADDR     0x1001a000
#define SRAMROM_SEC_CTRL  0x1001a010

#define CP15ISB	asm volatile ("mcr     p15, 0, %0, c7, c5, 4" : : "r" (0))
#define CP15DSB	asm volatile ("mcr     p15, 0, %0, c7, c10, 4" : : "r" (0))
#define CP15DMB	asm volatile ("mcr     p15, 0, %0, c7, c10, 5" : : "r" (0))

#if __LINUX_ARM_ARCH__ >= 7
#define ISB	asm volatile ("isb sy" : : : "memory")
#define DSB	asm volatile ("dsb sy" : : : "memory")
#define DMB	asm volatile ("dmb sy" : : : "memory")
#elif __LINUX_ARM_ARCH__ == 6
#define ISB	CP15ISB
#define DSB	CP15DSB
#define DMB	CP15DMB
#else
#define ISB	asm volatile ("" : : : "memory")
#define DSB	CP15DSB
#define DMB	asm volatile ("" : : : "memory")
#endif

#define isb()	ISB
#define dsb()	DSB
#define dmb()	DMB

static inline void mmio_write_32(uintptr_t addr, uint32_t value)
{
	dsb();
	*(volatile uint32_t*)addr = value;
}

static inline uint32_t mmio_read_32(uintptr_t addr)
{
	uint32_t readVal;

	readVal = *(volatile uint32_t*)addr;
	dsb();
	return readVal;
}

#if 0
void set_cpu_retention_control(int retention_value)
{
	uint64_t cpuectlr;

	cpuectlr = read_cpuectlr();
	cpuectlr = ((cpuectlr >> 3) << 3);
	cpuectlr |= retention_value;
	write_cpuectlr(cpuectlr);
}
#endif

void mcucfg_set_bootaddr(int cluster, int cpu, uintptr_t bootaddr)
{
	mmio_write_32(SRAMROM_SEC_CTRL, 0xf0000000);
	mmio_write_32(SRAM_BOOTADDR, bootaddr);
	mmio_write_32(per_cpu(cluster, cpu, MCUCFG_BOOTADDR), bootaddr);
}

uintptr_t mcucfg_get_bootaddr(int cluster, int cpu)
{
	return mmio_read_32(per_cpu(cluster, cpu, MCUCFG_BOOTADDR));
}

void mcucfg_init_archstate(int cluster, int cpu, int arm64)
{
	uintptr_t reg;

	reg = per_cluster(cluster, MCUCFG_INITARCH);

	if (arm64)
		mmio_setbits_32(reg, 1 << (12 + cpu));
	else
		mmio_clrbits_32(reg, 1 << (12 + cpu));
}

int spmc_init(void)
{
	int err = 0;

	/* TINFO="enable SPM register control" */
	mmio_write_32(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0) |
		AUDAFE_MTCMOS_CG_EN | AUDSRC_MTCMOS_CG_EN);
	/*
	 *  PRINTF_SPMC("POWERON_CONFIG_EN_0x%x=0x%x\n",
	 *	POWERON_CONFIG_EN, mmio_read_32(POWERON_CONFIG_EN));
	 */
#if CONFIG_SPMC_MODE != 0
	/* de-assert Bypass SPMC  0: SPMC mode	1: Legacy mode */
	mmio_write_32(BYPASS_SPMC, 0x0);
	/*
	 * PRINTF_SPMC("BYPASS_SPMC_0x%x=0x%x\n", BYPASS_SPMC,
	 *  mmio_read_32(BYPASS_SPMC));
	 */
	/* udelay(200); */
	PRINTF_SPMC("[%s]change to SPMC mode !!!\n", __func__);
#endif
/* MP0 SPMC power Ctrl signals */
	mmio_write_32(MP0_CPU0_PWR_CON,
		mmio_read_32(MP0_CPU0_PWR_CON) & ~PWR_ON_2ND);
	mmio_write_32(MP0_CPU0_PWR_CON,
		mmio_read_32(MP0_CPU0_PWR_CON) | PWR_RST_B);
	mmio_write_32(MP0_CPU1_PWR_CON,
		mmio_read_32(MP0_CPU1_PWR_CON) & ~PWR_ON_2ND);
	mmio_write_32(MP0_CPU1_PWR_CON,
		mmio_read_32(MP0_CPU1_PWR_CON) | PWR_RST_B);

	mmio_write_32(MP0_CPUTOP_PWR_CON,
		mmio_read_32(MP0_CPUTOP_PWR_CON) & ~PWR_ON_2ND);
	mmio_write_32(MP0_CPUTOP_PWR_CON,
		mmio_read_32(MP0_CPUTOP_PWR_CON) | PWR_RST_B);
	mmio_write_32(MP0_CPUTOP_PWR_CON,
		mmio_read_32(MP0_CPUTOP_PWR_CON) & ~PWR_CLK_DIS);
/* MP1 SPMC power Ctrl signals */

/* setup_sw_cluster_default_state(0x10,STA_POWER_ON); */
/* setup_sw_core_default_state(0x0,STA_POWER_ON); */

	cpu_bitmask = 1;

	return err;
}

int spmc_cpu_corex_onoff(int linear_id, int state, int mode)
{
	int err = 0;
	unsigned int CPUSYSx_CPUx_SPMC_CTL = 0, MPx_CPUx_PWR_CON = 0,
		MPx_CPUx_STANDBYWFI = 0, MPx_CPUx_PWR_STA_MASK = 0;

	/* TINFO="enable SPM register control" */
	mmio_write_32(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0) |
		AUDAFE_MTCMOS_CG_EN | AUDSRC_MTCMOS_CG_EN);

	/*
	 * PRINTF_SPMC(">>>>>>>> %s >>>>>>>>linear_id=%d state=%d mode=%d\n",
	 * __func__, linear_id,state, mode);
	 */
	switch (linear_id) {
	case 0:
		CPUSYSx_CPUx_SPMC_CTL = CPUSYS0_CPU0_SPMC_CTL;
		MPx_CPUx_STANDBYWFI = MP0_CPU0_STANDBYWFI;
		MPx_CPUx_PWR_CON = MP0_CPU0_PWR_CON;
		break;
	case 1:
		CPUSYSx_CPUx_SPMC_CTL = CPUSYS0_CPU1_SPMC_CTL;
		MPx_CPUx_STANDBYWFI = MP0_CPU1_STANDBYWFI;
		MPx_CPUx_PWR_CON = MP0_CPU1_PWR_CON;
		break;
	default:
		PRINTF_SPMC("%s: CPU%d not exists\n", __func__, (int)linear_id);
	/* ERROR("[ATF]: %s() CPU%d not exists\n", __func__, (int)linear_id); */
	/* assert(0); */
	}

	MPx_CPUx_PWR_STA_MASK = PWR_ACK;

	if (state == STA_POWER_DOWN) {
		/* TINFO="Start to turn off MP0_CPU0" */
		if (!(cpu_bitmask & (1 << linear_id))) {
			PRINTF_SPMC("core%d already turn off!",
				linear_id);
			PRINTF_SPMC("cpu_bitmask=0x%x\n",
				cpu_bitmask);
			return 0;
		}
		if (mode == MODE_AUTO_SHUT_OFF) {
			mmio_write_32(CPUSYSx_CPUx_SPMC_CTL,
					mmio_read_32(CPUSYSx_CPUx_SPMC_CTL)
					& ~cpu_sw_no_wait_for_q_channel);
			//set_cpu_retention_control(1);
		} else {
			mmio_write_32(CPUSYSx_CPUx_SPMC_CTL,
				mmio_read_32(CPUSYSx_CPUx_SPMC_CTL)
					| cpu_sw_no_wait_for_q_channel);
			/*
			 * PRINTF_SPMC("CPU_IDLE_STA_0x%x=0x%x\n",
			 * CPU_IDLE_STA,mmio_read_32(CPU_IDLE_STA));
			 */
			while (!(mmio_read_32(CPU_IDLE_STA)
				&MPx_CPUx_STANDBYWFI))
				;
		}

		/* TINFO="Set PWR_ON = 0" */
		mmio_write_32(MPx_CPUx_PWR_CON,
			mmio_read_32(MPx_CPUx_PWR_CON) & ~PWR_ON);
		/*
		 * PRINTF_SPMC("MPx_CPUx_PWR_CON_0x%x=0x%x\n",
		 * MPx_CPUx_PWR_CON, mmio_read_32(MPx_CPUx_PWR_CON));
		 */
		/* TINFO="Wait until CPU_PWR_STATUS = 0 */
		if (mode == MODE_SPMC_HW) {
			while (mmio_read_32(MPx_CPUx_PWR_CON)
				& MPx_CPUx_PWR_STA_MASK)
				;
			PRINTF_SPMC("MPx_CPUx_PWR_CON_0x%x=0x%x\n",
				MPx_CPUx_PWR_CON,
					mmio_read_32(MPx_CPUx_PWR_CON));
		}
		cpu_bitmask &= ~(1 << linear_id);
		/* PRINTF_SPMC("cpu_bitmask=0x%x\n", cpu_bitmask); */
		/* TINFO="Finish to turn off MP0_CPU0" */
	} else {
		/* TINFO="Start to turn on MP0_CPU0" */
		/* TINFO="Set PWR_ON = 1" */
		mmio_write_32(MPx_CPUx_PWR_CON,
			mmio_read_32(MPx_CPUx_PWR_CON) | PWR_ON);
		PRINTF_SPMC("MPx_CPUx_PWR_CON_0x%x=0x%x\n",
			MPx_CPUx_PWR_CON, mmio_read_32(MPx_CPUx_PWR_CON));
		/*
		 * TINFO="Wait until CPU_PWR_STATUS = 1
		 * and CPU_PWR_STATUS_2ND = 1"
		 */
		while ((mmio_read_32(MPx_CPUx_PWR_CON) & MPx_CPUx_PWR_STA_MASK)
			!= MPx_CPUx_PWR_STA_MASK)
			;
		/*
		 * PRINTF_SPMC("MPx_CPUx_PWR_CON_0x%x=0x%x\n",
		 * MPx_CPUx_PWR_CON, mmio_read_32(MPx_CPUx_PWR_CON));
		 */
		cpu_bitmask |= (1 << linear_id);
		PRINTF_SPMC("cpu_bitmask=0x%x\n", cpu_bitmask);
		/* TINFO="Finish to turn on MP0_CPU0" */
		/*
		 * PRINTF_SPMC("[AT] PowerOn CPU %d successfully\n",
		 *  linear_id);
		 */
	}
	return err;
}

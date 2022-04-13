#ifndef _MTSPMC_PRIVATE_H_
#define _MTSPMC_PRIVATE_H_

uint64_t read_cpuectlr(void);
void write_cpuectlr(uint64_t);

/*
 * per_cpu/cluster helper
 */
struct per_cpu_reg {
	int cluster_addr;
	int cpu_stride;
};

#define per_cpu(cluster, cpu, reg) (reg[cluster].cluster_addr + (cpu << reg[cluster].cpu_stride))
#define per_cluster(cluster, reg) (reg[cluster].cluster_addr)

#define AUDAFE_MTCMOS_CG_EN			(1 << 2)
#define AUDSRC_MTCMOS_CG_EN			(1 << 6)

#define MCUCFG_MP0_MISC_CONFIG2         MP0_MISC_CONFIG2
#define MCUCFG_MP0_MISC_CONFIG3         MP0_MISC_CONFIG3
#define MCUCFG_MP1_MISC_CONFIG2         MP1_MISC_CONFIG2
#define MCUCFG_MP1_MISC_CONFIG3         MP1_MISC_CONFIG3

/* per_cpu registers for MCUCFG_MP?_MISC_CONFIG2 */
static const struct per_cpu_reg MCUCFG_BOOTADDR[] = {
	[0] = { .cluster_addr = MCUCFG_MP0_MISC_CONFIG2, .cpu_stride = 3 },
	[1] = { .cluster_addr = MCUCFG_MP1_MISC_CONFIG2, .cpu_stride = 3 },
};

/* per_cpu registers for MCUCFG_MP?_MISC_CONFIG3 */
static const struct per_cpu_reg MCUCFG_INITARCH[] = {
	[0] = { .cluster_addr = MCUCFG_MP0_MISC_CONFIG3 },
	[1] = { .cluster_addr = MCUCFG_MP1_MISC_CONFIG3 },
};

#define MCUSYS_PROTECTEN_SET		(INFRACFG_AO_BASE + 0x2B4)
#define MCUSYS_PROTECTEN_CLR		(INFRACFG_AO_BASE + 0x2B8)
#define MCUSYS_PROTECTEN_STA1		(INFRACFG_AO_BASE + 0x2C0)

#define IDX_PROTECT_MP0_CACTIVE		0
#define IDX_PROTECT_ICC0_CACTIVE	1
#define IDX_PROTECT_ICD0_CACTIVE	2
#define IDX_PROTECT_L2C0_CACTIVE	3

#define IDX_PROTECT_MP1_CACTIVE		4
#define IDX_PROTECT_ICC1_CACTIVE	5
#define IDX_PROTECT_ICD1_CACTIVE	6
#define IDX_PROTECT_L2C1_CACTIVE	7

#define MP0_SNOOP_CTRL				(0x0C595000)
#define MP1_SNOOP_CTRL				(0x0C594000)
#define MPx_SNOOP_STATUS			(0x0C59000C)
#define MPx_SNOOP_ENABLE			(0x3)

#define MCUSYS_CPU_PWR_STA_MASK      (0x1 << 16)
#define MP_TOP_CPU_PWR_STA_MASK      (0x1 << 15)
#define MP0_CPUTOP_PWR_STA_MASK      (0x1 << 13)
#define MP0_CPU0_PWR_STA_MASK        (0x1 << 5)
#define MP0_CPU1_PWR_STA_MASK        (0x1 << 6)
#define MP0_CPU2_PWR_STA_MASK        (0x1 << 7)
#define MP0_CPU3_PWR_STA_MASK        (0x1 << 8)
#define MP1_CPUTOP_PWR_STA_MASK      (0x1 << 14)
#define MP1_CPU0_PWR_STA_MASK        (0x1 << 9)
#define MP1_CPU1_PWR_STA_MASK        (0x1 << 10)
#define MP1_CPU2_PWR_STA_MASK        (0x1 << 11)
#define MP1_CPU3_PWR_STA_MASK        (0x1 << 12)

#define MP0_CPU0_STANDBYWFI		(1U << 10)
#define MP0_CPU1_STANDBYWFI		(1U << 11)

#define MP0_SPMC_SRAM_DORMANT_EN            (1<<0)
#define MP1_SPMC_SRAM_DORMANT_EN            (1<<1)

#define PWR_ACK_2ND			(1U << 31)
#define PWR_ACK				(1U << 30)
#define SRAM_SLEEP_B_ACK	(1U << 28)
#define SRAM_PDN_ACK		(1U << 24)
#define SRAM_SLEEP_B		(1U << 12)
#define SRAM_PDN			(1U << 8)
#define SRAM_PD_SLPB_CLAMP	(1U << 7)
#define SRAM_ISOINT_B		(1U << 6)
#define SRAM_CKISO			(1U << 5)
#define PWR_CLK_DIS			(1U << 4)
#define PWR_ON_2ND			(1U << 3)
#define PWR_ON				(1U << 2)
#define PWR_ISO				(1U << 1)
#define PWR_RST_B			(1U << 0)

#endif



#ifndef __OPENLOONGSON_CPU_H
#define __OPENLOONGSON_CPU_H


/*
 * Location of exception vectors.
 */
#define RESET_EXC_VEC		0xbfc00000
#define TLB_MISS_EXC_VEC	0x80000000
#define XTLB_MISS_EXC_VEC	0x80000080
#define CACHE_ERR_EXC_VEC	0x80000100
#define GEN_EXC_VEC		0x80000180

/*
 * Coprocessor 0 registers:
 */
#define COP_0_TLB_INDEX		$0
#define COP_0_TLB_RANDOM	$1
#define COP_0_TLB_LO0		$2
#define COP_0_TLB_LO1		$3
#define COP_0_TLB_CONTEXT	$4
#define COP_0_TLB_PG_MASK	$5
#define COP_0_TLB_WIRED		$6
#define COP_0_BAD_VADDR		$8
#define COP_0_COUNT		$9
#define COP_0_TLB_HI		$10
#define COP_0_COMPARE		$11
#define COP_0_STATUS_REG	$12
#define COP_0_CAUSE_REG		$13
#define COP_0_EXC_PC		$14
#define COP_0_PRID		$15
#define COP_0_CONFIG		$16
#define COP_0_LLADDR		$17
#define COP_0_WATCH_LO		$18
#define COP_0_WATCH_HI		$19
#define COP_0_TLB_XCONTEXT	$20
#define COP_0_ECC		$26
#define COP_0_CACHE_ERR		$27
#define COP_0_TAG_LO		$28
#define COP_0_TAG_HI		$29
#define COP_0_ERROR_PC		$30


/*
 * The bits in the status register.  All bits are active when set to 1.
 */
#define SR_COP_USABILITY	0xf0000000
#define SR_COP_0_BIT		0x10000000
#define SR_COP_1_BIT		0x20000000
#define SR_RP			0x08000000
#define SR_FR_32		0x04000000
#define SR_RE			0x02000000
#define SR_BOOT_EXC_VEC		0x00400000
#define SR_TLB_SHUTDOWN		0x00200000
#define SR_SOFT_RESET		0x00100000
#define SR_DIAG_CH		0x00040000
#define SR_DIAG_CE		0x00020000
#define SR_DIAG_DE		0x00010000
#define SR_KX			0x00000080
#define SR_SX			0x00000040
#define SR_UX			0x00000020
#define SR_KSU_MASK		0x00000018
#define SR_KSU_USER		0x00000010
#define SR_KSU_SUPER		0x00000008
#define SR_KSU_KERNEL		0x00000000
#define SR_ERL			0x00000004
#define SR_EXL			0x00000002
#define SR_INT_ENAB		0x00000001
/*#define SR_INT_MASK		0x0000ff00*/




#endif


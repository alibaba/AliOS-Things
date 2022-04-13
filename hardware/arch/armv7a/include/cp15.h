/*
 * =====================================================================================
 *
 *       Filename:  cp15.h
 *
 *    Description:  cp15 processor.
 *
 *        Version:  1.0
 *        Created:  2019年07月10日 18时50分43秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  czl
 *   Organization:
 *
 * =====================================================================================
 */

#ifndef __ARMV7A_CP15_H__
#define __ARMV7A_CP15_H__

unsigned long xport_cpu_get_smp_id(void);
unsigned long xport_cpu_mmu_get_ttbr0(void);
unsigned long xport_cpu_mmu_get_ttbr1(void);
void xport_cpu_mmu_disable(void);
void xport_cpu_mmu_enable(void);
void xport_cpu_tlb_set(volatile unsigned long);
void xport_cpu_dcache_clean_flush(void);
void xport_cpu_icache_flush(void);
void xport_cpu_vector_set_base(unsigned int addr);
void xport_cpu_dcache_disable(void);
void xport_cpu_icache_disable(void);
void xport_cpu_dcache_enable(void);
void xport_cpu_icache_enable(void);
#endif

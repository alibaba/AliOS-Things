/*************************************************************************
 *
 * 高速缓存相关源文件文件
 *
 *************************************************************************/


#include <string.h>
#include "ls1c_mipsregs.h"
#include "ls1c_cacheops.h"
#include "ls1c_cache.h"


#define INDEX_BASE			        0x80000000


// icache和dcache的信息
struct cache_desc icache_desc;
struct cache_desc dcache_desc;


#define cache_op(op,addr)						\
	__asm__ __volatile__(						\
	"	.set	push					\n"	\
	"	.set	noreorder				\n"	\
	"	.set	mips3\n\t				\n"	\
	"	cache	%0, %1					\n"	\
	"	.set	pop					\n"	\
	:								\
	: "i" (op), "R" (*(unsigned char *)(addr)))


#define cache32_unroll32(base,op)					\
	__asm__ __volatile__(						\
	"	.set push					\n"	\
	"	.set noreorder					\n"	\
	"	.set mips3					\n"	\
	"	cache %1, 0x000(%0); cache %1, 0x020(%0)	\n"	\
	"	cache %1, 0x040(%0); cache %1, 0x060(%0)	\n"	\
	"	cache %1, 0x080(%0); cache %1, 0x0a0(%0)	\n"	\
	"	cache %1, 0x0c0(%0); cache %1, 0x0e0(%0)	\n"	\
	"	cache %1, 0x100(%0); cache %1, 0x120(%0)	\n"	\
	"	cache %1, 0x140(%0); cache %1, 0x160(%0)	\n"	\
	"	cache %1, 0x180(%0); cache %1, 0x1a0(%0)	\n"	\
	"	cache %1, 0x1c0(%0); cache %1, 0x1e0(%0)	\n"	\
	"	cache %1, 0x200(%0); cache %1, 0x220(%0)	\n"	\
	"	cache %1, 0x240(%0); cache %1, 0x260(%0)	\n"	\
	"	cache %1, 0x280(%0); cache %1, 0x2a0(%0)	\n"	\
	"	cache %1, 0x2c0(%0); cache %1, 0x2e0(%0)	\n"	\
	"	cache %1, 0x300(%0); cache %1, 0x320(%0)	\n"	\
	"	cache %1, 0x340(%0); cache %1, 0x360(%0)	\n"	\
	"	cache %1, 0x380(%0); cache %1, 0x3a0(%0)	\n"	\
	"	cache %1, 0x3c0(%0); cache %1, 0x3e0(%0)	\n"	\
	"	.set pop					\n"	\
		:							\
		: "r" (base),						\
		  "i" (op));



extern void disable_cpu_cache(void);
extern void enable_cpu_cache(void);



// 探测cache类型和大小
void cache_probe(void)
{
    unsigned long config1 = 0;      // 协处理器0的寄存器config1
    unsigned int lsize = 0;         // 行大小

    // 读取寄存器config1
    config1 = read_c0_config1();

    // icache相关信息
    lsize = (config1 >> 19) & 7;    // 寄存器config1的IL
    if (0 == lsize)                 // 判断是否有高速缓存
    {
        // 没有高速缓存，则返回
        icache_desc.linesz = 0;
        icache_desc.cachesize = 0;
        return ;
    }
    icache_desc.linesz  = 2 << lsize;                       // 行大小=2*2^L字节
    icache_desc.sets    = 64 << ((config1 >> 22) & 7);      // 可直接索引位置数=64*2^S
    icache_desc.ways    = 1 + ((config1 >> 16) & 7);        // 组相联路数=1+A
    icache_desc.cachesize = icache_desc.sets * icache_desc.ways * icache_desc.linesz;  // cache大小
    icache_desc.waysize = icache_desc.cachesize / icache_desc.ways;
    icache_desc.waybit  = ffs(icache_desc.waysize);

    // dcache相关信息
    lsize = (config1 >> 10) & 7;    // 寄存器config1的DL
    if (0 == lsize)                 // 判断是否有高速缓存
    {
        // 没有高速缓存，则返回
        dcache_desc.linesz = 0;
        dcache_desc.cachesize = 0;
        return ;
    }
    dcache_desc.linesz  = 2 << lsize;                       // 行大小=2*2^L字节
    dcache_desc.sets    = 64 << ((config1 >> 13) & 7);      // 可直接索引位置数=64*2^S
    dcache_desc.ways    = 1 + ((config1 >> 7) & 7);         // 组相联路数=1+A
    dcache_desc.cachesize = dcache_desc.sets * dcache_desc.ways * dcache_desc.linesz;  // cache大小
    dcache_desc.waysize = dcache_desc.cachesize / dcache_desc.ways;
    dcache_desc.waybit  = ffs(dcache_desc.waysize);

    return ;
}



// 回写dcache的某个指定区间
// @start 待回写的起始地址
// @end 待回写的结束地址
void dcache_writeback_invalidate_range(unsigned long start, unsigned long end)
{
    unsigned long dcache_size = 0;          // dcache大小
    unsigned long line_size = 0;            // dcache行大小
    unsigned long addr = 0;
    unsigned long aend = 0;

    // 判断大小是否超过dcache大小
    dcache_size = dcache_desc.cachesize;    // 获取dcache大小
    if (end - start > dcache_size)
    {
        // 回写整个dcache
        dcache_writeback_invalidate_all();
        return ;
    }

    // 回写指定区间
    line_size = dcache_desc.linesz;         // 获取dcache行大小
    addr = start & ~(line_size - 1);        // 一次回写一行，所以低位不参与行计算
    aend = (end - 1) & ~(line_size - 1);
    while (1)
    {
        cache_op(Hit_Writeback_Inv_D, addr);
        if (addr == aend)
            break;
        addr += line_size;
    }

    return ;
}


// 作废icache的某个指定区间
// @start 待作废的起始地址
// @end 待作废的结束地址
void icache_invalidate_range(unsigned long start, unsigned long end)
{
    unsigned long icache_size = 0;          // icache大小
    unsigned long line_size = 0;            // icache行大小
    unsigned long addr = 0;
    unsigned long aend = 0;

    // 判断大小是否超过icache大小
    icache_size = icache_desc.cachesize;    // 获取icache大小
    if (end - start > icache_size)
    {
        // 作废整个icache
        icache_invalidate_all();
        return ;
    }

    // 作废指定区间
    line_size = icache_desc.linesz;         // 获取icache行大小
    addr = start & ~(line_size - 1);        // 一次作废一行，所以低位不参与行计算
    aend = (end - 1) & ~(line_size - 1);
    while (1)
    {
        cache_op(Hit_Invalidate_I, addr);
        if (addr == aend)
            break;
        addr += line_size;
    }

    return ;
}


// 回写整个dcache
void dcache_writeback_invalidate_all(void)
{
    unsigned long start = INDEX_BASE;
    unsigned long end   = start + dcache_desc.waysize;
    unsigned long ws_inc= 1UL << dcache_desc.waybit;
    unsigned long ws_end= dcache_desc.ways << dcache_desc.waybit;
    unsigned long ws, addr;
    unsigned char lsize = dcache_desc.linesz;

    for (ws = 0; ws < ws_end; ws += ws_inc)
        for (addr = start; addr < end; addr += lsize * 32)
            cache32_unroll32(addr | ws, Index_Writeback_Inv_D);
    
    return ;
}


// 作废整个icache
void icache_invalidate_all(void)
{
    unsigned long start = INDEX_BASE;
    unsigned long end   = start + icache_desc.waysize;
    unsigned long ws_inc= 1UL << icache_desc.waybit;
    unsigned long ws_end= icache_desc.ways << icache_desc.waybit;
    unsigned long ws, addr;
    unsigned char lsize = icache_desc.linesz;

    for (ws = 0; ws < ws_end; ws += ws_inc)
        for (addr = start; addr < end; addr += lsize * 32)
            cache32_unroll32(addr | ws, Index_Invalidate_I);
    
    return ;
}






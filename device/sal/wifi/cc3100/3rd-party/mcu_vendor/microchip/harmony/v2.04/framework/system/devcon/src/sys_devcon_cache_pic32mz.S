/*******************************************************************************
Copyright 2017 Microchip Technology Incorporated and its subsidiaries.

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE
*******************************************************************************/

#include <xc.h>
#include <cp0defs.h>
#include "sys_devcon_cache.h"

/* Symbols defined in linker script */
.weak _pic32_init_cache_program_base_addr
.weak _pic32_init_cache_data_base_addr


/******************************************************************************
 * void _pic32_init_cache(SYS_CACHE_COHERENCY cacheCoherency)
 *
 * Size the caches and initialize them. The function MUST be called after a
 * hardware reset and before using the caches, otherwise they may be in an
 * inconsistent state. This is normally called by the standard reset code.
 * Do NOT call it from application code, as it may invalidate dirty cache
 * lines in a writeback cache, without actually writing them back to memory.
 *
 ******************************************************************************/

	.section .text, code
    .set nomips16
    .global _pic32_init_cache
	.ent _pic32_init_cache

_pic32_init_cache:

    /*
	* The caches may be in an indeterminate state, so we force an
	* invalidate, load/fill, and invalidate for each line.
	*/

    addiu   sp, sp, -24
    sw  ra, 0(sp)
    sw  s0, 4(sp)
    sw  s1, 8(sp)
    sw  s2, 12(sp)
    sw  s3, 16(sp)
	sw  s4, 20(sp)

    /* cache mode attribute */
	move    s4, a0

    /* get cache sizes */
    jal     _pic32_get_icache_size
    nop
    move    s0, v0
    jal     _pic32_get_dcache_size
    nop
    move    s1, v0

    /* get line sizes */
    jal     _pic32_get_icache_linesize
    nop
    move    s2, v0
    jal     _pic32_get_dcache_linesize
    nop
    move    s3, v0

	/* Disable all interrupts and cache exceptions */
	.set 	macro
	.set 	noreorder
	# Disable interrupts and set UM=1
	# Save current status in tmp
	mfc0 	t0, _CP0_STATUS
    move    t1, t0
    and 	t0, ~_CP0_STATUS_IE_MASK
	or      t0, _CP0_STATUS_ERL_MASK
	mtc0 	t0, _CP0_STATUS
	ehb

    mtc0 	zero, _CP0_ERRCTL
	mtc0 	zero, _CP0_TAGLO 		
	ehb

    /* Initialize primary instruction cache */
	.set 	noreorder
4:	la      a0, _pic32_init_cache_program_base_addr
	bne 	a0, zero, 0f

	/* Use a default if the symbol is not defined */
	li      a0, 0x9D000000 			/* KSEG0_PROGRAM_BASE */
0:	beqz 	s0, 8f                  # icachesize
	addu 	a1, a0, s0              # limit = base + icachesize
1:	addu 	a0, s2                  # ilinesize
    bne 	a0, a1, 1b
    cache 	Index_Store_Tag_I, -4(a0) 	# BDSLOT: clear tag

	/* Initialize primary data cache */
	.set 	noreorder
8:  la      a0, _pic32_init_cache_data_base_addr
	bne 	a0, zero, 0f

	/* Use a default if the symbol is not defined */
	li 	a0, 0x80000000  		/* KSEG_DATA_BASE */
0:  beqz 	s1, 8f
	addu 	a1, a0, s1 		# limit = base + dcachesize
1:  addu 	a0, s3
	bne 	a0, a1, 1b
	cache 	Index_Store_Tag_D, -4(a0) 	# BDSLOT: clear tag

	.set 	reorder

8:	sync

	.set 	noreorder

	mfc0 	t0, _CP0_CONFIG
	ori 	t0, _CP0_CONFIG_K0_MASK
	xori 	t0, _CP0_CONFIG_K0_MASK
	or      t0, s4
	mtc0 	t0, _CP0_CONFIG

	# restore status
	mtc0    t1,_CP0_STATUS
	ehb
    .set 	reorder

    lw  ra, 0(sp)
    lw  s0, 4(sp)
    lw  s1, 8(sp)
    lw  s2, 12(sp)
    lw  s3, 16(sp)
	lw  s4, 20(sp)
    addiu   sp, sp, 24

 	jr  ra
    nop

    .size _pic32_init_cache,.-_pic32_init_cache
    .end _pic32_init_cache
    

/******************************************************************************
 * void _pic32_clean_cache (unsigned kva, size_t n)
 *
 * Writeback and invalidate address range in all caches.
 * Write back and invalidate entries matching the given address
 * range from all caches. The most common routine to call in
 * device drivers before starting a DMA transfer, or after
 * dynamically modifying executable code.
 *
 ******************************************************************************/

	.set nomips16
	.globl 	_pic32_clean_cache
	.ent 	_pic32_clean_cache

_pic32_clean_cache:

    addiu   sp, sp, -8
    sw  ra, 0(sp)

    /* Cache Op : Hit_Writeback_Inv_D */
    jal     _pic32_clean_dcache
    nop
    /* Cache Op : Hit_Invalidate_I */
    jal     _pic32_clean_icache
    nop

    lw  ra, 0(sp)
    addiu   sp, sp, 8

 	jr  ra
    nop

	.size _pic32_clean_cache,.-_pic32_clean_cache
	.end _pic32_clean_cache
     

/******************************************************************************
 * void _pic32_clean_dcache (unsigned kva, size_t n)
 *
 * Writeback and invalidate address range in data caches only
 * separate instruction caches are unchanged.
 *
 ******************************************************************************/

	.set nomips16
	.globl _pic32_clean_dcache
	.ent _pic32_clean_dcache

_pic32_clean_dcache:

    addiu   sp, sp, -8
    sw  ra, 0(sp)

    /* get cache linesize */
    jal     _pic32_get_dcache_linesize
    nop
    move    t3, v0
    beqz 	t3, 9f

	/* Cache Op : Hit_Writeback_Inv_D */
	.set 	noreorder
	blez 	a1, 11f
	addu 	t1, a0, a1
	subu 	t2, t3, 1
	not 	t2
	and 	t0, a0, t2
	addu 	t1, -1
	and 	t1, t2
10: 	cache 	Hit_Writeback_Inv_D, 0(t0)
	bne 	t0, t1, 10b
	addu 	t0, t3
11: 	.set 	reorder

9:  lw  ra, 0(sp)
    addiu   sp, sp, 8

    jr 	ra
    nop

	.size _pic32_clean_dcache,.-_pic32_clean_dcache
	.end _pic32_clean_dcache
	

/******************************************************************************
 * void _pic32_clean_dcache_nowrite (unsigned kva, size_t n)
 *
 * Invalidate (but don't writeback) address range in data caches
 * XXX Only safe if region is totally cache-line aligned.
 *
 ******************************************************************************/

	.set nomips16
	.globl _pic32_clean_dcache_nowrite
	.ent _pic32_clean_dcache_nowrite

_pic32_clean_dcache_nowrite:

    addiu   sp, sp, -8
    sw  ra, 0(sp)

    /* get cache linesize */
    jal     _pic32_get_dcache_linesize
    nop
    move    t3, v0
    beqz 	t3, 9f

	/* Cache Op : Hit_Writeback_Inv_D */
	.set 	noreorder
	blez 	a1, 11f
	addu 	t1, a0, a1
	subu 	t2, t3, 1
	not 	t2
	and 	t0, a0, t2
	addu 	t1, -1
	and 	t1, t2
10: 	cache 	Hit_Invalidate_D, 0(t0)
	bne 	t0, t1, 10b
	addu 	t0, t3
11: 	.set 	reorder

9:  lw  ra, 0(sp)
    addiu   sp, sp, 8

    jr 	ra
    nop

	.size _pic32_clean_dcache_nowrite,.-_pic32_clean_dcache_nowrite
	.end _pic32_clean_dcache_nowrite


/******************************************************************************
 * void _pic32_clean_icache (unsigned kva, size_t n)
 *
 * Writeback and invalidate address range in instruction caches
 * Separate data caches are unchanged.
 *
 ******************************************************************************/

	.set nomips16
	.globl _pic32_clean_icache
	.ent _pic32_clean_icache

_pic32_clean_icache:

    addiu   sp, sp, -8
    sw  ra, 0(sp)

    /* get cache linesize */
    jal     _pic32_get_icache_linesize
    nop
    move    t3, v0
    beqz 	t3, 9f

	/* Cache Op : Hit_Invalidate_I */
	.set 	noreorder
	blez 	a1, 11f
	addu 	t1, a0, a1
	subu 	t2, t3, 1
	not 	t2
	and 	t0, a0, t2
	addu 	t1, -1
	and 	t1, t2
10: 	cache   Hit_Invalidate_I, 0(t0)
	bne 	t0, t1, 10b
	addu 	t0, t3
11: 	.set 	reorder

9:  lw  ra, 0(sp)
    addiu   sp, sp, 8

    jr 	ra
    nop

	.size _pic32_clean_icache,.-_pic32_clean_icache
	.end _pic32_clean_icache
	

/******************************************************************************
 * void _pic32_flush_cache (void)
 *
 * Writeback and invalidate all caches
 * The simplest way to completely synchronise caches and
 * memory, but not necessarily the most efficient.
 ******************************************************************************/

	.set nomips16
	.globl _pic32_flush_cache
	.ent _pic32_flush_cache

_pic32_flush_cache:

    addiu   sp, sp, -8
    sw  ra, 0(sp)

    /* Cache Operation : Index_Writeback_Inv_D */
    jal     _pic32_flush_dcache
    nop
    /* Cache Op : Index_Invalidate_I */
    jal     _pic32_flush_icache
    nop

    lw  ra, 0(sp)
    addiu   sp, sp, 8

 	jr  ra
    nop

	.size _pic32_flush_cache,.-_pic32_flush_cache
	.end _pic32_flush_cache
    

/******************************************************************************
 * void _pic32_flush_dcache (void)
 *
 * Writeback and invalidate data caches only.
 * Separate instruction caches are unchanged
 *
 ******************************************************************************/

	.set nomips16
	.globl _pic32_flush_dcache
	.ent _pic32_flush_dcache

_pic32_flush_dcache:

    /* function prologue */
    addiu   sp, sp, -16
    sw  s0, 0(sp)
    sw  s1, 4(sp)
	sw  ra, 8(sp)

    jal     _pic32_get_dcache_size
    nop
    move    s0, v0
    beqz 	s0, 9f
    jal     _pic32_get_dcache_linesize
    nop
    move    s1, v0
    beqz 	s1, 9f

	/* writeback and invalidate primary data cache */
    la 	a0, _pic32_init_cache_data_base_addr
	bne 	a0, zero, 0f
	/* Use a default if the symbol is not defined */
	li 	a0, 0x80000000  		/* KSEG_DATA_BASE */

	/* Cache Operation : Index_Writeback_Inv_D */
0:	.set 	noreorder
	blez 	s0, 11f
	addu 	t1, a0, s0
	subu 	t2, s1, 1
	not 	t2
	and 	t0, a0, t2
	addu 	t1, -1
	and 	t1, t2
10: 	cache 	Index_Writeback_Inv_D, 0(t0)
	bne 	t0, t1, 10b
	addu 	t0, s1
11: 	.set 	reorder

    /* function epilogue */
 9: lw  s0, 0(sp)
    lw  s1, 4(sp)
	lw  ra, 8(sp)
    addiu   sp, sp, 16

    jr 	ra
    nop

	.size _pic32_flush_dcache,.-_pic32_flush_dcache
	.end _pic32_flush_dcache
    

/******************************************************************************
 * void _pic32_flush_icache (void)
 *
 * Writeback and invalidate instruction cache only.
 * Separate data caches are unchanged.
 *
 ******************************************************************************/

	.set nomips16
	.globl _pic32_flush_icache
	.ent _pic32_flush_icache

_pic32_flush_icache:

    /* function prologue */
    addiu   sp, sp, -16
    sw  s0, 0(sp)
    sw  s1, 4(sp)
	sw  ra, 8(sp)

    jal     _pic32_get_icache_size
    nop
    move    s0, v0
    beqz 	s0, 9f
    jal     _pic32_get_icache_linesize
    nop
    move    s1, v0
    beqz 	s1, 9f

	/* writeback and invalidate primary instruction cache */
	la 	a0, _pic32_init_cache_program_base_addr
	bne 	a0, zero, 0f
	/* Use a default if the symbol is not defined */
	li 	a0, 0x9D000000  		/* KSEG0_PROGRAM_BASE */

	/* Cache Op : Index_Invalidate_I */
0:	.set 	noreorder
	blez 	s0, 11f
	addu 	t1, a0, s0
	subu 	t2, s1, 1
	not 	t2
	and 	t0, a0, t2
	addu 	t1, -1
	and 	t1, t2
10:	cache 	Index_Invalidate_I, 0(t0)
	bne 	t0, t1, 10b
	addu 	t0, s1
11:	.set 	reorder

    /* function epilogue */
 9: lw  s0, 0(sp)
    lw  s1, 4(sp)
	lw  ra, 8(sp)
    addiu   sp, sp, 16

    jr 	ra
    nop

	.size _pic32_flush_icache,.-_pic32_flush_icache
	.end _pic32_flush_icache
  

/******************************************************************************
 * void _pic32_lock_dcache (void *data, size_t n)
 *
 * Load and lock a block of date into the d-cache
 * On CPUs which support cache locking, this functions allow you to lock
 * regions of data into the primary data cache. Take care not to
 * use the global flush functions after locking caches, as they will
 * invalidate (and unlock) the locked cache lines.
 *
 ******************************************************************************/

	.set nomips16
	.globl _pic32_lock_dcache
	.ent _pic32_lock_dcache

_pic32_lock_dcache:

    /* function prologue */
    addiu   sp, sp, -8
    sw  ra, 0(sp)

    /* get cache linesize */
    jal     _pic32_get_dcache_linesize
    nop
    move    t3, v0
    beqz 	t3, 9f

	/* Cache Op : Fetch_Lock_D */
	.set 	noreorder
	blez 	a1, 11f
	addu 	t1, a0, a1
	subu 	t2, t3, 1
	not 	t2
	and 	t0, a0, t2
	addu 	t1, -1
	and 	t1, t2
10: 	cache 	Fetch_Lock_D, 0(t0)
	bne 	t0, t1, 10b
	addu 	t0, t3
11: 	.set 	reorder

	/* function epilogue */
9:  lw  ra, 0(sp)
    addiu   sp, sp, 8

    jr 	ra
    nop

	.size _pic32_lock_dcache,.-_pic32_lock_dcache
	.end _pic32_lock_dcache


/******************************************************************************
 * void _pic32_lock_icache (void *code, size_t n)
 *
 * Load and lock a block of instructions into the i-cache
 * On CPUs which support cache locking, this functions allow you to lock
 * regions of code into the primary instruction cache. Take care not to
 * use the global flush functions after locking caches, as they will
 * invalidate (and unlock) the locked cache lines.
 *
 ******************************************************************************/

	.set nomips16
	.globl _pic32_lock_icache
	.ent _pic32_lock_icache

_pic32_lock_icache:

    /* function prologue */
    addiu   sp, sp, -8
    sw  ra, 0(sp)

    /* get cache linesize */
    jal     _pic32_get_icache_linesize
    nop
    move    t3, v0
    beqz 	t3, 9f

	/* Cache Op : Fetch_Lock_I */
	.set 	noreorder
	blez 	a1, 11f
	addu 	t1, a0, a1
	subu 	t2, t3, 1
	not 	t2
	and 	t0, a0, t2
	addu 	t1, -1
	and 	t1, t2
10: 	cache 	Fetch_Lock_I, 0(t0)
	bne 	t0, t1, 10b
	addu 	t0, t3
11: 	.set 	reorder

	/* function epilogue */
9:  lw  ra, 0(sp)
    addiu   sp, sp, 8

    jr 	ra
    nop

	.size _pic32_lock_icache,.-_pic32_lock_icache
	.end _pic32_lock_icache


/******************************************************************************
 * void _pic32_sync_icache (void *code, size_t n)
 *
 * Synchronises the icache with the dcache, which is necessary when
 * the instruction stream is modified by software
 * (e.g. inserting software breakpoints, self-modifying code, etc).
 *
 ******************************************************************************/

    .set nomips16
	.globl _pic32_sync_icache
	.ent _pic32_sync_icache

_pic32_sync_icache:

        /* check for bad size */
        addu    t1, a0, a1
        blez    a1, 9f

        /* get synci step and skip if not required */
        rdhwr   a2, $1
        addu    t1, -1
        beqz    a2, 9f

        /* align to line boundaries */
        subu    t2, a2, 1
        not     t2
        and     t0, a0, t2
        and     t1, t2

        /* the cacheop loop */
        .set    noreorder
10:     synci   0(t0)
        bne     t0, t1, 10b
        addu    t0, a2
        .set    reorder

        sync
9:      jr.hb   ra

	.size _pic32_sync_icache,.-_pic32_sync_icache
	.end _pic32_sync_icache


/******************************************************************************
 * size_t _pic32_get_dcache_associativity (void)
 *
 * Returns the number of ways in the data cache (in decimal).
 *
 ******************************************************************************/
 
    .set nomips16
    .global _pic32_get_dcache_associativity
	.ent _pic32_get_dcache_associativity

_pic32_get_dcache_associativity:

    mfc0 	cfg, _CP0_CONFIG

    /* Check that we have Config1 */
    and 	tmp, cfg, _CP0_CONFIG_M_MASK
    beqz 	tmp, 9f
    mfc0 	cfg, _CP0_CONFIG1

    /* Get number of dcache ways (in decimal)*/
    and 	dways, cfg, _CP0_CONFIG1_DA_MASK
    srl 	dways, _CP0_CONFIG1_DA_POSITION
    addu 	dways, 1
    move    v0, dways
    
9: 	jr 	ra
    nop

    .size _pic32_get_dcache_associativity,.-_pic32_get_dcache_associativity
    .end _pic32_get_dcache_associativity
 
 
 /******************************************************************************
 * size_t _pic32_get_icache_associativity (void)
 *
 * Returns the number of ways in the instruction cache (in decimal).
 *
 ******************************************************************************/
 
    .set nomips16
    .global _pic32_get_icache_associativity
	.ent _pic32_get_icache_associativity

_pic32_get_icache_associativity:

    mfc0 	cfg, _CP0_CONFIG

    /* Check that we have Config1 */
    and 	tmp, cfg, _CP0_CONFIG_M_MASK
    beqz 	tmp, 9f
    mfc0 	cfg, _CP0_CONFIG1

    /* Get number of icache ways (in decimal)*/
    and 	iways, cfg, _CP0_CONFIG1_IA_MASK
    srl 	iways, _CP0_CONFIG1_IA_POSITION
    addu 	iways, 1
    move    v0, iways
    
9: 	jr 	ra
    nop

    .size _pic32_get_icache_associativity,.-_pic32_get_icache_associativity
    .end _pic32_get_icache_associativity
 
 
 /******************************************************************************
 * size_t _pic32_get_dcache_linesize (void)
 *
 * Returns the data cache linesize (in decimal).
 *
 ******************************************************************************/
 
    .set nomips16
    .global _pic32_get_dcache_linesize
	.ent _pic32_get_dcache_linesize

_pic32_get_dcache_linesize:

    mfc0 	cfg, _CP0_CONFIG

    /* Check that we have Config1 */
    and 	tmp, cfg, _CP0_CONFIG_M_MASK
    beqz 	tmp, 9f
    mfc0 	cfg, _CP0_CONFIG1

    /* Get number of bytes per dcache line (in decimal)*/
    and 	tmp, cfg, _CP0_CONFIG1_DL_MASK
    beqz	tmp, 9f # no d-cache
    srl 	tmp, _CP0_CONFIG1_DL_POSITION
    addu 	tmp, 1
    li      dlinesize, 1
    sll     dlinesize, tmp
    move    v0, dlinesize

9: 	jr 	ra
    nop

    .size _pic32_get_dcache_linesize,.-_pic32_get_dcache_linesize
    .end _pic32_get_dcache_linesize
 
 
 /******************************************************************************
 * size_t _pic32_get_icache_linesize (void)
 *
 * Returns the instruction cache linesize (in decimal).
 *
 ******************************************************************************/
 
    .set nomips16
    .global _pic32_get_icache_linesize
	.ent _pic32_get_icache_linesize

_pic32_get_icache_linesize:

    mfc0 	cfg, _CP0_CONFIG

    /* Check that we have Config1 */
    and 	tmp, cfg, _CP0_CONFIG_M_MASK
    beqz 	tmp, 9f
    mfc0 	cfg, _CP0_CONFIG1

    /* Get number of bytes per icache line (in decimal) */
    and 	tmp, cfg, _CP0_CONFIG1_IL_MASK
    beqz	tmp, 9f # no i-cache
    srl 	tmp, _CP0_CONFIG1_IL_POSITION
    addu 	tmp, 1
    li      ilinesize, 1
    sll     ilinesize, tmp
    move    v0, ilinesize

9: 	jr 	ra
    nop

    .size _pic32_get_icache_linesize,.-_pic32_get_icache_linesize
    .end _pic32_get_icache_linesize
 
 
 /******************************************************************************
 * size_t _pic32_get_dcache_lines_per_way (void)
 *
 * Returns the number of lines per way in the data cache (in decimal).
 ******************************************************************************/
 
    .set nomips16
    .global _pic32_get_dcache_lines_per_way
	.ent _pic32_get_dcache_lines_per_way

_pic32_get_dcache_lines_per_way:

    mfc0 	cfg, _CP0_CONFIG

    /* Check that we have Config1 */
    and 	tmp, cfg, _CP0_CONFIG_M_MASK
    beqz 	tmp, 9f
    mfc0 	cfg, _CP0_CONFIG1

    /* get dcache lines per way */
    and 	tmp, cfg, _CP0_CONFIG1_DS_MASK
    srl 	tmp, _CP0_CONFIG1_DS_POSITION
    addu 	tmp, 6
    li      dways, 1
    sll 	dways, tmp
    move    v0, dways

9: 	jr 	ra
    nop

    .size _pic32_get_dcache_lines_per_way,.-_pic32_get_dcache_lines_per_way
    .end _pic32_get_dcache_lines_per_way
 
 
 /******************************************************************************
 * size_t _pic32_get_icache_lines_per_way (void)
 *
 * Returns the number of lines per way in the instruction cache (in decimal).
 *
 ******************************************************************************/
 
    .set nomips16
    .global _pic32_get_icache_lines_per_way
	.ent _pic32_get_icache_lines_per_way

_pic32_get_icache_lines_per_way:

    mfc0 	cfg, _CP0_CONFIG

    /* Check that we have Config1 */
    and 	tmp, cfg, _CP0_CONFIG_M_MASK
    beqz 	tmp, 9f
    mfc0 	cfg, _CP0_CONFIG1

    /* get icache lines per way */
    and 	tmp, cfg, _CP0_CONFIG1_IS_MASK
    srl 	tmp, _CP0_CONFIG1_IS_POSITION
    addu 	tmp, 6
    li      iways, 1
    sll 	iways, tmp
    move    v0, iways

9: 	jr 	ra
    nop

    .size _pic32_get_icache_lines_per_way,.-_pic32_get_icache_lines_per_way
    .end _pic32_get_icache_lines_per_way
 
 
  /******************************************************************************
 * size_t _pic32_get_dcache_size (void)
 *
 * Returns the total number of bytes in the data cache (in decimal).
 *
 ******************************************************************************/
 
    .set nomips16
    .global _pic32_get_dcache_size
	.ent _pic32_get_dcache_size

_pic32_get_dcache_size:

    /* function prologue */
    addiu   sp, sp, -16
    sw  s0, 0(sp)
    sw  s1, 4(sp)
    sw  s2, 8(sp)
	sw  ra, 12(sp)

    /* calculate dcache size */
	jal 	_pic32_get_dcache_linesize
    nop
    move    s0, v0
    jal 	_pic32_get_dcache_associativity
    nop
    move    s1, v0
    jal 	_pic32_get_dcache_lines_per_way
    nop
    move    s2, v0

    multu   s0, s1
    mflo    t0
    multu   s2, t0
    mflo    v0

    /* function epilogue */
    lw  s0, 0(sp)
    lw  s1, 4(sp)
    lw  s2, 8(sp)
	lw  ra, 12(sp)
    addiu   sp, sp, 16

 	jr  ra
    nop

    .size _pic32_get_dcache_size,.-_pic32_get_dcache_size
    .end _pic32_get_dcache_size
 
 
  /******************************************************************************
 * size_t _pic32_get_icache_size (void)
 *
 * Returns the total number of bytes in the instruction cache (in decimal).
 *
 ******************************************************************************/
 
    .set nomips16
    .global _pic32_get_icache_size
	.ent _pic32_get_icache_size

_pic32_get_icache_size:

    /* function prologue */
    addiu   sp, sp, -16
    sw  s0, 0(sp)
    sw  s1, 4(sp)
    sw  s2, 8(sp)
	sw  ra, 12(sp)

    /* calculate icache size */
	jal 	_pic32_get_icache_linesize
    nop
    move    s0, v0
    jal 	_pic32_get_icache_associativity
    nop
    move    s1, v0
    jal 	_pic32_get_icache_lines_per_way
    nop
    move    s2, v0

    multu   s0, s1
    mflo    t0
    multu   s2, t0
    mflo    v0

    /* function epilogue */
    lw  s0, 0(sp)
    lw  s1, 4(sp)
    lw  s2, 8(sp)
	lw  ra, 12(sp)
    addiu   sp, sp, 16

 	jr  ra
    nop

    .size _pic32_get_icache_size,.-_pic32_get_icache_size
    .end _pic32_get_icache_size

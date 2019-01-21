#ifndef __CACHE_H__
#define __CACHE_H__

enum cache_t{ICACHE, DCACHE};

static inline unsigned long CACHE_SET(enum cache_t cache){

	if(cache == ICACHE)
		return 64 << ((GET_ICM_CFG() & ICM_CFG_mskISET) >> ICM_CFG_offISET);
	else
		return 64 << ((GET_DCM_CFG() & DCM_CFG_mskDSET) >> DCM_CFG_offDSET);
}

static inline unsigned long CACHE_WAY(enum cache_t cache){

	if(cache == ICACHE)
		return 1 + ((GET_ICM_CFG() & ICM_CFG_mskIWAY) >> ICM_CFG_offIWAY);
	else
		return 1 + ((GET_DCM_CFG() & DCM_CFG_mskDWAY) >> DCM_CFG_offDWAY);
}

static inline unsigned long CACHE_LINE_SIZE(enum cache_t cache){

	if(cache == ICACHE)
		return 8 << (((GET_ICM_CFG() & ICM_CFG_mskISZ) >> ICM_CFG_offISZ) - 1);
	else
		return 8 << (((GET_DCM_CFG() & DCM_CFG_mskDSZ) >> DCM_CFG_offDSZ) - 1);
}

extern void n12_dcache_invalidate(void);
extern void n12_dcache_flush(void);
extern void n12_icache_flush(void);
extern void n12_dcache_clean_range(unsigned long start, unsigned long end);
extern void n12_dma_clean_range(unsigned long start, unsigned long end);
extern void n12_dcache_invalidate_range(unsigned long start, unsigned long end);
extern void n12_dcache_flush_range(unsigned long start, unsigned long end);
extern void n12_dcache_writeback_range(unsigned long start, unsigned long end);
extern void n12_dma_inv_range(unsigned long start, unsigned long end);
extern void n12_dma_flush_range(unsigned long start, unsigned long end);
extern void n12_icache_invalidate_range(unsigned long start, unsigned long end);

#endif /* __CACHE_H__ */

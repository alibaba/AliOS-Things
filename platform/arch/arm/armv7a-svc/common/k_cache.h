#ifndef K_CACHE_H
#define K_CACHE_H

void k_icache_enable(void);
void k_icache_disable(void);
void k_dcache_invalidate(uintptr_t text, uintptr_t size);
void k_icache_invalidate_all(void);

void k_dcache_enable(void);
void k_dcache_disable(void);
void k_dcache_clean(uintptr_t buffer, uintptr_t size);
void k_dcache_invalidate(uintptr_t buffer, uintptr_t size);
void k_dcache_clean_invalidate(uintptr_t buffer, uintptr_t size);

void k_dcache_clean_all(void);
void k_dcache_invalidate_all(void);
void k_dcache_clean_invalidate_all(void);

#endif /* K_CACHE_H */

#ifndef K_CACHE_H
#define K_CACHE_H

void k_cpu_icache_enable(void);
void k_cpu_icache_disable(void);
void k_cpu_icache_invalidate_all(void);

void k_cpu_dcache_enable(void);
void k_cpu_dcache_disable(void);
void k_cpu_dcache_clean(unsigned int buffer, unsigned int size);
void k_cpu_dcache_invalidate(unsigned int buffer, unsigned int size);
void k_cpu_dcache_clean_invalidate(unsigned int buffer, unsigned int size);

void k_cpu_dcache_clean_all(void);
void k_cpu_dcache_invalidate_all(void);
void k_cpu_dcache_clean_invalidate_all(void);

#endif /* K_CACHE_H */

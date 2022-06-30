/*************************************************************************
 *
 * 高速缓存相关头文件
 *
 *************************************************************************/



// 一个cache的描述信息
struct cache_desc {
    unsigned int waysize;           // Bytes per way 每路的字节数
    unsigned short sets;            // Number of lines per set 对应寄存器中的S，组数
    unsigned char ways;             // Number of ways 对应寄存器中的A，相联度路数
    unsigned char linesz;           // size of line in bytes 对应寄存器中的L，行大小
    unsigned char waybit;           // Bits to select in a cache set 

    unsigned long cachesize;        // 高速缓存大小
};



// 探测cache类型和大小
void cache_probe(void);




// 回写dcache的某个指定区间
// @start 待回写的起始地址
// @end 待回写的结束地址
void dcache_writeback_invalidate_range(unsigned long start, unsigned long end);

// 作废icache的某个指定区间
// @start 待作废的起始地址
// @end 待作废的结束地址
void icache_invalidate_range(unsigned long start, unsigned long end);


// 回写整个dcache
void dcache_writeback_invalidate_all(void);

// 作废整个icache
void icache_invalidate_all(void);






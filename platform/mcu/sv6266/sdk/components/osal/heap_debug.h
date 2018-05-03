#ifndef __HEAP_DEBUG_H__
#define __HEAP_DEBUG_H__

extern void debug_traceMALLOC_FAIL(uint32_t need, uint32_t remain);
#ifndef traceMALLOC_FAIL
    #define traceMALLOC_FAIL( need, remain ) debug_traceMALLOC_FAIL((need),(remain))
#endif

extern void debug_traceMALLOC_FRAGMENT(void* p, uint32_t size);
#ifndef traceMALLOC_FRAGMENT
    #define traceMALLOC_FRAGMENT( p, size ) debug_traceMALLOC_FRAGMENT((p),(size))
#endif

#endif /* __DEBUG_H__ */

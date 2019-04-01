#ifndef _DRV_LMDMA_H_
#define _DRV_LMDMA_H_

#include "hal_lmdma.h"

#if (defined (CONFIG_OS_FREERTOS))
extern LMDMA_CHAN_E gLMDMA_CHANNEL_ENABLE_BITMAP;
#endif

/**
 * Initialize the Local Memory DMA hardware.
 */
void
drv_lmdma_init (
    void
    );

/**
 * Deinitialize the Local Memory DMA hardware.
 */
void
drv_lmdma_deinit (
    void
    );

/**
 * Copies a external memory buffer to a local memory buffer.
 *
 * If isr is NULL, then the Local Memory DMA interrupt is disabled.
 *
 * @param   local_memory_address        A pointer to the local memory buffer of the memory copy.
 * @param   external_memory_address     A pointer to the external memory buffer of the memory copy.
 * @param   count                       The number of bytes to copy from external memory buffer to local memory buffer.
 * @param   isr                         A pointer to an ISR function that is called when a Local Memory DMA interrupt handler occurs.
 *                                      NULL to disable interrupt.
 *
 * @retval  -19                         The Local Memory DMA has not been started.
 * @retval   -4                         The count was not the proper size.
 * @retval  131                         The address for local memory was out of range.
 * @retval    0                         The operation completed successfully.
 */
int32_t
drv_lmdma_em2lmcpy32 (
    void                    *local_memory_address,
    void                    *external_memory_address,
    int32_t                 count,
    LMDMA_ISR               isr
    );

/**
 * Copies a local memory buffer to a external memory buffer.
 *
 * If isr is NULL, then the Local Memory DMA interrupt is disabled.
 *
 * @param   external_memory_address     A pointer to the external memory buffer of the memory copy.
 * @param   local_memory_address        A pointer to the local memory buffer of the memory copy.
 * @param   count                       The number of bytes to copy from local memory buffer to external memory buffer.
 * @param   isr                         A pointer to an ISR function that is called when a Local Memory DMA interrupt handler occurs.
 *                                      NULL to disable interrupt.
 *
 * @retval  -19                         The Local Memory DMA has not been started.
 * @retval   -4                         The count was not the proper size.
 * @retval  131                         The address for local memory was out of range.
 * @retval    0                         The operation completed successfully.
 */
int32_t
drv_lmdma_lm2emcpy32 (
    void                    *external_memory_address,
    void                    *local_memory_address,
    int32_t                 count,
    LMDMA_ISR               isr
    );

#endif  // #ifndef _DRV_LMDMA_H_

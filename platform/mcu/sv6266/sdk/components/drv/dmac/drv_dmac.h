#ifndef _DRV_DMAC_H_
#define _DRV_DMAC_H_

#include "hal_dmac.h"

#if (defined (CONFIG_OS_FREERTOS))
extern DMAC_CH_E gDMAC_CHANNEL_ENABLE_BITMAP;
#endif

/**
 * Initialize the DMAC hardware.
 */
void
drv_dmac_init (
    void
    );

/**
 * Deinitialize the DMAC hardware.
 */
void
drv_dmac_deinit (
    void
    );

/**
 * Copies a source buffer to a destination buffer.
 *
 * If isr is NULL, then the DMAC interrupt is disabled.
 *
 * @param   dst                         A pointer to the destination buffer of the memory copy.
 * @param   src                         A pointer to the source buffer of the memory copy.
 * @param   count                       The number of bytes to copy from source buffer to destination buffer.
 *                                      Contains at least the [1, 16380] range.
 * @param   isr                         A pointer to an ISR function that is called when a DMAC interrupt handler occurs.
 *                                      NULL to disable interrupt.
 *
 * @retval  -19                         The DMAC has not been started.
 * @retval   -4                         The count was not the proper size.
 * @retval   -3                         The Un-aligned address is not supported.
 * @retval    0                         The operation completed successfully.
 */
int32_t
drv_dmac_memcpy32 (
    void                    *dst,
    void const              *src,
    int32_t                 count,
    DMAC_ISR                isr
    );

/**
 * Fills a destination buffer with an 8-bit value.
 *
 * If isr is NULL, then the DMAC interrupt is disabled.
 *
 * @param   dst                         A pointer to the destination buffer of the memory fill.
 * @param   value                       The value with which to fill count bytes of destination buffer.
 * @param   count                       The number of bytes to fill.
 *                                      Contains at least the [1, 16380] range.
 * @param   isr                         A pointer to an ISR function that is called when a DMAC interrupt handler occurs.
 *                                      NULL to disable interrupt.
 *
 * @retval  -19                         The DMAC has not been started.
 * @retval   -4                         The count was not the proper size.
 * @retval   -3                         The Un-aligned address is not supported.
 * @retval    0                         The operation completed successfully.
 */
int32_t
drv_dmac_memset8 (
    void                    *dst,
    uint8_t                 value,
    int32_t                 count,
    DMAC_ISR                isr
    );

/**
 * Fills a destination buffer with a 16-bit value.
 *
 * If isr is NULL, then the DMAC interrupt is disabled.
 *
 * @param   dst                         A pointer to the destination buffer of the memory fill.
 * @param   value                       The value with which to fill count bytes of destination buffer.
 * @param   count                       The number of bytes to fill.
 *                                      Contains at least the [1, 16380] range.
 * @param   isr                         A pointer to an ISR function that is called when a DMAC interrupt handler occurs.
 *                                      NULL to disable interrupt.
 *
 * @retval  -19                         The DMAC has not been started.
 * @retval   -4                         The count was not the proper size.
 * @retval   -3                         The Un-aligned address is not supported.
 * @retval    0                         The operation completed successfully.
 */
int32_t
drv_dmac_memset16 (
    void                    *dst,
    uint16_t                value,
    int32_t                 count,
    DMAC_ISR                isr
    );

/**
 * Fills a destination buffer with a 32-bit value.
 *
 * If isr is NULL, then the DMAC interrupt is disabled.
 *
 * @param   dst                         A pointer to the destination buffer of the memory fill.
 * @param   value                       The value with which to fill count bytes of destination buffer.
 * @param   count                       The number of bytes to fill.
 *                                      Contains at least the [1, 16380] range.
 * @param   isr                         A pointer to an ISR function that is called when a DMAC interrupt handler occurs.
 *                                      NULL to disable interrupt.
 *
 * @retval  -19                         The DMAC has not been started.
 * @retval   -4                         The count was not the proper size.
 * @retval   -3                         The Un-aligned address is not supported.
 * @retval    0                         The operation completed successfully.
 */
int32_t
drv_dmac_memset32 (
    void                    *dst,
    uint32_t                value,
    int32_t                 count,
    DMAC_ISR                isr
    );

#endif  // #ifndef _DRV_DMAC_H_

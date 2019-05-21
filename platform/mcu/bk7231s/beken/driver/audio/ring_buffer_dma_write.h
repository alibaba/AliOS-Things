#ifndef __RING_BUFFER_DMA_WR_H__
#define __RING_BUFFER_DMA_WR_H__


typedef struct rb_dma_rd_st
{
    UINT8 *address;     
    UINT32 capacity;    /**< memory capacity in bytes */
    UINT32 wp;          /**< write point in bytes     */
    UINT32 rp;          /**< read point in bytes      */
    UINT32 dma_ch;
}RB_DMA_WR_ST, *RB_DMA_WR_PTR;

void rb_init_dma_write(RB_DMA_WR_PTR rb, UINT8 *addr, UINT32 capacity, UINT32 ch);
void rb_clear_dma_write(RB_DMA_WR_PTR rb);
UINT32 rb_read_dma_write(RB_DMA_WR_PTR rb, UINT8 *buffer, UINT32 size, UINT32 count);
UINT32 rb_get_fill_size_dma_write(RB_DMA_WR_PTR rb);
UINT32 rb_get_free_size_dma_write(RB_DMA_WR_PTR rb);

#endif//__RING_BUFFER_DMA_RD_H__
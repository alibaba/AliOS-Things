#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__


typedef struct rb_st
{
    UINT8 *address;     
    UINT32 capacity;    /**< memory capacity in bytes */
    UINT32 wp;          /**< write point in bytes     */
    UINT32 rp;          /**< read point in bytes      */
}RB_ST, *RB_PTR;

void rb_init(RB_PTR rb, UINT8 *addr, UINT32 capacity);
void rb_clear(RB_PTR rb);
UINT32 rb_read(RB_PTR rb, UINT8 *buffer, UINT32 size, UINT32 count);
UINT32 rb_write(RB_PTR rb, UINT8 *buffer, UINT32 size, UINT32 count);
UINT32 rb_get_fill_size(RB_PTR rb);
UINT32 rb_get_free_size(RB_PTR rb);

#endif//__RING_BUFFER_H__

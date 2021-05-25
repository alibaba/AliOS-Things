#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <csi_core.h>
#include "dev_ringbuf.h"

#define min(a, b)  (((a) < (b)) ? (a) : (b))

/**
  * \brief  Removes the entire FIFO contents.
  * \param  [in] fifo: The fifo to be emptied.
  * \return None.
  */
void dev_ringbuff_reset(dev_ringbuf_t *fifo)
{
    uint32_t stat = csi_irq_save();
    fifo->write = fifo->read = 0;
    fifo->data_len = 0;
    csi_irq_restore(stat);
}

/**
  * \brief  Returns the size of the FIFO in bytes.
  * \param  [in] fifo: The fifo to be used.
  * \return The size of the FIFO.
  */
static inline uint32_t ringbuffer_size(dev_ringbuf_t *fifo)
{
    return fifo->size;
}

/**
  * \brief  Returns the number of used bytes in the FIFO.
  * \param  [in] fifo: The fifo to be used.
  * \return The number of used bytes.
  */
uint32_t dev_ringbuf_len(dev_ringbuf_t *fifo)
{
    return fifo->data_len;
}

/**
  * \brief  Returns the number of bytes available in the FIFO.
  * \param  [in] fifo: The fifo to be used.
  * \return The number of bytes available.
  */
uint32_t dev_ringbuf_avail(dev_ringbuf_t *fifo)
{
    return ringbuffer_size(fifo) - dev_ringbuf_len(fifo);
}

/**
  * \brief  Is the FIFO empty?
  * \param  [in] fifo: The fifo to be used.
  * \retval true:      Yes.
  * \retval false:     No.
  */
bool dev_ringbuf_is_empty(dev_ringbuf_t *fifo)
{
    return dev_ringbuf_len(fifo) == 0;
}

/**
  * \brief  Is the FIFO full?
  * \param  [in] fifo: The fifo to be used.
  * \retval true:      Yes.
  * \retval false:     No.
  */
bool dev_ringbuf_is_full(dev_ringbuf_t *fifo)
{
    return dev_ringbuf_avail(fifo) == 0;
}

/**
  * \brief  Puts some data into the FIFO.
  * \param  [in] fifo: The fifo to be used.
  * \param  [in] in:   The data to be added.
  * \param  [in] len:  The length of the data to be added.
  * \return The number of bytes copied.
  * \note   This function copies at most @len bytes from the @in into
  *         the FIFO depending on the free space, and returns the number
  *         of bytes copied.
  */
uint32_t dev_ringbuf_in(dev_ringbuf_t *fifo, const void *datptr, uint32_t len)
{
    uint32_t writelen = 0, tmplen = 0;

    if(dev_ringbuf_is_full(fifo))
        return 0;

    tmplen = fifo->size - fifo->data_len;
    writelen = tmplen > len ? len : tmplen;

    if(fifo->write < fifo->read) {
        memcpy((void*)&fifo->buffer[fifo->write], (void*)datptr, writelen);
    } else {
        tmplen = fifo->size - fifo->write;
        if(writelen <= tmplen) {
            memcpy((void*)&fifo->buffer[fifo->write], (void*)datptr, writelen);
        } else {
            memcpy((void*)&fifo->buffer[fifo->write], (void*)datptr, tmplen);
            memcpy((void*)fifo->buffer, (uint8_t*)datptr + tmplen, writelen - tmplen);
        }
    }

    uint32_t stat = csi_irq_save();
    fifo->write = (fifo->write + writelen) % fifo->size;
    fifo->data_len += writelen;
    csi_irq_restore(stat);

    return writelen;
}

/**
  * \brief  Gets some data from the FIFO.
  * \param  [in] fifo: The fifo to be used.
  * \param  [in] out:  Where the data must be copied.
  * \param  [in] len:  The size of the destination buffer.
  * \return The number of copied bytes.
  * \note   This function copies at most @len bytes from the FIFO into
  *         the @out and returns the number of copied bytes.
  */
uint32_t dev_ringbuf_out(dev_ringbuf_t *fifo, void *outbuf, uint32_t len)
{
    uint32_t readlen = 0, tmplen = 0;
    if(dev_ringbuf_is_empty(fifo))
        return 0;

    uint32_t data_len = fifo->data_len;
    readlen = len > data_len ? data_len : len;
    tmplen = fifo->size - fifo->read;

    if(NULL != outbuf) {
        if(readlen <= tmplen) {
            memcpy((void*)outbuf, (void*)&fifo->buffer[fifo->read], readlen);
        } else {
            memcpy((void*)outbuf,(void*)&fifo->buffer[fifo->read], tmplen);
            memcpy((uint8_t*)outbuf + tmplen,(void*)fifo->buffer,readlen - tmplen);
        }
    }

    uint32_t stat = csi_irq_save();
    fifo->read = (fifo->read + readlen) % fifo->size;
    fifo->data_len -= readlen;
    csi_irq_restore(stat);

    return readlen;
}


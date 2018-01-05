/**
 ******************************************************************************
 * @file    RingBufferUtils.c
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This file contains function called by ring buffer operation
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2014 MXCHIP Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */

#include "ringbuf.h"

int ring_buffer_init( ring_buffer_t* ring_buffer, uint8_t* buffer, uint32_t size )
{
  if (ring_buffer)
  {
    ring_buffer->buffer = (uint8_t*)buffer;
    ring_buffer->size   = size;
    ring_buffer->head   = 0;
    ring_buffer->tail   = 0;
    return 0;
  }
  else
    return -1;
}

int ring_buffer_deinit( ring_buffer_t* ring_buffer )
{
  //UNUSED_PARAMETER(ring_buffer);
  return 0;
}

uint32_t ring_buffer_free_space( ring_buffer_t* ring_buffer )
{
  uint32_t tail_to_end = ring_buffer->size-1 - ring_buffer->tail;
  return ((tail_to_end + ring_buffer->head) % ring_buffer->size);
}

uint32_t ring_buffer_used_space( ring_buffer_t* ring_buffer )
{
  uint32_t head_to_end = ring_buffer->size - ring_buffer->head;
  return ((head_to_end + ring_buffer->tail) % ring_buffer->size);
}

int ring_buffer_get_data( ring_buffer_t* ring_buffer, uint8_t** data, uint32_t* contiguous_bytes )
{
  uint32_t head_to_end = ring_buffer->size - ring_buffer->head;
  
  *data = &ring_buffer->buffer[ring_buffer->head];
  *contiguous_bytes = MIN(head_to_end, (head_to_end + ring_buffer->tail) % ring_buffer->size);
  
  return 0;
}

int ring_buffer_consume( ring_buffer_t* ring_buffer, uint32_t bytes_consumed )
{
  ring_buffer->head = (ring_buffer->head + bytes_consumed) % ring_buffer->size;
  return 0;
}

uint32_t ring_buffer_write( ring_buffer_t* ring_buffer, const uint8_t* data, uint32_t data_length )
{
  uint32_t tail_to_end;
  uint32_t space;
  uint32_t amount_to_copy;
  
  tail_to_end = ring_buffer->size-1 - ring_buffer->tail;
  space = (tail_to_end + ring_buffer->head) % ring_buffer->size;
  /* Calculate the maximum amount we can copy */
  amount_to_copy = MIN(data_length, space);

  /* Fix the bug when tail is at the end of buffer */
  tail_to_end++;
  
  /* Copy as much as we can until we fall off the end of the buffer */
  memcpy(&ring_buffer->buffer[ring_buffer->tail], data, MIN(amount_to_copy, tail_to_end));
  
  /* Check if we have more to copy to the front of the buffer */
  if ( tail_to_end < amount_to_copy )
  {
    memcpy( &ring_buffer->buffer[ 0 ], data + tail_to_end, amount_to_copy - tail_to_end );
  }
  
  /* Update the tail */
  ring_buffer->tail = (ring_buffer->tail + amount_to_copy) % ring_buffer->size;
  
  return amount_to_copy;
}

int ring_buffer_read( ring_buffer_t* ring_buffer, uint8_t* data, uint32_t data_length, uint32_t* number_of_bytes_read )
{
  uint32_t max_bytes_to_read;
  uint32_t i;
  uint32_t head;
  uint32_t used_size;
    
  head = ring_buffer->head;
  used_size = ring_buffer_used_space(ring_buffer);
  max_bytes_to_read = MIN(data_length, used_size);
  
  if ( max_bytes_to_read != 0 )
  {
    for ( i = 0; i != max_bytes_to_read; i++, ( head = ( head + 1 ) % ring_buffer->size ) )
    {
      data[ i ] = ring_buffer->buffer[ head ];
    }
    
    ring_buffer_consume( ring_buffer, max_bytes_to_read );
  }
  
  *number_of_bytes_read = max_bytes_to_read;
  
  return 0;
}

uint8_t ring_buffer_is_full(ring_buffer_t *ring_buffer)
{
    if (ring_buffer_used_space(ring_buffer) >= ring_buffer->size - 1)
        return 1;
    else
        return 0;
}

uint32_t ring_buffer_total_size(ring_buffer_t *ring_buffer)
{
  return ring_buffer->size - 1;
}

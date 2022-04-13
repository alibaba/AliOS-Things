#ifndef __RingBufferUtils_h__
#define __RingBufferUtils_h__

#include "common.h"

typedef struct
{
  uint32_t  size;
  uint32_t  head;
  uint32_t  tail;
  uint8_t*  buffer;
} ring_buffer_t;

#ifndef MIN
#define MIN(x,y)  ((x) < (y) ? (x) : (y))
#endif /* ifndef MIN */

OSStatus ring_buffer_init( ring_buffer_t* ring_buffer, uint8_t* buffer, uint32_t size );

OSStatus ring_buffer_deinit( ring_buffer_t* ring_buffer );

uint32_t ring_buffer_free_space( ring_buffer_t* ring_buffer );

uint32_t ring_buffer_used_space( ring_buffer_t* ring_buffer );

uint8_t ring_buffer_get_data( ring_buffer_t* ring_buffer, uint8_t** data, uint32_t* contiguous_bytes );

uint8_t ring_buffer_consume( ring_buffer_t* ring_buffer, uint32_t bytes_consumed );

uint32_t ring_buffer_write( ring_buffer_t* ring_buffer, const uint8_t* data, uint32_t data_length );

#endif // __RingBufferUtils_h__



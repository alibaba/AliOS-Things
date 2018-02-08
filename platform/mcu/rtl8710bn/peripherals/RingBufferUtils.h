/**
******************************************************************************
* @file    RingBufferUtils.h 
* @author  William Xu
* @version V1.0.0
* @date    05-May-2014
* @brief   This header contains function prototypes called by ring buffer 
*          operation
******************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, MXCHIP Inc. SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* <h2><center>&copy; COPYRIGHT 2014 MXCHIP Inc.</center></h2>
******************************************************************************
*/ 

#ifndef __RingBufferUtils_h__
#define __RingBufferUtils_h__

#include "Common.h"

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




#ifndef __RingBufferUtils_h__
#define __RingBufferUtils_h__

#include <stdint.h>

/** @addtogroup MICO_Middleware_Interface
  * @{
  */

/** @defgroup MICO_RingBuffer MiCO Ring Buffer
  * @brief Provide APIs for Ring Buffer
  * @{
  */

typedef struct
{
  uint8_t*  buffer;
  uint32_t  size;
  volatile uint32_t  head; /* Read from */
  volatile uint32_t  tail; /* Write to */
} ring_buffer_t;

#ifndef MIN
#define MIN(x,y)  ((x) < (y) ? (x) : (y))
#endif /* ifndef MIN */

/**
 * @brief ?
 *
 * @param ring_buffer:   ?
 * @param      buffer:   ?
 * @param        size:   ?
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */
int ring_buffer_init( ring_buffer_t* ring_buffer, uint8_t* buffer, uint32_t size );


/**
 * @brief ?
 *
 * @param ring_buffer:   ?
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */
int ring_buffer_deinit( ring_buffer_t* ring_buffer );


/**
 * @brief ?
 *
 * @param ring_buffer:   ?
 *
 * @return   
 */
uint32_t ring_buffer_free_space( ring_buffer_t* ring_buffer );


/**
 * @brief ?
 *
 * @param ring_buffer:   ?
 *
 * @return   
 */
uint32_t ring_buffer_used_space( ring_buffer_t* ring_buffer );


/**
 * @brief ?
 *
 * @param ring_buffer:   ?
 * @param data:   ?
 * @param contiguous_bytes: ?
 *
 * @return   
 */
int ring_buffer_get_data( ring_buffer_t* ring_buffer, uint8_t** data, uint32_t* contiguous_bytes );


/**
 * @brief ?
 *
 * @param ring_buffer:      ?
 * @param bytes_consumed:   ?
 *
 * @return   
 */
int ring_buffer_consume( ring_buffer_t* ring_buffer, uint32_t bytes_consumed );



/**
 * @brief ?
 *
 * @param ring_buffer:   ?
 * @param data:   ?
 * @param data_length: ?
 *
 * @return   
 */
uint32_t ring_buffer_write( ring_buffer_t* ring_buffer, const uint8_t* data, uint32_t data_length );

uint8_t ring_buffer_is_full(ring_buffer_t *ring_buffer);

int ring_buffer_read( ring_buffer_t* ring_buffer, uint8_t* data, uint32_t data_length, uint32_t* number_of_bytes_read );

uint32_t ring_buffer_total_size(ring_buffer_t *ring_buffer);
/**
  * @}
  */

/**
  * @}
  */


#endif // __RingBufferUtils_h__



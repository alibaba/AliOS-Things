/**
 *************************************************************************************
 * @file    audio_utility.h
 * @brief   Audio Utility API Interface.
 *
 * @author  Aissen Li
 * @version V2.3.0
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 *************************************************************************************
 */

#ifndef __AUDIO_UTILITY_H__
#define __AUDIO_UTILITY_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

#define RT_FAILURE              0           /**< return code: failure */
#define RT_SUCCESS              1           /**< return code: success */
#define RT_NO                   0           /**< return code: no      */
#define RT_YES                  1           /**< return code: yes     */

/**
 * @brief IO Type, can be FILE, MEMORY...
 */
typedef enum _IOType
{
	IO_TYPE_FILE    = 0,        /**< FILE   */
	IO_TYPE_MEMORY              /**< MEMORY */
} IOType;

/**
 * @brief Buffer type. The buffer can be used for input (BUFFER_FOR_INPUT) or output (BUFFER_FOR_OUTPUT)
 */
typedef enum _BufferType
{
    BUFFER_FOR_INPUT  = 0,      /**< buffer for input from external storage */
    BUFFER_FOR_OUTPUT = 1,      /**< buffer for output to external storage  */
} BufferType;

/**
 * @brief PCM data layout mode
 */
typedef enum _PCM_DATA_MODE
{
    DATA_MODE_MONO_S = 0,       /**< Mono-S    : one channel, the pcm data save as "S", both low 16bits and high 16bits have data   */
    DATA_MODE_MONO_LOW,         /**< Mono-Low  : one channel, only low 16bits have data                                             */
    DATA_MODE_MONO_HIGH,        /**< Mono-High : one channel, only high 16bits have data                                            */
    DATA_MODE_STEREO_LR,        /**< LR....LR  : two channels, low 16bits are left data, high 16bits are right data                 */
    DATA_MODE_STEREO_RL,        /**< RL....RL  : two channels, low 16bits are right data, high 16bits are left data                 */
    DATA_MODE_STEREO_HIGH,      /**< Stero-High: two channels, only high 16bits have data                                           */
    DATA_MODE_STEREO_LOW,       /**< Stero-low : two channels, only low 16bits have data                                            */
    DATA_MODE_STEREO_S          /**< Stereo-S  : two channels, the pcm data save as "S", both low 16bits and high 16bits have data  */
} PCM_DATA_MODE;

/**
 * @brief Memory Handle Struct
 */
typedef struct _MemHandle
{
	uint8_t*   addr;            /**< memory base address            */
	uint32_t   mem_capacity;    /**< memory capacity in bytes       */
	uint32_t   mem_len;         /**< actual memory length in bytes  */
	uint32_t   p;               /**< current pointer in bytes       */
} MemHandle;

/**
 * @brief Buffer context for bit reading (input buffer) / putting (output buffer)
 */
typedef struct _BufferContext
{
	void*        io_handle;          /**< IO handle/pointer, may be FILE *, Memory address. */
	int32_t      io_type;            /**< IO type, value must be in @code IOType            */
	int32_t      buffer_type;        /**< buffer type, value must be in @code BufferType    */
	uint8_t*     buffer;             /**< buffer base address                               */
	int32_t      buffer_capacity;    /**< buffer capacity in bytes                          */
	int32_t      p;                  /**< current pointer in bits                           */
	int32_t      buffer_len;         /**< actual buffer length in bytes (input buffer) or end position when p turns around (output buffer) */
	int32_t      flush_pointer;      /**< current flush pointer in bytes                    */
} BufferContext;

/**
 * @brief  Get audio utility library version
 * @return audio utility library version string
 */
const uint8_t* GetLibVersionAudioUtility(void);

typedef uint32_t (*mv_mread_callback_t)(void *buffer, uint32_t length);

/**
 * @brief  Memory handle open
 * @param  mp       Pointer to a memory pointer, @see MemHandle.
 * @param  addr     memory base address
 * @param  capacity memory capacity in bytes
 * @param  callback mv_mread callback function
 * @return NONE
 */
void mv_mopen(MemHandle* mp, uint8_t* addr, uint32_t capacity, mv_mread_callback_t callback);

/**
 * @brief  Memory handle close
 * @param  mp       Pointer to a memory pointer, @see MemHandle.
 * @return NONE
 */
void mv_mclose(MemHandle* mp);

/**
 * @brief  Set mv_mread callback function
 * @param  callback mv_mread callback function
 * @return NONE
 */
void mv_mread_callback_set(mv_mread_callback_t callback);

/**
 * @brief  Unset mv_mread callback function
 * @param  NONE
 * @return NONE
 */
void mv_mread_callback_unset(void);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif//__AUDIO_UTILITY_H__

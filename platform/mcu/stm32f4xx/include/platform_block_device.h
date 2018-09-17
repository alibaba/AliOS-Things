
#pragma once


#include <stdint.h>
#include "mico_result.h"
#include "common.h"


#ifdef __cplusplus
extern "C" {
#endif



#define BLOCK_DEVICE_ERASE_NOT_REQUIRED (0)


#define BLOCK_DEVICE_WRITE_NOT_ALLOWED (0)



typedef enum
{
    BLOCK_DEVICE_UNINITIALIZED,
    BLOCK_DEVICE_DOWN,
    BLOCK_DEVICE_UP_READ_ONLY,
    BLOCK_DEVICE_UP_READ_WRITE,
} mico_block_device_status_t;

typedef enum
{
    BLOCK_DEVICE_READ_ONLY,
    BLOCK_DEVICE_WRITE_IMMEDIATELY,
    BLOCK_DEVICE_WRITE_BEHIND_ALLOWED
} mico_block_device_write_mode_t;


typedef struct mico_block_device_struct        mico_block_device_t;       
typedef struct mico_block_device_driver_struct mico_block_device_driver_t;

typedef void (*mico_block_device_status_change_callback_t)( mico_block_device_t* device, mico_block_device_status_t new_status );


typedef struct
{
    uint64_t                                    base_address_offset;                        
    uint64_t                                    maximum_size;                              
    mico_bool_t                                 volatile_and_requires_format_when_mounting; 
} mico_block_device_init_data_t;


struct mico_block_device_struct
{
    const mico_block_device_init_data_t*  init_data;
    const mico_block_device_driver_t*     driver;
    mico_bool_t                           initialized;
    uint32_t                              device_id;
    uint64_t                              device_size;
    uint32_t                              read_block_size;     
    uint32_t                              write_block_size;    
    uint32_t                              erase_block_size;     
    void*                                 device_specific_data; 
    mico_block_device_status_change_callback_t callback;
};

struct mico_block_device_driver_struct
{

    OSStatus (*init)( mico_block_device_t* device, mico_block_device_write_mode_t write_mode );

    OSStatus (*deinit)( mico_block_device_t* device );

    OSStatus (*erase)( mico_block_device_t* device, uint64_t start_address, uint64_t size );

    OSStatus (*write)( mico_block_device_t* device, uint64_t start_address, const uint8_t* data, uint64_t size );

    OSStatus (*flush)( mico_block_device_t* device );

    OSStatus (*read)( mico_block_device_t* device, uint64_t start_address, uint8_t* data, uint64_t size );

    OSStatus (*status)( mico_block_device_t* device, mico_block_device_status_t* status );

    OSStatus (*register_callback)( mico_block_device_t* device, mico_block_device_status_change_callback_t callback );

};

#ifdef __cplusplus
} /*extern "C" */
#endif


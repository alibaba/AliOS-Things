/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "hal/soc/soc.h"
#include "mico_rtos.h"
#include "platform_mcu_peripheral.h"
#include "debug.h"
#include "board.h"

extern const hal_logic_partition_t hal_partitions[];
extern platform_flash_driver_t platform_flash_drivers[HAL_FLASH_MAX];
extern const platform_flash_t platform_flash_peripherals[];

hal_logic_partition_t *hal_flash_get_info(hal_partition_t in_partition)
{
    hal_logic_partition_t *logic_partition;

    logic_partition = (hal_logic_partition_t *)&hal_partitions[ in_partition ];

    return logic_partition;
}

static OSStatus hal_flash_init( hal_partition_t in_partition )
{
  OSStatus err = kNoErr;
  hal_logic_partition_t *partition_info;
  
  require_action_quiet( in_partition > HAL_PARTITION_ERROR, exit, err = kParamErr );
  require_action_quiet( in_partition < HAL_PARTITION_MAX, exit, err = kParamErr );

  partition_info = hal_flash_get_info( in_partition );
  require_action_quiet( partition_info->partition_owner != HAL_FLASH_NONE, exit, err = kNotFoundErr );
  
  if( platform_flash_drivers[ partition_info->partition_owner ].flash_mutex == NULL){
    err = mico_rtos_init_mutex( &platform_flash_drivers[ partition_info->partition_owner ].flash_mutex );
    require_noerr( err, exit );
  }
  
  mico_rtos_lock_mutex( &platform_flash_drivers[ partition_info->partition_owner ].flash_mutex );
  
  err = platform_flash_init( &platform_flash_peripherals[ partition_info->partition_owner ] );
  platform_flash_drivers[ partition_info->partition_owner ].peripheral = (platform_flash_t *)&platform_flash_peripherals[ partition_info->partition_owner ];
  platform_flash_drivers[ partition_info->partition_owner ].initialized = true;
  mico_rtos_unlock_mutex( &platform_flash_drivers[ partition_info->partition_owner ].flash_mutex );
  
exit: 
  return err;
}

static OSStatus hal_flash_deinit( hal_partition_t in_partition )
{
  OSStatus err = kNoErr;
  hal_logic_partition_t *partition_info;
  
  require_action_quiet( in_partition > HAL_PARTITION_ERROR, exit, err = kParamErr );
  require_action_quiet( in_partition < HAL_PARTITION_MAX, exit, err = kParamErr );

  partition_info = MicoFlashGetInfo( in_partition );
  require_action_quiet( partition_info->partition_owner != HAL_FLASH_NONE, exit, err = kNotFoundErr );
  
  if( platform_flash_drivers[ partition_info->partition_owner ].flash_mutex == NULL){
    err = mico_rtos_init_mutex( &platform_flash_drivers[ partition_info->partition_owner ].flash_mutex );
    require_noerr( err, exit );
  }
  
  mico_rtos_lock_mutex( &platform_flash_drivers[ partition_info->partition_owner ].flash_mutex );
  
  err = platform_flash_init( &platform_flash_peripherals[ partition_info->partition_owner ] );
  platform_flash_drivers[ partition_info->partition_owner ].peripheral = (platform_flash_t *)&platform_flash_peripherals[ partition_info->partition_owner ];
  platform_flash_drivers[ partition_info->partition_owner ].initialized = true;
  mico_rtos_unlock_mutex( &platform_flash_drivers[ partition_info->partition_owner ].flash_mutex );
  
exit: 
  return err;
}

int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
  OSStatus err = kNoErr;
  uint32_t start_addr, end_addr;
  hal_logic_partition_t *partition_info;

  require_quiet( size != 0, exit);
  require_action_quiet( in_partition > HAL_PARTITION_ERROR && in_partition < HAL_PARTITION_MAX, exit, err = kParamErr );
  require_action_quiet( in_partition < HAL_PARTITION_MAX, exit, err = kParamErr );

  partition_info = hal_flash_get_info( in_partition );
  require_action_quiet( partition_info->partition_owner != HAL_FLASH_NONE, exit, err = kNotFoundErr );
#if (!defined BOOTLOADER) && (!defined FIRMWARE_DOWNLOAD)
  require_action_quiet( ( partition_info->partition_options & PAR_OPT_WRITE_MASK ) == PAR_OPT_WRITE_EN, exit, err = kPermissionErr );
#endif

  start_addr = partition_info->partition_start_addr + off_set;
  end_addr = partition_info->partition_start_addr + off_set + size - 1;

  require_action_quiet( end_addr < partition_info->partition_start_addr + partition_info->partition_length, exit, err = kParamErr );

  if( platform_flash_drivers[ partition_info->partition_owner ].initialized == false )
  {
    err =  hal_flash_init( in_partition );
    require_noerr_quiet( err, exit );
  }

  mico_rtos_lock_mutex( &platform_flash_drivers[ partition_info->partition_owner ].flash_mutex );
  err = platform_flash_erase( &platform_flash_peripherals[ partition_info->partition_owner ], start_addr, end_addr );
  mico_rtos_unlock_mutex( &platform_flash_drivers[ partition_info->partition_owner ].flash_mutex );

exit:
  return err;
}

int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set, const void *in_buf , uint32_t in_buf_len)
{
  OSStatus err = kNoErr;
  uint32_t start_addr, end_addr;
  hal_logic_partition_t *partition_info;

  require_quiet( in_buf_len != 0, exit);
  require_action_quiet( in_partition > HAL_PARTITION_ERROR && in_partition < HAL_PARTITION_MAX, exit, err = kParamErr );
  
  partition_info = hal_flash_get_info( in_partition );
  require_action_quiet( partition_info->partition_owner != HAL_FLASH_NONE, exit, err = kNotFoundErr );
#if (!defined BOOTLOADER) && (!defined FIRMWARE_DOWNLOAD)
  require_action_quiet( ( partition_info->partition_options & PAR_OPT_WRITE_MASK ) == PAR_OPT_WRITE_EN, exit, err = kPermissionErr );
#endif

  start_addr = partition_info->partition_start_addr + *off_set;
  end_addr = partition_info->partition_start_addr + *off_set + in_buf_len - 1;

  require_action_quiet( end_addr < partition_info->partition_start_addr + partition_info->partition_length , exit, err = kParamErr );
  
  if( platform_flash_drivers[ partition_info->partition_owner ].initialized == false )
  {
    err =  hal_flash_init( in_partition );
    require_noerr_quiet( err, exit );
  }

  mico_rtos_lock_mutex( &platform_flash_drivers[ partition_info->partition_owner ].flash_mutex );
  err = platform_flash_write( &platform_flash_peripherals[ partition_info->partition_owner ], &start_addr, in_buf, in_buf_len );
  *off_set = start_addr - partition_info->partition_start_addr;
  mico_rtos_unlock_mutex( &platform_flash_drivers[ partition_info->partition_owner ].flash_mutex );
  
exit:
  return err;
}

int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set, void *out_buf, uint32_t out_buf_len)
{
  OSStatus err = kNoErr;
  uint32_t start_addr, end_addr;
  hal_logic_partition_t *partition_info;

  require_quiet( out_buf_len != 0, exit);
  require_action_quiet( in_partition > HAL_PARTITION_ERROR && in_partition < HAL_PARTITION_MAX, exit, err = kParamErr );

  partition_info = hal_flash_get_info( in_partition );
  require_action_quiet( partition_info->partition_owner != HAL_FLASH_NONE, exit, err = kNotFoundErr );
#if (!defined BOOTLOADER) && (!defined FIRMWARE_DOWNLOAD)
  require_action_quiet( ( partition_info->partition_options & PAR_OPT_READ_MASK ) == PAR_OPT_READ_EN, exit, err = kPermissionErr );
#endif

  start_addr = partition_info->partition_start_addr + *off_set;
  end_addr = partition_info->partition_start_addr + *off_set + out_buf_len - 1;
  require_action_quiet( end_addr < partition_info->partition_start_addr + partition_info->partition_length , exit, err = kParamErr );

  if( platform_flash_drivers[ partition_info->partition_owner ].initialized == false )
  {
    err =  hal_flash_init( in_partition );
    require_noerr_quiet( err, exit );
  }

  mico_rtos_lock_mutex( &platform_flash_drivers[ partition_info->partition_owner ].flash_mutex );
  err = platform_flash_read( &platform_flash_peripherals[ partition_info->partition_owner ], &start_addr, out_buf, out_buf_len );
  *off_set = start_addr - partition_info->partition_start_addr;
  mico_rtos_unlock_mutex( &platform_flash_drivers[ partition_info->partition_owner ].flash_mutex );

exit:
  return err;
}

int32_t hal_flash_enable_secure(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

int32_t hal_flash_dis_secure(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    return 0;
}


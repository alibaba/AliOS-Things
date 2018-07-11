EN| [中文](Flash-and-KV-Storage-Porting-Guide.zh) 

Flash is foundation of many system components (KV storage, OTA, etc.). This article describes the key steps in porting of Flash and support for KV storage based on it.

## Porting of Flash adaptation layer
Code example for porting of flash adaptation layer can refer to [implement example](https://github.com/alibaba/AliOS-Things/blob/master/platform/mcu/stm32l4xx/hal/flash_port.c).  
It mainly concerns the following function modification :
```C
  hal_logic_partition_t *hal_flash_get_info(hal_partition_t in_partition)

  int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)

  int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set,
                          const void *in_buf, uint32_t in_buf_len)

  int32_t hal_flash_erase_write(hal_partition_t in_partition, uint32_t *off_set,
                                const void *in_buf, uint32_t in_buf_len)

  int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set, void *out_buf,
                         uint32_t in_buf_len)

  int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set,
                                  uint32_t size)  

  int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
```

## Porting of KV（related to flash HAL）
 * developers need to implement related interfaces in flash HAL;
* developers need to declare the dependency of components in Makefile : $(NAME) _COMPONENTS = modules.fs.kv;
* developer needs to define the flash partition number used by the KV through CONFIG_AOS_KV_PTN 
* if the minimum erasure unit of the flash used by developers is greater than 4096 bytes, the size of the logical block in KV should be adjusted (the default is 4096 bytes).
* Developers need to define the size of the flash partition specified through CONFIG_AOS_KV_BUFFER_SIZE, which should more than 2 KV's logical block size, and the default value is 8192 bytes.
* KV length limit:
    - the maximum length should be less than 255 bytes;
    - the maximum length can be set in definition of ITEM_MAX_VAL_LEN, with a default value of 512 bytes.

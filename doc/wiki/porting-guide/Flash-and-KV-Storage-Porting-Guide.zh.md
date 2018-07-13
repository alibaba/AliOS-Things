[EN](Flash-and-KV-Storage-Porting-Guide) | 中文

Flash是多个系统组件（KV存储，OTA等）的基础，本文描述Flash对接，以及Flash基础上支持KV存储的关键步骤。

## Flash 抽象层对接
flash抽象层移植代码示例，[参考实现](https://github.com/alibaba/AliOS-Things/blob/master/platform/mcu/stm32l4xx/hal/flash_port.c)。  
主要涉及到以下函数的相关修改：
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

## KV组件移植（与flash hal层相关）
 * 开发者需要实现相关flash hal层接口；
 * 开发者需通过在Makefile中声明组件依赖关系：$(NAME)_COMPONENTS += modules.fs.kv；
 * 开发者需通过CONFIG_AOS_KV_PTN宏定义指定KV组件所使用的flash分区号；
 * 若开发者所使用的flash介质的最小擦除单位大于4096 bytes，需调整KV组件内的逻辑块大小（默认为4096 bytes）；
 * 开发者需通过CONFIG_AOS_KV_BUFFER_SIZE宏定义指定KV组件所使用的flash分区大小（不能小于2个kv组件的逻辑块大小，默认值为8192 bytes）；
 * kv键值长度限制：
    * 最大键(key)长度小于255 bytes;
    * 最大值(value)长度可通过ITEM_MAX_VAL_LEN宏定义进行设置，预设值为512 bytes。

@page flash_driver_development Flash设备驱动程序开发指南

# 1. 前言
本文介绍如何编写符合AliOS Things标准的Flash设备驱动程序。

# 2. 头文件
在程序中使用本文提及的功能应包含头文件 *aos/flash_core.h* 。

# 3. 数据结构
AliOS Things提供flash设备的抽象基础结构：
```c
typedef struct aos_flash aos_flash_t;
```
采用结构体嵌套的方式从基础结构派生出具体的硬件结构。派生类型应包含具体硬件操作所需的各种变量。例如：
```c
typedef struct {
    aos_flash_t flash;
    /* private data */
    void *reg_base;
    uint32_t buf[FLASH_ABC_PAGE_SIZE / sizeof(uint32_t)];
} flash_abc_t;
```
使用宏`aos_container_of`实现从基础结构指针到派生结构指针的转换。例如：
```c
aos_flash_t *flash_dev = foo();
flash_abc_t *flash_abc = aos_container_of(flash_dev, flash_abc_t, flash);
```

# 4. 注册及注销
## 4.1. 注册
AliOS Things提供如下函数用于注册flash设备：
```c
aos_status_t aos_flash_register(aos_flash_t *flash);
```
调用注册函数之前，BSP开发者应自行分配一个`aos_flash_t`类型或其派生类型的变量并对包含的如下变量赋值：
* `dev.id`：`uint32_t`类型，表示该设备的ID。不同flash设备不能拥有相同的ID。
* `ops`：`const aos_flash_ops_t *`类型，指向一组面向硬件的回调函数：
```c
typedef struct aos_flash_ops {
    void (*unregister)(aos_flash_t *flash);
    aos_status_t (*startup)(aos_flash_t *flash);
    void (*shutdown)(aos_flash_t *)flash;
    aos_status_t (*read_page)(aos_flash_t *flash, uint64_t page, size_t data_offset, size_t data_count, size_t spare_offset, size_t spare_count);
    aos_status_t (*write_page)(aos_flash_t *flash, uint64_t page, size_t data_offset, size_t data_count, size_t spare_offset, size_t spare_count);
    aos_status_t (*erase_block)(aos_flash_t *flash, uint64_t block);
    aos_status_t (*is_bad_block)(aos_flash_t *flash, uint64_t block);
    aos_status_t (*mark_bad_block)(aos_flash_t *flash, uint64_t block);
} aos_flash_ops_t;
```
* `flags`：`uint32_t`类型，可包含如下字段，各字段使用按位或运算连接：
    + 类型，掩码为`AOS_FLASH_F_TYPE_MASK`，取值必须为以下当中的一个：
        - `AOS_FLASH_F_TYPE_NOR`：表示NOR flash。
        - `AOS_FLASH_F_TYPE_NAND`：表示NAND flash。

    + ECC强度，掩码为`AOS_FLASH_F_ECC_STRENGTH_MASK`，取值为`AOS_FLASH_F_ECC_STRENGTH(x)`，其中`x`是单次ECC算法能纠正的最大比特翻转数目。如果一种NAND flash包含了多种ECC算法（例如in-band区域和out-of-band区域采用了不同的ECC算法），`x`取这些算法当中最大的一个。对于NOR flash，`x`应为0值。

* `block_count`：`uint64_t`类型，表示该flash设备包含的块数。
* `pages_per_block`：`size_t`类型，表示每个块包含的页数。
* `data_size`：`size_t`类型，表示每个页包含的in-band数据字节数。
* `spare_size`：`size_t`类型，表示每个页包含的out-of-band数据字节数。注意这里指的是暴露给用户的区域，不包含用于存储ECC附加数据和bad mark的区域。对于NOR flash，`spare_size`应设置为0值。
* `data_buf`：`void *`类型，指向存放in-band数据的缓冲区，该区域的尺寸不应小于`data_size`。应在注册前预先分配缓冲区。
* `spare_buf`：`void *`类型，指向存放out-of-band数据的缓冲区，该区域的尺寸不应小于`spare_size`。应在注册前预先分配缓冲区。对于NOR flash，`spare_buf`应设置为`NULL`。驱动程序可根据硬件特性确定in-band缓冲区和out-of-band缓冲区的相对位置和对齐方式。

调用注册函数之前，BSP开发者应初始化派生类型中的私有变量，并执行具体硬件相关的注册时初始化工作（例如映射寄存器地址等）。

## 4.2. 注销
AliOS Things提供如下函数用于注销flash设备：
```c
aos_status_t aos_flash_unregister(uint32_t id);
```
调用此函数之后，BSP开发者可回收相关联的`aos_flash_t`类型或其派生类型的变量。

# 5. 回调函数
驱动程序应实现`aos_flash_ops_t`定义的一组面向硬件的回调函数。

## 5.1. unregister
```c
void (*unregister)(aos_flash_t *flash);
```
`unregister`回调函数在设备注销时被调用，可在该函数中执行具体硬件相关的注销时反初始化工作（例如解除寄存器地址映射等）。

## 5.2. startup
```c
aos_status_t (*startup)(aos_flash_t *flash);
```
`startup`回调函数在设备引用计数从0增加到1时被调用，可在该函数中执行具体硬件相关的运行时初始化工作。初始化成功时应返回0；失败时应返回errno（负值）。

## 5.3. shutdown
```c
void (*shutdown)(aos_flash_t *flash);
```
`shutdown`回调函数在设备引用计数从1减小到0时被调用，可在该函数中执行具体硬件相关的运行时反初始化工作。

## 5.4. read_page
```c
aos_status_t (*read_page)(aos_flash_t *flash, uint64_t page, size_t data_offset, size_t data_count, size_t spare_offset, size_t spare_count);
```
`read_page`回调函数在读取页数据时被调用。

参数`page`表示页编号；参数`data_offset`表示读取数据在in-band区域中的偏移量；参数`data_count`表示in-band数据读取字节数；参数`spare_offset`表示读取数据在（暴露给用户的）out-of-band区域中的偏移量；参数`spare_count`表示out-of-band数据读取字节数。

读取到的数据应写入`flash->data_buf`和`flash->spare_buf`指向的缓冲区中，在各自缓冲区中的偏移量由`data_offset`和`spare_offset`决定。

对于NAND flash，读取成功时应返回ECC算法纠正的比特翻转数目；如果包含多个ECC过程，返回多个比特翻转纠正数目中的最大值；如果包含多种ECC算法，比特翻转纠正数目应按比例折算后再取最大值；如果存在ECC算法无法纠正的错误，应返回`AOS_FLASH_ECC_ERROR`（一个很大的正值）。对于NOR flash，读取成功时应返回0。读取失败时应返回errno（负值）。

## 5.5. write_page
```c
aos_status_t (*write_page)(aos_flash_t *flash, uint64_t page, size_t data_offset, size_t data_count, size_t spare_offset, size_t spare_count);
```
`write_page`回调函数在写入页数据时被调用。

参数`page`表示页编号；参数`data_offset`表示写入数据在in-band区域中的偏移量；参数`data_count`表示in-band数据写入字节数；参数`spare_offset`表示写入数据在（暴露给用户的）out-of-band区域中的偏移量；参数`spare_count`表示out-of-band数据写入字节数。

待写入的数据存放在`flash->data_buf`和`flash->spare_buf`指向的缓冲区中，在各自缓冲区中的偏移量由`data_offset`和`spare_offset`决定。`write_page`回调函数可自行调整缓冲区中的数据和排列位置以适配硬件特性。

参数`data_offset`、`data_count`、`spare_offset`、`spare_count`决定了页当中是否包含未被写入的区域。对于NOR flash，页当中未被写入的区域应保留原来的数据；对于NAND flash，页当中未被写入的区域的行为取决于具体实现。

写入成功时应返回0；写入失败时应返回errno（负值）。

## 5.6. erase_block
```c
aos_status_t (*erase_block)(aos_flash_t *flash, uint64_t block);
```
`erase_block`回调函数在擦除一个块时被调用。参数`block`表示块序号。成功时应返回0；失败时应返回errno（负值）。

## 5.7. is_bad_block
```c
aos_status_t (*is_bad_block)(aos_flash_t *flash, uint64_t block);
```
`is_bad_block`回调函数在检查一个块是否为坏块时被调用。参数`block`表示块序号。是坏块应返回1，不是坏块应返回0；失败时应返回errno（负值）。

## 5.8. mark_bad_block
```c
aos_status_t (*mark_bad_block)(aos_flash_t *flash, uint64_t block);
```
`mark_bad_block`回调函数在标记一个块为坏块时被调用。参数`block`表示块序号。成功时应返回0；失败时应返回errno（负值）。

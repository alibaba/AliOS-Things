@page spi_driver_development SPI Master设备驱动程序开发指南

**[更正文档](https://gitee.com/alios-things/documentation/edit/rel_3.3.0/manual/driver_development/spi_driver_development.md)** &emsp;&emsp;&emsp;&emsp; **[贡献说明](https://help.aliyun.com/document_detail/302301.html)**

# 1. 前言
本文介绍如何编写符合AliOS Things标准的SPI master设备驱动程序。

# 2. 头文件
在程序中使用本文提及的功能应包含头文件 *aos/spi_core.h* 。

# 3. 数据结构
AliOS Things提供SPI master设备的抽象基础结构：
```c
typedef struct aos_spi aos_spi_t;
```
采用结构体嵌套的方式从基础结构派生出具体的硬件结构。派生类型应包含具体硬件操作所需的各种变量。例如：
```c
typedef struct {
    aos_spi_t spi;
    /* private data */
    void *reg_base;
    int irq_num;
} spi_abc_t;
```
使用宏`aos_container_of`实现从基础结构指针到派生结构指针的转换。例如：
```c
aos_spi_t *spi_dev = foo();
spi_abc_t *spi_abc = aos_container_of(spi_dev, spi_abc_t, spi);
```

# 4. 注册及注销
## 4.1. 注册
AliOS Things提供如下函数用于注册SPI master设备：
```c
aos_status_t aos_spi_register(aos_spi_t *spi);
```
调用注册函数之前，BSP开发者应自行分配一个`aos_spi_t`类型或其派生类型的变量并对包含的如下变量赋值：
* `dev.id`：`uint32_t`类型，表示该设备的ID。不同SPI master设备不能拥有相同的ID。
* `ops`：`const aos_spi_ops_t *`类型，指向一组面向硬件的回调函数：
```c
typedef struct {
    void (*unregister)(aos_spi_t *spi);
    aos_status_t (*startup)(aos_spi_t *spi);
    void (*shutdown)(aos_spi_t *spi);
    aos_status_t (*start_xfer)(aos_spi_t *spi);
    void (*finish_xfer)(aos_spi_t *spi);
    void (*abort_xfer)(aos_spi_t *spi);
} aos_spi_ops_t;
```
* `flags`：`uint32_t`类型，可包含如下字段，各字段使用按位或运算连接：
    + `AOS_SPI_F_MODE_0`：表示支持SPI mode0。
    + `AOS_SPI_F_MODE_1`：表示支持SPI mode1。
    + `AOS_SPI_F_MODE_2`：表示支持SPI mode2。
    + `AOS_SPI_F_MODE_3`：表示支持SPI mode3。
    + `AOS_SPI_F_WIDTH_1`：表示支持单线传输。
    + `AOS_SPI_F_WIDTH_2`：表示支持双线传输。
    + `AOS_SPI_F_WIDTH_4`：表示支持四线传输。
    + `AOS_SPI_F_WIDTH_8`：表示支持八线传输。
    + `AOS_SPI_F_4WIRE`：表示单线传输支持四线模式（MISO和MOSI分开）。
    + `AOS_SPI_F_3WIRE`：表示单线传输支持三线模式（MISO和MOSI复用）。
    + `AOS_SPI_F_MSB_FIRST`：表示支持每字节MSB在前传输。
    + `AOS_SPI_F_LSB_FIRST`：表示支持每字节LSB在前传输。
    + `AOS_SPI_F_NO_CS`：表示该设备只与一个SPI slave设备连接，无需操作CS。此标志有效时，注册函数会把CS数目设置为1。

* `num_cs`: `uint32_t`类型，表示该装置支持的CS数目。
* `min_hz`：`uint32_t`类型，表示该设备传输数据时的最小时钟频率。
* `max_hz`：`uint32_t`类型，表示该设备传输数据时的最大时钟频率。

调用注册函数之前，BSP开发者应初始化派生类型中的私有变量，并执行具体硬件相关的注册时初始化工作（例如映射寄存器地址等）。

## 4.2. 注销
AliOS Things提供如下函数用于注销SPI master设备：
```c
aos_status_t aos_spi_unregister(uint32_t id);
```
调用此函数之后，BSP开发者可回收相关联的`aos_spi_t`类型或其派生类型的变量。

# 5. 工作模式
一次完整的SPI数据传输在AliOS Things中称为一个 **message** 。

相邻的相同CS且相同mode的一个或多个message在AliOS Things中称为一个 **sequence** 。同一个sequence中CS信号会自始至终保持有效。同一个sequence中的每一message的传输方向、数据线数、时钟频率可以不同。

每个message根据AliOS Things SPI设备驱动数据缓冲区的尺寸拆分为 **transfer** 。每个transfer的最大数据长度为`AOS_SPI_BUF_SIZE`。AliOS Things SPI设备驱动以 **transfer** 为单位传输数据。

# 6. 回调函数
驱动程序应实现`aos_spi_ops_t`定义的一组面向硬件的回调函数。

## 6.1. unregister
```c
void (*unregister)(aos_spi_t *spi);
```
`unregister`回调函数在设备注销时被调用，可在该函数中执行具体硬件相关的注销时反初始化工作（例如解除寄存器地址映射等）。

## 6.2. startup
```c
aos_status_t (*startup)(aos_spi_t *spi);
```
`startup`回调函数在设备引用计数从0增加到1时被调用，可在该函数中执行具体硬件相关的运行时初始化工作。初始化成功时应返回0；失败时应返回errno（负值）。

## 6.3. shutdown
```c
void (*shutdown)(aos_spi_t *spi);
```
`shutdown`回调函数在设备引用计数从1减小到0时被调用，可在该函数中执行具体硬件相关的运行时反初始化工作。

## 6.4. start_xfer
```c
aos_status_t (*start_xfer)(aos_spi_t *spi);
```
`start_xfer`回调函数在设备开始发起一次 **transfer** 传输时被调用，可在该函数中控制硬件发起传输（例如操作FIFO或DMA）。发起成功后返回0；失败时应返回errno（负值）。该函数不需等待数据全部传输完成后再返回，可在硬件中断处理程序中处理后续工作。

`start_xfer`回调函数或中断处理程序可访问`spi`指向的如下成员变量获取或修改传输信息：
* `x.flags`：`uint32_t`类型，可包含如下字段，各字段使用按位或运算连接：
    + `AOS_SPI_XF_MSG_HEAD`：表示当前transfer是message中的第一个transfer。
    + `AOS_SPI_XF_MSG_TAIL`：表示当前transfer是message中的最后一个transfer。
    + `AOS_SPI_XF_SEQ_HEAD`：表示当前transfer是sequence中的第一个transfer。此标志有效时应将CS信号设置为有效。
    + `AOS_SPI_XF_SEQ_TAIL`：表示当前transfer是sequence中的最后一个transfer。如果此硬件可指定传输结束后取消CS信号，应在此标志有效时指定传输结束后将CS信号设置为无效。
    + `AOS_SPI_XF_RX`：表示正在接收数据。
    + `AOS_SPI_XF_TX`：表示正在发送数据。`AOS_SPI_XF_RX`标志和`AOS_SPI_XF_TX`标志可能同时有效。

* `x.cfg`：`uint32_t`类型，可包含如下字段，各字段使用按位或运算连接：
    + Mode，掩码为`AOS_SPI_MCFG_MODE_MASK`，取值必须为以下当中的一个：
        - `AOS_SPI_MCFG_MODE_0`：表示SPI mode0。
        - `AOS_SPI_MCFG_MODE_1`：表示SPI mode1。
        - `AOS_SPI_MCFG_MODE_2`：表示SPI mode2。
        - `AOS_SPI_MCFG_MODE_3`：表示SPI mode3。
    + 传输线数，掩码为`AOS_SPI_MCFG_WIDTH_MASK`，取值必须为以下当中的一个：
        - `AOS_SPI_MCFG_WIDTH_1`：表示单线传输。
        - `AOS_SPI_MCFG_WIDTH_2`：表示双线传输。
        - `AOS_SPI_MCFG_WIDTH_4`：表示四线传输。
        - `AOS_SPI_MCFG_WIDTH_8`：表示八线传输。
    + `AOS_SPI_MCFG_LSB_FIRST`：该标志有效时表示传输时LSB在前，否则为MSB在前。
    + `AOS_SPI_MCFG_3WIRE`：该标志有效时表示使用三线模式（MISO和MOSI复用），否则为四线模式。只在单线传输时有意义。

* `x.cs`：`uint32_t`类型，表示当前CS序号。
* `x.hz`：`uint32_t`类型，表示当前传输时钟频率。
* `x.pre_cs`：`uint32_t`类型，表示从设置mode到CS信号有效之间的时间，单位为纳秒。
* `x.post_cs`：`uint32_t`类型，表示设置CS信号无效之后的保持时间，单位为纳秒。
* `x.pre_clk`：`uint32_t`类型，表示从设置CS信号有效到时钟信号开始产生之间的时间，单位为纳秒。
* `x.post_clk`：`uint32_t`类型，表示时钟信号结束到设置CS信号无效之间的时间，单位为纳秒。
* `x.timeout`：`uint32_t`类型，表示此次传输的超时时间，单位为毫秒。`start_xfer`回调函数可修改该变量来指定超时时间，默认值为1000。中断处理程序不应修改该变量。

硬件基于FIFO或DMA的一次传输称为 **硬件传输** 。一次硬件传输的最大长度等于FIFO深度或DMA数据最大长度，考虑到硬件传输最大长度有限，一个transfer可能包含一次或多次硬件传输。

AliOS Things提供如下函数操作硬件传输。这些函数可以在`start_xfer`回调函数或中断处理程序中调用，支持在关中断或者自旋锁加锁环境下调用。
```c
size_t aos_spi_hard_push(aos_spi_t *spi, void *tx_buf, size_t count);
```
使用`aos_spi_hard_push`获取下一次硬件传输的实际长度。参数`count`为硬件传输最大长度；返回值为下一次硬件传输的实际长度。如果当前正在发送数据，下一次硬件传输的数据将被复制到`tx_buf`指向的空间，驱动程序可将这些数据送给FIFO或DMA并发起下一次硬件传输。
```c
bool aos_spi_hard_pull(aos_spi_t *spi, const void *rx_buf, size_t count);
```
一次硬件传输成功后（例如FIFO或DMA操作完成并产生中断），使用`aos_spi_hard_pull`通知设备已完成本次硬件传输。参数`count`为本次硬件传输的实际长度。如果当前正在接收数据，驱动程序应在调用该函数之前从FIFO或DMA获取数据并存放到`rx_buf`指向的空间。如果当前transfer已全部完成，该函数返回`true`；否则返回`false`，驱动程序应再次调用`aos_spi_hard_push`并发起下一次硬件传输。
```c
void aos_spi_hard_fail(aos_spi_t *spi);
```
如果硬件传输过程中出现异常，使用`aos_spi_hard_fail`通知设备传输失败。

## 6.5. finish_xfer
```c
void (*finish_xfer)(aos_spi_t *spi);
```
`finish_xfer`回调函数在设备完成一次transfer传输时被调用。如果`AOS_SPI_XF_SEQ_TAIL`标志有效且此硬件需手动取消CS信号，应在此时将CS信号设置为无效。

## 6.6. abort_xfer
```c
void (*abort_xfer)(aos_spi_t *spi);
```
`abort_xfer`回调函数在设备传输超时或失败时被调用，应在该函数中取消FIFO或DMA操作，禁用相关中断，并将CS信号设置为无效。

## 6.7. transfer_sequence
```c
aos_status_t (*transfer_sequence)(aos_spi_t *spi, const aos_spi_msg_t *msgs, size_t num_msgs);
```
`transfer_sequence`函数指针不为`NULL`时，驱动程序不再使用`start_xfer`、`finish_xfer`、`abort_xfer`，而是通过`transfer_sequence`实现一个 **sequence** 的数据传输。

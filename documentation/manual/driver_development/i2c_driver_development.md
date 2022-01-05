@page i2c_driver_development I2C Master设备驱动程序开发指南

**[更正文档](https://gitee.com/alios-things/documentation/edit/rel_3.3.0/manual/driver_development/i2c_driver_development.md)** &emsp;&emsp;&emsp;&emsp; **[贡献说明](https://help.aliyun.com/document_detail/302301.html)**

# 1. 前言
本文介绍如何编写符合AliOS Things标准的I2C master设备驱动程序。

# 2. 头文件
在程序中使用本文提及的功能应包含头文件 *aos/i2c_core.h* 。

# 3. 数据结构
AliOS Things提供I2C master设备的抽象基础结构：
```c
typedef struct aos_i2c aos_i2c_t;
```
采用结构体嵌套的方式从基础结构派生出具体的硬件结构。派生类型应包含具体硬件操作所需的各种变量。例如：
```c
typedef struct {
    aos_i2c_t i2c;
    /* private data */
    void *reg_base;
    int irq_num;
} i2c_abc_t;
```
使用宏`aos_container_of`实现从基础结构指针到派生结构指针的转换。例如：
```c
aos_i2c_t *i2c_dev = foo();
i2c_abc_t *i2c_abc = aos_container_of(i2c_dev, i2c_abc_t, i2c);
```

# 4. 注册及注销
## 4.1. 注册
AliOS Things提供如下函数用于注册I2C master设备：
```c
aos_status_t aos_i2c_register(aos_i2c_t *i2c);
```
调用注册函数之前，BSP开发者应自行分配一个`aos_i2c_t`类型或其派生类型的变量并对包含的如下变量赋值：
* `dev.id`：`uint32_t`类型，表示该设备的ID。不同I2C master设备不能拥有相同的ID。
* `ops`：`const aos_i2c_ops_t *`类型，指向一组面向硬件的回调函数：
```c
typedef struct {
    void (*unregister)(aos_i2c_t *i2c);
    aos_status_t (*startup)(aos_i2c_t *i2c);
    void (*shutdown)(aos_i2c_t *i2c);
    aos_status_t (*set_hz)(aos_i2c_t *i2c);
    aos_status_t (*start_xfer)(aos_i2c_t *i2c);
    void (*finish_xfer)(aos_i2c_t *i2c);
    void (*abort_xfer)(aos_i2c_t *i2c);
    aos_status_t (*transfer_sequence)(aos_i2c_t *i2c, const aos_i2c_msg_t *msgs, size_t num_msgs);
} aos_i2c_ops_t;
```
* `flags`：`uint32_t`类型，可包含如下字段，各字段使用按位或运算连接：
    + `AOS_I2C_F_ADDR_10`：表示支持10位地址模式。

* `hz`：`uint32_t`类型，表示该设备传输数据时的总线时钟频率。

调用注册函数之前，BSP开发者应初始化派生类型中的私有变量，并执行具体硬件相关的注册时初始化工作（例如映射寄存器地址等）。

## 4.2. 注销
AliOS Things提供如下函数用于注销I2C master设备：
```c
aos_status_t aos_i2c_unregister(uint32_t id);
```
调用此函数之后，BSP开发者可回收相关联的`aos_i2c_t`类型或其派生类型的变量。

# 5. 工作模式
一次完整的I2C数据传输（以START（或repeated START）信号开始，随后是slave地址和读写标志，随后是一定长度的数据，以STOP（或repeated）信号结束）在AliOS Things中称为一个 **message** 。

相邻的同slave地址的一个或多个message在AliOS Things中称为一个 **sequence** 。同一个sequence中的第一个message以START信号开始，最后一个message以STOP信号结束，中间各message用repeated START信号连接。

每个message根据AliOS Things I2C设备驱动数据缓冲区的尺寸拆分为 **transfer** 。每个transfer的最大数据长度为`AOS_I2C_BUF_SIZE`。AliOS Things I2C设备驱动以 **transfer** 为单位传输数据。

# 6. 回调函数
驱动程序应实现`aos_i2c_ops_t`定义的一组面向硬件的回调函数。

## 6.1. unregister
```c
void (*unregister)(aos_i2c_t *i2c);
```
`unregister`回调函数在设备注销时被调用，可在该函数中执行具体硬件相关的注销时反初始化工作（例如解除寄存器地址映射等）。

## 6.2. startup
```c
aos_status_t (*startup)(aos_i2c_t *i2c);
```
`startup`回调函数在设备引用计数从0增加到1时被调用，可在该函数中执行具体硬件相关的运行时初始化工作。初始化成功时应返回0；失败时应返回errno（负值）。

## 6.3. shutdown
```c
void (*shutdown)(aos_i2c_t *i2c);
```
`shutdown`回调函数在设备引用计数从1减小到0时被调用，可在该函数中执行具体硬件相关的运行时反初始化工作。

## 6.4. set_hz
```c
aos_status_t (*set_hz)(aos_i2c_t *i2c);
```
`set_hz`回调函数在修改总线时钟频率时被调用，驱动程序应在该函数中根据新时钟频率（存放在`i2c->hz`变量中）修改硬件状态。修改成功后返回0；失败时应返回errno（负值），且驱动程序应将硬件状态恢复到此函数被调用之前的状态。该函数指针为`NULL`时禁止动态修改总线时钟频率，设备将一直维持注册时指定的时钟频率。

## 6.5. start_xfer
```c
aos_status_t (*start_xfer)(aos_i2c_t *i2c);
```
`start_xfer`回调函数在设备开始发起一次 **transfer** 传输时被调用，可在该函数中控制硬件发起传输（例如操作FIFO或DMA）。发起成功后返回0；失败时应返回errno（负值）。该函数不需等待数据全部传输完成后再返回，可在硬件中断处理程序中处理后续工作。

`start_xfer`回调函数或中断处理程序可访问`i2c`指向的如下成员变量获取或修改传输信息：
* `x.flags`：`uint32_t`类型，可包含如下字段，各字段使用按位或运算连接：
    + `AOS_I2C_XF_MSG_HEAD`：表示当前transfer是message中的第一个transfer。硬件在传输数据之前应依次发出START信号（如果当前也是sequence中的第一个transfer）或repeated START信号（如果当前不是sequence中的第一个transfer）、slave地址、传输方向标志。
    + `AOS_I2C_XF_MSG_TAIL`：表示当前transfer是message中的最后一个transfer。此标志有效且当前传输方向为接收时，最后一个字节接受后应发出NACK信号。
    + `AOS_I2C_XF_SEQ_HEAD`：表示当前transfer是sequence中的第一个transfer。
    + `AOS_I2C_XF_SEQ_TAIL`：表示当前transfer是sequence中的最后一个transfer。如果此硬件可指定传输结束后自动发出STOP信号，应在此标志有效时指定发出STOP信号。

* `x.cfg`：`uint32_t`类型，可包含如下字段，各字段使用按位或运算连接：
    + `AOS_I2C_MCFG_RX`：该标志有效时表示传输方向为接收，否则为发送。
    + `AOS_I2C_MCFG_ADDR_10`：该标志有效时表示10位地址模式，否则为7位地址模式。

* `x.addr`：`uint16_t`类型，表示slave地址。
* `x.timeout`：`uint32_t`类型，表示此次传输的超时时间，单位为毫秒。`start_xfer`回调函数可修改该变量来指定超时时间，默认值为1000。中断处理程序不应修改该变量。

硬件基于FIFO或DMA的一次传输称为 **硬件传输** 。一次硬件传输的最大长度等于FIFO深度或DMA数据最大长度，考虑到硬件传输最大长度有限，一个transfer可能包含一次或多次硬件传输。

AliOS Things提供如下函数操作硬件传输。这些函数可以在`start_xfer`回调函数或中断处理程序中调用，支持在关中断或者自旋锁加锁环境下调用。
```c
size_t aos_i2c_hard_push(aos_i2c_t *i2c, void *tx_buf, size_t count);
```
使用`aos_i2c_hard_push`获取下一次硬件传输的实际长度。参数`count`为硬件传输最大长度；返回值为下一次硬件传输的实际长度。如果当前传输方向为发送，下一次硬件传输的数据将被复制到`tx_buf`指向的空间，驱动程序可将这些数据送给FIFO或DMA并发起下一次硬件传输。
```c
bool aos_i2c_hard_pull(aos_i2c_t *i2c, const void *rx_buf, size_t count);
```
一次硬件传输成功后（例如FIFO或DMA操作完成并产生中断），使用`aos_i2c_hard_pull`通知设备已完成本次硬件传输。参数`count`为本次硬件传输的实际长度。如果当前传输方向为接收，驱动程序应在调用该函数之前从FIFO或DMA获取数据并存放到`rx_buf`指向的空间。如果当前transfer已全部完成，该函数返回`true`；否则返回`false`，驱动程序应再次调用`aos_i2c_hard_push`并发起下一次硬件传输。
```c
void aos_i2c_hard_fail(aos_i2c_t *i2c);
```
如果硬件传输过程中出现异常，使用`aos_i2c_hard_fail`通知设备传输失败。

## 6.6. finish_xfer
```c
void (*finish_xfer)(aos_i2c_t *i2c);
```
`finish_xfer`回调函数在设备完成一次transfer传输时被调用。如果`AOS_I2C_XF_SEQ_TAIL`标志有效且此硬件需手动发出STOP信号，应在此时发出STOP信号。

## 6.7. abort_xfer
```c
void (*abort_xfer)(aos_i2c_t *i2c);
```
`abort_xfer`回调函数在设备传输超时或失败时被调用，应在该函数中取消FIFO或DMA操作，禁用相关中断，恢复总线空闲状态。

## 6.8. transfer_sequence
```c
aos_status_t (*transfer_sequence)(aos_i2c_t *i2c, const aos_i2c_msg_t *msgs, size_t num_msgs);
```
`transfer_sequence`函数指针不为`NULL`时，驱动程序不再使用`start_xfer`、`finish_xfer`、`abort_xfer`，而是通过`transfer_sequence`实现一个 **sequence** 的数据传输。

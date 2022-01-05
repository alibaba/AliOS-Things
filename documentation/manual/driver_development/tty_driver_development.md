@page tty_driver_development TTY设备驱动程序开发指南

**[更正文档](https://gitee.com/alios-things/documentation/edit/rel_3.3.0/manual/driver_development/tty_driver_development.md)** &emsp;&emsp;&emsp;&emsp; **[贡献说明](https://help.aliyun.com/document_detail/302301.html)**

# 1. 前言
本文介绍如何编写符合AliOS Things标准的teletype设备（一般用于实现UART或虚拟终端）驱动程序。

# 2. 头文件
在程序中使用本文提及的功能应包含头文件 *aos/tty_core.h* 。

# 3. 数据结构
AliOS Things提供TTY设备的抽象基础结构：
```c
typedef struct aos_tty aos_tty_t;
```
采用结构体嵌套的方式从基础结构派生出具体的硬件结构。派生类型应包含具体硬件操作所需的各种变量。例如：
```c
typedef struct {
    aos_tty_t tty;
    /* private data */
    void *reg_base;
    int irq_num;
} tty_abc_t;
```
使用宏`aos_container_of`实现从基础结构指针到派生结构指针的转换。例如：
```c
aos_tty_t *tty_dev = foo();
tty_abc_t *tty_abc = aos_container_of(tty_dev, tty_abc_t, tty);
```

# 4. 注册及注销
## 4.1. 注册
AliOS Things提供如下函数用于注册TTY设备：
```c
aos_status_t aos_tty_register(aos_tty_t *tty);
```
调用注册函数之前，BSP开发者应自行分配一个`aos_tty_t`类型或其派生类型的变量并对包含的如下变量赋值：
* `dev.id`：`uint32_t`类型，表示该设备的ID。不同TTY设备不能拥有相同的ID。
* `ops`：`const aos_tty_ops_t *`类型，指向一组面向硬件的回调函数：
```c
typedef struct {
    void (*unregister)(aos_tty_t *tty);
    aos_status_t (*startup)(aos_tty_t *tty);
    void (*shutdown)(aos_tty_t *tty);
    aos_status_t (*set_attr)(aos_tty_t *tty);
    void (*enable_rx)(aos_tty_t *tty);
    void (*disable_rx)(aos_tty_t *tty);
    void (*start_tx)(aos_tty_t *tty);
    void (*stop_tx)(aos_tty_t *tty);
} aos_tty_ops_t;
```
* `flags`：`uint32_t`类型，可包含如下字段，各字段使用按位或运算连接：
    + `AOS_TTY_F_UNIQUE_REF`：表示该设备只能同时被引用一次。

调用注册函数之前，BSP开发者应初始化派生类型中的私有变量，并执行具体硬件相关的注册时初始化工作（例如映射寄存器地址等）。

## 4.2. 注销
AliOS Things提供如下函数用于注销TTY设备：
```c
aos_status_t aos_tty_unregister(uint32_t id);
```
调用此函数之后，BSP开发者可回收相关联的`aos_tty_t`类型或其派生类型的变量。

# 5. 回调函数
驱动程序应实现`aos_tty_ops_t`定义的一组面向硬件的回调函数。

## 5.1. unregister
```c
void (*unregister)(aos_tty_t *tty);
```
`unregister`回调函数在设备注销时被调用，可在该函数中执行具体硬件相关的注销时反初始化工作（例如解除寄存器地址映射等）。

## 5.2. startup
```c
aos_status_t (*startup)(aos_tty_t *tty);
```
`startup`回调函数在设备引用计数从0增加到1时被调用，可在该函数中执行具体硬件相关的运行时初始化工作。初始化成功时应返回0；失败时应返回errno（负值）。该函数不应使能硬件发送和接收功能。

`startup`回调函数可读取`tty->termios.c_cflag`变量获取设备初始属性并修改硬件状态；也可修改该变量使其反映硬件的实际状态。该变量是`tcflag_t`类型，可包含如下字段，各字段使用按位或运算连接：
* 波特率，掩码为`CBAUD`，取值必须为以下当中的一个：
    + `B50`
    + `B75`
    + `B110`
    + `B134`
    + `B150`
    + `B200`
    + `B300`
    + `B600`
    + `B1200`
    + `B1800`
    + `B2400`
    + `B4800`
    + `B9600`
    + `B19200`
    + `B38400`
    + `B57600`
    + `B115200`
    + `B230400`
    + `B460800`
    + `B500000`
    + `B576000`
    + `B921600`
    + `B1000000`
    + `B1152000`
    + `B1500000`
    + `B2000000`
    + `B2500000`
    + `B3000000`
    + `B3500000`
    + `B4000000`
* 字节长度，掩码为`CSIZE`，取值只能为以下当中的一个：
    + `CS5`
    + `CS6`
    + `CS7`
    + `CS8`
* `CSTOPB`：该标志有效时表示停止位为2位，否则为1位。
* `PARENB`：表示使能校验位。
* `PARODD`：`PARENB`有效时，若`PARODD`有效则校验方式为奇校验，否则为偶校验。

## 5.3. shutdown
```c
void (*shutdown)(aos_tty_t *tty);
```
`shutdown`回调函数在设备引用计数从1减小到0时被调用，可在该函数中执行具体硬件相关的运行时反初始化工作。该函数被调用时，硬件发送和接收功能已被禁用。

## 5.4. set_attr
```c
aos_status_t (*set_attr)(aos_tty_t *tty);
```
`set_attr`回调函数在修改设备属性时被调用，驱动程序应在该函数中根据新属性（存放在`tty->termios.c_cflag`变量中）修改硬件状态。修改成功后返回0；失败时应返回errno（负值），且驱动程序应将硬件状态恢复到此函数被调用之前的状态。该函数被调用时，硬件发送和接收功能已被禁用。

## 5.5. enable_rx
```c
void (*enable_rx)(aos_tty_t *tty);
```
`enable_rx`回调函数在使能接收功能时被调用，驱动程序应在该函数中使能硬件的接收功能。该函数无需等待有数据被接收到再返回，而是应该使能接收中断（本设备级别而非中断控制器级别）并立即返回，在硬件中断处理程序中处理后续接收工作。
```c
size_t aos_tty_rx_buffer_produce(aos_tty_t *tty, const void *buf, size_t count);
```
在中断处理程序中使用`aos_tty_rx_buffer_produce`将硬件接收到的数据送入设备软件核心层。参数`count`为硬件接收到的字节数目。驱动程序应在调用该函数之前从FIFO或DMA获取数据并存放到`buf`指向的空间。返回值为实际送入设备软件核心层的字节数目，在设备软件接收缓冲区已满的情况下返回值会小于`count`。`aos_tty_rx_buffer_produce`应在关闭本地CPU中断且`tty->lock`加锁的环境下调用。例如：
```c
void rx_irq_handler(tty_abc_t *tty_abc)
{
    aos_tty_t *tty = &tty_abc->tty;
    uint8_t fifo_data[RX_FIFO_SIZE];
    size_t rx_count;
    aos_irqsave_t flags;

    flags = aos_spin_lock_irqsave(&tty->lock);
    if (!is_rx_irq_en(tty_abc->reg_base) || !is_rx_ready(tty_abc->reg_base)) {
        aos_spin_unlock_irqrestore(&tty->lock, flags);
        return;
    }
    rx_count = get_rx_fifo_data(tty_abc->reg_base, fifo_data);
    (void)aos_tty_rx_buffer_produce(tty, fifo_data, rx_count);
    aos_spin_unlock_irqrestore(&tty->lock, flags);
}
```

## 5.6. disable_rx
```c
void (*disable_rx)(aos_tty_t *tty);
```
`enable_rx`回调函数在禁用接收功能时被调用，驱动程序应在该函数中禁用硬件的接收功能。此时FIFO中已接收的数据或DMA正在接收的数据可直接丢弃。

## 5.7. start_tx
```c
void (*start_tx)(aos_tty_t *tty);
```
`start_tx`回调函数在设备软件发送缓冲区由空变为非空时被调用，驱动程序应在该函数中将待发送数据送入FIFO或DMA。该函数无需等待数据全部被硬件发出再返回，而是应该使能发送中断（本设备级别而非中断控制器级别）并立即返回，在硬件中断处理程序中处理后续发送工作。注意`start_tx`回调函数在关闭本地CPU中断且`tty->lock`加锁的环境下被调用，请不要在该函数中进行任务调度或执行长耗时的工作。
```c
size_t aos_tty_tx_buffer_consume(aos_tty_t *tty, void *buf, size_t count);
```
在`start_tx`回调函数或中断处理程序中使用`aos_tty_tx_buffer_consume`从设备软件核心层获取待发送数据。参数`count`为硬件发送最大字节数，例如FIFO深度或DMA最大长度。待发送数据将被复制到`buf`指向的空间，驱动程序应将这些数据交给FIFO或DMA。返回值为实际从设备软件核心层获取的字节数目；返回0时说明设备软件发送缓冲区为空，驱动程序应禁用发送中断（本设备级别而非中断控制器级别）。`aos_tty_tx_buffer_consume`应在关闭本地CPU中断且`tty->lock`加锁的环境下调用。例如：
```c
void tx_irq_handler(tty_abc_t *tty_abc)
{
    aos_tty_t *tty = &tty_abc->tty;
    uint8_t fifo_data[TX_FIFO_SIZE];
    size_t tx_count;
    aos_irqsave_t flags;

    flags = aos_spin_lock_irqsave(&tty->lock);
    if (!is_tx_irq_en(tty_abc->reg_base) || !is_tx_empty(tty_abc->reg_base)) {
        aos_spin_unlock_irqrestore(&tty->lock, flags);
        return;
    }
    tx_count = aos_tty_tx_buffer_consume(tty, fifo_data, TX_FIFO_SIZE);
    if (tx_count > 0)
        set_tx_fifo_data(tty_abc->reg_base, fifo_data, tx_count);
    else
        disable_tx_irq(tty_abc->reg_base);
    aos_spin_unlock_irqrestore(&tty->lock, flags);
}
```

## 5.8. stop_tx
```c
void (*stop_tx)(aos_tty_t *tty);
```
`disable_tx`回调函数在中止发送时被调用，驱动程序应在该函数中中止硬件的发送流程。若此时FIFO中有待发送的数据或DMA正在发送数据，应先等待这些数据发送完成。

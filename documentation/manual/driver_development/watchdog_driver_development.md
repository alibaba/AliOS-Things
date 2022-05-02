@page watchdog_driver_development Watchdog设备驱动程序开发指南

# 1. 前言
本文介绍如何编写符合AliOS Things标准的watchdog设备驱动程序。

# 2. 头文件
在程序中使用本文提及的功能应包含头文件 *aos/watchdog_core.h* 。

# 3. 数据结构
AliOS Things提供watchdog设备的抽象基础结构：
```c
typedef struct aos_watchdog aos_watchdog_t;
```
采用结构体嵌套的方式从基础结构派生出具体的硬件结构。派生类型应包含具体硬件操作所需的各种变量。例如：
```c
typedef struct {
    aos_watchdog_t watchdog;
    /* private data */
    void *reg_base;
} watchdog_abc_t;
```
使用宏`aos_container_of`实现从基础结构指针到派生结构指针的转换。例如：
```c
aos_watchdog_t *watchdog_dev = foo();
watchdog_abc_t *watchdog_abc = aos_container_of(watchdog_dev, watchdog_abc_t, watchdog);
```

# 4. 注册及注销
## 4.1. 注册
AliOS Things提供如下函数用于注册watchdog设备：
```c
aos_status_t aos_watchdog_register(aos_watchdog_t *watchdog);
```
调用注册函数之前，BSP开发者应自行分配一个`aos_watchdog_t`类型或其派生类型的变量并对包含的如下变量赋值：
* `dev.id`：`uint32_t`类型，表示该设备的ID。不同watchdog设备不能拥有相同的ID。
* `ops`：`const aos_watchdog_ops_t *`类型，指向一组面向硬件的回调函数：
```c
typedef struct {
    void (*unregister)(aos_watchdog_t *watchdog);
    aos_status_t (*set_timeout)(aos_watchdog_t *watchdog);
    uint32_t (*get_timeleft)(aos_watchdog_t *watchdog);
    void (*kick)(aos_watchdog_t *watchdog);
} aos_watchdog_ops_t;
```
* `flags`：`uint32_t`类型，可包含如下字段，各字段使用按位或运算连接：
    + `AOS_WATCHDOG_F_DISABLE_ON_PUT`：表示不被引用时禁用该watchdog。

调用注册函数之前，BSP开发者应初始化派生类型中的私有变量，并执行具体硬件相关的初始化工作（例如映射寄存器地址等）。

## 4.2. 注销
AliOS Things提供如下函数用于注销watchdog设备：
```c
aos_status_t aos_watchdog_unregister(uint32_t id);
```
调用此函数之后，BSP开发者可回收相关联的`aos_watchdog_t`类型或其派生类型的变量。

# 5. 回调函数
驱动程序应实现`aos_watchdog_ops_t`定义的一组面向硬件的回调函数。

## 5.1. unregister
```c
void (*unregister)(aos_watchdog_t *watchdog);
```
`unregister`回调函数在设备注销时被调用，可在该函数中执行具体硬件相关的反初始化工作（例如解除寄存器地址映射等）。

## 5.2. set_timeout
```c
aos_status_t (*set_timeout)(aos_watchdog_t *watchdog);
```
`set_timeout`回调函数在修改watchdog超时时间时被调用，驱动程序应在该函数中根据新的超时时间修改硬件状态。成功时应刷新watchdog剩余时间并返回0；失败时应将硬件状态恢复到此函数被调用之前的状态并返回errno（负值）。

`set_timeout`回调函数可访问`watchdog->timeout`变量获取新的超时时间。该变量是`uint32_t`类型，单位为毫秒，0值表示禁用该watchdog。

## 5.3. get_timeleft
```c
uint32_t (*get_timeleft)(aos_watchdog_t *watchdog);
```
`get_timeleft`回调函数在获取距离watchdog超时的剩余时间时被调用，驱动程序应在该函数中从硬件中获取剩余时间。返回值表示剩余时间，单位为毫秒。该函数指针为`NULL`时不支持获取剩余时间。

## 5.4. kick
```c
void (*kick)(aos_watchdog_t *watchdog);
```
`kick`回调函数在刷新watchdog剩余时间（kick/feed）时被调用，驱动程序应在该函数中执行硬件的刷新操作。

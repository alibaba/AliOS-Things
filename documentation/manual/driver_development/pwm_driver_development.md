@page pwm_driver_development   PWM设备驱动程序开发指南

**[更正文档](https://gitee.com/alios-things/documentation/edit/rel_3.3.0/manual/driver_development/pwm_driver_development.md)** &emsp;&emsp;&emsp;&emsp; **[贡献说明](https://help.aliyun.com/document_detail/302301.html)**

# 1. 前言
本文介绍如何编写符合AliOS Things标准的PWM设备驱动程序。

# 2. 头文件
在程序中使用本文提及的功能应包含头文件 *aos/pwm_core.h* 。

# 3. 数据结构
AliOS Things提供PWM设备的抽象基础结构：
采用结构体嵌套的方式从基础结构派生出具体的硬件结构。派生类型应包含具体硬件操作所需的各种变量。例如：
```c
struct aos_pwm_ops;

typedef struct aos_pwm {
    aos_dev_t dev;

    const struct aos_pwm_ops *ops; /**< PWM设备的底层操作, 设备注册前必须赋值 */
    uint32_t period; /**< PWM设备周期(单位:ns) */
    uint32_t duty_cycle; /**< PWM设备占空比(单位:ns) */
    uint32_t polarity; /**< PWM极性（0或者1）*/
    bool enabled;
    void *priv; /**< 私有数据，可选 */
} aos_pwm_t;
```

# 4. 注册及注销
## 4.1. 注册
AliOS Things提供如下函数用于注册PWM设备：
```c
aos_status_t aos_pwm_register(aos_pwm_t *pwm);
```
调用注册函数之前，BSP开发者应自行分配一个`aos_pwm_t`类型或其派生类型的变量并对包含的如下变量赋值：
* `dev.id`：`uint32_t`类型，表示该设备的ID。不同PWM设备不能拥有相同的ID，必选。
* `ops`：`const struct aos_pwm_ops *ops`类型，指向一组面向硬件的回调函数，必选。
```c
typedef struct aos_pwm_ops {
    /**< PWM设备的注销操作（可选） */
    void (*unregister)(aos_pwm_t *pwm);
    /**< PWM设备的前期准备 */
    aos_status_t (*startup)(aos_pwm_t *pwm);
    /**< PWM设备的停止波形输出 */
    void (*shutdown)(aos_pwm_t *pwm);
    /**< 设置PWM设备的频率 */
    aos_status_t (*apply)(aos_pwm_t *pwm, aos_pwm_attr_t const *attr);
} aos_pwm_ops_t;
```
```c
typedef enum {
    AOS_PWM_POLARITY_NORMAL,
    AOS_PWM_POLARITY_INVERSE,
} aos_pwm_polarity_t;

typedef struct {
    uint32_t period; /*ns*/
    uint32_t duty_cycle; /*ns*/
    aos_pwm_polarity_t polarity;
    bool enabled;
} aos_pwm_attr_t;
```
* aos_pwm_attr_t 在aos/pwm.h头文件中定义，aos/pwm.h头文件已经在aos/pwm_core.h中引用。

调用注册函数之前，BSP开发者应初始化派生类型中的私有变量，并执行具体硬件相关的注册时初始化工作（例如映射寄存器地址等）。

## 4.2. 注销
AliOS Things提供如下函数用于注销PWM设备：
```c
aos_status_t aos_pwm_unregister(uint32_t id);
```
调用此函数之后，BSP开发者可回收相关联的`aos_pwm_t`类型或其派生类型的变量。

# 5. 工作模式
PWM设备的工作模式，有数据类型`aos_pwm_attr_t`定义，目前支持占空比、周期、极性和使能设置。

# 6. 回调函数
驱动程序应实现`aos_pwm_ops_t`定义的一组面向硬件的回调函数。

## 6.1. unregister
```c
void (*unregister)(aos_pwm_t *pwm);
```
`unregister`回调函数在设备注销时被调用，可在该函数中执行具体硬件相关的注销时反初始化工作（例如解除寄存器地址映射等）。

## 6.2. startup
```c
aos_status_t (*startup)(aos_pwm_t *pwm);
```
`startup`回调函数在设备引用计数从0增加到1时(如应用通过get操作获取一个设备的引用)被调用，可在该函数中执行具体硬件相关的运行时初始化工作。初始化成功时应返回0；失败时应返回errno（负值）。

## 6.3. shutdown
```c
void (*shutdown)(aos_pwm_t *pwm);
```
`shutdown`回调函数在设备引用计数从1减小到0时(如应用通过put操作释放一个设备)被调用，可在该函数中执行具体硬件相关的运行时反初始化工作。

## 6.4. apply
```c
aos_status_t (*apply)(aos_pwm_t *pwm, aos_pwm_attr_t const *attr);
```
`apply`回调函数用来设置pwm的占空比、周期、极性和使能。


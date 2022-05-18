@page adc_driver_development   ADC设备驱动程序开发指南

**[更正文档](https://gitee.com/alios-things/documentation/edit/rel_3.3.0/manual/driver_development/adc_driver_development.md)** &emsp;&emsp;&emsp;&emsp; **[贡献说明](https://help.aliyun.com/document_detail/302301.html)**

# 1. 前言
本文介绍如何编写符合AliOS Things标准的ADC设备驱动程序。

# 2. 头文件
在程序中使用本文提及的功能应包含头文件 *aos/adc_core.h* 。

# 3. 数据结构
AliOS Things提供ADC设备的抽象基础结构：
采用结构体嵌套的方式从基础结构派生出具体的硬件结构。派生类型应包含具体硬件操作所需的各种变量。例如：
```c
struct aos_adc_ops;

typedef struct aos_adc {
    aos_dev_t dev;

    const struct aos_adc_ops *ops; /**< ADC设备的底层操作, 设备注册前必须赋值 */
    uint32_t resolution; /**< ADC设备分辨率(单位:bits), 设备注册前必须赋值 */
    uint32_t freq; /**< ADC设备时钟频率(单位:HZ), 可选 */
    void *priv; /**< 私有数据，可选 */
} aos_adc_t;
```

# 4. 注册及注销
## 4.1. 注册
AliOS Things提供如下函数用于注册ADC设备：
```c
aos_status_t aos_adc_register(aos_adc_t *adc);
```
调用注册函数之前，BSP开发者应自行分配一个`aos_adc_t`类型或其派生类型的变量并对包含的如下变量赋值：
* `dev.id`：`uint32_t`类型，表示该设备的ID。不同ADC设备不能拥有相同的ID，必选。
* `ops`：`const struct aos_adc_ops *ops`类型，指向一组面向硬件的回调函数，必选。
```c
typedef struct aos_adc_ops {
    /**< ADC设备的注销操作（可选） */
    void (*unregister)(aos_adc_t *adc);
    /**< ADC设备的开始采样 */
    aos_status_t (*startup)(aos_adc_t *adc);
    /**< ADC设备的停止采样 */
    void (*shutdown)(aos_adc_t *adc);
    /**< 读取ADC设备的原始数据 */
    aos_status_t (*read)(aos_adc_t *adc, int32_t channel, int32_t *data);
    /**< 读取ADC设备的转换后的电压值（单位: mV）*/
    aos_status_t (*read_voltage)(aos_adc_t *adc, int32_t channel, int32_t *data);
    /**< 设置ADC设备的通道（开启与关闭）（可选）*/
    aos_status_t (*set_channel)(aos_adc_t *adc, int32_t channel, bool enable);
    /**< 设置ADC设备的采样时间（单位: 时钟周期的倍数）（可选）*/
    aos_status_t (*set_sample_time)(aos_adc_t *adc, int32_t channel, uint32_t clock);
    /**< 设置ADC设备的工作模式（可选）*/
    aos_status_t (*set_mode)(aos_adc_t *adc, aos_adc_mode_t clock);
    /**< 获取ADC设备的电压量程 */
    aos_status_t (*get_range)(aos_adc_t *adc, int32_t channel, int32_t *range);
} aos_adc_ops_t;
```
* `resolution`：`uint32_t`类型，可包含如下字段，ADC设备分辨率(单位:bits)，如12，必选。
* `freq`：`uint32_t`类型，ADC设备时钟频率(单位:HZ), 可选。

调用注册函数之前，BSP开发者应初始化派生类型中的私有变量，并执行具体硬件相关的注册时初始化工作（例如映射寄存器地址等）。

## 4.2. 注销
AliOS Things提供如下函数用于注销ADC设备：
```c
aos_status_t aos_adc_unregister(uint32_t id);
```
调用此函数之后，BSP开发者可回收相关联的`aos_adc_t`类型或其派生类型的变量。

# 5. 工作模式
ADC设备的工作模式，有数据类型`aos_adc_mode_t`定义，目前支持单次采样模式`AOS_ADC_MODE_SINGLE` 和连续采样模式`AOS_ADC_MODE_CONTINUE`的设置。

# 6. 回调函数
驱动程序应实现`aos_adc_ops_t`定义的一组面向硬件的回调函数。

## 6.1. unregister
```c
void (*unregister)(aos_adc_t *adc);
```
`unregister`回调函数在设备注销时被调用，可在该函数中执行具体硬件相关的注销时反初始化工作（例如解除寄存器地址映射等）。

## 6.2. startup
```c
aos_status_t (*startup)(aos_adc_t *adc);
```
`startup`回调函数在设备引用计数从0增加到1时(如应用通过get操作获取一个设备的引用)被调用，可在该函数中执行具体硬件相关的运行时初始化工作。初始化成功时应返回0；失败时应返回errno（负值）。

## 6.3. shutdown
```c
void (*shutdown)(aos_adc_t *adc);
```
`shutdown`回调函数在设备引用计数从1减小到0时(如应用通过put操作释放一个设备)被调用，可在该函数中执行具体硬件相关的运行时反初始化工作。

## 6.4. read
```c
aos_status_t (*read)(aos_adc_t *adc, int32_t channel, int32_t *data);
```
`read`回调函数在同步读取ADC设备原始数据时被调用，从ADC设备`adc`的通道`channel`读取原始数据，结果存放到到`data`中。

## 6.5. read_voltage
```c
aos_status_t (*read_voltage)(aos_adc_t *adc, int32_t channel, int32_t *data);
```
`read_voltage`回调函数在读取ADC设备转换后的电压值（单位: mV）时被调用，从ADC设备`adc`的通道`channel`读取转化后的电压值。结果存放到`data`处。

## 6.6. set_channel
（可选）
```c
aos_status_t (*set_channel)(aos_adc_t *adc, int32_t channel, bool enable);
```
`set_channel`函数设置ADC设备的通道，设置设备`adc`的通道`channel`的开启与关闭。

## 6.7. set_sample_time
（可选）
```c
aos_status_t (*set_sample_time)(aos_adc_t *adc, int32_t channel, uint32_t clock);
```
`set_sample_time`函数在设置ADC设备`adc`的通道`channel`的采样时间时被调用，`clock`为时钟周期的倍数。

## 6.8. set_mode
（可选）
```c
aos_status_t (*set_mode)(aos_adc_t *adc, aos_adc_mode_t clock);
```
`set_mode`函数设置ADC设备的工作模式，支持的工作模式由`aos_adc_mode_t`定义，目前支持单次采样模式`AOS_ADC_MODE_SINGLE` 和连续采样模式`AOS_ADC_MODE_CONTINUE`的设置。

## 6.9. get_range
```c
aos_status_t (*get_range)(aos_adc_t *adc, int32_t channel, int32_t *range);
```
`get_range`函数在获取ADC设备电压量程时被调用， 获取ADC设备`adc`的通道`channel`的电压量程，结果存放到`range`中。

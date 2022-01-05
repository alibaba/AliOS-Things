@page gpioc_driver_development GPIO控制器设备驱动程序开发指南

**[更正文档](https://gitee.com/alios-things/documentation/edit/rel_3.3.0/manual/driver_development/gpioc_driver_development.md)** &emsp;&emsp;&emsp;&emsp; **[贡献说明](https://help.aliyun.com/document_detail/302301.html)**

# 1. 前言
本文介绍如何编写符合AliOS Things标准的GPIO控制器设备驱动程序。

# 2. 头文件
在程序中使用本文提及的功能应包含头文件 *aos/gpioc_core.h* 。

# 3. 数据结构
AliOS Things提供GPIO控制器设备的抽象基础结构：
```c
typedef struct aos_gpioc aos_gpioc_t;
```
采用结构体嵌套的方式从基础结构派生出具体的硬件结构。派生类型应包含具体硬件操作所需的各种变量。例如：
```c
#define GPIOC_ABC_NUM_PINS 32

typedef struct {
    aos_gpioc_t gpioc;
    /* private data */
    aos_gpioc_pin_t pins[GPIOC_ABC_NUM_PINS];
    void *reg_base;
    int irq_num;
} gpioc_abc_t;
```
使用宏`aos_container_of`实现从基础结构指针到派生结构指针的转换。例如：
```c
aos_gpioc_t *gpioc_dev = foo();
gpioc_abc_t *gpioc_abc = aos_container_of(gpioc_dev, gpioc_abc_t, gpioc);
```

# 4. 注册及注销
## 4.1. 注册
AliOS Things提供如下函数用于注册GPIO控制器设备：
```c
aos_status_t aos_gpioc_register(aos_gpioc_t *gpioc);
```
调用注册函数之前，BSP开发者应自行分配一个`aos_gpioc_t`类型或其派生类型的变量并对包含的如下变量赋值：
* `dev.id`：`uint32_t`类型，表示该设备的ID。不同GPIO控制器设备不能拥有相同的ID。
* `ops`：`const aos_gpioc_ops_t *`类型，指向一组面向硬件的回调函数：
```c
typedef struct {
    void (*unregister)(aos_gpioc_t *gpioc);
    aos_status_t (*set_mode)(aos_gpioc_t *gpioc, uint32_t pin);
    void (*enable_irq)(aos_gpioc_t *gpioc, uint32_t pin);
    void (*disable_irq)(aos_gpioc_t *gpioc, uint32_t pin);
    int (*get_value)(aos_gpioc_t *gpioc, uint32_t pin);
    void (*set_value)(aos_gpioc_t *gpioc, uint32_t pin);
} aos_gpioc_ops_t;
```
* `num_pins`：`uint32_t`类型，表示该设备包含的GPIO pin数目。
* `pins`：`aos_gpioc_pin_t *`类型，指向元素数目为`num_pins`的数组。

调用注册函数之前，BSP开发者应初始化派生类型中的私有变量，并执行具体硬件相关的初始化工作（例如映射寄存器地址等）。

## 4.2. 注销
AliOS Things提供如下函数用于注销GPIO控制器设备：
```c
aos_status_t aos_gpioc_unregister(uint32_t id);
```
调用此函数之后，BSP开发者可回收相关联的`aos_gpioc_t`类型或其派生类型的变量。

# 5. 回调函数
驱动程序应实现`aos_gpioc_ops_t`定义的一组面向硬件的回调函数。

## 5.1. unregister
```c
void (*unregister)(aos_gpioc_t *gpioc);
```
`unregister`回调函数在设备注销时被调用，可在该函数中执行具体硬件相关的反初始化工作（例如解除寄存器地址映射等）。

## 5.2. set_mode
```c
aos_status_t (*set_mode)(aos_gpioc_t *gpioc, uint32_t pin);
```
`set_mode`回调函数在修改GPIO控制器设备上某个pin的模式时被调用，驱动程序应在该函数中根据新模式修改硬件状态。修改成功后返回0；失败时应返回errno（负值），且驱动程序应将硬件状态恢复到此函数被调用之前的状态。

`set_mode`回调函数可访问`gpioc->pins[pin].mode`变量获取被操作的pin的新模式。该变量是`uint32_t`类型，可包含如下字段，各字段使用按位或运算连接：
* 方向，掩码为`AOS_GPIO_DIR_MASK`，取值必须为以下当中的一个：
    + `AOS_GPIO_DIR_NONE`：表示该pin不作为GPIO使用，可能让出pin mux，具体由驱动程序实现决定。
    + `AOS_GPIO_DIR_INPUT`：表示该pin作为输入。
    + `AOS_GPIO_DIR_OUTPUT`：表示该pin作为输出。
* 输入配置，只在该pin作为输入时有意义，掩码为`AOS_GPIO_INPUT_CFG_MASK`，取值必须为以下当中的一个：
    + `AOS_GPIO_INPUT_CFG_DEFAULT`：表示输入配置为默认模式，具体由驱动程序实现，一般为最常用模式或硬件支持的唯一模式。
    + `AOS_GPIO_INPUT_CFG_HI`：表示输入配置为浮空模式。
    + `AOS_GPIO_INPUT_CFG_PU`：表示输入配置为上拉模式。
    + `AOS_GPIO_INPUT_CFG_PD`：表示输入配置为下拉模式。
* 中断触发模式。注意`set_mode`回调函数只能设置硬件中断模式但不能使能中断。只在该pin作为输入时有意义，掩码为`AOS_GPIO_IRQ_TRIG_MASK`，取值必须为以下当中的一个：
    + `AOS_GPIO_IRQ_TRIG_NONE`：表示不触发中断。
    + `AOS_GPIO_IRQ_TRIG_EDGE_RISING`：表示上升沿触发中断。
    + `AOS_GPIO_IRQ_TRIG_EDGE_FALLING`：表示下降沿触发中断。
    + `AOS_GPIO_IRQ_TRIG_EDGE_BOTH`：表示上升沿和下降沿都触发中断。
    + `AOS_GPIO_IRQ_TRIG_LEVEL_LOW`：表示低电平触发中断。
    + `AOS_GPIO_IRQ_TRIG_LEVEL_HIGH`：表示高电平触发中断。
* 输出配置，只在该pin作为输出时有意义，掩码为`AOS_GPIO_OUTPUT_CFG_MASK`，取值必须为以下当中的一个：
    + `AOS_GPIO_OUTPUT_CFG_DEFAULT`：表示输出配置为默认模式，具体由驱动程序实现，一般为最常用模式或硬件支持的唯一模式。
    + `AOS_GPIO_OUTPUT_CFG_PP`：表示输出配置为推挽模式。
    + `AOS_GPIO_OUTPUT_CFG_ODNP`：表示输出配置为开漏无上拉模式。
    + `AOS_GPIO_OUTPUT_CFG_ODPU`：表示输出配置为开漏上拉模式。
* 输出初始电平，只在该pin作为输出时有意义，掩码为`AOS_GPIO_OUTPUT_INIT_MASK`，取值必须为以下当中的一个：
    + `AOS_GPIO_OUTPUT_INIT_LOW`：表示输出低电平。
    + `AOS_GPIO_OUTPUT_INIT_HIGH`：表示输出高电平。

## 5.3. enable_irq
```c
void (*enable_irq)(aos_gpioc_t *gpioc, uint32_t pin);
```
`enable_irq`回调函数在使能某个pin的中断时被调用，驱动程序应在该函数中将硬件配置为使能该pin的中断。这里的使能中断是指GPIO控制器级别而非中断控制器级别。

## 5.4. disable_irq
```c
void (*disable_irq)(aos_gpioc_t *gpioc, uint32_t pin);
```
`disable_irq`回调函数在禁用某个pin的中断时被调用，驱动程序应在该函数中将硬件配置为禁用该pin的中断。这里的禁用中断是指GPIO控制器级别而非中断控制器级别。

## 5.5 get_value
```c
int (*get_value)(aos_gpioc_t *gpioc, uint32_t pin);
```
`get_value`回调函数在获取某个pin的输入电平时被调用，驱动程序应在该函数中从硬件获取输入电平。输入电平为低电平时返回0，为高电平时返回1。

## 5.5 set_value
```c
void (*set_value)(aos_gpioc_t *gpioc, uint32_t pin);
```
`set_value`回调函数在设置某个pin的输出电平时被调用，驱动函数应在该函数中向硬件配置输出电平。`gpioc->pins[pin].value`变量表示被输出的电平，0表示低电平，1表示高电平。

# 6. 中断处理
在GPIO控制器硬件中断处理程序中调用宏`aos_gpioc_hard_irq_handler`来触发中断处理流程。
```
aos_gpioc_hard_irq_handler(gpioc, pin, polarity)
```
`polarity`是获取双沿触发中断极性的表达式，若本次是上升沿触发则表达式的值应为`true`，否则为`false`。`polarity`表达式只有在中断模式为双沿触发时才会被求值。

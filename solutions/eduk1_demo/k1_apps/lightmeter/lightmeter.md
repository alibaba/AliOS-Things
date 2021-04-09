@page lightmeter 光照信息屏
# 实验整体介绍
## 背景
光照传感器用于检测光照强度，其工作原理是将检测光照强度值并将其转化为电压值，目前在智能家居，智慧城市，智慧农业，智慧工厂等物联网领域被大量使用。
本实验的主要结果是通过I2C总线控制光强度传感器AP3216C对环境光强度及物体接近情况进行测量并获取环境光强度及是否有物体靠近，然后通过OLED显示屏将读取到的光强度、接近程度、红外信息显示在HaaS EDU K1的屏幕上。
HaaS EDU K1搭载了丰富实用的传感器，开发者可以使用这些传感器针对实际场景需求开发自己的应用。

## 涉及的知识点
* AP3216C光照与接近传感器原理
* OLED绘图


# 开发环境准备
## 硬件
    开发用电脑一台
    HAAS EDU K1 开发板一块
    USB2TypeC 数据线一根

## 软件
### AliOS Things开发环境搭建
    开发环境的搭建请参考 @ref HaaS_EDU_K1_Quick_Start (搭建开发环境章节)，其中详细的介绍了AliOS Things 3.3的IDE集成开发环境的搭建流程。

### HaaS EDU K1 DEMO 代码下载
    HaaS EDU K1 DEMO 的代码下载请参考 @ref HaaS_EDU_K1_Quick_Start (创建工程章节)，其中，
    选择解决方案: 基于教育开发板的示例
    选择开发板: haaseduk1 board configure

### 代码编译、烧录
    参考 @ref HaaS_EDU_K1_Quick_Start (3.1 编译工程章节)，点击 ✅ 即可完成编译固件。
    参考 @ref HaaS_EDU_K1_Quick_Start (3.2 烧录镜像章节)，点击 "⚡️" 即可完成烧录固件。



## 上手把玩
实验运行结果如下：
1. 室内自然灯光下：
   

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01DYmUrt1LmDnLgN4a4_!!6000000001341-2-tps-1200-800.png" style="zoom:50%;" />
</div>
 

2. 用手机闪光灯照射屏幕上方的透明孔位
   

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01ZoZ2AI1RA6WahQENe_!!6000000002070-2-tps-1200-800.png" style="zoom:50%;" />
</div>
 

3. 屏幕前没有遮挡情况下
   

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01qmbhY627OUpqxiQLw_!!6000000007787-2-tps-1200-800.png" style="zoom:50%;" />
</div>
 

4. 拿物体靠近屏幕上方的透明孔位
   

<div align=center>
    <img src="://img.alicdn.com/imgextra/i4/O1CN01vKgrup1kunbe8wGgJ_!!6000000004744-2-tps-1200-800.png" style="zoom:50%;" />
</div>
 

## 实际应用场景（产品）介绍
目前光强度传感器及接近传感器在以下日常生活、工业生产等环境中都有非常广泛的应用。
* 家庭智能灯系统
	*  检测到光强度低于一定亮度之后，触发信号控制客厅自动开灯
	*  检测到光强度低于一定亮度之后，有人体靠近则自动打开夜灯/灯带
* 智能路灯
	*  智慧路灯安装光强度传感器后可以自动的感知环境强度，从而做到智能开关，保证交通安全的同时节省了系统能源
* 智能手机/平板/电视等
	*  智能手机、平板、电视等消费类电子需要感知环境光温度并根据环境光温度自动调节屏幕亮度，给使用者最佳的视觉体验的同时也降低了系统的功耗
	*  智能手机上都存在的距离传感器还可以在通话过程中检测到听筒靠近头部的时候自动熄灭屏幕，防止误触碰
	*  智能手机还会用距离传感器与其它感应器实现反转手机静音及解锁/锁屏等操作

这些是我们日常生活中最常见的几个应用场景。
光强度传感器是一种光电传感器，光电传感器拥有更广泛的应用范围，比如烟雾报警系统、工厂烟尘检测系统、条形码扫描仪、产品计数器、转速检测系统等等。

# 硬件介绍
## 电路原理图
AP3216芯片位于屏幕上方，是通过OLED扩展接口和HaaS EDU K1主板连接，最终连接到HaaS1000的I2C1通道。


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN013xnlcx1e1wTNimju3_!!6000000003812-2-tps-264-360.png" style="zoom:50%;" />
</div>
 

## AP3216C传感器
### AP3216C外观


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01w1xpIr1SPVSjxM5Ck_!!6000000002239-2-tps-514-468.png" style="zoom:50%;" />
</div>
 

### AP3216C芯片特性
* 7-bit地址模式，地址：0x1E
* 支持I2C接口Fast Mode (400kbps)
* 支持多种连续测量/单次测量及光强度、接近指标单独测试及组合测试
* 内置温度补偿电路
* 工作温度范围：-30°C到+80°C
* 光强度传感器
    16-bit有效线性输出
    4种动态可选范围
 * 接近传感器
     10-bit有效线性输出

### AP3216C内部框图
AP3216C主要包含如下几部分。
1. 光亮度敏感元件
2. 接近度敏感元件
3. ADC数模转换模块
4. 内部逻辑控制电路（包含条件触发中断功能）
5. I2C总线控制器
6. 红外LED发射二极管
   

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01NZKskY1MYlmDMNUVd_!!6000000001447-2-tps-1482-754.png" style="zoom:50%;" />
</div>
 

### AP3216C工作模式
根据AP3216C的datasheet说明，在正常工作时，它共有3种工作模式：
1. ALS模式
在这种模式下，AP3216C只对光强度进行量测
2. PS+IR模式
在这种模式下，AP3216C只对接近程度进行量测
3. ALS+PS+IR模式
在这种模式下，AP3216C会同时对光强度及接近程度进行量测

三种模式均支持单次量测也支持循环量测，其量测稳定时间如下：


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01cEXBbo1F4MASlldqn_!!6000000000433-2-tps-1536-306.png" style="zoom:50%;" />
</div>
 

### AP3216C中断模式
AP3216C提供中断检测功能，MCU可以设定ALS及PS中断触发门限及持续时间阈值。在环境亮度及接近程度到达门限值后持续时间超出持续时间阈值之后，会向INT管脚发送中断通知MCU进行处理。
本案例中未使用中断模式。

### AP3216C工作流程
AP3216C的典型工作流程如下：


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01uNp0mF1aUYl0COe4D_!!6000000003333-2-tps-726-576.png" style="zoom:50%;" />
</div>
 

对AP3226C进行复位之后，设定工作模式（连续/单次测量，ALS/PS/IR组合设定），之后等待硬件测量稳定时间，再发起读取测量值的过程。
其中，复位及模式设定是对AP3216C的System Configuration寄存器进行操作，datasheet中对System Configuration寄存器定义及描述如下：


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01EuhLeU1vxViwX7u1p_!!6000000006239-2-tps-1532-408.png" style="zoom:50%;" />
</div>
 

* 复位操作
参考datasheet的描述，是通过I2C 向地址为0x00的寄存器写入0x4 (对应二进制的100)。

* 模式设定
ALS only连续测量模式：是通过I2C 向地址为0x00的寄存器写入0x4 (对应二进制的100)。
PS/IR only连续测量模式：是通过I2C 向地址为0x00的寄存器写入0x1 (对应二进制的001)。
ALS+PS/IR连续测量模式：是通过I2C 向地址为0x00的寄存器写入0x2 (对应二进制的010)。
ALS only单次测量模式：是通过I2C 向地址为0x00的寄存器写入0x5 (对应二进制的101)。
PS/IR only单次测量模式：是通过I2C 向地址为0x00的寄存器写入0x6 (对应二进制的110)。
ALS+PS/IR单次测量模式：是通过I2C 向地址为0x00的寄存器写入0x7 (对应二进制的111)。

* 读取测量结果
ALS、PS、IR值可以通过如下寄存器的值来获取。寄存器地址及所读取数值详细解析方法请参考下表：


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01DSpQfM1To4JlpTvK3_!!6000000002428-2-tps-1522-834.png" style="zoom:50%;" />
</div>
 

### ALS光强度解析

通过读取ALS数据寄存器（0x0C和0x0D）得到16bit的ADC count数据之后，需要根据ALS Configuration寄存器的ALS Gain栏位将ADC count数据根据芯片手册的说明转换成以lux为单位的亮度值。如下图所示：


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01kRjroj24kX2lSRoU3_!!6000000007429-2-tps-1546-640.png" style="zoom:50%;" />
</div>
 

### PS/IR数据解析
PS Data寄存器详细定义如下:
- PS 有效位数为10bit。
- OBJ bit在有物体靠近的时候被设置成1，否则设置为0。
- IR_OF bit被设置成1代表PS值无效（高强度红外光的情况下会出现）
  

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN017IDLnb20pn7tOeDN1_!!6000000006899-2-tps-1522-834.png" style="zoom:50%;" />
</div>
 

IR数据有效位数同样为10bit，也有一个IR_OF来标识IR/PS数据是否有效。


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01LpyTnk1Kq3Ubov7q8_!!6000000001214-2-tps-1546-640.png" style="zoom:50%;" />
</div>
 

AP3216C判断物体靠近和远离的动作是通过两组PS 高低阈值寄存器和PS Data寄存器进行比对，PS Data高于PS High Threshold之后，则判定为物体远离；PS Data低于PS Low Threshold之后，则视为物体靠近。
- PS Low Threshold计算方法：Value(Reg_0x2B) * 4 + Value(Reg_0x2A)
- PS High Threshold计算方法：Value(Reg_0x2D) * 4 + Value(Reg_0x2C)
  

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01Xy2jsE1mgGGW09e5J_!!6000000004983-2-tps-1520-686.png" style="zoom:50%;" />
</div>
 

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01DHGoEZ1jt8YHQelbL_!!6000000004605-2-tps-1536-430.png" style="zoom:50%;" />
</div>
 

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01q0rVZC1rEqdf7JXre_!!6000000005600-2-tps-1516-180.png" style="zoom:50%;" />
</div>
 

### PS中断模式说明
在开启了AP3216C的中断模式，为了避免靠近物体抖动带来频繁触发中断的情况，在PS Data高于PS High Threashold或低于PS Low Threshold之后，需要等待一个PS persist时间，如果PS persist时间之内，PS Data状态没有发生变化，则触发中断。


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01llCSS51OmczflBnIS_!!6000000001748-2-tps-1452-492.png" style="zoom:50%;" />
</div>
 

PS Persist的时间可以通过配置PS Configuration寄存器的bit 0:1。


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01Xy2jsE1mgGGW09e5J_!!6000000004983-2-tps-1520-686.png" style="zoom:50%;" />
</div>
 


## I2C总线技术
HaaS EDU K1上AP3216C和HaaS1000通过I2C1接口进行通信。
I2C总线是飞利浦公司在80年代为了让CPU可以连接低速周边设备而设计的。外接传感器是I2C总线最典型的应用场景。目前I2C Spec已经发展到了6.0版本。可以通过https://www.nxp.com.cn/docs/en/user-guide/UM10204.pdf 进行下载
I2C采用多主从结构，I2C主设备基于地址对I2C从设备进行寻址，采用8-bit数据传输模式，支持7-bit/10-bit地址模式。I2C总线对I2C传输开始/结束/数据传输/ACK机制/时钟同步/冲突仲裁等进行了详细的定义。这里就不再赘述。

# 软件介绍
HaaS EDU K1的edk_demo 应用程序中包含多个应用案例，所有的案例软件设计都遵循HaaS EDK应用架构的设计思想。其设计思想可以参考“HaaS EDK主系统框架”中的“添加新应用”小节的说明。 
简单来说，所有的子应用页面都需要实现MENU_TYP结构体。
光照信息屏应用代码位于:solutions/eduk1_demo/k1_apps/lightmeter/lightmeter.c中。
```
MENU_TYP lightmeter = {
    "lightmeter",
    &lightmeter_cover,
    &lightmeter_tasks,
    NULL,
    &lightmeter_child_list};
```
在HaaS EDU K1上电之后，按K1或K2键左右切换应用的时候，如果切换到光照信息屏页面再到退出该页面的过程中，lightmeter下面的API会依此被呼叫：
1. lightmeter_init
2. lightmeter_uninit

## 软件流程
软件流程图如下所示。
在进入光照信息屏页面后，启动名为lightmeter_task的task，周期性的读取ALS/PS/IR数据并判断物体靠近标志之后将相关数据按照一定的规则打印到OLED屏幕上。


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01zu05lV1NtD4Gl3VLg_!!6000000001627-2-tps-992-1152.png" style="zoom:50%;" />
</div>
 

## 代码实现
### 光照信息屏页面
代码位置：solutions/eduk1_demo/k1_apps/lightmeter/lightmeter.c
详细代码及注释如下：
```
#include <stdio.h>
#include <stdlib.h>
#include "lightmeter.h"
#include "aos/kernel.h"
#include "ap3216c.h"

/* 按照主程序框架的规则，声明页面相关相关信息（封面、初始化、反初始化、子页面等）*/
MENU_COVER_TYP lightmeter_cover = {MENU_COVER_NONE};
MENU_TASK_TYP lightmeter_tasks = {
    lightmeter_init,
    lightmeter_uninit};
MENU_LIST_TYP lightmeter_child_list = {NULL, 0};
MENU_TYP lightmeter = {
    "lightmeter",
    &lightmeter_cover,
    &lightmeter_tasks,
    NULL,
    &lightmeter_child_list};

/* 页面初始化函数 */
int lightmeter_init(void)
{
    printf("lightmeter_init begin\n");
    /* AP3216C 初始化函数 */
    ap3216c_init();
    printf("lightmeter_init done\n");

    /* 清空OLED屏幕 */
    OLED_Clear();
    OLED_Refresh_GRAM();
    
    /* 启动光照信息屏主任务 */
    aos_task_new("lightmeter_task", lightmeter_task, NULL, 1000);
    printf("aos_task_new lightmeter_task \n");
    return 0;
}

void lightmeter_task(void)
{
    uint16_t tmp[3];
    uint8_t als[20];
    uint8_t ps[20];
    uint8_t ir[20];

    while (1)
    {
        /* 从AP3216C读取ALS、PS、IR数据 */
        tmp[0] = ap3216c_read_ambient_light();
        tmp[1] = ap3216c_read_ir_data();
        tmp[2] = ap3216c_read_ps_data();

        /* 打印ALS/IR信息字串 */
        sprintf(als, "ALS: %d", tmp[0]);
        sprintf(ir, "IR : %d", tmp[1]);

        /* 将lightmeter图标及ALS/IR信息输出到屏幕 */
        OLED_Clear();
        OLED_Icon_Draw(20, 14, &icon_lighter_32_32, 0);
        OLED_Show_String(64, 6, als, 12, 1);
        OLED_Show_String(64, 20, ir, 12, 1);

        /* 判断是否有物体靠近，并显示在屏幕上 */
        if ((tmp[2] >> 15) & 1)
            OLED_Show_String(64, 36, "near !", 16, 1);
        else
            OLED_Show_String(64, 40, "far !", 16, 1);

        /* 将左右按键标志输出到屏幕上 */
        OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
        OLED_Icon_Draw(122, 24, &icon_skip_right, 0);
        
        /* 刷新屏幕数据 */
        OLED_Refresh_GRAM();
        
        /* 休眠150ms */
        aos_msleep(150);
    }
}

/* 光照信息屏页面退出处理函数 */
int lightmeter_uninit(void)
{
    /* 删除光照信息屏主任务 */
    aos_task_delete("lightmeter_task");
    printf("aos_task_delete lightmeter_task \n");
    return 0;
}
```

### AP3216C驱动
代码位置：components/peripherals/sensor/drv/drv_als_ps_ir_liteon_ap3216c.c
以ap3216C初始化和读取ALS数值为例，详细代码及注释如下：
```
/**
 * This function initializes ap3216c registered device driver
 *
 * @param no
 *
 * @return the ap3216c device.
 */
void ap3216c_init(void)
{
    /**
    1. ap3216c连接到HaaS edu k1的I2C1端口, 所以i2c_dev的端口好需要设定为1
    2. ap3216c为7-bit地址模式
    3. haas1000芯片内部的I2C1作为主模式, ap3216c作为从设备
    4. ap3216c外设定制用AP3216C_ADDR声明
    */
#if 1
    i2c_dev.port                 = 1;
    i2c_dev.config.address_width = I2C_HAL_ADDRESS_WIDTH_7BIT;
    i2c_dev.config.freq          = I2C_BUS_BIT_RATES_100K;
    i2c_dev.config.mode          = I2C_MODE_MASTER;
    i2c_dev.config.dev_addr      = AP3216C_ADDR;
    /* 呼叫hal_i2c_init对haas1000内部的I2C1控制器进行初始化 */
    hal_i2c_init(&i2c_dev);
#endif
    /* 复位ap3216c芯片, 向system configuration寄存器写入0x0对其进行软件复位 */
    reset_sensor();
    /* 等待ap3216c复位稳定时间 */
    aos_msleep(100);
    /* 设置ap3216c工作在连续量测ALS/PS/IR模式 */
    ap3216c_set_param(AP3216C_SYSTEM_MODE, AP3216C_MODE_ALS_AND_PS);
    /* 休眠150ms等待量测到稳定值 */
    aos_msleep(150); // delay at least  150ms

    /* 配置中断脚 和 中断数据 - 函数内部实现置空	*/
    ap3216c_int_Config();
    ap3216c_int_init();
}
/**
 * This function reads light by ap3216c sensor measurement
 *
 * @param no
 *
 * @return the ambient light converted to float data.
 */
uint16_t ap3216c_read_ambient_light(void)
{
    uint16_t brightness = 0; // default error data
    uint16_t read_data;
    uint8_t range;

    /*分别读取 0x0c和0x0d的值，组成16-bit的ALS ADC count */
    read_data  = (uint16_t )read_low_and_high(AP3216C_ALS_DATA_L_REG, 1);
    /* 读取ALS range 配置 */
    ap3216c_get_param(AP3216C_ALS_RANGE, &range);
    /* 根据ALS range 配置，完成从ADC count到lux的换算 */
    if (range == AP3216C_ALS_RANGE_20661)
    {
        brightness = 0.36 * read_data; //sensor ambient light converse to reality
    }
    else if (range == AP3216C_ALS_RANGE_5162)
    {
        brightness = 0.089 * read_data; //sensor ambient light converse to reality
    }
    else if (range == AP3216C_ALS_RANGE_1291)
    {
        brightness = 0.022 * read_data; //sensor ambient light converse to reality
    }
    else if (range == AP3216C_ALS_RANGE_323)
    {
        brightness = 0.0056 * read_data; //sensor ambient light converse to reality
    }
    return brightness;
}

/**
 * This function is convenient to getting data except including high and low data for this sensor.
 * note:after reading lower register first,reading higher add one.
 */
 /* 读取从reg开始的两个字节， 并对数据进行合并*/
static uint32_t read_low_and_high(uint8_t reg, uint8_t len)
{
    uint32_t data;
    uint8_t buf = 0;

    read_regs( reg, len, &buf);        // 读低字节
    data = buf;
    read_regs( reg + 1, len, &buf);    // 读高字节
    data = data + (buf << len * 8);    // 合并数据

    return data;
}

/* 读寄存器的值 */
static void read_regs(uint8_t reg, uint8_t len, uint8_t *buf)
{  
    hal_i2c_mem_read(&i2c_dev, i2c_dev.config.dev_addr, reg, 1, buf, len, 100);
}

```
这里的hal_i2c_mem_read的实现和AP3216C datasheet上面读取寄存器的描述相匹配。
* MCU I2C 主设备先将寄存器地址通过I2C写操作发给AP3216C从设备
* MCUI2C 主设备发起读操作从AP3216C从设备读取寄存器的值
AP3216C寄存器读取的整个I2C传输过程中，详细数据传输过程如下图所示：


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN012WHnPA1kGUkd87seS_!!6000000004656-2-tps-1458-976.png" style="zoom:50%;" />
</div>
 

读取AP3216C的PS和IR寄存器的过程和读取ALS的过程中I2C的传输过程完全一致，这里就不再赘述。
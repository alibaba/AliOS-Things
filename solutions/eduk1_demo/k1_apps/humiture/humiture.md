@page humiture 温湿度计
# 实验介绍
本章主要介绍温湿度计的实现，本产品板载了温湿度传感器SI7006。该传感器不但能测量温度，还能测量相对湿度。本章我们将向大家介绍如何使用haas1000来读取SI7006数字温湿度传感器，从而得到环境温度和相对湿度等信息，并把从温湿度值显示在OLED模块上。
开始学习之前我们先看一下显示效果，如下图所示：



<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01qZ6L7Z1SxrAPcv9WY_!!6000000002314-2-tps-1200-800.png" style="zoom:50%;" />
</div>

# 涉及知识点

- I2C通信原理
- SI7006光照与接近传感器原理
- OLED绘图


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



### I2C通信原理
IIC总线是一种最早由PHILIPS公司开发的两线式串行总线，用于连接微控制器及其外围设备。它是由数据线SDA和时钟SCL构成的串行总线，可发送和接收数据。在CPU与被控IC之间、IC与IC之间进行双向传送，高速IIC总线一般可达400kbps以上。
I2C总线在传送数据过程中共有三种类型信号， 它们分别是：开始信号、结束信号和应答信号。 
开始信号：SCL为高电平时，SDA由高电平向低电平跳变，开始传送数据。 
结束信号：SCL为高电平时，SDA由低电平向高电平跳变，结束传送数据。 
应答信号：接收数据的IC在接收到8bit数据后，向发送数据的IC发出特定的低电平脉冲，表示已收到数据。CPU向受控单元发出一个信号后，等待受控单元发出一个应答信号，CPU接收到应答信号后，根据实际情况作出是否继续传递信号的判断。若未收到应答信号，由判断为受控单元出现故障。 
波形如下图：

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01RChTnX1b9mP4F1JX1_!!6000000003423-2-tps-1007-313.png" style="zoom:50%;" />
</div>

### SI7006传感器简介
   SI7006 是Silicon Lab公司推出的一款相对湿度及温度传感器，其结合了工厂校准湿度和温度传感器元件、模拟-数字转换器，信号处理和一个I2C主机接口。使用业界标准低K电介质聚合物提供了出色的精度和长期稳定性，同时具有低漂移和低滞后。同时其创新的CMOS设计还使其具备很低的功耗。
特征：

- 相对湿度传感器：
- ±5％RH（最大）@ 0-80％RH的
- 温度传感器：
- ±1.0℃的精确度（最大）@ -10至+ 85°C
- 0至100％RH的工作范围内
- 可达-40至+125°C的工作范围
- 工作电压范围宽（1.9〜3.6V）
- 低功耗：2.2μW平均功率为3.3V和每秒1个样本
- I2C主机接口
- 片上集成加热器
- 采用3mm x 3mm QFN封装
- 卓越的长期稳定性
- 支持工厂校准
- 在回流和运行寿命的保护
- 防止污染的灰尘，污垢，日用化工和其它液体

 
应用：

- 资产和货物跟踪
- 汽车气候控制和去雾
- 婴儿监视器
- 持续气道正压通气（CPAP）机
- 洪水和水检测
- 煤气，火灾和烟雾探测器
- 手机/智能手机
- 工业HVAC / R
- 笔记本电脑/平板电脑
- 激光打印机
- 微环境/数据中心
- PLC和IO模块
- 远程遥测装置
- 平板电脑
- 测试和测量
- 恒温/恒湿
- 通风和空调系统
- 气象站
- 风力发电逆变器
- 挡风玻璃和后视镜系统
- 无线基站
- 无线传感器网络
### OLED绘图
参考上一章节。
# 软硬件环境准备
## 硬件 
1、开发用电脑一台
2、HaaS EDU K1开发板一个 （开发板内置SI7006传感器）
3、USB Type-C 数据线一根

# 硬件设计
在本实验中，SI7006贴片在主板上，中间I2C与MCU通信，原理图如下所示：

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01uk6ZEJ27v0xC0hp7D_!!6000000007858-2-tps-810-512.png" style="zoom:50%;" />
</div>

EDU SI7006部分原理图
### 软件设计
#### 应用代码部分
文件路径如下：
solutions/eduk1_demo/k1_apps/humiture/humiture.c
solutions/eduk1_demo/k1_apps/humiture/humiture.h
```basic
        si7006_getTempHumidity(&hump, &temp);
        sprintf(temp_str, "T:%5.1fC", temp);
        sprintf(hump_str, "H:%5.1f%%", hump);

        OLED_Icon_Draw(14, 4, &icon_thermometer_24_24, 0);
        OLED_Icon_Draw(14, 36, &icon_hygrometer_24_24, 0);

        OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
        OLED_Icon_Draw(122, 24, &icon_skip_right, 0);

        OLED_Show_String(42, 8, temp_str, 16, 1);
        OLED_Show_String(42, 40, hump_str, 16, 1);

        OLED_Refresh_GRAM();
        aos_msleep(500);
```
#### 驱动部分
文件路径如下：
components/peripherals/sensor/drv/drv_temp_humi_si_si7006.c
驱动代码整体主要分为三部分：

- 获取产品ID
- 获取温度值
- 获取湿度值
```basic
void si7006_getID(uint8_t *id_buf)
{
    uint8_t reg[4]  = {Si7006_READ_ID_LOW_0,Si7006_READ_ID_LOW_1,Si7006_READ_ID_HIGH_0,Si7006_READ_ID_HIGH_1};

    hal_i2c_master_send(&i2c_dev, i2c_dev.config.dev_addr, reg, 2, 1000);
    aos_msleep(30);
    hal_i2c_master_recv(&i2c_dev, i2c_dev.config.dev_addr, id_buf, 4, 1000);

    hal_i2c_master_send(&i2c_dev, i2c_dev.config.dev_addr, &reg[2], 2, 1000);
    aos_msleep(30);
    hal_i2c_master_recv(&i2c_dev, i2c_dev.config.dev_addr, &id_buf[4], 4, 1000);

    return;
}

bool si7006_getTemperature(float *temperature)
{
    uint8_t reg  = Si7006_MEAS_TEMP_NO_MASTER_MODE;
    uint8_t read_data[2] = {0};
    unsigned int value;

    hal_i2c_master_send(&i2c_dev, i2c_dev.config.dev_addr, &reg, 1, 1000);
    aos_msleep(30);
    hal_i2c_master_recv(&i2c_dev, i2c_dev.config.dev_addr, read_data, 2, 1000);
    value = (read_data[0] << 8) | read_data[1];
    LOGI("APP", "%0x -- %0x -->0x%x\n", read_data[0],read_data[1],value);
    // A temperature measurement will always return XXXXXX00 in the LSB field.
    if (value & 0xFFFC)
    {
    	*temperature = (175.72f * (float)value) / 65536.0f - 46.85f;
        LOGI("APP", "temperature: %2f \n", *temperature);
    }
    else
    {
        LOGI("APP", "Error on temp\n");
        return 1;
    }
    return 0;
}

/*
i2c – the i2c device
dev_addr – device address
mem_addr – mem address
mem_addr_size – mem address
data – i2c master send data
size – i2c master send data size
*/
bool si7006_getHumidity(float *humidity)
{
    uint8_t reg  = Si7006_MEAS_REL_HUMIDITY_NO_MASTER_MODE;
    uint8_t read_data[3] = {0};
    unsigned int value;

    hal_i2c_master_send(&i2c_dev, i2c_dev.config.dev_addr, &reg, 1, 1000);

    aos_msleep(30);

    hal_i2c_master_recv(&i2c_dev, i2c_dev.config.dev_addr, read_data, 2, 1000);
    value = (read_data[0] << 8) | read_data[1];
    LOGI("APP", "%0x -- %0x -->0x%x\n", read_data[0],read_data[1],value);
    if (value & 0xFFFE)
    {
    	*humidity = ((125.0f * (float)value ) / 65535.0f) - 6.0f;
        LOGI("APP", "humidity: %f \n", *humidity);
    }
    else
    {
        LOGI("APP", "Error on humidity\n");
        return 1;
    }
    return 0;
}

//get temp and humidity
void si7006_getTempHumidity(float *humidity, float *temperature)
{
    si7006_getTemperature(temperature);
    si7006_getHumidity(humidity);
}
```



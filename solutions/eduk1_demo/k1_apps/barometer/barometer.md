@page barometer 大气压海拔仪
# 实验介绍
    本章主要介绍大气压计的实现，EDU k1板载了SI的一款温湿度传感器SPL06，该传感器不但能测量大气压，还能测量温度。由于大气压的跟海拔成反比关系，如果知道当地大气压，则可以估算出当地的海拔高度。本章我们将向大家介绍如何使用haas1000来读取传感器信息，并把所有这些显示在OLED模块上。
    开始学习之前我们先看一下显示效果。如下：


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN016aH1g11EdnC4Gu5Og_!!6000000000375-1-tps-1200-800.gif" style="zoom:50%;" />
</div>


# 涉及知识点
    - I2C通信原理
    - SPL06大气压传感器原理
    - OLED绘图

## I2C通信原理
    上一章已有介绍，这里不在赘述。
## SPL06传感器简介
       SPL06-007是歌尔推出的小型化数字气压传感器，具有高精度和低功耗的特点。SPL06-007既是压力传感器又是温度传感器。压力传感器元件基于电容式传感原理，确保温度变化时的高精度。小封装使SPL06-007成为移动应用和可穿戴设备的理想选择。
        SPL06-007的内部信号处理器将压力和温度传感器元件的输出转换为24位结果。每个压力传感器已单独校准，并包含校准系数。这些系数用于将测量结果转换为真实压力和温度值。
        SPL06-007有一个FIFO，可以存储最新的32个测量值。通过使用FIFO，主机处理器可以在两次读出之间的较长时间内保持休眠模式。这可以降低整个系统的功耗。传感器测量和校准系数可通过串行I2C接口获得。

### 主要特点
    - 压力范围：300hPa到1100hPa（+9000米到-500米（与海平面有关））
    - 温度范围：-40到+85°C
    - 电源电压：1.7V到3.6V（VDD），1.2V到3.6V（VDDIO）
    - 包装：带金属盖的LGA包装
    - 封装：2.5mm x 2.0mm；
    - 相对精度：±0.06hPa，相当于±0.5m
    - 绝对精度：典型。±1hPa（300hPa到1100hPa）
    - 温度精度：±0.5°C。
    - 压力温度灵敏度：<0.5Pa/K
    - 测量时间：典型值：28 ms；最小值：3 ms。
    - 平均电流消耗：高精度：60μA，低功耗：3μA，待机：<1μA。
    - I2C和SPI接口，嵌入式24位ADC
    - FIFO：存储最新的32个压力或温度测量值。
    - 无铅、无卤、符合RoHS

### 典型应用
    - 增强GPS导航（航位推算、坡度检测等）
    - 室内外导航
    - 休闲体育
    - 天气预报
    - 垂直速度指示（上升/下降速度）

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


# 硬件设计
    在本实验中，SPL06贴片在主板上，通过I2C与MCU通信，I2C从设备地址：0x77。
    原理图如下所示：

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01RBAfw324j9r7PVJlA_!!6000000007426-2-tps-1064-626.png" style="zoom:50%;" />
</div>


## SPL06特性
    寄存器列表

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01dT2djk1f96HKelrDV_!!6000000003963-2-tps-1116-1220.png" style="zoom:50%;" />
</div>


## 读取大气压
### 压力数据寄存器
    这个压力数据寄存器包含24位（3个字节）的压力值。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01eGALXU1vBsXCOdLbU_!!6000000006135-2-tps-1370-420.png" style="zoom:50%;" />
</div>


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01REUYOk1TlJwRjprs9_!!6000000002422-2-tps-1378-474.png" style="zoom:50%;" />
</div>


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN018fH3wN1EE90ym8uDo_!!6000000000319-2-tps-1366-454.png" style="zoom:50%;" />
</div>


### 校准寄存器
    校准系数寄存器包含用于计算补偿压力和温度值时所用的系数。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01KzTB9w1t0lh0SUmA0_!!6000000005840-2-tps-1374-1266.png" style="zoom:50%;" />
</div>


###  如何计算气压
    1、从校准系数寄存器读取校准系数（c00、c10、c20、c30、c01、c11和c21）。
    2、根据选定的精度速率选择比例因子kT（温度）和kP（压力）。表4列出了比例因子。
    3、从寄存器或FIFO读取压力和温度结果。根据所选的测量速率，自上次压力测量后可能没有测量温度。
    4、计算比例测量结果。
    Traw_sc = Traw/kT 
    Praw_sc = Praw/kP 
    5、计算补偿测量结果。
    Pcomp(Pa)= c00+ Praw_sc*(c10+ Praw_sc*(c20+ Praw_sc*c30)) + Traw_sc*c01 + Traw_sc*Praw_sc*(c11+Praw_sc*c21)
## 读取温度
### 温度数据寄存器
    温度寄存器同样保存着24位（3个字节）的温度数据。


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01hlX6cJ1UBsuaFcnAX_!!6000000002480-2-tps-1362-462.png" style="zoom:50%;" />
</div>


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01wPwn6B1GOL3tkHVqg_!!6000000000612-2-tps-1372-462.png" style="zoom:50%;" />
</div>


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01VUAGX41GTNQ3tDS3l_!!6000000000623-2-tps-1354-366.png" style="zoom:50%;" />
</div>


### 如何计算温度
    1、从校准系数（COEF）寄存器读取校准系数（c0和c1）。
    2、根据选定的精度速率选择比例因子kT（温度），表4列出了比例因子。
    3、从温度寄存器或FIFO读取温度结果。
    4计算比例测量结果。
    Traw_sc=Traw/kT
    5计算补偿测量结果
    Tcomp（°C）=c0*0.5+c1*Traw_sc


# 软件设计
## 应用代码部分
文件路径如下：

    solutions/eduk1_demo/k1_apps/barometer/barometer.c
    solutions/eduk1_demo/k1_apps/barometer/barometer.h

周期性读取大气压以及温度，并打印到OLED屏幕上。
```c
OLED_Clear();

OLED_Icon_Draw(14, 4, &icon_atmp_16_16, 0);
sprintf(pressure_str, " %-10.3lfkPa", spl06_data.pressure / 10);
LOGD(EDU_TAG, "%s\n", pressure_str);
OLED_Show_String(32, 6, pressure_str, 12, 1);

OLED_Icon_Draw(14, 23, &icon_asl_16_16, 0);
sprintf(altitude_str, " %-12.2lfm", spl06_data.altitude);
LOGD(EDU_TAG, "%s\n", altitude_str);
OLED_Show_String(32, 25, altitude_str, 12, 1);

OLED_Icon_Draw(14, 44, &icon_tempC_16_16, 0);
sprintf(Ctemp_str, "%-5.2lf", spl06_data.Ctemp);
LOGD(EDU_TAG, "%s\n", Ctemp_str);
OLED_Show_String(30, 46, Ctemp_str, 12, 1);

OLED_Icon_Draw(66, 44, &icon_tempF_16_16, 0);
sprintf(Ftemp_str, "%-5.2lf", spl06_data.Ftemp);
LOGD(EDU_TAG, "%s\n", Ftemp_str);
OLED_Show_String(82, 46, Ftemp_str, 12, 1);

OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
OLED_Icon_Draw(122, 24, &icon_skip_right, 0);

spl06_getdata(&spl06_data);

OLED_Refresh_GRAM();
```
## 驱动部分
驱动代码整体分为三部分

    - 初始化部分
    - 温度值读取
    - 大气压校准以及高度估算。

### 初始化

    - I2C初始化
    - 芯片复位以及模式配置（包含压力以及温度模式配置，同时采集温度以及大气压，打开FIFO接收）
在 AliOS Things 3.3中，对I2C的操作才用了VFS的方式，开发者只需要open相应的device设备，开发者只需要关心链接到的I2C设备号，从器件的设备地址，从器件支持的I2C速率。知道了从设备地址，读写地址也可以计算出，AliOS Things 会自动处理这些计算。
```c
int32_t ret = sensor_i2c_open(SPL06_I2C_PORT, EEPROM_CHIP_ADDRESS, I2C_BUS_BIT_RATES_100K, 0);
if (ret) {
    LOGE("SENSOR", "sensor i2c open failed, ret:%d\n", ret);
    return;
}
aos_msleep(500);

tmp = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0x0D);
i2c_eeprom_write_uint8_t(EEPROM_CHIP_ADDRESS, 0X06, 0x03); // Pressure 8x oversampling
i2c_eeprom_write_uint8_t(EEPROM_CHIP_ADDRESS, 0X07, 0X83); // Temperature 8x oversampling
i2c_eeprom_write_uint8_t(EEPROM_CHIP_ADDRESS, 0X08, 0B0111); // continuous temp and pressure measurement
// pressure measurement
i2c_eeprom_write_uint8_t(EEPROM_CHIP_ADDRESS, 0X09, 0X00); // FIFO Pressure measurement
```


### 温度读取及校准代码
```c
tmp_Byte = i2c_eeprom_read_uint8_t(EEPROM_CHIP_ADDRESS, 0X07); // MSB
tmp_Byte = tmp_Byte & 0B00000111;

switch (tmp_Byte)
{
    case 0B000:
        k = 524288.0d;
        break;

    case 0B001:
        k = 1572864.0d;
        break;

    case 0B010:
        k = 3670016.0d;
        break;

    case 0B011:
        k = 7864320.0d;
        break;

    case 0B100:
        k = 253952.0d;
        break;

    case 0B101:
        k = 516096.0d;
        break;

    case 0B110:
        k = 1040384.0d;
        break;

    case 0B111:
        k = 2088960.0d;
        break;
}

c0 = get_c0();
c1 = get_c1();
double traw_sc = (double)traw / get_temperature_scale_factor();
//printf("traw_sc: %0.2f\n", traw_sc);

sp->Ctemp = (double)c0 * 0.5f + (double)c1 * traw_sc;
sp->Ftemp = (sp->Ctemp * 9 / 5) + 32;
```


### 气压读取及校准代码
```c
c00 = get_c00();
c10 = get_c10();
c01 = get_c01();
c11 = get_c11();
c20 = get_c20();
c21 = get_c21();
c30 = get_c30();

int32_t praw = get_praw();
double praw_sc = (double)(praw) / get_pressure_scale_factor();
double pcomp = (double)(c00) + praw_sc * ((double)(c10) + praw_sc * ((double)(c20) + praw_sc * (double)(c30))) + traw_sc * (double)(c01) + traw_sc * praw_sc * ((double)(c11) + praw_sc * (double)(c21));
sp->pressure = pcomp / 100; // convert to mb

double local_pressure = 1011.1; //本地参考海平面参考气压值
sp->altitude = get_altitude(pcomp, local_pressure);
```

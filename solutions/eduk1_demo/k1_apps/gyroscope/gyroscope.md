@page gyroscope 陀螺仪小球
# 实验介绍
手机上的“重力感应”想必大家都不陌生，基于重力感应可以实现飞车、辅助瞄准等各种体感非常强的交互功能。所谓的“重力感应”，其实就可以使用“加速度计及陀螺仪传感器”来实现。
本节实验中，我们将会基于 MPU-6050 实现一个根据开发板姿态滚动的小球。当我们倾斜 HaaS EDU K1 时，会看见小球根据我们的倾斜方向发生相对应的偏移。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN019hMi8z1duc8ShPnTo_!!6000000003796-1-tps-1200-800.gif" style="zoom:50%;" />
</div>

# 涉及知识点

- MPU-6050 的驱动和使用
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

# 硬件介绍 - MPU-6050
## DataSheet
了解一款IC最快捷和精准的方法是查阅它的DataSheet。这款IC的DataSheet可以在以下链接获取。
[MPU-6000-Datasheet1.pdf](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf)
[MPU-6000-Register-Map1.pdf](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf)
## 硬件规格
     MPU-60X0是世界上第一款集成 6 轴MotionTracking设备。它集成了**3轴MEMS陀螺仪，3轴MEMS加速度计**，以及一个可扩展的数字运动处理器 DMP（ DigitalMotion Processor），可用I2C接口连接一个第三方的数字传感器，比如磁力计。
MPU-6050具有三个用于将陀螺仪输出数字化的16位模数转换器（ADC）和三个用于将加速度计输出数字化的16位ADC。 为了精确跟踪快速和慢速运动，这些部件具有用户可编程的陀螺仪满量程范围，范围为±250，±500，±1000和±2000°/ sec（dps），以及用户可编程的加速度计满量程范围 范围为±2g，±4g，±8g和±16g。使用400kHz的I2C与设备的所有寄存器进行通信。其他功能包括嵌入式温度传感器和片上振荡器，在整个工作温度范围内误差±1％。
更多硬件规格请参考 DataSheet。
## 传感原理
陀螺仪由1850年法国物理学家莱昂·傅科在研究地球自传中获得灵感而发明出来的，类似像是把一个高速旋转的陀螺放到一个万向支架上，靠陀螺的方向来计算角速度，和现在小巧的芯片造型大相径庭。

|  |
 |
| --- | --- |
| 早期的机械陀螺仪[1]              | 目前常见的电子陀螺仪 |

那么如何将这么庞大的机械设备，塞进小小的芯片当中呢？以目前广泛使用的MPU-6050为例，它属于传感MEMS分支。传感MEMS技术是指用微电子微机械加工出来的、用敏感元件如电容、压电、压阻、热电耦、谐振、隧道电流等来感受转换电信号的器件和系统。感兴趣的同学可以来这里学习亚德诺半导体的公开课 —— [MEMS传感器2：加速，旋转——陀螺仪工作原理](https://www.analog.com/cn/education/education-library/videos/5996766351001.html)。
我们使用的MPU-6050是一款经典的MEMS陀螺仪，即硅微机电陀螺仪。MEMS(Micro-Electro-Mechanical System)是指集机械元素、微型传感器、微型执行器以及信号处理和控制电路、接口电路、通信和电源于一体的完整微型机电系统。绝大多数的MEMS陀螺仪依赖于相互正交的振动和转动引起的交变科里奥利力。在MPU-6050内部，存在一质量块，当器件上电后，会触发质量块以固定频率横向运动。当器件遭受外力具备加速度时，就会触发质量块的纵向运动，从而改变四周梳齿之间的距离，改变输出的电容，进而通过ADC将模拟信号转换为数字信号，输出给外部[2]。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01XULlZo1EGQzbOj08P_!!6000000000324-1-tps-448-279.gif" style="zoom:50%;" />
</div>

## 原理图

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN0163mBYO1RNO0tDwwSg_!!6000000002099-2-tps-1338-976.png" style="zoom:50%;" />
</div>

在原理图中我们可以看出，器件使用I2C通讯接口。并且，器件支持使用过INT引脚，当数据到来时可以在该引脚上触发中断。需要注意的是，AD0引脚决定了器件地址的第 0 bit。当AD0连接高电平，即AD0 = 1，此时器件地址为0x69。
## 驱动方式
### 通讯接口
由DataSheet可知，MPU-6050采用的通讯方式为I2C。默认7bit设备地址：0x69 (DataSheet P33 9.2)
在 AliOS Things 3.3中，I2C操作方式采用VFS的方式，开发者只需要关心器件的设备地址即可，因为只要知道了设备地址，读写地址也可以计算出，AliOS Things 会自动处理这些计算。如果我们需要为了 MPU-6050 初始化I2C接口，那么对应的代码为：
```c
// solutions/eduk1_demo/drivers/sensor/drv_acc_gyro_inv_mpu6050.c

// 初始化I2C
int32_t ret = sensor_i2c_open (MPU_I2C_PORT, MPU_ADDR, I2C_BUS_BIT_RATES_100K, 0);
if (ret) {
    LOGE("SENSOR", "sensor i2c open failed, ret:%d\n", ret);
    return -EIO;
}
```
### 寄存器
一般，使用I2C通讯的器件，都是通过读写寄存器的方式来完成对设备的读取和配置，因此了解寄存器的分布就非常重要。由于 MPU-6050 的寄存器数量较多，建议读者们查阅 Regsiter Map 文档来获取这些信息。我们进列出部分较为关键的寄存器。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN019KZFs71fzJQikctwK_!!6000000004077-2-tps-1946-812.png" style="zoom:50%;" />
</div>

0x3B-0x40 三轴加速度寄存器     每轴数据2Byte
0x41-0x42 温度寄存器               2Byte
0x43-0x48 三轴陀螺仪寄存器     每轴数据2Byte
### 驱动实现
#### uint8_t MPU_Init(void)
器件初始化。
```c
// solutions/eduk1_demo/drivers/sensor/drv_acc_gyro_inv_mpu6050.c

uint8_t MPU_Init(void)
{
	uint8_t device_id = 0;

    MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X80); // 复位MPU6050
    aos_msleep(100);
    MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X00); // 唤醒MPU6050
    MPU_Set_Gyro_Fsr(3);                     // 陀螺仪传感器,±2000dps
    MPU_Set_Accel_Fsr(0);                    // 加速度传感器,±2g
    MPU_Set_Rate(50);                        // 设置采样率50Hz
    MPU_Write_Byte(MPU_INT_EN_REG, 0X00);    // 关闭所有中断
    MPU_Write_Byte(MPU_USER_CTRL_REG, 0X00); // I2C主模式关闭
    MPU_Write_Byte(MPU_FIFO_EN_REG, 0X00);   // 关闭FIFO
    MPU_Write_Byte(MPU_INTBP_CFG_REG, 0X80); // INT引脚低电平有效
    device_id = MPU_Read_Byte(MPU_DEVICE_ID_REG);
    if (device_id == MPU_DEV_ID) {
        // 器件ID正确
        LOGI("SENSOR", "MPU init OK\n");
        MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X01); // 设置CLKSEL,PLL X轴为参考
        MPU_Write_Byte(MPU_PWR_MGMT2_REG, 0X00); // 加速度与陀螺仪都工作
        MPU_Set_Rate(50);                        // 设置采样率为50Hz
    } else {
        LOGE("SENSOR", "MPU init Error -- %x\n", device_id);
        return 1;
    }
    return 0;
}
```
#### void MPU_Get_Gyroscope(short *gx, short *gy, short *gz)
读取三轴陀螺仪数据。由Register Map得知，只需要从GYRO_XOUTH向后依次读出6个寄存器内容即可。
```c

void MPU_Get_Gyroscope(short *gx, short *gy, short *gz)
{
	uint8_t buf[6];

	MPU_Read_Len(MPU_GYRO_XOUTH_REG, 6, buf);
	*gx = ((u16)buf[0] << 8) | buf[1];
	*gy = ((u16)buf[2] << 8) | buf[3];
	*gz = ((u16)buf[4] << 8) | buf[5];
}
```
#### void MPU_Get_Accelerometer(short *ax, short *ay, short *az)
读取三轴加速度数据。由Register Map得知，只需要从ACCEL_XOUTH向后依次读出6个寄存器内容即可。
```c
// solutions/eduk1_demo/drivers/sensor/drv_acc_gyro_inv_mpu6050.c

void MPU_Get_Accelerometer(short *ax, short *ay, short *az)
{
	uint8_t buf[6];

	MPU_Read_Len(MPU_ACCEL_XOUTH_REG, 6, buf);
	*ax = ((u16)buf[0] << 8) | buf[1];
	*ay = ((u16)buf[2] << 8) | buf[3];
	*az = ((u16)buf[4] << 8) | buf[5];
}
```
# 应用开发
本实验的应用较为简单，只需要读出加速度数据，并显示在屏幕上即可。OLED的相关接口中已经给出了丰富的绘图函数。具体实现如下。
```c
void gyroscope_task(void)
{
    while (1)
    {
        // 清除屏幕memory
        OLED_Clear();
        // 获取三轴加速度信息
        MPU_Get_Accelerometer(&r_ax, &r_ay, &r_az);
        // 画出固定的圆形边框
        OLED_DrawCircle(66, 32, 10, 1, 1);
        // 画出填充的圆
        OLED_FillCircle(66 - r_ax / 250, 32 + r_ay / 500, 8, 1);
        // 将屏幕memory显示出来
        OLED_Refresh_GRAM();
        // 暂停20ms
        aos_msleep(20);
    }
}
```
# 更多应用
本实验展示的仅是六轴传感器的一个非常简单的应用。随着类似传感器的体积越来越小，精度越来越高，它们也被应用在各种消费类电子产品，如穿戴设备、手机上。
使用它们可以进行一些非常有趣的应用，例如，手环中经常会使用到的计步算法，运动状态检测算法，都是基于其中的六轴传感器数据。近年来，还有很多学术界的工作，使用手环中的六轴传感器来实现空中写字的识别。
除此之外，它也广泛应用于辅助定位、飞行设备的姿态检测，摄像机云台的水平保持等等。期待读者们能够发掘出更多有价值的使用场景。
# 引用
[1] Gyroscope invented by Léon Foucault in 1852. Replica built by Dumoulin-Froment for the Exposition universelle in 1867. [National Conservatory of Arts and Crafts museum](https://en.wikipedia.org/wiki/Conservatoire_national_des_arts_et_m%C3%A9tiers), Paris.
By Stéphane Magnenat - Own work by uploader, subject public domain, Public Domain, [https://commons.wikimedia.org/w/index.php?curid=4302903](https://commons.wikimedia.org/w/index.php?curid=4302903)
[2] 图片来自 [https://www.analog.com/cn/education/education-library/videos/5996766351001.html](https://www.analog.com/cn/education/education-library/videos/5996766351001.html)

@page compass 电子罗盘
# 实验介绍
“电子罗盘”，作为一种非常常见的功能，目前市场上的智能手机几乎都默认配备了。在生活中，它可以帮助我们找准方向，进行辅助定位等等。除了手机之外，电子罗盘也广泛应用于航空、航天、机器人、航海、车辆自主导航等领域。在本实验中，我们也来实现一个“电子罗盘”。


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01yRJYEH1qpCSZuKNk3_!!6000000005544-1-tps-1200-800.gif" style="zoom:50%;" />
</div>


 从上图中可以看到，随着方向的转动，屏幕上的指针和度数也在发生着变化，指针指向的刻度标明了箭头所指方向的角度度数。实现这个效果的方法很简单，只需要获取电子罗盘传感器的读数，然后根据读数绘制罗盘界面即可。
# 涉及知识点

- QMC5883L 三轴AMR磁传感器的原理和使用
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


# 硬件介绍 - QMC5883L
## DataSheet
了解一款IC最快捷和精准的方法是查阅它的DataSheet。这款IC的DataSheet可以在以下链接获取。
[QMC5883L-Datasheet-1.0.pdf](https://nettigo.pl/attachments/440)
## 硬件规格
QMC5883L是一款多芯片三轴磁传感器。旨在用于无人机，机器人，移动和个人手持设备中的指南针，导航和游戏等高精度应用。QMC5883L 采用异相磁阻（anisotropic magnetoresistance  AMR）技术，这些各向异性传感器具有在轴向高灵敏度和线性高精度的特点。传感器具有的对正交轴低灵敏度的固相结构能用于测量地球磁场的方向和大小，其测量范围从毫高斯到 8 高斯。它具有低噪声，高精度，低功耗，失调消除和温度补偿的优点。 QMC5883L可实现1°至2°的罗盘航向精度。 使用I2C串行总线简化接口。
更详细的硬件规格参数可以参考 DataSheet 的 2.1 Product Specifications
## 传感原理


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01yUeybI1n44rRZrsUB_!!6000000005035-2-tps-854-465.png" style="zoom:50%;" />
</div>


通过这张内部原理图我们可以很清楚地看出，在QMC5883L内部，存在三组两两正交的磁阻，这种特殊的电阻的阻值会随着施加的磁场而变化，因此使用模数转换器读取这种变化，就能够获得器件在磁场中的变化信息，进而判断器件的指向。
### 名词释义

- AMR Bridge：三轴异相磁阻传感器，其结构是一个电阻电桥，其电阻会随着所施加的磁场而变化。三个传感器相互正交，从而能够得出三个方向上的磁场强度。
- MUX：多路复用通道，通过开关选中对应的通路，可以采集该通路的传感器数据。
- PGA：可编程控制的传感器信号增益放大器。
- Signal Conditioning：进行磁场信号校正及补偿的数字模块。
- ADC：模数转换器，用于将传感器侧得到的模拟信号转换为数字信号。
- NVM：用于校正的非易失性存储器。
- SET/RST Driver：用于初始化磁性传感器的内部驱动。
- Reference：用于内部偏移的电压/电流基准。
- Temperature Sensor：用于内部精度/偏移的温度传感器，也可以用于测量温度并输出。
## 驱动方式
### 通讯接口
由DataSheet可知，QMC5883L采用的通讯方式为I2C。默认7bit设备地址：0x0D      (DataSheet P10 5.4)
在 AliOS Things 3.3中，对I2C的操作才用了VFS的方式，开发者只需要open相应的device设备，开发者只需要关心链接到的I2C设备号，从器件的设备地址，从器件支持的I2C速率。知道了从设备地址，读写地址也可以计算出，AliOS Things 会自动处理这些计算。因此：
I2C设备的写地址 = I2C设备地址 << 1 = 0x0D << 1 = 0x1A
I2C设备的读地址 = (I2C设备地址 << 1) + 1 = 0x1B
这也符合在 (DataSheet P14 I2C R/W Operation) 中给出的结论。
如果我们需要为了QMC5883L初始化I2C接口，那么对应的代码为：
```c
int32_t ret = sensor_i2c_open(QMC5883L_I2C_PORT, QMC5883L_ADDR, I2C_BUS_BIT_RATES_100K, 0);
if (ret) {
    LOGE("SENSOR", "sensor i2c open failed, ret:%d\n", ret);
    return;
}
```
### 寄存器地址
一般，使用I2C通讯的器件，都是通过读写寄存器的方式来完成对设备的读取和配置，因此了解寄存器的分布就非常重要。查阅 DataSheet(from DataSheet P16 9.1 Register Map) 可以得到：


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01ejfcuf20HRQJGWzLf_!!6000000006824-2-tps-1890-902.png" style="zoom:50%;" />
</div>


其中，根据Access属性的不同，我们分为只读寄存器和读写寄存器，来分析一些驱动过程中较为重要的寄存器。
#### 只读寄存器（输出寄存器）
只读寄存器一般用于读取设备的状态和采样数据。
Table 14. Output Data Register


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01DpXHst1XyNhPzAmhj_!!6000000002992-2-tps-2454-548.png" style="zoom:50%;" />
</div>


- Data Output X Y Z  LSB&MSB **00H-05H**

三轴的采样数据，每个轴输出的数据大小均为16bit，分别存储在2个8bit寄存器中。
#### 读写寄存器（配置寄存器）
读写寄存器一般用于配置器件。对于QMC5883L而言，它的初始化只需要关心这一个寄存器**09H**。
Table 18. Control Register 1


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN014ksAji284AsJZJW1k_!!6000000007878-2-tps-2634-684.png" style="zoom:50%;" />
</div>


- OSR

Over Sample Ratio 过采样率寄存器。用于控制内部数字滤波器的带宽。较大的OSR值会带来较小的滤波器带宽，以及更少的带内噪声和更高的功耗。可以通过配置此寄存器达到噪声和功率之间的良好平衡。

- RNG

Range 量程选择寄存器。可以通过寄存器RNG配置磁传感器的感应范围。量程的选择应该是由具体的应用场景决定的。对于磁性清晰的环境，在弱磁场环境下，例如 +/- 2 gauss 应当选用较小的量程。量程与磁传感器的灵敏度息息相关。在采样位数不变的情况下，最低量程具有最高的灵敏度，因此分辨率更高。


- ODR

Output Data Rate 输出数据频率寄存器。

- MODE

模式控制寄存器。用于配置器件是否开启连续测量模式。在连续测量模式下，Data Output X Y Z 6个寄存器会连续刷新数据，只需要 从Data Output 地址处连续读取即可获得实时的测量数据。
### 驱动实现
完成I2C的初始化之后，我们就可以针对QMC5883L来进行初始化。在AliOS Things 3.3中，我们已经对器件进行了部分抽象，可以在初始化时配置。
```c
// solutions/eduk1_demo/drivers/sensor/drv_mag_honeywell_qmc5883l.c
// solutions/eduk1_demo/drivers/sensor/drv_mag_honeywell_qmc5883l.h

static void _qmc5883l_init() {
    /* This assumes the wire library has been initialized. */
    addr = QMC5883L_ADDR;
    oversampling = QMC5883L_CONFIG_OS512;		// 512倍过采样
    range = QMC5883L_CONFIG_8GAUSS;				// 8高斯量程
    rate = QMC5883L_CONFIG_200HZ;				// 200Hz数据输出频率
    mode = QMC5883L_CONFIG_CONT;				// 开启连续采样模式
    qmc5883l_reset();
}
```
完成配置后，驱动程序会向对应寄存器写入数据。
```c
// solutions/eduk1_demo/drivers/sensor/drv_mag_honeywell_qmc5883l.c
// solutions/eduk1_demo/drivers/sensor/drv_mag_honeywell_qmc5883l.h

# define QMC5883L_CONFIG2 0x09
# define QMC5883L_CONFIG2 0x0a

static qmc5883l_write_register(uint8_t addr, uint8_t reg, uint8_t data)
{
    uint8_t write_buffer[2] = {reg, data};
    qmc5883l_i2c_master_send(write_buffer, 2, 1000);
}

static void qmc5883l_reconfig()
{
    qmc5883l_write_register(addr, QMC5883L_CONFIG, oversampling|range|rate|mode);	// 向0x09配置寄存器写入配置数据
    qmc5883l_write_register(addr, QMC5883L_CONFIG2, 0x1);    						// 软重启
}
```
同理，如果需要读取寄存器，则需要先向器件写入目标读取地址，再读取器件数据。
```c
// solutions/eduk1_demo/drivers/sensor/drv_mag_honeywell_qmc5883l.c
// solutions/eduk1_demo/drivers/sensor/drv_mag_honeywell_qmc5883l.h

uint8_t qmc5883l_read_len(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len)
{
    qmc5883l_i2c_master_send(&reg, 1, 1000);
    aos_msleep(20);
    qmc5883l_i2c_master_recv(buf, len, 1000);
    return 1;
}

int qmc5883l_readRaw(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t data[6];
    uint32_t timeout = 10000;

    while(!qmc5883l_ready() && (timeout--));

    if(!qmc5883l_read_len(addr, QMC5883L_X_LSB, data, 6)) return 0;
    *x = data[0] | (data[1]<<8);
    *y = data[2] | (data[3]<<8);
    *z = data[4] | (data[5]<<8);

    printf("get org data [%d,%d,%d]\n", x, y, z);

    return 1;
}
```
至此，就可以从QMC5883L中读出采样数据。由于电子罗盘只需要平面的方向信息，因此只需要使用
```c
atan2(x_fit, y_fit) * 180.0 / M_PI;
// atan2(x, y) 返回以弧度表示的 y/x 的反正切
```
就可以得到当前的指向信息。
但是这样通过原始数据得到的角度，会表现出非常不准确。因此，接下来我们需要对数据进行校准。
## 数据校准
### 原始数据采样
通过前文中实现的 int qmc5883l_readRaw(int16_t *x, int16_t *y, int16_t *z) 函数，我们可以实现对器件在各个方向上的采样，由于我们主要关心 X，Y 轴的数据，因此我们需要将器件在水平面上遍历各个方向并读取数据。数据通过 printf("get org data [%d,%d,%d]\n", x, y, z) 打印在串口上。
具体的做法就是，在水平面上缓缓旋转 HaaS EDK 至少覆盖完整的一圈，并且在此过程中保证周围没有磁场干扰（比如任何包含磁性的物体）。


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01gN42Az1TdzbSG78Fr_!!6000000002406-1-tps-1200-1200.gif" style="zoom:50%;" />
</div>


采集完后的数据如下：
```python
org_list = [
    [-1438,-1690,-2665],
	[-1438,-1690,-2665],
	[-1446,-1660,-2667],
	[-1418,-1642,-2702],
    ...
]
```
我们使用matploatlib将这段数据可视化为散点图，散点图的X，Y轴分别代表了从QMC5883L中读取到的X，Y轴数据，点的颜色由深到浅标记了采样点的顺序由前到后。


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01X414R91vCKvgaYicC_!!6000000006136-2-tps-271-248.png"/>
</div>


可以观察到，随着我们的旋转，散点构成了一个较为完整的圆形，说明X，Y轴的两个磁力传感器对于磁场变化的灵敏度大致一致，即变化等量磁通量带来的电阻变化是相似的，才能够呈现出较为标准的圆形。而如果两个传感器的灵敏度差别较大，就会出现下右图的状况，X轴的响应更灵敏，则在X轴上测量出的间隔更大，X轴更长，类似一个椭圆。


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01V5VhAw1UEdI1STj82_!!6000000002486-2-tps-390-248.png"/>
</div>


由于我们使用 atan2 方法将 X Y 轴数据映射到极坐标系上，如果我们想得到精准的映射，我们需要保证圆心处于（0，0）位置。然而，这两张图可以看出，圆心都偏离了（0，0）。因此我们首先需要找到圆心（对于三轴而言是球心）坐标。
对于部分器件，可能存在不同轴灵敏度不一致的情况，我们可以通过如下公式将三个轴缩放到同一个尺度。其中，x_min，x_max 等代表数据边界，x_offset 等代表修正的数据球心。
```latex
\dfrac{x-x_{offset}}{x_{\max}-x_{\min }}=\dfrac{y-y_{offset}}{y_{\max}-y_{\min }}=\dfrac{z-z_{offset}}{z_{\max}-z_{\min }}
```


```c
 x_max = y_max = z_max = INT16_MIN;
 x_min = y_min = z_min = INT16_MAX;

int qmc5883l_readHeading()
{
    int16_t x_org, y_org, z_org;		// 原始数据
    float x_offset, y_offset, z_offset;	// 球心坐标偏移
    float x_fit, y_fit, z_fit;			// 修正数据

    if (!qmc5883l_readRaw(&x_org, &y_org, &z_org))
        return 0;

    // 在每次读取数据时都更新数据边界
    x_min = x_org < x_min ? x_org : x_min;
    x_max = x_org > x_max ? x_org : x_max;
    y_min = y_org < y_min ? y_org : y_min;
    y_max = y_org > y_max ? y_org : y_max;
    z_min = z_org < z_min ? z_org : z_min;
    z_max = z_org > z_max ? z_org : z_max;

    // 如果数据量不足以计算边界 则退出
    if (x_min == x_max || y_min == y_max || z_max == z_min)
        return 0;

    // 通过边界计算出圆心偏移
    x_offset = (x_max + x_min) / 2.0;
    y_offset = (y_max + y_min) / 2.0;
    z_offset = (z_max + z_min) / 2.0;

	// 修正比例
    x_fit = (x_org - x_offset) * 1000.0 / (x_max - x_min);
    y_fit = (y_org - y_offset) * 1000.0 / (y_max - y_min);
    z_fit = (z_org - z_offset) * 1000.0 / (z_max - z_min);

    printf("fix[%f,%f,%f],\n", x_fit, y_fit, z_fit);

    int heading = 180.0 * atan2(x_fit, y_fit) / M_PI;
    heading = (heading <= 0) ? (heading + 360) : heading;

    return heading;
}

```
下图是添加了修正算法后得到的数据，可以看到，随着颜色变浅，数据量增加，散点逐渐拟合到一个圆形上，且圆心处于 (0,0) 位置。


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01SAnRV91Giwt0DUF2M_!!6000000000657-2-tps-266-248.png"/>
</div>


至此，我们就得到了较为准确的朝向数据。
# 界面开发
在本实验的界面开发中，使用到了以下方法：
```c
while (1)
{
    heading = qmc5883l_readHeading(); // 读取电子罗盘数据

    OLED_Clear();					// 清理上一次绘画的信息
    OLED_Icon_Draw(COMPASS_CENTER_X - 27, COMPASS_CENTER_Y - 27, &icon_compass_55_55, 0);	// 用于绘制罗盘盘面
    OLED_DrawLine_ByAngle(COMPASS_CENTER_X, COMPASS_CENTER_Y, (-heading-90), arror_len, 1);	// 用于绘制罗盘指针
    OLED_Show_String(96, 4, code_str, 24, 1);		// 用于绘制方向字符
    OLED_Show_String(78, 36, number_str, 24, 1);	// 用于绘制方向读数
    OLED_Refresh_GRAM();			// 将画面显示在屏幕上

    aos_msleep(30);
}
```



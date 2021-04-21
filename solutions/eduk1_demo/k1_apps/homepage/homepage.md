@page homepage 首页信息屏
# 实验介绍
本章主要介绍首页信息屏的实现，作为第一页，首页信息实现了类手机的界面，其中包含了丰富的内容。
1）产品名称
2) 版本信息
3) 系统状态，系统时间，WIFI连接，蓝牙连接。
4) IP地址
#### 显示效果
界面显示效果如下：

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01MhNElU1M8fCNezy7v_!!6000000001390-2-tps-1200-800.png" style="zoom:50%;" />
</div>



本章包含的内容非常多，通过本章的学习，可以学到多个知识点，比如OLED的驱动显示，ADC获取电量信息，Wi-Fi连接状态获取等等。
# 涉及知识点

- ADC的配置以及使用
- OLED的使用
- Wi-Fi配网
- 系统时钟获取（NTP网络对时）

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


# 各实验介绍
## OLED开发与显示
### OLED背景介绍
OLED，即有机发光二极管（Organic Light-Emitting Diode），又称为有机电激光显示（Organic Electroluminesence Display，OELD）。OLED由于同时具备自发光，不需背光源、对比度高、厚度薄、视角广、反应速度快、可用于挠曲性面板、使用温度范围广、构造及制程较简单等优异之特性，被认为是下一代的平面显示器新兴应用技术。
LCD都需要背光，而OLED不需要，因为它是自发光的。这样同样的显示，OLED效果要来得好一些。以目前的技术，OLED的尺寸还难以大型化，但是分辨率确可以做到很高。
### 硬件设计
在本实验中，OLED作为一个小板固定在前面板上，中间通过排线与主板相连。
本实验采用的OLED屏有以下特点：
1）模块为单色显示、黑底白字
2）显示尺寸为1.3寸
3）高分辨率，该模块的分辨率为132*64
4）硬件接口采用SPI总线接口


板载OLED原理图：

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01gZptiv1Bun2rnrAxi_!!6000000000006-2-tps-1052-1162.png" style="zoom:50%;" />
</div>
 

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01fE0Xyc1IsgXFTHDm8_!!6000000000949-2-tps-594-388.png" style="zoom:50%;" />
</div>


### 软件设计
关于OLED的详细原理介绍以及使用，请参考第三章OLED部分。这里主要介绍如何使用，从原理图的得知，OLED连接的主板的SPI0，采用的是4线SPI模式。
#### 初始化
首先初始化SPI0,这个可以在总入口函数中找到：
solutions/eduk1_demo/app_start.c
```c
sh1106_init();
```
sh1106_init 初始化包含了SPI0的初始化以及GPIO初始化。
```c
uint8_t sh1106_init(void)
{
    uint8_t err_code;

    err_code = hardware_init();
    if (err_code != 0) {
        return err_code;
    }

    command_list();

    return err_code;
}
```
#### 显示部分
代码位于solutions/eduk1_demo/k1_apps/homepage/homepage.c
以显示版本信息为例：
```c
OLED_Clear(); // 清屏函数
OLED_Show_String(40, (12 + 4) * 1, "HaaS EDU", 12, 1); / 将字符串填入显示缓存
snprintf(image_version, 21, "VER: %s", BUILD_VERSION); // 格式化字符串
OLED_Show_String(33, (12 + 4) * 2, image_version, 12, 1); // 将格式化后的字符串-
																													//版本信息填入缓存
OLED_Refresh_GRAM(); // 刷新显存到屏幕上
```


## ADC操作与电量获取
### 背景介绍
ADC即模拟数字转换器（英语：Analog-to-digital converter）是用于将模拟形式的连续信号转换为数字形式的离散信号的一类设备。与之相对的设备成为数字模拟转换器（DAC）。
典型的模拟数字转换器将模拟信号转换为表示一定比例电压值的数字信号。然而，有一些模拟数字转换器并非纯的电子设备，例如旋转编码器，也可以被视为模拟数字转换器。
### 硬件设计
原理图如下：

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01vwWwcp1qAtbVBGzet_!!6000000005456-2-tps-790-552.png" style="zoom:50%;" />
</div>


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01RhdcEn1GyykObx9kd_!!6000000000692-2-tps-1866-570.png" style="zoom:50%;" />
</div>
 
电压检测主要需要注意如下几点：
1、电压检测使用的是MCU的GADC1通道。
2、带USB供电时，检测点电压恒定为为4.8~5.2V之间，切换为电池供电时（断掉USB供电），电压浮动在3.65V~4.2V之间，此时电量标志才会有变化。
3、因为GADC量程有限，电压检测采用分频，如原理图所示，ADC实测值约为VOLT的1/3，加上ADC内阻的损耗，实际比值为3.208。

### 软件设计
#### 驱动初始化
在 AliOS Things 3.3中，对ADC的操作才用了VFS的方式，开发者只需要open相应的device设备，通过提供的到的IOCTL接口，完成start、get，stop等一系列动作。

```c
// open相应的adc设备
snprintf(name, sizeof(name), "/dev/adc%d", index);
fd = open(name, 0);
// start adc 设备
if (fd >= 0) {
        ret = ioctl(fd, IOC_ADC_START, sampling_cycle);
        usleep(1000);
        adc_arg.value = 0;
        adc_arg.timeout = 500000; // in unit of us
}
```

#### 获取电压值
1、读取十次，
2、去掉最大最小值，然后取平均。
```c
    for (int32_t i = 0; i < 10; i++) {
        ret = ioctl(fd, IOC_ADC_GET_VALUE, (unsigned long)&adc_arg);
        test_sum += adc_arg.value;

        /* the min sampling voltage */
        if (test_min >= adc_arg.value) {
            test_min = adc_arg.value;
        }
        /* the max sampling voltage */
        if (test_max <= adc_arg.value) {
            test_max = adc_arg.value;
        }
    }
    usleep(1000);
    ret = ioctl(fd, IOC_ADC_STOP, 0);
    close(fd);

    test_avrg = (test_sum - test_min - test_max) >> 3;
    LOGD(EDU_TAG, "the samping volage is:%dmv\n", test_avrg);
    test_avrg *= 3.208;
    *volage = test_avrg;
```
#### 返回电量等级
实际电压值为ADC*3.208，然后根据这个值返回不同的level，主要分为五档，分别是0%，25%，50
%，75%，100%。
```c

    if (test_avrg > 4100)
    {
        *level = 4;
    }
    else if ((test_avrg > 3980) && (test_avrg < 4100))
    {
        *level = 3;
    }
    else if ((test_avrg > 3850) && (test_avrg < 3980))
    {
        *level = 2;
    }
    else if ((test_avrg > 3700) && (test_avrg < 3850))
    {
        *level = 1;
    }
    else if (test_avrg < 3700)
    {
        *level = 0;
    }
```
看到代码很多人可能会有疑问，为什么4100毫伏到4200毫伏代表100%，4100毫伏到3980毫伏代表75%，并不是一个线性的值。其实原因很简单，锂电池的电池放电曲线不是一个线性的。


#### 显示图标
显示不同单色电池图标代码如下：
```c
if (0 == get_battery(&battery_level))
{
  //printf("get_battery success %d\n", battery_level);
  OLED_Icon_Draw(110, 0, &icon_battery_20_12[battery_level], 0);
}

```


## 系统Wi-Fi以及网络对时
### 背景介绍

#### Wi-Fi
Wi-Fi这个名词想必大家都不陌生，就是联网的代名词。简单来说Wi-Fi(WirelessFidelity)是一种网络传输标准，与蓝牙技术一样，它同属于短距离无线技术。随着网络的应用普及，它给人们带来极大的方便，也因此得到了广泛应用，Wi-Fi让我们随时随地、的上网成为可能。
经常在路由器的包装中看到802.11b/g/n 2.4Ghz这类字样，那么这个是什么来的？这个是无线网络通信的标准，IEEE 802.11是现今无线局域网通用的标准，它是由电气和电子工程师协会（IEEE）所定义的。后面的这些后缀是协议迭代的版本号。

#### 2.4GHz频段

1. 802.11b/g/n中桂东 Wi-Fi无线电可以在2.4 GHz频带中传输，共有14个可用信道，工作频率范围是2.402GHz - 2.483Ghz。中国可用13个信道（1-13）。每相邻的2个信道之间的频宽就是5Mhz。如果使用信道1，频宽为20Mhz,则信道2,3,4,5都被占用。

2. 每个信道的带宽为22MHz，不过有效带宽只有20MHz，为了减少相邻信道的干扰，两侧预留1MHz的带宽边界。
3. 只有三个频道（1、6和11）不共享频率空间。
#### 5GHz频段


1. 802.11a/n/ac中规定Wi-Fi无线电可以在5GHz频带中传输，共有25个可用信道。中心频率范围5.150GHz-5.850GMz，中国可以使用的信道有低信道36,40, 44, 48, 52, 56, 60, 64, 高信道149,153,157, 161, 165。信道之间没有重叠。
1. 每个频宽为20MHz，如果使用信道149，当要用80Mhz时，则153,157,161都要被占用。



#### 系统时钟获取（SNTP网络对时）
在界面左上角显示的就是当前的系统时间，HaaS EDU K1本身没有带RTC芯片，所以不能自动保存时间。
那么如何获取准确的时间呢？这里就需要用到SNTP协议，SNTP是基于NTP协议的。
#### SNTP协议
简单网络时间协议（Simple Network Time Protocol），由NTP改编而来，主要用来同步因特网中的计算机时钟。在 RFC2030 中定义。
SNTP协议采用客户端/服务器的工作方式，可以采用单播（点对点）或者广播（一点对多点）模式操作。SNTP服务器通过接收GPS信号或自带的原子钟作为系统的时间基准。单播模式下，SNTP客户端能够通过定期访问SNTP服务器获得准确的时间信息，用于调整客户端自身所在系统的时间，达到同步时间的目的。广播模式下，SNTP服务器周期性地发送消息给指定的IP单播地址或者IP多播地址。SNTP客户端通过监听这些地址来获得时间信息。
### 硬件设计
本实验所用到的Wi-Fiy硬件模块已经包含在MCU之中，不需要额外提供。
### 软件设计
#### Wi-Fi模块
AliOS Things中关于Wi-Fi部分也已经封装成了netmgr组件，eduk1_demo的package.yaml已经默认包含。
```c
depends:
  - netmgr: dev_aos
```
代码位于solutions/eduk1_demo/app_start.c
```c
event_service_init(NULL);

netmgr_service_init(NULL);
event_subscribe(EVENT_NETMGR_DHCP_SUCCESS, wifi_event_cb, NULL);
```
event_service_init(NULL)， event_service 初始化。
netmgr_service_init netmgr服务初始化,主要包含CLI的注册，WIFi设备创建，相关信号量以及task的初始化，Wi-Fi硬件初始化等。
event_subscribe(EVENT_NETMGR_DHCP_SUCCESS, wifi_event_cb, NULL);这里则是注册了wifi_event的回调函数。
#### 使用Wi-Fi
当前可以通过代码配置或者命令行的形式配置将WI-FI名称（SSID）以及密码配置到EDU中。

填入SSID以及密码
```c
netmgr -t wifi -c haas-open 12345678
```

手动保存ssid和password到文件系统中。
```c
netmgr -t wifi -b 1
```

重启之后，手动重新连接。
```c
netmgr -t wifi -a 1
```
#### 网络对时（SNTP）
edu在连接网络之后，就会自动获取网络对时。并更新到本地时钟。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01T0ht7Z21GM6JjwbrD_!!6000000006957-2-tps-2702-222.png" style="zoom:50%;" />
</div>

此时如果需要获取系统时间，直接调用系统函数clock_gettime即可。
```basic
struct tm *info;
struct timespec tv;

/* 获取 GMT 时间 */
clock_gettime(CLOCK_REALTIME, &tv);
info = gmtime(&tv);

snprintf(tmp, 21, "%2d:%02d", (info->tm_hour + 8) % 24, info->tm_min);
OLED_Show_String(0, 12 * 0, tmp, 12, 1);
```



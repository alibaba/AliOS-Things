@page rfid_demo HaaS100实现RFID读卡器信息上报云端示例

[更正文档](https://gitee.com/alios-things/rfid_demo/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 1. 案例简介


无线射频识别即射频识别技术（Radio Frequency Identification，RFID）是自动识别技术的一种，通过无线射频方式进行非接触双向数据通信，利用无线射频方式对电子标签或射频卡进行读写，从而达到识别目标和数据交换的目的。


RFID技术具有抗干扰性强以及无需人工识别的特点，所以常常被应用在一些需要采集信息的领域上,例如物流，仓储，防伪，身份识别等领域。


接下来我们学习下怎么基于HaaS100 搭建RFID读卡器，读取卡片信息，并且上传到阿里云IOT平台上。

<img src="https://img.alicdn.com/imgextra/i2/O1CN012j7oiV1PpfERjfefk_!!6000000001890-2-tps-349-401.png" style="max-width:800px;" />

实验效果展示：

<img src="https://img.alicdn.com/imgextra/i4/O1CN01duVVEO1J7LCrbrpHm_!!6000000000981-2-tps-1704-860.png" style="max-width:800px;" />

# 2. 基础知识
RFID射频识别系统的工作原理:
电子标签进入天线磁场后，若接收到读写器发出的特殊射频信号，就能凭借感应电流所获得的能量发送出存储在芯片中的产品信息(无源标签)，或者主动发送某一频率的信号(有源标签)，读写器读取信息并解码后，送至中央信息系统进行有关数据处理。

典型的RFID系统主要由应用系统软件，RFID读写器，电子标签3部分构成。

<img src="https://img.alicdn.com/imgextra/i1/O1CN01dQNJ2x24FO6j8gfXW_!!6000000007361-2-tps-462-280.png" style="max-width:800px;" />

# 3. 物料清单

## 3.1 HaaS100 硬件

[HaaS100 硬件简介](https://help.aliyun.com/document_detail/184426.html)

<img src="https://img.alicdn.com/imgextra/i4/O1CN01XxD6Xo217CB3FZnEU_!!6000000006937-2-tps-746-497.png" style="max-width:800px;" />

## 3.2 RFID读写器

<img src="https://img.alicdn.com/imgextra/i4/O1CN015zTuPa1ipduhDVCPY_!!6000000004462-2-tps-720-364.png" style="max-width:800px;" />

TX522 是一款基于13.56MHz频率的Mifare卡读写模块，可以支持多种类型卡，TX522 Mifare卡读写模块具有天线一体化，识别距离达到80mm。可以通过UART接口跟HaaS100进行通信。


刷卡：
当检测到 RFID卡片靠近的时候；红色指示灯闪烁，并通过串口发送出卡片序列号相关的指令.


指令协议说明
数据通信以一帧为单位进行，格式如下：
数据通信帧结构:


第1字节：起始符；
第2字节：包号，默认：0x00；
第3字节：命令—上位机发送给IC卡读写器；
第4字节：此帧有效数据的长度。从紧接着的第1个字节开始，到倒数第3个字节结束，为有效字节。
第5字节 到  倒数第3字节： 有效数据。
倒数第2字节：校验和；
倒数第1字节：结束符；


刷卡。模块上的指示灯会亮，并且通过串口主动发送卡片序列号给上位机，此时上位机就会接收到卡片序列号的相关指令。指令：20 00 01 08 04 00 00 00 A6 40 FE E4 0E 03。
20：起始符
00：包号
01：命令字节，模块主动输出卡片序列号时，该字节为0x00；其他指令为命令字
08：表示后面8个字节为有效数据位
04 00：表示卡片属性为S50卡
00 00：此2个字节无实际意义。
A6 40 FE E4 ：表示卡片序列号。刷不同卡片，此4个字节会变。
0E：校验和。从包号（SEQNR）开始到数据（DATA）的最后一字节异或，然后再取反 得到。
03：帧结束符。

卡片序列号是我们关注的重要信息，我们通过串口读取卡片序列号信息后将其发送给阿里云IOT平台。



# 4. 案例实现

## 4.1 硬件连接


RFID模块通过UART接口跟HaaS100进行通信。


这个是HaaS100 硬件接口定义：

<img src="https://img.alicdn.com/imgextra/i1/O1CN01rvpScH1kBSOWmRDhj_!!6000000004645-2-tps-1066-1422.png" style="max-width:800px;" />

红框部分是串口2的引脚。


将RFID模块电源线，地线，串口RX，串口TX  4根线接到HaaS板子串口2上。



| HaaS板引脚编号 | HaaS板引脚说明 | RFID模块 |
| -------------- | -------------- | -------- |
| 5              | 5V             | VCC      |
| 3              | GND            | GND      |
| 10             | UART2_RXD      | TX       |
| 12             | UART2_TXD      | RX       |



<img src="https://img.alicdn.com/imgextra/i4/O1CN01n9cATi1c9brz4RRVl_!!6000000003558-0-tps-2048-1536.jpg" style="max-width:800px;" />


## 4.2 接入阿里云IOT平台

首先登陆[阿里云IOT平台](https://iot.console.aliyun.com/product)。

选择产品标签页，点击创建产品按钮，新建产品，填写产品信息。

<img src="https://img.alicdn.com/imgextra/i3/O1CN01DAuLhk1fEb1qOeI7n_!!6000000003975-2-tps-976-1406.png" style="max-width:800px;" />

选择添加功能

<img src="https://img.alicdn.com/imgextra/i2/O1CN01MAjlRL1Cyk4t3RcMC_!!6000000000150-2-tps-2138-1264.png" style="max-width:800px;" />

点击编辑草稿按钮，编辑自定义功能，记录下标识符信息后面代码中会用到。

<img src="https://img.alicdn.com/imgextra/i4/O1CN01ZGA4bi1T1yjDtlKTV_!!6000000002323-2-tps-940-1370.png" style="max-width:800px;" />

编辑完成后点击左下角发布上线。

<img src="https://img.alicdn.com/imgextra/i1/O1CN01T8sQFc1Wggmc5YR0C_!!6000000002818-0-tps-1513-830.jpg" style="max-width:800px;" />

开始添加设备，选择设备标签页，点击添加设备按钮。
<img src="https://img.alicdn.com/imgextra/i2/O1CN01hrEJd92431PsCejIV_!!6000000007334-2-tps-910-802.png" alt="add.png" style="max-width:800px;" />

查看设备三元组信息：
记录下设备的三元组信息： product key/device name/device secret ，这些信息后面要填写到代码中。

<img src="https://img.alicdn.com/imgextra/i1/O1CN01KFOGt21DY0ZfJh7f6_!!6000000000227-2-tps-1684-970.png" style="max-width:800px;" />

## 4.3 软件实现
接下来我们开始编写HaaS 上的软件代码，读取串口数据，从中提取出卡片序列号,通过linkkit SDK接口将序列号发送给阿里云IOT平台。


### 4.3.1 软件流程图
RFID相关代码在solutions/rfid_demo目录下。
<img src="https://img.alicdn.com/imgextra/i1/O1CN01SxtDtq1Cv4uV1PQdO_!!6000000000142-2-tps-638-1110.png" alt="image.png" style="max-width:800px;" />

### 4.3.2 HaaS100 串口通信部分

UART串口操作代码可以参考这个文件：solutions/rfid_demo/rfid_app.c
UART串口操作相关代码：

```c
int rfid_uart_init(void)
{
    int port_id = 2;
    int ret = 0;
    char dev_name[16] = {0};

    snprintf(dev_name, sizeof(dev_name), "/dev/ttyUART%d", port_id);
    fd = open(dev_name, 0);
    if(ret != 0) {
        printf("open uart error\r\n");
        return ret;
    }
    ret = ioctl(fd, IOC_UART_SET_CFLAG, B9600 | CS8);
    if(ret != 0) {
        close(fd);
        printf("ioctl uart error\r\n");
        return ret;
    }
}
static void task_recvdata_entry(void *arg)
{
    int  i = 0;
    int  ret = 0;
    char rfid_data_buf[50] = {0};
    int  rev_length = 0;
    char params[30];
    char rfid_deviceid[12];

    while (1) {
        ret = read(fd, rfid_data_buf, sizeof(rfid_data_buf));
        if (ret > 0) {
            printf("read length:%d\r\n", ret);
            for (i = 0; i < ret; i++) {
                printf("%02x ", rfid_data_buf[i]);
            }
         }
}
```


### 4.3.3 往阿里云IOT平台发送数据


我们使用send_property_post接口将读取的卡片序列号DeviceID发送给阿里云IOT平台。


```c
//card_id 这个字符串是之前在阿里云IOT平台添加自定义功能时的标志符。
//rfid_deviceid 里存储的是通过串口读到的RFID序列号。

snprintf(params, sizeof(params), "{\"card_id\": \"%s\"}", rfid_deviceid);
send_property_post(params);
```


## 4.4 端云联调
### 4.4.1 下载软件版本

开发环境的搭建请参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，其中详细的介绍了AliOS Things 3.3的IDE集成开发环境的搭建流程。

RFID的代码下载请参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)，

*> 选择解决方案: “rfid使用示例”或者“rfid demo”*

*> 选择开发板: Haas100 board configure*

源码下载完成后将在阿里云IOT平台获取的设备信息填写到文件solutions//rfid_demo/rfid_demo.c中。

```c
    char *product_key       = "";
    char *device_name       = "";
    char *device_secret     = "";
```

-- 编译固件可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

-- 烧录固件可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)。

### 4.4.2 设备配网
HaaS启动后通过串口命令行配网，输入如下指令可完成配网：
```
netmgr -t wifi -c {ssid} {password}
```
ssid, password 替换成你自己的网络热点信息。

### 4.4.3 演示效果图片
当有RFID卡片靠近读卡器时，我们会在串口log里看到RFID卡片数据信息：

<img src="https://img.alicdn.com/imgextra/i1/O1CN01c8wrDo1YXBnUbLawp_!!6000000003068-2-tps-632-136.png" style="max-width:800px;" />

在IOT平台上可以看到卡片的序列号。

<img src="https://img.alicdn.com/imgextra/i1/O1CN01DXv7B91z2xHBbdeNm_!!6000000006657-2-tps-1724-964.png" style="max-width:800px;" />


# 5. 总结

本文档介绍了如何基于HaaS100 搭建RFID读卡器，读取卡片信息，并且上传到阿里云IOT平台上，其中涉及到了HaaS 串口操作和给阿里云IOT发送数据。
RFID模块除了可以读卡上的数据外还可以给卡写入数据，还有更多的功能大家可以一起挖掘和探索。
HaaS除了串口外还可以通过i2c,spi 等接口链接外面的传感器或者其他设备。
通过本文档介绍的这个流程，大家可以打造更多的应用场景，一起行动起来，打造一套属于自己的智能设备吧。

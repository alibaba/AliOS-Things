- 开发板配置
- 代码下载
- 代码移植
- 功能调试
- 驱动代码提交

## 1 开发板配置
我们验证选用的开发板是基于STM32L496VGTx芯片研发的一款物联网开发板。其内核为ARM 32位Cortex-M4 CPU，最高80MHZ的主频率，1MB的闪存，320KB的SRAM，最多支持136个高速IO口，还支持SPI,CAN,I2C,I2S,USB,UART等常用的外设接口。

***调试时，请使用右上角的USB1 ST_Link接口***

![](https://i.imgur.com/ColtF57.png)

单板的背面有arduino接口，当前验证使用的外接sensor主要基于I2C总线进行连接。
SCL以及SDA的高电平为3V

![](https://i.imgur.com/nfThGYH.png)

developer kit开发板环境配置请参考以下链接，建议采用一键式安装：

https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Studio

## 2 代码下载
代码下载前，请确认已在github注册账号，链接及注册流程如下：
- https://github.com/

**为了便于后续的代码的审核提交，注册github账号时请使用本公司的邮箱**
![](https://i.imgur.com/q0JhzHJ.png)

打开以下代码链接后，可以通过以下方式下载代码。首先选择代码分支；
- https://github.com/andy2012zwj/AliOS-Things/tree/aos_udata

![](https://i.imgur.com/7LjS6d4.png)

然后选择zip格式下载；
![](https://i.imgur.com/AoAtQN8.png)

安装了git bash的同学也可以通过命令行下载：git clone git@github.com:andy2012zwj/AliOS-Things.git
## 3 代码移植
### 3.1 驱动代码集成
请参考以下链接完成uData架构下传感器驱动的移植：https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-uData-Sensor-Driver-Porting-Guide.zh
### 3.2 总线配置
在developer kit板上，我们是通过外接I2C3连接传感器，需要注意的地方是总线配置,port口为3，从设备的地址为8bit：
```
i2c_dev_t  ####_ctx = {
    .port = 3, /*developer kit上外接I2C的port为3*/
    .config.dev_addr = 0x5D<<1, /* 从设备I2C地址，8bit */
};

```
### 3.3 服务订阅
如果需要在串口查看调试信息，则需要在udata_sample函数中，修改函数udata_sample中的订阅的传感器service类型（路径：example\uDataapp\uData-example.c）；压力传感器如下所示：
```
int udata_sample(void)
{
    int ret = 0;

    aos_register_event_filter(EV_UDATA, uData_report_demo, NULL);

    ret = uData_subscribe(UDATA_SERVICE_BARO);/*UDATA_SERVICE_BARO为压力传感器对应的service 类型*/
    if (ret != 0) {
        LOG("%s %s %s %d\n", uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }

    return 0;
}

```

```
/*service 类型*/
typedef enum 
{
 UDATA_SERVICE_ACC = 0,     /* Accelerometer */ 
 UDATA_SERVICE_MAG,         /* Magnetometer */
 UDATA_SERVICE_GYRO,        /* Gyroscope */
 UDATA_SERVICE_ALS,         /* Ambient light sensor */
 UDATA_SERVICE_PS,          /* Proximity */
 UDATA_SERVICE_BARO,        /* Barometer */
 UDATA_SERVICE_TEMP,        /* Temperature  */
 UDATA_SERVICE_UV,          /* Ultraviolet */
 UDATA_SERVICE_HUMI,        /* Humidity */
 UDATA_SERVICE_HALL,        /* HALL sensor */
 UDATA_SERVICE_HR,          /* Heart Rate sensor */
 UDATA_SERVICE_PEDOMETER,   
 UDATA_SERVICE_PDR,     
 UDATA_SERVICE_VDR,
 UDATA_SERVICE_GPS,
 
 UDATA_MAX_CNT, 
}udata_type_e;
```

## 4 功能调试
下面以developer kit板为例说明linkkit用例的调试过程。
#### 4.1 编译
example\uDataapp目录下已集成了相关的用例代码，2、3两个章节完成配置修改后，执行以下命令则可以编译用例
aos make udataapp@developerkit
![](https://i.imgur.com/6CwKRXU.png)
编译完成后，生成的可执行文件为out\udataapp@developerkit\binary\udataapp@developerkit.bin
![](https://i.imgur.com/1k2DCSk.png)
#### 4.2 文件烧录
本示例采用ST-LINK工具烧写bin文件，用户也可参考developer kit板环境配置说明中的其他方法；
![](https://i.imgur.com/gB0snoy.png)
#### 4.3 用例执行
通过串口连接单板（串口速率为115200）
烧录完成后，复位单板，开始运行；如果配置流程没有错误，则可以在串口看到sensor通过udata上报的数据。

![](https://i.imgur.com/sXXs2SJ.png)
其中物理传感器对应的服务类型，请参考结构体udata_type_e；

物理传感器的上报的数据单位，请参考以下链接中的《传感器数据单位》章节

https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-uData-Sensor-Driver-Porting-Guide.zh

## 5 驱动代码提交
如果功能测试完成无误，则可以参考以下链接中外部代码提交方式，向AliOS Things提交代码和入申请。

***由于当前传感器集成是使用的稳定的分支代码，而上传代码则需要到AliOS-Things的主干中***，主干代码链接：https://github.com/alibaba/AliOS-Things

代码上传之前请确保已通过公司注册github账号。并完成SSH key认证，已完成则跳过5.1节。

#### 5.1 SSH key 认证
在git bash下执行以下命令

ssh-keygen -t rsa -C "example_mail@alibaba.com"

example_mail@alibaba.com为自己注册github使用的公司邮箱

打开生成的SSH key文件id_rsa.pub，可以看到SSH key值如下所示

![](https://i.imgur.com/B4mGWPE.png)

在github网站上添加该key值

![](https://i.imgur.com/hHPQQAA.png)

![](https://i.imgur.com/fH3NErZ.png)

添加成功后如下所示：

![](https://i.imgur.com/XHizOWp.png)

#### 5.2 fork Alios-Things到自己名下

![](https://i.imgur.com/Q6dMP2H.png)

fork成功后，在可以在自己名下看到AliOS-Things的代码

![](https://i.imgur.com/gdTT7or.png)

#### 5.3 代码修改下载
git邮箱修改：
git config user.email example_mail@alibaba.com

example_mail@alibaba.com -- github提交时显示的本公司邮箱

git用户名修改：
git config user.name xxx

xxx -- github提交时显示的用户名

在git bash下通过以下命令下载自己分支的代码：

git clone git@github.com:***yourname***/AliOS-Things.git

yourname -- 即自己github账号的名字

![](https://i.imgur.com/RgeFnnB.png)

#### 5.4 代码上传
每次代码上传之前，请同步AliOS-Things上master代码到自己名下的master

然后和入代码修改，和入的代码主要包括以下四个部分：
1. 新增的sensor驱动文件
2. sensor.mk中增加该驱动源文件
3. sensor.mk中增加相应的编译宏（***并注释掉，待用户使用时自行开启***）
![](https://i.imgur.com/em98eh1.png)
4. 在sensor_init函数中调用相应的sensor初始化接口（***并用编译宏隔离***）
![](https://i.imgur.com/cZcrBsJ.png)

修改完成后，通过以下命令上传代码到自己名下的master分支

查看修改后的文件： git status

添加要修改的文件信息到索引库： git add .

添加文件修改到本地库，并添加log： git commit -m "add driver for sensor"

将修改推送到远程分支： git push origin master

#### 5.5 提交pull request

在自己的代码路径下，点击“pull request”，待CI运行成功后，由AliOS-Things团队和入到主干分支
![](https://i.imgur.com/lr8u3Sk.png)
上传代码流程也可参考该链接：
https://github.com/alibaba/AliOS-Things/wiki/contributing.zh


## 6 自测流程

由于当前认证的用例带有检测仪器，不适合开发者验证。因此建议从以下几个方面实现自检自测：

1. 文件命名、函数命名、实现符合AliOS-Things下sensor驱动编程规范；

   https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Coding-Style-Guide

2. 需要保证sensor可以成功初始化，uData上层服务可以能够正确读出数据，且数据单位与uData要求保持一致（如果有不合理的地方请提出）；

   如果有条件，可以验证一下精度，在5%以内；

3. 对于带有self-test功能的sensor，建议实现self-test接口；
   可参考函数drv_acc_bosch_bma253_self_test（代码目录：AliOS-Things-master\device\sensor\drv\drv_acc_bosch_bma253.c）

   代码链接： https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Coding-Style-Guide


```
/*
drv   -- 类型： i2c_dev_t*   说明： 驱动参数；
data  -- 类型： int32_t*     说明： self-test测试返回的offset值，可参考结构体dev_sensor_info_t；
返回值 -- 成功返回0；失败返回-1；
*/

static int drv_acc_bosch_bma253_self_test(i2c_dev_t* drv,int32_t* data)；

/*并在相应sensor的ioctl函数中调用该接口*/
static int drv_acc_bosch_bma253_ioctl(int cmd, unsigned long arg)
{
    int ret = 0;
    dev_sensor_info_t *info = (dev_sensor_info_t *)arg;
    switch(cmd){
        
        case SENSOR_IOCTL_SELF_TEST:{
            ret = drv_acc_bosch_bma253_self_test(&bma253_ctx, info->data);
            printf("%d   %d   %d\n",info->data[0],info->data[1],info->data[2]);
            return ret;
        }

       default:break;
    }

    return 0;
}

```


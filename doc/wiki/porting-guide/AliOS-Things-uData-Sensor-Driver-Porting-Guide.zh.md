[EN](AliOS-Things-uData-Sensor-Driver-Porting-Guide)| 中文

- uData框架介绍
- uData传感器驱动开发概要
- 驱动层的目录结构
- 驱动程序命名格式
- 驱动程序实现
- IO总线配置
- 接口函数实现
- 设备初始化init函数实现
- 编译配置
- 传感器数据单位

### uData框架简介
uData框架设计之初的思想是基于传统sensorhub概念基础之上的，结合IoT的业务场景和AliOS Things物联网操作系统的特点设计而成的一个面对IoT的感知设备处理框架。uData主要分kernel和framework两层，kernel层主要是负责传感器驱动和相关的静态校准，包括轴向校准等；framework层主要是负责应用服务管理、动态校准管理和对外模块接口等。有关uData的具体设计将不再此文档中展开，下图可作为一个概要参考。本文档主要是介绍传感器驱动程序的开发。
![](https://i.imgur.com/iSaUEYR.png)

### uData传感器驱动开发概要
在本章节中，主要是介绍如何实现基于uData框架的传感器驱动开发。在uData设计之初，为了能更好的提供每个开发者极简开发，已将驱动相关的OS调用，比如中断注册接口，设备硬件配置接口和静态校准配置接口（目前暂时不支持)等都抽象到了sensor hal实现。每一个设备驱动程序，只需一个.c文件中，通过BUS总线如I2C，对设备进行读写和配置操作，就能实现全部的功能，同时也减少了开发者调试工作时间。

### 驱动层的目录结构

![](http://o7spigzvd.bkt.clouddn.com/udata_location.jpg)

#### 驱动程序命名格式
首先，为了代码的可读性，对文件命名和函数命名，都提供了一个很好的范例。
**文件命名格式：drv\_####\_####\_####.c** 

	参数1（设备类型）： 比如accel, gyro, mag, als, rgb, proximity, humi, temp, uv, baro等等。
	参数2（设备厂商）： 比如bosch等等
	参数3（设备型号）： 比如bma253,bme280等等。

如果你是在开发一个Bosch厂商的型号为bme280的湿度计计（humi）驱动，那可以文件命名为：drv\_humi\_bosch\_bme280.c
**函数命名格式：drv\_####\_####\_####\_####(...)**

	参数1（设备类型）： 比如humi等等。
	参数2（设备厂商）： 比如bosch等等
	参数3（设备型号）： 比如bme280等等。
	参数4（功能描述）： 比如setpowr，init等等。
例如，你是在为一个Bosch厂商的型号为bme280的湿度计（humi）驱动开发初始化函数，那可以文件命名为：drv\_humi_bosch\_bme280\_init（void）
####驱动程序实现
每一个传感器驱动程序都是以一个sensor\_obj_t对象来实现的，也就是说需要对这个对象的每个member进行实现。整个驱动程序主要分为三部分：IO总线配置，接口函数实现，初始化函数和编译配置。
```
struct _sensor_obj_t {
	char*                    path;                     /* 设备节点路径 */
	sensor_tag_e             tag;                      /* 设备类型 */
	dev_io_port_e            io_port;                  /* 使用的IO总线类型 */
	work_mode_e              mode;                     /* 设备的工作模式 */
	dev_power_mode_e         power;                    /* 设备电源状态 */
	gpio_dev_t               gpio;                     /* 中断工作模式下的中断配置信息 */
	dev_sensor_full_info_t   info;                     /* 设备信息 */
	int (*open)(void);                                 /* 接口函数：打开设备，暂时只需power on即可 */
	int (*close)(void);                                /* 接口函数：关闭设备，暂时只需power off即可 */
	int (*read)(void *, size_t);                       /* 接口函数：读设备数据操作 */
	int (*write)(const void *buf, size_t len);         /* 接口函数：写设备操作，暂不使用 */
	int (*ioctl)(int cmd, unsigned long arg);          /* 接口函数：ioctl配置 */
	void(*irq_handle)(void);                           /* 接口函数：中断服务程序，在中断模式下才需要 */
}
```

##### IO总线配置
定义一个的i2c\-dev\_t的全局变量，并配置其中的设备I2C地址，参考如下：
<pre><code>i2c_dev_t  ####_ctx = {
    .config.dev_addr = 0x5D, /* 从设备I2C地址 */
};</code></pre>
#### 接口函数实现
所有的外设驱动都是以vfs形式来读写操作的，所以每一个驱动按需实现必要的接口函数： 
<pre><code>int (*open)(void); 
int (*close)(void);
int (*read)(void *, size_t);
int (*write)(const void *buf, size_t len);
int (*ioctl)(int cmd, unsigned long arg);
void(*irq_handle)(void);</code></pre>
#### 设备初始化init函数实现
主要是实现对sensor\_ojb\_t的初始化设置并把设备驱动注册到sensor hal层统一调度管理；另外需要在初始化函数中实现对设备的默认参数配置，身份确定（validata id）并让设备进入低功耗模式。具体请参考如下实例：
```
int drv_humi_bosch_bme280_init(void){
    int ret = 0;
    sensor_obj_t sensor;
    /* fill the sensor obj parameters here */
    sensor.tag = TAG_DEV_HUMI;                   /* 传感器类型 */
    sensor.path = dev_humi_path;                 /* 打开设备路径 */
    sensor.io_port = I2C_PORT;                   /* 当前使用的总线端口类型 */
    sensor.open = drv_humi_bosch_bme280_open;    /* 打开设备接口函数 */
    sensor.close = drv_humi_bosch_bme280_close;  /* 关闭设备接口函数 */
    sensor.read = drv_humi_bosch_bme280_read;    /* 读设备数据接口函数 */
    sensor.write = NULL;                         /* 暂不需要 */
    sensor.ioctl = drv_humi_bosch_bme280_ioctl;  /* 配置设备接口函数 */
    sensor.irq_handle = NULL;                    /* 暂不需要，如中断模式必须注册 */

    /* 把设备驱动信息注册到sensor hal，并创建一个创建分配一个设备节点给此设备 */
    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
       return -1;
    }
	
    /* 验证设备ID，并非一定需要此功能 */
    ret = drv_humi_bosch_bme280_validate_id(&bme280_ctx, WHO_AM_I_REG, WHO_AM_I_VAL);
    if(unlikely(ret)){
       return -1;
    }

    /* 设定默认配置参数，实时通过ioctl可以调节配置 */
    ret = drv_humi_bosch_bme280_set_default_config(&bme280_ctx);
    if(unlikely(ret)){
       return -1;
    }
    return 0;
}
```
#### 编译配置
##### 1.在sensor.mk文件中添加所要编译的驱动代码并定义其相关的宏定义。比如，需要为BOSCH BME280湿度计添加编译配置：

    $(NAME)\_SOURCES += \  

        drv_sensor_sample.c \
        sensor_hal.c \
        sensor_drv_api.c \
        sensor_hw_config.c \
        sensor_static_cali.c \
        drv_humi_bosch_bme280.c   /* 请在最末处添加新的驱动代码 */
    
    GLOBAL_INCLUDES += .  
    
    GLOBAL_DEFINES      += AOS_SENSOR  
    
    GLOBAL_DEFINES      += AOS_SENSOR_HUMI_BOSCH_BME280 /* 请为新的驱动程序定义一个宏，在编译配置时将会使用到 */
##### 2.把所需要编译驱动的init函数添加到sensor\_hal.c的sensor\_init（）函数中，可参考如下：

```
int sensor_init(void){
int ret   = 0;
int index = 0; 
g_sensor_cnt = 0 ;

/* 请参考此格式添加init函数，初始化成功代表驱动已注册到sensor hal里了 */
#ifdef AOS_SENSOR_HUMI_BOSCH_BME280    
drv_humi_bosch_bme280_init();
#endif /* AOS_SENSOR_HUMI_BOSCH_BME280 */

ret = sensor_hal_register();
if(ret != 0){
    return -1;
}

return 0;
}
```
#### 传感器数据单位
主要是介绍uData相关的传感器数据的单位规范说明，从而避免所有开发者所提供的驱动代码上的传感器数据单位不一致，导致数据上云，算法等无法正常运算。考虑到不是所有的平台都支持浮点运算，也考虑到大数据的浮点运算对端侧系统带来的性能负担，约定基于uData的传感器数据运算均以整型数据计算。本数据单位标准规范当前仅限于物理传感器驱动数据和应用算法之间的范畴，呈现给最终用户的单位格式可以由最终产品所规定的单位格式而定。

| 传感器名称 |      简称       |  单位   | 传感器类别  |                                说明 |
| ----- | :-----------: | :---: | :----: | --------------------------------: |
| 加速度计  |      acc      |  mg   | 运动类传感器 | 1 g = 1000 mg, 1 g = 9.8 m/s2（米每秒） |
| 陀螺仪   |     gyro      | udps  | 运动类传感器 |   1 dps = 1000 000 udps, dps(度每秒) |
| 磁力计   |      mag      |  mG   | 位置类传感器 |                1 G = 1000 mG(毫高斯) |
| 接近光   |   proximity   |  cm   | 位置类传感器 |                 表示物体到传感器的距离，单位：厘米 |
| 光感计   |      als      |  lux  | 环境类传感器 |                             亮度流明值 |
| 气压计   | baro/pressure |  Pa   | 环境类传感器 |                100 Pa = 1 hPa（百帕） |
| 温度计   |     temp      | °c/10 | 环境类传感器 |                            0.1摄氏度 |
| 湿度计   |     humi      |   ‰   | 环境类传感器 |                   相对湿度%RH，一般只用‰表示 |
| 心率计   |      hr       |  bpm  | 健康类传感器 |                          bpm：次数每秒 |

##### 注：后期会迭代增加新的传感器数据单位。

#### AliOS Things支持的传感器列表
当前AliOS Things在uData框架下已支持以下传感器，随着生态的拓展，更多类型、更多厂家的传感器会持续集成进来。

| 传感器型号   | 传感器类型      | 厂家     |
| :------ | :--------- | :----- |
| BMA253  | 加速度传感器     | BOSCH  |
| BMP280  | 压力传感器      | BOSCH  |
| BMG160  | 陀螺仪传感器     | BOSCH  |
| BME280  | 湿度传感器      | BOSCH  |
| LPS22HB | 压力传感器      | ST     |
| LSM6DSL | 加速度、陀螺仪传感器 | ST     |
| LTR553  | 环境光、光接近传感器 | LITEON |
| DA217   | 加速度传感器     | 明皜科技   |

至此，uData软件框架的驱动程序开发教程已介绍完毕。
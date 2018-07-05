EN| [中文](AliOS-Things-uData-Sensor-Driver-Porting-Guide.zh)

- Introduction of uData framework

- Overview of uData sensor driver development
- Directory structure in driver layer
- Driver naming format
- Implement of drivers
- IO bus configuration
- Implement of interface function
- implement of device initiation function
- Compilation configuration 
- Sensor data unit

### Introduction of uData framework

uData is designed based on traditional sensorhub. It is an IoT oriented sensoring device processing framework integrating both characteristics of actual IoT application scenarios and AliOS Things itself. 

There are two layers in uData Framework. One is kernel layer, mainly responsible for sensor driving and related static calibration, including axial calibration. The other is framework layer, mainly responsible for application service management, dynamic calibration management and external module interfaces. The specific design of uData will not be unfolded in this document, but the following diagram can be used as a summary reference. This article will mainly introduce the development of sensor driver.

![](https://i.imgur.com/iSaUEYR.png)

### Overview of uData sensor driver development

This section will introduce the development of sensor drivers based on the uData. For the convenience of development, uData has abstracted OS calls related to drivers, such as registration interface interruption, hardware configuration interface and static calibration configuration interface (temporarily unsupported), to sensor HAL. For device driver, only one .c file is needed, through BUS line such as I2C, to read, write and configure the device, and realize all functions. 

### Directory structure in sensor layer

![](http://o7spigzvd.bkt.clouddn.com/udata_location.jpg)

#### Driver naming format

For the readability of code, we recommend you to name files as **：drv\_####\_####\_####.c** 

```
parameter 1（device tyoe）： such as accel, gyro, mag, als, rgb, proximity, humi, temp, uv, baro
parameter 2（device vendor）： such as bosch
parameter 3（device type）： such as bma253 and bme280
```

If you are developing a Bosch's humidometer driver with type of bme280, you can name the file as drv\_humi\_bosch\_bme280.c.

Similarly, we recommend you to name functions as **：drv\_####\_####\_####\_####(...)**

```
parameter 1（device type）： such as humi
parameter 2（device vendor）： such as bosch
parameter 3（device type）： such as bme280
parameter 4（function description）： such as setpowr，init
```

If you are developing the initiation function for a Bosch's humidometer driver with type of bme280, you can name the  function as drv\_humi_bosch\_bme280\_init (void).

#### Implement of drivers

Each sensor driver is implemented by a sensor\_obj_t member. In another words, each member of that driver needs to be implemented. The whole driver can be divided as three parts : IO bus configuration, interface function implementation, initialization function and compilation configuration.

```
struct _sensor_obj_t {
	char*                    path;                     /* device node path */
	sensor_tag_e             tag;                      /* device type */
	dev_io_port_e            io_port;                  /* IO bus type */
	work_mode_e              mode;                     /* work mode */
	dev_power_mode_e         power;                    /* power mode */
	gpio_dev_t               gpio;                     /* configuration information in interruption mode */
	dev_sensor_full_info_t   info;                     /* device information */
	i2c_dev_t*               bus;                      /* I2C bus information, such as I2C address of slave device */
	int (*open)(void);                                 /* interface function: power on to open the device */
	int (*close)(void);                                /* interface function: power off to close the device */
	int (*read)(void *, size_t);                       /* interface function: read device data */
	int (*write)(const void *buf, size_t len);         /* interface function: write device data, currently not used */
	int (*ioctl)(int cmd, unsigned long arg);          /* interface function：ioctl configuration */
	void(*irq_handle)(void);                           /* interface function：interrupt service, only needed in interruption mode */
}
```

##### IO bus configuration

Define a global variable i2c\-dev\_t, and set teh I2C address of that device, reference code:
<pre><code>i2c_dev_t  ####_ctx = {

```
.config.dev_addr = 0x5D, /* I2C address of that device */
```

};</code></pre>

#### Implement of interface function

All peripheral drivers read and write in the form of VFS, so each driver needs to implement its necessary interface functions:
<pre><code>int (*open)(void); 
int (*close)(void);
int (*read)(void *, size_t);
int (*write)(const void *buf, size_t len);
int (*ioctl)(int cmd, unsigned long arg);
void(*irq_handle)(void);</code></pre>

#### implement of device initiation function

The function mainly works to initialize the setting of sensor_ojb_t, and register drivers to sensor HAL for unified scheduling. It also works to set default parameter, validate ID and make device enter low-power mode. Please refer to the following examples.

```
int drv_baro_st_lps22hb_init(void){
    /* fill the sensor obj parameters here */
    sensor.tag = TAG_DEV_HUMI;                   /* sensor type */
    sensor.path = dev_humi_path;                 /* path to open the device */
    sensor.io_port = I2C_PORT;                   /* bus port type */
    sensor.open = drv_humi_bosch_bme280_open;    /* interface function to open the device*/
    sensor.close = drv_humi_bosch_bme280_close;  /* interface function to close the device */
    sensor.read = drv_humi_bosch_bme280_read;    /* interface function to read device data */
    sensor.write = NULL;                         /* currently no need */
    sensor.ioctl = drv_humi_bosch_bme280_ioctl;  /* interface function to configure device */
    sensor.irq_handle = NULL;                    /* currently no need, but you must register it in interruption mode */
    sensor.bus = &bme280_ctx;                    /* bus configuration information, such as I2C address of slave device */

    /* register driver information in sensor HAL, and create a device node for that device */
    ret = sensor_create_obj(&sensor);
    if(unlikely(ret)){
       return -1;
    }
	
    /* validate device ID. it is not necessarily needed. */
    ret = drv_humi_bosch_bme280_validate_id(&bme280_ctx, WHO_AM_I_REG, WHO_AM_I_VAL);
    if(unlikely(ret)){
       return -1;
    }

    /* set default parameter. You can adjust configuration through IOCTL */
    ret = drv_humi_bosch_bme280_set_default_config(&bme280_ctx);
    if(unlikely(ret)){
       return -1;
    }
    return 0;
```

#### Compilation configuration 

##### 1. Add driver code and define its related macro in sensor.mk. For example, add compilation configuration for BOSCH BME280 humi:

```
$(NAME)\_SOURCES += \  
```

```
    drv_sensor_sample.c \
    sensor_hal.c \
    sensor_drv_api.c \
    sensor_hw_config.c \
    sensor_static_cali.c \
    drv_humi_bosch_bme280.c   /* add new driver code at last */

GLOBAL_INCLUDES += .  

GLOBAL_DEFINES      += AOS_SENSOR  

GLOBAL_DEFINES      += AOS_SENSOR_HUMI_BOSCH_BME280 /* define a macro for the new driver, which will be used when compiling configuration */
```

##### 2. Add init function in sensor\_init（） in sensor\_hal.c. Reference：

```
int sensor_init(void){
int ret   = 0;
int index = 0; 
g_sensor_cnt = 0 ;

/* add init function as this format. Success of initialization means that driver has been registered in sensor HAL */
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

#### Sensor data unit

This part mainly introduce the standard unit of uData related sensor data, so as to avoid the inconsistency among developers, resulting in problems in cloud computing and algorithm operation on data. Considering that not all platforms support floating point operation, and the performance burden it may brings about, the operation of sensor data in uData is calculated by integer data. This unit standard is used in physical sensor driver data and application algorithms. The unit presented to users can be determined by its final product.

| Sensor name          | Abbreviation  | Unit |        Type        |               Instruction                |
| -------------------- | :-----------: | :--: | :----------------: | :--------------------------------------: |
| accelerometer        |      acc      |  mg  |   Motion sensor    |       1 g = 1000 mg, 1 g = 9.8 m/s       |
| gyroscope            |     gyro      | udps |   Motion sensor    |        1 dps = 1000 000 udps, dps        |
| magnetometer         |      mag      |  mG  |  Position sensor   |              1 G = 1000 mG               |
| proximity            |   proximity   |  cm  |  Position sensor   |    distance between object and sensor    |
| ambient light sensor |      als      | lux  | Environment sensor |          Luminance lumen value           |
| Barometer            | baro/pressure |  Pa  | Environment sensor |              100 Pa = 1 hPa              |
| temperature sensor   |     temp      |  °c  | Environment sensor |                                          |
| humidity sensor      |     humi      |  %   | Environment sensor | Relative humidity %RH is usually represented as % only |
| Heart rate sensor    |      hr       | bpm  |       健康类传感器       |           bpm：times per second           |

Note: New unit of sensor data may be added later.
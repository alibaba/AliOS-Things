## Introduction
This module is a hardware abstract layer for sensors.It's convenient to develop sensor driver, by provide unified I2C and SPI bus drivers. More than 100 sensors driver have been integrated.

## Features
1. Hardware abstract layer for sensors;
2. Unified I2C and SPI bus drivers;
3. Support modbus and CANbus sensors;
4. More than 100 sensors integrated;

## Directories

<pre><code>
sensor
├── drv
│   ├── drv_acc_adi_adxl345.c
│   ├── drv_acc_adi_adxl355.c
│   ├── ...
│   ├── drv_uv_liteon_ltr390.c
│   └── drv_ps_st_vl53l0x          # vl53l0x ToF sensor, include third source files
│       ├── drv_ps_st_vl53l0x.c
│       ├── vl53l0x
│       └── vl53l0x_platform.c
├── hal
│   ├── gps_parse.c
│   ├── sensor_drv_api.c
│   ├── sensor_hal.c
│   └── sensor_static_calibrate.c
└── include
</code></pre>

## Dependencies
- I2C driver
- SPI driver
- modbus driver
- CANbus driver

## API
User service APIs:
<pre><code>
typedef struct _sensor_obj_t
{
    char *                 path;       /*sensor path for different tags*/
    sensor_tag_e           tag;        /*sensor tags*/
    uint8_t                instance;   /*sensor instance/index for certain tag*/
    dev_io_port_e          io_port;    /*sensor bus type*/
    work_mode_e            mode;       /*sensor work mode*/
    uint32_t               data_len;   /*sensor read buffer length for fifo mode*/
    gpio_dev_t             gpio;       /*gpio info for int mode*/
    int (*open)(void);                           /*sensor open callback*/
    int (*close)(void);                          /*sensor close callback*/
    int (*read)(void *, size_t);                 /*sensor read callback*/
    int (*write)(const void *buf, size_t len);   /*sensor write callback*/
    int (*ioctl)(int cmd, unsigned long arg);    /*sensor ioctl callback*/
    void (*irq_handle)(void);                    /*sensor irq handle callback*/
} sensor_obj_t;

/*sensor export API*/
int sensor_create_obj(sensor_obj_t* sensor);
</code></pre>

Usage:
## RTOS build
cd ROOT DIR;
aos make udataapp@board;

## run test
Sensor is part of uData， user can run [udataapp](../../app/example/udataapp) to get sensor data.

## Reference

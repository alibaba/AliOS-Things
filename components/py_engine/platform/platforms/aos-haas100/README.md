## aos 适配说明

### 代码目录
```
├── amp_fs.c
├── amp_system.c
├── amp_tls_mbedtls.c
├── network
│   ├── amp_tcp.c
│   └── amp_udp.c
├── peripheral
│   ├── amp_hal_adc.c
│   ├── amp_hal_dac.c
│   ├── amp_hal_gpio.c
│   ├── amp_hal_i2c.c
│   ├── amp_hal_pwm.c
│   ├── amp_hal_rtc.c
│   ├── amp_hal_spi.c
│   ├── amp_hal_timer.c
│   ├── amp_hal_uart.c
│   └── amp_hal_wdg.c
└── README.md

### 详细说明

* amp_fs.c
    文件系统相关接口，接口列表和详情请见 adapter/include/amp_fs.h

主要包含如：fopen、fread、fwrite等文件操作接口

* amp_kv.c
    key、value本地数据保存，如接入阿里云物联网平台获取的device_secret等

* amp_system.c
    主要包含操作系统相关接口，如线程创建、信号量、锁、定时器、队列等接口

* amp_tcp.c
    包含TCP建立连接，发送和接收数据接口，底层网络接口会有依赖

* amp_udp.c
    包含UDP建立连接，收发数据等接口

# 介绍
驱动测试功能

# 特性
用户态驱动对外提供VFS的访问接口
VFS接口可以和原本AliOS Things提供的hal_xxx_xxx接口一一对应

# 目录
```sh
main_driver/
├── aos.mk          #AliOS Things makefile文件
├── Config.in       #AliOS Things menuconfig配置文件
├── README.md
├── adc             #ADC驱动测试程序（VFS方式）
├── dac             #DAC驱动测试程序（VFS方式）
├── flash           #Flash驱动测试程序（VFS方式）
├── gpio            #GPIO驱动测试程序（VFS方式）
├── i2c             #I2C驱动测试程序（VFS方式）
├── pwm             #PWM驱动测试程序（VFS方式）
├── qspi            #QSPI核心驱动（VFS方式）
├── ram             #ram driver驱动测试程序
├── spi             #SPI驱动测试程序（VFS方式）
├── timer           #Timer驱动测试程序（VFS方式）
├── uart            #串行口驱动测试程序（VFS方式）
└──  wdg             #watchdog驱动测试程序（VFS方式）


```

# 依赖
- vfs
- uspace_aos

# 使用
所有驱动子系统测试程序都是命名方式均为u_xxx_test, 如u_gpio_test, u_spi_test ...
在使用用户态xxxx驱动提供的服务时，请参考此目录中xxxx目录下的xxxx_test.c中的测试代码。
1. 包含hal2vfs/io_xxxx.h头文件
```
<#include <hal2vfs/io_xxxx.h>>
```
2. 通过VFS方式访问驱动提供的服务
以GPIO为例，要操作GPIO10的功能，需要先open /dev/gpio设备之后，
设置好gpio_io_config之后再通过ioctl去控制其功能
```
    int fd = -1;
    struct gpio_io_config config;

    fd = open("/dev/gpio", 0);
    ddkc_info("open gpio %s, fd:%d\r\n", fd >= 0 ? "success" : "fail", fd);

    if (fd >= 0) {
        config.id = 10; /* 需要读取的GPIO ID */
        config.config = GPIO_IO_INPUT | GPIO_IO_INPUT_PU;
        config.data = 0;
        /* 通过IOC_GPIO_GET获取GPIO作为输入口读取到的电平 */
        ret = ioctl(fd, IOC_GPIO_GET,  (unsigned long)&config);
        ddkc_info("gpio read %d return %d\r\n", id, ret);

        config.id = id; /* 需要控制的GPIO ID */
        config.config = GPIO_IO_OUTPUT | GPIO_IO_OUTPUT_PP;
        config.data = 1;
        /* IOC_GPIO_SET设置GPIO的输出值 */
        ret = ioctl(fd, IOC_GPIO_SET, (unsigned long)&config);
        ddkc_info("gpio write %d return %d\r\n", config.data, ret);
        
        usleep(1000000);
        
        /* IOC_GPIO_SET设置GPIO的输出值反转 */
        config.config = GPIO_IO_OUTPUT_TOGGLE;
        config.data = 0;
        ret = ioctl(fd, IOC_GPIO_SET, (unsigned long)&config);
        ddkc_info("gpio toggle return %d\r\n", ret);

        close(fd);
    }
```

其中i2c目录下的si7006.c原本是使用hal_i2c_xxxx实现的i2c驱动
在此测试代码中用open/ioctl/close的方式替换了原本调用hal_i2c_xxx的功能。


# 参考

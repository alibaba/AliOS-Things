@page pca9544 pca9544

[更正文档](https://gitee.com/alios-things/pca9544/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述

PCA9544是一颗有4个选通通道的I2C多路转接器。HaaS100的板上就使用了这颗做了I2C的通道扩展。本文档描述了该芯片的驱动详情，包括目录结构，依赖情况，API，使用等情况，供用户参考。
组件支持以下功能：

- 初始化
- 通道选通
- 当前通道读取
- 写寄存器
- 读寄存器
- 裸数据读写


## 版权信息

> Apache license v2.0

## 目录结构

```tree
├── example
│   └── i2c_muxer_example.c
├── include
│   └── pca9544.h
├── internal
│   └── pca9544_i2c_drv.h
├── package.yaml
├── README.md
└── src
    └── pca9544.c
    └── pca9544_i2c_drv.c

```

## 依赖组件

- base
- vfs
- i2c

# 常用配置

# API说明

```c
/**********************************************************
 * @fun    pca9544_init
 * @breif  pca9544 initialization
 * @param  i2c:the pointer for i2c configuration
 * @param  dev_cfg: the pointer for dev configuration
 * @rtn
 *********************************************************/
int pca9544_init(i2c_dev_t *i2c, PCA9544_DEV_CFG_T* dev_cfg);

/**********************************************************
 * @fun    pca9544_deinit
 * @breif  pca9544 deinitialization
 * @param  none
 * @rtn    none
 *********************************************************/
void pca9544_deinit();

/**********************************************************
 * @fun    pca9544_set_chan
 * @breif  pca9544 channel set
 * @param  chan: the object channel
 * @rtn    ret
 *********************************************************/
int pca9544_set_chan(PCA9544_CH_E chan);

/**********************************************************
 * @fun    pca9544_clean_chan
 * @breif  pca9544 channel clean
 * @param  none
 * @rtn    ret
 *********************************************************/
int pca9544_clean_chan();

/**********************************************************
 * @fun    pca9544_read_chan
 * @breif  pca9544 channel reading
 * @param  none
 * @rtn    chan: the current selected channel of pca9544
 *********************************************************/
PCA9544_CH_E pca9544_read_chan();

/**********************************************************
 * @fun    pca9544_write_reg
 * @breif  pca9544 register writing operation
 * @param  dev_addr: i2c device address
 * @param  regval: register value
 * @rtn    ret
 *********************************************************/
int pca9544_write_reg(uint16_t dev_addr, uint8_t regval);

/**********************************************************
 * @fun    pca9544_read_reg
 * @breif  pca9544 register reading operation
 * @param  dev_addr: i2c device address
 * @param  regval: register value
 * @rtn    ret
 *********************************************************/
int pca9544_read_reg(uint16_t dev_addr, uint8_t* data);

/**********************************************************
 * @fun    pca9544_raw_data_write
 * @breif  write raw data to the selected channel
 * @param  sub_addr: the sub dev addr for
 *                   the selected pca9544 channel
 * @param  reg_addr: the sub dev register addr for
 *                   the selected pca9544 channel
 * @param  size: register size
 * @param  data: the pointer for the writing data
 * @param  len: the data length
 * @rtn    ret
 *********************************************************/
int pca9544_raw_data_write(uint16_t subdev_addr, uint16_t reg_addr,
    uint16_t size,  uint8_t* data, uint16_t len);

/**********************************************************
 * @fun    pca9544_raw_data_read
 * @breif  read raw data to the selected channel
 * @param  sub_addr: the sub dev addr for
 *                   the selected pca9544 channel
 * @param  reg_addr: the sub dev register addr for
 *                   the selected pca9544 channel
 * @param  size: register size
 * @param  data: the pointer for the reading data
 * @param  len: the data length
 * @rtn    ret
 *********************************************************/
int pca9544_raw_data_read(uint16_t subdev_addr, uint16_t reg_addr,
    uint16_t size,  uint8_t* data, uint16_t len);

```

# 使用示例

组件使用示例相关的代码下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

## 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《aos-studio使用说明之打开工程》](https://g.alicdn.com/alios-things-3.3/doc/open_project.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。

## 步骤2 添加pca9544组件

> helloworld_demo组件的package.yaml中添加

```bash
depends:
  - pca9544: master # helloworld_demo中引入pca9544组件
```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install pca9544

```

上述命令执行成功后，组件源码则被下载到了./components/drivers/external_device路径中。

## 步骤4 添加示例代码

> 向pca9544组件的package.yaml中添加example

```bash
source_file:
  - "src/*.c"
  - "example/pca9544_example.c" # add pca9544_example.c
```

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 PCA9544示例测试

> CLI命令行输入：

```bash
pca9544_example
```

> CLI日志：

```bash
pca9544_example
PCA9544 cfg is successful
=====I2C Muxer PCA9544 test: set chan 2[5] of PCA9544=====
=====I2C Muxer PCA9544 test: read chan val of PCA9544 is 5=====
=====I2C Muxer PCA9544 test: PCA9544 test:  PASS=====
pca9544 comp test success!

```

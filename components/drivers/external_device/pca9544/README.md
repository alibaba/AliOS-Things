@page pca9544 pca9544
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


```bash
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


示例代码参考example/pca9544_example.c，以运行helloworld_demo为例，具体步骤如下：


## 添加示例代码


> 向pca9544组件的package.yaml中添加example



```bash
source_file:
  - "src/*.c"
  - "example/pca9544_example.c" # add pca9544_example.c
```


## 添加pca9544组件


> helloworld_demo组件的package.yaml中添加



```bash
depends:
  - pca9544: master # helloworld_demo中引入pca9544组件
```


## 编译


```sh
cd solutions/helloworld_demo && aos make
```


其中具体单板还需要先配置环境：


```sh
aos make helloworld_demo@haas100 -c config
```


## 烧录固件


> 参考具体板子的快速开始文档。



## PCA9544示例测试


> CLI命令行输入：



```bash
pca9544_example
```


## 关键日志


> CLI日志：



```bash
pca9544_example
PCA9544 cfg is successful
=====I2C Muxer PCA9544 test: set chan 2[5] of PCA9544=====
=====I2C Muxer PCA9544 test: read chan val of PCA9544 is 5=====
=====I2C Muxer PCA9544 test: PCA9544 test:  PASS=====
pca9544 comp test success!

```



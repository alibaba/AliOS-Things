@page rx8130ce rx8130ce

# 概述


RX8130CE是一颗RTC芯片，基于I2C总线进行通信，其内建了32.768KHZ的晶振，功能完备，HaaS100的板上就使用了这颗。本文档描述了该芯片的驱动详情，包括目录结构，依赖情况，API，使用等情况，供用户参考。
组件支持以下功能：

- rx8130ce初始化
- 设置时间
- 读取时间



## 版权信息


> Apache license v2.0



## 目录结构


```bash
├── example
│   └── rx8130ce_example.c
├── include
│   └── rx8130ce.h
├── internal
│   └── rtcif.h
├── package.yaml
├── README.md
└── src
    ├── rtcif.c
    └── rx8130ce.c
```


## 依赖组件


- base
- vfs
- pca9544



# 常用配置




# API说明


```c
/*********************************************************
 * @fun    rx8130ce_init
 * @breif  rx8130ce initialization
 * @param  none
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int rx8130ce_init(void);

/*********************************************************
 * @fun    rx8130ce_set_time
 * @param[in]  buf      the pointer for rx8130ce time
 * @param[in]  len      the length of time
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int rx8130ce_set_time(const void *buf, uint8_t len);

/*********************************************************
 * @fun    rx8130ce_get_time
 * @param[in]  buf      the pointer for rx8130ce time
 * @param[in]  len      the length of time
 * @rtn    0 : on success, EIO : error
 *********************************************************/
int rx8130ce_get_time(void *buf, uint8_t len);
```


# 使用示例


示例代码参考example/rx8130ce_example.c，以运行helloworld_demo为例，具体步骤如下：


## 添加示例代码


> 向rx8130ce组件的package.yaml中添加example



```bash
source_file:
  - "src/*.c"
  - "example/rx8130ce_example.c" # add rx8130ce_example.c
```


## 添加rx8130ce组件


> helloworld_demo组件的package.yaml中添加



```bash
depends:
  - rx8130ce: master # helloworld_demo中引入rx8130ce组件
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



## RX8130CE示例测试


> CLI命令行输入：



```bash
rx8130ce_example
```


## 关键日志


> CLI日志：



```bash
rx8130ce comp test success!
```


#


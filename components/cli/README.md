@page cli cli

# 概述
在日常嵌入式开发中，用户经常会自行实现一套类似Linux Shell的交互工具来实现通过串口命令控制设备进入某种特定的状态，或执行某个特定的操作。如系统自检，模拟运行，或者进入手动模式进行设备点动。AliOS Things原生实现了一套名为CLI（Command-Line Interface）的命令行交互工具，在提供基本的系统交互命令的基础上，也支持用户自定义命令。
组件支持以下功能：
- cli commands register
- cli commands unregister
- cli commands show
## 版权信息
> Apache license v2.0

## 目录结构
```sh
├── include
│   └── aos
│       └── cli.h        # cli的对外AOS API
├── package.yaml         # 编译配置文件
├── README.md
└── src
    ├── cli_adapt.c      # cli适配层
    ├── cli_adapt.h
    ├── cli_aos.c        # cli-aos接口层
    ├── cli_api.h
    ├── cli.c            # cli核心管理
    ├── cli_console.c    # cli console管理
    ├── cli_console.h
    ├── telnet_console.c # cli使用telnet输入输出（可选方式）
    ├── uart_console.c   # cli使用Uart输入输出（默认方式）
    ├── iobox            # 文件系统操作命令集
    │   ├── cat.c
    │   ├── cd.c
    │   ├── cp.c
    │   ├── df.c
    │   ├── echo.c
    │   ├── hexdump.c
    │   ├── ls.c
    │   ├── lsfs.c
    │   ├── mkdir.c
    │   ├── mv.c
    │   ├── path_helper.c
    │   ├── path_helper.h
    │   ├── pwd.c
    │   ├── rm.c
    │   └── touch.c
```

## 依赖组件
* rhino
* driver
* vfs(使用文件系统操作命令)
* littlefs(使用文件系统操作命令)


# 常用配置
系统中相关配置已有默认值，如需修改配置，统一在yaml中**def_config**节点修改，具体如下：
> cli 输入缓冲区大小: 默认256 bytes，即cli单次可接收的输入最大字节数，可修改yaml配置如：
```sh
def_config:
  CLI_INBUF_SIZE: 256
```
> cli 输出缓冲区大小: 默认512 bytes，即cli命令中单个printf输出的最大字节数，可修改yaml配置如：
```sh
def_config:
  CLI_OUTBUF_SIZE: 512
```
> 可注册的cli命令最大值，默认128个命令（通过help命令查看），可修改yaml配置如：
```sh
def_config:
  CLI_MAX_COMMANDS: 128
```
> 单个cli命令可带参数的最大值，默认16个参数，可修改yaml配置如：
```sh
def_config:
  CLI_MAX_ARG_NUM: 16
```
> 可连续执行的cli命令数量（命令中间用分号；间隔），默认4个，可修改yaml配置如：
```sh
def_config:
  CLI_MAX_ONCECMD_NUM: 4
```
> cli后台任务的优先级，默认为60，可修改yaml配置如：
```sh
def_config:
  CLI_TASK_PRIORITY: 60
```
> cli后台任务的栈大小，默认为2KB（2048Bytes），可修改yaml配置如：
```sh
def_config:
  CLI_CONFIG_STACK_SIZE: 2048
```
> cli使用telnet接管输入输出，此功能默认不开，可修改yaml配置如：
```sh
def_config:
  CLI_TELNET_SUPPORT: 0
```
> cli输出增加不可见字符，用于SmartTrace工具中，此功能默认打开，可修改yaml配置如：
```sh
def_config:
  CLI_SEPRATED_CONSOLE: 0
```

> cli使能文件系统操作命令，此功能默认不开，可修改yaml配置如：
```sh
def_config:
  CLI_IOBOX_ENABLE: 0
```
# API说明
@ref cli_aos_api

# 使用示例

## 示例代码

Example:
包含头文件 **#include "aos/cli.h"**

> 注册命令方式1：注册单个cli命令
```C
int ret;
void test_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    /* test_cmd 命令实现 */
    aos_cli_printf("this is test cmd\r\n");
}

const struct cli_command cmd = {"test", "test cmd description", test_cmd};

ret = aos_cli_register_command(&cmd);
if (ret) {
  /* 错误处理 */
  aos_cli_printf("test cmd register fail\r\n");
}
```

> 注册命令方式2：同时注册多个cli命令
```C
int ret;
void test1_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    /* test_cmd 命令实现 */
    aos_cli_printf("this is test cmd1\r\n");
}

void test2_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    /* test_cmd 命令实现 */
    aos_cli_printf("this is test cmd2\r\n");
}

const struct cli_command cmds[] = {
     { "test1", "show test1 info", test1_cmd },
     { "test2", "show test2 info", test2_cmd },
};

ret = aos_cli_register_commands(&cmds, sizeof(cmds) / sizeof(struct cli_command));
if (ret) {
    /* 错误处理 */
    aos_cli_printf("test cmds register fail\r\n");
}
```

> 注册命令方式3：通过宏注册单个cli命令
> 注意：这种方式不需要判断返回值，若注册失败，系统在初始化过程中会输出相应信息
```C
void test_cmd3(int32_t argc, char **argv)
{
    /* test_cmd3 命令实现 */
    aos_cli_printf("this is test cmd3\r\n");
}

/* 宏的参数说明：
 * 参数1. cmd具体实现
 * 参数2. 在串口下输入的命令
 * 参数3. cmd的描述信息
 */
ALIOS_CLI_CMD_REGISTER(test_cmd3, test3, show test3 info)
```

## 添加cli组件
> helloworld_demo组件的pacxkage.yaml中添加
```sh
depends:
  - cli: master # helloworld_demo中引入cli组件
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

## CLI示例测试
> 命令行输入：
```sh
help
```

## 关键日志
> CLI日志：
```sh
================ AliOS Things Command List ==============
help
...(注册的cli命令)
================ AliOS Things Command End ===============
```

# FAQ
无


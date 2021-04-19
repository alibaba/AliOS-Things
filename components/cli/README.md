@page cli cli

# 概述
在日常嵌入式开发中，用户经常会自行实现一套类似Linux Shell的交互工具来实现通过串口命令控制设备进入某种特定的状态，或执行某个特定的操作。如系统自检，模拟运行，或者进入手动模式进行设备点动。AliOS Things原生实现了一套名为CLI（Command-Line Interface）的命令行交互工具，在提供基本的系统交互命令的基础上，也支持用户自定义命令。
组件支持以下功能：
- cli commands register
- cli commands unregister
- cli commands show
- fs cmds support
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
    ├── uart_console.c   # cli使用Uart串口输入输出（默认方式）
    ├── telnet_console.c # cli使用telnet局域网输入输出（可选方式）
    ├── cli_uagent.c     # cli使用远程调试终端输入输出（可选方式）
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
系统中相关配置已有默认值，如需修改配置，在相应app下的package.yaml中**def_config**节点修改。例如：若运行的app为helloworld_demo，则在helloworld_demo下的package.yaml中修改：
> cli 输入缓冲区大小: 默认256 bytes，即cli单次可接收的输入最大字节数
```sh
def_config:
  CLI_INBUF_SIZE: 256
```
> cli 输出缓冲区大小: 默认512 bytes，即cli命令中单个printf输出的最大字节数
```sh
def_config:
  CLI_OUTBUF_SIZE: 512
```
> 可注册的cli命令最大值，默认最多可注册128个命令（通过help命令查看）
```sh
def_config:
  CLI_MAX_COMMANDS: 128
```
> 单个cli命令可带参数的最大值，默认最多可带16个参数
```sh
def_config:
  CLI_MAX_ARG_NUM: 16
```
> 可连续执行的cli命令数量（命令中间用分号；间隔），默认为4个
```sh
def_config:
  CLI_MAX_ONCECMD_NUM: 4
```
> cli后台任务的优先级，默认为60
```sh
def_config:
  CLI_TASK_PRIORITY: 60
```
> cli后台任务的栈大小，默认为2KB(2048Bytes)
```sh
def_config:
  CLI_CONFIG_STACK_SIZE: 2048
```
> cli使用telnet接管输入输出，此功能默认不开
```sh
def_config:
  CLI_TELNET_ENABLE: 0
```
> cli使用uagent接管输入输出，即cli日志输入输出在远程诊断终端上，此功能默认不开
```sh
def_config:
  CLI_UAGENT_ENABLE: 0
```

> cli输出增加不可见字符，用于SmartTrace工具中，此功能默认关闭
```sh
def_config:
  CLI_SEPRATED_CONSOLE: 0
```

> cli使能文件系统操作命令，此功能默认不开
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

> 开发环境的搭建请参考 @ref HaaS100_Quick_Start (搭建开发环境章节)，其中详细的介绍了AliOS Things 3.3的IDE集成开发环境的搭建流程。
## app中添加cli组件
> 以运行helloworld_demo为例，具体步骤如下：
> 在helloworld_demo组件的package.yaml中添加
```sh
depends:
  - cli: dev_aos # helloworld_demo中引入cli组件
```

## 编译烧录
>参考 @ref HaaS100_Quick_Start

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
Q1： 如何打开文件系统操作命令集？
```sh
答：
cli组件支持文件系统操作命令，默认关闭，打开方式为：
以运行helloworld_demo为例，在其package.yaml中添加

def_config:
    CLI_IOBOX_ENABLE: 1

然后重新编译烧录，上电后输入help，即可看到
ls
ll
mkdir
cat
df
touch
...
等文件系统命令
```

Q2：文件系统命令如何使用？
```sh
答：
cli组件支持文件系统操作命令，使用方法类似Linux常见shell命令，简单举例如下：

lsfs - 列出当前系统注册的文件系统节点
命令显示：
/data
表示当前文件系统(littlefs)注册的节点为/data

df - 显示文件系统当前使用情况
命名显示：
     Total      Used      Free   Use%    Mount
      4792      2416      2376    50%    /data
可以查看/data下文件的使用情况

mkdir - 创建文件夹
mkdir /data/123
ll /data （或者 ls -l /data）
命令显示：
drwxrwxrwx root root 0B .
drwxrwxrwx root root 0B ..
drwxrwxrwx root root 0B aaa
-rwxrwxrwx root root 21B demo
drwxrwxrwx root root 0B font
drwxrwxrwx root root 0B lib
(注意：当前尚不支持可读/可写/可执行等权限设置)

touch - 创建单个文件
cd /data/aaa
touch 123.txt
ll
命令显示：
drwxrwxrwx root root 0B .
drwxrwxrwx root root 0B ..
-rwxrwxrwx root root 0B 123.txt

pwd - 显示当前路径
命令显示：
/data/aaa

echo
1. 显示普通字符串:
echo "hello,world!"

2. 显示结果定向至文件
echo "alibaba" > /data/aaa/123.txt
cat /data/aaa/123.txt
命令显示：
alibaba
```


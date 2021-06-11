@page cli cli

[更正文档](https://gitee.com/alios-things/cli/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

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
```tree
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
```yaml
def_config:
  CLI_INBUF_SIZE: 256
```
> cli 输出缓冲区大小: 默认512 bytes，即cli命令中单个printf输出的最大字节数
```yaml
def_config:
  CLI_OUTBUF_SIZE: 512
```
> 可注册的cli命令最大值，默认最多可注册128个命令（通过help命令查看）
```yaml
def_config:
  CLI_MAX_COMMANDS: 128
```
> 单个cli命令可带参数的最大值，默认最多可带16个参数
```yaml
def_config:
  CLI_MAX_ARG_NUM: 16
```
> 可连续执行的cli命令数量（命令中间用分号；间隔），默认为4个
```yaml
def_config:
  CLI_MAX_ONCECMD_NUM: 4
```
> cli后台任务的优先级，默认为60
```yaml
def_config:
  CLI_TASK_PRIORITY: 60
```
> cli后台任务的栈大小，默认为2KB(2048Bytes)
```yaml
def_config:
  CLI_CONFIG_STACK_SIZE: 2048
```
> cli使用telnet接管输入输出，此功能默认不开
```yaml
def_config:
  CLI_TELNET_ENABLE: 0
```
> cli使用uagent接管输入输出，即cli日志输入输出在远程诊断终端上，此功能默认不开
```yaml
def_config:
  CLI_UAGENT_ENABLE: 0
```

> cli输出增加不可见字符，用于SmartTrace工具中，此功能默认关闭
```yaml
def_config:
  CLI_SEPRATED_CONSOLE: 0
```

> cli使能文件系统操作命令，此功能默认不开
```yaml
def_config:
  CLI_IOBOX_ENABLE: 0
```
# API说明
- 参考 [cli_aos_api](https://dev.g.alicdn.com/alios-things-3.3/doc/group__cli__aos__api.html)

# 使用示例

组件使用示例相关的代码下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。


## 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《aos-studio使用说明之打开工程》](https://g.alicdn.com/alios-things-3.3/doc/open_project.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。


## 步骤2 添加组件

案例下载完成后，以运行helloworld_demo为例，需要在helloworld_demo组件的package.yaml中添加对组件的依赖：

```yaml

depends:
  - cli: master  # helloworld_demo中引入cli组件

```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install cli

```

上述命令执行成功后，组件源码则被下载到了./components/cli路径中。

## 步骤4 添加示例

下面示例演示了注册cli命令的三种方式，分别是：
1. 注册单个cli命令
2. 同时注册对个cli命令
3. 通过宏注册单个cli命令

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
**代码示例**
helloworld_demo注册cli命令，修改helloworld.c如下：
```C
#include "aos/init.h"
#include "board.h"
#include <aos/errno.h>
#include <aos/kernel.h>
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "aos/cli.h" /* 加上头文件 */

void test_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    /* test_cmd 命令实现 */
    aos_cli_printf("this is test cmd\r\n");
}

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

const struct cli_command cmd = {"test", "test cmd description", test_cmd};

const struct cli_command cmds[] = {
     { "test1", "show test1 info", test1_cmd },
     { "test2", "show test2 info", test2_cmd },
};


int application_start(int argc, char *argv[])
{
    int count = 0;
    int ret;

    printf("nano entry here!\r\n");

    ret = aos_cli_register_command(&cmd);
    if (ret) {
      /* 错误处理 */
      aos_cli_printf("test cmd register fail\r\n");
    }

    ret = aos_cli_register_commands(&cmds, sizeof(cmds) / sizeof(struct cli_command));
    if (ret) {
        /* 错误处理 */
        aos_cli_printf("test cmds register fail\r\n");
    }

    while (1) {
        //printf("hello world! count %d \r\n", count++);
        aos_msleep(1000);
    };
}

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

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。


## 步骤8 测试示例

**CLI命令行输入：**
```shell
help
```

> 可以看到注册的命令：
```shell
test
test1
test2
test3
```
执行不同的命令，有相应的日志输出

# FAQ
Q1： 如何打开文件系统操作命令集？
答：
cli组件支持文件系统操作命令，默认关闭，打开方式为：
以运行helloworld_demo为例，在其package.yaml中添加

```yaml
def_config:
  CLI_IOBOX_ENABLE: 1
```

然后重新编译烧录，上电后输入help，即可看到
ls
ll
mkdir
cat
df
touch
...
等文件系统命令


Q2：文件系统命令如何使用？
答：
cli组件支持文件系统操作命令，使用方法类似Linux常见shell命令，简单举例如下：

lsfs - 列出当前系统注册的文件系统节点
命令显示：
/data
表示当前文件系统(littlefs)注册的节点为/data

df - 显示文件系统当前使用情况（单位：KB）
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


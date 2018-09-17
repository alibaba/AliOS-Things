# 定制工程全局的编译行为

## 增加硬件平台配置文件

在默认的`configs`目录, 或者在指定的`$(CONFIG_DIR)`目录下, 文件名形式为`config.<VENDOR>.<MODEL>`的文本文件, 会被构建系统认为是硬件平台配置文件, 每个文件对应一个嵌入式软硬件平台

* 其中<VENDOR>部分, 一般是指明嵌入式平台的软件OS提供方, 如`mxchip`, `ubuntu`, `win7`等. 另外, 这也会导致构建系统到`$(IMPORT_DIR)/<VENDOR>`目录下寻找预编译库的二进制库文件和头文件
* 其中<MODEL>部分, 一般是标明嵌入式平台的具体硬件型号, 如`mtk7687`, `qcom4004`等, 不过也可以写上其它信息, 因为构建系统不会去理解它, 比如`mingw32`, `x86-64`等

> 例如`config.mxchip.3080c`文件, 如果在`make reconfig`的时候被选择, 则会导致:

* 构建系统在`import/mxchip/`目录下寻找预编译库的二进制库文件和头文件
* 构建系统使用该文件内的变量指导编译行为, 具体来说, 可以根据说明使用如下变量

| 变量                  | 说明                                      |
|-----------------------|-------------------------------------------|
| `CONFIG_ENV_CFLAGS`   | 指定全局的`CFLAGS`编译选项, 传给`compiler`, 例如`CONFIG_ENV_CFLAGS += -DDEBUG` |
| `CONFIG_ENV_LDFLAGS`  | 指定全局的`LDFLAGS`链接选项, 传给`linker`, 例如`CONFIG_ENV_LDFLAGS += -lcrypto` |
| `CROSS_PREFIX`        | 指定交叉编译工具链共有的前缀, 例如`CROSS_PREFIX := arm-none-eabi-`, 会导致构建系统使用`arm-none-eabi-gcc`和`arm-none-eabi-ar`, 以及`arm-none-eabi-strip`等 |
| `OVERRIDE_CC`         | 当交叉工具链没有共有的前缀或者前缀不符合`prefix+gcc|ar|strip`类型时, 例如`armcc`, 可用`OVERRIDE_CC = armcc`单独指定C编译器 |
| `OVERRIDE_AR`         | 当交叉工具链没有共有的前缀或者前缀不符合`prefix+gcc|ar|strip`类型时, 例如`armar`, 可用`OVERRIDE_AR = armar`单独指定库压缩器 |
| `OVERRIDE_STRIP`      | 当交叉工具链没有共有的前缀或者前缀不符合`prefix+gcc|ar|strip`类型时, 例如`armcc`没有对应的strip程序, 可用`OVERRIDE_STRIP = true`单独指定strip程序不执行 |

## 定制单元测试

如果你的工程含有配合构建系统的单元测试用例, 则可以在顶层`makefile`中添写类似如下语句, 告诉构建系统在`make test`命令中执行它们做单元测试, 并统计源码覆盖率

    UTEST_PROG := utils-tests

> 以上面的`LITE-utils`为例, 其中的`utils-tests`就是它的单元测试程序, 这是在构建单元`testsuites`中产生的一个`Linux`下可执行程序

    $ cat testsuites/iot.mk -n
         1  TARGET      := utils-tests
         2  DEPENDS     += external/cut src
         3  HDR_REFS    := src
         4
         5  LDFLAGS     += -llite-utils -llite-log -llite-cut

为什么如上的一个简单`iot.mk`片段文件, 就能指导构建系统生成可执行程序`utils-tests`, 可以访问[**构建单元说明**](https://code.aliyun.com/edward.yangx/public-docs/wikis/build/build-system-units)

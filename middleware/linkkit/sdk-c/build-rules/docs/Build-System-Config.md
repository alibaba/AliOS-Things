# 构建系统配置

## 定制目录排布

默认情况下, 构建系统采用如下的目录排布:

    CONFIG_DIR      ?= $(TOP_DIR)/configs
    DIST_DIR        ?= $(TOP_DIR)/output
    FINAL_DIR       ?= $(DIST_DIR)/release
    IMPORT_DIR      ?= $(TOP_DIR)/import
    LCOV_DIR        ?= Coverage
    MAKE_SEGMENT    ?= iot.mk
    OUTPUT_DIR      ?= $(TOP_DIR)/.O
    PACKAGE_DIR     ?= $(TOP_DIR)/packages
    RULE_DIR        ?= $(TOP_DIR)/build-rules
    TOP_DIR         ?= $(CURDIR)

含义如下:

| 变量名            | 说明                                      |
|-------------------|-------------------------------------------|
| `CONFIG_DIR`      | *硬件平台配置文件*的存放目录, 其中的文件需要以`config.XXX.YYY`形式命名, 其中`XXX`一般表示操作系统, `YYY`一般表示具体的硬件型号 |
| `DIST_DIR`        | 最终的编译产物, 例如可执行程序, SDK的总库等, 存放的顶层目录 |
| `FINAL_DIR`       | 最终的编译产物摆放时, 会放在`$(FINAL_DIR)`下, 这个变量可以指定最终摆放位置在顶层目录下的子目录路径 |
| `IMPORT_DIR`      | 输入目录, 用于摆放预编译的二进制库, 以及使用这些预编译库对应的头文件 |
| `LCOV_DIR`        | 覆盖率文件产生目录, 用于存放`lcov`软件产生的中间文件和最终的`HTML`格式图形化文件 |
| `MAKE_SEGMENT`    | 构建单元中的片段文件, 开发者唯一需要编辑的文件, 指导构建系统如何编译该文件所在的构建单元 |
| `OUTPUT_DIR`      | 编译树中间目录 |
| `PACKAGE_DIR`     | 打包目录, 其中可以存放其它组件, 也可以存放压缩包形式的第三方软件, 例如`openssl-0.9.8.tar.gz`等 |
| `RULE_DIR`        | 构建系统核心目录, 如果不喜欢`build-rules`存放在自己的工程里, 完全可以通过这个变量把它移出去 |
| `TOP_DIR`         | 工程的顶级目录, 默认就是顶层`makefile`所在的路径, 极少改动 |

可以完全不对它们做任何改动, 依照上文的含义和默认值来排布自己工程

---
也可以通过顶层目录的`project.mk`, 可以改动它们, 例如:

    $ cat project.mk
    MAKE_SEGMENT := alink.mk

则表示在每个构建单元中, 指导构建过程的片段文件不再是默认的`iot.mk`, 而是`alink.mk`

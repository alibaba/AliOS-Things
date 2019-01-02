# 如何获取

* 可访问样板组件`LITE-utils`的仓库: [*git@gitlab.alibaba-inc.com:iot-middleware/LITE-utils.git*](http://gitlab.alibaba-inc.com/iot-middleware/LITE-utils)
* 从`master`分支的`build-rules`目录复制得到构建系统的最新副本
* 也可以直接在`LITE-utils`中体验构建系统的工作方式和观察工作过程

# 常用命令

| 命令                  | 解释                                                                              |
|-----------------------|-----------------------------------------------------------------------------------|
| `make distclean`      | **清除一切构建过程产生的中间文件, 使当前目录仿佛和刚刚clone下来一样**             |
| `make [all]`          | **使用默认的平台配置文件开始编译**                                                |
| `make reconfig`       | **弹出多平台选择菜单, 用户可按数字键选择, 然后根据相应的硬件平台配置开始编译**    |
| `make config`         | **显示当前被选择的平台配置文件**                                                  |
| `make help`           | **打印帮助文本**                                                                  |
| `make <directory>`    | **单独编译被<directory>指定的目录, 或者叫构建单元**                               |

# 如何开发

* 访问[**构建系统配置**](https://code.aliyun.com/edward.yangx/public-docs/wikis/build/build-system-config), 了解如何配置来影响构建系统的行为
* 访问[**构建工程说明**](https://code.aliyun.com/edward.yangx/public-docs/wikis/build/build-system-proj), 了解如何定制工程全局的编译
* 访问[**构建单元说明**](https://code.aliyun.com/edward.yangx/public-docs/wikis/build/build-system-units), 了解如何增删改查你自己的构建单元
* 访问[**构建系统调试**](https://code.aliyun.com/edward.yangx/public-docs/wikis/build/build-system-debug), 了解在定制构建单元时, 如何自己调试

# 高级命令

| 命令                  | 解释                                                                                      |
|-----------------------|-------------------------------------------------------------------------------------------|
| `make test`           | **运行指定的测试集程序, 统计显示测试例的通过率和源代码的覆盖率**                          |
| `make doc`            | **扫描源码目录中以`Doxygen`格式或者`Markdown`格式编写的注释, 在`html`目录产生帮助文档**   |
| `make repo-list`      | **列出当前工程引用的外部组件和它们的云端仓库地址**                                        |
| `make repo-update`    | **对所有引用到的外部组件, 用云端仓库更新本地仓库**                                        |

# 组成部分

    LITE-utils$ tree -A -L 1
    .
    +-- build-rules     # 构建系统核心
    +-- example
    +-- external
    +-- makefile        # 工程的makefile
    +-- packages
    +-- project.mk      # 工程设置文件
    +-- src
    +-- testsuites

> 以`LITE-utils`为例, 对整个构建过程需要知道的只有如上三个部分:

| 组成部分      | 说明                                                                          |
|---------------|-------------------------------------------------------------------------------|
| `project.mk`  | 本工程的目录排布, 工程名称, 版本信息等, 可选                                  |
| `makefile`    | 本工程的makefile, 基于`GNU Make`, 通常只含有极少的内容, 指定编译的范围, 必选  |
| `build-rules` | 构建系统核心, 指定编译的规则, 不需要关注                                      |

# 工作过程

可以从一个简单的`makefile`样例看起

     1  sinclude project.mk
     2  sinclude $(CONFIG_TPL)
     3
     4  SUBDIRS += \
     5      external/cut \
     6      src \
     7      example \
     8      testsuites \
     9
    10  include $(RULE_DIR)/rules.mk

0. 构建系统是基于`GNU Make`的简化系统, 所以工作过程的起点仍然是顶层的`makefile`
1. 读取`project.mk`, 如果当前工程的目录排布和默认的不一样, 则用当前设置, 可选
2. 读取`.config`文件, 这个文件其实是构建系统运行时所有输入的集合, 也称为*硬件平台配置文件*, 可选
3. 读取`SUBDIRS`变量, 这个变量指定了编译的范围, 必选
4. 读取`CFLAGS`或者`LDFLAGS`等变量, 顶层`makefile`的变量会被应用到每个构建单元的编译和链接, 可选
5. 读取`.../rules.mk`文件, 开始进入构建系统核心, 必选, 这又可以细分为如下过程

## 详细工作过程

* 产生*硬件平台配置文件*, 这个过程是为了产生顶层的`.config`文件, 也就是所谓的`$(CONFIG_TPL)`
* 识别构建单元, 所谓构建单元在构建系统看来就是一个又一个包含`iot.mk`的目录, 如果不希望用`iot.mk`作为每个单元的编译片段文件, 在`project.mk`中设置`MAKE_SEGMENT`变量即可
* 从`$(SUBDIRS)`变量从前往后逐个编译每个构建单元
* 构建单元如果有依赖到其它构建单元, 则先去编译被依赖的单元; 若后者又有依赖的其它的单元, 则同样, 递归的先去编译其依赖; 以此类推

# 高级命令示例

- **make test: 运行指定的测试集程序, 统计显示测试例的通过率和源代码的覆盖率**

![image](https://yuncodeweb.oss-cn-hangzhou.aliyuncs.com/uploads/edward.yangx/public-docs/52c806dd879f18cd7ea855a8549461f6/image.png)  

- **make doc: 扫描源码目录中以`Doxygen`格式或者`Markdown`格式编写的注释, 在`html`目录产生帮助文档**

        LITE-utils $ make doc
        ...
        ...
        Patching output file 15/16
        Patching output file 16/16
        lookup cache used 256/65536 hits=882 misses=258
        finished...

![image](https://yuncodeweb.oss-cn-hangzhou.aliyuncs.com/uploads/edward.yangx/public-docs/b9ea5662e866913efcaca28549d9f033/image.png)

- **make repo-list: 列出当前工程引用的外部组件和它们的云端仓库地址**

        LITE-utils $ make repo-list
        [external/log] git@gitlab.alibaba-inc.com:iot-middleware/LITE-log.git

> 以上输出表示在构建单元`external/log`, 引用了云端仓库位于`git@gitlab.alibaba-inc.com:iot-middleware/LITE-log.git`的外部组件`LITE-log`

- **make repo-update: 将所有引用的外部组件用云端仓库更新本地仓库**

        LITE-utils $ make repo-update
        [ LITE-log.git ] <= : git@gitlab.alibaba-inc.com:iot-middleware/LITE-log.git :: master
        + cd /home/edward/srcs/iot-middleware/LITE-utils/packages
        + rm -rf LITE-log.git
        + git clone -q --bare -b master --single-branch git@gitlab.alibaba-inc.com:iot-middleware/LITE-log.git LITE-log.git
        + rm -rf LITE-log.git/hooks/
        + mkdir -p LITE-log.git/hooks/
        + touch LITE-log.git/hooks/.gitkeep
        + touch LITE-log.git/refs/heads/.gitkeep LITE-log.git/refs/tags/.gitkeep
        + rm -rf /home/edward/srcs/iot-middleware/LITE-utils.pkgs/LITE-log
        + cd /home/edward/srcs/iot-middleware/LITE-utils
        + set +x

> 以上输出表示从`git@gitlab.alibaba-inc.com:iot-middleware/LITE-log.git`拉取最新的云端仓库, 更新到本地仓库`packages/LITE-log.git`

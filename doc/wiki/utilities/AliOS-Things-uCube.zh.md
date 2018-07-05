[EN](AliOS-Things-uCube) | 中文

# 目录
- [1 环境配置](#1-环境配置)
    - [1.1 python](#11-python)
    - [1.2 ucube](#12-ucube)
    - [1.3 工具链](#13-工具链)
- [2 新建](#2-新建)
- [3 组件](#3-组件)
    - [3.1 组件信息](#31-组件信息)
    - [3.2 添加组件](#32-添加组件)
    - [3.3 删除组件](#33-删除组件)
- [4 IDE支持](#4-IDE支持)
------
**AliOS-Things uCube** 是 AliOS-Things 项目开发管理工具（简写命令为 aos）：
1. 编译代码、Image下载、板子调试。
2. 创建模板工程，基于模板做再次开发。
3. 支持组件化，获取组件信息，组件的自由组合，满足业务场景的不同需求。

# 1 环境配置 
## 1.1 python
uCube 基于 Python（Version：2.7）语言开发，需要有 Python（Version：2.7）开发环境（Python 2.7.14 下验证测试通过）。
## 1.2 ucube
使用 pip 安装 uCube 工具（在 MacOS 和 Linux上需要 sudo）：  

`$ sudo pip install -U aos-cube`  

安装完成后， `aos --version` (aos-cube 简写命令）输出版本号。

## 1.3 工具链
使用 uCube 的编译、工程创建等功能，用到工具链。工具链的配置，参考 [AliOS Things Environment Setup](AliOS-Things-Environment-Setup) 系统环境配置 -> 交叉工具链配置。
# 2 新建
## 2.1 工程
`aos new helloworld`, 会新建 helloworld 目录，并在 helloworld 创建工程，进入 helloword 目录，`aos make helloworld@mk3060` 编译
## 2.1 组件
`aos new -c helloworld`，会新建 helloworld 组件目录。

# 3 组件
## 3.1 组件信息
`aos ls -c` 可以查看 AliOS-Things 所有的组件：
```
                                                      AliOS-Things COMPONENTS
|===================================================================================================================|
| NAME                           | LOCATION                                                                         |
| mbedtls                        | aos/security/mbedtls                                                             |
| stm32l071kb                    | aos/platform/mcu/stm32l0xx/stm32l071kb                                           |
| device_sal_mk3060              | aos/device/sal/wifi/mk3060                                                       |
| wsf                            | aos/framework/connectivity/wsf                                                   |
| wifimonitor                    | aos/example/wifimonitor                                                          |
| rhino_test                     | aos/test/testcase/kernel/rhino_test                                              |
| tls_client                     | aos/example/tls                                                                  |
| newlib_stub                    | aos/utility/libc                                                                 |
| hal                            | aos/kernel/hal                                                                   |
| coap                           | aos/framework/connectivity/coap                                                  |
......
|===================================================================================================================|
```

## 3.2 添加组件
在 AliOS-Things 工程目录下面（aos new PATH），添加组件：  
```
# aos add mesh
[AliOS-Things] Add component mesh (local) in aos/kernel/protocols/mesh
```  
mesh 组件添加成功。
组件添加后，基于组件功能开发。

uCube 支持通过 URL 添加组件（新建组件参考 [Add a new component example](Add-a-new-component-example))：
```
# aos add https://github.com/AliOS-Things/test_component.git
[AliOS-Things] Adding component "test_component" from "https://github.com/AliOS-Things/test_component.git" at latest revision in the current branch
[AliOS-Things] Add component test_component (remote) in test_component
```
URL 添加远程组件。

## 3.3 删除组件
```
# aos rm mesh
[AliOS-Things] Remove component mesh (local) in aos/kernel/protocols/mesh
```

## 4 IDE支持
uCube组件化功IDE支持正在开发中，敬请期待。

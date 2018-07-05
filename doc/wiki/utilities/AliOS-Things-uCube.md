EN | [中文](AliOS-Things-uCube.zh)

# Content

- [1 Environment setup](#1-环境配置)
  - [1.1 python](#11-python)
  - [1.2 ucube](#12-ucube)
  - [1.3 toolchain](#13-工具链)
- [2 Create](#2-新建)
- [3 Component](#3-组件)
  - [3.1 component information](#31-组件信息)
  - [3.2 add component](#32-添加组件)
  - [3.3 delete component](#33-删除组件)
- [4 IDE support](#4-IDE支持)

------

**AliOS-Things uCube** is AliOS-Things project management system (abbreviation command is aos). It can

1. compile code, download image and debug board.
2. create engineering template, and redevelop based on it.
3. obtain component information, and support free combination of components, according to different needs of the business scenarios.

# 1 Environment setup

## 1.1 python

UCube development based on Python (Version:2.7) needs Python (Version:2.7) development environment (pass Python 2.7.14 verification test).

## 1.2 ucube

Install uCube through pip (sudo is needed in MacOS and Linux):  

`$ sudo pip install -U aos-cube`  

When installation is completed,  `aos --version` (abbreviation of AliOS-Things uCube) outputs version number.

## 1.3 toolchain

Toolchain is needed when using uCube to compile code and create projects. The configuration of toolchain can refer to [AliOS Things Environment Setup](AliOS-Things-Environment-Setup) (system environment setup -> toolchain configuration).

# 2 Create

## 2.1 project

`aos new helloworld `  can create new helloworld directory, and create a new project. You can compile in `aos make helloworld@mk3060` 

## 2.1 component

`aos new -c helloworld` can create helloworld components.

# 3 Component

## 3.1 component information

`aos ls -c` can see all the components of AliOS-Things:

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

## 3.2 Add components

Add new components in AliOS-Things project (aos new PATH):  

```
# aos add mesh
[AliOS-Things] Add component mesh (local) in aos/kernel/protocols/mesh
```

Component mesh has been successfully added.
When component is added, you can develop its function.

uCube supports new component adding through URL (can refer to [Add a new component example](Add-a-new-component-example)):

```
# aos add https://github.com/AliOS-Things/test_component.git
[AliOS-Things] Adding component "test_component" from "https://github.com/AliOS-Things/test_component.git" at latest revision in the current branch
[AliOS-Things] Add component test_component (remote) in test_component
```

URL can add remote components.

## 3.3 delete components

```
# aos rm mesh
[AliOS-Things] Remove component mesh (local) in aos/kernel/protocols/mesh
```

## 4 IDE support

IDE support is coming soon.

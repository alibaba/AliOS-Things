@page cplusplus cplusplus

# 概述
AliOS Things支持C++ 11相关特性，并且将内核基本模块功能抽象出了namespace AliOS，供用户使用。
组件支持标准的C++ 11特性：
- 基本类的特性，继承等；
- exception catch；
- tuple数据结构
- 时钟延时等操作
- 线程创建操作等
- singleton单例模式
- 智能指针
- 右值引用
- RTTI运行阶段类型识别
- lambda匿名函数
- cond条件变量

组件自定义的namespace AliOS类：
- thread类
- Timer类
- Semaphore类
- Mutex类
- Queue类
- WorkQueue类

## 版权信息
> Apache license v2.0

## 目录结构
```sh
├── mutex.cpp           # AliOS: Mutex类
├── queue.cpp           # AliOS: Queue类
├── semaphore.cpp       # AliOS: Semaphore类
├── thread.cpp          # AliOS: thread类
├── timer.cpp           # AliOS: Timer类
├── workQueue.cpp       # AliOS: WorkQueue类
├── cpp_mem.cpp         # C++ new/delete适配
├── cpp_init.c          # C++ ctor基础功能初始化
├── example             # C++ 功能示例
├── package.yaml        # 编译配置文件
├── include             # 对外统一引用**alios_cpp.h**即可
│   ├── cpp_mem.h
│   ├── cpp_mutex.h
│   ├── cpp_queue.h
│   ├── cpp_semaphore.h  
│   ├── cpp_thread.h
│   ├── cpp_timer.h
│   └── cpp_workQueue.h
└── alios_cpp.h         # 对外统一头文件
```

## 依赖组件
* rhino

# 常用配置
无

# API说明
其中C++ 11标准使用参考相关C++标准；
AliOS Things自定义的内核相关功能类参考：
@ref cpp_aos_init
@ref cpp_aos_workqueue
@ref cpp_aos_timer
@ref cpp_aos_thread
@ref cpp_aos_sem
@ref cpp_aos_queue
@ref cpp_aos_mutex

# 使用示例
如果需要使用cplusplus的功能，需要注意下面内容：
- 需要首先调用cpp_init初始化cpp基础功能，一般aos_component_init中已经调用
- 需要关注相应的c++ 编译选项

标准C++功能使用示例参考**example/cpp_standard**，AliOS命令空间的内核类使用参考**example/cpp_aos**。

以运行helloworld_demo为例，添加运行组件的具体步骤如下：

## 添加示例代码
> cplusplus组件的package.yaml中添加example
```sh
source_file:
  - example/cpp_aos/*.c
  - example/cpp_aos/*.cpp
  - example/cpp_standard/*.c
  - example/cpp_standard/*.cpp
```
其中由于example/cpp_aos/basic_test.cpp要测试**catch**异常功能，需要编译选项加入**-fexceptions**。在实际使用中不使用该选项有利于降低空间大小。
```sh
build_config:
  cxxflag: '-fexceptions'
```

## 添加cplusplus组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - cplusplus: dev_aos # helloworld_demo中引入cplusplus组件
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

## cpp示例测试
### 运行标准C++示例
> CLI命令行输入：

```sh
cpp_stand
```

> CLI关键日志：
```sh
timed mutex test ok
thead test ok
conditon varialbe test ok
static singleton test ok
shared ptr test ok
lamda test ok
tuple test ok
rvalue test ok
atomic test ok
```
说明：上面是主要模块运行成功的日志打印，屏蔽了一些中间输出，以及hellworld_demo自身的循环输出，实际运行如果没有出现**test error**字样即代表功能运行正常。

### 运行aos封装类
> CLI命令行输入：

```sh
cpp_aos
```

> CLI关键日志：
```sh
hello world C!
hello world C++!
******（有省略）
demo_task1 count  9
demo_task1 lock
demo_task2 count  9
demo_task1 unlock
demo_task2 lock
demo_task2 unlock
demo_task2 count  4
```

# 注意事项
> rtti_test.cpp测试依赖打开rtti功能(dynamic_cast或typeid)，因此不能使用**-fno-rtti**编译选项。
> basic_test.cpp测试catch异常功能，因此依赖选项**-fexceptions**。

在正常使用时，按照自身需求，可以增加编译选项**-fno-rtti**，或去除编译选项**-fexceptions**来达到降低版本大小的目的。

# FAQ



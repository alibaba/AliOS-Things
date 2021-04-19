@page cplusplus cplusplus

# 概述
## AliOS Things C++组件支持C++11、C++库及STL，支持主要特性如下:
- 基本类特性，包括继承、模板、多态等；
- exception catch；
- tuple
- 时钟延时等操作
- 线程创建操作等
- singleton单例模式
- 智能指针
- 右值引用
- RTTI运行时类型识别
- lambda匿名函数
- cond条件变量

## AliOS Things C++组件对内核接口进行了封装，提供了一组自定义的类(原生类)。使用自定义类的优势是资源消耗少。这些原生类在自定义的命名空间AOS中。自定义类包括：
- thread线程类
- Timer软件定时器类
- Semaphore信号量类
- Mutex互斥信号量类
- Queue队列类
- WorkQueue工作队列类

## 版权信息
> Apache license v2.0

## 目录结构
```sh
├── mutex.cpp           # AOS: Mutex类
├── queue.cpp           # AOS: Queue类
├── semaphore.cpp       # AOS: Semaphore类
├── thread.cpp          # AOS: thread类
├── timer.cpp           # AOS: Timer类
├── workQueue.cpp       # AOS: WorkQueue类
├── cpp_mem.cpp         # C++ new/delete适配
├── cpp_init.c          # C++ C++初始化,将执行静态对象构造
├── example             # C++ 功能示例
├── package.yaml        # 编译配置文件
├── include             # 对外统一引用**aos_cpp.h**即可
│   ├── cpp_mem.h
│   ├── cpp_mutex.h
│   ├── cpp_queue.h
│   ├── cpp_semaphore.h
│   ├── cpp_thread.h
│   ├── cpp_timer.h
│   └── cpp_workQueue.h
└── aos_cpp.h           # 对外统一头文件
```

## 依赖组件
* rhino

# 常用配置
无

# API说明
## C++ 11标准使用参考相关C++标准
## AliOS Things自定义类使用说明如下
- 使用AliOS Things自定义类需要先声明AOS命令空间
> using namespace AOS;
- 各自定义类相关说明参见如下链接:
@ref cpp_aos_workqueue
@ref cpp_aos_timer
@ref cpp_aos_thread
@ref cpp_aos_sem
@ref cpp_aos_queue
@ref cpp_aos_mutex

# 使用示例
如果需要使用cplusplus的功能，需要注意下面内容：
- 需要首先调用cpp_init初始化cpp基础功能，添加对cplusplus组件的依赖后aos_component_init中自动会调用
- 需要关注相应的c++ 编译选项

标准C++使用示例参考**example/cpp_standard**，AliOS自定义类使用参考**example/cpp_aos**。

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
  - cplusplus: master # helloworld_demo中引入cplusplus组件
```

## 编译与下载
开发环境的搭建请参考 @ref HaaS100_Quick_Start (搭建开发环境章节)，其中详细的介绍了AliOS Things 3.3的IDE集成开发环境的搭建流程。

helloworld_demo与cplusplus的代码下载请参考 @ref HaaS100_Quick_Start (创建工程章节)，

*> 选择解决方案: “helloworld简单示例”*

*> 选择开发板: Haas100 board configure*

-- 参考 @ref HaaS100_Quick_Start (3.1 编译工程章节)，点击 ✅ 即可完成编译固件。

-- 参考 @ref HaaS100_Quick_Start (3.2 烧录镜像章节)，点击 "⚡️" 即可完成烧录固件。

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
Total area: 35
Total paint cost: $2450
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
Q1： 在编译时出现错误提示要加选项:error: exception handling disabled, use -fexceptions to enable
     或者编译带try-catch的代码时提示变量未定义:error: 'msg' was not declared in this scope
> 答：这是因为默认关闭了-fexceptions选项引起的，可在package.yaml中搜索语句#cxxflag: '-fexceptions', 把前面的#号去掉，使能该选项。注：使能该选项后将导致镜像变大。


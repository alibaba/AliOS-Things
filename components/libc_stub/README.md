@page libc_stub libc_stub

# 概述
libc_stub组件是对C库桩函数（stubs）的实现，用于AliOS Things与C库的对接。支持gcc(newlibc)、icc、armcc三种类型编译器的C库。
对接的功能包括：
- 内存申请释放：malloc/free/realloc/calloc
- 错误码errno记录、异常abort
- 文件系统相关open/read/write/close等
- 输出printf

该组件只对接C库相关接口，posix标准接口请参考posix组件；文件系统相关请参考vfs组件。

## 版权信息
> Apache license v2.0

## 目录结构
```sh
├── newlib_stub.c # gcc newlibc库的重定向实现
├── compilers
│   ├── armlibc   # armcc库的重定向实现
│   ├── gcc       # gcc对接头文件修改
│   └── iar       # icc库的重定向实现
└── package.yaml  # 编译配置文件
```

## 依赖组件
* osal_aos
* vfs

# 常用配置
无

# API说明
参考标准C库接口即可。

# 使用示例
目前使用的gcc编译器都是使用的newlibc库，因此一般都要加上**libc_stub**组件。如果采用其他C库如musl库，则不需要添加。

使用示例参考**example/libc_stub_example.c**，

以运行helloworld_demo为例，添加运行组件的具体步骤如下：

## 添加示例代码
> libc_stub组件的package.yaml中添加example
```sh
source_file:
  - "example/*.c"
```
由于libc_stub_example.c中要测试open文件系统相关接口，因此需要依赖文件系统，本示例以通用的ramfs为例，增加ramfs组件
```sh
depends:
  - ramfs：master
```

## 添加libc_stub组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - libc_stub: master # helloworld_demo中引入libc_stub组件
```
## 编译与下载
开发环境的搭建请参考 @ref HaaS100_Quick_Start (搭建开发环境章节)，其中详细的介绍了AliOS Things 3.3的IDE集成开发环境的搭建流程。

helloworld_demo与libc_stub组件的代码下载请参考 @ref HaaS100_Quick_Start (创建工程章节)，

*> 选择解决方案: “helloworld简单示例”*

*> 选择开发板: Haas100 board configure*

-- 参考 @ref HaaS100_Quick_Start (3.1 编译工程章节)，点击 ✅ 即可完成编译固件。

-- 参考 @ref HaaS100_Quick_Start (3.2 烧录镜像章节)，点击 "⚡️" 即可完成烧录固件。

## libc_stub示例测试
> CLI命令行输入：
```sh
libc_example
```

## 关键日志
> CLI日志：
```sh
libc_stub: malloc OK!
libc_stub: ramfs test success!
libc_stub comp test success!
```

# 注意事项
该模块只对接内核启动相关必要的接口，其他posix标准接口请参考posix组件；文件系统相关请参考vfs组件。

# FAQ



## Contents

## Introduction
**RPC** 提供进程之间服务调用的模块化开发方法, 服务提供者通过注册RPC服务供服务使用方使用

### Features
- 服务注册
- 服务参数列表的标准序列化方法

### Directories

```sh
rpc
├── aos.mk
├── Config.in
├── include
│   └── rpc.h  # RPC对外暴露的API和数据结构
└── rpc.c      # RPC主要的逻辑

```

### Dependencies
依赖于序列封装模块Parcel

## Reference
- [AliOS Things VFS Porting Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-VFS-Porting-Guide)

@page armv8m armv8m

# 概述
**armv8m** 使AliOS Things支持armv8m架构的CPU(包括cortex-m33)。主要提供以下功能：
- 中断管理与分发处理
- 异常处理，包括crash处理与栈回溯
- 任务切换，任务栈初始化等
- 向量表定义，系统启动入口等

## 版权信息
> Apache license v2.0

## 目录结构
```tree
armv8m/
|-- common
|   |-- backtrace.c
|   |-- panic_c.c
|   |-- panic_gcc.sx
|   `-- port_c.c
|-- gcc
|   `-- m33
|-- include
|   |-- backtrace.h
|   |-- k_compiler.h
|   |-- k_types.h
|   `-- port.h
|-- package.yaml  # AOS编译配置文件
`-- README.md
```

## 依赖组件
无

# 常用配置
无

# 注意事项

# FAQ

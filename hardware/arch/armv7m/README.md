@page armv7m armv7m

# 概述
**armv7m** 使AliOS Things支持armv7m架构的CPU(包括cortex-m3/cortex-m4/cortex-m7)。主要提供以下功能：
- 中断管理与分发处理
- 异常处理，包括crash处理与栈回溯
- 任务切换，任务栈初始化等
- 向量表定义，系统启动入口等

## 版权信息
> Apache license v2.0

## 目录结构
```tree
armv7m
|-- armcc
|   |-- m3
|   |-- m4
|   `-- m7
|-- common
|   |-- backtrace.c
|   |-- include
|   |-- panic_armcc.S
|   |-- panic_c.c
|   |-- panic_gcc.sx
|   |-- panic_iccarm.S
|   |-- panic_mpu.c
|   `-- port_c.c
|-- gcc
|   |-- m3
|   |-- m4
|   `-- m7
|-- iccarm
|   |-- m3
|   |-- m4
|   `-- m7
|-- include
|   |-- backtrace.h
|   |-- k_compiler.h
|   |-- k_types.h
|   `-- port.h
|-- package.yaml # AOS 编译配置文件
`-- README.md
```

## 依赖组件
无

# 常用配置
无

# 注意事项

# FAQ

# 介绍
CPU架构相关的功能实现。此目录为armv8m的实现。
主要包括异常向量、启动、任务切换、进程管理、内存映射、debug等的功能。

# 特性
- 异常向量、启动
- 任务切换、中断管理
- 进程启动管理
- 内存映射MMU、cache
- debug维测

# 目录

├── aos.mk
├── common
│   ├── include
│   ├── iomap.c                    # 开放给用户态映射内存空间，一般是寄存器空间
│   ├── k_asid.c                   # 进程pid管理
│   ├── k_proc.c                   # 进程启动加载
│   ├── mpu.c                      # V8M系列MPU功能
│   ├── port_c.c                   # 任务栈初始化
│   ├── svc_debug.c                # SVC维测 
│   └── svc_gcc.S                  # SVC gcc处理
├── Config.in
├── debug
│   └── debug.a                    # 内核维测功能集
├── gcc
│   └── m33
│       └── port_s.S               # 任务调度相关汇编
├── include
└── README.md

# 依赖
无

# 使用
在对应MCU的aos.mk中添加组件armv8m-mk，即可管理对应的CPU架构，如：
$(NAME)_COMPONENTS += armv8m-mk

# 参考
N/A
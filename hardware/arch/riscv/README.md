@page riscv riscv

# 概述
**riscv** 使AliOS Things支持risc-v 32/risc-v 64架构的CPU(包括RV32I/RV64I基础指令集，及其标准扩展)。主要提供以下功能：
- 中断管理与分发处理
- 异常处理，包括crash处理与栈回溯
- 任务切换，任务栈初始化等
- 中断栈/异常栈定义，系统启动入口等

## 版权信息
> Apache license v2.0

## 目录结构
```tree
riscv/
|-- include
|   |-- rv32_32gpr
|   |-- rv32fd_32gpr
|   `-- rv64fd_32gpr
|-- package.yaml   # AOS编译配置文件
|-- README.md
`-- src
    |-- rv32_32gpr
    |-- rv32fd_32gpr
    `-- rv64fd_32gpr
```

## 依赖组件
无

# 常用配置
无

# 注意事项

# FAQ

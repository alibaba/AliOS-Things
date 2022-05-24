# 概述
**c906** 是基于smarth_rv64芯片的qemu虚拟板子。

# 目录结构
```tree
c906/
|-- adapter   # 适配AliOS Things
|-- config
|-- csi_core  # 平头哥CSI CORE标准
|-- package.yaml # AOS编译配置文件
|-- README.md
|-- startup     # 启动文件
`-- VERSION
```

# 快速开始
## 准备环境
首先检查Host环境下是否已安装qemu，执行命令`which qemu-system-riscv64`查看。 如果没有安装，需要先按照qemu环境。

## 编译
编译AliOS Things的应用。
```bash
$ cd <aos_dir>/solutions/helloworld_demo
$ aos make -b c906
```

## 运行
```bash
qemu-system-riscv64 -M smarth -kernel out/helloworld_demo@c906.elf -nographic
```

## 退出
按下 `Ctrl + a`, 然后按下 `x`退出qemu， 终端显示`(ash)# QEMU: Terminated`

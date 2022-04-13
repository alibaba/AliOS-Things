# 介绍
AliOS Things系统调用实现

# 特性
- 支持系统调用过程中进行句柄-对象转换
- 支持用户自定义系统调用
- 内核部分实现与用户态部分实现解耦合

# 目录
```sh

syscall
├── include                       #不同模块系统调用函数的参数结构体描述
│   ├── aos_iomap_syscall_arg.h
│   ├── aos_obj_syscall_arg.h
│   ├── aos_syscall_arg.h
│   ├── cli_syscall_arg.h
│   ├── debug_syscall_arg.h
│   ├── epoll_syscall_arg.h
│   ├── hal_flash_syscall_arg.h
│   ├── hal_uart_syscall_arg.h
│   ├── kv_syscall_arg.h
│   ├── netmgr_syscall_arg.h
│   ├── network_syscall_arg.h
│   ├── rhino_syscall_arg.h
│   ├── select_syscall_arg.h
│   ├── signal_syscall_arg.h
│   ├── syscall_no.h               #按功能划分的位置固定的系统调用号定义
│   ├── uirq_syscall_arg.h
│   ├── ulog_syscall_arg.h
│   └── vfs_syscall_arg.h
├── ksyscall                       #系统调用的内核部分实现
│   ├── aos_iomap_ksyscall.c
│   ├── aos_ksyscall.c
│   ├── aos.mk
│   ├── aos_obj_ksyscall.c
│   ├── cli_ksyscall.c
│   ├── Config.in
│   ├── debug_ksyscall.c
│   ├── dyn_syscall.c
│   ├── epoll_ksyscall.c
│   ├── hal_flash_ksyscall.c
│   ├── hal_uart_ksyscall.c
│   ├── hal_wdg_ksyscall.c
│   ├── include
│   ├── irq_ksyscall.c
│   ├── irq_ksyscall_gic.c
│   ├── irq_ksyscall_nvic.c
│   ├── kv_ksyscall.c
│   ├── netmgr_ksyscall.c
│   ├── network_ksyscall.c
│   ├── rhino_ksyscall.c
│   ├── select_ksyscall.c
│   ├── signal_ksyscall.c
│   ├── syscall_tbl.c               #系统调用表实现
│   └── vfs_ksyscall.c
└── usyscall                        #系统调用用户态部分实现
    ├── aos_iomap_usyscall.c
    ├── aos.mk
    ├── aos_obj_usyscall.c
    ├── aos_usyscall.c
    ├── cli_usyscall.c
    ├── Config.in
    ├── debug_usyscall.c
    ├── epoll_usyscall.c
    ├── hal_flash_usyscall.c
    ├── hal_uart_usyscall.c
    ├── hal_wdg_usyscall.c
    ├── include
    ├── irq_usyscall.c
    ├── kv_usyscall.c
    ├── netmgr_usyscall.c
    ├── network_usyscall.c
    ├── rhino_usyscall.c
    ├── select_usyscall.c
    ├── signal_usyscall.c
    ├── svc_asm
    └── vfs_usyscall.c

```
# 依赖
- rhino
- osal_aos
- kobjs

# 使用
向上层提供aos api及内核态组件的api,具体参考对应api的说明

# 参考
N/A

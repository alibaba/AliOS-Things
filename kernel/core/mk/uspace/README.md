# 介绍
用户态内存管理、线程管理、定时器、回调、CLI等

# 特性
- 支持用户态动态内存管理
- 支持用户态任务管理
- 支持用户态定时器管理
- 支持用户态执行回调函数
- 支持用户态执行CLI命令

# 目录
```sh

├── aos.mk
├── Config.in
├── include
├── README.md
└── src
    ├── callback_task.c     #用户态回调函数执行
    ├── cli_task.c          #用户态CLI命令执行
    ├── debug               #调试打印相关
    │   ├── debug_api.c
    │   ├── debug_print.c
    │   └── debug_test.c
    ├── res_task.c
    ├── uassert.c
    ├── umm                 #用户态动态内存管理
    │   ├── u_mm_blk.c
    │   ├── u_mm.c
    │   └── u_mm_debug.c
    ├── utask.c             #用户态线程管理
    └── utimer.c            #用户态定时器管理

```
# 依赖
- usyscall

# 使用
N/A

# 参考
N/A

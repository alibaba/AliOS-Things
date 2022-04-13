# 介绍
微内核进程管理、进程间通信IPC、回调Callback、CLI、Signal实现

# 特性
- 支持进程创建和资源管理
- 支持进程间通信
- 支持内核回调用户态接口
- 支持内核管理用户态注册的CLI命令
- 支持Signal

# 目录
```sh

├── aos.mk
├── Config.in
├── core_ipc             #进程间通信IPC
│   ├── aos.mk
│   ├── Config.in
│   ├── include
│   ├── ipc.c
│   ├── ipc_fifo.c
│   ├── ipc_msg.c
│   ├── ipc_perf.c
│   ├── ipc_queue.c
│   ├── ipc_str.c
│   ├── ipc_stream.c
│   └── ipc_uds.c
├── core_src              #进程创建、回调Callback、CLI、Signal实现
│   ├── ash.c
│   ├── callback.c
│   ├── fsfd.c
│   ├── ipc.c
│   ├── res.c
│   ├── socketfd.c
│   ├── task_group.c
│   ├── ucli.c
│   ├── usignal.c
│   ├── uspace_init.c
│   └── utask.c
├── include
├── mksh                  #进程资源管理
│   ├── aos.mk
│   └── mksh.c
└── README.md

```
# 依赖
- ksyscall
- mksh
- core_ipc

# 使用
N/A

# 参考
N/A

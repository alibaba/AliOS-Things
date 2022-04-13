## Contents

## Introduction
**IPC** 提供进程之间高速并发无锁的通信方式，具体包括FIFO/MSG/STREAM类型的通信

### Features
- IPC对象的创创建删除;
- IPC对象的高速并发无锁通信

### Directories

```sh
core_ipc
├── aos.mk
├── Config.in
├── include
│   ├── kipc_perf.h  # IPC内部用于性能统计
│   ├── kipc_queue.h # 不同的IPC类型统一抽象模型
│   └── kipc_utils.h # IPC内部使用的工具
├── ipc.c            # IPC对外接口封装实现
├── ipc_fifo.c       # FIFO类型的IPC实现
├── ipc_msg.c        # MSG类型的IPC实现
├── ipc_perf.c       # IPC性能统计工具
├── ipc_queue.c      # IPC统一抽象模型实现
├── ipc_str.c        # IPC内部使用的工具函数
├── ipc_stream.c     # STREAM类型的IPC实现
├── ipc_uds.c        # 基于IPC实现的UDS

```

### Dependencies

## Reference
- [AliOS Things VFS Porting Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-VFS-Porting-Guide)

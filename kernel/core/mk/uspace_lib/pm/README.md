# 介绍
AliOS Things用户态进程加载卸载接口

# 特性
- 支持用户态创建并启动某进程
- 支持用户态卸载某进程

# 目录
```sh

├── aos.mk
└── pm_req
    ├── process_req.c
    └── procmgmt.h

```
# 依赖
- rpc

# 使用
int aos_process_load(char *procname)
用户传入需要加载的bin的全路径名，来启动一个进程；也可通过在CLI中输入“loadm + 文件名”来加载进程
int aos_process_unload(char *procname)
用户传入需要卸载的bin的全路径名，来启动一个进程；也可通过在CLI中输入“kill + 进程pid”来加载进程；
其中，CLI中可输入"ps"命令。来查看当前进程状态。

# 参考
N/A
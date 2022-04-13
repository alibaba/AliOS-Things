# 介绍
用户态aos api实现

# 特性
- 无

# 目录
```sh

aos
├── aos.mk
├── aos_socket.c       #用户态aos socket实现
├── fs_rpc_server.c    #用户态fs rpc服务端接口实现
├── fs_rpc_task.c      #用户态fs rpc处理线程
├── include            #对外头文件
│   ├── aos_socket.h
│   ├── dev_log.h
│   ├── fs_rpc.h
│   ├── usr_vfs.h
│   └── vfs_state.h
├── kv.c                #用户态kv接口实现
├── uspace_aos.c        #用户态基础aos api接口实现
├── usr_vfs.c           #用户态VFS RPC client接口实现
├── vfs.c               #用户态VFS接口实现
└── vfs_state.c         #用户态VFS数据结构操作函数的集合

```
# 依赖
- VFS
- KV
- uspace

# 使用
N/A

# 参考
N/A

# 介绍
内核对象模型框架。

# 特性
- 资源管理，被内核对象化的组件无需额外考虑进程资源回收问题。
- 有名共享，提供跨进程的资源共享能力。
- 安全隔离，提供进程独立的句柄映射能力，确保内核资源安全访问。


# 目录
```sh

kobj_core/
├── aos.mk                 #AliOS Things的makefile
├── Config.in              #AliOS Things的menuconfig配置文件
├── include
│   ├── object_api.h       #对外头文件
│   ├── object_common.h
│   └── private
├── lib
│   ├── cortex-a9          #arm cortex-a9闭源库
│   └── cortex-m33         #arm cortex-m33闭源库
└── README.md

```
# 依赖
- rhino

# 使用
可使用此框架对内核内核组件如信号量/锁等进行对象化封装，使得具备特性描述的能力。

具体可参照kernel/core/osal/kobjs中的组件封装；
以及kernel/core/mk/ksyscall/aos_obj_ksyscall.c中的内容查看句柄与对象的转换。

# 参考
N/A

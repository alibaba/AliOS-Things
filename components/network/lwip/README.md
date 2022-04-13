# 介绍
lwip是一款开源的TCP/IP协议栈。

# 特性
- 支持TCP/IP网络接口通信。
- 可运行在用户态或内核态
- 提供RPC服务处理跨进程网络资源的访问

# 目录
```sh
LWIP
├── README.md       #要点介绍
├── api             #API实现
├── apps            #网络应用
├── core            #核心基础功能
├── include         #头文件
├── netif           #网络接口实现
├── port            #系统相关移植接口实现
├── rpc             #RPC socket接口服务
├── Filelists.mk    #构建源文件配置脚本
├── Config.in       #构建配置
└── aos.mk          #构建脚本

```
# 依赖
N/A

# 使用
    用户可使用兼容musllibc的socket接口通过lwip进行网络通信。

# 参考
N/A
@page lwip lwip

# 概述
LwIP(Light Weight Internet Protoco1)是瑞士计算机科学院(Swedish Institute of Computer Science)AdamDunkels等人开发的一套用于嵌入式系统的开放源代码TCP／IP协议栈。LWIP的含义是Light Weight(轻型)IP协议。LWIP可以移植到操作系统上，也可以在无操作系统的情况下独立运行。LWIP TCP／IP实现的重点是在保持TCP协议主要功能的基础上减少对RAM的占用。一般它只需要几十KB的RAM和40 KB左右的ROM就可以运行，这使LWIP协议栈适合在小型嵌入式系统中使用。
官网：https://savannah.nongnu.org/projects/lwip/
将LwIP协议栈分成四层：
1.应用层
2.传输层
3.网络层
4.网卡层
本文将2，3，4统称为协议层
应用层支持：
arp, dhcpd, dns, httpd, ifconfig, iperf, lsfd, mdns, netbiosns, ping, sendfile, snmp, sntp, telnetd, tftp

协议层支持：
IP (Internet Protocol, IPv4 and IPv6) including packet forwarding over
    multiple network interfaces
ICMP (Internet Control Message Protocol) for network maintenance and debugging
IGMP (Internet Group Management Protocol) for multicast traffic management
MLD (Multicast listener discovery for IPv6). Aims to be compliant with
    RFC 2710. No support for MLDv2
ND (Neighbor discovery and stateless address autoconfiguration for IPv6).
    Aims to be compliant with RFC 4861 (Neighbor discovery) and RFC 4862
    (Address autoconfiguration)
UDP (User Datagram Protocol) including experimental UDP-lite extensions
TCP (Transmission Control Protocol) with congestion control, RTT estimation
    and fast recovery/fast retransmit
raw/native API for enhanced performance
Optional Berkeley-like socket API
DNS (Domain names resolver)

## 版权信息
> Apache license v2.0

## 目录结构
```sh
├── api
│   ├── api_lib.c  # 库
│   ├── api_msg.c  # 消息机制
│   ├── err.c      # 异常api
│   ├── netbuf.c   # 网络缓存api
│   ├── netdb.c    # dns实现
│   ├── netifapi.c # 网卡api
│   ├── sockets.c  # 套接字api
│   └── tcpip.c    # tcpip任务实现
├── apps           # LwIP 应用
│   ├── arp        # arp应用
│   ├── dhcpd      # dhcpd应用
│   ├── dns        # dns应用
│   ├── httpd      # httpd应用
│   ├── ifconfig   # ifconfig应用
│   ├── iperf      # iperf应用
│   ├── lsfd       # lsfd应用
│   ├── mdns       # mdns应用
│   ├── netbiosns  # netbiosns应用
│   ├── ping       # ping应用
│   ├── sendfile   # sendfile应用
│   ├── snmp       # snmp应用
│   ├── sntp       # sntp应用
│   ├── telnetd    # telnetd应用
│   └── tftp       # tftp应用
├── core           # LwIP协议栈核心模块，IPv4/IPv6/TCP/UDP等协议实现
├── include        # 头文件
├── netif          # 网卡锡相关
└── port           # 移植对接
```

## 依赖组件

# 常用配置
系统中相关配置已有默认值，如需修改配置，统一在yaml中**def_config**节点修改，具体如下：
> 使能TCPIP是1，不使能TCPIP是0，默认是1，可修改yaml配置如
```sh
def_config:
  CONFIG_TCPIP: 1  
```
> 使用AOS的LwIP是1， 不实用AOS的LwIP是0，默认是1，可修改yaml配置如：
```sh
def_config:
  CONFIG_AOS_LWIP: 1
```
> 使能LwIP数据包打印功能是1， 不使能是0，默认1，可修改yaml配置如：
```sh
def_config:
  WITH_LWIP_PKTPRINT: 1
```

# API说明

## 创建socket
```C
int socket(int domain, int type, int protocol);
```

|args                                    |description|
|:-----                                  |:----|
|domain                                  |协议域|
|type                                    |类型|
|protocol                                |传输协议|

## 绑定地址端口
```C
int bind(int s, const struct sockaddr *addr, socklen_t namelen)
```

|args                                    |description|
|:-----                                  |:----|
|s                                       |要绑定的 socket描述符|
|addr                                    |一个指向含有本机 IP 地址和端口号等信息的 sockaddr 结构的指针|
|namelen                                 |sockaddr 结构的长度|

## 将套接字设为监听模式，并在套接字指定的端口上开始监听，以便对到达的服务请求进行处理
```C
int listen(int s, int backlog)
```

|args                                    |description|
|:-----                                  |:----|
|s                                       |要绑定的 socket描述符|
|backlog                                 |连接请求队列可以容纳的最大数目|

## 从完全建立的连接的队列中接受一个连接
```C
int accept(int s, struct sockaddr *addr, socklen_t *addrlen)
```

|args                                    |description|
|:-----                                  |:----|
|s                                       |socket描述符|
|backlog                                 |连接请求队列可以容纳的最大数目|

## 与服务器建立一个 TCP 连接
```C
int connect(int s, const struct sockaddr *name, socklen_t namelen)
```

|args                                    |description|
|:-----                                  |:----|
|s                                       |socket描述符|
|name                                    |指向 sockaddr 结构的指针，存放要连接的服务器的 IP 地址和端口号等信息|
|namelen                                 |sockaddr 结构体的长度|

## 面向连接的数据流 socket 模式下发送数据
```C
int send(int s, const void *dataptr, size_t size, int flags)
```

|args                                    |description|
|:-----                                  |:----|
|s                                       |socket描述符|
|dataptr                                 |指向所要发送的数据区的指针|
|size                                    |要发送的字节数|
|flags                                   |控制选项，通常为 0|

## 面向连接的数据流 socket 模式下接收数据
```C
int send(int s, const void *dataptr, size_t size, int flags)
```

|args                                    |description|
|:-----                                  |:----|
|s                                       |socket描述符|
|mem                                     |指向存储数据的内存缓存区的指针|
|len                                     |缓冲区的长度|
|flags                                   |控制选项，通常为 0|

## 在无连接的数据报 socket 模式下发送数据
```C
int sendto(int s, const void *dataptr, size_t size, int flags, const struct sockaddr *to, socklen_t tolen)
```

|args                                    |description|
|:-----                                  |:----|
|s                                       |socket描述符|
|size                                    |要发送的字节数|
|flags                                   |控制选项，通常为 0|
|to                                      |指向 sockaddr 结构体的指针，存放目的主机的 IP 和端口号|
|tolen                                   |sockaddr 结构体的长度|

## 在无连接的数据报 socket 模式下接收数据
```C
int recvfrom(int s, void*mem, size_t size, int flags, struct sockaddr *from, socklen_t *fromlen)
```

|args                                    |description|
|:-----                                  |:----|
|s                                       |socket描述符|
|mem                                     |指向存储数据的内存缓存区的指针|
|size                                    |缓冲区的长度|
|flags                                   |控制选项，通常为 0|
|from                                    |指向 sockaddr 结构体的指针，存放源主机的 IP 和端口号|
|fromlen                                 |指向 sockaddr 结构体的长度的指针|

## 查询一个或者多个socket的可读性、可写性及错误状态信息
```C
int select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
                struct timeval *timeout)
```

|args                                    |description|
|:-----                                  |:----|
|maxfdp1	                               |最大的文件描述符|
|readset                                 |读文件描述符|
|writeset                                |写文件描述符|
|exceptset                               |异常的文件描述符|
|timeout                                 |超时时间|

## 传输完数据之后关闭 socket 并释放资源
```C
int closesocket(int s)
```

|args                                    |description|
|:-----                                  |:----|
|s                                       |socket 描述符|

## 允许进行单向的关闭操作，或是全部禁止掉
```C
int shutdown(int s, int how)
```

|args                                    |description|
|:-----                                  |:----|
|s                                       |socket 描述符|
|how                                     |控制选项|

## 通过域名来获取主机的 IP 地址等信息
```C
struct hostent* gethostbyname(const char*name)
```

|args                                    |description|
|:-----                                  |:----|
|name	                                   |主机域名|

## 获取本地主机的信息
```C
int getsockname(int s, struct sockaddr *name, socklen_t *namelen)
```

|args                                    |description|
|:-----                                  |:----|
|s	                                     |socket 描述符|
|name	                                   |sockaddr 结构体指针，用来存储得到的主机信息|
|namelen	                               |指向 sockaddr 结构体的长度的指针|

## 得到与本地主机连接的远程主机的信息
```C
int getpeername(int s, struct sockaddr *name, socklen_t *namelen)
```

|args                                    |description|
|:-----                                  |:----|
|s	                                     |socket 描述符|
|name	                                   |sockaddr 结构体指针，用来存储得到的主机信息|
|namelen	                               |指向 sockaddr 结构体的长度的指针|

## 设置套接字控制模式
```C
int ioctlsocket(int s, long cmd, void *argp)
```

|args                                    |description|
|:-----                                  |:----|
|s	                                     |socket 描述符|
|cmd	                                   |套接字操作命令|
|argp   	                               |操作命令所带参数|

## 获取套接字控制模式
```C
int getsockopt (int s, int level, int optname, void *optval, socklen_t *optlen)
```

|args                                    |description|
|:-----                                  |:----|
|s	                                     |socket 描述符|
|level	                                 |选项定义的层次；目前支持SOL_SOCKET, SOL_PACKET, IPPROTO_IP和IPPROTO_TCP|
|optname	                               |需设置的选项|
|optval	                                 |指向option属性的指针|
|optlen	                                 |指向option属性长度的指针|

## 设置套接字控制模式
```C
int setsockopt (int s, int level, int optname, const void *optval, socklen_t optlen)
```

|args                                    |description|
|:-----                                  |:----|
|s	                                     |socket 描述符|
|level	                                 |选项定义的层次；目前支持SOL_SOCKET, SOL_PACKET, IPPROTO_IP和IPPROTO_TCP|
|optname	                               |需设置的选项|
|optval	                                 |指向option属性的指针|
|optlen	                                 |option属性的长度|

# 使用示例
示例代码参考example/udp_test.c，以运行solution/helloworld_demo为例，具体步骤如下：

## 添加示例代码
> lwip组件的package.yaml中添加example
```sh
source_file:
  - example/*.c
```

## 添加lwip组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - netmgr:dev_aos     # 添加netmgr依赖，使用netmgr连接wifi
  - lwip:dev_aos       # helloworld_demo中引入lwip组件
```

## 编译
```sh
cd solutions/helloworld_demo && aos make
```
其中具体单板还需要先配置环境：
```sh
aos make helloworld_demo@haas100 -c config
```

## 烧录固件
> 参考具体板子的快速开始文档。

## lwip 示例测试
> CLI命令行输入netmgr相关命令进行联网操作：
```sh
netmgr_example                           # 启动netmgr组件
netmgr -t wifi -c wifissid wifipassword  # 输入ssid和密码
```
> CLI命令行输入udp 测试命令：
```sh
test_udp
```
## 关键日志
> 输入联网命令后的WIFI联网成功日志：
```sh
Got IP
```
> 启动test_udp之后的数据接收打印：
```sh
test_udp

(cli-uart)# hello world! count 15
recv: hello UDP
hello world! count 16
recv: hello UDP
hello world! count 17
recv: hello UDP
hello world! count 18
recv: hello UDP
hello world! count 19
recv: hello UDP
```
# 注意事项
* 使用lwip前，请先确定已经联网

# FAQ
NA

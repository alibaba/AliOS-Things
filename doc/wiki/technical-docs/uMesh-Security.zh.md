[EN](uMesh-Security) | 中文

# AliOS Things自组织网络安全认证架构概述

AliOS Things 是 AliOS 家族旗下的、面向 IoT 领域的、轻量级物联网嵌入式操作系统。 AliOS Things 致力于搭建云端一体化 IoT 基础设施，具备极致性能、极简开发、云端一体、丰富组件、安全防护等关键能力，并支持终端设备连接到阿里云Link，可广泛应用在智能家居、智慧城市、新出行等领域。

近年随着越来越多被曝光的物联网安全漏洞问题以及遭受到的网络攻击，安全将会成为物联网生态体系要面对的一个尖锐问题，尤其是嵌入式安全因为设备的数量巨大使得常规的更新和维护操作面临挑战，而基于云的操作使得边界安全不太有效。

针对上述物联网安全的痛点，作为AliOS Things核心组件之一自组织网络（uMesh）不仅提供了AliOS Things原生自组织网络能力，本地互联互通的能力，也将更多的注意力放到了如何保障嵌入式设备能够自主安全接入自组织网络，并保证和云端数据通信的完整性和机密性。uMesh具有自组织、自修复、多跳、兼容标准端口网络访问控制和扩展认证协议，可扩展多种标准安全认证方式等特性。适用于需要大规模部署且对设备节点有安全性需求的场景，如智能家居，智能照明，及商业楼宇等场景。主要功能特性包括：

- 无缝支持IPv4 和IPv6
- 支持WiFi、BLE和802.15.4等通信媒介
- 支持不同通信媒介间的异构组网
- 支持树状拓扑和网状拓扑
- 支持低功耗特性
- 支持阿里云设备身份认证平台ID²对接入设备进行安全认证授权
- 支持AES-128共享对称密钥在全网范围内点对点的数据加密解密
- 兼容标准IEEE802.1x端口控制协议和EAP扩展认证协议（RFC3748）

### 阿里云Link ID²设备身份认证平台

[ID²(Internet Device ID)](https://iot.aliyun.com/product/id2?spm=a2c2j.8959409.5007677.4)，是一种物联网设备的可信身份标识，具备不可篡改、不可伪造、全球唯一的安全属性，是实现万物互联、服务流转的关键基础设施。

ID²设备身份认证平台由互联网设备，ID²分发中心，云端ID²认证中心和部署在本地或者云端的互联网服务组成。芯片厂商产线通过调用提供的ID²产线烧录SDK（可集成到厂商的烧录工具）完成向ID²分发中心在线的ID²申请，获取和烧录。烧录完成后，可通过调用烧录回执相关的API来确认是否已经成功烧录到芯片。具体产线烧录ID²过程可参考：[ID²申请和产线烧录](https://iot.aliyun.com/docs/security/ID2_license_application.html)。

烧录ID²的同时也会将相应的私钥（private key）烧录到芯片内，公钥（public key）会上传给云端ID²认证中心，该私钥可用于解密由云端ID²认证中心下发的加密数据，这种模式可用于实现应用层协议的通道认证或者秘钥协商。ID²的一个重要应用场景就是连接协议的安全性增强，ID²和各种连接协议（如MQTT，CoAP）结合，为连接提供设备认证和密钥协商等基础能力，为整个IoT管理系统提供基础的安全保障。后面也会介绍利用该安全特性将ID²设备身份认证平台与Alios Things自组织网络节点安全入网相结合。ID²认证平台系统架构如下：

![](/Users/maxiao/Desktop/屏幕快照 2018-01-05 下午1.52.40.png)

ID²设备身份认证平台提供了两种认证模式：基于挑战应答模式（challenge-response Based）和基于时间戳（Timestamp-Based），可防止重放（replay）攻击。以挑战应答认证模式为例，sp server（业务服务器）作为消息代理（proxy），转发设备节点和云端ID²认证中心之间的交互消息（默认设备节点已经预置烧录ID²）。具体认证消息交互流程图如下：

![](/Users/maxiao/Desktop/屏幕快照 2018-01-05 下午2.21.56.png)

- 设备端发送认证请求给sp server，向云端ID²认证中心申请挑战随机数（challenge）
- sp server调用POP SDK Java API：getServerRandom()从云端ID²认证中心获取到挑战随机数并转发给终端设备节点
- 设备节点获取到挑战随机数后，根据预置根ID²，challenge，extra data（可选）作为计算auth code的参数，调用端上提供的TFS API：tfs_id2_get_challenge_auth_code()计算auth code
- 设备节点将计算出的auth code发送给sp server, 将帮助转发给云端ID²认证中心
- sp server调用POP SDK Java API：VerifyRequest() 与云端ID²认证中心做认证
- sp server最后将根据云端ID²认证中心返回的认证结果做相应的业务处理

具体认证过程中sp server所用到的POP SDK API用法可参考相应的sample code：[获取随机数](https://iot.aliyun.com/docs/security/Server_API_%20getServerRandom.html); [认证请求](https://iot.aliyun.com/docs/security/Server_API_%20verify.html); [认证请求并加密](https://iot.aliyun.com/docs/security/Server_API_%20verifyAndEncrypt.html)。终端节点上调用的TFS API可参考[相应的API说明](https://iot.aliyun.com/docs/security/Device_API.html#challenge)，tfs静态库的路径:AliOS-Things/security/tfs/lib.

### Alios Things自组织网络的安全认证架构

传统的AAA（Authentication、Authorization、Accounting）服务在部署和配置上都需要额外的专业IT人员操作，而对于像物联网这样拥有大量设备节点的场景，手动的为每一个设备节点生成证书显然有些不切实际。此外，x.509证书不仅需要出厂预置占用较多的flash资源，并且在ASN.1解析和认证过程中的消息传递也会消耗大量的MCU资源（根据不同的签名算法，密钥协商算法，加密算法而生成的证书的大小各不相同，大一点的证书可能会超过1KBytes），因此对于资源受限的嵌入式设备节点，基于证书的认证方式可能不是一个最优选择。

ID²设备身份认证平台是一个更为轻量级的基于身份信息的认证服务平台，尤其适合硬件资源不足的嵌入式设备的认证方式。认证服务中心云端化也省去大量IT人员的时间来重复相同的部署和配置过程，客户所需要做的仅仅是调用相应的SDK对接云端ID²认证中心。基于这个优势，设计自组织网络（uMesh）的设备节点端安全认证过程时也依托于上述ID²设备身份认证平台的挑战应答认证模式。目前新加入的设备节点和已经入网节点之间的认证通信协议兼容标准的IEEE802.1x和EAP扩展认证协议，利用IEEE802.11 WiFi协议标准进行数据传输。EAP扩展协议也为后续扩展和兼容多种标准认证方式（如MD5, OTP, TLS等）提供了基本协议框架。

自组织网络uMesh和ID²设备身份认证平台结合的安全认证架构框图如下：

![](/Users/maxiao/Desktop/屏幕快照 2018-01-06 下午4.11.01.png)

树莓派3作为直接和AP相关联的节点充当网络的Leader角色来创建一个新的自组织网络并负责分配短地址（16 bits）给后续加入网络的设备节点，该地址用于在uMesh网络内通信，同时在树莓派3上利用iptable命令建立NAT来相互转发tun0接口和eth0接口之间的ip数据包（NAT更改ip数据包头里的源地址），这样就可以让uMesh网络内的节点成功和外网的sp server通信，从而和云端ID²认证中心进行身份认证。

uMesh网络节点间的认证协议流程如下：

![](/Users/maxiao/Desktop/屏幕快照 2018-01-06 下午4.51.42.png)

EAP扩展认证协议框架不仅定义了标准的认证类型（如MD5, OTP, GTC, TLS等），还定义了扩展类型（Expanded Types， type值为254）用来兼容不同的vendor现有的自定义认证方式。EAP-ID²即为基于ID²设备身份认证所设计的一种认证协议用于uMesh自组织网络节点的安全认证方式之一。详细的扩展类型包头格式如下：

![](/Users/maxiao/Desktop/屏幕快照 2018-01-06 下午4.59.42.png)



### 总结

兼容IEEE802.1x端口访问控制协议和EAP扩展认证协议为自组织网络（uMesh）的安全认证体系提供了更为丰富，更加灵活可扩展的协议认证框架，可以满足不同客户现有自定义或者标准的认证流程。目前通过搭建实际测试环境验证了AliOS Things自组织网络（uMesh）结合ID²设备身份认证平台提供对新加入节点做身份信息的认证方案，也为后续大规模的商业场景部署提供了有力支撑。

想了解更多关于AliOS Things的细节，可以访问[AliOS Things Github](https://github.com/alibaba/AliOS-Things)。


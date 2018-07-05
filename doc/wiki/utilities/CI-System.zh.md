[EN](CI-System) | 中文

### 背景简介

[AliOS Things](https://github.com/alibaba/AliOS-Things) 是一款由阿里巴巴开发的轻量级物联网操作系统。拥有丰富的组件（实时操作系统内核，连接协议库、文件系统、libc接口、FOTA、Mesh、语音识别、安全等），并支持多样的芯片架构（ARM系列，Xtensa，C-Sky等）。可广泛应用在智能家居，智慧城市，工业等领域，使万物互联更容易，降低物联网开发门槛。

AliOS Things在快速的迭代进化之中，如何保证提交的代码质量，并保证在各个硬件平台上的稳定性，是一个非常大的挑战。同时，物联网硬件碎片化，资源紧张，对CI系统也提出了特殊的要求。本文介绍AliOS Things的CI系统的实现方式，及思考。

###  AliOS Things 持续集成（CI）系统简介

AliOS Things 持续集成系统是与代码仓库紧密结合的自动化代码验证测试系统。系统会对开发者提交的每一次代码合入自动运行一次全平台编译和全功能验证测试，并通知开发者测试结果。系统的运行流程为：

1. 开发者提交代码(merge request) 
2. Precheck+Prebuild系统自动做代码检查和编译
3. Prevalidate+uDevice Center平台自动完成AliOS Things各项功能的在设备上的验证测试

与一般的CI系统不同，AliOS Things的功能验证是需要运行在多种多样的嵌入式芯片中的，这里需要解决的难题是：如何屏蔽多样化嵌入式系统的差异来做自动化CI测试？我们开发了uDevice Ceter来解决这个问题。uDevice Center是一个在线的设备（模组/开发板）开发调试平台，通过统一方式屏蔽了开发板的硬件差异，让自动测试程序可以调用统一的接口来实现测试需要的操作（固件烧写，板子控制（启动/停止/重启)，shell命令交互等）。



![CI系统](https://img.alicdn.com/tfs/TB1VrI6nhrI8KJjy0FpXXb5hVXa-8044-2986.png)



AliOS Things 持续集成系统的架构如上图所示。系统目前主要支持code.aliyun代码仓库（github支持添加中）。除代码仓库外，系统主要还包括代码检查与编译系统(Precheck & Prebuild)，自动测试平台和uDevice Center在线设备平台。下面我们将详细介绍各项子系统的功能。




### Precheck代码检查系统

Precheck 是自动化的代码静态扫描和质量监控系统，能帮助开发者提高代码质量，预防代码中的潜在风险，提高开发效率。系统的大致工作流程为：

1. 用户提交代码到仓库，仓库发送请求给precheck平台；
2. 平台会自动获取用户所提交的代码，pull到本地，进行pmd，findbugs，androidlint 等静态代码扫描（对扫描出来的告警做增量存量判断）；
3. 执行完成后向仓库提交对应的comments（并自动通过邮件发送报告给用户）。

Precheck系统架构如下图所示。系统是一个弹性扩容的架构，可以方便地根据开发需求增减服务容量。

![precheck](https://img.alicdn.com/tfs/TB15ZPlnf2H8KJjy0FcXXaDlFXa-5752-1619.png)



### Prebuild分布式编译系统

Prebulild 是分布式的编译系统，使用机器集群来自动完成AliOS Things在多样的嵌入式平台下的代码编译构建，帮助开发者快速验证所提交代码的编译正确性。系统的大致工作流程为：

1. 开发者提交代码到仓库，仓库触发Probuild系统启动自动编译任务；
2. Prebuild系统利用集群分布式地完成编译构建，并将编译结果报告给仓库；
3. 编译通过的话，Prebuild系统会触发后端的自动测试的运行

Prebuild系统的架构如下图所示。Prebuild系统是全Docker化的系统，能很方便快速地按需增减容量。

![Prebuild](https://img.alicdn.com/tfs/TB10ZIjnfDH8KJjy1XcXXcpdXXa-5752-1580.png)



### Prevalidate+uDevice Center自动测试验证系统

Prevalidate是一个通用自动化测试平台，其能力包括：组织管理用户编写的测试程序(java/python/shell)，将测试程序有机组合为测试任务，按需求创建/排队/分发/执行/监控/管理测试任务，获取测试任务执行结果并通知用户等。Prevalidate系统能在PC上运行各种程序，但并不能运行AliOS Things在各种嵌入式硬件上的固件并完成功能验证，所以我们还开发了uDevice Center，一个在线的硬件（模组/开发板/设备）开发测试平台。uDevice Center用网络连接着AliOS Things支持的各种硬件（开发板/模组/设备等），并通过统一的接口屏蔽了各种硬件在操作控制上的差异，从而让自动测试程序能够统一操作各种不同的开发板。uDevice Center也容易扩展对接各种AliOS Things支持的设备。

在这套系统中，Prevalidate管理并运行自动测试程序，而uDevice Center为自动测试程序提供交互运行的具体硬件。我们开发了AliOS Things的各项功能测试程序并上传到Prevalidate平台作为测试项，再把相关联的测试项组合成可以被触发执行的PV测试任务。当用户提交代码并被Prebuild系统编译通过后：

1. Prebuild系统会触发Prevalidate系统创建PV测试任务；
2. Prevalidate系统对PV测试任务排队，从系统的slave cluster中选择合适的空闲机器执行任务；
3. 任务的各项测试脚本被执行，测试程序从uDevice Center平台分配合适型号的设备，将编译镜像烧录到设备中，并使用标准接口与设备交互，完成AliOS Things各项功能的测试；
4. Prevalidate系统将测试结果报告给代码仓库，并邮件通知相关人员。

Prevalidate+uDevice Center系统的架构如下图所示，系统具有很强的扩展性。

![Prevalidate](https://img.alicdn.com/tfs/TB1JlCbnDvI8KJjSspjXXcgjXXa-5755-1413.png)



### 小结

持续集成（CI）系统支撑着[AliOS Things](https://github.com/alibaba/AliOS-Things)的快速进化。后续我们也计划把这套能力开发给广大的开发者和合作伙伴，共同提升物联网时代的开发效率。



### 致谢

感谢OS中台团队：赵剑峰(牧鹏)，丁国正(轩哥)，翁开域(辟疆)，徐小春(明亦)，马飞飞(君琢)等人的鼎力支持

感谢AliOS Things团队：欧军礼(悟嗔)，廖怡然(宜髯) ，徐文杰，蔡俊杰，曾旭等人的开发支持




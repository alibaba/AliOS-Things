@page component 组件介绍

[更正文档](https://gitee.com/alios-things/documentation/edit/rel_3.3.0/manual/component.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

组件(Component)是构成Alios Things的基本单元。从系统角度看，除了构建脚本和辅助工具外，一切都是组件；根据组件的应用范围，可以将组件划分为以下三类：
   - BSP组件：指的是./hardware目录下的arch、chip、board三大组件的集合。包含了芯片架构、芯片驱动、板级区别等。
   - 内核组件：指的是./kernel目录下的RTOS内核的集合。其中rhino内核是AliOS Things提供的最小内核功能集。
   - 系统组件：指的是./components下所有系统组件的集合
   - 应用组件：指的是./solutions下的所有HaaS参考案例，包含helloworld_demo，云端一体/ 云端钉一体的参考设计等。

本文描述的组件主要是./components下面的系统组件部分。

**组件之间的关系**

原则上组件功能应该相对独立，可以单独对外提供接口和服务。但是在某些场景中，组件之间需要互相配合来一起完成一项复杂的功能。整理来看组件之间的关系可以分为必须组件和可选组件两种。
   - 必选依赖：是指组件A在完成某个功能时，必须引入组件B，一起配合。例如：HTTP组件，完成访问HTTP服务器的功能，必须引入TCP/IP组件。
   - 可选依赖：是指组件A在完成某个功能时，可以引入组件C，也可以引入组件D。例如：HTTP组件，要访问HTTPS服务器，可以引入mbedtls组件，也可以引入openssl组件，来完成访问服务器中加解密的功能。

**组件的构成**

为了使OS能被灵活地配置，适应多种硬件平台，AliOS Things的组件主要包含以下几个部分。
   - include: 指的是该组件对外暴露的API接口定义。
   - internal: 指的是该组件内部使用的API接口定义。
   - src: 指的是该组件的源码实现部分。
   - example: 组件使用示例代码。
   - README.md: 组件介绍文档，包含概述、版权信息、目录结构、依赖组件、常用配置、API描述、使用示例、关键日志、FAQ等信息。
   - package.yaml: AliOS Things 3.3版本采用的编译脚本。详细的.yaml语法信息可以参考《yaml语法说明》文章。


@subpage a2sa \n
@subpage ai_agent \n
@subpage ble_host \n
@subpage ble_netconfig \n
@subpage cjson \n
@subpage cli \n
@subpage cplusplus \n
@subpage debug \n
@subpage driver \n
@subpage epoll \n
@subpage fatfs \n
@subpage freetype \n
@subpage http \n
@subpage init \n
@subpage jsoncpp \n
@subpage kv \n
@subpage libc_stub \n
@subpage linksdk \n
@subpage littlefs \n
@subpage lwip \n
@subpage mbedtls \n
@subpage mbmaster \n
@subpage mqtt \n
@subpage netmgr \n
@subpage oss \n
@subpage ota \n
@subpage posix \n
@subpage py_engine \n
@subpage ramfs \n
@subpage SDL2 \n
@subpage select \n
@subpage sensor \n
@subpage sntp \n
@subpage trace \n
@subpage uagent \n
@subpage ucamera \n
@subpage ucloud_ai \n
@subpage udisplay \n
@subpage ugraphics \n
@subpage ulog \n
@subpage uvoice \n
@subpage uservice \n
@subpage vfs \n
# BoneEngine@Lite简介

**关键词说明** ：

TinyEngine： BoneEngine@Lite的别名用语，等同于BoneEngine@Lite。



## BoneEngine@Lite背景介绍：

BoneEngine@Lite是一个专门为嵌入式系统设计且面向IOT业务的高性能JavaScript引擎，同时针对主流的嵌入式操作系统提供一体化的开发框架，提供丰富的扩展接口及调试手段，以方便开发者开发。

通过BoneEngine@Lite引擎，之前在物联网领域必须通过C/C++来开发的方式也可以通过javascript语言来开发了。C/C++语言更倾向于内核及硬件底层的开发，而javascript语言更适合在内核之上开发物联网应用。

不同于Node.js，BoneEngine@Lite更加轻量化，适用于资源（CPU/ROM/RAM) 比较紧张的场景，当前主流的轻量化js引擎有Tiny-JS, JerryScript, Espruino, Duktape等。

BoneEngine@Lite的主要特点有:

* 面向IOT业务的高性能JS引擎

  * 资源占用少，BoneEngine@Lite专门针对嵌入式系统设计，所以在jse部分做了性能优化和裁剪。经测试，可以做到在RAM<10KB,ROM<10KB的系统上运行。
  * CPU性能高，通过优化的词法和句法分析器，支持栈模式，降低了CPU的使用率。
  * 面向IOT应用场景的JS支持能力，由于BoneEngine是面向IoT的，所以其内置了面向IOT的ES精简语法及常用的IOT功能模块，如MQTT，WIFI，HARDWARE扩展等。

* 跨平台及一体化的应用编程框架

  - 通过硬件抽象层HAL，及操作系统抽象层OSAL，BoneEngine@Lite可以运行在aos，freertos，linux之上。
  - 在OSAL和HAL层之上，BoneEngine@Lite构建了统一的物联网应用开发框架，内置了设备上云的能力，与阿里云一站式开发平台可以直接对接，并有标准的设备模型。
  - 支持板级驱动，模块，设备驱动 的动态加载，js应用可以云端动态加载运行。

* 提供一体化开源部署工具

  * BoneEngine@Lite提供了基于IDE图形化和CLI命令行的开发和部署工具，方便开发者方便的基于BoneEngine@Lite来开发IOT应用。

* 开源的开发者生态

  * BoneEngine@Lite通过开源吸引更过的开发者和ISV使用，并基于不同的IOT场景开发出更多的IOT应用。逐步完善基于阿里IOT的开源生态。

    ​

## 为什么要开发BoneEngine@Lite

上面介绍了BoneEngine@Lite的这些优点，那么下一步我们来讲讲为什么要开发BoneEngine@Lite以及BoneEngine@Lite技术解决当前的什么实际问题。

### 物联网设备的开发现状及问题

时下物联网开发比较火热，但是也暴露出物联网开发的一些问题。传统的物联网开发属于嵌入式开发，要求开发者具备较强的C/C++语言知识，同时要有较强的硬件基础，如通过指针操作内存，通过系统调用操作外设，通过寄存器读取外设状态等等。这对于开发者提出了较高的编程语言能力要求。同时传统的物联网开发由于使用本地编译，链接，下载的方式，针对不同的设备或平台，即使是完成同样的应用场景，例如点灯这个操作，也需要重新编译，链接下载，这种方式在物联网终端分散，碎片化，场景应用复杂的需求下，这种开发方式显得有些效率低下。另外由于C/C++的调试方式以gdb为主，图形化的调试工具较少或性能较低，也给后期的调试，发布带来了一些困难。

总的来说，当前的物联网开发现状如下：

* 开发难度高，需要有硬件基础。
* 模块复用难，集成功能效率低下。
* 开发和调试手段较少。
* 发布及升级有风险。

### BoneEngine@Lite是如何解决这些问题的

前面提到了传统的物联网设备开发的一些局限性，那么BoneEngine@Lite是如何解决这些问题的？

其关键在于引入了解释型语言JavaScript，解释型语言是相对于编译型语言来说的，解释型语言是指使用专门的解释器对源程序进行逐行解释成特定平台的机器码并立即执行的语言。由于有了解释器，所以语言无需提前编译，可以直接在运行的时候解析并运行。所以BoneEngine@Lite通过在嵌入式系统上实现javascript的解析器，使得javascript也可以运行在嵌入式系统甚至没有os的单片机上。

其次BoneEngine@Lite通过抽象操作系统接口层OSAL及硬件抽象层HAL，使得其可以运行在不同的系统和硬件上面，屏蔽了os和硬件的细节，使得开发者可以专心于使用javascript来开发应用。

最后，BoneEngine@Lite通过针对IOT开发的framework，提供了常用的IOT协议，如http，mqtt，wifi，zigbee等。并直接对接阿里一站式开发平台，使得开发一款IOT设备更加简单。

### 为什么选择javascrpit作为BoneEngine@Lite的应用层开发语言

前面提到了解释型语言是BoneEngine@Lite解决传统嵌入式开发问题的关键，那么解释型语言有lua，python，perl等，为什么要选择javascript呢？

这里有几点原因，javascript是时下比较流行的web开发语言，拥有众多的开发者，这部分开发者可以在无门槛的情况下来开发物联网应用，这对于IOT生态是非常有利的。另外javascript语言有很多开源的开发框架，可以更加方便的开发应用。其次是javascript语言本身是高效、简单、易用的，其性能可以接近C，但是相比C更加容易使用和适合业务开发。最后是javascript的IDE、调试手段比较丰富，如断点，内存分析，在线调试等。

### BoneEngine@Lite的技术架构

![BoneEngine@Lite arch](https://img.alicdn.com/tfs/TB1zK7JruOSBuNjy0FdXXbDnVXa-550-351.png)

可以看到BoneEngine@Lite在OS抽象层和硬件IO抽象层之上，OS抽象层（OSAL）用于封装操作系统的接口。

例如OSAL实现了这些接口：

be_osal_init_yloop：在主任务创建一个事件处理循环，类似于线程。

be_osal_post_event：事件通知接口

be_osal_delay：延时接口

be_osal_new_task：创建一个任务接口

be_osal_timer: 定时器接口

这些OSAL的封装屏蔽了具体的OS，但是实现了操作系统的通用操作接口，提供给BoneEngine@Lite调用，如此便实现了BoneEngine@Lite跨OS的效果。

同样的，HAL层封装了GPIO，I2C, UART, SPI, PWM的各种操作。这样对于BoneEngine@Lite的调用来说，则屏蔽了具体的硬件驱动实现，如此便实现了BoneEngine@Lite跨硬件平台的效果。

另外，BoneEngine@Lite通过app-manager实现了基于云平台的应用动态加载和分发，开发者可以在本地编写javascript应用并通过云端平台发布，并运行在不同的端设备上。

### 如何开发一个BoneEngine@Lite的javascript应用

* 基于alios编译并下载BoneEngine@Lite固件到BoneEngine@Lite支持的硬件上（例如mk3060，esp32devkit，developerkit等）。

  * 下面以 alios 的developerkit开发板举例编译和下载命令：
  * 编译：```aos make tinyengine_app@developerkit```
  * 下载：```aos upload tinyengine_app@developerkit```

* 通过IDE或者be-cli来开发javascript应用。

  这部分内容建议参考文档的《通过be-cli调试javascript应用》章节。

### 一个javascript例子

为了使同学们有更加感官的认识BoneEngine@Lite，下面通过一个js例子程序来演示如何在aos developer kit开发板上面使led每隔一秒闪烁。

* 硬件环境：一款aos things的developer kit开发板


* javascript程序：

  ```javascript
  console.log('This a demo javascript for bone-engine\n');
  console.log('this demo run on developerkit to blink led every second');

  var index = 0;
  var led_handle = GPIO.open('led');
  GPIO.write(led_handle,0);
  setInterval(function() {
  	index = 1-index;
  	GPIO.write(led_handle,index);
  	console.log('led blink every seconds...............');
  }, 1000);
  console.log('end gpio test........................');
  ```

* 运行

  * 参考文档《如何通过be-cli调试javascript应用》通过stlink usb口更新该javascipt文件到developerkit模组中 ```be push app.bin```
  * 查看LED的效果和串口日志，可以看到led灯每隔一秒闪烁。

* 源码解析

  console.log：打印log日志

  GPIO.open('led'): 获取led对应的gpio端口号，这个函数会读取解析板级配置文件board_config.json

  GPIO.write(led_handle,0)：将led对应的gpio拉低。

  setInterval（）：开启一个定时器，每隔一秒间隔将led的gpio拉高拉低。

* 总结：通过这个javascript程序可以看到，开发者无需了解底层硬件驱动的实现细节，而只需关注led灯这个对象，让开发者尽量关注应用本身的开发而无需考虑硬件驱动的实现。BoneEngine@Lite的初衷也是为了降低物联网开发的门槛，让物联网开发如开发web应用一样简单。

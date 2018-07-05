[EN](uDevice-Center) | 中文

### 背景

IoT的学习与开发是还是比较困难的。对于初学者来说，上手之路就很长：从某宝选购开发板-->等快递-->收到板子-->搞清楚怎么连线-->找个hub把一堆东西接到电脑-->安装开发软件熟悉开发环境。做完这么多事，才能写上第一行代码。选板子接线都是需要一定的硬件专业知识的，这会让很多没有基础的同学直接就放弃了。

![background.png](https://img.alicdn.com/tfs/TB1C2ximiqAXuNjy1XdXXaYcVXa-1282-959.png)

同时，对于嵌入式老鸟来说，IoT的开发也引入了新的困难：以前做设备开发一台电脑一个板子就搞定了；现在做IoT智能应用开发，需要要多台设备联网通信，就不太容易开发了。比如一套智能家居的房子几十个设备，放地上贴墙上挂天花板上都有，怎么开发呢？现有开发工具大多都不支持多设备同时开发。用一台电脑开发的话，电脑接成八爪鱼都不够，还需要开十几个界面切来切去，开发效率低下。 多用几台电脑开发的话，coding的时间都用来跑来跑去了，不能专注于解决问题。

IoT开发的痛点在：硬件获取与setup路径太长；开发工具不支持多设备同时开发，也不支持方便可视化的网络应用开发。我们每天都在suffer着这样的问题，遂而开发了uDevice Center - <strong>弹性在线的多设备</strong>开发平台 - 来让IoT开发更简单



### uDevcie Center介绍

uDevice Center主要采用如下的方式解决了上述问题：

1. 将IoT设备搬到线上，做成IoT的ECS：用户不用买板子不用接线，装个软件就能做IoT的开发了；
2. 提供信息丰富的多设备开发能力：多设备复杂网络开发，made easy！

#### 线上的IoT开发平台 - IoT的ECS

uDevice Center把标准的IoT开发板/设备都搬到了线上，让用户可以像使用ECS服务器一样使用IoT开发板。用户只用装好软件，就可以用在线的开发板/设备开始编程调试了，再也不用买板子连线了。开发路径的简化，可以让程序员们把时间精力都集中在业务逻辑的开发调试上，加速IoT学习与开发的历程。弹性在线平台的优点包括：

- 无需购买连接硬件，上手快
- 无时间地域限制，哪都能做开发
- 丰富可选的开发板型号，用哪种都有
- 弹性增减的开发板数量，用几个都行

此外，对于特定应用场景（如智能家具）的开发，弹性在线的平台可以帮助开发者利用公共资源（如全屋智能实验室）很快速地开始应用的开发验证，降低开发门槛，节省成本。

![architecture.png](https://img.alicdn.com/tfs/TB1hdFZoBfH8KJjy1XbXXbLdXXa-5791-3120.png)

uDevice Center的系统架构如上图所示，主要由：IoT硬件设备、uDevice Center Infrastructure和用户端AliOS Studio软件三部分构成。系统中的IoT硬件设备是由阿里及其合作伙伴提供的模组/开发板/智能设备/实验室等。uDevice Center Infrastructure平台用标准化的方式屏蔽了不同硬件在固件烧录/调试/传感/控制等方面的接口差异，以统一的方式将这些IoT硬件资源虚拟化，再通过网络提供给广大的开发者使用。用户端AliOS Studio软件为用户提供了友好地使用uDevice Center Infrastructure服务的界面。

此外，uDevice Center是一个开放的平台，可以为想要接入的厂商提供配套软件，帮助厂商将设备（芯片/模组等）快速接入到平台，并推广给开发者使用。

#### 多设备联合开发

uDevice Center与AliOS Studio IDE深度结合，为IoT开发者提供信息丰富的多设备联合开发能力。uDevice Center的使用界面如下图所示：中间窗口显示了连接在线的设备，其中每一个圆点就是一个实际的IoT设备；点'07d'c'上的悬浮窗实时显示了这个设备的状态信息；点与点之间的连线展示了mesh网络连接的情况；右边的窗口是设备‘1d1c'的Shell交互界面。
![image.png](https://img.alicdn.com/tfs/TB16t8ZoBfH8KJjy1XbXXbLdXXa-3999-2499.png)
uDevice Center为用户提供的多设备联合开发功能包括：

- 多设备的代码开发/编译/固件烧录
- 多设备shell命令行交互调试与设备控制
- 实时显示设备的各种状态信息
- 图形化显示设备的（uMesh）连接组网情况

这种信息丰富的多设备联合开发调试，可以加快物联网智能应用的开发与调试。



### 软件安装步骤

接下来我们将讲解使用uDevice Center进行开发需要安装的软件：

1. 下载[Visual Studio Code](https://code.visualstudio.com/download)并安装
2. 安装alios-studio插件：启动Visual Studio Code，并参考下图步骤安装
   ![image.png](https://img.alicdn.com/tfs/TB1KoTCovDH8KJjy1XcXXcpdXXa-3974-2227.png)
3. 下载安装AliOS Things代码及编译工具链[Linux](AliOS-Things-Linux-Environment-Setup)  [OSX](AliOS-Things-MAC-Environment-Setup)  [Windows](AliOS-Things-Windows-Environment-Setup) (Optional）



### uDevice Center使用

uDevice Center的使用方式很灵活：既可以作为多设备开发调试工具单独使用，也可以作为AliOS Thing集成开发环境的组件来使用。 

目前支持的开发板型号：

```
    乐鑫-esp32-devkitc
    庆科-MK3060
    STM-Cortex M0, M3, M4, M7系列
```



#### 获取开发板资源

uDevice Center平台上的设备是使用Access Key进行分配的，使用前请先邮件联系AliOS Things团队 (Email: <aliosthings@service.aliyun.com> ) 获取Access Key 。邮件请包含如下信息：

1. 个人信息：姓名   联系方式   公司名（optional）
2. 板子资源需求：型号、数量、使用时长等
3. 简要陈诉用途



#### 作为单独开发调试工具使用

uDevice Center可以作为开发流程中的单独调试工具使用。在这样的流程中，用户可以使用其他工具编辑编译代码，再将产生的固件作为uDevice Center的输入来开发调试设备。单独使用uDevice Center的流程为：
用其它工具编辑代码编译产生.bin --> 用uDevice Center烧录固件-->用uDevice Center调试

下面以庆科MK3060开发板为例，介绍单独使用uDevice Center进行开发调试的流程：
Step 1. 用其他工具编辑代码，并编译产生可以烧录的.bin固件

Step 2. 启动uDevice Center，并输入你的Access Key
![image.png](https://img.alicdn.com/tfs/TB1rADfoxrI8KJjy0FpXXb5hVXa-3999-2499.png)

![input-acceeskey.png](https://img.alicdn.com/tfs/TB1Y9j_or_I8KJjy1XaXXbsxpXa-3999-2508.png)

Step 3. 熟悉uDevice Center
![image.png](https://img.alicdn.com/tfs/TB17k_foxrI8KJjy0FpXXb5hVXa-3999-2497.png)

Step 4. 烧录固件
鼠标右键点击某个空闲MXCHIP3060开发板，选择“Program”将Step 1产生的固件烧录到开发板。Note：请注意核对板子型号，固件烧录到型号不对的板子将无法启动
![image.png](https://img.alicdn.com/tfs/TB1OhvEovDH8KJjy1XcXXcpdXXa-3999-2499.png)

Step 5. Shell交互调试
鼠标右键点击step 4烧录的MXCHIP3060开发板，选择“Shell”与开发板进行命令行交互
![image.png](https://img.alicdn.com/tfs/TB1sATfoxrI8KJjy0FpXXb5hVXa-3999-2499.png)

#### 作为AliOS Things的集成开发工具组件使用

作为AliOS Things集成开发工具的一部分使用uDevice Center的典型流程为：
建立AliOS Things工程 --> 编辑并编译源代码 --> 通过uDevice Center烧录/调试设备

下面以庆科MK3060开发板为例，介绍使用uDevice Center进行开发调试的流程：

Step 1. 新建项目，编辑并编译代码
![image.png](https://img.alicdn.com/tfs/TB1d24imiqAXuNjy1XdXXaYcVXa-3999-2499.png)
![image.png](https://img.alicdn.com/tfs/TB18Qn7or_I8KJjy1XaXXbsxpXa-3999-2499.png)
![image.png](https://img.alicdn.com/tfs/TB1QwdimiqAXuNjy1XdXXaYcVXa-3999-2499.png)

Step 2. 启动uDevice Center: 操作步骤同上

Step 3. 烧录固件
鼠标右键点击某个空闲MXCHIP3060开发板，选择“Program Last Build”将刚才编译固件烧录到开发板。Note：请注意核对板子型号，固件烧录到型号不对的板子将无法启动
![image.png](https://img.alicdn.com/tfs/TB13pbEovDH8KJjy1XcXXcpdXXa-3999-2499.png)

Step 4. Shell交互调试: 操作步骤同上



### 待完善功能点

1. 支持GDB代码级调试
2. 支持更丰富的设备状态显示
3. 丰富硬件的控制功能



### 总结

uDevice Center是在线的IoT多设备开发调试平台，能方便开发者快速高效地进行IoT云端一体应用的开发、调试和测试等。通过硬件在线与虚拟化，既能让更多的开发新手快速上手学习IoT开发，又能帮助资深开发者快速高效地开发调试IoT应用。



### 钉钉群

使用交流uDevice Center可加入以下钉钉群
![image.png](https://img.alicdn.com/tfs/TB1VuzJovDH8KJjy1XcXXcpdXXa-750-990.jpg)

#TinyEngine

TinyEngine 提供了在设备端用JS语言开发产品的应用开发框架，运行环境，以及开发和部署工具集，旨在让IoT设备应用和开发者不用过度关注硬件平台的差异性，底层驱动和系统的实现细节，也不用关注具体云端设备接入技术，就可以快速开发出IoT产品，并能实现最终应用跨硬件平台的运行能力，真正做到“ 一份代码，到处运行 ”。

TinyEngine 具备如下特点：

* 提供针对IoT应用特点而优化的高性能 JS 引擎：
	* 资源占用少：RAM< 10KB,ROM(Footprint) < 10KB；
	* CPU性能优化：优化的词法和句法分析器，精简的Javascript的语法支持；
	* 完备的JS语法支持能力：IoT应用的精简语法，CommonJS扩展语法，大量IoT Builtin模块；
* 包含IoT设备端的应用编程框架和设备模型：
	* 设备模型兼容阿里的标准设备模型（属性，事件，方法）；
	* 自带设备上云能力模块，与阿里的Link Platform IoT平台对接；
	* 支持板级驱动，模块及设备驱动的动态加载能力，应用可本地和云端加载；
	* 框架和JSE支持多OS (OSAL)，支持 AliOS，FreeRTOS；  
	* 支持多芯片平台移植(HAL)，已涵盖了庆科，乐鑫，STM，MTK等主流芯片；
* 提供方便的本地和云端开发工具：
	* WebIDE 支持本地和云端应用代码编辑，调试和发布；
	* be-cli 命令行工具可让开发者在本地开发，调试设备端应用；

* 提供丰富的设备，驱动，设备应用开发的工具，模块：
	* 实现并开源大量的通用设备驱动模块，场景应用，供开发者引用；
	* 提供应用，设备模块下载和分发通道和能力；
	* 开发者可提交模块供第三方应用；

TinyEngine 适用于IoT设备端的场景应用开发，设备开发，驱动模块等开发者。



##编译下载

* 在aos根目录执行```aos make tinyengine_app@版型```
* 当前已经验证通过的版型有 mk3060，esp32devkitc，developerkit。
* 下载：```aos upload tinyengine_app@版型```

##调试方法

* 在framework/tinyengine/tools目录执行```make cli. ```安装并更新be_cli工具。
* 参考docs/开发工具使用指南目录下的《be-cli 工具使用指南.md》，并通过be_cli 更新设备js程序。



## Javascript应用例子

* 在samples目录下有多个module和app的js例子程序，供参考试用。
* 例如我们需要调试gw的例子，使用如下几个步骤：
  * 使用```be -p samples/app/gw``` 将gw目录中的js文件打包成一个app.bin。
  * 使用```be connnect``` 连接模组/开发板的usb串口。
  * 使用```be push app.bin```  将app.bin更新到设备上。
  * 重启设备，设备启动后会自动加载并运行index.js这个文件。






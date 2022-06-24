# 电子价签系统
&emsp;&emsp;
下图是本案例除硬件连线外的3步导学，每个步骤中实现的功能请参考图中的说明。
<div align="center">
<img src=./../../../images/电子价签系统/步骤概述.jpg width=90%/>
</div>

## 1、简介
&emsp;&emsp;
电子价签，也叫电子货架标签（Electronic Shelf Label，ESL），是一种带有信息收发功能的电子显示装置，主要应用于超市、便利店、药房等显示价格信息的电子类标签。放置在货架上，可替代传统纸质价格标签的电子显示装置，每一个电子货架标签通过有线或者无线网络与商场计算机数据库相连，并将最新的商品信息通过电子货架标签上的屏显示出来。电子货架标签事实上成功地将货架纳入了计算机程序，摆脱了手动更换价格标签的状况，实现了收银台与货架之间的价格一致性。

&emsp;&emsp;
下图是本例开发的电子价签展示：
<div align="center">
<img src=./../../../images/电子价签系统/电子价签展示.jpg width=90%/>
</div>

### 1.1、背景知识
&emsp;&emsp;
本系统的核心有三点:

&emsp;&emsp;
1、墨水屏驱动

&emsp;&emsp;
2、墨水屏图像和文字展示(电子价签界面设计)

&emsp;&emsp;
3、云端模块下发，更新电子价签展示


&emsp;&emsp;
电子墨水是一种革新信息显示的新方法和技术。像多数传统墨水一样，电子墨水和改变它颜色的线路是可以打印到许多表面的，从弯曲塑料、聚脂膜、纸到布。和传统纸差异是电子墨水在通电时改变颜色，并且可以显示变化的图象，像计算器或手机那样的显示。

<div align="center">
<img src=./../../../images/电子价签系统/微雪电子墨水屏.jpg width=50%/>
</div>


### 1.2、准备

&emsp;&emsp;
本案例需要如下硬件。
* ESP32开发板一套
* [微雪2.9英寸三色墨水屏模块](https://www.waveshare.net/shop/2.9inch-e-Paper-Module-B.htm)一个
* 杜邦连接线若干

&emsp;&emsp;
硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/电子价签系统/ESP32接线图.jpg width=90%/>
</div>
<br>


## 2、设备端开发
### 2.1、开发环境

&emsp;&emsp;
在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[ESP32开发环境](../../../startup/ESP32_startup.md)的说明。

### 2.2、创建解决方案
&emsp;&emsp;
如下图所示，在Haas Studio中创建项目。先选择左侧的“开发板型号”再从右侧的案例中选择“电子价签”案例点击“立即创建”即可。

<div align="center">
<img src=./../../../images/HaaS_Studio_创建工程示范.png width=100%/>
</div>
<br>

### 2.3、工程文件解析
&emsp;&emsp;
工程文件结构如下:

```tree
|-- images                       # 电子价签本地图形
|    |-- alios.mono              # 电子价签本地示例图形（mono格式）
|-- fonts                        # 电子价签显示字库
|    |-- Wendiy7x8.c             # 字库文件
|    |-- Unispace12x24.c         # 字库文件
|    |-- EspressoDolce18x24.c    # 字库文件
|    |-- ArcadePix9x11.c         # 字库文件(由于ESP32内存较小，字库文件仅包含英文，不支持中文)
|-- utils                        # 电子价签图形转换工具 （请从工程中剪切到其他路径，不用推送到设备）
|    |-- img2monoHMSB.py         # png转换成mono格式 (PC上实现)
|    |-- readMonoBuf.py          # mono图片读取buf (PC上实现)
|    |-- showMonoBuf.py          # png图片转成成mono格式并读取buf（PC上实现）
|-- main.py                      # 电子价签主程序（包含云端连接，图片显示）
|-- showPriceData.py             # 电子价签显示设计
|-- esp2in9bv2.py                # 电子价签图形驱动
|-- xglcd_font.h                 # 电子价签文字驱动
|-- board.json                   # 配置文件
```

### 2.4、本地电子价签展示设计

&emsp;&emsp;
本地图形化设计时，可以先暂时不用连接网络，将网络连接相关内容注释掉：

```python
if __name__ == '__main__':

    # wlan = network.WLAN(network.STA_IF)    #创建WLAN对象 
    # get_wifi_status()
    # connect_lk(productKey, deviceName, deviceSecret)

    # 初始化墨水屏
    priceTagObj = ShowData(14500000, Pin(17), Pin(2), Pin(16), Pin(4))
    # 初始化显示
    updating = 1
    gc.collect()
    priceTagObj.show(name=sal_name, sel='$ '+sal_price, offerta='-'+sal_offerta+'%')
    gc.collect()
    updating = 0
```

默认设计的电子价签版面如下：
<div align="center">
<img src=./../../../images/电子价签系统/电子价签版面.jpg width=50%/>
</div>

版面设计代码见 showPriceData.py 文件，可以修改该文件自行设计电子价签版面。

### 2.5、图形工具支持
&emsp;&emsp;
墨水屏支持图形显示，对于ESP32来说，内存较小，更小的图片buf更利于展示。

&emsp;&emsp;
墨水屏为2.9英寸，分辨率为296x128 Pixel, 直接将图片分辨率设置为该大小，正好可以占满整个屏幕。

&emsp;&emsp;
对于示例电子价签而言，展示的图片为左边矩形区域，设计时设计该图形区域大小为 85x85 Pixel。
<div align="center">
<img src=./../../../images/电子价签系统/图像设置.bmp width=40%/>
</div>
设置好后使用 util 文件夹下的工具脚本对 png 图像进行转换。

转化前先安装图形转换依赖工具：
```python
pip install pillow
```
<div align="center">
<img src=./../../../images/电子价签系统/安装依赖库.bmp width=80%/>
</div>

然后本地使用工具进行图形转换以及获取 buf
```python
python showMonoBuf.py apple.png
```
<div align="center">
<img src=./../../../images/电子价签系统/获取图像buf.bmp width=80%/>
</div>

执行完该命令后，当前目录会获得 apple.mono 的图像，该图像可以放置在工程 image 下供设备端直接调用。

另外执行完后，图像的 buf 也会以字符串的形式直接打印出来，该字符串代表该 apple 图像，可以放置在云端直接下发，也可以在代码中直接设置 buf 进行显示。

在 main.py 文件中，有如下注释掉的地方，是直接以 buf 的形式输出图像:
```python
    # buf test code
    # apple_pic = b'ffffffffffffffffffff1fffffffffffffffffffff1fffffffffffffffffffff1fffffffffffffffffffff1fffffffffffffffffffff1fffffffffffff1ffeffff1fffffffffffff03f8ffff1fffffffffffff00f8ffff1fffffffffc77fe0f1ffff1fffffffffc71ffcf1ffff1fffffffff8f1ffef1ffff1fffffffff0f8ffff1ffff1fffffffff1f87fff1ffff1fffffffff3fc6fff9ffff1fffffffff3fe2fff8ffff1fffffffff7fe0fff8ffff1fffffffff7ff0fff8ffff1fffffffff7ff07ffcffff1ffffffffffff03ffcffff1ffffffffffff01ffeffff1ffffffffffff00fffffff1ffffffffffff083ffffff1ffffffffffd01c0ffffff1fffffff3fc00080ffffff1fffffff0f000100feffff1fffffff0706080afcffff1fffffffc33f807ff8ffff1fffffffe37fc0fff8ffff1ffffffff1ffe0fff1ffff1ffffffff1ffe0fff1ffff1ffffffff8fff1ffe3ffff1ffffffff8fff1ffe3ffff1ffffffff8fff1ffe3ffff1ffffffff8fff1ffe3ffff1ffffffff9fff1fff3ffff1ffffffff0ffe0fff1ffff1fffff3ff0ffe0ff81ffff1fffff0fe07fc0ff00feff1fffff07851f043f08fcff1fffffc307000e007cf8ff1fffffe11f801f00fff0ff1ffffff13fe0ffc0fff1ff1ffffff8fff0ffe1ffe3ff1ffffff8fff0fff1ffe3ff1ffffff8fff9ffe1ffe3ff1ffffff8fff1fff3ffe3ff1ffffff8fff8fff1ffe3ff1ffffff8fff0ffe1ffe3ff1ffffff8fff0ffe1ffe3ff1ffffff17fe0ffc0fff1ff1fffffe17fc07fc0fff0ff1fffffc31f863f0c7ff8ff1fffff07020e040e08fcff1fffff0f801f003f00feff1fffff3fe07fc07f80ffff1ffffffff8fff1ffe3ffff1ffffffff8fff1ffe3ffff1ffffffff8fff1ffe3ffff1ffffffff8fff1fff3ffff1ffffffff1ffe0fff1ffff1ffffffff1ffe0fff1ffff1fffffffe17fc0fff8ffff1fffffffc33f847ff8ffff1fffffff07060e16fcffff1fffffff0f001f00feffff1fffffff3fc07f80ffffff1ffffffffff1fff1ffffff1ffffffffff1fff1ffffff1ffffffffff1fff1ffffff1ffffffffff9fff3ffffff1ffffffffff1fff1ffffff1ffffffffff1fff1ffffff1ffffffffff1fff1ffffff1fffffffffe1fff0ffffff1fffffffffe3fff8ffffff1fffffffff873ffcffffff1fffffffff070ffcffffff1fffffffff0f00ffffffff1fffffffff3f80ffffffff1fffffffffffe9ffffffff1fffffffffffffffffffff1fffffffffffffffffffff1fffffffffffffffffffff1fffffffffffffffffffff1fffffffffffffffffffff1f'
    # picBuf = binascii.unhexlify(apple_pic)
    # arrayBuf = bytearray(picBuf)
    # priceTagObj.show(name=sal_name, sel='$ '+sal_price, offerta='-'+sal_offerta+'%', byteBuf=arrayBuf)
    # gc.collect()
```


## 3、物联网平台开发
### 3.1、开通公共实例
&emsp;&emsp;
对于第一次使用物联网平台的读者，需要开通实例以使用物联网平台的功能。这里可以使用免费的公共实例进行开发。

&emsp;&emsp;
在[物联网平台](https://iot.console.aliyun.com/lk/summary/new)中，左上角选择“华东2-上海”，点击“公共实例”，即可开通。
<div align="center">
<img src=./../../../images/5_3_开通公共实例.png
 width=80%/>
</div>

&emsp;&emsp;
开通物联网平台功能之后，需要完成下面的3个步骤完成云端设备的创建：
1. 创建云端产品
2. 创建产品属性（物模型）
3. 创建云端设备（获取三元组）

### 3.2、创建云端产品
&emsp;&emsp;
点击上图中的“公共实例”，即可进入[控制台](https://iot.console.aliyun.com/lk/summary/new)进行产品创建。然后，点击 **``创建产品``** 按钮，如下图所示。
<div align="center">
<img src=./../../../images/电子价签系统/空产品页.png width=80%/>
</div>

&emsp;&emsp;
在新建产品设定页面按照下图所示，设定“产品名称”，选择所属的“自定义品类”（自定义品类的物模型为空，需要自己创建，也可以通过导入外部物模型的方式导入），节点类型选择“直连设备”，联网方式选择“Wi-Fi”，数据格式选择“ICA标准数据格式”，检验类型和认证方式选择默认设定即可。还可以根据开发者自己的需求在“产品描述”页面添加针对此产品的描述。
<div align="center">
<img src=./../../../images/电子价签系统/新建产品.jpg width=80%/>
</div>

&emsp;&emsp;
选择之后，点击“确认”按钮，即可完成产品创建。返回“产品”页面之后可以看到产品类表中会出现刚刚创建的“空气检测系统”的产品，如下图所示。

<div align="center">
<img src=./../../../images/电子价签系统/产品列表页.png width=80%/>
</div>

<br>

### 3.3、创建产品属性（物模型）
&emsp;&emsp;
点击上图中的“查看”按钮，即可看到产品信息，Topic列表，功能定义，数据解析等跟产品相关功能的设定。点开“功能定义”标签页，可以看到设备物模型定义。

&emsp;&emsp;
标识符是设备端上报设备属性状态的消息中需要使用的标识符，并且只有在设备上报的属性内容符合**数据定义**中的数据取值范围的时候才会被物联网平台记录，否则会被物联网平台认定为非法属性而过滤掉。

&emsp;&emsp;
本节我们选择创建自定义物模型的方式来创建此系统需要的物模型信息，点击上图中的”编辑草稿“按钮。然后按照下图的步骤，选择添加自定义功能。

<div align="center">
<img src=./../../../images/电子价签系统/物模型功能.jpg width=80%/>
</div>

&emsp;&emsp;
物模型参数定义如下：

&emsp;&emsp;
1、sal_update: 用于更新电子价签标识符，当下发1时，设备端进行更新，更新后上传0表示更新完成。

&emsp;&emsp;
2、sal_pic: 用于设置下发的图片buf(既前面所述使用showMonoBuf.py显示的字符串)。

&emsp;&emsp;
3、sal_offerta: 用于设置下发物品打折比例。

&emsp;&emsp;
4、sal_price: 用于设置下发物品价格。

&emsp;&emsp;
5、sal_name: 用于设置下发物品名称。

&emsp;&emsp;
产品及其物模型创建完成后就可以创建这个产品的设备了。

<br>

### 3.4、创建云端设备（获取三元组）
&emsp;&emsp;
在产品列表页面中，点击 **``电子价签``** 后的 **``管理设备``**，就会进到设备管理页面。

<div align="center">
<img src=./../../../images/电子价签系统/产品设备管理.jpg width=80%/>
</div>

&emsp;&emsp;
在“设备”页面点击“添加设备”按钮，并在“添加设备”页面中设定“deviceName”,这里开发者可以自己填入自己想设定的设备名称(这里我们填入设备名称zeta_price_tag)，也可以不填任何内容让系统自动生成设备名称，点击 **确认** 后设备添加完成。

&emsp;&emsp;
添加完成后，再次从电子价签后的管理设备即可看到添加的设备。
<div align="center">
<img src=./../../../images/电子价签系统/电子价签设备.jpg width=80%/>
</div>

&emsp;&emsp;
设备信息中有两个信息需要和设备端开发相匹配：
1. 三元组（点击下图中的“查看”及可看到三元组信息）
2. 物模型属性信息

<br>

#### 3.4.1、**获取设备三元组**
&emsp;&emsp;
如上图所示，点击“查看”按钮，就可以看到设备的三元组信息（如下图所示），三元组是物联网设备端和物联网云端设备相关联的唯一标识符，在设备端连接云端的时候会使用三元组信息和云端进行鉴权，鉴权通过之后云端会认为设备已激活并上线。

<div align="center">
<img src=./../../../images/电子价签系统/三元组查看.jpg width=80%/>
</div>

<br>

#### 3.4.2、**查看设备属性信息**
&emsp;&emsp;
设备详情信息页中的“物模型数据”标签页中可以看到设备的所有属性信息、设备事件上报情况及设备服务调用情况，如下图所示。
<div align="center">
<img src=./../../../images/电子价签系统/物模型数据.jpg width=80%/>
</div>

<br>

> 创建产品和设备的过程是按照面向对象的思想进行设计的，其中创建产品可以看成是新建一个类，其中的物模型则是类的对象，创建设备则是进行类的实例化。

<br>


## 4、配置 IoT Studio 物联网

&emsp;&emsp;
电子价签系统，在设计好主要界面后，后续更新只需要更新相关物品和价格等参数，使用物联网平台进行下发，可以更简便，更快速的对价格进行调整。

&emsp;&emsp;
登录[IoT Studio](https://studio.iot.aliyun.com/projects)平台，在左侧的侧边栏点击``项目管理``，在项目管理页面点击``新建项目``按钮。
<div align="center">
<img src=./../../../images/电子价签系统/iotstudio_项目管理.png width=60%/>
</div>

&emsp;&emsp;
在``新建项目``页面点击``创建空白项目``。
<div align="center">
<img src=./../../../images/电子价签系统/iotstudio_新建项目.png width=60%/>
</div>

&emsp;&emsp;
弹出``新建空白项目``页面，输入项目名称及描述后点击``确定``按钮。
<div align="center">
<img src=./../../../images/电子价签系统/iotstudio_新建空白项目.jpg width=60%/>
</div>

&emsp;&emsp;
项目创建完成后会自动导航到新创建的项目，这里我们创建一个 Web 应用用于电子价签。在创建应用之前，需要先将物联网产品和设备关联到iotstudio上。

&emsp;&emsp;
选择左侧的``产品``侧边栏，点击``关联物联网平台产品``，在弹出的窗口中选择之前创建的产品并关联。
<div align="center">
<img src=./../../../images/电子价签系统/iotstudio_关联产品.jpg width=80%/>
</div>

&emsp;&emsp;
选择左侧的``设备``侧边栏，点击``关联物联网平台设备``，在弹出的窗口中选择关联产品下的设备。
<div align="center">
<img src=./../../../images/电子价签系统/iotsutdio_关联设备.jpg width=80%/>
</div>

&emsp;&emsp;
选择左侧的``主页``侧边栏，在弹出的项目详情页选择``Web应用``，点击``新建``按钮，输入应用名称，点击确定。
<div align="center">
<img src=./../../../images/电子价签系统/iotstudio_新建应用.jpg width=60%/>
</div>


&emsp;&emsp;
在空白Web页面中添加相关元素。 页面布局元素设置如下：

<div align="center">
<img src=./../../../images/电子价签系统/iotstudio_Web页面设计.jpg width=80%/>
</div>

&emsp;&emsp;
页面布局完成后，需要进行参数设置，包括需要展示的参数和需要设置的参数，才能和物联网平台和设备进行联动。
首先我们设置需要展示的参数，展示的参数就是读取设备端上报的参数。设备端会将参数上报给物联网平台，所以只需要将展示框的数据源关联到物联网平台相关参数即可。

&emsp;&emsp;
点击``文本框组件``，在右边的``组件配置框``中``样式``栏中，展示数据配置数据源，此处配置好的数据源，会在文本框中进行显示。
<div align="center">
<img src=./../../../images/电子价签系统/iotstudio_配置展示数据源.jpg width=80%/>
</div>

<div align="center">
<img src=./../../../images/电子价签系统/iotstudio_Web页面参数显示.jpg width=80%/>
</div>


&emsp;&emsp;
参数设置时，需要添加4个页面参数，用于收集用户输入。目的是为了在点击按键时，将这些参数下发给设备端。
首先，我们需要创建几个页面变量，用来储存输入的参数。
<div align="center">
<img src=./../../../images/电子价签系统/iotstudio_管理页面变量.jpg width=80%/>
</div>

<div align="center">
<img src=./../../../images/电子价签系统/iotstudio_页面变量值改变.jpg width=80%/>
</div>

<div align="center">
<img src=./../../../images/电子价签系统/iotstudio_页面变量交互.jpg width=80%/>
</div>

&emsp;&emsp;
如上配置完成之后，当在更新价签设置文本框中输入文字，对应的页面变量值也会发生改变。当我们在点击``设置参数``和``更新价签``按钮时，将对应的页面参数下发就可以实现了。

&emsp;&emsp;
对于图片下发，也是同样的道理，这里我们用一种直观的方式来进行设置，双击图片，可以将图片的buf设置到对应的页面变量中。
<div align="center">
<img src=./../../../images/电子价签系统/iotstudio_页面图片展示.jpg width=80%/>
</div>

<div align="center">
<img src=./../../../images/电子价签系统/iotstudio_图片参数设置.jpg width=80%/>
</div>

&emsp;&emsp;
这里的图片Buf就是前面提到的使用showMonoBuf.py显示的字符串。这里设置好后，意思就是双击图片，就能将图片对应的buf字符串赋值给对应的页面变量。

&emsp;&emsp;
最后配置按键。通过点击按键，将页面数据下发到设备端。
这里添加了两个按键，第一个按键时设置参数按键，点击这个按键，会把所有输入框中的信息都下发到设备端。
第二个按键是更新价签按键。点击这个按键后，会下发sal_update参数，设备端接收到该参数后，才真正进行墨水屏屏幕刷新。
> 原因： 目前 iotstudio Web 页面数据下发不能一次将所有数据进行下发，只能一个数据一个数据单独进行下发，对于设备端来说，物品名称，物品价格，折扣力度，物品图片是4个参数，设备端会单独收到4次消息，为了避免墨水屏每次收到信息都进行屏幕刷新，所以设置参数按钮把这些参数下发后，设备端仅仅是把相关参数保存起来。当更新价签参数下发后，设备端才真正去做屏幕刷新的动作。用于保护墨水屏。

<div align="center">
<img src=./../../../images/电子价签系统/iotstudio_页面按钮交互.jpg width=80%/>
</div>

<div align="center">
<img src=./../../../images/电子价签系统/iotstudio_页面参数下发.jpg width=80%/>
</div>


&emsp;&emsp;
编辑完成后，进行保存发布，这样就可以在产品中找到了。可以直接在线进行调试。

<div align="center">
<img src=./../../../images/电子价签系统/iotstudio_页面保存发布.jpg width=80%/>
</div>

<div align="center">
<img src=./../../../images/电子价签系统/iotstudio_页面发布地址.jpg width=80%/>
</div>

&emsp;&emsp;
到这里我们已经完成了IoT云端的所有配置，接下来就是完成设备端开发并完成设备属性的上报。

<br>

## 5、设备端开发之云端联动
### 5.1、开发配置

&emsp;&emsp;
前面已经介绍了设备端开发的一些知识，包括项目创建和代码介绍，这里主要介绍和云端相关的一些设置。

1. **修改路由器名称及密码**

&emsp;&emsp;
修改工程里main.py中wifiSsid和wifiPassword的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在""符号中间）。

```python
# Wi-Fi SSID和Password设置
wifiSsid = "请填写您的路由器名称"
wifiPassword = "请填写您的路由器密码"
```

&emsp;&emsp;
修改完成之后get_wifi_status函数中的nm.connect(wifiSsid, wifiPassword) 语句就会连接读者自己设定的路由器。

2. **修改设备端三元组**

&emsp;&emsp;
修改本工程里main.py中productKey、deviceName和deviceSecret的值为读者创建的物联网设备的三元组信息，具体获取方式如下图所示：

<div align="center">
<img src=./../../../images/电子价签系统/三元组查看.jpg width=80%/>
</div>

3. **设备远端联调**

&emsp;&emsp;
前面介绍电子价签开发的时候，关掉了联网的部分，仅用于本地界面设计，当需要联网时，需要再将这部分代码打开。

```python
if __name__ == '__main__':

    wlan = network.WLAN(network.STA_IF)    #创建WLAN对象 
    get_wifi_status()
    connect_lk(productKey, deviceName, deviceSecret)

    # 初始化墨水屏
    priceTagObj = ShowData(14500000, Pin(17), Pin(2), Pin(16), Pin(4))
    # 初始化显示
    updating = 1
    gc.collect()
    priceTagObj.show(name=sal_name, sel='$ '+sal_price, offerta='-'+sal_offerta+'%')
    gc.collect()
    updating = 0
```

部分代码解析：
云端下发的参数会在函数 def on_props(request) 中进行处理。将云端下发的参数保存到全局变量中。
只有接收到的 sal_update 参数时，才会刷新墨水屏。

```python
# 设置props 事件接收函数（当云平台向设备下发属性时）
def on_props(request):
    # global post_default_value
    global arrayBuf, sal_update, sal_name, sal_price, sal_offerta, sal_pic, updating
    try:
        props = eval(request['params'])
        print(props)

        # 获取 sal_name 参数
        if 'sal_name' in props.keys():
            sal_name = props['sal_name']
            print(sal_name)
        # 获取 sal_price 参数
        if 'sal_price' in props.keys():
            sal_price = props['sal_price']
            print(sal_price)
        # 获取 sal_offerta 参数
        if 'sal_offerta' in props.keys():
            sal_offerta = props['sal_offerta']
            print(sal_offerta)
        # 获取 sal_pic 参数
        if 'sal_pic' in props.keys():
            sal_pic = props['sal_pic']
            # print(sal_pic)
            print("recevied sal pic")

        if 'sal_update' in props.keys():
            sal_update = props['sal_update']
            print(sal_update)
            # 判断是否需要更新
            if sal_update == 1:
                if len(sal_pic) % 2 == 0:
                    picBuf = binascii.unhexlify(sal_pic)
                    # 改变显示图形buf
                    arrayBuf = bytearray(picBuf)
                    # print(arrayBuf)

                # 刷新屏幕
                # 墨水屏刷新慢，加入保护
                if updating == 0:
                    updating = 1
                    gc.collect()
                    priceTagObj.show(name=sal_name, sel='$ '+sal_price, offerta='-'+sal_offerta+'%', byteBuf=arrayBuf)
                    gc.collect()
                    updating = 0

                # 上传更新设置, 图片更新后才会更新
                sal_update = 0
                post_default_value()

    except Exception as e:
        print(e)

```

<br>

## 6、运行结果
程序运行后，可以在 Web 网页上查看到当前墨水屏中的设置的参数。

<div align="center">
<img src=./../../../images/电子价签系统/开机默认画面.jpg width=80%/>
</div>

<br>

&emsp;&emsp;
通过设置 Web 页面参数进行价签更新，可以看到墨水屏价签变化。

<div align="center">
<img src=./../../../images/电子价签系统/电子价签更改.jpg width=80%/>
</div>


&emsp;&emsp;
到此为止，电子价签系统的案例就已经完成了。

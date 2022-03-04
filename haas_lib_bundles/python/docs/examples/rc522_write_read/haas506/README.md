# RC522读卡


## 1.简介
&emsp;&emsp;
本案例使用了RC522模块和一张ic卡实现了写卡和读卡功能。该模块使用SPI进行通信。

&emsp;&emsp;
硬件模块如下图所示：
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_rc522_1.JPG width=60%/>
</div>
<br>


&emsp;&emsp;
硬件模块PIN管脚如下表所示：
<table align="center" class="table table-bordered table-striped table-condensed">
    <tr>
        <td>PIN</td>
	    <td>功能</td>
        <td>描述</td>
    </tr>
    <tr>
        <td>SDA</td>
	    <td>输入</td>
        <td>SPI口片选</td>
    </tr>
    <tr>
        <td>SCK</td>
	    <td>输入</td>
        <td>SPI口时钟</td>
    </tr>
    <tr>
        <td>MOSI</td>
	    <td>输入</td>
        <td>SPI口从机输入</td>
    </tr>
    <tr>
        <td>MISO</td>
	    <td>输出</td>
        <td>SPI口从机输出</td>
    </tr>        
    <tr>
        <td>IRQ</td>
	    <td> -</td>
        <td>-</td>
    </tr> 
    <tr>
        <td>GND</td>
	    <td>电源</td>
        <td>电源地</td>
    </tr> 
    <tr>
        <td>RST</td>
	    <td>输入</td>
        <td>复位</td>
    </tr> 
    <tr>
        <td>3.3V</td>
	    <td>电源</td>
        <td>电源正</td>
    </tr> 
</table>
<br>


&emsp;&emsp;
扇区介绍：
<br>
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_rc522_3.png width=60%/>
</div>

<br>
&emsp;&emsp;&emsp;&emsp;（1）每张IC卡都有16个扇区，包括1个公共区和15个数据区
<br>
&emsp;&emsp;&emsp;&emsp;（2）每个扇区有4个块，每个块占16字节
<br>
&emsp;&emsp;&emsp;&emsp;（3）第0扇区的块0为厂商代码，已经固化。包括芯片序列号UID，ATQA和SAK
<br>
&emsp;&emsp;&emsp;&emsp;（4）每个扇区的块3为控制块，用来存放密码和控制权限 ，不能用来存储数据
<br>
&emsp;&emsp;&emsp;&emsp;（5）每个扇区的块0，块1，块2可以用来存储数据（扇区0的块0除外）
<br>
&emsp;&emsp;&emsp;&emsp;（6）每个扇区块3的前6个字节为KeyA，后6个字节为KeyB。中间的4个字节为存储控制
<br>
&emsp;&emsp;&emsp;&emsp;（7）扇区的地址：扇区0块0的地址为0x00,扇区0块1的地址为0x01...以此类推得到其他扇区块的地址

<br>

&emsp;&emsp;
加密：
<br>
&emsp;&emsp;&emsp;&emsp;（1）非加密卡中所有扇区的KEYA和KEYB数值都是默认值FFFFFFFFFFFF
<br>
&emsp;&emsp;&emsp;&emsp;（2）加密卡中，其中有扇区的KEYA和KEYB不等于FFFFFFFFFFFF，部分扇区加密的卡称半加密卡，所有扇区都加密的卡称全加密卡
<br>
  

## 2.准备
（1）硬件器材
1. HaaS 506开发板一套
2. RC522模块一个
3. ic卡一张
4. 连接线若干
<br>



（2）硬件模块接线
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_rc522_2.jpg width=80%/>
</div>
<br>

&emsp;&emsp;将rc522模块的SDA、SCK、MOSI、MISO、GND、RST、3.3V引脚接入到haas506开发板的相应位置。需要注意的有：很多rc522模块没有标记CS引脚,只有SDA引脚，此时SDA即为CS；board.json将gpio31设置成了rst，实际测试的时候，有没有rst都可以运行程序；在spi写/读寄存器时，需要切换cs的高低电平值。如下图所示：

<div align="center">
<img src=./../../../images/haas506案例图片/haas506_rc522_4.jpg width=80%/>
</div>
<br>


## 3.物联网平台开发
&emsp;&emsp;
（1）在阿里云物联网平台创建一个产品
<br>
&emsp;&emsp;&emsp;&emsp;填写产品的名称，如“example”；选择自定义品类、直连设备、蜂窝方式、ICA标准数据格式（Alink JSON）。校验类型和认证方式选择默认选项即可。点击确认，完成产品创建。
<br>
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_rc522_5.jpg width=80%/>
</div>
<br>
&emsp;&emsp;
（2）点击查看该产品，打开动态注册
<br>
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_rc522_6.jpg width=80%/>
</div>
<br>


&emsp;&emsp;
（3）添加自定义功能
<br>
&emsp;&emsp;&emsp;&emsp;点击产品的“功能定义”，点击编辑草稿，设置上报的属性，如下图所示：
<br>
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_rc522_7.jpg width=80%/>
</div>
<br>
<br>
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_rc522_8.jpg width=80%/>
</div>
<br>
<br>
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_rc522_9.jpg width=80%/>
</div>
<br>


（4）发布上线

&emsp;&emsp;&emsp;&emsp;当自定义功能设置完毕，点击发布上线，确认发布物模型上线。
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_rc522_10.jpg width=80%/>
</div>
<br>
<br>
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_rc522_11.jpg width=80%/>
</div>
<br>

（5）添加一个设备
<br>
&emsp;&emsp;&emsp;&emsp;完成产品的创建和物模型属性的设置之后，在该产品下添加一台设备。此时需要填入设备的DeviceName,可以将haas506开发板的IMEI号填入到DeviceName中。点击确认，设备添加完毕。
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_rc522_12.jpg width=80%/>
</div>
<br>
&emsp;&emsp;&emsp;&emsp;此时的设备还未激活，在后续的操作下，激活该设备。
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_rc522_13.jpg width=80%/>
</div>
<br>



## 4.程序修改

&emsp;&emsp;（1）从该产品的详情页中获得productKey和productSecret的值
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_rc522_14.jpg width=80%/>
</div>
<br>
&emsp;&emsp;（2）在代码的相应位置修改productKey和productSecret的值

```python
productKey = "yourProductKey"
productSecret = "yourProductSecret"
     
```
<br>
&emsp;&emsp;（3）烧录程序
<br>
&emsp;&emsp;&emsp;&emsp;在VS code中点击“部署运行”，当出现“Please reboot the board manually”时，重启一下Haas506开发板。
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_rc522_15.jpg width=80%/>
</div>
<br>
&emsp;&emsp;&emsp;&emsp;如果没有出现 “but..”，即程序烧录成功
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_rc522_16.jpg width=80%/>
</div>
<br>

## 5.运行结果

### &emsp;&emsp;（1）程序执行步骤
&emsp;&emsp;&emsp;&emsp;&emsp;首先进行网络连接，然后进行动态注册，连接到阿里云物联网平台。然后往卡中的指定扇区块写数据和从指定扇区块中读数据，最后将IC卡的UID（扇区0块0数据中的前四个字节）上报到云端。

+ 写数据
&emsp;&emsp;
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_rc522_20.jpg width=80%/>
</div>
<br>

+ 读数据
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_rc522_21.jpg width=80%/>
</div>
<br>

+ 读取的数据
```python
New card detected
  - tag type: 0x10
  - uid  : 0x1ad15f83

data: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 255]
     
```

### &emsp;&emsp;（2）查看设备是否激活

<div align="center">
<img src=./../../../images/haas506案例图片/haas506_rc522_17.jpg width=80%/>
</div>
<br>

### &emsp;&emsp;（3）查看物模型数据

&emsp;&emsp;&emsp;&emsp;&emsp;未刷卡时的物模型：
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_rc522_18.jpg width=80%/>
</div>
<br>

&emsp;&emsp;&emsp;&emsp;&emsp;刷卡时的物模型：
<br>
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;刷卡之后，会将卡的UID信息上报到云端
<div align="center">
<img src=./../../../images/haas506案例图片/haas506_rc522_19.jpg width=80%/>
</div>
<br>



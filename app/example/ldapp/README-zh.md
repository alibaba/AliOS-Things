#### __[背景]：__
软件：AliOS-Things, github developer分支
硬件：Stm32l476rg-nucleo(带Arduino接口)开发板 + Sensors（Arduino I2C外接）+ MK3080 Wifi模组（Arduino接口）
目标：Local设备端与云端可以进行数据交互，如local设备端sensor数据上云，或云端可以下发命令控制板载设备（如LED灯）。

注：请注意mk3080模组固件版本，如需升级（V2.0），请参考：[https://lark.alipay.com/aliosthings/wiki/nkf9rx](https://lark.alipay.com/aliosthings/wiki/nkf9rx)

#### __[准备]：__

1，飞线(开发板本身的bug)，解释如下, 飞线如下面红线(图1)，实际可参考图2接线。
   a) <span data-type="color" style="color:rgb(25, 31, 37)"><span data-type="background" style="background-color:rgb(204, 230, 255)">开发板的外接arduino(CN9)无法直接给出UART, 原因：CN9中的D0,D1是与st-link复用同一个USART2_TX(PA2,PA3)且未    连接，需背部sb62/sb63焊接0Ohm电阻方可使用。</span></span>
   b) <span data-type="color" style="color:rgb(25, 31, 37)"><span data-type="background" style="background-color:rgb(204, 230, 255)">如果需要额外的串口用来连接wifi模组，需要外部飞线连接PA9/PA10（CN9的D2，CN5的D8，还有GND）。</span></span>
<div data-type="alignment" data-value="center" style="text-align:center">
  <div data-type="p">
    <div id="9acutm" data-type="image" data-display="block" data-align="left" data-src="https://cdn.nlark.com/lark/0/2018/png/107167/1533736524093-f78ffd23-6648-4beb-903d-3a32e7d5a9f7.png" data-width="826">
      <img src="https://cdn.nlark.com/lark/0/2018/png/107167/1533736524093-f78ffd23-6648-4beb-903d-3a32e7d5a9f7.png" width="826" />
    </div>
  </div>
</div>

<div data-type="alignment" data-value="center" style="text-align:center">
  <div data-type="p">图1. stm32l476rg-nucleo外接arduino接口原理图</div>
</div>




![飞线图.png | left | 826x707](https://cdn.nlark.com/lark/0/2018/png/107167/1534240863450-1f4bd991-e227-4fd4-9db3-24322a3cfc43.png "")

<div data-type="alignment" data-value="center" style="text-align:center">
  <div data-type="p">图2. 实际飞线图</div>
</div>


板载资源：
LED总共三颗，分别是：
LD1: ST-LINK烧录指示灯。
LD2: 可配(PA5，默认)， 或(PB13, 需要SB29 0 ohm电阻连接)。本次以LED2控制为例。
LD3: 5V电源指示灯。

2, 本次使用ST的温湿度sensor hts221，通过Arduino I2C接口外接，所以只需连接四根杜邦线: 3.3V, GND, SCL, SDA。
    接线图略。

3, 三元组申请：linkdevelop.aliyun.com， 使用正确的三元组替换"PRODUCT\_KEY", "DEVICE\_NAME", "DEVICE\_SECRET"( 代码路径：example/ldapp/mqtt-example.c)

4, 开发环境setup，可参考：[http://aliosthings.io/#/quickstart](http://aliosthings.io/#/quickstart)， 确保：a) aos-cube安装OK（可以编译/烧录），b) 使用github developer 分支

5，云端自定义功能(物模型)，LD平台: linkdevelop.aliyun.com，点击设备开发-->产品开发-->查看-->自定义功能, 如下图所示，增加如下功能属性：
a) report\_sensor, 设备端上报到云端，包括humi和temp。
b) cmd\_led, 云端发送命令控制设备端LED灯开/关。

注意：__标识符__需要与代码中对应起来，代码位置如：example/ldapp/mqtt-example.c，
#define PROP\_POST\_FORMAT\_HUMITEMP    "{\"<span data-type="color" style="color:#F5222D">report_sensor</span>\":{\"<span data-type="color" style="color:#F5222D">Humi</span>\":%f,\"<span data-type="color" style="color:#F5222D">Temp</span>\":%f}}"
#define PROP\_SET\_FORMAT\_CMDLED       "\"<span data-type="color" style="color:#F5222D">cmd_led</span>\":"


![image.png | left | 826x275](https://cdn.nlark.com/lark/0/2018/png/107167/1534242589145-7b757496-5757-4046-a895-c6b5de63b7c0.png "")


#### 下载/编译/烧录/运行
`git clone https://github.com/alibaba/AliOS-Things.git
`
`cd AliOS-Things 
`
`git checkout developer`
`aos make ldapp@stm32l476rg-nucleo`
`aos upload ldapp@stm32l476rg-nucleo`

a) 设备端温湿度数据-->云端
运行后，在串口终端中输入: netmgr connect  \*ssid   \*passwd，在串口会产生log如下。
`[043490]<V> packet-id=0, publish topic msg={"id":"11","version":"1.0","method":"thing.event.property.post","params":{"report_sensor":{"Humi":71.000000,"Temp":25.400000}}}`
此时在云端可以实时查看从当前设备端上报到云端的温湿度(产品开发-->查看-->设备开发-->调试)，如下图。


![image.png | left | 826x307](https://cdn.nlark.com/lark/0/2018/png/107167/1534243974274-6714f841-c162-4c04-b28a-aa242438f645.png "")


b) 云端控制设备端LED2灯亮/灭

如上图中，选择调试功能：控制设备端LED(cmd\_led)， 方法：设置。内容cmd\_led如果设置为1，则控制开发板LED2点亮；如果设置为0，则会控制LED2熄灭。
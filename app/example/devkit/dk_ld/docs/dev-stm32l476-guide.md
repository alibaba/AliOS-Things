## 准备
__为了更快上手，使用前请先完成__[开发准备](dev-prepare-guide)__。__ 

## __背景__
软件：AliOS-Things, github developer分支  
硬件：Stm32l476rg-nucleo(带Arduino接口)开发板 + Sensors（Arduino I2C外接）+ MK3080 Wifi模组（Arduino接口）  
目标：Local设备端与云端可以进行数据交互，如local设备端sensor数据上云，或云端可以下发命令控制板载设备（如LED灯）  

## __硬件__
#### 硬件接线  
1，飞线(开发板本身的bug)，解释如下, 飞线如下面红线(图1)，实际可参考图2接线。  
   a) 开发板的外接arduino(CN9)无法直接给出UART, 原因：CN9中的D0,D1是与st-link复用同一个USART2_TX(PA2,PA3)且未连接，需背部sb62/sb63焊接0Ohm电阻方可使用。  
   b) 如果需要额外的串口用来连接wifi模组，需要外部飞线连接PA9/PA10（CN9的D2，CN5的D8，还有GND）。   
![接口原理图](https://img.alicdn.com/tfs/TB1HCFkwDqWBKNjSZFxXXcpLpXa-1063-688.png)    
图1. stm32l476rg-nucleo外接arduino接口原理图   

![飞线图.png | left | 826x707](https://img.alicdn.com/tfs/TB1tjaEwmMmBKNjSZTEXXasKpXa-985-843.png)  
图2. 实际飞线图   


2, 硬件接线   

![image.png | left | 826x376](https://img.alicdn.com/tfs/TB1cpJlwA7mBKNjSZFyXXbydFXa-1357-617.png)  


3，板载资源：   
LED总共三颗，分别是：   
LD1: ST-LINK烧录指示灯。   
LD2: 可配(PA5，默认)， 或(PB13, 需要SB29 0 ohm电阻连接)。本次以LED2控制为例。   
LD3: 5V电源指示灯。   

## 软__件__  
#### 驱动安装   
Windowns：需下载STM32开发板的驱动，查看[ST-LINK USB Driver](http://www.st.com/en/development-tools/stsw-link009.html)。Mac/Ubuntu可免装。   

#### 下载/编译/烧录   

`git clone https://github.com/alibaba/AliOS-Things.git 
`
`cd AliOS-Things 
`
`git checkout developer`
`aos make ldapp@stm32l476rg-nucleo`
`aos upload ldapp@stm32l476rg-nucleo`

__注：__   
编译前参考[开发准备](dev-prepare-guide)(4.2节三元组和标识符)，更改正确的三元组。   
编译/烧录/串口等也可以参考[开发准备](dev-prepare-guide)(第3节IDE使用)，使用IDE进行。   
## 运行
#### 配网
运行后，在串口终端输入 `netmgr connect <ssid> <password>` 命令配网，配网成功后串口终端会输出如下日志。 
> 注：<ssid>,<password>是指wifi的用户名和密码。  



![image | left](https://img.alicdn.com/tfs/TB1JiAxXbPpK1RjSZFFXXa5PpXa-662-138.png)   


log__关键信息如下：__
[043490]<V> packet-id=0, publish topic msg={"id":"11","version":"1.0","method":"thing.event.property.post","params":{"report\_sensor":{"<span data-type="color" style="color:#2F54EB">Humi</span>":71.000000,"<span data-type="color" style="color:#2F54EB">Temp</span>":25.400000}}}

a) 设备端温湿度数据-->云端 
此时在云端可以实时查看从当前设备端上报到云端的温湿度(产品开发-->查看-->设备开发-->调试)，如下图。 


![image.png | left | 826x307](https://img.alicdn.com/tfs/TB1tYgxXhTpK1RjSZFKXXa2wXXa-1475-549.png)   



b) 云端控制设备端LED2灯亮/灭 

如上图中，选择调试功能：控制设备端LED(cmd\_led)， 方法：设置。内容cmd\_led如果设置为1，则控制开发板LED2点亮；如果设置为0，则会控制LED2熄灭。 


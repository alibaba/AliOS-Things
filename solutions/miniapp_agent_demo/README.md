@page miniapp_agent_demo 蓝牙配网：通过支付宝小程序为HaaS板便捷配网示例
# 1. 案例简介
miniapp_agent_demo是我们提供的小程序配网、上云配套固件。通过配套的“HaaS小程序”，开发者可以快速实现设备的配网、上云、连接 IoT Studio 快速实现应用开发及部署。
其完成的主要功能包括：

- 蓝牙配网
    上电启动蓝牙配网
    连接小程序指定的 Wi-Fi
    Ping "aliyun.com" 并将连接结果返回至小程序端显示

- LinkSDK 连接物联网平台
    接收小程序下发的三元组
    连接物联网平台并运行业务代码

该示例的运行依赖下述基本功能完成对接：
- 内核的任务和中断运行正常
- 系统tick定时器正常运行
- 蓝牙通信
- Wi-Fi连接

*方案限制*：目前不支持中文SSID和中文密码。

# 2. 物料清单

## 2.1 HaaS100 硬件

[HaaS100 硬件简介](https://help.aliyun.com/document_detail/184426.html)

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01XxD6Xo217CB3FZnEU_!!6000000006937-2-tps-746-497.png" style="max-width:800px;" />
</div>

# 3. 设备端实现


# 4. 完整使用流程
点击这里(https://cloud.video.taobao.com/play/u/1632932/p/1/e/6/t/1/307129850804.mp4)观看演示视频。
## 烧录固件至开发板
*> 选择解决方案: 小程序配网 上云 应用案例*
*> 选择开发板: Haas100 board configure*

-- 编译固件可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。
-- 烧录固件可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)。

## 小程序配网
- 准备一个连接至互联网的 Wi-Fi AP。它可以由手机热点产生，也可以通过路由器配置。在本案例中，热点信息如下：
    SSID: HaaS-AP
    PassWord: MiniAppAgentDemo
- 将烧录好固件的设备上电
- 在“支付宝”中搜索并打开“HaaS小程序”
- 点击底部“配网”Tab
- 输入目标AP的SSID及Password
- 打开手机蓝牙
- 点击扫描配网设备（授予蓝牙权限）
- 在扫描列表中选择待配网设备
- 点击“发起配网”
- 等待配网完成，显示配网状态。当“配网状态”中显示“设备IP”时，代表配网成功。
  
<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01i0HONM1TeS0OHtHk0_!!6000000002407-1-tps-626-1280.gif" style="max-width:800px;" />
</div>

## 配置物联网平台
点击这里(https://iot.console.aliyun.com/)进入物联网平台
### 创建产品

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01dvGFo01DIR79OnB1b_!!6000000000193-2-tps-4096-2560.png" style="max-width:800px;" />
</div>

点击 公共实例-产品-新建产品，选择自定义品类，点击创建。

### 配置物模型并发布

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01GcyU4T1vGutcRUrX6_!!6000000006146-2-tps-4096-2560.png" style="max-width:800px;" />
</div>

导入代码目录下的物模型文件，发布物模型。
### 创建设备

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01sOZ4lC26UA7drDcmQ_!!6000000007664-2-tps-4096-2560.png" style="max-width:800px;" />
</div>

在该产品下创建设备，获得三元组。
### 生成三元组二维码
点击这里(https://haas.iot.aliyun.com/gen-qrcode)，前往二维码生成工具。将三元组依次拷贝，生成二维码供小程序读取。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01DrQXDt1eS33GNbyf1_!!6000000003869-2-tps-4096-2560.png" style="max-width:800px;" />
</div>

## 小程序上云

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01brO1OO1zOwINrkWec_!!6000000006705-1-tps-626-1280.gif" style="max-width:800px;" />
</div>

- 点击底部“上云”Tab
- 输入目标三元组
  也可以使用二维码生成工具（https://haas.iot.aliyun.com/gen-qrcode）
  在页面输入三元组之后，点击生成二维码，并使用小程序“扫码获取三元组”
- 打开手机蓝牙
- 点击扫描设备（授予蓝牙权限）
- 在扫描列表中选择设备
- 点击“下发三元组”
- 等待三元组下发完成
## 配置 IoT Studio
点击这里(https://studio.iot.aliyun.com/)进入IoT Studio
### 创建项目

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01rWDZV31wO4hQAMVpg_!!6000000006297-2-tps-4096-2560.png" style="max-width:800px;" />
</div>

### 关联物联网平台

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01f9csYR25ePMtZTaZD_!!6000000007551-2-tps-4096-2560.png" style="max-width:800px;" />
</div>


<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01rWDZV31wO4hQAMVpg_!!6000000006297-2-tps-4096-2560.png" style="max-width:800px;" />
</div>

### 创建移动端应用

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01EqkRaK1Ii9QmrCITV_!!6000000000926-2-tps-4096-2560.png" style="max-width:800px;" />
</div>

编辑移动应用界面，并完成数据绑定。
本案例中，使用6个按钮对应 HaaS100 开发板上6个灯的亮灭。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01wNItls28Bxc44sieG_!!6000000007895-2-tps-4096-2560.png" style="max-width:800px;" />
</div>

点击右上角预览图标，进入预览。
### 预览移动端应用

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01Mp1Wfb1iKUzK1xtLj_!!6000000004394-2-tps-4096-2560.png" style="max-width:800px;" />
</div>

## 小程序预览移动端应用

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01zoMN1X1RqEy3X2R4P_!!6000000002162-1-tps-626-1280.gif" style="max-width:800px;" />
</div>

- 点击底部“IoT Studio”Tab
- 扫描 IoT Studio 中提供的预览二维码


# 5. 总结
本实验提供了小程序方法来实现设备的快速配网、上云，帮助开发者快速连接物联网平台。开发者们可以基于此案例，开发更多的联网应用。

@page haas_mini_app haas_mini_app


# 概述
HaaS超级小程序。 开发者借助本小程序，配合 ble_netconfig 以及 linksdk 组件，可以实现设备快速配网、上云、应用。


## 功能支持
HaaS超级小程序主要支持如下功能：

- 通过蓝牙连接设备，下发SSID及Password进行配网
- 通过蓝牙连接设备，下发设备三元组信息，将设备连接至物联网平台
- 预览 IoT Studio 移动开发页面

## 版权说明
> Apache license v2.0


## 目录结构

```
├── README.md
├── app.acss
├── app.js
├── app.json
├── mini.project.json
├── pages
│   ├── linkIot     // 连接 IoT Studio
│   ├── linkLP      // 设备上云页面
│   ├── netCfg      // 蓝牙配网页面
│   └── webview     // 承载 IoT Studio 页面
└── snapshot.png
```

## 依赖组件

- osal_aos
- ble_host
- netmgr
- link_sdk


# 公版小程序使用说明
## 获取小程序
支付宝搜索“HaaS小程序”

## 配网
![netCfg](https://img.alicdn.com/imgextra/i3/O1CN01i0HONM1TeS0OHtHk0_!!6000000002407-1-tps-626-1280.gif)
- 点击底部“配网”Tab
- 输入目标AP的SSID及Password
- 打开手机蓝牙
- 点击扫描配网设备（授予蓝牙权限）
- 在扫描列表中选择待配网设备
- 点击“发起配网”
- 等待配网完成，显示配网状态

## 上云
![linkLp](https://img.alicdn.com/imgextra/i1/O1CN01brO1OO1zOwINrkWec_!!6000000006705-1-tps-626-1280.gif)
- 点击底部“上云”Tab
- 输入目标三元组
  也可以使用二维码生成工具（https://haas.iot.aliyun.com/gen-qrcode）
  在页面输入三元组之后，点击生成二维码，并使用小程序“扫码获取三元组”
- 打开手机蓝牙
- 点击扫描设备（授予蓝牙权限）
- 在扫描列表中选择设备
- 点击“下发三元组”
- 等待三元组下发完成

## 预览 IoT Studio
![linkIot](https://img.alicdn.com/imgextra/i3/O1CN01zoMN1X1RqEy3X2R4P_!!6000000002162-1-tps-626-1280.gif)
- 点击底部“IoT Studio”Tab
- 扫描 IoT Studio 中提供的预览二维码

# 常见问题
```
Q：是否有IOS版的配网工具
A：有的，小程序支持IOS及安卓系统

Q：小程序是否支持二次开发
A：我们会在未来开源本小程序的代码，并提供相关的SDK，帮助用户快速实现

Q：是否支持微信小程序
A：小程序的代码开源后，经过简单修改，即可适配微信小程序
```


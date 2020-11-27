var app = getApp()
// 获取小程序全局对象
import { IotApi } from '/lib/iot-packet.js'

Page({
  data: {

  },

  haasCar: {
    DeviceName: '<- DeviceName ->',   // 填入目标设备 DeviceName 以及 ProductKey
    ProductKey: '<- ProductKey ->'
  },
  haasCarIotApi: null,

  onLoad() {
    let golbalAccessKey = app.accessKey                           // 获取全局变量 accessKey
    let targetDevice = this.haasCar
    this.haasCarIotApi = new IotApi({ accessKey: golbalAccessKey, device: targetDevice })  // 创建当前设备的IoTApi实例
  },

  async arrowKeyTap(event) {
    let dir = event.target.dataset.direction
    // 读取前端页面传递的属性信息

    var value = await this.haasCarIotApi.getDeviceProperty({ Identifier: dir });
    // 使用api获取对应属性（Identifier）的值 

    console.log(value)
    // 打印获取的属性值

    await this.haasCarIotApi.setDeviceProperty({ Identifier: dir, Value: value == '1' ? 0 : 1 })
    // 将对应属性值置反
  },

});


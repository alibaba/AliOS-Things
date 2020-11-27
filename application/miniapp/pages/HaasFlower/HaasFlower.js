var app = getApp()
import { IotApi } from '/lib/iot-packet.js'

Page({
  data: {
    temperature: 0,
    humidity: 0,
    refreshing: false
  },

  haasFlower: {
    DeviceName: '<- DeviceName ->',   // 填入目标设备 DeviceName 以及 ProductKey
    ProductKey: '<- ProductKey ->'
  },
  haasFlowerIotApi: null,

  onLoad() {
    console.log('page flower onLoad')
    let golbalAccessKey = app.accessKey                           // 获取全局变量 accessKey
    let targetDevice = this.haasFlower
    this.haasFlowerIotApi = new IotApi({ accessKey: golbalAccessKey, device: targetDevice })  // 创建当前设备的IoTApi实例
  },

  onShow() {
    // 页面显示
    console.log('page flower onShow')
  },
  onReady() {
    // 页面加载完成
    console.log('page flower onReady')
  },
  onHide() {
    // 页面隐藏
    console.log('page flower onHide')
  },
  onUnload() {
    // 页面被关闭
    console.log('page flower onUnload')
  },
  onTitleClick() {
    // 标题被点击
    console.log('page flower onTitleClick')
  },
  onPullDownRefresh() {
    // 页面被下拉
    console.log('page flower onPullDownRefresh')
  },
  onReachBottom() {
    // 页面被拉到底部
    console.log('page flower onReachBottom')
  },

  async onPullDownRefresh() {
    // 下拉刷新时触发
    console.log('page flower onPullDownRefresh')
    
    this.setData({ refreshing: true })
    let temp = await this.haasFlowerIotApi.getDeviceProperty({ Identifier: "Temperature" });
    let humd = await this.haasFlowerIotApi.getDeviceProperty({ Identifier: "Humidity" });
    // 更新变量， 同时前端页面刷新数值
    setTimeout(() => {
      this.setData({
        temperature: temp,
        humidity: humd,
        refreshing: false
      })
      my.stopPullDownRefresh();
    }, 2000)
  }

});

var app = getApp();
var ble_util = require("../../utils/ble_util.js")
var hex_util = require("../../utils/hex_util.js")

Page({
  data: {
    ProductKey: '',
    DeviceName: '',
    DeviceSecret: '',

    link_lp_devices: [],
    selected_device_index: 0,

    LinkLpState: {
      scanning: false,
      linking: false,
    },

    LinkState: 0
  },


  notifyCheckInt: null,
  notifyCheckCnt: 0,

  onLoad() {
  },

  genTriadPacket(ProductKey, DeviceName, DeviceSecret) {
    let ret = "ffb0"
    ret += ProductKey.length < 16 ? ('0' + (ProductKey.length).toString(16)) : ((ProductKey.length).toString(16))
    ret += DeviceName.length < 16 ? ('0' + (DeviceName.length).toString(16)) : ((DeviceName.length).toString(16))
    ret += DeviceSecret.length < 16 ? ('0' + (DeviceSecret.length).toString(16)) : ((DeviceSecret.length).toString(16))
    for (let i = 0; i < ProductKey.length; i++) {
      ret += ProductKey.charCodeAt(i) < 16 ? ('0' + ProductKey.charCodeAt(i).toString(16)) : ProductKey.charCodeAt(i).toString(16);
    }
    for (let i = 0; i < DeviceName.length; i++) {
      ret += DeviceName.charCodeAt(i) < 16 ? ('0' + DeviceName.charCodeAt(i).toString(16)) : DeviceName.charCodeAt(i).toString(16);
    }
    for (let i = 0; i < DeviceSecret.length; i++) {
      ret += DeviceSecret.charCodeAt(i) < 16 ? ('0' + DeviceSecret.charCodeAt(i).toString(16)) : DeviceSecret.charCodeAt(i).toString(16);
    }
    return ret
  },

  scanTriadQr() {
    my.scan({
      scanType: ['qrCode'],
      success: (res) => {
        console.log(res.code)
        let Triad = JSON.parse(res.code)
        console.log(Triad)
        if (Triad.ProductKey != undefined && Triad.DeviceName != undefined && Triad.DeviceSecret != undefined) {
          this.setData({
            ProductKey: Triad.ProductKey,
            DeviceName: Triad.DeviceName,
            DeviceSecret: Triad.DeviceSecret
          })
        } else {
          my.alert({ content: "二维码格式错误" });
        }
      },
    });
  },

  async startLinkLpDevicesDiscovery() {
    this.setData({ 'LinkLpState.scanning': true })
    this.setData({ link_lp_devices: [] })

    await ble_util.OpenBluetoothAdapter().catch(err => { console.error(err) })

    let res;
    for (let retry = 10; retry > 0; retry--) {
      console.log(retry)
      res = await ble_util.DiscoveryBLEDevice('FFB0')

      if (res && res.devices != null) {
        console.log(res.devices)
        this.setData({
          link_lp_devices: res.devices
        })
        break;
      }
    }
    console.log(res)
    if (res.devices == null)
      my.alert({ content: res.content })
    this.setData({ 'LinkLpState.scanning': false })
  },

  bindDevicePickerChange(event) {
    this.setData({
      selected_device_index: event.detail.value,
    });
  },

  async sendTriad() {
    if (this.data.ProductKey.length < 1 || this.data.DeviceName.length < 1 || this.data.DeviceSecret.length < 1) {
      my.alert({ content: '请输入正确的三元组信息' })
    }

    await this.OpenBluetoothAdapter().catch(err => { console.error(err) })

    this.setData({ 'LinkLpState.linking': true })
    await ble_util.DisconnectBLEDevice(this.data.net_config_devices[this.data.selected_device_index].deviceId).catch(err => { console.error(err) })

    my.connectBLEDevice({
      deviceId: this.data.link_lp_devices[this.data.selected_device_index].deviceId,
      success: res => {
        console.log('connect with ' + this.data.link_lp_devices[this.data.selected_device_index].deviceId + ' success')
        console.log(res)

        my.notifyBLECharacteristicValueChange({
          state: true,
          deviceId: this.data.link_lp_devices[this.data.selected_device_index].deviceId,
          serviceId: ble_util.UUID_VENDOR_SERVICE[app.globalData.sysname],
          characteristicId: ble_util.UUID_VENDOR_CHAR_NOTIFY[app.globalData.sysname],
          success: () => {
            my.offBLECharacteristicValueChange();
            my.onBLECharacteristicValueChange((res) => {
              console.log('得到响应数据 = ' + res.value);
              console.log(hex_util.hexString2String(res.value))
              this.LinkLpNotifyFormat(res.value)
            });
            console.log('监听成功');
          },
          fail: error => {
            this.setData({ 'LinkLpState.linking': false })
            console.log({ content: '监听失败' + JSON.stringify(error) });
          },
        });

        my.writeBLECharacteristicValue({
          deviceId: this.data.link_lp_devices[this.data.selected_device_index].deviceId,
          serviceId: ble_util.UUID_VENDOR_SERVICE[app.globalData.sysname],
          characteristicId: ble_util.UUID_VENDOR_CHAR_WRITE[app.globalData.sysname],
          value: this.genTriadPacket(this.data.ProductKey, this.data.DeviceName, this.data.DeviceSecret),
          success: res => {
            console.log(res);

            // retry here

            var notifyCheckCnt = 5
            var notifyCheckInt = setInterval(() => {
              if (this.data.LinkLpState.linking == 0) {
                notifyCheckCnt = notifyCheckCnt - 1
                console.log('notifyCheckCnt ' + notifyCheckCnt)
                if (notifyCheckCnt < 0) {
                  clearInterval(notifyCheckInt)
                  this.setData({ 'LinkLpState.linking': false })
                  my.alert({ content: "下发三元组失败：设备无回复" })
                }
              }
            }, 1000)


          },
          fail: error => {
            this.setData({ 'linkLpState.linking': false })
            console.log(error);
          },
        });

      },
      fail: error => {
        this.setData({ 'LinkLpState.linking': false })
        console.log(error);
      },
    })
  },

  LinkLpNotifyFormat(str) {
    console.log("LinkLpNotifyFormat")
    if (hex_util.hexString2String(str) == 'DEVSETOK') {
      console.log("link done")
      this.setData({
        'LinkLpState.linking': false,
        LinkState: 1
      })
    }
  },

  pkOnInput(e) {
    this.setData({
      ProductKey: e.detail.value
    })
  },

  dnOnInput(e) {
    this.setData({
      DeviceName: e.detail.value
    })
  },

  dsOnInput(e) {
    this.setData({
      DeviceSecret: e.detail.value
    })
  },

  onHide() {
    my.closeBluetoothAdapter();
  }

});

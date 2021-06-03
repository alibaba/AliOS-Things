var app = getApp();
var ble_util = require("../../utils/ble_util.js")
var hex_util = require("../../utils/hex_util.js")

Page({
  data: {
    ssid: '',
    ssidList: [],
    password: '',

    net_config_devices: [],
    selected_device_index: 0,

    netCfgState: {
      scanning: false,
      configing: false,
    },

    wifiSelect: false,

    netState: -1,
    ipAddr: null
  },

  onLoad() {
  },

  async startNetConfigDevicesDiscovery() {
    this.setData({ 'netCfgState.scanning': true })
    this.setData({ net_config_devices: [] })

    await ble_util.OpenBluetoothAdapter().catch(err => { console.error(err) })

    let res;
    for (let retry = 10; retry > 0; retry--) {
      console.log(retry)
      res = await ble_util.DiscoveryBLEDevice('FFA0') // TODO 传参数找

      if (res && res.devices != null) {
        console.log(res.devices)
        this.setData({
          net_config_devices: res.devices
        })
        break;
      }
    }

    console.log(res)
    if (res.devices == null)
      my.alert({ content: res.content })
    this.setData({ 'netCfgState.scanning': false })

  },

  bindDevicePickerChange(event) {
    this.setData({
      selected_device_index: event.detail.value,
    });
  },

  async getWiFiList() {
    if (this.data.net_config_devices.length == 0) {
      my.alert({ content: "请选择目标设备" })
    }

    this.setData({ ssidList: [] })
    my.showLoading({ content: "扫描中", mask: true });
    my.connectBLEDevice({
      deviceId: this.data.net_config_devices[this.data.selected_device_index].deviceId,
      success: res => {
        my.notifyBLECharacteristicValueChange({
          state: true,
          deviceId: this.data.net_config_devices[this.data.selected_device_index].deviceId,
          serviceId: ble_util.UUID_VENDOR_SERVICE[app.globalData.sysname],
          characteristicId: ble_util.UUID_VENDOR_CHAR_NOTIFY[app.globalData.sysname],
          success: () => {
            my.offBLECharacteristicValueChange();
            let wifiListHexStr = ""
            let headStr = "FFD"
            let headNum = 0
            my.onBLECharacteristicValueChange((res) => {
              console.log(res)
              console.log('得到响应数据 = ' + res.value);
              console.log(hex_util.hexString2String(res.value))
              if (res.value.indexOf(headStr) == 0) {
                if (res.value.length > 4) {
                  if (res.value[3] == ("" + headNum)) {
                    wifiListHexStr += res.value.slice(4)
                    headNum += 1
                  } else {
                    console.error("数据传输错误，请重试")
                    my.hideLoading();
                  }
                } else {
                  my.hideLoading();
                  console.log(wifiListHexStr)
                  console.log(hex_util.hexString2String(wifiListHexStr))
                  let wifiListStr = hex_util.hexString2String(wifiListHexStr)
                  this.setData({ ssidList: wifiListStr.slice(1).split(")(").slice(0, -2) })
                  if (this.data.ssidList.length > 0) {
                    my.hideLoading();
                    this.setData({ wifiSelect: true })
                  } else {
                    my.hideLoading();
                    my.showToast({
                      type: 'fail',
                      content: '未扫描到可用 Wi-Fi',
                      duration: 2000,
                    });
                  }
                }
              }
            });
            console.log('监听成功');
          },
          fail: error => {
            my.hideLoading();
            console.log({ content: '监听失败' + JSON.stringify(error) });
          },
        });

        my.writeBLECharacteristicValue({
          deviceId: this.data.net_config_devices[this.data.selected_device_index].deviceId,
          serviceId: ble_util.UUID_VENDOR_SERVICE[app.globalData.sysname],
          characteristicId: ble_util.UUID_VENDOR_CHAR_WRITE[app.globalData.sysname],
          value: "FFC0",
          success: res => {
            console.log(res);
            var notifyCheckCnt = 5
            var notifyCheckInt = setInterval(() => {
              if (this.data.ipAddr == null) {
                this.notifyCheckCnt = this.notifyCheckCnt - 1
                console.log('notifyCheckCnt ' + this.notifyCheckCnt)
                if (notifyCheckCnt < 0) {
                  clearInterval(notifyCheckInt)
                  this.setData({ 'netCfgState.configing': false })
                  my.alert({ content: "配网失败，设备无回复" })
                }
              }
            }, 1000)
          },
          fail: error => {
            this.setData({ 'netCfgState.configing': false })
            console.log(error);
          },
        });
      },
      fail: error => {
        this.setData({ 'netCfgState.configing': false })
        console.log(error);
      },
    })
  },

  async setNetConfig() {
    if (this.data.ssid.length < 1) {
      my.alert({ content: '请输入正确的SSID' })
      return
    }
    if (this.data.password.length < 8) {
      my.alert({ content: '请输入正确的密码' })
      return
    }

    await ble_util.OpenBluetoothAdapter().catch(err => { console.error(err) })

    this.setData({ 'netCfgState.configing': true })
    await ble_util.DisconnectBLEDevice(this.data.net_config_devices[this.data.selected_device_index].deviceId).catch(err => { console.error(err) })
    my.connectBLEDevice({
      deviceId: this.data.net_config_devices[this.data.selected_device_index].deviceId,
      success: res => {
        console.log('connect with ' + this.data.net_config_devices[this.data.selected_device_index].deviceId + ' success')
        console.log(res)
        console.log(ble_util.UUID_VENDOR_SERVICE[app.globalData.sysname])

        my.notifyBLECharacteristicValueChange({
          state: true,
          deviceId: this.data.net_config_devices[this.data.selected_device_index].deviceId,
          serviceId: ble_util.UUID_VENDOR_SERVICE[app.globalData.sysname],
          characteristicId: ble_util.UUID_VENDOR_CHAR_NOTIFY[app.globalData.sysname],
          success: () => {
            my.offBLECharacteristicValueChange();
            my.onBLECharacteristicValueChange((res) => {
              console.log('得到响应数据 = ' + res.value);
              console.log(hex_util.hexString2String(res.value))
              this.notifyFormat(res.value)
            });
            console.log('监听成功');
          },
          fail: error => {
            this.setData({ 'netCfgState.configing': false })
            console.log({ content: '监听失败' + JSON.stringify(error) });
          },
        });

        my.writeBLECharacteristicValue({
          deviceId: this.data.net_config_devices[this.data.selected_device_index].deviceId,
          serviceId: ble_util.UUID_VENDOR_SERVICE[app.globalData.sysname],
          characteristicId: ble_util.UUID_VENDOR_CHAR_WRITE[app.globalData.sysname],
          value: this.genNetConfigPacket(this.data.ssid, this.data.password),
          success: res => {
            console.log(res);
            console.log('写入数据成功！' + this.genNetConfigPacket(this.data.ssid, this.data.password));

            // retry here
            var notifyCheckCnt = 5
            var notifyCheckInt = setInterval(() => {
              if (this.data.ipAddr == null) {
                notifyCheckCnt = notifyCheckCnt - 1
                console.log('notifyCheckCnt ' + notifyCheckCnt)
                if (notifyCheckCnt < 0) {
                  clearInterval(notifyCheckInt)
                  this.setData({ 'netCfgState.configing': false })
                  my.alert({ content: "配网失败，设备无回复" })
                } else if (this.data.netCfgState.configing == false) {
                  clearInterval(notifyCheckInt)
                }
              }
            }, 1000)
          },
          fail: error => {
            this.setData({ 'netCfgState.configing': false })
            console.log(error);
          },
        });
      },
      fail: error => {
        this.setData({ 'netCfgState.configing': false })
        console.log(error);
      },
    })
  },

  notifyFormat(str) {
    if (str.length > 2) {
      let maybeIp = hex_util.hexString2String(str)
      console.log(maybeIp)
      if (this.StrIsIp(maybeIp)) {
        this.setData({
          ipAddr: maybeIp
        })
        this.setData({ 'netCfgState.configing': false })
      }
    }
  },

  genNetConfigPacket(ssid, password) {
    // [(num)ssid_len, (num)psd_len, (str)ssid, (str)psd] => acsii => hex str
    // 9,               6,            'A','U'.. '9','1','4'...
    // 06               09            4155524F5241393134323238383431
    // console.log(this.stringToBytes(String.fromCharCode(ssid.length) + String.fromCharCode(password.length) + ssid + password))
    let ret = "ffa0"
    ret += ssid.length < 16 ? ('0' + (ssid.length).toString(16)) : ((ssid.length).toString(16))
    ret += password.length < 16 ? ('0' + (password.length).toString(16)) : ((password.length).toString(16))
    for (let i = 0; i < ssid.length; i++) {
      ret += ssid.charCodeAt(i) < 16 ? ('0' + ssid.charCodeAt(i).toString(16)) : ssid.charCodeAt(i).toString(16);
    }
    for (let i = 0; i < password.length; i++) {
      ret += password.charCodeAt(i) < 16 ? ('0' + password.charCodeAt(i).toString(16)) : password.charCodeAt(i).toString(16);
    }
    return ret
  },

  StrIsIp(str) {
    return /^((2[0-4]\d|25[0-5]|[01]?\d\d?)\.){3}(2[0-4]\d|25[0-5]|[01]?\d\d?)$/.test(str)
  },

  ssidOnInput(e) {
    this.setData({
      ssid: e.detail.value
    })
  },

  passwordOnInput(e) {
    this.setData({
      password: e.detail.value
    })
  },

  bindSSIDPickerChange(event) {
    console.log(event)
    this.setData({
      ssid: this.data.ssidList[event.detail.value]
    })
  },

  wifiSelectCancel() {
    this.setData({ wifiSelect: false })
  },

  wifiPickerOnChange(e) {
    console.log(e)
    this.wifiSelectIndex = e.detail.value
  },

  wifiSelectConform() {
    this.setData({ wifiSelect: false, ssid: this.data.ssidList[this.wifiSelectIndex] })
  },

  onHide() {
    my.closeBluetoothAdapter();
  }

});
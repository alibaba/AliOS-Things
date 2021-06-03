const UUID_VENDOR_SERVICE = { "android": "0000FFA0-0000-1000-8000-00805F9B34FB", "ios": "FFA0" }
const UUID_VENDOR_CHAR_READ = { "android": "0000FFA1-0000-1000-8000-00805F9B34FB", "ios": "FFA1" }
const UUID_VENDOR_CHAR_WRITE = { "android": "0000FFA2-0000-1000-8000-00805F9B34FB", "ios": "FFA2" }
const UUID_VENDOR_CHAR_NOTIFY = { "android": "0000FFA3-0000-1000-8000-00805F9B34FB", "ios": "FFA3" }

function GetBluetoothAdapterState() {
  return new Promise((resolve, reject) => {
    my.getBluetoothAdapterState({
      success: res => {
        console.log(res)
        resolve(res);
      },
      fail: error => {
        reject(error);
      },
    })
  })
}

function OpenBluetoothAdapter() {
  return new Promise((resolve, reject) => {
    my.openBluetoothAdapter({
      success: res => {
        if (!res.isSupportBLE) {
          reject({ content: '抱歉，您的手机蓝牙暂不可用' });
        }
        resolve({ content: '初始化成功！' });
      },
      fail: error => {
        reject({ content: JSON.stringify(error) });
      },
    });
  })
}

function CloseBluetoothAdapter() {
  return new Promise((resolve, reject) => {
    my.offBLECharacteristicValueChange();
    my.closeBluetoothAdapter({
      success: () => {
        resolve({ content: '关闭蓝牙成功！' });
      },
      fail: error => {
        reject({ content: JSON.stringify(error) });
      },
    });
  })
}

function ConnectBLEDevice(deviceId) {
  return new Promise((resolve, reject) => {
    my.connectBLEDevice({
      deviceId: deviceId,
      success: res => {
        resolve({ content: res });
      },
      fail: error => {
        reject({ content: JSON.stringify(error) });
      },
    })
  })
}

function DisconnectBLEDevice(deviceId) {
  return new Promise((resolve, reject) => {
    my.disconnectBLEDevice({
      deviceId: deviceId,
      success: res => {
        resolve({ content: res });
      },
      fail: error => {
        reject({ content: JSON.stringify(error) });
      },
    })
  })
}

function DiscoveryBLEDevice(targetUUID) {
  return new Promise((resolve, reject) => {
    my.startBluetoothDevicesDiscovery({
      allowDuplicatesKey: true,
      success: () => {
        setTimeout(() => {
          my.getBluetoothDevices({
            success: res => {
              console.log(res)
              let _net_config_devices = [];
              for (let index = 0; index < res.devices.length; index++) {
                if (res.devices[index].advertisServiceUUIDs != undefined && res.devices[index].advertisServiceUUIDs[0] == targetUUID) {
                  _net_config_devices.push(res.devices[index]);
                }
              }
              if (_net_config_devices.length > 0) {
                resolve({ devices: _net_config_devices, content: 'success' })
              } else {
                resolve({ devices: null, content: '未搜索到目标设备' })
              }
            },
            fail: error => {
              resolve({ devices: null, content: '获取设备失败' + JSON.stringify(error) });
            },
          });
        }, 500)
      },
      fail: error => {
        resolve({ devices: null, content: '启动扫描失败' + JSON.stringify(error) });
      },
    });
  });
}

module.exports = {
  GetBluetoothAdapterState: GetBluetoothAdapterState,
  OpenBluetoothAdapter: OpenBluetoothAdapter,
  CloseBluetoothAdapter: CloseBluetoothAdapter,
  ConnectBLEDevice: ConnectBLEDevice,
  DisconnectBLEDevice: DisconnectBLEDevice,
  DiscoveryBLEDevice: DiscoveryBLEDevice,
  UUID_VENDOR_SERVICE,
  UUID_VENDOR_CHAR_READ,
  UUID_VENDOR_CHAR_WRITE,
  UUID_VENDOR_CHAR_NOTIFY
}
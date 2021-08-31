import * as std from 'std'
import * as NETWORK from 'NETWORK';

var WIFI_TYPE = 0;
var CELLULAR_TYPE = 1;
var ETHNET_TYPE = 2;

class ADVANCED_LOCATION {
  constructor() {
    this.devType = NETWORK.getType();
    if (this.devType === WIFI_TYPE) {
      std.eval('import * as NETMGR from \'NETMGR\'; globalThis.NETMGR = NETMGR');
    }
    else if (this.devType === CELLULAR_TYPE) {
      std.eval('import * as CELLULAR from \'CELLULAR\'; globalThis.CELLULAR = CELLULAR');
    }
  }


  getAccessApInfo() {
    var info = { mac: null, ip: null, rssi: null };
    var dev_handler = NETMGR.getDev('/dev/wifi0');
    if (this.devType === WIFI_TYPE) {
      if (NETMGR.getState(dev_handler) === 5) {
        var { mac, ip_addr, rssi } = NETMGR.getIfConfig(dev_handler);
        info.mac = mac;
        info.ip = ip_addr;
        info.rssi = rssi;
      } else {
        console.log('ERROR: wifi is not connected');
      }
    } else {
      console.log('ERROR: board does not support wifi');
    }
    return info;
  }

  getAccessedLbsInfo() {
    var info = {
      cellid: null,
      lac: null,
      mcc: null,
      mnc: null,
      signal: null
    };
    if (this.devType === CELLULAR_TYPE) {
      if (CELLULAR.getStatus() === 1) {
        var { cellid, lac, mcc, mnc, signal } = CELLULAR.getLocatorInfo();
        info.cellid = cellid;
        info.lac = lac;
        info.mcc = mcc;
        info.mnc = mnc;
        info.signal = signal;
      } else {
        console.log('ERROR: cellular is not connected')
      }
    } else {
      console.log('ERROR: board does not support cellular')
    }
    return info;
  }
}

export function init() {
  return new ADVANCED_LOCATION();
}

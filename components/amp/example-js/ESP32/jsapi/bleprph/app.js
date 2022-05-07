import * as ble_pripheral from 'ble_pripheral'
import * as network from 'network';
import * as http from 'http';
import * as fs from 'fs';
import * as kv from 'kv';

console.log('\r\n');
console.log('>>>>>>>>> boot.js >>>>>>>>>');

var key = 'oneMinuteCloudKey';
if(kv.getStorageSync(key) === 'SET') {
  console.log('KV oneMinuteCloudKey is SET, skip ble netconfig');
} else {
  console.log('KV oneMinuteCloudKey is RESET, start ble netconfig');
  bleNetConfig();
}

function bleNetConfig() {
  var appCmdJson;
  var wifiSsid, wifiPassword;
  var appScriptFile, appScriptURL;
  var appConfigFile, appConfigURL;
  var service = {service: "{\"s_uuid\":\"0xFFA0\",\"chars_list\":[\
                         {\"char_uuid\":\"0xFFA2\",\"char_permit\":\"RW\",},\
                         {\"char_uuid\":\"0xFFA3\",\"char_permit\":\"NOTIFY\"}]}"};
  var adv_params = {type: 0, adv_data: '020106', scan_rsp_data: '', interval_min: 160, interval_max: 320, channel_map: 7};

  var net = network.openNetWorkClient({devType: 'wifi'});
  var ble_pripheral_instant = ble_pripheral.open({
    deviceName: 'esp-node',
    success: function() {
      console.log('ble_pripheral init success');
    },
    fail: function() {
      console.log('ble_pripheral init failed');
    }
  });
  ble_pripheral_instant.add_service(service);
  ble_pripheral_instant.start_adv(adv_params);

  ble_pripheral_instant.on('connect', function() {
    console.log('ble_pripheral gap connected');
  });
  ble_pripheral_instant.on('disconnect', function() {
    console.log('ble_pripheral gap disconnected');
  });
  ble_pripheral_instant.on('onCharWrite', function(uuid, len, data) {
    var retJson, netConfig, jsonObj;
    if (data.startsWith('{\"cmd\":\"WiFiCon\"') || data.startsWith('{\"cmd\":\"PullCode\"') || data.startsWith('{\"cmd\":\"Reset\"') || data.startsWith('{\"cmd')) {
      if (data.startsWith('{\"cmd\":\"Reset\"')) {
        console.log('recv RESET cmd');
      }
      appCmdJson = data;
    } else {
      appCmdJson += data;
      if (appCmdJson.includes('}}')) {
        if (appCmdJson.includes('{\"cmd\":\"WiFiCon\"')) {
          jsonObj = JSON.parse(appCmdJson);
          wifiSsid = jsonObj.param.ssid;
          wifiPassword = jsonObj.param.pswd;
          console.log('wifi ssid: ' + wifiSsid);
          console.log('wifi pswd: ' + wifiPassword);
          net.connect({
            ssid: wifiSsid,
            password: wifiPassword,
          });
          net.on('connect', function () {
            console.log('network connected');
            netConfig = net.getInfo();
            retJson = '{"cmd":"WiFiCon", "ret":{"state":"STAT_GOT_IP", "ifconfig":["' + netConfig.netInfo.ip_addr + '","' + netConfig.netInfo.gw + '"]}}';
            ble_pripheral_instant.update_char({
              uuid: 0xFFA3,
              value: retJson
            });
          });
          net.on('disconnect', function () {
            console.log('network disconnected');
          });
        } else if (appCmdJson.includes('\"cmd\":\"PullCode\"')) {
          retJson = '{"cmd":"PullCode", "ret":{"state":"START_DOWNLOAD"}}';
          ble_pripheral_instant.update_char({
            uuid: 0xFFA3,
            value: retJson
          });
          jsonObj = JSON.parse(appCmdJson);
          for (var i = 0; i < jsonObj.param.filelist.length; i++) {
            console.log('type: ' + jsonObj.param.filelist[i].type);
            console.log('path: ' + jsonObj.param.filelist[i].path);
            if (jsonObj.param.filelist[i].type === '.json') {
              appConfigURL = jsonObj.param.filelist[i].url;
              appConfigFile = jsonObj.param.filelist[i].name;
              console.log('url: ' + appConfigURL);
              console.log('name: ' + appConfigFile);
            } else {
              appScriptURL = jsonObj.param.filelist[i].url;
              appScriptFile = jsonObj.param.filelist[i].name;
              console.log('url: ' + appScriptURL);
              console.log('name: ' + appScriptFile);
            }
          }
        } else {
          console.log('unrecognized json: ' + appCmdJson);
        }
      } else if (appCmdJson.startsWith('{\"cmd\":\"PullCodeCheck\"')) {
        console.log('recv PullCodeCheck cmd');
        retJson = '{\"cmd\":\"PullCode\", \"ret\":{\"state\":[\"SUCCESS\", \"SUCCESS\"]}}';
        ble_pripheral_instant.update_char({
          uuid: 0xFFA3,
          value: retJson
        });
        ble_pripheral_instant.close({
          success: function() {
            console.log('ble_pripheral close success');
          },
          fail: function() {
            console.log('ble_pripheral close failed');
          }
        });
        http_download_appjs(appScriptURL);
        http_download_deviceinfo(appConfigURL);
      }
    }
  });
}

function http_download_appjs(url) {
  var filePath = '/data/jsamp/app.js';
  http.download({
    url:url,
    filepath:filePath,
    success: function (data) {
      console.log('download ' + filePath + ' successfully!!');
      system_reboot();
    }
  });
}

function http_download_deviceinfo(url) {
  var filePath = '/DeviceInfo.json';
  http.download({
    url:url,
    filepath:filePath,
    success: function (data) {
      console.log('download ' + filePath + ' successfully!!');
      system_reboot();
    }
  });
}

var downloadCount = 0;
function system_reboot() {
  downloadCount = downloadCount + 1;
  if (downloadCount > 1) {
    kv.setStorageSync(key, "SET");
    console.log('SET KV oneMinuteCloudKey and Reboot');
    sleepMs(2000);
    system.reboot();
  }
}
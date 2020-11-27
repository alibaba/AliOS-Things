const Core = require('/lib/@alicloud/pop-core/index.js');

export class IotApi {
  constructor({ accessKey, device }) {
    this.accessKey = accessKey
    this.device = device
    this.client = new Core({
      accessKeyId: accessKey.accessKeyId,
      accessKeySecret: accessKey.accessKeySecret,
      endpoint: 'https://iot.cn-shanghai.aliyuncs.com',
      apiVersion: '2018-01-20'
    });

    this.params = {
      "RegionId": "cn-hangzhou",
      ProductKey: device.ProductKey,
      DeviceName: device.DeviceName
    }

    this.requestOption = {
      method: 'POST'
    };
  }

  getDeviceProperty({ Identifier }) {
    let params = this.params
    my.showLoading();
    return new Promise((resolve, reject) => {
      this.client.request('QueryDevicePropertyStatus', params, this.requestOption).then((result) => {
        if (result.status === 200 && result.data.Success == true) {
          let propList = result.data.Data.List.PropertyStatusInfo
          for (let prop of propList) {
            if (prop.Identifier == Identifier) {
              my.hideLoading();
              resolve(prop.Value);
              break;
            }
          }
        }
        else {  my.hideLoading(); reject(result)  }
      })
    })
  }

  setDeviceProperty({ Identifier, Value }) {
    my.showLoading();
    let params = this.params
    let item = '{\'' + Identifier + '\':' + Value + '}'
    console.log(item)
    Object.assign(params, { "Items": item })
    console.log(params)
    return new Promise((resolve, reject) => {
      this.client.request('SetDeviceProperty', params, this.requestOption).then((result) => {
        console.log(result)
        if (result.status === 200 && result.data.Success == true) { my.hideLoading();resolve(); }
        else { my.hideLoading();reject(result) }
      })
    })
  }

}
export default {
  getDeviceNameList(devices) {
    let device_name_list = []
    for(let index = 0; index < devices.length; index++){
        device_name_list.push(devices[index].localName)
    }
    console.log(device_name_list)
    return device_name_list
  }
}
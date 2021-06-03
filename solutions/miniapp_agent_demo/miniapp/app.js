App({
  globalData: {
    sysname: '',
  },
  onLaunch(options) {
    let sysInfo = my.getSystemInfoSync()
    let sysname = sysInfo.platform.toLowerCase()
    if (sysname.includes('ios') || sysname.includes('iphone')) {
      this.globalData.sysname = 'ios'
    } else {
      this.globalData.sysname = 'android'
    }
    console.log(this.globalData.sysname)
  },
  onShow(options) {
    // 从后台被 scheme 重新打开
    // options.query == {number:1}
  },
});

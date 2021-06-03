
App({
  onLaunch: function() {
    // 第一次打开
    console.log('app ---------------------------------------------- onLaunch');
  },
  onError: function() {
    // 出现错误
    console.log('app onError');
  },
  onExit: function() {
    // 退出轻应用
    console.log('app onExit');
  }
});
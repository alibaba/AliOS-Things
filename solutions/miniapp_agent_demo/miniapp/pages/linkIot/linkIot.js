Page({
  data: {},
  onLoad() { },
  scan() {
    my.scan({
      scanType: ['qrCode'],
      success: (res) => {
        console.log(res.code)
        if (/.*\.vapp.cloudhost.link/.test(res.code)) {
          my.alert({ contant: "请扫描正确的预览二维码" });
          return;
        }
        my.navigateTo({ url: '/pages/webview/webview?address=' + encodeURIComponent(res.code) });
      },
    });
  }
});

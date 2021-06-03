Page({
  data: {
    link: ''
  },
  onLoad(query) {
    console.warn(decodeURIComponent(query.address))
    this.setData({ link: decodeURIComponent(query.address) })
  },
});

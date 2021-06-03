
编译
linux下，执行以下编译命令：
make test os=linux case=render

运行
在linux系统下，按照以下步骤可以运行该用例：
1. 将编译生成的可执行文件（amp/build/bin/render）拷贝到该目录下；
2. 在命令行下，执行./render，即可观察到运行效果；

用例配置修改
默认只显示第一个页面，修改app.json下的pages的页面顺序，可以控制测试显示的页面
  "pages": [
	"pages/device_bind/device_bind",      /*当前显示的页面*/
	"pages/shop/shop",
	"pages/menu/menu",
	"pages/item_auto_add/item_auto_add",
	"pages/logo/logo",
	"pages/pay_start/pay_start",
	"pages/paying/paying",
	"pages/pay_success/pay_success"
  ],

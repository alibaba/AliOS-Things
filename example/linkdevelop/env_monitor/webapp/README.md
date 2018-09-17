## 环境监控 webapp

本webapp配合`example/linkdevelop/env_monitor`使用，实现`设备端`<==>`LD平台`<==>`webapp`3个通道。

### 目录结构

```
webapp
├── app
│   └── pages
│       ├── home
│       │   ├── index.css
│       │   └── index.js        # 业务相关代码
│       └── not-found
│           ├── index.css
│           └── index.js
├── app.js
├── bone-config.json
├── package.json
├── public
│   └── index.html
└── README.md
```

### 服务编排
本webapp需要调用`服务编排`创建的API实现相关逻辑，具体请参考LD平台的[文档中心](https://linkdevelop.aliyun.com).

### 更改api域名地址
更改`webapp/app/pages/home/index.js`中的`http://api.xiaxiaowen.top`为自己绑定的域名：
```js
const config = {
    domain: 'http://api.xiaxiaowen.top'
}
```

### 编译打包

**安装依赖**
```sh
bnpm install
```

**调试**
```sh
bone start
```

**打包**
```sh
bone build
bone pack
```

### libjs
把native的JS接口封装成更人性化更贴近前端开发的JSAPI

### 使用

**安装依赖**
```js
npm install
```

**生成libjs c源码**
> 进行了babel转译（ES6 => ES5）和minify操作。生成的文件为`../engine/duktape_engine/addons/libjs.c`。

```js
npm run build
```

**监测文件改动**
```js
npm run watch
```

### 参考资料
* babel转换工具（web）：https://babeljs.io/repl
* js minify 工具（web)：https://jsminify.org/

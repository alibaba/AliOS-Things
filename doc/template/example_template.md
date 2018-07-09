## example README.md模板

EN | [中文](./example_template-zh.md)

example README.md的模板可以参考[linkkitapp](../../example/linkkitapp/README.md).
### 规范

#### 语言规范

文档默认使用英文编写,当然也可以中英文都支持，中文的文档名称为`README-zh.md`，跟英文文档`README.md`同一个目录，文档中按照如下方式建立中英文索引：

[EN](README.md) | [中文](README-zh.md)

#### 图片规范

文档中涉及到的图片为了防止仓库过大，**请尽量使用外部链接**，当然，如果涉及到的图片需要保存到AliOS Things仓库里的，请统一保存到`AliOS-Things/doc/assets/`里面，通过相对路径引入图片，比如`example/linkkitapp/README.md`文件中可以这样引入图片：

`![](../../doc/assets/example_linkkitapp_1.jpg)`

图片类型不做要求，图片名称请按照如下格式命名：`example_<example_name>_<idx>`，比如：

* `example_helloworld_1.jpg`
* `example_linkkitapp_10.gif`

#### 文档内容规范

example的README文档按照以下几种部分依次编写：

* 目录
* 介绍(Overview)
* 必备条件(Requirements)
* 编译(Build)
* 运行效果(Run)

### 介绍

**介绍**不做过多要求，建议按照如下几个部分介绍：
* 功能
* 依赖的组件
* 补充说明

### 必备条件

**需要条件**主要说明一下运行该example需要具备哪些条件,比如是否需要wifi联网,需要智能生活开发平台的账号和三元组等.

### 编译
**编译**主要说明编译指令,以及编译前需要进行的代码修改.
* 编译
* 运行

### 运行效果
按照实际的效果来展示,最好能够有图片展示,或者动图展示.
## framework README.md模板

EN | [中文](./framework_template-zh.md)

framework README.md的模板可以参考[linkkitapp](../../example/linkkitapp/README.md).
### 规范

#### 语言规范

文档默认使用英文编写,当然也可以中英文都支持，中文的文档名称为`README-zh.md`，跟英文文档`README.md`同一个目录，文档中按照如下方式建立中英文索引：

[EN](README.md) | [中文](README-zh.md)

#### 图片规范

文档中涉及到的图片为了防止仓库过大，**请尽量使用外部链接**，当然，如果涉及到的图片需要保存到AliOS Things仓库里的，请统一保存到`AliOS-Things/doc/assets/`里面，通过相对路径引入图片，比如`framework/atparser/README.md`文件中可以这样引入图片：

`![](../../doc/assets/framework_atparser_1.jpg)`

图片类型不做要求，图片名称请按照如下格式命名：`framework_<framework_name>_<idx>`，比如：

* `framework_atparser_1.jpg`
* `framework_fota_10.gif`

#### 文档内容规范

framework的README文档按照以下几种部分依次编写：

* 目录
* 介绍(Overview)
* 需要条件(Requirements)
* API说明
* 如何使用(How to use)
* 运行效果(Run)

### 介绍

**介绍**不做过多要求，建议按照如下几个部分介绍：

* 功能
* 依赖的组件(Dependent components)
* 补充说明

### 需要条件

**需要条件**主要说明一下运行该组件需要具备哪些条件,比如是否需要串口,蓝牙，是否需要联网等.

### API说明

**API说明**主要说明一下API介绍，如果涉及到的API过多的话，可以引用外部链接。

### 如何使用

**如何使用**主要说明一下使用该组件需要在哪些地方加入代码，支持哪些CLI命令等。

### 运行效果

按照实际的效果来展示,最好能够有图片展示,或者动图展示
@page create_project 创建工程

[更正文档](https://gitee.com/alios-things/documentation/edit/rel_3.3.0/aos-studio/create_project.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

创建工程会自动拉取AliOS-Things的核心组件代码，并配置好相关编译选项。

# 开始创建
点击左下角的"+"图标开始创建工程

<img src="https://img.alicdn.com/imgextra/i2/O1CN01xmJW0b25z1BVejS40_!!6000000007596-2-tps-796-275.png" style="max-width:800px;" />

# 选择解决方案和开发板
随后VS Code会依次提示“请选择解决方案”和“请选择开发板”:

* 选择解决方案，即选择什么功能的示例程序，如helloworld demo, hal demo等
* 选择开发板，即选择在哪块开发板上运行这套示例程序，如HaaS100等

用鼠标选择意向的解决方案（本文以 helloworld demo为例）

<img src="https://img.alicdn.com/imgextra/i1/O1CN01UyKaz91bYyBgPFSzL_!!6000000003478-2-tps-1060-576.png" style="max-width:800px;" />

选择解决方案，之后用鼠标选择HaaS100开发板

<img src="https://img.alicdn.com/imgextra/i2/O1CN01eq0SHQ1oskIBE9WuD_!!6000000005281-2-tps-1060-254.png" style="max-width:800px;" />

# 输入项目名称和路径

<img src="https://img.alicdn.com/imgextra/i3/O1CN01unfkTF1bFjYKNXYMp_!!6000000003436-2-tps-1064-186.png" style="max-width:800px;" />

工作区路径即为源码路径(注：此目录名称必须为连续的英文字符)

<img src="https://img.alicdn.com/imgextra/i1/O1CN01bcjwNW1QGECspTNCo_!!6000000001948-2-tps-1072-200.png" style="max-width:800px;" />

过上述几步，VS Code会自动下载AliOS-Things代码，稍等几分钟即可看到整份源码：

<img src="https://img.alicdn.com/imgextra/i1/O1CN01w1MUey1dTaleqlgDB_!!6000000003737-2-tps-686-420.png" style="max-width:800px;" />
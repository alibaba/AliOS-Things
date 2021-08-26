@page haas_create_project 创建工程

[更正文档](https://gitee.com/alios-things/documentation/edit/rel_3.3.0/haas-studio/create_project.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://help.aliyun.com/document_detail/302301.html)

# 创建工程

创建工程会自动拉取AliOS-Things的核心组件代码，并配置好相关编译选项。

## 开始创建

点击左侧H形图标，选择快速开始选项，打开HaaS快速开始页面。

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i4/O1CN01EqwPwV1Q1ZYFhQR9c_!!6000000001916-2-tps-2804-1750.png" style="max-width:800px;" />
</div>


下面以C/C++开发为例,介绍AliOS-Things Native应用开发环境。

## 选择开发语言

选择C/C++开发选项，点击创建项目按钮，打开创建工程向导。

## 输入项目信息

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i2/O1CN01f0Z3rN1qIgMKYVmau_!!6000000005473-2-tps-1164-1002.png" style="max-width:800px;" />
</div>


* 项目名字，给这个项目起个名字
* 工作区路径，工作区路径即为源码路径(注：此目录名称必须为连续的英文字符)
* 选择硬件类型，即选择在哪块开发板上运行这套示例程序，如HaaS100等
* 选择解决方案，即选择什么功能的示例程序，如helloworld demo, hal demo等

点击“立即创建按钮”。

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i1/O1CN01Lf7GHh27Z1y0w9u36_!!6000000007810-2-tps-1156-916.png" style="max-width:800px;" />
</div>


点击“确认”按钮。

VS Code会自动下载AliOS-Things代码，稍等几分钟即可看到整份源码：

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i1/O1CN01w1MUey1dTaleqlgDB_!!6000000003737-2-tps-686-420.png" style="max-width:800px;" />
</div>


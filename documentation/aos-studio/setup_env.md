@page setup_env 搭建开发环境

[更正文档](https://gitee.com/alios-things/documentation/edit/rel_3.3.0/aos-studio/setup_env.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 安装工具
* Visual Studio Code

Visual Studio Code(下称VS Code) 是一款广泛使用的IDE工具。请根据环境下载对应版本并安装。

注意点: Windows 系统下请以管理员身份 运行VS Code，避免因为python权限问题导致创建失败

# 配置VS Code
“代码修改”、“编译”以及“烧入”都是在VS Code中完成的。在正式开始开发工作前，需要对VS Code进行简单地配置：

* aos-tools

aos-tools主要于编译AliOS-Things代码和镜像烧录。alios-studio安装成功后，VS Code会自动在窗口的右下角提示安装aos-tools, 请点击“是”确认安装：

<img src="https://img.alicdn.com/imgextra/i3/O1CN0123zx8A1oeY0wZdtTv_!!6000000005250-2-tps-784-321.png" style="max-width:800px;" />

等待几分钟后，aos-tools安装完毕。窗口右下角会显示“成功安装aos-tools”字样。已安装(INSTALLED)插件列表中会显示"alios-studio"和"C/C++"两个插件：

<img src="https://img.alicdn.com/imgextra/i2/O1CN01uSbuTA1Utqw2mAEEZ_!!6000000002576-2-tps-1348-802.png" style="max-width:800px;" />
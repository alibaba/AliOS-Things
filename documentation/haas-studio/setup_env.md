@page haas_setup_env 搭建开发环境

[更正文档](https://gitee.com/alios-things/documentation/edit/rel_3.3.0/haas-studio/setup_env.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://help.aliyun.com/document_detail/302301.html)

# 安装工具

* Visual Studio Code

Visual Studio Code(下称VS Code) 是一款广泛使用的IDE工具。请根据环境下载对应版本并安装。

注意点: Windows 系统下请以管理员身份 运行VS Code，避免因为python权限问题导致创建失败

* haas-studio

haas-studio是AliOS Things配套的开发环境基于VS Code的插件。VS Code安装完成后，可在VS Code的插件市场搜索haas-studio，然后点击“install”安装。

# 配置VS Code

“代码修改”、“编译”以及“烧入”都是在VS Code中完成的。在正式开始开发工作前，需要对VS Code进行简单地配置：

* aos-tools

aos-tools主要于编译AliOS-Things代码和镜像烧录。HaaS-studio安装成功后，VS Code会自动在窗口的右下角提示安装aos-tools, 请点击“是”确认安装：

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i3/O1CN0123zx8A1oeY0wZdtTv_!!6000000005250-2-tps-784-321.png" style="max-width:800px;" />
</div>


等待几分钟后，aos-tools安装完毕。窗口右下角会显示“成功安装aos-tools”字样。已安装(INSTALLED)插件列表中会显示"HaaS-studio"和"C/C++"两个插件：

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i1/O1CN01tDiYVM1dMiqwXlLoK_!!6000000003722-0-tps-1898-1030.jpg" style="max-width:800px;" />
</div>

安装插件后请重启下vscode让插件生效。
@page haas_view_log 查看日志

[更正文档](https://gitee.com/alios-things/documentation/edit/rel_3.3.0/haas-studio/view_log.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://help.aliyun.com/document_detail/302301.html)

* 点击VS Code的串口按钮（插头图标）, 配置串口端口号，并配置波特率至1500000，点击HaaS开发板Reset按键后，就可以在串口工具中看到日志打印了。

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i1/O1CN01WFJrEO2AI25lZacbI_!!6000000008179-2-tps-1066-374.png" style="max-width:800px;" />
</div>


* 串口异常情况处理

如果遇到打开串口失败，或者想要修改串口号和波特率时可以先删除.vscode/setting.json里关于串口的信息后再重新配置。

串口配置信息位置参考下图：

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i1/O1CN012PQ3mf1IDvJXTCGyQ_!!6000000000860-0-tps-1536-558.jpg" style="max-width:800px;" />
</div>


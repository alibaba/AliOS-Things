@page haas200_quick_start HaaS200快速开始

**[更正文档](https://gitee.com/alios-things/documentation/edit/rel_3.3.0/quickstart/haas200_quick_start.md)** &emsp;&emsp;&emsp;&emsp; **[贡献说明](https://help.aliyun.com/document_detail/302301.html)**


本文向开发者展示在HaaS200开发板上快速上手AliOS-Things 3.3.
具体操作分为三步：

- 搭建开发环境
- 创建工程
- 开发及调试

# 1. 搭建开发环境
- 详细步骤请参考**[搭建开发环境](https://help.aliyun.com/document_detail/302378.html)**

# 2. 创建工程
- 详细步骤请参考**[创建工程](https://help.aliyun.com/document_detail/302379.html)**，注意硬件类型选择HaaS200。

# 3. 开发及调试
## 3.1 编译工程
- 详细步骤请参考**[编译](https://help.aliyun.com/document_detail/302384.html)**

## 3.2 烧录镜像
- 如下图，先用数据线把HaaS开发板和电脑连起来。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01k0CS0o1HnMKMF5mOi_!!6000000000802-0-tps-4000-3000.jpg" style="max-width:800px;" />
</div>

- 启动烧录
   - 点击VS Code下方状态栏中的烧录图标，随后VS Code会显示电脑已识别到的串口的列表(Avaliable ports)
   - 根据HaaS200开发板的端口号，输入相对应的串口序号，注意根据不同的电脑环境，显示的串口号不同，注意选择。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01yczAeo27T4pOpysKd_!!6000000007797-2-tps-2018-348.png" style="max-width:800px;" />
</div>


- 根据提示可能需要按一下reset按钮重启板子，稍等两分钟就可以看到VS Code提示“成功烧录”
<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01FXvWEY1VYcDZuCBsi_!!6000000002665-2-tps-2336-578.png" style="max-width:800px;" />
</div>


## 3.3 查看日志

- 点击VS Code的Serial Monitor按钮, 选择对应串口号。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01d2WsXx1uS4xaL7rcB_!!6000000006035-2-tps-2880-1800.png" style="max-width:800px;" />
</div>

- 设置波特率为115200

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01GBxu7h1nkDKdxrlKH_!!6000000005127-2-tps-1210-354.png" style="max-width:800px;" />
</div>

- 串口日志输出

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01yDKygQ1F76ZoLRfkM_!!6000000000439-2-tps-894-654.png" style="max-width:800px;" />
</div>

# 4. 打开已有工程

第2章节中创建的工程关闭后，可以通过VS Code再次打开已有工程。

- 详细步骤请参考**[打开工程](https://help.aliyun.com/document_detail/302381.html)**


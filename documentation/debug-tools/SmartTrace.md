@page smarttrace 分析工具 SmartTrace

**[更正文档](https://gitee.com/alios-things/documentation/edit/rel_3.3.0/debug-tools/smarttrace.md)** &emsp;&emsp;&emsp;&emsp; **[贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)**

# 使用目的
- 接收日志和交互shell，取代串口工具
- 自动解析栈回溯backtrace，替代手动addr2line
- GDB常用调试功能，替代硬件仿真器
   - 查看函数反汇编
   - 查看地址反汇编
   - 查看变量值等

# 依赖组件
* debug
* cli

# 操作步骤

1. 打开SmartTrace（windows版本和mac版本分开,这里以windows版本为例）

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i1/O1CN01DtrUcH24Hg6euUzst_!!6000000007366-2-tps-803-743.png" style="max-width:800px;" />
</div>

## 设置

2. 点击左上角 “设置”，加入elf 文件路径（当前只需加入kernel elf文件即可）和过滤规则文件

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i3/O1CN01gAGgUk1jH7h3UUoWo_!!6000000004522-2-tps-1915-869.png" style="max-width:800px;" />
</div>


其中，日志过滤文件和uTrace.exe在同一个路径下

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i3/O1CN011my9nB1TrjV7Sna8K_!!6000000002436-2-tps-889-766.png" style="max-width:800px;" />
</div>

## 日志
点击“日志”标签，设置串口号，波特率，打开串口

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i2/O1CN01S5IxsH1j2vPkf1NNd_!!6000000004491-2-tps-1908-1007.png" style="max-width:800px;" />
</div>


可以看到设备日志，跟串口工具一样

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i2/O1CN01zMZ78m1uBagRLYMbB_!!6000000005999-2-tps-1920-1041.png" style="max-width:800px;" />
</div>


## 终端
点击左上角“终端”，有个简易的shell，操作方式跟串口工具一样，**这里是把AliOS Things cli 命令的输出，从日志里拿出来，输出到了这个“终端”标签下，**方便进行一些命令的调试。

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i3/O1CN01gSpkvT1iCiGUOrYkl_!!6000000004377-2-tps-1920-1041.png" style="max-width:800px;" />
</div>

# 查看栈回溯backtrace


## 直接解析日志中的backtrace
如果使用SmartTrace工具接上了串口，则在系统异常crash的时候会直接解析调用栈（右侧的解析窗口默认情况下是隐藏的，将光标移到右侧，光标图案变成左右箭头的时候，点击并往左拖动，可以拖出来“过滤日志窗口”。注意，在右边还有一个“Sequence Diagram”窗口，可查看时序图，这里暂时可以不管）

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i1/O1CN01HNx9NR1Sru1yNyhe3_!!6000000002301-2-tps-1890-1004.png" style="max-width:800px;" />
</div>

## 从其他地方复制backtrace并解析
如果日志来源于其他地方，如调试串口工具secureCRT，如果此时想查看栈回溯，复制完整的栈回溯信息
（“Kernel Call stack”栈信息是必要的）

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i2/O1CN01W15ku71mi5racVdbJ_!!6000000004987-2-tps-982-914.png" style="max-width:800px;" />
</div>

使用“本地日志”  --- “clipboard”  --- “**分析日志**”， 也可以解析调用栈

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i4/O1CN0186eqDH1zhG99TXySp_!!6000000006745-2-tps-1908-1003.png" style="max-width:800px;" />
</div>

# GDB调试功能
## 查看反汇编 $disass 命令
### `$disass function`：查看指定函数的反汇编


如：查看cli_main接口的反汇编
$disass cli_main
工具会输出cli_main 函数的反汇编

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i2/O1CN01VFFS9626FVSSVnavT_!!6000000007632-2-tps-1920-1041.png" style="max-width:800px;" />
</div>

### `$disass memory_address`：查看指定地址的反汇编


如：查看crash时PC的反汇编，从日志看得知PC=0x10057D40
$disass 0x10057D40
工具会输出

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i1/O1CN01mOjXgc1lIcDHRtGY3_!!6000000004796-2-tps-573-201.png" style="max-width:800px;" />
</div>


## 查看变量
查看 g_ready_queue结构体数据
$p  g_ready_queue
会输出：

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i1/O1CN018P0Z6S297D8GD2iV1_!!6000000008020-2-tps-719-693.png" style="max-width:800px;" />
</div>
​

<div align=left display=flex>
   <img src="https://img.alicdn.com/imgextra/i3/O1CN01iUfd0b21FtiPRM4sB_!!6000000006956-2-tps-677-453.png" style="max-width:800px;" />
</div>
​

## 当前支持的GDB命令汇总

- `$p variable`：打印变量variable的值
- `$p *(pointer_type *)memory_address`：按指定的数据类型读取内存数据并打印
- `$p &variable`：打印变量variable的地址
- `$x /nwx memory_address`：查看指定地址的内存数据，其中n表示数字，用1~9代替
- `$set variable=value`：设置某个变量的值为value
- `$set *(pointer_type *)memory_address`=value`：设置某个内存地址的值为value
- `$disass function`：查看指定函数的反汇编代码
- `$disass memory_address`：查看指定地址的反汇编代码

​

注：更多高级功能在持续演进。
​

# 附件
当前仅支持 windows 和 mac 环境，均在zip包里，无需安装，选择相应版本下载即可使用
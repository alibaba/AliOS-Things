## 介绍

该组件提供常用的文件和目录操作命令，如cd、、pwd、ls、mkdir、mv等。

## 特性

包括以下命令：

- cat：显示文件内容。
- cd：改变当前工作目录。
- cp：拷贝文件或目录。
- df：显示文件系统分区空间使用率信息。
- echo：往标准输出或文件写入字符串。
- hexdump：以ASCII、十进制、十六进制、八进制等方式显示文件内容。
- ls：列举某目录下的文件/目录信息。
- lsof：显示系统中已打开的文件描述符。
- mkdir：创建目录。
- mv：移动文件或目录。
- pwd：显示当前工作路径。
- rm：删除文件或目录。
- rwcheck：文件系统检查工作。
- rwspeed：文件系统读写性能测试工具。
- touch：创建文件。

## 目录

```shell
iobox/
├── aos.mk
├── cat.c
├── cd.c
├── Config.in
├── cp.c
├── df.c
├── echo.c
├── hexdump.c
├── ls.c
├── lsof.c
├── mkdir.c
├── mv.c
├── objects.mk
├── pwd.c
├── rm.c
├── rwcheck.c
├── rwspeed.c
├── touch.c

0 directories, 18 files
```

## 依赖

N/A

## 使用

1. 在board目录的Config.in中选中iobox组件：

   ```diff
   diff --git a/mcu/Config.in b/mcu/Config.in
   index c0a032b..8a80741 100644
   --- a/mcu/Config.in
   +++ b/mcu/Config.in
   @@ -17,6 +17,7 @@ config AOS_MCU_VEXPRESSA9
        select AOS_COMP_U_RAM_TEST if AOS_KERNEL_MODE
        select AOS_COMP_IRQ
        select RHINO_CONFIG_UCALLBACK
   +    select AOS_COMP_IOBOX

        help
          driver & sdk for platform/mcu vexpress-a9
   ```

2. 编译并运行固件，然后在命令行中进行iobox相关命令操作：

```shell
# ls /bin
# cd bin
# pwd
# cp /bin/README /mnt/sdcard/
# cat /bin/README
# df /mnt/sdcard
# echo "hello"
# touch /mnt/sdcard/test.txt
# echo "helloworld" > /mnt/sdcard/test.txt
# hexdump -C -s 4096 -n 256 /mnt/sdcard/1MB.bin
# ...
```

## 参考

NA

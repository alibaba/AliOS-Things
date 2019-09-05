1 Configuration选项中的link菜单需要修改linked libararies,即选取本地位置（路径）.\lib目录下的drivr，os，fs的库文件，如
  F:\O18B_GCCS_SCHEME\O18B_SDK_GCC_1028\O18B_SDK_GCC\lib\libdriver.a；Configuration选项中的compile菜单需要修改为本地文件所在位置，可修改最后一行的路径为本地路径；
2 .\MVs_new所处的目录为源代码以及CooCox工程所在位置；
3 使用.\MVs_new\\MVs_new\MVs_new.coproj打开CooCox工程，工程中使用的源代码文件在.\MVs_new\MVs18_SDK\src目录下，链接的库文件".a"位于.\lib目录下；
4 安装CoIDE和CoFlash-1.4.9；
5 编译成功后使用MVFLASH.elf进行程序的下载后可进行调试，同时Configuration选项中的download菜单下需要add保存本地MVFLASH.elf文件所处的路径
6 示例应用为简单的OS任务和串口打印输出。
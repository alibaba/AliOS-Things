# AliOS Things CMSIS PACK Makeing Guide

本文将讲述制作AliOS Things CMSIS PACK的要点，该PACK可以加载至CubeMX作为插件使用。

## 1、将可能需要PACK的模块加入至引用列表

因为目前AliOS Things内容较大，如果选择全部打包效率会比较慢。因此目前可以将所需要的组件列至同目录下的"packlist.txt"中。参照其中内容将需要的新模块列至其中。之后执行"PreparePACK.bat",即建立所有列在"packlist.txt"中的链接至"Files/aos/."目录下。

## 2、制作及打包

制作规则文件需要修改Alibaba.AliOSThings.pdsc, 打包执行gen_pack.bat。更多帮助请参考CMSIS PACK(https://www.keil.com/pack/doc/CMSIS/Pack/html/index.html)

## 3、删除链接

在进行新的PACK制作后，如果需要Commit至仓库中，对于新建Symbol Link及其内容并不希望提交，这时候可以执行"CleanSymbolLinks.bat"将其删除。

## 4、PatchFiles使用

目前CubeMX还存在一些不能完美配合外部插件，及CubeMX自身的Bug，可以将"PatchFiles"中的文件覆盖后使用，覆盖的目录一般为CubeMX的安装路径，如
"C:\Program Files (x86)\STMicroelectronics\STM32Cube\STM32CubeMX\db\templates"

## 5、addtional work around to complete project build(take helloworld for example)
### Common Setting: remove define USE_HAL_DRIVER
### IAR Setting: Open options of project --C/C++ Compiler --Extra Options, check "Use command line options" and fill below text
-f $PROJ_DIR$/../Middlewares/Third_Party/AliOSThings/opts/IAR/helloworld@starterkit.c_opts 
### KEIL Setting:
Options of project ---C/C++ --- Misc Controls, fill below text
--via ../Middlewares/Third_Party/AliOSThings/opts/KEIL/helloworld@starterkit.c_opts



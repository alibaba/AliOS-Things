1.为避免修改阿里公共代码，目前将HAL_PARTITION_SYS_DATA重定义为HAL_PARTITION_MAX，相关修改在board\uno-81a\board.c,board\uno-81c\board.c以及platform\mcu\rda5981x\feature\sysdata\rda5981_sys_data.c中
实际使用时可注释掉相关代码并将HAL_PARTITION_SYS_DATA添加至include\hal\soc\flash.h中的hal_partition_t。
2.如需升级Alios，只需将board\uno-81a，board\uno-81c以及platform\mcu\rda5981x拷贝至新版本相应路径即可
#include "dev_conf.h"
#include "lite-log.h"

int main()
{
    char *fw_version;
    IOT_OpenLog("config\n");
    IOT_SetLogLevel(IOT_LOG_DEBUG);

    int ret;

    ret = config_init();
    if (ret) {
        log_err("config init failed!");
        return -1;
    }


    fw_version = config_get_fw_version();
    log_info("get fw_version:%s", fw_version);
    config_exit();

    IOT_CloseLog();

    return 0;
}




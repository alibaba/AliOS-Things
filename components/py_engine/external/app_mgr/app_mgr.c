/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include "aos/vfs.h"
#include "amp_platform.h"
#include "aos_system.h"
#include "aos_fs.h"
#include "aos_tcp.h"
#include "amp_task.h"
#include "ota_agent.h"
#include "mbedtls/md5.h"
#include "py_defines.h"
#include "httpclient.h"
#include "app_upgrade.h"

#define MOD_STR "APP_MGR"

static char app_version[32] = {0};

void apppack_init()
{
    /* remove all js file */
    aos_rmdir_r(AMP_FS_ROOT_DIR);
    aos_mkdir(AMP_FS_ROOT_DIR);
}

static int task_http_download_func(char * url,char * filepath)
{
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    int num=0;
    int ret;
    char rsp_buf[2048];


    char * req_url = url;
    int fd = aos_open(filepath,  O_CREAT | O_RDWR );

    memset(rsp_buf, 0, sizeof(rsp_buf));
    client_data.response_buf = rsp_buf;
    client_data.response_buf_len = sizeof(rsp_buf);

    ret = httpclient_conn(&client, req_url);

    if (!ret) {
        ret = httpclient_send(&client, req_url, HTTP_GET, &client_data);

        do{
            ret = httpclient_recv(&client, &client_data);
            //printf("response_content_len=%d, retrieve_len=%d,content_block_len=%d\n", client_data.response_content_len,client_data.retrieve_len,client_data.content_block_len);
            //printf("ismore=%d \n", client_data.is_more);

            num = aos_write(fd, client_data.response_buf, client_data.content_block_len);
            if(num > 0){
                //printf("aos_write num=%d\n", num);
            }
        }while(client_data.is_more);
    }
    ret = aos_sync(fd);
    httpclient_clse(&client);
    return ret ;
}

//int apppack_download(char *url, download_js_cb_t func)
int pyapp_download_and_upgrade(char *url)

{
    int ret = -1 ;
    amp_warn(MOD_STR, "url = %s", url);
    ret = task_http_download_func(url, AMP_PY_PKG_DEFAULE);
    if(ret){
        amp_error(MOD_STR,"online debug download failed");
    }else{
        amp_warn(MOD_STR, "begin to install pyapp");
        ret = install_pyapp(AMP_PY_PKG_DEFAULE, AMP_PY_PKG_DIR);
    }
    return ret;
}

static int32_t update_done = 1;
static int app_fp          = -1;



int download_apppack(uint8_t *buf, int32_t buf_len)
{
    amp_warn(MOD_STR, "download buf len = %d", buf_len);
    int fd = aos_open("/data/pyamp/app.zip",  O_CREAT | O_RDWR );
    aos_write(fd, buf, buf_len);
 

    return 0;
}



extern void *task_mutex;
extern int upgrading;
static void download_work(void *arg)
{
    int ret;
    aos_task_t restart_task;

    // amp_warn(MOD_STR, "download_work task name=%s", jse_osal_get_taskname());
    amp_warn(MOD_STR, "url=%s", (char *)arg);

    ret = pyapp_download_and_upgrade((char *)arg);
    amp_debug(MOD_STR, "pyapp_download_and_upgrade done:%d", ret);
    update_done = 1;


    if (ret == 0) {
        amp_warn(MOD_STR, "Upgrade app success");
        aos_msleep(200);
    }

    upgrading = 0;

    aos_reboot();

    aos_task_exit(0);
    return;
}




int apppack_upgrade(char *url)
{
    upgrading = 1;
    aos_task_t upgrade_task;

    amp_warn(MOD_STR, "apppack_upgrade url=%s ", (char *)url);

    if (update_done) {
        update_done = 0;
        apppack_init();

        amp_warn(MOD_STR, "create upgrade task ...");

        if (aos_task_new_ext(&upgrade_task, "amp_upgrade", download_work, (void *)url, 1024 * 6, AOS_DEFAULT_APP_PRI) != 0) {
            update_done = 1;
            amp_warn(MOD_STR, "jse_osal_task_new fail");

            return -1;
        }

    } else {
        aos_free(url);
        amp_warn(MOD_STR, "apppack upgrading...");
    }

    return 0;
}



void amp_app_version_set(char *version)
{
    if (!version)
        return;
    snprintf(app_version, sizeof(app_version), "%s", version);
}

const char *amp_jsapp_version_get(void)
{
    return app_version;
}


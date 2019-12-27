#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "cli.h"
#include "aos/kernel.h"
#include "ulog/ulog.h"

#include <atparser.h>

#include "aos/yloop.h"

#ifndef AT_RECV_FAIL_POSTFIX
#define AT_RECV_FAIL_POSTFIX "ERROR\r\n"
#endif

#define STARTERKIT_WIFI_MODULE_FOTA "AT+FOTA"
#define FOTA_OOB_PREFIX "+FOTAEVENT:"
#define FOTA_OOB_POSTFIX "\r\n"

typedef struct modulefotastatus {
    int ret;
    aos_sem_t stmoduelfotasem;
} fotastatus_t;

static fotastatus_t fota_status;
#define FOTA_OOB_BUF_SIZE 64
extern int at_dev_fd;
static char fota_oob_buf[FOTA_OOB_BUF_SIZE];

static int module_fota_firmware_size_check(char *pcsize)
{
    int i = 0;
    if (NULL == pcsize){
        printf("invalid input at $s %d \r\n", __FILE__, __LINE__);
        return -1;
    }

    /*It's impossible that the size of firmware can over 10e9*/
    if (strlen(pcsize) > 10) {
        printf("invalid lenth %s at %s %d \r\n ", pcsize, __FILE__, __LINE__);
        return -1;
    }

    for (i = 0; i < strlen(pcsize); i++){
        if (pcsize[i] < '0' || pcsize[i] > '9'){
            printf("invalid lenth %s at %s %d \r\n ", pcsize, __FILE__, __LINE__);
            return -1;
        }
    }

    return 0;
}

static int module_fota_firmware_md5_check(char *pcmd5)
{
    int i = 0;
    if (NULL == pcmd5){
        printf("invalid input at $s %d \r\n", __FILE__, __LINE__);
        return -1;
    }

    /*It's impossible that the length of md5 value can over 64*/
    if (strlen(pcmd5) > 64) {
        printf("invalid lenth %s at %s %d \r\n ", pcmd5, __FILE__, __LINE__);
        return -1;
    }

    for (i = 0; i < strlen(pcmd5); i++){
        if (!((pcmd5[i] >= '0' && pcmd5[i] <= '9') 
            || (pcmd5[i] >= 'a' && pcmd5[i] <= 'f')
            || (pcmd5[i] >= 'A' && pcmd5[i] <= 'F'))){
            printf("invalid lenth %s at %s %d \r\n ", pcmd5, __FILE__, __LINE__);
            return -1;
        }
    }

    return 0;
}

static int wifi_module_fota(char *pcsize, char *pcversion, char *pcurl, char *pcmd5)
{
    char *pcatcmd = NULL;
    char out[64] = {0};
    int  cmdlen = 0;
    int  ret = 0;

    if (NULL == pcsize || NULL == pcversion || NULL == pcurl || NULL == pcmd5){
        printf("invalid input at %s %d \r\n", __FILE__, __LINE__);
        return -1;
    }

    pcatcmd = aos_malloc(1024);
    if (NULL == pcatcmd){
        printf("fail to malloc memory at %s %d \r\n", __FILE__, __LINE__);
        return -1;
    }

    memset(pcatcmd, 0, 1024);

    cmdlen = snprintf(pcatcmd, 1024, "%s=%s,%s,%s,%s", STARTERKIT_WIFI_MODULE_FOTA, pcsize,
            pcversion, pcurl, pcmd5);
    if (cmdlen >= 1024){
        printf("invalid cmd at  %s %d \r\n", __FILE__, __LINE__);
        ret = -1;
        goto end;
    }

    ret = at_send_wait_reply(at_dev_fd, pcatcmd, strlen(pcatcmd), true,
                             NULL, 0, out, sizeof(out), NULL);
    LOGD(TAG, "The AT response is: %s", out);
    if (strstr(out, AT_RECV_FAIL_POSTFIX) != NULL || ret != 0) {
        printf("%s %d failed", __func__, __LINE__);
        ret = -1;
        goto end;
    }

    aos_sem_wait(&fota_status.stmoduelfotasem , AOS_WAIT_FOREVER);
    if (fota_status.ret != 0){
        printf("module fota failed at  %s %d \r\n", __FILE__, __LINE__);
        ret = -1;
    }

end:
    aos_free(pcatcmd);
    return ret;
}

#ifdef AOS_COMP_CLI
static void handle_module_fota_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    int ret = 0;
    char *pcsize = NULL;
    char *pcversion = NULL;
    char *pcurl = NULL;
    char *pcmd5 = NULL;
    
    if (argc != 5 || NULL == argv){
        aos_cli_printf("invalid cmd at %s %d \r\n", __FILE__, __LINE__);
        return;
    }

    pcsize = argv[1];
    /*or check if atoi(pcsize) <= 0, is invalid*/
    if (module_fota_firmware_size_check(pcsize) != 0){
        printf("invalid cmd at %s %d \r\n", __FILE__, __LINE__);
        return;
    }

    pcversion = argv[2];
    if (strlen(pcversion) >= 64){
        printf("invalid cmd at %s %d \r\n", __FILE__, __LINE__);
        return;
    }

    pcurl = argv[3];
    if (strlen(pcversion) >= 512){
        printf("invalid cmd at %s %d \r\n", __FILE__, __LINE__);
        return;
    }

    pcmd5 = argv[4];
    if (module_fota_firmware_md5_check(pcmd5) != 0){
        printf("invalid cmd at %s %d \r\n", __FILE__, __LINE__);
        return;
    }

    printf("It's going to start wifi module fota it will cost several minutes. Please be patient, and Do not power off the module or reboot befor ota is finished. \r\n");
    ret = wifi_module_fota(pcsize, pcversion, pcurl, pcmd5);
    if (ret != 0){
        printf("module-ota excute failed \r\n");
    }else{
        printf("module-ota excute successed \r\n");
    }
    
    return;
}

struct cli_command module_ota_cli_cmd[] = {
    {
        .name = "module-ota",
        .help = "module-ota size version url md5-value",
        .function = handle_module_fota_cmd
    }
};
#endif

void fota_event_handler(void *arg, char *buf, int buflen)
{
    int ret = -1;
    
    if (NULL == buf || 0 == buflen){
        printf("invalid input  %s %d \r\n", __FILE__, __LINE__);
        goto end;
    }

    if (strstr(buf, AT_RECV_FAIL_POSTFIX) != NULL){
        printf("module-fota result is %s \r\n", buf);
        goto end;
    }
    ret = 0;
end:
    fota_status.ret = ret;
    aos_sem_signal(&fota_status.stmoduelfotasem);
}

static void wifi_event_handler(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) 
        return;
    
    if (event->code == CODE_WIFI_ON_GOT_IP){
        at_register_callback(at_dev_fd, FOTA_OOB_PREFIX, FOTA_OOB_POSTFIX, fota_oob_buf, FOTA_OOB_BUF_SIZE, fota_event_handler, NULL);
#ifdef AOS_COMP_CLI
        aos_cli_register_commands(&module_ota_cli_cmd[0],sizeof(module_ota_cli_cmd) / sizeof(struct cli_command));
#endif
        LOG("Hello, WiFi GOT_IP event! at %s %d\r\n", __FILE__, __LINE__);
    }
}


int board_cli_init(void)
{
    int ret = 0;
    ret = aos_sem_new(&fota_status.stmoduelfotasem, 0);
    if (ret){
        printf("fail to creat sem4  %s %d \r\n", __FILE__, __LINE__);
        return -1;
    }
    fota_status.ret = 0;

    aos_register_event_filter(EV_WIFI, wifi_event_handler, NULL);
    return 0;
}


/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*
*
*/

#include "gps_hal.h"

#define SIM868_AT_CMD_SUCCESS_RSP           "OK"
#define SIM868_AT_CMD_FAIL_RSP              "ERROR"

#define SIM868_AT_CMD_GPS_POWER_OFF         "AT+CGNSPWR=0"
#define SIM868_AT_CMD_GPS_POWER_ON          "AT+CGNSPWR=1"
#define SIM868_AT_CMD_GPS_POWER_CHECK       "AT+CGNSPWR?"

#define SIM868_AT_CMD_GPS_LASTPARSE_SET     "AT+CGNSSEQ=\"RMC\""
#define SIM868_AT_CMD_GPS_LASTPARSE_CHECK   "AT+CGNSSEQ?"

#define SIM868_AT_CMD_GPS_POSITION_GET      "AT+CGNSINF"

#define SIM868_AT_CMD_GPS_INTERVAL_SET      "AT+CGNSURC=100"
#define SIM868_AT_CMD_GPS_INTERVAL_CLOSE    "AT+CGNSURC=0"
#define SIM868_AT_CMD_GPS_INTERVAL_CHECK    "AT+CGNSURC?"

#define SIM868_AT_CMD_GPS_SEND_MODE_SET     "AT+CGNSTST=0"
#define SIM868_AT_CMD_GPS_SEND_MODE_CHECK   "AT+CGNSTST?"

#define SIM868_GPS_HEAD_LOOP                "\n+UGNSINF:"
#define SIM868_GPS_TAIL_LOOP                "\r\n"

#define SIM868_GPS_HEAD_POLL                "\n+CGNSINF:"
#define SIM868_GPS_TAIL_POLL                "\r\n"

#define SIM868_GPS_DEFAULT_CMD_LEN          (64)
#define SIM868_GPS_DEFAULT_RSP_LEN          (256)


static char    g_gps_sim868_addr[GPS_RCV_DATA_LEN];
static gps_data_t g_gps_sim868_data;

static int gps_simcom_sim868_power_on(work_mode_e mode)
{
    int ret = 0;
    char rsp[SIM868_GPS_DEFAULT_RSP_LEN] = {0};

    if((DEV_DATA_READY != mode ) && (DEV_POLLING != mode)){
        LOG("func:%s line: %d  para: %d error\n",__func__, __LINE__,mode);
        return -1;
    }

    memset(rsp, 0, SIM868_GPS_DEFAULT_RSP_LEN);
    ret = at.send_raw(SIM868_AT_CMD_GPS_POWER_ON, rsp, SIM868_GPS_DEFAULT_RSP_LEN);
    if ((0 != ret) || (strstr(rsp, SIM868_AT_CMD_SUCCESS_RSP) == NULL)) {
        LOG("%s %d failed rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);
        return -1;
    }

    memset(rsp, 0, SIM868_GPS_DEFAULT_RSP_LEN);
    ret = at.send_raw(SIM868_AT_CMD_GPS_LASTPARSE_SET, rsp, SIM868_GPS_DEFAULT_RSP_LEN);
    if ((0 != ret) || (strstr(rsp, SIM868_AT_CMD_SUCCESS_RSP) == NULL)) {
        LOG("%s %d failed rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);
        return -1;
    }
    
    memset(rsp, 0, SIM868_GPS_DEFAULT_RSP_LEN);
    if(DEV_POLLING == mode){
        ret = at.send_raw(SIM868_AT_CMD_GPS_INTERVAL_CLOSE, rsp, SIM868_GPS_DEFAULT_RSP_LEN);
    }
    else{
        ret = at.send_raw(SIM868_AT_CMD_GPS_INTERVAL_SET, rsp, SIM868_GPS_DEFAULT_RSP_LEN);
    }
    
    if ((0 != ret) ||  (strstr(rsp, SIM868_AT_CMD_SUCCESS_RSP) == NULL)) {
        LOG("%s %d failed rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);
        return -1;
    }

    memset(rsp, 0, SIM868_GPS_DEFAULT_RSP_LEN);
    ret = at.send_raw(SIM868_AT_CMD_GPS_SEND_MODE_SET, rsp, SIM868_GPS_DEFAULT_RSP_LEN);
    if ((0 != ret) || (strstr(rsp, SIM868_AT_CMD_SUCCESS_RSP) == NULL)) {
        LOG("%s %d failed rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);
        return -1;
    }  
        
    return 0;
}

static int gps_simcom_sim868_power_off()
{
    int ret = 0;
    char rsp[SIM868_GPS_DEFAULT_RSP_LEN] = {0};
    
    memset(rsp, 0, SIM868_GPS_DEFAULT_RSP_LEN);
    ret = at.send_raw(SIM868_AT_CMD_GPS_POWER_OFF, rsp, SIM868_GPS_DEFAULT_RSP_LEN);
    if ((0 != ret) || (strstr(rsp, SIM868_AT_CMD_SUCCESS_RSP) == NULL)) {
        LOG("%s %d failed rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);
        return -1;
    }
        
    return 0;
}

static int gps_simcom_sim868_check(void)
{
    int ret = 0;
    char rsp[SIM868_GPS_DEFAULT_RSP_LEN] = {0};

    memset(rsp, 0, SIM868_GPS_DEFAULT_RSP_LEN);
    ret = at.send_raw(SIM868_AT_CMD_GPS_POWER_CHECK, rsp, SIM868_GPS_DEFAULT_RSP_LEN);
    if ((0 != ret) || (strstr(rsp, SIM868_AT_CMD_SUCCESS_RSP) == NULL)) {
        LOG("%s %d failed rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);
        return -1;
    }
    LOG("GPS power check %s \r\n", rsp);

    memset(rsp, 0, SIM868_GPS_DEFAULT_RSP_LEN);
    ret = at.send_raw(SIM868_AT_CMD_GPS_LASTPARSE_CHECK, rsp, SIM868_GPS_DEFAULT_RSP_LEN);
    if ((0 != ret) || (strstr(rsp, SIM868_AT_CMD_SUCCESS_RSP) == NULL))  {
        LOG("%s %d failed rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);
        return -1;
    }
    LOG("GPS LAST PARSE %s \r\n", rsp);

    memset(rsp, 0, SIM868_GPS_DEFAULT_RSP_LEN);
    ret = at.send_raw(SIM868_AT_CMD_GPS_POSITION_GET, rsp, SIM868_GPS_DEFAULT_RSP_LEN);
    if ((0 != ret) || (strstr(rsp, SIM868_AT_CMD_SUCCESS_RSP) == NULL))  {
        LOG("%s %d failed rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);
        return -1;
    }
    LOG("GPS POSITION %s \r\n", rsp);

    memset(rsp, 0, SIM868_GPS_DEFAULT_RSP_LEN);
    ret = at.send_raw(SIM868_AT_CMD_GPS_INTERVAL_CHECK, rsp, SIM868_GPS_DEFAULT_RSP_LEN);
    if ((0 != ret) || (strstr(rsp, SIM868_AT_CMD_SUCCESS_RSP) == NULL))  {
        LOG("%s %d failed rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);
        return -1;
    }
    LOG("GPS INVERVAL %s \r\n", rsp);


    memset(rsp, 0, SIM868_GPS_DEFAULT_RSP_LEN);
    ret = at.send_raw(SIM868_AT_CMD_GPS_SEND_MODE_CHECK, rsp, SIM868_GPS_DEFAULT_RSP_LEN);
    if ((0 != ret) || (strstr(rsp, SIM868_AT_CMD_SUCCESS_RSP) == NULL))  {
        LOG("%s %d failed rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);
        return -1;
    }
    LOG("GPS send mode %s \r\n", rsp);
        
    return 0;
}


 int gps_simcom_sim868_config(work_mode_e mode)
{
    int ret;

    ret = gps_simcom_sim868_power_off(mode);
    if(0 != ret)
    {
        return ret;
    }

    ret = gps_simcom_sim868_power_on(mode);
    if(0 != ret)
    {
        return ret;
    }
    
    return gps_simcom_sim868_check();
}


static int gps_simcom_sim868_poll_read(char* rsp)
{
    int ret = 0;
    memset(rsp, 0, SIM868_GPS_DEFAULT_RSP_LEN);
    ret = at.send_raw(SIM868_AT_CMD_GPS_POSITION_GET, rsp, SIM868_GPS_DEFAULT_RSP_LEN);
    if (strstr(rsp, SIM868_AT_CMD_SUCCESS_RSP) == NULL) {
        LOG("%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    
    return (int)strlen(rsp);
}

static void gps_simcom_sim868_dispatcher(input_event_t *event, void *priv_data)
{
    int ret = 0;
    int len = 0;
    char* str;
    
    if ((event == NULL)||(event->type != GPS_EV_UDATA)) {
        return;
    }
    
    switch(event->code){
        case GPS_DEV_READ:{
            str = (char*)(void*)(event->value);
            if(NULL == str){
                return;
            }
            ret = gps_simcom_sim868_proc(str,&g_gps_sim868_data);
            if(0 != ret){
                return;
            }
            
        }break;

        case GPS_DEV_SEND:{

        }break;
        
        default:break;
    }
    
}


static int gps_simcom_sim868_data_proc_init(work_mode_e mode)
{
    int ret = 0;
    
    if(DEV_DATA_READY == mode){
        ret = aos_register_event_filter(GPS_EV_UDATA, gps_simcom_sim868_dispatcher, NULL);
        if(unlikely(ret)){
            return -1;
        }
        LOG(" %s successfully \n", __func__);
    }
    else{
        LOG("func : %s no need\n",__func__);
    }

    return 0;
}

static void gps_simcom_sim868_ood_cb(void *arg, void* buf, int size)
{
    int ret;
    char* str;
    if((NULL == buf) || (0 == size) || (size >= GPS_RCV_DATA_LEN)){
        return;
    }

    str = &g_gps_sim868_addr[0];
    memcpy(str,buf,size);
    str[size] = '\0';
    
    ret = aos_post_event(GPS_EV_UDATA, GPS_DEV_READ, (unsigned long)str);
    if(ret < 0){
        return;
    }
}

static int gps_simcom_sim868_ood_cb_reg(work_mode_e mode)
{

    if(DEV_DATA_READY == mode){
        at.oob(SIM868_GPS_HEAD_LOOP, SIM868_GPS_TAIL_LOOP,GPS_RCV_DATA_LEN-1, gps_simcom_sim868_ood_cb, NULL);
    }
    else{
        LOG("func : %s no need\n",__func__);
    }
    
    return 0;
}

static int gps_simcom_sim868_cb_unreg(void)
{
    /*unreg uart callback, ref to at.oob*/
    return 0;
}

static int gps_simcom_sim868_open(void)
{
    int ret;
    work_mode_e mode;
    
    mode = gps_mode_get();
    ret = gps_simcom_sim868_data_proc_init(mode);
    if(0 != ret){
        return -1;
    }
    
    ret = gps_simcom_sim868_ood_cb_reg(mode);
    if(0 != ret){
        return -1;
    }

    ret = gps_simcom_sim868_config(mode);
    if(0 != ret){
        return -1;
    }
    
    return 0;
}

static int gps_simcom_sim868_close(void)
{
    int ret;

    ret = gps_simcom_sim868_power_off();
    if(0 != ret){
        return -1;
    }

    ret = gps_simcom_sim868_cb_unreg();
    if(0 != ret){
        return -1;
    }

    return 0;
}

static int gps_simcom_sim868_data_get()
{   
    int ret = 0;
    char rsp[SIM868_GPS_DEFAULT_RSP_LEN] ={0};
    
    ret = gps_simcom_sim868_poll_read(rsp);
    if((ret <= 0) || (ret >= SIM868_GPS_DEFAULT_RSP_LEN))
    {
        return -1;
    }
    ret = gps_simcom_sim868_proc(rsp,&g_gps_sim868_data);
    if(0 != ret){
        return -1;
    }
    return 0;
}

static int gps_simcom_sim868_read(void *buf, size_t len)
{   
    int ret;
    size_t size = 0;
    gps_data_t* gps = buf;
    
    if(0 == buf){
        return -1;
    }

    size = sizeof(gps_data_t);
    if(len < size){
        return -1;
    }
    
    if(DEV_POLLING == gps_mode_get()){
        ret = gps_simcom_sim868_data_get();
        if(0 != ret){
            return -1;
        }
    }
    
    memcpy(&(gps->utc),&(g_gps_sim868_data.utc),sizeof(g_gps_sim868_data.utc));

    gps->lat = g_gps_sim868_data.lat;
    gps->lon = g_gps_sim868_data.lon;
    gps->elv = g_gps_sim868_data.elv;
    gps->timestamp = aos_now_ms();
    
    return (int)size;
}

static void gps_simcom_sim868_irq_handle(void)
{
    /* no handle so far */
}

static int gps_simcom_sim868_ioctl(int cmd, unsigned long arg)
{
    return 0;
}



static int gps_simcom_sim868_env_init()
{
    int ret = 0;
#if AOS_ATCMD

    ret = at.init(AT_RECV_PREFIX, AT_RECV_SUCCESS_POSTFIX, 
                  AT_RECV_FAIL_POSTFIX, AT_SEND_DELIMITER, 10000);
    if(0 != ret){
        LOG("%s :   line %d  ret %d\n",__func__,__LINE__,ret);
        ret -1;
    }
#endif
    
    return 0;

}


int gps_simcom_sim868_init(void)
{
    int ret = 0;
    sensor_obj_t gpsobj;

    ret = gps_simcom_sim868_env_init();
    if(unlikely(ret)){
        return -1;
    }
    /* fill the gps obj parameters here */
    gpsobj.io_port    = UART_PORT;
    gpsobj.tag        = TAG_DEV_GPS;
    gpsobj.path       = dev_gps_path;
    gpsobj.mode       = DEV_POLLING;
    gpsobj.open       = gps_simcom_sim868_open;
    gpsobj.close      = gps_simcom_sim868_close;
    gpsobj.read       = gps_simcom_sim868_read;
    gpsobj.write      = NULL;
    gpsobj.ioctl      = gps_simcom_sim868_ioctl;
    gpsobj.irq_handle = gps_simcom_sim868_irq_handle;         

    ret = gps_create_obj(&gpsobj);
    if(unlikely(ret)){
        return -1;
    }

    LOG("%s %s successfully \n", GPS_STR, __func__);
    
    return 0;
}



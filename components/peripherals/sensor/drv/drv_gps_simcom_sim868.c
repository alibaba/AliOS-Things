/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*
*
*/


#include "gps_parse.h"
#include "sensor_hal.h"


#ifdef AOS_ATCMD
#include <atparser.h>
#endif

#define GPS_EV_UDATA                        (0x40)
#define GPS_DEV_READ                        (1)
#define GPS_DEV_PROC                        (2)
#define GPS_DEV_SEND                        (3)



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

typedef enum{
    AT_CB_ON,
    AT_CB_OFF,
    AT_CB_INVLID,
}at_cb_mode_e;


typedef enum{
    NONINF   = 0x0000,
    UGNSINF  = 0x0001,
    CGNSINF  = 0x0002
}en_gnsinf_type;


typedef struct _gps_sim868
{
    char        name[GPS_TYPE_NAME_LEN];
    int         run_stat;
    int         fix_stat;
    gps_time_t  utc;      
    float       lat;        
    float       lon;
    float       elv;       
    float       speed;       
    float       course; 
    int         fix_mode; 
    int         rev1; 
    
    float       HDOP; 
    float       PDOP; 
    float       VDOP; 
    int         rev2; 

    int         gps_sat_num; 
    int         gnss_sat_num; 
    int         glo_sat_num; 
    int         rev3; 

    int         cn0_max;
    float       HPA;
    float       VPA;

} gps_sim868_t;

#define GPS_SIM868_T_PARA_NUM     (22)

typedef struct sim868_inernel_data_stu{
    gps_sim868_t  data_sim868;

}sim868_inernel_data_t;


static sim868_inernel_data_t  g_sim868value;
static int  g_sim868typebitmap = CGNSINF;

static at_cb_mode_e g_sim868_mode = AT_CB_OFF;

static char    g_gps_sim868_addr[GPS_RCV_DATA_LEN];
static gps_data_t g_gps_sim868_data;
static char g_gps_recv_buf[GPS_RCV_DATA_LEN];
extern int at_dev_fd;


static void  drv_gps_simcom_sim868_para_set(test_gps_data_t sim868_index[], gps_sim868_t* sim868_para)
{
    sim868_index[0].type = GPS_TYPE_STR;
    sim868_index[0].addr = &(sim868_para->name[0]);

    sim868_index[1].type = GPS_TYPE_INT32;
    sim868_index[1].addr = &(sim868_para->run_stat);
    
    sim868_index[2].type = GPS_TYPE_INT32;
    sim868_index[2].addr = &(sim868_para->fix_stat);

    sim868_index[3].type = GPS_TYPE_UTC;
    sim868_index[3].addr = &(sim868_para->utc);

    sim868_index[4].type = GPS_TYPE_FLOAT;
    sim868_index[4].addr = &(sim868_para->lat);
    sim868_index[5].type = GPS_TYPE_FLOAT;
    sim868_index[5].addr = &(sim868_para->lon);
    sim868_index[6].type = GPS_TYPE_FLOAT;
    sim868_index[6].addr = &(sim868_para->elv);


    sim868_index[7].type = GPS_TYPE_FLOAT;
    sim868_index[7].addr = &(sim868_para->speed);
    sim868_index[8].type = GPS_TYPE_FLOAT;
    sim868_index[8].addr = &(sim868_para->course);
    sim868_index[9].type = GPS_TYPE_INT32;
    sim868_index[9].addr = &(sim868_para->fix_mode);

    sim868_index[10].type = GPS_TYPE_INT32;
    sim868_index[10].addr = &(sim868_para->rev1);

    
    sim868_index[11].type = GPS_TYPE_FLOAT;
    sim868_index[11].addr = &(sim868_para->HDOP);
    sim868_index[12].type = GPS_TYPE_FLOAT;
    sim868_index[12].addr = &(sim868_para->PDOP);
    sim868_index[13].type = GPS_TYPE_FLOAT;
    sim868_index[13].addr = &(sim868_para->VDOP);


    sim868_index[14].type = GPS_TYPE_INT32;
    sim868_index[14].addr = &(sim868_para->rev2);

    
    sim868_index[15].type = GPS_TYPE_INT32;
    sim868_index[15].addr = &(sim868_para->gps_sat_num);
    sim868_index[16].type = GPS_TYPE_INT32;
    sim868_index[16].addr = &(sim868_para->gnss_sat_num);
    sim868_index[17].type = GPS_TYPE_INT32;
    sim868_index[17].addr = &(sim868_para->glo_sat_num);
    
    sim868_index[18].type = GPS_TYPE_INT32;
    sim868_index[18].addr = &(sim868_para->rev3);

    sim868_index[19].type = GPS_TYPE_INT32;
    sim868_index[19].addr = &(sim868_para->cn0_max);

    sim868_index[20].type = GPS_TYPE_FLOAT;
    sim868_index[20].addr = &(sim868_para->HPA);
    sim868_index[21].type = GPS_TYPE_FLOAT;
    sim868_index[21].addr = &(sim868_para->VPA);

}

static int drv_gps_simcom_sim868_data_parse(char *str, int len, gps_sim868_t *result)
{
    int i = 0;
    char data[GPS_RCV_DATA_LEN];
    char* prt0;
    char* prt1 = &data[0];
    test_gps_data_t    index[GPS_SIM868_T_PARA_NUM];

    if((NULL == str) || (0 == result)){
        return -1;
    }
    
    memcpy(data,str,len);
    data[len] = '\0';
     
    memset(result, 0, sizeof(gps_sim868_t));
    drv_gps_simcom_sim868_para_set(index,result);

    prt0 = gps_strtok(prt1,&prt1,':',strlen(prt1));
    gps_data_conv(prt0,strlen(prt0),index[i].addr,index[i].type);
    i++;

    for(; (i < 22) &&(NULL!=prt0); i++){
        prt0 = gps_strtok(prt1,&prt1,',',strlen(prt1));
        gps_data_conv(prt0,strlen(prt0),index[i].addr,index[i].type);
    }

    return 0;
}



static int drv_gps_simcom_sim868_data_get(gps_sim868_t *result,gps_data_t* pgpsdata)
{

    if((NULL == result) || (0 == pgpsdata)){
        return -1;
    }

    pgpsdata->utc.year = result->utc.year;
    pgpsdata->utc.mon = result->utc.mon;
    pgpsdata->utc.day = result->utc.day;
    pgpsdata->utc.hour = result->utc.hour;
    pgpsdata->utc.min = result->utc.min;
    pgpsdata->utc.sec = result->utc.sec;
    pgpsdata->utc.hsec = result->utc.hsec;

    pgpsdata->lat = result->lat;
    pgpsdata->lon = result->lon;
    pgpsdata->elv = result->elv;
     
    return 0;
}



static int drv_gps_simcom_sim868_type_get(const char *buf, int size)
{
    static const char *pheads[] = {
        "\n+UGNSINF",
        "\n+CGNSINF"
    };

    if(0 == buf){
        return -1;
    }

    if(size < 8)
        return NONINF;
    else if(0 == memcmp(buf, pheads[0], 5))
        return UGNSINF;
    else if(0 == memcmp(buf, pheads[1], 5))
        return CGNSINF;

    return NONINF;
}

UNUSED static bool drv_gps_simcom_sim868_type_filter(const char *buf, int size)
{
    int ret = drv_gps_simcom_sim868_type_get(buf,size);

    return (g_sim868typebitmap&ret) ? 1:0;
}


static int drv_gps_simcom_sim868_parse(char* str, int len, gps_data_t* pgpsdata)
{
    int ret;
    int ptype;
    
    ptype = drv_gps_simcom_sim868_type_get(str,len);

    switch(ptype){
        case UGNSINF:
        case CGNSINF:{
            ret = drv_gps_simcom_sim868_data_parse(str, len,&(g_sim868value.data_sim868));
            if(0 != ret){
                return -1;
            }

            ret = drv_gps_simcom_sim868_data_get(&(g_sim868value.data_sim868), pgpsdata);
            if(0 != ret){
                return -1;
            }
            break;
        }

        default:
            break;
    }

    return 0;
}

static int drv_gps_simcom_sim868_proc(const char* str, gps_data_t* pgpsdata)
{
    int ret;
    int len;

    if(0 == str){
        return -1;
    }

    if(0 == pgpsdata){
        return -1;
    }
    
    len = strlen(str);
    if(len >= GPS_RCV_DATA_LEN){
        return -1;
    }
    ret = drv_gps_simcom_sim868_parse((char*)str,len, pgpsdata);
    if(0 != ret){
        return -1;
    }

    return 0;
}


static int drv_gps_simcom_sim868_power_on(at_cb_mode_e mode)
{
    int ret = 0;
    char rsp[SIM868_GPS_DEFAULT_RSP_LEN] = {0};

    if(mode >= AT_CB_INVLID){
        LOG("func:%s line: %d  para: %d error\n",__func__, __LINE__,mode);
        return -1;
    }

    memset(rsp, 0, SIM868_GPS_DEFAULT_RSP_LEN);
    ret = at_send_wait_reply(at_dev_fd, SIM868_AT_CMD_GPS_POWER_ON, strlen(SIM868_AT_CMD_GPS_POWER_ON),
                             true, NULL, 0, rsp, SIM868_GPS_DEFAULT_RSP_LEN, NULL);
    if ((0 != ret) || (strstr(rsp, SIM868_AT_CMD_SUCCESS_RSP) == NULL)) {
        LOG("%s %d failed rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);
        return -1;
    }

    memset(rsp, 0, SIM868_GPS_DEFAULT_RSP_LEN);
    ret = at_send_wait_reply(at_dev_fd, SIM868_AT_CMD_GPS_LASTPARSE_SET, strlen(SIM868_AT_CMD_GPS_LASTPARSE_SET),
                             true, NULL, 0, rsp, SIM868_GPS_DEFAULT_RSP_LEN, NULL);
    if ((0 != ret) || (strstr(rsp, SIM868_AT_CMD_SUCCESS_RSP) == NULL)) {
        LOG("%s %d failed rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);
        return -1;
    }
    
    memset(rsp, 0, SIM868_GPS_DEFAULT_RSP_LEN);
    if(AT_CB_OFF == mode){
        ret = at_send_wait_reply(at_dev_fd, SIM868_AT_CMD_GPS_INTERVAL_CLOSE, strlen(SIM868_AT_CMD_GPS_INTERVAL_CLOSE),
                                 true, NULL, 0, rsp, SIM868_GPS_DEFAULT_RSP_LEN, NULL);
    }
    else{
        ret = at_send_wait_reply(at_dev_fd, SIM868_AT_CMD_GPS_INTERVAL_SET, strlen(SIM868_AT_CMD_GPS_INTERVAL_SET),
                                 true, NULL, 0, rsp, SIM868_GPS_DEFAULT_RSP_LEN, NULL);
    }
    
    if ((0 != ret) ||  (strstr(rsp, SIM868_AT_CMD_SUCCESS_RSP) == NULL)) {
        LOG("%s %d failed rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);
        return -1;
    }

    memset(rsp, 0, SIM868_GPS_DEFAULT_RSP_LEN);
    ret = at_send_wait_reply(at_dev_fd, SIM868_AT_CMD_GPS_SEND_MODE_SET, strlen(SIM868_AT_CMD_GPS_SEND_MODE_SET),
                             true, NULL, 0, rsp, SIM868_GPS_DEFAULT_RSP_LEN, NULL);
    if ((0 != ret) || (strstr(rsp, SIM868_AT_CMD_SUCCESS_RSP) == NULL)) {
        LOG("%s %d failed rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);
        return -1;
    }  
        
    return 0;
}

static int drv_gps_simcom_sim868_power_off()
{
    int ret = 0;
    char rsp[SIM868_GPS_DEFAULT_RSP_LEN] = {0};
    
    memset(rsp, 0, SIM868_GPS_DEFAULT_RSP_LEN);
    ret = at_send_wait_reply(at_dev_fd, SIM868_AT_CMD_GPS_POWER_OFF, strlen(SIM868_AT_CMD_GPS_POWER_OFF),
                             true, NULL, 0, rsp, SIM868_GPS_DEFAULT_RSP_LEN, NULL);
    if ((0 != ret) || (strstr(rsp, SIM868_AT_CMD_SUCCESS_RSP) == NULL)) {
        LOG("%s %d failed rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);
        return -1;
    }
        
    return 0;
}

static int drv_gps_simcom_sim868_check(void)
{
    int ret = 0;
    char rsp[SIM868_GPS_DEFAULT_RSP_LEN] = {0};

    memset(rsp, 0, SIM868_GPS_DEFAULT_RSP_LEN);
    ret = at_send_wait_reply(at_dev_fd, SIM868_AT_CMD_GPS_POWER_CHECK, strlen(SIM868_AT_CMD_GPS_POWER_CHECK),
                             true, NULL, 0, rsp, SIM868_GPS_DEFAULT_RSP_LEN, NULL);
    if ((0 != ret) || (strstr(rsp, SIM868_AT_CMD_SUCCESS_RSP) == NULL)) {
        LOG("%s %d failed rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);
        return -1;
    }
    LOG("GPS power check %s \r\n", rsp);

    memset(rsp, 0, SIM868_GPS_DEFAULT_RSP_LEN);
    ret = at_send_wait_reply(at_dev_fd, SIM868_AT_CMD_GPS_LASTPARSE_CHECK, strlen(SIM868_AT_CMD_GPS_LASTPARSE_CHECK),
                             true, NULL, 0, rsp, SIM868_GPS_DEFAULT_RSP_LEN, NULL);
    if ((0 != ret) || (strstr(rsp, SIM868_AT_CMD_SUCCESS_RSP) == NULL))  {
        LOG("%s %d failed rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);
        return -1;
    }
    LOG("GPS LAST PARSE %s \r\n", rsp);

    memset(rsp, 0, SIM868_GPS_DEFAULT_RSP_LEN);
    ret = at_send_wait_reply(at_dev_fd, SIM868_AT_CMD_GPS_POSITION_GET, strlen(SIM868_AT_CMD_GPS_POSITION_GET),
                             true, NULL, 0, rsp, SIM868_GPS_DEFAULT_RSP_LEN, NULL);
    if ((0 != ret) || (strstr(rsp, SIM868_AT_CMD_SUCCESS_RSP) == NULL))  {
        LOG("%s %d failed rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);
        return -1;
    }
    LOG("GPS POSITION %s \r\n", rsp);

    memset(rsp, 0, SIM868_GPS_DEFAULT_RSP_LEN);
    ret = at_send_wait_reply(at_dev_fd, SIM868_AT_CMD_GPS_INTERVAL_CHECK, strlen(SIM868_AT_CMD_GPS_INTERVAL_CHECK),
                             true, NULL, 0, rsp, SIM868_GPS_DEFAULT_RSP_LEN, NULL);
    if ((0 != ret) || (strstr(rsp, SIM868_AT_CMD_SUCCESS_RSP) == NULL))  {
        LOG("%s %d failed rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);
        return -1;
    }
    LOG("GPS INVERVAL %s \r\n", rsp);


    memset(rsp, 0, SIM868_GPS_DEFAULT_RSP_LEN);
    ret = at_send_wait_reply(at_dev_fd, SIM868_AT_CMD_GPS_SEND_MODE_CHECK, strlen(SIM868_AT_CMD_GPS_SEND_MODE_CHECK),
                             true, NULL, 0, rsp, SIM868_GPS_DEFAULT_RSP_LEN, NULL);
    if ((0 != ret) || (strstr(rsp, SIM868_AT_CMD_SUCCESS_RSP) == NULL))  {
        LOG("%s %d failed rsp %s errno %d\r\n", __func__, __LINE__, rsp,ret);
        return -1;
    }
    LOG("GPS send mode %s \r\n", rsp);
        
    return 0;
}


static int drv_gps_simcom_sim868_config(at_cb_mode_e mode)
{
    int ret;

    ret = drv_gps_simcom_sim868_power_off();
    if(0 != ret)
    {
        return ret;
    }

    ret = drv_gps_simcom_sim868_power_on(mode);
    if(0 != ret)
    {
        return ret;
    }
    
    return drv_gps_simcom_sim868_check();
}


static int drv_gps_simcom_sim868_poll_read(char* rsp)
{
    int ret = 0;
    memset(rsp, 0, SIM868_GPS_DEFAULT_RSP_LEN);
    ret = at_send_wait_reply(at_dev_fd, SIM868_AT_CMD_GPS_POSITION_GET, strlen(SIM868_AT_CMD_GPS_POSITION_GET),
                             true, NULL, 0, rsp, SIM868_GPS_DEFAULT_RSP_LEN, NULL);
    if ((strstr(rsp, SIM868_AT_CMD_SUCCESS_RSP) == NULL) || (ret != 0)) {
        LOG("%s %d failed rsp %s\r\n", __func__, __LINE__, rsp);
        return -1;
    }
    
    return (int)strlen(rsp);
}

static void drv_gps_simcom_sim868_ood_cb(void *arg, char* buf, int size)
{
    int ret;
    char* str;
    if((NULL == buf) || (0 == size) || (size >= GPS_RCV_DATA_LEN-1)){
        return;
    }

    str = &g_gps_sim868_addr[0];
    memcpy(str,(void*)buf,size);
    str[size] = '\0';

    ret = drv_gps_simcom_sim868_proc(str,&g_gps_sim868_data);
    if(unlikely(ret)){
        return;
    }
}

static int drv_gps_simcom_sim868_ood_cb_reg(at_cb_mode_e mode)
{

    if(AT_CB_ON == mode){
        at_register_callback(at_dev_fd, SIM868_GPS_HEAD_LOOP, SIM868_GPS_TAIL_LOOP, g_gps_recv_buf, GPS_RCV_DATA_LEN-1, drv_gps_simcom_sim868_ood_cb, NULL);
    }
    else{
        LOG("func : %s no need\n",__func__);
    }
    
    return 0;
}

static int drv_gps_simcom_sim868_cb_unreg(void)
{
    /*unreg uart callback, ref to at.oob*/
    return 0;
}

static int drv_gps_simcom_sim868_open(void)
{
    int ret;
    
    ret = drv_gps_simcom_sim868_ood_cb_reg(g_sim868_mode);
    if(0 != ret){
        return -1;
    }

    ret = drv_gps_simcom_sim868_config(g_sim868_mode);
    if(0 != ret){
        return -1;
    }
    
    return 0;
}

static int drv_gps_simcom_sim868_close(void)
{
    int ret;

    ret = drv_gps_simcom_sim868_power_off();
    if(0 != ret){
        return -1;
    }

    ret = drv_gps_simcom_sim868_cb_unreg();
    if(0 != ret){
        return -1;
    }

    return 0;
}

static int drv_gps_simcom_sim868_data_read()
{   
    int ret = 0;
    char rsp[SIM868_GPS_DEFAULT_RSP_LEN] ={0};
    
    ret = drv_gps_simcom_sim868_poll_read(rsp);
    if((ret <= 0) || (ret >= SIM868_GPS_DEFAULT_RSP_LEN))
    {
        return -1;
    }
    ret = drv_gps_simcom_sim868_proc(rsp,&g_gps_sim868_data);
    if(0 != ret){
        return -1;
    }
    return 0;
}

static int drv_gps_simcom_sim868_read(void *buf, size_t len)
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
    
    if(AT_CB_OFF == g_sim868_mode){
        ret = drv_gps_simcom_sim868_data_read();
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

static void drv_gps_simcom_sim868_irq_handle(void)
{
    /* no handle so far */
}

static int drv_gps_simcom_sim868_ioctl(int cmd, unsigned long arg)
{
    return 0;
}


int drv_gps_simcom_sim868_init(void)
{
    int ret = 0;
    sensor_obj_t gpsobj;

    /* fill the gps obj parameters here */
    gpsobj.io_port    = UART_PORT;
    gpsobj.tag        = TAG_DEV_GPS;
    gpsobj.path       = dev_gps_path;
    gpsobj.mode       = DEV_POLLING;
    gpsobj.open       = drv_gps_simcom_sim868_open;
    gpsobj.close      = drv_gps_simcom_sim868_close;
    gpsobj.read       = drv_gps_simcom_sim868_read;
    gpsobj.write      = NULL;
    gpsobj.ioctl      = drv_gps_simcom_sim868_ioctl;
    gpsobj.irq_handle = drv_gps_simcom_sim868_irq_handle;         

    ret = sensor_create_obj(&gpsobj);
    if(unlikely(ret)){
        return -1;
    }

    LOG("%s %s successfully \n", GPS_STR, __func__);
    
    return 0;
}

SENSOR_DRV_ADD(drv_gps_simcom_sim868_init);


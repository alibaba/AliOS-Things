/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*
*
*/

#include "gps_hal.h"
#include "gps_parse.h"

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

static void  gps_simcom_sim868_para_set(test_gps_data_t sim868_index[], gps_sim868_t* sim868_para)
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

static int gps_simcom_sim868_data_parse(char *str, int len, gps_sim868_t *result)
{
    int i = 0;
    int ret;
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
    gps_simcom_sim868_para_set(index,result);

    prt0 = gps_strtok(prt1,&prt1,':',strlen(prt1));
    gps_data_conv(prt0,strlen(prt0),index[i].addr,index[i].type);
    i++;

    for(; (i < 22) &&(NULL!=prt0); i++){
        prt0 = gps_strtok(prt1,&prt1,',',strlen(prt1));
        gps_data_conv(prt0,strlen(prt0),index[i].addr,index[i].type);
    }

    return 0;
}



static int gps_simcom_sim868_data_get(gps_sim868_t *result,gps_data_t* pgpsdata)
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



int gps_simcom_sim868_type_get(const char *buf, int size)
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

static bool gps_simcom_sim868_type_filter(const char *buf, int size)
{
    int ret = gps_simcom_sim868_type_get(buf,size);

    return (g_sim868typebitmap&ret) ? 1:0;
}


static int gps_simcom_sim868_parse(char* str, int len, gps_data_t* pgpsdata)
{
    int ret;
    int ptype;
    
    ptype = gps_simcom_sim868_type_get(str,len);

    switch(ptype){
        case UGNSINF:
        case CGNSINF:{
            ret = gps_simcom_sim868_data_parse(str, len,&(g_sim868value.data_sim868));
            if(0 != ret){
                return -1;
            }

            ret = gps_simcom_sim868_data_get(&(g_sim868value.data_sim868), pgpsdata);
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

int gps_simcom_sim868_proc(const char* str, gps_data_t* pgpsdata)
{
    int ret;
    int len;
    int ptype;
    char  gpsdata[GPS_CALC_BUF_LEN];

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
#if 0    
    if(!(gps_gp_type_filter((str+1), len-1))){
        return -1;
    }
#endif
    
    memcpy(&gpsdata[0], str, len);
    gpsdata[len] = '\0';

    ret = gps_simcom_sim868_parse(&gpsdata[0],len, pgpsdata);
    if(0 != ret){
        return -1;
    }

    return 0;
}




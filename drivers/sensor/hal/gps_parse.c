/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*
*
*/

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "aos/kernel.h"
#include <atparser.h>
#include "gps_parse.h"

typedef enum{
    GPNON   = 0x0000,
    GPGGA   = 0x0001,
    GPGSA   = 0x0002,
    GPGSV   = 0x0004,
    GPRMC   = 0x0008,
    GPVTG   = 0x0010
}en_gps_type;


typedef struct _gps_gpgga
{
    char        name[GPS_TYPE_NAME_LEN];
    gps_time_t  utc;
    float       lat;
    char        ns;
    float       lon;
    char        ew;
    int         sig;
    int         satinuse;
    float       HDOP;
    float       elv;
    char        elv_units;
    float       diff;
    char        diff_units;
    float       dgps_age;
    int         dgps_sid;
} gps_gpgga_t;

#define  GPS_GPGGA_T_PARA_NUM   (15)
typedef struct gps_inernel_data_stu{
    gps_gpgga_t  data_gga;

}gps_inernel_data_t;


gps_inernel_data_t  g_gpsvalue;
int     g_gpstypebitmap = GPGGA;

static void  gps_gpgga_para_set(test_gps_data_t gpgga_index[], gps_gpgga_t* gpgga_para)
{
    gpgga_index[0].type = GPS_TYPE_STR;
    gpgga_index[0].addr = &(gpgga_para->name[0]);
    gpgga_index[1].type = GPS_TYPE_UTC;
    gpgga_index[1].addr = &(gpgga_para->utc);

    gpgga_index[2].type = GPS_TYPE_FLOAT;
    gpgga_index[2].addr = &(gpgga_para->lat);
    gpgga_index[3].type = GPS_TYPE_UINT8;
    gpgga_index[3].addr = &(gpgga_para->ns);
    gpgga_index[4].type = GPS_TYPE_FLOAT;
    gpgga_index[4].addr = &(gpgga_para->lon);
    gpgga_index[5].type = GPS_TYPE_UINT8;
    gpgga_index[5].addr = &(gpgga_para->ew);

    gpgga_index[6].type = GPS_TYPE_INT32;
    gpgga_index[6].addr = &(gpgga_para->sig);
    gpgga_index[7].type = GPS_TYPE_INT32;
    gpgga_index[7].addr = &(gpgga_para->satinuse);
    gpgga_index[8].type = GPS_TYPE_FLOAT;
    gpgga_index[8].addr = &(gpgga_para->HDOP);
    gpgga_index[9].type = GPS_TYPE_FLOAT;
    gpgga_index[9].addr = &(gpgga_para->elv);

    gpgga_index[10].type = GPS_TYPE_UINT8;
    gpgga_index[10].addr = &(gpgga_para->elv_units);
    gpgga_index[11].type = GPS_TYPE_FLOAT;
    gpgga_index[11].addr = &(gpgga_para->diff);
    gpgga_index[12].type = GPS_TYPE_UINT8;
    gpgga_index[12].addr = &(gpgga_para->diff_units);
    gpgga_index[13].type = GPS_TYPE_FLOAT;
    gpgga_index[13].addr = &(gpgga_para->dgps_age);

    gpgga_index[14].type = GPS_TYPE_INT32;
    gpgga_index[14].addr = &(gpgga_para->dgps_sid);

}


static int gps_gpgga_data_parse(char *str, int len, gps_gpgga_t *result)
{
    int i = 0;
    char data[GPS_RCV_DATA_LEN];
    char* prt0;
    char* prt1 = &data[0];
    test_gps_data_t    gga_idx[GPS_GPGGA_T_PARA_NUM];

    if((NULL == str) || (0 == result)){
        return -1;
    }

    memcpy(data,str,len);
    data[len] = '\0';

    memset(result, 0, sizeof(gps_gpgga_t));
    gps_gpgga_para_set(gga_idx,result);

    for( i = 0; (i < 13) &&(NULL!=prt0); i++){
        //printf("strlen(prt1) == %d\n",strlen(prt1));

        prt0 = gps_strtok(prt1,&prt1,',',strlen(prt1));
        //printf("prt0 == %s\n",prt0);

        gps_data_conv(prt0,strlen(prt0),gga_idx[i].addr,gga_idx[i].type);
        //printf("index %d\n\n",i);
    }

    prt0 = gps_strtok(prt1,&prt1,',',strlen(prt1));
    gps_data_conv(prt0,strlen(prt0),gga_idx[i].addr,gga_idx[i].type);
    i++;
    //printf("index %d\n\n\n",i);

    prt0 = gps_strtok(prt1,&prt1,'*',strlen(prt1));
    gps_data_conv(prt0,strlen(prt0),gga_idx[i].addr,gga_idx[i].type);
    //printf("index %d\n\n\n",i);

    //printf("time %2d:%2d:%2d.%d\n",result->utc.hour,result->utc.min,result->utc.sec,result->utc.hsec);

    return 0;
}



static int gps_gpgga_data_get(gps_gpgga_t *result,gps_data_t* pgpsdata)
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

    pgpsdata->lat = ('N' == result->ns)? result->lat : -(result->lat);
    pgpsdata->lat = pgpsdata->lat/100;
    pgpsdata->lon = ('E' == result->ew)? result->lon : -(result->lon);
    pgpsdata->lon = pgpsdata->lon/100;
    pgpsdata->elv = result->elv;

    return 0;
}



int gps_gp_type_get(const char *buf, int size)
{
    static const char *pheads[] = {
        "GPGGA",
        "GPGSA",
        "GPGSV",
        "GPRMC",
        "GPVTG"
    };

    if(0 == buf){
        return -1;
    }

    if(size < 5)
        return GPNON;
    else if(0 == memcmp(buf, pheads[0], 5))
        return GPGGA;
    else if(0 == memcmp(buf, pheads[1], 5))
        return GPGSA;
    else if(0 == memcmp(buf, pheads[2], 5))
        return GPGSV;
    else if(0 == memcmp(buf, pheads[3], 5))
        return GPRMC;
    else if(0 == memcmp(buf, pheads[4], 5))
        return GPVTG;

    return GPNON;
}

bool gps_gp_type_filter(const char *buf, int size)
{
    int ret = gps_gp_type_get(buf,size);

    return (g_gpstypebitmap&ret) ? 1:0;
}


static int gps_gp_parse(char* str, int len, gps_data_t* pgpsdata)
{
    int ret;
    int ptype;

    ptype = gps_gp_type_get(str+1,len-1);

    switch(ptype){
        case GPGGA:{
            ret = gps_gpgga_data_parse(str, len,&(g_gpsvalue.data_gga));
            if(0 != ret){
                return -1;
            }

            ret = gps_gpgga_data_get(&(g_gpsvalue.data_gga), pgpsdata);
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

static int gps_gp_check(char* str, int len)
{
    int i;
    int crc_calc = 0;
    int crc = 0;
    char* str_tmp = str;

    if(str_tmp[0] != '$'){
        return -1;
    }

    str_tmp++;

    for(i = 1; (i < len) && (*str_tmp != '*'); i++,str_tmp++){
        crc_calc ^= (int)(*str_tmp);
    }

    crc = gps_atoi(&str_tmp[1],2,16);
    if(crc != crc_calc){
        LOG("crc_origin == 0x%08x, crc_calc == 0x%08x\n",crc,crc_calc);
        return -1;
    }

    return  0;
}

int gps_gp_proc(const char* str, gps_data_t* pgpsdata)
{
    int ret;
    int len;
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

    memcpy(&gpsdata[0], str, len);
    gpsdata[len] = '\0';


    ret = gps_gp_check(&gpsdata[0],len);
    if(0 != ret){
        return -1;
    }

    ret = gps_gp_parse(&gpsdata[0],len, pgpsdata);
    if(0 != ret){
        return -1;
    }

    return 0;
}




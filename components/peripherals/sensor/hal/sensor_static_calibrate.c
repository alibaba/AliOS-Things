#include "sensor_static_calibrate.h"

#define DST_X 0
#define DST_Y 1
#define DST_Z 2
#define SRC_X 0
#define SRC_Y 0
#define SRC_Z 0
#define MAX_SRNSOR_NUM 10

typedef struct _dev_3axis_data_t
{
    uint64_t timestamp;
    int32_t  data[3];
}dev_3axis_data_t;

typedef struct _dev_1axis_data_t
{
    uint64_t timestamp;
    int32_t  data;
}dev_1axis_data_t;

typedef struct _sensor_1axis_para_t
{
    int offset;
}sensor_1axis_para_t;

typedef struct _sensor_3axis_para_t
{
    int remap_x_coeff;
    int remap_y_coeff;
    int remap_z_coeff;
    int offset;
}sensor_3axis_para_t;

typedef struct _sensor_static_obj_t
{
    int in_use;
    sensor_tag_e tag;
    int sensor_index;
    int axis;
    void* para;
}sensor_static_obj_t;

sensor_static_obj_t g_sensor_static_para[MAX_SRNSOR_NUM] =  {0};


sensor_3axis_para_t default_3axis_para[] ={
1,   0,  0,        0,
0,   1,  0,        0,
0,   0,  1,        0,
};

sensor_1axis_para_t default_1axis_para ={0};

int findindex(sensor_tag_e tag ,int index)
{
}

void sensor_static_calibrate_3axis_default(void *buf)
{
    dev_3axis_data_t* pdata=(dev_3axis_data_t*)buf;
    dev_3axis_data_t datatmp;
    memcpy(&datatmp,pdata,sizeof(dev_3axis_data_t));

    pdata->data[DST_X] = datatmp.data[SRC_X]* default_3axis_para[DST_X].remap_x_coeff ;
    pdata->data[DST_X] += datatmp.data[SRC_Y]* default_3axis_para[DST_X].remap_y_coeff;
    pdata->data[DST_X] += datatmp.data[SRC_Z]* default_3axis_para[DST_X].remap_z_coeff;
    pdata->data[DST_X] += default_3axis_para[DST_X].offset;

    pdata->data[DST_Y] = datatmp.data[SRC_X]* default_3axis_para[DST_Y].remap_x_coeff;
    pdata->data[DST_Y] += datatmp.data[SRC_Y]* default_3axis_para[DST_Y].remap_y_coeff;
    pdata->data[DST_Y] += datatmp.data[SRC_Z]* default_3axis_para[DST_Y].remap_z_coeff;
    pdata->data[DST_Y] += default_3axis_para[DST_Y].offset;

    pdata->data[DST_Z] = datatmp.data[SRC_X]* default_3axis_para[DST_Z].remap_x_coeff ;
    pdata->data[DST_Z] += datatmp.data[SRC_Y]* default_3axis_para[DST_Z].remap_y_coeff;
    pdata->data[DST_Z] += datatmp.data[SRC_Z]* default_3axis_para[DST_Z].remap_z_coeff;
    pdata->data[DST_Z] += default_3axis_para[DST_Z].offset;
}


void sensor_static_calibrate_3axis(void *buf,void* para)
{
    dev_3axis_data_t* pdata=(dev_3axis_data_t*)buf;
    dev_3axis_data_t datatmp;
    sensor_3axis_para_t* ppara=(sensor_3axis_para_t*)para;
    memcpy(&datatmp,pdata,sizeof(dev_3axis_data_t));

    pdata->data[DST_X] = datatmp.data[SRC_X]* ppara->remap_x_coeff ;
    pdata->data[DST_X] += datatmp.data[SRC_Y]* ppara->remap_y_coeff;
    pdata->data[DST_X] += datatmp.data[SRC_Z]* ppara->remap_z_coeff;
    pdata->data[DST_X] += ppara->offset;
    ppara++;
    
    pdata->data[DST_Y] = datatmp.data[SRC_X]* ppara->remap_x_coeff;
    pdata->data[DST_Y] += datatmp.data[SRC_Y]* ppara->remap_y_coeff;
    pdata->data[DST_Y] += datatmp.data[SRC_Z]* ppara->remap_z_coeff;
    pdata->data[DST_Y] += ppara->offset;
    ppara++;
    
    pdata->data[DST_Z] = datatmp.data[SRC_X]* ppara->remap_x_coeff ;
    pdata->data[DST_Z] += datatmp.data[SRC_Y]* ppara->remap_y_coeff;
    pdata->data[DST_Z] += datatmp.data[SRC_Z]* ppara->remap_z_coeff;
    pdata->data[DST_Z] += ppara->offset;
}

void sensor_static_calibrate_1axis_default(void *buf)
{
    dev_1axis_data_t* pdata=(dev_1axis_data_t*)buf;
    pdata->data += default_1axis_para.offset;
}

void sensor_static_calibrate_1axis(void *buf,void* para)
{
    dev_1axis_data_t* pdata=(dev_1axis_data_t*)buf;
    sensor_1axis_para_t* ppara=(sensor_1axis_para_t*)para;
    pdata->data += ppara->offset;
}

ssize_t sensor_static_calibrate_acc_default(int sensor_index, int axis,void* buf)
{
    if(axis == 3)
    {
        sensor_static_calibrate_3axis_default(buf);
    }
    else if(axis == 1)
    {
        sensor_static_calibrate_1axis_default(buf);
    }
    else
    {
        return -1;
    }
    
    return 0;
}


ssize_t sensor_static_calibrate_acc(int axis, void *buf,void* para)
{
    if(axis == 3)
    {
        sensor_static_calibrate_3axis(buf,para);
    }
    else if(axis == 1)
    {
        sensor_static_calibrate_1axis(buf,para);
    }
    else
    {
        return -1;
    }
    
    return 0;
}
/*
int sensor_static_calibrate(int axis, void *buf,void *para)
{
    int ret =-1;
    if(buf == NULL||para == NULL) return -1;
    if(tag == TAG_DEV_ACC)
    {
        ret = sensor_static_calibrate_acc(axis,buf,para);
    }
    else
    {
    }
    return ret;
}
*/
int find_empty_obj()
{
    for(int i = 0;i < MAX_SRNSOR_NUM; i++)
    {
        if(g_sensor_static_para[i].in_use==0)
            return i;
    }
    return -1;
}

int sensor_obj_init_para(sensor_tag_e tag,int sensor_index,int axis,void* para)
{
    int index = find_empty_obj();
    if(index < 0) return -1;
    g_sensor_static_para[index].sensor_index = sensor_index;
    g_sensor_static_para[index].tag = tag;
    g_sensor_static_para[index].para = para;
    g_sensor_static_para[index].axis = axis;
    g_sensor_static_para[index].in_use = 1;
    return 0;
}

int find_sensor_index(sensor_tag_e tag,int sensor_index,int axis)
{
    for(int i = 0;i < MAX_SRNSOR_NUM; i++)
    {
        if(g_sensor_static_para[i].tag==tag
            &&g_sensor_static_para[i].sensor_index==sensor_index
            &&g_sensor_static_para[i].axis == axis
            &&g_sensor_static_para[i].in_use == 1
            &&g_sensor_static_para[i].para != NULL)
            return i;
    }
    return -1;
}

int sensor_calibrate_entry(sensor_tag_e tag,int sensor_index,int axis, void *buf)
{
    int ret = 0;
    if(buf == NULL) return -1;
    int index = find_sensor_index(tag,sensor_index,axis);
    if(index < 0) return -1;
    
    if(tag == TAG_DEV_ACC)
    {
        ret = sensor_static_calibrate_acc(axis,buf,g_sensor_static_para[index].para);
    }
    else
    {
        return -1;
    }
    return ret;
}

int sensor_calibrate_entry_default(sensor_tag_e tag,int sensor_index,int axis, void *buf)
{
    int ret = 0;
    if(buf == NULL) return -1;
    
    if(tag == TAG_DEV_ACC)
    {
        ret = sensor_static_calibrate_acc_default(sensor_index,axis,buf);
    }
    else
    {
        return -1;
    }
    return ret;
}



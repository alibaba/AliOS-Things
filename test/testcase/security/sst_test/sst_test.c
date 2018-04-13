/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <yunit.h>
#include <yts.h>
#include <ks.h>
#include <sst.h>
#include <stdio.h>
#include "aos/aos.h"



static void sst_test_3(void)
{
    uint32_t ret = 0;
    uint8_t file_name[] = "3.sst";
    size_t data_size =0;
    uint8_t data[32] = {0x01,0x02,0x03,0x4,0x05,0x06,0x07,0x08,
                        0x01,0x02,0x03,0x4,0x05,0x06,0x07,0x08,
                        0x01,0x02,0x03,0x4,0x05,0x06,0x07,0x08,
                        0x01,0x02,0x03,0x4,0x05,0x06,0x07,0x08};
    uint8_t obj_id[SST_OBJ_ID_LEN] = {0x13,0x02,0x03,0x4,0x05,0x06,0x07,0x08,
                                      0x21,0x02,0x03,0x4,0x05,0x06,0x07,0x08};

    uint8_t *p_out_data = NULL;

    printf(">>sst_test_3 start\n");

    ret = sst_store_file(file_name, obj_id, data, sizeof(data), 1);
    if( 0 != ret){
        printf("%s %d: error[%x]\n",__FUNCTION__, __LINE__, ret);
        return;
    }

    ret = sst_get_file_size(file_name, &data_size);
    if( 0 != ret){
        printf("%s %d: error[%x]\n",__FUNCTION__, __LINE__, ret);
        return;
    }

   
    p_out_data = aos_malloc(data_size);
    if(!p_out_data){
        printf("%s %d: error[%x]\n",__FUNCTION__, __LINE__, ret);
        return;
    }

    ret = sst_get_file(file_name, obj_id, p_out_data);
    if( 0 != ret){
        printf("sst get file err[%x]\n", ret);
    }
    if(0 != memcmp(p_out_data, data, sizeof(data))){
        printf(">>sst_test_3 fail\n");
    }else{
        printf(">>sst_test_3 success\n");
    }
    aos_free(p_out_data);
    return;
}

static void sst_test_4(void)
{
    uint32_t ret = 0;
    uint8_t file_name[] = "4.sst";
    uint8_t data[35] = {0x11,0x02,0x03,0x4,0x05,0x06,0x07,0x08,
                        0x21,0x02,0x03,0x4,0x05,0x06,0x07,0x08,
                        0x31,0x02,0x03,0x4,0x05,0x06,0x07,0x08,
                        0x41,0x02,0x03,0x4,0x05,0x06,0x07,0x08,
                        0x02,0x03,0x4};
    uint8_t obj_id[SST_OBJ_ID_LEN] = {0x14,0x02,0x03,0x4,0x05,0x06,0x07,0x08,
                                      0x21,0x02,0x03,0x4,0x05,0x06,0x07,0x08};

    printf(">>sst_test_4 start\n");

    ret = sst_store_file(file_name, obj_id, data, sizeof(data), 0);
    if( 0 != ret){
        printf("%s %d: error[%x]\n",__FUNCTION__, __LINE__, ret);
        printf(">>sst_test_4 fail\n");
    }else{
        printf(">>sst_test_4 success\n");
    }
}

static void sst_test_5(void)
{
    uint32_t ret = 0;
    uint8_t file_name[] = "5kjdaoasdfjasoidfjoasidfjasidofiasodfisaodifaosdijfosajfdaoisfdoasifdaosdfiasofdiasdfoasidfaisjfdoaifdosadifaosdifsaodfiaosdfaisfoasfdjfisaodjfasidfjadfasdsdddddddddsfajsdofiaosdfjasdfsadfioasifdasodfoasdifosaijfsaofdioafdiasodfosafdsaoifdfasfaefawefalsadifjosajfaisdfoijsabaoibjoaadfasfdasdfjasiodjfosaidfjaosidfjaosifdaosdifosaidfaosifdoasifdaosidfaosdifaosdfiasdofiasdfoasifdoasifdsaodifosadifsoifdosifdosdsdsadfiosadfiasofisafibjaobijaoidjaodijfaoidj.sst";
    uint8_t data[35] = {0x11,0x02,0x03,0x4,0x05,0x06,0x07,0x08,
                        0x21,0x02,0x03,0x4,0x05,0x06,0x07,0x08,
                        0x31,0x02,0x03,0x4,0x05,0x06,0x07,0x08,
                        0x41,0x02,0x03,0x4,0x05,0x06,0x07,0x08,
                        0x02,0x03,0x4};
    uint8_t obj_id[SST_OBJ_ID_LEN] = {0x15,0x02,0x03,0x4,0x05,0x06,0x07,0x08,
                                      0x21,0x02,0x03,0x4,0x05,0x06,0x07,0x08};
    printf(">>sst_test_5 start\n");

    ret = sst_store_file(file_name, obj_id, data, sizeof(data), 0);
    if( 0 != ret){
        printf("%s %d: error[%x]\n",__FUNCTION__, __LINE__, ret);
    }
    if(ret != RET_SST_SUCCESS)
        printf(">>sst_test_5 success\n");
    else 
        printf(">>sst_test_5 fail\n");

}

static void sst_test_6(void)
{
    uint32_t ret = 0;
    uint8_t file_name[] = "6.sst";
    uint8_t data[1] = {0x11};
    uint8_t obj_id[SST_OBJ_ID_LEN] = {0x16,0x02,0x03,0x4,0x05,0x06,0x07,0x08,
                                      0x21,0x02,0x03,0x4,0x05,0x06,0x07,0x08};

    printf(">>sst_test_6 start\n");

    ret = sst_store_file(file_name, obj_id, data, sizeof(data), 0);
    if( 0 != ret){
        printf("%s %d: error[%x]\n",__FUNCTION__, __LINE__, ret);
        printf(">>sst_test_7 fail\n");
    }else{
        printf(">>sst_test_6 success\n");
    }
}

static void sst_test_7(void)
{
    uint32_t ret = 0;
    uint8_t file_name[] = "7.sst";
    uint8_t data[32] = {0x11,0x02,0x03,0x4,0x05,0x06,0x07,0x08,
                        0x21,0x02,0x03,0x4,0x05,0x06,0x07,0x08,
                        0x31,0x02,0x03,0x4,0x05,0x06,0x07,0x08,
                        0x41,0x02,0x03,0x4,0x05,0x06,0x07,0x08};
    uint8_t obj_id[SST_OBJ_ID_LEN] = {0x17,0x02,0x03,0x4,0x05,0x06,0x07,0x08,
                                      0x21,0x02,0x03,0x4,0x05,0x06,0x07,0x08};

    printf(">>sst_test_7 start\n");
    
    ret = sst_store_file(file_name, obj_id, data, sizeof(data), 0);
    if( 0 != ret){
        printf("%s %d: error[%x]\n",__FUNCTION__, __LINE__, ret);
        printf(">>sst_test_7 fail\n");
    }else{
        printf(">>sst_test_7 success\n");
    }
}

static int init(void)
{
    uint32_t ret = 0;
    
    ret = ks_init();
    if(0 != ret){
        printf("%s %d: error[%x]\n",__FUNCTION__, __LINE__, ret);
        return -1;
    }

    ret = sst_init();
    if(0 != ret){
        printf("%s %d: error[%x]\n",__FUNCTION__, __LINE__, ret);
        return -1;
    }
    return 0;
}

static int cleanup(void)
{
    return 0;
}

static void setup(void)
{

}

static void teardown(void)
{

}

static yunit_test_case_t aos_sst_testcases[] = {
    { "sst_case3", sst_test_3},
    { "sst_case4", sst_test_4},
    { "sst_case5", sst_test_5},
    { "sst_case6", sst_test_6},
    { "sst_case7", sst_test_7},
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "sst", init, cleanup, setup, teardown, aos_sst_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_sst(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_sst);

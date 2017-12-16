/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include <yunit.h>
#include <yts.h>
#include <aos/types.h>
#include "digest_algorithm.h"
#include "crc.h"

static void test_md5(void)
{
    void *md5 = NULL;
    char *buf_md5 = "asf";
    unsigned char buf_store[256] = {0};  
    int len = 3;
    int ret = 0;

    md5 = digest_md5_init();
    YUNIT_ASSERT(NULL != md5);
    
    ret = digest_md5_update(md5,buf_md5,len);
    YUNIT_ASSERT(0 == ret);
    
    ret = digest_md5(buf_md5,len,buf_store);
    YUNIT_ASSERT(0 == ret);

#ifdef CONFIG_AOS_YTS_ALL
    ret = digest_md5_file("/etc/bash.bashrc",buf_store);
    YUNIT_ASSERT(0 == ret);
#endif

    ret = digest_md5_final(md5,buf_store);
    YUNIT_ASSERT(0 == ret);
}

static void test_sha256(void)
{
    char *buf_sha256 = "asf";
    unsigned char buf_store[256] = {0};  
    int len = 3;
    void *sha256 = NULL;
    int ret = 0;
    
    sha256 = digest_sha256_init();
    YUNIT_ASSERT(NULL != sha256);

    ret = digest_sha256_update(sha256,buf_sha256,len);
    YUNIT_ASSERT(0 == ret);
   
    ret = digest_sha256(buf_sha256,len,buf_store);
    YUNIT_ASSERT(0 == ret);
    
    ret = digest_sha256_final(sha256,buf_store);
    YUNIT_ASSERT(0 == ret);
}

static void test_sha384(void)
{
    char *buf_sha384 = "asf";
    unsigned char buf_store[256] = {0};  
    int len = 3;
    void *sha384 = NULL;
    int ret = 0;
    
    sha384 = digest_sha384_init();
    YUNIT_ASSERT(NULL != sha384);

    ret = digest_sha384_update(sha384,buf_sha384,len);
    YUNIT_ASSERT(0 == ret);
   
    ret = digest_sha384(buf_sha384,len,buf_store);
    YUNIT_ASSERT(0 == ret);
    
    ret = digest_sha384_final(sha384,buf_store);
    YUNIT_ASSERT(0 == ret);
}

static void test_sha512(void)
{
    char *buf_sha512 = "asf";
    unsigned char buf_store[256] = {0};  
    int len = 3;
    void *sha512 = NULL;
    int ret = 0;
    
    sha512 = digest_sha512_init();
    YUNIT_ASSERT(NULL != sha512);

    ret = digest_sha512_update(sha512,buf_sha512 ,len);
    YUNIT_ASSERT(0 == ret);
   
    ret = digest_sha512(buf_sha512,len,buf_store);
    YUNIT_ASSERT(0 == ret);
    
    ret = digest_sha512_final(sha512,buf_store);
    YUNIT_ASSERT(0 == ret);
}

static void test_crc(void)
{
    int ret = 0;
    char *buf_md5 = "asf";
    int len = 3;
    
    ret = utils_crc16((unsigned char *)buf_md5,len);
    YUNIT_ASSERT(0 != ret);
 
    ret = utils_crc32((unsigned char *)buf_md5,len);
    YUNIT_ASSERT(0 != ret);
}

static int init(void)
{

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

static yunit_test_case_t aos_digest_algorithm_testcases[] = {
    { "test_md5", test_md5},
    { "test_sha256", test_sha256},
    { "test_sha384 ", test_sha384 },
    { "test_sha512 ", test_sha512 },
    { "test_crc", test_crc},
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "hashtable", init, cleanup, setup, teardown, aos_digest_algorithm_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_digest_algorithm(void)
{    
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_digest_algorithm);

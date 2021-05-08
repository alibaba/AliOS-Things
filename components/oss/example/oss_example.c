/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
#include "stdio.h"
#include "oss_app.h"

//从阿里云平台购买OSS服务获取，替换成自己的就可以
char AccessKey_ID[] = "Access Key ID";
char AccessKey_Secret[] = "Access Key Secret";
char Endpoint[] = "End point";
char Bucket[] = "Bucket";

static void example_oss_local_upload(int argc, char **argv)
{
    printf("oss upload begin %s...\r\n",argv[1]);
    if(0 == strcmp(argv[1],"0")){
        oss_upload_local_content(AccessKey_ID, AccessKey_Secret, Endpoint, Bucket, argv[2]);
    }
    else if(0 == strcmp(argv[1],"1")){
        oss_upload_local_file(AccessKey_ID, AccessKey_Secret, Endpoint, Bucket, argv[2]);
    }
    
    printf("oss upload end !!!\r\n");
    return;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(example_oss_local_upload, oss_test, oss upload test example)
#endif

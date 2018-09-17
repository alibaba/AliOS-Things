/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "tsl_format_export.h"

typedef struct {
    char *src;
    char *dst;
    int is_slim;
} env_t;

void usage()
{
    printf("Usage: app [option]\n");
    printf("\t-i(src_path)\n");
    printf("\t-o[dst_path]\n");
    printf("\t-s slim thing mode\n");
}

void print_env(env_t *env)
{
    if (!env) {
        return;
    }

    printf("\nEnv\n");
    printf("=========================================\n");
    printf("SRC:%s\n", env->src ? env->src : "NULL");
    printf("DST:%s\n", env->dst ? env->dst : "NULL");
    printf("%s slim TSL mode\n", env->is_slim == 0 ? "not" : "");
    printf("=========================================\n");
}

void set_default(env_t *env)
{
    if (!env) {
        return;
    }

    if (!env->dst) {
        env->dst = "conv.txt";
    }
}

int main(int argc, char **argv)
{
    int ret = -1;
    env_t env;
    int opt = 0;

    memset(&env, 0, sizeof(env_t));

    while ((opt = getopt(argc, argv, "i:o::s")) != -1) {
        switch (opt) {
            case 'i':
                env.src = optarg;
                break;
            case 'o':
                env.dst = optarg;
                break;
            case 's':
                env.is_slim = 1;
                break;
            default:
                usage();
                return 0;
        }
    }

    if (!env.src) {
        usage();
        return 0;
    }

    set_default(&env);
    print_env(&env);

    if (tsl_format_convert(env.src, env.dst, env.is_slim) != 0) {
        TFormat_printf("[err] convert fail");
        goto do_exit;
    }
    tsl_format_dump(env.dst);
    ret = 0;
do_exit:
    return ret;
}

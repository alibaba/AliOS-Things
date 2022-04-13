/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <procmgmt.h>
#include "k_mk_config.h"

typedef struct {
    char proc_name[PROC_NAME_MAX];
    int  pid;
} proc_map_t;

static proc_map_t g_proc_map[MK_CONFIG_PROCESS_MAX];


int proc_map_init()
{
    memset(g_proc_map, 0, sizeof(g_proc_map));
    return 0;
}

int proc_map_add(const char* proc_name, int pid)
{
    int i;
    for (i = 0; i < MK_CONFIG_PROCESS_MAX; i++) {
        if (g_proc_map[i].pid == 0) {
            g_proc_map[i].pid = pid;
            strncpy(g_proc_map[i].proc_name, proc_name, PROC_NAME_MAX - 1);
            return 0;
        }
    }
    return -1;
}

int proc_map_id_remove(int pid)
{
    int i;
    for (i = 0; i < MK_CONFIG_PROCESS_MAX; i++) {
        if (g_proc_map[i].pid == pid) {
            g_proc_map[i].pid = 0;
            memset(g_proc_map[i].proc_name, 0, PROC_NAME_MAX);
            return 0;
        }
    }
    return -1;
}

int proc_map_name_remove(char* proc_name)
{
    int i;
    for (i = 0; i < MK_CONFIG_PROCESS_MAX; i++) {
        if (strncmp(g_proc_map[i].proc_name, proc_name, PROC_NAME_MAX) == 0) {
            g_proc_map[i].pid = 0;
            memset(g_proc_map[i].proc_name, 0, PROC_NAME_MAX);
            return 0;
        }
    }
    return -1;
}

int proc_map_id_get(char* proc_name)
{
    int i;
    for (i = 0; i < MK_CONFIG_PROCESS_MAX; i++) {
        if (strncmp(g_proc_map[i].proc_name, proc_name, PROC_NAME_MAX) == 0) {
            return g_proc_map[i].pid;
        }
    }
    return -1;
}

int proc_map_name_get(int pid , char buf[])
{
    int i;
    for (i = 0; i < MK_CONFIG_PROCESS_MAX; i++) {
        if (g_proc_map[i].pid == pid) {
            strncpy(buf, g_proc_map[i].proc_name, PROC_NAME_MAX);
            return 0;
        }
    }
    return -1;
}

int proc_is_running(char* proc_name)
{
    int i;
    for (i = 0; i < MK_CONFIG_PROCESS_MAX; i++) {
        if (strncmp(g_proc_map[i].proc_name, proc_name, PROC_NAME_MAX) == 0) {
            return 0;
        }
    }
    return -1;
}

int proc_map_show()
{
    int i;
    printf("----------------------------------------------------------------------\r\n");
    printf("|%-60s|%-20s|\r\n", "process name", "proc id");
    printf("----------------------------------------------------------------------\r\n");
    for (i = 0; i < MK_CONFIG_PROCESS_MAX; i++) {
        if (g_proc_map[i].pid != 0) {
            printf("|%-60s|%-20d|\r\n", g_proc_map[i].proc_name, g_proc_map[i].pid);
        }
    }
    printf("----------------------------------------------------------------------\r\n");
    return 0;
}



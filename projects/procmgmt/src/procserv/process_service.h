/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef  __PROCESS_SERVICE_H__
#define  __PROCESS_SERVICE_H__

#define SYS_PROC "/system/uapp_helloworld1@vexpressa9-mk.app.stripped.elf"

int  serive_proc_server_setup(void);
void serive_proc_cmd_init(void);

int proc_map_init(void);
int proc_map_add(const char* proc_name, int pid);
int proc_map_name_remove(char* proc_name);
int proc_map_id_remove(int pid);
int proc_map_id_get(char* proc_name);
int proc_map_name_get(int pid , char buf[]);
int proc_map_show();

int proc_is_running(char* proc_name);

#endif

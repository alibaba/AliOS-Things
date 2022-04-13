/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef ACTIVATION_H
#define ACTIVATION_H

/* tigger activation report */
int activation_report(void );
int activation_info_set(char *os, char *partner, char* app_net, 
                                char* type, char *project, char* cloud);

#endif


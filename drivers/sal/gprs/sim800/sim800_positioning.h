/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
 
#ifndef _SIM800_POSITIONING_H_
#define _SIM800_POSITIONING_H_

void sim800_get_gprs_network_info(char * bts, int btslen, char * neighterbts, int nbtslen);
void sim800_get_gps(float * latitude, float * longitude, float * altitude);

#endif
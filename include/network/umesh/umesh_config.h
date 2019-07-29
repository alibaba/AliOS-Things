/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UMESH_CONFIG_H
#define UMESH_CONFIG_H

#define MAX_NEIGHBORS_NUM 32

#define ATTACH_REQUEST_RETRY_TIMES 2
#define ATTACH_REQUEST_INTERVAL 1000

#ifdef CONFIG_AOS_MESH_BCAST
#define KEEP_ALIVE_COUNT 3               /* maximum retransmission count of keep alive msg */
#define KEEP_ALIVE_SHORT_TIMEOUT 3000    /* keep alive short interval in case that leader exists */
#define KEEP_ALIVE_LONG_TIMEOUT 10000    /* keep alive long interval during leader recover */

#define LEADER_RECOVER_COUNT 5           /* double check if leader has recovered really*/
#define LEADER_RECOVER_TIMEOUT 2000      /* leader recover timer timeout */
#endif

#ifdef CONFIG_AOS_MESH_LOWPOWER
#define SCHEDULE_SLOTS_SIZE 3
#define SCHEDULE_SLOT_INTERVAL 6000
#endif

#ifdef CONFIG_AOS_MESH_AUTH
#define AUTH_REQUEST_RETRY_TIMES 2
#define AUTH_RELAY_RETRY_TIMES 2

#define ID2_LISTEN_PORT 1234
#define ID2_SERVER_PORT 2345
#define ID2_SERVER_ADDR "192.168.16.188" /* testbed sp server ip */
#endif

#endif

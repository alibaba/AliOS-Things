/*!
 * @filename umesh_config.h
 * This is uMesh config header file
 *
 * @copyright  Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UMESH_CONFIG_H
#define UMESH_CONFIG_H

/** @addtogroup aos_umesh uMesh
 *  AliOS Things mesh networks
 *
 *  @{
 */

#define MAX_NEIGHBORS_NUM 32      /** < max number of neighbors */

#define ATTACH_REQUEST_RETRY_TIMES 2  /** < attach request retry times */
#define ATTACH_REQUEST_INTERVAL 1000  /** < attach request interval */

#ifdef CONFIG_AOS_MESH_BCAST
#define KEEP_ALIVE_COUNT 3               /** < maximum retransmission count of keep alive msg */
#define KEEP_ALIVE_SHORT_TIMEOUT 3000    /** < keep alive short interval in case that leader exists */
#define KEEP_ALIVE_LONG_TIMEOUT 10000    /** < keep alive long interval during leader recover */

#define LEADER_RECOVER_COUNT 5           /** < double check if leader has recovered really*/
#define LEADER_RECOVER_TIMEOUT 2000      /** < leader recover timer timeout */
#endif

#ifdef CONFIG_AOS_MESH_LOWPOWER
#define SCHEDULE_SLOTS_SIZE 3           /** < lowpower's schedule slots size */
#define SCHEDULE_SLOT_INTERVAL 6000     /** < lowpower's schedule slot time interval */
#endif

#ifdef CONFIG_AOS_MESH_AUTH
#define AUTH_REQUEST_RETRY_TIMES 2      /** < auth request retry times */
#define AUTH_RELAY_RETRY_TIMES 2        /** < auth relay retry times */

#define ID2_LISTEN_PORT 1234              /** < ID2 listen port number */
#define ID2_SERVER_PORT 2345              /** < ID2 server port number */
#define ID2_SERVER_ADDR "192.168.16.188"  /** < testbed sp server ip */
#endif

/** @} */

#endif

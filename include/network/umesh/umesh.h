/*!
 * @filename umesh.h
 * This is uMesh APIs header file
 *
 * @copyright  Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UMESH_MESH_H
#define UMESH_MESH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "umesh_config.h"

/** @addtogroup aos_umesh uMesh
 *  AliOS Things mesh networks
 *
 *  @{
 */

/**
 * mesh error number
 */
typedef enum ur_error_s {
    UR_ERROR_NONE          = 0,   /** < success */
    UR_ERROR_FAIL          = 1,   /** < fail */
    UR_ERROR_BUSY          = 2,   /** < error busy */
    UR_ERROR_DROP          = 3,   /** < error drop */
    UR_ERROR_MEM           = 4,   /** < error mem */
    UR_ERROR_ROUTE         = 5,   /** < error route */
    UR_ERROR_PARSE         = 6,   /** < error parse */
    UR_ERROR_ADDRESS_QUERY = 7,   /** < error address query */
    UR_ERROR_BUFFER        = 8,   /** < error buffer */
    UR_ERROR_INVALID_PARM  = 9,   /** < error invalid paramater */
} ur_error_t;

/**
 * node modes
 */
typedef enum node_mode_s {
    MODE_NONE     = 0x00,   /** < this is only for testing */
    MODE_LOW_MASK = 0x0f,   /** < mode low mask */
    MODE_RX_ON    = 0x10,   /** < normal node mode */
    MODE_SUPER    = 0x20,   /** < super node mode */
    MODE_LEADER   = 0x40,   /** < leader node mode */
    MODE_HI_MASK  = 0xf0,   /** < mode high mask */
} node_mode_t;

/**
 * initialize umesh
 *
 * @param[in] mode mesh node mode
 *
 * @return success or not
 */
ur_error_t umesh_init(node_mode_t mode);

/**
 * uMesh start related callback structure
 *
 * @param[in] is_leader_down indicate whether leader is down or not
 */
typedef void (* umesh_status_t)(bool is_leader_down);

/**
 * Start uMesh
 *
 * @param[in] callback uMesh start result callbacks
 *
 * @return success or not
 */
ur_error_t umesh_start(umesh_status_t callback);

/**
 * Check if uMesh is initialized or not
 *
 * @return started or not
 */
bool umesh_is_initialized(void);

/**
 * Stop uMesh
 *
 * @return success or not
 */
ur_error_t umesh_stop(void);

/**
 * Get device mode
 *
 * @return device mode
 */
uint8_t umesh_get_mode(void);

/**
 * Set device mode
 *
 * @param[in] device mode
 *
 * @return success or not
 */
ur_error_t umesh_set_mode(node_mode_t mode);

/**
 * mesh events code
 */
#define CODE_MESH_STARTED        1    /** < mesh started event */
#define CODE_MESH_ATTACHED       2    /** < mesh attached event */
#define CODE_MESH_DETACHED       3    /** < mesh detached event */
#define CODE_MESH_CONNECTED      4    /** < mesh connected event */
#define CODE_MESH_DISCONNECTED   5    /** < mesh disconnected event */
#define CODE_MESH_DATA_RECV      6    /** < mesh data recv event */
#define CODE_MESH_PSCHED_UP      7    /** < mesh lowpower parent schedule up event */
#define CODE_MESH_ASCHED_UP      8    /** < mesh lowpower schedule up event */
#define CODE_MESH_SCHED_DOWN     9    /** < mesh lowpower schedule down event */

/** @} */

#ifdef __cplusplus
}
#endif

#endif  /* UMESH_MESH_H */

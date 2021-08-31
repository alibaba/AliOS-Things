#ifndef L2CM_INT_H_
#define L2CM_INT_H_

/**
 ****************************************************************************************
 * @addtogroup L2C_INT Logical Link Control and Adaptation Protocol internals
 * @ingroup HOST
 * @brief Logical Link Control and Adaptation Protocol - Internal API.
 *
 * @{
 ****************************************************************************************
 */
#include "rwip_config.h"

#if (BLE_L2CM)
#include <stdbool.h>
#include <stdint.h>
#include "compiler.h"
#include "l2cm.h"

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#define TRACE_BLE_TX_BUFFER_USAGE	0

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

/// L2CAP Manager environment structure to be saved
struct l2cm_env_tag
{
    #if (BLE_L2CC)
    /// bit field used to know per connection if TX activity is on-going
    uint32_t    con_tx_state;
    #endif //  (BLE_L2CC)
    /// Lower Layer buffers length
    uint16_t    le_acl_data_pkt_len;
    /// Total number of available Lower Layer buffers
    uint16_t    le_acl_total_nb_acl_pkt;
    /// Number of available Lower Layer buffers
    uint16_t    nb_buffer_avail;

	uint16_t    usedBufferNumber[BLE_CONNECTION_MAX];
	
 };

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */
extern struct l2cm_env_tag l2cm_env;
/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Acquire low layer LE ACL packet
 ****************************************************************************************
 */
static __INLINE void l2cm_buffer_acquire(uint8_t conidx)
{
#if TRACE_BLE_TX_BUFFER_USAGE
	TRACE("pre ble nb -- to %d conidx %d to %d", l2cm_env.nb_buffer_avail,
			conidx, l2cm_env.usedBufferNumber[conidx]);
#endif
    l2cm_env.nb_buffer_avail--;	
	l2cm_env.usedBufferNumber[conidx]++;
#if TRACE_BLE_TX_BUFFER_USAGE	
	TRACE("ble nb -- to %d coidx %d to %d", l2cm_env.nb_buffer_avail,
		conidx, l2cm_env.usedBufferNumber[conidx]);
#endif
}

/**
 ****************************************************************************************
 * @brief Release low layer LE ACL packet
 *
 * @parm[in] nb Number of buffer to release.
 ****************************************************************************************
 */
static __INLINE void l2cm_buffer_release(uint8_t conidx, uint16_t nb)
{
#if TRACE_BLE_TX_BUFFER_USAGE    
	TRACE("pre ble nb ++ to %d conidx %d to %d", l2cm_env.nb_buffer_avail,
			conidx, l2cm_env.usedBufferNumber[conidx]);
#endif
	if (l2cm_env.usedBufferNumber[conidx] < nb)
	{
		l2cm_env.nb_buffer_avail += l2cm_env.usedBufferNumber[conidx];
		l2cm_env.usedBufferNumber[conidx] = 0;
	}
	else
	{
		l2cm_env.nb_buffer_avail += nb;
		l2cm_env.usedBufferNumber[conidx] -= nb;
	}
#if TRACE_BLE_TX_BUFFER_USAGE	
	TRACE("post ble nb ++ to %d conidx %d to %d", l2cm_env.nb_buffer_avail,
		conidx, l2cm_env.usedBufferNumber[conidx]);
#endif
}

static __INLINE void l2cm_buffer_reset(uint8_t conidx)
{
#if TRACE_BLE_TX_BUFFER_USAGE
	TRACE("pre ble reset to %d conidx %d to %d", l2cm_env.nb_buffer_avail,
			conidx, l2cm_env.usedBufferNumber[conidx]);
#endif

	l2cm_env.nb_buffer_avail += l2cm_env.usedBufferNumber[conidx];
	l2cm_env.usedBufferNumber[conidx] = 0;

#if TRACE_BLE_TX_BUFFER_USAGE	
	TRACE("post ble reset to %d conidx %d to %d", l2cm_env.nb_buffer_avail,
		conidx, l2cm_env.usedBufferNumber[conidx]);	
#endif
}

/**
 ****************************************************************************************
 * @brief Retrieve low layer LE ACL Buffers size
 *
 * @return size of LE ACL Buffers
 ****************************************************************************************
 */
static __INLINE uint16_t l2cm_get_buffer_size(void)
 {
     return l2cm_env.le_acl_data_pkt_len;
 }


/**
 ****************************************************************************************
 * @brief Set if TX queue for specific connection is empty or not.
 *
 * If not empty, it will request L2CM to start transmission of buffers
 *
 * @param[in] conidx           Connection index
 * @param[in] busy             False if TX buffer queue is empty, True else
 *
 ****************************************************************************************
 */
void l2cm_tx_status(uint8_t conidx, bool busy);

/// @} L2CM_INT

#endif //(BLE_L2CM)

#endif // L2CM_INT_H_

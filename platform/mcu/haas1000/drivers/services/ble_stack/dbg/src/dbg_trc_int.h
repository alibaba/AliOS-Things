#ifndef DBG_TRC_INT_H_
#define DBG_TRC_INT_H_

/**
 ****************************************************************************************
 * @addtogroup TRACER
 * @{
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (TRACER_PRESENT)
#include <stdint.h>         // standard definitions
#include <stdbool.h>        // boolean

/*
 * DEFINES
 ****************************************************************************************
 */

///Channel index length
#define CHANNEL_ID_LEN          1

///lengths of trace packet fields
#define SEQ_NUM_LEN             2
#define TIMESTAMP_LEN           4
#define TRC_CODE_LEN            1

#define TRC_FIX_LEN             \
        CHANNEL_ID_LEN          +\
        TRC_MSG_HDR_LEN         +\
        SEQ_NUM_LEN             +\
        TIMESTAMP_LEN           +\
        TRC_CODE_LEN

/**
 ****************************************************************************************
 * @brief Convenient wrapper to trc_mem_alloc()
 *
 * This macro calls trc_mem_alloc() passing as parameter the length of the trace packet
 *
 * @param[in] trace_pay        Trace payload length
 *
 * @return Pointer to trace code field(or NULL if the trace cannot be written)
 ****************************************************************************************
 */
#define TRC_MEM_ALLOC(trace_pay) \
        dbg_trc_mem_alloc(TRC_FIX_LEN + trace_pay)

typedef uint8_t trc_id_t;
typedef uint8_t trc_opcode_t;

/*
 * STRUCTURES DEFINITIONS
 ****************************************************************************************
 */
///Tracer Environment context structure
struct dbg_trc_env_tag
{
    /// Current tracer configuration word
    uint32_t curr_cw;

    /// Compiled tracer configuration word
    uint32_t compiled_cw;
};

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
///Tracer environment context
extern struct dbg_trc_env_tag dbg_trc_env;

/*
 * TRANSPORT LAYER FUNCTION DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief   initialize tracer TL
 ****************************************************************************************
 */
void dbg_trc_tl_init();

/**
 ****************************************************************************************
 * @brief   trigger the transmission of tracer packets
 ****************************************************************************************
 */
void dbg_trc_tx_trigger(void);

/*
 * MEMORY FUNCTION DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief   Initialize tracer memory
 ****************************************************************************************
 */
void dbg_trc_mem_init();

/**
 ****************************************************************************************
 * @brief   Try to write a trace in memory.
 * @param[in]  trace_len    Trace packet length (expressed in bytes)
 *
 * @return  Pointer to trace code field(or NULL if the trace cannot be written)
 ****************************************************************************************
 */
uint8_t *dbg_trc_mem_alloc(uint16_t const trace_len);

/**
 ****************************************************************************************
 * @brief   Try to read a trace from the memory.
 *
 * @return Pointer to the total size of the trace (or NULL if the trace cannot be read)
 ****************************************************************************************
 */
uint8_t *dbg_trc_mem_read();

/**
 ****************************************************************************************
 * @brief   Deallocate a trace from the memory.
 *
 * This function marks the trace block pointed by the reading pointer as invalid and moves
 * it to the next trace block
 *
 ****************************************************************************************
 */
void dbg_trc_mem_dealloc();

/**
 ****************************************************************************************
 * @brief Initialization of the tracer
 *
 * This function initializes the tracer
 *
 ****************************************************************************************
 */
void dbg_trc_init(bool reset);

#endif /* TRACER_PRESENT */
/// @} TRACER
#endif /* DBG_TRC_INT_H_ */

#ifndef _MB_H
#define _MB_H

#include "../port/port.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "mbproto.h"
#include "mbport.h"

typedef enum
{
    STATE_IDLE,

    STATE_TX_READY,              /*!< Transmitter is in idle state. */
    STATE_TX_XMIT,               /*!< Transmitter is in transfer state. */

    STATE_WAIT_RESPONSE,

    STATE_RX_RCV,               /*!< Frame is beeing received. */
    STATE_RX_ERROR,              /*!< If the frame is invalid. */

    STATE_END,
} mb_run_state_t;

mb_error_code_t mb_init(mb_mode_t mode, uint8_t port, uint32_t baud_rate, mb_parity_t parity);
mb_error_code_t mb_tcp_init(uint16_t usTCPPort);
mb_error_code_t mb_close(void);
mb_error_code_t mb_enable(void);
mb_error_code_t mb_disable(void);
mb_error_code_t mb_poll(void);

void            mb_set_cur_slave_address(uint8_t slave_address);
mb_run_state_t  mb_get_run_state();
void            mb_set_run_state(mb_run_state_t state);
bool            mb_run_state_is_idle();

#ifdef __cplusplus
}
#endif
#endif

#include "include.h"
#include "command_table.h"

#include "entry_declare.h"
#include "cmd_help.h"
#include "cmd_evm.h"
#include "cmd_rx_sensitivity.h"
#include "cmd_reg.h"

#if CFG_UART_DEBUG
cmd_tbl_t command_tbl[] =
{
    /* add new entry*/
    ENTRY_CMD_EVM,
    ENTRY_CMD_RX_SENSITIVITY,
    ENTRY_CMD_HELP,
    ENTRY_CMD_REG,

    /* last null entry*/
    {NULL,  0, 0, NULLPTR, NULLPTR}
};

cmd_tbl_t *entry_get_start(void)
{
    return &command_tbl[0];
}

int entry_get_count(void)
{
    return sizeof(command_tbl) / sizeof(command_tbl[0]);
}
#endif // CFG_UART_DEBUG

// eof


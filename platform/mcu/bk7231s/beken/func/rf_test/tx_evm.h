#ifndef _TX_EVM_H_
#define _TX_EVM_H_

#define EVM_DEBUG

#ifdef EVM_DEBUG
#define EVM_PRT       os_printf
#define EVM_WPRT      warning_prf
#else
#define EVM_PRT       os_null_printf
#define EVM_WPRT      os_null_printf
#endif

#define EVM_DEFAULT_CHANNEL         2437

/*******************************************************************************
* Function Declarations
*******************************************************************************/
void evm_bypass_mac(void);
uint32_t evm_via_mac_is_start(void);

#endif // _TX_EVM_H_
// eof


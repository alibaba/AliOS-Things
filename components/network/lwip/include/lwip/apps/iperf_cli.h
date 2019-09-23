
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *               Function Declarations
 ******************************************************/

/**
  * @brief  Add iperf command lines to MiCO CLI.
  * @param  none.
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
int iperf_cli_register( void );

#ifdef __cplusplus
} /*extern "C" */
#endif

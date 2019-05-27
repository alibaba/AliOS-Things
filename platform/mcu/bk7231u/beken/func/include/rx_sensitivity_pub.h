#ifndef _RX_SENSITIVITY_H_
#define _RX_SENSITIVITY_H_

/*******************************************************************************
* Function Declarations
*******************************************************************************/
extern void rs_test(void);
extern void rx_get_rx_result_begin(void);
extern void rx_get_rx_result_end(void);
extern UINT32 rs_set_channel(UINT32 channel_id);
extern UINT32 rs_set_mode(UINT32 mode);

extern void rx_clean_rx_statistic_result(void);
extern UINT32 rx_get_rx20M_statistic_result(void);
extern UINT32 rx_get_rx40M_statistic_result(void);

#endif //_RX_SENSITIVITY_H_
// eof


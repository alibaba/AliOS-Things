#ifndef _SDMA_PUB_H_
#define _SDMA_PUB_H_

#define SDMA_INTERACT_WITH_HOST

typedef void (*TX_FUNC)(void);
typedef void (*CMD_FUNC)(void *buf, UINT32 len);
typedef void (*RX_FUNC)(UINT32 count);

/*******************************************************************************
* Function Declarations
*******************************************************************************/
extern void sdma_init(void);

extern void sdma_open(void);

extern void sdma_uninit(void);

extern void sdma_close(void);

#ifdef SDMA_INTERACT_WITH_HOST
extern UINT32 sdma_get_blk_len(void);
extern void sdma_set_tx_valid(void);
extern void sdma_clr_tx_valid(void);
extern void sdma_set_tx_dat_count(UINT32 val);
#endif // SDMA_INTERACT_WITH_HOST

extern void sdma_register_handler(TX_FUNC tx_callback,
                                  RX_FUNC rx_callback,
                                  CMD_FUNC cmd_callback);

extern UINT32 sdma_start_rx(UINT8 *buf, UINT32 len);
extern UINT32 sdma_start_tx(UINT8 *buf, UINT32 len);

extern UINT32 sdma_start_cmd(UINT8 *cmd, UINT32 len);
extern void sdma_fake_stop_dma(void);
extern void sdma_isr(void);
#endif // _SDMA_PUB_H_

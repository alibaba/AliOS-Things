#ifndef _RW_MSG_RX_H_
#define _RW_MSG_RX_H_

extern void mr_kmsg_init(void);
extern void rwnx_recv_msg(void);
extern void mhdr_assoc_cfm_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt);
extern void mhdr_scanu_reg_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt);
extern void mhdr_deauth_evt_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt);
extern void mhdr_deassoc_evt_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt);

#endif // _RW_MSG_RX_H_
// eof


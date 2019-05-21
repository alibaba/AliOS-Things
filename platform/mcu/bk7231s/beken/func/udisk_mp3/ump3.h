#ifndef _U_MP3_H_
#define _U_MP3_H_

#define UM_DEBUG
#ifdef UM_DEBUG
#define UM_PRT       os_printf
#define UM_WPRT      warning_prf
#else
#define UM_PRT       os_null_printf
#define UM_WPRT      os_null_printf
#endif

extern uint32_t um_work_init(void);
extern void um_connect_cb(void);

#endif // _U_MP3_H_
// eof

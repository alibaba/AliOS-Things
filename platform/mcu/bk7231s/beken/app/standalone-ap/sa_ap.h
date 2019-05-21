#ifndef _SA_AP_H_
#define _SA_AP_H_

#define SAAP_DEBUG

#ifdef SAAP_DEBUG
#define SAAP_PRT                 os_printf
#define SAAP_WPRT                warning_prf
#else
#define SAAP_PRT                 os_null_printf
#define SAAP_WPRT                os_null_printf
#endif

extern void sa_ap_init(void);
#endif // _SA_AP_H_
// eof


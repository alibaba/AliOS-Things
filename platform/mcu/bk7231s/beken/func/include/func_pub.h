#ifndef _FUNC_PUB_H_
#define _FUNC_PUB_H_

#define FUNC_DEBUG
#ifdef FUNC_DEBUG
#define FUNC_PRT                 os_printf
#define FUNC_WPRT                warning_prf
#else
#define FUNC_PRT                 os_null_printf
#define FUNC_WPRT                os_null_printf
#endif

extern UINT32 func_init(void);

#endif // _FUNC_PUB_H_
// eof


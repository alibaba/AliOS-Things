#include "tx_swdesc.h"

#if  CFG_SDIO_TRANS

#define SDIO_TRANS_DEBUG

#ifdef SDIO_TRANS_DEBUG
#define SDIO_TRANS_PRT       os_printf
#define SDIO_TRANS_WPRT      warning_prf
#else
#define SDIO_TRANS_PRT       os_null_printf
#define SDIO_TRANS_WPRT      warning_prf
#endif

#define SDIO_TRANS_FAILURE        ((UINT32)-1)
#define SDIO_TRANS_SUCCESS        (0)

typedef struct _stm32_frame_hdr
{
    UINT16 len;
#if FOR_SDIO_BLK_512
    UINT8 type;
    UINT8 seq;
#else
    UINT16 type;
#endif
} STM32_FRAME_HDR;

#endif

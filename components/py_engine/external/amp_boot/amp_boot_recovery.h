/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef AMP_RECOVERY_H
#define AMP_RECOVERY_H

#include <stddef.h>
#include <stdint.h>


/* recovery switch & status led. default for esp32 */
#ifndef AMP_REC_UART
#define AMP_REC_UART 2
#endif

#ifndef AMP_STATUS_IO
#define AMP_STATUS_IO 33
#endif

#ifndef AMP_STATUS_IO_ON
#define AMP_STATUS_IO_ON 1
#endif

/* recovery switch */
#ifndef AMP_REC_IO
#define AMP_REC_IO 11
#endif

#ifndef AMP_REC_IO_ON
#define AMP_REC_IO_ON 1
#endif

typedef enum{
    AMP_STATUS_NORMAL = 0,
    AMP_STATUS_SERVICE_AVAILABLE, // 1
    AMP_STATUS_RECOVERY,		// 2
    AMP_STATUS_UPDATING,	// 3
    AMP_STATUS_JSERROR,	// 4
    AMP_STATUS_COREDUMP,	// 5
    AMP_STATUS_END
}AMP_STATUS;

int pyamp_recovery_init(void);

int pyamp_recovery_entry(void);

void pyamp_status_set(AMP_STATUS status);

void uart_send_byte(unsigned char c);

unsigned char uart_recv_byte(unsigned char *c);

int pyamp_recovery_appbin();

#endif /* AMP_RECOVERY_H */
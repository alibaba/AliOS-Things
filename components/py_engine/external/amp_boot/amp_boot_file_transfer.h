/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef AMP_FILE_TRANSFER_H
#define AMP_FILE_TRANSFER_H

#include <stddef.h>
#include <stdint.h>


typedef enum{
    AMP_FILE_TRANSFER_STATUS_NORMAL = 0,
    AMP_FILE_TRANSFER_STATUS_SERVICE_AVAILABLE, // 1
    AMP_FILE_TRANSFER_STATUS_RECOVERY,		// 2
    AMP_FILE_TRANSFER_STATUS_UPDATING,	// 3
    AMP_FILE_TRANSFER_STATUS_JSERROR,	// 4
    AMP_FILE_TRANSFER_STATUS_COREDUMP,	// 5
    AMP_FILE_TRANSFER_STATUS_INSTALL,	// 6
    AMP_FILE_TRANSFER_STATUS_END
}AMP_FILE_TRANSFER_STATUS;

typedef enum{
    YMODEM_RECEVIE_HEADER = 0,
    YMODEM_RECEVIE_FILE_SIZE,
    YMODEM_RECEVIE_BODY,
    YMODEM_RECEVIE_END,
    YMODEM_RECEVIE_ERROR
}AMP_FILE_RECEVIE_FLAG;

/* complete = 0 success, 1 check success(EOF),  -1 check failed */
// save file callback function
void amp_file_transfer_status_set(AMP_FILE_TRANSFER_STATUS status);

void pyamp_ymodem_recovery_file_transfer_init();

#endif /* AMP_FILE_TRANSFER_H */

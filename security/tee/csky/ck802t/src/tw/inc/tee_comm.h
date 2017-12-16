/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef _TEE_COMM_H_
#define _TEE_COMM_H_

#ifndef __ASSEMBLY__
#include "tee_types.h"
#endif
/* This may be included by assembly */

/* fall in Trusted OS Calls */
#define _NTW_SMC64_CALLING      (0)
#define NTW_NEW_RQST        (0x32000000 | ((_NTW_SMC64_CALLING) << 31))
#define NTW_NORM_CALL       (0x33000000 | ((_NTW_SMC64_CALLING) << 31))
#define NTW_LPM_RQST        (0x32000010 | ((_NTW_SMC64_CALLING) << 31))

#define _TW_SMC64_CALLING      (0)
#define TW_NEW_RQST         (0x34000000 | ((_TW_SMC64_CALLING) << 31))
#define TW_NORM_CALL        (0x35000000 | ((_TW_SMC64_CALLING) << 31))

#define COMM_MSG_RQST        (0x54535152) /* ASCII: RQST */
#define COMM_MSG_DONE        (0x454E4F44) /* ASCII: DONE */

#ifndef __ASSEMBLY__
extern int32_t teec_comm_send(uint32_t smcid, uint32_t subcmd, const void *msg);

#endif

#endif /* _TEE_COMM_H_ */

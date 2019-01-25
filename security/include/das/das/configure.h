/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __DAS_CONFIGURE_H__
#define __DAS_CONFIGURE_H__

/* DAS heartbeat period in ms */
#define DAS_HEARTBEAT_PERIOD        60000

/* Maximum topic length */
#define DAS_TOPIC_LENGTH            64

/* Message buffer length */
#define DAS_BUFFER_LENGTH           1024

#define DAS_MAX_SERVICE_NUMBER      8

/* servcie features */
#define DAS_SERVICE_ROM_ENABLED     1

#ifdef DAS_SERVICE_ROM_ENABLED
#define DAS_SERVICE_ROM_STEP_SIZE   8192
#endif

#endif /* __DAS_CONFIGURE_H__ */

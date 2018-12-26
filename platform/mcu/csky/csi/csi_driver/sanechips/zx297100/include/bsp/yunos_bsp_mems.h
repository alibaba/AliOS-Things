/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef YUNOS_BSP_MEMS_H
#define YUNOS_BSP_MEMS_H

#include <stdint.h>

#define YUNOS_BSP_API_MEMS_OPEN     0x0100
#define YUNOS_BSP_API_MEMS_CLOSE    0x0200
#define YUNOS_BSP_API_MEMS_READ     0x0300
#define YUNOS_BSP_API_MEMS_WRITE    0x0400

#define MEMS_PARAM_LEN              11
#define MEMS_PARAM_NUM              3
#define MEMS_PACK_HEAD              0x55
#define MEMS_ACCESPD_ID             0x51
#define MEMS_ANGUSPD_ID             0x52
#define MEMS_ANGLE_ID               0x53

/**
 * Open mems dev
 * @return      BSP_OK, success; YUNOS_BSP_ERRNO_MEMS_BASE | YUNOS_BSP_API_MEMS_OPEN | ENODEV, fail
 */
int yunos_bsp_mems_open(void);
/**
 * Close mems dev
 * @return      BSP_OK
 */
int yunos_bsp_mems_close(void);
/**
 * Read data from mems
 * @param[in]       desbuf      destinate data buffer
 * @param[out]      srcbuf      source data buffer
 * @param[in]       len         data length value
 * @return          BSP_OK, success; YUNOS_BSP_ERRNO_MEMS_BASE | YUNOS_BSP_API_MEMS_WRITE | EINVAL\
                                    YUNOS_BSP_ERRNO_MEMS_BASE | YUNOS_BSP_API_MEMS_READ | EIO, fail
 */
int yunos_bsp_mems_read(char * desbuf, char * srcbuf, int len);
/**
 * Write data into mems
 * @param[in]       desbuf      destinate data buffer
 * @param[in]       srcbuf      source data buffer
 * @param[in]       len         data length value
 * @return          0, success;-1, fail
 */
int yunos_bsp_mems_write(char * desbuf, char * srcbuf, int len);

#endif /* YUNOS_BSP_MEMS_H */

/**
 * @file canopen_od.h
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef CANOPEN_OD__H_
#define CANOPEN_OD__H_

/** @addtogroup aos_canopen canopen
 *  Optional policy of canopen.
 *
 *  @{
 */

#define CO_SW_VERSION_INDEX          0x100a
#define CO_SW_VERSION_SUB_INDEX      0x0

#define CO_HEART_BEAT_CYCLE_INDEX    0x100e
#define CO_HEART_BEAT_CYCLE_SUBINDEX 0x0

#define CO_UPGRADE_INDEX             0x3000
#define CO_UPGRADE_SUB_INDEX         0x0

#define CO_SECURITY_KEY2_INDEX     0x2FFF
#define CO_SECURITY_KEY2_SUB_INDEX 0x0

/** @} */
#endif /* CANOPEN_OD__H_ */

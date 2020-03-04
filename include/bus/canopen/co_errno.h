/**
 * @file canopen_od.h
 * @copyright Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef CO_ERRNO__H_
#define CO_ERRNO__H_

/** @addtogroup aos_canopen canopen
 *  Optional policy of canopen.
 *
 *  @{
 */
typedef enum {
    sdo_transfer_ok             = 0,
    sdo_transfer_busy           = -1,
    sdo_transfer_toggle         = -2,
    sdo_transfer_system_err     = -3,
    sdo_transfer_standout       = -5,
    sdo_transfer_can_err        = -6,
    sdo_transfer_invalid_para   = -7,
    sdo_transfer_no_enough_room = -8,
    sdo_transter_abort_remote   = -9,
    sdo_transter_illegal_arg    = -10,
} sdo_transfer_response_t;

/* error class and error code on sdo access according to CANopen Protocol */
typedef enum {
    NO_ERR          = 0,
    OTHER           = 0x0800,
    PARAM_INCONS    = 0x0503,
    PARAM_ILLEGAL   = 0x0504,
    ACCESS_UNSUPP   = 0x0601,
    OBJ_NOT_EXIST   = 0x0602,
    INVALID_ADDRESS = 0x0604,
    HARDW_FAULT     = 0x0606,
    TYPE_CONFLICT   = 0x0607,
    ATTR_INCONS     = 0x0609
} SDO_ABORT_ERR_T;

/* additional code on sdo access according to CANopen Protocol */
typedef enum {
    NO_DETAILS       = 0x0000,
    INVALID_VALUE    = 0x0010,
    NO_SUBINDEX      = 0x0011,
    LEN_TOHIGH       = 0x0012,
    LEN_TOLOW        = 0x0013,
    CANNOT_EXEC      = 0x0020,
    LOCAL_CONTROL    = 0x0021,
    ACTUAL_STATE     = 0x0022,
    NO_DICTIONARY    = 0x0023,
    RNG_EXCEEDED     = 0x0030,
    VAL_TOLHIGH      = 0x0031,
    VAL_TOLOW        = 0x0032,
    SUBRNG_EXCEEDED  = 0x0033,
    SUBVAL_TOLOW     = 0x0034,
    SUBVAL_TOHIGH    = 0x0035,
    MAX_LESS_MIN     = 0x0036,
    INCOMPAT         = 0x0040,
    CANNOT_MAP       = 0x0041,
    PDOLENGTH_EXCEED = 0x0042,
    PARA_INCOMPAT    = 0x0043,
    INTERN_INCOMPAT  = 0x0047
} SDO_ADDITIONAL_CODE_T;

typedef enum {
    EMERGENCY_CODE_ERR_RESET   = 0x0000, /* Error Reset or NO error */
    EMERGENCY_CODE_GENERIC     = 0x1000, /* Generic Error */
    EMERGENCY_CODE_CURRENT     = 0x2000, /* Current Error */
    EMERGENCY_CODE_DEV_SW      = 0x6000,
    EMERGENCY_CODE_DEV_SW_IN   = 0x6100,
    EMERGENCY_CODE_DEV_SW_USER = 0x6200,
    EMERGENCY_CODE_DEV_DATA    = 0x6300,
    EMERGENCY_CODE_PROTOCOL    = 0x8200,
} EMERGENCY_CODE_T;

/** @} */
#endif /* CO_ERRNO__H_ */

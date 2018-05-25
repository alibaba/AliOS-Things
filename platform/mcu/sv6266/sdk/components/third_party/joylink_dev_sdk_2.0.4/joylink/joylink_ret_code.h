#ifndef _JOYLINK_RET_CODE_H_
#define _JOYLINK_RET_CODE_H_

#include <stdio.h>

typedef enum _bool_{
    E_RET_FAIL = 0,
    E_RET_TRUE = 1
}E_JLBOOL_t;

typedef enum _ret_code{
    E_RET_ERROR_PKG_SAME            = -1001,
    E_RET_ERROR_PKG_NUM_BREAK_MAX   = -1002,
    E_RET_ERROR_PKG_BREAK_ARRAY     = -1003,
    E_RET_ERROR_PARAM_INVAID        = -3,
    E_RET_ERROR_NO_EXIST            = -2,
    E_RET_ERROR                     = -1,
    E_RET_OK                        = 0,
    E_RET_ALL_DATA_HIT              = 1001,
    E_RET_DATA_COMING               = 1002
}E_JLRetCode_t;

#endif

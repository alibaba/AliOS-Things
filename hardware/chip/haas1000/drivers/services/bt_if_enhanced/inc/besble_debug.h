#ifndef __BESBLE_DEBUG_H
#define __BESBLE_DEBUG_H

#include "hal_uart.h"

const char *DebugMask2Prefix(uint8_t mask);

#define DEBUG_PREFIX_FORMAT         "%s<%s> "
#define DEBUG_SUFFIX_FORMAT         "\n"

#define LOG_HCI_TAG  "[BLE HCI]: "
#define LOG_L2C_TAG  "[BLE L2C]: "
#define LOG_ATT_TAG  "[BLE ATT]: "
#define LOG_GATT_TAG "[BLE GATT]: "
#define LOG_SMP_TAG  "[BLE SMP]: "

#define LOG_GAP_TAG "[BLE GAP]: "
#define LOG_APP_TAG "[BLE APP]: "
#define LOG_BLE_TAG "[BLE]: "
#define LOG_HTP_TAG "[BLE HTP]: "

#define GAP_ERROR   1
#define GAP_OUT     2
#define GATT_ERROR  3
#define GATT_OUT    4
#define ATT_ERROR   5
#define ATT_OUT     6
#define L2C_ERROR   7
#define L2C_OUT     8
#define HCI_ERROR   9
#define HCI_OUT     10
#define SMP_ERROR   11
#define SMP_OUT     12
#define APP_ERROR   13
#define APP_OUT     14
/* Profile macro start */
#define PRF_HT_ERROR    15
#define PRF_HT_OUT      16
/* Profile macro end */
#define BLE_ERROR       30
#define BLE_OUT         31


#define DebugOut(mask, str,...)     do \
                                        { \
                                            const char *prefix = NULL; \
                                            prefix = DebugMask2Prefix(mask); \
                                            TRACE(DEBUG_PREFIX_FORMAT, prefix, __FUNCTION__); \
                                            TRACE (str, ##__VA_ARGS__); \
                                        } \
                                        while (0)

#if 0
#define DEBUG_HCI_DBG  	1
#define DEBUG_GAP_DBG  	0
#define DEBUG_GATT_DBG 	0
#define DEBUG_ATT_DBG  	0
#define DEBUG_L2C_DBG  	0
#define DEBUG_SMP_DBG  	0
#define DEBUG_APP_DBG  	0
#define DEBUG_PRF_DBG  	0
#define DEBUG_BLE_DBG  	0

#else
/* ble debug macro switch */
#define DEBUG_HCI_DBG  	0
#define DEBUG_GAP_DBG  	0
#define DEBUG_GATT_DBG 	0
#define DEBUG_ATT_DBG  	0
#define DEBUG_L2C_DBG  	0
#define DEBUG_SMP_DBG  	0
#define DEBUG_APP_DBG  	0
#define DEBUG_PRF_DBG  	0
#define DEBUG_BLE_DBG  	0
#endif

#if DEBUG_HCI_DBG                                        
#define BLE_HCI_DBG(str,...)     DebugOut(HCI_OUT, str, ##__VA_ARGS__)
#define BLE_HCI_ERR(str,...)     DebugOut(HCI_ERROR, str, ##__VA_ARGS__)
#define BLE_HCI_FUNC_ENTER()     TRACE(LOG_BLE_TAG"%s line: %d +++\n", __FUNCTION__, __LINE__)
#define BLE_HCI_FUNC_LEAVE()     TRACE(LOG_BLE_TAG"%s line: %d ---\n", __FUNCTION__, __LINE__)
#else
#define BLE_HCI_DBG(str,...)
#define BLE_HCI_ERR(str,...)     DebugOut(HCI_ERROR, str, ##__VA_ARGS__)
#define BLE_HCI_FUNC_ENTER()
#define BLE_HCI_FUNC_LEAVE()
#endif

#if DEBUG_GAP_DBG
#define BLE_GAP_DBG(str,...)     DebugOut(GAP_OUT, str, ##__VA_ARGS__)
#define BLE_GAP_ERR(str,...)     DebugOut(GAP_ERROR, str, ##__VA_ARGS__)
#define BLE_GAP_FUNC_ENTER()     TRACE(LOG_GAP_TAG"%s line: %d +++\n", __FUNCTION__, __LINE__)
#define BLE_GAP_FUNC_LEAVE()     TRACE(LOG_GAP_TAG"%s line: %d ---\n", __FUNCTION__, __LINE__)
#else
#define BLE_GAP_DBG(str,...)
#define BLE_GAP_ERR(str,...)     DebugOut(GAP_ERROR, str, ##__VA_ARGS__)
#define BLE_GAP_FUNC_ENTER()
#define BLE_GAP_FUNC_LEAVE()
#endif

#if DEBUG_GATT_DBG
#define BLE_GATT_DBG(str,...)    DebugOut(GATT_OUT, str, ##__VA_ARGS__)
#define BLE_GATT_ERR(str,...)    DebugOut(GATT_ERROR, str, ##__VA_ARGS__)
#define BLE_GATT_FUNC_ENTER()    TRACE(LOG_GATT_TAG"%s line: %d +++\n", __FUNCTION__, __LINE__)
#define BLE_GATT_FUNC_LEAVE()    TRACE(LOG_GATT_TAG"%s line: %d ---\n", __FUNCTION__, __LINE__)
#else
#define BLE_GATT_DBG(str,...)
#define BLE_GATT_ERR(str,...)    DebugOut(GATT_ERROR, str, ##__VA_ARGS__)
#define BLE_GATT_FUNC_ENTER()
#define BLE_GATT_FUNC_LEAVE()
#endif

#if DEBUG_ATT_DBG
#define BLE_ATT_DBG(str,...)     DebugOut(ATT_OUT, str, ##__VA_ARGS__)
#define BLE_ATT_ERR(str,...)     DebugOut(ATT_ERROR, str, ##__VA_ARGS__)
#define BLE_ATT_FUNC_ENTER()     TRACE(LOG_ATT_TAG"%s line: %d +++\n", __FUNCTION__, __LINE__)
#define BLE_ATT_FUNC_LEAVE()     TRACE(LOG_ATT_TAG"%s line: %d ---\n", __FUNCTION__, __LINE__)
#else
#define BLE_ATT_DBG(str,...)
#define BLE_ATT_ERR(str,...)     DebugOut(ATT_ERROR, str, ##__VA_ARGS__)
#define BLE_ATT_FUNC_ENTER()
#define BLE_ATT_FUNC_LEAVE()
#endif

#if DEBUG_L2C_DBG
#define BLE_L2C_DBG(str,...)     DebugOut(L2C_OUT, str, ##__VA_ARGS__)
#define BLE_L2C_ERR(str,...)     DebugOut(L2C_ERROR, str, ##__VA_ARGS__)
#define BLE_L2C_FUNC_ENTER()     TRACE(LOG_L2C_TAG"%s line: %d +++\n", __FUNCTION__, __LINE__)
#define BLE_L2C_FUNC_LEAVE()     TRACE(LOG_L2C_TAG"%s line: %d ---\n", __FUNCTION__, __LINE__)
#else
#define BLE_L2C_DBG(str,...)
#define BLE_L2C_ERR(str,...)     DebugOut(L2C_ERROR, str, ##__VA_ARGS__)
#define BLE_L2C_FUNC_ENTER()
#define BLE_L2C_FUNC_LEAVE()
#endif

#if DEBUG_L2C_DBG
#define BLE_SMP_DBG(str,...)     DebugOut(SMP_OUT, str, ##__VA_ARGS__)
#define BLE_SMP_ERR(str,...)     DebugOut(SMP_ERROR, str, ##__VA_ARGS__)
#define BLE_SMP_FUNC_ENTER()     TRACE(LOG_SMP_TAG"%s line: %d +++\n", __FUNCTION__, __LINE__)
#define BLE_SMP_FUNC_LEAVE()     TRACE(LOG_SMP_TAG"%s line: %d ---\n", __FUNCTION__, __LINE__)
#else
#define BLE_SMP_DBG(str,...) 
#define BLE_SMP_ERR(str,...)     DebugOut(SMP_ERROR, str, ##__VA_ARGS__)
#define BLE_SMP_FUNC_ENTER()
#define BLE_SMP_FUNC_LEAVE()
#endif

#if DEBUG_APP_DBG
#define BLE_APP_DBG(str,...)     DebugOut(APP_OUT, str, ##__VA_ARGS__)
#define BLE_APP_ERR(str,...)     DebugOut(APP_ERROR, str, ##__VA_ARGS__)
#define BLE_APP_FUNC_ENTER()     TRACE(LOG_APP_TAG"%s line: %d +++\n", __FUNCTION__, __LINE__)
#define BLE_APP_FUNC_LEAVE()     TRACE(LOG_APP_TAG"%s line: %d ---\n", __FUNCTION__, __LINE__)
#else
#define BLE_APP_DBG(str,...)
#define BLE_APP_ERR(str,...)     DebugOut(APP_ERROR, str, ##__VA_ARGS__)
#define BLE_APP_FUNC_ENTER()
#define BLE_APP_FUNC_LEAVE()
#endif

#if DEBUG_PRF_DBG
#define BLE_PRF_HP_DBG(str,...)  DebugOut(PRF_HT_OUT, str, ##__VA_ARGS__)
#define BLE_PRF_HP_ERR(str,...)  DebugOut(PRF_HT_ERROR, str, ##__VA_ARGS__)
#define BLE_PRF_HP_FUNC_ENTER()  TRACE(LOG_HTP_TAG"%s line: %d +++\n", __FUNCTION__, __LINE__)
#define BLE_PRF_HP_FUNC_LEAVE()  TRACE(LOG_HTP_TAG"%s line: %d ---\n", __FUNCTION__, __LINE__)
#else
#define BLE_PRF_HP_DBG(str,...)
#define BLE_PRF_HP_ERR(str,...)  DebugOut(PRF_HT_ERROR, str, ##__VA_ARGS__)
#define BLE_PRF_HP_FUNC_ENTER()
#define BLE_PRF_HP_FUNC_LEAVE()
#endif

#if DEBUG_BLE_DBG
#define BLE_DBG(str,...)         DebugOut(BLE_OUT, str, ##__VA_ARGS__)
#define BLE_ERR(str,...)         DebugOut(BLE_ERROR, str, ##__VA_ARGS__)
#define BLE_FUNC_ENTER()         TRACE(LOG_BLE_TAG"%s line: %d +++\n", __FUNCTION__, __LINE__)
#define BLE_FUNC_LEAVE()         TRACE(LOG_BLE_TAG"%s line: %d ---\n", __FUNCTION__, __LINE__)
#define BLE_DUMP8(x,y,z)    	 DUMP8(x,y,z)

#else
#define BLE_DBG(str,...)
#define BLE_ERR(str,...)         DebugOut(BLE_ERROR, str, ##__VA_ARGS__)
#define BLE_FUNC_ENTER() 
#define BLE_FUNC_LEAVE()
#define BLE_DUMP8(x,y,z)

#endif



#endif

#ifndef _AIS_PUB_H
#define _AIS_PUB_H

typedef struct
{
    uint8_t type;
} ais_uuid_t;

typedef struct
{
    ais_uuid_t uuid;
    uint16_t   val;
} ais_uuid_16_t;

typedef struct
{
    ais_uuid_t uuid;
    uint32_t   val;
} ais_uuid_32_t;

typedef struct
{
    ais_uuid_t uuid;
    uint8_t    val[16];
} ais_uuid_128_t;

typedef enum
{
    AIS_CCC_VALUE_NONE     = 0,
    AIS_CCC_VALUE_NOTIFY   = 1,
    AIS_CCC_VALUE_INDICATE = 2
} ais_ccc_value_t;

typedef void (*connected_callback_t)();
typedef void (*disconnected_callback_t)();
typedef size_t (*on_char_read_t)(void *buf, uint16_t len);
typedef size_t (*on_char_write_t)(void *buf, uint16_t len);
typedef void (*on_char_ccc_change_t)(ais_ccc_value_t value);

typedef struct
{
    /* Characteristics UUID */
    ais_uuid_t *uuid;
    /* Characteristics property */
    uint8_t prop;
    /* Characteristics value attribute permission */
    uint8_t perm;
    /* Characteristics value read handler, NULL if not used */
    on_char_read_t on_read;
    /* Characteristics value write handler, NULL if not used */
    on_char_write_t on_write;
    /**
     * Characteristics value ccc changed handler.
     * Only applied to NOFITY and INDICATE type Characteristics,
     * NULL if not applied.
     */
    on_char_ccc_change_t on_ccc_change;
} ais_char_init_t;

typedef struct
{
    /* AIS primamry service */
    ais_uuid_t *uuid_svc;
    /* AIS's Read Characteristics */
    ais_char_init_t rc;
    /* AIS's Write Characteristics */
    ais_char_init_t wc;
    /* AIS's Indicate Characteristics */
    ais_char_init_t ic;
    /* AIS's Write WithNoRsp Characteristics */
    ais_char_init_t wwnrc;
    /* AIS's Notify Characteristics */
    ais_char_init_t nc;
    /* Callback function when bluetooth connected */
    connected_callback_t on_connected;
    /* Callback function when bluetooth disconnected */
    disconnected_callback_t on_disconnected;
} ais_bt_init_t;

typedef void(*txdone)(uint8_t res);
extern txdone txdone_cb;
extern ais_bt_init_t ais_bt_init_info;

#endif

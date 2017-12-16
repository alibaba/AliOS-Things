/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>

#include "smartbt.h"
#include "smartbt_cfg.h"
#include "smartbt_smart_interface.h"
#include "smartbt_smartbridge.h"
#include "smartbt_smartbridge_gatt.h"

#include "StringUtils.h"
#include "LinkListUtils.h"

#ifdef MICO_CLI_ENABLE

static void socket_list ( CLI_ARGS );
static void paired_list ( CLI_ARGS );

static mico_bt_smartbridge_socket_t *smartbridge_socket = NULL;
static uint8_t smartbridge_socket_count = 0;


struct cli_command  ble_scan_cmd[] = {
    { "blesocket", "list ble connection status", socket_list },
    { "blepaired", "list ble paired devices information", paired_list },
};

void ble_commands_init ( mico_bt_smartbridge_socket_t *socket, uint8_t socket_count )
{
    smartbridge_socket = socket;
    smartbridge_socket_count = socket_count;
    /* Register an user command to cli interface */
    cli_register_commands( ble_scan_cmd, 2 );
}



/* Define a  command: blesocket, display ble socket infors
 */
static void socket_list ( CLI_ARGS )
{
    uint32_t i;

    char *status_str[3] = { "DISCONNECTED", "CONNECTING  ", "CONNECTED   " };
    mico_bt_smartbridge_socket_status_t status;
    char *bt_addr_str = NULL;

    /* Iterate all sockets and look for the first available socket */
    printf("+-ID-+-------Address-------+-----Status-----+--Handle---+\r\n");
    for ( i = 0; i < smartbridge_socket_count; i++ ) {
        bt_addr_str = DataToHexStringWithColons( smartbridge_socket[i].remote_device.address, BD_ADDR_LEN );
        mico_bt_smartbridge_get_socket_status( &smartbridge_socket[i], &status );
        printf("+-%02ld-+-[%s]-+--%s--+--0x%04x---+\r\n", i, bt_addr_str, status_str[status],
               smartbridge_socket[i].connection_handle);
    }
}

/* Define a  command: blepairedinfo, display paired devices and keys
 */
static void printf_device_data( uint8_t *address )
{
    char *debug_str = NULL;
    debug_str = DataToHexStringWithSpaces(address, BD_ADDR_LEN);
    printf("===============Peer Device:[%s]=====================\r\n", debug_str);
    free(debug_str);
}

static void printf_key_data( char *name, uint8_t *p_data, uint16_t len )
{
    char *debug_str = NULL;
    debug_str = DataToHexStringWithSpaces(p_data, len);
    printf("    %s:[%s]\r\n", name, debug_str);
    free(debug_str);
}

static void printf_key_number( char *name, uint32_t number )
{
    printf("    %s:%ld\r\n", name, number);
}

static void printf_key_desp( char *name, char *desp )
{
    printf("    %s:%s\r\n", name, desp);
}

static void printf_security_level( mico_bt_security_level level )
{
    if ( level == BTM_LE_SEC_NONE ) {
        printf("    Security Level:Not secured\r\n");
    } else if ( level == BTM_LE_SEC_UNAUTHENTICATE ) {
        printf("    Security Level:Not authenticated\r\n");
    } else if ( level == BTM_LE_SEC_AUTHENTICATED ) {
        printf("    Security Level:Authenticated\r\n");
    } else {
        printf("    Security Level:Unknown value\r\n");
    }
}


static void paired_list ( CLI_ARGS )
{
    OSStatus err = kNoErr;
    mico_bt_dev_bonded_device_info_t device_info[20];
    uint16_t i, num_devices = 20;
    mico_bt_security_key_value_t p_sec_keys;

    err = mico_bt_dev_get_bonded_devices(device_info, &num_devices);
    require_noerr(err, exit);

    for ( i = 0; i < num_devices; i++ ) {
        printf_device_data( device_info[i].bd_addr );

        if ( kNoErr == mico_bt_dev_get_key_by_keytype(device_info[i].bd_addr, BTM_LE_KEY_PENC, &p_sec_keys) ) {
            printf(" Encryption information of peer device:\r\n");
            printf_key_data     ( "LTK           ", p_sec_keys.le_keys.penc_key.ltk,    BT_OCTET16_LEN );
            printf_key_data     ( "rand          ", p_sec_keys.le_keys.penc_key.rand,   BT_OCTET8_LEN );
            printf_key_number   ( "ediv          ", p_sec_keys.le_keys.penc_key.ediv );
            printf_key_number   ( "Key size      ", p_sec_keys.le_keys.penc_key.key_size );
            printf_security_level ( p_sec_keys.le_keys.penc_key.sec_level );
        }

        if ( kNoErr == mico_bt_dev_get_key_by_keytype(device_info[i].bd_addr, BTM_LE_KEY_PID, &p_sec_keys)) {
            printf(" Identity key of the peer device:\r\n");
            printf_key_data     ( "IRK           ", p_sec_keys.le_keys.pid_key.irk,         BT_OCTET16_LEN );
            printf_key_data     ( "Static Addr   ", p_sec_keys.le_keys.pid_key.static_addr, BD_ADDR_LEN );
            printf_key_desp     ( "Address Type  ", (p_sec_keys.le_keys.pid_key.addr_type) ? "Static" : "Random" );
        }

        if ( kNoErr == mico_bt_dev_get_key_by_keytype(device_info[i].bd_addr, BTM_LE_KEY_LENC, &p_sec_keys)) {
            printf(" Encryption information of local device:\r\n");
            printf_key_data     ( "LTK           ", p_sec_keys.le_keys.lenc_key.ltk,        BT_OCTET16_LEN );
            printf_key_number   ( "Div           ", p_sec_keys.le_keys.lenc_key.div );
            printf_key_number   ( "Key size      ", p_sec_keys.le_keys.lenc_key.key_size );
            printf_security_level ( p_sec_keys.le_keys.lenc_key.sec_level );
        }

        if ( kNoErr == mico_bt_dev_get_key_by_keytype(device_info[i].bd_addr, BTM_LE_KEY_LID, &p_sec_keys)) {
            printf(" Identity key of the local device:\r\n");
        }

        if ( kNoErr == mico_bt_dev_get_key_by_keytype(device_info[i].bd_addr, BTM_LE_KEY_LCSRK, &p_sec_keys)) {
            printf(" local CSRK has been deliver to peer:\r\n");
            printf_key_data     ( "LCSRK         ", p_sec_keys.le_keys.lcsrk_key.csrk,      BT_OCTET16_LEN );
            printf_key_number   ( "counter       ", p_sec_keys.le_keys.lcsrk_key.counter );
            printf_key_number   ( "Div           ", p_sec_keys.le_keys.lcsrk_key.div );
            printf_security_level ( p_sec_keys.le_keys.lcsrk_key.sec_level );
        }
    }

exit:
    return;
}

#endif









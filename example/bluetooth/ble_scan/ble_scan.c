/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/aos.h"
#include "smartbt.h"
#include "smartbt_smartbridge.h"

#include "StringUtils.h"

#define ble_scan_log(M, ...) custom_log("LE SCAN", M, ##__VA_ARGS__)

/******************************************************************************
 *                                Constants
******************************************************************************/

/******************************************************************************
 *                           Function Prototypes
 ******************************************************************************/

/******************************************************************************
 *                                Structures
 ******************************************************************************/


/******************************************************************************
 *                                Variables Definitions
 ******************************************************************************/

static void start_scan                      ( CLI_ARGS );
static OSStatus scan_complete_handler       ( void *arg );

struct cli_command  ble_scan_cmd[] = {
    { "blescan", "blescan <passive/active>", start_scan },
};

/******************************************************************************
 *                          Function Definitions
 ******************************************************************************/

/*
 *  Entry point to the application.
 */

int application_start( void )
{
    OSStatus err = oNoErr;


    /* Initialise AOS Bluetooth Framework */
    err = aos_bt_init( AOS_BT_HCI_MODE, "SmartBridge Device", 0, 0 );
    require_noerr( err, exit );

    /* Initialise AOS SmartBridge, no connection is actually needed */
    err = aos_bt_smartbridge_init( 0 );
    require_noerr( err, exit );

    /* Register an user command to cli interface */
    err = aos_cli_register_commands( ble_scan_cmd, 1 );
    require_noerr( err, exit );

    LOG( "Initialize success, input \"blescan\" to start...");

exit:
    //aos_rtos_delete_thread( NULL );
    aos_task_exit(0);
    return err;
}

/* Start scan process
 */
static void start_scan ( CLI_ARGS )
{
    /* Scan settings */
    aos_bt_smart_scan_settings_t scan_settings;

    scan_settings.type              = BT_SMART_PASSIVE_SCAN;
    scan_settings.filter_policy     = FILTER_POLICY_NONE;
    scan_settings.filter_duplicates = DUPLICATES_FILTER_DISABLED;
    scan_settings.interval          = 96;
    scan_settings.window            = 48;
    scan_settings.duration_second   = 3;

    if (!strcasecmp(argv[1], "active")) {
        aos_cli_printf("Start ble active scan\r\n");
        scan_settings.type = BT_SMART_ACTIVE_SCAN;
    } else {
        aos_cli_printf("Start ble passive scan\r\n");
        scan_settings.type = BT_SMART_PASSIVE_SCAN;
    }

    /* Start scan */
    //aos_bt_smartbridge_stop_scan( );
    aos_bt_smartbridge_start_scan( &scan_settings, scan_complete_handler, NULL );
}

static void print_adv_data( uint8_t *adv_data, uint8_t adv_data_len )
{
    uint8_t         adv_type;
    uint8_t         adv_data_length;
    uint8_t         *p;
    char            *adv_data_str = NULL;

    p = adv_data;
    STREAM_TO_UINT8( adv_data_length, p );

    while ( (p - adv_data <= adv_data_len ) ) {
        STREAM_TO_UINT8( adv_type, p );

        adv_data_str = DataToHexStringWithSpaces( p, adv_data_length );
        printf( " =>(%2x):%s\r\n", adv_type, adv_data_str );
        free( adv_data_str );

        p += adv_data_length - 1; /* skip the length of data */
        STREAM_TO_UINT8( adv_data_length, p );
    }
}


/* Scan complete handler. Scan complete event reported via this callback.
 * It runs on the AOS_NETWORKING_WORKER_THREAD context.
 */
static OSStatus scan_complete_handler( void *arg )
{
    OSStatus err = oNoErr;
    uint32_t count = 0;
    char        *bd_addr_str = NULL;
    aos_bt_smart_scan_result_t *scan_result = NULL;

    printf("Scan complete\r\n");
    err = aos_bt_smartbridge_get_scan_result_list( &scan_result, &count );
    require_noerr( err, exit );

    if ( count == 0 ) {
        printf( "No ble device found\r\n" );
        err = oNotFoundErr;
        goto exit;
    }

    while ( scan_result != NULL ) {
        bd_addr_str = DataToHexStringWithColons( (uint8_t *)scan_result->remote_device.address, 6 );
        printf("[%s] ", bd_addr_str );
        printf("address type: %x ", (uint16_t)scan_result->remote_device.address_type );
        printf("rssi: %d ", scan_result->signal_strength );
        printf("name: %s\r\n", scan_result->remote_device.name );
        free( bd_addr_str );

        printf(" =>Advertisement data:   \r\n" );
        print_adv_data( scan_result->last_advertising_event_received.eir_data,
                        scan_result->last_advertising_event_received.eir_data_length );

        if ( scan_result->last_scan_response_received.event == BT_SMART_SCAN_RESPONSE_EVENT) {
            printf(" =>Scan respond data:   \r\n" );
            print_adv_data( scan_result->last_scan_response_received.eir_data,
                            scan_result->last_scan_response_received.eir_data_length );
        }

        printf("\r\n");
        scan_result = scan_result->next;
    }

exit:
    /* Scan duration is complete */
    return err;
}



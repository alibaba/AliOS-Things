/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#if 0
#pragma once

#include "common.h"
#include "system.h"
#include "command_console/mico_cli.h"
#include "json_c/json.h"

#ifndef MICO_PREBUILT_LIBS
#include "mico_config.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef system_state_t          mico_system_state_t;
typedef system_config_t         mico_Context_t;
typedef system_status_wlan_t    mico_system_status_wlan_t;

/** @defgroup MICO_SYSTEM_APIs  MICO System APIs
  * @brief MiCO System provide a basic framework for application based on MiCO core APIs
  * @{
  */

/** @addtogroup MICO_SYSTEM_APIs
  * @{
  */

/** @defgroup System_General System General Tools
  * @brief Read MiCO system's version
  * @{
  */

/**
  * @brief  Read MiCO SDK version.
  * @param  major: Major version number.
  * @param  minor: Minor version number.
  * @param  revision: Revision nember.
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
void mico_sdk_version( uint8_t *major, uint8_t *minor, uint8_t *revision );

/**
  * @brief  Read MiCO Application information.
  * @param  p_info: Point to the memory to store the info.
  * @param  len_info: Size of the menory.
  * @retval none.
  */
void mico_app_info(char *p_info, int len_info);

/** @} */


/** @defgroup system_context Core Data Functions
  * @brief System core data management, should initialized before other system functions
  * @{
  */

/* Parameter section store in flash */
typedef enum
{
    PARA_BOOT_TABLE_SECTION,
    PARA_MICO_DATA_SECTION,
#ifdef MICO_BLUETOOTH_ENABLE
    PARA_BT_DATA_SECTION,
#endif
    PARA_SYS_END_SECTION,
    PARA_APP_DATA_SECTION,
    PARA_END_SECTION
} para_section_t;

/**
  * @brief  Initialize core data used by MiCO system framework. System and
  *         application's configuration are read from non-volatile storage:
  *         flash etc.
  * @param  size_of_user_data: The length of config data used by application
  * @retval Address of core data.
  */
void* mico_system_context_init( uint32_t size_of_user_data );

/**
  * @brief  Get the address of the core data.
  * @param  none
  * @retval Address of core data.
  */
mico_Context_t* mico_system_context_get( void );

/**
  * @brief  Get the address of the application's config data.
  * @param  in_context: The address of the core data.
  * @retval Address of the application's config data.
  */
void* mico_system_context_get_user_data( mico_Context_t* const in_context );

/**
  * @brief  Restore configurations to default settings, it will cause a delegate
  *         function: appRestoreDefault_callback( ) to give a default value for
  *         application's config data.
  * @param  in_context: The address of the core data.
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
OSStatus mico_system_context_restore( mico_Context_t* const in_context );

/**
  * @brief  Application should give a default value for application's config data
  *         on address: user_config_data
  * @note   This a delegate function, can be completed by developer.
  * @param  user_config_data: The address of the application's config data.
  * @param  size: The size of the application's config data.
  * @retval None
  */
void appRestoreDefault_callback(void * const user_config_data, uint32_t size);

/**
  * @brief  Write config data hosted by core data to non-volatile storage
  * @param  in_context: The address of the core data.
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
OSStatus mico_system_context_update( mico_Context_t* const in_context );

OSStatus mico_system_para_read(void** info_ptr, int section, uint32_t offset, uint32_t size);
OSStatus mico_system_para_write(const void* info_ptr, int section, uint32_t offset, uint32_t size);
OSStatus mico_system_para_read_release( void* info_ptr );

/** @} */

/** @defgroup MiCO OTA Functions
  * @brief MiCO firmware updation functions
  * @{
  */

/**
  * @brief  Active downloaded firmware that stored in MICO_PARTITION_OTA_TEMP
  * @param  ota_data_len: the length of the new firmware
  * @param  ota_data_crc: the crc result of the new firmware
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
OSStatus mico_ota_switch_to_new_fw(int ota_data_len, uint16_t ota_data_crc);

/** @} */

/** @defgroup system System Framework Functions
  * @brief Initialize MiCO system functions defined in mico_config.h
  * @{
  */

/** @brief Wlan configuration source */
typedef enum{
  CONFIG_BY_NONE,             /**< Default value */
  CONFIG_BY_EASYLINK_V2,      /**< Wlan configured by EasyLink revision 2.0 */
  CONFIG_BY_EASYLINK_PLUS,    /**< Wlan configured by EasyLink Plus */
  CONFIG_BY_EASYLINK_MINUS,   /**< Wlan configured by EasyLink Minus */
  CONFIG_BY_AIRKISS,          /**< Wlan configured by airkiss from wechat Tencent inc. */
  CONFIG_BY_SOFT_AP,          /**< Wlan configured by EasyLink soft ap mode */
  CONFIG_BY_WAC,              /**< Wlan configured by wireless accessory configuration from Apple inc. */
  CONFIG_BY_USER,             /**< Wlan configured by user defined functions. */
} mico_config_source_t;

/**
  * @brief  Initialize MiCO system functions according to mico_config.h
  * @param  in_context: The address of the core data.
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
OSStatus mico_system_init( mico_Context_t* const in_context );

/**
  * @brief  Get IP, MAC, driver info for wlan interface
  * @param  status: The memory to store wlan status's address.
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
OSStatus mico_system_wlan_get_status( mico_system_status_wlan_t** status );

/**
  * @brief  Start wlan configuration mode, EasyLink, SoftAP, Airkiss...
  *         according to macro: MICO_WLAN_CONFIG_MODE
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
OSStatus mico_system_wlan_start_autoconf( void );

/**
  * @brief  Inform the application that EasyLink configuration will start
  * @note   This a delegate function, can be completed by developer.
  * @retval None
  */
void mico_system_delegate_config_will_start( void );

/**
  * @brief  Inform the application that EasyLink configuration will stop
  * @note   This a delegate function, can be completed by developer.
  * @retval None
  */
void mico_system_delegate_config_will_stop( void );

/**
  * @brief  Inform the application that EasyLink soft ap mode will start
  * @note   This a delegate function, can be completed by developer.
  * @retval None
  */
void mico_system_delegate_soft_ap_will_start( void );

/**
  * @brief  Inform the application that ssid and key are received from
  *         EasyLink client
  * @note   This a delegate function, can be completed by developer.
  * @param  ssid: The address of the SSID string.
  * @param  key: The address of the Key string.
  * @retval None
  */
void mico_system_delegate_config_recv_ssid ( char *ssid, char *key );

/**
  * @brief  Inform the application that auth data has received from
  *         EasyLink client
  * @note   This a delegate function, can be completed by developer.
  * @param  userInfo: Authentication data string.
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
OSStatus mico_system_delegate_config_recv_auth_data( char * userInfo );

/**
  * @brief  Inform the application that Easylink configuration is success.
  *         (MiCO has connect to wlan with the ssid and key)
  * @note   This a delegate function, can be completed by developer.
  * @param  source: The configuration mode used by EasyLink client
  * @retval None
  */
void mico_system_delegate_config_success( mico_config_source_t source );

/** @} */




/** @defgroup system_monitor System Monitor Functions
  * @brief Create monitors in MiCO. Monitors should be updated periodically,
  *        if not, a system reboot will perform.
  * @{
  */


/** @brief Structure to hold information about a system monitor item
  *        Application should not destroy or modify a system monitor item
  */
typedef struct _mico_system_monitor_t
{
    uint32_t last_update;              /**< Time of the last system monitor update */
    uint32_t longest_permitted_delay;  /**< Longest permitted delay between checkins with the system monitor */
} mico_system_monitor_t;

/**
  * @brief  Start the system monitor daemon
  * @note   This function can be called automatically by mico_system_init( )
  *         if macro: MICO_SYSTEM_MONITOR_ENABLE is defined
  * @retval None
  */
OSStatus mico_system_monitor_daemen_start( void );

/**
  * @brief  Register a system monitor point
  * @param  system_monitor: The address of a system monitor item.
  * @param  initial_permitted_delay: Longest permitted delay between checkins with the system monitor.
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
OSStatus mico_system_monitor_register( mico_system_monitor_t* system_monitor, uint32_t initial_permitted_delay );

/**
  * @brief  Perform a system monitor point checkin
  * @param  system_monitor: The address of a system monitor item.
  * @param  permitted_delay: The next permitted delay on the next checkin with the system monitor.
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
OSStatus mico_system_monitor_update ( mico_system_monitor_t* system_monitor, uint32_t permitted_delay );

/**
  *
  @}
  */


/** @defgroup system_power System Power Management Functions
  * @brief Perform a safety power status change on MiCO.
  * @{
  */


/**
  * @brief  Perform a system power change.
  * @note   This function can be used only after power management daemon is started
  * @param  in_context: The address of the core data.
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
OSStatus mico_system_power_perform( mico_Context_t* const in_context, mico_system_state_t new_state );
/** @} */

/*****************************************************************************/
/** @defgroup system_notify System Notify Functions
  * @brief Register a user function to a MiCO notification, this function will
  *        be called when a MiCO notification is triggered
  * @{
  */
/*****************************************************************************/

typedef notify_wlan_t WiFiEvent;

/** @brief MICO system defined notifications */
typedef enum{

  mico_notify_WIFI_SCAN_COMPLETED,        /**< A wlan scan is completed, type: void (*function)(ScanResult *pApList, void* arg)*/
  mico_notify_WIFI_STATUS_CHANGED,        /**< Wlan connection status is changed, type: void (*function)(WiFiEvent status, void* arg)*/
  mico_notify_WiFI_PARA_CHANGED,          /**< Wlan parameters has received (channel, BSSID, key...), called when connect to a wlan, type: void (*function)(apinfo_adv_t *ap_info, char *key, int key_len, void* arg)*/
  mico_notify_DHCP_COMPLETED,             /**< MiCO has get the IP address from DHCP server, type: void (*function)(IPStatusTypedef *pnet, void* arg)*/
  mico_notify_EASYLINK_WPS_COMPLETED,     /**< EasyLink receive SSID, Key, type: void (*function)(network_InitTypeDef_st *nwkpara, void* arg)*/
  mico_notify_EASYLINK_GET_EXTRA_DATA,    /**< EasyLink receive extra data, type: void (*function)(int datalen, char*data, void* arg)*/
  mico_notify_TCP_CLIENT_CONNECTED,       /**< A tcp client has connected to TCP server, type: void (*function)(int fd, void* arg)*/
  mico_notify_DNS_RESOLVE_COMPLETED,      /**< A DNS host address has resolved, type: void (*function)(uint8_t *hostname, uint32_t ip,  void* arg)*/
  mico_notify_SYS_WILL_POWER_OFF,         /**< System power will be turned off, type: void (*function)(void* arg)*/
  mico_notify_WIFI_CONNECT_FAILED,        /**< A wlan connection attemption is failed, type: void join_fail(OSStatus err, void* arg)*/
  mico_notify_WIFI_SCAN_ADV_COMPLETED,    /**< A anvanced wlan scan is completed, type: void (*function)(ScanResult_adv *pApList, void* arg)*/
  mico_notify_WIFI_Fatal_ERROR,           /**< A fatal error occured when communicating with wlan sub-system, type: void (*function)(void* arg)*/
  mico_notify_Stack_Overflow_ERROR,       /**< A MiCO RTOS thread's stack is over-flowed, type: void (*function)(char *taskname, void* arg)*/

} mico_notify_types_t;

/**
  * @brief  Register a user function to a MiCO notification.
  * @param  notify_type: The type of MiCO notification.
  * @param  functionAddress: The address of user function.
  * @param  arg: The address of argument, which will be called by registered user function.
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
OSStatus mico_system_notify_register( mico_notify_types_t notify_type, void* functionAddress, void* arg );

/**
  * @brief  Remove a user function from a MiCO notification.
  * @param  notify_type: The type of MiCO notification.
  * @param  functionAddress: The address of user function.
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
OSStatus mico_system_notify_remove( mico_notify_types_t notify_type, void *functionAddress );

/**
  * @brief  Remove all user function from a MiCO notification.
  * @param  notify_type: The type of MiCO notification.
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
OSStatus mico_system_notify_remove_all( mico_notify_types_t notify_type);


/** @} */


/** @defgroup config_server System Config Server Daemon
  * @brief Local network service, used to change MiCO system's configurations
  * @{
  */


/**
  * @brief  Start local config server.
  * @note   This function can be called automatically by mico_system_init( )
  *         if macro: MICO_CONFIG_SERVER_ENABLE is defined.
  * @param  in_context: The address of the core data.
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
OSStatus config_server_start ( void );

/**
  * @brief  Stop local config server.
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
OSStatus config_server_stop ( void );

/**
  * @brief  Inform the application that configuration data is received from
  *         config client
  * @note   This a delegate function, can be completed by developer.
  * @param  key: The name of a configuration item.
  * @param  value: The value of a configuration item, can be read use a json_object_get_xxx
  *         function from json_c library.
  * @param  in_context: The address of the core data.
  * @retval None.
  */
void config_server_delegate_recv( const char *key, json_object *value, bool *need_reboot, mico_Context_t *in_context );

/**
  * @brief  Inform the application that configuration server is collecting application's config data
  * @note   This a delegate function, can be completed by developer.
  * @param  config_cell_list: The UI element container to store application's config data.
  *         Developer can use config_server_create_xxx_cell functions to add customized UI element
  *         to config_cell_list.
  * @param  in_context: The address of the core data.
  * @retval None.
  */
void config_server_delegate_report( json_object *config_cell_list, mico_Context_t *in_context );

/**
  * @brief  Add a cell UI holds a string value. config_cell_list(1)<=config_cell(1).
  * @param  config_cell_list: The config cell container.
  * @param  name: String indicate the name of a config data.
  * @param  content: String value of a config data.
  * @param  privilege: The privilege of a value on config client.
  *                    @arg "RO": Read only. @arg "RW": Read or write.
  * @param  secectionArray: Generate a selection list for possible value, input NULL if not exist
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
OSStatus config_server_create_string_cell   (json_object* config_cell_list, char* const name, char* const content, char* const privilege, json_object* secectionArray);

/**
  * @brief  Add a cell UI holds a integer value. config_cell_list(1)<=config_cell(1).
  * @param  config_cell_list: The config cell container.
  * @param  name: String indicate the name of a config data.
  * @param  content: Integer value of a config data.
  * @param  privilege: The privilege of a value on config client.
  *                    @arg "RO": Read only. @arg "RW": Read or write.
  * @param  secectionArray: Generate a selection list for possible value, input NULL if not exist
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
OSStatus config_server_create_number_cell   (json_object* config_cell_list, char* const name, int content, char* const privilege, json_object* secectionArray);

/**
  * @brief  Add a cell UI holds a float value. config_cell_list(1)<=config_cell(1).
  * @param  config_cell_list: The config cell container.
  * @param  name: String indicate the name of a config data.
  * @param  content: Float value of a config data.
  * @param  privilege: The privilege of a value on config client.
  *                    @arg "RO": Read only. @arg "RW": Read or write.
  * @param  secectionArray: Generate a selection list for possible value
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
OSStatus config_server_create_float_cell    (json_object* config_cell_list, char* const name, float content, char* const privilege, json_object* secectionArray);

/**
  * @brief  Add a cell UI holds a bool value. config_cell_list(1)<=config_cell(1).
  * @param  config_cell_list: The config cell container.
  * @param  name: String indicate the name of a config data.
  * @param  content: Bool value of a config data.
  * @param  privilege: The privilege of a value on config client.
  *                    @arg "RO": Read only. @arg "RW": Read or write.
  * @param  secectionArray: Generate a selection list for possible value
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
OSStatus config_server_create_bool_cell     (json_object* config_cell_list, char* const name, boolean content, char* const privilege);

/**
  * @brief  Add a cell UI holds a sub menu, config_cell_list(1)<=sub_menu_array(1).
  * @param  config_cell_list: The config cell container.
  * @param  name: String indicate the name of a sub menu.
  * @param  sub_menu_array: An array that buids a sub menu list.
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
OSStatus config_server_create_sub_menu_cell (json_object* config_cell_list, char* const name, json_object* sub_menu_array);

/**
  * @brief  Create a new config cell list to an exist menu. sub_menu_array(1)<=config_cell_list(n)
  * @param  sub_menu_array: An array that buids a sub menu list.
  * @param  name: String indicate the name of a config cell list.
  * @param  config_cell_list: The config cell container.
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
OSStatus config_server_create_sector        (json_object* sub_menu_array, char* const name, json_object *config_cell_list);

/** @} */



/** \defgroup cli System Command Line Interface
  * @brief MiCO command console
  * @{
  */

/**
  * @brief  Initialize the CLI module
  * @note   This function can be called automatically by mico_system_init( )
  *         if macro: MICO_CLI_ENABLE is defined.
  * @retval 0 is returned on success, otherwise, -1 is returned.
  */
//int cli_init(void);

/** @} */



/** @defgroup system_mdns System mDNS service functions
  * @brief Multicast DNS is a way of using familiar DNS programming interfaces,
  *        packet formats and operating semantics, in a small network.
  * @{
  */


/** @brief Structure to store mDNS record data
  */

enum mdns_record_state_e{
    RECORD_REMOVED,     /**< The service and resources are removed.  */
    RECORD_UPDATE,      /**< The service record is updating, mdns routine will announce the new service content.  */
    RECORD_REMOVE,      /**< The service record is removing, mdns routine will announce the service with TTL = 0 for
                             1 second, resources will be removed after the announce period  */
    RECORD_SUSPEND,     /**< The service record is suspended, mdns routine will announce the service with TTL = 0 for
                             1 second and not respond the mdns query. */
    RECORD_NORMAL,      /**< The service can respond to mdns query .  */
};
typedef uint8_t mdns_record_state_t;

typedef struct _mdns_init_t
{
  char *service_name;     /**< The service name of a mDNS record, example: "_easylink_config._tcp.local."  */
  char *host_name;        /**< The host name of a mDNS record, example: "device_name.local."  */
  char *instance_name;    /**< The instance name of a mDNS record, example: "device_name"  */
  char *txt_record;       /**< Txt record holds customized info, every info is seperated by '.',
                               the orginal '.' is replaced by "/.", example: "record/.1=1.record/.2=2/.3"  */
  uint16_t service_port;  /**< Service port */
} mdns_init_t;

/**
  * @brief  Add a new mDNS record, a mDNS service daemon will be start if necessary
  * @param  record: mDNS record data.
  * @param  interface: Which network interface, the IP info will be read from when response a nDNS request.
  * @param  time_to_live: The TTL of a mDNS record.
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
OSStatus mdns_add_record( mdns_init_t record, hal_wifi_type_t interface, uint32_t time_to_live );

/**
  * @brief  Suspend a mDNS record, announce the TTL of this record equal to zero, and do not respond mDNS request
  * @param  service_name: The service name of a mDNS record. example: "_easylink_config._tcp.local."
  * @param  interface: Which network interface, the IP info will be read from when response a nDNS request.
  * @param  will_remove: Destory record info from memory after announced.
  * @retval None.
  */
void mdns_suspend_record( char *service_name, hal_wifi_type_t interface, bool will_remove );

/**
  * @brief  Resume a suspended mDNS record, announce the TTL of this record to orginal value, and respond to following mDNS request
  * @param  service_name: The service name of a mDNS record. example: "_easylink_config._tcp.local."
  * @param  interface: Which network interface, the IP info will be read from when response a nDNS request.
  * @retval None.
  */
void mdns_resume_record( char *service_name, hal_wifi_type_t interface );

/**
  * @brief  Update txt record with a new value
  * @param  service_name: The service name of a mDNS record. example: "_easylink_config._tcp.local."
  * @param  interface: Which network interface, the IP info will be read from when response a nDNS request.
  * @param  txt_record: Txt record holds customized info, every info is seperated by '.', the orginal '.' is
  *         replaced by "/.", example: "record/.1=1.record/.2=2/.3" = "record.1=1" and "record.2=2.3"
  * @retval None.
  */
void mdns_update_txt_record( char *service_name, hal_wifi_type_t interface, char *txt_record );

/**
  * @brief  Get mdns service status
  * @param  service_name: The service name of a mDNS record. example: "_easylink_config._tcp.local."
  * @param  interface: network interface, the mdns service is bonded
  * @retval @ref mdns_record_state_t.
  */
mdns_record_state_t mdns_get_record_status( char *service_name, hal_wifi_type_t interface);

/** @} */

/** @defgroup system_time MiCO System time functions
  * @brief Functions to get and set the real-time-clock time.
  * @{
  */



/** ISO8601 Time Structure
 */
#pragma pack(1)

typedef struct
{
    char year[4];        /**< Year         */
    char dash1;          /**< Dash1        */
    char month[2];       /**< Month        */
    char dash2;          /**< Dash2        */
    char day[2];         /**< Day          */
    char T;              /**< T            */
    char hour[2];        /**< Hour         */
    char colon1;         /**< Colon1       */
    char minute[2];      /**< Minute       */
    char colon2;         /**< Colon2       */
    char second[2];      /**< Second       */
    char decimal;        /**< Decimal      */
    char sub_second[6];  /**< Sub-second   */
    char Z;              /**< UTC timezone */
} iso8601_time_t;

#pragma pack()

/** Get the current system tick time in milliseconds
 *
 * @note The time will roll over every 49.7 days
 *
 * @param[out] time : A pointer to the variable which will receive the time value
 *
 * @return @ref OSStatus
 */
OSStatus mico_time_get_time( mico_time_t* time );


/** Set the current system tick time in milliseconds
 *
 * @param[in] time : the time value to set
 *
 * @return @ref OSStatus
 */
OSStatus mico_time_set_time( const mico_time_t* time );


/** Get the current UTC time in seconds
 *
 * This will only be accurate if the time has previously been set by using @ref mico_time_set_utc_time_ms
 *
 * @param[out] utc_time : A pointer to the variable which will receive the time value
 *
 * @return @ref OSStatus
 */
OSStatus mico_time_get_utc_time( mico_utc_time_t* utc_time );


/** Get the current UTC time in milliseconds
 *
 * This will only be accurate if the time has previously been set by using @ref mico_time_set_utc_time_ms
 *
 * @param[out] utc_time_ms : A pointer to the variable which will receive the time value
 *
 * @return @ref OSStatus
 */
OSStatus mico_time_get_utc_time_ms( mico_utc_time_ms_t* utc_time_ms );


/** Set the current UTC time in milliseconds
 *
 * @param[in] utc_time_ms : the time value to set
 *
 * @return @ref OSStatus
 */
OSStatus mico_time_set_utc_time_ms( const mico_utc_time_ms_t* utc_time_ms );


/** Get the current UTC time in iso 8601 format e.g. "2012-07-02T17:12:34.567890Z"
 *
 * @note The time will roll over every 49.7 days
 *
 * @param[out] iso8601_time : A pointer to the structure variable that
 *                            will receive the time value
 *
 * @return @ref OSStatus
 */
OSStatus mico_time_get_iso8601_time( iso8601_time_t* iso8601_time );


/** Convert a time from UTC milliseconds to iso 8601 format e.g. "2012-07-02T17:12:34.567890Z"
 *
 * @param[in] utc_time_ms   : the time value to convert
 * @param[out] iso8601_time : A pointer to the structure variable that
 *                            will receive the time value
 *
 * @return @ref OSStatus
 */
OSStatus mico_time_convert_utc_ms_to_iso8601( mico_utc_time_ms_t utc_time_ms, iso8601_time_t* iso8601_time );


#define MicoNanosendDelay mico_nanosecond_delay
/** Delay a period of time
 *
 * @param[in] delayus : the time value to set ( unit: nanosecond )
 *
 * @return none: this function will block until target time is reached
 */
void mico_nanosecond_delay( uint64_t delayus );

/** Get current nanosecond time
 *
 * @return current nanosecond time
 */
uint64_t mico_nanosecond_clock_value( void );

/** Set current nanosecond time to zero
 *
 * @return none
 */
void mico_nanosecond_reset( void );

/** Initialisse nanosecond counter function
 *
 * @return none
 */
void mico_nanosecond_init( void );

/** @} */


/** @defgroup tftp_ota Firmware Update From a TFTP Server
  * @brief Provide an easy way to download firmware from tftp server, use a
  *        predefined wlan and server address. It is used under factory environment
  * @{
  */

/**
  * @brief  Get new firmware from a TFTP Server, and replace the old one
  * @retval None.
  */
void tftp_ota(void);

/** @} */

/** @} */

/** @} */


#ifdef __cplusplus
} /*extern "C" */
#endif

#else

#define MICO_BT_PARA_LOCAL_KEY_DATA  65  /* BTM_SECURITY_LOCAL_KEY_DATA_LEN */

#define MICO_BT_DCT_NAME            249
#define MICO_BT_DCT_MAX_KEYBLOBS    146   /* Maximum size of key blobs to be stored :=  size of BR-EDR link keys +  size of BLE keys*/
#define MICO_BT_DCT_ADDR_FIELD      6
#define MICO_BT_DCT_LENGTH_FIELD    2
#ifndef MICO_BT_DCT_MAX_DEVICES
#define MICO_BT_DCT_MAX_DEVICES     10    /* Maximum number of device records stored in nvram */
#endif
#define MICO_BT_DCT_ADDR_TYPE       1
#define MICO_BT_DCT_DEVICE_TYPE     1

/* Length of BD_ADDR + 2bytes length field */
#define MICO_BT_DCT_ENTRY_HDR_LENGTH  (MICO_BT_DCT_ADDR_FIELD + MICO_BT_DCT_LENGTH_FIELD + MICO_BT_DCT_ADDR_TYPE + MICO_BT_DCT_DEVICE_TYPE)

#define MICO_BT_DCT_LOCAL_KEY_OFFSET  OFFSETOF( mico_bt_config_t, bluetooth_local_key )
#define MICO_BT_DCT_REMOTE_KEY_OFFSET OFFSETOF( mico_bt_config_t, bluetooth_remote_key )

#pragma pack(1)
typedef struct
{
//    uint8_t bluetooth_local_addeess[6];
//    uint8_t bluetooth_local_name[249]; /* including null termination */
    uint8_t bluetooth_local_key[MICO_BT_PARA_LOCAL_KEY_DATA];
    uint8_t bluetooth_remote_key[MICO_BT_DCT_ENTRY_HDR_LENGTH + MICO_BT_DCT_MAX_KEYBLOBS][MICO_BT_DCT_MAX_DEVICES];
    uint8_t padding[1];   /* to ensure 32-bit aligned size */
} mico_bt_config_t;
#pragma pack()

#endif


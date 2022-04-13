#ifndef RWIP_CONFIG_H_
#define RWIP_CONFIG_H_

/**
 ****************************************************************************************
 * @addtogroup ROOT
 * @{
 *
 *  Information about RW SW IP options and flags
 *
 *        BT_DUAL_MODE             BT/BLE Dual Mode
 *        BT_STD_MODE              BT Only
 *        BLE_STD_MODE             BLE Only
 *
 *        BT_EMB_PRESENT           BT controller exists
 *        BLE_EMB_PRESENT          BLE controller exists
 *        BLE_HOST_PRESENT         BLE host exists
 *
 * @name RW Stack Configuration
 * @{
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */
#include "rwapp_config.h"     // Audio Mode 0 configuration
#include "os_api.h"
#include "cmsis.h"
#include "besble_debug.h"

//#if (__IAG_BLE_INCLUDE__ == XA_ENABLED)

#ifdef __IAG_BLE_INCLUDE__
#ifndef FPGA
#define _BLE_NVDS_
#endif
#define CFG_BLE
#define CFG_HOST
#define CFG_APP

#if defined(BISTO_ENABLED)
#ifndef CFG_APP_SEC
#define CFG_APP_SEC
#endif

#define CFG_SEC_CON
#endif
#endif

#define GLOBAL_INT_DISABLE() do { uint32_t lock = int_lock();
#define GLOBAL_INT_RESTORE() int_unlock(lock); } while(0);

/******************************************************************************************/
/* --------------------------   GENERAL SETUP       --------------------------------------*/
/******************************************************************************************/

/// Flag indicating if stack is compiled in dual or single mode
#if defined(CFG_BT)
    #define BLE_STD_MODE                     0
    #if defined(CFG_BLE)
        #define BT_DUAL_MODE                 1
        #define BT_STD_MODE                  0
    #else // CFG_BLE
        #define BT_DUAL_MODE                 0
        #define BT_STD_MODE                  1
    #endif // CFG_BLE
#elif defined(CFG_BLE)
    #define BT_DUAL_MODE                     0
    #define BT_STD_MODE                      0
    #define BLE_STD_MODE                     1
#endif // CFG_BT

/******************************************************************************************/
/* -------------------------   STACK PARTITIONING      -----------------------------------*/
/******************************************************************************************/

#if (BT_DUAL_MODE)
    #define BT_EMB_PRESENT              1
    #define BLE_EMB_PRESENT             1
    #define HCI_PRESENT                 1
    #define BLE_HOST_PRESENT            0
    #define BLE_APP_PRESENT             0
#elif (BT_STD_MODE)
    #define BT_EMB_PRESENT              1
    #define BLE_EMB_PRESENT             0
    #define HCI_PRESENT                 1
    #define BLE_HOST_PRESENT            0
    #define BLE_APP_PRESENT             0
#elif (BLE_STD_MODE)
    #define BT_EMB_PRESENT              0
    #define HCI_PRESENT                 1
    #if defined(CFG_EMB)
        #define BLE_EMB_PRESENT         1
    #else
        #define BLE_EMB_PRESENT         0
    #endif //CFG_EMB
    #if defined(CFG_HOST)
        #define BLE_HOST_PRESENT        1
    #else
        #define BLE_HOST_PRESENT        0
    #endif //CFG_HOST
    #if defined(CFG_APP)
        #define BLE_APP_PRESENT         1
    #else
        #define BLE_APP_PRESENT         0
    #endif //CFG_APP
#endif // BT_DUAL_MODE / BT_STD_MODE / BLE_STD_MODE

#define EA_PRESENT                      (BT_EMB_PRESENT || BLE_EMB_PRESENT)

/******************************************************************************************/
/* -------------------------   INTERFACES DEFINITIONS      -------------------------------*/
/******************************************************************************************/
#define CFG_AHITL
#define CFG_HCITL

/// Application Host Interface
#if defined(CFG_AHITL)
#define AHI_TL_SUPPORT       1
#else // defined(CFG_AHITL)
#define AHI_TL_SUPPORT       0
#endif // defined(CFG_AHITL)


/// Host Controller Interface Support (defines if HCI parser is present or not)
#if defined(CFG_HCITL)
#define HCI_TL_SUPPORT      1
#else //defined(CFG_HCITL)
#define HCI_TL_SUPPORT      0
#endif //defined(CFG_HCITL)


#if BLE_HOST_PRESENT
#if BLE_EMB_PRESENT
#define H4TL_SUPPORT      (AHI_TL_SUPPORT)
#else // !BLE_EMB_PRESENT
#define H4TL_SUPPORT      0//((AHI_TL_SUPPORT) + (HCI_TL_SUPPORT))
#endif // BLE_EMB_PRESENT
#else // !BLE_HOST_PRESENT
#define H4TL_SUPPORT      (HCI_TL_SUPPORT)
#endif // BLE_HOST_PRESENT

/// TCI LMP trace support
#define TCI_LMP_ENABLED                             0

/******************************************************************************************/
/* --------------------------   BLE COMMON DEFINITIONS      ------------------------------*/
/******************************************************************************************/
/// Kernel Heap memory sized reserved for allocate dynamically connection environment
#define KE_HEAP_MEM_RESERVED        (4)

#define CFG_ALLROLES

#if defined(CFG_BLE)
/// Application role definitions
#define BLE_BROADCASTER  (defined(CFG_BROADCASTER) || defined(CFG_PERIPHERAL) || defined(CFG_ALLROLES))
#define BLE_OBSERVER     (defined(CFG_OBSERVER)    || defined(CFG_CENTRAL)    || defined(CFG_ALLROLES))
#define BLE_PERIPHERAL   (defined(CFG_PERIPHERAL)  || defined(CFG_ALLROLES))
#define BLE_CENTRAL      (defined(CFG_CENTRAL)     || defined(CFG_ALLROLES))
#define CFG_EXT_DB
#if (!BLE_BROADCASTER) && (!BLE_OBSERVER) && (!BLE_PERIPHERAL) && (!BLE_CENTRAL)
    #error "No application role defined"
#endif /* #if (!BLE_BROADCASTER) && (!BLE_OBSERVER) && (!BLE_PERIPHERAL) && (!BLE_CENTRAL) */

/// Maximum number of simultaneous BLE activities (scan, connection, advertising, initiating)
#define BLE_ACTIVITY_MAX        (BLE_CONNECTION_MAX)

/// Maximum number of audio connections
#if defined(CFG_AUDIO)
#if (BLE_CENTRAL || BLE_PERIPHERAL)
#define BLE_AUDIO      (1)
#define BLE_AUDIO_CONNECT_MAX    (CFG_AUDIO_CON)
#else
#define BLE_AUDIO      (0)
#endif /*(BLE_CENTRAL || BLE_PERIPHERAL)*/
#else
#define BLE_AUDIO      (0)
#endif /*defined(CFG_AUDIO)*/

/// Max advertising reports before sending the info to the host
#define BLE_ADV_REPORTS_MAX             1

/// Define Number of AUDIO TX/RX buffers per voice channel
#if (BLE_AUDIO)
    #if defined(CFG_AUDIO_AOAHI)
        // 3 buffers per connection using audio over AHI TL
        #define BLE_NB_INPUT_BUFF_PER_VC          (3)
        #define BLE_NB_OUTPUT_BUFF_PER_VC         (3)
    #else // defined(CFG_AUDIO_AOAHI)
        // 2 buffers if a codec is available
        #define BLE_NB_INPUT_BUFF_PER_VC          (2)
        #define BLE_NB_OUTPUT_BUFF_PER_VC         (2)
    #endif // defined(CFG_AUDIO_AOAHI)
    // add one more buffer for fake reception and fake transmit
    #define BLE_TX_AUDIO_BUFFER_CNT   ((BLE_AUDIO_CONNECT_MAX * BLE_NB_INPUT_BUFF_PER_VC)  + 1)
    #define BLE_RX_AUDIO_BUFFER_CNT   ((BLE_AUDIO_CONNECT_MAX * BLE_NB_OUTPUT_BUFF_PER_VC) + 1)
#endif // (BLE_AUDIO)

#define CFG_DEPRECATED_API
/// Support of Legacy Air Operations
#if defined(CFG_DEPRECATED_API)
#define BLE_DEPRECATED_API      (1)
#else //defined(CFG_DEPRECATED_API)
#define BLE_DEPRECATED_API      (0)
#endif //defined(CFG_DEPRECATED_API)
#endif //defined(CFG_BLE)


/******************************************************************************************/
/* --------------------------   DISPLAY SETUP        -------------------------------------*/
/******************************************************************************************/

/// Display controller enable/disable
#if defined(CFG_DISPLAY)
#define DISPLAY_SUPPORT      1
#else
#define DISPLAY_SUPPORT      0
#endif //CFG_DISPLAY


/******************************************************************************************/
/* --------------------------      RTC SETUP         -------------------------------------*/
/******************************************************************************************/

/// RTC enable/disable
#if defined(CFG_RTC)
#define RTC_SUPPORT      1
#else
#define RTC_SUPPORT      0
#endif //CFG_DISPLAY

/******************************************************************************************/
/* --------------------------      PS2 SETUP         -------------------------------------*/
/******************************************************************************************/

/// PS2 enable/disable
#if defined(CFG_PS2)
#define PS2_SUPPORT      1
#else
#define PS2_SUPPORT      0
#endif //CFG_PS2

/******************************************************************************************/
/* --------------------------      TRACER SETUP      -------------------------------------*/
/******************************************************************************************/

/// tracer enable/disable
#if defined(CFG_TRC_EN)
    #define TRACER_PRESENT                   1
    #include "dbg_trc_config.h"
#else
    #define TRACER_PRESENT                   0
#endif // CFG_TRC_EN

/******************************************************************************************/
/* -------------------------   DEEP SLEEP SETUP      -------------------------------------*/
/******************************************************************************************/

/// Use 32K Hz Clock if set to 1 else 32,768k is used
#define HZ32000                                     0

/// Time to wake-up Radio Module (in us)
#define SLEEP_RM_WAKEUP_DELAY                       625
/// Time for stabilization of the high frequency oscillator following a sleep-timer expiry (in us)
#define SLEEP_OSC_NORMAL_WAKEUP_DELAY               5000
/// Time for stabilization of the high frequency oscillator following an external wake-up request (in us)
#define SLEEP_OSC_EXT_WAKEUP_DELAY                  5000

/******************************************************************************************/
/* --------------------------   RADIO SETUP       ----------------------------------------*/
/******************************************************************************************/

/// Power control features
#define RF_TXPWR                            1
/// Class of device
#define RF_CLASS1                           0

/******************************************************************************************/
/* -------------------------   SUPPORTED RADIO PHY    ------------------------------------*/
/******************************************************************************************/

#if defined(CFG_RF_ATLAS)
#define BLE_PHY_1MBPS_SUPPORT                       1
#define BLE_PHY_2MBPS_SUPPORT                       1
#define BLE_PHY_CODED_SUPPORT                       0
#else
#define BLE_PHY_1MBPS_SUPPORT                       1
#define BLE_PHY_2MBPS_SUPPORT                       1
#define BLE_PHY_CODED_SUPPORT                       1
#endif

/******************************************************************************************/
/* -------------------------   COEXISTENCE SETUP      ------------------------------------*/
/******************************************************************************************/

/// WLAN Coexistence
#if defined(CFG_WLAN_COEX)
    #define RW_WLAN_COEX                 1
    #define RW_WLAN_COEX_TEST            (defined(CFG_WLAN_COEX_TEST))
#else
    #define RW_WLAN_COEX                 0
    #define RW_WLAN_COEX_TEST            0
#endif // defined(CFG_WLAN_COEX)

/// MWS Coexistence
#if defined(CFG_MWS_COEX)
    #define RW_MWS_COEX                 1
    #define RW_MWS_COEX_TEST            (defined(CFG_MWS_COEX_TEST))
#else
    #define RW_MWS_COEX                 0
    #define RW_MWS_COEX_TEST            0
#endif // defined(CFG_MWS_COEX)

/******************************************************************************************/
/* -------------------------   DM ARBITRATION SETUP      ---------------------------------*/
/******************************************************************************************/

#if BT_DUAL_MODE
/**
 * Dual mode arbitration margin (in us)
 *
 * BREDRMARGIN/BLEMARGIN corresponding to a timing value that allows the RF to power-down properly before any other
 * activity. This is radio dependent.
 */
#define DM_ARB_MARGIN      40
#endif //BT_DUAL_MODE

/******************************************************************************************/
/* --------------------   SECURE CONNECTIONS SETUP  --------------------------------------*/
/******************************************************************************************/
#if defined(CFG_SEC_CON)
#define SECURE_CONNECTIONS        (1)
#if defined(CFG_ECC_16_BITS_ALGO)
#define ECC_MULT_ALGO_TYPE        (16)
#else // !defined(CFG_ECC_16_BITS_ALGO)
#define ECC_MULT_ALGO_TYPE        (32)
#endif // defined(CFG_ECC_16_BITS_ALGO)
#if defined(CFG_CRYPTO_UT)
#define CRYPTO_UT                 (1)
#else //defined(CFG_CRYPTO_UT)
#define CRYPTO_UT                 (0)
#endif //defined(CFG_CRYPTO_UT)
#else // !defined(CFG_SEC_CON)
#define SECURE_CONNECTIONS        (0)
#define CRYPTO_UT                 (0)
#endif // defined(CFG_SEC_CON)


/******************************************************************************************/
/* --------------------------   DEBUG SETUP       ----------------------------------------*/
/******************************************************************************************/

/// Flag indicating if debug mode is activated or not
#if defined(CFG_DBG)
    #define RW_DEBUG                        ((BLE_EMB_PRESENT) || (BT_EMB_PRESENT) || (BLE_HOST_PRESENT))
#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    #define RW_SWDIAG                       1
#else
    #define RW_SWDIAG                       0
#endif
    #define KE_PROFILING                    1
#else
    #define RW_DEBUG                        0
    #define RW_SWDIAG                       0
    #define KE_PROFILING                    0
#endif /* CFG_DBG */

/// Flag indicating if Read/Write memory commands are supported or not
#if defined(CFG_DBG_MEM)
    #define RW_DEBUG_MEM               1
#else //CFG_DBG_MEM
    #define RW_DEBUG_MEM               0
#endif //CFG_DBG_MEM

/// Flag indicating if Flash debug commands are supported or not
#if defined(CFG_DBG_FLASH)
    #define RW_DEBUG_FLASH                  1
#else //CFG_DBG_FLASH
    #define RW_DEBUG_FLASH                  0
#endif //CFG_DBG_FLASH

/// Flag indicating if CPU stack profiling commands are supported or not
#if defined(CFG_DBG_STACK_PROF)
    #define RW_DEBUG_STACK_PROF             1
#else
    #define RW_DEBUG_STACK_PROF             0
#endif // defined (CFG_DBG_STACK_PROF)

/// Modem back to back setup
#define MODEM2MODEM                          0
/// Special clock testing
#define CLK_WRAPPING                         0

/******************************************************************************************/
/* --------------------------      NVDS SETUP       --------------------------------------*/
/******************************************************************************************/

/// Flag indicating if NVDS feature is supported or not
#if defined(CFG_NVDS)
    #define NVDS_SUPPORT                    1
#else //CFG_DBG_NVDS
    #define NVDS_SUPPORT                    0
#endif //CFG_DBG_NVDS

/******************************************************************************************/
/* --------------------------      MISC SETUP       --------------------------------------*/
/******************************************************************************************/
/// Manufacturer: RivieraWaves SAS
#define RW_COMP_ID                           0x0060

/// Bluetooth technologies version
#define RW_BT40_VERSION                      (6)
#define RW_BT41_VERSION                      (7)
#define RW_BT42_VERSION                      (8)
#define RW_BT50_VERSION                      (9)

/******************************************************************************************/
/* -------------------------   BT / BLE / BLE HL CONFIG    -------------------------------*/
/******************************************************************************************/

#if 0
#if (BT_EMB_PRESENT)
#include "rwbt_config.h"    // bt stack configuration
#endif //BT_EMB_PRESENT

#if (BLE_EMB_PRESENT)
#include "rwble_config.h"   // ble stack configuration
#endif //BLE_EMB_PRESENT
#endif

#if (BLE_HOST_PRESENT)
#include "rwble_hl_config.h"  // ble Host stack configuration
#endif //BLE_HOST_PRESENT

#if defined(CFG_AUDIO_AM0)
#include "rwam0_config.h"     // Audio Mode 0 configuration
#endif // defined(CFG_AUDIO_AM0)

#if defined(CFG_APP)
//#include "rwapp_config.h"     // Audio Mode 0 configuration
#endif // defined(CFG_APP)

#define BLE_INVALID_CONNECTION_INDEX	0xFF



#define SV_HIGH_SPEED_BLE_CONNECTION_INTERVAL_MIN_IN_MS		80
#define SV_HIGH_SPEED_BLE_CONNECTION_INTERVAL_MAX_IN_MS		400
#define SV_HIGH_SPEED_BLE_CONNECTION_SUPERVISOR_TIMEOUT_IN_MS	20000

#define SV_LOW_SPEED_BLE_CONNECTION_INTERVAL_MIN_IN_MS			400
#define SV_LOW_SPEED_BLE_CONNECTION_INTERVAL_MAX_IN_MS			1000
#define SV_LOW_SPEED_BLE_CONNECTION_SUPERVISOR_TIMEOUT_IN_MS	20000


/******************************************************************************************/
/* -------------------------   KERNEL SETUP          -------------------------------------*/
/******************************************************************************************/

/// Event types definition
enum KE_EVENT_TYPE
{
    KE_EVENT_KE_MESSAGE      ,
    KE_EVENT_KE_TIMER        ,
    #if (TRACER_PRESENT)
    KE_EVENT_TRC             ,
    #endif /*(TRACER_PRESENT)*/

    #if (AHI_TL_SUPPORT)
    KE_EVENT_AHI_TX_DONE     ,
    #endif //(AHI_TL_SUPPORT)

    #if (BLE_HOST_PRESENT)
    #if (BLE_L2CC)
    KE_EVENT_L2CAP_TX        ,
    #endif //(BLE_L2CC)
    #endif// (BLE_HOST_PRESENT)

    KE_EVENT_MAX             ,
};

/// Tasks types definition
enum KE_TASK_TYPE
{
#if (BLE_APP_PRESENT)
    TASK_APP,
#endif // (BLE_APP_PRESENT)

#if (BLE_HOST_PRESENT)
    TASK_L2CC,    // L2CAP Controller Task
    TASK_GATTM,   // Generic Attribute Profile Manager Task
    TASK_GATTC,   // Generic Attribute Profile Controller Task
    TASK_GAPM,    // Generic Access Profile Manager, 4
    TASK_GAPC,    // Generic Access Profile Controller

    // allocate a certain number of profiles task
    TASK_PRF_MAX = (TASK_GAPC + BLE_NB_PROFILES),

    #ifdef BLE_AUDIO_AM0_TASK
    TASK_AM0,     // BLE Audio Mode 0 Task
    #endif // BLE_AUDIO_AM0_TASK
#endif // (BLE_HOST_PRESENT)

#if (AHI_TL_SUPPORT)
    TASK_AHI,
#endif // (AHI_TL_SUPPORT)

    /// Maximum number of tasks
    TASK_MAX = 0xFE,

    TASK_NONE = 0xFF,
};

/// Kernel memory heaps types.
enum
{
    /// Memory allocated for environment variables
    KE_MEM_ENV,
    #if (BLE_HOST_PRESENT)
    /// Memory allocated for Attribute database
    KE_MEM_ATT_DB,
    #endif // (BLE_HOST_PRESENT)
    /// Memory allocated for kernel messages
    KE_MEM_KE_MSG,
    /// Non Retention memory block
    KE_MEM_NON_RETENTION,
    KE_MEM_BLOCK_MAX,
};



#if (BT_EMB_PRESENT)
#define BT_HEAP_MSG_SIZE_      BT_HEAP_MSG_SIZE
#define BT_HEAP_ENV_SIZE_      BT_HEAP_ENV_SIZE
#else
#define BT_HEAP_MSG_SIZE_      0
#define BT_HEAP_ENV_SIZE_      0
#endif //BT_EMB_PRESENT

#if (BLE_EMB_PRESENT)
#define BLE_HEAP_MSG_SIZE_     BLE_HEAP_MSG_SIZE
#define BLE_HEAP_ENV_SIZE_     BLE_HEAP_ENV_SIZE
#else
#define BLE_HEAP_MSG_SIZE_     0
#define BLE_HEAP_ENV_SIZE_     0
#endif //BLE_EMB_PRESENT

#if (BLE_HOST_PRESENT)
#define BLEHL_HEAP_MSG_SIZE_   BLEHL_HEAP_MSG_SIZE
#define BLEHL_HEAP_ENV_SIZE_   BLEHL_HEAP_ENV_SIZE
#define BLEHL_HEAP_DB_SIZE_    BLEHL_HEAP_DB_SIZE
#else
#define BLEHL_HEAP_MSG_SIZE_   0
#define BLEHL_HEAP_ENV_SIZE_   0
#define BLEHL_HEAP_DB_SIZE_    0
#endif //BLE_HOST_PRESENT


/// Kernel Message Heap
#define RWIP_HEAP_MSG_SIZE         (  BT_HEAP_MSG_SIZE_      + \
                                    BLE_HEAP_MSG_SIZE_     + \
                                    BLEHL_HEAP_MSG_SIZE_      )

/// Number of link in kernel environment
#define KE_NB_LINK_IN_HEAP_ENV   4

/// Size of Environment heap
#define RWIP_HEAP_ENV_SIZE         ( BT_HEAP_ENV_SIZE_         + \
                                     ( BLE_HEAP_ENV_SIZE_      + \
                                       BLEHL_HEAP_ENV_SIZE_ )    \
                                     * KE_NB_LINK_IN_HEAP_ENV )

/// Size of Attribute database heap
#define RWIP_HEAP_DB_SIZE         (  BLEHL_HEAP_DB_SIZE  )

/// Size of non retention heap - 512 bytes per ble link plus 4096 bytes for data throughput should be sufficient and should be tuned
#if (BLE_EMB_PRESENT || BLE_HOST_PRESENT)
#define RWIP_HEAP_NON_RET_SIZE    (( 512 * BLE_CONNECTION_MAX ) + 4096)
#else
#define RWIP_HEAP_NON_RET_SIZE    ( 1024 )
#endif

/// Minimum sleep time to enter in deep sleep (in half slot).
#define RWIP_MINIMUM_SLEEP_TIME                (1)

/******************************************************************************************/
/* -------------------------     CONFIGURABLE PARAMETERS     -----------------------------*/
/******************************************************************************************/

/// List of parameters identifiers
enum PARAM_ID
{
    /// Definition of the tag associated to each parameters
    /// Local Bd Address
    PARAM_ID_BD_ADDRESS                 = 0x01,
    /// Device Name
    PARAM_ID_DEVICE_NAME                = 0x02,
    /// Radio Drift
    PARAM_ID_LPCLK_DRIFT                = 0x07,
    /// Radio Jitter
    PARAM_ID_LPCLK_JITTER               = 0x08,
    /// Radio Class
    PARAM_ID_RADIO_CLASS                = 0x09,
    /// Bluejay specific Settings
    PARAM_ID_BJ_TXCNTL1                 = 0x0A,
    /// External wake-up time
    PARAM_ID_EXT_WAKEUP_TIME            = 0x0D,
    /// Oscillator wake-up time
    PARAM_ID_OSC_WAKEUP_TIME            = 0x0E,
    /// Radio wake-up time
    PARAM_ID_RM_WAKEUP_TIME             = 0x0F,
    /// UART baudrate
    PARAM_ID_UART_BAUDRATE              = 0x10,
    /// Enable sleep mode
    PARAM_ID_SLEEP_ENABLE               = 0x11,
    /// Enable External Wakeup
    PARAM_ID_EXT_WAKEUP_ENABLE          = 0x12,
    /// SP Private Key 192
    PARAM_ID_SP_PRIVATE_KEY_P192        = 0x13,
    /// SP Public Key 192
    PARAM_ID_SP_PUBLIC_KEY_P192         = 0x14,
    /// Errata adopted check
    PARAM_ID_ERRATA_ADOPTED             = 0x15,
    /// CQDDR Tags
    PARAM_ID_BASIC_THRESHOLD            = 0x16,
    PARAM_ID_EDR_THRESHOLD              = 0x17,
    PARAM_ID_BASIC_ALGORITHM            = 0x18,
    PARAM_ID_EDR_ALGORITHM              = 0x19,
    PARAM_ID_BASIC_PACKET_LUT           = 0x2A,
    PARAM_ID_EDR_PACKET_LUT             = 0x2B,
    /// Synchronous links configuration
    PARAM_ID_SYNC_CONFIG                = 0x2C,
    /// PCM Settings
    PARAM_ID_PCM_SETTINGS               = 0x2D,
    /// Sleep algorithm duration
    PARAM_ID_SLEEP_ALGO_DUR             = 0x2E,

    /// Diagport configuration
    PARAM_ID_DIAG_BT_HW                 = 0x30,
    PARAM_ID_DIAG_BLE_HW                = 0x31,
    PARAM_ID_DIAG_SW                    = 0x32,
    PARAM_ID_DIAG_PLF                   = 0x34,

    /// RSSI threshold tags
    PARAM_ID_RSSI_HIGH_THR              = 0x3A,
    PARAM_ID_RSSI_LOW_THR               = 0x3B,
    PARAM_ID_RSSI_INTERF_THR            = 0x3C,

    /// BLE Channel Assessment tags
    PARAM_ID_BLE_CA_TIMER_DUR           = 0x40,
    PARAM_ID_BLE_CRA_TIMER_CNT          = 0x41,
    PARAM_ID_BLE_CA_MIN_THR             = 0x42,
    PARAM_ID_BLE_CA_MAX_THR             = 0x43,
    PARAM_ID_BLE_CA_NOISE_THR           = 0x44,

    /// AFH algorithm tags
    PARAM_ID_AFH_REASS_NBCH             = 0x51,
    PARAM_ID_AFH_WINLGTH                = 0x52,
    PARAM_ID_AFH_RSSIMIN                = 0x53,
    PARAM_ID_AFH_PERTHRESBAD            = 0x54,
    PARAM_ID_AFH_REASS_INT              = 0x55,
    PARAM_ID_AFH_NMIN                   = 0x56,
    PARAM_ID_AFH_MAXADAPT               = 0x57,
    PARAM_ID_AFH_THSMIN                 = 0x58,


    PARAM_ID_BT_LINK_KEY_FIRST          = 0x60,
    PARAM_ID_BT_LINK_KEY_LAST           = 0x67,

    PARAM_ID_BLE_LINK_KEY_FIRST         = 0x70,
    PARAM_ID_BLE_LINK_KEY_LAST          = 0x7F,
    /// SC Private Key (Low Energy)
    PARAM_ID_LE_PRIVATE_KEY_P256        = 0x80,
    /// SC Public Key (Low Energy)
    PARAM_ID_LE_PUBLIC_KEY_P256         = 0x81,
    /// SC Debug: Used Fixed Private Key from NVDS (Low Energy)
    PARAM_ID_LE_DBG_FIXED_P256_KEY      = 0x82,
    /// SP Private Key (classic BT)
    PARAM_ID_SP_PRIVATE_KEY_P256        = 0x83,
    /// SP Public Key (classic BT)
    PARAM_ID_SP_PUBLIC_KEY_P256         = 0x84,

    /// Application specific
    PARAM_ID_APP_SPECIFIC_FIRST         = 0x90,
    PARAM_ID_APP_SPECIFIC_LAST          = 0xAF,
};

/// List of parameters lengths
enum PARAM_LEN
{
     // Definition of length associated to each parameters
     /// Local Bd Address
     PARAM_LEN_BD_ADDRESS                 = 6,
     /// Device Name
     PARAM_LEN_DEVICE_NAME                = 248,
     /// Low power clock drift
     PARAM_LEN_LPCLK_DRIFT                = 2,
     /// Low power clock jitter
     PARAM_LEN_LPCLK_JITTER               = 1,
     /// Radio Class
     PARAM_LEN_RADIO_CLASS                = 1,
     /// Bluejay specific Settings
     PARAM_LEN_BJ_TXCNTL1                 = 4,


     /// External wake-up time
     PARAM_LEN_EXT_WAKEUP_TIME            = 2,
     /// Oscillator wake-up time
     PARAM_LEN_OSC_WAKEUP_TIME            = 2,
     /// Radio wake-up time
     PARAM_LEN_RM_WAKEUP_TIME             = 2,
     /// UART baudrate
     PARAM_LEN_UART_BAUDRATE              = 4,
     /// Enable sleep mode
     PARAM_LEN_SLEEP_ENABLE               = 1,
     /// Enable External Wakeup
     PARAM_LEN_EXT_WAKEUP_ENABLE          = 1,
     /// SP Private Key 192
     PARAM_LEN_SP_PRIVATE_KEY_P192        = 24,
     /// SP Public Key 192
     PARAM_LEN_SP_PUBLIC_KEY_P192         = 48,
     /// Errata adopted check
     PARAM_LEN_ERRATA_ADOPTED             = 1,
     /// CQDDR Tags
     PARAM_LEN_BASIC_THRESHOLD            = 70,
     PARAM_LEN_EDR_THRESHOLD              = 70,
     PARAM_LEN_BASIC_ALGORITHM            = 21,
     PARAM_LEN_EDR_ALGORITHM              = 21,
     PARAM_LEN_BASIC_PACKET_LUT           = 16,
     PARAM_LEN_EDR_PACKET_LUT             = 16,
     /// Synchronous links configuration
     PARAM_LEN_SYNC_CONFIG                = 2,
     /// PCM Settings
     PARAM_LEN_PCM_SETTINGS               = 8,
     /// Diagport configuration
     PARAM_LEN_DIAG_BT_HW                 = 4,
     PARAM_LEN_DIAG_BLE_HW                = 4,
     PARAM_LEN_DIAG_SW                    = 4,
     PARAM_LEN_DIAG_PLF                   = 4,
     /// RSSI thresholds
     PARAM_LEN_RSSI_THR                   = 1,


     PARAM_LEN_BLE_CA_TIMER_DUR           = 2,
     PARAM_LEN_BLE_CRA_TIMER_CNT          = 1,
     PARAM_LEN_BLE_CA_MIN_THR             = 1,
     PARAM_LEN_BLE_CA_MAX_THR             = 1,
     PARAM_LEN_BLE_CA_NOISE_THR           = 1,

     /// AFH algorithm tags
     PARAM_LEN_AFH_REASS_NBCH             = 1,
     PARAM_LEN_AFH_WINLGTH                = 1,
     PARAM_LEN_AFH_RSSIMIN                = 1,
     PARAM_LEN_AFH_PERTHRESBAD            = 1,
     PARAM_LEN_AFH_REASS_INT              = 1,
     PARAM_LEN_AFH_NMIN                   = 1,
     PARAM_LEN_AFH_MAXADAPT               = 1,
     PARAM_LEN_AFH_THSMIN                 = 1,
     /// Link keys
     PARAM_LEN_BT_LINK_KEY                = 22,
     PARAM_LEN_BLE_LINK_KEY               = 48,

     /// P256
     PARAM_LEN_PRIVATE_KEY_P256           = 32,
     PARAM_LEN_PUBLIC_KEY_P256            = 64,
     PARAM_LEN_DBG_FIXED_P256_KEY         = 1,
};

/******************************************************************************************/
/* -------------------------        BT-BLE COEX        -----------------------------------*/
/******************************************************************************************/

///To let the HW using the default values set in the registers
#define RW_BLE_PTI_PRIO_AUTO    15

#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
/// Enable and diable definition for the PTI
///Enable TX busy signal
#define RWIP_PTI_TXEN           1
///Disable TX busy signal
#define RWIP_PTI_TXDIS          0
/// Tx busy position
#define RWIP_TXBSY_POS          0

///Enable RX busy signal
#define RWIP_PTI_RXEN           1
///Disable RX busy signal
#define RWIP_PTI_RXDIS          0
/// Rx busy position
#define RWIP_RXBSY_POS          1

///Enable do not abort TX
#define RWIP_PTI_DNABORTEN      1
///Disable do not abort TX
#define RWIP_PTI_DNABORTDIS     0
/// Do not abort busy position
#define RWIP_DNABORT_POS        2

///Allows Tx operation in the current frame.
#define RWIP_MWS_TXEN           0
///Prevent from any Tx operation in the current frame.
#define RWIP_MWS_TXDIS          1
/// MWS transmit disable position
#define RWIP_MWSTXDSB_POS       3

///Allows Rx operation in the current frame.
#define RWIP_MWS_RXEN           0
///Prevent from any Rx operation in the current frame.
#define RWIP_MWS_RXDIS          1
/// MWS transmit disable position
#define RWIP_MWSRXDSB_POS       4

/// Bit masking
#define RWIP_COEX_BIT_MASK      1

/// Coex configuration index
enum rwip_coex_config_idx
{
    #if (BT_EMB_PRESENT)
    RWIP_COEX_MSSWITCH_IDX ,
    RWIP_COEX_SNIFFATT_IDX ,
    RWIP_COEX_PAGE_IDX,
    RWIP_COEX_PSCAN_IDX,
    RWIP_COEX_INQ_IDX,
    RWIP_COEX_INQRES_IDX,
    RWIP_COEX_SCORSVD_IDX,
    RWIP_COEX_BCAST_IDX,
    RWIP_COEX_CONNECT_IDX,
    #endif //#if (BT_EMB_PRESENT)
    #if (BLE_EMB_PRESENT)
    RWIP_COEX_CON_IDX,
    RWIP_COEX_CON_DATA_IDX,
    RWIP_COEX_ADV_IDX,
    RWIP_COEX_SCAN_IDX,
    RWIP_COEX_INIT_IDX,
    #endif // #if (BLE_EMB_PRESENT)
    /// Max configuration index
    RWIP_COEX_CFG_MAX,
};
#endif //(BLE_EMB_PRESENT || BT_EMB_PRESENT)

/******************************************************************************************/
/* -------------------------     BT-BLE PRIORITIES     -----------------------------------*/
/******************************************************************************************/
#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
/// Priority index definition
enum rwip_prio_idx
{
    #if (BT_EMB_PRESENT)
    /// ACL event default priority
    RWIP_PRIO_ACL_DFT_IDX,
    /// ACL event priority with activity
    RWIP_PRIO_ACL_ACT_IDX,
    /// ACL Role Switch event default priority
    RWIP_PRIO_ACL_RSW_IDX,
    /// ACL sniff event default priority
    RWIP_PRIO_ACL_SNIFF_DFT_IDX,
    /// ACL sniff transition event default priority
    RWIP_PRIO_ACL_SNIFF_TRANS_IDX,
    #if MAX_NB_SYNC
    /// SCO event default priority
    RWIP_PRIO_SCO_DFT_IDX,
    #endif //MAX_NB_SYNC
    /// Broadcast ACL event default priority
    RWIP_PRIO_BCST_DFT_IDX,
    /// Broadcast ACL event with LMP activity priority
    RWIP_PRIO_BCST_ACT_IDX,
    /// CSB RX event default priority
    RWIP_PRIO_CSB_RX_DFT_IDX,
    /// CSB TX event default priority
    RWIP_PRIO_CSB_TX_DFT_IDX,
    /// Inquiry event default priority
    RWIP_PRIO_INQ_DFT_IDX,
    /// Inquiry Scan event default priority
    RWIP_PRIO_ISCAN_DFT_IDX,
    /// Page event default priority
    RWIP_PRIO_PAGE_DFT_IDX,
    /// Page event default priority
    RWIP_PRIO_PAGE_1ST_PKT_IDX,
    /// Page first packet event default priority
    RWIP_PRIO_PCA_DFT_IDX,
    /// Page scan event default priority
    RWIP_PRIO_PSCAN_DFT_IDX,
    /// Page scan event priority increment when canceled
    RWIP_PRIO_PSCAN_1ST_PKT_IDX,
    /// Synchronization Scan event default priority
    RWIP_PRIO_SSCAN_DFT_IDX,
    /// Synchronization Train event default priority
    RWIP_PRIO_STRAIN_DFT_IDX,
    #endif //#if (BT_EMB_PRESENT)
    #if (BLE_EMB_PRESENT)
    /// Default priority for scanning events
    RWIP_PRIO_SCAN_IDX,
    /// Default priority for initiating events
    RWIP_PRIO_INIT_IDX,
    /// Default priority for master connect events
    RWIP_PRIO_MCONNECT_IDX,
    /// Default priority for slave connect events
    RWIP_PRIO_SCONNECT_IDX,
    /// Default priority for advertising events
    RWIP_PRIO_ADV_IDX,
    /// Default priority for advertising high duty cycle events
    RWIP_PRIO_ADV_HDC_IDX,
    /// Default priority for resolvable private addresses renewal event
    RWIP_PRIO_RPA_RENEW_IDX,
    #endif // #if (BLE_EMB_PRESENT)
    RWIP_PRIO_IDX_MAX
};
/// Default priority value definition
enum rwip_prio_dft
{
    #if (BT_EMB_PRESENT)
    /// ACL event default priority
    RWIP_PRIO_ACL_DFT               = 5,
    /// ACL event priority with activity
    RWIP_PRIO_ACL_ACT               = 10,
    /// ACL Role Switch event default priority
    RWIP_PRIO_ACL_RSW               = 20,
    /// ACL sniff event default priority
    RWIP_PRIO_ACL_SNIFF_DFT         = 15,
    /// ACL sniff transition event default priority
    RWIP_PRIO_ACL_SNIFF_TRANS       = 10,
    #if MAX_NB_SYNC
    /// SCO event default priority
    RWIP_PRIO_SCO_DFT               = 18,
    #endif //MAX_NB_SYNC
    /// Broadcast ACL event default priority
    RWIP_PRIO_BCST_DFT              = 5,
    /// Broadcast ACL event with LMP activity priority
    RWIP_PRIO_BCST_ACT              = 10,
    /// CSB RX event default priority
    RWIP_PRIO_CSB_RX_DFT            = 10,
    /// CSB TX event default priority
    RWIP_PRIO_CSB_TX_DFT            = 10,
    /// Inquiry event default priority
    RWIP_PRIO_INQ_DFT               = 5,
    /// Inquiry Scan event default priority
    RWIP_PRIO_ISCAN_DFT             = 5,
    /// Page event default priority
    RWIP_PRIO_PAGE_DFT              = 8,
    /// Page first packet event default priority
    RWIP_PRIO_PAGE_1ST_PKT          = 20,
    /// PCA event default priority
    RWIP_PRIO_PCA_DFT               = 20,
    /// Page scan event default priority
    RWIP_PRIO_PSCAN_DFT             = 8,
    /// Page scan event priority increment when canceled
    RWIP_PRIO_PSCAN_1ST_PKT         = 20,
    /// Synchronization Scan event default priority
    RWIP_PRIO_SSCAN_DFT             = 10,
    /// Synchronization Train event default priority
    RWIP_PRIO_STRAIN_DFT            = 10,
    #endif //#if (BT_EMB_PRESENT)
    #if (BLE_EMB_PRESENT)
    /// Default priority for scanning events
    RWIP_PRIO_SCAN_DFT              = 5,
    /// Default priority for initiating events
    RWIP_PRIO_INIT_DFT              = 10,
    /// Default priority for master connect events
    RWIP_PRIO_MCONNECT_DFT          = 15,
    /// Default priority for slave connect events
    RWIP_PRIO_SCONNECT_DFT          = 15,
    /// Default priority for advertising events
    RWIP_PRIO_ADV_DFT               = 5,
    /// Default priority for advertising high duty cycle events
    RWIP_PRIO_ADV_HDC_DFT           = 10,
    /// Default priority for resolvable private addresses renewal event
    RWIP_PRIO_RPA_RENEW_DFT         = 10,
    #endif // #if (BLE_EMB_PRESENT)
    /// Max priority
    RWIP_PRIO_MAX                   = 31,
};
/// Default increment value definition
enum rwip_incr_dft
{
    #if (BT_EMB_PRESENT)
    /// ACL event default increment
    RWIP_INCR_ACL_DFT               = 1,
    /// ACL event increment with activity
    RWIP_INCR_ACL_ACT               = 1,
    /// ACL Role Switch event default increment
    RWIP_INCR_ACL_RSW               = 1,
    /// ACL sniff event default increment
    RWIP_INCR_ACL_SNIFF_DFT         = 1,
    /// ACL sniff transition event default increment
    RWIP_INCR_ACL_SNIFF_TRANS       = 1,
    #if MAX_NB_SYNC
    /// SCO event default increment
    RWIP_INCR_SCO_DFT               = 1,
    #endif //MAX_NB_SYNC
    /// Broadcast ACL event default increment
    RWIP_INCR_BCST_DFT              = 1,
    /// Broadcast ACL event with LMP activity increment
    RWIP_INCR_BCST_ACT              = 1,
    /// CSB RX event default increment
    RWIP_INCR_CSB_RX_DFT            = 1,
    /// CSB TX event default increment
    RWIP_INCR_CSB_TX_DFT            = 1,
    /// Inquiry event default increment
    RWIP_INCR_INQ_DFT               = 1,
    /// Inquiry Scan event default increment
    RWIP_INCR_ISCAN_DFT             = 1,
    /// Page event default increment
    RWIP_INCR_PAGE_DFT              = 1,
    /// Page event default increment
    RWIP_INCR_PAGE_1ST_PKT          = 2,
    /// Page first packet event default increment
    RWIP_INCR_PCA_DFT               = 1,
    /// Page scan event default increment
    RWIP_INCR_PSCAN_DFT             = 1,
    /// Page scan event increment increment when canceled
    RWIP_INCR_PSCAN_1ST_PKT         = 1,
    /// Synchronization Scan event default increment
    RWIP_INCR_SSCAN_DFT             = 1,
    /// Synchronization Train event default increment
    RWIP_INCR_STRAIN_DFT            = 1,
    #endif //#if (BT_EMB_PRESENT)
    #if (BLE_EMB_PRESENT)
    /// Default increment for scanning events
    RWIP_INCR_SCAN_DFT              = 1,
    /// Default increment for initiating events
    RWIP_INCR_INIT_DFT              = 1,
    /// Default increment for master connect events
    RWIP_INCR_MCONNECT_DFT          = 1,
    /// Default increment for slave connect events
    RWIP_INCR_SCONNECT_DFT          = 1,
    /// Default increment for advertising events
    RWIP_INCR_ADV_DFT               = 1,
    /// Default increment for advertising high duty cycle events
    RWIP_INCR_ADV_HDC_PRIO_DFT      = 1,
    /// Default increment for resolvable private addresses renewal event
    RWIP_INCR_RPA_RENEW_DFT         = 1,
    #endif // #if (BLE_EMB_PRESENT)
};
#endif //#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
/// @} BT Stack Configuration
/// @} ROOT

#endif //RWIP_CONFIG_H_

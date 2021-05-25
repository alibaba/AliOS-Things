#ifndef _DIS_CONFIG_H_
#define _DIS_CONFIG_H_



/** @defgroup DIS Device Information Service
  * @brief Device information service
  * @{
  */


/** @defgroup DIS_CONFIG Device Information Service Config
  * @brief Device information service config
  * @{
  */

/** @defgroup DIS_Common_Exported_Macros Device Information Service Config Common Exported Macros
  * @{
  */


/** @defgroup DIS_Config_Supported_Features Device Information Service Config Supported Features
  * @brief
    The Device Information Service may expose one or more of the characteristics, user can
    config DIS_CHAR_XXX_SUPPORT to 1 to support it, otherwise set it to 0.
  * @{
  */

/** @details
    Set DIS_CHAR_MANUFACTURER_NAME_SUPPORT to 1 to support Manufacturer Name String characteristic,
    otherwise set it to 0.
*/
#define DIS_CHAR_MANUFACTURER_NAME_SUPPORT          1

/** @details
    Set DIS_CHAR_MODEL_NUMBER_SUPPORT to 1 to support Model Number String characteristic,
    otherwise set it to 0.
*/
#define DIS_CHAR_MODEL_NUMBER_SUPPORT               1

/** @details
    Set DIS_CHAR_SERIAL_NUMBER_SUPPORT to 1 to support Serial Number String characteristic,
    otherwise set it to 0.
*/
#define DIS_CHAR_SERIAL_NUMBER_SUPPORT              1

/** @details
    Set DIS_CHAR_FIRMWARE_REVISION_SUPPORT to 1 to support Firmware Revision String characteristic,
    otherwise set it to 0.
*/
#define DIS_CHAR_FIRMWARE_REVISION_SUPPORT          1

/** @details
    Set DIS_CHAR_HARDWARE_REVISION_SUPPORT to 1 to support Hardware Revision String characteristic,
    otherwise set it to 0.
*/
#define DIS_CHAR_HARDWARE_REVISION_SUPPORT          1

/** @details
    Set DIS_CHAR_SOFTWARE_REVISION_SUPPORT to 1 to support Software Revision String characteristic,
    otherwise set it to 0.
*/
#define DIS_CHAR_SOFTWARE_REVISION_SUPPORT          1


/** @details
    Set DIS_CHAR_SYSTEM_ID_SUPPORT to 1 to support System ID characteristic,
    otherwise set it to 0.
*/
#define DIS_CHAR_SYSTEM_ID_SUPPORT                  1


/** @details
    Set DIS_CHAR_IEEE_CERTIF_DATA_LIST_SUPPORT to 1 to support IEEE 11073-20601 Regulatory Certification Data List characteristic,
    otherwise set it to 0.
*/
#define DIS_CHAR_IEEE_CERTIF_DATA_LIST_SUPPORT      1

/** @details
    Set DIS_CHAR_PNP_ID_SUPPORT to 1 to support PnP ID characteristic,
    otherwise set it to 0.
*/
#define DIS_CHAR_PNP_ID_SUPPORT                     1
/**
  * @}
  */



/** @defgroup DIS_Config_Max_Char_Value_Length Device Information Service Config Supported Value Length
  * @brief
    The max length of characteristic value can be configured here.
  * @{
  */
#ifdef DIS_CHAR_MANUFACTURER_NAME_SUPPORT
#define DIS_CHAR_MANUFACTURER_NAME_STR_MAX_LENGTH   20 /**< Max length of Manufacturer Name String characteristic value*/
#endif

#ifdef DIS_CHAR_MODEL_NUMBER_SUPPORT
#define DIS_CHAR_MODEL_NUMBER_STR_MAX_LENGTH        20  /**< Max length of Model Number String characteristic value*/
#endif

#ifdef DIS_CHAR_SERIAL_NUMBER_SUPPORT
#define DIS_CHAR_SERIAL_NUMBER_STR_MAX_LENGTH       20 /**< Max length of Serial Number String characteristic value*/
#endif

#ifdef DIS_CHAR_FIRMWARE_REVISION_SUPPORT
#define DIS_CHAR_FIRMWARE_REVISION_STR_MAX_LENGTH   20 /**< Max length of Firmware Revision String characteristic value*/
#endif

#ifdef DIS_CHAR_HARDWARE_REVISION_SUPPORT
#define DIS_CHAR_HARDWARE_REVISION_STR_MAX_LENGTH   20 /**< Max length of Hardware Revision String scharacteristic value*/
#endif

#ifdef DIS_CHAR_SOFTWARE_REVISION_SUPPORT
#define DIS_CHAR_SOFTWARE_REVISION_STR_MAX_LENGTH   20 /**< Max length of Software Revision String characteristic value*/
#endif


#ifdef DIS_CHAR_IEEE_CERTIF_DATA_LIST_SUPPORT
#define DIS_CHAR_IEEE_CERTIF_DATA_LIST_MAX_LENGTH  30 /**< Max length of IEEE 11073-20601 Regulatory Certification Data List characteristic value*/
#endif
/**
  * @}
  */



/** @defgroup DIS_Config_Char_Value_Index Device Information Service Config Characteristic Value Index
  * @brief
    Attribute index value of each characteristic shall be configured by characteristic supported. If some
  * @{
  */
#define GATT_SVC_DIS_MANU_NAME_INDEX        2   /**< Attribute index of Manufacturer Name String characteristic */
#define GATT_SVC_DIS_MODEL_NUM_INDEX        4   /**< Attribute index of Model Number String characteristic */
#define GATT_SVC_DIS_SERIAL_NUM_INDEX       6   /**< Attribute index of Serial Number String characteristic */
#define GATT_SVC_DIS_HARDWARE_REV_INDEX     8   /**< Attribute index of Hardware Revision String characteristic */
#define GATT_SVC_DIS_FIRMWARE_REV_INDEX     10  /**< Attribute index of Firmware Revision String characteristic */
#define GATT_SVC_DIS_SOFTWARE_REV_INDEX     12  /**< Attribute index of Software Revision String characteristic */
#define GATT_SVC_DIS_SYSTEM_ID_INDEX        14  /**< Attribute index of System ID characteristic */
#define GATT_SVC_DIS_IEEE_CERT_STR_INDEX    16  /**< Attribute index of IEEE 11073-20601 Regulatory Certification Data List characteristic */
#define GATT_SVC_DIS_PNP_ID_INDEX           18  /**< Attribute index of Pnp ID characteristic */
/**
  * @}
  */


/** @} End of DIS_Common_Exported_Macros */

/** @} End of DIS_CONFIG */

/** @} End of DIS */


#endif

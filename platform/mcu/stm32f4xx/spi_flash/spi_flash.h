
#ifndef INCLUDED_SPI_FLASH_API_H
#define INCLUDED_SPI_FLASH_API_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

   
/** @addtogroup MICO_Drivers_interface
  * @{
  */

/** @defgroup MiCO_SPI_Flash_Driver MiCO SPI_Flash Driver
  * @brief Provide driver interface for SPI Flash device
  * @{
  */

   
typedef enum
{
    SFLASH_WRITE_NOT_ALLOWED = 0,
    SFLASH_WRITE_ALLOWED     = 1,

} sflash_write_allowed_t;

typedef struct
{
    uint32_t device_id;
    void * platform_peripheral;
    sflash_write_allowed_t write_allowed;
} sflash_handle_t;


/**
 * @brief Initialize SPI flash
 *
 * @param  handle:  handle of spi flash to init
 * @param  peripheral_id: 
 * @param  write_allowed_in: 
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */ 
int init_sflash ( /*@out@*/ sflash_handle_t* const handle, /*@shared@*/ void* peripheral_id, sflash_write_allowed_t write_allowed_in );

/**
 * @brief SPI flash enter deep sleep mode
 *
 * @param  handle:  handle of spi flash to init
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */ 

int sflash_enter_dpmode( const sflash_handle_t* const handle );

/**
 *  De-initializes a SPI Flash chip
 *
 * @param[in] handle            Handle structure that will be used for this sflash instance - allocated by caller.
 *
 * @return @ref OSStatus
 */
int deinit_sflash       ( /*@out@*/ sflash_handle_t* const handle);

/**
 * @brief Read date from SPI flash
 *
 * @param  handle:          handle of spi flash to read
 * @param  device_address:  address of spi flash to read 
 * @param  data_addr:       buffer address to put data read from spi flash
 * @param  size:            size of data read from spi flash
 *
 * @return   kNoErr        : on success.
 * @return   kGeneralErr   : if an error occurred
 */ 
int sflash_read( const sflash_handle_t* const handle, unsigned long device_address, /*@out@*/  /*@dependent@*/ void* const data_addr, unsigned int size );


/**
 * @brief Write date to SPI flash
 *
 * @param  handle:         handle of spi flash to write
 * @param  device_address: address of spi flash to write
 * @param  data_addr:      buffer address to put data write to spi flash
 * @param  size:           size of data to be written in spi flash 
 *
 * @return  0       : on success.
 * @return  none    : if an error occurred
 */ 
int sflash_write( const sflash_handle_t* const handle, unsigned long device_address,  /*@observer@*/ const void* const data_addr, unsigned int size );



/**
 * @brief Erase all data of SPI flash
 *
 * @param  handle: handle of spi flash to be erased
 *
 * @return  0       : on success.
 * @return  none    : if an error occurred
 */ 
int sflash_chip_erase( const sflash_handle_t* const handle );


/**
 * @brief Erase part date of SPI flash
 *
 * @param  handle: handle of spi flash to be earsed
 * @param  device_address: address of spi flash to be erased
 *
 * @return  0       : on success.
 * @return  none    : if an error occurred
 */ 
int sflash_sector_erase( const sflash_handle_t* const handle, unsigned long device_address );


/**
 * @brief  Get size of SPI flash
 *
 * @param  handle: handle of spi flash
 * @param  size:  size of spi flash
 *
 * @return  0       : on success.
 * @return  none    : if an error occurred
 */ 
int sflash_get_size ( const sflash_handle_t* const handle, /*@out@*/ unsigned long* size );

/**
  * @}
  */


/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_SPI_FLASH_API_H */

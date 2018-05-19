#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "mico_platform.h"


extern platform_spi_driver_t            platform_spi_drivers[];
extern const platform_gpio_t            platform_gpio_pins[];
extern const platform_spi_t             platform_spi_peripherals[];


OSStatus MicoSpiInitialize( const platforom_spi_device_t* spi )
{
  platform_spi_config_t config;
  OSStatus              err = kNoErr;

  if ( spi->port >= MICO_SPI_NONE )
    return kUnsupportedErr;

#ifdef MICO_WIFI_SHARE_SPI_BUS
  if( platform_spi_peripherals[spi->port].port == wifi_spi.port )
  {
    return platform_wlan_spi_init( &platform_gpio_pins[spi->chip_select] );
  }
#endif

  if( platform_spi_drivers[spi->port].spi_mutex == NULL)
    mico_rtos_init_mutex( &platform_spi_drivers[spi->port].spi_mutex );
  
  config.chip_select = spi->chip_select == MICO_GPIO_NONE ? NULL : &platform_gpio_pins[spi->chip_select];
  config.speed       = spi->speed;
  config.mode        = spi->mode;
  config.bits        = spi->bits;

  mico_rtos_lock_mutex( &platform_spi_drivers[spi->port].spi_mutex );
  err = platform_spi_init( &platform_spi_drivers[spi->port], &platform_spi_peripherals[spi->port], &config );
  mico_rtos_unlock_mutex( &platform_spi_drivers[spi->port].spi_mutex );

  return err;
}

OSStatus MicoSpiFinalize( const platforom_spi_device_t* spi )
{
  OSStatus err = kNoErr;

  if ( spi->port >= MICO_SPI_NONE )
    return kUnsupportedErr;

#ifdef MICO_WIFI_SHARE_SPI_BUS
  if( platform_spi_peripherals[spi->port].port == wifi_spi.port )
  {
      return kUnsupportedErr;
    //return platform_wlan_spi_deinit( &platform_gpio_pins[spi->chip_select] );
  }
#endif

  if( platform_spi_drivers[spi->port].spi_mutex == NULL)
    mico_rtos_init_mutex( &platform_spi_drivers[spi->port].spi_mutex );
  
  mico_rtos_lock_mutex( &platform_spi_drivers[spi->port].spi_mutex );
  err = platform_spi_deinit( &platform_spi_drivers[spi->port] );
  mico_rtos_unlock_mutex( &platform_spi_drivers[spi->port].spi_mutex );

  return err;
}

OSStatus MicoSpiTransfer( const platforom_spi_device_t* spi, const platform_spi_message_segment_t* segments, uint16_t number_of_segments )
{
  platform_spi_config_t config;
  OSStatus err = kNoErr;

  if ( spi->port >= MICO_SPI_NONE )
    return kUnsupportedErr;

#ifdef MICO_WIFI_SHARE_SPI_BUS
  if( platform_spi_peripherals[spi->port].port == wifi_spi.port )
  {
    return platform_wlan_spi_transfer( &platform_gpio_pins[spi->chip_select], segments, number_of_segments );
  }
#endif

  if( platform_spi_drivers[spi->port].spi_mutex == NULL)
    mico_rtos_init_mutex( &platform_spi_drivers[spi->port].spi_mutex );

  config.chip_select = spi->chip_select == MICO_GPIO_NONE ? NULL : &platform_gpio_pins[spi->chip_select];
  config.speed       = spi->speed;
  config.mode        = spi->mode;
  config.bits        = spi->bits;

  mico_rtos_lock_mutex( &platform_spi_drivers[spi->port].spi_mutex );
  err = platform_spi_init( &platform_spi_drivers[spi->port], &platform_spi_peripherals[spi->port], &config );
  err = platform_spi_transfer( &platform_spi_drivers[spi->port], &config, segments, number_of_segments );
  mico_rtos_unlock_mutex( &platform_spi_drivers[spi->port].spi_mutex );

  return err;
}



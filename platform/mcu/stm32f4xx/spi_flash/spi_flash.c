
#include "platform_logging.h"
#include "platform.h"
#include "spi_flash.h"
#include "spi_flash_internal.h"
#include "spi_flash_platform_interface.h"
#include <string.h> /* for NULL */

#define sFLASH_SPI_PAGESIZE       0x100

int sflash_read_ID( const sflash_handle_t* const handle, void* const data_addr )
{
    return generic_sflash_command( handle, SFLASH_READ_JEDEC_ID, 0, NULL, 3, NULL, data_addr );
}

int sflash_enter_dpmode( const sflash_handle_t* const handle )
{
    return generic_sflash_command( handle, SFLASH_DEEP_POWER_DOWN, 0, NULL, 0, NULL, NULL );
}


int sflash_write_enable( const sflash_handle_t* const handle )
{
    if ( handle->write_allowed == SFLASH_WRITE_ALLOWED )
    {
        /* Send write-enable command */
        int status = generic_sflash_command( handle, SFLASH_WRITE_ENABLE, 0, NULL, 0, NULL, NULL );
        if ( status != 0 )
        {
            return status;
        }

        /* Check status register */
        unsigned char status_register;
        if ( 0 != ( status = sflash_read_status_register( handle, &status_register ) ) )
        {
            return status;
        }

        /* Check if Block protect bits are set */
        if ( status_register != SFLASH_STATUS_REGISTER_WRITE_ENABLED )
        {
            /* Disable protection for all blocks */
            if (0 != ( status = sflash_write_status_register( handle, 0 ) ) )
            {
                return status;
            }

            /* Re-Enable writing */
            if (0 != ( status = generic_sflash_command( handle, SFLASH_WRITE_ENABLE, 0, NULL, 0, NULL, NULL ) ) )
            {
                return status;
            }
        }
        return 0;
    }
    else
    {
        return -1;
    }
}

int sflash_chip_erase( const sflash_handle_t* const handle )
{
    int status = sflash_write_enable( handle );
    if ( status != 0 )
    {
        return status;
    }
    return generic_sflash_command( handle, SFLASH_CHIP_ERASE1, 0, NULL, 0, NULL, NULL );
}

int sflash_sector_erase ( const sflash_handle_t* const handle, unsigned long device_address )
{

    char device_address_array[3] =  { ( ( device_address & 0x00FF0000 ) >> 16 ),
                                      ( ( device_address & 0x0000FF00 ) >>  8 ),
                                      ( ( device_address & 0x000000FF ) >>  0 ) };

    int retval;
    int status = sflash_write_enable( handle );
    if ( status != 0 )
    {
        return status;
    }
    retval = generic_sflash_command( handle, SFLASH_SECTOR_ERASE, 3, device_address_array, 0, NULL, NULL );
    check_string(retval == 0, "SPI Flash erase error");
    return retval;
}

int sflash_read_status_register( const sflash_handle_t* const handle, void* const dest_addr )
{
    return generic_sflash_command( handle, SFLASH_READ_STATUS_REGISTER, 0, NULL, 1, NULL, dest_addr );
}



int sflash_read( const sflash_handle_t* const handle, unsigned long device_address, void* const data_addr, unsigned int size )
{
    char device_address_array[3] =  { ( ( device_address & 0x00FF0000 ) >> 16 ),
                                      ( ( device_address & 0x0000FF00 ) >>  8 ),
                                      ( ( device_address & 0x000000FF ) >>  0 ) };

    return generic_sflash_command( handle, SFLASH_READ, 3, device_address_array, size, NULL, data_addr );
}


// int sflash_get_size( const sflash_handle_t* const handle, unsigned long* const size )
// {
//     *size = 0; /* Unknown size to start with */

// #ifdef SFLASH_SUPPORT_MACRONIX_PARTS
//     if ( handle->device_id == SFLASH_ID_MX25L8006E )
//     {
//         *size = 0x100000; /* 1MByte */
//     }
// #endif /* ifdef SFLASH_SUPPORT_MACRONIX_PARTS */

// #ifdef SFLASH_SUPPORT_WINBOND_PARTS
//     if ( handle->device_id == SFLASH_ID_W25X80AVSIG )
//     {
//         *size = 0x100000; /* 1MByte */
//     }
// #endif /* ifdef SFLASH_SUPPORT_MACRONIX_PARTS */

// #ifdef SFLASH_SUPPORT_SST_PARTS
//     if ( handle->device_id == SFLASH_ID_SST25VF080B )
//     {
//         *size = 0x100000; /* 1MByte */
//     }
// #endif /* ifdef SFLASH_SUPPORT_SST_PARTS */
//     return 0;
// }

int sflash_get_size( const sflash_handle_t* const handle, /*@out@*/ unsigned long* const size )
{
    *size = 0; /* Unknown size to start with */

#ifdef SFLASH_SUPPORT_MACRONIX_PARTS
    if ( handle->device_id == SFLASH_ID_MX25L8006E )
    {
        *size = (unsigned long) 0x100000; /* 1MByte */
    }
    else if ( handle->device_id == SFLASH_ID_MX25L1606E )
    {
        *size = (unsigned long) 0x200000; /* 2MByte */
    }
#endif /* ifdef SFLASH_SUPPORT_MACRONIX_PARTS */
#ifdef SFLASH_SUPPORT_SST_PARTS
    if ( handle->device_id == SFLASH_ID_SST25VF080B )
    {
        *size = (unsigned long) 0x100000; /* 1MByte */
    }
#endif /* ifdef SFLASH_SUPPORT_SST_PARTS */
#ifdef SFLASH_SUPPORT_EON_PARTS
    if ( handle->device_id == SFLASH_ID_EN25QH16 )
    {
        *size = (unsigned long) 0x200000; /* 2MByte */
    }
#endif /* ifdef SFLASH_SUPPORT_EON_PARTS */
#ifdef SFLASH_SUPPORT_WINBOND_PARTS
    if ( handle->device_id == SFLASH_ID_W25X80AVSIG )
    {
        *size = (unsigned long) 0x100000; /* 1MByte */
    }
#endif /* ifdef SFLASH_SUPPORT_WINBOND_PARTS */
    return 0;
}


int sflash_write_page( const sflash_handle_t* const handle, unsigned long device_address, const void* const data_addr, int size )
{
    int status;
    int write_size;
    int max_write_size = 256;
    unsigned char enable_before_every_write = 1;
    unsigned char* data_addr_ptr = (unsigned char*) data_addr;
    unsigned char curr_device_address[3];

    if ( handle->write_allowed == SFLASH_WRITE_ALLOWED )
    {
    }
    else
    {
        return -1;
    }

    /* Some manufacturers support programming an entire page in one command. */

#ifdef SFLASH_SUPPORT_MACRONIX_PARTS
    if ( SFLASH_MANUFACTURER( handle->device_id ) == SFLASH_MANUFACTURER_MACRONIX )
    {
        max_write_size = 256;  /* TODO: this should be 256, but that causes write errors */
        enable_before_every_write = 1;
    }
#endif /* ifdef SFLASH_SUPPORT_MACRONIX_PARTS */

#ifdef SFLASH_SUPPORT_WINBOND_PARTS
    if ( SFLASH_MANUFACTURER( handle->device_id ) == SFLASH_MANUFACTURER_WINBOND )
    {
        max_write_size = 256;  /* TODO: this should be 256, but that causes write errors */
        enable_before_every_write = 1;
    }
#endif /* ifdef SFLASH_SUPPORT_MACRONIX_PARTS */

#ifdef SFLASH_SUPPORT_SST_PARTS
    if ( SFLASH_MANUFACTURER( handle->device_id ) == SFLASH_MANUFACTURER_SST )
    {
        max_write_size = 1;
        enable_before_every_write = 1;
    }
#endif /* ifdef SFLASH_SUPPORT_SST_PARTS */
#ifdef SFLASH_SUPPORT_EON_PARTS
    if ( SFLASH_MANUFACTURER( handle->device_id ) == SFLASH_MANUFACTURER_EON )
    {
        max_write_size = (unsigned int) 1;
        enable_before_every_write = 1;
    }
#endif /* ifdef SFLASH_SUPPORT_EON_PARTS */


    if ( ( enable_before_every_write == 0 ) &&
         ( 0 != ( status = sflash_write_enable( handle ) ) ) )
    {
        return status;
    }

    /* Generic x-bytes-at-a-time write */

    while ( size > 0 )
    {
        write_size = ( size > max_write_size )? max_write_size : size;
        curr_device_address[0] = ( ( device_address & 0x00FF0000 ) >> 16 );
        curr_device_address[1] = ( ( device_address & 0x0000FF00 ) >>  8 );
        curr_device_address[2] = ( ( device_address & 0x000000FF ) >>  0 );

        if ( ( enable_before_every_write == 1 ) &&
             ( 0 != ( status = sflash_write_enable( handle ) ) ) )
        {
            return status;
        }

        if ( 0 != ( status = generic_sflash_command( handle, SFLASH_WRITE, 3, curr_device_address, write_size, data_addr_ptr, NULL ) ) )
        {
            return status;
        }

        data_addr_ptr += write_size;
        device_address += write_size;
        size -= write_size;

    }

    return 0;
}

/**
  * @brief  Writes block of data to the FLASH. In this function, the number of
  *         WRITE cycles are reduced, using Page WRITE sequence.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the FLASH.
  * @retval None
  */
int sflash_write( const sflash_handle_t* const handle, unsigned long device_address, const void* const data_addr, unsigned int size )
{
  int status;
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
  unsigned char* data_addr_ptr = (unsigned char*) data_addr;

  Addr = device_address % sFLASH_SPI_PAGESIZE;
  count = sFLASH_SPI_PAGESIZE - Addr;
  NumOfPage =  size / sFLASH_SPI_PAGESIZE;
  NumOfSingle = size % sFLASH_SPI_PAGESIZE;

  if (Addr == 0) /*!< WriteAddr is sFLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      status = sflash_write_page( handle, device_address, data_addr_ptr, size );
      //sFLASH_WritePage(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /*!< NumByteToWrite > sFLASH_PAGESIZE */
    {
      while (NumOfPage--)
      {
        status = sflash_write_page( handle, device_address, data_addr_ptr, sFLASH_SPI_PAGESIZE );
        //sFLASH_WritePage(pBuffer, WriteAddr, sFLASH_SPI_PAGESIZE);
        device_address +=  sFLASH_SPI_PAGESIZE;
        data_addr_ptr += sFLASH_SPI_PAGESIZE;
      }

      status = sflash_write_page( handle, device_address, data_addr_ptr, NumOfSingle);
    }
  }
  else /*!< WriteAddr is not sFLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      if (NumOfSingle > count) /*!< (NumByteToWrite + WriteAddr) > sFLASH_PAGESIZE */
      {
        temp = NumOfSingle - count;

        status = sflash_write_page( handle, device_address, data_addr_ptr, count );
        //sFLASH_WritePage(pBuffer, WriteAddr, count);
        device_address +=  count;
        data_addr_ptr += count;

        status = sflash_write_page( handle, device_address, data_addr_ptr, temp);
      }
      else
      {
        status = sflash_write_page(handle, device_address, data_addr_ptr, size);
      }
    }
    else /*!< NumByteToWrite > sFLASH_PAGESIZE */
    {
      size -= count;
      NumOfPage =  size / sFLASH_SPI_PAGESIZE;
      NumOfSingle = size % sFLASH_SPI_PAGESIZE;

      status = sflash_write_page( handle, device_address, data_addr_ptr, count );
      //sFLASH_WritePage(pBuffer, WriteAddr, count);
      device_address +=  count;
      data_addr_ptr += count;

      while (NumOfPage--)
      {
        status = sflash_write_page( handle, device_address, data_addr_ptr, sFLASH_SPI_PAGESIZE );
        //sFLASH_WritePage(pBuffer, WriteAddr, sFLASH_SPI_PAGESIZE);
        device_address +=  sFLASH_SPI_PAGESIZE;
        data_addr_ptr += sFLASH_SPI_PAGESIZE;
      }

      if (NumOfSingle != 0)
      {
        status = sflash_write_page( handle, device_address, data_addr_ptr, NumOfSingle );
        //sFLASH_WritePage(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
  return status;
}

int sflash_write_status_register( const sflash_handle_t* const handle, char value )
{
    char status_register_val = value;
#ifdef SFLASH_SUPPORT_SST_PARTS
    /* SST parts require enabling writing to the status register */
    if ( SFLASH_MANUFACTURER( handle->device_id ) == SFLASH_MANUFACTURER_SST )
    {
        int status;
        if ( 0 != ( status = generic_sflash_command( handle, SFLASH_ENABLE_WRITE_STATUS_REGISTER, 0, NULL, 0, NULL, NULL ) ) )
        {
            return status;
        }
    }
#endif /* ifdef SFLASH_SUPPORT_SST_PARTS */

    return generic_sflash_command( handle, SFLASH_WRITE_STATUS_REGISTER, 0, NULL, 1, &status_register_val, NULL );
}

int deinit_sflash( /*@out@*/ sflash_handle_t* const handle)
{
    int status;
    (void) handle;
    status = sflash_platform_deinit( );
    if ( status != 0 )
    {
        return status;
    }
    return 0;
}

int init_sflash( /*@out@*/ sflash_handle_t* const handle, /*@shared@*/ void* peripheral_id, sflash_write_allowed_t write_allowed_in )
{
   int status;
    device_id_t tmp_device_id;

    status = sflash_platform_init( peripheral_id, &handle->platform_peripheral );
    if ( status != 0 )
    {
        return status;
    }

    handle->write_allowed = write_allowed_in;
    handle->device_id     = 0;

    status = sflash_read_ID( handle, &tmp_device_id );
    if ( status != 0 )
    {
        return status;
    }

    handle->device_id = ( ((uint32_t) tmp_device_id.id[0]) << 16 ) +
                        ( ((uint32_t) tmp_device_id.id[1]) <<  8 ) +
                        ( ((uint32_t) tmp_device_id.id[2]) <<  0 );


    if ( write_allowed_in == SFLASH_WRITE_ALLOWED )
    {
        /* Enable writing */
        if (0 != ( status = sflash_write_enable( handle ) ) )
        {
            return status;
        }
    }

    return 0;
}

static inline int is_write_command( sflash_command_t cmd )
{
    return ( ( cmd == SFLASH_WRITE             ) ||
             ( cmd == SFLASH_CHIP_ERASE1       ) ||
             ( cmd == SFLASH_CHIP_ERASE2       ) ||
             ( cmd == SFLASH_SECTOR_ERASE      ) ||
             ( cmd == SFLASH_BLOCK_ERASE_MID   ) ||
             ( cmd == SFLASH_BLOCK_ERASE_LARGE ) )? 1 : 0;
}


int generic_sflash_command(                                      const sflash_handle_t* const handle,
                                                                 sflash_command_t             cmd,
                                                                 unsigned long                num_initial_parameter_bytes,
                            /*@null@*/ /*@observer@*/            const void* const            parameter_bytes,
                                                                 unsigned long                num_data_bytes,
                            /*@null@*/ /*@observer@*/            const void* const            data_MOSI,
                            /*@null@*/ /*@out@*/ /*@dependent@*/ void* const                  data_MISO )
{
    int status;
    
    sflash_platform_message_segment_t segments[3] =
    {
            { &cmd,            NULL,       (unsigned long) 1 },
            { parameter_bytes, NULL,       num_initial_parameter_bytes },
            /*@-compdef@*/ /* Lint: Tell lint that it is OK that data_MISO is not completely defined */
            { data_MOSI,       data_MISO,  num_data_bytes }
            /*@+compdef@*/
    };


    status = sflash_platform_send_recv( handle->platform_peripheral, segments, (unsigned int) 3  );

    if ( status != 0 )
    {
        /*@-mustdefine@*/ /* Lint: do not need to define data_MISO due to failure */
        return status;
        /*@+mustdefine@*/
    }

    if ( is_write_command( cmd ) == 1 )
    {
        unsigned char status_register;
        /* write commands require waiting until chip is finished writing */

        do
        {
            status = sflash_read_status_register( handle, &status_register );
            if ( status != 0 )
            {
                /*@-mustdefine@*/ /* Lint: do not need to define data_MISO due to failure */
                return status;
                /*@+mustdefine@*/
            }
        } while( ( status_register & SFLASH_STATUS_REGISTER_BUSY ) != (unsigned char) 0 );

    }

    /*@-mustdefine@*/ /* Lint: lint does not realise data_MISO was set by sflash_platform_send_recv */
    return 0;
    /*@+mustdefine@*/
}


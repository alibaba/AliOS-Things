
#include "spi_flash_platform_interface.h"
#include "mico_platform.h"

#if defined ( USE_MICO_SPI_FLASH )
extern const platforom_spi_device_t mico_spi_flash;

int sflash_platform_init ( /*@shared@*/ void* peripheral_id, /*@out@*/ void** platform_peripheral_out )
{
    UNUSED_PARAMETER( peripheral_id );  /* Unused due to single SPI Flash */

    if ( kNoErr != MicoSpiInitialize( &mico_spi_flash ) )
    {
        /*@-mustdefine@*/ /* Lint: failed - do not define platform peripheral */
        return -1;
        /*@+mustdefine@*/
    }

    if( platform_peripheral_out != NULL)
      *platform_peripheral_out = NULL;
    
    return 0;
}


extern int sflash_platform_send_recv ( const void* platform_peripheral, /*@in@*/ /*@out@*/ sflash_platform_message_segment_t* segments, unsigned int num_segments  )
{
    UNUSED_PARAMETER( platform_peripheral );

    if ( kNoErr != MicoSpiTransfer( &mico_spi_flash, (platform_spi_message_segment_t*) segments, (uint16_t) num_segments ) )
    {
        return -1;
    }

    return 0;
}

int sflash_platform_deinit( void )
{
    if ( kNoErr != MicoSpiFinalize( &mico_spi_flash ) )
    {
        /*@-mustdefine@*/ /* Lint: failed - do not define platform peripheral */
        return -1;
        /*@+mustdefine@*/
    }

    return 0;
}

#else
int sflash_platform_init( /*@shared@*/ void* peripheral_id, /*@out@*/ void** platform_peripheral_out )
{
    UNUSED_PARAMETER( peripheral_id );
    UNUSED_PARAMETER( platform_peripheral_out );
    return -1;
}

extern int sflash_platform_send_recv( const void* platform_peripheral, /*@in@*//*@out@*/sflash_platform_message_segment_t* segments, unsigned int num_segments )
{
    UNUSED_PARAMETER( platform_peripheral );
    UNUSED_PARAMETER( segments );
    UNUSED_PARAMETER( num_segments );
    return -1;
}

int sflash_platform_deinit( void )
{
    return -1;
}
#endif

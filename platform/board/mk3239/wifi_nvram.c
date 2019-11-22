
#include <stdint.h>
#include <wifi_nvram_image.h>

uint32_t host_platform_memory_wifi_nvram_size( void )
{
    return sizeof(wifi_nvram_image);
}

uint8_t* host_platform_read_wifi_nvram_image( int offset )
{
    return (uint8_t*) &wifi_nvram_image[offset];
}

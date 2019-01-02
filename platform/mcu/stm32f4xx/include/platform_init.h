
#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int main( void );

extern void platform_init_system_clocks( void );
extern void init_clocks( void );

extern void platform_init_memory( void );

extern void platform_init_mcu_infrastructure( void );

extern void platform_init_connectivity_module( void );

extern void platform_init_external_devices( void );

extern void platform_init_peripheral_irq_priorities( void );

extern void platform_init_rtos_irq_priorities( void );

void init_clocks( void );
void init_memory( void );
void init_architecture( void) ;
void init_platform( void) ;
void init_platform_bootloader( void );
void startApplication( uint32_t app_addr );

#ifdef __cplusplus
} /*extern "C" */
#endif

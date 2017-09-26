/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Main
 *
 * @param[in] : void
 * @return    : int
 *
 * @usage
 * \li Defined by RTOS or application and called by CRT0
 */
int main( void );

/**
 * Initialise system clock(s)
 * This function includes initialisation of PLL and switching to fast clock
 *
 * @param[in] : void
 * @return    : void
 *
 * @usage
 * \li Defined internally in platforms/MCU/<MCU>/platform_init.c and called by CRT0
 * \li Weakly defined in platforms/MCU/<MCU>/platform_init.c. Users may override it as desired
 */
extern void platform_init_system_clocks( void );
extern void init_clocks( void );

/**
 * Initialise memory subsystem
 * This function initialises memory subsystem such as external RAM
 *
 * @param[in] : void
 * @return    : void
 *
 * @usage
 * \li Defined internally in platforms/MCU/<MCU>/platform_init.c and called by CRT0
 * \li Weakly defined in platforms/MCU/<MCU>/platform_init.c. Users may override it as desired
 */
extern void platform_init_memory( void );

/**
 * Initialise default MCU infrastructure
 * This function initialises default MCU infrastructure such as watchdog
 *
 * @param[in] : void
 * @return    : void
 *
 * @usage
 * \li Defined and used internally in platforms/MCU/<MCU>/platform_init.c
 */
extern void platform_init_mcu_infrastructure( void );

/**
 * Initialise connectivity module(s)
 * This function initialises and puts connectivity modules (Wi-Fi, Bluetooth, etc) into their reset state
 *
 * @param[in] : void
 * @return    : void
 *
 * @usage
 * \li Defined and used internally in platforms/MCU/<MCU>/platform_init.c
 */
extern void platform_init_connectivity_module( void );

/**
 * Initialise external devices
 * This function initialises and puts external peripheral devices on the board such as LEDs, buttons, sensors, etc into their reset state
 *
 * @param[in] : void
 * @return    : void
 *
 * @usage     :
 * \li MUST be defined in platforms/<Platform>/platform.c
 * \li Called by @ref platform_init_mcu_infrastructure()
 */
extern void platform_init_external_devices( void );

/**
 * Initialise priorities of interrupts used by the platform peripherals
 *
 * @param[in] : void
 * @return    : void
 *
 * @usage
 * \li MUST be defined in platforms/<Platform>/platform.c
 * \li Called by @ref platform_init_mcu_infrastructure()
 */
extern void platform_init_peripheral_irq_priorities( void );

/**
 * Initialise priorities of interrupts used by the RTOS
 *
 * @param[in] : void
 * @return    : void

 * @usage
 * \li MUST be defined by the RTOS
 * \li Called by @ref platform_init_mcu_infrastructure()
 */
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

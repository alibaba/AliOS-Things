/*
 *  Copyright (c) 2018, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file includes the platform-specific configuration.
 *
 */

/**
 * @def OPENTHREAD_SIMULATION_UART_BAUDRATE
 *
 * This setting configures the baud rate of the UART.
 *
 */
#ifndef OPENTHREAD_SIMULATION_UART_BAUDRATE
#define OPENTHREAD_SIMULATION_UART_BAUDRATE B115200
#endif

/**
 * @def OPENTHREAD_SIMULATION_VIRTUAL_TIME
 *
 * This setting configures whether to use virtual time (used for simulation) in simulation platform.
 *
 */
#ifndef OPENTHREAD_SIMULATION_VIRTUAL_TIME
#define OPENTHREAD_SIMULATION_VIRTUAL_TIME 0
#endif

/**
 * @def OPENTHREAD_SIMULATION_VIRTUAL_TIME_UART
 *
 * This setting configures whether to use virtual time for UART.
 *
 */
#ifndef OPENTHREAD_SIMULATION_VIRTUAL_TIME_UART
#define OPENTHREAD_SIMULATION_VIRTUAL_TIME_UART 0
#endif

/**
 * @def OPENTHREAD_PLATFORM_USE_PSEUDO_RESET
 *
 * Define as 1 to enable pseudo-reset.
 *
 */
#ifndef OPENTHREAD_PLATFORM_USE_PSEUDO_RESET
#define OPENTHREAD_PLATFORM_USE_PSEUDO_RESET 0
#endif

/**
 * @def OPENTHREAD_CONFIG_NCP_SPI_ENABLE
 *
 * Define as 1 to enable SPI NCP interface.
 *
 */
#ifndef OPENTHREAD_CONFIG_NCP_SPI_ENABLE
#define OPENTHREAD_CONFIG_NCP_SPI_ENABLE 0
#endif

/**
 * Check OTNS configurations
 *
 */
#if OPENTHREAD_CONFIG_OTNS_ENABLE

#if !OPENTHREAD_SIMULATION_VIRTUAL_TIME
#error "OTNS requires virtual time simulations"
#endif

#endif // OPENTHREAD_CONFIG_OTNS_ENABLE

/**
 * @def OPENTHREAD_SIMULATION_MAX_NETWORK_SIZE
 *
 * This setting configures the maximum network size in simulation.
 *
 */
#ifndef OPENTHREAD_SIMULATION_MAX_NETWORK_SIZE
#define OPENTHREAD_SIMULATION_MAX_NETWORK_SIZE 33
#endif

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/** @mainpage MICO 

    This documentation describes the MICO APIs.
    It consists of:
     - MICO Core APIs   
     - MICO Hardware Abstract Layer APIs    
     - MICO Algorithm APIs        
     - MICO System APIs        
     - MICO Middleware APIs
     - MICO Drivers interface
 */

#ifndef __MICO_H_
#define __MICO_H_

/* MiCO SDK APIs */
#include "debug.h"
#include "common.h"
#include <hal/hal.h>

#include "mico_rtos.h"
//#include "mico_socket.h"
//#include "mico_security.h"
#include "mico_platform.h"
//#include "mico_system.h"


#define MicoGetRfVer                wlan_driver_version
#define MicoGetVer                  system_lib_version
#define MicoInit                    mxchipInit

/** @defgroup MICO_Core_APIs MICO Core APIs
  * @brief MiCO Initialization, RTOS, TCP/IP stack, and Network Management
  */

/** @addtogroup MICO_Core_APIs
  * @{
  */

/** \defgroup MICO_Init_Info Initialization and Tools
  * @brief Get MiCO version or RF version, flash usage information or init MiCO TCPIP stack
  * @{
 */

 /******************************************************
 *                    Structures
 ******************************************************/



/******************************************************
 *              Function Declarations
 ******************************************************/

/**
  * @brief  Get RF driver's version.
  *
  * @note   Create a memery buffer to store the version characters.
  *         THe input buffer length should be 40 bytes at least.
  * @note   This must be executed after micoInit().
  * @param  inVersion: Buffer address to store the RF driver. 
  * @param  inLength: Buffer size. 
  *
  * @return int
  */
int MicoGetRfVer( char* outVersion, uint8_t inLength );

/**
  * @brief  Get MICO's version.
  *
  * @param  None 
  *
  * @return Point to the MICO's version string.
  */
char* MicoGetVer( void );

/**
  * @brief  Initialize the TCPIP stack thread, RF driver thread, and other
            supporting threads needed for wlan connection. Do some necessary
            initialization
  *
  * @param  None
  *
  * @return kNoErr: success, kGeneralErr: fail
  */
OSStatus MicoInit( void );


/**
  * @brief  Get an identifier id from device, every id is unique and will not change in life-time
  *
  * @param  identifier length 
  *
  * @return Point to the identifier 
  */
const uint8_t* mico_generate_cid( uint8_t *length );

#endif /* __MICO_H_ */

/**
  * @}
  */

/**
  * @}
  */


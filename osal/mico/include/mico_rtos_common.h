/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
#define rtos_log(M, ...) custom_log("RTOS", M, ##__VA_ARGS__)
#define rtos_log_trace() custom_log_trace("RTOS")

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/* MiCO <-> RTOS API */
extern OSStatus mico_rtos_init  ( void );
extern OSStatus mico_rtos_deinit( void );

/* Entry point for user Application */
extern void application_start          ( void );


#ifdef __cplusplus
} /* extern "C" */
#endif

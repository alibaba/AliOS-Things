/*****************************************************************************
 **
 **  Name:          mico_bt_logmsg.c
 **
 **  Description: Contains the LogMsg wrapper routines for BTE.  It routes calls
 **               the appropriate application's LogMsg equivalent.
 **
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "bt_types.h"
#include "buildcfg.h"
#include "data_types.h"
#include "mico_rtos.h"

extern int mico_debug_enabled;
extern mico_mutex_t stdio_tx_mutex;

static void printlog(char *M, char *N)
{
    if (mico_debug_enabled == 0) {
        return;
    }
    mico_rtos_lock_mutex( &stdio_tx_mutex );
    printf("[%ld][%s] %s\r\n", mico_rtos_get_time(), M, N);
    mico_rtos_unlock_mutex( &stdio_tx_mutex );
}
void WPRINT_BT_APP_INFO(const char *info, ...)
{
    char buffer[256]; // Save stack space - make global
    va_list ap;
    va_start(ap, info);
    vsprintf(buffer, info, ap);
    va_end(ap);

    printlog("BTAPP",  buffer);
}

#if BT_USE_TRACES == TRUE

//extern mico_mutex_t global_trace_mutex;

UINT8 mico_log_enabled = 1;

void
LogMsg(UINT32 trace_set_mask, const char *fmt_str, ...)
{
    char buffer[256]; // Save stack space - make global
    //    char timeBuf[16];
    va_list ap;

    if ((!mico_log_enabled) && (TRACE_GET_TYPE(trace_set_mask) != TRACE_TYPE_ERROR)) {
        /* If  mico logging disabled, then only log errors */
        return;
    }


    va_start(ap, fmt_str);
    vsprintf(buffer, fmt_str, ap);
    va_end(ap);

    printlog("BTLOG",  buffer);
    //printf("[btlog] %s\r\n",  buffer);
}

void
ScrLog(UINT32 trace_set_mask, const char *fmt_str, ...)
{
    char buffer[256]; // Save stack space - make global
    va_list ap;


    if ((!mico_log_enabled) && (TRACE_GET_TYPE(trace_set_mask) != TRACE_TYPE_ERROR)) {
        /* If  mico logging disabled, then only log errors */
        return;
    }

    va_start(ap, fmt_str);
    vsprintf(buffer, fmt_str, ap);
    va_end(ap);

    printlog("BTLOG",  buffer);
}


/********************************************************************************
 **
 **    Function Name:   LogMsg_0
 **
 **    Purpose:  Encodes a trace message that has no parameter arguments
 **
 **    Input Parameters:  trace_set_mask: tester trace type.
 **                       fmt_str: displayable string.
 **    Returns:
 **                      Nothing.
 **
 *********************************************************************************/
void LogMsg_0( UINT32 trace_set_mask, const char *fmt_str )
{
    LogMsg( trace_set_mask, fmt_str );
}

/********************************************************************************
 **
 **    Function Name:   LogMsg_1
 **
 **    Purpose:  Encodes a trace message that has one parameter argument
 **
 **    Input Parameters:  trace_set_mask: tester trace type.
 **                       fmt_str: displayable string.
 **    Returns:
 **                      Nothing.
 **
 *********************************************************************************/
void LogMsg_1( UINT32 trace_set_mask, const char *fmt_str, UINTPTR p1 )
{
    LogMsg( trace_set_mask, fmt_str, p1 );
}

/********************************************************************************
 **
 **    Function Name:   LogMsg_2
 **
 **    Purpose:  Encodes a trace message that has two parameter arguments
 **
 **    Input Parameters:  trace_set_mask: tester trace type.
 **                       fmt_str: displayable string.
 **    Returns:
 **                      Nothing.
 **
 *********************************************************************************/
void LogMsg_2( UINT32 trace_set_mask, const char *fmt_str, UINTPTR p1, UINTPTR p2 )
{
    LogMsg( trace_set_mask, fmt_str, p1, p2 );
}

/********************************************************************************
 **
 **    Function Name:   LogMsg_3
 **
 **    Purpose:  Encodes a trace message that has three parameter arguments
 **
 **    Input Parameters:  trace_set_mask: tester trace type.
 **                       fmt_str: displayable string.
 **    Returns:
 **                      Nothing.
 **
 *********************************************************************************/
void LogMsg_3( UINT32 trace_set_mask, const char *fmt_str, UINTPTR p1, UINTPTR p2, UINTPTR p3 )
{
    LogMsg( trace_set_mask, fmt_str, p1, p2, p3 );
}

/********************************************************************************
 **
 **    Function Name:   LogMsg_4
 **
 **    Purpose:  Encodes a trace message that has four parameter arguments
 **
 **    Input Parameters:  trace_set_mask: tester trace type.
 **                       fmt_str: displayable string.
 **    Returns:
 **                      Nothing.
 **
 *********************************************************************************/
void LogMsg_4( UINT32 trace_set_mask, const char *fmt_str, UINTPTR p1, UINTPTR p2, UINTPTR p3, UINTPTR p4 )
{
    LogMsg( trace_set_mask, fmt_str, p1, p2, p3, p4 );
}

/********************************************************************************
 **
 **    Function Name:   LogMsg_5
 **
 **    Purpose:  Encodes a trace message that has five parameter arguments
 **
 **    Input Parameters:  trace_set_mask: tester trace type.
 **                       fmt_str: displayable string.
 **    Returns:
 **                      Nothing.
 **
 *********************************************************************************/
void LogMsg_5( UINT32 trace_set_mask, const char *fmt_str, UINTPTR p1, UINTPTR p2, UINTPTR p3, UINTPTR p4, UINTPTR p5 )
{
    LogMsg( trace_set_mask, fmt_str, p1, p2, p3, p4, p5 );
}

/********************************************************************************
 **
 **    Function Name:   LogMsg_6
 **
 **    Purpose:  Encodes a trace message that has six parameter arguments
 **
 **    Input Parameters:  trace_set_mask: tester trace type.
 **                       fmt_str: displayable string.
 **    Returns:
 **                      Nothing.
 **
 *********************************************************************************/
void LogMsg_6( UINT32 trace_set_mask, const char *fmt_str, UINTPTR p1, UINTPTR p2, UINTPTR p3, UINTPTR p4, UINTPTR p5,
               UINTPTR p6 )
{
    LogMsg( trace_set_mask, fmt_str, p1, p2, p3, p4, p5, p6 );
}
#endif

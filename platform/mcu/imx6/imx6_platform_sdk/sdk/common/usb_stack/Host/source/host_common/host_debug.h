#ifndef __debug_h__
#define __debug_h__
/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: host_debug.h$
* $Version : 3.4.6.0$
* $Date    : Sep-11-2009$
*
* Comments:
*
*   This file contains definitions for debugging the software stack
*
*END************************************************************************/
#include "types.h"

/*--------------------------------------------------------------**
** Debug macros, assume _DBUG_ is defined during development    **
**    (perhaps in the make file), and undefined for production. **
**--------------------------------------------------------------*/
// EAI #define	_DBUG_
#ifndef _DBUG_
   #define DEBUG_FLUSH
   #define DEBUG_PRINT(X)
   #define DEBUG_PRINT2(X,Y)
#else
   #define DEBUG_FLUSH        fflush(stdout);
   #define DEBUG_PRINT(X)     printf(X); 
   #define DEBUG_PRINT2(X,Y)  printf(X,Y);
#endif


/*--------------------------------------------------------------**
** ASSERT macros, assume _ASSERT_ is defined during development **
**    (perhaps in the make file), and undefined for production. **
** This macro will check for pointer values and other validations
** wherever appropriate.
**--------------------------------------------------------------*/
#define	_ASSERT_
#ifndef _ASSERT_
   #define ASSERT(X,Y)
#else
   #define ASSERT(X,Y) if(Y) { printf(X); exit(1);}
#endif


/************************************************************
The following array is used to make a run time trace route
inside the USB stack.
*************************************************************/
//#define _DEBUG_INFO_TRACE_LEVEL_
//#define _HOST_DEBUG_
#ifdef _DEBUG_INFO_TRACE_LEVEL_

#define TRACE_ARRAY_SIZE 1000
#define MAX_STRING_SIZE  50

#ifndef __TRACE_VARIABLES_DEFINED__
extern uint_16 DEBUG_TRACE_ARRAY_COUNTER;
extern char   DEBUG_TRACE_ARRAY[TRACE_ARRAY_SIZE][MAX_STRING_SIZE];
extern boolean DEBUG_TRACE_ENABLED;
#else
uint_16 DEBUG_TRACE_ARRAY_COUNTER;
char   DEBUG_TRACE_ARRAY[TRACE_ARRAY_SIZE][MAX_STRING_SIZE];
boolean DEBUG_TRACE_ENABLED;
#endif

#if 0
#define DEBUG_LOG_TRACE(x) \
{ \
  if(DEBUG_TRACE_ENABLED) \
 { \
	 USB_mem_copy(x,DEBUG_TRACE_ARRAY[DEBUG_TRACE_ARRAY_COUNTER],MAX_STRING_SIZE);\
	 DEBUG_TRACE_ARRAY_COUNTER++;\
	 if(DEBUG_TRACE_ARRAY_COUNTER >= TRACE_ARRAY_SIZE) \
	 {DEBUG_TRACE_ARRAY_COUNTER = 0;}\
 }\
}

#else

#define DEBUG_LOG_TRACE(x) printf("\n%s",x);

#endif

#define START_DEBUG_TRACE \
{ \
	DEBUG_TRACE_ARRAY_COUNTER =0;\
	DEBUG_TRACE_ENABLED = TRUE;\
} 

#define STOP_DEBUG_TRACE \
{ \
	DEBUG_TRACE_ENABLED = FALSE;\
} 

/*if trace switch is not enabled define debug log trace to empty*/
#else
	#define DEBUG_LOG_TRACE(x)
	#define START_DEBUG_TRACE
	#define STOP_DEBUG_TRACE
#endif


/************************************************************
The following are global data structures that can be used
to copy data from stack on run time. This structure can
be analyzed at run time to see the state of various other
data structures in the memory.
*************************************************************/


#ifdef _DEBUG_INFO_DATA_LEVEL_

typedef struct debug_data {
} DEBUG_DATA_STRUCT,_PTR_ DEBUG_DATA_STRUCT_PTR;

#endif

/**************************************************************
	The following lines assign numbers to each of the routines
	in the stack. These numbers can be used to generate a trace
	on sequenece of routines entered.
**************************************************************/



#endif
/* EOF */

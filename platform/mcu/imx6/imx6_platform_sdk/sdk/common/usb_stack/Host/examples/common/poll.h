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
* $FileName: poll.h$
* $Version : 
* $Date    : 
*
* Comments:
*
*   This file describes data structure and function prototytes are used in polling functionality.
*
*END************************************************************************/
#ifndef _POLL_H_
#define _POLL_H_
#include "psptypes.h"

/* Maximum number of functions called every time Poll function is invoked */
#define POLL_MAX_NUM  5

/* return code */
#define POLL_OK            0 /* register successfully */
#define POLL_REGISTER_FAIL 1 /* can NOT register more function to poll*/
#define POLL_NOT_FOUND     2 /* polling function pointer array is empty */

/* Poll function pointer */
typedef void (*POLL_POINTER)(void);

/* Poll structure */
typedef struct{
   POLL_POINTER   p_func[POLL_MAX_NUM]; /* polling function pointer array */
   uint_8         registered_no;  /* number of registered function */
}POLL_STRUCT, *PTR_POOL_STRUCT;

/* Register funtion to poll */
uint_8 POLL_register(POLL_POINTER);

/* Poll function to call in while loop */
void Poll(void);

/* Init POLL_STRUCT object */
void POLL_init(void);

/* Unregister polling function */
uint_8 POLL_unregister(POLL_POINTER);

#endif /* End of #ifndef _POLL_H_ */
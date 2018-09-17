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
* $FileName: poll.c$
* $Version : 
* $Date    : 
*
* Comments:
*
*   This file implements polling functionality.
*
*END************************************************************************/

#include "poll.h"

/* poll global variale */

POLL_STRUCT g_poll;

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : POLL_init
* Returned Value   : void
* Comments         : Init POLL_STRUCT object
*   
*END*----------------------------------------------------------------------*/

void POLL_init() 
{
  uint_8 i;
  
  g_poll.registered_no = 0; 
  for (i = 0; i <POLL_MAX_NUM; i++)
  {
    g_poll.p_func[i] = NULL;
  }
 
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : POLL_register
* Returned Value   : POLL_OK: register successfully
*                    POLL_REGISTER_FAIL: can NOT register more function to poll
* Comments         : Register funtion to poll
*   
*END*----------------------------------------------------------------------*/

uint_8 POLL_register(POLL_POINTER func) 
{
  if (POLL_MAX_NUM <= g_poll.registered_no)
  {
    return POLL_REGISTER_FAIL;
  }
  else
  {
    g_poll.p_func[g_poll.registered_no] = func;
    g_poll. registered_no ++;
    return POLL_OK;
  }
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : Poll
* Returned Value   : void
* Comments         : Poll function to call in while loop
*   
*END*----------------------------------------------------------------------*/
void Poll() 
{

  uint_8 i;
  
  for (i = 0; i < g_poll.registered_no; i++)
  {
    g_poll.p_func[i]();
  }
  
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : Poll
* Returned Value   : POLL_OK: register successfully
*                    POLL_NOT_FOUND: Nothing to unregister
* Comments         : Unregister polling function
*   
*END*----------------------------------------------------------------------*/
uint_8 POLL_unregister(POLL_POINTER func) 
{
  uint_8 i, j =0, ret = POLL_NOT_FOUND;
  
  POLL_POINTER temp[POLL_MAX_NUM] = {NULL};
  
  /* find and unregister func */
  for(i = 0; i < POLL_MAX_NUM; i++) 
  {
    if(g_poll.p_func[i] == func)
    {
       g_poll.p_func[i] = NULL;
       g_poll. registered_no--;
       ret = POLL_OK;
    }
    else
    {
      temp[j] = g_poll.p_func[i];
      j++;
    }
  }
  
  /* sort the function pointer erray */
  for(i = 0; i < POLL_MAX_NUM; i++) 
  {
    g_poll.p_func[i] = temp [i];
  }
  return ret;
}


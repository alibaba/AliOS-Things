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
* $FileName: testcase.h$
* $Version : 
* $Date    : 
*
* Comments:
*
*   
*
*END************************************************************************/
#ifndef _TEST_CASE_H_
#define _TEST_CASE_H_

#include "tmp.h"

typedef struct tTestCase 
{
  unsigned TestCaseNo;
  const char* const pTextCaseTitle;
  unsigned char(*pTestCaseFxn)();
} tTestCase;

extern const tTestCase aTestCases[];

#define CHK(_ASSUMPTION_)                                       \
  if (!(_ASSUMPTION_))                                          \
  {                                                             \
    printf ("Error in file `%s' on line %u (res=%u)!\n",        \
            __FILE__, (unsigned)__LINE__, (unsigned)res);       \
    goto lend;                                                  \
  }

#endif 

#define ALL 0

#define NUM_REPEAT  10

#define RUN_TEST_101_111_201_209_301  
//#define RUN_TEST_112

//#define RUN_TEST_210_213

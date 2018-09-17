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
* $FileName: fattest.c$
* $Version : 
* $Date    : 
*
* Comments:
*
*   
*
*END************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "ff.h"
#include "testcase.h"

/**************************************************************************
   Funciton Prototypes
**************************************************************************/
int fattest(void);
int fattestcase(int arg);

/**************************************************************************
   Global variables
**************************************************************************/
/* Test case to be executed 
	 ex: 
	 execute test cases 101 :
		 int testcase = 101
	 execute all test cases:
		 int testcase = ALL
 
 */ 
int testcase = ALL;	

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : fattest
* Returned Value   : 0: 		Pass.
*                    1: 		Fail.
*                    
* Comments         : Execute test fat module 
*   
*END*----------------------------------------------------------------------*/
					
int fattest(void)
{
	return fattestcase(testcase);	
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : fattestcase
* Returned Value   : 0: 		Pass.
*                    1: 		Fail.
*                    
* Comments         : Execute test fat module with specified test cases
*   
*END*----------------------------------------------------------------------*/

int fattestcase(int arg)
{
  uint_8 index, startIndex = 0;
  uint_32 stopIndex = UINT_MAX;
  uint_8 ExecutedCnt = 0, PassedCnt = 0;
  int res = 1;
  
  printf ("Test Cases:\n");
  for (index = 0; index < UINT_MAX; index++) 
  {
    if ((aTestCases[index].pTextCaseTitle == NULL) ||(aTestCases[index].pTestCaseFxn == NULL))
        break;
    printf ("%5u: %s\n",aTestCases[index].TestCaseNo,aTestCases[index].pTextCaseTitle);
  }
  printf ("\n");
  
  if (arg != ALL)
  {
    for (index = 0; index < UINT_MAX; index++)
    {
      if ((aTestCases[index].pTextCaseTitle == NULL) ||
          (aTestCases[index].pTestCaseFxn == NULL))
      {
        printf ("Wrong test case number %s\n", arg);
      }
      if (aTestCases[index].TestCaseNo == arg)
      {
        stopIndex = startIndex = index;
        break;
      }
    }
  }

  for (index = startIndex; index <= stopIndex; index++)
  {
    uint_8 TestRes;

    if ((aTestCases[index].pTextCaseTitle == NULL) || (aTestCases[index].pTestCaseFxn == NULL))
      break;

    ExecutedCnt++;

    printf ("Test case %u: %s\n", aTestCases[index].TestCaseNo,aTestCases[index].pTextCaseTitle);
    
    TestRes = aTestCases[index].pTestCaseFxn();
		if (TestRes)
    {
       printf("Test case failed\n\n");
    }
     else
    {
       PassedCnt++;
       printf("Test case passed\n\n");
      }
    }
    
  if (PassedCnt == ExecutedCnt)
    res = 0;

  printf ("\nTest cases:\nExecuted: %u, Passed: %u, Failed: %u\n\n",
          ExecutedCnt, PassedCnt, ExecutedCnt - PassedCnt);

  return res;
}

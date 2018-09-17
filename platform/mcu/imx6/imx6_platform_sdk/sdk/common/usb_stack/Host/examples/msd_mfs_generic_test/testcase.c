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
* $FileName: testcase.c$
* $Version : 
* $Date    : 
*
* Comments:
*
*   
*
*END************************************************************************/
#include <stdio.h>
#include <limits.h>
#include "testcase.h"
#include "ff.h"
#include "diskio.h"

/* Definitions on sector size */
#if _MAX_SS != 512 && _MAX_SS != 1024 && _MAX_SS != 2048 && _MAX_SS != 4096
#error Wrong sector size.
#endif
#if _MAX_SS != 512
#define	SS(fs)	((fs)->ssize)	  /* Multiple sector size */
#else
#define	SS(fs)	512U			      /* Fixed sector size */
#endif


#ifdef RUN_TEST_101_111_201_209_301
 #if (_FS_TINY == 0)
  #error "_FS_TINY == 0: Please enable using sharing sector buffer (_FS_TINY = 1)"
 #endif
 #if (_FS_READONLY == 1)
  #error "_FS_READONLY == 1: Please disable read only configurarion (_FS_READONLY = 0)"
 #endif
 #if (_FS_MINIMIZE > 0)
   #error "_FS_MINIMIZE == 0, Please set _FS_MINIMIZE  to 0"
 #endif
  #if (_USE_STRFUNC == 0)
    #error "_USE_STRFUNC == 0: Please anable using string function (_USE_STRFUNC = 1)"
  #endif
  #if (_USE_FORWARD == 0)
    #error "_USE_FORWARD == 0: Please anable using forward function(_USE_FORWARD = 1)"
  #endif
  #if (_USE_LFN != 3)
    #error "_USE_LFN != 3: Please enable LFN with dynamic working buffer on the HEAP"
  #endif  
  #if (_MAX_LFN != 255)
    #error "_MAX_LFN != 255: Please set _MAX_LFN to 255"
  #endif  
  #if (_FS_RPATH != 2)
    #error "_FS_RPATH != 2: Please set _FS_RPATH to 2 " 
  #endif   
 #if (_MULTI_PARTITION != 0)
    #error "_MULTI_PARTITION == 0: Please diable multi-partition option"
	#endif
	#if (_VOLUMES != 1) 
    #error "_VOLUMES != 1: Please set _VOLUMES to 1"
  #endif
#endif /* End of #ifdef RUN_TEST_101_111_201_209 */

#ifdef RUN_TEST_112
   #if (_FS_TINY == 0)
  #error "_FS_TINY == 0: Please enable using sharing sector buffer (_FS_TINY = 1)"
 #endif
 #if (_FS_READONLY == 1)
  #error "_FS_READONLY == 1: Please disable read only configurarion (_FS_READONLY = 0)"
 #endif
 #if (_FS_MINIMIZE > 0)
   #error "_FS_MINIMIZE == 0, Please set _FS_MINIMIZE  to 0"
 #endif
  #if (_MULTI_PARTITION == 0)
    #error "_MULTI_PARTITION == 0: Please enable multi-partition option"
  #endif
  #if (_VOLUMES != 2)
     #error "_VOLUMES != 2: Please set _VOLUMES to 2"
  #endif
  #if (_FS_RPATH != 2)
    #error "_FS_RPATH != 2: Please set _FS_RPATH to 2 " 
  #endif   
#endif /* End of #ifdef RUN_TEST_112 */

 #ifdef RUN_TEST_210_213
  #if (_FS_TINY == 0)
  #error "_FS_TINY == 0: Please enable using sharing sector buffer (_FS_TINY = 1)"
 #endif
 #if (_FS_READONLY == 1)
  #error "_FS_READONLY == 1: Please disable read only configurarion (_FS_READONLY = 0)"
 #endif
 #if (_FS_MINIMIZE > 0)
   #error "_FS_MINIMIZE == 0, Please set _FS_MINIMIZE  to 0"
 #endif
  #if (_FS_RPATH != 0)
    #error "_FS_RPATH != 0: Please set _FS_RPATH to 0 " 
  #endif
  #if (_USE_LFN != 0)
    #error "_USE_LFN != 0: Please diable LFN"
  #endif
  #if (_FS_RPATH != 0)
    #error "_FS_RPATH != 0: Please set _FS_RPATH to 0 " 
  #endif   
  #if (_FS_SHARE != 2)
    #error "_FS_SHARE != 2: Please set _FS_SHARE to 2 " 
  #endif   
  #if (_MULTI_PARTITION != 0)
    #error "_MULTI_PARTITION == 0: Please diable multi-partition option"
  #endif
  #if (_VOLUMES != 1) 
    #error "_VOLUMES != 1: Please set _VOLUMES to 1"
  #endif
 #endif /* End of #ifdef RUN_TEST_210_301 */
 
/**************************************************************************
   Global variables
**************************************************************************/
static FRESULT res;				/* Result code */
uint_8 TestDir1(void);
uint_8 TestDir2(void);
uint_8 TestDir3(void);
uint_8 TestDir4(void);
uint_8 TestDir5(void);
uint_8 TestDir6(void);
uint_8 TestDir7(void);
uint_8 TestDir8(void);
uint_8 TestDir9(void);
uint_8 TestDir10(void);
uint_8 TestDir11(void);
uint_8 TestDir12(void);
uint_8 TestFile1(void);
uint_8 TestFile2(void);
uint_8 TestFile3(void);
uint_8 TestFile4(void);
uint_8 TestFile5(void);
uint_8 TestFile6(void);
uint_8 TestFile7(void);
uint_8 TestFile8(void);
uint_8 TestFile9(void);
uint_8 TestFile10(void);
uint_8 TestFile11(void);
uint_8 TestFile12(void);   
uint_8 TestFile13(void);
uint_8 TestDirFileMixup1(void); 

const tTestCase aTestCases[] =
{
#ifdef RUN_TEST_101_111_201_209_301
  {101, "Test Directory Functions - 1: f_mkdir, f_unlink functions with 0 of recursive level.", TestDir1},
  {102, "Test Directory Functions - 2: f_mkdir in cases of invalid directory names.", TestDir2},
  {103, "Test Directory Functions - 3: f_unlink in cases of invalid directory names.", TestDir3},
  {104, "Test Directory Functions - 4: f_mkdir, f_unlink functions with 1 of recursive level.", TestDir4},
  {105, "Test Directory Functions - 5: f_mkdir, f_unlink functions with 2 of recursive level.", TestDir5},
  {106, "Test Directory Functions - 6: f_chdir, f_getcwd, f_unlink functions.", TestDir6},
  {107, "Test Directory Functions - 7: f_mkdir, f_unlink in case of many sub-directories.", TestDir7},
  {108, "Test Directory Functions - 8: f_opendir, f_readdir functions.", TestDir8},
  {109, "Test Directory Functions - 9: f_chdir function with .. directory.", TestDir9},
  {110, "Test Directory Functions - 10: f_readdir in case of there are many files in read directory.", TestDir10},
  {111, "Test Directory Functions - 11: f_stat, f_utime, f_chmod functions.", TestDir11},
#endif
#ifdef RUN_TEST_112
  {112, "Test Directory Functions - 12: f_chdrive, f_getfree, f_mount functions.", TestDir12},
#endif
#ifdef RUN_TEST_101_111_201_209_301  
  {201, "Test File Functions - 1: f_open, f_close, and f_unlink", TestFile1},
  {202, "Test File Functions - 2: f_write and f_read", TestFile2},
  {203, "Test File Functions - 3: f_lseek", TestFile3},
  {204, "Test File Functions - 4: f_stat, f_utime, f_chmod", TestFile4},
  {205, "Test File Functions - 5: f_forward", TestFile5},
  {206, "Test File Functions - 6: f_truncate", TestFile6}, 
  {207, "Test File Functions - 7: f_sync", TestFile7},
  {208, "Test File Functions - 8: f_printf, f_puts, f_putc, f_gets", TestFile8},
  {209, "Test File Functions - 9: f_rename",TestFile9},
 #endif
 #ifdef RUN_TEST_210_213
  {210, "Test File Functions - 10: file sharing policy",TestFile10},
  {211, "Test File Functions - 11: invalid drive status - FR_NOT_ENABLED ",TestFile11},
  {212, "Test File Functions - 12: LFN disable", TestFile12},
  {213, "Test File Functions - 13: _RS_PATH = 0", TestFile13},
 #endif
 #ifdef RUN_TEST_101_111_201_209_301
  {301, "File/Dir: file operations on dirs & vice versa", TestDirFileMixup1},
 #endif
  {0, NULL, NULL}
};
/**************************************************************************
   Private functions
**************************************************************************/

 uint_32 Func(const uint_8 *p, uint_32 btf);
 #if (_MULTI_PARTITION)
 
 const PARTITION VolToPart[_VOLUMES] = {{0,0},{0,1}};
 
 #endif
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TestDir1
* Returned Value   : 0: 		Pass.
*                    1: 		Fail.
*                    
* Comments         : This test group is to test  f_mkdir, f_unlink functions with 0 of recursive level  
*   
*END*----------------------------------------------------------------------*/

uint_8 TestDir1(void)
{
	FATFS *fs, fatfs;			/* File system object */	
	uint_32 free_clust1,free_clust2;
	uint_8 result = 1;

	/* Mount the logical driver 0 */
	res = f_mount(0,&fatfs);

	time_delay(1000);
	disk_initialize(0);
		
#if(defined SCSI_DEV)
	/* Send some SCSI requests first */	
	time_delay(1000);
	disk_ioctl(0, UFI_INQUIRY_CMD, NULL);
	disk_ioctl(0, UFI_READ_FORMAT_CAPACITY_CMD, NULL);
	disk_ioctl(0, REQUEST_SENSE_CMD, NULL);
	//disk_ioctl(0, UFI_READ_FORMAT_CAPACITY_CMD, NULL);
#endif	

	/* Get free cluster */
	res = f_getfree ("0:",&free_clust1,&fs);
	CHK(res == FR_OK);

	res =	f_unlink ("directory1");
	CHK(res == FR_NO_FILE);
	res =	f_unlink ("directory1");
	CHK(res == FR_NO_FILE);
	res = f_mkdir("directory1");
	CHK(res == FR_OK);
	res = f_mkdir("directory1");
	CHK(res == FR_EXIST);
	res =	f_unlink ("directory1");
	CHK(res == FR_OK);
	res =	f_unlink ("directory1");
	CHK(res == FR_NO_FILE);
	res =	f_mkdir ("directory1");
	CHK(res == FR_OK);
	res =	f_unlink ("directory1");
	CHK(res == FR_OK);
	/* Check free cluster */
	res = f_getfree ("0:",&free_clust2,&fs);
	CHK(res == FR_OK);
	CHK(free_clust1 == free_clust2);
	/* Unmount the logical driver 0 */
	res = f_mount(0,NULL);  
	result = 0;

	lend:
	return result;
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TestDir2
* Returned Value   : 0: 		Pass.
*                    1: 		Fail.
*                    
* Comments         : This test group is to test  f_mkdir in cases of invalid directory names
*   
*END*----------------------------------------------------------------------*/


uint_8 TestDir2(void)
{
	FATFS *fs, fatfs;			/* File system object pointer */	
	uint_32 free_clust1,free_clust2;
  uint_8 result = 1; 	
  
  res = f_mount(0,&fatfs);
  /* Get free cluster */
 	res = f_getfree ("0:",&free_clust1,&fs);
  CHK(res == FR_OK);

	/* bad chars: \/:*?"<>|*/
	res = f_mkdir(NULL);
  CHK(res == FR_INVALID_NAME);
	res = f_mkdir("");
  CHK(res == FR_EXIST);
	res = f_mkdir("\\");
  CHK(res == FR_EXIST);
	res = f_mkdir(".");
  CHK(res == FR_EXIST);
	res = f_mkdir("..");
  CHK(res == FR_EXIST);
	res = f_mkdir("\"");
  CHK(res == FR_INVALID_NAME);
	res = f_mkdir("*");
  CHK(res == FR_INVALID_NAME);
	res = f_mkdir("/");
  CHK(res == FR_EXIST);
	res = f_mkdir(":");
  CHK(res == FR_INVALID_NAME);
	res = f_mkdir("<");
  CHK(res == FR_INVALID_NAME);
	res = f_mkdir(">");
  CHK(res == FR_INVALID_NAME);
	res = f_mkdir("?");
  CHK(res == FR_INVALID_NAME);
	res = f_mkdir("|");
  CHK(res == FR_INVALID_NAME);
	res = f_mkdir(" ");
  CHK(res == FR_INVALID_NAME);
  /* Check free cluster */
	res = f_getfree ("0:",&free_clust2,&fs);
  CHK(res == FR_OK);
  CHK(free_clust1 == free_clust2);
  /* Unmount the logical driver 0 */
  res = f_mount(0,NULL);  
  result = 0;

lend:
  return result;
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TestDir3
* Returned Value   : 0: 		Pass.
*                    1: 		Fail.
*                    
* Comments         : This test group is to test  f_unlink in cases of invalid directory names
*   
*END*----------------------------------------------------------------------*/
uint_8 TestDir3(void)
{
  FATFS *fs, fatfs;			/* File system object pointer */	
  uint_32 free_clust1,free_clust2;
  uint_8 result = 1;

  res = f_mount(0,&fatfs);
  /* Get free cluster */
 	res = f_getfree ("0:",&free_clust1,&fs);
  CHK(res == FR_OK);

  /* bad chars: \/:*?"<>|*/
	res =	f_unlink (NULL);
  CHK(res == FR_INVALID_NAME);
	res =	f_unlink ("");
  CHK(res == FR_INVALID_NAME);
	res =	f_unlink ("\\");
  CHK(res == FR_INVALID_NAME);
	res =	f_unlink (".");
  CHK(res == FR_INVALID_NAME);
	res =	f_unlink ("..");
  CHK(res == FR_INVALID_NAME);
	res =	f_unlink (" ");
  CHK(res == FR_INVALID_NAME);
  /* Check free cluster */
	res = f_getfree ("0:",&free_clust2,&fs);
  CHK(res == FR_OK);
  CHK(free_clust1 == free_clust2);
  /* Unmount the logical driver 0 */
  res = f_mount(0,NULL);  

  result = 0;

lend:
  return result;
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TestDir4
* Returned Value   : 0: 		Pass.
*                    1: 		Fail.
*                    
* Comments         : This test group is to test  f_mkdir, f_unlink functions with 1 of recursive level 
*   
*END*----------------------------------------------------------------------*/
uint_8 TestDir4(void)
{
  FATFS *fs, fatfs;			/* File system object pointer */	
  uint_32 free_clust1,free_clust2;
  uint_8 result = 1;

  res = f_mount(0,&fatfs);
  /* Get free cluster */
 	res = f_getfree ("0:",&free_clust1,&fs);
  CHK(res == FR_OK);

	res = f_mkdir("0:/directory1");
  CHK(res == FR_OK);
	res =	f_unlink ("0:/directory1/directory2");
  CHK(res == FR_NO_FILE);
	res =	f_unlink ("0:/directory1/directory2");
  CHK(res == FR_NO_FILE);
	res = f_mkdir("0:/directory1/directory2");
  CHK(res == FR_OK);
	res = f_mkdir("0:/directory1/directory2");
  CHK(res == FR_EXIST);
	res =	f_unlink ("0:/directory1/directory2");
  CHK(res == FR_OK);
	res =	f_unlink ("0:/directory1/directory2");
  CHK(res == FR_NO_FILE);
	res = f_mkdir("0:/directory1/directory2");
  CHK(res == FR_OK);
	res =	f_unlink ("0:/directory1");
  CHK(res == FR_DENIED);
	res =	f_unlink ("0:/directory1/directory2");
  CHK(res == FR_OK);
	res =	f_unlink ("0:/directory1");
  CHK(res == FR_OK);
  /* Check free cluster */
	res = f_getfree ("0:",&free_clust2,&fs);
  CHK(res == FR_OK);
  CHK(free_clust1 == free_clust2);
  /* Unmount the logical driver 0 */
  res = f_mount(0,NULL);  

  result = 0;

lend:
  return result;
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TestDir5
* Returned Value   : 0: 		Pass.
*                    1: 		Fail.
*                    
* Comments         : This test group is to test  f_mkdir, f_unlink functions with 2 of recursive level 
*   
*END*----------------------------------------------------------------------*/
uint_8 TestDir5(void)
{
  FATFS *fs, fatfs;			/* File system object pointer */	
  uint_32 free_clust1,free_clust2;
  uint_8 result = 1;
  res = f_mount(0,&fatfs);
  /* Get free cluster */
 	res = f_getfree ("0:",&free_clust1,&fs);
  CHK(res == FR_OK);

	res = f_mkdir("0:/directory1");
  CHK(res == FR_OK);
	res = f_mkdir("0:/directory1/directory2");
  CHK(res == FR_OK);
	res = f_unlink("0:/directory1/directory2/directory3");
  CHK(res == FR_NO_FILE);
	res = f_unlink("0:/directory1/directory2/directory3");
  CHK(res == FR_NO_FILE);
	res = f_mkdir("0:/directory1/directory2/directory3");
  CHK(res == FR_OK);
	res = f_mkdir("0:/directory1/directory2/directory3");
  CHK(res == FR_EXIST);
	res = f_unlink("0:/directory1/directory2/directory3");
  CHK(res == FR_OK);
	res = f_unlink("0:/directory1/directory2/directory3");
  CHK(res == FR_NO_FILE);
	res = f_mkdir("0:/directory1/directory2/directory3");
  CHK(res == FR_OK);
	res = f_unlink("0:/directory1/directory2");
  CHK(res == FR_DENIED);
	res = f_unlink("0:/directory1");
  CHK(res == FR_DENIED);
	res = f_unlink("0:/directory1/directory2/directory3");
  CHK(res == FR_OK);
	res = f_unlink("0:/directory1/directory2");
  CHK(res == FR_OK);
	res = f_unlink("0:/directory1");
  CHK(res == FR_OK);
  /* Check free cluster */
	res = f_getfree ("0:",&free_clust2,&fs);
  CHK(res == FR_OK);
  CHK(free_clust1 == free_clust2);
  /* Unmount the logical driver 0 */
  res = f_mount(0,NULL); 
  result = 0;

lend:
  return result;
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TestDir6
* Returned Value   : 0: 		Pass.
*                    1: 		Fail.
*                    
* Comments         : This test group is to test  f_chdir, f_getcwd, f_unlink functions
*   
*END*----------------------------------------------------------------------*/
uint_8 TestDir6(void)
{
  FATFS *fs,fatfs;			/* File system object pointer */	
  uint_32 free_clust1,free_clust2;
  uint_8 result = 1;
  char string[50];
 #if _FS_RPATH == 2  
  res = f_mount(0,&fatfs);
  /* Get free cluster */
 	res = f_getfree ("0:",&free_clust1,&fs);
  CHK(res == FR_OK);

	res =  f_chdir("0:");
  CHK(res == FR_OK);
	res =  f_getcwd (string,50);
  CHK(res == FR_OK);
  res = (strcmp (string, "0:/") == 0) ? FR_OK : FR_INVALID_NAME;
  CHK(res == FR_OK);
	res =  f_chdir("0:/directory");
  CHK(res == FR_NO_PATH);
  	res =  f_chdir("0:/directory11");
  CHK(res == FR_NO_PATH);
	res = f_mkdir("0:/directory1");
  CHK(res == FR_OK);
	res =  f_chdir("0:/directory1");
  CHK(res == FR_OK);
	res = f_unlink("0:/directory1");
  CHK(res == FR_DENIED);
	res = f_unlink("../directory1");
  CHK(res == FR_DENIED);
	res = f_unlink(".");
  CHK(res == FR_INVALID_NAME);
	res = f_unlink("..");
  CHK(res == FR_INVALID_NAME);
	res =  f_getcwd (string,sizeof(string));
  CHK(res == FR_OK);
  res = (strcmp (string, "0:/directory1") == 0) ? FR_OK : FR_INVALID_NAME;
  CHK(res == FR_OK);
	res =  f_chdir("..");
  CHK(res == FR_OK);
	res = f_unlink("0:/directory1");
  CHK(res == FR_OK);
	res =  f_chdir("1:");
  CHK(res == FR_INVALID_DRIVE);
	res =  f_chdir("[:");
  CHK(res == FR_INVALID_NAME);
	res =  f_chdir("0:");
  CHK(res == FR_OK);
	res = f_mkdir("0:/directory1.dir");
  CHK(res == FR_OK);
	res =  f_chdir("0:/directory1.dir");
  CHK(res == FR_OK);
	res =  f_getcwd (string,sizeof(string));
  CHK(res == FR_OK);
  	res = (strcmp (string, "0:/directory1.dir") == 0) ? FR_OK : FR_INVALID_NAME;
  CHK(res == FR_OK);
	res =  f_getcwd (string,sizeof("0:/directory1.dir")-1);	
  CHK(res == FR_NOT_ENOUGH_CORE);
	res =  f_getcwd (string,sizeof("0:/directory1.dir"));
  CHK(res == FR_OK);
  	res = (strcmp (string, "0:/directory1.dir") == 0) ? FR_OK : FR_INVALID_NAME;
  CHK(res == FR_OK);
	res =  f_chdir("..");
  CHK(res == FR_OK);
	res =  f_unlink("0:/directory1.dir");
  CHK(res == FR_OK);
  /* Check free cluster */
	res = f_getfree ("0:",&free_clust2,&fs);
  CHK(res == FR_OK);
  CHK(free_clust1 == free_clust2);
  /* Unmount the logical driver 0 */
  res = f_mount(0,NULL); 
#endif
  result = 0;

lend:
  return result;
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TestDir7
* Returned Value   : 0: 		Pass.
*                    1: 		Fail.
*                    
* Comments         : This function is to test f_mkdir, f_unlink in case of many sub-directories
*   
*END*----------------------------------------------------------------------*/

uint_8 TestDir7(void)
{

  FATFS *fs,fatfs;			/* File system object pointer */	
  uint_32 free_clust1,free_clust2;
  uint_8 result = 1;
  uint_16 i,cnt = 0;
  char name[13];
  res = f_mount(0,&fatfs);
  /* Get free cluster */
 	res = f_getfree ("0:",&free_clust1,&fs);
  CHK(res == FR_OK);

	res = f_mkdir("0:/directory1");
  CHK(res == FR_OK);
 #if (_FS_RPATH )
	res =  f_chdir("0:/directory1");
  CHK(res == FR_OK);
#endif
  for (i = 0; i < NUM_REPEAT; i++)
  {
    sprintf (name, "%u", i);
    res = f_mkdir(name);
    CHK((res == FR_OK) || (res == FR_DENIED));
    if (res == FR_OK)
      cnt++;
    else
      break;
  }

  for (i = 0; i < cnt; i++)
  {
    sprintf (name, "%u", i);
    res = f_unlink(name);
    CHK(res == FR_OK);
  }
#if (_FS_RPATH )
	res =  f_chdir("..");
#endif
  CHK(res == FR_OK);
	res = f_unlink("0:/directory1");
  CHK(res == FR_OK);
 
  /* Check free cluster */
	res = f_getfree ("0:",&free_clust2,&fs);
  CHK(res == FR_OK);
  CHK(free_clust1 == free_clust2);
  /* Unmount the logical driver 0 */
  res = f_mount(0,NULL); 
  result = 0;

lend:
  return result;
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TestDir8
* Returned Value   : 0: 		Pass.
*                    1: 		Fail.
*                    
* Comments         : This test group is to test  f_opendir, f_readdir functions
*   
*END*----------------------------------------------------------------------*/

uint_8 TestDir8(void)
{
  FATFS *fs,fatfs;			/* File system object pointer */	
  uint_32 free_clust1,free_clust2;
  uint_8 result = 1;
  uint_32 i, cnt, j;
  char name[13];
  FILINFO fno;
  DIR dir;
#if _USE_LFN == 3  
  fno.lfname = ff_memalloc(_MAX_LFN);
#endif
  res = f_mount(0,&fatfs);
  /* Get free cluster */
 	res = f_getfree ("0:",&free_clust1,&fs);
  CHK(res == FR_OK);
  /* TC10801: Call f_opendir with NULL dir object pointer */
  res = f_opendir(NULL,"0:");
  CHK(res == FR_INT_ERR);
  /* TC10802: Call f_opendir with nonexisting path*/
	res = f_opendir(&dir,"a");
  CHK(res == FR_NO_PATH);
  /* TC10803: Call f_opendir with invalid path*/
	res = f_opendir(&dir,"*");
  CHK(res == FR_INVALID_NAME);
  /* TC10804: Open current dir (root dir)*/ 
	res = f_opendir(&dir,".");
  CHK(res == FR_OK);
  /* TC10805: Read the current dir */
	res = f_readdir(&dir, &fno);
  CHK(res == FR_OK);
  /* TC10806:Read several dirs in the root dir */
  for (cnt = i = 0; i < NUM_REPEAT; i++)
  {
    sprintf (name, "%u", i);
    res = f_mkdir(name);
    CHK((res == FR_OK) || (res == FR_DENIED));
    if (res == FR_OK)
      cnt++;
    else
      break;
  }
	res = f_opendir(&dir,".");
  CHK(res == FR_OK);

  for (j = 0; j < 2; j++)
  {
    for (i = 0; i < cnt; i++)
    {
      sprintf (name, "%u", i);
      res = f_readdir(&dir, &fno);
      CHK(res == FR_OK);
      CHK(!strcmp (name, fno.fname));
    }

    if (!j)
    {
      res = f_readdir(&dir, NULL);	/* Rewind the directory  */ 
      CHK(res == FR_OK);
    }
  }
  /* TC10807:Read the dir which has no child */
	res = f_readdir(&dir, &fno);
  CHK(res == FR_OK);
  for (i = 0; i < cnt; i++)
  {
    sprintf (name, "%u", i);
    res = f_unlink(name);
    CHK(res == FR_OK);
  }
	res = f_opendir(&dir,".");
  CHK(res == FR_OK);
	res = f_readdir(&dir, &fno);
  CHK(res == FR_OK);

  /* repeat*/
  for (i = 0; i < cnt; i++)
  {
    sprintf (name, "%u", i);
     res = f_mkdir(name);
    CHK((res == FR_OK) || (res == FR_DENIED));
    if (res != FR_OK)
    {
      CHK(i == cnt - 1);
      break;
    }
  }

  for (i = 0; i < cnt; i++)
  {
    sprintf (name, "%u", i);
    res = f_unlink(name);
    CHK(res == FR_OK);
  }
#if _USE_LFN == 3  
  ff_memfree(fno.lfname);
#endif
  /* Check free cluster */
	res = f_getfree ("0:",&free_clust2,&fs);
  CHK(res == FR_OK);
  CHK(free_clust1 == free_clust2);
  /* Unmount the logical driver 0 */
  res = f_mount(0,NULL); 

  result = 0;

lend:
  return result;
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TestDir9
* Returned Value   : 0: 		Pass.
*                    1: 		Fail.
*                    
* Comments         : This test group is to test  f_chdir function with ".." directory
*END*----------------------------------------------------------------------*/

uint_8 TestDir9(void)
{
  FATFS *fs, fatfs;			/* File system object pointer */	
  uint_32 free_clust1,free_clust2;
  uint_8 result = 1;
  TCHAR string[50];
 #if (_FS_RPATH )  
  res = f_mount(0,&fatfs);
  /* Get free cluster */
 	res = f_getfree ("0:",&free_clust1,&fs);
  CHK(res == FR_OK);
  /* TC10901: Change dir using ".." */ 
	res = f_mkdir("0:/directory1");
  CHK(res == FR_OK);
	res = f_mkdir("0:/directory1/directory2");
  CHK(res == FR_OK);
	res = f_mkdir("0:/directory1/directory2/directory3");
  CHK(res == FR_OK);
	res =  f_chdir("0:/directory1/directory2/directory3");
  CHK(res == FR_OK);
  res = f_getcwd(string,50);
  res = (strcmp (string, "0:/directory1/directory2/directory3") == 0) ? FR_OK : FR_INVALID_NAME;
  CHK(res == FR_OK);
 
	res =  f_chdir("..");
  CHK(res == FR_OK);
  res = f_getcwd(string,50);
  res = (strcmp (string, "0:/directory1/directory2") == 0) ? FR_OK : FR_INVALID_NAME;
  CHK(res == FR_OK);

	res =  f_chdir("..");
  CHK(res == FR_OK);
  res = f_getcwd(string,50);
  res = (strcmp (string, "0:/directory1") == 0) ? FR_OK : FR_INVALID_NAME;
  CHK(res == FR_OK);
  
	res =  f_chdir("..");
  CHK(res == FR_OK);
  res = f_getcwd(string,50);
  res = (strcmp (string, "0:/") == 0) ? FR_OK : FR_INVALID_NAME;
  CHK(res == FR_OK);
  /* TC10902: Change dir using "../.." */ 
	res =  f_chdir("0:/directory1/directory2/directory3");
  CHK(res == FR_OK);
	res =  f_chdir("../..");
  CHK(res == FR_OK);
  res = f_getcwd(string,50);
  res = (strcmp (string, "0:/directory1") == 0) ? FR_OK : FR_INVALID_NAME;
  CHK(res == FR_OK);
  
	res =  f_chdir("..");
  CHK(res == FR_OK);
  res = f_getcwd(string,50);
  res = (strcmp (string, "0:/") == 0) ? FR_OK : FR_INVALID_NAME;
  CHK(res == FR_OK);
  /* TC10903: Change dir using "../../.." */ 
	res =  f_chdir("0:/directory1/directory2/directory3");
  CHK(res == FR_OK);
	res =  f_chdir("../../..");
  CHK(res == FR_OK);
  res = f_getcwd(string,50);
  res = (strcmp (string, "0:/") == 0) ? FR_OK : FR_INVALID_NAME;
  CHK(res == FR_OK);
  
	res =  f_unlink("0:/directory1/directory2/directory3");
  CHK(res == FR_OK);
	res =  f_unlink("0:/directory1/directory2");
  CHK(res == FR_OK);
	res =  f_unlink("0:/directory1");
  CHK(res == FR_OK);
  /* Check free cluster */
	res = f_getfree ("0:",&free_clust2,&fs);
  CHK(res == FR_OK);
  CHK(free_clust1 == free_clust2);
  /* Unmount the logical driver 0 */
  res = f_mount(0,NULL); 
#endif
  result = 0;

lend:
  return result;
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TestDir10
* Returned Value   : 0: 		Pass.
*                    1: 		Fail.
*                    
* Comments         : This test group is to test  f_readdir in case of there are many files in read directory
*END*----------------------------------------------------------------------*/

uint_8 TestDir10(void)
{
  FATFS *fs,fatfs;			/* File system object pointer */	
  uint_32 free_clust1,free_clust2;
  FIL fil;
  DIR dir;
  FILINFO fno;
  uint_8 result = 1;
  uint_32 i, cnt;
  char name[12+1+13];
   
  uint_32 FreeSpace1, FreeSpace2;
  int subtest;
#if _USE_LFN == 3  
  fno.lfname = ff_memalloc(_MAX_LFN);
#endif
  /* tbd: allocate almost all free space to big file(s), leave 1 free cluster*/
  /* create directory ending up with disk full, create empty files in it*/ 
  /* until it needs to grow and fails*/
  res = f_mount(0,&fatfs);
  /* Get free cluster */
 	res = f_getfree ("0:",&free_clust1,&fs);
  CHK(res == FR_OK);
	res = f_mkdir("0:/directory1");
  CHK(res == FR_OK);

  for (subtest = 0; subtest < 2; subtest++)
  {
  	res = f_open(&fil, "huge0", FA_WRITE | FA_CREATE_ALWAYS);
    CHK(res == FR_OK);
	  
	  res = f_getfree ("0:",&free_clust2,&fs);
    CHK(res == FR_OK);
    FreeSpace1 = fs->free_clust * fs->csize * SS(fs);
    CHK(FreeSpace1 >= 2* fs->csize * SS(fs));

    if (!subtest)
    {
      FreeSpace2 = 0;
    }
    else
    {
      FreeSpace1 -=fs->csize * SS(fs);
      FreeSpace2 = (uint_32)(fs->csize * SS(fs));
    }
	  
	  res = f_lseek (&fil, FreeSpace1);
    CHK(res == FR_OK);
	  
	  res = f_close(&fil);
    CHK(res == FR_OK);
	  
	  res = f_open(&fil, "huge1", FA_WRITE | FA_CREATE_ALWAYS);
    CHK(res == FR_OK);
	  
	  res = f_lseek (&fil, FreeSpace2);
    CHK(res == FR_OK);
	  
	  res = f_close(&fil);
    CHK(res == FR_OK);

    if (subtest)
    {
      int idx;
      for (idx = 0; idx < 2; idx++)
      {
      	res = f_unlink("0:/huge0");
        CHK(res == FR_OK);
		    
		    res = f_open(&fil, "huge0", FA_WRITE | FA_CREATE_ALWAYS);
        CHK(res == FR_OK);
		    res = f_lseek (&fil, FreeSpace1);
        CHK(res == FR_OK);
		    res = f_close(&fil);
        CHK(res == FR_OK);
      }
    }

    for (cnt = i = 0; i < NUM_REPEAT; i++)
    {
      strcpy (name, "0:/directory1/");
      sprintf (name + strlen(name), "%u", i);
      res = f_open(&fil, name, FA_WRITE | FA_CREATE_ALWAYS);
      CHK((res == FR_OK) || (res == FR_DENIED));
      if (res == FR_OK)
      {
      	res = f_close(&fil);
        CHK(res == FR_OK);
        cnt++;
      }
      else
        break;
    }
	  
	  res = f_opendir(&dir, "0:/directory1");
    CHK(res == FR_OK);

    for (i = 0; i < cnt; i++)
    {
      sprintf (name, "%u", i);
lSkipDotEntries:
	    res = f_readdir(&dir, &fno);		/* Read a directory item */
      CHK(res == FR_OK);
      if (fno.fname[0] ==  '.') goto lSkipDotEntries;
      CHK(!strcmp (name, fno.fname));
    }
  	res = f_readdir(&dir, &fno);
    CHK(res == FR_OK);
    CHK(fno.fname[0] == 0);
    
    for (i = 0; i < cnt; i++)
    {
      strcpy (name, "0:/directory1/");
      sprintf (name + strlen(name), "%u", i);
      res = f_unlink(name);
      CHK(res == FR_OK);
    }
	res = f_unlink("0:/huge1");	
    CHK(res == FR_OK);
	res = f_unlink("0:/huge0");
    CHK(res == FR_OK);
  }
	res = f_unlink("0:/directory1");
  CHK(res == FR_OK);
  /* Check free cluster */
	res = f_getfree ("0:",&free_clust2,&fs);
  CHK(res == FR_OK);
  CHK(free_clust1 == free_clust2);
  /* Unmount the logical driver 0 */
  res = f_mount(0,NULL); 
#if _USE_LFN == 3  
  ff_memfree(fno.lfname);
#endif
   result = 0;
lend:
  return result;
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TestDir11
* Returned Value   : 0: 		Pass.
*                    1: 		Fail.
*                    
* Comments         : This test group is to test  f_stat, f_utime, f_chmod functions
*END*----------------------------------------------------------------------*/

uint_8 TestDir11(void)
{


  FATFS *fs, fatfs;			/* File system object pointer */
  FILINFO fno;	
  uint_32 free_clust1,free_clust2;
  uint_8 result = 1;
#if _USE_LFN == 3  
  fno.lfname = ff_memalloc(_MAX_LFN);
#endif
  res = f_mount(0,&fatfs);
  /* Get free cluster */
 	res = f_getfree ("0:",&free_clust1,&fs);
  CHK(res == FR_OK);
  /* TC11101: Change the attribute of nonexisting directory  */
	res = f_chmod ("0:/directory1",AM_RDO,AM_RDO);
  CHK(res == FR_NO_FILE);
  /* TC11102: Get the information of nonexisting directory */
	res= f_stat ("0:/directory1", &fno);
  CHK(res == FR_NO_FILE);
  /* TC11103: Get the information of directory successfully */
	res = f_mkdir("0:/directory1");
  CHK(res == FR_OK);
	res= f_stat ("0:/directory1", &fno);
  CHK(res == FR_OK);
  CHK(fno.fattrib == AM_DIR);
  CHK(fno.fsize == 0);
  /* TC11104: Change the attribute of  directory successfully */
	res = f_chmod ("0:/directory1",AM_RDO,AM_RDO);
  CHK(res == FR_OK);
	res= f_stat ("0:/directory1", &fno);
  CHK(res == FR_OK);
  CHK(fno.fattrib == (AM_DIR | AM_RDO));
  CHK(fno.fsize == 0);
	res = f_unlink("0:/directory1");
  CHK(res == FR_DENIED);
  /*TC11104: Change the timestap of directory when it is in read only mode */
  fno.fdate.Bits.year = 2107 - YEAR_ORIGIN;
  fno.fdate.Bits.month = 12;
  fno.fdate.Bits.day = 31;
  fno.ftime.Bits.hour = 23;
  fno.ftime.Bits.minute = 59;
  fno.ftime.Bits.second = 58/2; 
  res = f_utime("0:/directory1",&fno);
  CHK(res == FR_OK);
	res= f_stat ("0:/directory1", &fno);
  CHK(res == FR_OK);
  CHK(fno.fattrib == (AM_DIR | AM_RDO));
  CHK(fno.fsize == 0);
  CHK(fno.fdate.Bits.year   == 2107 - YEAR_ORIGIN);
  CHK(fno.fdate.Bits.month == 12);
  CHK(fno.fdate.Bits.day    == 31);
  CHK(fno.ftime.Bits.hour   == 23);
  CHK(fno.ftime.Bits.minute == 59);
  CHK(fno.ftime.Bits.second == 58/2);
  fno.fdate.Bits.year = 1980 - YEAR_ORIGIN;
  fno.fdate.Bits.month = 1;
  fno.fdate.Bits.day = 1;
  fno.ftime.Bits.hour = 0;
  fno.ftime.Bits.minute = 0;
  fno.ftime.Bits.second = 0; 
  res = f_utime("0:/directory1",&fno);
  CHK(res == FR_OK);
	res= f_stat ("0:/directory1", &fno);
  CHK(res == FR_OK);
  CHK(fno.fattrib == (AM_DIR | AM_RDO));
  CHK(fno.fsize == 0);
  CHK(fno.fdate.Bits.year   == 1980 - YEAR_ORIGIN);
  CHK(fno.fdate.Bits.month  == 1);
  CHK(fno.fdate.Bits.day    == 1);
  CHK(fno.ftime.Bits.hour   == 0);
  CHK(fno.ftime.Bits.minute == 0);
  CHK(fno.ftime.Bits.second == 0);
	res = f_unlink("0:/directory1");
  CHK(res == FR_DENIED);
 	res = f_chmod ("0:/directory1",0,AM_RDO);
  CHK(res == FR_OK);	
	res = f_unlink("0:/directory1");
  CHK(res == FR_OK);	
	res = f_mkdir("0:/directory1");
  CHK(res == FR_OK);
  res = f_chmod ("0:/directory1",AM_RDO,AM_RDO);
  CHK(res == FR_OK);
  res = f_unlink("0:/directory1");
  CHK(res == FR_DENIED);
	res = f_chmod ("0:/directory1",0,AM_RDO);
  CHK(res == FR_OK);
	res = f_unlink("0:/directory1");
  CHK(res == FR_OK);
#if _USE_LFN == 3  
  ff_memfree(fno.lfname);
#endif
  /* Check free cluster */
	res = f_getfree ("0:",&free_clust2,&fs);
  CHK(res == FR_OK);
  CHK(free_clust1 == free_clust2);
  /* Unmount the logical driver 0 */
  res = f_mount(0,NULL); 

  result = 0;

lend:
  return result;
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TestDir12
* Returned Value   : 0: 		Pass.
*                    1: 		Fail.
*                    
* Comments         : This test group is to test f_chdrive, f_getfree, f_mount functions
*END*----------------------------------------------------------------------*/

uint_8 TestDir12(void)
{
  FATFS *fs, fatfs, fatfs1;			/* File system object pointer */
  uint_32 free_clust1;
  uint_8 result = 1;
  char buf[512];
  uint_8 disk;
  
  /* TC11201: Call f_mount with invalid logical drive number */
  res = f_mount(10,&fatfs);
  CHK(res == FR_INVALID_DRIVE);

  
  /* TC11203: Mount logical drive successfully */
  res = f_mount(0,&fatfs);
  CHK(res == FR_OK);  
  /* TC11202: Call f_getfree with invalid logical dirve number */
  res = f_getfree(NULL,&free_clust1,&fs); 
  CHK(res == FR_OK);
  res = f_getfree("a:/",&free_clust1,&fs); 
  CHK(res == FR_OK);
  /* TC11204: Call f_getfree with NULL pointer to the variable to store number of free clusters */
  res = f_getfree("0:/",NULL,&fs); 
  CHK(res == FR_INT_ERR);
  /* TC11205: Call f_getfree with NULL pointer to pointer to file system object */
  res = f_getfree("0:/",&free_clust1,NULL); 
  CHK(res == FR_INT_ERR);
  /* TC11206: Mount the second partition */
    res =  f_mount(1,&fatfs1);
    CHK(res == FR_OK);
  /* TC11207: Get number of free cluster in the current working drive */
	res = f_getfree ("/",&free_clust1,&fs);
  CHK(res == FR_OK);
#if (_FS_RPATH == 2)
	res = f_getcwd(buf,30);
  CHK(res == FR_OK);
  CHK(buf[0] == '0');
	res =  f_chdrive (1);
  CHK(res == FR_OK);
  /* TC11209: Change working drive to invalid drive number */
	res =  f_chdrive (2);
  CHK(res == FR_INVALID_DRIVE);
	res =  f_chdrive (3);
  CHK(res == FR_INVALID_DRIVE);
  res = f_getcwd(buf,30);
  CHK(res == FR_OK);
  CHK(buf[0] == '1');
  
  /* TC11210: Get free space sucessfully */
  res =  f_chdrive (0);
  CHK(res == FR_OK);
  	res = f_getcwd(buf,30);
  CHK(buf[0] == '0');
 #endif 
  for (disk = '0'; disk <= '9'; disk++)
  {
    char path[]="0:/";
    path[0] = (char)disk;
    res = f_getfree (path,&free_clust1,&fs);
    CHK((res == FR_OK) || (res == FR_INVALID_DRIVE));
    if (res == FR_OK)
    {
      printf ("Disk \'%c\':\n"
              "  ClusterSize: %lu\n"
              "  TotalClusterCount: %lu\n"
              "  TotalFreeClusterCount: %lu\n\n",
              disk,
              (uint_32)(fs->csize * SS(fs)),
              (uint_32)(fs->n_fatent - 2),
              (uint_32)(fs->free_clust));
    }
  }


  /* TC11211: Unmount the logical drivers */
  res = f_mount(0,NULL); 
  CHK(res == FR_OK)
 
  res = f_mount(1,NULL); 
  CHK(res == FR_OK) 

  result = 0;

lend:
  return result;
}
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : TestFile1
* Returned Value : 
* Comments       : This function is to test f_open, f_close, f_unlink
*
*END*--------------------------------------------------------------------*/

uint_8 TestFile1(void)
{
  FRESULT res;
  uint_8 result = 1;
  FIL file;
  FATFS *fs;
  FATFS fatfs;			/* File system object */
  uint_32 fre_clust1, fre_clust2;
  

  /* Mount the logical driver 0 */
  res = f_mount(0,&fatfs);
  
  /* Get free cluster */
  res = f_getfree("0:",&fre_clust1,&fs);
  CHK(res == FR_OK);

  /*********************** Open file with invalid file name ****************/
  /* TC20101: Open a file for read with file nane similar to directory  */
  res = f_open (&file, ".", FA_READ);
  CHK(res == FR_INVALID_NAME);
  
  
  /* TC20102: Open a file with NULL file name */
  res = f_open (&file, NULL, FA_WRITE|FA_CREATE_NEW);
  CHK(res == FR_INVALID_NAME);
  
  /* TC20103: Open a file with file name containing following characters: \ / : * ? " < > |*/
  res = f_open (&file, "file*1.txt", FA_WRITE|FA_CREATE_NEW);
  CHK(res == FR_INVALID_NAME);
  
  /* TC20104: Open a file with file name length > _MAX_LFN*/
  res = f_open (&file, "File_with_file_name_length_exceed_256_characters_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.txt", FA_WRITE|FA_CREATE_NEW);
  CHK(res == FR_INVALID_NAME);
  
  /* TC20105: Open a file with file name specified to a non-exist path */
  res = f_open (&file, "level1/file1", FA_WRITE|FA_CREATE_NEW);
  CHK(res == FR_NO_PATH);
  
  /* TC20106: Open a file for read with invalid drive */
#if !(_FS_RPATH)  
  res = f_open (&file, "10:file1", FA_WRITE|FA_CREATE_NEW);
  CHK(res == FR_OK);
  res = f_open (&file, "a:file1", FA_WRITE|FA_CREATE_NEW);
  CHK(res == FR_OK); 
#endif
  /********************* Delete file with invalid file name ****************/  
  /* TC20107: Delete the file with file nane similar to directory  */
  res = f_unlink (".");
  CHK(res == FR_INVALID_NAME);
 
  /* TC20108: Delete  a file with NULL file name  */
  res = f_unlink (NULL);
  CHK(res == FR_INVALID_NAME);
 
  /* TC20109: Delete the file which is non-existing  */
  res = f_unlink ("file1");
  CHK(res == FR_NO_FILE);

  /* TC20110: Delete  a file with file name containing following characters: \ / : * ? " < > |                                      */
  res = f_unlink ("file*1.txt");
  CHK(res == FR_INVALID_NAME);

  /* TC20111: Delete  a file with file name length > _MAX_LFN */
  res = f_unlink ("File_with_file_name_length_exceed_256_characters_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.txt");
  CHK(res == FR_INVALID_NAME);
 
  /* TC20112: Delete a file with file name specified to a non-exist path*/
  res = f_unlink ("level1/file1");
  CHK(res == FR_NO_PATH);

  /* TC20113: Delete a file for read with invalid drive */
#if !(_FS_RPATH) 
  res = f_unlink ("10:file1");
  CHK(res == FR_OK);
  res = f_unlink ("a:file1");
  CHK(res == FR_OK);
#endif
 /********************* Close file with invalid file object ****************/ 
  /* TC20114: Invoke f_close with NULL file object  */
  res = f_close (NULL);
  CHK(res == FR_INVALID_OBJECT);
  
  /* TC20115: Close  a file with an not_initialized file object  */
  res = f_close (&file);
  CHK(res == FR_INVALID_OBJECT);

  /** Open file with various mode and normal case of f_unlink and f_close ***/  
  /* TC20116: Open the non_existing file with mode of FA_READ */
  res = f_open (&file, "file1", FA_READ);
  CHK(res == FR_NO_FILE);
 
  /* TC20117: Open the non_existing file with mode of FA_WRITE */
  res = f_open (&file, "file1", FA_WRITE);
  CHK(res == FR_NO_FILE);
 
  /* TC20118: Open the non_existing file with mode of FA_OPEN_EXISTING|FA_READ  */
  res = f_open (&file, "file1", FA_OPEN_EXISTING|FA_READ);
  CHK(res == FR_NO_FILE);
  

  /* TC20119: Create a new file for writing uses mode of FA_OPEN_ALWAYS|FA_WRITE   */
  res = f_open (&file, "file1", FA_OPEN_ALWAYS|FA_WRITE);
  CHK(res == FR_OK);


  /* TC20120: Close a open file with valid file object */
  res = f_close (&file);
  CHK(res == FR_OK);
  

  /* TC20121: Create a new file with file name is similar to the existing one use mode  mode of FA_CREATE_NEW|FA_WRITE  */
  res = f_open (&file, "file1", FA_CREATE_NEW|FA_WRITE);
  CHK(res == FR_EXIST);


  /* TC20122: Create a new file with file name is similar to the existing one use mode  mode of FA_CREATE_ALWAYS|FA_WRITE  */
  res = f_open (&file, "file1", FA_CREATE_ALWAYS|FA_WRITE);
  CHK(res == FR_OK);

  res = f_close (&file);
  CHK(res == FR_OK);
  
   /* TC20123: Open an existing file for read operation */
  res = f_open (&file, "file1", FA_READ);
  CHK(res == FR_OK);

  res = f_close(&file);
  CHK(res == FR_OK);

  /* TC20124: Open an existing file for write operation */
  res = f_open ( &file, "file1", FA_WRITE);
  CHK(res == FR_OK);

  res = f_close (&file);
  CHK(res == FR_OK);


  res = f_unlink ("file1");
  CHK(res == FR_OK);

  res = f_unlink ( "file1");
  CHK(res == FR_NO_FILE);

  res = f_open (&file, "file1", FA_READ);
  CHK(res == FR_NO_FILE);

  /*Check free cluster */
  res = f_getfree("0:",&fre_clust2,&fs);
  CHK(res == FR_OK);
  CHK(fre_clust1 == fre_clust2);
  
  /* Mount the logical driver 0 */
  res = f_mount(0,NULL);
  result = 0;

lend:
  return result;
}
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : TestFile2
* Returned Value : 
* Comments       : This function is to test f_write, f_read
*
*END*--------------------------------------------------------------------*/
#define FAT_SECTOR_SIZE         512
#define MIN(a,b)                ((a>b)?b:a)

uint_8 TestFile2 (void) 
{
  uint_8 result = 1;
  FRESULT res;
  FATFS fatfs, *fs;
  uint_32 fre_clust1=0, fre_clust2=0, byte_cnt = 0;
  FIL file;
  
  uint_8 c;
  
  uchar buf[FAT_SECTOR_SIZE+1];
  uint_32 aFileSize[]=
  {0, 1,                                                  // ~= 0
   FAT_SECTOR_SIZE-1, FAT_SECTOR_SIZE, FAT_SECTOR_SIZE+1, // ~= sector
  };
  int FileSizeIndex = 0;
  int ReadSizeIndex = 0;  
  /* Mount the logical driver 0 */
  res = f_mount(0,&fatfs);
  
  /* Get free cluster */
  res = f_getfree("0:",&fre_clust1,&fs);
  CHK(res == FR_OK);
  
 
  /*************** Call f_write with invalid parameters ********************/
  /* TC20201: Call f_write with NULL file object */
  res = f_write(NULL,"NULL file object",16,&byte_cnt);
  CHK(res == FR_INVALID_OBJECT);
    
  /* TC20202: Call f_write while the file has been opened in non-write mode */ 
  /* Create a new file to read */ 
  res = f_open(&file,"test2.txt",FA_CREATE_ALWAYS|FA_READ);
  CHK(res == FR_OK);
  
  /* Write to file test2.txt */
  res = f_write(&file,"The file in in non write mode",30,&byte_cnt);
  CHK(res == FR_DENIED);
  
  /* Close file test2.txt */
  res = f_close(&file);
  CHK(res == FR_OK);
  
  /* TC20203: Call f_write with NULL data buffer pointer */
  /* Open file test2.txt to write */
  res = f_open(&file,"test2.txt",FA_OPEN_EXISTING|FA_WRITE);
  CHK(res == FR_OK);
  
  /* Call f_write with NULL data buffer pointer */
  res = f_write(&file,NULL,1,&byte_cnt);
  CHK(res == FR_INT_ERR); 
  
  /* TC20204: Call f_write with NULL number of written bytes pointer */
  res = f_write(&file,"NUll number of written bytes pointer",38,NULL);
  CHK(res == FR_INT_ERR);
  
  /* Close file test2.txt */
  res = f_close(&file);
  CHK(res == FR_OK);
  /*************** Call f_read with invalid parameters ********************/
  /* TC20205: Call f_read with NULL file object */
  res = f_read(NULL,buf,16,&byte_cnt);
  CHK(res == FR_INVALID_OBJECT);
    
  /* TC20206: Call f_read while the file has been opened in non-read mode */ 
  /* Create a new file to read */ 
  res = f_open(&file,"test2.txt",FA_CREATE_ALWAYS|FA_WRITE);
  CHK(res == FR_OK);
  
  /* Write to test2.txt */
  res = f_write(&file,"Test f_read with invalid parameters",26,&byte_cnt);
  CHK(res == FR_OK);
  CHK(byte_cnt == 26);
  
  /* Read from file test2.txt */
  res = f_read(&file,buf,26,&byte_cnt);
  CHK(res == FR_DENIED);
  
  /* Close file test2.txt */
  res = f_close(&file);
  CHK(res == FR_OK);
  
  /* TC20207: Call f_read with NULL data buffer pointer */
  /* Open file test2.txt to write */
  res = f_open(&file,"test2.txt",FA_OPEN_EXISTING|FA_READ);
  CHK(res == FR_OK);
  
    
  /* Call f_read with NULL data buffer pointer */
  res = f_read(&file,NULL,1,&byte_cnt);
  CHK(res == FR_INT_ERR); 
  
  /* TC20208: Call f_read with NULL number of read bytes pointer */
  res = f_read(&file,buf,26,NULL);
  CHK(res == FR_INT_ERR);
  
  /* Close file test2.txt */
  res = f_close(&file);
  CHK(res == FR_OK);  
  
  
  /* Delele file test2.txt */
  res = f_unlink("test2.txt");
  CHK(res == FR_OK);
  
  /************************** Read/ Write 1 character ********************/
  /* TC20209: Read/ Write 1 character */
  /* Create a new file file_long_name1.txt for read and write */
  res = f_open (&file, "0:file_long_name1.txt", FA_CREATE_ALWAYS|FA_WRITE|FA_READ);
  CHK(res == FR_OK);
  
  /* Write character 'A' to file_long_name1.txt */
  c = 'A';
  res = f_write (&file,&c , 1, &byte_cnt);
  CHK((res == FR_OK) && (byte_cnt == 1));

  /* Seek file pointer backward 1 byte */
  res = f_lseek (&file, file.fptr -1);
  CHK(res == FR_OK);

  /* Read one character from the file */
  c = 'z';
  res = f_read (&file, &c, 1, &byte_cnt);
  CHK((res == FR_OK) && (byte_cnt == 1) && (c == 'A'));

  /* Close file file_long_name1.txt */
  f_close (&file);
  CHK(res == FR_OK);

  /* Open file file_long_name1.txt for read */
  res = f_open (&file, "0:\\.\\file_long_name1.txt", FA_READ);
  CHK(res == FR_OK);

  /* Read one character from the file */
  c = 'z';
  res = f_read (&file, &c, 1, &byte_cnt);
  CHK((res == FR_OK) && (byte_cnt == 1) && (c == 'A'));

  /* Close file file_long_name1.txt */
  res = f_close (&file);
  CHK(res == FR_OK);

  /* Open file file_long_name1.txt for read */
  res = f_open (&file, "0:file_long_name1.txt", FA_READ);
  CHK(res == FR_OK);
  
  /* Read one character from the file */
  c = 'z';
  res = f_read (&file, &c,1, &byte_cnt);
  CHK((res == FR_OK) && (byte_cnt == 1) && (c == 'A'));

  /* Close file file_long_name1.txt */
  res = f_close (&file);
  CHK(res == FR_OK);

  /* Delete the file */
  res = f_unlink ("0:file_long_name1.txt");
  CHK(res == FR_OK);

  /***************    Read/ Write with various size   ********************/
  /* TC20210: Read/ Write with various size */
  for (FileSizeIndex = 0; FileSizeIndex < 5; FileSizeIndex++)
  {
    for (ReadSizeIndex = 0; ReadSizeIndex < 5; ReadSizeIndex++)
    {
      uint_32 size, pos=0, ReadPos;
      size_t ReadSize;
      int IsEof;

      /* Init size and ReadSize variable */
      size = (size_t)aFileSize[FileSizeIndex];
      ReadSize = aFileSize[ReadSizeIndex];

      /* Create a new file file1 */
      res = f_open (&file,"file1",FA_CREATE_ALWAYS|FA_WRITE|FA_READ);
      CHK(res == FR_OK);
      
      /* Init buf */
      for (pos = 0; pos < size; pos++)
      {
        c = (uchar)(pos % 251);
        buf[pos] = c;
      }
      
      /* Write buf into file1 */
      res = f_write(&file,buf,size,&byte_cnt);
      CHK(res == FR_OK);
      CHK(size == byte_cnt);
      
      /* Seek file pointer to the origin of file */
      res = f_lseek (&file,0);
      CHK(res == FR_OK);
 
      /* Get the file size */
      ReadPos = f_size (&file);
      CHK(size == ReadPos);

      /* Check if it is end of file */
      IsEof = f_eof (&file);
      CHK(size ? !IsEof : IsEof);
 
      for (pos = 0; pos < size; pos++)
      {
        buf[pos] = 0;
      }


      pos = 0;
      /* Get the current file pointer */
      ReadPos = f_tell (&file);
      CHK(pos == ReadPos);


      while (pos < size)
      {
        uint_32 i;
        
        res = f_read (&file, buf, ReadSize,&byte_cnt);
        CHK(res == FR_OK);
        CHK(byte_cnt == MIN(size - pos, ReadSize));

        if (!ReadSize)
          break;

        for (i = 0; i < byte_cnt; i++)
        {
          CHK(buf[i] == (pos + i) % 251);
        }

        pos += byte_cnt;
        ReadPos = f_tell  (&file);
        CHK(pos == ReadPos);
      }

      if (ReadSize)
      {
        res = f_read (&file, buf, ReadSize, &byte_cnt);
        CHK(res == FR_OK);
        CHK(byte_cnt == 0);
        CHK(pos == size);

        IsEof = f_eof (&file);
        CHK(IsEof);
      }

      res = f_close ( &file);
      CHK(res == FR_OK);

      res = f_unlink ("file1");
      CHK(res == FR_OK);
    }
  }
 
  
  /*Check free cluster */
  res = f_getfree("0:",&fre_clust2,&fs);
  CHK(res == FR_OK);
  CHK(fre_clust1 == fre_clust2);
  
  /* Unmount the logical driver 0 */
  res = f_mount(0,NULL);
  CHK(res == FR_OK)
  
  result = 0;
lend:
  return result;  
  
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : TestFile3
* Returned Value : 
* Comments       : This function is to test f_lseek function
*
*END*--------------------------------------------------------------------*/

uint_8 TestFile3(void) 
{
  uint_8 result = 1;
  FRESULT res;
  FATFS fatfs, *fs; 
  FIL file;  
  uint_32 fre_clust1 = 0, fre_clust2 = 0,FileSizeCount = 5,FileSizeIndex= 0,ReadSizeIndex = 0 ;
  uint_32 aFileSize[]=
  {0, 1,                                                  // ~= 0
   FAT_SECTOR_SIZE-1, FAT_SECTOR_SIZE, FAT_SECTOR_SIZE+1, // ~= sector
   0, 0, 0,                                              // ~= cluster
   0, 0, 0,                                              // ~= 2 clusters
   };                                            
  uchar buf[FAT_SECTOR_SIZE+1];
  uint_32 ReadPos,size,pos = 0;
  size_t ReadSize;
    
  /* Mount the logical drive 0 */
  res = f_mount(0,&fatfs);
  CHK(res == FR_OK);
  
  /* Get number of free cluster of drive */
  res = f_getfree("0:",&fre_clust1,&fs);
  CHK(res == FR_OK);
  /**************** Call f_lssek with invalid parameters *****************/  
  /* TC20301 Call f_lseek with invalid file object*/
  res = f_lseek(NULL,1000);
  CHK(res == FR_INVALID_OBJECT); 
 
  /* TC20302 Try to expand a file in READ mode*/
  res = f_open(&file, "file1",FA_CREATE_ALWAYS|FA_READ);
  CHK(res == FR_OK);
  res = f_lseek(&file,1000);
  CHK(res == FR_OK); 
  CHK(0 == f_size(&file));
  res = f_close(&file);
  CHK(res == FR_OK);
  
   /* TC20303: Try to expand a file with size larger than number of free space in the drive */
  res = f_open(&file, "file1",FA_WRITE);
  CHK(res == FR_OK);
  res = f_lseek(&file,512*(fre_clust1));
  CHK(res == FR_OK); 
 
   
  res = f_close(&file);
  CHK(res == FR_OK);
  /*****************  Call f_lseek with various offset value **************/  
  /* TC20303: Call f_lssek with invalid parameters */
  aFileSize[FileSizeCount++] = (uint_32)((fs->csize)*SS(fs) - 1);
  aFileSize[FileSizeCount++] = (fs->csize)*SS(fs);
  aFileSize[FileSizeCount++] = (fs->csize)*SS(fs) + 1;
  aFileSize[FileSizeCount++] = 2 * (fs->csize)*SS(fs) - 1;
  aFileSize[FileSizeCount++] = 2 * (fs->csize)*SS(fs);
  aFileSize[FileSizeCount++] = 2 * (fs->csize)*SS(fs) + 1;
  
  for (FileSizeIndex = 0; FileSizeIndex < 5; FileSizeIndex++)
  {
    size = (size_t)aFileSize[FileSizeIndex];
    /* Create a new file file1 */
    res = f_open (&file,"file1",FA_CREATE_ALWAYS|FA_WRITE|FA_READ);
    CHK(res == FR_OK);
    /* Init buf */
    for (pos = 0; pos < size; pos++)
    {
      buf[pos] = (uchar)(pos % 251);
    }
    /* Write buf into file1 */
    res = f_write(&file,buf,size,&pos);
    CHK(res == FR_OK);
    CHK(size == pos);
    for (ReadSizeIndex = 2; ReadSizeIndex < FileSizeCount - 1; ReadSizeIndex++)
    {
      ReadSize = aFileSize[ReadSizeIndex];
      /* Seek file pointer to the origin of file */
      res = f_lseek (&file,0);
      CHK(res == FR_OK);

      pos = 0;

      while (pos < aFileSize[FileSizeCount-1])
      {
        res = f_lseek (&file,pos);
        CHK(res == FR_OK);

        ReadPos = f_tell(&file);
        CHK(pos == ReadPos);
        pos += MIN(aFileSize[FileSizeCount-1] - pos,ReadSize);
        
        if ((!ReadSize)||(!pos))
          break;
      }
    }
    pos = f_tell(&file);
    size = f_size(&file);
    CHK(pos == size);

    res = f_close ( &file);
    CHK(res == FR_OK);

    res = f_unlink ("file1");
    CHK(res == FR_OK);
  }
  /* Check free cluster */
  res = f_getfree("0:",&fre_clust2,&fs);
  CHK(res == FR_OK);
  CHK(fre_clust1 == fre_clust2);
  
  /* Unmount the drive */
  res = f_mount(0,NULL);
  CHK(res == FR_OK);
  
  result = 0;
lend:
  return result;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : TestFile4
* Returned Value : 
* Comments       : This function is to test f_stat, f_utime, f_chmod functions
*
*END*--------------------------------------------------------------------*/

uint_8 TestFile4(void) 
{
  uint_8 result = 1;
  FRESULT res;
  FATFS fatfs,*fs;
  uint_32 fre_clust1 = 0, fre_clust2 =0;
  FIL file;
  FILINFO finfo;
#if(_USE_LFN == 3)  
  finfo.lfname = ff_memalloc(_MAX_LFN);
#endif  
  /* Mount the logical drive 0 */
  res = f_mount(0,&fatfs);
  CHK(res == FR_OK);
  /* Get the free cluster of drive 0 */
  res = f_getfree("0:",&fre_clust1,&fs);
  CHK(res == FR_OK);
  /*********************** Get the file information with invalid parameters ****************/ 
  /* TC20401: Get file info with NULL file name */
  res = f_stat(NULL,&finfo);
  CHK(res == FR_INVALID_NAME);
  /* TC20402: Get file info with file name containing following characters: \ / : * ? " < > |*/
  res = f_stat ("file*1.txt", &finfo);
  CHK(res == FR_INVALID_NAME);
  /* TC20403: Get file info with file name length > _MAX_LFN*/
  res = f_stat ("File_with_file_name_length_exceed_256_characters_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.txt", &finfo);
  CHK(res == FR_INVALID_NAME);
  /* TC20404: Get file info with file name specified to a non-exist path */
  res = f_stat ( "level1/file1", &finfo);
  CHK(res == FR_NO_PATH);
  /* TC20405: Get file info with invalid drive */
  res = f_stat ("10:file1", &finfo);
  CHK(res == FR_INVALID_NAME);
  res = f_stat ( "a:file1", &finfo);
  CHK(res == FR_INVALID_NAME);
  /* TC20406: Get file info of non_existing file */
  res = f_stat("file11234567",&finfo);
  CHK(res == FR_NO_FILE);
  /* TC20407 Get the fine info with NULL file info object*/
  res = f_open(&file,"file1",FA_CREATE_ALWAYS|FA_WRITE);
  CHK(res == FR_OK);
  res = f_close(&file);
  CHK(res == FR_OK);
  res = f_stat("file1",NULL);
  CHK(res == FR_INT_ERR)
/*********************** Change the file attribute with invalid parameters ****************/   
  /* TC20408: Change the file attribute with NULL file name */
  res = f_chmod(NULL,AM_RDO,AM_RDO);
  CHK(res == FR_INVALID_NAME);
  /* TC20409: Change the file attribute with file name containing following characters: \ / : * ? " < > |*/
  res = f_chmod ("file*1.txt", AM_RDO,AM_RDO);
  CHK(res == FR_INVALID_NAME);
  /* TC20410: Change the file attribute with file name length > _MAX_LFN*/
  res = f_chmod ("File_with_file_name_length_exceed_256_characters_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.txt", AM_RDO,AM_RDO);
  CHK(res == FR_INVALID_NAME);
  /* TC20411: Change the file attribute with file name specified to a non-exist path */
  res = f_chmod ("level1/file1", AM_RDO,AM_RDO);
  CHK(res == FR_NO_PATH);
  /* TC20412: Change the file attribute with invalid drive */
  res = f_chmod ( "10:file1",AM_RDO,AM_RDO);
  CHK(res == FR_INVALID_NAME);
  res = f_chmod ( "a:file1",AM_RDO,AM_RDO);
  CHK(res == FR_INVALID_NAME);
  /* TC20413: Change the file attribute of non_existing file */
  res = f_chmod("file1234567",AM_RDO,AM_RDO);
  CHK(res == FR_NO_FILE);
 /*********************** Change the file timestamp with invalid parameters ****************/   
  /* TC20414: Change the file timestamp with NULL file name */
  res = f_utime(NULL,&finfo);
  CHK(res == FR_INVALID_NAME);
  /* TC20415: Change the file timestamp with file name containing following characters: \ / : * ? " < > |*/
  res = f_utime ("file*1.txt",&finfo);
  CHK(res == FR_INVALID_NAME);
  /* TC20416: Change the file timestamp with file name length > _MAX_LFN*/
  res = f_utime ("File_with_file_name_length_exceed_256_characters_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.txt",&finfo);
  CHK(res == FR_INVALID_NAME);
  /* TC20417: Change the file timestamp with file name specified to a non-exist path */
  res = f_utime ("level1/file1",&finfo);
  CHK(res == FR_NO_PATH);
  /* TC20418: Change the file timestamp with invalid drive */
  res = f_utime ( "10:file1",&finfo);
  CHK(res == FR_INVALID_NAME);
  res = f_utime ( "a:file1",&finfo);
  CHK(res == FR_INVALID_NAME); 
  /* TC20419: Change the file timestamp of non_existing file */
  res = f_utime("file1234567",&finfo);
  CHK(res == FR_NO_FILE);
  /* TC20420: Change the file timestamp with NULL file info object*/
  res = f_utime("file1",NULL); 
  CHK(res == FR_INT_ERR)
   /*********************** Normal cases using f_stat, f_utime, f_chmod ****************/ 
  /* TC20421: Call get file status successfully */
  /* Create a new file */
  res = f_open (&file, "file1", FA_CREATE_ALWAYS|FA_WRITE);
  CHK(res == FR_OK);
  /* Write string "Blah" to the file */
  res = f_write (&file, "Blah",strlen("Blah"), &fre_clust2);
  CHK((res == FR_OK) && (fre_clust2 == strlen("Blah")));
  /* Close the file */
  res = f_close (&file);
  CHK(res == FR_OK);
  /* Get file info */
  res = f_stat("file1",&finfo);
  CHK(res == FR_OK);
  CHK(finfo.fattrib == AM_ARC);
  CHK(finfo.fsize == strlen("Blah"));
  /* TC20422: Change file attribute successfully */
  /* Change the attribute of file to read only */
  res = f_chmod("file1",AM_ARC|AM_RDO,AM_ARC|AM_RDO);
  CHK(res == FR_OK);
  /* Get the file info */
  res = f_stat("file1",&finfo);
  CHK(res == FR_OK);
  CHK(finfo.fattrib == (AM_ARC | AM_RDO));
  CHK(finfo.fsize == strlen("Blah"));
  /* Open file1 for read operation */
  res = f_open (&file, "file1", FA_READ);
  CHK(res == FR_OK);
  /* Write to file1 */
  res = f_write (&file, "Blah", strlen("Blah"), &fre_clust2);
  CHK(res == FR_DENIED);
  /* Close the file */
  res = f_close (&file);
  CHK(res == FR_OK);
  /* Open file 1 to write */
  res = f_open (&file, "file1", FA_WRITE);
  CHK(res == FR_DENIED);
  /* Open file1 to read and write */  
  res = f_open (&file, "file1", FA_READ|FA_WRITE);
  CHK(res == FR_DENIED);
  /* Delete file1 */
  res = f_unlink ("file1");
  CHK(res == FR_DENIED);
  /* TC20423: Try to chang timestamp of file while the file is in read only mode */
  finfo.fdate.Bits.year = 2107 - YEAR_ORIGIN;
  finfo.fdate.Bits.month = 12;
  finfo.fdate.Bits.day = 31;
  finfo.ftime.Bits.hour = 23;
  finfo.ftime.Bits.minute = 59;
  finfo.ftime.Bits.second = 58/2;
  /* Change the timestamp of file1 */
  res = f_utime("file1",&finfo); 
  CHK(res == FR_OK);
  /* Remove read only attribute from file1 */
  res = f_chmod("file1",0,AM_RDO);
  CHK(res == FR_OK);
  /* TC20424: Chang the timestamp of file to successfully */
  /* Change the timestamp of file1 to 31/12/2107, 23h59'58  */
  res = f_utime("file1",&finfo);
  CHK(res == FR_OK);
  /* Get the file info of file1 */
  res = f_stat ("file1", &finfo);
  CHK(res == FR_OK);
  CHK(finfo.fattrib == AM_ARC);
  CHK(finfo.fsize == strlen("Blah"));
  CHK(finfo.fdate.Bits.year   == 2107-YEAR_ORIGIN);
  CHK(finfo.fdate.Bits.month  == 12);
  CHK(finfo.fdate.Bits.day    == 31);
  CHK(finfo.ftime.Bits.hour   == 23);
  CHK(finfo.ftime.Bits.minute == 59);
  CHK(finfo.ftime.Bits.second == 58/2);
  finfo.fdate.Bits.year = 1980 - YEAR_ORIGIN;
  finfo.fdate.Bits.month = 1;
  finfo.fdate.Bits.day = 1;
  finfo.ftime.Bits.hour = 0;
  finfo.ftime.Bits.minute = 0;
  finfo.ftime.Bits.second = 0;
  /* Change the timestamp of file1 to 1/11/1980, 0h0'0  */
  res = f_utime("file1",&finfo);  
  CHK(res == FR_OK);
  res = f_stat("file1",&finfo);
  CHK(res == FR_OK);
  CHK(finfo.fattrib == AM_ARC);
  CHK(finfo.fsize == strlen("Blah"));
  CHK(finfo.fdate.Bits.year   == 1980-YEAR_ORIGIN);
  CHK(finfo.fdate.Bits.month  == 1);
  CHK(finfo.fdate.Bits.day    == 1);
  CHK(finfo.ftime.Bits.hour   == 0);
  CHK(finfo.ftime.Bits.minute == 0);
  CHK(finfo.ftime.Bits.second == 0);
  /* Delete the file */
  res = f_unlink ("file1");
  CHK(res == FR_OK);
#if(_USE_LFN == 3)  
  ff_memfree(finfo.lfname);
#endif
  /* Check free cluster */
  res = f_getfree("0:",&fre_clust2,&fs);
  CHK(res == FR_OK);
  CHK(fre_clust1 == fre_clust2);
  
  /* Unmount the drive */
  res = f_mount(0,NULL);
  CHK(res == FR_OK);
  
  result = 0;
lend:
  return result; 
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : TestFile5
* Returned Value : 
* Comments       : This function is to test f_forward function
*
*END*--------------------------------------------------------------------*/

uint_32 Func(const uint_8 *p, uint_32 btf) 
{
  UNUSED(p)
  return(btf?btf:1); 
}
uint_8 TestFile5(void) 
{
  uint_8 result = 1;
  FRESULT res;
  FATFS fatfs,*fs;
  FIL file;
  uint_32 fre_clust1 = 0, fre_clust2 =0,byte_cnt,pos,size;
  uint_16 aFileSize[]= {1,FAT_SECTOR_SIZE-1, FAT_SECTOR_SIZE, FAT_SECTOR_SIZE+1};
  uchar buf[FAT_SECTOR_SIZE+1];
  uint_16 FileSizeIndex,i, fwsize;
  
  /* Mount the logical drive 0 */
  res = f_mount(0,&fatfs);
  CHK(res == FR_OK);
  /* Get the free cluster of drive 0 */
  res = f_getfree("0:",&fre_clust1,&fs);
  CHK(res == FR_OK);
  /* TC20501: Call f_forward with invalid file object*/
  res = f_forward(NULL,Func,100,&byte_cnt);
  CHK(res == FR_INVALID_OBJECT);
  /* TC20502: Call f_forward with NULL number of bytes forwarded */
  res = f_open (&file, "file1", FA_CREATE_ALWAYS|FA_WRITE|FA_READ);
  CHK(res == FR_OK);
  res = f_write (&file, "Blah",strlen("Blah"), &byte_cnt);
  CHK((res == FR_OK) && (byte_cnt == strlen("Blah")));
  res = f_lseek(&file,0);
  CHK(res == FR_OK);
  res = f_forward(&file,Func,100,NULL);
  CHK(res == FR_INT_ERR);     
  res = f_close (&file);
  /* TC20503: Forward file data when file is opened in non-read mode */
  res = f_open(&file,"file1",FA_WRITE);
  CHK(res == FR_OK)
  res = f_forward(&file,Func,strlen("Blah"),&byte_cnt);
  CHK(res == FR_DENIED);
  res = f_close(&file);
  /* TC20504: Forwad file content with various size*/
  for (FileSizeIndex = 0; FileSizeIndex < 3; FileSizeIndex++)
  {
    size = aFileSize[FileSizeIndex];
    /* Create a new file file1 */
    res = f_open (&file,"file1",FA_CREATE_ALWAYS|FA_WRITE|FA_READ);
    CHK(res == FR_OK);
    /* Init buf */
    for (pos = 0; pos < size; pos++)
    {
      buf[pos] = (uchar)(pos % 251);
    }
    /* Write buf into file1 */
    res = f_write(&file,buf,size,&byte_cnt);
    CHK(res == FR_OK);
    CHK(size == pos);
    for (i = FileSizeIndex; i < 4; i++)
    {
      fwsize = aFileSize[i];
      /* Seek file pointer to the origin of file */
      res = f_lseek (&file,0);
      CHK(res == FR_OK);

      pos = 0;

      while (pos < size)
      {
        res = f_forward (&file,Func,fwsize,&byte_cnt);
        CHK(res == FR_OK);
        CHK(byte_cnt == MIN(size - pos,fwsize));
        pos += MIN(size - pos,fwsize);
        if ((!fwsize)||(!pos))
          break;
      }
    }
    pos = f_tell(&file);
    size = f_size(&file);
    CHK(pos == size);

    res = f_close ( &file);
    CHK(res == FR_OK);

    res = f_unlink ("file1");
    CHK(res == FR_OK);
  } 
  /* Check free cluster */
  res = f_getfree("0:",&fre_clust2,&fs);
  CHK(res == FR_OK);
  CHK(fre_clust1 == fre_clust2);
  
  /* Unmount the drive */
  res = f_mount(0,NULL);
  CHK(res == FR_OK);
  
  result = 0;
lend:
  return result; 
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : TestFile6
* Returned Value : 
* Comments       : This function is to test f_truncate function
*
*END*--------------------------------------------------------------------*/

uint_8 TestFile6(void) 
{
  uint_8 result = 1;
  FRESULT res;
  FATFS fatfs,*fs;
  FIL file;
  uint_32 fre_clust1 = 0, fre_clust2 =0;
  uint_16 trcSize[]= {0,1,FAT_SECTOR_SIZE-1, FAT_SECTOR_SIZE, FAT_SECTOR_SIZE+1};
  uchar buf[FAT_SECTOR_SIZE+1];
  int_16 i;
  /* Mount the logical drive 0 */
  res = f_mount(0,&fatfs);
  CHK(res == FR_OK);
  /* Get the free cluster of drive 0 */
  res = f_getfree("0:",&fre_clust1,&fs);
  CHK(res == FR_OK);
  /* TC20601: Call f_truncate function with NULL file object */
  res = f_truncate(NULL);
  CHK(res == FR_INVALID_OBJECT);
  /* TC20602: Truncate a file while the file is opened in non-write mode */
  res = f_open(&file,"file1",FA_CREATE_ALWAYS|FA_WRITE); 
  CHK(res == FR_OK);
  res = f_write(&file,"Blah",strlen("Blah"),&fre_clust2);
  CHK(res == FR_OK);
  res = f_close(&file);
  CHK(res == FR_OK);
  res = f_open(&file,"file1",FA_READ); 
  CHK(res == FR_OK);
  res = f_truncate(&file);
  CHK(res == FR_DENIED);
  res = f_close ( &file);
  CHK(res == FR_OK);
  /* TC20603: Truncate file with various length */  
  res = f_open (&file,"file1",FA_CREATE_ALWAYS|FA_WRITE|FA_READ);
  CHK(res == FR_OK);
  /* Init buf */
  
  for (i = 0; i < FAT_SECTOR_SIZE +1; i++)
  {
    buf[i] = (uchar)(i % 251);
  }
  /* Write buf into file1 */
  res = f_write(&file,buf,FAT_SECTOR_SIZE +1,&fre_clust2);
  for (i = 4; i >= 0;i--)
  {
    res = f_lseek(&file,trcSize[i]);
    CHK(res == FR_OK)    
    res = f_truncate(&file);
    CHK(res == FR_OK);
    CHK(f_size(&file) == trcSize[i]);
  } 
  res = f_close ( &file);
  CHK(res == FR_OK);
  res = f_unlink ("file1");
  CHK(res == FR_OK);   /* Check free cluster */
  res = f_getfree("0:",&fre_clust2,&fs);
  CHK(res == FR_OK);
  CHK(fre_clust1 == fre_clust2);
  
  /* Unmount the drive */
  res = f_mount(0,NULL);
  CHK(res == FR_OK);
  
  result = 0;
lend:
  return result; 
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : TestFile7
* Returned Value : 
* Comments       : This function is to test f_sync function
*
*END*--------------------------------------------------------------------*/

uint_8 TestFile7(void) 
{
  uint_8 result = 1;
  FRESULT res;
  FATFS fatfs,*fs;
  FIL file;
  uint_32 fre_clust1 = 0, fre_clust2 =0;
  
  /* Mount the logical drive 0 */
  res = f_mount(0,&fatfs);
  CHK(res == FR_OK);
  /* Get the free cluster of drive 0 */
  res = f_getfree("0:",&fre_clust1,&fs);
  CHK(res == FR_OK);
  
  /* TC20701: Call f_sync with NULl file object */
  res = f_sync(NULL);
  CHK(res == FR_INVALID_OBJECT);
  
  /* TC20702: Normal case of flushing cached data */
  res = f_open (&file, "file1", FA_CREATE_ALWAYS|FA_WRITE);
  CHK(res == FR_OK);
  res = f_write(&file,"test f_sync function",strlen("test f_sync function"),&fre_clust2);
  CHK(res == FR_OK);
  res = f_sync(&file);
  CHK(res == FR_OK);
  res = f_close ( &file);
  CHK(res == FR_OK);
  res = f_unlink ("file1");
  CHK(res == FR_OK);   /* Check free cluster */
  
  /* Check free cluster */
  res = f_getfree("0:",&fre_clust2,&fs);
  CHK(res == FR_OK);
  CHK(fre_clust1 == fre_clust2);
  
  /* Unmount the drive */
  res = f_mount(0,NULL);
  CHK(res == FR_OK);
  
  result = 0;
lend:
  return result; 
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : TestFile8
* Returned Value : 
* Comments       : This function is to test string functions
*
*END*--------------------------------------------------------------------*/

uint_8 TestFile8(void) 
{
  uint_8 result = 1;
  int rc;
  FATFS fatfs,*fs;
  FIL file;
  uint_32 fre_clust1 = 0, fre_clust2 =0,size = 0, byte_cnt = 0;
  uint_8 buf[16], *gets_buff;
  
  /* Mount the logical drive 0 */
  res = f_mount(0,&fatfs);
  CHK(res == FR_OK);
  /* Get the free cluster of drive 0 */
  res = f_getfree("0:",&fre_clust1,&fs);
  CHK(res == FR_OK);
  /* TC20801: Call f_printf with NULL file object */
  rc = f_printf(NULL, "%d",10);
  CHK(rc == EOF);
  /* TC20802: Call f_printf with format control directive of "%6d" */
  res = f_open(&file,"file1",FA_CREATE_ALWAYS|FA_WRITE|FA_READ);
  CHK(res == FR_OK);
  rc = f_printf(&file, "%6d", -200);         /* "  -200" */
  CHK(rc == 6);
  res = f_lseek(&file,f_tell(&file) - 6);
  CHK(res == FR_OK);
  res = f_read(&file,buf,16,&byte_cnt);
  CHK(res == FR_OK);
  CHK(byte_cnt == 6)
  /* TC20803: Call f_printf with format control directive of "%02u" */
  rc = f_printf(&file, "%02u", 5);           /* "05" */
  CHK(rc == 2);
  res = f_lseek(&file,f_tell(&file) - 2);
  CHK(res == FR_OK);
  res = f_read(&file,buf,16,&byte_cnt);
  CHK(res == FR_OK);
  CHK(byte_cnt == 2)
  /* TC20804: Call f_printf with format control directive of "%ld" */
  rc = f_printf(&file, "%ld", 12345678L);    /* "12345678" */
  CHK(rc == 8);
  res = f_lseek(&file,f_tell(&file) - 8);
  CHK(res == FR_OK);
  res = f_read(&file,buf,16,&byte_cnt);
  CHK(res == FR_OK);
  CHK(byte_cnt == 8)
  /* TC20805: Call f_printf with format control directive of "%08lX" */
  rc = f_printf(&file, "%08lX", 1194684UL);  /* "00123ABC" */
  CHK(rc == 8);
  res = f_lseek(&file,f_tell(&file) - 8);
  CHK(res == FR_OK);
  res = f_read(&file,buf,16,&byte_cnt);
  CHK(res == FR_OK);
  CHK(byte_cnt == 8)
  /* TC20806: Call f_printf with format control directive of "%08b" */
  rc = f_printf(&file, "%08b", 0x55);        /* "01010101" */
  CHK(rc == 8);
  res = f_lseek(&file,f_tell(&file) - 8);
  CHK(res == FR_OK);
  res = f_read(&file,buf,16,&byte_cnt);
  CHK(res == FR_OK);
  CHK(byte_cnt == 8)
  /* TC20807: Call f_printf with format control directive of "%s" */
  rc = f_printf(&file, "%s", "String");      /* "String" */
  CHK(rc == strlen("String"));
  res = f_lseek(&file,f_tell(&file) - strlen("String"));
  CHK(res == FR_OK);
  res = f_read(&file,buf,16,&byte_cnt);
  CHK(res == FR_OK);
  CHK(byte_cnt == strlen("String"))
  /* TC20808: Call f_printf with format control directive of "%c" */
  rc = f_printf(&file, "%c", 'a');           /* "a" */
  CHK(rc == 1);
  res = f_lseek(&file,f_tell(&file) - 1);
  CHK(res == FR_OK);
  res = f_read(&file,buf,16,&byte_cnt);
  CHK(res == FR_OK);
  CHK(byte_cnt == 1)
  /* TC20809: Call f_putc with NULL file pointer*/
  rc = f_putc('c',NULL);
  CHK(rc == EOF);
  /* TC20810: Normal case using f_putc*/
  rc = f_putc('c',&file);
  CHK(rc == 1);
  
  /* TC20811: Call f_puts with NULL file pointer*/
  rc = f_puts("string",NULL);
  CHK(rc == EOF );
  
  /* TC20812: Call f_gets with NULL file pointer*/
  gets_buff = (uint_8 *)f_gets((char*)(&buf[0]),16,NULL);
  CHK(gets_buff == NULL );
  
  /* TC20813: Normal case using f_puts*/
  rc = f_puts("string",&file);
  CHK(rc == strlen("string") );
  
  /* TC20814: Normal case using f_gets*/
  res = f_lseek(&file,strlen("string"));
  CHK(res == FR_OK);
  gets_buff = (uint_8 *)f_gets((char*)(&buf[0]),(int)strlen("string"),&file);
  CHK(gets_buff != NULL);
  
  res = f_close(&file);
  CHK(res == FR_OK);
  res = f_unlink("file1");
  CHK(res == FR_OK);
  
  /* Check free cluster */
  res = f_getfree("0:",&fre_clust2,&fs);
  CHK(res == FR_OK);
  CHK(fre_clust1 == fre_clust2);
  
  /* Unmount the drive */
  res = f_mount(0,NULL);
  CHK(res == FR_OK);
  
  result = 0;
lend:
  return result; 
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : TestFile9
* Returned Value : 
* Comments       : This function is to test f_rename function
*
*END*--------------------------------------------------------------------*/

uint_8 TestFile9(void) 
{
  uint_8 result = 1;
  FRESULT res;
  FATFS fatfs,*fs;
  FIL file;
  uint_32 fre_clust1 = 0, fre_clust2 =0;
  
  /* Mount the logical drive 0 */
  res = f_mount(0,&fatfs);
  CHK(res == FR_OK);
  /* Get the free cluster of drive 0 */
  res = f_getfree("0:",&fre_clust1,&fs);
  CHK(res == FR_OK);
  
  /* TC20901: Rename with the old name silimar to a directory  */
  res = f_rename (".", "rename_file");
  CHK(res == FR_INVALID_NAME);
  /* TC20902: Rename with NULL old file name */
  res = f_rename (NULL,"rename_file");
  CHK(res == FR_INVALID_NAME);
  /* TC20903: Rename file with old file name containing following characters: \ / : * ? " < > |*/
  res = f_rename ("file*1.txt", "rename_file");
  CHK(res == FR_INVALID_NAME);
  /* TC20904: Rename a file with old file name length > _MAX_LFN*/
  res = f_rename ("File_with_file_name_length_exceed_256_characters_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.txt","rename_file");
  CHK(res == FR_INVALID_NAME);
  /* TC20905: Rename a file with file name specified to a non-exist path */
  res = f_rename ("level1/file1", "rename_file");
  CHK(res == FR_NO_PATH);
  /* TC20906: Rename a file with invalid drive in old file name */
  res = f_rename ("10:file1","rename_file");
  CHK(res == FR_INVALID_NAME);
  res = f_rename ( "a:rename_file", "file1");
  CHK(res == FR_INVALID_NAME);
  /* TC20907: Rename with the new name silimar to a directory  */
  res = f_open(&file,"file1",FA_CREATE_ALWAYS|FA_WRITE);
  CHK(res == FR_OK) ;
  res = f_close(&file);
  CHK(res == FR_OK) ;
  res = f_rename ("file1", ".");
  CHK(res == FR_EXIST);
  /* TC20908: Rename with NULL new file name */
  res = f_rename ("file1",NULL);
  CHK(res == FR_INVALID_NAME); 
  /* TC20909: Rename file with new file name containing following characters: \ / : * ? " < > |*/
  res = f_rename ("file1","file*1.txt");
  CHK(res == FR_INVALID_NAME);
  /* TC20910: Rename a file with new file name length > _MAX_LFN*/
  res = f_rename ("file1","File_with_file_name_length_exceed_256_characters_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.txt");
  CHK(res == FR_INVALID_NAME);
  /* TC20911: Rename a file with file name specified to a non-exist path */
  res = f_rename ("file1","level1/file1");
  CHK(res == FR_NO_PATH);
  /* TC20912: Rename a file with invalid drive in new file name */
  res = f_rename ("file1","10:file2");
  CHK(res == FR_INVALID_NAME);
  res = f_rename ("file1", "a:file1");
  CHK(res == FR_INVALID_NAME);
  
  /* TC20913: Rename the file with new file name similar to an existed file */
  res = f_open (&file, "file2",FA_CREATE_ALWAYS|FA_WRITE);
  CHK(res == FR_OK);
  res = f_close(&file);
  CHK(res == FR_OK);
  res = f_rename("file1","file2");
  CHK(res == FR_EXIST);
    
  /* TC20914:Try to remane a file in read only mode  */
  res = f_chmod("file1",AM_RDO,AM_RDO);
  CHK(res == FR_OK);
  res = f_rename("file1", "rename_file");
  CHK(res == FR_OK); 
  res = f_chmod("rename_file",0,AM_RDO);
  CHK(res == FR_OK);
  
  /* TC20915: Normal case using f_rename function */ 
  res = f_rename("rename_file","file1");
  CHK(res == FR_OK);
  
  res = f_unlink("file1");
  CHK(res == FR_OK);
  res = f_unlink("file2");
  CHK(res == FR_OK);

  /* Check free cluster */
  res = f_getfree("0:",&fre_clust2,&fs);
  CHK(res == FR_OK);
  CHK(fre_clust1 == fre_clust2);
  
  /* Unmount the drive */
  res = f_mount(0,NULL);
  CHK(res == FR_OK);
  
  result = 0;
lend:
  return result; 
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : TestFile10
* Returned Value : 
* Comments       : This function is to test file sharing policy (_FS_SHARE = 2)
*
*END*--------------------------------------------------------------------*/

uint_8 TestFile10(void) 
{
  uint_8 result = 1;
  FRESULT res;
  FATFS fatfs,*fs;
  FIL fil1, fil2, fil3;
  uint_32 fre_clust1 = 0, fre_clust2 =0;
  
  /* Mount the logical drive 0 */
  res = f_mount(0,&fatfs);
  CHK(res == FR_OK);
  /* Get the free cluster of drive 0 */
  res = f_getfree("0:",&fre_clust1,&fs);
  CHK(res == FR_OK);
  /* TC21001: When file sharing policy is enabled, more than one files can be open for write operation*/
  res = f_open (&fil1, "file1",FA_CREATE_ALWAYS|FA_WRITE);
  CHK(res == FR_OK);
  res = f_open (&fil2, "file2",FA_CREATE_ALWAYS|FA_WRITE); 
  CHK(res == FR_OK);
  
  /* TC21002: Try to open another file for write when number of open files = _FS_SHARE */
  res = f_open (&fil3, "file3",FA_CREATE_ALWAYS|FA_WRITE);
  CHK(res == FR_TOO_MANY_OPEN_FILES); 
  /* TC21003: Try to open another file for read when number of open files = _FS_SHARE */
  res = f_open (&fil3, "file3",FA_CREATE_ALWAYS|FA_READ);
  CHK(res == FR_TOO_MANY_OPEN_FILES); 
  /*Close file3*/
   
  /* TC21004: When file sharing policy is enabled, a file can be opened many times for read operation*/
  res = f_close(&fil1);
  CHK(res == FR_OK);
  res = f_close(&fil2);
  CHK(res == FR_OK);
  res = f_open(&fil1,"file1",FA_READ);
  CHK(res == FR_OK);
  res = f_open(&fil2,"file1",FA_READ);
  CHK(res == FR_OK);
  res = f_close(&fil1);
  CHK(res == FR_OK);
  res = f_close(&fil2);
  CHK(res == FR_OK);
  /* TC21005: Once file is opened for write, it can not be re_opened */
  res = f_open(&fil1,"file1", FA_WRITE);
  CHK(res == FR_OK);
  res = f_open (&fil2,"file1", FA_READ);
  CHK(res == FR_LOCKED);
  res = f_close(&fil1);
  CHK(res == FR_OK);
  /* TC21006: Open one file, rename another file */
  res = f_open(&fil1,"file1", FA_WRITE);
  CHK(res == FR_OK);
  res = f_rename("file2","file_new");      
  CHK(res == FR_OK);
  /* TC21007: Open 2 files, rename another file */
  res = f_open(&fil3,"file3",FA_CREATE_ALWAYS|FA_WRITE);
  CHK(res == FR_OK);
  res = f_rename("file_new","file2");
  CHK(res == FR_OK);
  /* TC21008: Open 2 files, try to delete another file */
  res = f_unlink("file2");
  CHK(res == FR_OK);
  /* TC21009: Open 1 file, delete another file */
  res = f_close(&fil3);
  CHK(res == FR_OK);
  res = f_close(&fil1);
  CHK(res == FR_OK);
  res = f_unlink("file1");
  CHK(res == FR_OK);
  res = f_unlink("file3");
  CHK(res == FR_OK);   
  /* Check free cluster */
  res = f_getfree("0:",&fre_clust2,&fs);
  CHK(res == FR_OK);
  CHK(fre_clust1 == fre_clust2);
  
  /* Unmount the drive */
  res = f_mount(0,NULL);
  CHK(res == FR_OK);
  
  result = 0;
lend:
  return result; 
}
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : TestFile11
* Returned Value : 
* Comments       : This function is to test how FAT apis work when drive status is invalid  FR_NOT_ENABLED
*
*END*--------------------------------------------------------------------*/

uint_8 TestFile11(void) 
{
  uint_8 result = 1;
  FRESULT res;
  FIL file;
  FILINFO finfo;
  uint_32 fre_clust1 = 0, fre_clust2 =0;
  /* TC21101: Call f_open when there is no mounted logical drive */
  res = f_open (&file, "file1", FA_CREATE_ALWAYS|FA_READ);
  CHK(res == FR_NOT_ENABLED);
  /* TC21102: Call f_utime when there is no mounted logical drive */
  res = f_utime("file1",&finfo);
  CHK(res == FR_NOT_ENABLED);
  /* TC21103: Call f_chmod when there is no mounted logical drive */
  res = f_chmod("file1",AM_RDO,AM_RDO);
  CHK(res == FR_NOT_ENABLED);
  /* TC21104: Call f_stat when there is no mounted logical drive */
  res = f_stat("file1",&finfo);
  CHK(res == FR_NOT_ENABLED);
  /* TC21105: Call f_unlink when there is no mounted logical drive */
  res = f_unlink("file1");
  CHK(res == FR_NOT_ENABLED);
  /* TC21106: Call f_rename when there is no mounted logical drive */
  res = f_rename("file1","new_name");
  CHK(res == FR_NOT_ENABLED);
   
  result = 0;
lend:
  return result; 
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : TestFile12
* Returned Value : 
* Comments       : This function is to test how FAT apis work when LFN is disable
*
*END*--------------------------------------------------------------------*/

uint_8 TestFile12(void) 
{
  uint_8 result = 1;
  FRESULT res;
  FATFS fatfs,*fs;
  FIL file;
  FILINFO finfo;
  uint_32 fre_clust1 = 0, fre_clust2 =0;
  
  /* Mount the logical drive 0 */
  res = f_mount(0,&fatfs);
  CHK(res == FR_OK);
  /* Get the free cluster of drive 0 */
  res = f_getfree("0:",&fre_clust1,&fs);
  CHK(res == FR_OK);
  /* TC21201: Call f_open with long file name */
  res = f_open (&file, "long_file_name1.txt", FA_CREATE_ALWAYS|FA_READ);
  CHK(res == FR_INVALID_NAME);
  /* TC21202: Call f_utime with long file name */
  res = f_utime("long_file_name1.txt",&finfo);
  CHK(res == FR_INVALID_NAME);
  /* TC21203: Call f_chmod with long file name */
  res = f_chmod("long_file_name1.txt",AM_RDO,AM_RDO);
  CHK(res == FR_INVALID_NAME);
  /* TC21204: Call f_stat with long file name */
  res = f_stat("long_file_name1.txt",&finfo);
  CHK(res == FR_INVALID_NAME);
  /* TC21205: Call f_unlink with long file name */
  res = f_unlink("long_file_name1.txt");
  CHK(res == FR_INVALID_NAME);
  /* TC21206: Call f_rename with long file name */
  res = f_rename("long_file_name1.txt","new_name");
  CHK(res == FR_INVALID_NAME);
  
  /* Check free cluster */
  res = f_getfree("0:",&fre_clust2,&fs);
  CHK(res == FR_OK);
  CHK(fre_clust1 == fre_clust2);
  
  /* Unmount the drive */
  res = f_mount(0,NULL);
  CHK(res == FR_OK);
  
  result = 0;
lend:
  return result; 
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : TestFile13
* Returned Value : 
* Comments       : This function is to test how FAT apis work when _RS_PATH = 0
*
*END*--------------------------------------------------------------------*/

uint_8 TestFile13(void) 
{
  uint_8 result = 1;
  FRESULT res;
  FATFS fatfs,*fs;
  FIL file;
  FILINFO finfo;
  uint_32 fre_clust1 = 0, fre_clust2 =0;
  
  /* Mount the logical drive 0 */
  res = f_mount(0,&fatfs);
  CHK(res == FR_OK);
  /* Get the free cluster of drive 0 */
  res = f_getfree("0:",&fre_clust1,&fs);
  CHK(res == FR_OK);
  
  /* TC21301: Call f_open with invalid file name */
  res = f_open (&file, "../file1.txt", FA_CREATE_ALWAYS|FA_READ);
  CHK(res == FR_INVALID_NAME);
  res = f_open (&file, "./file1.txt", FA_CREATE_ALWAYS|FA_READ);
  CHK(res == FR_NO_PATH);
  
  /* TC21302: Call f_close with invalid file name */
  res = f_utime("../file1.txt",&finfo);
  CHK(res == FR_INVALID_NAME);
  res = f_utime("./file1.txt",&finfo);
  CHK(res == FR_NO_PATH);
  
  /* TC21303: Call f_chmod with invalid file name */
  res = f_chmod("../file1.txt",AM_RDO,AM_RDO);
  CHK(res == FR_INVALID_NAME);
  res = f_chmod("./file1.txt",AM_RDO,AM_RDO);
  CHK(res == FR_NO_PATH);

  /* TC21304: Call f_stat with long file name */
  res = f_stat("../file1.txt",&finfo);
  CHK(res == FR_INVALID_NAME);
  res = f_stat("./file1.txt",&finfo);
  CHK(res == FR_NO_PATH);
  /* TC21305: Call f_unlink with invalid file name */
  res = f_unlink("../file1.txt");
  CHK(res == FR_INVALID_NAME);
  res = f_unlink("./file1.txt");
  CHK(res == FR_NO_PATH);
    /* TC21306: Call f_rename with invalid file name */
  res = f_rename("../file1.txt","new_name");
  CHK(res == FR_INVALID_NAME);
  res = f_rename("./file1.txt","new_name");
  CHK(res == FR_NO_PATH);
    /* TC21307: Call f_open with invalid drive */
  res = f_open (&file, "10:file1.txt", FA_CREATE_ALWAYS|FA_READ);
  CHK(res == FR_INVALID_NAME);
  res = f_open (&file, "a:file1.txt", FA_CREATE_ALWAYS|FA_READ);
  CHK(res == FR_INVALID_NAME);
  
  /* TC21308: Call f_close with invalid drive */
  res = f_utime("10:file1.txt",&finfo);
  CHK(res == FR_INVALID_NAME);
  res = f_utime("a:file1.txt",&finfo);
  CHK(res == FR_INVALID_NAME);
  
  /* TC21309: Call f_chmod with invalid drive */
  res = f_chmod("10:file1.txt",AM_RDO,AM_RDO);
  CHK(res == FR_INVALID_NAME);
  res = f_chmod("a:file1.txt",AM_RDO,AM_RDO);
  CHK(res == FR_INVALID_NAME);

  /* TC21310: Call f_stat with long drive */
  res = f_stat("10:file1.txt",&finfo);
  CHK(res == FR_INVALID_NAME);
  res = f_stat("a:file1.txt",&finfo);
  CHK(res == FR_INVALID_NAME);
  /* TC21311: Call f_unlink with invalid drive */
  res = f_unlink("10:file1.txt");
  CHK(res == FR_INVALID_NAME);
  res = f_unlink("a:file1.txt");
  CHK(res == FR_INVALID_NAME);

  /* TC21312: Call f_rename with invalid drive */
  res = f_rename("10:file1.txt","new_name");
  CHK(res == FR_INVALID_NAME);
  res = f_rename("a:file1.txt","new_name");
  CHK(res == FR_INVALID_NAME);
  
  /* Check free cluster */
  res = f_getfree("0:",&fre_clust2,&fs);
  CHK(res == FR_OK);
  CHK(fre_clust1 == fre_clust2);
  
  /* Unmount the drive */
  res = f_mount(0,NULL);
  CHK(res == FR_OK);
  
  result = 0;
lend:
  return result; 
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : TestDirFileMixup1
* Returned Value : 
* Comments       : This function is to test Mix file and directory
*
*END*--------------------------------------------------------------------*/

uint_8 TestDirFileMixup1(void)
{
  uint_8 result = 1;
  FRESULT res;
  FATFS fatfs,*fs;
  FIL file;
  uint_32 fre_clust1 = 0, fre_clust2 =0;
  
  /* Mount the logical drive 0 */
  res = f_mount(0,&fatfs);
  CHK(res == FR_OK);
  /* Get the free cluster of drive 0 */
  res = f_getfree("0:",&fre_clust1,&fs);
  CHK(res == FR_OK);
  /* TC30101: Change directory with name similar to the existing file name */
  /* Create file1 */
  res = f_open (&file, "file1", FA_CREATE_ALWAYS|FA_WRITE);
  CHK(res == FR_OK);
  /* Close the file */
  res = f_close (&file);
  CHK(res == FR_OK);
  /* Change to "file1" directory */
#if (_FS_RPATH == 2)  
  res = f_chdir ("file1");
  CHK(res == FR_NO_PATH);
#endif   
  /* TC30102: Make a directory with name similar to the existing file name */
  res = f_mkdir ("file1");
  CHK(res == FR_EXIST);
 
  res = f_unlink ("file1");
  CHK(res == FR_OK);
  
  res = f_open (&file, "file1", FA_READ);
  CHK(res == FR_NO_FILE);
  /* TC30103: Create a file with name similar to the existing directory name */
  res = f_mkdir ("level1");
  CHK(res == FR_OK);

  res = f_open (&file, "level1", FA_CREATE_ALWAYS|FA_WRITE);
  CHK(res == FR_DENIED);
 #if (_FS_RPATH == 2)  
  res = f_chdir ("level1");
  CHK(res == FR_OK);

  res = f_chdir ("..");
  CHK(res == FR_OK);
#endif 
  res = f_unlink ("level1");
  CHK(res == FR_OK);
#if (_FS_RPATH == 2)
  res = f_chdir ("..");
  CHK(res == FR_OK);
#endif 

  /* Check free cluster */
  res = f_getfree("0:",&fre_clust2,&fs);
  CHK(res == FR_OK);
  CHK(fre_clust1 == fre_clust2);
  
  /* Unmount the drive */
  res = f_mount(0,NULL);
  CHK(res == FR_OK);
  
  result = 0;
lend:
  return result; 
}


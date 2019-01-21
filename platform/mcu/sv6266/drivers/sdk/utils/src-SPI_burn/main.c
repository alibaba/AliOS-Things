#include <sys/time.h>
#ifdef __MINGW32__
#include <winsock2.h>  // for Sleep()
#endif
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "util.h"
#include "flash.h"

//-------------------------------------------------------------------------------------------------
/* Help menu */
int usage(void)
{
  printf( "--port:\t\t\tPort number to connect with ICEman (default to 2354)\n");
  printf( "--read-all:\t\t\tRead entire flash content\n");
  printf( "--addr:\t\t\tFlash target address to write (default to 0x0)\n");
  printf( "--image:\t\tImage name to burn\n");
  printf( "--erase-all:\t\tErase entire flash before burning\n");
  printf( "--help:\t\t\tThe usage for NDS32 Burner\n");
  return 0;
}

/* Log file descriptor */
FILE *pLogFile;
int coreid = 0;

/* option structure */
static struct option long_option[] =
{
  {"port", required_argument, 0, 'p'},
  {"read-all", required_argument, 0, 'r'},
  {"addr", required_argument, 0, 'a'},
  {"image", required_argument, 0, 'i'},
  {"erase-all", no_argument, 0, 'e'},
  {"help", no_argument, 0, 'h'},
  {0, 0, 0, 0}
};

/* main function 
 *  . parse parameters
 *  . initial socket
 *  . select target coreid
 *  . reset AICE 
 *  . issue RESET_HOLD to let target board restore to reset status 
 *    avoid boot code to damage flash registers' settings 
 *  . call target-dependent initial function and calculate flash base address (Device Address) 
 *     xc5()/p24()/ag101()/ag301p() 
 *  . read image binary data from --image specified file 
 *  . if user specifies --verify-only, verify the target with the image data and exit 
 *  . preserve first/last blocks if needed (As start/end addresses are not block-aligned)
 *  . erase flash
 *  . burn image - call Flash_BurnImage()
 *  . verify    */
int main(int argc, char **argv)
{
  pLogFile = stdout;
  int c = 0;
  unsigned int port = 2354;
  unsigned int readall = 0;
  unsigned int read_addr = 0;
  char *readbuf = (char*)malloc(FLASH_SECTOR_SIZE*sizeof(char));
  unsigned int address = 0;             /* default write to flash base */
  unsigned int size = 0;                /* image size */
  unsigned int program_size = 0;
  unsigned int writelength;
  unsigned int flashaddr;
  unsigned int erase_size;              /* erase size */
  unsigned int erase_addr;				/* erase addr */
  unsigned char *start = NULL;          /* image pointer */
  unsigned char *content = NULL;
  FILE *image = NULL;                   /* image to burn */
  char *name = (char*)malloc(1024*sizeof(char));
  int erase_all = 0;
  
  struct timeval erase_begin_time;
  struct timeval erase_end_time;
  struct timeval erase_total_time;
  double erase_time_sec;
  
  struct timeval burn_begin_time;
  struct timeval burn_end_time;
  struct timeval burn_total_time;
  double burn_time_sec;

  /* parameter handler */
  while(1)
  {
    int option_index;
    c = getopt_long(argc, argv, "p:r:a:i:e:h", long_option, &option_index);
    if (c == EOF)
      break;

    switch (c)
    {
	  case 'p':
      port = strtol(optarg, NULL, 0);
      fprintf(pLogFile, "Port number to connect with ICEman: 0x%x",port);
      break;
	  
	  case 'r':
      if (strncmp(optarg, "/cygdrive/", 10) == 0)
      {
        name[0] = optarg[10];
        name[1] = ':';
        name[2] = '\0';
        strcat(name, optarg + 11);
      }
      else
	  {
        name = optarg;
	  }
      if ((image = fopen(name, "w+b"))== NULL)
      {
        fprintf (pLogFile, "Error: can't open file %s\n", optarg);
        fflush(pLogFile);
        exit(1);
      }
	  fprintf (pLogFile, "open file %s\n", optarg);
	  readall = 1;
      break;
	
	  case 'a':
      address = strtol(optarg, NULL, 0);
      fprintf(pLogFile, "Flash target address to write: 0x%x",address);
      break;
	  
      case 'i':
      if (strncmp(optarg, "/cygdrive/", 10) == 0)
      {
        name[0] = optarg[10];
        name[1] = ':';
        name[2] = '\0';
        strcat(name, optarg + 11);
      }
      else
	  {
        name = optarg;
	  }
      if ((image = fopen(name, "rb"))== NULL)
      {
        fprintf (pLogFile, "Error: can't open file %s\n", optarg);
        fflush(pLogFile);
        exit(1);
      }
	  fprintf (pLogFile, "open file %s\n", optarg);
      break;	  
	  
      case 'e':
      erase_all = 1;
      break;
	  
      case 'h':
      case '?':
      default:
      usage();
      exit(0);
      break;
    }  /* switch (c)  */
  }  /* while(1)  */
  
  if(!image)
  {
	fprintf(pLogFile, "ERROR!! no image!!\n");
	fflush(pLogFile);
    exit(0);
  }

  fprintf (pLogFile, "NDS32 Burner BUILD_ID: %d\n", BUILD_ID);
  fflush(pLogFile);

  initial_socket(port);
#ifdef __MINGW32__
  Sleep (200);
#else
  usleep (200000);
#endif
 
  // select core 
  if( coreid != 0 )
  {
      if (send_coreid(coreid) < 0 )
      {
          close_connection ();
          exit(0);
      }
  }

  // init AICE
  if (send_cmd(RESET_AICE) < 0)
  {
    close_connection ();
    exit(0);
  }

  // reset-and-hold to init target
  send_cmd (RESET_HOLD);

  /* get burn image content */
  if(!readall)
  {
    fprintf(pLogFile, "get burn image content...\n");
    content = get_image(image, &size);
    start = content;
    fclose (image);
  }
  /* handle signal */
  signal(SIGTERM, handle_int);
  signal(SIGINT, handle_int);
#ifndef __MINGW32__
  signal(SIGKILL, handle_int);
#endif
  if(!readall)
  {
    if(size > FLASH_SIZE)
    {
      fprintf(pLogFile, "ERROR!! image size(%d) > flash size!!\n", size);
      return 0;
    }
    else
    {
      fprintf (pLogFile, "image size: %d bytes\n", size);
      fflush(pLogFile);
    }
  }
  
  flash_init();
  flash_get_jedec();
  
  if(readall)
  {
    fprintf(pLogFile, "Flash readall begin.\n");
	
	for(read_addr=0; read_addr<FLASH_SIZE; read_addr+=FLASH_SECTOR_SIZE)
	{
		flash_fast_read(read_addr, FLASH_SECTOR_SIZE, readbuf);
		fwrite(readbuf, sizeof(char), FLASH_SECTOR_SIZE, image);
		fprintf(pLogFile, "read 0x%x\n",read_addr);
	}
	
	fprintf(pLogFile, "Flash readall done.\n");
	fclose(image);
    terminate();
    return 0;
  }
  
#if 1
  /* block erase */
  if (erase_all)
  {
    erase_addr = 0;
    erase_size = FLASH_SIZE;
  }
  else
  {
    erase_addr = address;
    erase_size = (((size+4095)>>12)<<12);
  }
  
  fprintf(pLogFile, "Flash erase begin.\n");
  
  gettimeofday(&erase_begin_time, NULL);
  while(erase_addr < erase_size)
  {
	flash_sector_erase(erase_addr);
	erase_addr += FLASH_SECTOR_SIZE;
  }
  gettimeofday (&erase_end_time, NULL);
  timeval_diff (&erase_total_time, &erase_begin_time, &erase_end_time);
  erase_time_sec = erase_total_time.tv_sec + (double)erase_total_time.tv_usec / 1000000;
  
  fprintf(pLogFile, "\nTotal erase time: %lu sec, %lu usec\n",
  erase_total_time.tv_sec, erase_total_time.tv_usec);
  fprintf(pLogFile, "Average erase rate: %.2lf KBytes/s\n",
  (double)(erase_size >> 10) / erase_time_sec);
  
  fprintf(pLogFile, "Flash erase done.\n");

  /* burn image */
  fprintf(pLogFile, "Flash burning begin.\n");

  flash_set_quadbit();
  fprintf(pLogFile, "Flash quadbits enable.\n");
  
  program_size = size;
  gettimeofday (&burn_begin_time, NULL);
  
  flashaddr = address;
  while(program_size)
  {
	if(program_size > 0xFF)
	{
	  writelength = FLASH_PAGE_SIZE;
	}
	else
	{
	  writelength = program_size;//the final write
	}
	flash_page_program(flashaddr, writelength, start);
	program_size -= writelength;
	flashaddr += writelength;
	start += writelength;
  }
  gettimeofday (&burn_end_time, NULL);
  timeval_diff (&burn_total_time, &burn_begin_time, &burn_end_time);
  burn_time_sec = burn_total_time.tv_sec + (double)burn_total_time.tv_usec / 1000000;

  fprintf(pLogFile, "\n\nTotal burn time: %lu sec, %lu usec\n",
  burn_total_time.tv_sec, burn_total_time.tv_usec);
  fprintf(pLogFile, "Average burn rate: %.2lf KBytes/s\n", (double)(size >> 10) / burn_time_sec);
  
  fprintf(pLogFile, "Flash burning done.\n");
#endif
  release_image(content);

  terminate();
  return 0;
}

/* record the flash memory content to buffer */

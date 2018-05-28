/*******************************************************************************
 * File name: util.cpp
 * Description: Intel Flash burner
 * Author: Shiva	
 * Revision history:
 * 2011/05/23	Create this file.
 * 2012/05/24   Add comments - Kai
 * ****************************************************************************/

#include "util.h"
#include <stdio.h>

#ifdef USB_BURN
#ifdef LINUX
#include <dlfcn.h>
static void* hinstLib;
#else
#include <windows.h>
static HINSTANCE hinstLib;
#endif

typedef int (*fp_utif_init)(void);
typedef int (*fp_utif_select)(int);
typedef int (*fp_utif_open)(void);
typedef int (*fp_utif_close)(void);
typedef int (*fp_utif_read_reg)(int, int*);
typedef int (*fp_utif_write_reg)(int, int);
typedef int (*fp_utif_download_fw)(int, char*, int);

static fp_utif_init utif_init;
static fp_utif_select utif_select;
static fp_utif_open utif_open;
static fp_utif_close utif_close;
static fp_utif_read_reg utif_read_reg;
static fp_utif_write_reg utif_write_reg;
static fp_utif_download_fw utif_download_fw;
#endif

#ifdef __MINGW32__
SOCKET sock;
#else
int sock;
#endif

extern unsigned int guiWithMultiout;
extern FILE *pLogFile;                  /* log file */

#define TARGET_ERROR "Burning failed due to target errors."
#define CONNECT_ERROR "Burning failed due to protocol errors or command timeout. (Wrong Port/Service!?)"

/* send command to iceman
 * command code refer to util.h */
int send_cmd(char cmd)
{
#ifdef SPI_BURN
  char send_data[2],recv_data[2];
  int retval __attribute__((unused));
  send_data[0] = cmd;

  retval = SEND(sock, send_data, 1);
  retval = RECV(sock, recv_data, 2);

  if (recv_data[0] == cmd) 
  {
    if( recv_data[1] == 0 ) {
      return (int)(recv_data[1]);
    }
    else {
      fprintf(pLogFile, "\n!! send_cmd: %s !!\n", TARGET_ERROR);
      fflush(pLogFile);
      close_connection ();
      exit (0);
    }
  }
  else 
  {
    fprintf(pLogFile, "\n!! send_cmd: %s !!\n", CONNECT_ERROR);
    fflush(pLogFile);
    close_connection ();
    exit (0);
  }
#else
  return 0;
#endif
}

/* write word */
/* could use outw write word to target memory */
int outw(unsigned int address, unsigned int data)
{
#ifdef SPI_BURN
  char send_data[10],recv_data[2];
  int retval __attribute__((unused));
  send_data[0] = WRITE_WORD;
  send_data[2] = (char)((address & 0x000000FF) >> 0);
  send_data[3] = (char)((address & 0x0000FF00) >> 8);
  send_data[4] = (char)((address & 0x00FF0000) >> 16);
  send_data[5] = (char)((address & 0xFF000000) >> 24);
  send_data[6] = (char)((data & 0x000000FF) >> 0);
  send_data[7] = (char)((data & 0x0000FF00) >> 8);
  send_data[8] = (char)((data & 0x00FF0000) >> 16);
  send_data[9] = (char)((data & 0xFF000000) >> 24);

  retval = SEND(sock, send_data, 10);
  retval = RECV(sock, recv_data, 2);

  if (recv_data[0] == WRITE_WORD) 
  {
    return (int)(recv_data[1]);
  }
  else 
  {
    fprintf(pLogFile, "\n!! outw: %s !!\n", TARGET_ERROR);
    fflush(pLogFile);

    close_connection ();
    exit (0);
  }
#else
  utif_write_reg(address,data);
  return 0;
#endif
}

/* fast write */
/* could use fastout write mult-word from target memory */
int fastout(unsigned int address, unsigned int size, char * buffer)
{
#ifdef SPI_BURN
  char send_data[2048],recv_data[2];
  int retval __attribute__((unused));
  send_data[0] = FAST_WRITE;
  *(int *)(send_data + 2) = address;
  *(int *)(send_data + 6) = size;

  memcpy(send_data + 12, buffer, size);

  retval = SEND(sock, send_data, size+12);
  retval = RECV(sock, recv_data, 2);

  if (recv_data[0] == FAST_WRITE)
  {
    return 0;
  }
  else
  {
    fprintf(pLogFile, "\n!! fastout: %s !!\n", TARGET_ERROR);
    fflush(pLogFile);

    close_connection ();
    exit (0);
  }
#else
  utif_download_fw(address, buffer, size);
  return 0;
#endif
}

/* read word */
/* could use inw read word from target memory */
int inw(unsigned int address)
{
#ifdef SPI_BURN
  int data;
  char send_data[6],recv_data[6];
  int retval __attribute__((unused));
  send_data[0] = READ_WORD;
  send_data[2] = (char)((address & 0x000000FF) >> 0);
  send_data[3] = (char)((address & 0x0000FF00) >> 8);
  send_data[4] = (char)((address & 0x00FF0000) >> 16);
  send_data[5] = (char)((address & 0xFF000000) >> 24);

  retval = SEND(sock, send_data, 6);
  retval = RECV(sock, recv_data, 6);

  data = (((recv_data[2]<< 24) & 0xFF000000) |
          ((recv_data[3]<< 16) & 0x00FF0000) |
          ((recv_data[4]<< 8) & 0x0000FF00) |
          ((recv_data[5]<< 0) & 0x000000FF));

  if (recv_data[0] == READ_WORD) 
    return data;
  else 
  {
    fprintf(pLogFile, "\n!! inw: %s !!\n", TARGET_ERROR);
    fflush(pLogFile);

    close_connection ();
    exit (0);
  }
#else
  int val;
  utif_read_reg(address,&val);
  return val;
#endif
}

/* Close connection with ICEman */
void close_connection (void)
{
#ifdef SPI_BURN
  char send_data[2];
  int retval __attribute__((unused));

  send_data[0] = 4; // Termination command code
  retval = SEND(sock, send_data, 1);

  // free socket resource
#ifdef __MINGW32__
  closesocket (sock);
  do
  {
    WSACleanup ();
  } while (WSAGetLastError () != WSANOTINITIALISED);
#else
  close (sock);
#endif
#endif
}

/* Need send terminate to ICEman before quit Burner */
void terminate(void)
{
#ifdef SPI_BURN
  close_connection ();
#else
  utif_close();
  #ifdef LINUX
  dlclose(hinstLib);
  #else
  FreeLibrary(hinstLib);
  #endif
#endif
}

/* SIGINT handling function */
void handle_int(int signo)
{
  terminate();
  fprintf(pLogFile, "\n!! burn not done yet !!\n");
  fflush(pLogFile);
  exit(0);
}

#define DEF_CONNECT_HOST      "127.0.0.1"
/* Prepare socket communication */
void initial_socket(unsigned int port)
{
#ifdef SPI_BURN	
	
  struct sockaddr_in server_addr;  

#ifdef __MINGW32__
  WSADATA wsadata;
  if (WSAStartup(MAKEWORD(1,1), &wsadata) == SOCKET_ERROR) {
      fprintf(pLogFile, "Error creating socket.");
      fflush(pLogFile);
      fflush(stdout);
  }
#endif

#ifdef __MINGW32__
  if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
#else
  if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
#endif
  {
     fprintf(pLogFile, "can't create socket\n");
     fflush(pLogFile);
     exit(1);
  }
  server_addr.sin_family = PF_INET;     
  server_addr.sin_port = htons(port);   
#ifdef __MINGW32__
  server_addr.sin_addr.s_addr = inet_addr(DEF_CONNECT_HOST);
#else
  inet_aton(DEF_CONNECT_HOST, &server_addr.sin_addr);
#endif 
  memset(&(server_addr.sin_zero),0,8); 
  if (connect(sock, (struct sockaddr *)&server_addr,
                    sizeof(struct sockaddr)) == -1)
  {
    fprintf(pLogFile, "connecting fail !!\n");
    fflush(pLogFile);
    exit(1);
  }

  // Set RCV/SND Timeout 
  struct timeval timeout;

#ifdef __MINGW32__ 
  timeout.tv_sec = 30000;  // 30s
  timeout.tv_usec = 0;
#else 
  timeout.tv_sec = 30;  // 30s
  timeout.tv_usec = 0;
#endif

  if( setsockopt( sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0 )
  {
    fprintf(pLogFile, "setsockopt SO_RCVTIMEO fail !!\n");
    fflush(pLogFile);
    exit(1);
  }

  if( setsockopt( sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0 ) {
    fprintf(pLogFile, "setsockopt SO_SNDTIMEO fail !!\n");
    fflush(pLogFile);
    exit(1);
  }

#else
    int rlt = 0;

	#ifdef LINUX
	hinstLib = dlopen("./libutif.so",RTLD_LAZY);
	#else
    hinstLib = LoadLibrary("utif.dll");
    #endif
    if(hinstLib == NULL)
    {
        fprintf(pLogFile, "LOAD UTIF FAILED\n");
		fflush(pLogFile);
        exit(1);
    }

    fprintf(pLogFile, "LOAD UTIF OK\n");

	#ifdef LINUX
	utif_init = (fp_utif_init)dlsym(hinstLib, "utif_init");
    utif_select = (fp_utif_select)dlsym(hinstLib, "utif_select");
    utif_open = (fp_utif_open)dlsym(hinstLib, "utif_open");
    utif_close = (fp_utif_close)dlsym(hinstLib, "utif_close");
    utif_read_reg = (fp_utif_read_reg)dlsym(hinstLib, "utif_read_reg");
    utif_write_reg = (fp_utif_write_reg)dlsym(hinstLib, "utif_write_reg");
    utif_download_fw = (fp_utif_download_fw)dlsym(hinstLib, "utif_download_fw");
	#else
    utif_init = (fp_utif_init)GetProcAddress(hinstLib, "utif_init");
    utif_select = (fp_utif_select)GetProcAddress(hinstLib, "utif_select");
    utif_open = (fp_utif_open)GetProcAddress(hinstLib, "utif_open");
    utif_close = (fp_utif_close)GetProcAddress(hinstLib, "utif_close");
    utif_read_reg = (fp_utif_read_reg)GetProcAddress(hinstLib, "utif_read_reg");
    utif_write_reg = (fp_utif_write_reg)GetProcAddress(hinstLib, "utif_write_reg");
    utif_download_fw = (fp_utif_download_fw)GetProcAddress(hinstLib, "utif_download_fw");
	#endif

    // usb interface init
    rlt = utif_select(4);
    rlt = utif_open();
    if(rlt)
    {
        fprintf(pLogFile, "USB OPEN FAILED\n");
		fflush(pLogFile);
		#ifdef LINUX
		dlclose(hinstLib);
		#else
        FreeLibrary(hinstLib);
        #endif
        exit(1);
    }

    fprintf(pLogFile, "USB OPEN OK\n");
    return 0;
#endif
}

/* Read image binary data */
unsigned char* get_image(FILE* image, unsigned int* size)
{
  unsigned char* flash;
  int retval __attribute__((unused));
  if (image == NULL)
  {
    fprintf (pLogFile, "Error: Burn image have to specify.\n");
    fflush(pLogFile);
    exit(1);
  }

  fseek(image, 0L, SEEK_END);
  (*size) = (ftell(image));
  fseek(image, 0L, SEEK_SET);

  if ((flash = (UINT8*)malloc((*size) * (sizeof(UINT8)))) == NULL) {
     fprintf(pLogFile, "Error: can't allocate memory (%d bytes) for flash file\n", (*size));
     fflush(pLogFile);
     return 0;
  }
  retval = fread(flash, (*size), 1, image);
  return flash;
}

void release_image(unsigned char* handle)
{
	free(handle);
}

/* Performance utility functions - to calculate the difference of a_timeval_begin and a_timeval_end */
void timeval_diff (struct timeval *a_result, struct timeval *a_timeval_begin, struct timeval *a_timeval_end)
{
  if (a_timeval_end->tv_usec >= a_timeval_begin->tv_usec)
  {
    a_result->tv_usec = (a_timeval_end->tv_usec - a_timeval_begin->tv_usec);
    a_result->tv_sec = (a_timeval_end->tv_sec - a_timeval_begin->tv_sec);
    if (a_result->tv_usec >= 1000000)
    {
      a_result->tv_usec -= 1000000;
      a_result->tv_sec += 1;
    }
  }
  else
  {
    a_result->tv_usec = (a_timeval_end->tv_usec + (1000000 - a_timeval_begin->tv_usec));
    a_result->tv_sec = (a_timeval_end->tv_sec - a_timeval_begin->tv_sec - 1);
  }
}

/* send command to iceman
 * command code refer to util.h */
int send_coreid(int coreid)
{
#ifdef SPI_BURN
  char send_data[6],recv_data[2];
  int retval __attribute__((unused));
  send_data[0] = BURNER_SELECT_CORE;
 
  set_u32(send_data+1, coreid);  

  retval = SEND(sock, send_data, 5);
  retval = RECV(sock, recv_data, 2);

  if (recv_data[0] == BURNER_SELECT_CORE && recv_data[1] == 0) 
  {
    return (int)(recv_data[1]);
  }
  else 
  {
    fprintf(pLogFile, "\n!! send_coreid: %s !!\n", TARGET_ERROR);
    fflush(pLogFile);

    close_connection ();
    exit (0);
  }
#else
  return 0;
#endif
}

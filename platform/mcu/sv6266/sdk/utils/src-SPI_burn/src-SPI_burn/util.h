/*******************************************************************************
 * File name: util.h
 * Description: Flash Burner
 * Author: Shiva
 * Revision history:
 * 2011/05/23	Create this file.
 * ****************************************************************************/
#ifndef __UTIL__
#define __UTIL__

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <getopt.h>

#ifdef __MINGW32__
#include <winsock2.h>
#undef socklen_t
#define socklen_t int
#define sleep(t) _sleep(t * 1000)
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#ifdef __MINGW32__
#define    SEND(SOCKET, DATA, SIZE) send(SOCKET, DATA, SIZE, 0);
#define    RECV(SOCKET, DATA, SIZE) recv(SOCKET, DATA, SIZE, 0);
#else
#define    SEND(SOCKET, DATA, SIZE) write(SOCKET, DATA, SIZE);
#define    RECV(SOCKET, DATA, SIZE) read(SOCKET, DATA, SIZE);
#endif

#define MAX_MULTI_WRITE_PAIR  256

// PIPE MAX_SIZE = 65536 Bytes
#define MAX_SOCKET_BUF        65536



// Command code
#define WRITE_WORD   (0x1A)
#define READ_WORD    (0x1B)
#define WRITE_BYTE   (0x2A)
#define READ_BYTE    (0x2B)
#define WRITE_HALF   (0x4A)
#define READ_HALF    (0x4B)
#define FAST_READ    (0x1C)
#define WRITE_IO     (0x1F)
#define FAST_WRITE   (0x20)
#define BURNER_QUIT  (0x04)
#define MULTIPLE_WRITE_WORD (0x5A)
#define MULTIPLE_WRITE_HALF (0x5B)
#define MULTIPLE_WRITE_BYTE (0x5C)
#define MULTIPLE_READ_WORD  (0x5D)
#define MULTIPLE_READ_HALF  (0x5E)
#define MULTIPLE_READ_BYTE  (0x5F)
#define RESET_TARGET        (0x3A)
#define RESET_HOLD          (0x3B)
#define RESET_AICE          (0x3C)
#define HOLD_CORE           (0x1D)
#define BURNER_SELECT_CORE  (0x05)
#define MONITOR_CMD         (0x06)
#define READ_EDM_JDP        (0x6E)
#define WRITE_EDM_JDP       (0x6F)

#define set_u32(buffer, value) h_u32_to_le((uint8_t *)buffer, value)
#define get_u32(buffer) le_to_h_u32((const uint8_t *)buffer)

/* type define */
typedef unsigned long long      UINT64;
typedef long long               INT64;
typedef unsigned int            UINT32;
typedef int                     INT32;
typedef unsigned short          UINT16;
typedef short                   INT16;
typedef unsigned char           UINT8;
//typedef char                    INT8;


/* read/write API */
int outw(unsigned int address, unsigned int data);
int inw(unsigned int address);
int fastout(unsigned int address, unsigned int size, char * buffer);

void close_connection (void);
void terminate(void);
void handle_int(int signo);
int send_cmd(char cmd);
int send_coreid(int coreid);

void initial_socket(unsigned int port);
/* get image from file */
unsigned char* get_image(FILE* image, unsigned int* size);
void release_image(unsigned char* handle);
void timeval_diff (struct timeval *a_result, struct timeval *a_timeval_begin, struct timeval *a_timeval_end);


static inline void h_u32_to_le(uint8_t* buf, int val)
{
    buf[3] = (uint8_t) (val >> 24);
    buf[2] = (uint8_t) (val >> 16);
    buf[1] = (uint8_t) (val >> 8);
    buf[0] = (uint8_t) (val >> 0);
}

static inline uint32_t le_to_h_u32(const uint8_t* buf)
{
    return (uint32_t)(buf[0] | buf[1] << 8 | buf[2] << 16 | buf[3] << 24);
}


#endif


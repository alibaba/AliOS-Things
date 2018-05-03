#ifndef PACKETS_H
#define PACKETS_H

#define MAX_STRING_SIZE 1024
#define MAX_MODE_SIZE 8
#define MAX_DATA_SIZE 512

//optcodes
#define TFTP_OPTCODE_RRQ  1
#define TFTP_OPTCODE_WRQ  2
#define TFTP_OPTCODE_DATA 3
#define TFTP_OPTCODE_ACK  4
#define TFTP_OPTCODE_ERR  5

//Error Codes
#define TFTP_ERRCODE_UNDEFINED           0
#define TFTP_ERRCODE_FILE_NOT_FOUND      1
#define TFTP_ERRCODE_ACCESS_VIOLATION    2
#define TFTP_ERRCODE_DISK_FULL           3
#define TFTP_ERRCODE_ILLEGAL_OPERATION   4
#define TFTP_ERRCODE_UNKNOWN_TRANSFER_ID 5
#define TFTP_ERRCODE_FILE_ALREADY_EXISTS 6
#define TFTP_ERRCODE_NO_SUCH_USER        7

typedef struct {
    char filename[MAX_STRING_SIZE+1];
    char mode[MAX_MODE_SIZE+1];
} RRQ, WRQ;

typedef struct {
  uint16_t blockNumber;
  unsigned int dataSize;
  char data[MAX_DATA_SIZE];
} DATA;

typedef struct {
  uint16_t blockNumber;
} ACK;

typedef struct {
  uint16_t errorCode;
  char message[MAX_STRING_SIZE];
} ERROR;

typedef struct {
  uint16_t optcode;
  union {
    RRQ read_request;
    WRQ write_request;
    DATA data;
    ACK ack;
    ERROR error;
  };
} PACKET;


PACKET* unserializePacket(char * buffer, size_t bufferSize, PACKET * packet);
size_t serializePacket(const PACKET* packet, char * buffer);

void printPacket(PACKET* packet);

void printError(PACKET * packet);

#endif

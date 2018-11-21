#ifndef __HAL_SECURE_H
#define __HAL_SECURE_H

/* Exported constants --------------------------------------------------------*/
#define T0_PROTOCOL        0x00  /* T0 protocol */
#define ATR_BYTE0             0x3B  /* Direct bit convention */
#define INDIRECT           0x3F  /* Indirect bit convention */
#define SETUP_LENGTH       20
#define HIST_LENGTH        20
#define LC_MAX             256
#define SC_RECEIVE_TIMEOUT  200  /* Direction to reader */
#define SC_TRANSMIT_TIMEOUT 200  /* Direction to transmit */
#define MAX_ATR_LENGTH  40

/* ATR structure - Answer To Reset -------------------------------------------*/
typedef struct
{
  uint8_t TS;               /* Bit Convention */
  uint8_t T0;               /* High nibble = Number of setup byte; low nibble = Number of historical byte */
  uint8_t T[SETUP_LENGTH];  /* Setup array */
  uint8_t H[HIST_LENGTH];   /* Historical array */
  uint8_t Tlength;          /* Setup array dimension */
  uint8_t Hlength;          /* Historical array dimension */
} SC_ATR;

/* ADPU-Header command structure ---------------------------------------------*/
typedef struct
{
  uint8_t CLA;  /* Command class */
  uint8_t INS;  /* Operation code */
  uint8_t P1;   /* Selection Mode */
  uint8_t P2;   /* Selection Option */
  uint8_t P3;   /* LC or LE */
} SC_Header;

/* ADPU-Body command structure -----------------------------------------------*/
typedef struct 
{
  uint8_t LC;           /* Data field length */
  uint8_t Data[LC_MAX];  /* Command parameters */
  uint8_t LE;           /* Expected length of data to be returned */
} SC_Body;

/* ADPU Command structure ----------------------------------------------------*/
typedef struct
{
  SC_Header header;
  SC_Body body;
} SC_ADPU_Commands;

/* SC response structure -----------------------------------------------------*/
typedef struct
{
  uint8_t data[LC_MAX];  /* Data returned from the card */
  uint8_t SW1;          /* Command Processing status */
  uint8_t SW2;          /* Command Processing qualification */
} SC_ADPU_Response;

int DeviceOpen(void **handle);
int DeviceTransmit(void *handle, unsigned char *input, int input_len,
                    unsigned char *output, int *output_len);
int DeviceClose(void *handle);
int id2_test_se(void);

#endif
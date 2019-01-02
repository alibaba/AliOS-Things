/*
 * Copyright 2013 Tenkiv, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

/**
 * @file TelnetServer.h
 * @brief Header file for the Telnet server of the Tekdaqc.
 *
 * Contains public definitions and data types for the Tekdaqc Telnet server.
 *
 * @author Jared Woolston (jwoolston@tenkiv.com)
 * @since v1.0.0.0
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TELNET_SERVER_H_
#define TELNET_SERVER_H_

/* Define to provide proper behavior with C++ compilers ----------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------------------------------------*/

#include "lwip/tcp.h"

/** @addtogroup tekdaqc_firmware_libraries Tekdaqc Firmware Libraries
 * @{
 */

/** @addtogroup telnet_server Telnet Server
 * @{
 */

/*--------------------------------------------------------------------------------------------------------*/
/* EXPORTED CONSTANTS */
/*--------------------------------------------------------------------------------------------------------*/

/**
 * @def TELNET_BUFFER_LENGTH
 * @brief The length of the buffer to use for Telnet data.
 */
/* modify from 2056 to 4096 */
#define TELNET_BUFFER_LENGTH 4096

/**
 * @def NOT_CONNECTED
 * @brief The Telnet server is not connected to a client.
 */
#define NOT_CONNECTED   0

/**
 * @def CONNECTED
 * @brief The Telnet server is connected to a client.
 */
#define CONNECTED       1

/**
 * @internal
 * Telnet commands, as defined by RFC854.
 */
#define TELNET_IAC              ((char) 255)
#define TELNET_WILL             ((char) 251)
#define TELNET_WONT             ((char) 252)
#define TELNET_DO               ((char) 253)
#define TELNET_DONT             ((char) 254)
#define TELNET_SE               ((char) 240)
#define TELNET_NOP              ((char) 241)
#define TELNET_DATA_MARK        ((char) 242)
#define TELNET_BREAK            ((char) 243)
#define TELNET_IP               ((char) 244)
#define TELNET_AO               ((char) 245)
#define TELNET_AYT              ((char) 246)
#define TELNET_EC               ((char) 247)
#define TELNET_EL               ((char) 248)
#define TELNET_GA               ((char) 249)
#define TELNET_SB               ((char) 250)

/**
 * @internal
 * Telnet options, as defined by RFC856-RFC861.
 */
#define TELNET_OPT_BINARY       ((char) 0)
#define TELNET_OPT_ECHO         ((char) 1)
#define TELNET_OPT_SUPPRESS_GA  ((char) 3)
#define TELNET_OPT_STATUS       ((char) 5)
#define TELNET_OPT_TIMING_MARK  ((char) 6)
#define TELNET_OPT_EXOPL        ((char) 255)

/**
 * @def OPT_FLAG_WILL
 * @brief The bit in the ucFlags member of the tTelnetOpts that is set when the remote client has sent a WILL
 * request and the server has accepted it.
 */
#define OPT_FLAG_WILL           ((uint8_t) 1)

/**
 * @def OPT_FLAG_DO
 * @brief The bit in the ucFlags member of tTelnetOpts that is set when the remote
 * client has sent a DO request and the server has accepted it.
 */
#define OPT_FLAG_DO             ((uint8_t) 2)

/**
 * @brief Telnet options state structure.
 * A structure that contains the state of the options supported by the telnet
 * server, along with the possible flags.
 */
typedef struct {
	char option; /**< The option byte. */
	char flags; /**< The flags for this option. The bits in this byte are defined by OPT_FLAG_WILL and OPT_FLAG_DO. */
} TelnetOpts_t;

/**
 * @brief Telnet parser state enumeration.
 * The possible states of the telnet option parser.
 */
typedef enum {
	STATE_NORMAL, /**< The telnet option parser is in its normal mode.  Characters are passed as is until an IAC byte is received. */
	STATE_IAC, /**< The previous character received by the telnet option parser was an IAC byte. */
	STATE_WILL, /**< The previous character sequence received by the telnet option parser was IAC WILL. */
	STATE_WONT, /**< The previous character sequence received by the telnet option parser was IAC WONT. */
	STATE_DO, /**< The previous character sequence received by the telnet option parser was was IAC DO. */
	STATE_DONT, /**< The previous character sequence received by the telnet option parser was was IAC DONT. */
} TelnetState_t;

/**
 * @brief Telnet status enumeration.
 * The possible success/error causes for the Telnet server's operation.
 */
typedef enum {
	TELNET_OK, /**< Everything is normal with the telnet server. */
	TELNET_ERR_CONNECTED, /**< There was an error in connection with the telnet server. */
	/*TELNET_ERR_BADALOC,*//**< There was an error allocating memory for the telnet server. */
	TELNET_ERR_BIND, /**< There was an error binding a socket to a port for the telnet server. */
	TELNET_ERR_PCBCREATE /**< There was an error creating a PCB structure for the telnet server. */
} TelnetStatus_t;

/**
 * @brief Data structure to hold the state of the Telnet server.
 * Contains all of the necessary state variables to impliment the Telnet server. Direct manipulation of these
 * members is not recommended as it may leave the server in an inconsistent state. Instead, helper methods are
 * provided which will ensure that all necessary operations occur as a result of any change.
 */
typedef struct {
	int halt; /**< Halt signal when the lwIP TCP/IP stack has detected an error */
	TelnetState_t state; /**< The current state of the telnet option parser. */
	volatile unsigned long outstanding; /**< A count of the number of bytes that have been transmitted but have not yet been ACKed. */
	unsigned long close; /**< A value that is non-zero when the telnet connection should be closed down. */
	unsigned char buffer[TELNET_BUFFER_LENGTH]; /**< A buffer used to construct a packet of data to be transmitted to the telnet client. */
	volatile unsigned long length; /**< The number of bytes of valid data in the telnet packet buffer. */
	unsigned char recvBuffer[TELNET_BUFFER_LENGTH]; /**< A buffer used to receive data from the telnet connection. */
	volatile unsigned long recvWrite; /**< The offset into g_pucTelnetRecvBuffer of the next location to be written in the buffer.
	 The buffer is full if this value is one less than g_ulTelnetRecvRead (modulo the buffer size).*/
	volatile unsigned long recvRead; /**< The offset into g_pucTelnetRecvRead of the next location to be read from the buffer.
	 The buffer is empty if this value is equal to g_ulTelnetRecvWrite. */
	struct tcp_pcb* pcb; /**< A pointer to the telnet session PCB data structure. */
	unsigned char previous; /**< The character most recently received via the telnet interface.  This is used to convert CR/LF sequences
	 into a simple CR sequence. */
} TelnetServer_t;

/**
 * @brief Initialize a TelnetServer_t struct with default values.
 */
TelnetStatus_t InitializeTelnetServer(void);

/**
 * @brief This function is called when the the TCP connection should be closed.
 */
void TelnetClose(void);

/**
 * @brief Indicates if the Telnet server is occupied or not.
 */
bool TelnetIsConnected(void);

/**
 * @brief Called when the lwIP TCP/IP stack needs to poll the server with/for data.
 */
err_t TelnetPoll(void *arg, struct tcp_pcb *tpcb);

/**
 * @brief Writes a character into the telnet receive buffer.
 */
void TelnetRecvBufferWrite(char character);

/**
 * @brief Reads a character from the telnet interface.
 */
char TelnetRead(void);

/**
 * @brief Writes a character to the telnet interface.
 */

void TelnetWrite(const char character);

/**
 * @brief Writes a string to the telnet interface.
 */
void TelnetWriteString(char* string);

/**
 * @brief Handle a WILL request for a telnet option.
 */
void TelnetProcessWill(char option);

/**
 * @brief Handle a WONT request for a telnet option.
 */
void TelnetProcessWont(char option);

/**
 * @brief Handle a DO request for a telnet option.
 */
void TelnetProcessDo(char option);

/**
 * @brief Handle a DONT request for a telnet option.
 */
void TelnetProcessDont(char option);

/**
 * @brief Process a character received from the telnet port.
 */
void TelnetProcessCharacter(char character);

/**
 * @brief Print a message to the telnet connection formatted as an error.
 */
void TelnetWriteErrorMessage(char* message);

/**
 * @brief Print a message to the telnet connection formatted as a status.
 */
void TelnetWriteStatusMessage(char* message);

/**
 * @brief Print a message to the telnet connection formatted as a debug.
 */
void TelnetWriteDebugMessage(char* message);

/**
 * @brief Print a message to the telnet connection formatted as a command data message.
 */
void TelnetWriteCommandDataMessage(char* message);

void telnetserver_start();

void telnetserver_stop();

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* TELNET_SERVER_H_ */

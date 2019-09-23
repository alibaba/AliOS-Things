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
 * @file TelnetServer.c
 * @brief Implements a control interface for the Tekdaqc via the Telnet protocol.
 *
 * Implements a control interface for the Tekdaqc via the Telnet protocol. Only a single connection
 * is allowed at a time. Any attempts to connect while an active connection is present will result in
 * an error message from the board.
 *
 * This file based on the Telnet server implementation in the TI Stellaris example Cave Adventure game,
 * in particular, the methods for processing the Telnet state machine.
 *
 * @author Jared Woolston (jwoolston@tenkiv.com)
 * @since v1.0.0.0
 */

/*--------------------------------------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------------------------------------*/

#include "lwip/apps/telnetserver.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include <stdio.h>
#include <inttypes.h>
#define TAG "TELNETD"
/*--------------------------------------------------------------------------------------------------------*/
/* PRIVATE DEFINES */
/*--------------------------------------------------------------------------------------------------------*/
#define SIZE_TOSTRING_BUFFER 512U

#define TELNET_PORT 23U

#define ERROR_MESSAGE_HEADER "\n\r--------------------\n\rError Message\n\r\tMessage: %s\n\r--------------------\n\r\x1E"
#define STATUS_MESSAGE_HEADER "\n\r--------------------\n\rStatus Message\n\r\tMessage: %s\n\r--------------------\n\r\x1E"
#define DEBUG_MESSAGE_HEADER "\n\r--------------------\n\rDebug Message\n\r\tMessage: %s\n\r--------------------\n\r\x1E"
#define COMMAND_DATA_MESSAGE_HEADER "\n\r--------------------\n\rCommand Data Message\n\r\tMessage: %s\n\r--------------------\n\r\x1E"

//#define TELNET_DEBUG
//#define TELNET_CHAR_DEBUG
/*--------------------------------------------------------------------------------------------------------*/
/* PRIVATE VARIABLES */
/*--------------------------------------------------------------------------------------------------------*/

/**
 * @internal
 * @brief Pointer to the TCP port being used for the Telnet server.
 */
static struct tcp_pcb *telnet_pcb;

/**
 * @internal
 * @brief Pointer to the current Telnet server.
 */
static TelnetServer_t telnet_server;

/**
 * @internal
 * @brief Indicates the connection status of the telnet server.
 */
static bool IsConnected = false;

/**
 * @internal
 * @brief Buffer for printing the TOSTRING_BUFFER with additional formatting.
 */
static char MESSAGE_BUFFER[SIZE_TOSTRING_BUFFER];

/**
 * @internal
 * @brief The error message provided when an attempt is made to play the game when
 *         it is already being played over a different interface.
 */
static const char ErrorMessage[53] =
		"The Tekdaqc is already in use...try again later!\r\n";

/**
 * @internal
 * @brief The initialization sequence sent to a remote telnet client when it first connects to the telnet server.
 */
static const char TelnetInit[] = { TELNET_IAC, TELNET_DO,
TELNET_OPT_SUPPRESS_GA, TELNET_IAC, TELNET_WILL, TELNET_OPT_ECHO };

/**
 * @internal
 * @brief This telnet server will always suppress go ahead generation, regardless of this setting.
 */
static TelnetOpts_t TelnetOptions[] = { { .option = TELNET_OPT_SUPPRESS_GA,
		.flags = (0x01 << OPT_FLAG_WILL) }, { .option = TELNET_OPT_ECHO,
		.flags = (1 << OPT_FLAG_DO) } };

/*--------------------------------------------------------------------------------------------------------*/
/* PRIVATE FUNCTION PROTOTYPES */
/*--------------------------------------------------------------------------------------------------------*/

/**
 * @brief Called when the lwIP TCP/IP stack has an incoming connection request on the telnet port.
 */
static err_t TelnetAccept(void *arg, struct tcp_pcb *pcb, err_t err);

/**
 * @brief Called when the lwIP TCP/IP stack has an incoming packet to be processed.
 */
static err_t TelnetReceive(void *arg, struct tcp_pcb *pcb, struct pbuf *p,
		err_t err);

/**
 * @brief Called when the lwIP TCP/IP stack has received an acknowledge for data that has been transmitted.
 */
static err_t TelnetSent(void *arg, struct tcp_pcb *pcb, u16_t len);

/**
 * @brief Called when the lwIP TCP/IP stack has detected an error.
 */
static void TelnetError(void *arg, err_t err);

/**
 * @brief Clears the internal message string buffer.
 */
static void ClearToMessageBuffer(void);

/**
 * @brief Creates an initalizes a Telnet server.
 */
static TelnetServer_t* CreateTelnetServer(void);

/*--------------------------------------------------------------------------------------------------------*/
/* PRIVATE METHODS */
/*--------------------------------------------------------------------------------------------------------*/

/*
 * @internal
 * This function is called when the lwIP TCP/IP stack has an incoming
 * connection request on the telnet port.
 *
 * @param arg void* Argument pointer passed to the handler by the lwIP stack.
 * @param pcb tcp_pcb* struct The PCB structure this callback is for.
 * @param err lwIP err_t with the current error status.
 * @retval err lwIP err_t with the result of the this function.
 */
static err_t TelnetAccept(void *arg, struct tcp_pcb *pcb, err_t err) {
#ifdef TELNET_DEBUG
	LOGD(TAG, "[Telnet Server] Incoming connection requested on Telnet port.\n\r");
#endif
	err_t ret_err;

	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(err);

	/* Check if already connected. */
	if (TelnetIsConnected() == true) {
		/* There is already a connected client, so refuse this connection with
		 a message indicating this fact. */
#ifdef TELNET_DEBUG
		LOGD(TAG, "[Telnet Server] A connection was attempted while an active connection is open.\n\r");
#endif
		tcp_accepted(pcb);
		tcp_arg(pcb, NULL);
		tcp_sent(pcb, TelnetSent);
		tcp_write(pcb, ErrorMessage, sizeof(ErrorMessage), 1);
		tcp_output(pcb);
		/* Temporarily accept this connection until the message is transmitted. */
		return (ERR_OK);
	}

	/* Setup the TCP connection priority. */
	tcp_setprio(pcb, TCP_PRIO_MIN);

	CreateTelnetServer();
#ifdef TELNET_DEBUG
	LOGD(TAG, "[Telnet Server] Initializing telnet server.\n\r");
#endif
	tcp_nagle_enable(pcb);
	telnet_server.pcb = pcb;
	telnet_server.pcb->so_options |= SOF_KEEPALIVE;
	telnet_server.pcb->keep_idle = 300000UL; // 5 Minutes
	telnet_server.pcb->keep_intvl = 1000UL; // 1 Second
	telnet_server.pcb->keep_cnt = 9; // 9 Consecutive failures terminate

	/* Mark that a client has connected. */
	IsConnected = true;
	/* Accept this connection. */
	tcp_accepted(pcb);
#ifdef TELNET_DEBUG
	LOGD(TAG, "[Telnet Server] An incoming connection was accepted.\n\r");
#endif

	/* Setup the TCP callback argument. */
	tcp_arg(pcb, &telnet_server);

	/* Initialize lwIP tcp_recv callback function for pcb  */
	tcp_recv(pcb, TelnetReceive);

	/* Initialize lwIP tcp_err callback function for pcb  */
	tcp_err(pcb, TelnetError);

	/* Initialize lwIP tcp_poll callback function for pcb */
	tcp_poll(pcb, TelnetPoll, 1);

	/* Setup the TCP sent callback function. */
	tcp_sent(pcb, TelnetSent);
	/* Initialize the count of outstanding bytes.  The initial byte acked as
	 part of the SYN -> SYN/ACK sequence is included so that the byte count
	 works out correctly at the end. */
	telnet_server.outstanding = sizeof(TelnetInit) + 1;
	/* Do not close the telnet connection until requested. */
	telnet_server.close = 0;
#ifdef TELNET_DEBUG
	LOGD(TAG, "[Telnet Server] Writing the init messages\n\r");
#endif
	/* Send the telnet initialization string. */
	tcp_write(pcb, TelnetInit, sizeof(TelnetInit), 1);
	tcp_output(pcb);

	TelnetWriteDebugMessage("[TELNET] Telnet Server Connected. Welcome.");

#ifdef TELNET_DEBUG
	LOGD(TAG, "[Telnet Server] Telnet Server Connected. Welcome.\n\r");
#endif
	/* Return a success code. */
	ret_err = ERR_OK;
	return ret_err;
}

/*
 * @internal
 * This function is called when the lwIP TCP/IP stack has an incoming packet to
 * be processed.
 *
 * @param arg void* Argument pointer passed to the handler by the lwIP stack.
 * @param pcb tcp_pcb* struct The PCB structure this callback is for.
 * @param p pbuf* struct The data buffer from the lwIP stack.
 * @param err lwIP err_t with the current error status.
 * @retval err lwIP err_t with the result of the this function.
 */
static err_t TelnetReceive(void *arg, struct tcp_pcb *pcb, struct pbuf *p,
		err_t err) {
	struct pbuf *q;
	unsigned long ulIdx;
	unsigned char *pucData;
	TelnetServer_t* server;
	if (arg != NULL) {
		server = (TelnetServer_t*) arg;

		/* Process the incoming packet. */
		if ((err == ERR_OK) && (p != NULL)) {
#ifdef TELNET_DEBUG
			LOGD(TAG, "[Telnet Server] Processing received packet.\n\r");
#endif
			/* Accept the packet from TCP. */
			tcp_recved(pcb, p->tot_len);
			/* Loop through the pbufs in this packet. */
			for (q = p, pucData = (unsigned char*) q->payload; q != NULL;
					q = q->next) {
				/* Loop through the bytes in this pbuf. */
				for (ulIdx = 0; ulIdx < q->len; ulIdx++) {
					/* Process this character. */
					TelnetProcessCharacter(pucData[ulIdx]);
				}
			}
			/* Free the pbuf. */
			pbuf_free(p);
		} else if ((err == ERR_OK) && (p == NULL)) {
			/* If a null packet is passed in, close the connection. */
			server->length = 0;
			TelnetClose();
		}
	} else {
#ifdef TELNET_DEBUG
		LOGD(TAG, "[Telnet Server] Could not cast receive args to telnet server struct because they were null.\n\r");
		return ERR_VAL;
#endif
	}
	/* Return okay. */
	return (ERR_OK);
}

/**
 * @internal
 * This function is called when the lwIP TCP/IP stack has received an
 * acknowledge for data that has been transmitted.
 *
 * @param arg void* Argument pointer passed to the handler by the lwIP stack.
 * @param pcb tcp_pcb* struct The PCB structure this callback is for.
 * @param len u16_t The number of bytes which were sent.
 * @retval err lwIP err_t with the result of the this function.
 */
static err_t TelnetSent(void *arg, struct tcp_pcb *pcb, u16_t len) {
	/* See if this is for the game connection or for a secondary connection. */
	if (arg) {
		TelnetServer_t* server = (TelnetServer_t*) arg;
#ifdef TELNET_DEBUG
		LOGD(TAG, "[Telnet Server] Sent packet was acknowledged.\n\r");
#endif
		/* Decrement the count of outstanding bytes. */
		server->outstanding -= len;
	} else {
		/* See if this is the ACK for the error message. */
		if (len == sizeof(ErrorMessage)) {
			/* Close this telnet connection now that the error message has been transmitted. */
			tcp_sent(pcb, 0);
			tcp_close(pcb);
		}
	}
	/* Return OK. */
	return (ERR_OK);
}

/**
 * @internal
 * This function implements the tcp_err callback function (called when a fatal
 * tcp_connection error occurs. As soon as this function is called, the Telnet
 * server is invalid and should not be used anymore.
 *
 * @param  arg Pointer to argument parameter
 * @param  err Not used
 * @retval None
 */
static void TelnetError(void *arg, err_t err) {
	LWIP_UNUSED_ARG(err);
	//#ifdef TELNET_DEBUG
	LOGD(TAG, "[Telnet Server] Telnet error received: %i\n\r", err);
	//#endif
	TelnetServer_t* server;
	server = (TelnetServer_t*) arg;
	if (server != NULL) {
		/* free es structure */
		mem_free(server);
	}
	IsConnected = false;
}

/**
 * @internal
 * Creates and initializes a Telnet server.
 *
 * @param none
 * @retval TelnetServer_t* The structure used to represent the current server.
 */
static TelnetServer_t* CreateTelnetServer(void) {
#ifdef TELNET_DEBUG
	LOGD(TAG, "[Telnet Server] Allocating memory for server.\n\r");
#endif
	/* Allocate structure server to maintain Telnet connection information */
	IsConnected = false;
	telnet_server.halt = false;
	telnet_server.recvWrite = 0;
	telnet_server.recvRead = 0;
	telnet_server.previous = 0;
	telnet_server.length = 0;
	for (int i = 0; i < TELNET_BUFFER_LENGTH; ++i) {
		telnet_server.recvBuffer[i] = 0;
	}
	return &telnet_server;
}

/**
 * @internal
 * Clears the internal message string buffer.
 *
 * @param none
 * @retval none
 */
static void ClearToMessageBuffer(void) {
	for (int i = 0; i < SIZE_TOSTRING_BUFFER; ++i) {
		MESSAGE_BUFFER[i] = '\0';
	}
}

/**
 * Initializes the provided TelnetServer_t struct with default values and creates a TCP port for it.
 *
 * @param none
 * @retval none
 */
TelnetStatus_t InitializeTelnetServer(void) {
	telnet_server.length = 0;
	telnet_server.outstanding = 0;
	for (uint_fast16_t i = 0; i < sizeof(telnet_server.buffer); ++i) {
		telnet_server.buffer[i] = 0;
	}
	/* Create a new tcp pcb */
#ifdef TELNET_DEBUG
	LOGD(TAG, "[Telnet Server] Creating TCP port for Telnet server.\n\r");
#endif
	telnet_pcb = tcp_new();

	if (telnet_pcb != NULL) {
#ifdef TELNET_DEBUG
		LOGD(TAG, "[Telnet Server] Successfully created Telnet TCP port.\n\r");
#endif
		err_t err;
		/* Bind telnet to port TELNET_PORT */
		err = tcp_bind(telnet_pcb, IP_ADDR_ANY, TELNET_PORT);

		if (err == ERR_OK) {
			/* Start tcp listening for Telnet PCB */
			telnet_pcb = tcp_listen(telnet_pcb);
#ifdef TELNET_DEBUG
			LOGD(TAG, "[Telnet Server] Now listening for incoming connections on port %i\n\r", TELNET_PORT);
#endif
			/* Initialize LwIP tcp_accept callback function */
			tcp_accept(telnet_pcb, TelnetAccept);
			return TELNET_OK;
		} else {
			/* Deallocate the pcb */
			memp_free(MEMP_TCP_PCB, telnet_pcb);
#ifdef TELNET_DEBUG
			LOGD(TAG, "[Telnet Server] Can not bind pcb\n\r");
#endif
			return TELNET_ERR_BIND;
		}
	} else {
#ifdef TELNET_DEBUG
		LOGD(TAG, "[Telnet Server] Can not create new TCP port.\n\r");
#endif
		return TELNET_ERR_PCBCREATE;
	}
}

/**
 * This function is called when the the TCP connection should be closed.
 *
 * @param none
 * @retval none
 */
void TelnetClose(void) {
	LOGD(TAG, "Closing telnet connection.\n\r");
	struct tcp_pcb *pcb = telnet_server.pcb;

	/* Remove all callbacks */
	tcp_arg(pcb, NULL);
	tcp_sent(pcb, NULL);
	tcp_recv(pcb, NULL);
	tcp_err(pcb, NULL);
	tcp_poll(pcb, NULL, 0);

	/* Clear the telnet data structure pointer, to indicate that there is no longer a connection. */
	telnet_server.pcb = 0;

	/* Close tcp connection */
	tcp_close(pcb);
	IsConnected = false;

	/* Re-initialize the Telnet Server */
	InitializeTelnetServer();
}

/**
 * Returns the connection status of the Telnet server. Since this implementation supports only
 * one client at a time, this method can be used to determine if an incoming request can be honored
 * as well as allowing other parts of the program to adjust their behavior depending on if a client
 * is connected.
 *
 * @param none
 * @retval bool TRUE if the telnet server has an active connection.
 */
bool TelnetIsConnected(void) {
	return (IsConnected);
}

/**
 * Called by the lwIP stack when there is data to send/receive to/from the Telnet server.
 *
 * @param arg void* to argument passed to callback by lwIP stack.
 * @param tpcb tcp_pcb* To the tcp_pcb struct for the current tcp connection.
 * @retval err_t The error/status code.
 */
err_t TelnetPoll(void *arg, struct tcp_pcb *tpcb) {
	err_t ret_err;
	TelnetServer_t* server;
	server = (TelnetServer_t*) arg;

	if (server != NULL) {
		unsigned long length = server->length;
		if ((server->pcb != NULL) && (length != 0)) {
			/* Write the data from the transmit buffer. */
			tcp_write(server->pcb, server->buffer, length, 1);

			/* Increment the count of outstanding bytes. */
			server->outstanding += length;

			/* Output the telnet data. */
			tcp_output(server->pcb);

			/* Reset the size of the data in the transmit buffer. */
			server->length = 0;
		}
		/* See if the telnet connection should be closed; this will only occur once
		 all transmitted data has been ACKed by the client (so that some or all
		 of the final message is not lost). */
		if (server->pcb && (server->outstanding == 0) && (server->close != 0)) {
#ifdef TELNET_DEBUG
			LOGD(TAG, "[Telnet Server] Telnet server should be closed.\n\r");
#endif
			TelnetClose();
		}
		ret_err = ERR_OK;
	} else {
#ifdef TELNET_DEBUG
		LOGD(TAG, "[Telnet Server] Cannot process poll request due to null server structure.\n\r");
#endif
		/* Nothing to be done */
		tcp_abort(tpcb);
		ret_err = ERR_ABRT;
	}
	return ret_err;
}

/**
 * Writes a character into the telnet receive buffer.
 *
 * @param character char The character to write.
 * @retval none
 */
void TelnetRecvBufferWrite(char character) {
	unsigned long ulWrite;
	/* Ignore this character if it is the NULL character. */
	if (character == 0) {
#ifdef TELNET_CHAR_DEBUG
		LOGD(TAG, "[Telnet Server] Ignorning NULL character.\n\r");
#endif
		return;
	}

	/* Ignore this character if it is the second part of a CR/LF or LF/CR sequence. */
	if (((character == '\r') && (telnet_server.previous == '\n'))
			|| ((character == '\n') && (telnet_server.previous == '\r'))) {
		return;
	}

	/* Store this character into the receive buffer if there is space for it. */
	ulWrite = telnet_server.recvWrite;
	if (((ulWrite + 1) % sizeof(telnet_server.recvBuffer))
			!= telnet_server.recvRead) {
		telnet_server.recvBuffer[ulWrite] = character;
		telnet_server.recvWrite = (ulWrite + 1)
				% sizeof(telnet_server.recvBuffer);
#ifdef TELNET_CHAR_DEBUG
		for (int i = 0; i <= telnet_server.recvWrite; ++i) {
			LOGD(TAG, "%c", telnet_server.recvBuffer[i]);
		}
		LOGD(TAG, "\n\r");
#endif
	} else {
#ifdef TELNET_DEBUG
		LOGD(TAG, "[Telnet Server] Could not store new character because buffer was full.\n\r");
#endif
	}

	/* Save this character as the previously received telnet character. */
	telnet_server.previous = character;
}

/**
 * Reads a character from the telnet interface.
 *
 * @param none
 * @retval char The character read from the telnet interface.
 */
char TelnetRead(void) {
    if (TelnetIsConnected() == true) {
	uint32_t read;
	char ret;
	/* Return a NULL if there is no data in the receive buffer. */
	read = telnet_server.recvRead;
	if (read == telnet_server.recvWrite) {
		return (0);
	}
	/* Read the next byte from the receive buffer. */
	ret = telnet_server.recvBuffer[read];
	telnet_server.recvRead = (read + 1) % sizeof(telnet_server.recvBuffer);
	/* Return the byte that was read. */
	return (ret);
     }
     else
     {
        return 0;
     }
}

/**
 * Writes a character to the telnet interface.
 *
 * @param character const char The character to write to the interface.
 * @retval none
 */
void TelnetWrite(const char character) {
    if (TelnetIsConnected() == true) {
	/* Delay until there is some space in the output buffer.  The buffer is not
	 completly filled here to leave some room for the processing of received
	 telnet commands. */
	while (telnet_server.length > (sizeof(telnet_server.buffer) - 32)) {
#ifdef TELNET_DEBUG
		LOGD(TAG, "[Telnet Server] Telnet buffer is full!\n\r");
#endif
		/* Handle periodic timers for LwIP */
                aos_msleep(200);
		//LwIP_Periodic_Handle(GetLocalTime());
	}

	/* Write this character into the output buffer. */
	telnet_server.buffer[telnet_server.length++] = character;
    }
}

/**
 * Writes a string to the specified Telnet server. Disables Ethernet
 * interrupts during this process in order to prevent an intervening
 * interrupt from corrupting the output buffer.
 *
 * @param string char* Pointer to a C-String to write to the interface.
 * @retval none
 */
void TelnetWriteString(char* string) {
	if (TelnetIsConnected() == true) {
#if 0
		Eth_EXTI_Disable();
#endif
		while (*string) {
			TelnetWrite(*string);
			++string;
		}
#if 0
		Eth_EXTI_Enable();
#endif
	}
}

/**
 * This function will handle a WILL request for a telnet option.  If it is an
 * option that is known by the telnet server, a DO response will be generated
 * if the option is not already enabled.  For unknown options, a DONT response
 * will always be generated.
 *
 * The response (if any) is written into the telnet transmit buffer.
 *
 * @param option char Option for the WILL command.
 * @retval none
 */
void TelnetProcessWill(char option) {
	unsigned long ulIdx;
#ifdef TELNET_CHAR_DEBUG
	LOGD(TAG, "[Telnet Server] Processing WILL command with option: %c/0x%02X\n\r", option, option);
#endif
	/* Loop through the known options. */
	for (ulIdx = 0; ulIdx < (sizeof(TelnetOptions) / sizeof(TelnetOptions[0]));
			ulIdx++) {
		/* See if this option matches the option in question. */
		if (TelnetOptions[ulIdx].option == option) {
			/* See if the WILL flag for this option has already been set. */
			if (((TelnetOptions[ulIdx].flags >> OPT_FLAG_WILL) & 0x01) == 0) {
				/* Set the WILL flag for this option. */
				TelnetOptions[ulIdx].flags = (TelnetOptions[ulIdx].flags & 0xFD)
						| (0x01 << OPT_FLAG_WILL);
				/* Send a DO response to this option. */
				telnet_server.buffer[telnet_server.length++] = TELNET_IAC;
				telnet_server.buffer[telnet_server.length++] = TELNET_DO;
				telnet_server.buffer[telnet_server.length++] = option;
			}
			/* Return without any further processing. */
			return;
		}
	}

	/* This option is not recognized, so send a DONT response. */
	telnet_server.buffer[telnet_server.length++] = TELNET_IAC;
	telnet_server.buffer[telnet_server.length++] = TELNET_DONT;
	telnet_server.buffer[telnet_server.length++] = option;
}

/**
 * This function will handle a WONT request for a telnet option.  If it is an
 * option that is known by the telnet server, a DONT response will be generated
 * if the option is not already disabled.  For unknown options, a DONT response
 * will always be generated.
 *
 * The response (if any) is written into the telnet transmit buffer.
 *
 * @param server Pointer to the server to process.
 * @param option char Option for the WONT command.
 * @retval none
 */
void TelnetProcessWont(char option) {
	unsigned long ulIdx;
#ifdef TELNET_CHAR_DEBUG
	LOGD(TAG, "[Telnet Server] Processing WONT command with option: %c/0x%02X\n\r", option, option);
#endif
	/* Loop through the known options. */
	for (ulIdx = 0; ulIdx < (sizeof(TelnetOptions) / sizeof(TelnetOptions[0]));
			ulIdx++) {
		/* See if this option matches the option in question. */
		if (TelnetOptions[ulIdx].option == option) {
			/* See if the WILL flag for this option is currently set. */
			if (((TelnetOptions[ulIdx].flags >> OPT_FLAG_WILL) & 0x01) == 1) {
				/* Clear the WILL flag for this option. */
				TelnetOptions[ulIdx].flags = (TelnetOptions[ulIdx].flags & 0xFD)
						| 0x00;
				/* Send a DONT response to this option. */
				telnet_server.buffer[telnet_server.length++] = TELNET_IAC;
				telnet_server.buffer[telnet_server.length++] = TELNET_DONT;
				telnet_server.buffer[telnet_server.length++] = option;
			}
			/* Return without any further processing. */
			return;
		}
	}

	/* This option is not recognized, so send a DONT response. */
	telnet_server.buffer[telnet_server.length++] = TELNET_IAC;
	telnet_server.buffer[telnet_server.length++] = TELNET_DONT;
	telnet_server.buffer[telnet_server.length++] = option;
}

/**
 * This function will handle a DO request for a telnet option.  If it is an
 * option that is known by the telnet server, a WILL response will be generated
 * if the option is not already enabled.  For unknown options, a WONT response
 * will always be generated.
 *
 * The response (if any) is written into the telnet transmit buffer.
 *
 * @param option char Option for the DO command.
 * @return none
 */
void TelnetProcessDo(char option) {
	unsigned long ulIdx;
#ifdef TELNET_CHAR_DEBUG
	LOGD(TAG, "[Telnet Server] Processing DO command with option: %c/0x%02X\n\r", option, option);
#endif
	/* Loop through the known options. */
	for (ulIdx = 0; ulIdx < (sizeof(TelnetOptions) / sizeof(TelnetOptions[0]));
			ulIdx++) {
		/* See if this option matches the option in question. */
		if (TelnetOptions[ulIdx].option == option) {
			/* See if the DO flag for this option has already been set. */
			if (((TelnetOptions[ulIdx].flags >> OPT_FLAG_DO) & 0x01) == 0) {
				/* Set the DO flag for this option. */
				TelnetOptions[ulIdx].flags = (TelnetOptions[ulIdx].flags & 0xFB)
						| (0x01 << OPT_FLAG_DO);
				/* Send a WILL response to this option. */
				telnet_server.buffer[telnet_server.length++] = TELNET_IAC;
				telnet_server.buffer[telnet_server.length++] = TELNET_WILL;
				telnet_server.buffer[telnet_server.length++] = option;
			}
			/* Return without any further processing. */
			return;
		}
	}

	// This option is not recognized, so send a WONT response.
	telnet_server.buffer[telnet_server.length++] = TELNET_IAC;
	telnet_server.buffer[telnet_server.length++] = TELNET_WONT;
	telnet_server.buffer[telnet_server.length++] = option;
}

/**
 * This funciton will handle a DONT request for a telnet option.  If it is an
 * option that is known by the telnet server, a WONT response will be generated
 * if the option is not already disabled.  For unknown options, a WONT resopnse
 * will always be generated.
 *
 * The response (if any) is written into the telnet transmit buffer.
 *
 * @param option char Option for the DONT command.
 * @return none
 */
void TelnetProcessDont(char option) {
	unsigned long ulIdx;
#ifdef TELNET_CHAR_DEBUG
	LOGD(TAG, "[Telnet Server] Processing DONT command with option: %c/0x%02X\n\r", option, option);
#endif
	/* Loop through the known options. */
	for (ulIdx = 0; ulIdx < (sizeof(TelnetOptions) / sizeof(TelnetOptions[0]));
			ulIdx++) {
		/* See if this option matches the option in question. */
		if (TelnetOptions[ulIdx].option == option) {
			/* See if the DO flag for this option is currently set. */
			if (((TelnetOptions[ulIdx].flags >> OPT_FLAG_DO) & 0x01) == 1) {
				/* Clear the DO flag for this option. */
				TelnetOptions[ulIdx].flags = (TelnetOptions[ulIdx].flags & 0xFB)
						| 0x00;
				/* Send a WONT response to this option. */
				telnet_server.buffer[telnet_server.length++] = TELNET_IAC;
				telnet_server.buffer[telnet_server.length++] = TELNET_WONT;
				telnet_server.buffer[telnet_server.length++] = option;
			}
			/* Return without any further processing. */
			return;
		}
	}

	/* This option is not recognized, so send a WONT response. */
	telnet_server.buffer[telnet_server.length++] = TELNET_IAC;
	telnet_server.buffer[telnet_server.length++] = TELNET_WONT;
	telnet_server.buffer[telnet_server.length++] = option;
}

/*
 * This function processes a character received from the telnet port, handling
 * the interpretation of telnet commands (as indicated by the telnet interpret
 * as command (IAC) byte).
 *
 * @param character char The character to process.
 * @retval none
 */
void TelnetProcessCharacter(char character) {
#ifdef TELNET_CHAR_DEBUG
	LOGD(TAG, "[Telnet Server] Processing Character: %c/0x%02X\n\r", character, character);
#endif
	/* Determine the current state of the telnet command parser. */
	switch (telnet_server.state) {
	/* The normal state of the parser, were each character is either sent
	 to the UART or is a telnet IAC character. */
	case STATE_NORMAL: {
		/* See if this character is the IAC character. */
		if (character == TELNET_IAC) {
			/* Skip this character and go to the IAC state. */
			telnet_server.state = STATE_IAC;
		} else {
			/* Write this character to the receive buffer. */
			TelnetRecvBufferWrite(character);
			/* Echo this character */
			TelnetWrite(character);                    //Echo the character back
		}
		break;
	}
		/* The previous character was the IAC character. */
	case STATE_IAC: {
		/* Determine how to interpret this character. */
		switch (character) {
		/* See if this character is also an IAC character. */
		case TELNET_IAC: {
			/* Write 0xff to the receive buffer. */
			TelnetRecvBufferWrite(0xff);
			/* Switch back to normal mode. */
			telnet_server.state = STATE_NORMAL;
			/* This character has been handled. */
			break;
		}
			/* See if this character is the WILL request. */
		case TELNET_WILL: {
			/* Switch to the WILL mode; the next character will have
			 the option in question. */
			telnet_server.state = STATE_WILL;
			/* This character has been handled. */
			break;
		}
			/* See if this character is the WONT request. */
		case TELNET_WONT: {
			/* Switch to the WONT mode; the next character will have
			 the option in question. */
			telnet_server.state = STATE_WONT;
			/* This character has been handled. */
			break;
		}
			/* See if this character is the DO request. */
		case TELNET_DO: {
			/* Switch to the DO mode; the next character will have the
			 option in question. */
			telnet_server.state = STATE_DO;
			/* This character has been handled. */
			break;
		}
			/* See if this character is the DONT request. */
		case TELNET_DONT: {
			/* Switch to the DONT mode; the next character will have
			 the option in question. */
			telnet_server.state = STATE_DONT;
			/* This character has been handled. */
			break;
		}
			/* See if this character is the AYT request. */
		case TELNET_AYT: {
			/* Send a short string back to the client so that it knows
			 that the server is still alive. */
			telnet_server.buffer[telnet_server.length++] = '\r';
			telnet_server.buffer[telnet_server.length++] = '\n';
			telnet_server.buffer[telnet_server.length++] = '[';
			telnet_server.buffer[telnet_server.length++] = 'Y';
			telnet_server.buffer[telnet_server.length++] = 'e';
			telnet_server.buffer[telnet_server.length++] = 's';
			telnet_server.buffer[telnet_server.length++] = ']';
			telnet_server.buffer[telnet_server.length++] = '\r';
			telnet_server.buffer[telnet_server.length++] = '\n';
			/* Switch back to normal mode. */
			telnet_server.state = STATE_NORMAL;
			/* This character has been handled. */
			break;
		}
			/* Explicitly ignore the GA and NOP request, plus provide a
			 catch-all ignore for unrecognized requests. */
		case TELNET_GA:
		case TELNET_NOP:
		default: {
			/* Switch back to normal mode. */
			telnet_server.state = STATE_NORMAL;
			/* This character has been handled. */
			break;
		}
		}
		/* This state has been handled. */
		break;
	}
		/* The previous character sequence was IAC WILL. */
	case STATE_WILL: {
		/* Process the WILL request on this option. */
		TelnetProcessWill(character);
		/* Switch back to normal mode. */
		telnet_server.state = STATE_NORMAL;
		/* This state has been handled. */
		break;
	}
		/* The previous character sequence was IAC WONT. */
	case STATE_WONT: {
		/* Process the WONT request on this option. */
		TelnetProcessWont(character);
		/* Switch back to normal mode. */
		telnet_server.state = STATE_NORMAL;
		/* This state has been handled. */
		break;
	}
		/* The previous character sequence was IAC DO. */
	case STATE_DO: {
		/* Process the DO request on this option. */
		TelnetProcessDo(character);
		/* Switch back to normal mode. */
		telnet_server.state = STATE_NORMAL;
		/* This state has been handled. */
		break;
	}
		/* The previous character sequence was IAC DONT. */
	case STATE_DONT: {
		/* Process the DONT request on this option. */
		TelnetProcessDont(character);
		/* Switch back to normal mode. */
		telnet_server.state = STATE_NORMAL;
		/* This state has been handled. */
		break;
	}
		/* A catch-all for unknown states.  This should never be reached, but
		 is provided just in case it is ever needed. */
	default: {
		/* Switch back to normal mode. */
		telnet_server.state = STATE_NORMAL;
		/* This state has been handled. */
		break;
	}
	}
}

/**
 * Print a message to the telnet connection formatted as an error.
 *
 * @param message char* Pointer to the string to send
 * @retval none
 */
void TelnetWriteErrorMessage(char* message) {
	if (TelnetIsConnected() == true) {
		ClearToMessageBuffer();
		char* character = message;
		while (*character) {
			character++;
		}
		uint16_t n = snprintf(MESSAGE_BUFFER, sizeof(MESSAGE_BUFFER),
		ERROR_MESSAGE_HEADER, message);
		if (n > 0) {
			TelnetWriteString(MESSAGE_BUFFER);
		}
	}
}

/**
 * Print a message to the telnet connection formatted as a status.
 *
 * @param message char* Pointer to the string to send
 * @retval none
 */
void TelnetWriteStatusMessage(char* message) {
	if (TelnetIsConnected() == true) {
		ClearToMessageBuffer();
		uint8_t count = 0;
		char* character = message;
		while (*character) {
			++character;
			++count;
		}
		uint16_t n = snprintf(MESSAGE_BUFFER, sizeof(MESSAGE_BUFFER),
		STATUS_MESSAGE_HEADER, message);
		if (n > 0) {
			TelnetWriteString(MESSAGE_BUFFER);
		}
	}
}

/**
 * Print a message to the telnet connection formatted as a debug.
 *
 * @param message char* Pointer to the string to send
 * @retval none
 */
void TelnetWriteDebugMessage(char* message) {
	if (TelnetIsConnected() == true) {
		ClearToMessageBuffer();
		uint8_t count = 0;
		char* character = message;
		while (*character) {
			++character;
			++count;
		}
		uint16_t n = snprintf(MESSAGE_BUFFER, sizeof(MESSAGE_BUFFER),
		DEBUG_MESSAGE_HEADER, message);
		if (n > 0) {
			TelnetWriteString(MESSAGE_BUFFER);
		}
	}
}

/**
 * Print a message to the telnet connection formatted as a command data.
 *
 * @param message char* Pointer to the string to send
 * @retval none
 */
void TelnetWriteCommandDataMessage(char* message) {
	if (TelnetIsConnected() == true) {
		ClearToMessageBuffer();
		uint8_t count = 0;
		char* character = message;
		while (*character) {
			++character;
			++count;
		}
		uint16_t n = snprintf(MESSAGE_BUFFER, sizeof(MESSAGE_BUFFER),
		COMMAND_DATA_MESSAGE_HEADER, message);
		if (n > 0) {
			TelnetWriteString(MESSAGE_BUFFER);
		}
	}
}

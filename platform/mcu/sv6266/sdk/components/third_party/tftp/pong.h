#ifndef PONG_H
#define PONG_H

#include <signal.h>

/* Network communications are nothing but a game of pong.
 * I send a packet, you respond. I send another, miss your resposne,
 * you get a point while I get all angry, timeout and send another.
 *   --Ian
 */

int createUDPSocketAndBind(int port);

bool send_generic(int sockfd, struct sockaddr* sockInfo, PACKET* packet);

bool send_RRQ(int sockfd, struct sockaddr* sockInfo, char* filename, char* mode);
bool send_WRQ(int sockfd, struct sockaddr* sockInfo, char* filename, char* mode);

bool send_data(int sockfd, struct sockaddr* sockInfo, uint16_t blockNumber, char* data, size_t data_size);

bool send_ack(int sockfd, struct sockaddr* sockInfo, uint16_t blockNumber);

bool send_error(int sockfd, struct sockaddr* sockInfo, uint16_t errorCode, char* error_message);

int waitForPacket(int sockfd, struct sockaddr* cli_addr, uint16_t optcode, PACKET* packet);

#endif

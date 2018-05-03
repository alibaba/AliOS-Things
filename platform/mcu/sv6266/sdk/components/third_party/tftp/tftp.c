#include "tftp.h"

int sockfd;
struct sockaddr_in serv_addr;
unsigned int timeout_counter = 0;
uint16_t blockNumber = 1;

int tftp_recv_req(char *servername, uint16_t server_port, char *filename)
{
    sockfd = createUDPSocketAndBind(0);

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    serv_addr.sin_addr.s_addr = inet_addr(servername);
    serv_addr.sin_port = htons(server_port);

    printf("sockfd : %d\n", sockfd);

    if(sockfd < 0)
    {
        printf("Couldn't open socket\n");
        return -1;
    }

    if(!send_RRQ(sockfd, (struct sockaddr *) &serv_addr, filename, TFTP_SUPORTED_MODE))
    {
        printf("Error: couldn't send RRQ\n");
        closesocket(sockfd);
        return -1;
    }

    timeout_counter = 0;
    blockNumber = 1;

    return 0;
}

int tftp_recv(PACKET *packet, char *filename)
{
    int result;
    uint8_t re_ack = 0;
    uint8_t re_try = 0;

    do{
        result = waitForPacket(sockfd, (struct sockaddr *) &serv_addr, TFTP_OPTCODE_DATA, packet);
        if(result == -1)     //timeout
        {
            if(timeout_counter < MAX_TFTP_TIMEOUTS)
            {
                //send gerneric
                if (filename != NULL && blockNumber == 1) //resent the inital RRQ if we timeout on the first packet
                {
                    send_RRQ(sockfd, (struct sockaddr *) &serv_addr, filename, TFTP_SUPORTED_MODE);
                }
                timeout_counter++;
                re_try = 1;
            }
            else
            {
                send_error(sockfd, (struct sockaddr *) &serv_addr, TFTP_ERRCODE_ACCESS_VIOLATION, "Too many timeouts\n");
                closesocket(sockfd);
                return -1;
            }
        }
        else if(packet->optcode == TFTP_OPTCODE_ERR)
        {
            //error handler
            printError(packet);
            closesocket(sockfd);
            return -1;
        }
        else //correct packet type
        {
            //check for correct blocknumber
            if(packet->data.blockNumber == blockNumber)
            {
#if 0      
                if(fwrite(packet.data.data, 1, packet.data.dataSize, fileh))
                {
                  blockNumber++;
                }
                //this check should go first
                else if(packet.data.dataSize != 0)
                {
                  send_error(sockfd, cli_addr, TFTP_ERRCODE_UNDEFINED, "Got no data to write to file");
                  return false;
                }
#else
                printf("packet.data.dataSize = %d, blockNumber = %d \n", packet->data.dataSize, blockNumber);
                blockNumber++;        
#endif
                if (DEBUG) printf("Sending Ack: [%u] ",packet->data.blockNumber);
                //this should be mvoed ot of the else to be a genreric send
                if (!send_ack(sockfd, (struct sockaddr *) &serv_addr, packet->data.blockNumber))
                {
                    closesocket(sockfd);
                    return -1;
                }
                re_ack = 0;
                re_try = 0;
                if (DEBUG) printf("Sent\n");
            }
            else
            {
                if (DEBUG) printf("Error: expected block [%u] but got [%u]\n",blockNumber,packet->data.blockNumber);
                //skip over packets left in queue
                if (blockNumber < packet->data.blockNumber)
                {
                    send_error(sockfd, (struct sockaddr *) &serv_addr, TFTP_ERRCODE_ILLEGAL_OPERATION, "Wrong block number recieved");
                    closesocket(sockfd);
                    return -1;
                }
                else
                {
                    if (DEBUG) printf("Re-Sending Ack: [%u] ",packet->data.blockNumber);
                    //this should be mvoed ot of the else to be a genreric send
                    if (!send_ack(sockfd, (struct sockaddr *) &serv_addr, packet->data.blockNumber))
                    {
                        closesocket(sockfd);
                        return -1;
                    }
                    re_ack = 1;
                }
            }
        }
    } while(re_ack || re_try);

    if(packet->data.dataSize == MAX_DATA_SIZE || (filename != NULL && blockNumber == 1)) {
        return 1;
    }
    else {
        closesocket(sockfd);
        return 0;
    }
}


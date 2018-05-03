#include "tftp.h"

//returns the handle to a socket on the given port
//if the port is 0 then the OS will pick an avaible port
int createUDPSocketAndBind(int port)
{
  int sockfd;
  struct sockaddr_in serv_addr;
  
  //create a socket
  sockfd = socket(PF_INET, SOCK_DGRAM, 0);

  //return -1 on error
  if (sockfd == -1)
  {
    return -1;
  }
  
  //zero out the struct
  bzero((char*) &serv_addr, sizeof(serv_addr));

  //create the struct to bind on
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(port);

  //bind to it
  if (bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
  {
    //error
    return -1;
  }

  return sockfd;
}


bool send_generic(int sockfd, struct sockaddr* sockInfo, PACKET* packet)
{
  size_t n;
  char buffer[BUFSIZE];
  n = serializePacket(packet,buffer);

  return (sendto(sockfd,buffer,n,0,(struct sockaddr *)sockInfo,sizeof(struct sockaddr)) >= 0);
}


bool send_RRQ(int sockfd, struct sockaddr* sockInfo, char* filename, char* mode)
{
  PACKET packet;

  packet.optcode = TFTP_OPTCODE_RRQ;
  charncpy(packet.read_request.filename,filename,MAX_STRING_SIZE);
  charncpy(packet.read_request.mode,mode,MAX_MODE_SIZE);

  return send_generic(sockfd,sockInfo,&packet);
}

bool send_WRQ(int sockfd, struct sockaddr* sockInfo, char* filename, char* mode)
{
  PACKET packet;

  packet.optcode = TFTP_OPTCODE_WRQ;
  charncpy(packet.read_request.filename,filename,MAX_STRING_SIZE);
  charncpy(packet.read_request.mode,mode,MAX_MODE_SIZE);

  return send_generic(sockfd,sockInfo,&packet);
}

bool send_data(int sockfd, struct sockaddr* sockInfo, uint16_t blockNumber, char* data, size_t data_size)
{
  PACKET packet;

  packet.optcode = TFTP_OPTCODE_DATA;
  packet.data.blockNumber = blockNumber;
  packet.data.dataSize = data_size;
  memcpy(packet.data.data,data,data_size);

  return send_generic(sockfd,sockInfo,&packet);
}

//sends the arror message to the socket with the provided errorcode and message
bool send_error(int sockfd, struct sockaddr* sockInfo, uint16_t errorCode, char* error_message)
{
  PACKET packet;

  packet.optcode = TFTP_OPTCODE_ERR;
  packet.error.errorCode = errorCode;
  strncpy(packet.error.message, error_message,MAX_STRING_SIZE);

  return send_generic(sockfd,sockInfo,&packet);
}

//send an ack reply back to somewhere
bool send_ack(int sockfd, struct sockaddr* sockInfo, uint16_t blockNumber)
{
  PACKET packet;

  packet.optcode = TFTP_OPTCODE_ACK;
  packet.ack.blockNumber = blockNumber;

  return send_generic(sockfd,sockInfo,&packet);
}

//3 possible outcomes
//timeout or other error: return -1
//got desired packet: size of packet returned
//error recieved: 0
int waitForPacket(int sockfd, struct sockaddr* cli_addr, uint16_t optcode, PACKET* packet)
{
  int rc; 
  fd_set readSet; 
  struct timeval timeout; 

  char buffer[BUFSIZE];
  size_t n;
  size_t cli_size = sizeof(cli_addr);

  errno = 0;
  while (true)
  {
    if (DEBUG) printf("Waiting for response..");
    FD_ZERO(&readSet); 
    FD_SET(sockfd, &readSet); 
    timeout.tv_sec  = TFTP_TIMEOUT_DURATION; 
    timeout.tv_usec = 0; // quarter second, 250 ms 

    rc = select(sockfd + 1, &readSet, NULL, NULL, &timeout); 

    if (rc == 0) {
      if (DEBUG) printf("Timeout detected\n");
      return -1; 
    }
    else if (rc < 0) {
      if (DEBUG) printf("Errno with value[%d]\n",errno);
    }
    
    n = recvfrom(sockfd, buffer, BUFSIZE, 0, cli_addr, (socklen_t *)&cli_size);
    if (DEBUG) printf("done\n");
    if (errno != 0)
    {
      if (errno == EINTR)
      {
        //we got a timeout
        if (DEBUG) printf("Timeout detected\n");

      }else{
        //some other error
        if (DEBUG) printf("Errno with value[%d]\n",errno);
      }

      return -1;
    }
    if (unserializePacket(buffer, n, packet) == NULL)
    {
      return -1;
    }
    if (packet->optcode == optcode)
    {
      return n;
    }
    else if(packet->optcode == TFTP_OPTCODE_ERR)
    {
      return 0;
    }
  }
  //should never reach this
  return -1;
}

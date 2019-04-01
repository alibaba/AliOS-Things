#include "tftp.h"

//used for counting the number of children procs
static unsigned int childCount = 0;

//called when the server needs to send a file to a client
bool server_send(int sockfd, struct sockaddr* cli_addr, PACKET* packet)
{
  FILE* fileh;
  
  //we only support octet mode
  if (strncmp(packet->read_request.mode,TFTP_SUPORTED_MODE,MAX_MODE_SIZE) != 0)
  {
    //unsuported mode
    send_error(sockfd,cli_addr,TFTP_ERRCODE_ILLEGAL_OPERATION,"Only octet mode is supported");
    return false;
  }

  if (strchr(packet->read_request.filename,'/') != NULL )
  {
    send_error(sockfd,cli_addr,TFTP_ERRCODE_ACCESS_VIOLATION,"Folders not permitted");
    return false;
  }

  fileh = fopen(packet->read_request.filename,"rb");

  //check for file errors
  if (fileh == NULL)
  {
    if (errno == ENOENT)
    {
      //file 404
      send_error(sockfd,cli_addr,TFTP_ERRCODE_FILE_NOT_FOUND,strerror(errno));
    }
    //other cases
    send_error(sockfd,cli_addr,TFTP_ERRCODE_ACCESS_VIOLATION,strerror(errno));
    return false;
  }

  return sendFile(sockfd,cli_addr,fileh);
}

//called when the server recieves a WRQ
bool server_recieve(int sockfd, struct sockaddr* cli_addr, PACKET* packet)
{
    FILE* fileh;

    //we only support octet mode
    if (strncmp(packet->write_request.mode,TFTP_SUPORTED_MODE,MAX_MODE_SIZE) != 0)
    {
      //unsuported mode
      send_error(sockfd,cli_addr,TFTP_ERRCODE_ILLEGAL_OPERATION,"Only octet mode is supported");
      return false;
    }

    if (strchr(packet->write_request.filename,'/') != NULL )
    {
      send_error(sockfd,cli_addr,TFTP_ERRCODE_ACCESS_VIOLATION,"Folders not permitted");
      return false;
    }

    //open file handle
    fileh = fopen(packet->write_request.filename,"wb");

    //check for file errors
    if (fileh == NULL)
    {
      if (errno == ENOMEM)
      {
        //disk full
        send_error(sockfd,cli_addr,TFTP_ERRCODE_DISK_FULL,strerror(errno));
      }else if (errno == EEXIST)
      {
        //file exists
        send_error(sockfd,cli_addr,TFTP_ERRCODE_FILE_ALREADY_EXISTS,strerror(errno));
      }
      //other cases
      send_error(sockfd,cli_addr,TFTP_ERRCODE_ACCESS_VIOLATION,strerror(errno));
      return false;
    }

    //send ack
    send_ack(sockfd,cli_addr,0);
    
    return recvFile(sockfd,cli_addr,fileh,NULL);
}

//this method is in charge of each new client request
void run_child(struct sockaddr cli_addr, PACKET * packet)
{
  if (packet == NULL)
  {
    return;
  }

  int child_sockfd;
  bool result;

  //create the child socket which will be on a new port
  child_sockfd = createUDPSocketAndBind(0);

  switch (packet->optcode)
  {
    case TFTP_OPTCODE_RRQ:
      printf("Prossessng client READ request for %s\n",packet->read_request.filename);
      result = server_send(child_sockfd,&cli_addr,packet);
      break;
    case TFTP_OPTCODE_WRQ:
      printf("Prossessng client WRITE request for %s\n",packet->read_request.filename);
      result = server_recieve(child_sockfd,&cli_addr,packet);
      break;
    default:
      result = send_error(child_sockfd, &cli_addr, TFTP_ERRCODE_ILLEGAL_OPERATION,"Unexpected Packet");
      break;
  }

  if (result == false)
  {
    printf("Failed to handle client request\n");
  }
  printf("Done processing client request\n");
}


//main loop which waits for all packets and acts on them
void packet_recieve_loop(int sockfd)
{
  struct sockaddr cli_addr;
  size_t cli_size = sizeof(cli_addr);
  int recv_len;
  char buffer[BUFSIZE];
  pid_t fork_id;
  PACKET packet;

  printf("Server started\n");

  //main loop
  while (true)
  {
    //clean the buffer
    memset(buffer,0,BUFSIZE);

    //recieve the data
    recv_len = recvfrom(sockfd, buffer, BUFSIZE, 0, &cli_addr, (socklen_t *)&cli_size);

    //check for errors
    if (recv_len <= 0)
    {
      printf("recvfrom error\n");
      perror("Socket read:");
      if (recv_len == 0)
      {
        printf("Socket closed\n");
      }
      exit(1);
    }
    
    if (childCount < MAX_TFTP_CLIENTS)
    {
      unserializePacket(buffer,recv_len,&packet);
      fork_id = fork();
    }else{
      printf("Error: Server too busy to accept new clients\n");
      send_error(sockfd, &cli_addr, TFTP_ERRCODE_UNDEFINED,"Too many clients");
      continue;
    }

    if (fork_id < 0)
    {
      printf("Error: could not fork child process\n");
      send_error(sockfd, &cli_addr, TFTP_ERRCODE_UNDEFINED,"Unable to handle client request");
      exit(1);
    }else if (fork_id == 0)
    {
      //close the parents Socket, we will make our own
      close(sockfd);
      childCount++;

      //if (DEBUG) printPacket(&packet);

      //child code here, deal with the packet
      run_child(cli_addr,&packet);

      //end of child execution
      childCount--;
      exit(0); //end the child
    }else
    {
      //main server code
      if (DEBUG) printf("spawning fork: %u to handle new request\n",fork_id);
    }

  }
  close(sockfd);
}

//main server function, checks for possible args and starts the server
int main(int argc, char *argv[])
{
  int port = SERV_UDP_PORT;
  if (argc > 2 && argv[1][0] == '-' && argv[1][1] == 'p')
  {
    //change the default port
    port = atoi(argv[2]);
  }else if (argc != 1)
  {
    //print usage
    printf("Usage: %s [-p port]\n",argv[0]);
    return 0;
  }

  int sockfd;

  //bind to our socket
  printf("Using Port %d\n",port);
  sockfd = createUDPSocketAndBind(port);

  //run the main loop
  packet_recieve_loop(sockfd);

  return 0;
}


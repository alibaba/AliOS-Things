
bool sendFile(int sockfd,struct sockaddr* cli_addr, FILE* fileh);

bool recvFile(int sockfd, struct sockaddr* cli_addr,FILE* fileh, char* filename);

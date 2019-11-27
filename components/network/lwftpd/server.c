#include "common.h"
#include <aos/kernel.h>
#include <ulog/ulog.h>
#include <network/network.h>

#define FTP_CLIENT_MAX 3

/* Welcome message */
static char *welcome_message = "A very warm welcome!";
static void client_conn_handler(void *arg);

/* String mappings for cmdlist */
static const char *cmdlist_str[] = 
{
  "ABOR", "CWD", "DELE", "LIST", "MDTM", "MKD", "NLST", "PASS", "PASV",
  "PORT", "PWD", "QUIT", "RETR", "RMD", "RNFR", "RNTO", "SITE", "SIZE",
  "STOR", "TYPE", "USER", "NOOP" 
};

/** 
 * Sets up server and handles incoming connections
 * @param port Server port
 */
void aos_ftp_server(int port)
{
  int conn;
  int sock = create_socket(port);
  struct sockaddr_in client_address;
  socklen_t len = sizeof(client_address);
  //char addr_str[128];

  if (sock < 0) {
    LOGE(TAG, "Failed to create FTP server.");
    return;
  }

  LOG("FTP server started on port %d", port);

  while(1) {
    conn = accept(sock, (struct sockaddr*)&client_address, &len);
    //inet_ntoa_r(((struct sockaddr_in *)&client_address)->sin_addr.s_addr,
    //            addr_str, sizeof(addr_str) - 1);
    LOG("Connection from client 0x%08x (fd: %d)", client_address.sin_addr.s_addr, conn);
    aos_task_new("ftp", client_conn_handler, (void *)conn, 16384);
  }

  LOG("FTP server to exit");
  close(sock);
}

static void client_conn_handler(void *arg)
{
  Command *cmd;
  State *state;
  char buffer[BSIZE], welcome[BSIZE] = "220";
  int fd, bytes_read;

  if (!arg) return;
  else fd = (int)arg;

  cmd = malloc(sizeof(Command));
  state = malloc(sizeof(State));

  if (strlen(welcome_message) < (BSIZE - 4)) {
    strcat(welcome, welcome_message);
  }else{
    strcat(welcome, "Welcome to nice FTP service.");
  }

  /* Write welcome message */
  strcat(welcome, "\n");
  write(fd, welcome, strlen(welcome));

  /* Read commands from client */
  while ((bytes_read = read(fd, buffer, BSIZE)) != 0) {
    if (!(bytes_read > BSIZE)) {
      /* TODO: output this to log */
      buffer[BSIZE-1] = '\0';
      LOG("User %s sent command: %s", (state->username==0) ?
          "unknown" : state->username, buffer);
      parse_command(buffer, cmd);
      state->connection = fd;
      
      /* Ignore non-ascii char. Ignores telnet command */
      if(buffer[0] <= 127 || buffer[0] >= 0) {
        response(cmd, state);
      }

      memset(buffer, 0, BSIZE);
      memset(cmd, 0, sizeof(Command));
    }else{
      /* Read error */
      LOGE(TAG, "server read error");
    }
  }

  LOG("Client (fd: %d) disconnected.", fd);
}

/**
 * Creates socket on specified port and starts listening to this socket
 * @param port Listen on this port
 * @return int File descriptor for new socket
 */
int create_socket(int port)
{
  int sock;
  int reuse = 1;

  /* Server addess */
  struct sockaddr_in server_address;

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    LOGE(TAG, "Cannot open socket");
    return -1;
  }

  /* Address can be reused instantly after program exits */
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof reuse);

  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);

  /* Bind socket to server address */
  if (bind(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
    LOGE(TAG, "Cannot bind socket to address (errno: %d)", errno);
    return -2;
  }

  listen(sock, FTP_CLIENT_MAX);

  return sock;
}

/**
 * Accept connection from client
 * @param socket Server listens this
 * @return int File descriptor to accepted connection
 */
int accept_connection(int socket)
{
  socklen_t addrlen = 0;
  struct sockaddr_in client_address;
  addrlen = sizeof(client_address);
  return accept(socket, (struct sockaddr*)&client_address, &addrlen);
}

/**
 * Get ip where client connected to
 * @param sock Commander socket connection
 * @param ip Result ip array (length must be 4 or greater)
 * result IP array e.g. {127,0,0,1}
 */
void getip(int sock, int *ip)
{
  socklen_t addr_size = sizeof(struct sockaddr_in);
  struct sockaddr_in addr;
  getsockname(sock, (struct sockaddr *)&addr, &addr_size);
 
  char* host = inet_ntoa(addr.sin_addr);
  sscanf(host,"%d.%d.%d.%d",&ip[0],&ip[1],&ip[2],&ip[3]);
}

/**
 * Lookup enum value of string
 * @param cmd Command string 
 * @return Enum index if command found otherwise -1
 */

int lookup_cmd(char *cmd)
{
  const int cmdlist_count = sizeof(cmdlist_str)/sizeof(char *);
  return lookup(cmd, cmdlist_str, cmdlist_count);
}

/**
 * General lookup for string arrays
 * It is suitable for smaller arrays, for bigger ones trie is better
 * data structure for instance.
 * @param needle String to lookup
 * @param haystack Strign array
 * @param count Size of haystack
 */
int lookup(char *needle, const char **haystack, int count)
{
  int i;

  for (i=0; i<count; i++) {
    if(strcmp(needle,haystack[i])==0)return i;
  }

  return -1;
}


/** 
 * Writes current state to client
 */
void write_state(State *state)
{
  write(state->connection, state->message, strlen(state->message));
}

/**
 * Generate random port for passive mode
 * @param state Client state
 */
void gen_port(Port *port)
{
  srand(time(NULL));
  port->p1 = 128 + (rand() % 64);
  port->p2 = rand() % 0xff;
}

/**
 * Parses FTP command string into struct
 * @param cmdstring Command string (from ftp client)
 * @param cmd Command struct
 */
void parse_command(char *cmdstring, Command *cmd)
{
  sscanf(cmdstring,"%s %s", cmd->command, cmd->arg);
}

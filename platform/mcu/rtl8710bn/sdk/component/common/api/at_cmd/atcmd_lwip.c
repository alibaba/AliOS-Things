#include <platform/platform_stdlib.h>
#include <platform_opts.h>

#include <stdio.h>
#include "log_service.h"
#include "atcmd_wifi.h"
#include "atcmd_lwip.h"
#include "osdep_service.h"

#if ATCMD_SUPPORT_SSL
#include "polarssl/config.h"
#include "polarssl/net.h"
#include "polarssl/ssl.h"
#include "polarssl/error.h"
#include "polarssl/memory.h"
#include "polarssl/net.h"
#include "polarssl/ssl.h"
#include "polarssl/memory.h"
#include "polarssl/certs.h"
#if !defined(POLARSSL_BIGNUM_C) || !defined(POLARSSL_CERTS_C) || \
	!defined(POLARSSL_SSL_TLS_C) || !defined(POLARSSL_SSL_SRV_C) || \
	!defined(POLARSSL_SSL_CLI_C) || !defined(POLARSSL_NET_C) || \
	!defined(POLARSSL_RSA_C) || !defined(POLARSSL_X509_CRT_PARSE_C)
#error ("some define missing, please check config_rsa.h")
#endif
#endif

//#define MAX_BUFFER 	256
#define MAX_BUFFER 	(LOG_SERVICE_BUFLEN)
#define ATCP_STACK_SIZE		512//2048
#define ATCP_SSL_STACK_SIZE		2048

extern char log_buf[LOG_SERVICE_BUFLEN];
extern struct netif xnetif[NET_IF_NUM]; 

static unsigned char _tx_buffer[MAX_BUFFER];
static unsigned char _rx_buffer[MAX_BUFFER];
static unsigned char *tx_buffer = _tx_buffer;
static unsigned char *rx_buffer = _rx_buffer;
int tx_buffer_size = MAX_BUFFER;
int rx_buffer_size = MAX_BUFFER;

#if ATCMD_VER == ATVER_2

#if (CONFIG_EXAMPLE_SPI_ATCMD == 1)
#define EXTEND_ATPR_SIZE (1) 
#endif

#if defined(EXTEND_ATPR_SIZE) && (EXTEND_ATPR_SIZE == 1)
#define FETCH_TIMEOUT         (3)
#define ATPR_RSVD_HEADER_SIZE (100)
#endif

node node_pool[NUM_NS];

node* mainlist;

static int atcmd_lwip_auto_recv = FALSE;
volatile int atcmd_lwip_tt_mode = FALSE; //transparent transmission mode
xTaskHandle atcmd_lwip_tt_task = NULL;
_sema atcmd_lwip_tt_sema = NULL;
volatile int atcmd_lwip_tt_datasize = 0;
volatile int atcmd_lwip_tt_lasttickcnt = 0;

#ifdef ERRNO
_WEAK int errno = 0; //LWIP errno
#endif

#if ATCMD_SUPPORT_SSL
#define ATCMD_SSL_DEBUG_LEVEL   0
static void atcmd_ssl_debug(void *ctx, int level, const char *str)
{
	if(level <= ATCMD_SSL_DEBUG_LEVEL) {
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"%s", str);
	}
}
static int atcmd_ssl_random(void *p_rng, unsigned char *output, size_t output_len)
{
	rtw_get_random_bytes(output, output_len);
	return 0;
}
static void *atcmd_ssl_malloc(size_t size){
    return (void *)rtw_malloc(size);
}
static void atcmd_ssl_free(void *p){
	rtw_mfree((u8 *)p, 0);
}
/*********************************
* certificate and key for ssl server
*********************************/
static char *atcmd_ssl_server_crt[NUM_NS] = {NULL}; //TODO:should be input by user
static char *atcmd_ssl_server_ca_list[NUM_NS] = {NULL}; //TODO:should be input by user
static x509_crt *atcmd_ssl_srv_crt[NUM_NS] = {NULL}; 
static char *atcmd_ssl_server_key[NUM_NS] = {NULL}; //TODO:should be input by user
static pk_context *atcmd_ssl_srv_key[NUM_NS] = {NULL};

/*********************************
* certificate and key for ssl client
*********************************/
static char *atcmd_ssl_client_ca_crt[NUM_NS] = {NULL}; //TODO:should be input by user
static x509_crt* atcmd_ssl_cli_ca_crt[NUM_NS] = {NULL};
static char *atcmd_ssl_client_crt[NUM_NS] = {NULL}; //TODO:should be input by user
static x509_crt* atcmd_ssl_cli_crt[NUM_NS] = {NULL};
static char *atcmd_ssl_client_key[NUM_NS] = {NULL}; //TODO:should be input by user
static pk_context* atcmd_ssl_clikey_rsa[NUM_NS] = {NULL};
#endif

static void atcmd_lwip_receive_task(void *param);
int atcmd_lwip_start_autorecv_task(void);
int atcmd_lwip_is_autorecv_mode(void);
void atcmd_lwip_set_autorecv_mode(int enable);
int atcmd_lwip_start_tt_task(void);
int atcmd_lwip_is_tt_mode(void);
void atcmd_lwip_set_tt_mode(int enable);
int atcmd_lwip_write_info_to_flash(struct atcmd_lwip_conn_info *cur_conn, int enable);
#else //#if ATCMD_VER == ATVER_2 

xTaskHandle server_task = NULL;
xTaskHandle client_task = NULL;

static int mode = 0;
static int local_port;
static int remote_port;
static char remote_addr[16];
static int packet_size;

static int sockfd, newsockfd;
static socklen_t client;
static struct sockaddr_in serv_addr, cli_addr;
static int opt = 1;

static int type; //TCP SERVER:1, TCP CLIENT: 2, UDP SERVER:3, UDP CLIENT: 4

static void init_transport_struct(void)
{
	mode = 0;
	local_port = 0;
	remote_port = 0;
	sockfd = -1;
	newsockfd = -1;
	packet_size = 0;
	rtw_memset(remote_addr, 0, sizeof(remote_addr));
	rtw_memset(&client, 0, sizeof(client));
	rtw_memset(&serv_addr, 0, sizeof(serv_addr));
	
}

void socket_close(void)
{
	close(sockfd);
	if(server_task != NULL)
	{
		vTaskDelete(server_task);
		server_task = NULL;
	}
	if(client_task != NULL)
	{
		vTaskDelete(client_task);
		client_task = NULL;
	}
	type = 0;
	init_transport_struct();
}
#endif //#if ATCMD_VER == ATVER_2 

static void server_start(void *param)
{
	int s_mode;
	int s_sockfd, s_newsockfd;
	socklen_t s_client;
	struct sockaddr_in s_serv_addr, s_cli_addr;
	int s_local_port;
	int error_no = 0;
	int s_opt = 1;
#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
	int ret;
	ssl_context *ssl;
	x509_crt *server_x509;
	pk_context *server_pk;
#endif
#if ATCMD_VER == ATVER_2 
	node* ServerNodeUsed = (node*)param;
	if(ServerNodeUsed){
		s_mode = ServerNodeUsed->protocol;
		s_local_port = ServerNodeUsed->port;
	}
//	else
//#endif
#else
	{
		s_mode = mode;
		s_local_port = local_port;
		s_opt = opt;
	}
#endif

	/***********************************************************
	* Create socket and set socket options, then bind socket to local port
	************************************************************/
#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
	if(s_mode == NODE_MODE_SSL){
		if((ret = net_bind(&s_sockfd, NULL, s_local_port)) != 0) {
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"ERROR: net_bind %d\n", ret);
			error_no = 15;
			goto err_exit;
		}
	}
	else
#endif
	{
		if(s_mode == NODE_MODE_UDP)
			s_sockfd = socket(AF_INET,SOCK_DGRAM,0);
		else if(s_mode == NODE_MODE_TCP)
			s_sockfd = socket(AF_INET, SOCK_STREAM, 0);
		else
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"Unknown connection type[%d]", s_mode);

		if (s_sockfd == INVALID_SOCKET_ID) {
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"ERROR opening socket");
			error_no = 5;
			goto err_exit;
		}

		if((setsockopt(s_sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&s_opt, sizeof(s_opt))) < 0){
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"ERROR on setting socket option");
			close(s_sockfd);
			error_no = 6;
			goto err_exit;
		}
		
		rtw_memset((char *)&s_serv_addr, 0, sizeof(s_serv_addr));
		s_serv_addr.sin_family = AF_INET;
		s_serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		s_serv_addr.sin_port = htons(s_local_port);

		if (bind(s_sockfd, (struct sockaddr *)&s_serv_addr,sizeof(s_serv_addr)) < 0) {
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"ERROR on binding");
			close(s_sockfd);
			error_no = 7;
			goto err_exit;
		}
	}

	/***********************************************************
	* Assign IP address and socket fd to the node used for this server
	************************************************************/
#if ATCMD_VER == ATVER_2 
	if(ServerNodeUsed != NULL) {
		uint8_t *ip = (uint8_t *)LwIP_GetIP(&xnetif[0]);
		ServerNodeUsed->sockfd = s_sockfd;
		ServerNodeUsed->addr = ntohl(*((u32_t *)ip));
	}
//	else
//#endif
#else
	{
		sockfd = s_sockfd;
		rtw_memcpy(&serv_addr, &s_serv_addr, sizeof(s_serv_addr));
	}
#endif

#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
	if(s_mode == NODE_MODE_SSL){
		/***********************************************************
		*  SSL 1. Prepare the certificate and key for ssl server
		************************************************************/
		char *srv_crt = NULL;
		char *ca_list = NULL;
		char *srv_key = NULL;
		atcmd_ssl_srv_crt[ServerNodeUsed->con_id] = (x509_crt *)rtw_zmalloc(sizeof(x509_crt));
		if(atcmd_ssl_srv_crt[ServerNodeUsed->con_id] == NULL){
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"malloc fail for ssl server crt");
			error_no = 16;
			goto err_exit;	
		}
		atcmd_ssl_srv_key[ServerNodeUsed->con_id] = (pk_context *)rtw_zmalloc(sizeof(pk_context));
		if(atcmd_ssl_srv_key[ServerNodeUsed->con_id] == NULL){
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"malloc fail for ssl server key");
			error_no = 17;
			goto err_exit;	
		}
		memory_set_own(atcmd_ssl_malloc, atcmd_ssl_free);
		server_x509 = atcmd_ssl_srv_crt[ServerNodeUsed->con_id];
		server_pk = atcmd_ssl_srv_key[ServerNodeUsed->con_id];
		x509_crt_init(server_x509);
		pk_init(server_pk);
		srv_crt = (atcmd_ssl_server_crt[ServerNodeUsed->con_id])?atcmd_ssl_server_crt[ServerNodeUsed->con_id]:(char *)test_srv_crt;
		if((ret = x509_crt_parse(server_x509, (const unsigned char *)srv_crt, strlen(srv_crt))) != 0) {
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"ERROR: x509_crt_parse server_x509/srv_crt %d\n", ret);
			error_no = 18;
			goto err_exit;
		}
		ca_list = (atcmd_ssl_server_ca_list[ServerNodeUsed->con_id])?atcmd_ssl_server_ca_list[ServerNodeUsed->con_id]:(char *)test_ca_list;
		if((ret = x509_crt_parse(server_x509, (const unsigned char *)ca_list, strlen(ca_list))) != 0) {
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"ERROR: x509_crt_parse server_x509/ca_list %d\n", ret);
			error_no = 19;
			goto err_exit;
		}
		srv_key = (atcmd_ssl_server_key[ServerNodeUsed->con_id])?atcmd_ssl_server_key[ServerNodeUsed->con_id]:(char *)test_srv_key;
		if((ret = pk_parse_key(server_pk, srv_key, strlen(srv_key), NULL, 0)) != 0) {
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"ERROR: pk_parse_key server_pk %d\n", ret);
			error_no = 20;
			goto err_exit;
		}
		/***********************************************************
		*  SSL 2. Hang node on mainlist for global management
		************************************************************/
		if(hang_node(ServerNodeUsed) < 0)
		{
			error_no = 21;
			goto err_exit;
		}else{
			#if CONFIG_LOG_SERVICE_LOCK
			log_service_lock();
			#endif
			at_printf("\r\n[ATPS] OK"
			"\r\n[ATPS] con_id=%d",
			ServerNodeUsed->con_id);
			at_printf(STR_END_OF_ATCMD_RET);
			#if CONFIG_LOG_SERVICE_LOCK
			log_service_unlock();
			#endif
		}
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"The SSL SERVER START OK!");
		/***********************************************************
		*  SSL 3. Waiting for ssl client to connect
		************************************************************/
		while(1){
			//not using net_accept() here because it can't get client port in net_accept()
			if((s_newsockfd = accept(s_sockfd,(struct sockaddr *) &s_cli_addr,&s_client)) < 0){
				AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
					"[ATPS] ERROR:ERROR on net_accept ret=%d",ret);
				error_no = 22;
				goto err_exit;
			}
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"An SSL/TLS client[%s:%d] is connecting", inet_ntoa(s_cli_addr.sin_addr),ntohs(s_cli_addr.sin_port));
			/***********************************************************
			*  SSL 4. Setup stuff for this ssl connection
			************************************************************/
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"Setting up the SSL/TLS structure..." );
			ssl = (ssl_context *)rtw_malloc(sizeof(ssl_context));
			if(ssl == NULL){
				AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
					"[ATPS] malloc fail for ssl client context!");
				error_no = 23;
				goto err_exit;			
			}
			if((ret = ssl_init(ssl)) != 0) {
				AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"ERROR: ssl_init %d\n", ret);
				rtw_free((void *)ssl);
				error_no = 24;
				goto err_exit;
			}
			ssl_set_endpoint(ssl, SSL_IS_SERVER);
			ssl_set_ca_chain( ssl, server_x509->next, NULL, NULL );
			ssl_set_authmode(ssl, SSL_VERIFY_NONE);
			ssl_set_rng(ssl, atcmd_ssl_random, NULL);
			ssl_set_bio(ssl, net_recv, &s_newsockfd, net_send, &s_newsockfd);
			if((ret = ssl_set_own_cert(ssl, server_x509, server_pk)) != 0) {
				AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"ERROR: ssl_set_own_cert %d\n", ret);
				error_no = 25;
				rtw_free((void *)ssl);
				goto err_exit;
			}

			/***********************************************************
			*  SSL 5. Wait for the ssl handshake done
			************************************************************/
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"Performing the SSL/TLS handshake...");
			if((ret = ssl_handshake(ssl)) != 0) {
				AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"ERROR: ssl_handshake -0x%x\n", -ret);
				rtw_free((void *)ssl);
				error_no = 26;
				goto err_exit;
			}

			/***********************************************************
			*  SSL 6. Hang node on mainlist for global management
			************************************************************/
			node* seednode = create_node(s_mode, NODE_ROLE_SEED);
			if(seednode == NULL){
				AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
					"[ATPS]create node failed!");
				rtw_free((void *)ssl);
				error_no = 27;
				goto err_exit;
			}
			seednode->sockfd = s_newsockfd;
			seednode->port = ntohs(s_cli_addr.sin_port);
			seednode->addr = ntohl(s_cli_addr.sin_addr.s_addr);
			seednode->context = (void *)ssl;
			if(hang_seednode(ServerNodeUsed,seednode) < 0){
				delete_node(seednode);
				seednode = NULL;
			}
			else{
				AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"The SSL/TLS client is connected");
				#if CONFIG_LOG_SERVICE_LOCK
				log_service_lock();
				#endif
				at_printf("\r\n[ATPS] A client connected to server[%d]\r\n"
					"con_id:%d,"
					"seed,"
					"ssl,"
					"address:%s,"
					"port:%d,"
					"socket:%d", 
					ServerNodeUsed->con_id,
					seednode->con_id,
					inet_ntoa(s_cli_addr.sin_addr),
					ntohs(s_cli_addr.sin_port),
					seednode->sockfd
				);
				at_printf(STR_END_OF_ATCMD_RET);
				#if CONFIG_LOG_SERVICE_LOCK
				log_service_unlock();
				#endif
			}
		}
	}
	else
#endif // #if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
	{
		if (s_mode == NODE_MODE_TCP){//TCP MODE
			/***********************************************************
			*  TCP 1. Set this socket into listen mode
			************************************************************/
			if(listen(s_sockfd , 5) < 0){
				AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"ERROR on listening");
				error_no = 8;
				goto err_exit;
			}	
			/***********************************************************
			*  TCP 2. Hang node on mainlist for global management
			************************************************************/
#if ATCMD_VER == ATVER_2
			if(param != NULL) {
				if(hang_node(ServerNodeUsed) < 0)
				{
					error_no = 9;
					goto err_exit;
				}else{
					#if CONFIG_LOG_SERVICE_LOCK
					log_service_lock();
					#endif
					at_printf("\r\n[ATPS] OK"
					"\r\n[ATPS] con_id=%d",
					ServerNodeUsed->con_id);
					at_printf(STR_END_OF_ATCMD_RET);
					#if CONFIG_LOG_SERVICE_LOCK
					log_service_unlock();
					#endif
				}
			}
#endif
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"The TCP SERVER START OK!");
			s_client = sizeof(s_cli_addr);
			/***********************************************************
			*  TCP 3. Waiting for TCP client to connect
			************************************************************/
			while(1){
				if((s_newsockfd = accept(s_sockfd,(struct sockaddr *) &s_cli_addr,&s_client)) < 0){
#if ATCMD_VER == ATVER_2
					if(param != NULL) {
						AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
							"[ATPS] ERROR:ERROR on accept");
					}
	//				else
	//#endif
#else
					{
						AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
						"ERROR on accept");
					}
#endif
					error_no = 10;
					goto err_exit;
				}
				else{
					/***********************************************************
					*  TCP 4. Hang node on mainlist for global management of this TCP connection
					************************************************************/
#if ATCMD_VER == ATVER_2
					if(param != NULL) {
						node* seednode = create_node(s_mode, NODE_ROLE_SEED);
						if(seednode == NULL){
							AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
								"[ATPS]create node failed!");
							error_no = 11;
							goto err_exit;
						}
						seednode->sockfd = s_newsockfd;
						seednode->port = ntohs(s_cli_addr.sin_port);
						seednode->addr = ntohl(s_cli_addr.sin_addr.s_addr);
						if(hang_seednode(ServerNodeUsed,seednode) < 0){
							delete_node(seednode);
							seednode = NULL;
						}
						else{
							#if CONFIG_LOG_SERVICE_LOCK
							log_service_lock();
							#endif
							at_printf("\r\n[ATPS] A client connected to server[%d]\r\n"
								"con_id:%d,"
								"seed,"
								"tcp,"
								"address:%s,"
								"port:%d,"
								"socket:%d", 
								ServerNodeUsed->con_id,
								seednode->con_id,
								inet_ntoa(s_cli_addr.sin_addr.s_addr),
								ntohs(s_cli_addr.sin_port),
								seednode->sockfd
								);
							at_printf(STR_END_OF_ATCMD_RET);
							#if CONFIG_LOG_SERVICE_LOCK
							log_service_unlock();
							#endif
						}
					}
	//				else
	//#endif
#else
					{
						newsockfd = s_newsockfd;
							rtw_memcpy(&cli_addr, &s_cli_addr, sizeof(cli_addr));
						client = s_client;
						AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,
							"A client connected to this server :\r\n[PORT]: %d\r\n[IP]:%s", 
							ntohs(cli_addr.sin_port), inet_ntoa(cli_addr.sin_addr.s_addr));
					}
#endif
				}
			}
		}
		else{
			/***********************************************************
			*  UDP 1. Enable broadcast on this socket 
			************************************************************/
#if IP_SOF_BROADCAST && IP_SOF_BROADCAST_RECV
			int so_broadcast = 1; 
			if(setsockopt(s_sockfd, SOL_SOCKET, SO_BROADCAST, &so_broadcast,  sizeof(so_broadcast)) < 0){
				error_no = 14;
				goto err_exit;
			}
#endif
			/***********************************************************
			*  UDP 2. Hang node on mainlist for global management
			************************************************************/
#if ATCMD_VER == ATVER_2
			if(ServerNodeUsed != NULL) {
				if(hang_node(ServerNodeUsed) < 0){
					error_no = 12;
					goto err_exit;
				}
				#if CONFIG_LOG_SERVICE_LOCK
				log_service_lock();
				#endif
				at_printf("\r\n[ATPS] OK"
					"\r\n[ATPS] con_id=%d",
					ServerNodeUsed->con_id);
				at_printf(STR_END_OF_ATCMD_RET);
				#if CONFIG_LOG_SERVICE_LOCK
				log_service_unlock();
				#endif
				//task will exit itself
				ServerNodeUsed->handletask = NULL;
			}
#endif
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"The UDP SERVER START OK!");
		}
	}
	goto exit;
err_exit:
#if ATCMD_VER == ATVER_2 
	if(ServerNodeUsed){
		//task will exit itself if getting here
		ServerNodeUsed->handletask = NULL;
		delete_node(ServerNodeUsed);
	}
	//else
//#endif
#else
	{
		socket_close();
	}
#endif
	#if CONFIG_LOG_SERVICE_LOCK
	log_service_lock();
	#endif
	at_printf("\r\n[ATPS] ERROR:%d", error_no);
	at_printf(STR_END_OF_ATCMD_RET);
	#if CONFIG_LOG_SERVICE_LOCK
	log_service_unlock();
	#endif
exit:
	return;
}

static void client_start(void *param)
{
	int c_mode;
	int c_remote_port;
	char c_remote_addr[16];
	int c_sockfd;
	struct sockaddr_in c_serv_addr;
	int error_no = 0;
#if ATCMD_VER == ATVER_2
#if ATCMD_SUPPORT_SSL
	ssl_context *ssl;
	int ret;
#endif
	struct in_addr c_addr;
	node * ClientNodeUsed = (node *)param;
	if(ClientNodeUsed){
		c_mode = ClientNodeUsed->protocol;
		c_remote_port = ClientNodeUsed->port;
		c_addr.s_addr = htonl(ClientNodeUsed->addr);
		if(inet_ntoa_r(c_addr, c_remote_addr, sizeof(c_remote_addr))==NULL){
			error_no = 6;
			goto err_exit;
		}
	}
	//else
//#endif
#else
	{
		c_mode = mode;
		c_remote_port = remote_port;
		rtw_memcpy(c_remote_addr, remote_addr, 16*sizeof(char));	
	}
#endif

	/***********************************************************
	* Create socket and set socket options, then bind socket to local port
	************************************************************/
#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
	if(c_mode == NODE_MODE_SSL){
		if((ret = net_connect(&c_sockfd, c_remote_addr, c_remote_port)) != 0){
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"Failed to create sock_fd!ret=%d", ret);
			error_no = 18;
			goto err_exit;
		}
	}
	else
#endif
	{ 
		if(c_mode == NODE_MODE_UDP)
			c_sockfd = socket(AF_INET,SOCK_DGRAM,0);
		else if(c_mode == NODE_MODE_TCP)
			c_sockfd = socket(AF_INET, SOCK_STREAM, 0);
		else
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"Unknown connection type[%d]", c_mode);
		if (c_sockfd == INVALID_SOCKET_ID) {
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"Failed to create sock_fd!");
			error_no = 7;
			goto err_exit;
		}
		rtw_memset(&c_serv_addr, 0, sizeof(c_serv_addr));
		c_serv_addr.sin_family = AF_INET;
		c_serv_addr.sin_addr.s_addr = inet_addr(c_remote_addr);
		c_serv_addr.sin_port = htons(c_remote_port);
	}
	AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"OK to create sock_fd!");

	/***********************************************************
	* Assign socket fd to the node used for this client
	************************************************************/
#if ATCMD_VER == ATVER_2
	if(ClientNodeUsed){
		ClientNodeUsed->sockfd = c_sockfd;
	}
	//else
//#endif
#else
	{
		sockfd = c_sockfd;
		rtw_memcpy(&serv_addr, &c_serv_addr, sizeof(c_serv_addr));
	}
#endif

#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
	if (c_mode == NODE_MODE_SSL){//SSL MODE
		/***********************************************************
		*  SSL 1. Setup stuff for this ssl connection
		************************************************************/
		int retry_count = 0;
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"Setting up the SSL/TLS structure..." );
		memory_set_own(atcmd_ssl_malloc, atcmd_ssl_free);
		ssl = (ssl_context *)rtw_zmalloc(sizeof(ssl_context));
		if(ssl == NULL){
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"malloc fail for ssl");
			error_no = 19;
			goto err_exit;
		}
		ClientNodeUsed->context = (void*)ssl;
		if((ret = ssl_init(ssl)) != 0) {
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
				" ssl_init failed: %d", ret);
			error_no = 20;
			goto err_exit;
		}
		ssl_set_endpoint(ssl, SSL_IS_CLIENT);
		ssl_set_authmode(ssl, SSL_VERIFY_NONE);
		ssl_set_rng(ssl, atcmd_ssl_random, NULL);
		ssl_set_bio(ssl, net_recv, &ClientNodeUsed->sockfd, net_send, &ClientNodeUsed->sockfd);
		ssl_set_dbg(ssl, atcmd_ssl_debug, NULL);
#ifdef POLARSSL_DEBUG_C
		debug_set_threshold(ATCMD_SSL_DEBUG_LEVEL);
#endif
		/***********************************************************
		*  SSL 2. Wait for the ssl handshake done
		************************************************************/
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"Performing the SSL/TLS handshake...");
		while((ret = ssl_handshake(ssl)) != 0) {
			if((ret != POLARSSL_ERR_NET_WANT_READ && ret != POLARSSL_ERR_NET_WANT_WRITE  
				&& ret != POLARSSL_ERR_NET_RECV_FAILED) || retry_count >= 5) {
				AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"ssl_handshake failed -0x%x\n", -ret);
				error_no = 21;
				goto err_exit;
			}
			retry_count++;
		}
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"Connect to Server successful!");
		/***********************************************************
		*  SSL 3. Hang node on mainlist for global management
		************************************************************/
		if(hang_node(ClientNodeUsed) < 0){
			error_no = 22;
			goto err_exit;
		}
		#if CONFIG_LOG_SERVICE_LOCK
		log_service_lock();
		#endif
		at_printf("\r\n[ATPC] OK\r\n[ATPC] con_id=%d",ClientNodeUsed->con_id);
		at_printf(STR_END_OF_ATCMD_RET);
		#if CONFIG_LOG_SERVICE_LOCK
		log_service_unlock();
		#endif		
	}
	else 
#endif //#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
	{
		if (c_mode == NODE_MODE_TCP){//TCP MODE
			/***********************************************************
			*  TCP 1. Connect a netconn to a specific remote IP address and port
			************************************************************/
			if(connect(c_sockfd, (struct sockaddr *)&c_serv_addr,  sizeof(c_serv_addr)) == 0){
				AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"Connect to Server successful!");
				/***********************************************************
				*  TCP 2. Hand node on mainlist for global management if connect success
				************************************************************/
#if ATCMD_VER == ATVER_2
				if(ClientNodeUsed != NULL) {
					if(hang_node(ClientNodeUsed) < 0){
						error_no = 8;
						goto err_exit;
					}
					#if CONFIG_LOG_SERVICE_LOCK
					log_service_lock();
					#endif
					at_printf("\r\n[ATPC] OK\r\n[ATPC] con_id=%d",ClientNodeUsed->con_id);
					at_printf(STR_END_OF_ATCMD_RET);
					#if CONFIG_LOG_SERVICE_LOCK
					log_service_unlock();
					#endif
				}
#endif
			}else{
				/***********************************************************
				*  TCP 2. Free node if connect fail
				************************************************************/
#if ATCMD_VER == ATVER_2
				if(ClientNodeUsed != NULL) {
					AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"[ATPC] ERROR:Connect to Server failed!");
				}
				//else
//#endif
#else
				{
					AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"Connect to Server failed!");
				}
#endif
				error_no = 9;
				goto err_exit;
			}
		}
		else{
#if ATCMD_VER == ATVER_2
			if(ClientNodeUsed != NULL) {
				#if IP_SOF_BROADCAST && IP_SOF_BROADCAST_RECV
				/* all ones (broadcast) or all zeroes (old skool broadcast) */
				if((c_serv_addr.sin_addr.s_addr == htonl(INADDR_BROADCAST))||
					(c_serv_addr.sin_addr.s_addr == htonl(INADDR_ANY))){
					int so_broadcast = 1; 
					if(setsockopt(c_sockfd, SOL_SOCKET, SO_BROADCAST, &so_broadcast,  
						sizeof(so_broadcast)) < 0){
						error_no = 14;
						goto err_exit;
					}
				}
				#endif
				#if LWIP_IGMP
				ip_addr_t dst_addr;
				dst_addr.addr = c_serv_addr.sin_addr.s_addr;
				if(ip_addr_ismulticast(&dst_addr)){
					struct ip_mreq imr;
					struct in_addr intfAddr;
					// Set NETIF_FLAG_IGMP flag for netif which should process IGMP messages
					xnetif[0].flags |= NETIF_FLAG_IGMP;
					imr.imr_multiaddr.s_addr = c_serv_addr.sin_addr.s_addr;
					imr.imr_interface.s_addr = INADDR_ANY;
					if(setsockopt(c_sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
						&imr, sizeof(imr)) < 0){
						xnetif[0].flags &= ~NETIF_FLAG_IGMP;
						error_no = 15;
						goto err_exit;
					}
					intfAddr.s_addr = INADDR_ANY;
					if(setsockopt(c_sockfd, IPPROTO_IP, IP_MULTICAST_IF, 
						&intfAddr, sizeof(struct in_addr)) < 0){
						xnetif[0].flags &= ~NETIF_FLAG_IGMP;
						error_no = 16;
						goto err_exit;				
					}
				}
				#endif
				if(ClientNodeUsed->local_port){
					struct sockaddr_in addr;  
						rtw_memset(&addr, 0, sizeof(addr));  
					addr.sin_family=AF_INET;  
					addr.sin_port=htons(ClientNodeUsed->local_port);  
					addr.sin_addr.s_addr=htonl(INADDR_ANY) ;  
					if (bind(ClientNodeUsed->sockfd, (struct sockaddr *)&addr, sizeof(addr))<0) {
						AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"bind sock error!"); 
						error_no = 12;
						goto err_exit;
					}
				}
				if(hang_node(ClientNodeUsed) < 0){
					error_no = 10;
					goto err_exit;
				}
				#if CONFIG_LOG_SERVICE_LOCK
				log_service_lock();
				#endif
				at_printf("\r\n[ATPC] OK\r\n[ATPC] con_id=%d",ClientNodeUsed->con_id);
				at_printf(STR_END_OF_ATCMD_RET);
				#if CONFIG_LOG_SERVICE_LOCK
				log_service_unlock();
				#endif
			}
#endif
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"UDP client starts successful!");
		}
	}
	goto exit;
err_exit:
#if ATCMD_VER == ATVER_2 
	if(ClientNodeUsed)
	{
		delete_node(ClientNodeUsed);
	}
	//else
//#endif
#else
	{
		socket_close();
	}
#endif
	#if CONFIG_LOG_SERVICE_LOCK
	log_service_lock();
	#endif
	at_printf("\r\n[ATPC] ERROR:%d", error_no);
	at_printf(STR_END_OF_ATCMD_RET);
	#if CONFIG_LOG_SERVICE_LOCK
	log_service_unlock();
	#endif
exit:
	return;
}

static void client_start_task(void *param)
{
	vTaskDelay(1000);
#if ATCMD_VER == ATVER_2 
	if(param){
		client_start(param);
		vTaskDelete(NULL);
		return;
	}
//#endif
#else
	if(remote_addr == NULL){
		printf("\r\n[ERROR] Please using ATP3 to input an valid remote IP address!\r\n");
		vTaskDelete(client_task);
		client_task = NULL;
	}
	else if(remote_port == 0){
		printf("\r\n[ERROR] Please using ATP4 to input an valid remote PORT!\r\n");
		vTaskDelete(client_task);
		client_task = NULL;
	}
	else{
		printf("\n\r\tStart Client\r\n\t[IP]: %s\r\n\t[PORT]:%d\n\r\n\r", remote_addr, remote_port);
		client_start(param);
	}
#endif
	vTaskDelete(NULL);
}

static void server_start_task(void *param)
{
	vTaskDelay(1000);
#if ATCMD_VER == ATVER_2      
	if(param != NULL){
		server_start(param);
		vTaskDelete(NULL);
		return;
	}
//#endif
#else
	if(local_port == 0){
		printf("\r\n[ERROR] Please using ATP2 to input an valid local PORT!\r\n");
		vTaskDelete(server_task);
		server_task = NULL;
	}
	else{
		uint8_t *ip = (uint8_t *)LwIP_GetIP(&xnetif[0]);
		printf("\n\rStart Server\r\n\t[IP]: %d.%d.%d.%d\r\n\t[PORT]:%d\n\r\n\r", ip[0], ip[1], ip[2], ip[3], local_port);
		server_start(param);
	}
#endif
	vTaskDelete(NULL);
}

//AT Command function
#if ATCMD_VER == ATVER_1
void fATP1(void *arg){
	if(!arg){
		printf("[ATP1]Usage: ATP1=MODE\n\r");
		goto exit;
	}	
	mode = atoi((char*)arg);
	printf("[ATP1]: _AT_TRANSPORT_MODE_ [%d]\n\r", mode);
exit:
	return;
}

void fATP2(void *arg){
	if(!arg){
		printf("[ATP2]Usage: ATP2=LOCAL_PORT\n\r");
		goto exit;
	}
	local_port = atoi((char*)arg);
	printf("[ATP2]: _AT_TRANSPORT_LOCAL_PORT_ [%d]\n\r", local_port);

exit:
	return;
}

void fATP3(void *arg){
	if(!arg){
		printf("[ATP3]Usage: ATP3=REMOTE_IP\n\r");
		goto exit;
	}
	strcpy((char*)remote_addr, (char*)arg);
	printf("[ATP3]: _AT_TRANSPORT_REMOTE_IP_ [%s]\n\r", remote_addr);

exit:
	return;
}

void fATP4(void *arg){
	if(!arg){
		printf("[ATP4]Usage: ATP4=REMOTE_PORT\n\r");
		goto exit;
	}
	remote_port = atoi((char*)arg);
	printf("[ATP4]: _AT_TRANSPORT_REMOTE_PORT_ [%d]\n\r", remote_port);

exit:
	return;
}

void fATP5(void *arg){
	int server_status = 0;
	if(!arg){
		printf("[ATP5]Usage: ATP5=0/1(0:server disable; 1: server enable)\n\r");
		goto exit;
	}
	server_status = atoi((char*)arg);
	printf("[ATP5]: _AT_TRANSPORT_START_SERVER_ [%d]\n\r", server_status);
	if(mode == 0){
		if(server_status == 0){
			socket_close();
			type = 0;
		}
		else if(server_status == 1){
			if(server_task == NULL)
			{
				if(xTaskCreate(server_start_task, ((const char*)"server_start_task"), ATCP_STACK_SIZE, NULL, ATCMD_LWIP_TASK_PRIORITY, &server_task) != pdPASS)
					printf("\r\n[ATP5]ERROR: Create tcp server task failed.\r\n");
			}
			type = 1;
		}
		else
		printf("[ATP5]ERROR: Just support two mode : 0 or 1\n\r");
	}
	else if(mode == 1){
		if(server_status == 0){
			socket_close();
			type = 0;
		}
		else if(server_status == 1){
			if(server_task == NULL)
			{
				if(xTaskCreate(server_start_task, ((const char*)"server_start_task"), ATCP_STACK_SIZE, NULL, ATCMD_LWIP_TASK_PRIORITY, &server_task) != pdPASS)
					printf("\r\n[ATP5]ERROR: Create udp server task failed.\r\n");
			}
			type = 3;
		}
		else
			printf("[ATP5]ERROR: Just support two mode : 0 or 1\n\r");
	}
	else 
		printf("[ATP5]Error: mode(TCP/UDP) can't be empty\n\r");
	
exit:
	return;
}

void fATP6(void *arg){
	int client_status = 0;
	if(!arg){
		printf("[ATP6]Usage: ATP6=0/1(0:Client disable; 1: Client enable)\n\r");
		goto exit;
	}
	client_status = atoi((char*)arg);
	printf("[ATP6]: _AT_TRANSPORT_START_CLIENT_ [%d]\n\r", client_status);
	if(mode == 0){
		if(client_status == 0){
			socket_close();
			type = 0;
		}
		else if(client_status == 1){
			printf("\r\n[ATP6]TCP Client mode will start\r\n");
			if(client_task == NULL)
			{
				if(xTaskCreate(client_start_task, ((const char*)"client_start_task"), ATCP_STACK_SIZE, NULL, ATCMD_LWIP_TASK_PRIORITY, &client_task) != pdPASS)
					printf("\r\n[ATP6]ERROR: Create tcp client task failed.\r\n");
			}
			type = 2;
		}
		else
			printf("[ATP6]ERROR: Just support two mode : 0 or 1\n\r");
	}
	else if(mode == 1){
		if(client_status == 0){
			socket_close();
			type = 0;
		}
		else if(client_status == 1){
			if(client_task == NULL)
			{
				if(xTaskCreate(client_start_task, ((const char*)"client_start_task"), ATCP_STACK_SIZE, NULL, ATCMD_LWIP_TASK_PRIORITY, &client_task) != pdPASS)
					printf("\r\n[ATP6]ERROR: Create udp client task failed.\r\n");
			}
			type = 4;
		}
		else
			printf("[ATP6]ERROR: Just support two mode : 0 or 1\n\r");
	}
	else 
		printf("[ATP6]Error: mode(TCP/UDP) can't be empty\n\r");
	
exit:
	return;
}

void fATPZ(void *arg){
	uint8_t *ip;
	printf("\n\r\nThe current Transport settings:");
	printf("\n\r==============================");
	if(mode == 0)
		printf("\n\r Protocol: TCP");
	else if(mode == 1)
		printf("\n\r Protocol: UDP");
	
	ip = (uint8_t *)LwIP_GetIP(&xnetif[0]);
	printf("\n\r LOCAL_IP  => %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
	printf("\n\r LOCAL_PORT => %d", local_port);
	printf("\n\r REMOTE_IP  => %s", remote_addr);
	printf("\n\r REMOTE_PORT => %d", remote_port);

	printf("\n\r");	
}

void fATR0(void *arg){
	if(packet_size <= 0){
		packet_size = MAX_BUFFER;
		printf("[ATR0]Notice: Didn't set the value of packet_size, will using the MAX_BUFFER: %d\r\n", MAX_BUFFER);
	}
	rtw_memset(rx_buffer, 0, MAX_BUFFER);
	if(type == 1){//tcp server
		if((read(newsockfd,rx_buffer,MAX_BUFFER)) > 0)
			printf("[ATR0]Receive the data:%s\r\n with packet_size: %d\r\n", rx_buffer, packet_size);
		else
			printf("[ATR0]ERROR: Failed to receive data!\r\n");
		close(newsockfd);
		newsockfd = -1;
	}
	else{
		if((read(sockfd,rx_buffer,MAX_BUFFER)) > 0)
			printf("[ATR0]Receive the data:%s\r\n with packet_size: %d\r\n", rx_buffer, packet_size);
		else
			printf("[ATR0]ERROR: Failed to receive data!\r\n");
	}
}

void fATR1(void *arg){
	int size;
	if(!arg){
		printf("[ATR1]Usage: ATR1=packet_size(cannot exceed %d)\n\r", MAX_BUFFER);
		goto exit;
	}
	size = atoi((char*)arg);
	printf("[ATR1]: _AT_TRANSPORT_RECEIVE_PACKET_SIZE_ [%d]\n\r", size);
	if(size < 1)
		printf("[ATR1]Error: packet size need be larger than 0!\n\r");
	else if(size > MAX_BUFFER)
		printf("[ATR1]Error: packet size exceeds the MAX_BUFFER value: %d!\n\r", MAX_BUFFER);
	else 
		packet_size = size;
exit:
	return;
}


void fATRA(void *arg){
	if(!arg){
		printf("[ATRA]Usage: ATRA=[data](Data size cannot exceed the MAX_BUFFER SIZE: %d)\n\r", MAX_BUFFER);
		return;
	}

	if(packet_size <= 0){
		packet_size = MAX_BUFFER;
		printf("[ATRA]Notice: Didn't set the value of packet_size, will using the MAX_BUFFER SIZE: %d\r\n", MAX_BUFFER);
	}


	int argc;
	char *argv[MAX_ARGC] = {0};

	if((argc = parse_param(arg, argv)) != 2){
		printf("[ATRA]Usage: ATRA=[data](Data size cannot exceed the MAX_BUFFER SIZE: %d)\n\r", MAX_BUFFER);
		goto exit;
	}
	else 
		printf("[ATRA]: _AT_TRANSPORT_WRITE_DATA_ [%s]\n\r", argv[1]);
	rtw_memset(tx_buffer, 0, MAX_BUFFER);
	rtw_memcpy(tx_buffer, argv[1], strlen(argv[1]));

	if(type == 1){//tcp server
		if((write(newsockfd,tx_buffer,strlen(tx_buffer))) > 0)
			printf("[ATRA] Sending data:%s\r\n with packet_size:%d\r\n", tx_buffer, packet_size);
		else
			printf("[ATRA]ERROR: Failed to send data\r\n");
		close(newsockfd);
		newsockfd = -1;
	}
	else if(type == 4){//udp client
		int ret = 0;
		ret = sendto(sockfd, tx_buffer, strlen(tx_buffer), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

		printf("\r\nThe value of ret is %d\r\n", ret);
	}
	else if(type == 3)
		printf("\r\nThe UDP Server mode not support Sending data service!\r\n");
	else{
		if((write(sockfd,tx_buffer, strlen(tx_buffer))) > 0)
			printf("[ATRA] Sending data:%s\r\n with packet_size:%d\r\n", tx_buffer, packet_size);
		else
			printf("[ATRA]ERROR: Failed to send data\r\n");
	}
exit:
	return;
}

void fATRB(void *arg){
	int size;
	if(!arg){
		printf("[ATRB]Usage: ATRB=packet_size(cannot exceed %d)\n\r", MAX_BUFFER);
		goto exit;
	}
	size = atoi((char*)arg);
	printf("[ATRB]: _AT_TRANSPORT_WRITE_PACKET_SIZE_ [%d]\n\r", size);
	if(size < 1)
		printf("[ATRB]Error: packet size need be larger than 0!\n\r");
	else if(size > MAX_BUFFER)
		printf("[ATRB]Error: packet size exceeds the MAX_BUFFER value: %d!\n\r", MAX_BUFFER);
	else 
		packet_size = size;
exit:
	return;
}
#endif
#if ATCMD_VER == ATVER_2
void fATP0(void *arg){
	AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS, 
		"[ATP0]: _AT_TRANSPORT_ERRNO");
#ifdef ERRNO
	at_printf("\r\n[ATP0] OK:%d", errno);
#else
	AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"errno isn't enabled");
	at_printf("\r\n[ATP0] ERROR");
#endif
}

void fATPC(void *arg){

	int argc;
	char* argv[MAX_ARGC] = {0};
	node* clientnode = NULL;
	int mode = 0;
	int remote_port;
	int local_port = 0;
	//char remote_addr[DNS_MAX_NAME_LENGTH];
	struct in_addr addr;
	int error_no = 0;
	u32 client_task_stksz = ATCP_STACK_SIZE;
#if LWIP_DNS  
	struct hostent *server_host;
#endif

	AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS, 
		"[ATPC]: _AT_TRANSPORT_START_CLIENT");
	
	if(atcmd_lwip_is_tt_mode() && mainlist->next){
		error_no = 13;
		goto err_exit;
	}
	
	argc = parse_param(arg, argv);
	if(argc < 4){
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
			"[ATPC] Usage: ATPC=<TCP:0/UDP:1/SSL:2>,<REMOTE_IP>,<REMOTE_Port(1~65535)>,[<LOCAL_PORT>]");
		error_no = 1;
		goto err_exit;
	}

	mode = atoi((char*)argv[1]);//tcp, udp or ssl
	if(mode != NODE_MODE_TCP && mode != NODE_MODE_UDP
#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
		&& mode != NODE_MODE_SSL
#endif
		){
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
			"[ATPC] Unknown connection type[%d]", mode);
		error_no = 17;
		goto err_exit;		
	}
	
	remote_port = atoi((char*)argv[3]);
	if (inet_aton(argv[2], &addr) == 0) 
	{
#if LWIP_DNS  
		server_host = gethostbyname(argv[2]);
		if (server_host){
			rtw_memcpy(&addr, server_host->h_addr, 4);
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,
				"[ATPC] Found name '%s' = %s", 
				argv[2],
				inet_ntoa(addr)
			);
		}
		else
#endif
		{
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
					"[ATPC] ERROR: Host '%s' not found.", argv[2]);
			error_no = 2;
			goto err_exit; 
		}
	}
	
	if(remote_port < 0 || remote_port > 65535)    {
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
			"[ATPC] ERROR: remote port invalid");
		error_no = 3;
		goto err_exit;
	}
	
	if(argv[4]){
		local_port = atoi((char*)argv[4]);
		if(local_port < 0 || local_port > 65535)    {
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
				"[ATPC] ERROR: local port invalid");
			error_no = 11;
			goto err_exit;
		}
	}
	
	clientnode = create_node(mode, NODE_ROLE_CLIENT);
	if(clientnode == NULL){
		error_no = 4;
		goto err_exit;
	}
	clientnode->port = remote_port;
	clientnode->addr = ntohl(addr.s_addr);
	clientnode->local_port = local_port;
#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
	if(mode == NODE_MODE_SSL)
		client_task_stksz = ATCP_SSL_STACK_SIZE;
#endif
	if(xTaskCreate(client_start_task, ((const char*)"client_start_task"), client_task_stksz, clientnode, ATCMD_LWIP_TASK_PRIORITY, NULL) != pdPASS)
	{	
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
			"[ATPC] ERROR: Create tcp/udp/ssl client task failed.");
		error_no = 5;
		goto err_exit;
	}

	goto exit;
err_exit:
	if(clientnode)
		delete_node(clientnode);
	at_printf("\r\n[ATPC] ERROR:%d", error_no);
exit:
	return;
}


void fATPS(void *arg){
	int argc;
	char *argv[MAX_ARGC] = {0};
	node* servernode = NULL;
	int mode;
	int local_port;
	int error_no = 0;
	u32 server_task_stksz = ATCP_STACK_SIZE;

	AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS, 
		"[ATPS]: _AT_TRANSPORT_START_SERVER");
	
	if(atcmd_lwip_is_tt_mode()){
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
			"[ATPS] ERROR: Server can only start when TT is disabled");
		error_no = 13;
		goto err_exit;
	}
	
	argc = parse_param(arg, argv);
	if(argc != 3){
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
			"[ATPS] Usage: ATPS=[TCP:0/UDP:1/SSL:2],[Local port(1~65535)]");
		error_no = 1;
		goto err_exit;
	}

	mode = atoi((char*)argv[1]);
	if(mode != NODE_MODE_TCP && mode != NODE_MODE_UDP
#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
		&& mode != NODE_MODE_SSL
#endif
		){
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
			"[ATPS] Unknown connection type[%d]", mode);
		error_no = 15;
		goto err_exit;		
	}

	local_port = atoi((char*)argv[2]);
	if(local_port < 0 || local_port > 65535){
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
			"[ATPS] Usage: ATPS=[TCP:0/UDP:1/SSL:2],[Local port]");
		error_no = 2;
		goto err_exit;
	}

	servernode = create_node(mode, NODE_ROLE_SERVER);
	if(servernode == NULL){
		error_no = 3;
		goto err_exit;
	}
	servernode->port = local_port;

#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
	if(mode == NODE_MODE_SSL)
		server_task_stksz = ATCP_SSL_STACK_SIZE;
#endif //#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL

	if(xTaskCreate(server_start_task, ((const char*)"server_start_task"), server_task_stksz, servernode, ATCMD_LWIP_TASK_PRIORITY, &servernode->handletask) != pdPASS)
	{	
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
			"[ATPS] ERROR: Create tcp/udp/ssl server task failed.");
		error_no = 4;
		goto err_exit;
	}

	goto exit;
err_exit:
	if(servernode)
		delete_node(servernode);
	at_printf("\r\n[ATPS] ERROR:%d", error_no);
exit:
	return;
}

void socket_close_all(void)
{
	node *currNode = mainlist->next;
	
	while(currNode)
	{
		delete_node(currNode);
		currNode = mainlist->next;
	}
	currNode = NULL;
}

void fATPD(void *arg){
	int con_id = INVALID_CON_ID;
	int error_no = 0;
	node *s_node;

	AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS, 
		"[ATPD]: _AT_TRANSPORT_CLOSE_CONNECTION");
	
	if(!arg){
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
			"[ATPD] Usage: ATPD=con_id or 0 (close all)");
		error_no = 1;
		goto exit;
	}
	con_id = atoi((char*)arg);

	if(con_id == 0){
		if(atcmd_lwip_is_autorecv_mode()){
			atcmd_lwip_set_autorecv_mode(FALSE);
		}
		socket_close_all();
		goto exit;
	}

	s_node = seek_node(con_id);
	if(s_node == NULL){
		error_no = 3;
		goto exit;
	}
	delete_node(s_node);

exit:
	s_node = NULL;
	if(error_no)
		at_printf("\r\n[ATPD] ERROR:%d", error_no);
	else
		at_printf("\r\n[ATPD] OK");
	return;
}

int atcmd_lwip_send_data(node *curnode, u8 *data, u16 data_sz, struct sockaddr_in cli_addr){
	int error_no = 0;
	
	if((curnode->protocol == NODE_MODE_UDP) && (curnode->role == NODE_ROLE_SERVER)) //UDP server
	{
		if (sendto(curnode->sockfd, data, data_sz, 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr)) <= 0 ){
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
				"[ATPT] ERROR:Failed to send data");
			error_no = 5;
		}
	}else{
		if(curnode->protocol == NODE_MODE_UDP) //UDP client/seed
		{
			struct sockaddr_in serv_addr;
			rtw_memset(&serv_addr, 0, sizeof(serv_addr));
			serv_addr.sin_family = AF_INET; 
			serv_addr.sin_port = htons(curnode->port);
			serv_addr.sin_addr.s_addr = htonl(curnode->addr);
			if(sendto( curnode->sockfd, data, data_sz, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <= 0){
				AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
					"[ATPT] ERROR:Failed to send data\n");
				error_no = 6;
			}
		}else if((curnode->protocol == NODE_MODE_TCP)
#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
		|| (curnode->protocol == NODE_MODE_SSL)
#endif
		)//TCP or SSL
		{
			int ret;
			if(curnode->role == NODE_ROLE_SERVER){
				AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
					"[ATPT] ERROR: TCP Server must send data to the seed");
				error_no = 7;
				goto exit;
			}
#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
			if(curnode->protocol == NODE_MODE_SSL){
				ret = ssl_write((ssl_context *)curnode->context, data, data_sz);
			}
			else
#endif // #if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
			{
				ret = write(curnode->sockfd, data, data_sz);
			}
			if(ret <= 0){
				AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
					"[ATPT] ERROR:Failed to send data");
				error_no = 8;
			}
		}
	}
	
exit:
	return error_no;
}

void fATPT(void *arg){

	int argc;
	char *argv[MAX_ARGC] = {0};
	int con_id = INVALID_CON_ID;
	int error_no = 0;
	node* curnode = NULL;
	struct sockaddr_in cli_addr;
	int data_sz;
	int data_pos = C_NUM_AT_CMD + C_NUM_AT_CMD_DLT+ strlen(arg) + 1;
	u8 *data = (u8 *)log_buf + data_pos;

	AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS, 
		"[ATPT]: _AT_TRANSPORT_SEND_DATA");
	
	argc = parse_param(arg, argv);

	if(argc != 3 && argc != 5) {
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
			"[ATPT] Usage: ATPT=<data_size>,"
			"<con_id>[,<dst_ip>,<dst_port>]"
			":<data>(MAX %d)", 
			MAX_BUFFER);
		error_no = 1;
		goto exit;
	}

	data_sz = atoi((char*)argv[1]);
	if(data_sz > MAX_BUFFER){
		error_no = 2;
		goto exit;
	}
	
	con_id = atoi((char*)argv[2]);
	curnode = seek_node(con_id);
	if(curnode == NULL){
		error_no = 3;
		goto exit;
	}

	if((curnode->protocol == NODE_MODE_UDP)
		&&(curnode->role == NODE_ROLE_SERVER))
	{
		char udp_clientaddr[16]={0};
		strcpy((char*)udp_clientaddr, (char*)argv[3]);
		cli_addr.sin_family = AF_INET;
		cli_addr.sin_port = htons(atoi((char*)argv[4]));
		if (inet_aton(udp_clientaddr , &cli_addr.sin_addr) == 0) 
		{
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
			"[ATPT]ERROR:inet_aton() failed");
			error_no = 4;
			goto exit;
		}
	}
	error_no = atcmd_lwip_send_data(curnode, data, data_sz, cli_addr);
exit:
	if(error_no)
		at_printf("\r\n[ATPT] ERROR:%d,%d", error_no, con_id);
	else
		at_printf("\r\n[ATPT] OK,%d", con_id);
	return;
}

void fATPR(void *arg){

	int argc,con_id = INVALID_CON_ID;
	char *argv[MAX_ARGC] = {0};
	int error_no = 0;
	int recv_size = 0;	
	int packet_size = 0;
	node* curnode = NULL;
	u8_t udp_clientaddr[16] = {0};
	u16_t udp_clientport = 0;

#if defined(EXTEND_ATPR_SIZE) && (EXTEND_ATPR_SIZE == 1)
    int total_recv_size = 0;
    int next_expected_size = 0;
    int fetch_counter = 0;
    char tmpbuf[ATPR_RSVD_HEADER_SIZE];
    int header_len = 0;
#endif

	AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS, 
		"[ATPR]: _AT_TRANSPORT_RECEIVE_DATA");

	if(atcmd_lwip_is_autorecv_mode()){
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
			"[ATPR] ERROR: Receive changed to auto mode.");
		error_no = 10;
		goto exit;
	}
	
	argc = parse_param(arg, argv);
	if( argc != 3){
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR, 
			"[ATPR] Usage: ATPR =<con_id>,<Buffer Size>\n\r");
		error_no = 1;
		goto exit;
	}

	con_id = atoi((char*)argv[1]);
	if(con_id <= 0 || con_id > NUM_NS){
		error_no = 9;
		goto exit;
	}
	
	packet_size = atoi((char*)argv[2]);

    if (packet_size <= 0
#if defined(EXTEND_ATPR_SIZE) && (EXTEND_ATPR_SIZE==1)
        || packet_size > ((rx_buffer_size > MAX_BUFFER) ? rx_buffer_size : MAX_BUFFER)
#else
        || packet_size > MAX_BUFFER
#endif
    ) {
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR, 
			"[ATPR] Recv Size(%d) exceeds MAX_BUFFER(%d)", packet_size, 
			MAX_BUFFER);
		error_no = 2;
		goto exit;
	}

	curnode = seek_node(con_id);
	if(curnode == NULL){
		error_no = 3;
		goto exit;
	}

	if(curnode->protocol == NODE_MODE_TCP && curnode->role == NODE_ROLE_SERVER){
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
			"[ATPR] ERROR: TCP Server must receive data from the seed");
		error_no = 6;
		goto exit;
	}

	memset(rx_buffer, 0, rx_buffer_size);
	error_no = atcmd_lwip_receive_data(curnode, rx_buffer, ETH_MAX_MTU, &recv_size, udp_clientaddr, &udp_clientport);
exit:
	if(error_no == 0){
#if defined(EXTEND_ATPR_SIZE) && (EXTEND_ATPR_SIZE == 1)
        total_recv_size = recv_size;
        fetch_counter = 0;
        while (total_recv_size < packet_size - ATPR_RSVD_HEADER_SIZE ) { // 100 is reserved for the AT command header
            next_expected_size = packet_size - total_recv_size - ATPR_RSVD_HEADER_SIZE;
            if (next_expected_size > ETH_MAX_MTU) {
                next_expected_size = ETH_MAX_MTU;
            }
            error_no = atcmd_lwip_receive_data(curnode, rx_buffer + total_recv_size, next_expected_size, &recv_size, udp_clientaddr, &udp_clientport);
            fetch_counter = (recv_size == 0) ? (fetch_counter+1) : 0;
            if (fetch_counter >= FETCH_TIMEOUT) {
                break;
            }
            total_recv_size += recv_size;
            if (error_no != 0) {
                break;
            }
        }
        memset(tmpbuf, 0, ATPR_RSVD_HEADER_SIZE);
		if(curnode->protocol == NODE_MODE_UDP && curnode->role == NODE_ROLE_SERVER){
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,
					"\r\n[ATPR] OK,%d,%d,%s,%d:%s", total_recv_size, con_id, udp_clientaddr, udp_clientport, rx_buffer);
            sprintf(tmpbuf, "\r\n[ATPR] OK,%d,%d,%s,%d:", total_recv_size, con_id, udp_clientaddr, udp_clientport);
		}
		else{
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,
					"\r\n[ATPR] OK,%d,%d:%s", total_recv_size, con_id, rx_buffer);
            sprintf(tmpbuf, "\r\n[ATPR] OK,%d,%d:", total_recv_size, con_id);
		}
        header_len = strlen(tmpbuf);
        memmove(rx_buffer + header_len, rx_buffer, total_recv_size);
        memcpy(rx_buffer, tmpbuf, header_len);
        at_print_data(rx_buffer, total_recv_size + header_len);
#else // #if (EXTEND_ATPR_SIZE)
		if(curnode->protocol == NODE_MODE_UDP && curnode->role == NODE_ROLE_SERVER){
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,
					"\r\n[ATPR] OK,%d,%d,%s,%d:%s", recv_size, con_id, udp_clientaddr, udp_clientport, rx_buffer);
			at_printf("\r\n[ATPR] OK,%d,%d,%s,%d:", recv_size, con_id, udp_clientaddr, udp_clientport);
		}
		else{
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,
					"\r\n[ATPR] OK,%d,%d:%s", recv_size, con_id, rx_buffer);
			at_printf("\r\n[ATPR] OK,%d,%d:", recv_size, con_id);
		}
		if(recv_size)
			at_print_data(rx_buffer, recv_size);
#endif // #if (EXTEND_ATPR_SIZE)
	}
	else
		at_printf("\r\n[ATPR] ERROR:%d,%d", error_no, con_id);
	return;
}

void fATPK(void *arg){

	int argc;
	int error_no = 0;
	int enable = 0;
	char *argv[MAX_ARGC] = {0};
	
	AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS, 
		"[ATPK]: _AT_TRANSPORT_AUTO_RECV");
	
	argc = parse_param(arg, argv);
	if( argc < 2){
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR, 
			"[ATPK] Usage: ATPK=<0/1>\n\r");
		error_no = 1;
		goto exit;
	}

	enable = atoi((char*)argv[1]);

	if(enable){
		if(atcmd_lwip_is_autorecv_mode()){
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS, "[ATPK] already enter auto receive mode");
		}
		else{
			if(atcmd_lwip_start_autorecv_task())
				error_no = 2;
			}
	}else{
		if(atcmd_lwip_is_autorecv_mode())
			atcmd_lwip_set_autorecv_mode(FALSE);
		else{
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"[ATPK] already leave auto receive mode");
		}
	}

exit:
	if(error_no)
		at_printf("\r\n[ATPK] ERROR:%d", error_no);
	else
		at_printf("\r\n[ATPK] OK");
	return;
}

void fATPU(void *arg){

	int argc;
	int error_no = 0;
	int enable = 0;
	char *argv[MAX_ARGC] = {0};
	
	AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS, 
		"[ATPU]: _AT_TRANSPORT_TT_MODE");
	
	argc = parse_param(arg, argv);
	if( argc < 2){
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR, 
			"[ATPU] Usage: ATPU=<1>\n\r");
		error_no = 1;
		goto exit;
	}

	enable = atoi((char*)argv[1]);

	if(enable){
		if(!mainlist->next){
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR, "[ATPU] No conn found");
				error_no = 2;
		}else if(mainlist->next->role == NODE_ROLE_SERVER){
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR, "[ATPU] No TT mode for server");
			error_no = 3;
		}
		else if(mainlist->next->next || mainlist->next->nextseed){
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR, "[ATPU] More than one conn found");
			error_no = 4;
		}
		else{
			if(atcmd_lwip_start_tt_task()){
				error_no = 5;
			}
		}
	}

exit:
	if(error_no)
		at_printf("\r\n[ATPU] ERROR:%d", error_no);
	else
		at_printf("\r\n[ATPU] OK");
	return;
}

//ATPL=<enable>
void fATPL(void *arg)
{
	int argc, error_no = 0;
	char *argv[MAX_ARGC] = {0};

	if(!arg){
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
			"\r\n[ATPL] Usage : ATPL=<enable>");
		error_no = 1;
		goto exit;
	}
	argc = parse_param(arg, argv);
	if(argc != 2){
		error_no = 2;
		goto exit;
	}

	//ENABLE LWIP FAST CONNECT
	if(argv[1] != NULL){
		int enable = atoi(argv[1]);
		struct atcmd_lwip_conn_info cur_conn = {0};
		node *cur_node = mainlist->next;
		if(enable && cur_node == NULL){
			error_no = 3;
			goto exit;
		}
		cur_conn.role = cur_node->role;
		cur_conn.protocol = cur_node->protocol;
		cur_conn.remote_addr = cur_node->addr;
		cur_conn.remote_port = cur_node->port;
		cur_conn.local_addr = cur_node->local_addr;
		cur_conn.local_port = cur_node->local_port;
		atcmd_lwip_write_info_to_flash(&cur_conn, enable);
	}

exit:
	if(error_no == 0)
		at_printf("\r\n[ATPL] OK");
	else
		at_printf("\r\n[ATPL] ERROR:%d",error_no);

	return;
}

extern void do_ping_call(char *ip, int loop, int count);
extern int get_ping_report(int *ping_lost);
void fATPP(void *arg){
	int count, argc = 0;
	char buf[32] = {0};
	char *argv[MAX_ARGC] = {0};
	int con_id=INVALID_CON_ID;
	int error_no = 0;
	int ping_lost = 0;
	
	AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS, 
		"[ATPP]: _AT_TRANSPORT_PING");
	
	if(!arg){
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"[ATPP] Usage: ATPP=xxxx.xxxx.xxxx.xxxx[y/loop] or ATPP=[con_id],[y/loop]\n\r");
		error_no = 1;
		goto exit;
	}
	
	argc = parse_param(arg, argv);

	if( strlen(argv[1]) < 3 )
	{
		node* curnode;
		struct in_addr addr;
		con_id = atoi( (char*)argv[1] );
		curnode = seek_node(con_id);
		if(curnode == NULL){
			error_no = 2;
			goto exit;
		}
		if( curnode->role == 1){ //ping remote server
			addr.s_addr = htonl(curnode->addr);
			inet_ntoa_r(addr, buf, sizeof(buf));
		}else if( curnode->role == 0){//ping local server
			strcpy(buf,SERVER);
		}else if( curnode->role == 2){ //ping seed
			strcpy(buf,(char*) curnode->addr);
		}
	}else
		strcpy(buf, argv[1]);

	if(argc == 2){
		count = 5;
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"[ATPP]Repeat Count: %d", count);
		do_ping_call(buf, 0, count);	//Not loop, count=5
	}else{
		if(strcmp(argv[2], "loop") == 0){
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"[ATPP]Repeat Count: %s", "loop");
			do_ping_call(buf, 1, 0);	//loop, no count
		}else{
			count = atoi(argv[2]);
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"[ATPP]Repeat Count: %d", count);
			do_ping_call(buf, 0, count);	//Not loop, with count
		}
	}

	get_ping_report(&ping_lost);
	if(ping_lost)
		error_no = (ping_lost == count)?4:3; // 3: partially lost, 4: totally lost
	
exit:
	if(error_no)
		at_printf("\r\n[ATPP] ERROR:%d", error_no);
	else
		at_printf("\r\n[ATPP] OK");
	return;
}

void fATPI(void *arg){
	node* n = mainlist->next;
	struct in_addr addr;

	AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS, 
		"[ATPI]: _AT_TRANSPORT_CONNECTION_INFO");
	
	while (n != NULL)
	{
		if(n->con_id == 0)
			continue;

		at_printf("\r\ncon_id:%d,", n->con_id);

		if(n->role == NODE_ROLE_SERVER)
			at_printf("server,");
		else
			at_printf("client,");
		if(n->protocol == NODE_MODE_TCP)
			at_printf("tcp,");
#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
		else if(n->protocol == NODE_MODE_SSL)
			at_printf("ssl,");
#endif
		else
			at_printf("udp,");

		addr.s_addr = htonl(n->addr);
		at_printf("address:%s,port:%d,socket:%d", inet_ntoa(addr) ,n->port, n->sockfd);
		if(n->nextseed != NULL)
		{
			node* seed = n;
			do{
				seed = seed->nextseed;
				at_printf("\r\ncon_id:%d,seed,", seed->con_id);
				if(seed->protocol == NODE_MODE_TCP)
					at_printf("tcp,");
#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
				else if(n->protocol == NODE_MODE_SSL)
					at_printf("ssl,");
#endif
				else
					at_printf("udp,");
				addr.s_addr = htonl(seed->addr);
				at_printf("address:%s,port:%d,socket:%d", inet_ntoa(addr), seed->port, seed->sockfd);
			}while (seed->nextseed != NULL);
		}
		n = n->next;
	}

	at_printf("\r\n[ATPI] OK");

	return;
}

void init_node_pool(void){
	int i;
	rtw_memset(node_pool, 0, sizeof(node_pool));
	for(i=0;i<NUM_NS;i++){
		node_pool[i].con_id = INVALID_CON_ID;
	}
}

node* create_node(int mode, s8_t role){
	int i;

	SYS_ARCH_DECL_PROTECT(lev);
	for (i = 0; i < NUM_NS; ++i) {
		SYS_ARCH_PROTECT(lev);
		if (node_pool[i].con_id == INVALID_CON_ID) {
			node_pool[i].con_id = i;
			SYS_ARCH_UNPROTECT(lev);
			node_pool[i].sockfd = INVALID_SOCKET_ID;
			node_pool[i].protocol = mode; // 0:TCP, 1:UDP
			node_pool[i].role = role; // 0:server, 1:client, 2:SEED
			node_pool[i].addr = 0;
			node_pool[i].port = -1;
			node_pool[i].handletask = NULL;
			node_pool[i].next = NULL;
			node_pool[i].nextseed = NULL;
#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
			node_pool[i].context = NULL;
#endif
			return &node_pool[i];
		}
		SYS_ARCH_UNPROTECT(lev);
	}
	AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR, "No con_id available");
	return NULL;
}

void delete_node(node *n)
{
	node *currNode, *prevNode, *currSeed, *precvSeed;
	if(n == NULL)
		return;
	SYS_ARCH_DECL_PROTECT(lev);
	SYS_ARCH_PROTECT(lev);
	//need to remove it from mainlist first 
	for(currNode = mainlist; currNode != NULL; prevNode = currNode, currNode = currNode->next )
	{
		if(currNode == n){
			prevNode->next = currNode->next;
		}

		if(currNode->role != NODE_ROLE_SERVER)
			continue;
		
		precvSeed = currNode;
		currSeed = currNode->nextseed;
		while (currSeed != NULL)
		{
			if(currSeed == n){
				precvSeed->nextseed = n->nextseed;
			}
			precvSeed = currSeed;
			currSeed = currSeed->nextseed;
		}	
	}
	SYS_ARCH_UNPROTECT(lev);
	
	if(n->role == NODE_ROLE_SERVER){
		//node may have seed if it's under server mode
		while(n->nextseed != NULL){
			currSeed = n->nextseed;
			// only tcp/ssl seed has its own socket, udp seed uses its server's
			// so delete udp seed can't close socket which is used by server
			if(currSeed->protocol == NODE_MODE_TCP && currSeed->sockfd != INVALID_SOCKET_ID){
				close(currSeed->sockfd);
			currSeed->sockfd = INVALID_SOCKET_ID;
			}
#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
			else if(currSeed->protocol == NODE_MODE_SSL && currSeed->sockfd != INVALID_SOCKET_ID){
				ssl_context *ssl = (ssl_context *)currSeed->context;
				ssl_close_notify(ssl);
				net_close(currSeed->sockfd);
				ssl_free(ssl);
				rtw_free(currSeed->context);
				currSeed->context = NULL;
			}
#endif
			// no task created for seed
			//if(s->handletask != NULL)
			//	vTaskDelete(s->handletask);
			n->nextseed = currSeed->nextseed;
			currSeed->con_id = INVALID_CON_ID;
		};
	}
	
	if(!((n->protocol == NODE_MODE_UDP)&&(n->role == NODE_ROLE_SEED))){
		if(n->sockfd != INVALID_SOCKET_ID){
#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
			if(n->protocol == NODE_MODE_SSL){
				if(n->role == NODE_ROLE_SEED){
					ssl_context *ssl = (ssl_context *)n->context;
					ssl_close_notify(ssl);
					net_close(currSeed->sockfd);
					ssl_free(ssl);
				}
				else if(n->role == NODE_ROLE_CLIENT){
					ssl_context *ssl = (ssl_context *)n->context;
					ssl_close_notify(ssl);
					net_close(n->sockfd);
					if(atcmd_ssl_cli_ca_crt[n->con_id]){
						x509_crt_free(atcmd_ssl_cli_ca_crt[n->con_id]);
						rtw_free((void *)atcmd_ssl_cli_ca_crt[n->con_id]);
						atcmd_ssl_cli_ca_crt[n->con_id] = NULL;
					}
					if(atcmd_ssl_client_ca_crt[n->con_id]){
						rtw_free(atcmd_ssl_client_ca_crt[n->con_id]);
						atcmd_ssl_client_ca_crt[n->con_id] = NULL;
					}
					if(atcmd_ssl_cli_crt[n->con_id]){
						x509_crt_free(atcmd_ssl_cli_crt[n->con_id]);
						rtw_free((void *)atcmd_ssl_cli_crt[n->con_id]);
						atcmd_ssl_cli_crt[n->con_id] = NULL;
					}
					if(atcmd_ssl_client_crt[n->con_id]){
						rtw_free(atcmd_ssl_client_crt[n->con_id]);
						atcmd_ssl_client_crt[n->con_id] = NULL;
					}
					if(atcmd_ssl_clikey_rsa[n->con_id]){
						pk_free(atcmd_ssl_clikey_rsa[n->con_id]);
						rtw_free((void *)atcmd_ssl_clikey_rsa[n->con_id]);
						atcmd_ssl_clikey_rsa[n->con_id] = NULL;
					}
					if(atcmd_ssl_client_key[n->con_id]){
						rtw_free(atcmd_ssl_client_key[n->con_id]);
						atcmd_ssl_client_key[n->con_id] = NULL;
					}
					ssl_free(ssl);
				}else{
					net_close(n->sockfd);
					if(atcmd_ssl_srv_crt[n->con_id]){
						x509_crt_free(atcmd_ssl_srv_crt[n->con_id]);
						rtw_free((void *)atcmd_ssl_srv_crt[n->con_id]);
						atcmd_ssl_srv_crt[n->con_id] = NULL;
					}
					if(atcmd_ssl_server_crt[n->con_id]){
						rtw_free(atcmd_ssl_server_crt[n->con_id]);
						atcmd_ssl_server_crt[n->con_id] = NULL;
					}
					if(atcmd_ssl_server_ca_list[n->con_id]){
						rtw_free(atcmd_ssl_server_ca_list[n->con_id]);
						atcmd_ssl_server_ca_list[n->con_id] = NULL;
					}
					if(atcmd_ssl_srv_key[n->con_id]){
						pk_free(atcmd_ssl_srv_key[n->con_id]);
						rtw_free((void *)atcmd_ssl_srv_key[n->con_id]);
						atcmd_ssl_srv_key[n->con_id] = NULL;
					}
					if(atcmd_ssl_server_key[n->con_id]){
						rtw_free(atcmd_ssl_server_key[n->con_id]);
						atcmd_ssl_server_key[n->con_id] = NULL;
					}
				}
			}
			else
#endif
			{
				close(n->sockfd);
			}
			n->sockfd = INVALID_SOCKET_ID;
		}
	}
	//task will exit itself in fail case
	if(n->handletask){
		vTaskDelete(n->handletask);
		n->handletask = NULL;
	}
#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
	if(n->context){
		rtw_free(n->context);
		n->context = NULL;
	}
#endif
	n->con_id = INVALID_CON_ID;
	return;
}

int hang_node(node* insert_node)
{
	node* n = mainlist;
	SYS_ARCH_DECL_PROTECT(lev);
	SYS_ARCH_PROTECT(lev);
	while (n->next != NULL)
	{
		n = n->next;
		if(insert_node->role == NODE_ROLE_SERVER) //need to check for server in case that two conns are binded to same port, because SO_REUSEADDR is enabled
		{
			if( (n->port == insert_node->port) && ((n->addr== insert_node->addr) && (n->role == insert_node->role) && (n->protocol == insert_node->protocol) ) ){
				SYS_ARCH_UNPROTECT(lev);
					struct in_addr addr;
					addr.s_addr = htonl(insert_node->addr); 
				AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
						"This conn(IP:%s PORT:%d) already exist",
						inet_ntoa(addr),insert_node->port);
				return -1;
			}
		}
	}

	n->next = insert_node;
	SYS_ARCH_UNPROTECT(lev);
	return 0;
}

int hang_seednode(node* main_node ,node* insert_node)
{
	node* n = main_node;
	 
	SYS_ARCH_DECL_PROTECT(lev);
	SYS_ARCH_PROTECT(lev);	 
	while (n->nextseed != NULL)
	{
		n = n->nextseed;
		if( (n->port == insert_node->port) && (n->addr == insert_node->addr)){
			SYS_ARCH_UNPROTECT(lev);
			struct in_addr addr;
			addr.s_addr = htonl(insert_node->addr); 
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,
				"This seed IP:%s PORT:%d already exist",
				inet_ntoa(addr),insert_node->port);
			return -1;
		}
	}

	n->nextseed = insert_node;
	SYS_ARCH_UNPROTECT(lev);
	return 0;
}

node *seek_node(int con_id)
{
	node* n = mainlist;
	while (n->next != NULL)
	{
		n = n->next;
		if(n->con_id == con_id)
			return n;

		if(n->nextseed != NULL)
		{
			node* seed = n;
			do{
				seed = seed->nextseed;
				if(seed->con_id == con_id)
					return seed;
			}while (seed->nextseed != NULL);
		}
	}
	return NULL;
}

node *tryget_node(int n)
{
	SYS_ARCH_DECL_PROTECT(lev);
	if ((n <= 0) || (n > NUM_NS)) {
		return NULL;
	}
	SYS_ARCH_PROTECT(lev);
	if (node_pool[n].con_id == INVALID_CON_ID || node_pool[n].sockfd == INVALID_SOCKET_ID) {
		SYS_ARCH_UNPROTECT(lev);
		return NULL;
	}
	SYS_ARCH_UNPROTECT(lev);
	return &node_pool[n];
}

int atcmd_lwip_receive_data(node *curnode, u8 *buffer, u16 buffer_size, int *recv_size, 
	u8_t *udp_clientaddr, u16_t *udp_clientport){
		
	struct timeval tv;
	fd_set readfds;
	int error_no = 0, ret = 0, size = 0;

	FD_ZERO(&readfds);
	FD_SET(curnode->sockfd, &readfds);
	tv.tv_sec = RECV_SELECT_TIMEOUT_SEC;
	tv.tv_usec = RECV_SELECT_TIMEOUT_USEC;
	ret = select(curnode->sockfd + 1, &readfds, NULL, NULL, &tv);
	if(!((ret > 0)&&(FD_ISSET(curnode->sockfd, &readfds))))
	{
		//AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS, 
		//	"[ATPR] No receive event for con_id %d", curnode->con_id);
		goto exit;
	}

	if(curnode->protocol == NODE_MODE_UDP) //udp server receive from client
	{
		if(curnode->role == NODE_ROLE_SERVER){
			//node * clinode;
			struct sockaddr_in client_addr;
			u32_t addr_len = sizeof(struct sockaddr_in);  
			rtw_memset((char *) &client_addr, 0, sizeof(client_addr));

			if ((size = recvfrom(curnode->sockfd, buffer, buffer_size, 0, (struct sockaddr *) &client_addr, &addr_len)) > 0){
				//at_printf("[ATPR]:%d,%s,%d,%s\r\n with packet_size: %d\r\n",con_id, inet_ntoa(client_addr.sin_addr.s_addr), ntohs(client_addr.sin_port), rx_buffer, packet_size);	
				//at_printf("\r\nsize: %d\r\n", recv_size);
				//at_printf("%s", rx_buffer);
			}
			else{
				AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
					"[ATPR] ERROR:Failed to receive data");
				error_no = 4;
			}
#if 0			
			clinode = create_node(NODE_MODE_UDP, NODE_ROLE_SEED);
			clinode->sockfd = curnode->sockfd;
			clinode->addr = ntohl(client_addr.sin_addr.s_addr);
			clinode->port = ntohs(client_addr.sin_port);
			if(hang_seednode(curnode,clinode) < 0){
				delete_node(clinode);
				clinode = NULL;
			}
#else
			inet_ntoa_r(client_addr.sin_addr.s_addr, (char *)udp_clientaddr, 16);
			*udp_clientport = ntohs(client_addr.sin_port);
#endif
		}
		else{
			struct sockaddr_in serv_addr;
			u32_t addr_len = sizeof(struct sockaddr_in);  
			rtw_memset((char *) &serv_addr, 0, sizeof(serv_addr));
			serv_addr.sin_family = AF_INET;
			serv_addr.sin_port = htons(curnode->port);
			serv_addr.sin_addr.s_addr = htonl(curnode->addr);
			
			if ((size = recvfrom(curnode->sockfd, buffer, buffer_size, 0, (struct sockaddr *) &serv_addr, &addr_len)) > 0){
				//at_printf("[ATPR]:%d,%s,%d,%s\r\n with packet_size: %d\r\n",con_id, inet_ntoa(serv_addr.sin_addr.s_addr), ntohs(serv_addr.sin_port), rx_buffer, packet_size);
				//at_printf("\r\nsize: %d\r\n", recv_size);
				//at_printf("%s", rx_buffer);
			}
			else{
				AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
					"[ATPR] ERROR:Failed to receive data");
				error_no = 5;
			}
		}
	}
	else{
		#if 0
		if(curnode->role == NODE_ROLE_SERVER){
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
				"[ATPR] ERROR: TCP Server must receive data from the seed");
			error_no = 6;
		}
		#endif
#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
		//receive from seed or server
		if(curnode->protocol == NODE_MODE_SSL){
			size = ssl_read((ssl_context *)curnode->context, buffer, buffer_size);
		}
		else
#endif // #if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
		{
			size = read(curnode->sockfd,buffer,buffer_size);
		}
		if(size == 0){
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
			"[ATPR] ERROR:Connection is closed!");
			error_no = 7;
		}
		else if(size < 0){
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
			"[ATPR] ERROR:Failed to receive data.ret=-0x%x!",-size);
			error_no = 8;
		}
	}
exit:
	if(error_no == 0)
		*recv_size = size;
	else{
#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
		if(curnode->protocol == NODE_MODE_SSL){
			ssl_close_notify((ssl_context *)curnode->context);
			net_close(curnode->sockfd);
			ssl_free((ssl_context *)curnode->context);
		}
		else
#endif //#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
		{
			close(curnode->sockfd);
		}
		curnode->sockfd = INVALID_SOCKET_ID;
	}
	return error_no;
}

static void atcmd_lwip_receive_task(void *param)
{

	int i;
	int packet_size = ETH_MAX_MTU;

	AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS, 
			"Enter auto receive mode");
	
	while(atcmd_lwip_is_autorecv_mode())
	{
		for (i = 0; i < NUM_NS; ++i) {
			node* curnode = NULL;
			int error_no = 0;
			int recv_size = 0;	
			u8_t udp_clientaddr[16] = {0};
			u16_t udp_clientport = 0;			
			curnode = tryget_node(i);
			if(curnode == NULL)
				continue;
			if((curnode->protocol == NODE_MODE_TCP 
#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
				||curnode->protocol == NODE_MODE_SSL
#endif
				)
				&& curnode->role == NODE_ROLE_SERVER){
				//TCP Server must receive data from the seed
				continue;
			}
			error_no = atcmd_lwip_receive_data(curnode, rx_buffer, packet_size, &recv_size, udp_clientaddr, &udp_clientport);

			if(atcmd_lwip_is_tt_mode()){
				if((error_no == 0) && recv_size){
					rx_buffer[recv_size] = '\0';
					AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"Recv[%d]:%s", recv_size, rx_buffer);
					at_print_data(rx_buffer, recv_size);
					rtw_msleep_os(20);
				}
				continue;
			}
			
			if(error_no == 0){
				if(recv_size){
					rx_buffer[recv_size] = '\0';
					#if CONFIG_LOG_SERVICE_LOCK
					log_service_lock();
					#endif
					if(curnode->protocol == NODE_MODE_UDP && curnode->role == NODE_ROLE_SERVER){
						AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,
								"\r\n[ATPR] OK,%d,%d,%s,%d:%s", recv_size, curnode->con_id, udp_clientaddr, udp_clientport, rx_buffer);
						at_printf("\r\n[ATPR] OK,%d,%d,%s,%d:", recv_size, curnode->con_id, udp_clientaddr, udp_clientport);
					}
					else{
						AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,
								"\r\n[ATPR] OK,%d,%d:%s", 
								recv_size, 
								curnode->con_id, rx_buffer);
						at_printf("\r\n[ATPR] OK,%d,%d:", recv_size, curnode->con_id);
					}
					at_print_data(rx_buffer, recv_size);
					at_printf(STR_END_OF_ATCMD_RET);
					#if CONFIG_LOG_SERVICE_LOCK
					log_service_unlock();
					#endif
				}
			}
			else{
				#if CONFIG_LOG_SERVICE_LOCK
				log_service_lock();
				#endif
				at_printf("\r\n[ATPR] ERROR:%d,%d", error_no, curnode->con_id);				
				at_printf(STR_END_OF_ATCMD_RET);
				#if CONFIG_LOG_SERVICE_LOCK
				log_service_unlock();
				#endif
			}
		}
	}

	AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS, 
			"Leave auto receive mode");
	
	vTaskDelete(NULL);
}

int atcmd_lwip_start_autorecv_task(void){
	atcmd_lwip_set_autorecv_mode(TRUE);
	if(xTaskCreate(atcmd_lwip_receive_task, ((const char*)"atcmd_lwip_receive_task"), ATCP_STACK_SIZE, NULL, ATCMD_LWIP_TASK_PRIORITY, NULL) != pdPASS)
	{	
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
			"ERROR: Create receive task failed.");
		atcmd_lwip_set_autorecv_mode(FALSE);
		return -1;
	}
	return 0;
}

int atcmd_lwip_is_tt_mode(void){
	return (atcmd_lwip_tt_mode == TRUE);
}
void atcmd_lwip_set_tt_mode(int enable){
	atcmd_lwip_tt_mode = enable;
}

int atcmd_lwip_is_autorecv_mode(void){
	return (atcmd_lwip_auto_recv == TRUE);
}
void atcmd_lwip_set_autorecv_mode(int enable){
	atcmd_lwip_auto_recv = enable;
}

static void _tt_wait_rx_complete(){
	s32 tick_current = rtw_get_current_time();

	while(rtw_systime_to_ms(tick_current -atcmd_lwip_tt_lasttickcnt) < ATCMD_LWIP_TT_MAX_DELAY_TIME_MS ){
		rtw_msleep_os(5);
		tick_current = rtw_get_current_time();
	}
}

static void atcmd_lwip_tt_handler(void* param)
{
	struct sockaddr_in cli_addr;
	AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS, 
			"Enter TT data mode");

	while(rtw_down_sema(&atcmd_lwip_tt_sema) == _SUCCESS) {
		_lock lock;
		_irqL irqL;
		int tt_size = 0;
		_tt_wait_rx_complete();
		
		rtw_enter_critical(&lock, &irqL);
		if((atcmd_lwip_tt_datasize >= 4) && (rtw_memcmp(log_buf, "----", 4) == _TRUE)){
			atcmd_lwip_set_tt_mode(FALSE);
			atcmd_lwip_tt_datasize = 0;
			rtw_exit_critical(&lock, &irqL);
			break;
		}
		rtw_memcpy(tx_buffer, log_buf, atcmd_lwip_tt_datasize);
		tt_size = atcmd_lwip_tt_datasize;
		atcmd_lwip_tt_datasize = 0;
		rtw_exit_critical(&lock, &irqL);
		tx_buffer[tt_size] = '\0';
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"Send[%d]:%s", tt_size, tx_buffer);
		atcmd_lwip_send_data(mainlist->next, tx_buffer, tt_size, cli_addr);
	}
exit:
	AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS, 
			"Leave TT data mode");
	rtw_free_sema(&atcmd_lwip_tt_sema);
	atcmd_lwip_set_autorecv_mode(FALSE);
	at_printf(STR_END_OF_ATCMD_RET); //mark return to command mode
	vTaskDelete(NULL);
}

int atcmd_lwip_start_tt_task(void){
	int ret;
	int enable = 1;
	int send_timeout = 20; //20 milliseconds
	node *n = mainlist->next;
	ret = setsockopt(n->sockfd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));
	if (ret < 0) {
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR, "set TCP_NODELAY error! ");
		goto err_exit;
	}
#if LWIP_TCP_KEEPALIVE
	ret = setsockopt(n->sockfd, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable));
	if (ret < 0) {
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR, "set SO_KEEPALIVE error! ");
	}
#endif
#if LWIP_SO_SNDTIMEO
	ret = setsockopt(n->sockfd, SOL_SOCKET, SO_SNDTIMEO, &send_timeout, sizeof(int));
	if (ret < 0) {
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR, "SO_SNDTIMEO error! ");
	}
#endif

	rtw_init_sema(&atcmd_lwip_tt_sema, 0);
	atcmd_lwip_set_tt_mode(TRUE);
	if(xTaskCreate(atcmd_lwip_tt_handler, ((const char*)"tt_hdl"), ATCP_STACK_SIZE, NULL, ATCMD_LWIP_TASK_PRIORITY, &atcmd_lwip_tt_task) != pdPASS){
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
			"ERROR: Create tt task failed.");
		goto err_exit;
	}
	rtw_msleep_os(20);
	if(atcmd_lwip_is_autorecv_mode() != 1){
		if(atcmd_lwip_start_autorecv_task()){
			vTaskDelete(atcmd_lwip_tt_task);
			goto err_exit;
		}
	}

	return 0;
	
err_exit:
	atcmd_lwip_set_tt_mode(FALSE);
	return -1;
}

void atcmd_lwip_erase_info(void){
	atcmd_update_partition_info(AT_PARTITION_LWIP, AT_PARTITION_ERASE, NULL, 0);
}

int atcmd_lwip_write_info_to_flash(struct atcmd_lwip_conn_info *cur_conn, int enable)
{
	struct atcmd_lwip_conf read_data = {0};
	int i = 0, found = 0;	
	
	atcmd_update_partition_info(AT_PARTITION_LWIP, AT_PARTITION_READ, (u8 *) &read_data, sizeof(struct atcmd_lwip_conf));

	//fake that the conn exists already when disabling or there is no active conn on this moment
	if(enable == 0){
		atcmd_lwip_erase_info();
		goto exit;
	}

	if(read_data.conn_num < 0 || read_data.conn_num > ATCMD_LWIP_CONN_STORE_MAX_NUM){
		read_data.conn_num = 0;
		read_data.last_index = -1;
	}

	for(i = 0; i < read_data.conn_num; i++){
		if(rtw_memcmp((u8 *)cur_conn, (u8 *)&read_data.conn[i], sizeof(struct atcmd_lwip_conn_info)) == _TRUE) {
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,
				"the same profile found in flash");
			found = 1;
			break;
		}
	}

	if(!found){
		read_data.last_index++;
		if(read_data.last_index >= ATCMD_LWIP_CONN_STORE_MAX_NUM)
			read_data.last_index -= ATCMD_LWIP_CONN_STORE_MAX_NUM;
		rtw_memcpy((u8 *)&read_data.conn[read_data.last_index], (u8 *)cur_conn, sizeof(struct atcmd_lwip_conn_info));
		read_data.conn_num++;
		if(read_data.conn_num > ATCMD_LWIP_CONN_STORE_MAX_NUM)
			read_data.conn_num = ATCMD_LWIP_CONN_STORE_MAX_NUM;
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,
			"not the same proto/addr/port, write new profile to flash");
	}
	if(!found || read_data.enable != enable){
		read_data.enable = enable;
		atcmd_update_partition_info(AT_PARTITION_LWIP, AT_PARTITION_WRITE, (u8 *) &read_data, sizeof(struct atcmd_lwip_conf)); 
	}
exit:	
	return 0;
}

int atcmd_lwip_read_info_from_flash(u8 *read_data, u32 read_len)
{
	atcmd_update_partition_info(AT_PARTITION_LWIP, AT_PARTITION_READ, read_data, read_len);
	return 0;
}

int atcmd_lwip_auto_connect(void)
{
	struct atcmd_lwip_conf read_data = {0};
	struct atcmd_lwip_conn_info *re_conn;
	node* re_node = NULL;
	int i, error_no = 0;
	int last_index;
	
	atcmd_lwip_read_info_from_flash((u8 *)&read_data, sizeof(struct atcmd_lwip_conf));
	if(read_data.enable == 0){
		error_no = 1;
		goto exit;
	}
	if(read_data.conn_num > ATCMD_LWIP_CONN_STORE_MAX_NUM || read_data.conn_num <= 0){
		error_no = 2;
		goto exit;
	}

	last_index = read_data.last_index;
	for(i = 0; i < read_data.conn_num; i++){
		error_no = 0;
		re_conn = &read_data.conn[last_index];
		last_index ++;
		if(last_index >= ATCMD_LWIP_CONN_STORE_MAX_NUM)
			last_index -= ATCMD_LWIP_CONN_STORE_MAX_NUM;
		re_node = create_node(re_conn->protocol, re_conn->role);
		if(re_node == NULL){
			error_no = 3;
			break;
		}
		re_node->addr = re_conn->remote_addr;
		re_node->port = re_conn->remote_port;
		re_node->local_addr = re_conn->local_addr;
		re_node->local_port = re_conn->local_port;
		if(re_node->role == NODE_ROLE_SERVER){
			//TODO: start server here
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"Auto connect isn't enabled for server");
			error_no = 4;
			goto exit;
		}
		struct in_addr addr;
		addr.s_addr = htonl(re_node->addr);
		AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,
			"\r\nAuto connect to: %d,%s,%d", 
			re_node->protocol, 
			inet_ntoa(addr), re_node->port);

#if (ATCMD_VER == ATVER_2) && ATCMD_SUPPORT_SSL
		if(re_node->protocol == NODE_MODE_SSL){
			int ret;
			char c_remote_addr[16] ;
			if(inet_ntoa_r(addr, c_remote_addr, sizeof(c_remote_addr))==NULL){
				delete_node(re_node);
				re_node = NULL;
				error_no = 5;
				continue;
			}
			if((ret = net_connect(&re_node->sockfd, c_remote_addr, re_node->port)) != 0){
				AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"Failed to net_connect!ret=%d", ret);
				delete_node(re_node);
				re_node = NULL;
				error_no = 6;
				continue;
			}
			/***********************************************************
			*  SSL 1. Setup stuff for this ssl connection
			************************************************************/
			int retry_count = 0;
			ssl_context *ssl;
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"Setting up the SSL/TLS structure..." );
			memory_set_own(atcmd_ssl_malloc, atcmd_ssl_free);
			ssl = (ssl_context *)rtw_zmalloc(sizeof(ssl_context));
			if(ssl == NULL){
				AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"malloc fail for ssl");
				error_no = 7;
				delete_node(re_node);
				re_node = NULL;
				continue;
			}
			if((ret = ssl_init(ssl)) != 0) {
				AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,
					" ssl_init failed: %d", ret);
				error_no = 8;
				rtw_free((void *)ssl);
				delete_node(re_node);
				re_node = NULL;
				continue;
			}
			ssl_set_endpoint(ssl, SSL_IS_CLIENT);
			ssl_set_authmode(ssl, SSL_VERIFY_NONE);
			ssl_set_rng(ssl, atcmd_ssl_random, NULL);
			ssl_set_bio(ssl, net_recv, &re_node->sockfd, net_send, &re_node->sockfd);
			ssl_set_dbg(ssl, atcmd_ssl_debug, NULL);
#ifdef POLARSSL_DEBUG_C
			debug_set_threshold(ATCMD_SSL_DEBUG_LEVEL);
#endif
			/***********************************************************
			*  SSL 2. Wait for the ssl handshake done
			************************************************************/
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"Performing the SSL/TLS handshake...");
			while((ret = ssl_handshake(ssl)) != 0) {
				if((ret != POLARSSL_ERR_NET_WANT_READ && ret != POLARSSL_ERR_NET_WANT_WRITE  
					&& ret != POLARSSL_ERR_NET_RECV_FAILED) || retry_count >= 5) {
					AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"ssl_handshake failed -0x%x\n", -ret);
					error_no = 9;
					break;
				}
				retry_count++;
			}
			if(ret != 0){
				rtw_free((void *)ssl);
				delete_node(re_node);
				re_node = NULL;
				error_no = 10;
				continue;
			}
			AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"Connect to Server successful!");
			/***********************************************************
			*  SSL 3. Hang node on mainlist for global management
			************************************************************/
			re_node->context = (void *)ssl;
			if(hang_node(re_node) < 0){
				error_no = 11;
			}	
			break;
		}
		else
#endif
		{
			if(re_node->protocol == NODE_MODE_UDP)
				re_node->sockfd = socket(AF_INET,SOCK_DGRAM,0);
			else if(re_node->protocol == NODE_MODE_TCP)
				re_node->sockfd = socket(AF_INET, SOCK_STREAM, 0);
			else
				AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"Unknown connection type[%d]", re_node->protocol);
			
			if (re_node->sockfd == INVALID_SOCKET_ID) {
				AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"Failed to create sock_fd!");
				error_no = 12;
				break;
			}
			
			if (re_node->protocol == NODE_MODE_TCP){//TCP MODE
				struct sockaddr_in c_serv_addr;
				rtw_memset(&c_serv_addr, 0, sizeof(c_serv_addr));
				c_serv_addr.sin_family = AF_INET;
				c_serv_addr.sin_addr.s_addr = htonl(re_node->addr);
				c_serv_addr.sin_port = htons(re_node->port);
				if(connect(re_node->sockfd, (struct sockaddr *)&c_serv_addr,  sizeof(c_serv_addr)) == 0){
					AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"Connect to Server successful!");
					if(hang_node(re_node) < 0){
						error_no = 13;
					}
					break;
				}else{
					AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"Connect to Server failed(%d)!", errno);
					error_no = 14;
					delete_node(re_node);
					re_node = NULL;
					continue; //try next conn
				}
			}
			else{
#if IP_SOF_BROADCAST && IP_SOF_BROADCAST_RECV
				/* all ones (broadcast) or all zeroes (old skool broadcast) */
				if((re_node->addr == INADDR_BROADCAST)||
					(re_node->addr == INADDR_ANY)){
					int so_broadcast = 1; 
					if(setsockopt(re_node->sockfd, SOL_SOCKET, SO_BROADCAST, &so_broadcast,  
						sizeof(so_broadcast)) < 0){
						error_no = 15;
						delete_node(re_node);
						re_node = NULL;
						continue;
					}
				}
#endif
#if LWIP_IGMP
				ip_addr_t dst_addr;
				dst_addr.addr = htonl(re_node->addr);
				if(ip_addr_ismulticast(&dst_addr)){
					struct ip_mreq imr;
					struct in_addr intfAddr;
					//printf("multicasting\r\n");
					// Set NETIF_FLAG_IGMP flag for netif which should process IGMP messages
					xnetif[0].flags |= NETIF_FLAG_IGMP;
					imr.imr_multiaddr.s_addr = htonl(re_node->addr);
					imr.imr_interface.s_addr = htonl(INADDR_ANY);
					if(setsockopt(re_node->sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
						&imr, sizeof(imr)) < 0){
						xnetif[0].flags &= ~NETIF_FLAG_IGMP;
						error_no = 16;
						delete_node(re_node);
						re_node = NULL;
						continue;
					}
					intfAddr.s_addr = INADDR_ANY;
					if(setsockopt(re_node->sockfd, IPPROTO_IP, IP_MULTICAST_IF, 
						&intfAddr, sizeof(struct in_addr)) < 0){
						xnetif[0].flags &= ~NETIF_FLAG_IGMP;
						error_no = 17;
						delete_node(re_node);
						re_node = NULL;
						continue;			
					}
				}
#endif
				if(re_node->local_port){
					struct sockaddr_in addr;  
					rtw_memset(&addr, 0, sizeof(addr));  
					addr.sin_family=AF_INET;  
					addr.sin_port=htons(re_node->local_port);  
					addr.sin_addr.s_addr=htonl(INADDR_ANY) ;  
					if (bind(re_node->sockfd, (struct sockaddr *)&addr, sizeof(addr))<0) {
						AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ERROR,"bind sock error!"); 
						error_no = 18;
						delete_node(re_node);
						re_node = NULL;
						continue;
					}
				}
				if(hang_node(re_node) < 0){
					error_no = 19;
				}
				AT_DBG_MSG(AT_FLAG_LWIP, AT_DBG_ALWAYS,"UDP client starts successful!");
				break;
			}
		}
	}
	
exit:
	if(re_node && error_no)
		delete_node(re_node);
	return error_no;
}

int atcmd_lwip_restore_from_flash(void){
	int ret = -1;
	if(atcmd_lwip_auto_connect() == 0){
		if(atcmd_lwip_start_tt_task() == 0)
			ret = 0;
	}
	return ret;
}

void atcmd_lwip_set_rx_buffer(unsigned char * buf, int bufsize) {
    rx_buffer = buf;
    rx_buffer_size = bufsize;
}

#endif	

#if CONFIG_TRANSPORT
log_item_t at_transport_items[ ] = {
#if ATCMD_VER == ATVER_1
	{"ATP1", fATP1,},//mode TCP=0,UDP=1
	{"ATP2", fATP2,},//LOCAL PORT
	{"ATP3", fATP3,},//REMOTE IP
	{"ATP4", fATP4,},//REMOTE PORT
	{"ATP5", fATP5,},//START SERVER
	{"ATP6", fATP6,},//START CLIENT
	{"ATP?", fATPZ,},//SETTING
	{"ATR0", fATR0,},//READ DATA
	{"ATR1", fATR1,},//SET PACKET SIZE
	{"ATRA", fATRA,},//WRITE DATA
	{"ATRB", fATRB,},//SET WRITE PACKET SIZE
#endif
#if ATCMD_VER == ATVER_2 
	{"ATP0", fATP0,},//query errno if defined
	{"ATPS", fATPS,},//Create Server
	{"ATPD", fATPD,},//Close Server/Client connection
	{"ATPC", fATPC,},//Create Client
	{"ATPT", fATPT,},//WRITE DATA
	{"ATPR", fATPR,},//READ DATA
	{"ATPK", fATPK,},//Auto recv
	{"ATPP", fATPP,},//PING
	{"ATPI", fATPI,},//printf connection status
	{"ATPU", fATPU,}, //transparent transmission mode
	{"ATPL", fATPL,}, //lwip auto reconnect setting
#endif	
};

#if ATCMD_VER == ATVER_2
void print_tcpip_at(void *arg){
	int index;
	int cmd_len = 0;

	cmd_len = sizeof(at_transport_items)/sizeof(at_transport_items[0]);
	for(index = 0; index < cmd_len; index++)
		at_printf("\r\n%s", at_transport_items[index].log_cmd);
}
#endif

void at_transport_init(void)
{    
#if ATCMD_VER == ATVER_2 
	init_node_pool();
	mainlist = create_node(-1,-1);
#endif	
	log_service_add_table(at_transport_items, sizeof(at_transport_items)/sizeof(at_transport_items[0]));
}

log_module_init(at_transport_init);
#endif

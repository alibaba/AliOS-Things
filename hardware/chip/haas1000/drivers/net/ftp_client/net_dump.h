#ifndef __NET_DUMP_H__
#define __NET_DUMP_H__

#ifdef __cplusplus
extern "C" {
#endif


typedef void *net_dump_handle_t;
int net_dump_connect(net_dump_handle_t net_dump_t,char *server_addr,int port);
void net_dump_disconnect(net_dump_handle_t net_dump_t);
int net_dump_data(net_dump_handle_t net_dump_t,char *dump_data,int dump_len);
int net_get_dump_size(net_dump_handle_t net_dump_t,char *file_name);
int net_dump_login(net_dump_handle_t net_dump_t,char *username,char*password);
int net_dump_notice_server(net_dump_handle_t net_dump_t,char *dump_file_name);
void net_dump_free(net_dump_handle_t net_dump_ops_t);
net_dump_handle_t net_dump_init(void);


#ifdef __cplusplus
}
#endif

#endif
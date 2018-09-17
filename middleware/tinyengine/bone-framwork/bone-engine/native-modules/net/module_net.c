/**************************************************************************

Copyright © 2018 alibaba. All rights reserved.

**************************************************************************/
#define JS_NATIVE_MODULE_NET_C

#include <hal.h>
#include <stdio.h>
#include <string.h>
#include <be_jse_module.h>
#include <be_osal.h>
#include "be_jse_api.h"
#ifdef WITH_LWIP
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#else
#include <aos/network.h>
#endif
#include "module_net.h"
#include <aos/yloop.h>


	
#define JS_NET_TAG "NET"
#define TEST_TRACK_PRINT printf("%s,%d\n\r",__FUNCTION__,__LINE__)
#define CONFIG_LOGMACRO_DETAILS

static net_task_shedule_t sock_conn_list[16]={0};

/*for example addr: tcp://www.baidu.com:80*/
static int parse_net_option_addr(const char *addr,net_sckt_param_t *p_sckt_param)
{
	char *begin,*colon;
	
	if (!addr || !p_sckt_param) return -1;

	if (strncmp(addr,"tcp",strlen("tcp")) == 0){
		p_sckt_param->proto = SOCK_STREAM;
	}else if (strncmp(addr,"udp",strlen("udp")) == 0){
		p_sckt_param->proto = SOCK_DGRAM;
	}else if (strncmp(addr,"raw",strlen("raw")) == 0){
		p_sckt_param->proto = SOCK_RAW;
	}else{
		be_error(JS_NET_TAG,"invalid net connection protocol type");
		return -1;
	}

	 begin = strstr(addr, "//");
	 if(!begin) return -1;

     begin = begin + 2;
     colon = strrchr(addr, ':');

	 if (!colon) return -1;
	 if (colon-begin <= 0) return -1;
	 
	 strncpy(p_sckt_param->host_ip,begin,colon-begin);
	 p_sckt_param->host_ip[colon-begin] = '\0';

	 colon = colon + 1;
	 p_sckt_param->port=atoi(colon);
	 if (p_sckt_param->port <= 0) return -1;

	 return 0;
}


static int net_socket_connect(net_sckt_param_t *p_sckt_param)
{
	int ret = -1;
	
	struct addrinfo hints;
	int sockfd = 0;
	struct addrinfo *res = NULL;
	char s_port[10] = {0};

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = p_sckt_param->proto;

	itoa(p_sckt_param->port,s_port,10);
	ret = getaddrinfo(p_sckt_param->host_ip, s_port, &hints, &res);
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd < 0) return -1;

	ret = connect(sockfd, res->ai_addr, res->ai_addrlen);
  	if (ret != 0)  return -1;
	
	return sockfd;	
}


/*说明: 创建一个task任务，用于socket请求*/
static void task_net_connect_fun(void * arg)
{
	int ret = -1;
	net_sckt_param_t sckt_param = {0};
	net_options_t *msg = (net_options_t *)arg;
	int socketid;

	BE_ASYNC_S* async = (BE_ASYNC_S*)calloc(1,sizeof(BE_ASYNC_S));
	async->param_count = 1;
	async->params = (be_jse_symbol_t**) calloc(1,sizeof(be_jse_symbol_t*) * async->param_count);
	memset(&sckt_param,0,sizeof(net_sckt_param_t));
	
	ret = parse_net_option_addr(msg->addr,&sckt_param);
	if (ret < 0){
		async->func = (be_jse_symbol_t *)msg->onerror_cb;
		async->params[0] = new_str_symbol("paramter error");
	}

	socketid = net_socket_connect(&sckt_param);
	if  (socketid > 0){
		async->func = (be_jse_symbol_t *)msg->onconn_cb;
		async->params[0] = new_int_symbol(socketid);
		be_debug(JS_NET_TAG,"NET.connect ok ,socketid = %d",socketid);
	}else{
		async->func = (be_jse_symbol_t *)msg->onerror_cb;
		async->params[0] = new_str_symbol("connect error");
		be_debug(JS_NET_TAG,"NET.connect error");
	}
	
	be_debug(JS_NET_TAG,"task: host=%s,port=%d,type=%d",sckt_param.host_ip,sckt_param.port,sckt_param.proto);
	ret = be_osal_schedule_call(be_jse_async_event_cb, async);
	if (ret >=0){
		INC_SYMBL_REF(async->func);//because be_jse_async_event_cb will auto unlock async->func
	}else{
		free(async);
		free(async->params);
	}

	if (socketid > 0){
		  char *buffer = calloc(1,NET_SOCKET_RECV_BUFF_LEN);
		  int bytes_received = 0;
		  while(1){
		  	be_debug(JS_NET_TAG,"ready to recv...socketid=%d",socketid);
		  	bytes_received = recv(socketid, buffer, NET_SOCKET_RECV_BUFF_LEN, 0);
		    if (bytes_received > 0){ //recv成功
					be_debug(JS_NET_TAG,"recv...bytes_received=%d",bytes_received);
					BE_ASYNC_S* async = (BE_ASYNC_S*)calloc(1,sizeof(BE_ASYNC_S));
					async->param_count = 2;
					async->params = (be_jse_symbol_t**) calloc(1,sizeof(be_jse_symbol_t*) * async->param_count);
					async->func = (be_jse_symbol_t *)msg->ondata_cb;
					async->params[0] = new_int_symbol(socketid);
					async->params[1] = new_str_symbol(buffer);
					
					ret = be_osal_schedule_call(be_jse_async_event_cb, async);
					if(ret >= 0){
						INC_SYMBL_REF(async->func); //be_jse_async_event_cb会自动释放func，而这里是while循环
					}else{
						free(async);
					    free(async->params);
					}
					memset(buffer,0,NET_SOCKET_RECV_BUFF_LEN);
		    }else if (bytes_received < 0){//recv失败
		    	if (errno == EINTR) continue;
				be_warn(JS_NET_TAG,"recv...error=%d",bytes_received);
				break;
			}else{
				be_warn(JS_NET_TAG,"server closed=%d",bytes_received);
				break;
			}
		  }

		/*callback when close or error*/
		BE_ASYNC_S* async = (BE_ASYNC_S*)calloc(1,sizeof(BE_ASYNC_S));
		async->param_count = 1;
		async->params = (be_jse_symbol_t**) calloc(1,sizeof(be_jse_symbol_t*) * async->param_count);
		if (bytes_received != 0){
			async->func = (be_jse_symbol_t *)msg->onerror_cb;
			async->params[0] = new_str_symbol("recv error");
		}else{
			async->func = (be_jse_symbol_t *)msg->onclose_cb;
			async->params[0] = new_int_symbol(socketid);
		}
		
		ret = be_osal_schedule_call(be_jse_async_event_cb, async);
		if(ret >= 0){
			INC_SYMBL_REF(async->func); 
		}else{
			free(async);
			free(async->params);
		}
		  	
	}

done:
	/*onxxx_cb callback is optional,need check if vaild*/
	if (msg->onclose_cb) symbol_unlock((be_jse_symbol_t *)msg->onclose_cb);
	if (msg->ondata_cb)  symbol_unlock((be_jse_symbol_t *)msg->ondata_cb);
	if (msg->onerror_cb) symbol_unlock((be_jse_symbol_t *)msg->onerror_cb);
	if (msg->onconn_cb)  symbol_unlock((be_jse_symbol_t *)msg->onconn_cb);
	free(msg->addr);
	free(msg);
	be_debug(JS_NET_TAG,"aos get task exit name=%s",be_osal_task_name());
	be_osal_exit_task(0);
}


static void task_net_send_fun(void * arg)
{
	int ret = -1;
	int bytes_sent = 0;
	send_data_t *msg = (send_data_t *)arg;
	
	if (!msg) return;

	int size = strlen(msg->p_data) + 1;//TODO,use size
	
	char *buffer = msg->p_data;
	while (size > 0) {
		  bytes_sent = send(msg->sock_id, buffer, size, 0);
		  if (bytes_sent <= 0) break;
		  size = size - bytes_sent;
		  buffer = buffer + bytes_sent;
		  printf("task_net_send_fun sent: %ld", bytes_sent);
	}

	free(msg->p_data);
	free(msg);
	be_osal_exit_task(0);
}


/*********************************************************
*Function:    module_net_connect
*Description: js native addon for NET.connect
*Called by:   js api
*Input:       NET.connect(options),options is a object like this 
				{ addr: 'tcp://127.0.0.1:1234',	
				  onconnect: function(conn) {},	
				  ondata: function(conn, data){},
				  onclose: function(conn) {}, 
				  onerror: function(err) {}, 
				}
*Output: 0 try connect ok ,other try connect fail 
*********************************************************/
static be_jse_symbol_t* module_net_connect()
{
	be_jse_symbol_t *arg0 = NULL;
	int ret = -1;

	be_jse_handle_function(0, &arg0,NULL,NULL,NULL);
	if ( (arg0 == NULL) || !symbol_is_object(arg0) ){
		be_error(JS_NET_TAG,"module_net_connect param invalid");
		goto done;
	}

	be_jse_symbol_t* addrNetSymbol = unlock_symbol_value(lookup_named_child_symbol(get_symbol_node_id(arg0),"addr", false));
	be_jse_symbol_t* onConnNetSymbol = unlock_symbol_value(lookup_named_child_symbol(get_symbol_node_id(arg0), "onconnect", false));
	be_jse_symbol_t* onDataNetSymbol = unlock_symbol_value(lookup_named_child_symbol(get_symbol_node_id(arg0), "ondata", false));
	be_jse_symbol_t* onCloseNetSymbol = unlock_symbol_value(lookup_named_child_symbol(get_symbol_node_id(arg0), "onclose", false));
	be_jse_symbol_t* onErrorNetSymbol = unlock_symbol_value(lookup_named_child_symbol(get_symbol_node_id(arg0), "onerror", false));


	if ( addrNetSymbol == NULL || onConnNetSymbol == NULL ){
		be_error(JS_NET_TAG,"module_net_connect param invalid");
		goto done;
	}

	net_options_t *p_net_options = calloc(1,sizeof(net_options_t));
	p_net_options->addr = calloc(1,symbol_str_len(addrNetSymbol) + 1);

	if (!p_net_options || p_net_options->addr == NULL) goto done;

	/*because be_jse_async_event_cb will auto DEC_SYMBL_REF func,so need INC_SYMBL_REF before*/
	symbol_to_str(addrNetSymbol, p_net_options->addr, symbol_str_len(addrNetSymbol));
	symbol_unlock(addrNetSymbol);
	if(onConnNetSymbol && symbol_is_function(onConnNetSymbol)){  
			p_net_options->onconn_cb = (onconnect)onConnNetSymbol;
			INC_SYMBL_REF(onConnNetSymbol);
	}
	if(onDataNetSymbol && symbol_is_function(onDataNetSymbol)){  
			p_net_options->ondata_cb = (ondata)onDataNetSymbol;
			INC_SYMBL_REF(onDataNetSymbol);
	}
	if(onCloseNetSymbol && symbol_is_function(onCloseNetSymbol)){  
			p_net_options->onclose_cb = (onclose)onCloseNetSymbol;
			INC_SYMBL_REF(onCloseNetSymbol);
	}
	if(onErrorNetSymbol && symbol_is_function(onErrorNetSymbol)){ 
			p_net_options->onerror_cb = (onerror)onErrorNetSymbol;
			INC_SYMBL_REF(onErrorNetSymbol);
	}
	
	ret = be_osal_new_task("net_connect",task_net_connect_fun,p_net_options,4096,NULL);
	
	done:
	symbol_unlock(arg0);
	return new_int_symbol(ret);
}


/*************************************************************************************
*Function:    module_net_send
*Description: js native addon for NET.send
*Called by:   js api
*Input:       NET.send(conn，data), conn is the socket handle, data is which to be sent
*Output: 0 js call native ok ,other js call native fail
**************************************************************************************/

static be_jse_symbol_t* module_net_send()
{
	be_jse_symbol_t *arg0 = NULL,*arg1 = NULL;
	int ret = -1;int sock_id = 0;
	char *data = NULL;
	send_data_t *p_send_data= NULL;

	be_jse_handle_function(0, &arg0,&arg1,NULL,NULL);
	if ( (arg0 == NULL) || !symbol_is_int(arg0) 
		 || (arg1 == NULL) || !symbol_is_string(arg1)){
		be_error(JS_NET_TAG,"module_net_connect param invalid");
		goto done;
	}

	sock_id = get_symbol_value_int(arg0);
	data = calloc(1,symbol_str_len(arg1)+1);
	if (!data) goto done;
	
	symbol_to_str(arg1, data, symbol_str_len(arg1));
	be_debug(JS_NET_TAG,"NET.send(%d,%s)",sock_id,data);

	p_send_data = calloc(1,sizeof(send_data_t));
	p_send_data->p_data = data;
	p_send_data->sock_id = sock_id;
	ret = be_osal_new_task("net_connect",task_net_send_fun,p_send_data,4096,NULL);
	if (ret < 0){
		free(p_send_data);
		free(p_send_data->p_data);
	}
	
	done:
	symbol_unlock(arg0);
	symbol_unlock(arg1);
	return new_int_symbol(ret);
}


/*****************************************************************************
*Function:    module_net_close
*Description: js native addon for NET.close
*Called by:   js api
*Input:       NET.closet(conn) ; conn is the handle which NET.connect's 
			  callback return ,for example onconnect: function(conn) {}
*Output:      0 close ok ,other close fail 
*****************************************************************************/

static be_jse_symbol_t* module_net_close()
{
	be_jse_symbol_t *arg0 = NULL;
	int ret = -1;int sock_id = 0;

	be_jse_handle_function(0, &arg0,NULL,NULL,NULL);
	if ( (arg0 == NULL) || !symbol_is_int(arg0) ){
		be_error(JS_NET_TAG,"module_net_close param invalid");
		goto done;
	}

	sock_id = get_symbol_value_int(arg0);
	if (sock_id > 0){
		be_warn(JS_NET_TAG,"socket id[%d] has been closed",sock_id);
		ret = close(sock_id);
	}
	
	done:
	symbol_unlock(arg0);
	return new_int_symbol(ret);
}


static be_jse_symbol_t* module_handle_cb(be_jse_vm_ctx_t* execInfo, 
											  be_jse_symbol_t* var, 
											  const char* name) 
{
	be_debug(JS_NET_TAG,"%s Enter: name=%s",__FUNCTION__,name);
	
	if (strcmp(name,"connect") == 0) 	return module_net_connect();
	if (strcmp(name,"send") == 0) 	return module_net_send();
	if (strcmp(name,"close") == 0) 	return module_net_close();
	
	return BE_JSE_FUNC_UNHANDLED;
}

											  									  
void module_net_load(void)
{
    be_jse_module_load(JS_NET_TAG, module_handle_cb);
}


#undef JS_NATIVE_MODULE_NET_C


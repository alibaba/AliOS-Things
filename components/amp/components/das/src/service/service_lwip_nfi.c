/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <das/platform.h>
#include <das/service.h>

#if (DAS_SERVICE_LWIP_NFI_ENABLED)

#ifdef DAS_PLATFORM_FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"
#include <lwip/sockets.h>
#endif // DAS_PLATFORM_FREERTOS

#ifdef DAS_PLATFORM_AOS
#include "aos/kernel.h"
#include <network/network.h>
#endif // DAS_PLATFORM_AOS

#ifdef DAS_PLATFORM_LINUX
#include <pthread.h>
#include <arpa/inet.h>
#endif // DAS_PLATFORM_LINUX

#define NFI_INFO_KEY_NETFLOW 	"netflow:"

#define LOCAL_SERVICE 			1
#define REMOTE_SERVICE  		2

#define IP_PROTO_UDP     		17
#define IP_PROTO_TCP     		6
#define STR_PROTO_UDP			"udp"
#define STR_PROTO_TCP			"tcp"

typedef struct _netflow_info {
	uint32_t local_ip;
	uint32_t remote_ip;
	uint16_t local_port;
	uint16_t remote_port;
	uint16_t direction;		// local service -> 1, remote service -> 2
	uint16_t protocol;		// tcp & udp
	uint32_t rx_bytes;		// receive	
	uint32_t rx_packets;
	uint32_t tx_bytes;		// transport
	uint32_t tx_packets;
	uint32_t flag;
	uint32_t reserved;
} netflow_info_t;

typedef enum {
	NFI_UNKNOW = 0,
	NFI_INITED = 1,
} NFI_STATUS;

typedef struct _local_service_info {
	uint32_t local_ip;
	uint32_t local_port;
} local_service_info_t;

static netflow_info_t netflow_infos[DAS_SERVICE_NFI_MAX_INFO_CNT];
static local_service_info_t local_service_infos[DAS_SERVICE_NFI_MAX_LOCAL_SVC_CNT];

static uint32_t report_info_index = 0;
static uint32_t local_service_cnt = 0;

#ifdef DAS_PLATFORM_FREERTOS
static SemaphoreHandle_t lwip_nfi_mutex;
#endif // DAS_PLATFORM_FREERTOS

#ifdef DAS_PLATFORM_AOS
static aos_mutex_t lwip_nfi_mutex;
#endif // DAS_PLATFORM_AOS

#ifdef DAS_PLATFORM_LINUX
static pthread_mutex_t lwip_nfi_mutex;
#endif // DAS_PLATFORM_LINUX

static int lwip_nfi_service_inited = 0;

static char *_fill_report_buf(char *buf, size_t len, netflow_info_t *pinfo);
static int _is_local_service(uint32_t local_port);
static netflow_info_t *_get_netflow_info(uint32_t local_ip, 
										uint32_t remote_ip, 
										uint32_t local_port, 
										uint32_t remote_port, 
										uint32_t proto);

static int _lwip_nfi_init(void)
{
	memset(netflow_infos, 0, sizeof(netflow_info_t) * DAS_SERVICE_NFI_MAX_INFO_CNT);

#ifdef DAS_PLATFORM_FREERTOS
	lwip_nfi_mutex = xSemaphoreCreateMutex();
	if( lwip_nfi_mutex == NULL ) {
#endif // DAS_PLATFORM_FREERTOS

#ifdef DAS_PLATFORM_AOS
	if (0 != aos_mutex_new(&lwip_nfi_mutex)) {
#endif // DAS_PLATFORM_AOS

#ifdef DAS_PLATFORM_LINUX
	if (pthread_mutex_init(&lwip_nfi_mutex, NULL) != 0){
#endif // DAS_PLATFORM_LINUX
		DAS_LOG("Creating nfi mutex failed (%s %d).\n", __func__, __LINE__);
		return -1;
	}

	lwip_nfi_service_inited = 1;
	return 0;
}

/*
netflow:local_ip|local_port|remote_ip|remote_port|direction|protocol|rx_bytes|rx_packets|tx_bytes|tx_packets,
netflow:local_ip|local_port|remote_ip|remote_port|direction|protocol|rx_bytes|rx_packets|tx_bytes|tx_packets,
...
*/
static das_result_t _lwip_nfi_info(char* buffer, size_t size, das_service_state_t* state)
{
	size_t offset, remain_len, pre_len;
	int lock = 1;

	if (buffer == NULL || size == 0 || state == NULL) {
		return DAS_ERROR_BAD_PARAMETERS;
	}

	offset = 0;
	remain_len = size;
	pre_len = strlen(NFI_INFO_KEY_NETFLOW);

	if (pre_len >= size) {
		DAS_LOG("No buffer size for sys info: %lx\n", (long)pre_len);
		return DAS_ERROR_SHORT_BUFFER;
	}

#ifdef DAS_PLATFORM_FREERTOS
	if (xSemaphoreTake(lwip_nfi_mutex, (TickType_t)100) != pdTRUE) {
#endif // DAS_PLATFORM_FREERTOS
#ifdef DAS_PLATFORM_AOS
	if (aos_mutex_lock(&lwip_nfi_mutex, AOS_WAIT_FOREVER) != 0) {
#endif // DAS_PLATFORM_AOS
#ifdef DAS_PLATFORM_LINUX
	if (pthread_mutex_lock(&lwip_nfi_mutex) != 0) {
#endif // DAS_PLATFORM_LINUX
        lock = 0;
    }

	for (; report_info_index < DAS_SERVICE_NFI_MAX_INFO_CNT; report_info_index++) {
		int exit = 0;
		netflow_info_t *pinfo = &(netflow_infos[report_info_index]);
		
		if (pinfo->flag == NFI_UNKNOW) {
			break;
		} else if (pinfo->flag == NFI_INITED) {
			char tmp_buf[100] = {0};
			char *buf = _fill_report_buf(tmp_buf, 100, pinfo);
			if (buf && (strlen(buf) + pre_len + 1) < remain_len) {
				if (strlen(buffer) > 0) {
					buffer[offset] = INFOS_SEPARATE_TAG;
					offset++;
					remain_len--;
				}
				
				memcpy(buffer + offset, NFI_INFO_KEY_NETFLOW, pre_len * sizeof(char));
				offset += pre_len;
				remain_len -= pre_len;

				memcpy(buffer + offset, buf, strlen(buf));
				offset += strlen(buf);
				remain_len -= strlen(buf);
			} else {
                exit = 1;
            }
			
			if (exit == 1 || remain_len < 60) {
				report_info_index++;
				state->status = DAS_SRV_STATUS_PUBLISH;
				break;
			}
		}
	}

	if (report_info_index >= DAS_SERVICE_NFI_MAX_INFO_CNT ||
		(&(netflow_infos[report_info_index]))->flag == NFI_UNKNOW) {
		report_info_index = 0;
		
		// just report rx/tx numbers during a heartbeat period. so clear all netflow infos when reported. 
		memset(netflow_infos, 0, sizeof(netflow_info_t) * DAS_SERVICE_NFI_MAX_INFO_CNT);
		
		state->status = DAS_SRV_STATUS_FINISH;
	}

	if (lock) {
#ifdef DAS_PLATFORM_FREERTOS
		xSemaphoreGive(lwip_nfi_mutex);
#endif // DAS_PLATFORM_FREERTOS
#ifdef DAS_PLATFORM_AOS
		aos_mutex_unlock(&lwip_nfi_mutex);
#endif // DAS_PLATFORM_AOS
#ifdef DAS_PLATFORM_LINUX
		pthread_mutex_unlock(&lwip_nfi_mutex);
#endif // DAS_PLATFORM_LINUX
	}
	
	if (strlen(buffer) == 0) {
        return DAS_ERROR_GENERIC;
    }
	
	return DAS_SUCCESS;
}

static char *_fill_report_buf(char *buf, size_t len, netflow_info_t *pinfo)
{
	char *ret = NULL;
	char tmp_buf[16] = {0};
	struct in_addr ip_addr;
	char *ip_str = NULL;
	int off = 0;

	if (buf == NULL || len == 0 || pinfo == NULL) {
		return ret;
	}

	// local ip
	ip_addr.s_addr = pinfo->local_ip;
	ip_str = inet_ntoa(ip_addr);
	if (off + strlen(ip_str) >= len) {
		return ret;
	}
	memcpy(buf+off, ip_str, strlen(ip_str));
	off += strlen(ip_str);
	buf[off] = FIELDS_SEPARATE_TAG;
	off++;

	// local port
	das_itoa(pinfo->local_port, tmp_buf, 10);
	if (off + strlen(tmp_buf) >= len) {
		return ret;
	}
	memcpy(buf+off, tmp_buf, strlen(tmp_buf));
	off += strlen(tmp_buf);
	buf[off] = FIELDS_SEPARATE_TAG;
	off++;
	memset(tmp_buf, 0, 16);

	// remote ip
	ip_addr.s_addr = pinfo->remote_ip;
	ip_str = inet_ntoa(ip_addr);
	if (off + strlen(ip_str) >= len) {
		return ret;
	}
	memcpy(buf+off, ip_str, strlen(ip_str));
	off += strlen(ip_str);
	buf[off] = FIELDS_SEPARATE_TAG;
	off++;

	// remote port
	das_itoa(pinfo->remote_port, tmp_buf, 10);
	if (off + strlen(tmp_buf) >= len) {
		return ret;
	}
	memcpy(buf+off, tmp_buf, strlen(tmp_buf));
	off += strlen(tmp_buf);
	buf[off] = FIELDS_SEPARATE_TAG;
	off++;
	memset(tmp_buf, 0, 16);

	// direction
	das_itoa(pinfo->direction, tmp_buf, 10);
	if (off + strlen(tmp_buf) >= len) {
		return ret;
	}
	memcpy(buf+off, tmp_buf, strlen(tmp_buf));
	off += strlen(tmp_buf);
	buf[off] = FIELDS_SEPARATE_TAG;
	off++;
	memset(tmp_buf, 0, 16);

	// protocol
	if (pinfo->protocol == IP_PROTO_UDP) {
		memcpy(tmp_buf, STR_PROTO_UDP, strlen(STR_PROTO_UDP));
	} else if (pinfo->protocol == IP_PROTO_TCP) {
		memcpy(tmp_buf, STR_PROTO_TCP, strlen(STR_PROTO_TCP));
	} else {
		das_itoa(pinfo->protocol, tmp_buf, 10);
	}
	if (off + strlen(tmp_buf) >= len) {
		return ret;
	}
	memcpy(buf+off, tmp_buf, strlen(tmp_buf));
	off += strlen(tmp_buf);
	buf[off] = FIELDS_SEPARATE_TAG;
	off++;
	memset(tmp_buf, 0, 16);

	// rx_bytes
	das_itoa(pinfo->rx_bytes, tmp_buf, 10);
	if (off + strlen(tmp_buf) >= len) {
		return ret;
	}
	memcpy(buf+off, tmp_buf, strlen(tmp_buf));
	off += strlen(tmp_buf);
	buf[off] = FIELDS_SEPARATE_TAG;
	off++;
	memset(tmp_buf, 0, 16);

	// rx_packets
	das_itoa(pinfo->rx_packets, tmp_buf, 10);
	if (off + strlen(tmp_buf) >= len) {
		return ret;
	}
	memcpy(buf+off, tmp_buf, strlen(tmp_buf));
	off += strlen(tmp_buf);
	buf[off] = FIELDS_SEPARATE_TAG;
	off++;
	memset(tmp_buf, 0, 16);

	// tx_bytes
	das_itoa(pinfo->tx_bytes, tmp_buf, 10);
	if (off + strlen(tmp_buf) >= len) {
		return ret;
	}
	memcpy(buf+off, tmp_buf, strlen(tmp_buf));
	off += strlen(tmp_buf);
	buf[off] = FIELDS_SEPARATE_TAG;
	off++;
	memset(tmp_buf, 0, 16);

	// tx_packets
	das_itoa(pinfo->tx_packets, tmp_buf, 10);
	if (off + strlen(tmp_buf) >= len) {
		return ret;
	}
	memcpy(buf+off, tmp_buf, strlen(tmp_buf));
	off += strlen(tmp_buf);
	
	ret = buf;
	return ret;
}

static netflow_info_t *_get_netflow_info(uint32_t local_ip, 
										uint32_t remote_ip, 
										uint32_t local_port, 
										uint32_t remote_port, 
										uint32_t proto) 
{
	int i = 0;
	netflow_info_t *pinfo = NULL, *pmin_info = NULL;
	int lock = 1;
	uint16_t min_packets = 65535;

#ifdef DAS_PLATFORM_FREERTOS
	if (xSemaphoreTake(lwip_nfi_mutex, (TickType_t)100) != pdTRUE) {
#endif // DAS_PLATFORM_FREERTOS
#ifdef DAS_PLATFORM_AOS
	if (aos_mutex_lock(&lwip_nfi_mutex, AOS_WAIT_FOREVER) != 0) {
#endif // DAS_PLATFORM_AOS
#ifdef DAS_PLATFORM_LINUX
	if (pthread_mutex_lock(&lwip_nfi_mutex) != 0) {
#endif // DAS_PLATFORM_LINUX
        lock = 0;
    }

	for (i = 0; i < DAS_SERVICE_NFI_MAX_INFO_CNT; i++) {
		netflow_info_t *pinfo_tmp = &(netflow_infos[i]);

		if (pinfo_tmp->flag == NFI_UNKNOW) {
			pinfo = pinfo_tmp;
			break;
		} else if (local_ip == pinfo_tmp->local_ip &&
				remote_ip == pinfo_tmp->remote_ip && 
				(uint16_t)local_port == pinfo_tmp->local_port && 
				(uint16_t)remote_port == pinfo_tmp->remote_port &&
				(uint16_t)proto == pinfo_tmp->protocol) {
			pinfo = pinfo_tmp;
			break;
		} else {
			if (pinfo_tmp->tx_packets < min_packets) {
				min_packets = pinfo_tmp->tx_packets;
				pmin_info = pinfo_tmp;
			}
		}
	}

	if (pinfo == NULL && pmin_info != NULL) {
		pinfo = pmin_info;
	}

	if (lock) {
#ifdef DAS_PLATFORM_FREERTOS
		xSemaphoreGive(lwip_nfi_mutex);
#endif // DAS_PLATFORM_FREERTOS
#ifdef DAS_PLATFORM_AOS
		aos_mutex_unlock(&lwip_nfi_mutex);
#endif // DAS_PLATFORM_AOS
#ifdef DAS_PLATFORM_LINUX
		pthread_mutex_unlock(&lwip_nfi_mutex);
#endif // DAS_PLATFORM_LINUX
	}

	return pinfo;
}

static int _is_local_service(uint32_t local_port) 
{
	int ret = 0;
	int i = 0; 

	if (local_port == 0 || local_service_cnt == 0) {
		return ret;
	}

	for (i = 0; i < local_service_cnt; i++) {
		if (local_service_infos[i].local_port == local_port) {
			ret = 1;
		}
	}

	return ret;
}

static int _set_netflow_info(uint32_t out,	// in:0; out:1
							uint32_t local_ip, 
							uint32_t remote_ip, 
							uint32_t local_port, 
							uint32_t remote_port, 
							uint32_t proto,
							uint32_t len,
							netflow_info_t *pinfo) 
{
	int ret = -1;
	int lock = 1;

	if (pinfo == NULL) {
		return ret;
	}

#ifdef DAS_PLATFORM_FREERTOS
	if (xSemaphoreTake(lwip_nfi_mutex, (TickType_t)100) != pdTRUE) {
#endif // DAS_PLATFORM_FREERTOS
#ifdef DAS_PLATFORM_AOS
	if (aos_mutex_lock(&lwip_nfi_mutex, AOS_WAIT_FOREVER) != 0) {
#endif // DAS_PLATFORM_AOS
#ifdef DAS_PLATFORM_LINUX
	if (pthread_mutex_lock(&lwip_nfi_mutex) != 0) {
#endif // DAS_PLATFORM_LINUX
        lock = 0;
    }
	
	memset(pinfo, 0, sizeof(netflow_info_t));

	pinfo->local_ip = local_ip;
	pinfo->remote_ip = remote_ip;
	pinfo->local_port = (uint16_t)local_port;
	pinfo->remote_port = (uint16_t)remote_port;
	pinfo->protocol = (uint16_t)proto;
	pinfo->direction = _is_local_service(local_port) ? LOCAL_SERVICE : REMOTE_SERVICE;

	if (out) {
		pinfo->tx_bytes += len;
		pinfo->tx_packets++;
	} else {
		pinfo->rx_bytes += len;
		pinfo->rx_packets++;
	}

	pinfo->flag = NFI_INITED;
	
	if (lock) {
#ifdef DAS_PLATFORM_FREERTOS
		xSemaphoreGive(lwip_nfi_mutex);
#endif // DAS_PLATFORM_FREERTOS
#ifdef DAS_PLATFORM_AOS
		aos_mutex_unlock(&lwip_nfi_mutex);
#endif // DAS_PLATFORM_AOS
#ifdef DAS_PLATFORM_LINUX
		pthread_mutex_unlock(&lwip_nfi_mutex);
#endif // DAS_PLATFORM_LINUX
	}
	ret = 0;
	return ret;
}

static int _update_netflow_info(uint32_t out,	// in:0; out:1
								uint32_t len,
								netflow_info_t *pinfo) 
{
	int ret = -1;
	int lock = 1;

	if (pinfo == NULL) {
		return ret;
	}

#ifdef DAS_PLATFORM_FREERTOS
	if (xSemaphoreTake(lwip_nfi_mutex, (TickType_t)100) != pdTRUE) {
#endif // DAS_PLATFORM_FREERTOS
#ifdef DAS_PLATFORM_AOS
	if (aos_mutex_lock(&lwip_nfi_mutex, AOS_WAIT_FOREVER) != 0) {
#endif // DAS_PLATFORM_AOS
#ifdef DAS_PLATFORM_LINUX
	if (pthread_mutex_lock(&lwip_nfi_mutex) != 0) {
#endif // DAS_PLATFORM_LINUX
        lock = 0;
    }

	if (out) {
		pinfo->tx_bytes += len;
		pinfo->tx_packets++;
	} else {
		pinfo->rx_bytes += len;
		pinfo->rx_packets++;
	}

	if (lock) {
#ifdef DAS_PLATFORM_FREERTOS
		xSemaphoreGive(lwip_nfi_mutex);
#endif // DAS_PLATFORM_FREERTOS
#ifdef DAS_PLATFORM_AOS
		aos_mutex_unlock(&lwip_nfi_mutex);
#endif // DAS_PLATFORM_AOS
#ifdef DAS_PLATFORM_LINUX
		pthread_mutex_unlock(&lwip_nfi_mutex);
#endif // DAS_PLATFORM_LINUX
	}

	ret = 0;
	return ret;
}

static int _lwip_nfi_input_handler(va_list params)
{
	int ret = -1;
	uint32_t remote_ip = va_arg(params, uint32_t);
	uint32_t local_ip = va_arg(params, uint32_t);
	uint32_t remote_port = va_arg(params, uint32_t);
	uint32_t local_port = va_arg(params, uint32_t);
	uint32_t len = va_arg(params, uint32_t);
	uint32_t proto = va_arg(params, uint32_t);
	netflow_info_t *pinfo = NULL;

	if (lwip_nfi_service_inited == 0) {
		return ret;
	}

	if (remote_ip == 0 || (int)remote_ip == -1 ||
		local_ip == 0 || (int)local_ip == -1) {
		return ret;
	}

	pinfo = _get_netflow_info(local_ip, remote_ip, local_port, remote_port, proto);
	if (pinfo == NULL) {
		return ret;
	}

	if (pinfo->flag == NFI_UNKNOW) {
		_set_netflow_info(0, local_ip, remote_ip, local_port, remote_port, proto, len, pinfo);
	} else {
		_update_netflow_info(0, len, pinfo);
	}

	ret = 0;
	return ret;
}

static int _lwip_nfi_output_handler(va_list params)
{
	int ret = -1;
	uint32_t local_ip = va_arg(params, uint32_t);
	uint32_t remote_ip = va_arg(params, uint32_t);
	uint32_t local_port = va_arg(params, uint32_t);
	uint32_t remote_port = va_arg(params, uint32_t);
	uint32_t len = va_arg(params, uint32_t);
	uint32_t proto = va_arg(params, uint32_t);
	netflow_info_t *pinfo = NULL;

	if (lwip_nfi_service_inited == 0) {
		return ret;
	}

	if (remote_ip == 0 || (int)remote_ip == -1 ||
		local_ip == 0 || (int)local_ip == -1) {
		return ret;
	}

	pinfo = _get_netflow_info(local_ip, remote_ip, local_port, remote_port, proto);
	if (pinfo == NULL) {
		return ret;
	}

	if (pinfo->flag == NFI_UNKNOW) {
		_set_netflow_info(1, local_ip, remote_ip, local_port, remote_port, proto, len, pinfo);
	} else {
		_update_netflow_info(1, len, pinfo);
	}

	ret = 0;
	return ret;
}

static int _lwip_nfi_tcp_listen_handler(va_list params)
{
	int ret = -1;
	uint32_t local_port = va_arg(params, uint32_t);
	uint32_t local_ip = va_arg(params, uint32_t);

	if (lwip_nfi_service_inited == 0) {
		return ret;
	}

	if (local_port == 0 || (int)local_port == -1) {
		return ret;
	}

	if (local_service_cnt >= DAS_SERVICE_NFI_MAX_LOCAL_SVC_CNT) {
		return ret;
	}

	local_service_infos[local_service_cnt].local_ip = local_ip;
	local_service_infos[local_service_cnt].local_port = local_port;
	local_service_cnt++;

	ret = 0;
	return ret;
}

das_service_t service_lwip_nfi = {
	.name = "LWIP_NFI",
	.init = _lwip_nfi_init,
	.info = _lwip_nfi_info,
	.attest = NULL,
	.measure = NULL,
};

das_attest_handler_t lwip_nfi_input_handler = {
	.tag = "NFI_INPUT",
	.handler = _lwip_nfi_input_handler,
};

das_attest_handler_t lwip_nfi_output_handler = {
	.tag = "NFI_OUTPUT",
	.handler = _lwip_nfi_output_handler,
};

das_attest_handler_t lwip_nfi_tcp_listen_handler = {
	.tag = "NFI_TCP_LISTEN",
	.handler = _lwip_nfi_tcp_listen_handler,
};

#endif /* DAS_SERVICE_LWIP_NFI_ENABLED */